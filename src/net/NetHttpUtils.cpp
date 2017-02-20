// [
// $        HttpUtils.cpp
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
//         RCS:
//                 $Revision:$
//                 $Author:$
//                 $Date:$
// 
// ]
#include <cslib.h>

#include <net/NetHttpUtils.h>
#include <util/UtilStringUtils.h>


static const CHAR_T* _digits = NTEXT("0123456789ABCDEF");

// parse a url in the form
// <protocol>://address[:port]/serverpath
//
void HttpUtils::parseURL(
			const String&	url,
            String&         protocol,
            String&         host,
            unsigned short& port,
            String&         path)
{
    //
    // Extract the protocol ...
    //
    size_t start = 0;
    size_t pos = url.find( SLASHSLASH );

    if (pos > 0)
    {
        protocol = url.substr(0, pos-1);
        start = pos + 2;
    }
    else
    {
        protocol = HTTP_NAME;
        start = 0;
    }

    //
    // Extract the host ...
    //
    bool extractPort = false;
    bool extractPath = false;

    pos = url.find( COLON, start);

    if ((pos > start) && (pos != String::npos))
    {
        extractPort = true;
        host = url.substr(start, pos - start);
        start = pos+1;
    }
    else
    {
        extractPort = false;

        pos = url.find(SLASH, start);

	    if ((pos > start) && (pos != String::npos))
        {
            extractPath = true;
            host = url.substr(start, pos - start);
            start = pos;
        }
        else
        {
            host = url.substr(start, url.size() - start);
        }
    }

    //
    // Extract the port ...
    //
    if (extractPort)
    {
        String portStr;

        pos = url.find(SLASH, start);

        if ((pos > start) && (pos != String::npos))
        {
            extractPath = true;
            portStr = url.substr(start, pos - start);
            start = pos;
        }
        else
        {
            extractPath = false;
            portStr = url.substr(start, url.size() - start);
        }

        port = (unsigned short)StringUtils::toInt(portStr.c_str());
    }
    else
    {
        port = keDefaultPort;
    }

    //
    // Extract the path ...
    //
    if (extractPath)
    {
        path = url.substr(start, url.size() - start);
    }
    else
    {
        path = SLASH;
    }
}



CHAR_T
HttpUtils::htoi(CHAR_T hi, CHAR_T low)
{

    hi  = toupper(hi);
    low = toupper(low);

    return ((STRCHR(_digits, hi)  - STRCHR(_digits, NTEXT('0'))) << 4) +
            (STRCHR(_digits, low) - STRCHR(_digits, NTEXT('0')));
}


void 
HttpUtils::urlDecode(
		const String& input,
        String&       output)
{
    output = NTEXT("");
    String::size_type i = 0;
    String::size_type sz = input.size();

    while (i < sz)
    {
        CHAR_T ch = input[i];
        i++;

        switch(ch)
        {
            case NTEXT('+') :
                output += ' ';
                break;

            case NTEXT('%') :
                if (i+1 < input.size())
                {
                    output += htoi(input[i], input[i+1]);
                    i += 2;
                }
                break;

            default :
                output += ch;
                break;
        }
    }
}



void 
HttpUtils::urlEncode(
		const String& input,
        String&       output)
{
    output.erase();
    for (size_t i=0, imax=input.size(); i < imax; i++)
    {
        CHAR_T ch = input[i];

        switch(ch)
        {
            case NTEXT(' ') :
                output += NTEXT('+');
                break;

            case NTEXT('$') :
            case NTEXT('-') :
            case NTEXT('_') :
            case NTEXT('.') :
            case NTEXT('+') :
            case NTEXT('!') :
            case NTEXT('*') :
            case NTEXT('\'') :
            case NTEXT('(') :
            case NTEXT(')') :
            case NTEXT(',') :
                output += ch;
                break;

            default :
                if ( (ch >= NTEXT('a')  &&  ch <= NTEXT('z'))  ||
                     (ch >= NTEXT('A')  &&  ch <= NTEXT('Z'))  ||
                     (ch >= NTEXT('0')  &&  ch <= NTEXT('9')) )
                {   
                    output += ch;
                }
                else
                {
                    output += NTEXT('%');
                    output += _digits[ch / 16];  // left 4 bits
                    output += _digits[ch % 16];  // right 4 bits
                }
        }
    }
}

String 
HttpUtils::getErrorDescription( const int code )
{
    switch( code )
    {
        case keCodeOk:
            return HTTP_DESC_OK;

        case keCodeCreated:
            return HTTP_DESC_CREATED;

        case keCodeAccepted:
            return HTTP_DESC_ACCEPTED;

        case keCodeNoContent:
            return HTTP_DESC_NOCONTENT;

        case keCodeMovePerm:
            return HTTP_DESC_MOVEPERM;

        case keCodeMoveTemp:
            return HTTP_DESC_MOVETEMP;

        case keCodeNotMod:
            return HTTP_DESC_NOTMOD;

        case keCodeUnauthorized:
            return HTTP_DESC_UNAUTH;

        case keCodeForbidden:
            return HTTP_DESC_FORBIDDEN;

        case keCodeNotFound:
            return HTTP_DESC_NOTFOUND;

        case keCodeInternalErr:
            return HTTP_DESC_INTERR;

        case keCodeNotImplemented:
            return HTTP_DESC_NOTIMPL;

        case keCodeBadGateway:
            return HTTP_DESC_BADGATEWAY;

        case keCodeNotAvailable:
            return HTTP_DESC_NOTAVAIL;

        default:
        case keCodeBadRequest:
            return HTTP_DESC_BADREQ;
    }

    return EMPTYSTR;
}


