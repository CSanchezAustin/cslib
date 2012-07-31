#ifndef SerSerializationMgr_H_LOADED
#define SerSerializationMgr_H_LOADED
// [
// $        SerSerializationMgr.h
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
//         Contents: SerSerializationMgr class
//
//         Purpose: This class holds system serialization manager
//
//         Usage Notes: 
//          This system component supports the following configuration directives
//          XML Root node: <ser-mgr>
//                          Name        Type        Default                 Description
//          Config Element: 
//          Config Element: 
//
//          Reference schema:
//			<xsd:element name="ser-mgr">
//			</xsd:element>
//
//          Example:
//          <ser-mgr>
//          </ser-mgr>
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
#include <util/UtilRefCountedPtr.h>


class StdLogger;
class SysPathMgr;
class iSerializable;
class iSerOutputSource;
class iSerInputSource;

class EXPORT_DLL SerSerializationMgr : public iSysComponent
{
public:

    /**
       The following enums are returned by
       serialize, deserialize and copy
     */
    enum 
    {
        SER_NOERR = 0,
        SER_UNKNOWN = -1,       // unknown error
        SER_BAD_STREAM = -2,    // the stream is bad or invalid
        SER_WRITE_ERR = -3,     // write stream error
        SER_READ_ERR = -4,      // read stream error
        SER_INVALID_FORMAT = -5 // invalid stream format
    };

public:

    SerSerializationMgr();

    ~SerSerializationMgr();


    /**
       serialize
       write out theWorld object to the given output source
     */
    int serialize( iSerializable& theWorld, iSerOutputSource& theSource );


    /**
       deserialize
       read in the  theWorld object from the given input source
     */
    int deserialize( iSerializable& theWorld, iSerInputSource& theSource );


    /**
       copy
       copy one serializable world to another
     */
    int copy( iSerializable& theInputWorld, iSerializable& theOutputWorld );
    

    /**
        Interfaces from iSysComponent
    */

    int init( 
            const DOMNode* config, 
            RefCountedPtr<SysContext>& ctx ) { return 0; }


    /**
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

    /**
        called to shutdown a component. DOM_Node is the same configuration
        node that was passed to init. The component should update the any
        data that it wants persisted
    */
    int update( DOMNode* config ) { return 0; }

    const String& getComponentName() { return SER_MGR_NAME; }
    
    static const String& getRegistryName() { return SER_MGR_NAME; }

    static size_t getMgrMem();

private:
    RefCountedPtr<StdLogger>    _logger;
    RefCountedPtr<SysPathMgr>   _paths;

private:
    static const String SER_MGR_NAME;
    static int SER_FORMAT_SENTINEL;
    static int SER_FORMAT_CURVER;
    static int SER_FORMAT_MINVER;
    static int SER_FORMAT_MAXVER;
    
    DEFINE_SYS_COMPONENT(SerSerializationMgr)
};


// SerSerializationMgr_H_LOADED
#endif
