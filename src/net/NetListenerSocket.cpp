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
// Copyright (c) 1998-2005 Chris Sanchez. All rights reserved.
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
//         Contents: Socket Listener Class
//
//         Purpose:
//
//         Usage Notes:
//
//                 
// ?
// !        Changes History
//     $Header: $
// ]

#include <cslib.h>

#include <memory>
#include <errno.h>

#ifdef UNIX
#include <sys/select.h>
#include <sys/time.h>
#endif

#include <net/NetListenerSocket.h>
#include <util/UtilStringUtils.h>

ListenerSocket::ListenerSocket()
{
    m_sock = -1;
    m_status = STATUS_CLOSED;
}


ListenerSocket::~ListenerSocket()
{
    close();
}


bool ListenerSocket::listen(
			const unsigned short port,
			const int backlog,
			const String ipAddr)
{
    struct sockaddr_in serv_addr;
    int                sockfd;
    
    // Make sure that socket is closed ...
    close();

    // Open a TCP socket (an Internet stream socket) ...
    if ((sockfd = ::socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        DBG_LOGERR(NTEXT("ListenerSocket::listen: socket() failed"));
        return false;
    }

    int val = 1;    
    if( ::setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, 
                     (char *)&val, sizeof(val)) != 0)
    {
        DBG_LOGERR(NTEXT("ListenerSocket::listen: setsockopt() failed"));
        closesocket(sockfd);
        return false;
    }

    // Bind to the specified port ...
    ::bzero((char*)&serv_addr, sizeof(serv_addr));

    serv_addr.sin_family      = AF_INET;
    serv_addr.sin_port        = htons(port);
	if ( ipAddr.empty() )
	{
		serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	}
	else
	{
	    serv_addr.sin_addr.s_addr = inet_addr(StringUtils::toCString(ipAddr).get());
	}

    if ( ::bind(sockfd,(struct sockaddr*)&serv_addr, sizeof(serv_addr)) != 0)
    {
        DBG_LOGERR(NTEXT("ListenerSocket::listen: bind() failed"));
        closesocket(sockfd);
        return false;
    }

    if( ::listen(sockfd, backlog) != 0)
    {
        DBG_LOGERR(NTEXT("ListenerSocket::list: listen() failed"));
        closesocket(sockfd);
        return false;
    }

    m_sock = sockfd;
    m_status = STATUS_LISTENING;

    return true;
}

SOCKET ListenerSocket::accept()
{
    SOCKET sock = 0;
    
    // Make sure that we're listing ...
    if (m_status != STATUS_LISTENING)
    {
        DBG_LOGERR(NTEXT("ListenerSocket::accept: failed - not currently listening"));
        return 0;
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
        DBG_LOGERR(NTEXT("ListenerSocket::accept: accept() failed"));
        return 0;
    }

    return sock;
}




void ListenerSocket::close()
{
    if (m_status != STATUS_CLOSED)
    {
        closesocket(m_sock);

        m_sock = -1;
        m_status = STATUS_CLOSED;
    }
}
