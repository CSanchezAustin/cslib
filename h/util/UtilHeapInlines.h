#ifndef UtilHeapInlines_H_LOADED
#define UtilHeapInlines_H_LOADED
// [
// $        UtilHeapInlines.h
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
//         Contents: UtilHeapInlines class
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

#include <algorithm>

#ifdef _DEBUG
    #include <iostream>
#endif

#include <util/UtilHeap.h>

const int GROW_SIZE = 3;

template <class Key, class Type, class  TypeCompare, class KeyCompare>
inline Heap<Key, Type, TypeCompare, KeyCompare>::Heap()
{
	_heap.reserve(GROW_SIZE);
}

template <class Key, class Type, class  TypeCompare, class KeyCompare>
inline Heap<Key, Type, TypeCompare, KeyCompare>::Heap(
            const Heap& rhs):
    _heap(rhs._heap), 
    _lookup(rhs._lookup), 
    _less(rhs._less)
{
	_heap.reserve(GROW_SIZE);
}


template <class Key, class Type, class  TypeCompare, class KeyCompare>
inline Heap<Key, Type, TypeCompare, KeyCompare>::~Heap()
{
	_heap.reserve(GROW_SIZE);
}


template <class Key, class Type, class  TypeCompare, class KeyCompare>
inline bool
Heap<Key, Type, TypeCompare, KeyCompare>::
add( 
            const Key &key, 
            const Type &type )
{
//	ASSERT_D( _lookup.find(key) == _lookup.end() );
    if ( _lookup.find(key) == _lookup.end() )
    {
        //  Add the key to the lookup map
        //  and then add the object to the heap
        insert( HeapVectorEntry( key, type ) );
        return true;
    }

    return false;
}

template <class Key, class Type, class  TypeCompare, class KeyCompare>
inline bool 
Heap<Key, Type, TypeCompare, KeyCompare>::
remove( 
            const Key &key )
{
    LookupMapIterator deleteIter = _lookup.find(key);
    if ( deleteIter != _lookup.end() )
    {
        //  Erase the item from the heap and then remove the 
        //  item reference from the lookup map
		ASSERT_D( (*deleteIter).first == key );
		ASSERT_D( (*deleteIter).second->first == key );
        remove( (*deleteIter).second );
        return true;
    }

    return false;
}


template <class Key, class Type, class  TypeCompare, class KeyCompare>
inline Type*
Heap<Key, Type, TypeCompare, KeyCompare>::
find( 
            const Key &key )
{
    Type *result = 0;

    LookupMapIterator iter = _lookup.find(key);
    if ( iter != _lookup.end() )
    {
        return &(*iter).second->second;
    }

    return result;
}

template <class Key, class Type, class  TypeCompare, class KeyCompare>
inline bool
Heap<Key, Type, TypeCompare, KeyCompare>::
exists( 
            const Key &key )
{
    return ( _lookup.find(key) != _lookup.end() );
}


template <class Key, class Type, class  TypeCompare, class KeyCompare>
inline Type* 
Heap<Key, Type, TypeCompare, KeyCompare>::
first()
{
    _iter = _heap.begin();
    return next();
}

template <class Key, class Type, class  TypeCompare, class KeyCompare>
inline Type* 
Heap<Key, Type, TypeCompare, KeyCompare>::
next()
{
    Type* ret = 0;
    if ( _iter != _heap.end() )
    {
        ret = &_iter->second;
        ++_iter;
    }

    return ret;
}


template <class Key, class Type, class  TypeCompare, class KeyCompare>
inline void
Heap<Key, Type, TypeCompare, KeyCompare>::
build()
{
    HeapVectorSizeType sz = _heap.size();
    for ( HeapVectorSizeType i = sz/2; sz >= 1; sz-- )
    {
        heapify( _heap[ i - 1 ].first );            
    }
}


template <class Key, class Type, class  TypeCompare, class KeyCompare>
inline Type& 
Heap<Key, Type, TypeCompare, KeyCompare>::
head()
{
    return _heap.front().second;
}


template <class Key, class Type, class  TypeCompare, class KeyCompare>
inline size_t
Heap<Key, Type, TypeCompare, KeyCompare>::
size() const
{
    return _heap.size();
}


template <class Key, class Type, class  TypeCompare, class KeyCompare>
inline Type 
Heap<Key, Type, TypeCompare, KeyCompare>::
pop()
{
	ASSERT_D(_heap.size() >= 0);
	HeapVectorEntry hp = _heap.front();
    remove( hp.first );
    return hp.second;
}


template <class Key, class Type, class  TypeCompare, class KeyCompare>
inline void
Heap<Key, Type, TypeCompare, KeyCompare>::
push(
            const Key &key, 
            const Type &type )
{
    add( key, type );
}


template <class Key, class Type, class  TypeCompare, class KeyCompare>
inline void
Heap<Key, Type, TypeCompare, KeyCompare>::
heapify( 
            const Key &key )
{
    LookupMapIterator item = _lookup.find( key );
    if ( item != _lookup.end() )
    {
        down( (*item).second );
        up( (*item).second );
#ifdef _DEBUG
		ASSERT_D( check_heap() );
#endif
    }
}


template <class Key, class Type, class  TypeCompare, class KeyCompare>
inline void
Heap<Key, Type, TypeCompare, KeyCompare>::
print()
{
#ifdef _DEBUG
	ASSERT_D( check_heap() );

    HeapVectorIterator pos = _heap.begin();
    COUT << std::endl << NTEXT("Heap") << std::endl;
    while ( pos != _heap.end() )
    {
        COUT << pos->first << NTEXT(":\t") << pos->second << std::endl;
        pos++;
    }

    LookupMapIterator obj = _lookup.begin();
    COUT << "LookupTree" << std::endl;
    while ( obj != _lookup.end() )
    {
      COUT << NTEXT("Iterator: ") << (*obj).first << NTEXT(":\t") << (HeapVectorIterator*) (&(*obj).second) <<
                NTEXT(" ( ") << (*obj).second->first << NTEXT(", ") << (*obj).second->second << NTEXT(" )") << std::endl;
        obj++;
    }

    COUT << std::endl;
#endif
}


template <class Key, class Type, class  TypeCompare, class KeyCompare>
inline void
Heap<Key, Type, TypeCompare, KeyCompare>::
print( 
            OSTREAM& out, 
            OSTREAM& (*print)( OSTREAM&, const Type& ) )
{
#ifdef _DEBUG
	ASSERT_D( check_heap() );

    HeapVectorIterator pos = _heap.begin();
    out << std::endl << NTEXT("Heap") << std::endl;
    while ( pos != _heap.end() )
    {
        print( out, pos->second );
        pos++;
    }

    LookupMapIterator obj = _lookup.begin();
    out << "LookupTree" << std::endl;
    while ( obj != _lookup.end() )
    {
		print( out, (*obj).second->second );
        obj++;
    }

    out << std::endl;
#endif
}


template <class Key, class Type, class  TypeCompare, class KeyCompare>
inline bool
Heap<Key, Type, TypeCompare, KeyCompare>::
check_heap()
{
	if ( _heap.size() > 0 )
	{
		std::vector< Type > testVector;
		HeapVectorIterator it = _heap.begin();
		while ( it != _heap.end() )
		{
			testVector.push_back( it->second );
			it++;
		}

		return std::__is_heap( testVector.begin(), testVector.end(), _less );
	}

	return true;
}


template <class Key, class Type, class  TypeCompare, class KeyCompare>
inline void
Heap<Key, Type, TypeCompare, KeyCompare>::
remove(
            HeapVectorIterator deleteIter )
{
    //  This function takes the last element in the heap and 
    //  moves it to the deleted position
	ASSERT_D( deleteIter < _heap.end() );
    if ( deleteIter < _heap.end() )
    {
        HeapVectorIterator lastElementIter = _heap.end() - 1;
		HeapVectorIterator updateIter = _heap.end();
		HeapVectorEntry hve = *deleteIter;

		//	first update the lookup map and the vector
		//	if the item that we are removing is not 
		//	the last item.
		bool needsHeapify = false;
		if ( deleteIter != lastElementIter )
		{
			_lookup[ lastElementIter->first ] = _lookup[ hve.first ];
			*deleteIter = *lastElementIter;
			updateIter = deleteIter;
			needsHeapify = true;
		}


		//	delete the items from the vector
        _heap.pop_back();
		_lookup.erase( hve.first );

		ASSERT_D( std::find( _heap.begin(), _heap.end(), hve ) == _heap.end() );

		//	adjust the heap if we did not merely delete the last element in the tree
		if ( needsHeapify )
		{
			heapify( updateIter->first );
		}
    }
#ifdef _DEBUG
	ASSERT_D( check_heap() );
#endif
}


template <class Key, class Type, class  TypeCompare, class KeyCompare>
inline void
Heap<Key, Type, TypeCompare, KeyCompare>::
insert( 
            const HeapVectorEntry &item )
{
    HeapVectorSizeType	capacity = _heap.capacity();
    HeapVectorSizeType	size = _heap.size();
	HeapVectorIterator it = _heap.insert( _heap.end(), item );
    _lookup[ item.first ] = it;

    //  If the heap vector was allocated, then we have to iterate
	// through the lookup vector and reassign all the iterator values
	// to point to the proper iterators in the heap
	if ( capacity == size )
    {
	    HeapVectorIterator hoi = _heap.begin();
        while ( hoi != _heap.end() )
        {
            _lookup[ (*hoi).first ] = hoi;
            hoi++;
        }
    }

    up( _lookup[ item.first ] );
#ifdef _DEBUG
	ASSERT_D( check_heap() );
#endif
}


template <class Key, class Type, class  TypeCompare, class KeyCompare>
inline void
Heap<Key, Type, TypeCompare, KeyCompare>::
down( 
            HeapVectorIterator downIter )
{
    //  Reorders from a specific item down
    while ( downIter >= _heap.begin() )
    {
        HeapVectorDifferenceType parentIdx = std::distance( _heap.begin(), downIter );
		HeapVectorDifferenceType heapSize = std::distance( _heap.begin(), _heap.end() );
        HeapVectorDifferenceType pLeftIdx = parentIdx * 2 + 1;
        HeapVectorDifferenceType pRightIdx = pLeftIdx + 1;

		// if the left child is not found, then we are complete, just return
		if ( pLeftIdx >= heapSize )
		{
			break;
		}

		// at this point, we know we have at least a left child
		// so we have to continue with this routine
        HeapVectorIterator pLeft = downIter;
		HeapVectorIterator pRight = _heap.end();

		// set up the left child iterator
        std::advance( pLeft, pLeftIdx - parentIdx);

		// if the right child is within range, then initialize it as well
		if ( pRightIdx < heapSize )
		{
			pRight = pLeft + 1;
		}

		// now start the check to see which is the largest, the left, the right or the current.
        HeapVectorIterator pLargest = downIter;

        if ( ( pLeft < _heap.end() ) && !_less( pLeft->second, downIter->second ) )
        {
            pLargest = pLeft;
        }

        if ( ( pRight < _heap.end() ) && !_less( pRight->second, pLargest->second ) )
        {
            pLargest = pRight;
        }

        if ( pLargest != downIter )
        {
            //  exchange the indicies in the lookup tree and
            //  exchange the nodes.
            exchange( downIter, pLargest );
            downIter = pLargest;
        }
        else
        {
            break;
        }
    }
}


template <class Key, class Type, class  TypeCompare, class KeyCompare>
inline void
Heap<Key, Type, TypeCompare, KeyCompare>::
up( 
            HeapVectorIterator upIter )
{
    while ( upIter > _heap.begin() )
    {
        HeapVectorIterator pParent = _heap.begin();
        HeapVectorDifferenceType dist = std::distance( _heap.begin(), upIter );
        std::advance( pParent, ( ( dist + 1 ) / 2 ) - 1  );

        if ( _less( pParent->second, upIter->second ) )
        {
            //  have to adjust the indicies in the lookup tree.
            exchange( upIter, pParent );
            upIter = pParent;
        }
        else
        {
            break;
        }
    }
}


template <class Key, class Type, class  TypeCompare, class KeyCompare>
inline void
Heap<Key, Type, TypeCompare, KeyCompare>::
exchange( 
            HeapVectorIterator pItemA, 
            HeapVectorIterator pItemB )
{
    LookupMapIterator iterA = _lookup.find( pItemA->first );
    LookupMapIterator iterB = _lookup.find( pItemB->first );

    //  Exchange the indicies in the lookup map first
    HeapVectorIterator oldIndex = (*iterA).second;
    (*iterA).second = (*iterB).second;
    (*iterB).second = oldIndex;

    //  Then exchange the heap items
    HeapVectorEntry temp = *pItemA;
    *pItemA = *pItemB;
    *pItemB = temp;
}

// UtilHeapInlines_H_LOADED
#endif
