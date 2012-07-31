#ifndef RpcHttpTransport_H_LOADED
#define RpcHttpTransport_H_LOADED
// [
// $        RpcHttpTransport.h
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
//         Contents: RpcHttpTransport class
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
class HttpContent;


class EXPORT_DLL RpcHttpTransport : public iRpcTransport
{
public:
    RpcHttpTransport();
    ~RpcHttpTransport();

    int init(	const DOMNode* config, 
                RefCountedPtr<SysContext> &ctx, 
                iRpcServer* masterServer );
    
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

    /**
       [ client interface ]
       call to send a request to another transport
       @param address the RPC address to send the request to
       @param request the data to send
       @param response the response to the request. On input null, on
       output the response of the message
       @return the synchronous result of the call
    */
    bool sendRequest( 
        const String& address,
        const String& request, 
        String& response );
    
    /**
       [ server interface ]
       The RpcServer will call this function to 
       return a response to a request
    */
    void sendResponse( 
        const unsigned long sessionId,
        const StringUtils::StringMap &headers,
        const String &response );

    static const String& getRegistryName() { return RPC_HTTP_NAME; }

private:
    typedef std::pair<RefCountedPtr<HttpSerPost>, RefCountedPtr<HttpContent> > PendingPostReference;
    typedef std::map<unsigned long, PendingPostReference > PendingServerPosts;
    typedef std::map<RefCountedPtr<HttpClientPost>, RefCountedPtr<HttpClientPost> > PendingClientPosts;
    typedef ThdRunner<RpcHttpTransport> MyThdFn;

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
        PendingPostReference& post );

    PendingPostReference removePost(
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
    RpcHttpTransport( const RpcHttpTransport& );
    RpcHttpTransport& operator=(const RpcHttpTransport&);

private:
	DEFINE_RPC_TRANSPORT(RpcHttpTransport)
};

// RpcHttpTransport_H_LOADED
#endif
