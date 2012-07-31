//      NetSocketServer.cpp
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
//      Contents:	This is the interface class that contains one
//                  or more RPC methods. 
//	
#include <cslib.h>

#include <vector>
#include <map>


#include <dom/DOM_DOMException.hpp>
#include <dom/DOM.hpp>
#include <dom/DOM_ProcessingInstruction.hpp>
#include <dom/DOM_NodeList.hpp>
#include <dom/DOM_Document.hpp>
#include <dom/DOM_Node.hpp>

#include <rpc/http/NetSocketServer.h>
#include <rpc/iRpcServer.h>
#include <log/LogStdLogger.h>

#include <net/Net.h>
#include <net/NetHttpClientPost.h>
#include <net/NetHttpSerPost.h>
#include <net/NetHttpContent.h>
#include <net/NetHttpUtils.h>
#include <sys/SysContext.h>
#include <thd/ThdAutoLock.h>
#include <thd/ThdManager.h>
#include <thd/ThdPool.h>
#include <util/UtilMapping.h>
#include <util/UtilRefCountedPtrInlines.h>
#include <util/UtilStringUtils.h>
#include <xml/XmlDomUtils.h>
#include <xml/XmlStringFormatTarget.h>


const String NetSocketServer::RPC_HTTP_NAME = NTEXT("rpc-http");
const String NetSocketServer::RPC_LISTEN_PORT = NTEXT("server-port");
const String NetSocketServer::RPC_BACKLOG_ATTR = NTEXT("listeners");
const String NetSocketServer::RPC_PROXY_NAME = NTEXT("client-proxy");
const String NetSocketServer::RPC_PROXY_PORTATTR = NTEXT("port");
const String NetSocketServer::RPC_CLIENT_RETRIES = NTEXT("client-retries");
const String NetSocketServer::RPC_CLIENT_TOATTR = NTEXT("timeout");

NetSocketServer::Initializer NetSocketServer::_init;
InterLockedLong NetSocketServer::_sSessionId;

// create an NetSocketServer
NetSocketServer::NetSocketServer() :
    _state(keRpcNoState),
    _masterServer(NULL),
    _shuttingDown(false),
    _proxyPort(HttpUtils::keDefaultPort),
    _retries(keRetries),
    _sleepInterval(keSleepInterval),
    _port(keDefaultPort),
    _backlog(keBacklog)
{
}


NetSocketServer::~NetSocketServer()
{
    CBLOGDBG(_logger, NTEXT("NetSocketServer::~NetSocketServer"));
    
    eRpcStatus curStat = status();
    if( curStat == keRpcStarted || curStat == keRpcStopped || curStat == keRpcInitted)
    {
        control(keShutdownCtl);
    }
    
    CBLOGDBG(_logger, NTEXT("NetSocketServer::~NetSocketServer left"));
}


/**
   take in a configuration xml document and a system 
   context and initialize the transport
*/
int 
NetSocketServer::init( 
    const DOM_Node &config, 
    RefCountedPtr<SysContext> &ctx, 
    iRpcServer* masterServer)
{
    int res = -1;
    
    ASSERT_D(status() == keRpcNoState);
    
    _logger = (RefCountedPtr<StdLogger>&)ctx->getComponent( StdLogger::getRegistryName() );
    ASSERT_D(_logger != NULL);
    
    RefCountedPtr<ThdPool> pool = (RefCountedPtr<ThdPool>&)ctx->getComponent( ThdPool::getRegistryName() );
    ASSERT_D(pool != NULL);
    
    // inititalize socket environment
    if ( config.getNodeType() == DOM_Node::ELEMENT_NODE )
    {
        DOM_Element configElem = (DOM_Element&)config;
        
        String val;
        Mapping attrs;
        
        // first configure the server attributes
        DOM_NodeList nodes = configElem.getElementsByTagName( RPC_LISTEN_PORT.c_str() );
        if ( DomUtils::getNodeValue( (const DOM_Element&)nodes.item( 0 ), &val, &attrs ) )
        {
            Mapping::const_iterator it = attrs.find( RPC_BACKLOG_ATTR );
            if( it != attrs.end() )
            {
                _port = StringUtils::toInt( val );
                _backlog = StringUtils::toInt( (*it).second );
            }
            else
            {
                CBLOGERR(_logger,
                         NTEXT("RpcHttpClientTransport::init: can't find attributes to server listener, using defaults"));
            }
        }
        
        
        // now configure the client attributes
        attrs.clear();
        nodes = configElem.getElementsByTagName( RPC_PROXY_NAME.c_str() );
        if ( DomUtils::getNodeValue( (const DOM_Element&)nodes.item( 0 ), &val, &attrs ) )
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
        
        attrs.clear();
        nodes = configElem.getElementsByTagName( RPC_CLIENT_RETRIES.c_str() );
        if ( DomUtils::getNodeValue( (const DOM_Element&)nodes.item( 0 ), &val, &attrs ) )
        {
            Mapping::const_iterator it = attrs.find( RPC_CLIENT_TOATTR );
            if( it != attrs.end() )
            {
                _retries = StringUtils::toInt( val );
                _sleepInterval = StringUtils::toInt( (*it).second );
            }
            else
            {
                CBLOGERR(_logger,
                         NTEXT("RpcHttpClientTransport::init: can't find attributes to configure client communications parameters"));
            }
        }
        
        // setup the rpc address for this server
        String address;
        address = Net::getHostName();
        address += COLON;
        address += StringUtils::toString( _port );
        
        _transportAddress.setTransport( RPC_HTTP_NAME );
        _transportAddress.setAddress( address );

        // all is well set the initial state
        res = 0;
        _state = keRpcInitted;
    }    
    
    if ( res == 0 )
    {
        RefCountedPtr<MyThdFn> wfn(new MyThdFn( *this, &NetSocketServer::myWorkerFunction ));
        pool->add( 1, (RefCountedPtr<iRunnable> &)wfn );

        // inititalize MasterServer Pointer
        ASSERT_D(masterServer != NULL);
        _masterServer = masterServer;
        _triggerEvent.reset();
        _startedEvent.reset();
    }
    
    return res;
}

/**
   control the state of the transport
*/
bool 
NetSocketServer::control( const eRpcControl eCtl)
{
    ASSERT_D(_logger != NULL);
    ASSERT_D(status() != keRpcNoState);

    bool res = false;
    
    eRpcStatus curStat = status(); 
    if( (curStat != keRpcShutdown) && (curStat != keRpcError) && (eCtl != keNoCtl) )
    {
        switch (eCtl)
        {
        case keStartCtl:
            if( curStat != keRpcStarted )
            {
                CBLOGDBG(_logger, NTEXT("NetSocketServer::control: Start Command entered"));
                // tell the thread to actually start processing
                // reset startedEvent because we are going to wait on it
                // signal the trigger so that myWorkerFunction can start
                // doing work
                _startedEvent.reset();
                if(_triggerEvent.signal())
                {
                    res = _startedEvent.wait() == 0;
                }
                
                CBLOGDBG(_logger, NTEXT("NetSocketServer::control: Start Command Completed"));
            }			
            break;
            
        case keShutdownCtl:
            // Note that shutdown and stop are similar and thus are handled
            // in the same manner. That is, if the server is running and we get
            // a shutdown control event, we start off by telling the server to
            // stop, then we fall through and perform the shutdown
        case keStopCtl:
            if( curStat == keRpcStarted )
            {
                CBLOGDBG(_logger, NTEXT("NetSocketServer::control: Stop Command entered"));
                
                // reset stoppedEvent because we are going to wait on it
                // reset trigger event so that the worker thread will 
                //	block on it in myWorkerFunction when the thread has stopped
                _stoppedEvent.reset();
                _triggerEvent.reset();
                
                // doing a hard close on the socket will cause the listener in 
                // processRequest to return a NULL socket, in which case
                // processRequest will return and block on the trigger
                setStatus( keRpcStopPending );
                _listener.close(true);
                
                res = _stoppedEvent.wait() == 0;
                
                CBLOGDBG(_logger, NTEXT("NetSocketServer::control: Stop Command Completed"));
            }
            
            // we break only if we were requested to perform a stop
            if ( eCtl == keStopCtl )
            {
                break;
            } // otherwise the fall through is intentional
            
            CBLOGDBG(_logger, NTEXT("NetSocketServer::control: Shutdown Command entered"));
            
            setShuttingDown( true );
            
            // reset shutdownEvent because we are going to wait on it
            // reset trigger event so that the worker thread will 
            //	block on it in myWorkerFunction when the thread has stopped
            _shutdownEvent.reset();
            _triggerEvent.reset();
            
            if(_triggerEvent.signal())
            {
                res = _shutdownEvent.wait() == 0;
                if ( res )
                {
                    abortAllPosts();
                }
            }
            
            // now make sure that any client posts are completed.
            abortAllClientPosts();
            
            CBLOGDBG(_logger, NTEXT("NetSocketServer::control: Shutdown Command Completed"));
            break;

        default:
            CBLOGDBG(_logger, NTEXT("NetSocketServer::control: unknown state"));
            break;
        }
    }
    
    return res;
}


/**
   process http requests coming in
*/
void
NetSocketServer::processRequest()
{
    while ( true ) // ( status() == keRpcStarted ) 
    {
        // Accept a connection
        //    (this will block until client initiates request).
        RefCountedPtr<Socket> newSock( _listener.accept() );
        
        // if the process is stopped or shutting down
        if ( newSock == NULL )
        {
            if ( status() == keRpcStopPending )
            {
                // a hard close was performed on the listener socket
                // this is the signal for this thread to exit
                break;
            }
            else
            {
                // some other type of socket error, but not a hard close
                // in this case, we will just attempt to re-listen
                // probably the client connected and then immediately disconnected
                continue;
            }
        }

        CBLOGDBG(_logger, NTEXT("NetSocketServer::processRequest: received socket connection...") );

        RefCountedPtr<HttpSerPost> post(new HttpSerPost(newSock));
        HttpContent content;
        if (	post->readRequest( content ) && 
                ( content.getHttpMsgType() == HttpContent::HTTP_MSG_TYPE_REQUEST ) )
        {
            int errCode = HttpUtils::keCodeBadRequest;
            String payload;
            
            if ( content.getIsForm() )
            {
                payload = content.getQueryString( Rpc::PAYLOAD );
            }
            else
            {
                payload = content.getRequestBody();
            }
            
            // Now process the payload if it is not empty
            if ( !payload.empty() )
            {
                CBLOGDBG(_logger, NTEXT("NetSocketServer::processRequest: receiving request - ") + payload );
                
                // store post in pending list
                unsigned long sessionId = _sSessionId++;
                addPost( sessionId, post );
                
                _masterServer->queueMessage( sessionId, content.getHeaders(), payload, RPC_HTTP_NAME );
                
                errCode = HttpUtils::keCodeOk;
            }
            
            if ( errCode != HttpUtils::keCodeOk )
            {
                if ( !payload.empty() )
                {
                    CBLOGERR(_logger, NTEXT("HttpTransport::processRequest - Unable to Parse XML Data: ") + payload);
                }
                else
                {
                    CBLOGERR(_logger, NTEXT("HttpTransport::processRequest - Unable to Parse XML Data"));
                }
                
                /** Send an http reply in case of exception, back to client
                 **/
                if ( post != NULL )
                {
                    HttpContent errContent;
                    errContent.setHttpMsgType( HttpContent::HTTP_MSG_TYPE_RESPONSE );
                    errContent.setErrorCode( errCode );
                    post->sendResponse( errContent );
                } 
            } // if ( errCode != HttpUtils::keCodeOk )
        }
        else
        {
            CBLOGERR(_logger, NTEXT("HttpTransport::processRequest - Unable to read request from client"));
        }
    }
}

/**
   get the status of the rpc transport
*/
eRpcStatus
NetSocketServer::status()
{
    ThdAutoCriticalSection lock(_guard);
    return _state;
}


/**
   set the status of the rpc transport
*/
void 
NetSocketServer::setStatus(
    const eRpcStatus rpcStatus )
{
    ThdAutoCriticalSection lock(_guard);
    if(_state != keRpcShutdown)
        _state = rpcStatus;
}


void 
NetSocketServer::addPost(
    const unsigned long id, 
    RefCountedPtr<HttpSerPost>& post )
{
    ThdAutoCriticalSection lock(_guard);
    _pendingServer[ id ] = post;
}


RefCountedPtr<HttpSerPost>
NetSocketServer::removePost( 
    const unsigned long id )
{
    RefCountedPtr<HttpSerPost> res;
    
    ThdAutoCriticalSection lock(_guard);
    PendingServerPosts::iterator it = _pendingServer.find( id );
    if ( it != _pendingServer.end() )
    {
        res = (*it).second;
        _pendingServer.erase( it );
    }
    
    return res;
}


void 
NetSocketServer::addClientPost(
    RefCountedPtr<HttpClientPost>& post )
{
    ThdAutoCriticalSection lock(_guard);
    _pendingClient[ post ] = post;
}


void
NetSocketServer::removeClientPost(
    RefCountedPtr<HttpClientPost>& post )
{
    ThdAutoCriticalSection lock(_guard);
    _pendingClient.erase( post );
}


/**
   [ client interface ]
   call to send a request to another transport
   @param address the RPC address to send the request to
   @param request the data to send
   @param response the response to the request. On input null, on
   output the response of the message
   @return the synchronous result of the call
*/
bool 
NetSocketServer::sendRequest( 
    const String& address,
    const String& request, 
    String& response )
{	
    RefCountedPtr<HttpClientPost> httpPost( new HttpClientPost );
    
    // check to see if we are using proxy settings
    httpPost->setServerURL( address );
    if(_proxy.size() > 0 && _proxyPort > 0)
    {
        httpPost->setProxyServer( _proxy, _proxyPort );
    }
    
    // Set the request in HttpContent Format
    HttpContent httpRequest;
    httpRequest.setHttpMsgType( HttpContent::HTTP_MSG_TYPE_REQUEST );
    httpRequest.setHttpVersion( HttpUtils::HTTP_VERSION );
    
    // RPC specific attributes
    // we add to the address attribute an address where the server we are contacting
    // would be able to get back to us.
    httpRequest.setHeaderProperty( Rpc::ADDRESS_ATTR, _transportAddress.toString() );
    httpRequest.setRequestBody( request );
    
    String protocol;
    String host;
    unsigned short port;
    String path;
    
    HttpUtils::parseURL( address, protocol, host, port, path );
    if( !path.empty() )
    {
        httpRequest.setServerPath( path );
    }
    else
    {
        httpRequest.setServerPath( HttpUtils::HTTP_DEFAULT_SVRPATH );
    }
    
    host += COLON;
    host += StringUtils::toString( (int)port );
    httpRequest.setHeaderProperty( HttpUtils::HTTP_HOST, host );
    
    // setup the retry mechanism
    int currentRetry = 0;
    bool sendResult = false;
    
    // we call add post to allow this post to be cancelled
    // asynchronously by another thread
    HttpContent httpReply;
    addClientPost( httpPost );
    
    while ( !sendResult && ( currentRetry < _retries ) )
    {
        /**
           one quick check before we send
           send the request and check for errors
        */
        if( status() != keRpcStarted )
        {
            CBLOGERR(_logger, NTEXT("RpcHttpClientTransport::sendRequest - Unable to Send data because the server is down") );
            break;
        }
        
        sendResult = httpPost->send( httpRequest, httpReply );
        currentRetry++;
        
        if ( !sendResult && ( currentRetry < _retries ) )
        {
            CBLOGERR(_logger, NTEXT("RpcHttpClientTransport::sendRequest - preparing to resend data...") );
            ThdManager::Sleep( _sleepInterval );
        }
    }
    
    removeClientPost( httpPost );

    if( !sendResult )
    {
        CBLOGERR(_logger, NTEXT("RpcHttpClientTransport::sendRequest - Unable to Send data") );
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
    
    if( httpReply.getErrorCode() != HttpUtils::keCodeNoContent )
    {
    	response = httpReply.getResponseBody();	
    }
    
    return true;
}


/**
   [ server interface ]
   The RpcServer will call this function to 
   return a response to a request
*/
void 
NetSocketServer::sendResponse(
    const unsigned long sessionId,
    const StringUtils::StringMap &headers,
    const String &response )
{
    // this function is called to respond to a request
    // in the context of an inbound request
    RefCountedPtr<HttpSerPost> post = removePost( sessionId );
    if ( post != NULL )
    {
        bool hasBody = response.length() > 0;
        
        HttpContent content;
        content.setHttpMsgType( HttpContent::HTTP_MSG_TYPE_RESPONSE );
        content.setHeaders( headers );
        content.setErrorCode( hasBody ? HttpUtils::keCodeOk : HttpUtils::keCodeNoContent);

        // this has to be performed after the setHeaders on the content 
        // to make sure that the accept types have been set to 
        // check agains
        if ( content.doesAcceptType(HttpUtils::HTTP_MIME_XMLTEXT) )
        {
            content.setMimeType(HttpUtils::HTTP_MIME_XMLTEXT);
        }
        else
        {
            content.setMimeType(HttpUtils::HTTP_MIME_PLAINTEXT);
        }
        
        content.setMimeEncoding( HttpUtils::HTTP_ENC_UTF8 );
        
        if ( hasBody )
        {
            content.setResponseBody( response );
        }
        
        if ( !post->sendResponse( content ) )
        {
            CBLOGERR( _logger, NTEXT("HttpTransport::respond - Unable to repond to client") );
        }
    }
    else
    {
        CBLOGERR(_logger, NTEXT("HttpTransport::respond - Post not found with the sessionId") );
    }
}


unsigned
NetSocketServer::myWorkerFunction()
{
    CBLOGDBG(_logger, NTEXT("NetSocketServer::myWorkerFunction: entered"));
    
    unsigned int res = 0;
    while ( true ) // (status() != keRpcShutdown) 
    {
        // first update the status of the server
        setStatus(keRpcStopped);
        _stoppedEvent.signal();
        
        // wait on event indicating that the server should start
        // processing requests of to shutdown
        if ( _triggerEvent.wait() != 0 )
        {
            CBLOGERR( _logger, NTEXT("HttpTransport::myWorkerFunction - Fatal Error") );
            res = 1;
            break;
            
        }
        // check the status flag of the server and make sure it's 
        // shuttingdown
        if(getShuttingDown())
        {
            res = 0;
            break;
        }
        /** Start listening to the Socket
         **/
        if(!_listener.listen(_port, _backlog))
        {
            CBLOGERR( _logger, NTEXT("HttpTransport::myWorkerFunction - Unable to Listen to port : " 
                                     + StringUtils::toString(_port)) );
            setStatus(keRpcError); 
            _startedEvent.signal();
            res = 1;
            break;
        }
        
        /** Set the status to started
         **/
        setStatus(keRpcStarted); 
        _startedEvent.signal();
        
        CBLOGMSG( _logger, NTEXT("HttpTransport::myWorkerFunction - Transport started, Listening on port : "
                                 + StringUtils::toString(_port)) );
        
        processRequest();
        
        CBLOGMSG( _logger, NTEXT("HttpTransport::myWorkerFunction - Transport stopped, Listening on port : "
                                 + StringUtils::toString(_port)) );
    }
    
    CBLOGDBG(_logger, NTEXT("NetSocketServer::myWorkerFunction: exited"));



    setStatus(keRpcShutdown);
    _shutdownEvent.signal();
    
    return res;
}


/**
   Send Default Message that server is shutting Down
**/
void 
NetSocketServer::abortAllPosts()
{
    PendingServerPosts serverPosts;
    {
        ThdAutoCriticalSection lock(_guard);
        serverPosts = _pendingServer;
        _pendingServer.clear();
    }
    
    HttpContent errContent;
    errContent.setHttpMsgType( HttpContent::HTTP_MSG_TYPE_RESPONSE );
    errContent.setErrorCode( HttpUtils::keCodeInternalErr );  // Check that its a proper error code
    
    for ( PendingServerPosts::const_iterator itr = serverPosts.begin(); 
          itr != serverPosts.end(); itr++)
    {
        RefCountedPtr<HttpSerPost> post = (RefCountedPtr<HttpSerPost>&)((*itr).second);
        post->sendResponse(errContent);
    }
}


/**
   tell all client posts to shutdown
**/
void 
NetSocketServer::abortAllClientPosts()
{
    if ( getNumActiveClientPosts() > 0 )
    {
        ThdAutoCriticalSection lock(_guard);
        for ( PendingClientPosts::const_iterator itr = _pendingClient.begin(); 
              itr != _pendingClient.end(); itr++)
        {
            (*itr).second->abortSend();
        }
    }
}


/** Set function for shutting down flag
 **/
void 
NetSocketServer::setShuttingDown( const bool down )
{
    ThdAutoCriticalSection lock(_guard);
    _shuttingDown = down;
}

/** get function for shutting down flag
 **/
bool
NetSocketServer::getShuttingDown() const
{
    ThdAutoCriticalSection lock(_guard);
    return _shuttingDown;
}


int 
NetSocketServer::getNumActiveClientPosts()
{
    ThdAutoCriticalSection lock(_guard);
    return _pendingClient.size();
}
