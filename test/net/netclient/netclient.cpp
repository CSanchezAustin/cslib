//      netclient.cpp
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

#if defined(_WINDOWS) || defined(WIN32)
#pragma warning(disable: 4786)
#endif

#include <fstream>

#include "cslib.h"
#include "net/Net.h"
#include "net/NetSocket.h"
#include "net/NetHttpSerPost.h"
#include "net/NetHttpClientPost.h"
#include "net/NetHttpContent.h"
#include "net/NetHttpUtils.h"
#include "util/UtilRefCountedPtrInlines.h"
#include "util/UtilStringUtils.h"


void testHttpUsingRequest(const String& url);
void testHttpUsingForm(const String& url);


const String httpPayload(NTEXT("Sample request data.\nMore payload.\n\n"));

int main()
{
    Net::Initializer initter;

  OFSTREAM out(NTEXT("debug-log"));
  OFSTREAM err(NTEXT("err.log"));
//    Debug::setOutputStream( out );
//    Debug::setErrStream( err );

    Debug::setCurrentModule( NTEXT("netclient") );

    const String url(NTEXT("http://localhost:8080"));

    while(true)
    {
        testHttpUsingRequest(url);
        testHttpUsingForm(url); 
    }

    return 0;
}


void testHttpUsingRequest(const String& url)
{
    CHAR_T buff[256];

    SPRINTF(buff, NTEXT("Sending post data via request to %s"), url.c_str());
    COUT << buff << std::endl;

    HttpClientPost post;
    post.setServerURL(url);

    HttpContent httpRequest;
    HttpContent httpReply;

    httpRequest.setHttpMsgType(HttpContent::HTTP_MSG_TYPE_REQUEST);

    String protocol;
    String host;
    unsigned short port;
    String path;

    HttpUtils::parseURL(url, protocol, host, port, path);
    if(path.size() != 0)
        httpRequest.setServerPath(path);
    else
        httpRequest.setServerPath(NTEXT("/"));

    httpRequest.setHttpVersion(NTEXT("1.0"));
    httpRequest.setHeaderProperty(NTEXT("Host"), StringUtils::toString( port ));
    httpRequest.setHeaderProperty(NTEXT("CALEB XML Type"), NTEXT("Good ole XML"));
    httpRequest.setRequestBody(httpPayload);

    bool ok = post.send(httpRequest, httpReply);

    if ( !ok )
    {
        DBG_LOGERR(NTEXT("testHttp: post.send() failed"));
        return;
    }
     
    COUT << NTEXT("Reply ********") << std::endl;
    COUT << httpReply.getResponseBody() << std::endl;
}


void testHttpUsingForm(const String& url)
{
    CHAR_T buff[256];

    SPRINTF(buff, NTEXT("Sending post data via form to %s"), url.c_str());
    COUT << buff << std::endl;

    HttpClientPost post;
    post.setServerURL(url);

    HttpContent httpRequest;
    HttpContent httpReply;

    httpRequest.setHttpMsgType(HttpContent::HTTP_MSG_TYPE_REQUEST);

    String protocol;
    String host;
    unsigned short port;
    String path;

    HttpUtils::parseURL(url, protocol, host, port, path);
    if(path.size() != 0)
        httpRequest.setServerPath(path);
    else
        httpRequest.setServerPath(NTEXT("/"));

    httpRequest.setHttpVersion(NTEXT("1.0"));
    httpRequest.setHeaderProperty(NTEXT("Host"), StringUtils::toString( port ));
    httpRequest.setHeaderProperty(NTEXT("CALEB XML Type"), NTEXT("Good ole XML"));
    httpRequest.setIsForm();
    httpRequest.setQueryString(NTEXT("TEST_QRY"), httpPayload);

    bool ok = post.send(httpRequest, httpReply);

    if ( !ok )
    {
        DBG_LOGERR(NTEXT("testHttp: post.send() failed"));
        return;
    }
     
    COUT << NTEXT("Reply ********") << std::endl;
    COUT << httpReply.getResponseBody() << std::endl;
}

