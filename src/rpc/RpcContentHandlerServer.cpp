//      RpcContentServer.cpp
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
//              $Revision: 2.0 $
//              $Author: $
//              $Date: $
//
//      Contents:   an extensible http service allowing for new content handlers
//                  to be added, or by default serving up a file from the file system
//	
// 
#include <cslib.h>

#include <dom/DOM_Node.hpp>
#include <dom/DOM_Element.hpp>
#include <dom/DOM_NodeList.hpp>

#include <rpc/RpcContentServer.h>
#include <rpc/RpcDispatcher.h>
#include <rpc/iRpcTransport.h>
#include <rpc/iRpcAuthenticator.h>
#include <rpc/RpcInterface.h>
#include <rpc/RpcAddress.h>
#include <rpc/RpcTransportFactory.h>
#include <rpc/RpcAuthenticatorFactory.h>
#include <log/LogStdLogger.h>
#include <sys/SysContext.h>
#include <thd/ThdAutoLock.h>
#include <thd/ThdPool.h>
#include <util/UtilRefCountedPtrInlines.h>
#include <util/UtilStringUtils.h>
#include <util/UtilMapping.h>
#include <xml/XmlStringFormatTarget.h>
#include <xml/XmlDomUtils.h>

const String RpcContentServer::RPC_SERVER_NAME = NTEXT("content-server");
const String RpcContentServer::RPC_WORKER_THD = NTEXT("workers");
const String RpcContentServer::RPC_NAME_ATT = NTEXT("name");
const String RpcContentServer::RPC_TRANSPORTS_ELEM = NTEXT("content-handlers");
const String RpcContentServer::RPC_TRANSPORT_ELEM = NTEXT("content-handler");

RpcContentServer::Initializer RpcContentServer::_init;


RpcContentServer::RpcContentServer() :
    _logger(NULL),
    _state(keRpcNoState)
{
}


RpcContentServer::~RpcContentServer()
{
    CBLOGDBG( _logger, NTEXT("RpcContentServer::~RpcContentServer - closing service") );

    shutdown();
}


/**
    start the rpc server, the return indicates whether or not
    the server is in a ready state to receive messages
    Check the state of the server and then iterate all the
    transports and instruct them what to do.
*/
bool
RpcContentServer::start()
{
    bool res = false;

    eRpcControl nextCmd = keNoCtl;
    eRpcStatus currentStatus = getStatus();

    ASSERT_D(currentStatus != keRpcNoState);
    ASSERT_D(currentStatus != keRpcStarted);
    ASSERT_D(currentStatus != keRpcShutdown);

    if ((currentStatus == keRpcStopped) || (currentStatus == keRpcInitted))
    {
        nextCmd = keStartCtl;
    }

    res = commandAll(nextCmd);
    if ( res )
    {
        setStatus(keRpcStarted);
    }

    return res;
}


/**
    start the rpc server, the return indicates whether or not
    the server is in a ready state to receive messages. 
    First we command all transports to stop working
    then we tell the message queue to shutdown
*/
bool 
RpcContentServer::stop()
{
    bool res = false;

    eRpcControl nextCmd = keNoCtl;
    eRpcStatus currentStatus = getStatus();

    ASSERT_D(currentStatus != keRpcNoState);
    ASSERT_D(currentStatus != keRpcShutdown);
    ASSERT_D(currentStatus != keRpcStopped);

    if (currentStatus == keRpcStarted)
    {
        nextCmd = keStopCtl;
    }

    res = commandAll(nextCmd);
    if ( res )
    {
        setStatus(keRpcStopped);
    }

    return res;
}


/**
   used to completely shutdown the rpc server
*/
bool
RpcContentServer::shutdown()
{
    CBLOGDBG(_logger, NTEXT("RpcContentServer::shutdown: entered"));

    eRpcStatus currentStatus = getStatus();
    if ( currentStatus != keRpcShutdown && 
         currentStatus != keRpcNoState )
    {
        if ( currentStatus == keRpcStarted )
        {
            stop();
        }

        // have to do some of these under a lock
        _messageQueue.down();
        {
            ThdAutoCriticalSection lock(_guard);
            _transports.clear();
        }

        setStatus(keRpcShutdown);
    }

    CBLOGDBG(_logger, NTEXT("RpcContentServer::shutdown: left"));

    return true;
}


/**
    internal function to send a command to all 
    transports and returns true if at least one 
    transport acknowledged the control. Note: we are not synchronizing
    on the transport map since it will never change while the 
    program is running.
*/
bool
RpcContentServer::commandAll(
    const eRpcControl nextCmd)
{
    bool res = false;
    if ( nextCmd != keNoCtl )
    {
        for (TransportMap::iterator it = _transports.begin();
             it != _transports.end(); it++ )
        {
            if( (*it).second->control( nextCmd) )
            {
                CBLOGDBG(_logger, NTEXT("RpcContentServer::commandAll - controlled transport: ") + 
                         (*it).second->getName() + 
                         NTEXT(", ctl: ") +
                         StringUtils::toString(int(nextCmd)) );
                res = true;
            }
            else
            {
                CBLOGDBG(_logger, NTEXT("RpcContentServer::commandAll - falied to control transport: ") + 
                         (*it).second->getName() + 
                         NTEXT(", ctl: ") +
                         StringUtils::toString(int(nextCmd)) );
            }
        }
    }
    
    return res;
}


/**
   return the status of the server in a thread safe manner
*/
eRpcStatus 
RpcContentServer::getStatus()
{
    ThdAutoCriticalSection lock(_guard);
    return _state;
}


/**
   set the value of the status in a thread safe manner
*/
void
RpcContentServer::setStatus(
    const eRpcStatus state)
{
    ThdAutoCriticalSection lock(_guard);
    _state = state;
}


void 
RpcContentServer::addTransport(
    RefCountedPtr<iRpcTransport> &intf )
{
    CBLOGDBG( _logger, NTEXT("RpcContentServer::addTransport added transport: ") + intf->getName() );
    
    _transports[ intf->getName() ] = intf;
    
    if ( getStatus() == keRpcStarted )
        intf->control(keStartCtl);
}


void 
RpcContentServer::removeTransport( 
    const String& name )
{
    CBLOGDBG( _logger, NTEXT("RpcContentServer::removeTransport removing transport: ") + name );

    TransportMap::iterator it = _transports.find( name );
    if ( it != _transports.end() )
    {
        if ( getStatus() == keRpcStarted )
            (*it).second->control(keStopCtl);
        
        _transports.erase( it );
    }
}


RefCountedPtr<iRpcTransport> 
RpcContentServer::getTransport(const String& name)
{
    RefCountedPtr<iRpcTransport> res;
    
    TransportMap::iterator it = _transports.find( name );
    if ( it != _transports.end() )
        res = (*it).second;
    
    return res;
}



/**
     add an rpc interface to the dispatcher
*/
bool 
RpcContentServer::addInterface( 
    RefCountedPtr<RpcInterface> &intf )
{
    ASSERT_D(_dispatcher != NULL);
    CBLOGDBG( _logger, NTEXT("RpcContentServer::addInterface added interface: ") + intf->getName() );
    return _dispatcher->addInterface( intf );
}


/**
     and remove an interface
*/
bool 
RpcContentServer::removeInterface( 
    const String &name )
{
    ASSERT_D(_dispatcher != NULL);
    CBLOGDBG( _logger, NTEXT("RpcContentServer::removeInterface removed interface: ") + name );
    return _dispatcher->removeInterface( name );
}

/*
    called to initialize the component. DOM_Node is a configuration
    for which the sub-tree represented by getConfigKey will be fetched
    and used to configure this component. The reason that the entire tree
    is used is because there may be some shared configuration data
    that this component may use...
*/
int 
RpcContentServer::init( 
    const DOM_Node &config, 
    RefCountedPtr<SysContext> &ctx )
{
    int res = -1;

    ASSERT_D(getStatus() == keRpcNoState);

    _logger = (RefCountedPtr<StdLogger>&)ctx->getComponent( StdLogger::getRegistryName() );

    RefCountedPtr<ThdPool> pool = (RefCountedPtr<ThdPool>&)ctx->getComponent( ThdPool::getRegistryName() );
    ASSERT_D(pool != NULL);

    int workers = keMaxWorkers;

    if ( config.getNodeType() == DOM_Node::ELEMENT_NODE )
    {
        DOM_Element configElem = (DOM_Element&)config;

        //
        // READ THE HEADER TO UNDERSTAND THIS SEQUENCE
        //

        // first look for the max size setting
        String val;
        DOM_NodeList nodes = configElem.getElementsByTagName( RPC_WORKER_THD.c_str() );
        if ( DomUtils::getNodeValue( (const DOM_Element&)nodes.item( 0 ), &val ) )
        {
            workers = StringUtils::toInt( val );
        }

        // now look for transports to use
        DOM_NodeList transportRoot = configElem.getElementsByTagName( RPC_TRANSPORTS_ELEM.c_str() );
        if ( transportRoot.getLength() > 0 )
        {
            // a transport tree contains one or more transport elements, however
            // there can be only one transfer root
            const DOM_Element elem = (const DOM_Element&)transportRoot.item(0);
            ASSERT_D(elem.getNodeType() == DOM_Node::ELEMENT_NODE ); 
            DOM_NodeList transport = elem.getElementsByTagName( RPC_TRANSPORT_ELEM.c_str() );
            for ( unsigned int i = 0, sz = transport.getLength(); i < sz; i++ )
            {
                // the name is an attribute of a transport configuration
                const DOM_Element cfg = (const DOM_Element&)transport.item(i);
                Mapping attrs;
                if ( DomUtils::getAttributes( cfg, attrs ) )
                {
                    String transportName;
                    attrs.getIfAvailable(RPC_NAME_ATT, transportName);

                    RefCountedPtr<iRpcTransport> obj( RpcTransportFactory::createObject( transportName ) );
                    if ( (obj != NULL) && (obj->init( cfg, ctx , this) == 0) )
                    {
                        res = 0;
                        addTransport( obj );
                    }
                    else
                    {
                        CBLOGERR( _logger, NTEXT("RpcContentServer::init: can't find transport: ") + transportName );
                    }
                }
                else
                {
                    CBLOGERR( _logger, NTEXT("RpcContentServer::init: can't find attributes to configure transport"));
                }
            }
        }
    }

    if ( res == 0 )
    {
        setStatus(keRpcInitted);

        // now add the workers to the thread pool
        RefCountedPtr<iRunnable> theRunner( new WorkerRunner( *this, &RpcContentServer::doWork) );
        pool->add( workers, theRunner );
	
        // finally try to start the transport
        res = start() ? 0 : -1;
    }

    return res;
}


/**
    this is the worker thread entry point. All requests are retrieved
    from a shared queue. We assume that if a work item makes it into the
    queue, then we have to process it. Server control exists at a higher level
    thus we do not have to worry about it at this level
*/
unsigned 
RpcContentServer::doWork()
{
    CBLOGDBG( _logger, NTEXT("RpcContentServer::doWork - entering worker thread") );

    while ( true )
    {
        RefCountedPtr<WorkerMessage> request;
        enumSyncOpResult res = _messageQueue.remove( request );
        if (res == SYNCOP_SUCCESS)
        {
            // response structures always get populates
            StringFormatTarget targ;
            DOM_Document responseDoc = DomUtils::createDocument( Rpc::RESPONSE_NAME );
            StringUtils::StringMap responseHeaders;

            // make sure that we have a valid transport
            RefCountedPtr<iRpcTransport> transport = getTransport( request->getTransport() );
            ASSERT_D( transport != NULL );
            if ( transport == NULL )
            {
                CBLOGERR( _logger, NTEXT("RpcContentServer::doWork - could not find the requested transport: ") + 
                          request->getTransport() );
                request = NULL;
                continue;
            }

            // BUGBUG: add content filtering here

        }
        else
        {
            CBLOGDBG( _logger, NTEXT("RpcContentServer::doWork - got an abort message"));
            break;
        }

        request = NULL;
    }

    CBLOGDBG( _logger, NTEXT("RpcContentServer::doWork - exiting worker thread") );
    return 0;
}



/**
    this function is called to tell the server to handle a message
    this function merely creates a new rpc message and passes it
    to the rpc worker threads via the message queue. If the rpc server is down,
    then we don't allow messages to be routed
*/
bool 
RpcContentServer::queueMessage(
    const unsigned long sessionId,
    const StringUtils::StringMap& headers,
    const String &data,
    const String& fromTransport )
{
    bool res = false;
    if ( getStatus() == keRpcStarted )
    {
        RefCountedPtr<WorkerMessage> message(
            new WorkerMessage( sessionId, headers, data, fromTransport ) );
        _messageQueue.insert( message );
        res = true;
    }

    return res;
}

/**
    used to close the system component
*/
int 
RpcContentServer::close() 
{
    shutdown();
    return 0; 
}


/**
    send a request to a client server
    @param rpcRequest a DOM document representing the message to send to the server
    @param rpcObject the object to send the request
    @param rpcMethod the method of the object that will handle the request
    @param rpcServerAddress the addess of the server
    @param rpcOutResponse the rpc call document if no error
    @return the RPC result code
*/
enumRpcCallResult 
RpcContentServer::sendRequest(
    const DOM_Node &rpcRequest,
    const String &rpcObject,
    const String &rpcMethod,
    const RpcAddress& rpcServerAddress,
    DOM_Node &rpcOutResponse )
{
    ASSERT_D( !rpcRequest.isNull() );

    if ( rpcRequest.isNull() )
    {
        // have to have a document to send.
        CBLOGDBG( _logger, NTEXT("RpcContentServer::sendRequest - invalid DOM document attempted to be sent.") );
        return keParamError;
    }

    RefCountedPtr<iRpcTransport> transport = getTransport( rpcServerAddress.getTransport() );
    if ( transport == NULL )
    {
        CBLOGERR( _logger, 
                  NTEXT("RpcContentServer::sendRequest - can not find transport to send to. Looking for: ") +
                  rpcServerAddress.getTransport() );
        return keParamError;
    }

    // first add the processing instruction in the form
    // <?rpc object='rpcObject' method='rpcMethod' [xml='path to root']?>
    String request;
    request = OPENANGLEBRACKET;
    request += QUESTION;
    request += Rpc::TARGET_PI;
    request += SPACE;
    request += Rpc::OBJECT_ATTR;
    request += EQUAL;
    request += APOSTROPHE;
    request += rpcObject;
    request += APOSTROPHE;
    request += SPACE;
    request += Rpc::METHOD_ATTR;
    request += EQUAL;
    request += APOSTROPHE;
    request += rpcMethod;
    request += APOSTROPHE;

    // the payload is optional
    if ( !rpcRequest.isNull() )
    {
        request += SPACE;
        request += Rpc::XML_ATTR;
        request += EQUAL;
        request += APOSTROPHE;
        std::autoptr<char> nodeName(rpcRequest.getNodeName().transcode());
        request += nodeName.get();
        request += APOSTROPHE;
    }

    request += QUESTION;
    request += CLOSEANGLEBRACKET;

    if ( !rpcRequest.isNull() )
    {
        // now construct the body of the message enclosing it in <rpc-request></rpc-request>
        request += OPENANGLEBRACKET;
        request += Rpc::REQUEST_NAME;
        request += CLOSEANGLEBRACKET;

        // pipe the request structure in rpcRequest to the body of the tags
        StringFormatTarget targ;
        DomUtils::print( rpcRequest, NTEXT("UTF-8"), targ );
        request += targ.getString();

        // now close the request structure
        request += OPENANGLEBRACKET;
        request += SLASH;
        request += Rpc::REQUEST_NAME;
        request += CLOSEANGLEBRACKET;
    }

    // we should be ready to send the request to the server to handle
    String response;
    enumRpcCallResult callResult = keError;
    if ( transport->sendRequest( rpcServerAddress.getAddress(), request, response ) )
    {
        // then extract the data from the response string
        DOM_Document doc = DomUtils::toDocument( response );

        // we expect the response data to have a processing instruction similar to the request one (see comment above)
        // followed by a body in the form of <rpc-response> <something/> </rpc-response>
        // the <something/> tag should be the response node that the processing instruction header points to

        // we expect the header pi to contain the result attribute
        std::vector<DOM_ProcessingInstruction> pis;
        DomUtils::findProcessingInstructions( doc, Rpc::TARGET_PI, pis );
        if ( pis.size() > 0 )
        {
            Mapping attributes;
            if ( DomUtils::getAttributes( pis[0], attributes ) )
            {
                String resultStr;
                attributes.getIfAvailable(Rpc::RESULT_ATTR, resultStr);

                callResult = enumRpcCallResult( StringUtils::toInt( resultStr ) );
                if ( callResult == keOk )
                {
                    // we have a result, now we have to retrieve the data and return it to the caller
                    DOM_Node responseNode = doc.getDocumentElement();
                    std::autoptr<char> nodeName(responseNode.getNodeName().transcode());
                    if ( Rpc::RESPONSE_NAME.compare( nodeName.get() ) == 0 )
                    {
                        // since this was a callResult == keOk we expect an 'xml' attribute in the processing instruction
                        // that gives us the path to the response xml data island in the response document
                        
                        attributes.getIfAvailable(Rpc::XML_ATTR, resultStr);
                        if ( !DomUtils::selectSingleNode( responseNode, resultStr, rpcOutResponse ) )
                        {
                            // not fatal, we just state there is no return data
                            callResult = keOkNoResult;
                        }
                    }
                    else
                    {
                        StringFormatTarget targ;
                        DomUtils::print( doc, NTEXT("UTF-8"), targ );
                        CBLOGERR(   _logger, 
                                    NTEXT("RpcContentServer::sendRequest - no root element called 'rpc-response': ") + 
                                    targ.getString() );
                        callResult = keError;
                    }
                }
            }
            else
            {
                StringFormatTarget targ;
                DomUtils::print( doc, NTEXT("UTF-8"), targ );
                CBLOGERR(   _logger, 
                            NTEXT("RpcContentServer::sendRequest - unable to retrieve attributes of processing instruction") + 
                            targ.getString() );
                callResult = keError;
            }
        }
        else
        {
            StringFormatTarget targ;
            DomUtils::print( doc, NTEXT("UTF-8"), targ );
            CBLOGERR(   _logger, 
                        NTEXT("RpcContentServer::sendRequest - xml response result had no processing instruction: ") + 
                        targ.getString() );
            callResult = keError;
        }
    }

    return callResult;
}


int 
RpcContentServer::postInit( 
    const DOM_Node &config, 
    RefCountedPtr<SysContext> &ctx )
{
    if ( _logger == NULL )
    {
        _logger = (RefCountedPtr<StdLogger>&)ctx->getComponent( StdLogger::getRegistryName() );
    }

    return 0;
}
