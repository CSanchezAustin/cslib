#ifndef fwkConduit_H_LOADED
#define fwkConduit_H_LOADED
// [
// $        fwkConduit.h
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
// Copyright © CALEB Technologies Corp. 2001
// This software is protected under the copyright laws as an 
// unpublished work which was created in the year(s) indicated above,
// and includes information which is confidential and proprietary 
// to CALEB Technologies Corp.  All Rights Reserved.
//----------------------------------------------------------------------------
// Copyright (c) 2001 Chris Sanchez. All rights reserved.
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
//         Contents: fwkConduit class
//
//         Purpose: This class is an internal communications mechanism
//
//         Usage Notes:
//
//                 
// ?
// !        Changes History
//     $Header: $
//         
// ]

#include <algorithm>
#include <vector>

#include <cslib.h>
#include <fwk/ifwkConduitListener.h>


template <class T>
class EXPORT_DLL Conduit
{
public:
    Conduit()
    {
    }

    virtual ~Conduit()
    {
    }

    /**
        Send an event to all listeners of the conduit
    */
    void notify( const T evtType, const void* data )
    {
        for ( ConstIterator it = _listeners.begin();
              it != _listeners.end() ; it++ )
        {
            (*it)->doNotify( evtType, data );
        }  
    }

    /**
    */
    void addListener( iConduitListener<T>* listener )
    {
        _listeners.push_back( listener );
    }


    /**
    */
    void removeListener( iConduitListener<T>* listener )
    {
        Iterator it = std::find( _listeners.begin(), _listeners.end(), listener );
        if ( it != _listeners.end() )
        {
            _listeners.erase( it );
        }
        
//        std::remove( _listeners.begin(), _listeners.end(), listener );
    }

private:

private:
    typedef typename std::vector< iConduitListener<T>* > ListenerMap;
    typedef typename ListenerMap::const_iterator ConstIterator;
    typedef typename ListenerMap::iterator Iterator;

    ListenerMap _listeners;
};


// fwkConduit_H_LOADED
#endif
