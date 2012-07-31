#ifndef iRpcTransport_H_LOADED
#define iRpcTransport_H_LOADED
// [
// $        iRpcTransport.h
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
//         Contents: iRpcTransport class
//
//         Purpose: interface for a generic transport
//
//         Usage Notes:
//
//                 
// ?
// !        Changes History
//     $Header: $
//         
// ]

#include <cslib.h>
#include <rpc/RpcTypes.h>
#include <util/UtilRefCountedPtr.h>
#include <util/UtilStringUtils.h>

#include <xercesc/dom/DOMNode.hpp>

XERCES_CPP_NAMESPACE_USE

class iRpcServer;
class RpcServer;
class SysContext;
class RpcAddress;

class EXPORT_DLL iRpcTransport
{
public:
    virtual ~iRpcTransport(){}

    virtual int init(	const DOMNode* config, 
                        RefCountedPtr<SysContext>& ctx, 
                        iRpcServer* masterServer ) = 0;
    /**
       control the state of the transport
    */
    virtual bool control( const eRpcControl eCtl ) = 0;

    /**
       get the status of the rpc transport
    */
    virtual eRpcStatus status() = 0;

    /**
       get the name of the transport
    */
    virtual const String& getName() = 0;

    /**
       get the name of the transport
    */
    virtual const RpcAddress& getAddress() = 0;

    /**
       [ client interface ]
       call to send a request to another transport
       @param address the RPC address to send the request to
       @param request the data to send
       @param response the response to the request. On input null, on
       output the response of the message
       @return the synchronous result of the call
    */
    virtual bool sendRequest( 
        const String& address,
        const String& request, 
        String& response) = 0;

    /**
       [ server interface ]
       The RpcServer will call this function to 
       return a response to a request
    */
    virtual void sendResponse( 
        const unsigned long sessionId,
        const StringUtils::StringMap &headers,
        const String &response ) = 0;
};

// iRpcTransport_H_LOADED
#endif
