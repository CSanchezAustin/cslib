#ifndef ScheduleTime_H_LOADED
#define ScheduleTime_H_LOADED
// [
// $        ScheduleTime.h
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
//         Contents: ScheduleTime class
//
//         Purpose:
//
//         Usage Notes: The schedule time class is a high resolution time whose format is 
//                      relative UTC time
//
//                 
// ?
// !        Changes History
//     $Header: $
//         
// ]

#include <time.h>

#include <cslib.h>

// conversion mulipliers for milliseconds
#define SECONDS_TO_MILLISECS (1000)
#define MINUTES_TO_MILLISECS (1000 * 60)
#define HOURS_TO_MILLISECS (1000 * 60 * 60)
#define DAYS_TO_MILLISECS (1000 * 60 * 60 * 24)



/*******************************************************************
    Windows functions
*******************************************************************/

#if defined(_WINDOWS) || defined(WIN32)

class EXPORT_DLL ScheduleTime
{
public:
    static const unsigned long kMilliIntPerSecond;

        //  These times are not translated to UTC. The caller
        //  must account for this.
    ScheduleTime();
    ScheduleTime( const ScheduleTime &rhs );
    ScheduleTime( const FILETIME &ftTime );
    ScheduleTime( const SYSTEMTIME &sysTime );
    ScheduleTime( const LARGE_INTEGER &liTime, const bool isUTC = true );
    ScheduleTime( const struct tm &tmTime );
    ScheduleTime( const unsigned long millis );
    ~ScheduleTime();

    void break_time();

    const LARGE_INTEGER &getTime() const { return _time; }

    operator LARGE_INTEGER()const { return _time; }
    operator SYSTEMTIME() const;
    operator FILETIME() const;
    operator struct tm() const;

    /**
        converts a large integer local time to/from a large integer system time 
        conversion to struct tm, file time and system time
    */
    static void toLocalTime( const LARGE_INTEGER& theSystemTime, LARGE_INTEGER& theLocalTime );
    static void toUTCTime( const LARGE_INTEGER& theLocalTime, LARGE_INTEGER& theSystemTime );
    static void toTM( const LARGE_INTEGER &li, struct tm &tmTime );
    static void toFILETIME( const LARGE_INTEGER &li, FILETIME &ftTime );
    static void toSYSTEMTIME( const LARGE_INTEGER &li, SYSTEMTIME &sysTime );

    ScheduleTime &operator=( const ScheduleTime &rhs );

    inline bool operator==( const ScheduleTime &rhs ) const
	{
	    return _time.QuadPart == rhs._time.QuadPart;
	}
    inline bool operator!=( const ScheduleTime &rhs ) const
	{
	    return _time.QuadPart != rhs._time.QuadPart;
	}
    inline bool operator<( const ScheduleTime &rhs ) const
	{
	    return _time.QuadPart < rhs._time.QuadPart;
	}
    inline bool operator<=( const ScheduleTime &rhs ) const
	{
	    return _time.QuadPart <= rhs._time.QuadPart;
	}
    inline bool operator>( const ScheduleTime &rhs ) const
	{
	    return _time.QuadPart > rhs._time.QuadPart;
	}
	inline bool operator>=( const ScheduleTime &rhs ) const
	{
	    return _time.QuadPart >= rhs._time.QuadPart;
	}

    friend ScheduleTime operator+(const ScheduleTime& rhs, const unsigned long millis);
    friend ScheduleTime operator-(const ScheduleTime& rhs, const unsigned long millis);

    // returns the difference between the this object and the parameter in milliseconds
    unsigned long difference(const ScheduleTime& rhs);

    int compare( const ScheduleTime &compareTo ) const;
    int compare( const LARGE_INTEGER &compareTo ) const;
    int compare( const struct tm &compareTo ) const;
    int compare( const FILETIME &compareTo ) const;
    int compare( const SYSTEMTIME &compareTo ) const;

protected:

private:

    //  if the time is absolute or relative.
    bool    _isAbsolute;

    //  time value
    LARGE_INTEGER   _time;

    // relative time in milliseconds
    unsigned long _millis;
};

// friend operators

inline ScheduleTime 
operator+(
            const ScheduleTime& rhs, 
            const unsigned long millis)
{
    LONGLONG i = Int32x32To64( millis, ScheduleTime::kMilliIntPerSecond );
    LARGE_INTEGER li = rhs;
    li.QuadPart += i;
    return ScheduleTime( li );
}


inline ScheduleTime 
operator-(
            const ScheduleTime& rhs, 
            const unsigned long millis)
{
    LONGLONG i = Int32x32To64( millis, ScheduleTime::kMilliIntPerSecond );
    LARGE_INTEGER li = rhs;
    li.QuadPart -= i;
    return ScheduleTime( li );
}



inline unsigned long
ScheduleTime::difference(
    const ScheduleTime& rhs )
{
    LARGE_INTEGER meLI = *this;
    LARGE_INTEGER rightLI = rhs;
    return (unsigned long)((meLI.QuadPart - rightLI.QuadPart) / ScheduleTime::kMilliIntPerSecond);
}



#else // time_t version
/*******************************************************************
    time_t functions
*******************************************************************/


class EXPORT_DLL ScheduleTime
{
public:
        //  These times are not translated to UTC. The caller
        //  must account for this.
    ScheduleTime();
    ScheduleTime( const ScheduleTime &rhs );
    ScheduleTime( const time_t &liTime, const bool isUTC = true );
    ScheduleTime( const struct tm &tmTime );
    ScheduleTime( const unsigned long millis );
    ~ScheduleTime();

    void break_time();

    const time_t &getTime() const { return _time; }

    operator time_t() const { return _time; }
    operator struct tm() const;

    /**
        converts a large integer local time to/from a large integer system time 
        conversion to struct tm, file time and system time
    */
    static void toLocalTime( const time_t& theSystemTime, time_t& theLocalTime );
    static void toUTCTime( const time_t& theLocalTime, time_t& theSystemTime );
    static void toTM( const time_t &li, struct tm &tmTime );

    ScheduleTime &operator=( const ScheduleTime &rhs );

    inline bool operator==( const ScheduleTime &rhs ) const
	{
	    return _time == rhs._time;
	}
    inline bool operator!=( const ScheduleTime &rhs ) const
	{
	    return _time != rhs._time;
	}
    inline bool operator<( const ScheduleTime &rhs ) const
	{
	    return _time < rhs._time;
	}
    inline bool operator<=( const ScheduleTime &rhs ) const
	{
	    return _time <= rhs._time;
	}
    inline bool operator>( const ScheduleTime &rhs ) const
	{
	    return _time > rhs._time;
	}
	inline bool operator>=( const ScheduleTime &rhs ) const
	{
	    return _time >= rhs._time;
	}

    friend ScheduleTime operator+(const ScheduleTime& rhs, const unsigned long millis);
    friend ScheduleTime operator-(const ScheduleTime& rhs, const unsigned long millis);

    // returns the difference between the this object and the parameter in milliseconds
    unsigned long difference(const ScheduleTime& rhs);

    int compare( const ScheduleTime &compareTo ) const;
    int compare( const time_t &compareTo ) const;
    int compare( const struct tm &compareTo ) const;

protected:

private:

    //  if the time is absolute or relative.
    bool    _isAbsolute;

    //  time value
    time_t   _time;

    // relative time in milliseconds
    unsigned long _millis;
};


// friend operators

inline ScheduleTime 
operator+(
            const ScheduleTime& rhs, 
            const unsigned long millis)
{
    time_t oldTime = rhs._time;
    oldTime += (millis/1000);
    return ScheduleTime( oldTime );
}


inline ScheduleTime 
operator-(
            const ScheduleTime& rhs, 
            const unsigned long millis)
{
    time_t oldTime = rhs._time;
    oldTime -= (millis/1000);
    return ScheduleTime( oldTime );
}



inline unsigned long
ScheduleTime::difference(
    const ScheduleTime& rhs )
{
//    double newTime = ::difftime(_time, rhs._time);
    double newTime = _time - rhs._time;
    return (unsigned long)(newTime * SECONDS_TO_MILLISECS);
}

#endif // time_t version


// ScheduleTime_H_LOADED
#endif
