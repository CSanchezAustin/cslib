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



#if defined(_WINDOWS) || defined(WIN32)
#   include <PROCESS.H>
#   include <conio.h>
#else
#   include <curses.h>
#endif

#include <time.h>
#include <fstream>
#include <stdio.h>

#include "thd/ThdThread.h"
#include "thd/ThdAutoLock.h"
#include "thd/ThdMutex.h"
#include "thd/ThdPool.h"
#include "thd/ThdRWLock.h"
#include "thd/ThdSemaphore.h"
#include "thd/ThdEvent.h"
#include "thd/ThdQueue.h"
#include "thd/ThdDeque.h"
#include "thd/ThdScheduler.h"
#include "thd/ThdManager.h"

#include "pat/PatSchedulable.h"
#include "pat/PatRunnable.h"
#include "sys/SysManager.h"
#include "sys/SysComInitializer.h"
#include "util/UtilRefCountedPtrInlines.h"

#if defined(_WINDOWS) || defined(WIN32)
#pragma warning(disable : 4355)
#pragma warning(disable : 4786)
#endif

static void test_run_once();
static void test_pool();
static void test_queue();
static void test_deque();
static void test_scheduler();
static void test_rwlock();
static void MyStarter( void *context );

int main(int argc, char* argv[])
{
    SysComInitializer comInitter;

  OFSTREAM out(NTEXT("debug-log"));
  OFSTREAM err(NTEXT("err.log"));
//    Debug::setOutputStream( out );
//    Debug::setErrStream( err );

    Debug::setCurrentModule( NTEXT("THDTEST") );

    COUT << NTEXT("start test_scheduler") << std::endl;
    test_scheduler();
    COUT << NTEXT("end test_scheduler") << std::endl;

    COUT << NTEXT("start test_pool") << std::endl;
    test_pool();
    COUT << NTEXT("end test_pool") << std::endl;
    
    COUT << NTEXT("start test_run_once") << std::endl;
    test_run_once();
    COUT << NTEXT("end test_run_once") << std::endl;
        
    COUT << NTEXT("start test_queue") << std::endl;
    test_queue();
    COUT << NTEXT("end test_queue") << std::endl;
    
    COUT << NTEXT("start test_deque") << std::endl;
    test_deque();
    COUT << NTEXT("end test_deque") << std::endl;

    COUT << NTEXT("start test_rwlock") << std::endl;
    test_rwlock();
    COUT << NTEXT("end test_rwlock") << std::endl;
    
    return 0;
}


class RunOnceRunner : public iRunnable
{
public:
    ThdEvent _s;
    
public:
    RunOnceRunner() :
        _s(NULL, false, false)
    {
    }
    
    virtual ~RunOnceRunner()
    {
    }

    unsigned run()
    {
        time_t tt = time(NULL);
        COUT << NTEXT("Ran once and signalling caller to proceed, (thread: ") << SysManager::getCurrentThreadId() 
             << NTEXT(" at ") << tt << NTEXT(")") << std::endl;

        _s.signal();
        return 0;
    }
};

class MyDataClass : public iRunnable
{
public:
    ThdMutex _l;
    ThdEvent _s;
    int		 _i;

public:
    MyDataClass() :
        _i(0)
    {
    }

    virtual ~MyDataClass()
    {
    }

    int inc()
    {
        ThdAutoLock stLocker(_l);
        return _i++;
    }

    unsigned run()
    {
        COUT << NTEXT("Entered MyDataClass::run, (thread: ") << SysManager::getCurrentThreadId() 
             <<  NTEXT(")") << std::endl;

        enumSyncOpResult res = _s.wait(0);
        while( res == SYNCOP_TIMEOUT )
        {
            inc();
            COUT << NTEXT("MyDataClass::run, (thread: ") << SysManager::getCurrentThreadId() 
                 <<  NTEXT(")") << std::endl;
            res = _s.wait(0);
        }

        COUT << NTEXT("Exiting MyDataClass::run, (thread: ") << SysManager::getCurrentThreadId() 
             <<  NTEXT(")") << std::endl;

        return 0;
    }

};




///////////////////////////////////////////
// Classes for testing ThdPool
///////////////////////////////////////////
static void MyStarter( void *context )
{
    RefCountedPtr<MyDataClass> data(new MyDataClass);
    ThdPool *pool = new ThdPool( 1, (RefCountedPtr<iRunnable>&)data );

    CHAR_T ch;
    do {
      CIN >> ch;
    } while ( ch != NTEXT('q') );

    data->_s.signal();

    delete pool;
}


static void test_run_once()
{
    ThdPool *pool = new ThdPool();
    RefCountedPtr<RunOnceRunner> runner(new RunOnceRunner);

    pool->run(ThdPool::keOnce, (RefCountedPtr<iRunnable>&)runner);
    runner->_s.wait();
    pool->run(ThdPool::keOnce, (RefCountedPtr<iRunnable>&)runner);
    runner->_s.wait();
    pool->run(ThdPool::keOnce, (RefCountedPtr<iRunnable>&)runner);
    runner->_s.wait();
    pool->run(ThdPool::keOnce, (RefCountedPtr<iRunnable>&)runner);
    runner->_s.wait();
    pool->run(ThdPool::keOnce, (RefCountedPtr<iRunnable>&)runner);
    runner->_s.wait();

    ThdManager::Sleep(1000);

    delete pool;
}


static void test_pool()
{
# if 1
    MyStarter(NULL);
#else
    HANDLE h = (HANDLE)_beginthread( MyStarter, 0, NULL );
    ::WaitForSingleObject( h, INFINITE );
#endif
}

///////////////////////////////////////////
// End classes for testing ThdPool
///////////////////////////////////////////


///////////////////////////////////////////
// Begin classes for testing test_queue
///////////////////////////////////////////
class Objects
{
private:
    static InterLockedLong cnt;

public:
    Objects() :
        i(cnt++)
    {
    }

    bool operator==(const Objects &rhs)
    {
        return i == rhs.i;
    }

public:
    int i;
};

InterLockedLong Objects::cnt;

class QueueRunner : public iRunnable
{
private:
    ThdQueue<Objects>   &_objs;
public:
    QueueRunner(ThdQueue<Objects>& objs) :
        _objs(objs)
    {}

    virtual ~QueueRunner(){}

    unsigned run()
    {
        while ( true )
        {
            Objects o;
            time_t tt = time(NULL);
            enumSyncOpResult result = _objs.remove(o);
            if (result == SYNCOP_SUCCESS)
                COUT << NTEXT("Retrieving: ") << o.i << NTEXT(", thread: ") << SysManager::getCurrentThreadId() 
                     << NTEXT(" at ") << tt << std::endl;

            if ( result == SYNCOP_ABORT || result == SYNCOP_TIMEOUT)
            {
                COUT << NTEXT("returning: ") << result << std::endl;
                return 0;
            }

            ThdManager::Sleep(20);
        }

        return 0;
    }
};


inline bool operator<( const Objects &obj1, const Objects &rhs )
{
    return obj1.i < rhs.i;
}

static void test_queue()
{
    ThdQueue<Objects> myQ(false);
    RefCountedPtr<QueueRunner> data(new QueueRunner(myQ));
    ThdPool *pool = new ThdPool( 5, (RefCountedPtr<iRunnable>&)data );

    long last = 0;
    time_t to = time(NULL) + 5;
    while ( time(NULL) < to )
    {
        Objects o;
        time_t tt = time(NULL);
        COUT << NTEXT("Inserting: ") << o.i << NTEXT(", thread: ") << SysManager::getCurrentThreadId() 
             << NTEXT(" at ") << tt << std::endl;
        myQ.insert(o);
        last = o.i;
//		ThdManager::Sleep(10);
    }

    CHAR_T ch;
    do {
      CIN >> ch;
    } while ( ch != NTEXT('q') );

    COUT << NTEXT("last inserted was ") << last << std::endl;

    myQ.down();
    delete pool;
}

///////////////////////////////////////////
// End classes for testing test_queue
///////////////////////////////////////////


static void test_deque()
{
    ThdDeque<Objects> myQ;
    Objects o;
    myQ.insert(o);
//    myQ.removeItem( o );
    
    Objects o2;
    myQ.remove( o2 );
}


///////////////////////////////////////////
// Begin classes for testing ThdScheduler
///////////////////////////////////////////
class ItemProcessor : public iSchedulable
{
public:
    ItemProcessor(const int id) :
        _id(id),
        _cnt(0),
        _delay(id * 1000) // delay n millis
    {
    }

    ~ItemProcessor()
    {
    }

    std::ostream& debugout(std::ostream &out)
    {
        COUT << NTEXT("id: ") << _id << NTEXT(",\tcount: ") << _cnt << std::endl;		
        return out;
    }

    enumExecute execute( ScheduleTime &outTime )
    {
        enumExecute result = keSchedule;

        {
            ThdAutoCriticalSection lock(gCrit);
            time_t curTime = ::time(NULL);
            COUT << NTEXT("Processing: ") << _id 
                 << NTEXT(", time: ") << curTime 
                 << NTEXT(", thread: ") << SysManager::getCurrentThreadId() 
                 << NTEXT(" count: ") << ++_cnt << std::endl;
        }

        ScheduleTime newTime;
        outTime = newTime + _delay;

        return result;
    }

    int getPriority()
    {
        return 100;
    }

private:
    int _id;
    int _cnt;
    unsigned long _delay;

    static ThdCriticalSection gCrit;
};

ThdCriticalSection ItemProcessor::gCrit;

void test_scheduler()
{
    ThdScheduler sched( 5 );

    RefCountedPtr<iSchedulable> a( new ItemProcessor(1) );
    RefCountedPtr<iSchedulable> b( new ItemProcessor(2) );
    RefCountedPtr<iSchedulable> c( new ItemProcessor(3) );
    RefCountedPtr<iSchedulable> d( new ItemProcessor(4) );
    RefCountedPtr<iSchedulable> e( new ItemProcessor(5) );
    RefCountedPtr<iSchedulable> f( new ItemProcessor(6) );
    RefCountedPtr<iSchedulable> g( new ItemProcessor(7) );
    RefCountedPtr<iSchedulable> h( new ItemProcessor(8) );
    RefCountedPtr<iSchedulable> i( new ItemProcessor(9) );
    RefCountedPtr<iSchedulable> j( new ItemProcessor(10) );

    ScheduleTime now;

    sched.add(a,now);
    now.break_time();
    sched.add(b,now);
    now.break_time();
    sched.add(c,now);
    now.break_time();
    sched.add(d,now);
    now.break_time();
    sched.add(e,now);
    now.break_time();
    sched.add(f,now);
    now.break_time();
    sched.add(g,now);
    now.break_time();
    sched.add(h,now);
    now.break_time();
    sched.add(i,now);
    now.break_time();
    sched.add(j,now);

    sched.signal(a);
    sched.signal(b);

    CHAR_T ch;
    do {
      CIN >> ch;
    } while ( ch != NTEXT('q') );

/*
  while ( true )
  {
  ThdManager::Sleep(100);
  sched.Signal(a);
  ThdManager::Sleep(100);
  sched.Signal(b);
  ThdManager::Sleep(100);
  sched.Signal(c);
  ThdManager::Sleep(100);
  sched.Signal(d);
  ThdManager::Sleep(100);
  sched.Signal(e);
  ThdManager::Sleep(100);
  sched.Signal(f);
  ThdManager::Sleep(100);
  sched.Signal(g);
  ThdManager::Sleep(100);
  sched.Signal(h);
  ThdManager::Sleep(100);
  sched.Signal(i);
  ThdManager::Sleep(100);
  sched.Signal(j);
  }
*/

}


class SharedRWObject
{
public:
    SharedRWObject() : _i(0) {}

    int get()
    {
        if (_lock.acquireReadLock() )
        {
            int res = _i;
            _lock.releaseReadLock();
            return res;
        }
        else
        {
            return -1;
        }
    }

    bool inc()
    {
        if (_lock.acquireWriteLock() )
        {
            _i++;
            _lock.releaseWriteLock();
            return true;
        }
        else
        {
            return false;
        }        
    }

private:
    int _i;
    ThdRWLock _lock;
};


class Reader : public iRunnable
{
public:
    Reader( SharedRWObject& object ): _object(object) {}

    unsigned run()
    {
        enumSyncOpResult res = _s.wait(0);
        while( res == SYNCOP_TIMEOUT )
        {
            int val = _object.get();
            if ( val >= 0 )
            {
                COUT << NTEXT("Thread: ") << SysManager::getCurrentThreadId() 
                     << NTEXT(" - Reader::run: got value: ") << val << std::endl;
            }
            else
            {
                COUT << NTEXT("Thread: ") << SysManager::getCurrentThreadId() 
                     << NTEXT(" - Reader::run: was not able to get the object value") << std::endl;
            }
//            ::Sleep( 5 );
            res = _s.wait(0);
        }
        return 0;
    }

    ThdMutex _l;
    ThdEvent _s;
    SharedRWObject& _object;
};

class Writer : public iRunnable
{
public:
    Writer( SharedRWObject& object ): _object(object) {}

    unsigned run()
    {
        enumSyncOpResult res = _s.wait(0);
        while( res == SYNCOP_TIMEOUT )
        {
            COUT << NTEXT("Thread: ") << SysManager::getCurrentThreadId() 
                 << NTEXT(" - Writer::run: trying to acquire a write lock") << std::endl;
            if ( _object.inc() )
            {
                COUT << NTEXT("Thread: ") << SysManager::getCurrentThreadId() 
                     << NTEXT(" - Writer::run: increment the counter") << std::endl;
            }
            else
            {
                COUT << NTEXT("Thread: ") << SysManager::getCurrentThreadId() 
                     << NTEXT(" - Writer::run: was not able to get the object to increment") << std::endl;
            }
//            ::Sleep( 25 );
            res = _s.wait(0);
        }
        return 0;
    }

    ThdMutex _l;
    ThdEvent _s;
    SharedRWObject& _object;
};


void test_rwlock()
{
    SharedRWObject object;
    ThdPool pool;

    RefCountedPtr<Reader> readers(new Reader(object) );
    pool.add( 5, (RefCountedPtr<iRunnable>&)readers );

    RefCountedPtr<Writer> writer(new Writer(object) );
    pool.add( 1, (RefCountedPtr<iRunnable>&)writer );

    CHAR_T ch;
    do {
      CIN >> ch;
    } while ( ch != NTEXT('q') );

    readers->_s.signal(); 
    writer->_s.signal();
}

///////////////////////////////////////////
// End classes for testing ThdScheduler
///////////////////////////////////////////
