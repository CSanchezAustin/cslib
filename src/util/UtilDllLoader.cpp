//      UtilDllLoader.cpp
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

#include <util/UtilDllLoader.h>
#include <util/UtilStringUtils.h>
#ifdef UNIX
#include <errno.h>
#include <dlfcn.h>
#endif

void* GetCurrentModule();

DllLoader::DllLoader() :
		_dllHandle(NULL),
		_loadErr(0),
		_filePath(EMPTYSTR),
		_fileOptions(0)
{
}


DllLoader::DllLoader(
            const String& inDllName,
            const int inOption ) :
		_dllHandle(NULL),
		_loadErr(0),
		_filePath(inDllName),
		_fileOptions(inOption)
{
	openFile();
}


DllLoader::DllLoader( 
            const DllLoader &rhs ) :
		_dllHandle(NULL),
		_loadErr(0),
		_filePath(EMPTYSTR),
		_fileOptions(0)
{
    *this = rhs;
}


DllLoader::~DllLoader()
{
	if (isValid() )
    {
#if defined(_WINDOWS) || defined(WIN32)
		::FreeLibrary(_dllHandle);
#else
		::dlclose(_dllHandle);
#endif   
		_dllHandle = NULL;
	}    
}


DllLoader&
DllLoader::operator=(
            const DllLoader &rhs )
{
    if ( this != &rhs )
    {
		if ( isValid() )
		{
#if defined(_WINDOWS) || defined(WIN32)
		::FreeLibrary(_dllHandle);
#else
		::dlclose(_dllHandle);
#endif   
		}

		_dllHandle = NULL;
		_loadErr = 0;
		_filePath = rhs._filePath;
		_fileOptions = rhs._fileOptions;

		openFile();
    }

    return *this;
}


// This function retrieves the address of the facility, returns
// NULL if it is not found.
void*
DllLoader::getFacility(
			const String& inFacilityName)
{
    if ( isValid() )
    {
#if defined(_WINDOWS) || defined(WIN32)
		return ::GetProcAddress( _dllHandle, StringUtils::toCString(inFacilityName).get() );
#else
        return ::dlsym( _dllHandle, inFacilityName.c_str() );
#endif
    }

    return NULL;
}

const String 
DllLoader::getString(
			const int stringID)
{
#if defined(_WINDOWS) || defined(WIN32)
	WCHAR * pBuf = NULL;

	int len = 0;
	
	if ( isValid() )
	{
		len = ::LoadStringW( _dllHandle, 
							stringID, 
							reinterpret_cast< LPWSTR >( &pBuf ), 
							0 );
		_loadErr = GetLastError();
	}

	if( len )
		return String( pBuf, pBuf + len );
	else
		return EMPTYSTR;
#else
// TODO: implement for Unix
    return EMPTYSTR;
#endif
}

bool 
DllLoader::isValid() const 
{ 
    return _dllHandle != NULL; 
}


int 
DllLoader::getErrorCondition() const 
{
    return _loadErr; 
}

void
DllLoader::openFile()
{
#if defined(_WINDOWS) || defined(WIN32)
            
	if ( keOpenExecutable == _fileOptions )
	{
		_dllHandle = ::LoadLibrary( _filePath.c_str());
	}
	else 
	{
		_dllHandle = ::LoadLibraryEx(_filePath.c_str(), NULL, _fileOptions);
	}
            
	_loadErr = ::GetLastError();
#else
	if ( keOpenExecutable == _fileOptions )
	{
		_dllHandle = ::dlopen( _filePath.c_str(), RTLD_LAZY | RTLD_GLOBAL );
	}
	else
	{
		_dllHandle = ::dlopen( _filePath.c_str(), RTLD_NOW | RTLD_GLOBAL );
	}
            
	if ( _dllHandle != NULL )
	{
		_loadErr = 0;
	}
	else
	{
	  char* theErr = ::dlerror();
	  COUT << theErr << std::endl;
	  _loadErr = errno; // some generic error that we don't know about
	}
#endif
}

