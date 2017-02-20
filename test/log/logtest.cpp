//      logtest.cpp
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

#if defined(_WINDOWS) || defined(WIN32)
#pragma warning(disable: 4786)
#endif

#include <fstream>

#include <dom/DOM.hpp>

#include "fwk/fwkBootstrap.h"
#include "log/LogStdFormatter.h"
#include "log/LogStdLogger.h"
#include "sys/SysManager.h"
#include "sys/SysContext.h"
#include "xml/XmlDomUtils.h"
#include "xml/XmlStXmlOpener.h"
#include "xml/XmlStreamFormatTarget.h"
#include "util/UtilRefCountedPtrInlines.h"

void test_formatter();
void test_logger();

int main()
{
    Fwk::Initializer initter;
  OFSTREAM out(NTEXT("debug-log"));
  OFSTREAM err(NTEXT("err.log"));
//    Debug::setOutputStream( out );
//    Debug::setErrStream( err );
    Debug::setCurrentModule( NTEXT("LOGTEST") );

    test_formatter();
    test_logger();

    return 0;
}

void test_formatter()
{
    StdFormatter fmt(NTEXT("$0$ $1$: $2$: $3$:\t$4$\n"));
    COUT << fmt.format( NTEXT("format test 1") );

    StdFormatter fmt2(NTEXT("($0$ $1$): $2$: $3$:\t$4$\n"));
    COUT << fmt2.format( NTEXT("format test 2") );

    StdFormatter fmt3(NTEXT("  ($0$ $1$): $2$: $3$:\t$4$\n"));
    COUT << fmt3.format( NTEXT("format test 3") );
}


void test_logger()
{
    RefCountedPtr<SysContext> ctx(new SysContext);
    StreamFormatTarget targ(COUT);

    DOMDocument* doc = DomUtils::getDocument( NTEXT("logtest.xml") ); // BUGBUG review for document leak
    if ( doc != NULL )
    {
        // construct the system counter manager
        StdLogger *logger = new StdLogger();

        // load the xml counter document
        DOMElement* root = doc->getDocumentElement();

        logger->init( root, ctx );
        logger->postInit( root, ctx );
		doc->release();

        // should always fail
        fopen("testing", "r");

        logger->logMessage(101, StdLogger::LOGC_ERR );
        logger->logMessage(102, StdLogger::LOGC_ERR );
        logger->logMessage(103, StdLogger::LOGC_ERR );
        logger->logMessage(103, StdLogger::LOGC_ERR );
        logger->logMessage(NTEXT("config-test notshown debug"), StdLogger::LOGC_DEBUG );
        logger->logMessage(NTEXT("config-test shown error"), StdLogger::LOGC_ERR );
        logger->logMessage(NTEXT("config-test shown warning"), StdLogger::LOGC_WARN );
        logger->logMessage(NTEXT("config-test shown message"), StdLogger::LOGC_MSG );

        // macro test
        CBLOGDBG(logger, NTEXT("config-test notshown debug macro"));
        CBLOGERR(logger, NTEXT("config-test shown error macro"));
        CBLOGWRN(logger, NTEXT("config-test shown warning macro"));
        CBLOGMSG(logger, NTEXT("config-test shown message macro"));

        logger->setLogLevel(StdLogger::LOGC_DEBUG);

        CBLOGDBG(logger, NTEXT("config-test shown debug macro"));
        CBLOGERR(logger, NTEXT("config-test notshown error macro"));
        CBLOGWRN(logger, NTEXT("config-test notshown warning macro"));
        CBLOGMSG(logger, NTEXT("config-test notshown message macro"));

        // BUGBUG - when message catalog is implemented test
        // the logger's interface to that

        delete logger;
        logger = NULL;

        CBLOGDBG(logger, NTEXT("global: config-test shown in debug macro"));
        CBLOGERR(logger, NTEXT("global: config-test notshown error macro"));
        CBLOGWRN(logger, NTEXT("global: config-test notshown warning macro"));
        CBLOGMSG(logger, NTEXT("global: config-test notshown message macro"));
    }
    else
    {
        CERR << NTEXT("Could not parse: 'logtest.xml'") << std::endl;
    }
}


