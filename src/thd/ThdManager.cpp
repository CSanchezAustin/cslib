//      ThdManager.cpp
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

#ifdef UNIX
#define __USE_BSD 1
#include <unistd.h>
#include <limits.h>
#endif

#include <thd/ThdManager.h>
#include <thd/ThdThread.h>
#include <pat/PatRunnable.h>
#include <util/UtilRefCountedPtrInlines.h>

// Create a new worker thread
ThdThread*
ThdManager::newWorkerThread(
            RefCountedPtr<iRunnable> &runner,
            const enumThreadPriority  priority,
            const enumThreadStartMode dwCreateFlags, 
            unsigned int    nStackSize )
{
    ThdThread* newThread = new ThdThread( runner );
    if ( newThread != NULL )
    {
        if ( !newThread->create( priority, 
                                 enumThreadStartMode(dwCreateFlags | keTSSuspended),
                                 nStackSize ) )
        {
            delete newThread;
            return NULL;
        }

	    if ( !( dwCreateFlags & keTSSuspended ) )
        {
		    newThread->releaseThread();
        }
    }

    return newThread;
}

void 
ThdManager::Sleep( const long ms )
{
#if defined(_WINDOWS) || defined(WIN32)
	::Sleep(ms);
#else
	::usleep(ms*1000);
#endif	/* _WIN32 */
}


#ifdef UNIX
void 
ThdManager::toTimeSpec( const long millis, timespec& waitTime )
{
    timeval tv;
    toTimeVal( millis, tv );

    // now convert to a timespec
    waitTime.tv_sec = tv.tv_sec;
    waitTime.tv_nsec = ( tv.tv_usec * 1000 );
}

void 
ThdManager::toTimeVal( const long millis, timeval& tv )
{
    ::gettimeofday( &tv, NULL );

    if ( millis == WAIT_INFINITE )
    {
        // infinite timeout
        tv.tv_sec  = LONG_MAX - 1;
        tv.tv_usec = 0;
    }
    else
    {
        // add the number of seconds
        tv.tv_sec += (millis / 1000);
        tv.tv_usec += millis % 1000000;

        if ( tv.tv_usec > 1000000 )
        {
            tv.tv_sec++;
            tv.tv_usec -= 1000000;
        }
    }
}

#endif    
