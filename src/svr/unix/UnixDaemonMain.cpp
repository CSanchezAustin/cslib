//      UnixDaemonMain.cpp
//
// Copyright 2011 Chris Sanchez
//
//   Licensed under the Apache License, Version 2.0 (the "License");
//   you may not use this file except in compliance with the License.
//   You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
//   Unless required by applicable law or agreed to in writing, software
//   distributed under the License is distributed on an "AS IS" BASIS,
//   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//   See the License for the specific language governing permissions and
//   limitations under the License.
// ====================================================================
//
// Copyright (c) 2001-2002 Chris Sanchez. All rights reserved.
// 
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions
// are met:
// 
// 1. Redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer. 
// 
// 
// THIS SOFTWARE IS PROVIDED ``AS IS'' AND ANY EXPRESSED OR IMPLIED
// WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
// OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED.  IN NO EVENT SHALL CHRIS SANCHEZ OR
// ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF
// USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
// OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
// OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
// SUCH DAMAGE.
// ====================================================================
// 
// This software consists of voluntary contributions made by Chris Sanchez. 
// Portions of other open source software were used in the creation of this
// software. Header files for work based on other source bears the 
// orininator copyright.
//
//         Author: Chris Sanchez
// 
//      RCS:
//              $Revision: 2.1 $
//              $State: Exp $
//              $Author: benavidesg $
//              $Date: 2002/09/23 16:27:29 $
//
//      Contents:	
//	
#include <cslib.h>
#include <sys/types.h>
#include <unistd.h>

#include <fstream>

#include <svr/unix/UnixDaemonMain.h>
#include <svr/unix/UnixSignalHandler.h>
#include <svr/iSvrService.h>
#include <svr/SvrServiceKeys.h>
#include <fwk/fwkBootstrap.h>
#include <pat/PatRunnable.h>
#include <sys/SysContext.h>
#include <sys/SysComponentLoader.h>
#include <thd/ThdManager.h>
#include <thd/ThdThread.h>
#include <util/UtilMapping.h>
#include <util/UtilStringUtils.h>
#include <util/UtilRefCountedPtrInlines.h>
#include <xml/XmlDomUtils.h>

#include <xercesc/dom/DOMDocument.hpp>


int 
UnixDaemonMain::InitializeService( 
            RefCountedPtr<iSvrService>& theService,
            Mapping& parameters,
            RefCountedPtr<ThdThread>& signalHandler_thread )
{
    // Lets make sure we install the signal handler before any threads
    // are created. 
    // (particularly the contex creation below, which spawn threads)

    // We are passing an uninited 'theService' so we need to be aware the
    // service state in the handler to appropriately handle shutdown's.
    // RefCountedPtr<iRunnable> theRunner( new CSignalHandler(theService) );

    RefCountedPtr<iRunnable>  signal_handler(UnixSignalHandler::instance(theService));
    // first make sure that we can locate a sys context config file (required)
    String paramCfg;
       
    if ( parameters.count( SvrKeys::SVC_CFG_KEY ) == 0 )
    {
    	// Before we error out, lets try the current directory for the default config file
	paramCfg = theService->getConfigFileName();
       
    }
    else
    {
      paramCfg = (String)parameters[ SvrKeys::SVC_CFG_KEY ];
    }
    
#ifdef __GNUG__
    std::fstream cfgFile(paramCfg.c_str(), std::ios::in);
#else
    std::fstream cfgFile(paramCfg.c_str(), std::ios_base::in);
#endif    
    // Test to see if file exists.
    if (!cfgFile.is_open())
    {
    	// Could not open file, error out
        CERR << NTEXT("Can not find service config key") << std::endl;
        return -1;
    }
    else
    {
    	// Found file, lets close the stream and proceed.
    	cfgFile.close();
    }
    
    DOMDocument* doc = DomUtils::getDocument( paramCfg ); // BUGBUG review for document leak
    if ( doc == NULL )
    {
        CERR << NTEXT("Error initializing service") << std::endl;
        return -1;
    }

    // now build the sys context
    RefCountedPtr<SysContext> ctx = 
            SysComponentLoader::createComponentContext( doc );
    if ( ctx == NULL )
    {
        CERR << NTEXT("Could create a service application container") << std::endl;
        return -1;
    }

    // now get the application configuration (optional)
    DOMNode* theRoot = NULL;
    bool foundRoot = DomUtils::selectSingleNode( 
                doc->getDocumentElement(), 
                theService->getConfigTag(), &theRoot );

    if ( foundRoot )
    {	// Before returning, lets create the signal handler thread at this point.
        signalHandler_thread = ThdManager::newWorkerThread(signal_handler);
        return theService->init( parameters, theRoot, ctx );
    }
    else
    {
        CERR << NTEXT("Could not find the root configuration node") << std::endl;
        return -1;
    }
    
   
}


// This function assumes the following command line parameters
// The following are the parameters that need to be passed to this
// function from the command line
// 1. program name [default by the runtime]
// 2. -action [console|daemon] - The mode to run the application, Required
// 3. -display-name <name>: name of the service to replace in argv[0] for ps to display. Optional
// 4. -service-name <name>: name of the service to replace in included in debug output (systemwide unique name). Optional
// 4. -stdout <path>: path to a log file for standard output. Optional
// 5. -stderr <path>: path to a log file for standard error. Optional
// 6. -service-cfg <path>: path to the system configuration file. Required
// 7. Any other option required by the system in the form "-key value" -or- "-switch"


int
UnixDaemonMain::main(
    const int argc,
    const char* argv[],             
    iSvrService* theServicePtr )
{
    int res = -1;

    Fwk::Initializer initter;

    RefCountedPtr<iSvrService> theService(theServicePtr);

    String serviceName = theService->getName();
    String displayName = theService->getDisplayName();

    // Process command line
    Mapping cmdParams(argc, argv);

    String actionCode;
    // defined if not being run by SCM
    if ( cmdParams.count( SvrKeys::ACTION_KEY ) > 0 )
    {
      actionCode = (String)cmdParams[ SvrKeys::ACTION_KEY ];
    }

      // the registration name to the service
    // [optional] provide service name override
    if ( cmdParams.count( SvrKeys::SERVICE_NAME_KEY ) > 0 )
    {
      serviceName = (String)cmdParams[ SvrKeys::SERVICE_NAME_KEY ];
    }

    // [optional] provide service display name override
    if ( cmdParams.count( SvrKeys::DISPLAY_NAME_KEY ) > 0 )
    {
      displayName = (String)cmdParams[ SvrKeys::DISPLAY_NAME_KEY ];
    }

    argv[0] = displayName.c_str();
    Debug::setCurrentModule( serviceName );

    std::ofstream out;
    std::ofstream err;
    
    // set up the output files if they are specified
    if ( cmdParams.count( SvrKeys::STDOUT_KEY ) != 0 )
    {
#ifdef __GNUG__
        out.open( String( cmdParams[ SvrKeys::STDOUT_KEY ] ).c_str(), 
                  std::ios::out | std::ios::app );
#else
        out.open( String( cmdParams[ SvrKeys::STDOUT_KEY ] ).c_str(), 
                  std::ios_base::out | std::ios_base::app );
#endif
        Debug::setOutputStream( out );
    }
    
    if ( cmdParams.count( SvrKeys::STDERR_KEY ) != 0 )
    {
#ifdef __GNUG__
        err.open( String( cmdParams[ SvrKeys::STDERR_KEY ] ).c_str(), 
                  std::ios::out | std::ios::app );
#else
        err.open( String( cmdParams[ SvrKeys::STDERR_KEY ] ).c_str(), 
                  std::ios_base::out | std::ios_base::app );
#endif
        Debug::setErrStream( err );
    }
    
    if ( cmdParams.count( SvrKeys::DEBUG_KEY ) > 0 )
    {
        Debug::setLevel( StringUtils::toInt( cmdParams[ SvrKeys::DEBUG_KEY ] ) );
    }
    
    try
    {
        RefCountedPtr<ThdThread> signalHandler_thread;
        bool isDaemon = actionCode.compare( SvrKeys::DAEMON_KEY ) == 0;
        // deamonize the application
        if ( isDaemon )
        {
            pid_t child;
            // do daemon stuff here....
            
            if ((child = fork()) != -1 && !child)
            {
	    	//Create a new session if we are not the session leader
	    	pid_t session = setsid();
		
		if (session != -1 && (child = fork()) != -1 && !child)
		{	
                	//We are the child. Init the server and start
                	COUT << NTEXT("Daemon starting...")<< std::endl;
                	if ( UnixDaemonMain::InitializeService( 
                         	theService, cmdParams, signalHandler_thread) == 0 )
                	{
			     
                    	     res = theService->start();
                	}
                	else
                	{
                    	     CERR << NTEXT("Error initializing service") << std::endl;
                	}
		}
		else
            	{
                	//We are parent. Either we  created a child or couldnt create
                	// a child or could create new session
                	if (child == -1)
                	{
                    	     CERR << NTEXT("Couldnt fork process") << std::endl;
                	}
                	else 
                	{
                    	     COUT << NTEXT("We successfully created a daemon, pid:") << child << std::endl;
                	}   
            	}
            }
            else
            {
                //We are parent. Either we  created a child or couldnt create
                // a child
                if (child == -1)
                {
                    CERR << NTEXT("Couldnt fork process") << std::endl;
                }
                else 
                {
                    COUT << NTEXT("We successfully created a child, pid:") << child << std::endl;
                }   
            }		
        }
        else if ( UnixDaemonMain::InitializeService( 
                      theService, cmdParams, signalHandler_thread ) == 0 )
        {
            res = theService->start();
            
        }
        else
        {
            CERR << NTEXT("Error initializing service") << std::endl;
        }
    }
    catch (...)
    {
    }
    
    if ( out.is_open() )
    {
        out.close();
    }
    
    if ( err.is_open() )
    {
        err.close();
    }
    
    return res;
}
