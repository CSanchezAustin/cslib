// [
// $        NetHttpSerPost.cpp
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

#include <net/NetHttpSerPost.h>
#include <net/NetSocket.h>
#include <net/NetHttpContent.h>
#include <net/NetHttpUtils.h>
#include <thd/ThdAutoLock.h>
#include <util/UtilRefCountedPtrInlines.h>
#include <util/UtilStringUtils.h>


HttpSerPost::HttpSerPost(RefCountedPtr<Socket> & v_socket)
{
    m_socket = v_socket;
}


HttpSerPost::~HttpSerPost()
{
    m_socket->close();
}


/**
   getHttpHeader
   reads the stream until a valid http header is
   found or an error occurs
*/
bool 
HttpSerPost::getHttpHeader(
    String& ioData)
{
    while ( ioData.find( HttpUtils::HTTP_HEADER_ENDL ) == String::npos )
    {
        int bytesRead = m_socket->read( ioData, 16*1024 );
        if ( bytesRead < 0 )
        {
            DBG_LOGERR(NTEXT("HttpSerPost::getHttpHeader: did not get all the headers - ") + ioData );
            return false;
        }
    }

    return true;
}


bool 
HttpSerPost::readRequest(
    HttpContent& content )
{
    bool readOk = false;

    String httpText;
    if ( getHttpHeader(httpText) )
    {
        // have to check and see if the entire message was retrieved in one message
        String header;
        String body;
        if ( StringUtils::splitFront( HttpUtils::HTTP_HEADER_ENDL, httpText, &header, &body ) )
        {
            // header received, save it off 
            DBG_LOGDBG(NTEXT("HttpSerPost::readRequest: header received - ") + header ); 

            readOk = content.parseHeader( header );
            if ( readOk )
            {
                // now we have to make sure that we read the rest of the 
                // body in if it hasn't already been read in...	
                int sz = StringUtils::toInt( content.getHeaderProperty( HttpUtils::HTTP_CONTENT_LEN ) );
                int numToRead = sz - body.length();

                if ( numToRead >= 0 )
                {
                    // see if we have the body, if not, then we have to fetch it
                    while ( ( numToRead > 0 ) && readOk )
                    {
                        // get the size of the body and read it in
                        String moreText;
                        int numRead = m_socket->read( moreText, numToRead );
                        if ( numRead > 0 )
                        {
                            numToRead -= moreText.length();
                            body += moreText;
                        }
                        else if ( numRead <= 0 )
                        {
                            DBG_LOGERR(NTEXT("HttpSerPost::readRequest: failed to read entire body - ") + body );
                            readOk = false;
                        }
                    }
                }
                else
                {
                    // the body that we read in was larger than what the header indicated. In
                    // this case we have to truncate the string
                    body = body.substr( 0, sz );
                }

                // body received
                if ( readOk )
                {
                    readOk = content.parseBody( body );
                    DBG_LOGDBG(NTEXT("HttpSerPost::readRequest: body received - ") + body );
                }
            }
            else
            {
                DBG_LOGERR(NTEXT("HttpSerPost::readRequest: failed read the header - ") + header );
            }
        }
        else
        {
            DBG_LOGERR(NTEXT("HttpSerPost::readRequest: failed to find HTTP delimiters - ") + httpText );
        }
    }
    else
    {
        DBG_LOGERR(NTEXT("HttpSerPost::readRequest: failed get the http header - ") + httpText );
    }

    if ( !readOk )
    {
        HttpContent errContent;
        errContent.setHttpMsgType( HttpContent::HTTP_MSG_TYPE_RESPONSE );
        errContent.setErrorCode ( HttpUtils::keCodeBadRequest );

        String header;
        String body;
        errContent.generate( header, body );
        m_socket->send( header + body );

        DBG_LOGERR( NTEXT("HttpSerPost::readRequest: parse request failed") );
    }

    return readOk;
}



bool 
HttpSerPost::sendResponse(
    HttpContent& content )
{
   
    // Only allow one send() at a time ...
    ThdAutoCriticalSection lock(m_sendMutex);

    // Send the HTTP request ...
    String header;
    String body;
    content.generate( header, body );
    if ( !m_socket->send( header ) )
    {
        DBG_LOGERR( NTEXT("HttpSerPost::sendResponse: failed to send header") + header );
        return false;
    }

    if ( !m_socket->send( body ) )
    {
        DBG_LOGERR( NTEXT("HttpSerPost::sendResponse: failed to send body") + body );
        return false;
    }

    DBG_LOGDBG( (NTEXT("HttpSerPost::sendResponse: sent - ") + header) + body ); 

    return true;
}
