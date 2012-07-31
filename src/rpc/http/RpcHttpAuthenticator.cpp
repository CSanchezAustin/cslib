//      RpcHttpAuthenticator.cpp
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

#include <rpc/http/RpcHttpAuthenticator.h>
#include <sys/SysContext.h>
#include <util/UtilRefCountedPtrInlines.h>

const String RpcHttpAuthenticator::RPC_HTTPAUTH_NAME = NTEXT("rpc-auth");

RpcHttpAuthenticator::Initializer RpcHttpAuthenticator::_init;

RpcHttpAuthenticator::RpcHttpAuthenticator()
{
}


RpcHttpAuthenticator::~RpcHttpAuthenticator()
{
}


int 
RpcHttpAuthenticator::init( 
            const DOMNode* config, 
            RefCountedPtr<SysContext>& ctx )
{
    int res = 0;

	// BUGBUG. implement

    return res;
}


// Authenticate returns true of the request data in 
// inRequest passes validation, false otherwise
bool 
RpcHttpAuthenticator::authenticate( 
            const StringUtils::StringMap &inHeaders, 
            StringUtils::StringMap &outHeaders )
{
    bool res  = true;

	// BUGBUG. implement

    return res;
}

