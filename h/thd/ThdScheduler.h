#ifndef ThdScheduler_H_LOADED
#define ThdScheduler_H_LOADED
// [
// $        ThdScheduler.h
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
//         Contents: ThdScheduler class
//
//         Purpose: Provide a class that can be used to execute any type
//                  of schedulable activity
//
//    Usage Notes:
//     This system component supports the following configuration directives
//     XML Root node: <thread-scheduler>
//                     Name        Type        Default                 Description
//     Config Element: max-threads int         4                       max numer of threads
//
//    Reference schema:
//      <xsd:element name="thread-scheduler">
//  		<xsd:complexType content="elementOnly">
//      		<xsd:element name="max-threads" type="xsd:integer" minOccurs="1" maxOccurs="1"/>
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

#include <functional>
#include <algorithm>
#include <iostream>

#include <cslib.h>
#include <lang/ScheduleTime.h>
#include <pat/PatSchedulable.h>
#include <sys/iSysComponent.h>
#include <sys/SysComponentFactory.h>
#include <thd/ThdTypes.h>
#include <thd/ThdCriticalSection.h>
#include <thd/ThdEvent.h>
#include <thd/ThdQueue.h>
#include <thd/ThdRunner.h>
#include <util/UtilRefCountedPtr.h>
#include <util/UtilStringUtils.h>
#include <util/UtilHeap.h>


class ThdThread;
class iSchedulable;
class ThdPool;


class EXPORT_DLL ThdScheduler : public iSysComponent
{
public:

    // create a ThdScheduler
    ThdScheduler( const int numThreads = 0 );

    virtual ~ThdScheduler();
    
    void add( 
            RefCountedPtr<iSchedulable>& item, 
            const ScheduleTime& delay );

    bool remove( RefCountedPtr<iSchedulable>& item );

    bool signal( RefCountedPtr<iSchedulable>& item );
    
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
    int close();

    /**
        called to shutdown a component. DOMNode is the same configuration
        node that was passed to init. The component should update the any
        data that it wants persisted
    */
    int update( DOMNode* config ) { return 0; }
    const String& getComponentName() { return THD_SCH_NAME; }

    static const String& getRegistryName() { return THD_SCH_NAME; }

protected:

private:
    static const String THD_SCH_NAME;
    static const String THD_SCH_THREADS;

private:
	//	thread entry points
	unsigned myWorkerFunction();
	unsigned myScheduleFunction();

	void shutdown();

	typedef ThdQueue< RefCountedPtr<iSchedulable> > Requests;
	typedef std::pair< RefCountedPtr<iSchedulable>, ScheduleTime > HeapEntry;

	class PriorityQueueFunc : public std::binary_function<HeapEntry, HeapEntry, bool>
	{
        public:
            bool operator() ( const HeapEntry &item1, 
                              const HeapEntry &item2 ) const
            {
				// This function is called to compare two heap items
                // the criteria for the function is to return
                // true if item1 is greater than item2
                // the first check is against the items themselves
                // if they are exactly the same, then we check the 
                // priority of the items as a second check.
                // Note: priority is in ascending order with 0 being the lowest
                if ( item1.second > item2.second )
                    return true;
                else if ( item1.second < item2.second )
                    return false;
                else if ( item1.first->getPriority() < item2.first->getPriority() )
                    return true;
                else
                    return false;
            }
	};

    friend OSTREAM& operator<<( OSTREAM &o, const HeapEntry& entry )
    {
		o << entry.first << NTEXT(" : ") << StringUtils::toString(entry.second);
        return o;
    }

    typedef Heap<RefCountedPtr<iSchedulable>, HeapEntry, PriorityQueueFunc> PriorityQueue;
    typedef ThdRunner<ThdScheduler> MyThdFn;
    
    ThdCriticalSection	_pqGuard;	// protect member data
    PriorityQueue	    _pq;		// this is where all participants are kept.
    ThdEvent       	    _sig;		// the object that wakes up the scheduling thread
    Requests		    _reqs;		// this is where objects that are waiting to be executed
    // are picked up from
    ThdPool*            _workers;	// these are the worker threads
    ThdThread*          _scheduler; // this is the scheduler thread
    
    bool		        _down;
    
    // Unimplemented standard methods
    ThdScheduler(const ThdScheduler&);
    ThdScheduler& operator=(const ThdScheduler&);

    DEFINE_SYS_COMPONENT(ThdScheduler)
};

// ThdScheduler_H_LOADED
#endif
