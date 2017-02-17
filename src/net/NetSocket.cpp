// [
// $        NetSocket.cpp
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

#include <memory>
#include <errno.h>

#ifdef UNIX
#include <sys/select.h>
#include <sys/time.h>
#endif

#include <net/NetSocket.h>
#include <net/Net.h>
#include <util/UtilStringUtils.h>

Socket::Socket()
{
    m_sock = -1;
    m_status = STATUS_CLOSED;
}

#if defined(_WINDOWS) || defined(WIN32)
Socket::Socket(SOCKET sock)
#else
Socket::Socket(int sock)
#endif
{
    m_sock = sock; 
    m_status = STATUS_CONNECTED;
}

Socket::~Socket()
{
    close();
}

bool Socket::listen(
            const unsigned short port, 
            const int backlog )
{
    struct sockaddr_in serv_addr;
#if defined(_WINDOWS) || defined(WIN32)
    SOCKET sockfd;
#else
    int sockfd;
#endif
    
    // Make sure that socket is closed ...
    close();

    // Open a TCP socket (an Internet stream socket) ...
    if ((sockfd = ::socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        DBG_LOGERR(NTEXT("Socket::listen: socket() failed"));
        return false;
    }

    int val = 1;    
    if( ::setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, 
                     (char *)&val, sizeof(val)) != 0)
    {
        DBG_LOGERR(NTEXT("Socket::listen: setsockopt() failed"));
        closesocket(sockfd);
        return false;
    }

    // Bind to the specified port ...
    ::bzero((char*)&serv_addr, sizeof(serv_addr));

    serv_addr.sin_family      = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port        = htons(port);

    if ( ::bind(sockfd,(struct sockaddr*)&serv_addr, sizeof(serv_addr)) != 0)
    {
        DBG_LOGERR(NTEXT("Socket::listen: bind() failed"));
        closesocket(sockfd);
        return false;
    }

    if( ::listen(sockfd, backlog) != 0)
    {
        DBG_LOGERR(NTEXT("Socket::list: listen() failed"));
        closesocket(sockfd);
        return false;
    }

    m_sock = sockfd;
    m_status = STATUS_LISTENING;

    return true;
}

bool Socket::connect(
            const String& host, 
            const unsigned short port)
{
    // Initialize ...
    struct sockaddr_in ad;
    struct hostent *hp;
    
    // initialize the sockaddr struct ...
    ::memset(&ad, 0, sizeof(ad));
    ad.sin_family = AF_INET;
    
    // Get the ip address of the host ...
	hp = ::gethostbyname(StringUtils::toCString(host).get());
    
    if (hp == NULL)
    {
        DBG_LOGERR(NTEXT("doSend: gethostbyname() failed"));
        return false;
    }
    ::memcpy(&ad.sin_addr,hp->h_addr,hp->h_length);
    
    // Set the port ...
    ad.sin_port = htons(port);  // convert to big-endian

    // Create the socket ...

    m_sock = ::socket(AF_INET, SOCK_STREAM, 0);

    if(m_sock < 0)
    {
        DBG_LOGERR(NTEXT("doSend: error creating socket"));
        return false;
    }

    // Connect to the host ...
    if(::connect(m_sock,(struct sockaddr*)&ad, sizeof(ad)) < 0)
    {
        DBG_LOGERR(NTEXT("doSend: error connecting to host"));
        close();
        return false;
    }

    m_status = STATUS_CONNECTED;
    return true;
}

Socket* Socket::accept()
{
#if defined(_WINDOWS) || defined(WIN32)
    SOCKET sock;
#else
    int sock;
#endif
    
    // Make sure that we're listing ...
    if (m_status != STATUS_LISTENING)
    {
        DBG_LOGERR(NTEXT("Socket::accept: failed - not currently listening"));
        return (Socket*)0;
    }

    // Accept the connection ...
    struct sockaddr_in cli_addr;
#if defined(_WINDOWS) || defined(WIN32)
    int clilen = sizeof(cli_addr);
#else
    socklen_t clilen = sizeof(cli_addr);
#endif

    sock = ::accept(m_sock,
                    (struct sockaddr*)&cli_addr,
                    &clilen);

    if (sock == -1)
    {
        DBG_LOGERR(NTEXT("Socket::accept: accept() failed"));
        return (Socket*) 0;
    }

    return new Socket(sock);
}

bool Socket::send(
    const String& data)
{
    // we are connected
    if (m_status != STATUS_CONNECTED)
    {
        DBG_LOGERR(NTEXT("Socket::sendData: not connected"));
        return false;
    }

//    if (isPending(SOCKPENDING_ERR, TIMEOUT_ERR))
//    {
//        DBG_LOGERR(NTEXT("Socket::sendData: not connected"));
//        return false;
//    }

    if (::send(m_sock, StringUtils::toCString(data).get(), data.size(), 0) < 0)
    {
        DBG_LOGERR(NTEXT("Socket::sendData: send() failed"));
        return false;
    }

    return true;
}


int 
Socket::read(
            String& buff, 
            const int sz)
{
    if (m_status != STATUS_CONNECTED)
    {
        return -1;
    }

    std::unique_ptr<char> sockbuff(new char[ BUFF_SIZE + 1 ]);

    int totalRead = 0;
    int bytesToRead = sz;

    while ( isPending( SOCKPENDING_IN, TIMEOUT_READ ) && ( bytesToRead > 0 ) )
    {
        int readSize = bytesToRead > BUFF_SIZE ? BUFF_SIZE : bytesToRead;
#if defined(_WINDOWS) || defined(WIN32)
        int bytesRead = ::recv(m_sock, sockbuff.get(), readSize, 0);
#else
        int bytesRead = ::recv(m_sock, (void*)sockbuff.get(), readSize, 0);
#endif
        // couple of things can happen here.
        // 1.  bytesRead == readSize, all data was read
        // 2. bytesRead < 0, an error occurred - kill the connection
        // 3. bytesRead < readSize, not all data has arrived - keep trying to read
        // 4. bytesRead == 0, no data was read - same as #3
        if (bytesRead < 0)
        {
            String errStr = NTEXT("Socket::read: recv() failed, err:");
            errStr += StringUtils::toString( errno );
            DBG_LOGERR(errStr);
            m_status = STATUS_DISCONNECTED;
            return -1;
        }
        else if ( bytesRead == 0 )
        {
            // this happens when the other side has
            // performed an orderly shutdown
            m_status = STATUS_READ_SHUTDOWN;
            break;
        }
        else
        {
            sockbuff.get()[ bytesRead ] = '\0';

            buff += StringUtils::toString(sockbuff.get());
            totalRead += bytesRead;
            bytesToRead -= bytesRead;
        }
    }

    return totalRead;
}


void Socket::shutdown()
{
    if (m_status != STATUS_CLOSED)
    {
#if defined(_WINDOWS) || defined(WIN32)
        ::shutdown(m_sock, 0x01);
#else
        ::shutdown(m_sock, SHUT_RDWR);
#endif
        m_status = STATUS_WRITE_SHUTDOWN;
    }
}


void 
Socket::linger(const int secs)
{
    struct linger opt;

    opt.l_onoff  = 1;
    opt.l_linger = secs;

    ::setsockopt(
        m_sock,
        SOL_SOCKET,
        SO_LINGER,
        (char*)&opt,
        sizeof(opt));
}


void Socket::close()
{
    shutdown();

    if (m_status != STATUS_CLOSED)
    {
        closesocket(m_sock);

        m_sock = -1;
        m_status = STATUS_CLOSED;
    }
}


bool 
Socket::isPending(
    const SocketPending_e &action, 
    const int timeout) const
{
    timeval tv;
    timeval *tvPtr = NULL;

    fd_set grp;

    FD_ZERO(&grp);
    FD_SET(m_sock, &grp);

    // if a timeout is given, then set up the tvPtr
    // variasble for the select
    if ( timeout != TIMEOUT_INFINITE )
    {
        tv.tv_usec = ( timeout % 1000 ) * 1000;
        tv.tv_sec = timeout / 1000;
        tvPtr = &tv;
    }

#if defined(_WINDOWS) || defined(WIN32)
    SOCKET status = -1;
#else
    int status = -1;
#endif

    switch (action)
    {
    case SOCKPENDING_IN:
        status = ::select( m_sock + 1, &grp, NULL, NULL, tvPtr );
        break;

    case SOCKPENDING_OUT:
        status = ::select( m_sock + 1, NULL, &grp, NULL, tvPtr );
        break;

    case SOCKPENDING_ERR:
        status = ::select( m_sock + 1, NULL, NULL, &grp, tvPtr );
        break;
    }

    // no error or timeout on the socket
    if ( status < 0 )
    {
        return false;
    }

    
    if (FD_ISSET(m_sock, &grp))
    {
        return true;
    }

    return false;
}
