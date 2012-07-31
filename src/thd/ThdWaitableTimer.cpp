//      ThdWaitableTimer.cpp
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
//      Contents:	
//	
#include <cslib.h>

#include <thd/ThdWaitableTimer.h>

#if defined(_WINDOWS) || defined(WIN32)

ThdWaitableTimer::ThdWaitableTimer(
            const String *name,
            const bool manualReset)
{
    _mSyncObj = ::CreateWaitableTimer(0,
             manualReset ? TRUE : FALSE, 
             (name != 0) ? name->c_str() : 0 );
}


ThdWaitableTimer::ThdWaitableTimer( 
            const String &name, 
            const unsigned long access, 
            const bool inherit)
{
    _mSyncObj = ::OpenWaitableTimer( 
            access, 
            inherit ? TRUE : FALSE, 
            name.c_str() );
}


ThdWaitableTimer::~ThdWaitableTimer()
{
    if ( _mSyncObj != NULL )
    {
        ASSERT_D( ::CancelWaitableTimer( _mSyncObj ) == TRUE );
        ASSERT_D( ::CloseHandle( _mSyncObj ) == TRUE );
    }
}


enumSyncOpResult
ThdWaitableTimer::wait( 
            const long millis )
{
    DWORD res = ::WaitForSingleObject( _mSyncObj, millis );
    if (res == WAIT_OBJECT_0)
    {
        return SYNCOP_SUCCESS;
    }
    else if (res == WAIT_TIMEOUT)
    {
        return SYNCOP_TIMEOUT;
    }
    else
    {
        return SYNCOP_FAILED;
    }
}


bool 
ThdWaitableTimer::pulse()
{
    ScheduleTime now;
    return ::SetWaitableTimer( _mSyncObj, &now.getTime(), 0, 0, 0, FALSE ) == TRUE;
}


bool 
ThdWaitableTimer::set(
            const ScheduleTime &execute, 
            const long interval )
{
    _execute = execute;
    _interval = interval;
    return ::SetWaitableTimer( _mSyncObj, &_execute.getTime(), _interval, 0, 0, FALSE) == TRUE;
}


bool 
ThdWaitableTimer::reset()
{
    return ::SetWaitableTimer( _mSyncObj, &_execute.getTime(), _interval, 0, 0, FALSE ) == TRUE;
}


bool
ThdWaitableTimer::cancel()
{
    return ::CancelWaitableTimer( _mSyncObj ) == TRUE;
}


ScheduleTime&
ThdWaitableTimer::getExecute()
{
    return _execute;
}


long 
ThdWaitableTimer::getInterval()
{
    return _interval;
}



#else // UNIX

ThdWaitableTimer::ThdWaitableTimer(
            const String *name,
            const bool manualReset)
{
    ASSERT_D(!NTEXT("ThdWaitableTimer::ThdWaitableTimer: BUGBUG: not implemented for unix"));
}


ThdWaitableTimer::ThdWaitableTimer( 
            const String &name, 
            const unsigned long access, 
            const bool inherit)
{
    ASSERT_D(!NTEXT("ThdWaitableTimer::ThdWaitableTimer: inter-process threads are not yet supported") );
}


ThdWaitableTimer::~ThdWaitableTimer()
{
    ASSERT_D(!NTEXT("ThdWaitableTimer::~ThdWaitableTimer: BUGBUG: not implemented for unix"));
}


enumSyncOpResult
ThdWaitableTimer::wait( 
            const long millis )
{
    ASSERT_D(!NTEXT("ThdWaitableTimer::wait: BUGBUG: not implemented for unix"));
    return SYNCOP_FAILED;
}


bool 
ThdWaitableTimer::pulse()
{
    ASSERT_D(!NTEXT("ThdWaitableTimer::pulse: BUGBUG: not implemented for unix"));
    return false;
}


bool 
ThdWaitableTimer::set(
            const ScheduleTime &execute, 
            const long interval )
{
    ASSERT_D(!NTEXT("ThdWaitableTimer::set: BUGBUG: not implemented for unix"));
    return false;
}


bool 
ThdWaitableTimer::reset()
{
    ASSERT_D(!NTEXT("ThdWaitableTimer::reset: BUGBUG: not implemented for unix"));
    return false;
}


bool
ThdWaitableTimer::cancel()
{
    ASSERT_D(!NTEXT("ThdWaitableTimer::cancel: BUGBUG: not implemented for unix"));
    return false;
}


ScheduleTime&
ThdWaitableTimer::getExecute()
{
    return _execute;
}


long 
ThdWaitableTimer::getInterval()
{
    return _interval;
}


#endif // UNIX
