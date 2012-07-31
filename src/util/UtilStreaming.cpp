//      UtilStreaming.cpp
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
#include <cslib.h>

#include <lang/ScheduleTime.h>
#include <util/UtilStreaming.h>
#include <util/UtilMapping.h>


#if defined(_WINDOWS) || defined(WIN32)

bool
StreamUtils::putKey( 
            const String &key, 
            const ScheduleTime &data, 
            Mapping &outMap )
{
    LARGE_INTEGER outTime = data;
    return StreamUtils::putKey( key, outTime, outMap );
}


bool
StreamUtils::putKey( 
            const String &key, 
            const LARGE_INTEGER &data, 
            Mapping &outMap )
{
    outMap[ key ] = (long)data.LowPart;
    outMap[ key + kHighPart ] = (long)data.HighPart;

    return true;
}

#else

bool
StreamUtils::putKey( 
            const String &key, 
            const ScheduleTime &data, 
            Mapping &outMap )
{
  time_t outTime = (time_t)data;
  return StreamUtils::putKey( key, (long)outTime, outMap );
}

#endif

bool
StreamUtils::putKey( 
            const String &key, 
            const long data, 
            Mapping &outMap )
{
    outMap[ key ] = data;
    return true;
}


bool
StreamUtils::putKey( 
            const String &key, 
            const Mapping &data, 
            Mapping &outMap )
{
    outMap[ key ] = data;
    return true;
}


bool
StreamUtils::putKey( 
            const String &key, 
            const String &data, 
            Mapping &outMap )
{
    outMap[ key ] = data;
    return true;
}


#if defined(_WINDOWS) || defined(WIN32)

bool
StreamUtils::getKey( 
            const String &key, 
            const Mapping &inMap, 
            ScheduleTime &data )
{
    LARGE_INTEGER inTime = { 0, 0 };
    bool result = StreamUtils::getKey( key, inMap, inTime );
    if ( result )
    {
        data = ScheduleTime( inTime );
    }
    else
    {
        data.break_time();
    }

    return result;
}

bool
StreamUtils::getKey( 
            const String &key, 
            const Mapping &inMap, 
            LARGE_INTEGER &data )
{
    Mapping::const_iterator key1Iter = inMap.get_ref( key, MappingRef::keLong );
    Mapping::const_iterator key2Iter = inMap.get_ref( key + kHighPart, MappingRef::keLong );

    bool result =  ( key1Iter != inMap.end() ) &&
                   ( key2Iter != inMap.end() );
    if ( result )
    {
        data.LowPart = (long)(*key1Iter).second;
        data.HighPart = (*key2Iter).second;
    }
    else
    {
        data.LowPart = 0;
        data.HighPart = 0;
    }

    return result;
}

#else

bool
StreamUtils::getKey( 
            const String &key, 
            const Mapping &inMap, 
            ScheduleTime &data )
{
    time_t inTime;
    bool result = StreamUtils::getKey( key, inMap, inTime );
    if ( result )
    {
        data = ScheduleTime( inTime );
    }
    else
    {
        data.break_time();
    }

    return result;
}

#endif

bool
StreamUtils::getKey( 
            const String &key, 
            const Mapping &inMap, 
            long &data )
{
    Mapping::const_iterator keyIter = inMap.get_ref( key, MappingRef::keLong );
    bool result =  keyIter != inMap.end();
    if ( result )
    {
        data = (*keyIter).second;
    }
    else
    {
        data = 0;
    }

    return result;
}


bool
StreamUtils::getKey( 
            const String &key, 
            const Mapping &inMap, 
            Mapping &data )
{
    Mapping::const_iterator keyIter = inMap.get_ref( key, MappingRef::keMapping );
    bool result =  keyIter != inMap.end();
    if ( result )
    {
        data = (*keyIter).second;
    }

    return result;
}


bool
StreamUtils::getKey( 
            const String &key, 
            const Mapping &inMap, 
            String &data )
{
    Mapping::const_iterator keyIter = inMap.get_ref( key, MappingRef::keString );
    bool result =  keyIter != inMap.end();
    if ( result )
    {
        data = (String)(*keyIter).second;
    }
    else
    {
        data = NTEXT("");
    }

    return result;
}
