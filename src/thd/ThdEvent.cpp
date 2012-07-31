//      ThdEvent.cpp
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

#include <thd/ThdEvent.h>
#include <thd/ThdManager.h>

#ifdef UNIX
#include <errno.h>
#endif

#if defined(_WINDOWS) || defined(WIN32) // Win32 threads

ThdEvent::ThdEvent( 
            const String *name,
            const bool manualReset,
            const bool signalled  )
{
    _mEvent = ::CreateEvent( NULL, 
                    manualReset ? TRUE : FALSE, 
                    signalled ? TRUE : FALSE, 
                    (name != NULL) ? name->c_str() : NULL );
}


//  open an existing mutex
ThdEvent::ThdEvent( 
            const String &name,
            const unsigned long access, 
            const bool inherit )
{
    _mEvent = ::OpenEvent( access, inherit ? TRUE : FALSE, name.c_str() );
}


ThdEvent::~ThdEvent()
{
    if ( _mEvent != NULL )
    {
        ASSERT_D( ::CloseHandle( _mEvent ) == TRUE );
    }
}


enumSyncOpResult
ThdEvent::wait( 
            const long millis )
{
    DWORD res = ::WaitForSingleObject( _mEvent, millis );
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
ThdEvent::signal()
{
    return ::SetEvent( _mEvent ) == TRUE;
}


bool ThdEvent::reset()
{
    return ::ResetEvent( _mEvent ) == TRUE;
}

#else // PTHREADS VERSION

ThdEvent::ThdEvent( 
            const String *name,
            const bool manualReset,
            const bool signalled  )
{
    // BUGBUG: put this is shared memory so that it can be retrieved inter-process
    _manualReset = manualReset;
    _mSignalled = signalled;
    _mWaiters = 0;

    ::pthread_mutex_init( &_mMutex, NULL );
    ::pthread_cond_init( &_mCond, NULL );
}


//  open an existing mutex
ThdEvent::ThdEvent( 
            const String &name,
            const unsigned long access, 
            const bool inherit )
{
    _manualReset = false;
    _mSignalled = false;
    _mWaiters = 0;

    ASSERT_D( !NTEXT("ThdEvent::ThdEvent: inter-process threads are not yet supported") );
}


ThdEvent::~ThdEvent()
{
    // make sure no threads are waiting on the condition
    ASSERT_D( ::pthread_cond_destroy( &_mCond ) == 0 );
    ASSERT_D( ::pthread_mutex_destroy( &_mMutex ) == 0 );
}


enumSyncOpResult
ThdEvent::wait( 
            const long millis )
{
    enumSyncOpResult res = SYNCOP_FAILED;

    ::pthread_mutex_lock( &_mMutex );
    _mWaiters++;

    // make sure that the event is not already 
    // in a signalled state. If it is, then we
    // return a success code for the calling thread
    // and reset the event if appropriate
    if ( _mSignalled )
    {
        res = SYNCOP_SUCCESS;
        if ( !_manualReset )
        {
            _mSignalled = false;
        }
    }
    else
    {
        // otherwise the thread is not in a signalled
        // state. In this case we must attempt to 
        // wait on the event until it becomes signalled
        timespec waitTime;
        ThdManager::toTimeSpec( millis, waitTime );

        while ( true )
        {
            int condRes;
            if ( millis == WAIT_INFINITE )
            {
                condRes = ::pthread_cond_wait( &_mCond, &_mMutex );
            }
            else
            {
                condRes = ::pthread_cond_timedwait( &_mCond, &_mMutex, &waitTime );
            }

            if ( condRes == 0 )
            {
                // we make an additional check to protect against
                // spurrious interrupts (pp83. pthreads programming)
                if ( _mSignalled )
                {
                    res = SYNCOP_SUCCESS;
                    if ( !_manualReset )
                    {
                        _mSignalled = false;
                    }
                    break;
                }
                else
                {
                    DBG_LOGERR(NTEXT("ThdEvent::wait: got a spurrious interrupt"));
                }
            }
            else if ( condRes == ETIMEDOUT )
            {
                res = SYNCOP_TIMEOUT;
                break;
            }
            else
            {
                // some other error (EINTR)
                res = SYNCOP_FAILED;
                break;
            }
        }
    }

    _mWaiters--;
    ::pthread_mutex_unlock( &_mMutex );

    return res;
}


bool 
ThdEvent::signal()
{
    ::pthread_mutex_lock( &_mMutex );
    _mSignalled = true;
    ::pthread_mutex_unlock( &_mMutex );

    // here's the logic for signal:
    // If the event is a manual reset event, then
    // all waiting threads are released and the event
    // is set to a nonsignalled state.
    //
    // If the event is an automatic reset event, then
    // a single waiting thread is released (even if 
    // multiple waiting threads are waiting) and the
    // event is reset to a nonsignalled state.
    //
    // Note: the logic in the wait() function supports
    // this because upon return from the condition wait
    // the event has to be reset by that thread automatically
    // and, thus signal can not release all waiting threads because
    // subsequent threads that were woke up would see
    // immediately that the previous thread reset the event and
    // would go to sleep trying to prevent spurrious interupts.
    if ( _manualReset )
    {
        ::pthread_cond_broadcast( &_mCond );
    }
    else
    {
        ::pthread_cond_signal( &_mCond );
    }

    return true;
}


bool ThdEvent::reset()
{
    ::pthread_mutex_lock( &_mMutex );

    _mSignalled = false;

    ::pthread_mutex_unlock( &_mMutex );

    return true;
}

#endif // else PTHREADS VERSION

