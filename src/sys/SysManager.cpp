//      SysManager.cpp
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
//      RCS:
//                 $Revision:$
//                 $Author:$
//                 $Date:$
//
//      Contents:	
//	
#include <cslib.h>

#include <sys/SysManager.h>
#include <util/UtilStringUtils.h>
#include <util/UtilStringTokenizer.h>
#include <util/UtilMapping.h>


#ifdef UNIX
#include <pthread.h>
#include <unistd.h>
#endif

#ifdef MACOSX
#include <sys/sysctl.h>
#endif

unsigned long
SysManager::getNumProcessors()
{
#if defined(_WINDOWS) || defined(WIN32)
    SYSTEM_INFO  _sysinfo;
    ::GetSystemInfo( &_sysinfo );
    return _sysinfo.dwNumberOfProcessors;
#elif defined(MACOSX)
    unsigned long numCPU = 1;
    
    int mib[4];
    size_t len = sizeof(numCPU); 
    
    /* set the mib for hw.ncpu */
    mib[0] = CTL_HW;
    mib[1] = HW_AVAILCPU;  // alternatively, try HW_NCPU;
    
    /* get the number of CPUs from the system */
    sysctl(mib, 2, &numCPU, &len, NULL, 0);
    
    if( numCPU < 1 ) 
    {
        mib[1] = HW_NCPU;
        sysctl( mib, 2, &numCPU, &len, NULL, 0 );
        
        if( numCPU < 1 )
        {
            numCPU = 1;
        }
    }
    return numCPU;
#else
    return ::sysconf( _SC_NPROCESSORS_ONLN );
#endif
}


unsigned long
SysManager::getPageSize()
{
#if defined(_WINDOWS) || defined(WIN32)
    SYSTEM_INFO  _sysinfo;
    ::GetSystemInfo( &_sysinfo );
    return _sysinfo.dwPageSize;
#else
    return ::sysconf( _SC_PAGE_SIZE );
#endif
}


unsigned long 
SysManager::getOptimalThreadCount()
{
    return getNumProcessors() * 2 + 2;
}


unsigned long 
SysManager::getCurrentThreadId()
{
#if defined(_WINDOWS) || defined(WIN32)
    return ::GetCurrentThreadId();
#else
    return (unsigned long)::pthread_self();
#endif
}



