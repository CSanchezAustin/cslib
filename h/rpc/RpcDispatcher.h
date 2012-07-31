#ifndef RpcDispatcher_H_LOADED
#define RpcDispatcher_H_LOADED
// [
// $        RpcDispatcher.h
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
//         Contents: RpcDispatcher class
//
//         Purpose: Allows a call to a specific class of
//                  of RPC objects by it's interface
//
//         Usage Notes:
//          This class is used to dispatch calls based on the input
//          XML documents to a registered function in the system.
//          Processing instructions are used as the RPC routing mechanism
//
//          <?rpc object="O_NAME" method="M_NAME" xml="xpath"?>
//          <rpc-request>
//              ... request data ...
//          </rpc-request>
//
//          The response object is similarly structured as follows
//          <?rpc object="O_NAME" method="M_NAME" xml="xpath"?>
//          <rpc-response>
//              ... response data ...
//          </rpc-response>
//                 
// ?
// !        Changes History
//     $Header: $
//         
// ]


#include <map>

#include <cslib.h>
#include <thd/ThdCriticalSection.h>
#include <util/UtilRefCountedPtr.h>

#include <xercesc/dom/DOMDocument.hpp>

XERCES_CPP_NAMESPACE_USE

class RpcInterface;
class SysContext;
class StdLogger;

class EXPORT_DLL RpcDispatcher
{
public:
    static const String DEFAULT_TENANT;
 
public:

    // create an RpcDispatcher
    RpcDispatcher( 
            const String &name, 
            RefCountedPtr<SysContext> &ctx );

    virtual ~RpcDispatcher();

    // add an rpc interface to the dispatcher
    // @@param tenancy the entry point to a specific tenancy for the dispatcher
    bool addInterface( 
                RefCountedPtr<RpcInterface> &intf, 
                const String& tenancy = DEFAULT_TENANT );
    
    // and remove an interface
    // @@param tenancy the entry point to a specific tenancy for the dispatcher
    bool removeInterface( 
                const String &name, 
                const String& tenancy = DEFAULT_TENANT );

    const String& getName() { return _name; }

    void dispatch( 
                const DOMDocument* inRequest, 
                DOMDocument* outResponse, 
                const String& tenancy = DEFAULT_TENANT );

private:

    typedef std::map<String, RefCountedPtr<RpcInterface> > InterfaceMap;
    typedef std::map<String, InterfaceMap> TenantMap;

    String                      _name;
    TenantMap                   _tenants;
    ThdCriticalSection          _guard;
    RefCountedPtr<StdLogger>    _logger;

private:
    // Unimplemented standard methods
    RpcDispatcher(const RpcDispatcher& );
    RpcDispatcher& operator=(const RpcDispatcher&);
};

// RpcDispatcher_H_LOADED
#endif
