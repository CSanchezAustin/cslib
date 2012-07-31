//      coretest.cpp
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

#include <stdio.h>
#include <fstream>

#include <cslib.h>
//#include "core/Assert.h"
//#include "core/Debug.h"

void test_assert();

int main()
{
  OFSTREAM out(NTEXT("debug-log"));
  OFSTREAM err(NTEXT("err.log"));
//    Debug::setOutputStream( out );
//    Debug::setErrStream( err );

    Debug::breakpoint();

    Debug::setCurrentModule( NTEXT("CORETEST") );
    Debug::setLevel( 100 );

	DBG_LOGERR(NTEXT("DBG_LOGERR"));
	DBG_LOGWRN(NTEXT("DBG_LOGWRN"));
	DBG_LOGMSG(NTEXT("DBG_LOGMSG"));
	DBG_LOGDBG(NTEXT("DBG_LOGDBG"));
	
    if ( ISDBG(99) )
    {
        COUT << NTEXT("Runtime was 99") << std::endl;
    }
    else
    {
        COUT << NTEXT("Runtime was not 101") << std::endl;
    }

    if ( ISDBG(100) )
    {
        COUT << NTEXT("Runtime was 100") << std::endl;
    }
    else
    {
        COUT << NTEXT("Runtime was not 100") << std::endl;
    }

    if ( ISDBG(101) )
    {
        COUT << NTEXT("Runtime was 101") << std::endl;
    }
    else
    {
        COUT << NTEXT("Runtime was not 101") << std::endl;
    }

    DBGBREAK(100);

    test_assert();
    return 0;
}


void
test_assert()
{
    String test1(NTEXT("First"));
    String test2(NTEXT("Second"));

    ASSERT(test1 != test2 );
    ASSERT(test1 != NTEXT("third") );
    ASSERT(test1 == test1);
    ASSERT(test1 != test1 );
}
