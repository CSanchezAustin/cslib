#ifndef SerInputters_H_LOADED
#define SerInputters_H_LOADED
// [
// $        SerInputters.h
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
//         Contents: Inputters for SerUtils
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

#include <iostream>

#include <cslib.h>
#include <pat/PatSerializable.h>

namespace EXPORT_DLL SerUtils
{
    //////////////////////////////////////////////
    //////////////////////////////////////////////
    // helper inputter template classes
    //////////////////////////////////////////////
    //////////////////////////////////////////////

    /**
       __input_native__
       Template class to read in a native data type
       @param in - a stream to read from
       @param data - the data to read to
       @return - the stream
     */
    template <class T>
    class __input_native__
    {
    public:
        std::istream& operator()(
            std::istream& in, 
            T& data) const
        {
            if ( in.good() )
            {
                size_t bytes = sizeof(T);
                in.read( (char*)&data, bytes );
            }
            
            return in;
        }
    };

    /**
       __input_serializable__
       Template class to read in a iSerializable data type
       This inputter is used to input object references 
       derived from the interface
       @param in - a stream to read from
       @param data - the data to read to
       @return - the stream
     */
    template <class T>
    class __input_serializable__
    {
    public:
        std::istream& operator()(
            std::istream& in, 
            T& data) const
        {
            if ( in.good() )
            {
                data.deserialize( in );
            }
            
            return in;
        }
    };


    /**
       __input_serializable_ptr__
       Class to read in a iSerializable* data type
       @param in - a stream to read from
       @param data - the data to read to
       @return - the stream
     */
    template <class T>
    class __input_serializable_ptr__
    {
    public:
        std::istream& operator()(
            std::istream& in, 
            T* data) const
        {
            if ( in.good() )
            {
                data->deserialize( in );
            }
            
            return in;
        }
    };

    /**
       __input_string__
       Template class to read in a string data type
       @param in - a stream to read from
       @param data - the data to read to
       @return - the stream
     */
    template <class T>
    class __input_string__
    {
        typedef typename T::value_type ValueType;
        typedef typename T::size_type SizeType;
        
    public:
        std::istream& operator()(
            std::istream& in, 
            T& data) const
        {            
            if ( in.good() )
            {
                size_t bytes = 0;
                if ( in.read( (char*)&bytes, sizeof(bytes) ).good() && 
                     bytes > 0 )
                {
                    data.resize( bytes + 1 );
                    if ( in.read( (char*)&data[0],
                                  bytes * sizeof(ValueType) ).good() )
                    {
                        data[ bytes ] = (ValueType)0;
                    }
                }
            }
            
            return in;
        }
    };

    typedef __input_native__<double> input_double;
    typedef __input_native__<long> input_long;
    typedef __input_native__<int> input_int;
    typedef __input_native__<short> input_short;
    typedef __input_native__<float> input_float;
    typedef __input_native__<bool> input_bool;

    typedef __input_string__<String> input_String;
    typedef __input_string__<std::string> input_string;
    typedef __input_string__<std::wstring> input_wstring;

}

// SerInputters_H_LOADED
#endif
