//      ThdSemaphore.cpp
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
#include <config.h>
#endif

#include <thd/ThdSemaphore.h>
#include <thd/ThdManager.h>
#include <thd/ThdAutoLock.h>


//////////// COMMON //////////////

void 
ThdSemaphore::setAbandoned()
{
    ThdAutoCriticalSection lock(_guard);
    _abandoned = true;
}


bool 
ThdSemaphore::getAbandoned()
{
    ThdAutoCriticalSection lock(_guard);
    return _abandoned;
}


long
ThdSemaphore::abandon()
{
    setAbandoned();

    long released = 0;

    while (_waiters > 0 )
    {
        increment();
        released++;
    }
    
    return released;
}


#if defined(_WINDOWS) || defined(WIN32)

ThdSemaphore::ThdSemaphore() :
    _mSem(NULL),
    _abandoned(false),
    _waiters(0L)
{
}

int
ThdSemaphore::create(
            const String &name,
            const long initial,
            const long max )
{
    _mSem = ::CreateSemaphore( 0, initial, max, name.c_str() );
    BUGBUG - fix
}

//  open an existing semaphore
int
ThdSemaphore::open( 
            const String &name,
            const unsigned long access, 
            const bool inherit)
{
    _mSem = ::OpenSemaphore( access, inherit ? TRUE : FALSE, name.c_str() );
    BUGBUG - fix
}


ThdSemaphore::~ThdSemaphore()
{
    if ( _mSem != NULL )
    {
        ASSERT_D( ::CloseHandle( _mSem ) == TRUE );
    }
}


long
ThdSemaphore::increment( 
            const long count )
{
    long lPrevCount = 0;
    if ( ::ReleaseSemaphore( _mSem, count, &lPrevCount ) == TRUE )
        return lPrevCount;
    else
        return 0;
}


enumSyncOpResult
ThdSemaphore::decrement(
            const  long millis )
{
    if ( getAbandoned())
    {
        return SYNCOP_FAILED;
    }

    _waiters++;

    DWORD res = ::WaitForSingleObject( _mSem, millis );

    _waiters--;

    if ( getAbandoned())
    {
        return SYNCOP_FAILED;
    }

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


#else // UNIX

#include <sys/stat.h>
#include <errno.h>
#include <fcntl.h>

// this constructor will attempt to open a create an unnamed semaphore
ThdSemaphore::ThdSemaphore() :
    _mSemPtr((sem_t*)SEM_FAILED),
    _abandoned(false),
    _waiters(0L)
{
}

ThdSemaphore::~ThdSemaphore()
{
    if (_mSemName.length() > 0 )
    {
        ::sem_unlink( _mSemName.c_str() );
    }
    
    if ( _mSemPtr != (sem_t*)SEM_FAILED )
    {
        ::sem_close( _mSemPtr );
    }
}


long
ThdSemaphore::increment( 
            const long count )
{
    const long lPrevCount = _waiters;
    const long numToRelease = count > lPrevCount ? lPrevCount : count;
    
    for (int i = 0; i < numToRelease; i++ )
    {
        ::sem_post( _mSemPtr );
    }

    return lPrevCount;
}

// this constructor will create a new semaphore with the option to either create it 
// exclusively or open a named semaphore if it exists already
int
ThdSemaphore::create( 
                     const String &name,
                     const unsigned int initial,
                     const long max)
{
    int iRes = 0;
    // this method is for the process to own the semaphore
    _mSemName = name;
    _mSemPtr = ::sem_open(  _mSemName.c_str(), O_CREAT|O_EXCL, 
                            S_IRWXU|S_IRWXG, initial );
    if (_mSemPtr == SEM_FAILED)
        iRes = errno;

    return iRes;
}


//  open an existing semaphore
int
ThdSemaphore::open( const String &name )

{
    int iRes = 0;
    _mSemPtr = ::sem_open( name.c_str(), O_CREAT,
			   S_IRWXU|S_IRWXG, (unsigned int)0);
    if (_mSemPtr == SEM_FAILED)
        iRes = errno;
    
    return iRes;
}


enumSyncOpResult
ThdSemaphore::decrement(
            const long millis )
{
    int res = -1;

    if ( getAbandoned())
    {
        return SYNCOP_FAILED;
    }

    _waiters++;

    if ( millis == WAIT_INFINITE )
    {
        res = ::sem_wait( _mSemPtr );
    }
    else
    {
#ifdef HAVE_SEM_TIMEDWAIT
        timespec waitTime;
        ThdManager::toTimeSpec( millis, waitTime );

        res = ::sem_timedwait( _mSemPtr, &waitTime );
#else
        ASSERT_D(!NTEXT("ThdSemaphore::decrement timeout is not supported on this platform"));
#endif
    }

    _waiters--;

    if ( getAbandoned())
    {
        return SYNCOP_FAILED;
    }

    // otherwise process the wait normally
    if (res == 0)
    {
        return SYNCOP_SUCCESS;
    }
    else if (res == ETIMEDOUT || errno == ETIMEDOUT)
    {
        return SYNCOP_TIMEOUT;
    }
    else
    {
        return SYNCOP_FAILED;
    }
}

#endif // UNIX
