#ifndef ThdWaitableTimer_H_LOADED
#define ThdWaitableTimer_H_LOADED
// [
// $        ThdWaitableTimer.h
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
//         Contents: ThdWaitableTimer class
//
//         Purpose:
//
//         Usage Notes:
//
//                 
// ?
// !        Changes History
//     $Header: $
//         
// ]

#include <cslib.h>
#include <thd/ThdTypes.h>
#include <lang/ScheduleTime.h>

class EXPORT_DLL ThdWaitableTimer
{
public:

    // create an ThdWaitableTimer
    ThdWaitableTimer( 
            const String *name = NULL, 
            const bool manualReset = true );

    // open an existing one
    ThdWaitableTimer( 
            const String &name, 
            const unsigned long access, 
            const bool inherit = true );

    ~ThdWaitableTimer();

    enumSyncOpResult wait( const long millis = WAIT_INFINITE );

    /**
       set
       Set the waitable timer to execure at a given time and
       optionally an interval
       @param execute the time to execute
       @param interval if given, this is the time interval to signal
       the timer
       @result bool true if there were no problems setting the timer
     */
    bool set( 
            const ScheduleTime &execute, 
            const long interval = 0l );

    /**
       reset
       resets the timer into a non-signalled state and will
       execute when next it needs
       @return bool true if the timer was reset properly
     */
    bool reset();

    /**
       pulse
       set the timer into a signalled state immediately
       @return bool true if the timer was pulsed properly
     */
    bool pulse();

    /**
       cancel
       turns off the timer. It will no longer execute
       @return bool true if the timer was cancelled properly
     */
    bool cancel();

    /**
       getExecute
       @return  the schedule time of the timer
     */
    ScheduleTime& getExecute();

    /**
       getInterval
       @return the interval of the timer
     */
    long getInterval();

protected:

#if defined(_WINDOWS) || defined(WIN32)
    HANDLE _mSyncObj;
#else
#endif

private:

    ScheduleTime  _execute;
    long          _interval;

    // Unimplemented standard methods
    ThdWaitableTimer(const ThdWaitableTimer& );
    ThdWaitableTimer& operator=(const ThdWaitableTimer&);
};

// ThdWaitableTimer_H_LOADED
#endif
