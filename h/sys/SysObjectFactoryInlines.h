#ifndef SysObjectFactoryInlines_H_LOADED
#define SysObjectFactoryInlines_H_LOADED
//      SysObjectFactoryInlines.h
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
//              $Revision: 2.0 $
//              $Date: 2002/09/10 18:51:32 $
//              $Author: $
//              $Date: $
//
//      Contents:	This template class is used to register creators 
//                  of dynamic objects
//	


#include <sys/SysObjectFactory.h>
#include <thd/ThdAutoLock.h>
#include <thd/ThdCriticalSection.h>

template <class T>
ThdCriticalSection* SysObjectFactory<T>::_guard = NULL;

template <class T>
typename SysObjectFactory<T>::CreatorMap* SysObjectFactory<T>::_creators = NULL;

/*
    register an object creator with the object factory
    @param tagName name of the tag of the object
*/
template <class T>
inline void
SysObjectFactory<T>::registerObject( 
            const String& tagName, 
            NewObjectFunc creator )
{
    if ( _guard == NULL )
    {
        _creators = new CreatorMap;
        _guard = new ThdCriticalSection;
    }

    ThdAutoCriticalSection lock(*_guard);
    (*_creators)[ tagName ] = creator;
}



    /*
        create a dynamic object
        @param tagName the name of the object to create
        @param the configuration of the object
    */
template <class T>
inline T*
SysObjectFactory<T>::createObject( 
            const String& tagName )
{
    NewObjectFunc callee = NULL;

    if( _guard != NULL)
    {
        ThdAutoCriticalSection lock(*_guard);
        CreatorMapIterator it =  _creators->find( tagName );
        if (it != _creators->end())
        {
            callee = (*it).second;
        }
    }
    else
    {
        CERR << NTEXT("SysObjectFactory<T>::createObject: could not create an object: ")
             << tagName.c_str() << std::endl;
    }

    if ( callee != NULL )
    {
        return callee();
    }

    return NULL;
}

#endif
