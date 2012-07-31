#ifndef ThdTypes_H_LOADED
#define ThdTypes_H_LOADED
// [
// $        ThdTypes.h
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
//         Author: Chris Sanchez
// 
//         RCS:
//                 $Revision: 2.0 $
//                 $State: Exp $
//                 $Author: $
//                 $Date: $
// ?
//         Contents: ThdTypes class
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

#ifdef UNIX
#  include <pthread.h>
#  include <unistd.h>
#endif

EXPORT_DLL
enum ThdEnums
{
    WAIT_INFINITE = -1
};

EXPORT_DLL
enum enumThreadPriority
{
    keTPLowest = -2,
    keTPBelowNormal = -1,
    keTPNormal = 0,
    keTPAboveNormal = 1,
    keTPHighest = 2
};

EXPORT_DLL
enum enumThreadStartMode
{
    keTSNormal = 0x00000000,
    keTSSuspended = 0x00000004
};


EXPORT_DLL 
enum enumSyncOpResult
{ 
    SYNCOP_SUCCESS = 0,
    SYNCOP_FAILED = -1,
    SYNCOP_TIMEOUT = -2,
    SYNCOP_ABORT = -3
};

// ThdTypes_H_LOADED
#endif
