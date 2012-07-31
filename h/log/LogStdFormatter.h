#ifndef LogStdFormatter_H_LOADED
#define LogStdFormatter_H_LOADED
//      LogStdFormatter.h
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
//      RCS:
//              $Revision: 2.0 $
//              $Author: $
//              $Date: $
//
//      Contents:	This class formats a line of text for the log file
//          This class takes in a string with the following identifiers
//          supported.
//          0 : date
//          1 : time
//          2 : current thread id
//          3 : current module (if not DLL)
//          4 : message
//          5 : log level
//          6:  strerror value
//      for example:
//          "$0$ - $1$: $4$" will produce the output
//          "12/2/00 - 10:00AM: Here is the log message
//	
//      Current Info:
//      $Header: $

#include <time.h>
#include <string.h>
#include <errno.h>
#include <vector>

#include <cslib.h>
#include <sys/SysManager.h>
#include <util/UtilStringUtils.h>

class EXPORT_DLL StdFormatter
{
public:
    /**
        construct a Standard logger
        @param level level to log at
        @param stderr log to stderr also
        @param stdout log to std out
   */
    StdFormatter( const String& fmt );

    ~StdFormatter();

    /**
        formats a log line
        @param msg the message to format
        @param the level the message is being logged at
    */
    String format( const String& msg, const int level = 0 );

private:
    class Token
    {
    public:
        virtual ~Token(){}
        virtual String toString() = 0;
        virtual bool isMessageToken() { return false; }
        virtual bool isLogLevelToken() { return false; }
        virtual bool isModuleToken() { return false; }
    };

    class DateTimeToken : public Token
    {
    public:
        DateTimeToken(const bool isDate) :
            _isDate(isDate)
        {}
        ~DateTimeToken(){}

        String toString()
        {
            time_t t = ::time(NULL);
            struct tm currentTime = *::localtime( &t );
			return StringUtils::toString( currentTime, _isDate );
        }

    private:
        bool _isDate;
    };


    class ThreadToken : public Token
    {
    public:
        ThreadToken()
        {}
        ~ThreadToken(){}

        String toString()
        {
			return StringUtils::toString( 
                    (long)SysManager::getCurrentThreadId() );
        }
    };

    class ErrorToken : public Token
    {
    public:
        ErrorToken()
        {}
        ~ErrorToken(){}

        String toString()
        {
			return StringUtils::toString( STRERROR( errno ) ); 
        }
    };

    class StringToken : public Token
    {
    public:
        StringToken(
                    const String val, 
                    const bool isMessage = false,
                    const bool isLogLevel = false,
                    const bool isModule = false ) :
             _val(val),
             _isMessage(isMessage),
             _isLogLevel(isLogLevel),
             _isModule(isModule)
        {}
        ~StringToken(){}

        String toString() { return _val; }
        bool isMessageToken() { return _isMessage; }
        bool isLogLevelToken() { return _isLogLevel; }
        bool isModuleToken() { return _isModule; }

    private:
        String _val;
        bool _isMessage;
        bool _isLogLevel;
        bool _isModule;
    };

private:
    std::vector<Token*> _tokens;

private:
    StdFormatter();
    StdFormatter( const StdFormatter &);
};

#endif // LogStdFormatter_H_LOADED
