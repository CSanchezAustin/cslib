//      ThdScheduler.cpp
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

#include <xercesc/dom/DOMNode.hpp>
#include <xercesc/dom/DOMElement.hpp>
#include <xercesc/dom/DOMNodeList.hpp>

#include <thd/ThdScheduler.h>
#include <thd/ThdThread.h>
#include <thd/ThdManager.h>
#include <thd/ThdAutoLock.h>
#include <thd/ThdPool.h>
#include <util/UtilStringUtils.h>
#include <util/UtilHeapInlines.h>
#include <util/UtilRefCountedPtrInlines.h>
#include <xml/XmlDomUtils.h>

const String ThdScheduler::THD_SCH_NAME = NTEXT("thread-scheduler");
const String ThdScheduler::THD_SCH_THREADS = NTEXT("max-threads");

DECLARE_SYS_COMPONENT(ThdScheduler)

ThdScheduler::ThdScheduler( 
            const int numThreads ) :
            _sig(NULL, false),
            _workers(NULL),
            _scheduler(NULL),
            _down(false)
{
    if ( numThreads > 0 )
    {
        RefCountedPtr<iRunnable> scfn(new MyThdFn( *this, &ThdScheduler::myScheduleFunction ));
        _scheduler = ThdManager::newWorkerThread( scfn );
        
        RefCountedPtr<iRunnable> wfn(new MyThdFn( *this, &ThdScheduler::myWorkerFunction ));
        _workers = new ThdPool( numThreads, wfn );
    }
}


ThdScheduler::~ThdScheduler()
{
    shutdown();

    delete _scheduler;
    delete _workers;
}


void 
ThdScheduler::add( 
            RefCountedPtr<iSchedulable> &item, 
            const ScheduleTime &delay )
{
    ThdAutoCriticalSection lock(_pqGuard);
    if ( !_down )
    {
        ScheduleTime nextTime;
        size_t sz = _pq.size();
        if ( sz > 0 )
        {
            nextTime = _pq.head().second;
        }

        if ( _pq.add( item, HeapEntry( item, delay ) ) )
        {
            // if there wasn't anything on the heap, then force
            // thread to wake up and reschedule
            // if there was something on the heap, then see if 
            // the item that we just added to the heap needs to be 
            // scheduled before the current item

            if ( ( sz == 0 ) ||
                 ( delay.compare( nextTime ) < 0 ) ||
                 ( _pq.head().first == item ) )
            {
                _sig.signal();
            }
        }
    }
}


bool 
ThdScheduler::remove( 
            RefCountedPtr<iSchedulable> &item )
{
    bool result = false;
    ThdAutoCriticalSection lock(_pqGuard);
    if ( !_down )
    {
        bool wasTop = ( _pq.size() >= 1 ) && 
            ( _pq.head().first == item );
        result = _pq.remove( item );
        if ( wasTop )
        {
            // reset the top of the heap for the new item
            _sig.signal();
        }
    }
    
    return result;
}


bool 
ThdScheduler::signal( 
            RefCountedPtr<iSchedulable> &item )
{
    bool result = false;
    ThdAutoCriticalSection lock(_pqGuard);
    if ( !_down )
    {
        HeapEntry* theItem = _pq.find( item );
        if ( theItem != NULL )
        {
            ScheduleTime now;
            theItem->second = now;

            _pq.heapify( item );
            _sig.signal();

            result = true;
        }
    }
    
    return result;
}


// This function is queue worker function
unsigned
ThdScheduler::myWorkerFunction()
{
    ScheduleTime nextTime;
    RefCountedPtr<iSchedulable> item;
    while ( _reqs.remove( item ) == SYNCOP_SUCCESS )
    {
        while ( true )
        {
            enumExecute result = item->execute( nextTime );
            if ( result == keSchedule )
            {
                // the callee wants to be rescheduled
                add( item, nextTime );
                break;
            }
            else if ( result == keNoSchedule )
            {
                // the callee no longer wants to be scheduled or called
                break;
            } // otherwise the caller wants to be executed again...
        }
        item = NULL;
    }
    
    return 0;
}


// MyScheduleFunction
//	This function is the scheduler routine
unsigned
ThdScheduler::myScheduleFunction()
{
    int nextWake = -1;

    while ( true )
    {
        enumSyncOpResult result = _sig.wait(nextWake);
        if ( result == SYNCOP_SUCCESS || result == SYNCOP_TIMEOUT )
        {
            ThdAutoCriticalSection lock(_pqGuard);
            if ( _down )
            {
                break;
            }
            
            // this next loop ensures that all items in the heap
            // that need to be are popped into the service queue
            ScheduleTime now;
            while ( true )
            {
                if ( _pq.size() == 0 ) 
                {
                    // nothing in the queue to process, so cancel the timer
                    nextWake = WAIT_INFINITE;
                    break;
                }
                
                //	We remove items in the heap
                //	until there are no more items that
                //	wish to be executed.
                
                ScheduleTime nextTime = _pq.head().second;
                if ( nextTime <= now )
                {
                    RefCountedPtr<iSchedulable> item = _pq.pop().first;
                    _reqs.insert( item );
                }
                else
                {
                    // no one else wants to be executed, 
                    // to reset the timer and sleep until it pops
                    nextWake = nextTime.difference(now);
                    break;
                }
            }			
        }
        else
        {
            break;
        }
    }
    
    return 0;
}


// Shutdown
//	closes the services provided by the scheduler.
//	This function will block until all threads have
//	exited.
void 
ThdScheduler::shutdown()
{
    ThdAutoCriticalSection lock(_pqGuard);
    _down = true;
    _reqs.down();
    _sig.signal();
}


/**
    called to initialize the component. DOMNode is a configuration
    for which the sub-tree represented by getConfigKey will be fetched
    and used to configure this component. The reason that the entire tree
    is used is because there may be some shared configuration data
    that this component may use...
*/
int 
ThdScheduler::init( 
            const DOMNode* config, 
            RefCountedPtr<SysContext>& ctx )
{
    int result = -1;
    
    if ( ( config != NULL ) && ( config->getNodeType() == DOMNode::ELEMENT_NODE ) )
    {
        const DOMElement* configElem = (const DOMElement*)config;
        
        String val;
        DOMNodeList* nodes = DomUtils::getNodeList( configElem, THD_SCH_THREADS );
        if ( nodes != NULL )
        {
            if ( DomUtils::getNodeValue( (const DOMElement*)nodes->item( 0 ), &val ) )
            {
                // create the threads required
                RefCountedPtr<iRunnable> scfn(new MyThdFn( *this, &ThdScheduler::myScheduleFunction ));
                _scheduler = ThdManager::newWorkerThread( scfn );

                RefCountedPtr<iRunnable> wfn(new MyThdFn( *this, &ThdScheduler::myWorkerFunction ));
                _workers = new ThdPool( StringUtils::toInt( val ), wfn );
        
                result = 0;
            }
        }
    }    
    
    return result;
}


/**
   used to close the system component
*/
int 
ThdScheduler::close() 
{
    shutdown();
    return 0; 
}


int 
ThdScheduler::postInit( 
    const DOMNode* config, 
    RefCountedPtr<SysContext>& ctx )
{
    return 0;
}
