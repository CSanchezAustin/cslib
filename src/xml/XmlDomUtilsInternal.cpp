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
//              $Revision: 2.0 $
//              $Author: $
//              $Date: $
//
//      Contents:	standard logger implementation
//	
//      Current Info:
//      $Header: $

#include <cslib.h>


#include <dom/DOMDocument.hpp>
#include <dom/DOMNode.hpp>
#include <dom/DOMElement.hpp>
#include <dom/DOMNamedNodeMap.hpp>
#include <framework/XMLFormatter.hpp>
#include <util/XMLUniDefs.hpp>
#include <util/XMLString.hpp>

#include <XmlDomUtilsInternal.h>


/**************************
    local const data
**************************/
static const XMLCh  gEndElement[] = { chOpenAngle, chForwardSlash, chNull };
static const XMLCh  gEndPI[] = { chQuestion, chCloseAngle, chNull};
static const XMLCh  gStartPI[] = { chOpenAngle, chQuestion, chNull };
static const XMLCh  gXMLDecl1[] =
{
        chOpenAngle, chQuestion, chLatin_x, chLatin_m, chLatin_l
    ,   chSpace, chLatin_v, chLatin_e, chLatin_r, chLatin_s, chLatin_i
    ,   chLatin_o, chLatin_n, chEqual, chDoubleQuote, chNull
};
static const XMLCh  gXMLDecl2[] =
{
        chDoubleQuote, chSpace, chLatin_e, chLatin_n, chLatin_c
    ,   chLatin_o, chLatin_d, chLatin_i, chLatin_n, chLatin_g, chEqual
    ,   chDoubleQuote, chNull
};
static const XMLCh  gXMLDecl3[] =
{
        chDoubleQuote, chSpace, chLatin_s, chLatin_t, chLatin_a
    ,   chLatin_n, chLatin_d, chLatin_a, chLatin_l, chLatin_o
    ,   chLatin_n, chLatin_e, chEqual, chDoubleQuote, chNull
};
static const XMLCh  gXMLDecl4[] =
{
        chDoubleQuote, chQuestion, chCloseAngle
    ,   chCR, chLF, chNull
};

static const XMLCh  gStartCDATA[] =
{ 
        chOpenAngle, chBang, chOpenSquare, chLatin_C, chLatin_D,
        chLatin_A, chLatin_T, chLatin_A, chOpenSquare, chNull
};

static const XMLCh  gEndCDATA[] =
{
    chCloseSquare, chCloseSquare, chCloseAngle, chNull
};
static const XMLCh  gStartComment[] =
{ 
    chOpenAngle, chBang, chDash, chDash, chNull
};

static const XMLCh  gEndComment[] =
{
    chDash, chDash, chCloseAngle, chNull
};

static const XMLCh  gStartDoctype[] =
{ 
    chOpenAngle, chBang, chLatin_D, chLatin_O, chLatin_C, chLatin_T,
    chLatin_Y, chLatin_P, chLatin_E, chSpace, chNull
};
static const XMLCh  gPublic[] =
{ 
    chLatin_P, chLatin_U, chLatin_B, chLatin_L, chLatin_I,
    chLatin_C, chSpace, chDoubleQuote, chNull
};
static const XMLCh  gSystem[] =
{ 
    chLatin_S, chLatin_Y, chLatin_S, chLatin_T, chLatin_E,
    chLatin_M, chSpace, chDoubleQuote, chNull
};
static const XMLCh  gStartEntity[] =
{ 
    chOpenAngle, chBang, chLatin_E, chLatin_N, chLatin_T, chLatin_I,
    chLatin_T, chLatin_Y, chSpace, chNull
};
static const XMLCh  gNotation[] =
{ 
    chLatin_N, chLatin_D, chLatin_A, chLatin_T, chLatin_A,
    chSpace, chDoubleQuote, chNull
};



void
DomUtilsInternal::Dom2XMLString( 
            const DOMNode* toWrite, 
            XMLFormatter& fmt )
{
    int i;
    DOMNode child;
    int attrCount;
    DOMString id;
    DOMNamedNodeMap attributes;
    DOMDocumentType doctype;


    // Get the name and value out for convenience
    DOMString   nodeName = toWrite.getNodeName();
    DOMString   nodeValue = toWrite.getNodeValue();
    unsigned long lent = nodeValue.length();

    switch (toWrite.getNodeType())
    {
        case DOMNode::ELEMENT_NODE:
            // The name has to be representable without any escapes
            fmt  << XMLFormatter::NoEscapes << chOpenAngle << nodeName;

            // Output the element start tag.

            // Output any attributes on this element
            attributes = ((DOMElement&)toWrite).getAttributes();
            attrCount = attributes.getLength();
            for (i = 0; i < attrCount; i++)
            {
                DOMNode  attribute = attributes.item(i);

                //
                //  Again the name has to be completely representable. But the
                //  attribute can have refs and requires the attribute style
                //  escaping.
                //
                fmt  << XMLFormatter::NoEscapes
                             << chSpace << attribute.getNodeName() 
                             << chEqual << chDoubleQuote
                             << XMLFormatter::AttrEscapes
                             << attribute.getNodeValue() 
                             << XMLFormatter::NoEscapes
                             << chDoubleQuote;
            }

            //
            //  Test for the presence of children, which includes both
            //  text content and nested elements.
            //
            child = toWrite.getFirstChild();
            if ( !child.isNull() )
            {
                // There are children. Close start-tag, and output children.
                // No escapes are legal here
                fmt << XMLFormatter::NoEscapes << chCloseAngle;

                while( !child.isNull() )
                {
                    DomUtilsInternal::Dom2XMLString( child, fmt );
                    child = child.getNextSibling();
                }

                //
                // Done with children.  Output the end tag.
                //
                fmt << XMLFormatter::NoEscapes << gEndElement
                            << nodeName << chCloseAngle;
            }
            else
            {
                //
                //  There were no children. Output the short form close of
                //  the element start tag, making it an empty-element tag.
                //
                fmt << XMLFormatter::NoEscapes << chForwardSlash << chCloseAngle;
            }
            break;

        case DOMNode::TEXT_NODE:
            fmt.formatBuf( nodeValue.rawBuffer(),  lent, XMLFormatter::CharEscapes );
            break;


        case DOMNode::CDATA_SECTION_NODE :
            fmt << XMLFormatter::NoEscapes << gStartCDATA
                        << nodeValue << gEndCDATA;
            break;

        case DOMNode::ENTITY_REFERENCE_NODE:
            fmt << XMLFormatter::NoEscapes << chAmpersand
                << nodeName << chSemiColon;
            break;

        case DOMNode::PROCESSING_INSTRUCTION_NODE :
            fmt << XMLFormatter::NoEscapes << gStartPI << nodeName;
            if (lent > 0)
            {
                fmt << chSpace << nodeValue;
            }
            fmt << XMLFormatter::NoEscapes << gEndPI;
            break;

        case DOMNode::COMMENT_NODE :
            fmt << XMLFormatter::NoEscapes << gStartComment
                        << nodeValue << gEndComment;
            break;

        case DOMNode::DOCUMENT_NODE :
            child = toWrite.getFirstChild();
            while( !child.isNull() )
            {
                DomUtilsInternal::Dom2XMLString( child, fmt );
                child = child.getNextSibling();
                if ( !child.isNull() )
                {
                    fmt << chCR;
                }
            }
            break;

        case DOMNode::DOCUMENT_TYPE_NODE :
            doctype = (DOMDocumentType &)toWrite;

            fmt << XMLFormatter::NoEscapes  << gStartDoctype
                        << nodeName;

            id = doctype.getPublicId();
            if (id != 0)
            {
                fmt << XMLFormatter::NoEscapes << chSpace << gPublic
                    << id << chDoubleQuote;
                id = doctype.getSystemId();
                if (id != 0)
                {
                    fmt << XMLFormatter::NoEscapes << chSpace 
                       << chDoubleQuote << id << chDoubleQuote;
                }
            }
            else
            {
                id = doctype.getSystemId();
                if (id != 0)
                {
                    fmt << XMLFormatter::NoEscapes << chSpace << gSystem
                        << id << chDoubleQuote;
                }
            }

            id = doctype.getInternalSubset(); 
            if (id !=0)
                fmt << XMLFormatter::NoEscapes << chOpenSquare
                            << id << chCloseSquare;

            fmt << XMLFormatter::NoEscapes << chCloseAngle;
            break;


        case DOMNode::ENTITY_NODE:
            fmt << XMLFormatter::NoEscapes << gStartEntity
                        << nodeName;

            id = ((DOMEntity &)toWrite).getPublicId();
            if (id != 0)
                fmt << XMLFormatter::NoEscapes << gPublic
                            << id << chDoubleQuote;

            id = ((DOMEntity &)toWrite).getSystemId();
            if (id != 0)
                fmt << XMLFormatter::NoEscapes << gSystem
                            << id << chDoubleQuote;

            id = ((DOMEntity &)toWrite).getNotationName();
            if (id != 0)
                fmt << XMLFormatter::NoEscapes << gNotation
                            << id << chDoubleQuote;

            fmt << XMLFormatter::NoEscapes << chCloseAngle << chCR << chLF;
            break;

        case DOMNode::XML_DECL_NODE:
            fmt << gXMLDecl1 << ((DOMXMLDecl &)toWrite).getVersion();

            fmt << gXMLDecl2 << fmt.getEncodingName();
            
            id = ((DOMXMLDecl &)toWrite).getStandalone();
            if ( id!= 0 )
                fmt << gXMLDecl3 << id;
            
            fmt << gXMLDecl4;
            break;
    }
}


XMLFormatter&
DomUtilsInternal::operator<< (
            XMLFormatter& strm, 
            const DOMString& s)
{
    unsigned int lent = s.length();

	if (lent <= 0)
		return strm;

    XMLCh*  buf = new XMLCh[lent + 1];
    XMLString::copyNString(buf, s.rawBuffer(), lent);
    buf[lent] = 0;
    strm << buf;
    delete [] buf;
    return strm;
}

