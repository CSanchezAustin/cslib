#ifndef RpcHttpAuthenticator_H_LOADED
#define RpcHttpAuthenticator_H_LOADED
// [
// $        RpcHttpAuthenticator.h
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
//         Contents: RpcHttpAuthenticator class
//
//         Purpose: HTTP specific Authenticator
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
#include <rpc/iRpcAuthenticator.h>
#include <rpc/RpcAuthenticatorFactory.h>
#include <util/UtilStringUtils.h>

class EXPORT_DLL RpcHttpAuthenticator : public iRpcAuthenticator
{
public:
    RpcHttpAuthenticator();
    ~RpcHttpAuthenticator();

    int init(	
                const DOMNode* config, 
                RefCountedPtr<SysContext>& ctx );

    // Authenticate returns true of the request data in 
    // inRequest passes validation, false otherwise
    bool authenticate(	
                const StringUtils::StringMap &inHeaders, 
                StringUtils::StringMap &outHeaders );

    const String& getName() { return RPC_HTTPAUTH_NAME; }

    static const String& getRegistryName() { return RPC_HTTPAUTH_NAME; }

private:
    static const String RPC_HTTPAUTH_NAME;

private:
    friend class Initializer;
    class Initializer
    {
    public:
        Initializer()
        {
            RpcAuthenticatorFactory::registerObject(
                RpcHttpAuthenticator::getRegistryName(), creatorFunction );

        }

        static iRpcAuthenticator* creatorFunction() { return new RpcHttpAuthenticator; }
    };

    static Initializer _init;

};

// RpcHttpAuthenticator_H_LOADED
#endif
