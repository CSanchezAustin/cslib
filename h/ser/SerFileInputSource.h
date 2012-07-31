#ifndef SerFileInputSource_H_LOADED
#define SerFileInputSource_H_LOADED
// [
// $        SerFileInputSource.h
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
//         Contents: SerFileInputSource class
//
//         Purpose:
//
//         Usage Notes:
//          Provide a file-based implementation of a serialization source
//
//                 
// ?
// !        Changes History
//     $Header: $
//         
// ]

#include <fstream>

#include <cslib.h>
#include <ser/iSerInputSource.h>

class SerMemInputSource;


class EXPORT_DLL SerFileInputSource : public iSerInputSource{
public:

    /**
       Initialize with a file path
       @param filePath a valid path name
       @param bufferMem buffer the file into memory
     */
    SerFileInputSource(
        const String& filePath, 
        const bool bufferMem = true);


    virtual ~SerFileInputSource();

    /**
       getInputStream
       Returns a new stream suitable for reading in 
       data. This assumes that the source of the stream
       exists.
       @return std::istream reference to an input stream
     */
    std::istream& getInputStream();

protected:
    bool _initted;
    bool _buffered;
    String _fileName;

    // if no buffering then use the direct strstream
	std::ifstream* _fileIn;

    // otherwise use the memory input source
    SerMemInputSource* _memIn;

private:

    // Unimplemented standard methods
};


// SerFileInputSource_H_LOADED
#endif
