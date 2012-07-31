
#if !defined(NetHttpUtils_H_LOADED)
#define NetHttpUtils_H_LOADED

// [
// $        NetHttpUtils.h
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


class HttpContent;


namespace HttpUtils
{
    enum ParseState
    {
        keCommand,
        keHeader,
        keBody,
        keComplete
    };

    enum
    {
        keCodeOk = 200,
        keCodeCreated = 201,
        keCodeAccepted = 202,
        keCodeNoContent = 204,
        keCodeMovePerm = 301,
        keCodeMoveTemp = 302,
        keCodeNotMod = 304,
        keCodeBadRequest = 400,
        keCodeUnauthorized = 401,
        keCodeForbidden = 403,
        keCodeNotFound = 404,
        keCodeInternalErr = 500,
        keCodeNotImplemented = 501,
        keCodeBadGateway = 502,
        keCodeNotAvailable = 503
    };

    const unsigned short keDefaultPort = 80;

    // stuff in the http command header
    const String HTTP_NAME = NTEXT("HTTP");
    const String HTTP_POST_METHOD = NTEXT("POST");
    const String HTTP_VERSION_PREFIX = NTEXT("HTTP/");
    const String HTTP_VERSION = NTEXT("1.0");
    const String HTTP_ENDL = NTEXT("\r\n");
    const String HTTP_HEADER_ENDL = NTEXT("\r\n\r\n");
    const String HTTP_DEFAULT_SVRPATH = NTEXT("/");

    // stuff in the http header
    const String HTTP_FORM_APPLICATION_FORM = NTEXT("application/x-www-form-urlencoded");
    const String HTTP_MIME_PLAINTEXT = NTEXT("text/plain");
    const String HTTP_MIME_XMLTEXT = NTEXT("text/xml");
    const String HTTP_ENC_8859 = NTEXT("iso-8859-1");
    const String HTTP_ENC_UTF8 = NTEXT("UTF-8");
    const String HTTP_CHARSET = NTEXT("charset");
    const String HTTP_CONTENT_TYPE = NTEXT("Content-Type");
    const String HTTP_CONTENT_LEN = NTEXT("Content-Length");
    const String HTTP_ACCEPT_LIST = NTEXT("Accept");
    const String HTTP_ACCEPT_ALL = NTEXT("*/*");
    const String HTTP_ACCEPT_ENC = NTEXT("Accept-Encoding");
    const String HTTP_ACCEPT_LANG = NTEXT("Accept-Language");
    const String HTTP_ACCEPT_CHARSET = NTEXT("Accept-Charset");
    const String HTTP_HOST = NTEXT("host");


    // error strings
    const String HTTP_DESC_OK = NTEXT("OK");
    const String HTTP_DESC_CREATED = NTEXT("Created");
    const String HTTP_DESC_ACCEPTED = NTEXT("Accepted");
    const String HTTP_DESC_NOCONTENT = NTEXT("No Content");
    const String HTTP_DESC_MOVEPERM = NTEXT("Moved Permanently");
    const String HTTP_DESC_MOVETEMP = NTEXT("Moved Temporarily");
    const String HTTP_DESC_NOTMOD = NTEXT("Not Modified");
    const String HTTP_DESC_BADREQ = NTEXT("Bad Request");
    const String HTTP_DESC_UNAUTH = NTEXT("Unauthorized");
    const String HTTP_DESC_FORBIDDEN = NTEXT("Forbidden");
    const String HTTP_DESC_NOTFOUND = NTEXT("Not Found");
    const String HTTP_DESC_INTERR = NTEXT("Internal Server Error");
    const String HTTP_DESC_NOTIMPL = NTEXT("Not Implemented");
    const String HTTP_DESC_BADGATEWAY = NTEXT("Bad Gateway");
    const String HTTP_DESC_NOTAVAIL = NTEXT("Service Unavailable");

    // Utilities
    void urlEncode(const String&    input,
                   String&          output);

    void urlDecode(const String&    input,
                   String&          output);

    void parseURL(const String&     url,
                  String&           protocol,
                  String&           host,
                  unsigned short&   port,
                  String&           path);

    String getErrorDescription( const int code );

    CHAR_T htoi(CHAR_T hi, CHAR_T low);
}

#endif // NetHttpUtils_H_LOADED
