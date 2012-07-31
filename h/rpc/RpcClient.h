#ifndef RpcClient_H_LOADED
#define RpcClient_H_LOADED
// [
// $        RpcClient.h
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
//         Contents: RpcClient class
//
//      
//    Reference schema:
//
//    Example:
//      <rpc-client>
//          <transports>
//              <transport name="Name">
//                  Internals are defined by the transport
//              </transport>
//          </transports>
//       </rpc-client>           
// ?
// !        Changes History
//     $Header: $
//         
// ]

#include <map>

#include <cslib.h>
#include <rpc/RpcTypes.h>
#include <rpc/RpcAddress.h>
#include <sys/iSysComponent.h>
#include <sys/SysComponentFactory.h>
#include <thd/ThdCriticalSection.h>
#include <util/UtilRefCountedPtr.h>

class iRpcClientTransport;
class SysContext;
class StdLogger;
class Mapping;
class DOM_Document;


class EXPORT_DLL RpcClient : public iSysComponent
{
public:
    RpcClient();
    ~RpcClient();

    /**
       return the status of the server in a thread safe manner
    */
    eRpcStatus getStatus();

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
       function to call the rpc method
    */
    enumRpcCallResult callMethod( 
        const RpcAddress& address,
        const DOM_Node& requestnode,
        const String& object,
        const String& method,
        DOM_Node& responsenode);

    /**
       used to close the system component
    */
    int close();

    /*
      called to shutdown a component. DOM_Node is the same configuration
      node that was passed to init. The component should update the any
      data that it wants persisted
    */
    int update( DOM_Node &config ) { return 0; }

    /**
       get the name of the component
    */
    const String& getComponentName() { return RPC_CLIENT_NAME; }

    static const String& getRegistryName() { return RPC_CLIENT_NAME; }

private:

    typedef std::map< String, RefCountedPtr<iRpcClientTransport> > TransportMap;

    RefCountedPtr<StdLogger>    _logger;
    TransportMap _transports;
    ThdCriticalSection _guard;
    eRpcStatus      _state;

    static const String RPC_CLIENT_NAME;
    static const String RPC_NAME_ATT;
    static const String RPC_TRANSPORTS_ELEM;
    static const String RPC_TRANSPORT_ELEM;


private:
    /**
       These internal functions are used to add transports
       and authenticators to the rpc server
    */
    void addTransport( RefCountedPtr<iRpcClientTransport> &intf );
    void removeTransport( const String& name );
    RefCountedPtr<iRpcClientTransport> getTransport(const String& name);


    /**
       set the value of the status in a thread safe manner
    */
    void setStatus(const eRpcStatus state);
    
    friend class Initializer;
    class Initializer
    {
    public:
        Initializer()
        {
            SysComponentFactory::registerObject(
                RpcClient::getRegistryName(), creatorFunction );
        }

        static iSysComponent* creatorFunction() { return new RpcClient; }
    };

    static Initializer _init;

    // Unimplemented standard methods
    RpcClient( const RpcClient& );
    RpcClient& operator=(const RpcClient&);
};

// RpcClient_H_LOADED
#endif
