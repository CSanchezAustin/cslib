//      SysContext.cpp
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

#include <sys/SysCounterMgr.h>
#include <sys/SysContext.h>
#include <util/UtilMapping.h>
#include <util/UtilStringUtils.h>
#include <xml/XmlDomUtils.h>


const String SysCounterMgr::SYS_CTR_NAME = NTEXT("sys-counter");
const String SysCounterMgr::SYS_CTR_ENTRY = NTEXT("counter");
const String SysCounterMgr::SYS_CTR_NAMEATTR = NTEXT("name");

DECLARE_SYS_COMPONENT(SysCounterMgr)

SysCounterMgr::SysCounterMgr()
{
}


SysCounterMgr::~SysCounterMgr()
{
}


/* 
    Interfaces from iSysComponent
*/

int 
SysCounterMgr::init( 
            const DOMNode* config, 
            RefCountedPtr<SysContext>& ctx )
{
    int res = -1;

    // I expect the config node to be an element node
    if ( ( config != NULL ) && ( config->getNodeType() == DOMNode::ELEMENT_NODE ) )
    {
        // iterate through the list of child elements whose tag is SYS_CTR_ENTRY
        // and get the name attribute and value. If all is found properly, then
        // construct a counter whose value is retrieved from the config node
        DOMNodeList* children = DomUtils::getNodeList( (const DOMElement*)config, SYS_CTR_ENTRY );
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

                        String ctrName;
                        attrs.getIfAvailable(SYS_CTR_NAMEATTR, ctrName);

                        if ( ctrName.length() > 0 )
                        {
                            _counters[ ctrName ] = StringUtils::toLong( value );
                        }
                    }
                }
            }
        }
    }

    return res;
}


int 
SysCounterMgr::update(
            DOMNode* config )
{
    int res = -1;

    // I expect the config node to be an element node
    if ( ( config != NULL) && ( config->getNodeType() == DOMNode::ELEMENT_NODE ) )
    {
        DOMNodeList* children = DomUtils::getNodeList( (const DOMElement*)config, SYS_CTR_ENTRY );

        /*
            iterate through all the entries in the map and save them to the 
            node, updating any nodes that exist or creating new nodes for 
            those that don't exist in the config but do in the map
        */
        for( std::map< String, InterLockedLong >::const_iterator it = _counters.begin();
                it != _counters.end(); it++ )
        {
            const String &counterName = (*it).first;
            const InterLockedLong &counter = (*it).second;
        
            DOMNode* node = DomUtils::findNodeByAttribute( children, SYS_CTR_NAMEATTR, counterName );
            if ( node != NULL )
            {
                // found a node that has the desired attributes
                // now, update it with the latest counter
                if ( node->getNodeType() == DOMNode::ELEMENT_NODE )
                {
                    String counterValStr = StringUtils::toString( counter );
                    res = DomUtils::setNodeValue( (DOMElement*)node, &counterValStr ) ? 0 : -1;
                }
            }
            else
            {
                // the node does not exist, so create a new node and add to the config tree
                String counterValStr = StringUtils::toString( counter );
                Mapping attrs;
                attrs[ SYS_CTR_NAMEATTR ] = counterName;

                DOMDocument* doc = config->getOwnerDocument();
                DOMElement* newNode = DomUtils::createDomNode( 
                                                    doc,
                                                    SYS_CTR_ENTRY, 
                                                    &counterValStr, 
                                                    &attrs );
                if ( newNode != NULL )
                {
                    config->appendChild( newNode );   
                    res = 0;
                }
            }
        }
    }

    return res;
}

int 
SysCounterMgr::postInit( 
            const DOMNode* config, 
            RefCountedPtr<SysContext>& ctx )
{
    return 0;
}
