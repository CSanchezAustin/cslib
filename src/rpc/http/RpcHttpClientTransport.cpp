// Copyright © CALEB Technologies Corp. 2001
// This software is protected under the copyright laws as an 
// unpublished work which was created in the year(s) indicated above,
// and includes information which is confidential and proprietary 
// to CALEB Technologies Corp.  All Rights Reserved.
//----------------------------------------------------------------------------
//      RpcHttpClientTransport.cpp
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

#include <cslib.h>

#include <vector>
#include <map>

#include <xercesc/dom/DOMProcessingInstruction.hpp>
#include <xercesc/dom/DOMNodeList.hpp>
#include <xercesc/dom/DOMDocument.hpp>
#include <xercesc/dom/DOMNode.hpp>

#include <rpc/http/RpcHttpClientTransport.h>
#include <log/LogStdLogger.h>

#include <net/NetHttpClientPost.h>
#include <net/NetHttpContent.h>
#include <sys/SysContext.h>
#include <thd/ThdAutoLock.h>
#include <util/UtilMapping.h>
#include <util/UtilRefCountedPtrInlines.h>
#include <util/UtilStringUtils.h>
#include <xml/XmlDomUtils.h>
#include <xml/XmlStringFormatTarget.h>
#include <net/NetHttpContent.h>
#include <net/NetHttpUtils.h>


const String RpcHttpClientTransport::RPC_HTTPCLIENT_NAME = NTEXT("rpc-http-client");
const String RpcHttpClientTransport::RPC_PROXY_NAME = NTEXT("proxyserver");
const String RpcHttpClientTransport::RPC_PROXY_PORTATTR = NTEXT("port");

DECLARE_RPCCLIENT_TRANSPORT(RpcHttpClientTransport);


// create an RpcHttpClientTransport
RpcHttpClientTransport::RpcHttpClientTransport() : 
    _logger(NULL),
    _proxyPort( HttpUtils::keDefaultPort )
{

}


RpcHttpClientTransport::~RpcHttpClientTransport()
{
}


/**
   take in a configuration xml document and a system 
   context and initialize the transport
*/
int 
RpcHttpClientTransport::init( 
    const DOMNode* config, 
    RefCountedPtr<SysContext>& ctx )
{
	REFCOUNTED_CAST(iSysComponent, StdLogger, ctx->getComponent( StdLogger::getRegistryName()), _logger);
    ASSERT_D(_logger != NULL);
	
    // parsing for proxy server
    if ( config->getNodeType() == DOMNode::ELEMENT_NODE )
    {
        const DOMElement* configElem = (const DOMElement*)config;
        // look for the listening port setting
        String val;
        Mapping attrs;

        DOMNodeList* nodes = DomUtils::getNodeList( configElem, RPC_PROXY_NAME );
        if ( DomUtils::getNodeValue( (const DOMElement*)nodes->item( 0 ), &val, &attrs ) )
        {
            Mapping::const_iterator it = attrs.find( RPC_PROXY_PORTATTR );
            if( it != attrs.end() )
            {
                _proxy = val;
                _proxyPort = StringUtils::toInt( (*it).second );
            }
            else
            {
                CBLOGERR(_logger,
                         NTEXT("RpcHttpClientTransport::init: can't find attributes to configure Proxy Port"));
            }
        }
    }

    return 0;
}

/**
   call to send a response to a message
   Note This function will create an instance of httpClientpost for every
   request and then waits for the reply to come
*/
bool
RpcHttpClientTransport::request(
    const String& address,
    const String& request, 
    String& response)
{
    HttpClientPost httpPost;
    httpPost.setServerURL( address );
    if(_proxy.size() > 0 && _proxyPort > 0)
    {
        httpPost.setProxyServer( _proxy, _proxyPort );
    }
	
    /**
       Set the request in HttpContent Format
    */
    HttpContent httpRequest;
    httpRequest.setHttpMsgType( HttpContent::HTTP_MSG_TYPE_REQUEST );

    String protocol;
    String host;
    unsigned short port;
    String path;

    HttpUtils::parseURL( address, protocol, host, port, path );
    if( path.size() != 0 )
        httpRequest.setServerPath( path );
    else
        httpRequest.setServerPath( HttpUtils::HTTP_DEFAULT_SVRPATH );

    httpRequest.setHttpVersion( HttpUtils::HTTP_VERSION );
    httpRequest.setMimeEncoding( HttpUtils::HTTP_FORM_APPLICATION_FORM );
    host += COLON;
    host += StringUtils::toString( (int)port );
    httpRequest.setHeaderProperty( HttpUtils::HTTP_HOST, host );

    // RPC specific attributes
    //
    // this needs to be the address of how to get back to this server
    httpRequest.setHeaderProperty( Rpc::ADDRESS_ATTR, host ); //BUGBUG
    httpRequest.setQueryString( Rpc::PAYLOAD, request );

    /**
       send the request and check for errors
    */
    HttpContent httpReply;
    if( !httpPost.send( httpRequest, httpReply ) )
    {
        CBLOGERR(_logger, NTEXT("RpcHttpClientTransport::request - Unable to Send data") );
        return false;
    }

    /**
       Check for return code
    */
    if( httpReply.getErrorCode() > HttpUtils::keCodeNoContent )
    {
        CBLOGERR(_logger,
                 (NTEXT("RpcHttpClientTransport::request - The following error was returned: '") + 
                  httpReply.getErrorCode()) +
                 (NTEXT("' for the following request '") + 
                  request) +
                 NTEXT("'") );
        return false;
    }

    response = httpReply.getResponseBody();	
    return true;
}
