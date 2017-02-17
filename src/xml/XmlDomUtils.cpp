//      XmlDomUtils.cpp
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
//      Contents:	DOM utilities
//	
//      Current Info:
//      $Header:$

#include <cslib.h>

#include <xercesc/dom/DOMDocument.hpp>
#include <xercesc/dom/DOMNode.hpp>
#include <xercesc/dom/DOMNodeIterator.hpp>
#include <xercesc/dom/DOMElement.hpp>
#include <xercesc/dom/DOMText.hpp>
#include <xercesc/dom/DOMAttr.hpp>
#include <xercesc/dom/DOMCharacterData.hpp>
#include <xercesc/dom/DOMNamedNodeMap.hpp>
#include <xercesc/dom/DOMProcessingInstruction.hpp>
#include <xercesc/dom/DOMException.hpp>
#include <xercesc/dom/DOMImplementation.hpp>
#include <xercesc/dom/DOMImplementationRegistry.hpp>
#include <xercesc/dom/DOMLSSerializer.hpp>
#include <xercesc/dom/DOMLSOutput.hpp>

#include <xercesc/framework/XMLFormatter.hpp>
#include <xercesc/framework/MemBufInputSource.hpp>
#include <xercesc/framework/LocalFileInputSource.hpp>
#include <xercesc/util/XMLString.hpp>

#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/sax/HandlerBase.hpp>

#include <xml/XmlDomUtils.h>
#include <util/UtilMapping.h>
#include <util/UtilStringUtils.h>
#include <util/UtilStringTokenizer.h>


DomUtils::StXMLCh::StXMLCh() :
    _value(NULL)
{
}


DomUtils::StXMLCh::StXMLCh(
            const String& value) :
    _value(NULL)

{
    _value = DomUtils::toString( value );
}


DomUtils::StXMLCh::StXMLCh(
            const char* value) :
    _value(NULL)

{
    _value = XMLString::transcode( value );
}


DomUtils::StXMLCh::~StXMLCh()
{
    if ( _value != NULL )
    {
	    XMLString::release( &_value );
        _value = NULL;
    }
}


/**
    Convert a DOMString to a String
    @param domstr the dom string
    @return a regular string
*/
String 
DomUtils::toString(
            const XMLCh *domstr)
{
	// BUGBUG TODO: need to review if this is really necessary. XMLCh* is typedef'd to wchar_t when unicode is defined and char* when it is not defined.
    String result;

	char *name = XMLString::transcode(domstr);
    if ( name != NULL )
    {
        result = StringUtils::toString( name );
		XMLString::release(&name);
    }

    return result;
}

/**
    Convert a String to an XMLCh*
    @param String
    @return aXMLCh which the caller is responsible for disposing
*/

XMLCh* 
DomUtils::toString(
        const String& domstr)
{
	return XMLString::transcode( StringUtils::toCString(domstr).get() );
}

/**
    Add a mapping of attributes to a DOMElement
    @param elem the element to add the attributes to
    @param attrs the attributes to add
*/
void 
DomUtils::addAttributes(
            DOMNode* node, 
            const Mapping& attrs )
{
    if ( node != NULL )
    {
        ASSERT_D( node->getNodeType() == DOMNode::PROCESSING_INSTRUCTION_NODE || 
                  node->getNodeType() == DOMNode::ELEMENT_NODE );

        if ( node->getNodeType() == DOMNode::PROCESSING_INSTRUCTION_NODE )
        {
            DOMProcessingInstruction* pi = (DOMProcessingInstruction*)node;

			StXMLCh buffer( attrs.toString() );
			pi->setData( buffer._value );
        }
        else
        {
            DOMElement* elem = (DOMElement*)node;

            for ( Mapping::const_iterator it = attrs.begin();
                    it != attrs.end(); it++ )
            {
                StXMLCh name( (*it).first );
                if ( (*it).second._type == MappingRef::keLong )
                {
                    StXMLCh value( StringUtils::toString( (long)(*it).second ) );
                    elem->setAttribute( name._value, value._value );
                }
                else if ( (*it).second._type == MappingRef::keString )
                {
                    StXMLCh value( String((*it).second) );
                    elem->setAttribute( name._value, value._value );
                }
                else if ( (*it).second._type == MappingRef::keBoolean )
                {
                    StXMLCh value( StringUtils::toString( (bool)(*it).second ) );
                    elem->setAttribute( name._value, value._value );
                }
            }
        }
    }
}


/**
    get a mapping of attributes from a DOMElement
    @param elem the node to get the attributes from
    @param attrs the attributes
*/
bool 
DomUtils::getAttributes(
            const DOMNode* node, 
            Mapping& attrs )
{
    bool result = false;

    if( node != NULL )
    {
        if ( node->getNodeType() == DOMNode::PROCESSING_INSTRUCTION_NODE )
        {
            DOMProcessingInstruction* pi = (DOMProcessingInstruction*)node;
            Mapping newMap( DomUtils::toString( pi->getData() ) );
            attrs.merge( newMap );
            result = true;
        }
        else
        {
            DOMNamedNodeMap* domAttrs = node->getAttributes();
            if ( domAttrs != NULL )
            {
                for ( XMLSize_t i = 0, sz = domAttrs->getLength() ;
                        i < sz; i++ )
                {
                    DOMNode* item = domAttrs->item(i);
                    if ( (item != NULL) && ( item->getNodeType() == DOMNode::ATTRIBUTE_NODE ) )
                    {
                        DOMAttr* attr = (DOMAttr*)item;
                        String name = DomUtils::toString( attr->getName() );
                        String value = DomUtils::toString( attr->getValue() );
                        attrs[ name ] = value;
                        result = true;
                    }                
                }
            }
        }
    }

    return result;
}


/**
    create and return a text node for a document given
    a name and set of attributes
    @param doc a dom document
    @param nodeName the name of the name
    @param nodeValue the text node's value
    @param Mapping a mapping of attributes
    @return a text node
*/
DOMElement*
DomUtils::createDomNode(
            DOMDocument* doc, 
            const String& nodeName, 
            const String* nodeValue, 
            Mapping* attrs )
{
    ASSERT_D( nodeName.size() > 0 );

    // first create the node
    DOMElement* result = NULL;

    if ( doc != NULL )
    {
        StXMLCh stNodeName( nodeName );
        result = doc->createElement( stNodeName._value );
		if ( result != NULL )
		{
			// then the value of the node if it is not empty
			if ( nodeValue != NULL )
			{
                StXMLCh stNodeValue( *nodeValue );
				DOMText* textNode = doc->createTextNode( stNodeValue._value );
				result->appendChild( textNode );
			}

			// if there are attributes, then create them too
			if ( attrs != NULL )
			{
				DomUtils::addAttributes( result, *attrs );
			}
		}
    }

    return result;
}


/**
    get the text value of a node and optionally return 
    the attributes of the node
    @param node a dom node
    @param outValue the value of the node
    @param attrs optional mapping to put the attributes into
    @return a boolean indicating success of the operation
*/
bool 
DomUtils::getNodeValue( 
            const DOMElement* node, 
            String* outValue, 
            Mapping* attrs )
{
    ASSERT_D( ( outValue != NULL ) || ( attrs != NULL ) );
    bool result = false;

    if ( node != NULL )
    {
        // process the value of the node first
        if ( outValue != NULL )
        {
            DOMText* textNode = DomUtils::getTextNode( node );
            if ( textNode != NULL )
            {
                *outValue = DomUtils::toString( textNode->getData() );
                result = true;
            }
        }

        // then process the attributes
        if ( attrs != NULL )
        {
            result |= DomUtils::getAttributes( node, *attrs );
        }
    }


    return result;
}


/**
    set the text value and attributes of a node
    @param node a dom node
    @param outValue the value of the node
    @param attrs optional mapping to put the attributes into
    @return a boolean indicating success of the operation
*/
bool
DomUtils::setNodeValue( 
            DOMElement* node, 
            String* inValue, 
            Mapping* attrs)
{
    bool result = false;

    if ( node != NULL )
    {
        // then the value of the node if it is not empty
        if ( inValue != NULL )
        {
            DOMText* textNode = DomUtils::getTextNode( node );
            StXMLCh stInValue( *inValue );
            if ( textNode != NULL )
            {
                textNode->setData( stInValue._value );
                result = true;
            }
            else
            {
                textNode = node->getOwnerDocument()->createTextNode( stInValue._value );
                node->appendChild( textNode );
                result = true;
            }
        }

        // if there are attributes, then create them too
        if ( attrs != NULL )
        {
            DomUtils::addAttributes( node, *attrs );
            result = true;
        }
    }

    return result;
}


/**
    get the first text node of the given element
    @param elem the element
    @return the node reference
*/
DOMText*
DomUtils::getTextNode( 
            const DOMElement* node )
{
    DOMText* result = NULL;

    if ( node != NULL )
    {
        bool done = false;
        DOMNodeList* children = node->getChildNodes();
        for ( XMLSize_t i = 0, sz = children->getLength() ;
                i < sz && !done; i++ )
        {
            DOMNode* child = children->item(i);
            if ( child != NULL )
            {
                // for this function we are only interested
                // in the attributes and value of the given node
                switch ( child->getNodeType() )
                {
                    case DOMNode::TEXT_NODE:
                        result = (DOMText*)child;
                        done = true;
                        break;
		    default:
		      break;
                }
            }
        }
    }

    return result;
}



    /**
        print out the entire tree to the given format target
        @param the node to start and write out to
        @param targ a XMLFormatTarget to output the data to
    */

void
DomUtils::print( 
            const DOMNode* toWrite, 
            const String& encoding, 
            XMLFormatTarget& targ )
{
    if ( toWrite != NULL )
    {
    //    static const XMLCh gLS[] = { chLatin_L, chLatin_S, chNull };
	    StXMLCh features("XML 1.0 Traversal 2.0");

        DOMImplementation *impl = 
             DOMImplementationRegistry::getDOMImplementation(features._value);

        DOMLSSerializer* mySerializer = ((DOMImplementationLS*)impl)->createLSSerializer();
		DOMLSOutput *theOutputDesc = ((DOMImplementationLS*)impl)->createLSOutput();
        if (( mySerializer != NULL ) && (theOutputDesc != NULL) )
        {
			DOMConfiguration *domCfg = mySerializer->getDomConfig();

            // set user specified output encoding
            StXMLCh stEncoding( encoding );
            theOutputDesc->setEncoding( stEncoding._value );

            if (domCfg->canSetParameter(XMLUni::fgDOMWRTFormatPrettyPrint, true))
            {
                domCfg->setParameter(XMLUni::fgDOMWRTFormatPrettyPrint, true);
            }

            // set the byte-order-mark feature      
//            if (domCfg->canSetParameter(XMLUni::fgDOMWRTBOM, true))
//            {
//                domCfg->setParameter(XMLUni::fgDOMWRTBOM, true);
//            }

			theOutputDesc->setByteStream( &targ );
            mySerializer->write( toWrite,  theOutputDesc );

            mySerializer->release();
            theOutputDesc->release();
        }
    }
}


DOMNodeList*
DomUtils::getNodeList( 
            const DOMElement* node, 
            const String& tag )
{
    DOMNodeList* nodeList = NULL;

    if ( node != NULL )
    {
        StXMLCh tagString(tag);
        nodeList = node->getElementsByTagName( tagString._value );
    }

    return nodeList;
}

/**
    This function will return a node who has attributes
    matching the input parameters
    @param nodes a list of nodes to search
    @param matchAttrStr the name of the attribute to search for
    @param matchAttrVal the value of the attribute
    @return a DOMNode 
*/
DOMNode*
DomUtils::findNodeByAttribute(
            const DOMNodeList* nodes,
            const String& matchAttrStr,
            const String& matchAttrVal )
{
    DOMNode* result = NULL;
    bool done = false;

    if ( ( nodes != NULL ) && ( nodes->getLength() > 0 ) )
    {
        StXMLCh matchAttName( matchAttrStr );
        StXMLCh matchAttVal( matchAttrVal );

        for ( XMLSize_t i = 0, sz = nodes->getLength() ;
                i < sz && !done; i++ )
        {
            DOMNode* child = nodes->item(i);
            if ( child->getNodeType() == DOMNode::ELEMENT_NODE )
            {
                const XMLCh* attrVal = ((DOMElement*)child)->getAttribute( matchAttName._value );
                if ( XMLString::equals( matchAttVal._value, attrVal ) )
                {
                    result = child;
                    done = true;
                }
            }
        }
    }


    return result;
}

    /**
        This function will return a list of nodes who has attributes
        matching the input parameters
        @param nodes a list of nodes to search
        @param matchAttrStr the name of the attribute to search for
        @param matchAttrVal the value of the attribute
		@param nodeList the list of nodes with the matching attribute
        @return number of matching nodes
    */
size_t 
DomUtils::findNodesByAttribute(
			DOMNodeList* nodes,
			const String &matchAttrStr,
            const String &matchAttrVal,
			std::vector< DOMNode* >& nodeList )
{
	// Clear the old list
    if ( ( nodes != NULL ) && ( nodes->getLength() > 0 ) )
    {
	    nodeList.clear();

        StXMLCh matchAttName( matchAttrStr );
        StXMLCh matchAttVal( matchAttrVal );

        for ( XMLSize_t i = 0, sz = nodes->getLength() ;
                i < sz; i++ )
        {
            DOMNode* child = nodes->item(i);
            if ( child->getNodeType() == DOMNode::ELEMENT_NODE )
            {
                const XMLCh* attrVal = ((DOMElement*)child)->getAttribute( matchAttName._value );
                if ( XMLString::equals( matchAttVal._value, attrVal ) )
                {
                    nodeList.push_back(child);
                }
            }
        }

        return nodeList.size();
    }

    return 0;
}

    /**
        this function will search a DOM node for all 
        processing instructions whose target matches
        the input parameters
        @param node the node to search
        @param target to look for
        @outNodes the list of processing instruction nodes
    */
void
DomUtils::findProcessingInstructions(
            const DOMNode* node, 
            const String& target,
            std::vector<DOMProcessingInstruction*>& outNodes )
{
    if ( node != NULL )
    {
        StXMLCh domTarget( target );
        DOMNodeList* children = node->getChildNodes();
        if ( children != NULL )
        {
            for ( XMLSize_t i = 0, sz = children->getLength() ;
                    i < sz; i++ )
            {
                DOMNode* child = children->item(i);
                if ( child != NULL )
                {
                    // for this function we are only interested
                    // in the attributes and value of the given node
                    switch ( child->getNodeType() )
                    {
                        case DOMNode::PROCESSING_INSTRUCTION_NODE:
                            if ( XMLString::equals( domTarget._value, 
                                                    ((DOMProcessingInstruction*)child)->getTarget() ) )
                            {
                                outNodes.push_back( (DOMProcessingInstruction*)child );
                            }
		        default:
			  break;
                    }
                }
            }
        }
    }
}



/**
    this function will create a new processing instruction
    in the input document
    @param doc the DOM document
    @param target the name of the target
    @attrs the mapping of attributes
*/
bool 
DomUtils::addProcessingInstruction(
            DOMDocument* doc,
            const String& target,
            const Mapping& attrs )
{
    bool result = false;

    if ( doc != NULL )
    {
        StXMLCh name( target );
        StXMLCh value( attrs.toString() );
        DOMProcessingInstruction* newPI = doc->createProcessingInstruction( name._value, value._value );
        if ( newPI!= NULL )
        {
            DOMElement* elem = doc->getDocumentElement();
            if ( elem != NULL )
            {
                doc->insertBefore( newPI, elem );
            }
            else
            {
                doc->appendChild( newPI );
            }
			result = true;
        }
    }
    
    return result;
}


/**
    this function will create a new processing instruction
    in the input document
    @param doc the DOM document
    @param target the name of the target
    @attrs the mapping of attributes
*/
bool 
DomUtils::addProcessingInstruction(
            DOMDocument* doc,
            const XMLCh* target,
            const Mapping& attrs )
{
    bool result = false;

    if ( doc!= NULL )
    {
        StXMLCh value( attrs.toString() );
        DOMProcessingInstruction* newPI = doc->createProcessingInstruction( target, value._value );
        if ( newPI != NULL )
        {
            DOMElement* elem = doc->getDocumentElement();
            if ( elem != NULL )
            {
                doc->insertBefore( newPI, elem );
            }
            else
            {
                doc->appendChild( newPI );
            }
			result = true;
        }
    }
    
    return result;
}


    /**
        returns a single node in the out parameter
        @param root the node to search
        @param tag the sub element to look for
        @outNode the response
        @return true of the node was found
    */
bool 
DomUtils::selectSingleNode(
            const DOMNode* root,
            const String& tag,
            DOMNode** outNode )
{
    bool res = false;

   if ( ( root != NULL ) && ( root->getNodeType() == DOMNode::ELEMENT_NODE ) )
    {
        String localTag = tag;

        DOMElement* elem = (DOMElement*)root;

        // in support of relative and absolute paths in the tag name
        if ( localTag.compare( 0, SLASHSLASH.length(), SLASHSLASH ) == 0 )
        {
            elem = elem->getOwnerDocument()->getDocumentElement();
            StringUtils::trimFront( SLASHSLASH, localTag );
        }

        StringTokenizer tok( localTag, SLASH );
        while ( tok.hasMoreTokens() )
        {
            StXMLCh stLocal( tok.getNextToken() );
            DOMNodeList* nodes = elem->getElementsByTagName( stLocal._value );
            if ( nodes->getLength() > 0 )
            {
                *outNode = nodes->item( 0 );
                res = true;
            }
            else
            {
                res = false;
                break;
            }
        }
    }

    return res;
}

DOMDocument*
DomUtils::getDocument(
                const String& path,
                const AbstractDOMParser::ValSchemes newScheme, 
                const bool namespaces)
{
    DOMDocument* res = NULL;

    // prepare a DOM document
    try
    {
        XercesDOMParser* parser = new XercesDOMParser();
        ErrorHandler* errHandler = (ErrorHandler*) new HandlerBase();
        
        parser->setValidationScheme( newScheme );
        parser->setDoNamespaces( namespaces );
        parser->setErrorHandler ( errHandler );
        
        StXMLCh fileSource( path );
        parser->parse( fileSource._value );
        res = parser->adoptDocument();

	delete parser;
        delete errHandler;
    }
    catch(const SAXParseException& toCatch)
    {
        CERR << NTEXT("SAX Error during parsing: ")
             << path.c_str() << std::endl
             << NTEXT("Exception message is: ")
             << toCatch.getMessage() << std::endl;
    }
    catch (const XMLException& toCatch)
    {
        CERR << NTEXT("XMLException Error during parsing: ")
             << path.c_str() << std::endl
             << NTEXT("Exception message is: ")
             << toCatch.getMessage() << std::endl;
    }
    catch (const DOMException& toCatch)
    {
        CERR << NTEXT("DOMException Error during parsing: ")
             << path.c_str() << std::endl
             << NTEXT("Exception message is: ")
             << toCatch.msg << std::endl;
    }
    catch (...)
    {
        CERR << NTEXT("Unexpected exception during parsing: ") 
			 << path.c_str() << std::endl;
    }

    return res;
}


/**
	Convert a string to a document
*/
DOMDocument*
DomUtils::toDocument(
			const String& data,
			const AbstractDOMParser::ValSchemes newScheme, 
			const bool namespaces )
{
	DOMDocument* res = NULL;

	try
	{
        XercesDOMParser* parser = new XercesDOMParser();
        ErrorHandler* errHandler = (ErrorHandler*) new HandlerBase();
        
        parser->setValidationScheme( newScheme );
        parser->setDoNamespaces( namespaces );
        parser->setErrorHandler ( errHandler );
        
		std::unique_ptr<char> bytes( StringUtils::toCString(data) );
		size_t len = ::strlen(bytes.get());
		MemBufInputSource memBuf( (XMLByte*)bytes.get(), len, NTEXT("dom-message"));
        parser->parse( memBuf );
        res = parser->adoptDocument();

        delete parser;
        delete errHandler;
	}
    catch(const SAXParseException& toCatch)
    {
        CERR << NTEXT("Error during parsing: ")
             << data.c_str() << std::endl
             << NTEXT("Exception message is: ")
             << toCatch.getMessage() << std::endl;
    }
    catch (const XMLException& toCatch)
    {
        CERR << NTEXT("Error during parsing: ")
             << data.c_str() << std::endl
             << NTEXT("Exception message is: ")
             << toCatch.getMessage() << std::endl;
    }
    catch (const DOMException& toCatch)
    {
        CERR << NTEXT("Error during parsing: ")
             << data.c_str() << std::endl
             << NTEXT("Exception message is: ")
             << toCatch.msg << std::endl;
    }
    catch (...)
    {
        CERR << NTEXT("Unexpected exception during parsing: ") 
			 << data.c_str() << std::endl;
    }

	return res;
}


DOMDocument*
DomUtils::createDocument(
            const String& rootName )
{
    StXMLCh features("XML 1.0 Traversal 2.0");
    StXMLCh name(rootName);

    DOMImplementation* impl = DOMImplementationRegistry::getDOMImplementation( features._value );
    DOMDocument* doc = impl->createDocument(
            NULL,                  // root element namespace URI.
            name._value,   // root element name
            NULL );// document type object (DTD).
    return doc;
}

