#ifndef iSysComponent_H_LOADED
#define iSysComponent_H_LOADED
// [
// $        iSysComponent.h
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
//         Contents: iSysComponent class
//
//         Purpose: a generic system component class
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
#include <util/UtilRefCountedPtr.h>

#include <xercesc/dom/DOMNode.hpp>

XERCES_CPP_NAMESPACE_USE

class SysContext;


class EXPORT_DLL iSysComponent
{
public:
    virtual ~iSysComponent(){}

    /*
        called to initialize the component. DOMNode is a configuration
        for which the sub-tree represented by getConfigKey will be fetched
        and used to configure this component. The reason that the entire tree
        is used is because there may be some shared configuration data
        that this component may use...
    */
    virtual int init( const DOMNode* config, RefCountedPtr<SysContext>& ctx ) = 0;

    /*
        Called after initialization of the system context. this allows
        system components to utilize other components as necessary
        SEE init()
    */
    virtual int postInit( const DOMNode* config, RefCountedPtr<SysContext>& ctx ) = 0;

    /**
        used to close the system component
    */
    virtual int close() = 0;

    /*
        called to shutdown a component. DOMNode is the same configuration
        node that was passed to init. The component should update the any
        data that it wants persisted
    */
    virtual int update( DOMNode* config ) = 0;

    /**
        get the name of the component
    */
	virtual const String& getComponentName() = 0;
};


// iSysComponent_H_LOADED
#endif
