#ifndef SysComponentLoader_H_LOADED
#define SysComponentLoader_H_LOADED
// [
// $        SysComponentLoader.h
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
//         Contents: SysComponentLoader class
//
//         Purpose: This class builds the system context based
//                  on the XML configuration
//
//         Usage Notes: Given a list of processing instructions this loader 
//              calls dynamic constructors to create, initialize and register
//              them with a system context. The structure of the processing 
//              instruction is as follows.
//                  <? component-loader name="NAME" cfg="CFG-PATH" ?>
//                      component-loader = target name of the processing instruction
//                      name = name of the component to load as registered with
//                              SysComponentFactory class
//                      cfg = path to XML configuration data to pass to the component
//                              when it is initalized by the loader
//
//              See: ../../etc/cslib-cfg.xml for an example of the config file
//
//                 
// ?
// !        Changes History
//     $Header: $
//         
// ]


#include <cslib.h>
#include <util/UtilRefCountedPtr.h>

#include <xercesc/dom/DOMDocument.hpp>

XERCES_CPP_NAMESPACE_USE

class SysContext;

namespace EXPORT_DLL SysComponentLoader
{

    /*
        loads and configures components  
        in a SysContext
        @param doc the DOM_Document
        @return SysContext the system context where all components are loaded into
   */
    RefCountedPtr<SysContext> createComponentContext( 
                const DOMDocument* doc );

    /**
        called to update a component configuration. The document passed was the 
        original used in createComponentContext
    */
    void updateComponentContext( 
                RefCountedPtr<SysContext>& ctx, 
                const DOMDocument* doc );

    static const String TARGET_NAME = NTEXT("component-loader");
    static const String CMP_NAME_ATTR = NTEXT("name");
    static const String CMP_CFG_ATTR = NTEXT("cfg");
}


// SysComponentLoader_H_LOADED
#endif
