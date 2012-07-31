//      RpcDispatcher.cpp
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
//      RCS:
//                 $Revision:$
//                 $Author:$
//                 $Date:$
//
//      Contents:	This is the interface class that contains one
//                  or more RPC methods. 
//	
#include <cslib.h>

#include <vector>

#include <xercesc/dom/DOMProcessingInstruction.hpp>
#include <xercesc/dom/DOMDocument.hpp>
#include <xercesc/dom/DOMNode.hpp>

#include <rpc/RpcDispatcher.h>
#include <rpc/RpcInterface.h>
#include <rpc/RpcTypes.h>
#include <log/LogStdLogger.h>
#include <sys/SysContext.h>
#include <thd/ThdAutoLock.h>
#include <util/UtilMapping.h>
#include <util/UtilStringUtils.h>
#include <util/UtilRefCountedPtrInlines.h>
#include <xml/XmlDomUtils.h>

const String RpcDispatcher::DEFAULT_TENANT = NTEXT("--default--");


// create an RpcDispatcher
RpcDispatcher::RpcDispatcher(
            const String &name, 
            RefCountedPtr<SysContext> &ctx ) :
        _name(name)
{
    REFCOUNTED_CAST(iSysComponent, StdLogger, ctx->getComponent( StdLogger::getRegistryName()), _logger);
    CBLOGDBG(_logger, NTEXT("RpcDispatcher::RpcDispatcher - initialized interface: ") + name );
}


RpcDispatcher::~RpcDispatcher()
{
}


/*
    add an rpc method to the interface
*/
bool 
RpcDispatcher::addInterface( 
            RefCountedPtr<RpcInterface> &intf,
            const String& tenancy )
{
    ThdAutoCriticalSection lock( _guard );

    _tenants[ tenancy ][ intf->getName() ] = intf;
    CBLOGDBG(_logger, NTEXT("RpcDispatcher::addInterface - added ") + intf->getName() + NTEXT(" to tenancy:") + tenancy );
    return true;
}


// and remove a method
bool 
RpcDispatcher::removeInterface( 
            const String &name,
            const String& tenancy )
{
    bool res = false;

    ThdAutoCriticalSection lock( _guard );

    TenantMap::iterator tenantIt = _tenants.find( tenancy );
    if ( tenantIt == _tenants.end() )
    {
        // use the default tenancy
        tenantIt = _tenants.find( DEFAULT_TENANT );
    }

    InterfaceMap::iterator intfIt = (*tenantIt).second.find( name );
    if ( intfIt != (*tenantIt).second.end() )
    {
        (*tenantIt).second.erase( intfIt );
        CBLOGDBG(_logger, NTEXT("RpcDispatcher::removeInterface - removed ") + 
                 name + NTEXT(" from tenancy:") + tenancy );
        res = true;
    }
    else
    {
        CBLOGERR(_logger, NTEXT("RpcDispatcher::removeInterface - could not remove interface: ") + 
                 name + NTEXT(" from tenancy:") + tenancy );
        res = false;
    }

    return res;    
}


/**
    Tis function is responsible for taking in an XML document
    and parsing out processing instructions to determine where 
    to route the XML message.
*/
void 
RpcDispatcher::dispatch( 
            const DOMDocument* inRequest, 
            DOMDocument* outResponse,
            const String& tenancy )
{
    if ( inRequest == NULL || outResponse == NULL || outResponse->getDocumentElement() == NULL )
    {
        CBLOGERR(_logger, NTEXT("RpcDispatcher::dispatch - bad document passed") );
    }

    // find all processing instructions in the request
    std::vector<DOMProcessingInstruction*> pis;
    DomUtils::findProcessingInstructions( (const DOMNode*)inRequest, Rpc::TARGET_PI, pis );
    if ( pis.size() == 0 )
    {
        Mapping resultAttrs;
        resultAttrs[ Rpc::RESULT_ATTR ] = StringUtils::toString((int)keParamError);
        DomUtils::addProcessingInstruction( outResponse, Rpc::TARGET_PI, resultAttrs );
        return;
    }

    // process all instructions - 
    // get the object name, method name, and xml configuration
    // from the node attributes and make the RPC call
    for ( std::vector<DOMProcessingInstruction*>::const_iterator it = pis.begin();
            it != pis.end(); it++ )
    {
        const DOMProcessingInstruction* pi = (*it);

        Mapping attrs;
        Mapping resultAttrs;
        bool hasAttrs = DomUtils::getAttributes( pi, attrs );
        if ( hasAttrs )
        {
            // parameter validation (object name and method are required)
            String object;
            attrs.getIfAvailable(Rpc::OBJECT_ATTR, object);

            if ( object.size() == 0 )
            {
                // have an improperly formatted node - just create a response
                // dom node with an error condition
                // copy the processing instruction, adding the result of the rpc call

                resultAttrs[ Rpc::RESULT_ATTR ] = StringUtils::toString((int)keNoRpcObjectParam);
                DomUtils::addProcessingInstruction( outResponse, Rpc::TARGET_PI, resultAttrs );
                continue;
            }

            // the XML node is optional
            DOMNode* xmlNode = NULL;

            String xmlIsland;
            attrs.getIfAvailable(Rpc::XML_ATTR, xmlIsland);

            // an XML data fragment name has been specified, pull out the fragment
            // so that it can be passed to the RPC call later
            if ( xmlIsland.length() > 0 )
            {
                const DOMNode* theNode = (const DOMNode*)inRequest->getDocumentElement();
                DomUtils::selectSingleNode( theNode, xmlIsland, &xmlNode );
            }

            // find the correct entry to call
            RefCountedPtr<RpcInterface> callee;
            {
                ThdAutoCriticalSection lock( _guard );

                TenantMap::iterator tenantIt = _tenants.find( tenancy );
                if ( tenantIt == _tenants.end() )
                {
                    // use the default tenancy
                    tenantIt = _tenants.find( DEFAULT_TENANT );
                }

                // finally dispatch the request to the tenant
                if ( tenantIt != _tenants.end() )
                {
                    InterfaceMap::iterator intfIt = (*tenantIt).second.find( object );
                    if ( intfIt != (*tenantIt).second.end() )
                    {
                        callee = (*intfIt).second;
                    }
                }
            }

            // make the call and process the results, then depending on whether or not a response
            // is required, (i.e. a function returning void) add the output processing instruction
            // to the result document
            if ( callee != NULL )
            {
                DOMNode* outNode = NULL;
                enumRpcCallResult res = callee->dispatch( outResponse, xmlNode, attrs, outNode );

                // output is required, so we check and see if the node was allocated by
                // the callee. If so, then we add it to the element root and add an 
                // attribute in the processing instruction
                // we also return the RPC error code
                if ( ( res == keOk ) && ( outNode != NULL ) )
                {
                    if ( outResponse != outNode->getOwnerDocument() )
                    {
                        // the out node has been associated with another document. Import the node
                        // to the outResponse node
                        outNode = outResponse->importNode( outNode, true );
                    }

                    outResponse->getDocumentElement()->appendChild( outNode );

                    resultAttrs[ Rpc::XML_ATTR ] = DomUtils::toString( outNode->getNodeName() );
                }

                resultAttrs[ Rpc::RESULT_ATTR ] = StringUtils::toString((int)res);
                DomUtils::addProcessingInstruction( outResponse, Rpc::TARGET_PI, resultAttrs );
            }
            else
            {
                resultAttrs[ Rpc::RESULT_ATTR ] = StringUtils::toString((int)keNoRpcObject);
                DomUtils::addProcessingInstruction( outResponse, Rpc::TARGET_PI, resultAttrs );

                CBLOGERR(_logger, String(NTEXT("RpcDispatcher::dispatch - could not find object: ") + object) );
            }
        }
        else
        {
            resultAttrs[ Rpc::RESULT_ATTR ] = StringUtils::toString((int)keParamError);
            DomUtils::addProcessingInstruction( outResponse, Rpc::TARGET_PI, resultAttrs );

            CBLOGERR(_logger, NTEXT("RpcDispatcher::dispatch - could not find attributes to make an rpc call") );
        }
    }
}
