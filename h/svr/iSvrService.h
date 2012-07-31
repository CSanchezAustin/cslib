#ifndef iSvrService_H_LOADED
#define iSvrService_H_LOADED
// [
// $        iSvrService.h
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
// Copyright (c) 2001 Chris Sanchez. All rights reserved.
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
//                 $Revision:$
//                 $Author:$
//                 $Date:$
// ?
//         Contents: iSvrService class
//
//         Purpose:
//
//         Usage Notes:
//
//                 
// ?
// !        Changes History
//     $Header:$
//         
// ]

#include <cslib.h>
#include <util/UtilRefCountedPtr.h>

#include <xercesc/dom/DOMNode.hpp>

XERCES_CPP_NAMESPACE_USE

class Mapping;
class SysContext;

class EXPORT_DLL iSvrService
{
public:
    virtual ~iSvrService() {}

    /**
        Initialize the server
        @param params is a Mapping of parameters passed to the server
        @param config the server's configuration file
        @param ctx a SysContext registry object
        @return a user defined result
    */
    virtual int init(
            const Mapping &params,
            const DOMNode* config,
            RefCountedPtr<SysContext> &ctx ) = 0;

    /**
        Called to start a server application
        @return a user defined result

    */
    virtual int start() = 0;

    /**
        Called to stop the server application
        @return a user defined result

    */
    virtual int stop() = 0;

    /**
        returns whether or not the service has stopped
    */
    virtual bool isStopped() = 0;

    /**
     *  return the name of the server
     *  @return the name of the server
     */
    virtual const String& getName() = 0;

    /**
     *  return the display name of the server
     *  @return the name of the server
     */
    virtual const String& getDisplayName() = 0;

    /**
     *  return the name of the config tag element for the server
     *  @return the name of the tag
     */
    virtual const String& getConfigTag() = 0;
    
    /**
     * return the name of the default config file
     */
    virtual const String& getConfigFileName() =0;

};

// iSvrService_H_LOADED
#endif


