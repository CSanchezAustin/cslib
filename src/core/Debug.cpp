//      Debug.cpp
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
//              $Revision: 2.1 $
//              $Date: 2002/09/26 17:20:09 $
//              $Author: sanchezc $
//              $Date: 2002/09/26 17:20:09 $
//
//      Contents:	
//	
#include <cslib.h>

#include <core/Debug.h>
#include <util/UtilStringUtils.h>

#ifdef UNICODE
static OSTREAM* _gout = &std::wcout;
static OSTREAM* _gerr = &std::wcerr;
static ISTREAM* _gin = &std::wcin;
#else
static OSTREAM* _gout = &std::cout;
static OSTREAM* _gerr = &std::cerr;
static ISTREAM* _gin = &std::cin;
#endif

static int _gDebugLevel = 0;
static String _sCurrentModule = NTEXT("CSLIB");

void 
Debug::setOutputStream( OSTREAM &out )
{
    _gout = &out; 
}


OSTREAM& 
Debug::getOutputStream()
{
    return *_gout; 
}

void 
Debug::setInputStream( ISTREAM &in )
{
    _gin = &in; 
}


ISTREAM& 
Debug::getInputStream()
{
    return *_gin; 
}

void 
Debug::setErrStream( OSTREAM &out ) 
{
    _gerr = &out;
}


OSTREAM& 
Debug::getErrStream()
{ 
    return *_gerr; 
}


int 
Debug::getLevel()
{
    return _gDebugLevel;
}


void
Debug::setLevel(
            const int newLevel )
{
    _gDebugLevel = newLevel;
}


void 
Debug::breakpoint()
{
#if defined(_WINDOWS) || defined(WIN32)
        DebugBreak();
#else
//BUGBUG: implement to make enter the debugger at runtime
//#error "implement Debug::breakpoint"
#endif
}


const String&
Debug::getCurrentModule()
{
    return _sCurrentModule;
}

void 
Debug::setCurrentModule(
            const String& mod)
{
    _sCurrentModule = mod;
}


void
Debug::traceout(
    const String& fileName, 
    const int lineNum, 
    const LogType sev, 
    const String& msg )
{
    String fmtStr;

	switch (sev)
	{
	case ERROR_TYPE:
		fmtStr = NTEXT("ERR[");
		break;

	case WARN_TYPE:
		fmtStr = NTEXT("WRN[");
		break;

	case DEBUG_TYPE:
		fmtStr = NTEXT("DBG[");
		break;

	case MSG_TYPE:
	default: // Message
		fmtStr = NTEXT("MSG[");
		break;
	}

	// format the string as follows: "MSG[%s:%i]: %s\n"
	fmtStr += fileName;
	fmtStr += NTEXT(":");
	fmtStr += StringUtils::toString(lineNum);
	fmtStr += NTEXT("]: ");
	fmtStr += msg;
	fmtStr += NTEXT('\n');

	COUT << fmtStr.c_str() << std::flush;
}

