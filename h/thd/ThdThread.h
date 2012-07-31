#ifndef ThdThread_H_LOADED
#define ThdThread_H_LOADED
// [
// $        ThdThread.h
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
//                 $Author: $
//                 $Date: $
// ?
//         Contents: ThdThread class
//
//         Purpose:
//
//         Usage Notes:
//
//                 
// ?
// !        Changes History
//     $Header: $
//         
// ]


#include <cslib.h>

#include <thd/ThdTypes.h>
#include <util/UtilRefCountedPtr.h>

class ThdEvent;
class iRunnable;

class EXPORT_DLL ThdThread
{
public:
    // create a ThdThread
    ThdThread();
    ThdThread( RefCountedPtr<iRunnable> &runner );

    virtual ~ThdThread();

    bool create( 
        const enumThreadPriority  priority = keTPNormal,
        const enumThreadStartMode dwCreateFlags = keTSNormal, 
        unsigned int    nStackSize = 0 );

    // call if you aren't using a runner
    // to start the thread with
    virtual unsigned int run() { return 0;}

    // called by the new thread after started, but before
    // is does any work
    virtual bool init() { return true; }

    // call to release the new thread
    // if it was created in suspended mode
    bool releaseThread();

    // the runner object (if supplied in the constructor)
    RefCountedPtr<iRunnable>  getRunner() const { return _entry; }

    // used to get and set a thread identifier
    unsigned long getThreadID() const { return _threadID; }
    void setThreadID(const unsigned long threadId) { _threadID = threadId; }

protected:


private:
    void closeThread();

    // Unimplemented standard methods
    ThdThread(const ThdThread& );
    ThdThread& operator=(const ThdThread&);

private:
    // data
#if defined(_WINDOWS) || defined(WIN32)
    HANDLE _mSyncObj;
#else
    pthread_t _mSyncObj;
    pthread_attr_t _attr;
#endif

    RefCountedPtr<iRunnable>   _entry;
    unsigned long              _threadID;
    RefCountedPtr<ThdEvent>    _pContinueEvent;

    friend class Startup;
public:
    class Startup
    {
        public:
            Startup( ThdThread *controlThread );
            ~Startup();

            void waitForInitializeEvent();
            void initializeComplete();

        public:
            ThdThread*              _controlThread;
            RefCountedPtr<ThdEvent> _hInitEvent;
            RefCountedPtr<ThdEvent> _hContinueEvent;
            bool                    _bError;
    };
};

// ThdThread_H_LOADED
#endif
