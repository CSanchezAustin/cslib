#ifndef SerOutputters_H_LOADED
#define SerOutputters_H_LOADED
// [
// $        SerOutputters.h
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
//         Contents: Outputter classes
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
    // helper outputter template classes
    //////////////////////////////////////////////
    //////////////////////////////////////////////

    /**
       __output_native__
       Template class to write out a native data type
       @param out - a stream to write to
       @param data - the data to write
       @return - the stream
     */
    template <class T>
    class __output_native__
    {
    public:
        std::ostream& operator()(
            std::ostream& out, 
            const T& data) const
        {
            if ( out.good() )
            {
                size_t bytes = sizeof(T);
                out.write( (const char*)&data, bytes );
            }
            
            return out;
        }
    };

    /**
       __output_serializable__
       Template class to write out a iSerializable data type
       This outputter is used to output object references 
       derived from the interface
       @param out - a stream to write to
       @param data - the data to write
       @return - the stream
     */
    template <class T>
    class __output_serializable__
    {
    public:
        std::ostream& operator()(
            std::ostream& out, 
            const T& data) const
        {
            if ( out.good() )
            {
                data.serialize( out );
            }
            
            return out;
        }
    };

    /**
       __output_serializable_ptr__
       Class to write out a iSerializable* data type
       This outputter is used to output object pointers
       derived from the interface
       @param out - a stream to write to
       @param data - the data to write
       @return - the stream
     */
    template <class T>
    class __output_serializable_ptr__
    {
    public:
        std::ostream& operator()(
            std::ostream& out, 
            const T* data) const
        {
            if ( out.good() )
            {
                data->serialize( out );
            }
            
            return out;
        }
    };

    /**
       __output_string__
       Template class to write out a string data type
       @param out - a stream to write to
       @param data - the data to write
       @return - the stream
     */
    template <class T>
    class __output_string__
    {
        typedef typename T::value_type ValueType;
        typedef typename T::size_type SizeType;

    public:
        std::ostream& operator()(
            std::ostream& out, 
            const T& data) const
        {            
            if ( out.good() )
            {
                // write out length byte and then the cstring data
                
                size_t bytes = data.length();
                if ( out.write( (const char*)&bytes, sizeof(bytes) ).good() )
                {
                    out.write( (const char*)&data[0],
                               bytes * sizeof(ValueType) );
                }
            }
            
            return out;
        }
    };

    typedef __output_native__<double> output_double;
    typedef __output_native__<long> output_long;
    typedef __output_native__<int> output_int;
    typedef __output_native__<short> output_short;
    typedef __output_native__<float> output_float;
    typedef __output_native__<bool> output_bool;

    typedef __output_string__<String> output_String;
    typedef __output_string__<std::string> output_string;
    typedef __output_string__<std::wstring> output_wstring;
}

// SerOutputters_H_LOADED
#endif

