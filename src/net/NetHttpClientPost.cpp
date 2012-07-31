//
// HttpClientPost.cpp
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


#include <cslib.h>

#include <net/NetHttpClientPost.h>
#include <net/NetHttpSerPost.h>
#include <net/NetSocket.h>
#include <net/NetHttpContent.h>
#include <net/NetHttpUtils.h>
#include <thd/ThdAutoLock.h>
#include <util/UtilRefCountedPtrInlines.h>

#define MAX_BUFF 4096


HttpClientPost::HttpClientPost():
    m_socket(new Socket)
{
    m_abort     = false;
    m_proxyPort = 0;
}



HttpClientPost::~HttpClientPost()
{
    abortSend();
    m_socket->close();
}


void 
HttpClientPost::abortSend()
{
    m_abort = true;
    
    // Make sure that abortSend does not return until doSend()
    // releases the mutex.
    ThdAutoCriticalSection lock(m_sendMutex);
}


void 
HttpClientPost::setProxyServer(
    const String& server,
    const unsigned short port )
{
    ThdAutoCriticalSection lock(m_sendMutex);
    
    m_proxyServer = server;
    m_proxyPort   = port;
}



void 
HttpClientPost::setServerURL(
    const String& serverURL )
{
    ThdAutoCriticalSection lock(m_sendMutex);
    
    m_serverURL = serverURL;
    
    String protocol;
    HttpUtils::parseURL( m_serverURL.c_str(),
                         protocol,
                         m_server,
                         m_serverPort,
                         m_serverPath);
}


bool HttpClientPost::send(
    const HttpContent& httpRequest,
    HttpContent&       httpReply )
{
    //
    // Abort any previously executing send ...
    //
    abortSend();
    
    //
    // Only allow one send() at a time ...
    //
    ThdAutoCriticalSection lock(m_sendMutex);
    
    //
    // Make sure that server is valid ...
    //
    if (m_server.size() == 0  ||  m_serverPort <= 0)
    {
        DBG_LOGERR(NTEXT("HttpClientPost::send: failed"));
        return false;
    }
    
    //    
    // If proxy server is used, make sure that proxy port is valid ...
    //    
    if (m_proxyServer.size() > 0  &&  m_proxyPort <= 0)
    {
        DBG_LOGERR(NTEXT("HttpClientPost::send: failed"));
        return false;
    }
    

    //
    // Send the request to the server (or proxy server if necessary) ...
    //
    bool ok = false;

    if (m_proxyServer.size() == 0)   // no proxy server specified
    {
        ok = doSend(m_server,
                    m_serverPort,
                    httpRequest,
                    httpReply);
    }
    else  // use the proxy server
    {
        ok = doSend(m_proxyServer,
                    m_proxyPort,
                    httpRequest,
                    httpReply);
    }

    if ( !ok )
    {
        DBG_LOGERR(NTEXT("HttpClientPost::send: failed"));
        return false;
    }

    return true;
}


bool 
HttpClientPost::doSend(
    const String& host,
    const unsigned short port,
    const HttpContent& httpRequest,
    HttpContent&       httpReply)
{
    m_abort = false;
    
    String path;
    
    if (m_proxyServer.size() > 0)
    {
        path = m_serverURL;
    }
    else
    {
        // If a proxy server is not being used, and there is no path,
        // simply use "/" as the path.
        if (m_serverPath.size() == 0)
        {
            path += SLASH;
        }
        else
        {
            path += m_serverPath;
        }
    }
    
    //
    // Connect to the host ...
    //
    if( !m_socket->connect( host, port ) )
    {
        DBG_LOGERR(NTEXT("HttpClientPost::doSend: error connecting to host"));
        return false;
    }
    
    if (m_abort)
    {
        DBG_LOGDBG(NTEXT("HttpClientPost::doSend: aborting send"));
        return false;
    }
    
    String headers;
    String body;
    httpRequest.generate( headers, body );
    if( !m_socket->send( headers ) )
    {
        DBG_LOGERR(NTEXT("HttpClientPost::doSend: failed sending headers"));
        return false;
    }
    
    if( !m_socket->send( body ) )
    {
        DBG_LOGERR(NTEXT("HttpClientPost::doSend: failed sending body"));
        return false;
    }
    
    DBG_LOGDBG((NTEXT("HttpClientPost::doSend: sent - ") + headers) + body ); 
    
    //
    // Receive the reply ...
    //
    HttpSerPost post( m_socket );
    if ( !post.readRequest( httpReply ) )
    {
        DBG_LOGERR(NTEXT("HttpClientPost::doSend: content.readRequest() failed"));
        return false;
    }
    
    return true;
}

