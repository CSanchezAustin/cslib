#ifndef InterLockedLong_H_LOADED
#define InterLockedLong_H_LOADED
// [
// $        InterLockedLong.h
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
//         Contents: InterLockedLong class
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

class EXPORT_DLL InterLockedLong
{
public:

    // create an InterLockedLong
    InterLockedLong() : _lValue(0){}

    InterLockedLong( const InterLockedLong &rhs ) : _lValue(0)
    {
        const long val = rhs;
        _AtomicSwap( &_lValue, val );
    }

    InterLockedLong( const long rhs ) : _lValue(0)
    {
        _AtomicSwap( &_lValue, rhs );
    }

    ~InterLockedLong(){}

    bool operator==( const InterLockedLong &rhs )
    {
        long left = *this;
        long right = rhs;
        return left == right;
    }

    bool operator==( const long rhs )
    {
        long left = *this;
        return left == rhs;
    }

    InterLockedLong& operator=( const InterLockedLong &rhs )
    {
        if ( this != &rhs )
        {
            _AtomicSwap( &_lValue, rhs._lValue );
        }
        return *this;
    }

    InterLockedLong& operator=( const long rhs )
    {
        _AtomicSwap( &_lValue, rhs );
        return *this;
    }

    operator long() const
    {
        const long zero = 0;
        return _AtomicExchangeAdd( &_lValue, zero );        
    }

    long operator++()
    {
        return _AtomicIncrement( &_lValue );
    }

    long operator++( int )
    {
        const long one = 1;
        return _AtomicExchangeAdd( &_lValue, one );
    }

    long operator--()
    {
        return _AtomicDecrement( &_lValue );
    }

    long operator--( int )
    {
        const long negativeOne= -1;
        return _AtomicExchangeAdd( &_lValue, negativeOne );
    }

    const InterLockedLong& operator+=( const InterLockedLong &rhs )
    {
        _AtomicExchangeAdd( &_lValue, rhs._lValue );
        return *this;
    }

    const InterLockedLong& operator+=( const long rhs )
    {
        _AtomicExchangeAdd( &_lValue, rhs );
        return *this;
    }

    friend InterLockedLong operator+( const InterLockedLong &rhs, const InterLockedLong &val )
    {
        long lVal = val;
        long lResult = _AtomicExchangeAdd( &rhs._lValue, lVal );
        return InterLockedLong( lResult + lVal );
    }

    friend InterLockedLong operator+( const long rhs, const InterLockedLong &val )
    {
        long lResult = _AtomicExchangeAdd( &val._lValue, rhs );
        return InterLockedLong( lResult + rhs );
    }

    friend InterLockedLong operator+( const InterLockedLong &rhs, const long val )
    {
        long lResult = _AtomicExchangeAdd( &rhs._lValue, val );
        return InterLockedLong( lResult + val );
    }

    friend InterLockedLong operator-( const InterLockedLong &rhs, const InterLockedLong &val )
    {
        long lVal = val;
        long lResult = _AtomicExchangeAdd( &rhs._lValue, -lVal );
        return InterLockedLong( lResult - lVal );
    }

    friend InterLockedLong operator-( const long rhs, const InterLockedLong &val )
    {
        long lResult = _AtomicExchangeAdd( &val._lValue, -rhs );
        return InterLockedLong( lResult - rhs );
    }

    friend InterLockedLong operator-( const InterLockedLong &rhs, const long val )
    {
        long lResult = _AtomicExchangeAdd( &rhs._lValue, -val );
        return InterLockedLong( lResult - val );
    }


protected:

private:

    mutable long _lValue;
};

// InterLockedLong_H_LOADED
#endif
