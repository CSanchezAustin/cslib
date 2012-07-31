//      ThdPool.cpp
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

#include <thd/ThdPool.h>
#include <thd/ThdManager.h>
#include <thd/ThdThread.h>
#include <sys/SysManager.h>
#include <util/UtilStringUtils.h>
#include <util/UtilRefCountedPtrInlines.h>
#include <xml/XmlDomUtils.h>


const String ThdPool::THD_POOL_NAME = NTEXT("thread-pool");
const String ThdPool::THD_MAX_SIZE = NTEXT("max-size");

DECLARE_SYS_COMPONENT(ThdPool)


// nested class for thread entry
ThdPool::PoolEntry::PoolEntry(
            PoolQueue &poolQ) :
	_queue(poolQ)
{
    RefCountedPtr<iRunnable> pqRunner(new PoolQueueRunner(_queue));
    _runner = ThdManager::newWorkerThread( pqRunner, keTPNormal );
}


ThdPool::PoolEntry::~PoolEntry()
{
}


// same as above, but for PoolDeque
ThdPool::PoolDEntry::PoolDEntry(
            PoolDeque& poolQ ) :
    _queue(poolQ)
{
    RefCountedPtr<iRunnable> pqRunner(new PoolDQueueRunner(_queue));
    _runner = ThdManager::newWorkerThread( pqRunner, keTPNormal );
}


ThdPool::PoolDEntry::~PoolDEntry()
{
}


///////////////////////////////////////////////
//	Thread Pool itself
///////////////////////////////////////////////

ThdPool::ThdPool() :
	_down(false)
{
    _maxPoolSize = SysManager::getOptimalThreadCount();
}



ThdPool::ThdPool(
            const int iNumThreads, 
            RefCountedPtr<iRunnable> &runner ) :
            _down(false)
{	
    //	Add to the permanent pool
    _maxPoolSize = SysManager::getOptimalThreadCount();
    add( iNumThreads, runner );
}


ThdPool::~ThdPool()
{
    // set shutdown mode
    _down = true;

    // signal all threads waiting on a queues to return
    _systemQ.down();
    _poolQ.down();
    _poolDQ.down();
    
    // finally wait for all threads to exit by setting the 
    // thread pool vectors to null, erasing their existance
    _system = NULL;
    _pool = NULL;
    _poolD = NULL;
    _permanent = NULL;

    // by the time we reach here, all WAITING threads should
    // have exited.
}


void
ThdPool::add(
            const int iNumThreads, 
            RefCountedPtr<iRunnable> &runner )
{
    // make sure that the permanent thread pool vector 
    // exists by synchronizing on its guard
    {
        ThdAutoCriticalSection guard( _permGuard );
        if ( _permanent == NULL )
        {
            _permanent = new PermanentThreads;
        }
    }

    // create all the threads that the user wants to add
    for ( int i = 0; i < iNumThreads; i++ )
    {
        RefCountedPtr<ThdThread> pThread( ThdManager::newWorkerThread( runner ) );
        if ( pThread != NULL )
        {
            ThdAutoCriticalSection guard( _permGuard );
            _permanent->push_back( pThread );
        }
        else
        {
            break;
        }
    }
}


void 
ThdPool::run( 
            const enumRunMode mode,
            RefCountedPtr<iRunnable> &runner )
{
    if ( mode == keOnce )
    {
        // create the pool if it is not there already
        // and kick it off and running for max pool size
        // threads.
        {
            ThdAutoCriticalSection guard( _poolGuard );
            if ( _pool == NULL )
            {
                _pool = new PoolThreads;
                for (int i = 0; i < _maxPoolSize; i++ )
                {
                    RefCountedPtr<PoolEntry> item( new PoolEntry( _poolQ ) );
                    _pool->push_back( item );
                }
            }
        }
        
        // insert the runner into thework queue for the thread pool
        _poolQ.insert( runner );
    }
    else if ( mode == keImmediate )
    {
        // Create a single instance of the system thread.
        {
            ThdAutoCriticalSection guard( _systemGuard );
            if ( _system == NULL )
            {
                RefCountedPtr<iRunnable> pqRunner(new PoolQueueRunner(_systemQ));
                _system = ThdManager::newWorkerThread( pqRunner, keTPHighest );
            }
        }

        // insert the runner into thework queue for the system thread pool
        _systemQ.insert( runner );
    }
}


void 
ThdPool::enqueue( 
            RefCountedPtr<iRunnable> &runner )
{
    // create the pool if it is not there already
    {
        ThdAutoCriticalSection guard( _poolDGuard );
        if ( _poolD == NULL )
        {
            _poolD = new PoolDThreads;
            for (int i = 0; i < _maxPoolSize; i++ )
            {
                RefCountedPtr<PoolDEntry> item( new PoolDEntry( _poolDQ ) );
                _poolD->push_back( item );
            }
        }
    }

    // add to the list of things to be run
    _poolDQ.insert( runner );
}


bool 
ThdPool::dequeue( 
            RefCountedPtr<iRunnable> &runner )
{
    return _poolDQ.removeItem( runner );
}

/*
    called to initialize the component. DOM_Node is a configuration
    for which the sub-tree represented by getConfigKey will be fetched
    and used to configure this component. The reason that the entire tree
    is used is because there may be some shared configuration data
    that this component may use...
*/
int 
ThdPool::init( 
            const DOMNode* config, 
            RefCountedPtr<SysContext>& ctx )
{
    int result = -1;

    if ( ( config != NULL ) && ( config->getNodeType() == DOMNode::ELEMENT_NODE ) )
    {
        const DOMElement* configElem = (const DOMElement*)config;
        
        // first look for the max size setting
        String val;
        DOMNodeList* nodes = DomUtils::getNodeList( configElem, THD_MAX_SIZE );
        if ( nodes != NULL )
        {
            if ( DomUtils::getNodeValue( (const DOMElement*)nodes->item( 0 ), &val ) )
            {
                _maxPoolSize = StringUtils::toInt( val );
                result = 0;
            }
        }
    }    
    
    return result;
}


/*
    Run
        This method waits to be signalled and then processes work
        from a queue until there is no more work to perform.
*/
unsigned
ThdPool::PoolQueueRunner::run()
{
    RefCountedPtr<iRunnable> item;

    for ( enumSyncOpResult res = _queue.remove( item );
          res == SYNCOP_SUCCESS;
          item = NULL, res = _queue.remove( item ) )
    {
        try
        {
            item->run();
        }
        catch(...)
        {
            CERR << "Uncaught exception in ThdPool::PoolEntry::Runner::Run, threadId: " << SysManager::getCurrentThreadId() << std::endl;
        }
    }
    
    return 0;
}


// same as above but for a PoolDeque
unsigned
ThdPool::PoolDQueueRunner::run()
{
    RefCountedPtr<iRunnable> item;
    
    for ( enumSyncOpResult res = _queue.remove( item );
          res == SYNCOP_SUCCESS;
          item = NULL, res = _queue.remove( item ) )
    {
        try
        {
            item->run();
        }
        catch(...)
        {
            CERR << "Uncaught exception in ThdPool::PoolEntry::Runner::Run, threadId: " << SysManager::getCurrentThreadId() << std::endl;
        }
    }
    
    return 0;
}



int 
ThdPool::postInit( 
    const DOMNode* config, 
    RefCountedPtr<SysContext>& ctx )
{
    return 0;
}
