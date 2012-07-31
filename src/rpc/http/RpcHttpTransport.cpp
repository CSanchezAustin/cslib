//      RpcHttpTransport.cpp
//
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
//      Contents:	This is the interface class that contains one
//                  or more RPC methods. 
//	
#include <cslib.h>

#include <vector>
#include <map>

#include <xercesc/dom/DOMException.hpp>
#include <xercesc/dom/DOM.hpp>
#include <xercesc/dom/DOMProcessingInstruction.hpp>
#include <xercesc/dom/DOMNodeList.hpp>
#include <xercesc/dom/DOMDocument.hpp>
#include <xercesc/dom/DOMNode.hpp>

#include <rpc/http/RpcHttpTransport.h>
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


const String RpcHttpTransport::RPC_HTTP_NAME = NTEXT("rpc-http");
const String RpcHttpTransport::RPC_LISTEN_PORT = NTEXT("server-port");
const String RpcHttpTransport::RPC_BACKLOG_ATTR = NTEXT("listeners");
const String RpcHttpTransport::RPC_PROXY_NAME = NTEXT("client-proxy");
const String RpcHttpTransport::RPC_PROXY_PORTATTR = NTEXT("port");
const String RpcHttpTransport::RPC_CLIENT_RETRIES = NTEXT("client-retries");
const String RpcHttpTransport::RPC_CLIENT_TOATTR = NTEXT("timeout");

DECLARE_RPC_TRANSPORT(RpcHttpTransport)

InterLockedLong RpcHttpTransport::_sSessionId;

// create an RpcHttpTransport
RpcHttpTransport::RpcHttpTransport() :
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


RpcHttpTransport::~RpcHttpTransport()
{
    CBLOGDBG(_logger, NTEXT("RpcHttpTransport::~RpcHttpTransport"));
    
    eRpcStatus curStat = status();
    if( curStat == keRpcStarted || curStat == keRpcStopped || curStat == keRpcInitted)
    {
        control(keShutdownCtl);
    }
    
    CBLOGDBG(_logger, NTEXT("RpcHttpTransport::~RpcHttpTransport left"));
}


/**
   take in a configuration xml document and a system 
   context and initialize the transport
*/
int 
RpcHttpTransport::init( 
    const DOMNode* config, 
    RefCountedPtr<SysContext>& ctx, 
    iRpcServer* masterServer)
{
    int res = -1;
    
    ASSERT_D(status() == keRpcNoState);
    
     REFCOUNTED_CAST(iSysComponent, StdLogger, ctx->getComponent( StdLogger::getRegistryName()), _logger);
    
     RefCountedPtr<ThdPool> pool;
    REFCOUNTED_CAST(iSysComponent, ThdPool, ctx->getComponent( ThdPool::getRegistryName()), pool);
    ASSERT_D(pool != NULL);
    
    // inititalize socket environment
    if (( config != NULL ) && ( config->getNodeType() == DOMNode::ELEMENT_NODE ))
    {
        const DOMElement* configElem = (const DOMElement*)config;
        
        String val;
        Mapping attrs;
        
        // first configure the server attributes
        DOMNodeList* nodes = DomUtils::getNodeList( configElem, RPC_LISTEN_PORT );
        if ( DomUtils::getNodeValue( (const DOMElement*)nodes->item( 0 ), &val, &attrs ) )
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
        nodes = DomUtils::getNodeList( configElem, RPC_PROXY_NAME );
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
        
        attrs.clear();
        nodes = DomUtils::getNodeList( configElem, RPC_CLIENT_RETRIES );
        if ( DomUtils::getNodeValue( (const DOMElement*)nodes->item( 0 ), &val, &attrs ) )
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
        RefCountedPtr<MyThdFn> wfn(new MyThdFn( *this, &RpcHttpTransport::myWorkerFunction ));
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
RpcHttpTransport::control( const eRpcControl eCtl)
{
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
                CBLOGDBG(_logger, NTEXT("RpcHttpTransport::control: Start Command entered"));
                // tell the thread to actually start processing
                // reset startedEvent because we are going to wait on it
                // signal the trigger so that myWorkerFunction can start
                // doing work
                _startedEvent.reset();
                if(_triggerEvent.signal())
                {
                    res = _startedEvent.wait() == 0;
                }
                
                CBLOGDBG(_logger, NTEXT("RpcHttpTransport::control: Start Command Completed"));
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
                CBLOGDBG(_logger, NTEXT("RpcHttpTransport::control: Stop Command entered"));
                
                // reset stoppedEvent because we are going to wait on it
                // reset trigger event so that the worker thread will 
                //	block on it in myWorkerFunction when the thread has stopped
                _stoppedEvent.reset();
                _triggerEvent.reset();
                
                // doing a hard close on the socket will cause the listener in 
                // processRequest to return a NULL socket, in which case
                // processRequest will return and block on the trigger
                setStatus( keRpcStopPending );
                _listener.close();
                
                res = _stoppedEvent.wait() == 0;
                
                CBLOGDBG(_logger, NTEXT("RpcHttpTransport::control: Stop Command Completed"));
            }
            
            // we break only if we were requested to perform a stop
            if ( eCtl == keStopCtl )
            {
                break;
            } // otherwise the fall through is intentional
            
            CBLOGDBG(_logger, NTEXT("RpcHttpTransport::control: Shutdown Command entered"));
            
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
            
            CBLOGDBG(_logger, NTEXT("RpcHttpTransport::control: Shutdown Command Completed"));
            break;

        default:
            CBLOGDBG(_logger, NTEXT("RpcHttpTransport::control: unknown state"));
            break;
        }
    }
    
    return res;
}


/**
   process http requests coming in
*/
void
RpcHttpTransport::processRequest()
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

        CBLOGDBG(_logger, NTEXT("RpcHttpTransport::processRequest: received socket connection...") );

        RefCountedPtr<HttpSerPost> post(new HttpSerPost(newSock));
        RefCountedPtr<HttpContent> content(new HttpContent);
        if (	post->readRequest( *content ) && 
                ( content->getHttpMsgType() == HttpContent::HTTP_MSG_TYPE_REQUEST ) )
        {
            int errCode = HttpUtils::keCodeBadRequest;
            String payload;
            
            if ( content->getIsForm() )
            {
                payload = content->getQueryString( Rpc::PAYLOAD );
            }
            else
            {
                payload = content->getRequestBody();
            }
            
            // Now process the payload if it is not empty
            if ( !payload.empty() )
            {
                CBLOGDBG(_logger, NTEXT("RpcHttpTransport::processRequest: receiving request - ") + payload );
                
                // store post in pending list
                unsigned long sessionId = _sSessionId++;
                PendingPostReference postRef(post, content);
                addPost( sessionId, postRef );

//BUGBUG                handleContent( sessionId, content );
                _masterServer->queueMessage( sessionId, content->getHeaders(), payload, RPC_HTTP_NAME );
                
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
RpcHttpTransport::status()
{
    ThdAutoCriticalSection lock(_guard);
    return _state;
}


/**
   set the status of the rpc transport
*/
void 
RpcHttpTransport::setStatus(
    const eRpcStatus rpcStatus )
{
    ThdAutoCriticalSection lock(_guard);
    if(_state != keRpcShutdown)
        _state = rpcStatus;
}


void 
RpcHttpTransport::addPost(
    const unsigned long id, 
    PendingPostReference& post )
{
    ThdAutoCriticalSection lock(_guard);
    _pendingServer[ id ] = post;
}


RpcHttpTransport::PendingPostReference
RpcHttpTransport::removePost( 
    const unsigned long id )
{
    PendingPostReference res;
    
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
RpcHttpTransport::addClientPost(
    RefCountedPtr<HttpClientPost>& post )
{
    ThdAutoCriticalSection lock(_guard);
    _pendingClient[ post ] = post;
}


void
RpcHttpTransport::removeClientPost(
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
RpcHttpTransport::sendRequest( 
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
//        if( status() != keRpcStarted )
//        {
//            CBLOGERR(_logger, NTEXT("RpcHttpClientTransport::sendRequest - Unable to Send data because the server is down") );
//            break;
//        }
        
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
RpcHttpTransport::sendResponse(
    const unsigned long sessionId,
    const StringUtils::StringMap &headers,
    const String &response )
{
    // this function is called to respond to a request
    // in the context of an inbound request
    PendingPostReference post = removePost( sessionId );
    if ( post.first != NULL )
    {
        bool hasBody = response.length() > 0;
        
        HttpContent content;
        content.setHttpMsgType( HttpContent::HTTP_MSG_TYPE_RESPONSE );
        content.setHeaders( headers );
        content.setErrorCode( hasBody ? HttpUtils::keCodeOk : HttpUtils::keCodeNoContent);

        // this has to be performed after the setHeaders on the content 
        // to make sure that the accept types have been set to 
        // check agains
        if ( post.second->doesAcceptType(HttpUtils::HTTP_MIME_XMLTEXT) )
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
        
        if ( !post.first->sendResponse( content ) )
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
RpcHttpTransport::myWorkerFunction()
{
    CBLOGDBG(_logger, NTEXT("RpcHttpTransport::myWorkerFunction: entered"));
    
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
    
    CBLOGDBG(_logger, NTEXT("RpcHttpTransport::myWorkerFunction: exited"));



    setStatus(keRpcShutdown);
    _shutdownEvent.signal();
    
    return res;
}


/**
   Send Default Message that server is shutting Down
**/
void 
RpcHttpTransport::abortAllPosts()
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
        PendingPostReference& post = (PendingPostReference&)((*itr).second);
        post.first->sendResponse(errContent);
    }
}


/**
   tell all client posts to shutdown
**/
void 
RpcHttpTransport::abortAllClientPosts()
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
RpcHttpTransport::setShuttingDown( const bool down )
{
    ThdAutoCriticalSection lock(_guard);
    _shuttingDown = down;
}

/** get function for shutting down flag
 **/
bool
RpcHttpTransport::getShuttingDown() const
{
    ThdAutoCriticalSection lock(_guard);
    return _shuttingDown;
}


int 
RpcHttpTransport::getNumActiveClientPosts()
{
    ThdAutoCriticalSection lock(_guard);
    return (int)_pendingClient.size();
}
