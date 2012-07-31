#ifndef SerMemOutputSource_H_LOADED
#define SerMemOutputSource_H_LOADED
// [
// $        SerMemOutputSource.h
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
//         Contents: SerMemOutputSource class
//
//         Purpose:
//
//         Usage Notes:
//          Provide a memory-based implementation of a serialization source
//
//                 
// ?
// !        Changes History
//     $Header: $
//         
// ]

#include <strstream>

#include <cslib.h>
#include <ser/iSerOutputSource.h>


class EXPORT_DLL SerMemOutputSource : public iSerOutputSource
{
public:

    /**
       Construct with a buffer that will grow dynamically.
       The buffer is owned by the SerMemOutputSource object
     */
    SerMemOutputSource();


    /**
       Preallocate a buffer of maxBytes in size. The buffer is 
       owned by the SerMemOutputSource object
       @param preAllocSz the size to preallocate
     */
    SerMemOutputSource(const size_t preAllocSz);


    virtual ~SerMemOutputSource();

    /**
       getOutputStream
       Returns a new stream suitable for writing out 
       data. Any such existing source will be overwritten
       with any new data output to the stream
       @return std::ostream reference to an output stream
     */
    std::ostream& getOutputStream() { return _out; }

    /**
       Return the current size of the stream
     */
    virtual size_t getSize();

    /**
       return the start of the buffer
     */
    virtual const char* getBuff();

protected:
    // if the object owns the buffer, then these two
    // data members track the buffer size and pointer
    char*  _buff;
    bool _owns;

    std::ostrstream _out;

private:

    // Unimplemented standard methods
    SerMemOutputSource(const SerMemOutputSource&);
    SerMemOutputSource& operator=(const SerMemOutputSource&);
};


// SerMemOutputSource_H_LOADED
#endif
