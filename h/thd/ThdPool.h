#ifndef ThdPool_H_LOADED
#define ThdPool_H_LOADED
// [
// $        ThdPool.h
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
//         Contents: ThdPool class
//
//         Purpose: Provide a pool of threads for to
//				1. Execute a single function
//				2. Queue a long operation (network IO, may cause the pool to grow)
//				3. Queue a short operation (in high priority threads)
//
//    Usage Notes:
//     This system component supports the following configuration directives
//     XML Root node: <thread-pool>
//                     Name        Type        Default                 Description
//     Config Element: size        int         4                       max pool size
//
//    Reference schema:
//      <xsd:element name="thread-pool">
//  		<xsd:complexType content="elementOnly">
//      		<xsd:element name="max-size" type="xsd:integer" minOccurs="1" maxOccurs="1"/>
//			</xsd:complexType>
//		</xsd:element>
//
//    Example:
//      <thread-pool>
//          <max-size>4</max-size>
//       </thread-pool>
//
//                 
// ?
// !        Changes History
//     $Header: $
//         
// ]

#include <vector>

#include <cslib.h>
#include <pat/PatRunnable.h>
#include <sys/iSysComponent.h>
#include <sys/SysComponentFactory.h>
#include <thd/ThdCriticalSection.h>
#include <thd/ThdQueue.h>
#include <thd/ThdDeque.h>
#include <thd/ThdTypes.h>
#include <util/UtilRefCountedPtr.h>

class ThdThread;

class EXPORT_DLL ThdPool : public iSysComponent
{
public:
	enum enumRunMode { keOnce, keImmediate };

public:
    ThdPool();

    ThdPool(  
            const int iNumThreads,
            RefCountedPtr<iRunnable> &runner);

    virtual ~ThdPool();

    void run(
            const enumRunMode mode,
            RefCountedPtr<iRunnable> &runner);
    
    void add( 
            const int iNumThreads,
            RefCountedPtr<iRunnable> &runner);
    
    /**
       Add to the thread pool until someone can handle it
       Note items can be removed from the thread pool by calling
       deque
    */
    void enqueue( RefCountedPtr<iRunnable> &runner );
    
    
    /**
       Remove an item that may be enqueued but not yet executed
    */
    bool dequeue( RefCountedPtr<iRunnable> &runner );
    
    /**
      called to initialize the component. DOMNode is a configuration
      for which the sub-tree represented by getConfigKey will be fetched
      and used to configure this component. The reason that the entire tree
      is used is because there may be some shared configuration data
      that this component may use...
    */
    int init( 
            const DOMNode* config, 
            RefCountedPtr<SysContext>& ctx );
    
    /**
      Called after initialization of the system context. this allows
      system components to utilize other components as necessary
    */
    int postInit( 
            const DOMNode* config, 
            RefCountedPtr<SysContext>& ctx );
    
    /**
       used to close the system component
    */
    int close() { return 0; }
    
    /**
      called to shutdown a component. DOMNode is the same configuration
      node that was passed to init. The component should update the any
      data that it wants persisted
    */
    int update( DOMNode* /* config */ ) { return 0; }
    
    const String& getComponentName() { return THD_POOL_NAME; }
    
    static const String& getRegistryName() { return THD_POOL_NAME; }

private:

protected:

private:
    typedef ThdQueue< RefCountedPtr<iRunnable> > PoolQueue;
    typedef ThdDeque< RefCountedPtr<iRunnable> > PoolDeque;
    
    struct PoolEntry;
    struct PoolDEntry;
    
    class PoolQueueRunner : public iRunnable
    {
    private:
        PoolQueue &_queue;
        
    public:
        PoolQueueRunner( PoolQueue &poolQ ) :
            _queue( poolQ )
        {}
        
        ~PoolQueueRunner() 
        {
        }
        
        unsigned run();
    };

    class PoolDQueueRunner : public iRunnable
    {
    private:
        PoolDeque &_queue;
        
    public:
        PoolDQueueRunner( PoolDeque &poolQ ) :
            _queue( poolQ )
        {}
        
        ~PoolDQueueRunner() 
        {
        }
        
        unsigned run();
    };

    // This is the data structure for a single entry
    //  in the thread pool
    struct PoolEntry
    {
    public:
        RefCountedPtr<ThdThread>    _runner;
        PoolQueue   	            &_queue;
        
    public:
        PoolEntry(
            PoolQueue &poolQ );
        
        ~PoolEntry();
        
    private:
        PoolEntry();
        PoolEntry(const PoolEntry &rhs);
        PoolEntry &operator=(const PoolEntry &rhs);
    };
    
    // Same as above but for a PoolDeque
    //  in the thread pool
    struct PoolDEntry
    {
    public:
        RefCountedPtr<ThdThread>    _runner;
        PoolDeque      	            &_queue;
        
    public:
        PoolDEntry(
            PoolDeque &poolQ );
        
        ~PoolDEntry();
        
    private:
        PoolDEntry();
        PoolDEntry(const PoolDEntry &rhs);
        PoolDEntry&operator=(const PoolDEntry &rhs);
    };


    typedef std::vector< RefCountedPtr< PoolEntry > > PoolThreads;
    typedef std::vector< RefCountedPtr< PoolDEntry > > PoolDThreads;
    
    typedef std::vector< RefCountedPtr<ThdThread> > PermanentThreads;
    
    //  This thread runs at a high priority. Items queued to this thread
    //  will run immediately and preempt all other threads. Make sure
    //  that work done under this thread is very short and efficient.
    RefCountedPtr<ThdThread>	_system;		//	a high priority thread
    PoolQueue			        _systemQ;		//	queue of requests
    ThdCriticalSection	        _systemGuard;
    
    //  These threads are static. Items added here merely
    //  run and return on their own. The workers are not controlled
    //  or managed by the thread pool. This is the simplist most
    //  common use of the thread pool
    RefCountedPtr<PermanentThreads> _permanent;		//	permanent pool of threads
    ThdCriticalSection	            _permGuard;
    
    //  This is the dynamic pool. Threads created under this pool are 
    // created on demand as conditions require. However, in order to
    //  minimize the impact on the operating system, in terms of
    //  context switches ans such, no more than 2*number of processors
    //  +1 threads will be created
    RefCountedPtr<PoolThreads>  _pool;			//	pool of workers
    PoolQueue			        _poolQ;			//	queue of requests
    ThdCriticalSection	        _poolGuard;
    

    // This is the same as the regular pool
    RefCountedPtr<PoolDThreads> _poolD;			//	pool of workers
    PoolDeque			        _poolDQ;		//	queue of requests
    ThdCriticalSection	        _poolDGuard;

    int     _maxPoolSize;
    bool	_down;

	friend struct PoolEntry;

    // Unimplemented standard methods
    ThdPool(const ThdPool& );
    ThdPool& operator=(const ThdPool&);

private:
    static const String THD_POOL_NAME;
    static const String THD_MAX_SIZE;

    DEFINE_SYS_COMPONENT(ThdPool)
};

// ThdPool_H_LOADED
#endif
