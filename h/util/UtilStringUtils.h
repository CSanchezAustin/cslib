#ifndef StringUtils_H_LOADED
#define StringUtils_H_LOADED
// [
// $        StringUtils.h
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
//                 $Author: sanchezc $
//                 $Date: 2002/09/26 17:19:31 $
// ?
//         Contents: StringUtils class
//
//         Purpose: Utility classes for Strings
//
//         Usage Notes:
//
//                 
// ?
// !        Changes History
//  $Header: /cvs/common/cslib/h/util/UtilStringUtils.h,v 2.1 2002/09/26 17:19:31 sanchezc Exp $       
// ]

#include <time.h>
#include <map>

#include <cslib.h>

namespace EXPORT_DLL StringUtils
{
    String& upper(String& ioString);
    String& lower(String& ioString);

    int compareCaseless(const String& source, const String& dest);

    String::size_type findCaseless(
        const String& source, 
        const String& pat);

    String toString(const int i, const int radix = 10);
    int toInt(const String& str);

    String toString(const long i, const int radix = 10);
    long toLong(const String& str);

    String toString(const ScheduleTime &time, const int radix = 10);
	std::auto_ptr<char> toCString(const ScheduleTime &time, const int radix = 10);
    ScheduleTime toScheduleTime(const String& str);    

    String toString(const char *inStr, const enumCodePage cp = keAnsi);
    String toString(const wchar_t *inStr, const enumCodePage cp = keAnsi);
    std::auto_ptr<char> toCString(const String &inStr, const enumCodePage cp = keAnsi);

    String toString( const Date& inDate );
    Date toDate( const String& inStr );

    String toString( const float inFloat );
    float toFloat( const String& inStr );

    String toString( const double inDouble );
    double toDouble( const String& inStr );

    String toString( const bool inBool );
    bool toBool( const String& inString );

    String toString( const struct tm &t, const bool makeDate = true );
	struct tm toTM( const String& inStr );

    void trimFront( const String& pattern, String& source );
    void trimBack( const String& pattern, String& source );
    void trimBoth( const String& pattern, String& source );

    bool splitFront( 
                const String& pattern, 
                const String& source, 
                String* front, 
                String* back );

    bool splitBack( 
                const String& pattern, 
                const String& source, 
                String* front, 
                String* back );

    class CaselessCompare : public std::binary_function<String, String, bool>
    {
    public:
        bool operator() ( const String &item1, 
                          const String &item2 ) const
        {
            return compareCaseless(item1, item2) < 0;
        }
    };

    typedef std::map<String, String, CaselessCompare> StringMap;
}

// StringUtils_H_LOADED
#endif
