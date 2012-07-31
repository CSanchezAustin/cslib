#ifndef ThdSemaphore_H_LOADED
#define ThdSemaphore_H_LOADED
// [
// $        ThdSemaphore.h
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
//         Contents: ThdSemaphore class
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
#include <thd/ThdCriticalSection.h>

#include <limits.h>
#ifdef UNIX
#include <semaphore.h>
#endif

class EXPORT_DLL ThdSemaphore
{
public:
    // initialize an empty semaphore
    ThdSemaphore();

    ~ThdSemaphore();

    // create a ThdSemaphore exclusively
    int create( 
            const String &name, 
            const unsigned int initial = 0, 
            const long max = LONG_MAX);

    //  open an existing semapbore
    int open( 
            const String &name);

    // increment the semaphore value by count and
    // return the previous count of the semaphore
    long increment( const long count = 1 );

    // returns an error condition, 0 = no error
    enumSyncOpResult decrement( const long millis = WAIT_INFINITE );

    // call to abandon the semaphore and release all threads waiting
    long abandon();

protected:

private:  
#if defined(_WINDOWS) || defined(WIN32)
    HANDLE _mSem;
#else
    sem_t*  _mSemPtr;
    String _mSemName;
#endif

    bool    _abandoned;
    InterLockedLong _waiters;
    ThdCriticalSection _guard;

private:
    void setAbandoned();
    bool getAbandoned();

    // Unimplemented standard methods
    ThdSemaphore( const ThdSemaphore& rhs );
    ThdSemaphore& operator=( const ThdSemaphore& rhs );
};

// ThdSemaphore_H_LOADED
#endif
