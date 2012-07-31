#ifndef XmlDomUtils_H_LOADED
#define XmlDomUtils_H_LOADED
// [
// $        XmlDomUtils.h
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
//         Contents: DomUtils class
//
//         Purpose:
//
//         Usage Notes:
//
//                 
// ?
// !        Changes History
//     $Header: $
// ]

#include <cslib.h>

#include <vector>
#include <xercesc/parsers/AbstractDOMParser.hpp>
#include <xercesc/framework/XMLFormatter.hpp>
#include <xercesc/dom/DOMNode.hpp>
#include <xercesc/dom/DOMElement.hpp>
#include <xercesc/dom/DOMText.hpp>
#include <xercesc/dom/DOMDocument.hpp>
#include <xercesc/dom/DOMNodeList.hpp>
#include <xercesc/dom/DOMProcessingInstruction.hpp>

XERCES_CPP_NAMESPACE_USE

class Mapping;

class DOMString;

namespace EXPORT_DLL DomUtils
{
    struct StXMLCh
    {
        StXMLCh();
        explicit StXMLCh(const String& value);
        explicit StXMLCh(const char* value);
        ~StXMLCh();

        XMLCh* _value;

        private: 
            StXMLCh& operator=(const StXMLCh& value);
            StXMLCh(const StXMLCh& value);
    };

    /**
       Convert a XMLCh* to a String
       @param domstr the dom string
       @return a regular string
    */
    String toString(
        const XMLCh *domstr);

    /**
       Convert a String to an XMLCh*
       @param String
       @return aXMLCh which the caller is responsible for disposing
    */
    XMLCh* toString(
        const String& str);

    /**
       Add a mapping of attributes to a DOMElement
       @param elem the element to add the attributes to
       @param attrs the attributes to add
    */
    void addAttributes(
		DOMNode* node, 
        const Mapping& attrs );

    /**
       get a mapping of attributes from a DOMElement
       @param elem the node to get the attributes from
       @param attrs the attributes
    */
    bool getAttributes(
        const DOMNode* node, 
        Mapping& attrs );

    /**
       create and return a text node for a document given
       a name and set of attributes
       @param doc a dom document
       @param nodeName the name of the name
       @param nodeValue the text node's value
       @param Mapping a mapping of attributes
       @return a text node
    */
    DOMElement* createDomNode(
        DOMDocument* doc, 
        const String& nodeName, 
        const String* nodeValue = NULL, 
        Mapping* attrs = NULL );


    /**
       get the text value of a node and optionally return 
       the attributes of the node
       @param node a dom node
       @param outValue the value of the node
       @param attrs optional mapping to put the attributes into
       @return a boolean indicating success of the operation
    */
    bool getNodeValue( 
        const DOMElement* node, 
        String* outValue = NULL, 
        Mapping* attrs = NULL );


    /**
       set the text value and attributes of a node
       @param node a dom node
       @param outValue the value of the node
       @param attrs optional mapping to put the attributes into
       @return a boolean indicating success of the operation
    */
    bool setNodeValue( 
        DOMElement* node, 
        String* inValue = NULL, 
        Mapping* attrs = NULL );


    /**
       get the first text node of the given element
       @param elem the element
       @return the node reference
    */
    DOMText* getTextNode( const DOMElement* node );


    /**
       print out the entire tree to the given format target
       @param the node to start and write out to
       @param targ a XMLFormatTarget to output the data to
    */
    void print( 
        const DOMNode* toWrite, 
        const String& encoding, 
        XMLFormatTarget& targ );

    /**
        Return a list of nodes with a given tag name
        @param node the node which to search
        @param tag the name which to look for nodes for in the input node
        @return the list of nodes matching the tag.
    */
    DOMNodeList* getNodeList( 
        const DOMElement* node, 
        const String& tag );
    
    /**
       This function will return a node who has attributes
       matching the input parameters
       @param nodes a list of nodes to search
       @param matchAttrStr the name of the attribute to search for
       @param matchAttrVal the value of the attribute
       @return a DOMAttr
    */
    DOMNode* findNodeByAttribute(
        const DOMNodeList* nodes,
        const String& matchAttrStr,
        const String& matchAttrVal );

    /**
       This function will return a list of nodes who has attributes
       matching the input parameters
       @param nodes a list of nodes to search
       @param matchAttrStr the name of the attribute to search for
       @param matchAttrVal the value of the attribute
       @param nodeList the list of nodes with the matching attribute
       @return number of matching nodes
    */
    size_t findNodesByAttribute(
        DOMNodeList* nodes,
        const String &matchAttrStr,
        const String &matchAttrVal,
        std::vector< DOMNode* >& nodeList );

    /**
       this function will search a DOM node for all 
       processing instructions whose target matches
       the input parameters
       @param node the node to search
       @param target to look for
       @outNodes the list of processing instruction nodes
    */
    void findProcessingInstructions(
        const DOMNode* node,
        const String& target,
        std::vector<DOMProcessingInstruction*>& outNodes );

    /**
       this function will create a new processing instruction
       in the input document
       @param doc the DOM document
       @param target the name of the target
       @attrs the mapping of attributes
    */
    bool addProcessingInstruction(
        DOMDocument* doc,
        const String& target,
        const Mapping& attrs );

    /**
       this function will create a new processing instruction
       in the input document
       @param doc the DOM document
       @param target the name of the target
       @attrs the mapping of attributes
    */
    bool addProcessingInstruction(
        DOMDocument* doc,
        const XMLCh* target,
        const Mapping& attrs );


    /**
       returns a single node in the out parameter
       @param root the node to search
       @param tag the sub element to look for
       @outNode the response
       @return true of the node was found
    */
    bool selectSingleNode(
        const DOMNode* root,
        const String& tag,
        DOMNode** outNode );


    /**
       returns a newly constructed document from the given path
       @param path the path to the document to load
    */
    DOMDocument* getDocument(
        const String& path,
        const AbstractDOMParser::ValSchemes newScheme = AbstractDOMParser::Val_Auto, 
        const bool namespaces = false );

    /**
       convert a string to a DOMDocument
    */
    DOMDocument* toDocument(
        const String& data,
        const AbstractDOMParser::ValSchemes newScheme = AbstractDOMParser::Val_Auto, 
        const bool namespaces = false );

    /**
       Create a new empty document with root name as the root element tag
    */
    DOMDocument* createDocument(
        const String& rootName );

}

// DomUtils_H_LOADED
#endif
