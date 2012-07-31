//      xmltest.cpp
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
//      Contents:	
//	

#pragma warning(disable: 4786)

#include <fstream>

#include <dom/DOM.hpp>
#include <framework/XMLFormatter.hpp>

#include "xml/XmlDomUtils.h"
#include "xml/XmlStXmlOpener.h"
#include "xml/XmlStreamFormatTarget.h"
#include "xml/XmlStringFormatTarget.h"
#include "util/UtilMapping.h"

void test_domutil();

int main()
{
  OFSTREAM out(NTEXT("debug-log"));
  OFSTREAM err(NTEXT("err.log"));
//    Debug::setOutputStream( out );
//    Debug::setErrStream( err );

    Debug::setCurrentModule( NTEXT("XMLTEST") );

    test_domutil();
    return 0;
}

void
test_domutil()
{
    StXmlOpener st;

    DomUtils::StXMLCh domStr("this is a test of stack xml char string");

    // test the toString functions
    String convertedString = DomUtils::toString(domStr._value);

    DOMElement* elem = NULL;

    DOMDocument* doc = DomUtils::createDocument(NTEXT("people"));
    DOMDocument* copyDoc = DomUtils::createDocument(NTEXT("people"));
    DOMElement* rootElem = doc->getDocumentElement();

    String name = NTEXT("person");
    String val;
    Mapping attrs;

    // first user
    val = NTEXT("Chris Sanchez");
    attrs[ NTEXT("id") ] = NTEXT("123456");
    attrs[ NTEXT("id_number") ] = (long)123456;
    elem = DomUtils::createDomNode( doc, name, &val, &attrs );

    rootElem->appendChild( elem );
    
    // next user
    val = NTEXT("John Zhang");
    attrs[ NTEXT("id") ] = NTEXT("222222");
    attrs[ NTEXT("id_number") ] = (long)222222;
    elem = DomUtils::createDomNode( doc, name, &val, &attrs );
    rootElem->appendChild( elem );

    val = NTEXT("Harold Rimmer");
    elem = DomUtils::createDomNode( doc, name, &val );
    rootElem->appendChild( elem );

    // another user
    val = NTEXT("Bill Santiago");
    attrs[ NTEXT("id") ] = NTEXT("909090");
    attrs[ NTEXT("id_number") ] = (long)909090;
    elem = DomUtils::createDomNode( doc, name, &val, &attrs );
    rootElem->appendChild( elem );

    // test print 
    StreamFormatTarget targ(COUT);
    DomUtils::print( doc, NTEXT("UTF-8"), targ );

    // test the string format target
    StringFormatTarget stringTarg;
    DomUtils::print( doc, NTEXT("UTF-8"), stringTarg);
    COUT << std::endl << stringTarg.getString().c_str();

    // test get node value

    String strValue;
    bool res = DomUtils::getNodeValue( elem, &strValue, &attrs );

    // test set node value
    strValue = NTEXT("William Santiago");
    attrs[ NTEXT("id_id") ] = NTEXT("WWWW_22222");
    res = DomUtils::setNodeValue( elem, &strValue, &attrs );

    // look at the output again
    DomUtils::print( doc, NTEXT("UTF-8"), targ );

    DOMText* textNode = DomUtils::getTextNode( elem );

    // test findNodeByAttribute
    DOMNodeList* theNodes = rootElem->getChildNodes();
    DOMNode* foundNode = DomUtils::findNodeByAttribute(
        theNodes,
        NTEXT("id"), NTEXT("909090") );

    if ( foundNode != NULL )
    {
        // change the name again
        strValue = NTEXT("William Santiago III");
        res = DomUtils::setNodeValue( elem, &strValue );
        DomUtils::print( doc, NTEXT("UTF-8"), targ );
    }

    // test findNodesByAttribute
    std::vector< DOMNode* > vect;
    theNodes = rootElem->getChildNodes();
    size_t numNodes = DomUtils::findNodesByAttribute(
        theNodes,
        NTEXT("id"), NTEXT("909090"), vect );
    for ( std::vector< DOMNode* >::const_iterator it = vect.begin();
          it != vect.end(); it++ )
    {
        DOMNode* theNode = *it;
        DomUtils::print( theNode, NTEXT("UTF-8"), targ );
    }

    //////////////////////////////////////////////////////////////////////////////
    // test out addAttributes for element, text, and processing instruction
    DomUtils::StXMLCh attrElemStr(NTEXT("attrElemNode"));
    DOMElement* attrElem = doc->createElement( attrElemStr._value );
    DomUtils::addAttributes( attrElem, attrs );

    COUT << std::endl;
    COUT << std::endl;
    DomUtils::addProcessingInstruction( doc, String(NTEXT("attrPINode")), attrs );
    DomUtils::print( doc, NTEXT("UTF-8"), targ );
    //////////////////////////////////////////////////////////////////////////////

    // test findProcessingInstructions
    std::vector<DOMProcessingInstruction*> piNodes;
    DomUtils::findProcessingInstructions( doc, String(NTEXT("attrPINode")), piNodes );
    for ( std::vector<DOMProcessingInstruction*>::const_iterator pit = piNodes.begin();
          pit != piNodes.end(); pit++ )
    {
        DOMProcessingInstruction* theNode = *pit;
        DomUtils::print( theNode, NTEXT("UTF-8"), targ );
    }

    COUT << std::endl;

    Mapping mapElem;
    DomUtils::getAttributes( attrElem, mapElem );
    COUT << mapElem.toString().c_str() << std::endl;


    DOMNode* firstNode = NULL;
    if ( DomUtils::selectSingleNode( rootElem, name, &firstNode ) )
    {   
        DomUtils::print( firstNode, NTEXT("UTF-8"), targ );
    }

    DOMDocument* fileDoc = DomUtils::getDocument( NTEXT("xmltest.xml") );
    DomUtils::print( fileDoc, NTEXT("UTF-8"), targ );

    doc->release();
    fileDoc->release();
    copyDoc->release();
}
