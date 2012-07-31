#ifndef SysCounterMgr_H_LOADED
#define SysCounterMgr_H_LOADED
// [
// $        SysCounterMgr.h
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
//         Contents: SysCounterMgr class
//
//         Purpose: This class holds system counters
//
//         Usage Notes: 
//          This system component supports the following configuration directives
//          XML Root node: <sys-counter>
//                          Name        Type        Default                 Description
//          Config Element: counter     String      N/A                     a counter entry
//          Config Element: name        Attribute of counter                the name of the counter as  
//                                                                          referenced by this object at rumtime
//
//          Reference schema:
//			<xsd:element name="sys-counter">
//				<xsd:complexType content="elementOnly">
//				    <xsd:element name="counter" minOccurs="1" maxOccurs="unbounded">
//					    <xsd:complexType content="elementOnly">
//						    <xsd:attribute name="name" type="xsd:string"/>
//					    </xsd:complexType>
//				    </xsd:element>
//				</xsd:complexType>
//			</xsd:element>
//
//          Example:
//          <sys-counter>
//              <counter name="name">value</counter>
//              ...
//          </sys-counter>
//
// ?
// !        Changes History
//     $Header: $
//         
// ]

#include <map>

#include <cslib.h>
#include <sys/iSysComponent.h>
#include <sys/SysComponentFactory.h>


class EXPORT_DLL SysCounterMgr : public iSysComponent
{
public:

    SysCounterMgr();

    ~SysCounterMgr();

    InterLockedLong& operator[]( const String& index )
    {
        return _counters[ index ];
    }

    /* 
        Interfaces from iSysComponent
    */

    int init( 
            const DOMNode* config, 
            RefCountedPtr<SysContext>& ctx );

    /*
        Called after initialization of the system context. this allows
        system components to utilize other components as necessary
    */
    int postInit( 
            const DOMNode* config, 
            RefCountedPtr<SysContext>& ctx );

    /**
        used to close the system component
    */
    int close() { return 0; }

    /*
        called to shutdown a component. DOM_Node is the same configuration
        node that was passed to init. The component should update the any
        data that it wants persisted
    */
    int update( DOMNode* config );

	const String& getComponentName() { return SYS_CTR_NAME; }

    static const String& getRegistryName() { return SYS_CTR_NAME; }

private:
    std::map< String, InterLockedLong > _counters;

private:
    static const String SYS_CTR_NAME;
    static const String SYS_CTR_ENTRY;
    static const String SYS_CTR_NAMEATTR;

    DEFINE_SYS_COMPONENT(SysCounterMgr)
};


// SysCounterMgr_H_LOADED
#endif
