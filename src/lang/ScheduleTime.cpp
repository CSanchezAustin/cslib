//      ScheduleTime.cpp
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

#include <lang/ScheduleTime.h>

#if defined(_WINDOWS) || defined(WIN32)
const unsigned long ScheduleTime::kMilliIntPerSecond = 10000;
#endif
/*******************************************************************
    Common functions
*******************************************************************/
ScheduleTime::ScheduleTime( 
            const ScheduleTime &rhs )
{
    *this = rhs;
}

ScheduleTime::ScheduleTime( 
            const unsigned long millis )
{
    _isAbsolute = false;
    _millis = millis;

    break_time();
}


ScheduleTime::~ScheduleTime()
{
}


ScheduleTime&
ScheduleTime::operator=( 
            const ScheduleTime &rhs )
{
    if ( &rhs != this )
    {
        _isAbsolute = rhs._isAbsolute;
        _time = rhs._time;
        _millis = rhs._millis;
    }

    return *this;
}


ScheduleTime::operator struct tm() const
{
    struct tm tmTime;

    toTM( _time, tmTime );

    return tmTime;
}


/*******************************************************************
    Windows functions
*******************************************************************/

#if defined(_WINDOWS) || defined(WIN32)

/**
    Initialize a schedule time with the current system time (UTC)
*/
ScheduleTime::ScheduleTime()
{
    FILETIME utcTime;
    ::GetSystemTimeAsFileTime( &utcTime );

    _time.LowPart = utcTime.dwLowDateTime;
    _time.HighPart = utcTime.dwHighDateTime;

    _isAbsolute = true;
    _millis = 0;
    break_time();
}


ScheduleTime::ScheduleTime( 
            const FILETIME &ftTime )
{
    _time.LowPart = ftTime.dwLowDateTime;
    _time.HighPart = ftTime.dwHighDateTime;

    _isAbsolute = true;
    _millis = 0;
    break_time();
}


ScheduleTime::ScheduleTime( 
            const SYSTEMTIME &sysTime )
{
    FILETIME ftTime;
    ::SystemTimeToFileTime( &sysTime, &ftTime );

    _time.LowPart = ftTime.dwLowDateTime;
    _time.HighPart = ftTime.dwHighDateTime;

    _isAbsolute = true;
    _millis = 0;
    break_time();
}


ScheduleTime::ScheduleTime( 
            const LARGE_INTEGER &liTime,
            const bool isUTC )
{
    if ( isUTC )
    {
        _time = liTime;
    }
    else
    {
        toUTCTime( liTime, _time );
    }

    _isAbsolute = true;
    _millis = 0;
    break_time();
}


ScheduleTime::ScheduleTime( 
            const struct tm &tmTime )
{
    SYSTEMTIME sysTime;
    sysTime.wYear = tmTime.tm_year + 1900;
    sysTime.wMonth = tmTime.tm_mon + 1;
    sysTime.wDayOfWeek = tmTime.tm_wday;
    sysTime.wDay = tmTime.tm_mday;
    sysTime.wHour = tmTime.tm_hour;
    sysTime.wMinute = tmTime.tm_min;
    sysTime.wSecond = tmTime.tm_sec;
    sysTime.wMilliseconds = 0;

    FILETIME ftTime;
    ::SystemTimeToFileTime( &sysTime, &ftTime );

    _time.LowPart = ftTime.dwLowDateTime;
    _time.HighPart = ftTime.dwHighDateTime;

    _isAbsolute = true;
    _millis = 0;
    break_time();
}


void 
ScheduleTime::break_time()
{
    if ( !_isAbsolute )
    {
        FILETIME utcTime;
        ::GetSystemTimeAsFileTime( &utcTime );

        LARGE_INTEGER relativeTime;
        relativeTime.LowPart = utcTime.dwLowDateTime;
        relativeTime.HighPart = utcTime.dwHighDateTime;

        LONGLONG i = Int32x32To64( _millis, kMilliIntPerSecond );
        _time.QuadPart = relativeTime.QuadPart + i;
    }
}


ScheduleTime::operator SYSTEMTIME() const
{
    SYSTEMTIME sysTime;

    toSYSTEMTIME( _time, sysTime );

    return sysTime;
}


void 
ScheduleTime::toSYSTEMTIME( 
            const LARGE_INTEGER &li, 
            SYSTEMTIME &sysTime )
{
    FILETIME ftTime;
    ftTime.dwLowDateTime = li.LowPart;
    ftTime.dwHighDateTime = li.HighPart;

    ::FileTimeToSystemTime( &ftTime, &sysTime );
}


ScheduleTime::operator FILETIME() const
{
    FILETIME ftTime;

    toFILETIME( _time, ftTime );

    return ftTime;
}


void 
ScheduleTime::toFILETIME( 
            const LARGE_INTEGER &li,
            FILETIME &ftTime )
{
    ftTime.dwLowDateTime = li.LowPart;
    ftTime.dwHighDateTime = li.HighPart;
}


void 
ScheduleTime::toTM( 
            const LARGE_INTEGER &li, 
            struct tm &tmTime )
{
    FILETIME ftTime;
    ftTime.dwLowDateTime = li.LowPart ;
    ftTime.dwHighDateTime = li.HighPart;

    SYSTEMTIME sysTime;
    ::FileTimeToSystemTime( &ftTime, &sysTime );

    tmTime.tm_year= sysTime.wYear - 1900;
    tmTime.tm_mon  = sysTime.wMonth - 1;
    tmTime.tm_wday = sysTime.wDayOfWeek;
    tmTime.tm_mday = sysTime.wDay;
    tmTime.tm_hour = sysTime.wHour;
    tmTime.tm_min = sysTime.wMinute;
    tmTime.tm_sec = sysTime.wSecond;
}


int 
ScheduleTime::compare( 
            const ScheduleTime &compareTo ) const
{
    FILETIME me = (FILETIME)(*this);
    FILETIME him = (FILETIME)compareTo;
    return ::CompareFileTime( &me, &him );
}


int 
ScheduleTime::compare( 
            const LARGE_INTEGER &compareTo ) const
{
    ScheduleTime tempTime( compareTo );
    FILETIME him = (FILETIME)tempTime;
    FILETIME me = (FILETIME)(*this);
    return ::CompareFileTime( &me, &him );
}


int 
ScheduleTime::compare( 
            const struct tm &compareTo ) const
{
    ScheduleTime tempTime( compareTo );
    FILETIME him = (FILETIME)tempTime;
    FILETIME me = (FILETIME)(*this);
    return ::CompareFileTime( &me, &him );
}


int 
ScheduleTime::compare( 
            const FILETIME &compareTo ) const
{
    ScheduleTime tempTime( compareTo );
    FILETIME him = (FILETIME)tempTime;
    FILETIME me = (FILETIME)(*this);
    return ::CompareFileTime( &me, &him );
}


int 
ScheduleTime::compare( 
            const SYSTEMTIME &compareTo ) const
{
    ScheduleTime tempTime( compareTo );
    FILETIME him = (FILETIME)tempTime;
    FILETIME me = (FILETIME)(*this);
    return ::CompareFileTime( &me, &him );
}


void
ScheduleTime::toLocalTime( 
    const LARGE_INTEGER& theUTCTime,
    LARGE_INTEGER& theLocalTime )
{
    FILETIME utcTime;
    FILETIME ftTime;

    utcTime.dwLowDateTime = theUTCTime.LowPart;
    utcTime.dwHighDateTime = theUTCTime.HighPart;

    ::FileTimeToLocalFileTime( &utcTime, &ftTime );

    theLocalTime.LowPart = ftTime.dwLowDateTime;
    theLocalTime.HighPart = ftTime.dwHighDateTime;
}


void
ScheduleTime::toUTCTime( 
    const LARGE_INTEGER& theLocalTime,
    LARGE_INTEGER& theUTCTime )
{
    FILETIME utcTime;
    FILETIME ftTime;

    ftTime.dwLowDateTime = theLocalTime.LowPart;
    ftTime.dwHighDateTime = theLocalTime.HighPart;

    ::LocalFileTimeToFileTime( &ftTime, &utcTime );

    theUTCTime.LowPart = utcTime.dwLowDateTime;
    theUTCTime.HighPart = utcTime.dwHighDateTime;
}


#endif // Windows functions




/*******************************************************************
    time_t functions
*******************************************************************/

#if defined(UNIX) || defined(_UNIX)

/**
    Initialize a schedule time with the current system time (UTC)
*/
ScheduleTime::ScheduleTime()
{
    _time = ::time(NULL);
    _isAbsolute = true;
    _millis = 0;
    break_time();
}


ScheduleTime::ScheduleTime( 
            const time_t &liTime,
            const bool isUTC )
{
    if ( isUTC )
    {
        _time = liTime;
    }
    else
    {
        toUTCTime( liTime, _time );
    }

    _isAbsolute = true;
    _millis = 0;
    break_time();
}


ScheduleTime::ScheduleTime( 
            const struct tm &tmTime )
{
    _time = ::mktime( (struct tm*) &tmTime );
    _isAbsolute = true;
    _millis = 0;
    break_time();
}


void 
ScheduleTime::break_time()
{
    if ( !_isAbsolute )
    {
        _time = ::time(NULL);
        _time += (_millis/1000);
    }
}


void 
ScheduleTime::toTM(
            const time_t &li, 
            struct tm &tmTime )
{
    tmTime = *::gmtime( (time_t*)&li );
}


int 
ScheduleTime::compare( 
            const ScheduleTime &compareTo ) const
{
    time_t myTime = getTime();
    time_t hisTime = compareTo.getTime();
    return (myTime < hisTime) ? -1 : (myTime > hisTime) ? 1 : 0;
}


int 
ScheduleTime::compare( 
            const time_t &compareTo ) const
{
    time_t myTime = getTime();
    return (myTime < compareTo) ? -1 : (myTime > compareTo) ? 1 : 0;
}

int 
ScheduleTime::compare( 
            const struct tm &compareTo ) const
{
    time_t myTime = getTime();
    time_t hisTime = ::mktime( (struct tm*) &compareTo );
    return (myTime < hisTime) ? -1 : (myTime > hisTime) ? 1 : 0;
}


void
ScheduleTime::toLocalTime( 
    const time_t& theUTCTime,
    time_t& theLocalTime )
{
    struct tm theTime = *::localtime( &theUTCTime );
    theLocalTime = ::mktime( &theTime );
}


void
ScheduleTime::toUTCTime( 
    const time_t& theLocalTime,
    time_t& theUTCTime )
{
    struct tm theTime = *::gmtime( &theLocalTime );
    theUTCTime = ::mktime( &theTime );
}


#endif // time_t functions


