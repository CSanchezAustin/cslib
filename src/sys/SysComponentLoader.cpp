//      SysComponentLoader.cpp
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

#include <vector>
#include <map>

#include <xercesc/dom/DOMDocument.hpp>

#include <sys/SysComponentLoader.h>
#include <sys/SysComponentFactory.h>
#include <sys/SysContext.h>
#include <sys/iSysComponent.h>
#include <util/UtilMapping.h>
#include <util/UtilRefCountedPtrInlines.h>
#include <xml/XmlDomUtils.h>


/*
    loads and configures components  
    in a SysContext
    @param doc the DOMDocument
    @return SysContext the system context where all components are loaded into
*/
RefCountedPtr<SysContext>
SysComponentLoader::createComponentContext( 
            const DOMDocument* doc )
{
    RefCountedPtr<SysContext> resContext(new SysContext);

    std::vector<DOMProcessingInstruction*> pis;
    DomUtils::findProcessingInstructions( doc, TARGET_NAME, pis );

    // map of components to their configuration
    std::map< RefCountedPtr<iSysComponent>, DOMElement* > luckyComponents;

    for ( std::vector<DOMProcessingInstruction*>::const_iterator it = pis.begin();
            it != pis.end(); it++ )
    {
        // check to see if the processing instruction has the 
        // following attributes:
        // "component-name" - name of the class to create
        // "component-config" - path of the config tree for the component
        // 
        Mapping attrs;
        if ( DomUtils::getAttributes( (*it), attrs ) )
        {
            String cmpName;
            attrs.getIfAvailable(CMP_NAME_ATTR, cmpName);

            RefCountedPtr<iSysComponent> cmp( SysComponentFactory::createObject( cmpName ) );
            if ( cmp != NULL )
            {
                // locate it's configuration and call it to initialize
                DOMElement* xmlNode = NULL;

                String xmlIsland;
                attrs.getIfAvailable(CMP_CFG_ATTR, xmlIsland);

                if ( xmlIsland.length() > 0 )
                {
                    const DOMNode* theNode = (const DOMNode*)doc->getDocumentElement();
                    DomUtils::selectSingleNode( theNode, xmlIsland, (DOMNode**)&xmlNode );
                }

                int initRes = cmp->init( xmlNode, resContext );
                if ( initRes == 0 )
                {
                    resContext->addComponent( cmp );
                    luckyComponents[ cmp ] = xmlNode;
                }
            }
        }
    }

    // now post initialize the components that were successfully initialized
    for ( std::map< RefCountedPtr<iSysComponent>, DOMElement* >::iterator lit = luckyComponents.begin();
            lit != luckyComponents.end(); lit++ )
    {
        (*lit).first->postInit( (*lit).second, resContext );
    }

    return resContext;
}

void 
SysComponentLoader::updateComponentContext( 
            RefCountedPtr<SysContext> &ctx, 
            const DOMDocument* doc )
{

    std::vector<DOMProcessingInstruction*> pis;
    DomUtils::findProcessingInstructions( doc, TARGET_NAME, pis );

    for ( std::vector<DOMProcessingInstruction*>::const_iterator it = pis.begin();
            it != pis.end(); it++ )
    {
        // check to see if the processing instruction has the 
        // following attributes:
        // "component-name" - name of the class to create
        // "component-config" - path of the config tree for the component
        // 
        Mapping attrs;
        if ( DomUtils::getAttributes( (*it), attrs ) )
        {
            String cmpName;
            attrs.getIfAvailable(CMP_NAME_ATTR, cmpName);

            RefCountedPtr<iSysComponent> cmp = ctx->getComponent( cmpName );
            if ( cmp != NULL )
            {
                // locate it's configuration and call it to initialize    
                String xmlIsland;
                attrs.getIfAvailable(CMP_CFG_ATTR, xmlIsland);

                if ( xmlIsland.length() > 0 )
                {
                    const DOMNode* theNode = (const DOMNode*)doc->getDocumentElement();

                    DOMElement* xmlNode = NULL;
                    DomUtils::selectSingleNode( theNode, xmlIsland, (DOMNode**)&xmlNode );

                    cmp->update( xmlNode );
                }
            }
        }
    }
}

