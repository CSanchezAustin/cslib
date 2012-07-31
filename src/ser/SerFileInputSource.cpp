//      SerFileInputSource.cpp
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
// Copyright (c) 1998-2002 Chris Sanchez. All rights reserved.
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
//      RCS:
//                 $Revision:$
//                 $Author:$
//                 $Date:$
//
//      Contents:
//          Provide a memory-based implementation of a serialization source
	
#include <cslib.h>

#include <ser/SerFileInputSource.h>
#include <ser/SerMemInputSource.h>
#include <file/FileUtils.h>

/**
   Initialize with a file path
   @param filePath a valid path name
   @param bufferMem buffer the file into memory
*/
SerFileInputSource::SerFileInputSource(
    const String& filePath, 
    const bool bufferMem) :
    _initted(false),
    _buffered(bufferMem),
    _fileName( filePath ),
    _fileIn(NULL),
    _memIn(NULL)
{
}


SerFileInputSource::~SerFileInputSource()
{
    if ( _fileIn != NULL )
    {
        delete _fileIn;
    }

    if ( _memIn != NULL )
    {
        delete _memIn;
    }
}


/**
   getInputStream
   Returns a new stream suitable for reading in 
   data. This assumes that the source of the stream
   exists.
   @return std::istream reference to an input stream
*/
std::istream& 
SerFileInputSource::getInputStream() 
{
    if ( !_initted )
    {
        if ( _buffered )
        {
            char* buff = NULL;
            size_t buffSize = 0;

            if ( FileUtils::getFile( _fileName, buff, buffSize ) )
            {
                _memIn = new SerMemInputSource( buff, buffSize, true);
                _initted = true;
            }
            else
            {
                _buffered = false;
            }
        }

        if ( !_initted )
        {
#ifdef __GNUG__
            _fileIn = new std::ifstream( _fileName.c_str(), 
                                         std::ios::in | 
                                         std::ios::binary );
#else
            _fileIn = new std::ifstream( _fileName.c_str(), 
                                         std::ios_base::in| 
                                         std::ios_base::binary );
#endif
            _initted = true;
        }    
    }
    
    if ( _buffered )
    {
        return _memIn->getInputStream();
    }
    else
    {
        return *_fileIn;
    }
}
