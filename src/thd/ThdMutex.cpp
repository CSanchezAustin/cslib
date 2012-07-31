
//      ThdMutex.cpp
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

#include <thd/ThdMutex.h>
#include <thd/ThdManager.h>

#ifdef UNIX
#include <errno.h>
#endif

#if defined(_WINDOWS) || defined(WIN32)

ThdMutex::ThdMutex( 
    String *name,
    const bool initialOwner )
{
    _mMutex = ::CreateMutex( NULL, initialOwner ? TRUE : FALSE, 
                               (name != NULL) ? name->c_str() : NULL );
}


//  open an existing mutex
ThdMutex::ThdMutex( 
    const String &name,
    const unsigned long access, 
    const bool inherit )
{
    _mMutex = ::OpenMutex( access, inherit ? TRUE : FALSE, name.c_str() );
}


ThdMutex::~ThdMutex()
{
    if (  _mMutex != NULL )
    {
        ASSERT_D( ::CloseHandle( _mMutex ) == TRUE );
    }
}


enumSyncOpResult
ThdMutex::acquire( 
    const long millis )
{
    DWORD res = ::WaitForSingleObject( _mMutex, millis );
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
ThdMutex::release()
{
    return ::ReleaseMutex( _mMutex ) == TRUE;
}


#else  // PTHREADS VERSION


ThdMutex::ThdMutex( 
    String *name,
    const bool initialOwner )
{
    ::pthread_mutexattr_init (&_attrs);

    ::pthread_mutexattr_settype (&_attrs, PTHREAD_MUTEX_RECURSIVE);
    ::pthread_mutex_init( &_mMutex, &_attrs );

    if ( initialOwner )
    {
        ::pthread_mutex_lock( &_mMutex );
    }
}


//  open an existing mutex
ThdMutex::ThdMutex( 
    const String &name,
    const unsigned long access, 
    const bool inherit )
{
    ASSERT( !NTEXT("ThdMutex::ThdMutex: inter-process threads are not yet supported") );
}


ThdMutex::~ThdMutex()
{
    ::pthread_mutexattr_destroy (&_attrs);
    ::pthread_mutex_destroy( &_mMutex );
}


enumSyncOpResult
ThdMutex::acquire( 
    const long millis )
{
    enumSyncOpResult res = SYNCOP_FAILED;

    int lockRes;
    if ( millis == WAIT_INFINITE )
    {
        // try forever
        lockRes = ::pthread_mutex_lock( &_mMutex );
        if ( lockRes == 0 )
        {
            res = SYNCOP_SUCCESS;
        }
        else
        {
            res = SYNCOP_FAILED;
        }
    }
    else
    {
#ifdef HAVE_PTHREAD_MUTEX_TIMEDLOCK
        timespec ts;
        ThdManager::toTimeSpec( millis, ts );

        lockRes = ::pthread_mutex_timedlock( &_mMutex, &ts );
        if ( lockRes == 0 )
        {
            res = SYNCOP_SUCCESS;
        }
        else if ( lockRes == ETIMEDOUT )
        {
            res = SYNCOP_TIMEOUT;
        }
        else
        {
            res = SYNCOP_FAILED;
        }
#else
        ASSERT(!NTEXT("ThdMutex::acquire timeout is not supported on this platform"));
#endif
    }

    return res;
}


bool 
ThdMutex::release()
{
    return ::pthread_mutex_unlock( &_mMutex ) == 0;
}

#endif // PTHREADS VERSION

