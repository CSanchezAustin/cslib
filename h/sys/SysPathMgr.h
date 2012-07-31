#ifndef SysPathMgr_H_LOADED
#define SysPathMgr_H_LOADED
// [
// $        SysPathMgr.h
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
//         Contents: SysPathMgr class
//
//         Purpose: This class holds system paths
//
//         Usage Notes: 
//          This system component supports the following configuration directives
//          XML Root node: <sys-paths>
//                          Name        Type        Default   Description
//          Config Element: root        Attribute   N/A       This is a path [relative or absolute] of the container
//                                                            and all elements contained in it. Default is "." meaning 
//                                                            the current directory.
//                                                            NOTE: it is legal to use either "/" or "\" as path delimiters
//          Config Element: path        String      N/A       a path name. Other mnemonics may be embedded in this path
//                                                            for example - $tmp_dir\cache will be resolved using a previously
//                                                            resolved value for $tmp_dir.
//                                                            NOTE: it is legal to use either "/" or "\" as path delimiters
//          Config Element: tag         Attribute   path      the name of the tag. This is used by clients to reference paths
//                                                            and create new paths at runtime
//
//          Reference schema:
//			<xsd:element name="sys-paths">
//				<xsd:complexType content="elementOnly">
//				    <xsd:element name="path" minOccurs="1" maxOccurs="unbounded">
//					    <xsd:complexType content="elementOnly">
//						    <xsd:attribute name="tag" type="xsd:string"/>
//					    </xsd:complexType>
//				    </xsd:element>
//				    <xsd:attribute name="root" type="xsd:string"/>
//				</xsd:complexType>
//			</xsd:element>
//
//          Example:
//            <sys-paths root="cslib">
//                <path tag="$tmp_dir">tmp</path>
//                <path tag="$cache_dir">cache</path>
//                <path tag="$session_dir">$cache_dir/sessions</path>
//                <path tag="$cfg_dir">cfg</path>
//            </sys-paths>
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

class EXPORT_DLL SysPathMgr : public iSysComponent
{
public:

    SysPathMgr();

    ~SysPathMgr();

	/**
		returns an full path to the file passed in name
		@param tag $ of the directory to create under
		@param name file name. This includes support for relative
		path names using $ variables. For example, passing $root/test/data/txt
		will return (if root is defined to be /dev) /dev/test/data.txt
		@return full path
        NOTE: it is legal to use either "/" or "\" as path delimiters
	*/
	String getPath(const String& tag, const String& fileName);

    /**
        return a full path with resolved tokens
        For example if $temp_dir/test.txt is passed and $temp_dir
        resolves to c:\temp\somefolder, the result is c:\temp\somefolder\test.txt
        @param an unresolved path
        @return a resolved full path
        NOTE: it is legal to use either "/" or "\" as path delimiters
    */
	String getPath(const String& unresolved);

	/**
    	adds a mnemonic tag associated with an unresolved path (which
        gets resolved) to the managers known mnemonic
        @param tag the mnemonic reference
        @param an unresolved path name that may contain additional $ variables
        @return whether or not the path was added resolved to a directory and added
        to the manager's known mnemonic
        NOTE: it is legal to use either "/" or "\" as path delimiters
    */
	bool add(const String& tag, const String& unresolved);


    /* 
        Interfaces from iSysComponent
    */

    int init( const DOMNode* config, RefCountedPtr<SysContext>& ctx );

    /*
        Called after initialization of the system context. this allows
        system components to utilize other components as necessary
    */
    int postInit( const DOMNode* config, RefCountedPtr<SysContext> &ctx );

    /**
        used to close the system component
    */
    int close() { return 0; }

    /*
        called to shutdown a component. DOMNode is the same configuration
        node that was passed to init. The component should update the any
        data that it wants persisted
    */
    int update( DOMNode* /* config */) { return 0; }

	const String& getComponentName() { return SYS_PATH_NAME; }

    static const String& getRegistryName() { return SYS_PATH_NAME; }

private:
    /**
        resolves tags in a path
    */
    String resolve(const String& path);

private:
    typedef std::map< String, String> PathMap;
    PathMap _paths;
    String _root;
    RefCountedPtr<StdLogger> _logger;

private:
    static const String SYS_PATH_NAME;
    static const String SYS_PATH_ENTRY;
    static const String SYS_PATH_ROOTATTR;
    static const String SYS_PATH_TAGATTR;
    static const String SYS_PATH_IND;

    DEFINE_SYS_COMPONENT(SysPathMgr)
};


// SysPathMgr_H_LOADED
#endif
