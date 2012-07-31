#ifndef StringTokenizer_H_LOADED
#define StringTokenizer_H_LOADED
// [
// $        StringTokenizer.h
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
//         Contents: StringTokenizer class
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

class EXPORT_DLL StringTokenizer
{
public :
    /**
        Construct a tokenizer
        @param str the string to parse looking for tokens
        @param tokens a list of token delimiters to look for
        @param skipDelimiters if true is passed, then series of
                token delimiters encountered in str will be 
                skipped over and treated as one delimiter
    */
    StringTokenizer(const String& str,
                    const String& tokens,
                    const bool skipDelimiters = true,
                    const bool findAllTokens = false): 
            _str(str),
            _tokens(tokens),
            _updated(false),
            _moreTokens(true),
            _skipDelimiters(skipDelimiters),
            _findAllTokens(findAllTokens),
            _currentPos(String::npos),
            _nextPos(String::npos)
    {
    }

    ~StringTokenizer()
    {
    }

    /**
        hasMoreTokens
        Determines if there are more tokens in the tokenizer
        @return bool true if more tokens exist
    */
    bool hasMoreTokens()
    {
        return _moreTokens ? advance() : false;
    }

    /**
        getNextToken
        Advances the internal token pointer and returns the current token
        if one exists
        @return String the token or empty string 
    */
    String getNextToken()
    {
        if ( advance() )
        {
            _updated = false;

            return String( _str, _currentPos, _nextPos - _currentPos );
        }
        else
        {
            return EMPTYSTR;
        }
    }

private:

    /**
        advance
        Internal routine to advance the internal token pointer to the next token
        This function operates in two modes depending on how the tokenizer was constructed
        The first mode is _findAllTokens mode. In this mode we match all elements in the pattern
        string that we were constructed with. For example, if the pattern for a delimiter is "ab"
        then the entire string "ab" will be used to delimit tokens. Otherwise, either "a" or "b"
        will be used to delimit tokens.
        This function also supports a "_skipDelimiters" mode. In this mode We allow multiple
        delimiters to be skipped over. For example, taking the previous pattern of "ab" if we encounter
        a string with the following data "123ab123abab123", then getNextToken will return "123", 123", 123". 
        If we are not in "_skipDelimiters" mode calls to getNextToken will return "123", "123", "", "123".
        i.e. it did not skip over runs of delimiters.
    */    
    bool advance()
    {
        if(_updated)
            return _moreTokens;

        bool atStart = ( _nextPos == String::npos && _currentPos == String::npos );
        if ( atStart )
        {
            _nextPos = 0;
        }

        _currentPos = _nextPos;
        if ( _findAllTokens )
        {
            _nextPos = _str.find( _tokens, _currentPos );
        }
        else
        {
            _nextPos = _str.find_first_of( _tokens, _currentPos );
        }

        // handle the encountering a token at the start of the string specially
        if ( _nextPos > 0 || !atStart )
        {
            int delimiterCnt = 0;
            while( ( _nextPos <= _str.size() ) && ( _currentPos == _nextPos ) )
            {
                delimiterCnt++;
                if ( delimiterCnt > 1 && !_skipDelimiters )
                {
                    break;
                }
                
                if ( _findAllTokens )
                {
                    if ( _nextPos != 0 )
                    {
                        _currentPos = _nextPos + _tokens.length();
                    }
                    _nextPos = _str.find( _tokens, _currentPos );
                }
                else
                {
                    _currentPos++;
                    _nextPos = _str.find_first_of( _tokens, _currentPos );
                }
            }
        }

        _updated = true;

        if( _currentPos >= _str.size() )
        {
            _moreTokens = false;
        }

        return _moreTokens;
    }

private:
    String _str;
    String _tokens;

    bool _updated;
    bool _moreTokens;
    bool _skipDelimiters;
    bool _findAllTokens;

    String::size_type _currentPos;
    String::size_type _nextPos;

private:
    // Unimplemented standard methods
    StringTokenizer();
    StringTokenizer(const StringTokenizer& );
    StringTokenizer& operator=(const StringTokenizer&);

};

// StringTokenizer_H_LOADED
#endif
