//      filetest.cpp
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
//              $Revision: 2.1 $
//              $State: Exp $
//              $Author: sanchezc $
//              $Date: 2002/09/25 15:44:28 $
//
//      Contents:	
//	

#include <stdio.h>
#include <fstream>

#include "file/FileIterator.h"
#if defined(_WINDOWS) || defined(WIN32)
#include "file/FileIterImpWin32.h"
#else
#include "file/FileIterImpUnix.h"
#endif
#include "file/FileAttributes.h"
#include "file/FileUtils.h"

void test_fiterator();
void test_utils();

int main()
{
  OFSTREAM out(NTEXT("debug-log"));
  OFSTREAM err(NTEXT("err.log"));
//    Debug::setOutputStream( out );
//    Debug::setErrStream( err );

    Debug::setCurrentModule( NTEXT("FILETEST") );
    
    test_fiterator();
    test_utils();
    return 0;
}


void
test_utils()
{
    FileAttributes attrs;

#if defined(_WINDOWS) || defined(WIN32)

    bool res = FileUtils::getAttributes( NTEXT("c:\\temp"), attrs );
    res = FileUtils::getAttributes( NTEXT("\\temp\\"), attrs );
    res = FileUtils::getAttributes( NTEXT("c:\\temp\\"), attrs );
    res = FileUtils::mkdirs( NTEXT("c:\\temp\\foldera\\folderb") );
    res = FileUtils::mkdirs( NTEXT("\\temp\\foldera\\folderb\\") );
    res = FileUtils::mkdirs( NTEXT("..\\temp\\foldera\\folderb\\") );
    res = FileUtils::mkdirs( NTEXT(".\\temp\\foldera\\folderb\\") );
#else
    bool res = FileUtils::getAttributes( NTEXT(".."), attrs );
    res = FileUtils::getAttributes( NTEXT("../../"), attrs );
    res = FileUtils::getAttributes( NTEXT("/usr"), attrs );
    res = FileUtils::getAttributes( NTEXT("file"), attrs );
    res = FileUtils::mkdirs( NTEXT("./level1/level2/level3/level4") );
    res = FileUtils::mkdirs( NTEXT("child1/grandchild1") );
    res = FileUtils::mkdirs( NTEXT("../../sibling") );
    res = FileUtils::mkdirs( NTEXT("./child2") );
    res = FileUtils::fileCopy( NTEXT("file"), NTEXT("file2"), true );
    res = FileUtils::fileMove( NTEXT("file2"), NTEXT("file3") );
    res = FileUtils::fileDelete( NTEXT("file3") );
    res = FileUtils::rmdir(NTEXT("./level1"));
    String res1 = FileUtils::resolve(NTEXT("~"));
    res1 = FileUtils::resolve(NTEXT("~/cslib"));
    res1 = FileUtils::resolve(NTEXT("~sanchezc/cslib"));
#endif
}


void
test_fiterator()
{
#if defined(_WINDOWS) || defined(WIN32)

    TCHAR winFolder[ MAX_PATH + 1 ];
    if ( GetWindowsDirectory( winFolder, MAX_PATH ) > 0 )
    {
        FileIterator iter( new FileIterImpWin32 );
        FileAttributes fileAttr;
        String fileName;
        String folder = winFolder;

        folder += NTEXT("\\*.dll");

        for (   FileIterator::Status stat = iter.findFirst( folder, fileName, &fileAttr );
                stat == FileIterator::cFound;
                stat = iter.findNext( fileName, &fileAttr ) )
        {
            COUT << fileName.c_str() << ": " << fileAttr.getFileSize() << std::endl;
        }
    }
#else
    FileIterator iter( new FileIterImpUnix );
    FileAttributes fileAttr;
    String fileName;
    String folder = NTEXT("/etc/*.conf");

    for (   FileIterator::Status stat = iter.findFirst( folder, fileName, &fileAttr );
            stat == FileIterator::cFound;
            stat = iter.findNext( fileName, &fileAttr ) )
    {
        COUT << fileName.c_str() << ": " << fileAttr.getFileSize() << std::endl;
    }

#endif
}
