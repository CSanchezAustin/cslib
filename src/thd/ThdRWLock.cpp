//      ThdRWLock.cpp
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

#include <thd/ThdRWLock.h>
#include <util/UtilStringUtils.h>

// create an ThdRWLock
ThdRWLock::ThdRWLock() :
	_readCount(0)
{
    String semName = NTEXT("ThdRWLockSem-");
    semName += StringUtils::toString(_gUnnamedInstCtr++);
    _dataLock.create(semName, 1, 1);
}


ThdRWLock::~ThdRWLock()
{
}


/**
	This function ensures that a read lock can be acquired on 
	the object this lock is protecting
	@return true if the read lock was acquired
*/
bool 
ThdRWLock::acquireReadLock()
{
    bool result = false;
    
    // acquire the lock on the mutex that protects the read count
    if ( _readLock.acquire() == SYNCOP_SUCCESS )
    {
        // if this thread is the first reader, then try to 
        // acquire the data lock. Note: readers will have the data lock
        // if the count != 1, in which case this thread does not need 
        // to decrement the data lock. Using a semaphore for the data lock
        // provides the ability for the thread incrementing the data lock to not
        // have to be the thread that decremented the lock.
        if( ++_readCount == 1 )
        {
            result = _dataLock.decrement() == SYNCOP_SUCCESS;
        }
        else
        {
            result = true;
        }
        
        _readLock.release();
    }
    
    return result;
}


/**
   This function ensures that a read lock can be acquired on 
   the object this lock is protecting
   @return true if the read lock was acquired
*/
bool 
ThdRWLock::acquireWriteLock()
{
    return _dataLock.decrement() == SYNCOP_SUCCESS;
}


/**
   these functions release the read/write locks
*/
bool 
ThdRWLock::releaseReadLock()
{
    bool result = false;
    
    // acquire the lock on the mutex that protects the read count
    if ( _readLock.acquire() == SYNCOP_SUCCESS )
    {
        // If the reader count has gone to 0 then we release the data lock
        // which will allow another thread to acquire a read or write lock
        // Once again, the thread that acquired the data lock does not have
        // to be the thread to release it.
        if ( --_readCount == 0 )
        {
            result = _dataLock.increment() != 0;
        }
        else
        {
            result = true;
        }
        
        _readLock.release();
    }
    
    return result;
}


bool 
ThdRWLock::releaseWriteLock()
{
	// release the data lock and return true if the 
	// data lock was not already locked. 
	return _dataLock.increment() != 0;
}
