//      RpcInterface.cpp
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
//                 $Revision:$
//                 $Author:$
//                 $Date:$
//
//      Contents:	This is the interface class that contains one
//                  or more RPC methods. 
//	
#include <cslib.h>

#include <rpc/iRpcMethod.h>
#include <rpc/RpcInterface.h>
#include <rpc/RpcTypes.h>
#include <log/LogStdLogger.h>
#include <sys/SysContext.h>
#include <thd/ThdAutoLock.h>
#include <util/UtilRefCountedPtrInlines.h>
#include <util/UtilMapping.h>


// create an RpcInterface
RpcInterface::RpcInterface(
            const String &name, 
            RefCountedPtr<SysContext> &ctx ) :
        _name(name)
{
    REFCOUNTED_CAST(iSysComponent, StdLogger, ctx->getComponent( StdLogger::getRegistryName()), _logger);
    CBLOGDBG(_logger, NTEXT("RpcInterface::RpcInterface - initialized interface: ") + name );
}


RpcInterface::~RpcInterface()
{
}


/*
    add an rpc method to the interface
*/
bool 
RpcInterface::addMethod( 
            RefCountedPtr<iRpcMethod> &method )
{
    ThdAutoCriticalSection lock( _guard );

    _methods[ method->getName() ] = method;
    CBLOGDBG(_logger, NTEXT("RpcInterface::addMethod - added ") + method->getName() );

    return true;
}


// and remove a method
bool 
RpcInterface::removeMethod( 
            const String &name )
{
    bool res = false;

    ThdAutoCriticalSection lock( _guard );

    MethodMap::iterator it = _methods.find( name );
    if ( it != _methods.end() )
    {
        _methods.erase( it );
        CBLOGDBG(_logger, NTEXT("RpcInterface::removeMethod - removed ") + name );
        res = true;
    }
    else
    {
        CBLOGDBG(_logger, NTEXT("RpcInterface::removeMethod - could not remove method: ") + name );
        res = false;
    }

    return res;    
}

/**
        dispatch an RPC call to a registered method
        @responseDoc the response document, used for creating new nodes
        @inRequest the data for the request
        @param inOutAttrs a map of attributes for the processing instruction that
                caused this interface to be called. On output these attributes
                are added to a response processing instruction
        @outResponse a node where the callee may add any result data
        @return a result code
*/
enumRpcCallResult 
RpcInterface::dispatch( 
            DOMDocument* responseDoc,
            const DOMNode* inRequest, 
            Mapping& inOutAttrs,
            DOMNode* &outResponse )
{

    enumRpcCallResult res = keError;

    String methodName;
    inOutAttrs.getIfAvailable(Rpc::METHOD_ATTR, methodName);

    RefCountedPtr<iRpcMethod> callee;
    {
        ThdAutoCriticalSection lock( _guard );
        MethodMap::iterator it = _methods.find( methodName );
        if ( it != _methods.end() )
        {
            callee = (*it).second;
        }
    }

    if ( callee != NULL )
    {
        res = callee->callMethod( responseDoc, inRequest, inOutAttrs, outResponse );
        CBLOGDBG(_logger, String(NTEXT("RpcInterface::dispatch - called: ")) + methodName );
    }
    else
    {
        CBLOGDBG(_logger, String(NTEXT("RpcInterface::dispatch - could not find method: ")) + methodName );
        res = keParamError;
    }

    return res;
}

