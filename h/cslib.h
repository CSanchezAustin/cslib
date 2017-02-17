#ifndef csdef_H_LOADED
#define csdef_H_LOADED
// [
// $        csdef.h
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
//                 $Revision: 2.1 $
//                 $State: Exp $
//                 $Author: sanchezc $
//                 $Date: 2002/09/20 21:28:55 $
// ?
//         Contents: csdef class
//
//         Purpose:
//
//         Usage Notes:
//
//                 
// ?
// !        Changes History
//         
// ]
#include <string.h>
#include <memory>
#if defined(_WINDOWS) || defined(WIN32)
#pragma warning(disable:4786)
#pragma warning(disable:4503)
#endif

#ifdef USE_DLL
    #ifdef BUILD_DLL
        #if defined(_WINDOWS) || defined(WIN32)
	        #define EXPORT_DLL __declspec(dllexport)
        #else
            #error "Define EXPORT_DLL for export"
        #endif
    #else
        #if defined(_WINDOWS) || defined(WIN32)
	        #define EXPORT_DLL __declspec(dllimport)
        #else
            #error "Define EXPORT_DLL for import"
        #endif
    #endif
#else
	#define EXPORT_DLL
#endif

#if defined(_WINDOWS) || defined(WIN32)

#define WIN32_LEAN_AND_MEAN

// Windows specific
#include <Windows.h>
#include <Process.h>


/** _AtomicSwap swaps the values of x and y in an 
    atomic operation */
#define _AtomicSwap(x,y) ::InterlockedExchange((x),(y))


/** _AtomicExchangeAdd returns the value of the x 
    argument and increments the final value of x by y */
#define _AtomicExchangeAdd(x, y) ::InterlockedExchangeAdd((x),(y))


/** _AtomicIncrement increments the value of x and returns 
    the incremented value */
#define _AtomicIncrement(x) ::InterlockedIncrement((x))


/** _AtomicDecrement decrements the value of x and returns 
    the decremented value */
#define _AtomicDecrement(x) ::InterlockedDecrement((x))



#else // UNIX variants

#include <pthread.h>


extern pthread_mutex_t s_globalInterlocked;

/** _AtomicSwap swaps the values of x and y in an 
    atomic operation */
template <class T>
inline T _AtomicSwap(T* x, const T& y)
{
    T z;

    ::pthread_mutex_lock(&s_globalInterlocked);
    z = *x;
    *x = y;
    ::pthread_mutex_unlock(&s_globalInterlocked);

    return z;
}


/** _AtomicExchangeAdd returns the value of the x 
    argument and increments the final value of x by y */
template <class T>
inline T _AtomicExchangeAdd(T* x, const T& y)
{
    T z;

    ::pthread_mutex_lock(&s_globalInterlocked);
    z = *x;
    *x += y;
    ::pthread_mutex_unlock(&s_globalInterlocked);

    return z;
}


/** _AtomicIncrement increments the value of x and returns 
    the incremented value */
template <class T>
inline T _AtomicIncrement(T* x)
{
    T z;

    ::pthread_mutex_lock(&s_globalInterlocked);
    (*x)++;
    z = *x;
    ::pthread_mutex_unlock(&s_globalInterlocked);

    return z;
}


/** _AtomicDecrement decrements the value of x and returns 
    the decremented value */
template <class T>
inline T _AtomicDecrement(T* x)
{
    T z;

    ::pthread_mutex_lock(&s_globalInterlocked);
    (*x)--;
    z = *x;
    ::pthread_mutex_unlock(&s_globalInterlocked);

    return z;
}

#endif // UNIX variants


// other generic defines after platform processing
#ifndef NULL
    #define NULL 0
#endif

#ifdef UNICODE
#define OSTREAM std::wostream
#define ISTREAM std::wistream
#define OFSTREAM std::wofstream
#define IFSTREAM std::wifstream
#define OSTRSTREAM std::wostrstream
#define ISTRSTREAM std::wistrstream
#define OSTRINGSTREAM std::wostringstream
#define ISTRINGSTREAM std::wistringstream
#define FSTREAM std::wfstream;
#define CHAR_T	wchar_t
#define GETCH	::_getwch
#define STRERROR ::_wcserror
#define GETENV	::_wgetenv
#define SPRINTF	::wsprintf
#define SSCANF	::wsscanf
#define ATOI	::wtoi
#define ATOL	::wtol
#define ATOF	::wtof
#define STRCHR	::wcschr
#define STRFTIME ::wcsftime
#else
#define OSTREAM std::ostream
#define ISTREAM std::istream
#define OFSTREAM std::ofstream
#define IFSTREAM std::ifstream
#define OSTRSTREAM std::ostrstream
#define ISTRSTREAM std::istrstream
#define FSTREAM std::fstream;
#define OSTRINGSTREAM std::ostringstream
#define ISTRINGSTREAM std::istringstream
#define CHAR_T	char
#define GETCH	::getch
#define STRERROR ::strerror
#define GETENV	::getenv
#define SPRINTF	::sprintf
#define SSCANF	::sscanf
#define ATOI	::atoi
#define ATOL	::atol
#define ATOF	::atof
#define STRCHR	::strchr
#define STRFTIME ::strftime
#endif

// core cslib structures
#include "lang/String.h"
#include "lang/Date.h"
#include "lang/ScheduleTime.h"
#include "lang/InterLockedLong.h"

#include "core/Debug.h"
#include "core/Assert.h"

extern InterLockedLong _gUnnamedInstCtr;

// csdef_H_LOADED
#endif
