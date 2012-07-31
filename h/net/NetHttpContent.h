
#if !defined(NetHttpContent_H_LOADED)
#define NetHttpContent_H_LOADED

// [
// $        NetHttpContent.h
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

#include <map>

#include <cslib.h>
#include <util/UtilStringUtils.h>

class HttpContent  
{
public:

    enum HttpMsgTypeEnum
    {
        HTTP_MSG_TYPE_NONE = 0,
        HTTP_MSG_TYPE_REQUEST = 1,
        HTTP_MSG_TYPE_RESPONSE = 2
    };

    HttpContent();
    ~HttpContent();

    // this tells what type of http content the object represents
    // Options are a request or a response
    HttpMsgTypeEnum getHttpMsgType() const { return _msgType; }
    void setHttpMsgType(const HttpMsgTypeEnum msgType) { _msgType = msgType; }

    const String& getMimeType() const { return _mimeType; }
    void setMimeType(const String& mime);

    const String& getMimeEncoding() const { return _encoding; }
    void setMimeEncoding(const String& enc);
    
    const String& getHttpVersion() const { return _version; }
    void setHttpVersion(const String& version) { _version = version; }

    bool getIsForm() const { return _contentIsForm; }
    void setIsForm();

    // HTPP headers
    const StringUtils::StringMap& getHeaders() const { return _headers; }
    void setHeaders( const StringUtils::StringMap& headers ) { _headers = headers; }

    const String& getHeaderProperty(const String& propName) const;
    void setHeaderProperty(const String& propName, const String& propVal) { _headers[propName] = propVal; }

    void removeHeaderProperty(const String& propName) { _headers.erase( propName ); }
    bool propertyExists(const String& propName) const { return (_headers.find(propName) != _headers.end()); }

    bool generate(String& header, String& body) const;
    bool parseHeader( const String& header );
    bool parseBody( const String& body );

    // these are specific to HTTP_MSG_TYPE_REQUEST
    const String& getServerPath() const { return _serverPath; }
    void setServerPath(const String& serverPath) { _serverPath = serverPath; }

    const String& getQueryString(const String& key) const;
    void setQueryString(const String& key, const String& value) { _queries[ key ] = value; }

    const String& getRequestBody() const;
    void  setRequestBody( const String& body );

    bool doesAcceptType( const String& theType ) const;
    const String& getAcceptList() const;
    const String& getAcceptEncoding() const;
    const String& getAcceptLanguage() const;
    const String& getAcceptCharset() const;

    // these are specific to HTTP_MSG_TYPE_RESPONSE
    int getErrorCode() const { return _errorCode; }
    void setErrorCode(const int code);

    const String& getErrorDesc() const { return _errDesc; }
    void setErrorDesc(const String& desc) { _errDesc = desc; }

    const String& getResponseBody() const;
    void setResponseBody(const String& body);

private:
    bool parseCommand( const String& command );
    bool parseOneHeader( const String& header );

    String generateCommand() const;
    String generateHeader() const;
    void setContentQueries(const String& body);

private:

    typedef std::map<String, String> QueryMap;

    bool _contentIsForm;

    HttpMsgTypeEnum _msgType;

    String _mimeType;
    String _encoding;
    StringUtils::StringMap _headers;

    // specific to a request object
    String _version;
    String _serverPath;
    String _body;

    // for a response object
    int _errorCode;
    String _errDesc;
    QueryMap _queries;
};

#endif // NetHttpContent_H_LOADED
