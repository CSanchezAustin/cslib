//      http_client_transport.cpp
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
//              $Revision: 1.1 $
//              $Author: chriss $
//              $Date: 2005/06/15 04:59:20 $
//
//      Contents:	
//	
#if defined(_WINDOWS) || defined(WIN32)
#pragma warning(disable:4786)
#pragma warning(disable:4503)
#endif

#include <fstream>
#include <dom/DOM.hpp>

#include "rpc/iRpcMethod.h"
#include "rpc/iRpcServer.h"
#include "rpc/RpcAddress.h"
#include "rpc/RpcMessage.h"

#include "fwk/fwkBootstrap.h"
#include "log/LogStdLogger.h"
#include "net/Net.h"
#include "sys/SysContext.h"
#include "sys/SysManager.h"
#include "util/UtilMapping.h"
#include "util/UtilRefCountedPtrInlines.h"
#include "xml/XmlDomUtils.h"
#include "xml/XmlStXmlOpener.h"
#include "thd/ThdPool.h"

#include "rpc/http/RpcHttpTransport.h"

void test_httpClient();

RpcHttpTransport * t_pRpcHttpTrans;

int main()
{
    Fwk::Initializer initter;

  OFSTREAM out(NTEXT("debug-log"));
  OFSTREAM err(NTEXT("err.log"));
//    Debug::setOutputStream( out );
//    Debug::setErrStream( err );

    Debug::setCurrentModule( NTEXT("http_client_transport") );

	test_httpClient();
    
    return 0;
}


class MyRpcServer: public iRpcServer
{
public:
    MyRpcServer(){};
    ~MyRpcServer(){};

    
    bool queueMessage( 
				const unsigned long sessionId,
                const StringUtils::StringMap& headers,
				const String &data,
                const String& fromTransport )
	{
		t_pRpcHttpTrans->sendResponse( sessionId, headers, data );
		return true;
	};

    
};

void test_httpClient()
{
	DOMDocument* requestDoc = DomUtils::getDocument( NTEXT("http_client_transport.xml") );
	if ( requestDoc != NULL )
	{
		DOMElement* rootElem = requestDoc->getDocumentElement();

		RefCountedPtr<SysContext> ctx(new SysContext);
		RefCountedPtr<StdLogger> log( new StdLogger );
		log->setLogLevel( StdLogger::LOGC_ALL );
		log->logStdOut( true );
		ctx->addComponent( (RefCountedPtr<iSysComponent>&)log );

		RefCountedPtr<ThdPool> pool( new ThdPool );
		ctx->addComponent( (RefCountedPtr<iSysComponent>&)pool );

		MyRpcServer * t_pRpcServer = new MyRpcServer();
		RpcHttpTransport t_RpcHttpTrans;
		t_pRpcHttpTrans = &t_RpcHttpTrans;

		if(t_RpcHttpTrans.init(rootElem, ctx, t_pRpcServer) == 0)
		{
			// check for errors
			t_RpcHttpTrans.control(keStartCtl);

			// now try the client interface to send a message to the rpc server
			String theResponse;
			t_RpcHttpTrans.sendRequest(NTEXT("localhost:8080"),  NTEXT("<hello>hi</hello>"), theResponse );      

			// wait for some time to complete the client communication
			t_RpcHttpTrans.control(keStopCtl);
			t_RpcHttpTrans.control(keShutdownCtl);

            COUT << theResponse.c_str() << std::endl;
			COUT << "TEST Completed" << std::endl;
		}
		delete t_pRpcServer;
		delete requestDoc;
	}
	else
	{
        CERR << NTEXT("Could not parse: 'http_client_transport.xml'") << std::endl;
	}
}
