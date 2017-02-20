#ifndef SerUtils_H_LOADED
#define SerUtils_H_LOADED
// [
// $        SerUtils.h
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
//         Contents: Serialization utilities
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
    const int VECTOR_SENTINEL = 0xBEBE0100;
    const int MAP_SENTINEL    = 0xBEBE0200;
    const int SET_SENTINEL    = 0xBEBE0300;

    //////////////////////////////////////////////
    // serialization classes for non container objects
    //////////////////////////////////////////////

    /**
       getSize
       Template class to get the number of bytes that
       a call to write will add to the stream
       @param data - a native data type
       @return - the number of bytes
     */
    template <class T, class T_Count>
    size_t getSize(
        const T& data,
        const T_Count counter)
    {
        return counter(data);
    }

    /**
       write
       Template class to write out a native data type
       @param out - a stream to write to
       @param data - the data to write
       @return - the stream
     */
    template <class T, class T_Outputter>
    std::ostream& write(
        std::ostream& out, 
        const T& data,
        const T_Outputter outputter)
    {
        if ( out.good() )
        {
            outputter( out, data );
        }

        return out;
    }

    /**
       read
       Template class to read in a native data type
       @param in - a stream to read from
       @param data - the data to read to
       @return - the stream
     */
    template <class T, class T_Inputter>
    std::istream& read(
        std::istream& in, 
        T& data,
        const T_Inputter inputter)
    {
        if ( in.good() )
        {
            inputter( in, data );
        }

        return in;
    }

    //////////////////////////////////////////////
    // serialization classes for vectors
    //////////////////////////////////////////////

    /**
       getVectorSize
       Calculates the size of bytes required by
       the vector of objects
       @param data - a vector of some objects
       @param counter - a class to perform the count
       @return - the number of bytes
     */
    template <class T, class T_Count>
    size_t getVectorSize(
        const std::vector<T>& data,
        const T_Count counter)
    {
        typedef typename std::vector<T>::size_type SizeType;

        size_t totSize = 0;

        SizeType num = data.size();
        if ( num > 0 )
        {
            totSize += counter( data.begin(), data.end() );
        }

        return 
            sizeof(VECTOR_SENTINEL) +
            sizeof(num) + 
            totSize;
    }

    /**
       writeVector
       Write out a vector of objects
       @param data - a vector of some objects
       @param outputter - a class to perform the output
       @return - the stream
     */
    template <class T, class T_Outputter>
    std::ostream& writeVector(
        std::ostream& out, 
        const std::vector<T>& data,
        const T_Outputter outputter)
    {
        typedef typename std::vector<T>::size_type SizeType;
        typedef typename std::vector<T>::const_iterator ConstIterator;

        if ( out.good() )
        {
            // write a sentinel to the stream
            if ( !out.write( (const char*)&VECTOR_SENTINEL, sizeof(VECTOR_SENTINEL) ).good() )
            {
                return out;
            }
            
            // then write a length byte to the stream
            SizeType num = data.size();
            if ( !out.write( (const char*)&num, sizeof(SizeType) ).good() )
            {
                return out;
            }
            
            // then write the object to the stream
            for ( ConstIterator it = data.begin(), end = data.end() ;
                  it != end ;
                  it++ )
            {
                if ( !outputter( out, (*it) ).good() )
                {
                    break;
                }
            }
        }

        return out;
    }


    /**
       readVector
       Read in  a vector of objects
       @param data - a vector of some objects
       @param inputter - a class to perform the input
       @param erase - overwrite the stream
       @return - the stream
     */
    template <class T, class T_Inputter>
    std::istream& readVector(
        std::istream& in, 
        std::vector<T>& data,
        const T_Inputter inputter,
        const bool erase = true)
    {
        typedef typename std::vector<T>::size_type SizeType;

        if ( in.good() )
        {
            // write a sentinel to the stream
            int sentinel;
            if ( !in.read( (char*)&sentinel, sizeof(sentinel) ).good() )
            {
                return in;
            }
            
            if ( sentinel != VECTOR_SENTINEL )
            {
#ifdef __GNUG__
                in.setstate(std::ios::failbit);
#else
                in.setstate(std::ios_base::failbit);
#endif
                return in;
            }
            
            // then read in the length byte
            SizeType num = 0;
            if ( !in.read( (char*)&num, sizeof(SizeType) ).good() )
            {
                return in;
            }

            SizeType currentSize;
            if ( erase )
            {
                currentSize = 0;
            }
            else
            {
                currentSize = data.size();
            }

            // now reserve the space for the vector
            data.resize( currentSize + num );

            // now read in all the objects of the vector
            for ( SizeType i = 0;
                  i < num ;
                  i++ )
            {
                T &obj = data[ currentSize + i ];
                if ( !inputter( in, obj ).good() )
                {
                    break;
                }
            }
        }

        return in;
    }


    //////////////////////////////////////////////
    // serialization classes for vectors of
    // serilizable object pointers (iSerializable*)
    //////////////////////////////////////////////

    /**
       getVectorPtrSize
       Calculates the size of bytes required by
       the vector of objects
       @param data - a vector of some objects
       @param counter - a class to perform the count
       @return - the number of bytes
     */
    template <class T, class T_Count>
    size_t getVectorPtrSize(
        const std::vector<T*>& data,
        const T_Count counter)
    {
        typedef typename std::vector<T*>::size_type SizeType;

        size_t totSize = 0;

        SizeType num = data.size();
        if ( num > 0 )
        {
            totSize += counter( data.begin(), data.end() );
        }

        return 
            sizeof(VECTOR_SENTINEL) +
            sizeof(num) + 
            totSize;
    }

    /**
       writeVectorPtr
       Write out a vector of objects
       @param data - a vector of some objects
       @param outputter - a class to perform the output
       @return - the stream
     */
    template <class T, class T_Outputter>
    std::ostream& writeVectorPtr(
        std::ostream& out, 
        const std::vector<T*>& data,
        const T_Outputter outputter)
    {
        typedef typename std::vector<T*>::size_type SizeType;
        typedef typename std::vector<T*>::const_iterator ConstIterator;

        if ( out.good() )
        {
            // write a sentinel to the stream
            if ( !out.write( (const char*)&VECTOR_SENTINEL, sizeof(VECTOR_SENTINEL) ).good() )
            {
                return out;
            }
            
            // then write a length byte to the stream
            SizeType num = data.size();
            if ( !out.write( (const char*)&num, sizeof(SizeType) ).good() )
            {
                return out;
            }
            
            // then write the object to the stream
            for ( ConstIterator it = data.begin(), end = data.end() ;
                  it != end ;
                  it++ )
            {
                if ( !outputter( out, (*it) ).good() )
                {
                    break;
                }
            }
        }

        return out;
    }


    /**
       readVectorPtr
       Read in  a vector of objects
       @param data - a vector of some objects
       @param inputter - a class to perform the input
       @param erase - overwrite the stream
       @return - the stream
     */
    template <class T, class T_Inputter>
    std::istream& readVectorPtr(
        std::istream& in, 
        std::vector<T*>& data,
        const T_Inputter inputter,
        const bool erase = true)
    {
        typedef typename std::vector<T*>::size_type SizeType;

        if ( in.good() )
        {
            // write a sentinel to the stream
            int sentinel;
            if ( !in.read( (char*)&sentinel, sizeof(sentinel) ).good() )
            {
                return in;
            }
            
            if ( sentinel != VECTOR_SENTINEL )
            {
#ifdef __GNUG__
                in.setstate(std::ios::failbit);
#else
                in.setstate(std::ios_base::failbit);
#endif
                return in;
            }
            
            // then read in the length byte
            SizeType num = 0;
            if ( !in.read( (char*)&num, sizeof(SizeType) ).good() )
            {
                return in;
            }

            SizeType currentSize;
            if ( erase )
            {
                currentSize = 0;
            }
            else
            {
                currentSize = data.size();
            }

            // now reserve the space for the vector
            data.resize( currentSize + num );

            // now read in all the objects of the vector
            for ( SizeType i = 0;
                  i < num ;
                  i++ )
            {
                data[ currentSize + i ] = new T();
                if ( !inputter( in, data[ currentSize + i ] ).good() )
                {
                    break;
                }
            }
        }

        return in;
    }



    //////////////////////////////////////////////
    // serialization classes for maps
    //////////////////////////////////////////////

    /**
       getMapSize
       Calculates the size of bytes required by
       the map of objects
       @param data - a map of some objects
       @param counter - a class to perform the count
       @return - the number of bytes
     */
    template <class K, class V, class K_Count, class V_Count>
    size_t getMapSize(
        const std::map<K, V>& data,
        const K_Count keyCounter,
        const V_Count valueCounter)
    {
        typedef typename std::map<K, V>::size_type SizeType;
        typedef typename std::map<K, V>::const_iterator ConstIterator;

        size_t totSize = 0;

        SizeType num = data.size();
        for ( ConstIterator it = data.begin(), end = data.end();
              it != end;
              it++ )
        {
            totSize += keyCounter( it->first );
            totSize += valueCounter( it->second );
        }

        return 
            sizeof(MAP_SENTINEL) +
            sizeof(num) + 
            totSize;
    }

    /**
       writeMap
       Write out a map of objects
       @param data - a vector of some objects
       @param outputter - a class to perform the output
       @return - the stream
     */
    template <class K, class V, class K_Outputter, class V_Outputter>
    std::ostream& writeMap(
        std::ostream& out, 
        const std::map<K, V>& data,
        const K_Outputter keyOutputter,
        const V_Outputter valueOutputter)
    {
        typedef typename std::map<K, V>::size_type SizeType;
        typedef typename std::map<K, V>::const_iterator ConstIterator;

        if ( out.good() )
        {
            // write a sentinel to the stream
            if ( !out.write( (const char*)&MAP_SENTINEL, sizeof(MAP_SENTINEL) ).good() )
            {
                return out;
            }
            
            // then write a length byte to the stream
            SizeType num = data.size();
            if ( !out.write( (const char*)&num, sizeof(SizeType) ).good() )
            {
                return out;
            }
            
            // then write the object to the stream
            for ( ConstIterator it = data.begin(), end = data.end() ;
                  it != end ;
                  it++ )
            {
                if ( !keyOutputter( out, it->first ).good() )
                {
                    break;
                }

                if ( !valueOutputter( out, it->second ).good() )
                {
                    break;
                }

            }
        }

        return out;
    }


    /**
       readMap
       Read in  a map of objects
       @param data - a map of some objects
       @param inputter - a class to perform the input
       @param erase - overwrite the stream
       @return - the stream
     */
    template <class K, class V, class K_Inputter, class V_Inputter>
    std::istream& readMap(
        std::istream& in, 
        std::map<K, V>& data,
        const K_Inputter keyInputter,
        const V_Inputter valueInputter,
        const bool erase = true)
    {
        typedef typename std::map<K, V>::size_type SizeType;

        if ( in.good() )
        {
            // write a sentinel to the stream
            int sentinel;
            if ( !in.read( (char*)&sentinel, sizeof(sentinel) ).good() )
            {
                return in;
            }
            
            if ( sentinel != MAP_SENTINEL )
            {
#ifdef __GNUG__
                in.setstate(std::ios::failbit);
#else
                in.setstate(std::ios_base::failbit);
#endif
                return in;
            }
            
            // then read in the length byte
            SizeType num = 0;
            if ( !in.read( (char*)&num, sizeof(SizeType) ).good() )
            {
                return in;
            }

            if ( erase )
            {
                data.clear();
            }

            // now read in all the objects of the vector
            for ( SizeType i = 0;
                  i < num ;
                  i++ )
            {
                // get the key first
                K key;
                if ( !keyInputter( in, key ).good() )
                {
                    break;
                }
                
                // now get the object
                V &obj = data[ key ];
                if ( !valueInputter( in, obj ).good() )
                {
                    break;
                }
            }
        }

        return in;
    }


} // namespace EXPORT_DLL SerUtils

// SerUtils_H_LOADED
#endif
