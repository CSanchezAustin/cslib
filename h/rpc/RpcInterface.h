#ifndef RpcInterface_H_LOADED
#define RpcInterface_H_LOADED
// [
// $        RpcInterface.h
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
//         Contents: RpcInterface class
//
//         Purpose: Allows a call to a specific class of
//                  of RPC objects by it's interface
//
//         Usage Notes: This class is not thread safe. If you want to
//                      add/remove methods after initialization, then
//                      you must synchronize externally
//
//                 
// ?
// !        Changes History
//     $Header: $
//         
// ]

#include <map>

#include <cslib.h>
#include <rpc/RpcTypes.h>
#include <thd/ThdCriticalSection.h>
#include <util/UtilRefCountedPtr.h>

#include <xercesc/dom/DOMDocument.hpp>
#include <xercesc/dom/DOMNode.hpp>

XERCES_CPP_NAMESPACE_USE

class iRpcMethod;
class SysContext;
class StdLogger;
class Mapping;

class EXPORT_DLL RpcInterface
{
public:

    // create an RpcInterface
    RpcInterface( 
                    const String &name, 
                    RefCountedPtr<SysContext> &ctx );

    virtual ~RpcInterface();

    // add an rpc method to the interface
    bool addMethod( RefCountedPtr<iRpcMethod> &method );
    
    // and remove a method
    bool removeMethod( const String &name );

    const String& getName() { return _name; }

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
    enumRpcCallResult dispatch( 
                    DOMDocument* responseDoc,
                    const DOMNode* inRequest, 
                    Mapping& inOutAttrs, 
                    DOMNode* &outResponse );

private:

    typedef std::map< String, RefCountedPtr<iRpcMethod> > MethodMap;

    String      _name;
    MethodMap   _methods;
    RefCountedPtr<StdLogger> _logger;

    ThdCriticalSection  _guard;

private:
    // Unimplemented standard methods
    RpcInterface();
    RpcInterface(const RpcInterface& );
    RpcInterface& operator=(const RpcInterface&);
};

// RpcInterface_H_LOADED
#endif
