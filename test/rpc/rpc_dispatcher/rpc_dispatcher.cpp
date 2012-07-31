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
#pragma warning(disable:4786)
#pragma warning(disable:4503)
#endif

#include <fstream>
#include <dom/DOM.hpp>
//#include <parsers/DOMParser.hpp>
#include <framework/XMLFormatter.hpp>
#include <sax/ErrorHandler.hpp>
#include <sax/SAXParseException.hpp>

#include "rpc/iRpcMethod.h"
#include "rpc/RpcInterface.h"
#include "rpc/RpcDispatcher.h"
#include "log/LogStdLogger.h"
#include "sys/SysContext.h"
#include "sys/SysManager.h"
#include "util/UtilRefCountedPtrInlines.h"
#include "xml/XmlDomUtils.h"
#include "xml/XmlStXmlOpener.h"
#include "xml/XmlStreamFormatTarget.h"

static const String APACHE_TENANT = NTEXT("apache");
static const String CSLIB_TENANT = NTEXT("cslib");
static const String INVALID_TENANT = NTEXT("invalid");

void test_dispatcher();

int main(int argc, char *argv[])
{
    StXmlOpener opener;

  OFSTREAM out(NTEXT("debug-log"));
  OFSTREAM err(NTEXT("err.log"));
//    Debug::setOutputStream( out );
//    Debug::setErrStream( err );

    Debug::setCurrentModule( NTEXT("DISPATCHER") );

	test_dispatcher();

	return 0;
}


class Object1Method1 : public iRpcMethod
{
public:
    Object1Method1(const String& name, const String& tenancy):
        _name(name),
        _tenancy(tenancy)
    {
    }

    enumRpcCallResult callMethod( 
                DOMDocument*responseDoc,
                const DOMNode* inRequest, 
                const Mapping& inOutAttrs,
                DOMNode* &outResponse )
    {
        COUT << NTEXT("\nObject1Method1::callMethod called - ") << _tenancy << std::endl;
        outResponse = const_cast<DOMNode*>(inRequest);
        return keOk;
    }

    const String& getName()
    {
        return _name;
    }

private:
    String _name;
    String _tenancy;
};


void test_dispatcher()
{
    RefCountedPtr<SysContext> ctx(new SysContext);
    RefCountedPtr<iSysComponent> cpt( new StdLogger( StdLogger::LOGC_DEBUG) );
    ctx->addComponent( cpt );

    // create the interfaces
    RefCountedPtr<RpcInterface> intf( new RpcInterface( NTEXT("interface"), ctx ));
    RefCountedPtr<RpcInterface> intf1( new RpcInterface( NTEXT("interface"), ctx ));
    RefCountedPtr<RpcInterface> intf2( new RpcInterface( NTEXT("interface"), ctx ));
 
    // create the methods
    RefCountedPtr<Object1Method1> method( 
        new Object1Method1(NTEXT("method"), NTEXT("default")) );
    intf->addMethod( (RefCountedPtr<iRpcMethod>&)method );

    RefCountedPtr<Object1Method1> method1( 
        new Object1Method1(NTEXT("method"), NTEXT("apache")) );
    intf1->addMethod( (RefCountedPtr<iRpcMethod>&)method1 );

    RefCountedPtr<Object1Method1> method2( 
        new Object1Method1(NTEXT("method"), NTEXT("cslib")) );
    intf2->addMethod( (RefCountedPtr<iRpcMethod>&)method2 );

    RpcDispatcher disp( NTEXT("test-dispatcher"), ctx );
    disp.addInterface( intf );
    disp.addInterface( intf1, APACHE_TENANT );
    disp.addInterface( intf2, CSLIB_TENANT );

    DOMDocument* requestDoc = DomUtils::getDocument( NTEXT("rpc_dispatcher.xml") );
    if (requestDoc != NULL)
    {
        StreamFormatTarget targ(COUT);

        DOMDocument* responseDoc = DomUtils::createDocument( NTEXT("default") );
	if (responseDoc != NULL)
	{
	    disp.dispatch( requestDoc, responseDoc );
	    disp.dispatch( requestDoc, responseDoc, APACHE_TENANT );
	    disp.dispatch( requestDoc, responseDoc, CSLIB_TENANT );
	    disp.dispatch( requestDoc, responseDoc, INVALID_TENANT );
	  
	    DomUtils::print( responseDoc, NTEXT("UTF-8"), targ );

	    responseDoc->release();
	}

	requestDoc->release();
    }
}
