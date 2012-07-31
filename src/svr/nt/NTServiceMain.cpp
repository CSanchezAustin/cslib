//      NTServiceMain.cpp
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
// Portions Copyright (c) 2000 Kevin Miller
// Copyright (c) 1998-2002 Chris Sanchez. All rights reserved.
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
//                 $Revision:$
//                 $Author:$
//                 $Date:$
//
//      Contents:	
//
#include <cslib.h>

#include <TCHAR.H>
#include <conio.h>
#include <fstream>

#include <svr/nt/NTServiceMain.h>
#include <svr/iSvrService.h>
#include <svr/SvrServiceKeys.h>
#include <svr/nt/CService.h>
#include <svr/nt/CServiceInstall.h>
#include <fwk/fwkBootstrap.h>
#include <log/nt/EventLog.h>
#include <pat/PatRunnable.h>
#include <sys/SysContext.h>
#include <sys/SysComponentLoader.h>
#include <sys/SysManager.h>
#include <thd/ThdPool.h>
#include <thd/ThdManager.h>
#include <util/UtilMapping.h>
#include <util/UtilStringUtils.h>
#include <util/UtilRefCountedPtrInlines.h>
#include <xml/XmlDomUtils.h>

#include <xercesc/dom/DOMDocument.hpp>

// used to install an exception handler to intercept control c events
class CServiceExHandler : public iRunnable
{
public:
    CServiceExHandler(RefCountedPtr<iSvrService> &itsService ) :
        _itsService(itsService)
    {
    }

    unsigned run()
    {
        // look for a q and then exit
	    while( GETCH() != NTEXT('q') )
        {
        }

        _itsService->stop();
        while ( true )
        {
            if ( !_itsService->isStopped() )
            {
                ThdManager::Sleep( 1000 );
            }
            else
            {
                break;
            }
        }    

        return 0;
    }

private:
    RefCountedPtr<iSvrService> _itsService;
};


int 
NTService::InitializeService( 
    RefCountedPtr<iSvrService>& theService,
    Mapping& parameters,
    const bool installEXHandler )
{ 
    // first make sure that we can locate a sys context config file (required)
    String paramCfg;

    if ( parameters.count( SvrKeys::SVC_CFG_KEY ) == 0 )
    {
    	// Before we error out, lets try the current directory for the default config file
	paramCfg = theService->getConfigFileName();
       
    }
    else
    {
    	paramCfg = parameters[ SvrKeys::SVC_CFG_KEY ];
    }
    
    std::fstream cfgFile(paramCfg.c_str(), std::ios::in);
    
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
    {
        int res = theService->init( parameters, theRoot, ctx );
        if ( ( res == 0 ) && installEXHandler )
        {
            RefCountedPtr<ThdPool> pool;
		    REFCOUNTED_CAST(iSysComponent, ThdPool, ctx->getComponent( ThdPool::getRegistryName()), pool);
            ASSERT_D(pool != NULL);
            RefCountedPtr<iRunnable> theRunner( new CServiceExHandler(theService) );
            pool->add( 1, theRunner );
        }

        return res;
    }
    else
    {
        CERR << NTEXT("Could not find the root configuration node") << std::endl;
        return -1;
    }
}


// This function assumes the following command line parameters
int
NTService::main(
    iSvrService* theServicePtr )
{
    Fwk::Initializer initter;
    
    RefCountedPtr<iSvrService> theService(theServicePtr);

    String serviceName = theService->getName();
    String displayName = theService->getDisplayName();

    // Process command line
    TCHAR* pszCmdLine = ::GetCommandLine();

    // have to get rid of the first argument which is the executable name
    // the rest of the arguments should be in for form key="value"

    // see if the file executable name is quoted "file path"
    if ( *pszCmdLine && *pszCmdLine == NTEXT('"') )
    {
        pszCmdLine++;

        // now find the next "
        while ( *pszCmdLine && *pszCmdLine != NTEXT('"') )
            pszCmdLine++;

        pszCmdLine++;
    }

    // and skip over any white space
    while ( *pszCmdLine && *pszCmdLine != NTEXT(' ') )
        pszCmdLine++;

    String test = String(pszCmdLine);
    Mapping cmdParams( test );

    String actionCode;
    // defined if not being run by SCM
    if ( cmdParams.count( SvrKeys::ACTION_KEY ) > 0 )
    {
        actionCode = cmdParams[ SvrKeys::ACTION_KEY ];
    }

      // the registration name to the service
    // [optional] provide service name override
    if ( cmdParams.count( SvrKeys::SERVICE_NAME_KEY ) > 0 )
    {
        serviceName = cmdParams[ SvrKeys::SERVICE_NAME_KEY ];
    }

    // [optional] provide service display name override
    if ( cmdParams.count( SvrKeys::DISPLAY_NAME_KEY ) > 0 )
    {
        displayName = cmdParams[ SvrKeys::DISPLAY_NAME_KEY ];
    }

    Debug::setCurrentModule( serviceName );

    CServiceInstall si( serviceName, displayName );
    if ( actionCode.compare( SvrKeys::INSTALL_KEY ) == 0 )
    {   // install the service
        String msgPath;
        if ( cmdParams.count( SvrKeys::MSGPATH_KEY ) > 0 )
        {
            msgPath = cmdParams[ SvrKeys::MSGPATH_KEY ];
        }

        // get additional parameters for the install
        // we support the login name and password for the service
        LPCTSTR lpName = NULL;
        LPCTSTR lpPassword = NULL;

        String loginName;
        String loginPwd;
        if ( cmdParams.count( SvrKeys::LOGIN_NAME_KEY ) > 0 &&
             cmdParams.count( SvrKeys::LOGIN_PWD_KEY ) > 0 )
        {
            loginName = cmdParams[ SvrKeys::LOGIN_NAME_KEY ];
            loginPwd = cmdParams[ SvrKeys::LOGIN_PWD_KEY ];
        }

        if ( loginName.length() > 0 )
        {
            lpName = loginName.c_str();
            lpPassword = loginPwd.c_str();
        }

        // remove parameters that specific to install
        // and install the rest with the service
        cmdParams.erase( SvrKeys::MSGPATH_KEY );
        cmdParams.erase( SvrKeys::ACTION_KEY );
        cmdParams.erase( SvrKeys::SERVICE_NAME_KEY );
        cmdParams.erase( SvrKeys::DISPLAY_NAME_KEY );
        cmdParams.erase( SvrKeys::LOGIN_NAME_KEY );
        cmdParams.erase( SvrKeys::LOGIN_PWD_KEY );

        si.Install( cmdParams, 
                    SERVICE_WIN32_OWN_PROCESS, 
                    SERVICE_AUTO_START, 
                    NULL,
                    lpName,
                    lpPassword );

        // if we have a message DLL, then register it
        if( msgPath.length() > 0 )
        {
            CEventLog log( serviceName );
            log.RegisterLog( msgPath.c_str() );
        }
    }
    else if ( actionCode.compare( SvrKeys::REMOVE_KEY ) == 0 )
    {   // remove the service
        si.Remove();
    }
    else  if ( actionCode.compare( SvrKeys::CONSOLE_KEY ) == 0 )
    {   // run as a console application
		OFSTREAM out;
		OFSTREAM err;

        if ( cmdParams.count( SvrKeys::STDOUT_KEY ) != 0 )
        {
            out.open( String( cmdParams[ SvrKeys::STDOUT_KEY ] ).c_str(), std::ios_base::out | std::ios_base::app );
		    Debug::setOutputStream( out );
        }

        if ( cmdParams.count( SvrKeys::STDERR_KEY ) != 0 )
        {
            err.open( String( cmdParams[ SvrKeys::STDERR_KEY ] ).c_str(), std::ios_base::out | std::ios_base::app );
		    Debug::setErrStream( err );
        }

        if ( cmdParams.count( SvrKeys::DEBUG_KEY ) > 0 )
        {
            Debug::setLevel( StringUtils::toInt( cmdParams[ SvrKeys::DEBUG_KEY ] ) );
        }

        try
        {
            if ( NTService::InitializeService( theService, cmdParams, true ) == 0 )
            {
                
                theService->start();
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
    }
    else
    {   // run as a service
        //  first locate the configuration file and then 
        Mapping serviceParams = si.GetParameters();

		OFSTREAM out;
		OFSTREAM err;

        if ( serviceParams.count( SvrKeys::STDOUT_KEY ) != 0 )
        {
            out.open( String( serviceParams[SvrKeys::STDOUT_KEY] ).c_str(), std::ios_base::out | std::ios_base::app );
        }
        else
        {
            String logPath = theService->getName();
            logPath += NTEXT("-out.log");
            out.open(  logPath.c_str() , std::ios_base::out | std::ios_base::app );
        }
        Debug::setOutputStream( out );

        if ( serviceParams.count( SvrKeys::STDERR_KEY ) != 0 )
        {
            err.open( String( serviceParams[ SvrKeys::STDERR_KEY ] ).c_str(), std::ios_base::out | std::ios_base::app );
        }
        else
        {
            String logPath = theService->getName();
            logPath += NTEXT("-err.log");
            out.open(  logPath.c_str() , std::ios_base::out | std::ios_base::app );
        }
        Debug::setErrStream( err );

        if ( serviceParams.count( SvrKeys::DEBUG_KEY ) > 0 )
        {
            Debug::setLevel( StringUtils::toInt( serviceParams[ SvrKeys::DEBUG_KEY ] ) );
        }

        // Create the object instance
        // have to initialize the service
        CService NTService( theService, serviceParams, SERVICE_WIN32_OWN_PROCESS);

        SERVICE_TABLE_ENTRY svcTable[] = 
        {
            { (LPTSTR)serviceName.c_str(), (LPSERVICE_MAIN_FUNCTION)CService::Main },
            {NULL, NULL}
        };
        StartServiceCtrlDispatcher(svcTable);

        // close out debug stream
        if ( out.is_open() )
        {
            out.close();
        }

        if ( err.is_open() )
        {
            err.close();
        }

    }

    return 0;
}

