//      RpcClient.cpp
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
//	
// 

#include <cslib.h>

#include <parsers/DOMParser.hpp>
#include <framework/MemBufInputSource.hpp>
#include <util/PlatformUtils.hpp>

#include <dom/DOM.hpp>
#include <dom/DOM_Node.hpp>
#include <dom/DOM_Element.hpp>
#include <dom/DOM_NodeList.hpp>
#include <dom/DOM_ProcessingInstruction.hpp>
#include <dom/DOM_Document.hpp>

#include <rpc/RpcClient.h>
#include <rpc/iRpcClientTransport.h>
#include <rpc/RpcClientTransportFactory.h>
#include <rpc/RpcTypes.h>
#include <rpc/RpcErrorHandler.h>

#include <log/LogStdLogger.h>
#include <net/NetHttpUtils.h>
#include <sys/SysContext.h>
#include <thd/ThdAutoLock.h>

#include <util/UtilRefCountedPtrInlines.h>
#include <util/UtilStringUtils.h>
#include <util/UtilMapping.h>
#include <xml/XmlDomUtils.h>
#include <xml/XmlStringFormatTarget.h>


const String RpcClient::RPC_CLIENT_NAME = NTEXT("rpc-client");
const String RpcClient::RPC_NAME_ATT = NTEXT("name");
const String RpcClient::RPC_TRANSPORTS_ELEM = NTEXT("transports");
const String RpcClient::RPC_TRANSPORT_ELEM = NTEXT("transport");


RpcClient::Initializer RpcClient::_init;


RpcClient::RpcClient() :
    _logger(NULL),
    _state(keRpcNoState)
{
}


RpcClient::~RpcClient()
{
    CBLOGDBG( _logger, NTEXT("RpcClient::~RpcClient - closing service") );
    if(getStatus() != keRpcShutdown)
        close();
    
}

/**
    return the status of the server in a thread safe manner
*/
eRpcStatus 
RpcClient::getStatus()
{
    ThdAutoCriticalSection lock(_guard);
    return _state;
}


/**
    set the value of the status in a thread safe manner
*/
void
RpcClient::setStatus(
            const eRpcStatus state)
{
    ThdAutoCriticalSection lock(_guard);
    _state = state;
}


/*
    called to initialize the component. DOM_Node is a configuration
    for which the sub-tree represented by getConfigKey will be fetched
    and used to configure this component. The reason that the entire tree
    is used is because there may be some shared configuration data
    that this component may use...
*/
int 
RpcClient::init( 
            const DOM_Node &config, 
            RefCountedPtr<SysContext> &ctx )
{
    int res = -1;
    
    ASSERT_D(getStatus() == keRpcNoState);
    
    _logger = (RefCountedPtr<StdLogger>&)ctx->getComponent( StdLogger::getRegistryName() );
    ASSERT_D(_logger != NULL);
    
    if ( config.getNodeType() == DOM_Node::ELEMENT_NODE )
    {
        DOM_Element configElem = (DOM_Element&)config;
        
        //
        // READ THE HEADER TO UNDERSTAND THIS SEQUENCE
        //
        
        // now look for transports to use
        DOM_NodeList transportRoot = configElem.getElementsByTagName( RPC_TRANSPORTS_ELEM.c_str() );
        if ( transportRoot.getLength() > 0 )
        {
            // a transport tree contains one or more transport elements, however
            // there can be only one transfer root
            DOM_Element elem = (DOM_Element&)transportRoot.item(0);
            ASSERT_D(elem.getNodeType() == DOM_Node::ELEMENT_NODE ); 
            DOM_NodeList transport = elem.getElementsByTagName( RPC_TRANSPORT_ELEM.c_str() );
            for ( unsigned int i = 0, sz = transport.getLength(); i < sz; i++ )
            {
                // the name is an attribute of a transport configuration
                DOM_Element cfg = (DOM_Element&)transport.item(i);
                Mapping attrs;
                if ( DomUtils::getAttributes( cfg, attrs ) )
                {
                    if(attrs.count(RPC_NAME_ATT) == 0)
                    {
                        CBLOGERR( _logger, NTEXT("RpcClient::init: no name attribute found for entry: ") + attrs.toString() );
                        continue;
                    }

                    String transportName = attrs[RPC_NAME_ATT];
                    RefCountedPtr<iRpcClientTransport> obj( RpcClientTransportFactory::createObject( transportName ) );
                    if ( (obj != NULL) && (obj->init( cfg, ctx) == 0) )
                    {
                        res = 0;
                        addTransport( obj );
                    }
                    else
                    {
                        CBLOGERR( _logger, NTEXT("RpcClient::init: can't find transport: ") + transportName );
                    }
                }
                else
                {
                    CBLOGERR( _logger, NTEXT("RpcClient::init: can't find attributes to configure transport"));
                }
            }
        }
    }    

    if ( res == 0 )
    {
        setStatus(keRpcInitted);
    }

    return res;
}

/*
  Called after initialization of the system context. this allows
  system components to utilize other components as necessary
  SEE init()
*/
int 
RpcClient::postInit( 
    const DOM_Node &config, 
    RefCountedPtr<SysContext> &ctx )
{
    return 0;
}


void 
RpcClient::addTransport(
            RefCountedPtr<iRpcClientTransport> &intf )
{
    CBLOGDBG( _logger, NTEXT("RpcClient::addTransport added transport: ") + intf->getName() );

    ThdAutoCriticalSection lock(_guard);
    _transports[ intf->getName() ] = intf;
}


void 
RpcClient::removeTransport( 
            const String& name )
{
    CBLOGDBG( _logger, NTEXT("RpcClient::removeTransport removing transport: ") + name );

    ThdAutoCriticalSection lock(_guard);
    _transports.erase( name );
}


RefCountedPtr<iRpcClientTransport> 
RpcClient::getTransport(const String& name)
{
    ThdAutoCriticalSection lock(_guard);
    TransportMap::iterator it = _transports.find( name );
    if ( it != _transports.end() )
        return (*it).second;

    return NULL;
}


int 
RpcClient::close()
{
    {
        ThdAutoCriticalSection lock(_guard);
        _transports.clear();	
    }

    setStatus( keRpcShutdown );
    return 0;
}


/**
  function to call the rpc method
*/
enumRpcCallResult 
RpcClient::callMethod( 
    const RpcAddress& address,
    const DOM_Node& requestnode,
    const String& object,
    const String& method,
    DOM_Node& responsenode)
{
    ASSERT_D(getStatus() == keRpcInitted);
    
    String request;
    request = OPENANGLEBRACKET + QUESTION;
    request += Rpc::TARGET_PI;
    request += SPACE;
    request += Rpc::OBJECT_ATTR;
    request += EQUAL;
    request += QUOTE;
    request += object;
    request += QUOTE;
    request += SPACE;
    request += Rpc::METHOD_ATTR;
    request += EQUAL;
    request += QUOTE;
    request += method;
    request += QUOTE;
    
    if(!requestnode.isNull())
    {	
        ASSERT_D(requestnode.getNodeType() == DOM_Node::ELEMENT_NODE);
        request += SPACE;
        request += Rpc::XML_ATTR;
        request += EQUAL;
        request += QUOTE;
        request += DomUtils::toString( requestnode.getNodeName() );
        request += QUOTE;
        request += QUESTION;
        request += CLOSEANGLEBRACKET;
        request += OPENANGLEBRACKET;
        request += Rpc::REQUEST_NAME;
        request += CLOSEANGLEBRACKET;
        StringFormatTarget tgt;
        DomUtils::print(requestnode, HttpUtils::HTTP_ENC_UTF8, tgt);
        request += tgt.getString();
        request += OPENANGLEBRACKET;
        request += SLASH;
        request += Rpc::REQUEST_NAME;
        request += CLOSEANGLEBRACKET;
    }
    else
    {
        request += QUESTION;
        request += CLOSEANGLEBRACKET;	
    }
    
    RefCountedPtr<iRpcClientTransport> transport = getTransport( address.getTransport() );
    if( transport == NULL)
    {
        CBLOGERR( _logger, 
                  NTEXT("RpcClient::callMethod - Transport not supported \n Transport : ") + address.getTransport() );
        return keError;
    }

    String response;
    if( !transport->request( address.getAddress(), request, response ) )
    {
        CBLOGERR( _logger, 
                  NTEXT("RpcClient::callMethod - Transport Protocol Error \n Request : ") + request);
        return keError;
    }
    
    DOM_Document doc;
    try
    {
        // prepare Input Buffer for Parsing
        MemBufInputSource memBuf((XMLByte*)(response.c_str()), 
                                 response.size(), "HTTP_CLIENT_BODY");
        DOMParser parser;
        RpcErrorHandler errHandler;
        parser.setValidationScheme( DOMParser::Val_Auto );
        parser.setErrorHandler ( &errHandler );
        parser.parse(memBuf); 
        doc = parser.getDocument();
        if(doc.isNull())
        {
            CBLOGERR( _logger, 
                      NTEXT("RpcClient::callMethod - Replied Node is Empty \n Response String :" + response));
            return keError;
        }
    }
    catch(...)
    {
        CBLOGERR( _logger,
                  NTEXT("RpcClient::callMethod - Transport Unable to Parse Data \n Response") + response);
        return keError;
    }
	
	/** Looking for the processing Instruction
		If multiple processing instruction present for rpc then
		taking the first one
	**/
    std::vector<DOM_ProcessingInstruction> pis;
    DomUtils::findProcessingInstructions( (const DOM_Node&)doc, Rpc::TARGET_PI, pis );

    if( pis.size() == 0 )
    {
        /** Returning error if no processing instruction Present
         **/
        CBLOGERR( _logger, NTEXT("RpcClient::callMethod - No Return Instruction Present"));
        return keParamError;
    }
    
    /**
       Looking for attribute in the processing instruction
    **/
    
    enumRpcCallResult res = keError;
    Mapping attrs;
    if ( DomUtils::getAttributes( pis.at(0), attrs ) )
    {
        /**
           Looking for the ret_code in the attributes
        **/
    	if(attrs.count(Rpc::RESULT_ATTR) == 0)
        {
            /**
               If no return code present retruning error
            **/
            CBLOGERR( _logger, NTEXT("RpcClient::callMethod - No Return Code Present"));
            return keError;
        }
        
        res = enumRpcCallResult(StringUtils::toInt(attrs[ Rpc::RESULT_ATTR]));
        if ( res == keOk )
        {
            /**
               Looking for the parameter in processing instruction If present
               add to the dom document
            **/
            
            DOM_Node xmlNode;
            Mapping::const_iterator it = attrs.find( Rpc::XML_ATTR );
            if ( it != attrs.end() )
            {
                DomUtils::selectSingleNode( doc.getDocumentElement(), *it, xmlNode );
            }
        }
    }
    else
    {
        CBLOGERR( _logger, NTEXT("RpcClient::callMethod - No Return Attribute Present"));
        res = keError;
    }
    
    return res; 
}

