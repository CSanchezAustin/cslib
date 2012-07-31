//      rpctest.cpp
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
// Copyright (c) 1998-2001 Chris Sanchez. All rights reserved.
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
//              $Revision: 2.0 $
//              $Author: $
//              $Date: $
//
//      Contents:
//	

#if defined(_WINDOWS) || defined(WIN32)
#pragma warning(disable: 4786)
#endif

#include <fstream>
#if defined(_WINDOWS) || defined(WIN32)
#include <stdio.h>
#else
#include <curses.h>
#endif
#include <dom/DOM.hpp>
//#include <parsers/DOMParser.hpp>
#include <framework/XMLFormatter.hpp>
#include <framework/MemBufInputSource.hpp>
#include <sax/ErrorHandler.hpp>
#include <sax/SAXParseException.hpp>

#include "fwk/fwkBootstrap.h"
#include "rpc/iRpcMethod.h"
#include "rpc/RpcInterface.h"
#include "rpc/RpcDispatcher.h"
#include "rpc/RpcServer.h"
#include "rpc/RpcMessage.h"
#include "log/LogStdLogger.h"
#include "net/Net.h"
#include "sys/SysContext.h"
#include "sys/SysManager.h"
#include "sys/SysComponentLoader.h"
#include "sys/SysComInitializer.h"
#include "util/UtilRefCountedPtrInlines.h"
#include "xml/XmlDomUtils.h"
#include "xml/XmlStXmlOpener.h"
#include "xml/XmlStreamFormatTarget.h"
#include "xml/XmlStringFormatTarget.h"

#include "thd/ThdPool.h"

void test_server();

int main(int argc, char *argv[])
{
    Fwk::Initializer initter;

  OFSTREAM out(NTEXT("debug-log"));
  OFSTREAM err(NTEXT("err.log"));
//    Debug::setOutputStream( out );
//    Debug::setErrStream( err );

    Debug::setCurrentModule( NTEXT("rpc_server") );
    
    test_server();
    
    return 0;
}


class Object1Method1 : public iRpcMethod
{
public:
    Object1Method1()
        : _name(NTEXT("object1-method1"))
    {
    }

    enumRpcCallResult callMethod( 
                DOMDocument* responseDoc,
                const DOMNode* inRequest, 
                const Mapping& inOutAttrs,
                DOMNode* &outResponse )
    {
        COUT << NTEXT("Object1Method1::callMethod called") << std::endl;

        outResponse = responseDoc->importNode( const_cast<DOMNode*>(inRequest), true );

        return keOk;
    }

    const String& getName()
    {
        return _name;
    }

private:
    String _name;
};


void test_server()
{
    DOMDocument* doc = DomUtils::getDocument( NTEXT("rpc_server.xml") );
    if ( doc != NULL )
    {
        RefCountedPtr<SysContext> ctx = SysComponentLoader::createComponentContext( doc );
        if ( ctx != NULL )
        {
	  RefCountedPtr<RpcServer> pServer;
	  REFCOUNTED_CAST(iSysComponent, RpcServer, ctx->getComponent( RpcServer::getRegistryName()), pServer);
            if ( pServer != NULL )
            {
	            // create the interfaces that the rpc server will respond to.
                RefCountedPtr<RpcInterface> intf1( new RpcInterface( NTEXT("rpc-object1"), ctx ));
                RefCountedPtr<Object1Method1> method1( new Object1Method1 );
                intf1->addMethod( (RefCountedPtr<iRpcMethod>&)method1 );
    	        pServer->addInterface( intf1 );

                COUT << NTEXT("Server is running") << std::endl;

		CHAR_T ch;
		do {
		  CIN >> ch;
		} while ( ch != NTEXT('q') );
            }
        }

	delete doc;
    }
    else
    {
        CERR << NTEXT("Error parsing: 'cslib-cfg.xml'") << std::endl;
    }
}


