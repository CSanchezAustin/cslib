#ifndef RpcTypes_H_LOADED
#define RpcTypes_H_LOADED
// [
// $        RpcTypes.h
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
//                 $State: Exp $
//                 $Author: $
//                 $Date: $
// ?
//         Contents: RpcTypes class
//
//         Purpose: Represents a specific method of an
//                  RPC interface
//
//         Usage Notes:
//
//                 
// ?
// !        Changes History
//         
// ]

#include <cslib.h>

EXPORT_DLL
namespace Rpc
{
    const String TARGET_PI = NTEXT("rpc");
    const String REQUEST_NAME = NTEXT("rpc-request");
    const String RESPONSE_NAME = NTEXT("rpc-response");
    const String OBJECT_ATTR = NTEXT("object");
    const String METHOD_ATTR = NTEXT("method");
    const String XML_ATTR = NTEXT("xml");
    const String RESULT_ATTR = NTEXT("result");
    const String ADDRESS_ATTR = NTEXT("address");
    const String AUTH_CODE = NTEXT("rpc-auth");
    const String PAYLOAD = NTEXT("rpc-payload");
    const String RQST_TIME = NTEXT("rpc-time");
}

EXPORT_DLL
enum enumRpcCallResult
{
    keOk = 0,       // same as no error
    keOkNoResult,   // a response is not needed
    keError,        // an error occurred
    keParamError,   // parameter error
    keNoRpcObjectParam,  // no RPC object parameter
    keNoRpcMethodParam,  // ... parameter
    keNoRpcObject,  // no RPC object
    keNoRpcMethod,  // ...
    keCallAuthFail  // RPC authentication failed
};


EXPORT_DLL
enum eRpcStatus
{
    keRpcNoState = 0,   // the rpc server state is undefined
    keRpcInitted,       // the rpc server has been initialized
    keRpcStopped,       // the server was stopped
    keRpcStarted,       // the server is running
    keRpcShutdown,      // the server was shutdown
    keRpcError,         // the server encountered an error initializing
    keRpcStopPending   // the server is stopping
};

EXPORT_DLL
enum eRpcControl
{
    keNoCtl = 0,
    keStopCtl,
    keStartCtl,
    keShutdownCtl
};

// RpcTypes_H_LOADED
#endif
