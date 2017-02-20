//      systest.cpp
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
//              $Revision: 2.2 $
//              $State: Exp $
//              $Author: sanchezc $
//              $Date: 2002/09/26 17:17:58 $
//
//      Contents:	
//	

#if defined(_WINDOWS) || defined(WIN32)
#pragma warning(disable: 4786)
#endif

#include <stdio.h>
#include <string.h>
#include <fstream>
#include <vector>

#include <dom/DOM.hpp>
#include <framework/XMLFormatter.hpp>
#include <sax/ErrorHandler.hpp>
#include <sax/SAXParseException.hpp>

#if defined(_WINDOWS) || defined(WIN32)
#include "sys/SysSecurityAttributes.h"
#include "sys/SysSecurityDescriptor.h"
#endif
#include "sys/SysContext.h"
#include "sys/SysCounterMgr.h"
#include "sys/SysPathMgr.h"
#include "sys/SysManager.h"
#include "sys/SysComponentLoader.h"
#include "sys/iSysComponent.h"
#include "util/UtilRefCountedPtr.h"
#include "util/UtilRefCountedPtrInlines.h"
#include "xml/XmlDomUtils.h"
#include "xml/XmlStreamFormatTarget.h"
#include "fwk/fwkBootstrap.h"


#if defined(_WINDOWS) || defined(WIN32)
void test_security();
#endif
void test_context();
void test_counter();
void test_loader();
void test_paths();
void test_version();

int main()
{
    Fwk::Initializer initter;

  OFSTREAM out(NTEXT("debug-log"));
  OFSTREAM err(NTEXT("err.log"));
//    Debug::setOutputStream( out );
//    Debug::setErrStream( err );

    Debug::setCurrentModule( NTEXT("SYSTEST") );
    
#if defined(_WINDOWS) || defined(WIN32)
    test_security();
#endif
    test_context();
    test_counter();
    test_paths();
    test_loader();

    return 0;
}

#if defined(_WINDOWS) || defined(WIN32)
void test_security()
{

	String fileName(NTEXT(".\\test.txt"));

	std::vector<String> allowed;
    std::vector<String> denied;
    denied.push_back( NTEXT("moochie\\WSLE") );

    SysSecurityAttributes attrs( false, new SysSecurityDescriptor ( allowed, denied, false ) );

    denied.clear();
    allowed.push_back( NTEXT("moochie\\Chris Sanchez") );
    SysSecurityAttributes attrs2( false, new SysSecurityDescriptor ( allowed, denied, false ) );

    HANDLE hFile = CreateFile(
                        fileName.c_str(), GENERIC_WRITE, 0,
                        attrs, OPEN_ALWAYS, 
                        FILE_ATTRIBUTE_NORMAL, 
                        NULL );
	DWORD lastErr = ::GetLastError();
    if ( hFile != INVALID_HANDLE_VALUE )
    {
        CloseHandle( hFile );
        hFile = NULL;
    }

    hFile = CreateFile(
                        fileName.c_str(), GENERIC_WRITE, 0,
                        attrs2, OPEN_ALWAYS, 
                        FILE_ATTRIBUTE_NORMAL, 
                        NULL );
	lastErr = ::GetLastError();
	if ( hFile != INVALID_HANDLE_VALUE )
    {
        CloseHandle( hFile );
        hFile = NULL;
    }
}
#endif

/********************************************************

    test the registry services

********************************************************/

class TestComponent : public iSysComponent
{
public:
    TestComponent(){}
    ~TestComponent(){}

    int init( const DOMNode* config, RefCountedPtr<SysContext> &ctx )
    {
        return 0;
    }

    /**
        used to close the system component
    */
    int close() { return 0; }

    int postInit( const DOMNode* config, RefCountedPtr<SysContext> &ctx )
    {
        return 0;
    }

    /*
        called to shutdown a component. DOMNode is the same configuration
        node that was passed to init. The component should update the any
        data that it wants persisted
    */
    int update( DOMNode* config ) { return 0; }

	const String& getComponentName() { return TC; }

public: 
    static const String TC;
};
const String TestComponent::TC = NTEXT("TC1");


class TestComponent2 : public iSysComponent
{
public:
    TestComponent2(){}
    ~TestComponent2(){}

    int init( const DOMNode* config, RefCountedPtr<SysContext> &ctx )
    {
        return 0;
    }

    /**
        used to close the system component
    */
    int close() { return 0; }

    int postInit( const DOMNode* config, RefCountedPtr<SysContext> &ctx )
    {
        return 0;
    }

    /*
        called to shutdown a component. DOMNode is the same configuration
        node that was passed to init. The component should update the any
        data that it wants persisted
    */
    int update( DOMNode* config ) { return 0; }

	const String& getComponentName() { return TC; }

public: 
    static const String TC;
};

const String TestComponent2::TC = NTEXT("TC2");


void test_context()
{
    RefCountedPtr<SysContext> ctx(new SysContext);

    DOMNode* node = NULL;
    RefCountedPtr<TestComponent> cmp( new TestComponent );
    cmp->init( node, ctx );
    ctx->addComponent( (RefCountedPtr<iSysComponent>&) cmp );

    COUT << ctx->getComponent( TestComponent::TC )->getComponentName().c_str();

    RefCountedPtr<TestComponent2> cmp2 ( new TestComponent2 );
    cmp2->init( node, ctx );
    ctx->addComponent( (RefCountedPtr<iSysComponent>&)cmp2 );

    COUT << ctx->getComponent( TestComponent2::TC )->getComponentName().c_str();

    ctx->removeComponent( TestComponent2::TC );
}   // load the xml counter document



void test_counter()
{
    RefCountedPtr<SysContext> ctx(new SysContext);
    StreamFormatTarget targ(COUT);

    DOMDocument* pDoc = DomUtils::getDocument(NTEXT("systest.xml"));
    if ( pDoc != NULL )
     {
        // construct the system counter manager
        SysCounterMgr *ctr = new SysCounterMgr();

        // load the xml counter document
        DOMElement* root = pDoc->getDocumentElement();

        // get a look before
        DomUtils::print( root, NTEXT("UTF-8"), targ );

        ctr->init( root, ctx );

        for (int i = 0; i < 10; i++ )
        {
            (*ctr)[ NTEXT("xerces") ]++;
            (*ctr)[ NTEXT("apache") ]++;
            (*ctr)[ NTEXT("caleb") ]++;
            (*ctr)[ NTEXT("csilb") ]++;
            (*ctr)[ NTEXT("newcounter") ]++;
        }

        // save it
        ctr->update( root );

        // and after
        DomUtils::print( root, NTEXT("UTF-8"), targ );

        delete ctr;
	delete pDoc;
     }
}


void test_loader()
{
    DOMDocument* pDoc = DomUtils::getDocument(NTEXT("systest-cfg.xml"));
    if ( pDoc != NULL )
    {
      StreamFormatTarget targ(COUT);
      DOMElement* root = pDoc->getDocumentElement();
      
      DomUtils::print( root, NTEXT("UTF-8"), targ );
      RefCountedPtr<SysContext> ctx = SysComponentLoader::createComponentContext( pDoc );
      
      SysComponentLoader::updateComponentContext( ctx, pDoc );
      DomUtils::print( root, NTEXT("UTF-8"), targ );
      
      delete pDoc;
    }
}


void test_paths()
{
    RefCountedPtr<SysContext> ctx(new SysContext);

    DOMDocument* pDoc = DomUtils::getDocument(NTEXT("syspath.xml"));
    if ( pDoc != NULL )
      {
        // construct the system counter manager
        SysPathMgr *paths = new SysPathMgr();
	
        // load the xml counter document
        DOMElement* root = pDoc->getDocumentElement();
	
        int res = paths->init( root, ctx );
        if ( res == 0 )
	  {
            String pathStr;
	    
            // unresolved path names
#if defined(_WINDOWS) || defined(WIN32)
            pathStr = paths->getPath( NTEXT("$root\\tempfolder"));
            pathStr = paths->getPath( NTEXT("$root\\tempfolder\\tempfilename.txt"));
            pathStr = paths->getPath( NTEXT("$cache_dir\\"));
#else
            pathStr = paths->getPath( NTEXT("$root/tempfolder"));
            pathStr = paths->getPath( NTEXT("$root/tempfolder/tempfilename.txt"));
            pathStr = paths->getPath( NTEXT("$cache_dir/"));
#endif
            // by tag
            pathStr = paths->getPath( NTEXT("$cache_dir"), NTEXT("filename.txt") );
            pathStr = paths->getPath( NTEXT("$session_dir"), NTEXT("another_filename.txt") );
	    
            paths->add( NTEXT("$from_test"), NTEXT("$cache_dir/tempfolder") );
            pathStr = paths->getPath( NTEXT("$from_test"), NTEXT("newfile.txt") );
	  }
	
        delete paths;
	delete pDoc;
      }
}

