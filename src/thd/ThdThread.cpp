//      ThdThread.cpp
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

#include <thd/ThdThread.h>
#include <thd/ThdEvent.h>
#include <thd/ThdTypes.h>
#include <thd/ThdAutoLock.h>
#include <thd/ThdCriticalSection.h>
#include <pat/PatRunnable.h>
#include <sys/SysComInitializer.h>
#include <sys/SysManager.h>
#include <util/UtilRefCountedPtrInlines.h>

#ifdef _DEBUG

static ThdCriticalSection gCrit;

#define THD_DEBUG(x, y) \
            { \
                ThdAutoCriticalSection lock(gCrit); \
                COUT << (x) << NTEXT(" (thread: ") <<  \
                                      (y) << NTEXT(")") \
                                      << std::endl; \
            }
#else
#define THD_DEBUG(x,y)
#endif



#if defined(_WINDOWS) || defined(WIN32)
    UINT APIENTRY _ThreadEntry( ThdThread::Startup *starter );
#else
    void* _ThreadEntry( ThdThread::Startup *starter );
#endif


ThdThread::ThdThread() :
#if defined(_WINDOWS) || defined(WIN32)
    _mSyncObj(NULL),
#endif
    _entry(NULL),
    _threadID(0)
{
}


ThdThread::ThdThread( RefCountedPtr<iRunnable> &runner) :
#if defined(_WINDOWS) || defined(WIN32)
    _mSyncObj(NULL),
#endif
    _entry(runner),
    _threadID(0)
{
}


ThdThread::~ThdThread()
{
    THD_DEBUG(NTEXT("ThdThread::~ThdThread: destroying"), SysManager::getCurrentThreadId());

    closeThread();
}


void
ThdThread::closeThread()
{
#if defined(_WINDOWS) || defined(WIN32)
    if ( _mSyncObj != NULL )
    {
        // wait for the thread to become signalled. (meaning that it has exited)
        ASSERT_D(::WaitForSingleObject( _mSyncObj, INFINITE ) == WAIT_OBJECT_0);
        ASSERT_D( ::CloseHandle( _mSyncObj ) == TRUE );
    }
#else // UNIX
    ASSERT_D( ::pthread_join(_mSyncObj, NULL ) == 0 );
#endif

    THD_DEBUG(NTEXT("ThdThread::closeThread: exiting"), getThreadID());
}


bool 
ThdThread::create( 
    const enumThreadPriority  priority,
    const enumThreadStartMode dwCreateFlags,
    unsigned int    nStackSize )
{
    Startup starter( this );
    _pContinueEvent = starter._hContinueEvent;

#if defined(_WINDOWS) || defined(WIN32)

    _mSyncObj = (HANDLE)_beginthreadex(   0, nStackSize,
                                          (unsigned (__stdcall *) (void *))&_ThreadEntry, &starter, 
                                          dwCreateFlags | keTSSuspended, 
                                          (unsigned int*)&_threadID );
    if ( _mSyncObj == NULL )
    {
        _pContinueEvent = NULL;
        return false;
    }

    // now set the threads priority
    int newPriority = THREAD_PRIORITY_NORMAL;
    switch ( priority )
    {
        case keTPLowest:
            newPriority = THREAD_PRIORITY_LOWEST;
            break;

        case keTPBelowNormal:
            newPriority = THREAD_PRIORITY_BELOW_NORMAL;
            break;

        case keTPNormal:
            newPriority = THREAD_PRIORITY_NORMAL;
            break;

        case keTPAboveNormal:
            newPriority = THREAD_PRIORITY_ABOVE_NORMAL;
            break;

        case keTPHighest:
            newPriority = THREAD_PRIORITY_HIGHEST;
            break;
    }

    ::SetThreadPriority( _mSyncObj, newPriority ); 
    ::ResumeThread( _mSyncObj );

#else // UNIX

    DBG_LOGERR( NTEXT("ThdThread::create: need to use create flags to start the thread") );
    DBG_LOGERR( NTEXT("ThdThread::create: need to set thread priority") );

    ::pthread_attr_init( &_attr );
    ::pthread_attr_setdetachstate( &_attr, PTHREAD_CREATE_JOINABLE );

    if ( nStackSize > 0 )
    {
        ::pthread_attr_setstacksize( &_attr, (int)nStackSize );
    }

    if ( ::pthread_create( &_mSyncObj, &_attr,(void* (*)(void*)) &_ThreadEntry, &starter ) != 0 )
    {
        return false;
    }

#endif

    // wait until the thread tells us it's started and has completed
    // its initialization sequence
    starter.waitForInitializeEvent();

    THD_DEBUG(NTEXT("ThdThread::create: got initialize event"), SysManager::getCurrentThreadId());
    
    // if error during startup, shut things down
    if ( starter._bError )
    {
        // if there was an error, then wait until the 
        // thread exits and then return from this fn()
        THD_DEBUG(NTEXT("ThdThread::create: got initialize error, closing the thread"), SysManager::getCurrentThreadId());
        closeThread();
        return false;
    }
    
    // at this point the thread should be started and 
    // trying to acquire the blocker mutex. If we have 
    // started up with that option, then the new thread
    // will block waiting for someone to release the 
    // mutex

    // if they want the thread started up in a "suspended mode"
    // then don't tell the new thread to start working, i.e.
    // set the signal
    if ( ( dwCreateFlags & keTSSuspended ) == 0 )
    {
        THD_DEBUG(NTEXT("ThdThread::create: preparing to release thread"), SysManager::getCurrentThreadId());
        releaseThread();
    }
    
    return true;
}


bool
ThdThread::releaseThread() 
{ 
    if ( _pContinueEvent != NULL )
    {
        THD_DEBUG(NTEXT("ThdThread::releaseThread: releasing thread"), getThreadID());
        _pContinueEvent->signal();
        _pContinueEvent = NULL;
        return true;
    }

    return false;
}



/////////////////////////////
//  Embedded classes
////////////////////////////


ThdThread::Startup::Startup( 
    ThdThread *controlThread ) :
    _hInitEvent(new ThdEvent),
    _hContinueEvent(new ThdEvent)
{
    _controlThread = controlThread;
    _bError = false;
}


ThdThread::Startup::~Startup()
{
}


void 
ThdThread::Startup::waitForInitializeEvent()
{
    ASSERT_D( _hInitEvent != NULL );
    _hInitEvent->wait();
}


void 
ThdThread::Startup::initializeComplete()
{
    ASSERT_D( _hInitEvent != NULL );
    _hInitEvent->signal();
}


#if defined(_WINDOWS) || defined(WIN32)
UINT APIENTRY _ThreadEntry( ThdThread::Startup *starter )
#else
void* _ThreadEntry( ThdThread::Startup *starter )
#endif
{
    unsigned int nResult = 0;
    
    { // scope for SysComInitializer
        SysComInitializer init;

        ASSERT_D( starter->_controlThread != NULL );
        ASSERT_D( starter->_hInitEvent != NULL );
        ASSERT_D( starter->_hContinueEvent != NULL );

        ThdThread* pStarterThread = starter->_controlThread;

        pStarterThread->setThreadID( SysManager::getCurrentThreadId() );

        THD_DEBUG(NTEXT("_ThreadEntry: Entering"), SysManager::getCurrentThreadId() );

        RefCountedPtr<ThdEvent> pContinueEvent(starter->_hContinueEvent);

        THD_DEBUG(NTEXT("_ThreadEntry: Beginning initialization"), SysManager::getCurrentThreadId());

        bool initErr = !pStarterThread->init();
        starter->_bError = initErr;

        //  Finished the initialization
        starter->initializeComplete();

        THD_DEBUG(NTEXT("_ThreadEntry: Completing initialization"), SysManager::getCurrentThreadId());

        if ( !initErr )
        {
            //  Now wait until we are told to continue
            pContinueEvent->wait();
            pContinueEvent = NULL;

            THD_DEBUG(NTEXT("_ThreadEntry: Beginning execution"), SysManager::getCurrentThreadId());
            
            //  Now we can do our thread work here
            if ( pStarterThread->getRunner() != NULL )
            {
                nResult = pStarterThread->getRunner()->run();
            }
            else
            {
                nResult = pStarterThread->run();
            }

            THD_DEBUG(NTEXT("_ThreadEntry: Completed execution"), SysManager::getCurrentThreadId());
        }
        else
        {
            THD_DEBUG(NTEXT("_ThreadEntry: Initialization error"), SysManager::getCurrentThreadId());
        }
    } // scope for SysComInitializer

    THD_DEBUG(NTEXT("_ThreadEntry: Exiting"), SysManager::getCurrentThreadId());

#if defined(_WINDOWS) || defined(WIN32)
    _endthreadex( nResult );
#else
    ::pthread_exit( reinterpret_cast<void*>(nResult) );
#endif

    return 0;
}
