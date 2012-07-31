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
#include <string.h>
#include <fstream>
#include <vector>

#include "cslib.h"
#include "sys/SysManager.h"


void test_date();
void test_schedule_time();


int main()
{
  OFSTREAM out(NTEXT("debug-log"));
  OFSTREAM err(NTEXT("err.log"));
//    Debug::setOutputStream( out );
//    Debug::setErrStream( err );

    Debug::setCurrentModule( NTEXT("LANGTEST") );

    test_date();
    test_schedule_time();

    return 0;
}

void test_date()
{
    Date Testing(1,31,2001); 
    Date starting(2, 20, 2000);
    Date ending(7, 14, 2001);
    Date Compare1(11, 20, 1999);
    Date Compare4(12, 3, 2002);
    
    COUT << Testing.month() << " " << Testing.day() << " " <<
        Testing.year() << " " << Testing.dayOfWeek() << " " << Testing.dayOfYear() << std::endl;
    
    Testing++;
    
    COUT << Testing.month() << " " << Testing.day() << " " <<
        Testing.year() << " " << Testing.dayOfWeek() << " " << Testing.dayOfYear() << std::endl;
    
    ASSERT(Compare1<Testing);
    ASSERT(ending>Testing);
    ASSERT(starting<Testing);
    ASSERT(Compare4>Testing);
    
    Testing = Compare4;
    
    COUT << Testing.month() << " " << Testing.day() << " " <<
        Testing.year() << " " << Testing.dayOfWeek() << " " << Testing.dayOfYear() << std::endl;
}


void test_schedule_time()
{
    ScheduleTime t1;
    ScheduleTime t2( ::time(NULL) );
    ASSERT( t1 < t2 );
    
    ScheduleTime t3 = t1 + (unsigned long)30000;
    ScheduleTime t4 = t3 - (unsigned long)30000;
    ASSERT(t1 == t4);
    long diff = t3.difference(t1);
    ASSERT(diff == 30000);

    unsigned long theTime = 30 * ( 1000 * 60 );
    ScheduleTime thirtyMinutesRelative( theTime  );
    
    time_t curTime = ::time( NULL );
    struct tm t = *(::localtime( &curTime ));
    t.tm_min += 30;
    
    ScheduleTime  thirtyMinutesAbsolute( t );
    t = thirtyMinutesRelative;
    t = thirtyMinutesAbsolute;
}
