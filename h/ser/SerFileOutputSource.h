#ifndef SerFileOutputSource_H_LOADED
#define SerFileOutputSource_H_LOADED
// [
// $        SerFileOutputSource.h
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
//         Contents: SerFileOutputSource class
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
#include <ser/iSerOutputSource.h>

class EXPORT_DLL SerFileOutputSource : public iSerOutputSource
{
public:

    /**
       Initialize with a file path
       @param filePath a valid path name
     */
    SerFileOutputSource(const String& filePath);

    virtual ~SerFileOutputSource();

    /**
       getOutputStream
       Returns a new stream suitable for writing out 
       data. Any such existing source will be overwritten
       with any new data output to the stream
       @return std::istream reference to an output stream
     */
	std::ostream& getOutputStream() { return _out; }

protected:

	std::ofstream _out;

private:

    // Unimplemented standard methods
    SerFileOutputSource(const SerFileOutputSource&);
    SerFileOutputSource& operator=(const SerFileOutputSource&);
};


// SerFileOutputSource_H_LOADED
#endif
