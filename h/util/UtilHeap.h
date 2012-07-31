#ifndef UtilHeap_H_LOADED
#define UtilHeap_H_LOADED
// [
// $        UtilHeap.h
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
//         Contents: UtilHeap class
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

#include <map>
#include <vector>
#include <functional>
#include <iostream>

#include <cslib.h>

template <  class Key,  // type of the key for the heap
            class Type,  // type of the value for the heap
            class  TypeCompare = std::less<Type>, // comparison function for the type
            class KeyCompare = std::less<Key> > // comparison function for the key
class EXPORT_DLL Heap
{
private:

    typedef typename std::pair< Key, Type > HeapVectorEntry;

    typedef typename std::vector< HeapVectorEntry > HeapVector;
    typedef typename HeapVector::iterator HeapVectorIterator;
    typedef typename HeapVector::size_type HeapVectorSizeType;
    typedef typename HeapVector::difference_type HeapVectorDifferenceType;

    typedef typename std::map< Key, HeapVectorIterator, KeyCompare > LookupMap;
    typedef typename LookupMap::iterator LookupMapIterator;

public:
    // create an Heap
    Heap();
    Heap( const Heap& rhs);
    virtual ~Heap();

    bool add( const Key &key, const Type &type );
    bool remove( const Key &key );
    Type* find( const Key &key );
	bool exists( const Key &key );

    Type* first();
    Type* next();

    size_t size() const;
    void heapify( const Key &key );
    void build();

    Type& head();
    Type pop();
    void push( const Key &key, const Type &type );

    void print( OSTREAM& out, OSTREAM& (*print)( OSTREAM&, const Type& ) );
    void print();
    bool check_heap();

protected:

private:
    void remove( HeapVectorIterator deleteIter );
    void insert( const HeapVectorEntry &item );

    void down( HeapVectorIterator downIter );
    void up( HeapVectorIterator upIter );

    void exchange( 
			HeapVectorIterator pItemA, 
			HeapVectorIterator pItemB );

    HeapVector  _heap;
    TypeCompare _less;
    LookupMap   _lookup;
    HeapVectorIterator _iter;

    // Unimplemented standard methods
    Heap& operator=(const Heap&);
};

// UtilHeap_H_LOADED
#endif
