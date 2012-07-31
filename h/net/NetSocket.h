#if !defined(NetSocket_H_LOADED)
#define NetSocket_H_LOADED

// [
// $        NetSocket.h
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
#include <net/Net.h>

class Socket  
{
public:
    enum SocketPending_e 
    {
        SOCKPENDING_IN,
        SOCKPENDING_OUT,
        SOCKPENDING_ERR
    };

    enum SocketTimeout_e 
    {
        TIMEOUT_INFINITE = -1,
        TIMEOUT_READ = 5,
        TIMEOUT_WRITE = 5,
        TIMEOUT_ERR = 10
    };

public:
    Socket();
    virtual ~Socket();
    
    virtual bool listen(const unsigned short port, const int backlog = SOMAXCONN);
    virtual Socket* accept();
    virtual bool connect(const String& host, const unsigned short port);
    virtual void close();
    virtual void shutdown();
    virtual void linger(const int secs);

    virtual int  read(String&, const int sz);
    virtual bool send(const String& data);
    
    virtual bool isPending(
        const SocketPending_e &action, 
        const int timeout = TIMEOUT_INFINITE) const;

    enum 
    {
        STATUS_CLOSED = 0,
        STATUS_LISTENING = 1,
        STATUS_CONNECTED = 2,
        STATUS_WRITE_SHUTDOWN = 3,
        STATUS_DISCONNECTED = 4,
        STATUS_READ_SHUTDOWN = 5
    } m_status;
    
    enum 
    {
        BUFF_SIZE = 1024
    };
    
private:
#if defined(_WINDOWS) || defined(WIN32)
    SOCKET m_sock;

    Socket(SOCKET sock);
#else
    Socket(int sock);

    int m_sock;
#endif
};

#endif // NetSocket_H_LOADED
