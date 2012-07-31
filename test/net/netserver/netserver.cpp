//      netserver.cpp
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

void doListen(int port);

int main()
{
    Net::Initializer initter;

  OFSTREAM out(NTEXT("debug-log"));
  OFSTREAM err(NTEXT("err.log"));
//    Debug::setOutputStream( out );
//    Debug::setErrStream( err );

    Debug::setCurrentModule( NTEXT("NETTEST") );
    Debug::setLevel(100);
 
    const int serverPort = 8080;
    doListen(serverPort);

    return 0;
}

void doListen(int port)
{
    Socket sock;

    sock.listen(port, 5);

    bool running = true;
    while ( running )
    {
        RefCountedPtr<Socket> t_socket(sock.accept());
        if ( t_socket == NULL )
        {
            return;
        }

        HttpSerPost post(t_socket);

        HttpContent content;
        if ( post.readRequest(content) )
        {
            String theData;

            if ( content.getIsForm() )
            {
    	        theData = content.getQueryString(NTEXT("TEST_QRY"));
                if ( theData == NTEXT("quit") )
                {
                    running = false;
                    theData = NTEXT("Server is shutting down");
                }
            }
            else
            {
    	        theData = content.getRequestBody();
            }

    	    COUT << theData.c_str() << std::endl; 

            HttpContent c2;
            c2.setHttpMsgType(HttpContent::HTTP_MSG_TYPE_RESPONSE);
            c2.setHttpVersion(NTEXT("1.0"));
            c2.setHeaderProperty(NTEXT("Server"), NTEXT("NerdStuff.com"));
            c2.setMimeEncoding(HttpUtils::HTTP_ENC_UTF8);

            if ( content.doesAcceptType(HttpUtils::HTTP_MIME_XMLTEXT) )
            {
                c2.setMimeType(HttpUtils::HTTP_MIME_XMLTEXT);
            }
            else
            {
                c2.setMimeType(HttpUtils::HTTP_MIME_PLAINTEXT);
            }

            c2.setHeaderProperty(NTEXT("Date"),NTEXT("Wed, 21 Feb 2001 04:48:27 GMT"));
            c2.setErrorCode(HttpUtils::keCodeOk);
	        
            String httpReplyData;
            httpReplyData  = NTEXT("<?rpc rpc-code=\"4\" xml=\"result\"?>");
            httpReplyData += NTEXT("<rpc-response>");
            httpReplyData += NTEXT("<result>");
            httpReplyData += theData;
            httpReplyData += NTEXT("</result>");
            httpReplyData += NTEXT("</rpc-response>");
            c2.setResponseBody(httpReplyData);
            post.sendResponse(c2);
        }
    }
}
