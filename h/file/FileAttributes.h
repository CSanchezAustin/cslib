#ifndef FileAttributes_H_LOADED
#define FileAttributes_H_LOADED
// [
// $        FileAttributes.h
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
//         Contents: FileAttributes class
//
//         Purpose: Provides an interface for getting and setting
//                  file attributes
//
//         Usage Notes:
//
//                 
// ?
// !        Changes History
//     $Header: $
//         
// ]

#include <time.h>

#include <cslib.h>

class EXPORT_DLL FileAttributes
{
    enum Flags {    cArchive    = 0x0001,
                    cCompressed = 0x0002,
                    cDirectory  = 0x0004, 
                    cHidden     = 0x0008,
                    cOffline    = 0x0010, 
                    cReadonly   = 0x0020,
                    cTemporary  = 0x0040, 
                    cSystem     = 0x0080 };

public:

    // create an FileAttributes
    FileAttributes() :
        _attrs(0),_fileSize(0),_fileSizeEx(0)
    {}

    FileAttributes( const FileAttributes &rhs ) :
        _attrs(rhs._attrs),
        _fileCreateTime(rhs._fileCreateTime), 
        _fileModifyTime(rhs._fileModifyTime),
        _fileSize(rhs._fileSize),
        _fileSizeEx(rhs._fileSizeEx),
        _fileName(rhs._fileName)
    {}

    ~FileAttributes() {}

    FileAttributes& operator=(const FileAttributes &rhs)
    {
        if ( this != &rhs )
        {
            _attrs = rhs._attrs;
            _fileSize = rhs._fileSize;
            _fileSizeEx = rhs._fileSizeEx;
            _fileCreateTime = rhs._fileCreateTime;
            _fileModifyTime = rhs._fileModifyTime;
            _fileName = rhs._fileName;
        }

        return *this;
    }

    const String& getFileName() const { return _fileName; }
    void setFileName( const String &name ) { _fileName = name; }

    unsigned long getFileSize() const { return _fileSize; }
    unsigned long setFileSizeEx() const { return _fileSizeEx; }

    void setFileSize( const unsigned long sz ) { _fileSize = sz; }
    void setFileSizeEx( const unsigned long sz ) { _fileSizeEx = sz; }

    struct tm getCreateTime() const { return _fileCreateTime; }
    void setCreateTime( const struct tm &tyme ) { _fileCreateTime = tyme; }

    struct tm getModifyTime() const { return _fileModifyTime; }
    void setModifyTime( const struct tm &tyme ) { _fileModifyTime = tyme; }

    //  Getting and setting attributes    
    bool isArchive() const { return isSet( cArchive ); }
    bool isCompressed() const { return isSet( cCompressed ); }
    bool isDirectory() const { return isSet( cDirectory ); }
    bool isHidden() const { return isSet( cHidden ); }
    bool isOffline() const { return isSet( cOffline ); }
    bool isReadonly() const { return isSet( cReadonly ); }
    bool isTemp() const { return isSet( cTemporary ); }
    bool isSystem() const { return isSet( cSystem ); }

    void setArchive( const bool setit ) { setFlag( setit, cArchive ); }
    void setCompressed(const bool setit ) { setFlag( setit, cCompressed ); }
    void setDirectory(const bool setit ) { setFlag( setit, cDirectory ); }
    void setHidden(const bool setit ) { setFlag( setit, cHidden ); }
    void setOffline(const bool setit ) { setFlag( setit, cOffline ); }
    void setReadonly(const bool setit ) { setFlag( setit, cReadonly ); }
    void setTemp(const bool setit ) { setFlag( setit, cTemporary ); }
    void setSystem(const bool setit ) { setFlag( setit, cSystem ); }

protected:

private:
    bool isSet( const int flag ) const { return ( _attrs & flag ) == flag; }
    void setFlag( const bool setit, const int flag ) { if ( setit ) _attrs |= flag; else _attrs &= ~flag; }

    int         _attrs;
    struct tm  _fileCreateTime;  
    struct tm  _fileModifyTime;  
    unsigned long _fileSize;
    unsigned long _fileSizeEx;
    String _fileName;
};

// FileAttributes_H_LOADED
#endif
