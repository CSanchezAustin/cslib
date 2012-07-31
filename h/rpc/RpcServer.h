#ifndef RpcServer_H_LOADED
#define RpcServer_H_LOADED
// [
// $        RpcServer.h
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
//         RCS:
//                 $Revision: 2.0 $
//                 $Author: $
//                 $Date: $
// ?
//         Contents: RpcServer class
//
//         Purpose: Master RPC server
//
//         Usage Notes:
//     This system component supports the following configuration directives
//     XML Root node: <rpc-server>
//                     Name        Type        Default                 Description
//     Config Element: workers     int         4                       numer of workers to handle requests
//                     transports  see below    N/A                    transport tree
//                     authen      see below    N/A                    authenticator tree
//
//    Reference schema:
//
//    Example:
//      <rpc-server>
//          <workers>4</workers>
//          <transports>
//              <transport name="Name">
//                  Internals are defined by the transport
//              </transport>
//          </transports>
//          <authenticators>
//              <authenticator name="Name">
//                  Internals are defined by the authenticator
//              </authenticator>
//          </authenticators>
//       </rpc-server>
//
//                 
// ?
// !        Changes History
//     $Header: $
//         
// ]

#include <map>
#include <time.h>

#include <cslib.h>
#include <pat/PatRunnable.h>
#include <rpc/RpcTypes.h>
#include <rpc/iRpcServer.h>
#include <sys/iSysComponent.h>
#include <sys/SysComponentFactory.h>
#include <thd/ThdCriticalSection.h>
#include <thd/ThdQueue.h>
#include <thd/ThdRunner.h>
#include <util/UtilRefCountedPtr.h>

#include <xercesc/dom/DOMDocument.hpp>

XERCES_CPP_NAMESPACE_USE

class iRpcTransport;
class iRpcAuthenticator;
class SysContext;
class RpcInterface;
class StdLogger;
class RpcDispatcher;
class RpcAddress;


class EXPORT_DLL RpcServer : public iSysComponent, public iRpcServer
{
public:
    RpcServer();
    ~RpcServer();

    /**
       return the status of the server in a thread safe manner
    */
    eRpcStatus getStatus();

    /**
       add an rpc interface to the dispatcher
    */
    bool addInterface( RefCountedPtr<RpcInterface> &intf );
    
    /**
       and remove an interface
    */
    bool removeInterface( const String &name );

    /**
       [ client interface ]
       send a request to a client server
       @param rpcRequest a DOM document representing the message to send to the server
       @param rpcObject the object to send the request
       @param rpcMethod the method of the object that will handle the request
       @param rpcServerAddress the addess of the server
       @param rpcOutResponse the rpc call document if no error
       @return the RPC result code
    */
    enumRpcCallResult sendRequest(
            const DOMNode* rpcRequest,
            const String &rpcObject,
            const String &rpcMethod,
            const RpcAddress& rpcServerAddress,
            DOMNode* rpcOutResponse );

    /**
       [ transport interface ]
       this function is called by an rpc transport
       when it has a message for the rpc server to 
       process.
       @param sessionId a session identifier assigned 
       and managed by the transport
       @param headers some message headers the rpc server may use
       @param data the message the transport has received
       @param fromTransport the transport that queued the message 
       @return the return whether or not the server 
       will handle the message
    */
    bool queueMessage( 
            const unsigned long sessionId,
            const StringUtils::StringMap& headers,
            const String &data,
            const String& fromTransport );

    /*
      Called after initialization of the system context. this allows
      system components to utilize other components as necessary
    */
    int postInit( 
            const DOMNode* config, 
            RefCountedPtr<SysContext> &ctx );

    /*
      called to initialize the component. DOMNode is a configuration
      for which the sub-tree represented by getConfigKey will be fetched
      and used to configure this component. The reason that the entire tree
      is used is because there may be some shared configuration data
      that this component may use...
    */
    int init( 
            const DOMNode* config, 
            RefCountedPtr<SysContext>& ctx );

    /**
       used to close the system component
    */
    int close();

    /*
      called to shutdown a component. DOMNode is the same configuration
      node that was passed to init. The component should update the any
      data that it wants persisted
    */
    int update( DOMNode* config ) { return 0; }

    /**
       get the name of the component
    */
    const String& getComponentName() { return RPC_SERVER_NAME; }

    static const String& getRegistryName() { return RPC_SERVER_NAME; }

private:
    enum 
    {
        keMaxWorkers = 4
    };

    class WorkerMessage
    {
    public:
        WorkerMessage() 
        {
            _time = ::clock();
        }
        WorkerMessage(
            const unsigned long sessionId,
            const StringUtils::StringMap& headers,
            const String &data,
            const String& fromTransport ) :
            _sessionId(sessionId),
            _headers(headers),
            _data(data),
            _fromTransport(fromTransport)
        {
            _time = ::clock();
        }

        WorkerMessage( const WorkerMessage &rhs )
        {
            _sessionId = rhs._sessionId;
            _headers = rhs._headers;
            _data = rhs._data;
            _fromTransport = rhs._fromTransport;
            _time = rhs._time;
        }

        WorkerMessage& operator=(const WorkerMessage rhs)
        {
            if ( this != &rhs )
            {
                _sessionId = rhs._sessionId;
                _headers = rhs._headers;
                _data = rhs._data;
                _fromTransport = rhs._fromTransport;
                _time = rhs._time;
            }

            return *this;
        }

        const unsigned long getSessionId() const { return _sessionId; }
        const StringUtils::StringMap& getHeaders() const { return _headers; }
        const String& getData() const { return _data; }
        const String& getTransport() const { return _fromTransport; }
        const clock_t getStartTime() const { return _time; }
	const clock_t getElapsedTime() const { return ::clock() - _time; }

    private:
        unsigned long _sessionId;
        StringUtils::StringMap _headers;
        String _data;
        String _fromTransport;
        clock_t _time;
    };

    typedef std::map< String, RefCountedPtr<iRpcTransport> > TransportMap;
    typedef std::map< String, RefCountedPtr<iRpcAuthenticator> > AuthenticatorMap;
    typedef ThdQueue< RefCountedPtr<WorkerMessage> > WorkerQueue;
    typedef ThdRunner<RpcServer> WorkerRunner;

    RefCountedPtr<StdLogger>    _logger;
    WorkerQueue                 _messageQueue;

    RefCountedPtr<RpcDispatcher> _dispatcher;

    TransportMap _transports;

    ThdCriticalSection _guard;
    eRpcStatus         _state;

    AuthenticatorMap _authenticators;

    static const String RPC_SERVER_NAME;
    static const String RPC_WORKER_THD;
    static const String RPC_NAME_ATT;
    static const String RPC_TRANSPORTS_ELEM;
    static const String RPC_TRANSPORT_ELEM;
    static const String RPC_AUTHENTICATORS_ELEM;
    static const String RPC_AUTHENTICATOR_ELEM;

private:

    /**
       start the rpc server, the return indicates whether or not
       the server is in a ready state to receive messages
    */
    bool start();

    /**
       stops the rpc server, the return indicates whether or not
       the server was indeed stopped
    */
    bool stop();

    /**
       used to completely shutdown the rpc server
    */
    bool shutdown();

    /**
       These internal functions are used to add transports
       and authenticators to the rpc server
    */
    void addTransport( RefCountedPtr<iRpcTransport> &intf );
    void removeTransport( const String& name );
    RefCountedPtr<iRpcTransport> getTransport(const String& name);

    void addAuthenticator( RefCountedPtr<iRpcAuthenticator> &intf );
    void removeAuthenticator( const String& name );
    RefCountedPtr<iRpcAuthenticator> getAuthenticator(const String& name);

    /**
       internal function to send a command to all 
       transports and returns true if at least one 
       transport acknowledged the control
    */
    bool commandAll(const eRpcControl nextCmd);

    /**
       set the value of the status in a thread safe manner
    */
    void setStatus(const eRpcStatus state);

    /**
       this is the worker thread entry point. All requests are retrieved
       from a shared queue
    */
    unsigned doWork();

    /**
       call all registered authenicators to have one authenticate the 
       current request. If no authenticator is able to verify authentication
       then the return result is false. responseHeaders may be modified by
       a given authenticator
    */
    bool authenticate( 
        const StringUtils::StringMap& requestHeaders, 
        StringUtils::StringMap& responseHeaders );

    // Unimplemented standard methods
    RpcServer( const RpcServer& );
    RpcServer& operator=(const RpcServer&);

    DEFINE_SYS_COMPONENT(RpcServer)
};

// RpcServer_H_LOADED
#endif
