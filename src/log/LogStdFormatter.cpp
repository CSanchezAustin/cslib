//      LogStdLogger.cpp
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
//      Contents:	log formater implementation
//	
//      Current Info:
//      $Header:$

#include <cslib.h>

#include <log/LogStdFormatter.h>
#include <log/LogStdLogger.h>
#include <util/UtilStringUtils.h>
#include <util/UtilStringTokenizer.h>

StdFormatter::StdFormatter(
            const String& fmt)
{
    bool processingToken = false;
    StringTokenizer st( fmt, NTEXT("$"), false );

    // special case to make sure that the curren token is not at
    // the beginning of the line

    while ( st.hasMoreTokens() )
    {
        // look for a token
        String token = st.getNextToken();
        if (token.size() == 0 )
        {
            processingToken = !processingToken;
            continue;
        }

        if (!processingToken)
        {
            _tokens.push_back( 
                new StringToken( token ) );            
        }
        else
        {
            // process the token
            int id = StringUtils::toInt( token );
            if ( id == 0 ) // date
            {
                _tokens.push_back( new DateTimeToken( true ) );
            }
            else if ( id == 1 ) // time
            {
                _tokens.push_back( new DateTimeToken( false ) );
            }
            else if ( id == 2 ) // thread id
            {
                _tokens.push_back( new ThreadToken );
            }
            else if ( id == 3 ) // current module (application defined)
            {
                _tokens.push_back(
                    new StringToken( EMPTYSTR, false, false, true) );
            }
            else if ( id == 4 ) // message (user defined)
            {
                _tokens.push_back( 
                    new StringToken( EMPTYSTR, true, false, false ) );
            }
            else if ( id == 5 ) // log level token (user defined)
            {
                _tokens.push_back( 
                    new StringToken( EMPTYSTR, false, true, false ) );
            }
            else if ( id == 6 ) // strerror token
            {
                _tokens.push_back( new ErrorToken() );
            }
            else
            {
                if ( token.length() > 0 )
                    _tokens.push_back( 
                        new StringToken( token ) );
            }
        }
        
        processingToken = !processingToken;
    }
}


StdFormatter::~StdFormatter()
{
    while (_tokens.size() > 0 )
    {
        std::vector<Token*>::iterator it = _tokens.begin();
        Token* token = (*it);
        _tokens.erase( it );
        delete token;
    }
}

/*
    formats a log line
    @param msg the message to format
*/
String 
StdFormatter::format(
            const String& msg,
            const int level )
{
    String result;

    for ( std::vector<Token*>::iterator it = _tokens.begin();
            it != _tokens.end(); it++ )
    {
        Token* token = (*it);
        if ( token->isMessageToken() )
        {
            result += msg;
        }
        else if ( token->isLogLevelToken() )
        {
            if ( level == StdLogger::LOGC_ERR )
            {
                result += NTEXT("ERR");
            }
            else if ( level == StdLogger::LOGC_MSG )
            {
                result += NTEXT("MSG");
            }
            else if ( level == StdLogger::LOGC_WARN )
            {
                result += NTEXT("WRN");
            }
            else if ( level == StdLogger::LOGC_DEBUG )
            {
                result += NTEXT("DBG");
            }
            else
            {
                result += NTEXT("U");
                result += StringUtils::toString( level );
            }
        }
        else if ( token->isModuleToken() )
        {
            result += Debug::getCurrentModule();
        }
        else
        {
            result += token->toString();
        }
    }

    return result;
}
