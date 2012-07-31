#ifndef SerCounters_H_LOADED
#define SerCounters_H_LOADED
// [
// $        SerCounters.h
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
//         Contents: counter classes for SerUtils
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

#include <vector>
#include <map>
#include <iostream>

#include <cslib.h>
#include <pat/PatSerializable.h>

namespace EXPORT_DLL SerUtils
{
    //////////////////////////////////////////////
    // helper counter template classes
    //////////////////////////////////////////////

    template <class T>
    class __counter_native__
    {
    public:
        size_t operator()( const T& data ) const
        {
            return sizeof(T);
        }
    };


    template <class T>
    class __counter_string__
    {
    public:
        
    public:
        size_t operator()( const T& data ) const
        {
            typedef typename T::value_type ValueType;
            typedef typename T::size_type SizeType;

            SizeType bytes = data.length();

            return 
                sizeof(SizeType) + 
                (bytes * sizeof(ValueType));
        }
    };

    /**
       template to count native objects in a vector
     */
    template <class T>
    class __vector_counter_native__
    {
    public:
        typedef typename std::vector<T>::size_type SizeType;
        typedef typename std::vector<T>::const_iterator ConstIterator;
        typedef typename std::vector<T>::difference_type DifferenceType;
        
    public:
        size_t operator()( const ConstIterator& beg,  
                           const ConstIterator& end ) const
        {
            // all native objects take up the same amount
            // of memory
            DifferenceType diff=
                std::distance( beg, end );

            return diff * sizeof(T);
        }
    };


    /**
       class  to count iSerializable*  in a vector
     */
    template <class T>
    class __vector_counter_serializable_ptr__
    {
    public:
        typedef typename std::vector<T*>::size_type SizeType;
        typedef typename std::vector<T*>::const_iterator ConstIterator;
        typedef typename std::vector<T*>::difference_type DifferenceType;

    public:
        size_t operator()( ConstIterator beg,  
                           ConstIterator end ) const
        {
            size_t total = 0;

            for ( ConstIterator cur = beg;
                  cur != end; cur++ )
            {
                total += (*cur)->size();
            }

            return total;
        }
    };


    /**
       template to count objects derived from iSerializable in a vector
     */
    template <class T>
    class __vector_counter_serializable__
    {
    public:
        typedef typename std::vector<T>::size_type SizeType;
        typedef typename std::vector<T>::const_iterator ConstIterator;
        typedef typename std::vector<T>::difference_type DifferenceType;

    public:
        size_t operator()( const ConstIterator& beg,  
                           const ConstIterator& end ) const
        {
            size_t total = 0;

            for ( ConstIterator cur = beg;
                  cur != end; cur++ )
            {
                total += cur->size();
            }

            return total;
        }
    };

    /**
       template to count strings  in a vector
     */
    template <class T>
    class __vector_counter_string__
    {
    public:
        typedef typename std::vector<T>::size_type SizeType;
        typedef typename std::vector<T>::value_type ValueType;
        typedef typename std::vector<T>::const_iterator ConstIterator;
        typedef typename std::vector<T>::difference_type DifferenceType;

    public:
        size_t operator()( const ConstIterator& beg,  
                           const ConstIterator& end ) const
        {
            size_t total = 0;

            DifferenceType diff=
                std::distance( beg, end );

            for ( ConstIterator cur = beg;
                  cur != end; cur++ )
            {
                SizeType bytes = cur->length();
                total +=
                    sizeof(SizeType) + 
                    (bytes * sizeof(ValueType));
            }

            return total;
        }
    };
    
    // helper counters
    typedef __counter_native__<double> count_double;
    typedef __counter_native__<long> count_long;
    typedef __counter_native__<int> count_int;
    typedef __counter_native__<short> count_short;
    typedef __counter_native__<float> count_float;
    typedef __counter_native__<bool> count_bool;

    typedef __counter_string__<String> count_String;
    typedef __counter_string__<std::string> count_string;
    typedef __counter_string__<std::wstring> count_wstring;

    typedef __vector_counter_native__<double> count_double_v;
    typedef __vector_counter_native__<long> count_long_v;
    typedef __vector_counter_native__<int> count_int_v;
    typedef __vector_counter_native__<short> count_short_v;
    typedef __vector_counter_native__<float> count_float_v;
    typedef __vector_counter_native__<bool> count_bool_v;

    typedef __vector_counter_string__<String> count_String_v;
    typedef __vector_counter_string__<std::string> count_string_v;
    typedef __vector_counter_string__<std::wstring> count_wstring_v;
}

// SerCounters_H_LOADED
#endif

