#ifndef PatSerializable_H_LOADED
#define PatSerializable_H_LOADED
// [
// $        PatSerializable.h
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
//         Contents: PatSerializable class
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


class EXPORT_DLL iSerializable
{
public:
    virtual ~iSerializable(){}

    /**
       return the total size that the stream currently occupies.
       i.e. if serialize were to be called, this would be the size of the 
       output stream
       @return the size the stream will be when streammed out
     */
    virtual size_t size() const = 0;

    /**
       serialize
       write out the object to the stream
       @param a stream to write to
       @return the same as the input stream
     */
	virtual std::ostream& serialize(std::ostream& out) const = 0;

    /**
       deserialize
       read from the stream into the object
       @param a stream to read from
       @return the same as the input stream
     */
    virtual std::istream& deserialize(std::istream& in) = 0;

    /**
       lock
       Lock the stream for exclusive access
     */
    virtual bool lock() = 0;

    /**
       unlock
       Release exclusive access from the stream
     */
    virtual bool unlock() = 0;

protected:

private:

    // Unimplemented standard methods
};


/**
   StSerializableLocker
   A class to lock a serializable object
 */
class StSerializableLocker
{
public:
    StSerializableLocker(iSerializable& theObject) :
        _serObject(theObject)
    {
        _serObject.lock();
    }

    ~StSerializableLocker()
    {
        _serObject.unlock();
    }

private:
    iSerializable& _serObject;
};

// PatSerializable_H_LOADED
#endif
