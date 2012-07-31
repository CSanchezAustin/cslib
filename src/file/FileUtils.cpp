//      FileUtils.cpp
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
//      Contents:	Implementation of file utilities
//	
#include <cslib.h>

#include <stdlib.h>
#include <limits.h>
#include <errno.h>
#include <fstream>

#if defined(_WINDOWS) || defined(WIN32)
#   include <TCHAR.H>
#   include <DIRECT.H>
#else
#   include <stdlib.h>
#   include <pwd.h>
#   include <unistd.h>
#   include <sys/stat.h>
#   include <sys/param.h>
#endif

#include <file/FileUtils.h>
#include <file/FileAttributes.h>
#include <util/UtilStringUtils.h>
#include <util/UtilStringTokenizer.h>
#if defined(_WINDOWS) || defined(WIN32)
#    include <file/FileIterImpWin32.h>
#else
#    include <file/FileIterImpUnix.h>
#endif

// static functions
namespace FileUtils
{
    const int COPY_BUFF_SIZE = 1024;

    String properName(const String& source)
    {
        String result = source;
        StringUtils::trimBack( FileUtils::PATH_SEP_PAT, result );
        return result;
    }

    /**
       Resolve a string with a ~ or ~username
       pattern to a home directory
     */
    String resolveHome(const String& source)
    {
        String workingStr;
        if (source.find(NTEXT("~")) != 0)
        {
            // the source name does not begin with a ~
            return source;
        }

		// assume a default of ~/somepath
        workingStr = FileUtils::properName(
            StringUtils::toString(GETENV(NTEXT("HOME"))));

        StringTokenizer tok(source, FileUtils::PATH_SEP_PAT);
        if ( tok.hasMoreTokens() )
        {
            String firstElement = tok.getNextToken();
            if ( firstElement.length() != 1 )
            {
                // is a ~username since the length of the first element
                // is longer than 1 (len("~")
#ifdef UNIX
                String userNameOnly = firstElement.substr( 1 );
                struct passwd* thePwdRec = ::getpwnam( userNameOnly.c_str() );
                if ( thePwdRec != NULL )
                {
                    workingStr = FileUtils::properName(
                        StringUtils::toString(thePwdRec->pw_dir));
                }
#endif
            }

            // construct the rest of the path
            while( tok.hasMoreTokens() )
            {
                workingStr += FileUtils::PATH_SEP;
                workingStr += tok.getNextToken();
            }

            return workingStr;
        }
        else
        {
            // the source only contained a ~ as the path
            return workingStr;
        }

	return workingStr;
    }
}

bool 
FileUtils::fileMove(
    const String &source, 
    const String &dest)
{
    ASSERT_D( source.length() > 0 );
    ASSERT_D( dest.length() > 0 );

#if defined(_WINDOWS) || defined(WIN32)
    return ::MoveFile( source.c_str(), dest.c_str() ) == TRUE;
#else
    return 
        FileUtils::fileCopy( source, dest, true ) && 
        FileUtils::fileDelete( source );
#endif
}


bool 
FileUtils::fileCopy( 
    const String &source, 
    const String& dest,
    const bool replace )
{
    ASSERT_D( source.length() > 0 );
    ASSERT_D( dest.length() > 0 );

#if defined(_WINDOWS) || defined(WIN32)
    return ::CopyFile( source.c_str(), dest.c_str(), replace ? FALSE : TRUE ) == TRUE;
#else

    // validate the source file 
    FileAttributes attrs;
    bool res = FileUtils::getAttributes( source, attrs );
    if ( !res || attrs.isDirectory() )
    {
        return false;
    }

    // if the destination file exists, replace is false, then we don't 
    // allow the procedure to complete
    FileAttributes destattrs;
    if ( FileUtils::getAttributes( dest, destattrs ) && 
         !destattrs.isDirectory() && 
         (!replace || destattrs.isReadonly()) )
    {
        return false;
    }

#ifdef __GNUG__
            IFSTREAM input( source.c_str(),
                                 std::ios::in |
                                 std::ios::binary );
            OFSTREAM output( dest.c_str(),
                                 std::ios::out |
                                 std::ios::binary );
#else
            IFSTREAM input( source.c_str(),
                                 std::ios_base::in |
                                 std::ios_base::binary );
            OFSTREAM output( dest.c_str(),
                                 std::ios::out |
                                 std::ios::binary );
#endif

    if ( input.good() && output.good() )
    {
        std::auto_ptr<char> buff( new char[COPY_BUFF_SIZE] );
        if ( buff.get() != NULL )
        {
            const char* pBuff = buff.get();
#ifdef __GNUG__
            input.seekg( 0, std::ios::end );
            int numBytesLeft = input.tellg() ;
            input.seekg( 0, std::ios::beg );
#else
            input.seekg( 0, std::ios_base::end );
            int numBytesLeft = input.tellg() ;
            input.seekg( 0, std::ios_base::beg );
#endif

            while ( numBytesLeft > 0 && res )
            {
                int numToRead = (numBytesLeft > COPY_BUFF_SIZE) ? COPY_BUFF_SIZE : numBytesLeft;
                res = ( input.read( (char*)pBuff, numToRead ) && 
                        output.write( pBuff, numToRead ) );
                numBytesLeft -= numToRead;
            }
        }
    
        if ( res )
        {
            res = FileUtils::setAttributes( dest, attrs );
        }
    }
    
    return res;
#endif

}


bool 
FileUtils::getAttributes(
    const String &source, 
    FileAttributes &outAttrs )
{
    bool res = false;
    ASSERT_D( source.length() > 0 );
#if defined(_WINDOWS) || defined(WIN32)

    // get the full path of the input source and get it's attributes
    WIN32_FILE_ATTRIBUTE_DATA attrs;
    TCHAR abspath[_MAX_PATH];
    String properSource = FileUtils::properName( _tfullpath( abspath, source.c_str(), _MAX_PATH ) );
    res = ::GetFileAttributesEx( properSource.c_str(), GetFileExInfoStandard, &attrs) == TRUE;
    if ( res )
    {
        // parse out the file name
        String::size_type pos = properSource.rfind(FileUtils::PATH_SEP);
        if ( pos != String::npos )
        {
            String name = properSource.substr( pos + 1 );
            outAttrs.setFileName( name );
        }
        else
        {
            outAttrs.setFileName( properSource );
        }

        // fill out the size and time structures
        outAttrs.setFileSize( attrs.nFileSizeLow );
        outAttrs.setFileSizeEx( attrs.nFileSizeHigh );

        FILETIME localFileTime;
        SYSTEMTIME sysTime;
        struct tm tmTime;

        if ( ::FileTimeToLocalFileTime( &attrs.ftCreationTime, &localFileTime ) )
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

                outAttrs.setCreateTime( tmTime );
            }
        }

        if ( ::FileTimeToLocalFileTime( &attrs.ftLastWriteTime, &localFileTime ) )
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

                outAttrs.setModifyTime( tmTime );
            }
        }

        // finally, fill out the attributes
        outAttrs.setArchive( ( attrs.dwFileAttributes & FILE_ATTRIBUTE_ARCHIVE) == FILE_ATTRIBUTE_ARCHIVE );
        outAttrs.setCompressed( ( attrs.dwFileAttributes & FILE_ATTRIBUTE_COMPRESSED) == FILE_ATTRIBUTE_COMPRESSED );
        outAttrs.setDirectory( ( attrs.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY );
        outAttrs.setHidden( ( attrs.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN) == FILE_ATTRIBUTE_HIDDEN );
        outAttrs.setOffline( ( attrs.dwFileAttributes & FILE_ATTRIBUTE_OFFLINE) == FILE_ATTRIBUTE_OFFLINE );
        outAttrs.setReadonly( ( attrs.dwFileAttributes & FILE_ATTRIBUTE_READONLY) == FILE_ATTRIBUTE_READONLY );
        outAttrs.setTemp( ( attrs.dwFileAttributes & FILE_ATTRIBUTE_TEMPORARY) == FILE_ATTRIBUTE_TEMPORARY );
        outAttrs.setSystem( ( attrs.dwFileAttributes & FILE_ATTRIBUTE_SYSTEM) == FILE_ATTRIBUTE_SYSTEM );
    }

#else

    String resolvedPath = FileUtils::resolve( source );
    if ( resolvedPath.length() > 0 )
    {
        struct stat buff;
        res =  ::stat( resolvedPath.c_str(), &buff ) == 0;
        if ( res )
        {
            String theFileName;
            if ( StringUtils::splitBack( 
                     FileUtils::PATH_SEP,
                     resolvedPath,
                     NULL, &theFileName ) )
            {
                outAttrs.setFileName( theFileName );
            }
            else
            {
                outAttrs.setFileName( resolvedPath );
            }
            
            outAttrs.setFileSize( buff.st_size );
            outAttrs.setFileSizeEx(0);
            
            struct tm tmTime;
            
            tmTime = *::gmtime( &buff.st_atime );
            outAttrs.setCreateTime( tmTime );
            
            tmTime = *::gmtime( &buff.st_mtime );    
            outAttrs.setModifyTime( tmTime );
            
            outAttrs.setArchive( !S_ISDIR(buff.st_mode) );
            outAttrs.setDirectory( S_ISDIR(buff.st_mode) );
            outAttrs.setHidden( theFileName[0] == NTEXT('.') );
            outAttrs.setReadonly( ( buff.st_mode & S_IWUSR) == 0 );

            outAttrs.setCompressed( false );
            outAttrs.setOffline( false );
            outAttrs.setTemp( false );
            outAttrs.setSystem( false );   
        }    
    }
    
#endif

    return res;

}


bool 
FileUtils::setAttributes( 
    const String &source, 
    const FileAttributes &inAttrs )
{
    // BUGBUG: implement
    return true;
}



bool 
FileUtils::mkdirs( 
    const String &path )
{
    bool res = false;
    ASSERT_D( path.length() > 0 );

#if defined(_WINDOWS) || defined(WIN32)

    TCHAR abspath[_MAX_PATH];
    TCHAR drive[_MAX_DRIVE];
    TCHAR dir[_MAX_DIR];
    TCHAR fname[_MAX_FNAME];
    TCHAR ext[_MAX_EXT];

    _tsplitpath_s( _tfullpath( abspath, path.c_str(), _MAX_PATH ), drive, dir, fname, ext );

    // remember the current drive
    int curDrive = _getdrive();

    // set the current drive
    _chdrive( toupper(drive[0]) - 'A' + 1 );

    // now start parsing out the path and create all directoried in the 
    // heirarchy
    String createPath;
    String dirStr(dir);
    dirStr += fname;
    StringUtils::trimBoth( FileUtils::PATH_SEP_PAT, dirStr );
    StringTokenizer tok(dirStr, FileUtils::PATH_SEP_PAT);
    while( tok.hasMoreTokens() )
    {
        createPath += FileUtils::PATH_SEP;
        createPath += tok.getNextToken();
        res = FileUtils::mkdir( createPath );
        if ( !res )
        {
            break;
        }
    }

    _chdrive( curDrive );

#else

    // the first step is to figure out where the path
    // starts. If the path contains a .. or . prefix, then
    // we have to calculate the start of the creation path
    // if it does not contain such prefix, then we'll just create
    // the directories relative to the current working directory

    String rootPath;
    String parsePath;

    String::size_type startPos = path.find_first_not_of( NTEXT("/.") );
    if ( startPos != 0 ) 
    {
        // there was at least one of these delimiters as a prefix
        String prefixStr( path, 0, startPos );

        rootPath = FileUtils::resolve( prefixStr );
        if ( rootPath.length() > 0 )
        {
            if ( rootPath[ rootPath.length() - 1 ] != NTEXT('/') )
            {
                rootPath += FileUtils::PATH_SEP;
            }
        }

        parsePath = path.substr( startPos );
    }
    else
    {
        // no delimiters, so just parse the path
        parsePath = path;
    }

    StringTokenizer tok( parsePath, FileUtils::PATH_SEP_PAT );
    while( tok.hasMoreTokens() )
    {
        rootPath += tok.getNextToken();
        rootPath += FileUtils::PATH_SEP;
        res = FileUtils::mkdir( rootPath );
        if ( !res )
        {
            break;
        }
    }
#endif

    return res;
}


bool 
FileUtils::mkdir( 
    const String &dirName )
{
    ASSERT_D( dirName.length() > 0 );
    int res = 0;

#if defined(_WINDOWS) || defined(WIN32)
    res = _tmkdir( dirName.c_str() );
#else    
    res = ::mkdir(dirName.c_str(), 0755);
#endif

    return (res == 0) || (res == -1 && errno == EEXIST);
}


String 
FileUtils::resolve( const String& source )
{
    ASSERT_D( source.length() > 0 );
#if defined(_WINDOWS) || defined(WIN32)
    TCHAR abspath[_MAX_PATH];
    return String(_tfullpath( abspath, source.c_str(), _MAX_PATH ));
#else

    char resolvedPath[MAXPATHLEN+1];
    resolvedPath[0] = '\0';

    String workingStr(resolveHome(source));

    ::realpath( workingStr.c_str(), resolvedPath );

    if ( errno == ENOENT )
    {
        return workingStr;
    }
    else
    {
        return String(resolvedPath);
    }
#endif
}

bool 
FileUtils::fileDelete(
    const String &source )
{
    ASSERT_D( source.length() > 0 );
#if defined(_WINDOWS) || defined(WIN32)
    return ::DeleteFile( source.c_str() ) == TRUE;
#else
    return ::remove( source.c_str() ) == 0;
#endif
}


bool 
FileUtils::getFile( 
    const String& fileName, 
    char*& buff, 
    size_t& buffSize )
{
    // initialize the output parameters
    buff = NULL;
    buffSize = 0;
    
    FileAttributes attrs;
    if ( getAttributes( fileName, attrs ) )
    {
        size_t tempBuffSize = (size_t)attrs.getFileSize();
        char* tempBuff = new char[ tempBuffSize ];

#ifdef __GNUG__
        std::ifstream input( fileName.c_str(),
                             std::ios::in |
                             std::ios::binary );
#else
        std::ifstream input( fileName.c_str(),
                             std::ios_base::in |
                             std::ios_base::binary );
#endif

        if ( input.read( tempBuff, tempBuffSize ).good() )
        {
            buff = tempBuff;
            buffSize = tempBuffSize;
        }
        else
        {
            delete [] tempBuff;
        }
    }
    
    return buff != NULL;
}


int FileUtils::rmdir(
    const String &dirName )
{
    int res = 0;
    
    String cleanName = FileUtils::properName( dirName );
    
    { // required because the directory would otherwise be in use
#if defined(_WINDOWS) || defined(WIN32)
        FileIterator it(new FileIterImpWin32);
#else
        FileIterator it(new FileIterImpUnix);
#endif
        String deletePattern = cleanName;
        deletePattern += FileUtils::PATH_SEP;
        deletePattern += FileUtils::ALL_FILES_PAT;

        String dataFileName;

        FileIterator::Status stat = it.findFirst( deletePattern, dataFileName );
        while ( (res == 0) && (stat == FileIterator::cFound) )
        {
            // construct the path to the target
            String targetName = cleanName;
            targetName += FileUtils::PATH_SEP;
            targetName += dataFileName;

            // check to see if the target is a directory
            FileAttributes attrs;
            if ( FileUtils::getAttributes( targetName, attrs ) && 
                 attrs.isDirectory() )
            {
                // if this is a directory, then call rmdir to delete
                res = FileUtils::rmdir( targetName );
            }
            else
            {
                // otherwise delete all the file from this directory
                res = FileUtils::fileDelete( targetName ) ? 0 : -1;
            }

            stat = it.findNext( dataFileName );
        }
    }

#if defined(_WINDOWS) || defined(WIN32)
    res = _trmdir( cleanName.c_str() );
#else
    // now delete the target directory
    res = ::rmdir( cleanName.c_str() );
#endif

    return res;
}

