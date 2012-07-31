#ifndef SysObjectFactory_H_LOADED
#define SysObjectFactory_H_LOADED
// [
// $        SysObjectFactory.h
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
//         Contents: SysObjectFactory class
//
//         Purpose: This template class is used to register creators 
//                  of dynamic objects
//                  
//
//         Usage Notes:
//              All classes that wish to be dynamically instantiated should register 
//              a creator function with the global SysObjectFactory<T>.
//              Care must be taken to ensure that the linker does not dead 
//              strip your component.
//                 
// ?
// !        Changes History
//     $Header: $
//         
// ]

#include <map>

#include <cslib.h>

class ThdCriticalSection;

template<class T>
class EXPORT_DLL SysObjectFactory
{
public:
    typedef T* (*NewObjectFunc)();
public:
    /*
      register an object creator with the object factory
      @param tagName name of the tag of the object
    */
    static void registerObject( 
        const String& tagName, 
        NewObjectFunc creator );

    /*
      create a dynamic object
      @param tagName the name of the object to create
      @param the configuration of the object
    */
    static T* createObject( 
        const String& tagName );
    
private:
    typedef typename std::map<String, NewObjectFunc> CreatorMap;
    typedef typename CreatorMap::iterator CreatorMapIterator;
    typedef typename CreatorMap::const_iterator CreatorMapConstIterator;

    static ThdCriticalSection* _guard;
    static CreatorMap* _creators;
};

// SysObjectFactory_H_LOADED
#endif
