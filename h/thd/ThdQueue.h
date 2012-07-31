#ifndef ThdQueue_H_LOADED
#define ThdQueue_H_LOADED
// [
// $        ThdQueue.h
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
//         Contents: ThdQueue class
//
//         Purpose:
//			This class is a blocking queue for threaded
//			processing of requests of type T.
//
//         Usage Notes:
//			The caller inserts items T into the queue. 
//			consumers of the queue wait on items to be inserted
//			into the queue. They will block until an item arrives
//			into the queue or until a timeout occurs.
//
//                 
// ?
// !        Changes History
//     $Header: $
//         
// ]

#include <limits.h>
#include <queue>

#include <cslib.h>
#include <thd/ThdCriticalSection.h>
#include <thd/ThdSemaphore.h>
#include <thd/ThdAutoLock.h>
#include <thd/ThdTypes.h>
#include <util/UtilStringUtils.h>

template <class T>
class EXPORT_DLL ThdQueue
{
public:

    // create a ThdQueue
    ThdQueue( const bool isFifo = true ) :
        _isFifo(isFifo),
        _fifoQ(NULL),
        _priorityQ(NULL),
        _down(false)
    {
        if ( _isFifo )
        {
            _fifoQ = new std::queue<T>();
        }
        else
        {
            _priorityQ = new std::priority_queue<T>();
        }
        
        String semName = NTEXT("ThdQueueSem-");
        semName += StringUtils::toString(_gUnnamedInstCtr++);
        _signal.create(semName);
    }

    virtual ~ThdQueue()
    {
        down();
        if ( _isFifo )
        {
            delete _fifoQ;
        }
        else
        {
            delete _priorityQ;
        }
    }
    
    // add an item to the thread queue
    void insert( T &obj )
    {
        ThdAutoCriticalSection lock(_guard);
        if ( !_down)
        {
            if ( _isFifo )
            {
                _fifoQ->push( obj );
            }
            else
            {
                _priorityQ->push( obj );
            }

            _signal.increment();
        }
    }
    
    
    // remove an item from the queue
    enumSyncOpResult remove( T &obj )
    {
        enumSyncOpResult result = SYNCOP_FAILED;

        while( true )
        {
            if ( _signal.decrement() == SYNCOP_SUCCESS )
            {
                ThdAutoCriticalSection lock(_guard);
                
                //	we are guranteed to have something in the queue or
                //	the pool has been shutdown
                if ( !_down )
                {
                    if ( _isFifo )
                    {
                        obj = _fifoQ->front();
                        _fifoQ->pop();
                    }
                    else
                    {
                        obj = _priorityQ->top();
                        _priorityQ->pop();
                    }

                    result = SYNCOP_SUCCESS;
                    break;
                }
                else
                {
                    result = SYNCOP_ABORT;
                    break;
                }
            }
            else
            {
                //	abandoned or timed out
                result = SYNCOP_TIMEOUT;
                break;
            }
        }
        
        return result;
    }

    void down()
    {
        ThdAutoCriticalSection lock(_guard);
        if ( !_down )
        {
            _down = true;
            _signal.abandon();
        }
    }
    
    bool isDown()
    {
        ThdAutoCriticalSection lock(_guard);
        return _down;        
    }
    
    int size()
    {
        ThdAutoCriticalSection lock(_guard);
        if ( !_down )
        {
            if ( _isFifo )
            {
                return _fifoQ->size();
            }
            else
            {
                return _priorityQ->size();
            }
        }
        
        return 0;
    }
    
protected:

private:
    bool _isFifo;
    std::queue<T>	        *_fifoQ;     //	list of requests
    std::priority_queue<T>  *_priorityQ; //
    ThdCriticalSection      _guard;		 //
    ThdSemaphore   	        _signal;     // for anyone waiting on a request
    bool		            _down;

     // Unimplemented standard methods
    ThdQueue(const ThdQueue& );
    ThdQueue& operator=(const ThdQueue&);
};

// ThdQueue_H_LOADED
#endif
