//      SysPathMgr.cpp
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
//      Contents:	Implementation of the system context registry
//                  this registry keeps commonly used objects that
//                  are passed to other library components
//	
#include <cslib.h>

#include <xercesc/dom/DOMNode.hpp>
#include <xercesc/dom/DOMNodeList.hpp>
#include <xercesc/dom/DOMElement.hpp>
#include <xercesc/dom/DOMDocument.hpp>

#include <sys/SysPathMgr.h>
#include <sys/SysContext.h>
#include <file/FileUtils.h>
#include <file/FileAttributes.h>
#include <log/LogStdLogger.h>
#include <util/UtilMapping.h>
#include <util/UtilRefCountedPtrInlines.h>
#include <util/UtilStringUtils.h>
#include <util/UtilStringTokenizer.h>
#include <xml/XmlDomUtils.h>


const String SysPathMgr::SYS_PATH_NAME = NTEXT("sys-paths");
const String SysPathMgr::SYS_PATH_ENTRY = NTEXT("path");
const String SysPathMgr::SYS_PATH_ROOTATTR = NTEXT("root");
const String SysPathMgr::SYS_PATH_TAGATTR = NTEXT("tag");
const String SysPathMgr::SYS_PATH_IND = NTEXT("$");


DECLARE_SYS_COMPONENT(SysPathMgr)

SysPathMgr::SysPathMgr()
{
}


SysPathMgr::~SysPathMgr()
{
}


/**
   returns an full path to the file passed in name
   @param tag $ of the directory to create under
   @param name file name. This includes support for relative
   path names using $ variables. For example, passing $root/test/data/txt
   will return (if root is defined to be /dev) /dev/test/data.txt
   @return full path
*/
String 
SysPathMgr::getPath(
            const String& tag,
            const String& fileName )
{
    PathMap::iterator it = _paths.find(tag);
    if ( it != _paths.end() )
    {
        String path = (*it).second;
        path += FileUtils::PATH_SEP;
        path += fileName;

        return path;
    }

    return fileName;
}


/**
   return a full path with resolved tokens
   For example if $temp_dir/test.txt is passed and $temp_dir
   resolves to c:\temp\somefolder, the result is c:\temp\somefolder\test.txt
   @param an unresolved path
   @return a resolved full path
*/
String 
SysPathMgr::getPath(
            const String& unresolved )
{
    return resolve(unresolved);
}


/**
   adds a mnemonic tag associated with an unresolved path (which
   gets resolved) to the managers known mnemonic
   @param tag the mnemonic reference
   @param an unresolved path name that may contain additional $ variables
   @return whether or not the path was added resolved to a directory and added
   to the manager's known mnemonic
*/
bool 
SysPathMgr::add(
            const String& tag, 
            const String& unresolved)
{
    bool res = false;

    String resolved = resolve(unresolved);
    
    // if it does not exist create
    FileAttributes attrs;
    if ( !FileUtils::getAttributes( resolved, attrs ) )
    {
        if (!FileUtils::mkdirs( resolved ))
        {
            CBLOGERR(_logger, NTEXT("SysPathMgr::add: could not create directory '") + resolved + NTEXT("'"));
            return false;
        }

        // update the file attributes with another call to get info in the directory
        FileUtils::getAttributes( resolved, attrs );
    }

    if (!attrs.isDirectory())
    {
        CBLOGERR(_logger, NTEXT("SysPathMgr::add: found a file where a directory '") + resolved + NTEXT("' was expected"));
        return false;
    }

    _paths[tag] = resolved;

    return res;
}


/* 
   Interfaces from iSysComponent
*/

int 
SysPathMgr::init( 
            const DOMNode* config, 
            RefCountedPtr<SysContext>& ctx )
{
    int res = -1;

    REFCOUNTED_CAST(iSysComponent, StdLogger, ctx->getComponent( StdLogger::getRegistryName()), _logger);

	// I expect the config node to be an element node
    if ( ( config != NULL ) && ( config->getNodeType() == DOMNode::ELEMENT_NODE ) )
    {
        // set the root to be the current directory by default
        String root(NTEXT("."));

        Mapping rootAttrs;
        if ( DomUtils::getNodeValue( (const DOMElement*)config, NULL, &rootAttrs ) )
        {
            rootAttrs.getIfAvailable(SYS_PATH_ROOTATTR, root);
        }

        // resolve whatever we have in the root and then verify a file does not exist
        // in place of the intended directory
        _root = FileUtils::resolve( root );

        FileAttributes fattrs;
        if ( !FileUtils::getAttributes( _root, fattrs ) )
        {
            if (!FileUtils::mkdirs( _root ))
            {
                CBLOGERR(_logger, NTEXT("SysPathMgr::init: could not create directory '") + _root + NTEXT("'"));
                return -1;
            }

            // update the file attributes with another call to get info in the directory
            FileUtils::getAttributes( _root, fattrs );
        }

        if (!fattrs.isDirectory())
        {
            CBLOGERR(_logger, NTEXT("SysPathMgr::init: found a file where a directory '") + _root + NTEXT("' was expected"));
            return -1;
        }

        // put a special key with the root attribute in the map
    	_paths[ SYS_PATH_IND + SYS_PATH_ROOTATTR ] = _root;

        // ok to this point, no errors
        res = 0;

        // iterate through the list of child elements whose tag is SYS_PATH_ENTRY
        // and get the name attribute and value. If all is found properly, then
        // construct a counter whose value is retrieved from the config node
        DOMNodeList* children = DomUtils::getNodeList( (const DOMElement*)config, SYS_PATH_ENTRY );
        if ( children != NULL )
        {
            for ( XMLSize_t i = 0, sz = children->getLength() ;
                  i < sz; i++ )
            {
                DOMNode* child = children->item(i);
                if ( (child != NULL) && (child->getNodeType() == DOMNode::ELEMENT_NODE) )
                {
                    String value;
                    Mapping attrs;

                    bool found = DomUtils::getNodeValue( (const DOMElement*)child, &value, &attrs );
                    if ( found )
                    {
                        res = 0;

                        String tagName;
                        attrs.getIfAvailable(SYS_PATH_TAGATTR, tagName);

                        if ( tagName.length() > 0 )
                        {
                            add( tagName, value );
                        }
                    }
                }
            }
        }
    }

    return res;
}


String
SysPathMgr::resolve(
            const String& path )
{
    String buildPath;

    // if the path starts with a $ variable, then we assume that
    // it will be relative to another reference path. Accordingly,
    // we allow this function to to the $ substitution. However, this
    // section of code handles the opposite, where there is no preceeding
    // $ variable. When this is true, then we assure that the scope of the 
    // path is relative to the given root.
    if ( path.find( SYS_PATH_IND ) != 0 )
    {
        buildPath = _root;
    }
	
    StringTokenizer st(path, FileUtils::PATH_SEP_PAT);
    while(st.hasMoreTokens())
    {
        // check to see if its a template variable.
        // if so resolve it.
        String dir = st.getNextToken();
        if (dir.length() == 0)
        {
            // this is a special case where there is a / without
            // a preceeding directory name. For example, parsing
            // "/test//dir1" would return an empty dir
            continue;
        }

        // have a template variable in the path, so we have to resolve it
        if ( dir.find( SYS_PATH_IND ) == 0 )
        {
            PathMap::const_iterator it = _paths.find( dir );
            if( it == _paths.end() )
            {
                // a variable in the path can not be resolved,
                // so just return an empty string
                CBLOGERR(_logger, NTEXT("SysPathMgr::resolve: could not resolve $ vairable '") + dir + NTEXT("'"));
                return NTEXT("");
            }
            
            // copy the resolved path to the 
            dir = (*it).second;
        }

        // if no template variables were processed
        if (buildPath.length() != 0)
        {
            buildPath += FileUtils::PATH_SEP;
        }

        // finally, add our directory to the path
        buildPath += dir;
    }

    return buildPath;
}


int 
SysPathMgr::postInit( 
            const DOMNode* config, 
            RefCountedPtr<SysContext>& ctx )
{
    if ( _logger == NULL )
    {
        REFCOUNTED_CAST(iSysComponent, StdLogger, ctx->getComponent( StdLogger::getRegistryName()), _logger);
    }

    return 0;
}
