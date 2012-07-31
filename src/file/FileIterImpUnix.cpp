//      FileIterImpUnix.cpp
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
//      RCS:
//                 $Revision: 2.0 $
//                 $State: Exp $
//                 $Author: $
//                 $Date: $
//
//      Contents:	
//	
#include <cslib.h>

#include <time.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <sys/param.h>

#include <file/FileIterImpUnix.h>
#include <file/FileAttributes.h>
#include <file/FileUtils.h>
#include <util/UtilPattern.h>
#include <util/UtilStringUtils.h>


FileIterImpUnix::FileIterImpUnix()
{
    _fd = NULL;
}


FileIterImpUnix::~FileIterImpUnix()
{
    closeFind();
}


FileIterator::Status 
FileIterImpUnix::findFirst(
            const String &findStr,
            FileAttributes &attrs )
{    
    FileIterator::Status status = FileIterator::cFindError;
    
    closeFind();

    // split the filter from the base path
    StringUtils::splitBack(
        FileUtils::PATH_SEP, 
        findStr,
        &_basePath, &_filter);

    // Make sure that the user has not passed us a directory
    FileAttributes sourceAttrs;
    if ( fpattern_isvalid(_filter.c_str()) &&
         FileUtils::getAttributes(_basePath, sourceAttrs) && 
         sourceAttrs.isDirectory() )
    {
        // have the directory, so now start returning the 
        // and a valid starch path - make sure that the
        // path is fully resolved
        char resolvedPath[MAXPATHLEN+1];
        resolvedPath[0] = '\0';
        
        if ( ::realpath(_basePath.c_str(), resolvedPath) != NULL )
        {
            _fd = ::opendir( resolvedPath );
            if ( _fd != NULL )
            {
                status = findNext( attrs );
            }
        }
    }
    
    return status;
}


FileIterator::Status 
FileIterImpUnix::findNext(
            FileAttributes &attrs )
{
    FileIterator::Status status = FileIterator::cFindError;

    while ( _fd != NULL )
    {
        errno = 0;
        struct dirent* ent = ::readdir(_fd);
        if ( ent != NULL )
        {
            // we want to ignore the "." and ".." directories
            if ( (::strcmp( PERIOD.c_str(), ent->d_name) == 0) ||
                 (::strcmp( DOTDOT.c_str(), ent->d_name) == 0) )
            {
                continue;
            }

            // now see if the file should be filtered out
            if ( fpattern_matchn(_filter.c_str(), ent->d_name) == 0 )
            {
                continue;
            }

            status = FileIterator::cFound;

            String fullPath(_basePath);
            fullPath += FileUtils::PATH_SEP;
            fullPath += ent->d_name;

            FileUtils::getAttributes( fullPath, attrs );
        }
        else if ( errno == 0 )
        {
            status = FileIterator::cNoMoreFiles;
        }

        break; // found a file or no more files to be iterated
    }

    return status;
}


void
FileIterImpUnix::closeFind()
{
    if ( _fd != NULL )
    {
        ::closedir( _fd );
        _fd = NULL;
    }
}
