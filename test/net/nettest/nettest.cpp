//      nettest.cpp
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

#if defined(_WINDOWS) || defined(WIN32)
#include <conio.h>
#endif

#include "cslib.h"
#include "net/Net.h"
#include "net/NetSocket.h"
#include "net/NetHttpSerPost.h"
#include "net/NetHttpClientPost.h"
#include "net/NetHttpContent.h"
#include "net/NetHttpUtils.h"
#include "net/NetListenerSocket.h"


void test_utils();
void test_listener();

int main()
{
  OFSTREAM out(NTEXT("debug-log"));
  OFSTREAM err(NTEXT("err.log"));
//    Debug::setOutputStream( out );
//    Debug::setErrStream( err );

    Debug::setCurrentModule( NTEXT("NETTEST") );
    Net::Initializer initter;

    // test some standard functions first
    test_utils();

	test_listener();
    return 0;
}


void test_utils()
{
    const String url1(NTEXT("http://www.test.com"));
    const String url2(NTEXT("http://www.test.com/"));
    const String url3(NTEXT("http://www.test.com:8080"));
    const String url4(NTEXT("http://www.test.com:9090/"));
    const String url5(NTEXT("http://www.test.com/serverpath"));
    const String url6(NTEXT("http://www.test.com/serverpath/"));
    const String url7(NTEXT("http://www.test.com:9090/serverpath"));
    const String url8(NTEXT("http://www.test.com:9090/serverpath/"));

    String host;
    unsigned short port;
    String path;
    String protocol;

    HttpUtils::parseURL(url1, protocol, host, port, path );
    HttpUtils::parseURL(url2, protocol, host, port, path );
    HttpUtils::parseURL(url3, protocol, host, port, path );
    HttpUtils::parseURL(url4, protocol, host, port, path );
    HttpUtils::parseURL(url5, protocol, host, port, path );
    HttpUtils::parseURL(url6, protocol, host, port, path );
    HttpUtils::parseURL(url7, protocol, host, port, path );
    HttpUtils::parseURL(url8, protocol, host, port, path );


    String encodedStr;
    String decodedStr;
    HttpUtils::urlEncode(url8, encodedStr);
    HttpUtils::urlDecode(encodedStr, decodedStr);
    ASSERT(url8 == decodedStr);
}

void test_listener()
{
	ListenerSocket listener;
	bool listening = listener.listen(1000, 5, NTEXT("192.168.0.120"));
	if (listening)
	{
		listener.accept();
	}
}

