#ifndef UtilRefCountedPtrInlines_H_LOADED
#define UtilRefCountedPtrInlines_H_LOADED
// [
// $        UtilRefCountedPtrInlines.h
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
//         Contents: RefCountedPtrInlines class
//          RefCountedPtr - implementation simple reference counted pointer.
//
//          The is a non-intrusive implementation that allocates an additional
//          int and pointer for every counted object.
//
//         Purpose:
//
//         Usage Notes:
//
//                 
// ?
// !        Changes History
//     $Header: $
//         
// ]

#include <cslib.h>
#include <util/UtilRefCountedPtr.h>

template <class T>
inline RefCountedPtr<T>::RefCountedPtr(
            T* p) :
    _itsCounter(NULL) 
{
    if (p != NULL)
    {
        _itsCounter = new counter(p);
    }
}

template <class T>
inline RefCountedPtr<T>::~RefCountedPtr()
{
    release();
}

template <class T>
inline RefCountedPtr<T>::RefCountedPtr(
            const RefCountedPtr<T>& r)
{
    acquire(r._itsCounter);
}

template <class T>
inline RefCountedPtr<T>& 
RefCountedPtr<T>::operator=(
            const RefCountedPtr<T>& r)
{
    if (this != &r) 
    {
        release();
        acquire(r._itsCounter);
    }

    return *this;
}

template <class T>
inline RefCountedPtr<T>& 
RefCountedPtr<T>::operator=(
            T* p)
{
    if (p != get())
    {
        release();
        _itsCounter = new counter(p);
    }

    return *this;
}

template <class T>
inline T& 
RefCountedPtr<T>::operator*() const
{
    return *_itsCounter->_ptr;
}

template <class T>
inline T*
RefCountedPtr<T>::operator->() const
{
    return _itsCounter->_ptr;
}

template <class T>
inline T*
RefCountedPtr<T>::get() const
{
    return _itsCounter ? _itsCounter->_ptr : NULL;
}

template <class T>
inline RefCountedPtr<T>::operator T*() const 
{
    return get(); 
}

template <class T>
inline bool 
RefCountedPtr<T>::operator !() const 
{
    return NULL == get(); 
}

template <class T>
inline bool 
RefCountedPtr<T>::operator == (
            const RefCountedPtr<T>& obj2) const
{
	return get() == obj2.get();
}

template <class T>
inline bool 
RefCountedPtr<T>::operator == (
            const T* pObj1)  const
{ 
	return get() == pObj1;
}

template <class T>
inline bool 
RefCountedPtr<T>::operator != (
            const RefCountedPtr<T>& obj2) const
{
	return get() != obj2.get();
}

template <class T>
inline bool 
RefCountedPtr<T>::operator != (
            const T* pObj1) const
{ 
	return pObj1 != get();
}

template <class T>
inline bool 
RefCountedPtr<T>::operator < (
            const RefCountedPtr<T>& obj2) const
{
	return get() < obj2.get();
}

template <class T>
inline bool 
RefCountedPtr<T>::operator < (
            const T* obj2) const
{
	return get() < obj2;
}

template <class T>
inline bool 
RefCountedPtr<T>::operator <= (
            const RefCountedPtr<T>& obj2) const
{
	return get() <= obj2.get();
}

template <class T>
inline bool 
RefCountedPtr<T>::operator <= (
            const T* obj2) const
{
	return get() <= obj2;
}

template <class T>
inline bool 
RefCountedPtr<T>::operator > (
            const RefCountedPtr<T>& obj2) const
{
	return get() > obj2.get();
}

template <class T>
inline bool 
RefCountedPtr<T>::operator > (
            const T* obj2) const
{
	return get() > obj2;
}

template <class T>
inline bool 
RefCountedPtr<T>::operator >= (
            const RefCountedPtr<T>& obj2) const
{
	return get() >= obj2.get();
}

template <class T>
inline bool 
RefCountedPtr<T>::operator >= (
            const T* obj2) const
{
	return get() >= obj2;
}



template <class T>
inline void 
RefCountedPtr<T>::acquire(counter* c)
{ // increment the count
    _itsCounter = c;
    if (c)
    {
        c->_count++;
    }
}

template <class T>
inline void 
RefCountedPtr<T>::release()
{ // decrement the count, delete if it is NULL
    if (_itsCounter) 
    {
        if (--_itsCounter->_count == 0) 
        {
            delete _itsCounter->_ptr;
            delete _itsCounter;
        }

        _itsCounter = NULL;
    }
}

#endif
