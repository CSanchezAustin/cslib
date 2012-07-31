//      FileIterImpWin32.cpp
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
//	
#include <cslib.h>

#include <time.h>

#include <file/FileIterImpWin32.h>
#include <file/FileAttributes.h>

FileIterImpWin32::FileIterImpWin32()
{
    _fh = INVALID_HANDLE_VALUE;
}


FileIterImpWin32::~FileIterImpWin32()
{
    closeFind();
}


FileIterator::Status 
FileIterImpWin32::findFirst(
            const String &findStr, 
            FileAttributes &attrs )
{
    FileIterator::Status status;
    closeFind();

    _fh = ::FindFirstFile( findStr.c_str(), &_fd );
    if ( _fh == INVALID_HANDLE_VALUE )
    {
        status = FileIterator::cFindError;
    }
    else
    {
        status = FileIterator::cFound;
        assignFileAttributes( attrs );
    }

    return status;
}


FileIterator::Status 
FileIterImpWin32::findNext(
            FileAttributes &attrs )
{
    FileIterator::Status status;

    BOOL found = ::FindNextFile( _fh, &_fd );
    if ( !found )
    {
        status = ( ::GetLastError() == ERROR_NO_MORE_FILES ) ? 
            FileIterator::cNoMoreFiles : FileIterator::cFindError ;
    }
    else
    {
        status = FileIterator::cFound;
        assignFileAttributes( attrs );
    }

    return status;
}


void
FileIterImpWin32::closeFind()
{
    if ( _fh != INVALID_HANDLE_VALUE )
    {
        ::FindClose( _fh );
        _fh = INVALID_HANDLE_VALUE;
    }
}


void
FileIterImpWin32::assignFileAttributes(
            FileAttributes &attrs )
{
    attrs.setFileName( String(_fd.cFileName ) );
    attrs.setFileSize( _fd.nFileSizeLow );
    attrs.setFileSizeEx( _fd.nFileSizeHigh );

    FILETIME localFileTime;
    SYSTEMTIME sysTime;
    struct tm tmTime;

    if ( ::FileTimeToLocalFileTime( &_fd.ftCreationTime, &localFileTime ) )
    {
        if ( ::FileTimeToSystemTime( &localFileTime, &sysTime ) )
        {
            tmTime.tm_year= sysTime.wYear - 1900;
            tmTime.tm_mon  = sysTime.wMonth - 1;
            tmTime.tm_wday = sysTime.wDayOfWeek;
            tmTime.tm_mday = sysTime.wDay;
            tmTime.tm_hour = sysTime.wHour;
            tmTime.tm_min = sysTime.wMinute;
            tmTime.tm_sec = sysTime.wSecond;

            attrs.setCreateTime( tmTime );
        }
    }

    if ( ::FileTimeToLocalFileTime( &_fd.ftLastWriteTime, &localFileTime ) )
    {
        if ( ::FileTimeToSystemTime( &localFileTime, &sysTime ) )
        {
            tmTime.tm_year= sysTime.wYear - 1900;
            tmTime.tm_mon  = sysTime.wMonth - 1;
            tmTime.tm_wday = sysTime.wDayOfWeek;
            tmTime.tm_mday = sysTime.wDay;
            tmTime.tm_hour = sysTime.wHour;
            tmTime.tm_min = sysTime.wMinute;
            tmTime.tm_sec = sysTime.wSecond;

            attrs.setModifyTime( tmTime );
        }
    }

    attrs.setArchive( ( _fd.dwFileAttributes & FILE_ATTRIBUTE_ARCHIVE) == FILE_ATTRIBUTE_ARCHIVE );
    attrs.setCompressed( ( _fd.dwFileAttributes & FILE_ATTRIBUTE_COMPRESSED) == FILE_ATTRIBUTE_COMPRESSED );
    attrs.setDirectory( ( _fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY );
    attrs.setHidden( ( _fd.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN) == FILE_ATTRIBUTE_HIDDEN );
    attrs.setOffline( ( _fd.dwFileAttributes & FILE_ATTRIBUTE_OFFLINE) == FILE_ATTRIBUTE_OFFLINE );
    attrs.setReadonly( ( _fd.dwFileAttributes & FILE_ATTRIBUTE_READONLY) == FILE_ATTRIBUTE_READONLY );
    attrs.setTemp( ( _fd.dwFileAttributes & FILE_ATTRIBUTE_TEMPORARY) == FILE_ATTRIBUTE_TEMPORARY );
    attrs.setSystem( ( _fd.dwFileAttributes & FILE_ATTRIBUTE_SYSTEM) == FILE_ATTRIBUTE_SYSTEM );
}
