#ifndef Net_H_LOADED
#define Net_H_LOADED
// [
// $        Net.h
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
// ?
//         Contents: Net common
//
//         Purpose:
//
//         Usage Notes:
//
//                 
// ?
// !        Changes History
//         
// ]

#include <cslib.h>

#if defined(_WINDOWS) || defined(WIN32)

// Windows specific
#include <Winsock.h>

inline void
bzero(void *buff,const int sz) 
{
    memset(buff, 0x00, sz );
}

inline void
bcopy(void *dest,void *src, const int sz) 
{
    memcpy(dest, src, sz );
}

#else  // UNIX code

#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <signal.h>
#include <sys/socket.h>
#include <sys/resource.h>
#include <unistd.h>
#include <strings.h>

#ifndef RLIMIT_NOFILE
#ifdef RLIMIT_OFILE
#define RLIMIT_NOFILE RLIMIT_OFILE
#endif
#endif                

// Windows compatibility
#define closesocket ::close

#define SOCKET int

#endif


namespace Net
{
    class Initializer
    {
        public:
            Initializer()
            {
#if defined(_WINDOWS) || defined(WIN32)
                WSADATA wd;
                _initted = ::WSAStartup(MAKEWORD(1,1),&wd) == 0;
#else // UNIX
                ::signal(SIGPIPE, SIG_IGN);
                
#ifdef RLIMIT_NOFILE
                struct rlimit limit;
                if (::getrlimit(RLIMIT_NOFILE, &limit) == 0) 
                {
                    limit.rlim_cur = limit.rlim_max;
                    ::setrlimit(RLIMIT_NOFILE, &limit);
                    ::getrlimit(RLIMIT_NOFILE, &limit);
                }
#endif  // RLIMIT_NOFILE
#endif  // UNIX
            }


            ~Initializer()
            {
#if defined(_WINDOWS) || defined(WIN32)
                ::WSACleanup();
#else

#endif
            }

            const bool isInitted() const { return _initted; }

        private:
            bool _initted;
    };

    String getHostName();
}


// Net_H_LOADED
#endif
