//      StringUtils.cpp
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
//  $Header:$       
//

#include <cslib.h>

#include <util/UtilStringUtils.h>
#include <util/UtilStringTokenizer.h>

#if defined(_WINDOWS) || defined(WIN32)
    #include <TCHAR.h>
#else
    #include <stdlib.h>
#endif
#include <algorithm>
#include <cctype>

String&
StringUtils::upper(String& ioString)
{
    std::transform(ioString.begin(), ioString.end(), ioString.begin(), toupper);
    return ioString;
}

String&
StringUtils::lower(String& ioString)
{
    std::transform(ioString.begin(), ioString.end(), ioString.begin(), tolower);
    return ioString;
}

int
StringUtils::compareCaseless(
    const String& source, 
    const String& dest)
{
    String lowerSource(source);
    String lowerDest(dest);


    return StringUtils::lower(lowerSource ).compare( StringUtils::lower(lowerDest));
}

String::size_type
StringUtils::findCaseless(
    const String& source, 
    const String& pat)
{
    String lowerSource(source);
    String lowerPat(pat);

    return lowerSource.find(pat);
}


String 
StringUtils::toString(
			const int i, 
			const int radix)
{
#if defined(_WINDOWS) || defined(WIN32)
    TCHAR localKey[ 20 ];
    ::_itot_s( i, localKey, radix );
    return String( localKey );
#else
    CHAR_T localKey[ 20 ];
    SPRINTF( localKey, NTEXT("%d"), i );
    return String( localKey );
#endif
}


String 
StringUtils::toString(
			const long i, 
			const int radix)
{
#if defined(_WINDOWS) || defined(WIN32)
    TCHAR localKey[ 20 ];
    ::_ltot_s( i, localKey, radix );
    return String( localKey );
#else
    CHAR_T localKey[ 20 ];
    SPRINTF( localKey, NTEXT("%d"),(int) i );
    return String( localKey );
#endif
}


String 
StringUtils::toString(
			const ScheduleTime &sTime,  
			const int radix )
{
#if defined(_WINDOWS) || defined(WIN32)
    TCHAR localKey[ 34 ];
    ::_i64tot_s(  LARGE_INTEGER(sTime).QuadPart, localKey, 34, radix );
    return String( localKey );
#else
    CHAR_T localKey[ 20 ];
    SPRINTF( localKey, NTEXT("%d"), (int)sTime.getTime() );
    return String( localKey );
#endif
}


int 
StringUtils::toInt(
            const String &str )
{
#if defined(_WINDOWS) || defined(WIN32)
    return _ttoi( str.c_str() );
#else
    return ATOI( str.c_str() );
#endif
}


long 
StringUtils::toLong(
            const String &str )
{
#if defined(_WINDOWS) || defined(WIN32)
    return _ttol( str.c_str() );
#else
    return ATOL( str.c_str() );
#endif
}

ScheduleTime
StringUtils::toScheduleTime(
			const String &str )
{
#if defined(_WINDOWS) || defined(WIN32)
    LARGE_INTEGER x;
    x.QuadPart = _ttoi64( str.c_str() );
    return ScheduleTime( x );
#else
    return ScheduleTime( (time_t)ATOI(str.c_str()) );
#endif
}


AUTO_PTR(char)
StringUtils::toCString(
            const String &inStr, 
            const enumCodePage cp )
{
#ifndef UNICODE
  return AUTO_PTR(char)(::strcpy(new char[inStr.size() + 1], inStr.c_str()));
#else
#if defined(_WINDOWS) || defined(WIN32)

	UINT realCodePage = ( cp == keAnsi ) ? CP_ACP : CP_UTF8;
        
    //  get the size of the buffer that we need to create
    int sz = WideCharToMultiByte(realCodePage, 0, inStr.c_str(), -1, NULL, 0, NULL, NULL );

    //  convert the string 
    if (sz > 0 )
    {
        char *transcoded = new char[sz];
        if (transcoded != NULL)
        {
            sz = WideCharToMultiByte(realCodePage, 0, 
                                inStr.c_str(), -1,
                                transcoded, sz, NULL, NULL );
            if (sz != 0)
            {
                return std::unique_ptr<char>(transcoded);
            }
            else
            {
                delete [] transcoded;
            }
        }
    }

#else
		size_t buffSize = inStr.length() + 1;
		char* transcoded = new char[buffSize];
		if (transcoded != NULL)
		{
			size_t sz = ::wcstombs( transcoded, inStr.c_str(), buffSize );
			if (sz > 0)
			{
				return std::unique_ptr<char>(transcoded);
			}
			else
			{
				delete [] transcoded;
			}
		}
#endif // WINDOWS

		return std::unique_ptr<char>(NULL);

#endif // UNICODE
}

String 
StringUtils::toString(
            const wchar_t *inStr,
            const enumCodePage cp)
{
#ifdef UNICODE
    return String(inStr);
#else
    #if defined(_WINDOWS) || defined(WIN32)

        String result;

        if (inStr != NULL)
        {
            UINT realCodePage = ( cp == keAnsi ) ? CP_ACP : CP_UTF8;

            //  get the size of the buffer that we need to create
            int sz = WideCharToMultiByte(realCodePage, 0, inStr, -1, NULL, 0, NULL, NULL );

            //  convert the string 
            if (sz != 0)
            {
                char *transcoded = new char[sz];
                if (transcoded != NULL)
                {
		            sz = WideCharToMultiByte(realCodePage, 0, inStr, -1, transcoded, sz, NULL, NULL );
                    if (sz != 0)
                    {
                        result = transcoded;
                    }

                    delete [] transcoded;
                }
            }
        }

        return result;

    #else
		String result;

		size_t buffSize = ::wcslen(inStr) + 1;
		char* transcoded = new char[buffSize];
		if (transcoded != NULL)
		{
			size_t sz = ::wcstombs( transcoded, inStr, buffSize );
			if (sz != 0)
			{
				result = transcoded;
			}

			delete [] transcoded;
		}

		return result;
    #endif // windows    
#endif
}

String 
StringUtils::toString(
            const char *inStr,
            const enumCodePage cp)
{
#ifndef UNICODE
    return String(inStr);
#else
    #if defined(_WINDOWS) || defined(WIN32)

        String result;

        if (inStr != NULL)
        {
            UINT realCodePage = ( cp == keAnsi ) ? CP_ACP : CP_UTF8;

            //  get the size of the buffer that we need to create
            int sz = MultiByteToWideChar(realCodePage, 0, inStr, -1, NULL, 0 );

            //  convert the string 
            if (sz != 0)
            {
                wchar_t *transcoded = new wchar_t[sz];
                if (transcoded != NULL)
                {
                    sz = MultiByteToWideChar(realCodePage, 0, inStr, -1, transcoded, sz );
                    if (sz != 0)
                    {
                        result = transcoded;
                    }

                    delete [] transcoded;
                }
            }
        }

        return result;

    #else
		String result;

		size_t buffSize = inStr.length() + 1;
		wchar_t* transcoded = new wchar_t[buffSize];
		if (transcoded != NULL)
		{
			size_t sz = ::mbstowcs( transcoded, inStr.c_str(), buffSize );
			if (sz != 0)
			{
				result = transcoded;
			}

			delete [] transcoded;
		}

		return result;
    #endif // windows

#endif // UNICODE
}


String 
StringUtils::toString( 
			const Date& inDate )
{
    String result;
    result = toString( inDate.year() ) + NTEXT("-");
    if ( 10 > inDate.month() )
    {
        result = result + NTEXT("0");
    }
    result = result + toString( inDate.month() ) + NTEXT("-");
    if ( 10 > inDate.day() )
    {
        result = result + NTEXT("0");
    }
    result = result + toString( inDate.day() );
    return result;
}


// This utility expects the string to be in yyyy-mm-dd format, i.e. June 2, 
// 2001 is 2001-06-02
Date 
StringUtils::toDate( 
			const String& inStr )
{
    int month = 0;
    int day = 0;
    int year = 0;
	
	StringTokenizer tok( inStr, NTEXT("-") );
	if ( tok.hasMoreTokens() )
	{
		year = StringUtils::toInt( tok.getNextToken() );
	}

	if ( tok.hasMoreTokens() )
	{
		month = StringUtils::toInt( tok.getNextToken() );
	}

	if ( tok.hasMoreTokens() )
	{
		day = StringUtils::toInt( tok.getNextToken() );
	}

    return Date( ( short )month, ( short )day, ( short )year );
}

String 
StringUtils::toString( 
			const bool inBool )
{
    String result;
    if ( inBool )
    {
        result = TRUE_STR;
    }
    else
    {
        result = FALSE_STR;
    }
    return result;
}

bool StringUtils::toBool( const String& inString )
{
    bool result;
    if ( TRUE_STR == inString )
    {
        result = true;
    }
    else
    {
        result = false;
    }
    return result;
}


String 
StringUtils::toString( 
			const float inFloat )
{
#if defined(_WINDOWS) || defined(WIN32)
    TCHAR buf[1024];
    ::_stprintf_s( buf, NTEXT("%f"), inFloat );
    return String( buf );
#else
    CHAR_T localKey[ 1024 ];
    SPRINTF( localKey, NTEXT("%f"), (double)inFloat );
    return String( localKey );
#endif
}


float 
StringUtils::toFloat( 
			const String& inStr )
{
#if defined(_WINDOWS) || defined(WIN32)
    float result;
    ::_stscanf_s( inStr.c_str(), NTEXT("%f"), &result );
    return result;
#else
    float result;
    SSCANF( inStr.c_str(), NTEXT("%f"), &result );
    return result;
#endif
}


String 
StringUtils::toString( 
			const double inDouble )
{
#if defined(_WINDOWS) || defined(WIN32)
    TCHAR buf[1024];
    ::_stprintf_s( buf, NTEXT("%f"), inDouble );
    return String( buf );
#else
    CHAR_T localKey[ 1024 ];
    SPRINTF( localKey, NTEXT("%f"), inDouble );
    return String( localKey );
#endif
}


double 
StringUtils::toDouble( 
			const String& inStr )
{
#if defined(_WINDOWS) || defined(WIN32)
    double result;
    ::_stscanf_s( inStr.c_str(), NTEXT("%lf"), &result );
    return result;
#else
    return ATOF( inStr.c_str() );
#endif
}


String 
StringUtils::toString(
			const struct tm &t,
			const bool makeDate )
{
#if defined(_WINDOWS) || defined(WIN32)
    String::value_type buff[256];
    if ( makeDate )
    {
        _tcsftime(buff, sizeof(buff), NTEXT("%x"), &t );
    }
    else
    {
        _tcsftime(buff, sizeof(buff), NTEXT("%X"), &t );
    }
    return String(buff);
#else
    CHAR_T buff[256];
    if ( makeDate )
    {
        STRFTIME(buff, sizeof(buff), NTEXT("%x"), &t );
    }
    else
    {
        STRFTIME(buff, sizeof(buff), NTEXT("%X"), &t );
    }

    return String(buff);
#endif
}


void 
StringUtils::trimFront( 
            const String& pattern, 
            String& source )
{
    bool done = false;
    for ( size_t pos = 0, sz = source.size();
          pos < sz && !done; pos++ )
    {
        // look for the current character in the
        // pattern string. The first time we do not find
        // the character, we copy the trimmed string and exit
        if ( pattern.find_first_of( source[pos] ) == String::npos )
        {
            source = source.substr( pos );
            done = true;
        }
    }
}


void 
StringUtils::trimBack( 
            const String& pattern, 
            String& source )
{
    bool done = source.size() == 0;
	for ( String::size_type pos = source.size() - 1;
          (pos > 0) && !done; pos-- )
    {
        // look for the current character in the
        // pattern string. The first time we do not find
        // the character, we copy the trimmed string and exit
        if ( pattern.find_first_of( source[pos] ) == String::npos )
        {
            source = source.substr( 0, pos + 1 );
            done = true;
        }
    }
}


void 
StringUtils::trimBoth( 
            const String& pattern, 
            String& source )
{
    StringUtils::trimFront( pattern, source );
    StringUtils::trimBack( pattern, source );
}



bool 
StringUtils::splitFront(
            const String& pattern, 
            const String& source, 
            String* front, 
            String* back )
{
    bool res = false;

    String::size_type pos = source.find( pattern );
    if ( pos != String::npos )
    {
        if ( front != NULL )
        {
            *front = source.substr( 0, pos );
            res = true;
        }

        if ( back != NULL )
        {
            *back = source.substr( pos + pattern.length() );
            res = true;
        }
    }

    return res;
}



bool 
StringUtils::splitBack(
            const String& pattern, 
            const String& source, 
            String* front, 
            String* back )
{
    bool res = false;

    String::size_type pos = source.rfind( pattern );
    if ( pos != String::npos )
    {
        if ( front != NULL )
        {
            *front = source.substr( 0, pos );
            res = true;
        }

        if ( back != NULL )
        {
            *back = source.substr( pos + pattern.length() );
            res = true;
        }
    }

    return res;
}
