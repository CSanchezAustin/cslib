#ifndef NetSocketServer_H_LOADED
#define NetSocketServer_H_LOADED
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
// Copyright © CALEB Technologies Corp. 2001
// This software is protected under the copyright laws as an 
// unpublished work which was created in the year(s) indicated above,
// and includes information which is confidential and proprietary 
// to CALEB Technologies Corp.  All Rights Reserved.
//----------------------------------------------------------------------------
// [
// $        NetSocketServer.h
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
//         Contents: NetSocketServer class
//
//         Purpose: HTTP specific transport
//
//         Usage Notes:
//     This system component supports the following configuration directives
//     XML Root node: <transports>
//                     Name        Type        Default                 Description
//     Config Element: client-proxy String      N/A                    For client side requests this is the 
//                                                                     proxy information to go through
//                     client-retries Number   5                       Number of retries the client interface should try
//                                                                     to contact the server
//                      
//                     port        String      8080                    server listener port for incoming requests
//                     listeners   String      5                       the number of outstanding listeners to support
//
//    Reference schema:
//          TBD: BUGBUG include
//
//    Example:
//              <transport name="Name">
//                  <client-proxy port="8080">127.0.0.1</client-proxy>
//                  <client-retries timeout="1000">5</client-retries>
//                  <server-port listeners="5">12000</server-port>
//              </transport>
//

//
//                 
// ?
// !        Changes History
//     $Header: $
//         
// ]

#include <cslib.h>
#include <rpc/RpcTypes.h>
#include <rpc/iRpcTransport.h>
#include <rpc/RpcAddress.h>
#include <rpc/RpcTransportFactory.h>
#include <net/NetSocket.h>
#include <thd/ThdRunner.h>
#include <thd/ThdEvent.h>
#include <thd/ThdCriticalSection.h>

class StdLogger;
class HttpSerPost;
class HttpClientPost;
class iRpcServer;


class EXPORT_DLL NetSocketServer : public iSysComponent
{
public:
    NetSocketServer();
    ~NetSocketServer();

    /*
        called to initialize the component. DOM_Node is a configuration
        for which the sub-tree represented by getConfigKey will be fetched
        and used to configure this component. The reason that the entire tree
        is used is because there may be some shared configuration data
        that this component may use...
    */
    int init( const DOM_Node &config, RefCountedPtr<SysContext> &ctx );

    /*
        Called after initialization of the system context. this allows
        system components to utilize other components as necessary
        SEE init()
    */
    int postInit( const DOM_Node &config, RefCountedPtr<SysContext> &ctx );

    /**
        used to close the system component
    */
    int close();

    /*
        called to shutdown a component. DOM_Node is the same configuration
        node that was passed to init. The component should update the any
        data that it wants persisted
    */
    int update( DOM_Node &config ){ return 0; }

    /**
        get the name of the component
    */
	const String& getComponentName() { return NET_SOCKET_SERVER_NAME; }


    
    /**
       control the state of the transport
    */
    bool control( const eRpcControl eCtl );

    /**
       get the status of the rpc transport
    */
    eRpcStatus status();

    /**
       get the name of the transport
    */
    const String& getName() { return RPC_HTTP_NAME; }

    /**
       get the name of the transport
    */
    const RpcAddress& getAddress() { return _transportAddress; }

    static const String& getRegistryName() { return RPC_HTTP_NAME; }

private:
    typedef std::map<unsigned long, RefCountedPtr<HttpSerPost> > PendingServerPosts;
    typedef std::map<RefCountedPtr<HttpClientPost>, RefCountedPtr<HttpClientPost> > PendingClientPosts;
    typedef ThdRunner<NetSocketServer> MyThdFn;

    // defaults for the socket settings
    enum 
    { 
        keDefaultPort = 8080, 
        keBacklog = 5,
        keRetries = 5,
        keSleepInterval = 1000
    };

    eRpcStatus _state;

    RefCountedPtr<StdLogger> _logger;
    iRpcServer* _masterServer;
    RpcAddress _transportAddress;

    bool _shuttingDown;

    mutable ThdCriticalSection _guard;

    // client interface data members
    String _proxy;
    unsigned short _proxyPort;
    int _retries;
    int _sleepInterval;

    // server interface data members

    // inter-thread communication
    ThdEvent _triggerEvent;
    ThdEvent _startedEvent;
    ThdEvent _stoppedEvent;
    ThdEvent _shutdownEvent;
    PendingServerPosts _pendingServer;
    PendingClientPosts _pendingClient;

    // socket parameters
    int _port;
    int _backlog;
    Socket _listener;

    static InterLockedLong _sSessionId;

private:
    static const String RPC_HTTP_NAME;
    static const String RPC_LISTEN_PORT;
    static const String RPC_BACKLOG_ATTR;
    static const String RPC_PROXY_NAME;
    static const String RPC_PROXY_PORTATTR;
    static const String RPC_CLIENT_RETRIES;
    static const String RPC_CLIENT_TOATTR;

    void addPost(
        const unsigned long id, 
        RefCountedPtr<HttpSerPost>& post );

    RefCountedPtr<HttpSerPost> removePost(
        const unsigned long id );

    void addClientPost( 
        RefCountedPtr<HttpClientPost>& post );

    void removeClientPost( 
        RefCountedPtr<HttpClientPost>& post );

    int getNumActiveClientPosts();

    /** Get and set function for shutting down flag
     **/
    void setShuttingDown( const bool down );
    bool getShuttingDown() const;

    /**
       wait for connection requests from clients and dispatch Message to rpcServer
    */
    void processRequest();

    /**
       Send Default Message that server is shutting Down
    **/
    void abortAllPosts();
    void abortAllClientPosts();

    /**
       to Set the the Status
    **/
    void setStatus(const eRpcStatus rpcStatus);

    unsigned myWorkerFunction();

    // Unimplemented standard methods
    NetSocketServer( const NetSocketServer& );
    NetSocketServer& operator=(const NetSocketServer&);

private:
    friend class Initializer;
    class Initializer
    {
    public:
        Initializer()
        {
            RpcTransportFactory::registerObject(
                NetSocketServer::getRegistryName(), creatorFunction );

        }

        static iRpcTransport* creatorFunction() { return new NetSocketServer; }
    };

    static Initializer _init;
};

// NetSocketServer_H_LOADED
#endif
