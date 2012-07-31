#ifndef SerStdLib_H_LOADED
#define SerStdLib_H_LOADED
// [
// $        SerStdLib.h
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
//         Contents: standard serialization library routines
//
//         Purpose: macros to make the underlying serialization transparent
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

#include <ser/SerInputters.h>
#include <ser/SerOutputters.h>
#include <ser/SerCounters.h>
#include <ser/SerUtils.h>


////////////////////////////////////////////////////////////////////////////////////
// Counter functions
////////////////////////////////////////////////////////////////////////////////////


///////////////////////////////////
// Native counters
///////////////////////////////////
#define SerSizeDouble(val) SerUtils::getSize((val), SerUtils::count_double())
#define SerSizeLong(val) SerUtils::getSize((val), SerUtils::count_long())
#define SerSizeInt(val) SerUtils::getSize((val), SerUtils::count_int())
#define SerSizeShort(val) SerUtils::getSize((val), SerUtils::count_short())
#define SerSizeFloat(val) SerUtils::getSize((val), SerUtils::count_float())
#define SerSizeBool(val) SerUtils::getSize((val), SerUtils::count_bool())

#define SerSizeString(val) SerUtils::getSize((val), SerUtils::count_String())
#define SerSizestring(val) SerUtils::getSize((val), SerUtils::count_string())
#define SerSizewstring(val) SerUtils::getSize((val), SerUtils::count_wstring())

///////////////////////////////////
// Vector counters for native types
///////////////////////////////////
#define SerSizeVectorDouble(val) SerUtils::getVectorSize((val), SerUtils::count_double_v())
#define SerSizeVectorLong(val) SerUtils::getVectorSize((val), SerUtils::count_long_v())
#define SerSizeVectorInt(val) SerUtils::getVectorSize((val), SerUtils::count_int_v())
#define SerSizeVectorShort(val) SerUtils::getVectorSize((val), SerUtils::count_short_v())
#define SerSizeVectorFloat(val) SerUtils::getVectorSize((val), SerUtils::count_float_v())
#define SerSizeVectorBool(val) SerUtils::getVectorSize((val), SerUtils::count_bool_v())

#define SerSizeVectorString(val) SerUtils::getVectorSize((val), SerUtils::count_String_v())
#define SerSizeVectorstring(val) SerUtils::getVectorSize((val), SerUtils::count_string_v())
#define SerSizeVectorwstring(val) SerUtils::getVectorSize((val), SerUtils::count_wstring_v())

///////////////////////////////////
// Map counters for native types
///////////////////////////////////

// cslib String key
#define SerSizeMapStringString(val) SerUtils::getMapSize((val), SerUtils::count_String(), SerUtils::count_String())
#define SerSizeMapStringDouble(val) SerUtils::getMapSize((val), SerUtils::count_String(), SerUtils::count_double())
#define SerSizeMapStringLong(val) SerUtils::getMapSize((val), SerUtils::count_String(), SerUtils::count_long())
#define SerSizeMapStringInt(val) SerUtils::getMapSize((val), SerUtils::count_String(), SerUtils::count_int())
#define SerSizeMapStringShort(val) SerUtils::getMapSize((val), SerUtils::count_String(), SerUtils::count_short())
#define SerSizeMapStringFloat(val) SerUtils::getMapSize((val), SerUtils::count_String(), SerUtils::count_float())
#define SerSizeMapStringBool(val) SerUtils::getMapSize((val), SerUtils::count_String(), SerUtils::count_bool())
#define SerSizeMapStringBool(val) SerUtils::getMapSize((val), SerUtils::count_String(), SerUtils::count_bool())

// std::string key
#define SerSizeMapstringstring(val) SerUtils::getMapSize((val), SerUtils::count_string(), SerUtils::count_string())
#define SerSizeMapstringDouble(val) SerUtils::getMapSize((val), SerUtils::count_string(), SerUtils::count_double())
#define SerSizeMapstringLong(val) SerUtils::getMapSize((val), SerUtils::count_string(), SerUtils::count_long())
#define SerSizeMapstringInt(val) SerUtils::getMapSize((val), SerUtils::count_string(), SerUtils::count_int())
#define SerSizeMapstringShort(val) SerUtils::getMapSize((val), SerUtils::count_string(), SerUtils::count_short())
#define SerSizeMapstringFloat(val) SerUtils::getMapSize((val), SerUtils::count_string(), SerUtils::count_float())
#define SerSizeMapstringBool(val) SerUtils::getMapSize((val), SerUtils::count_string(), SerUtils::count_bool())
#define SerSizeMapstringBool(val) SerUtils::getMapSize((val), SerUtils::count_string(), SerUtils::count_bool())

// std::wstring key
#define SerSizeMapwstringwstring(val) SerUtils::getMapSize((val), SerUtils::count_wstring(), SerUtils::count_wstring())
#define SerSizeMapwstringDouble(val) SerUtils::getMapSize((val), SerUtils::count_wstring(), SerUtils::count_double())
#define SerSizeMapwstringLong(val) SerUtils::getMapSize((val), SerUtils::count_wstring(), SerUtils::count_long())
#define SerSizeMapwstringInt(val) SerUtils::getMapSize((val), SerUtils::count_wstring(), SerUtils::count_int())
#define SerSizeMapwstringShort(val) SerUtils::getMapSize((val),SerUtils::count_wstring(), SerUtils::count_short())
#define SerSizeMapwstringFloat(val) SerUtils::getMapSize((val),SerUtils::count_wstring(), SerUtils::count_float())
#define SerSizeMapwstringBool(val) SerUtils::getMapSize((val), SerUtils::count_wstring(), SerUtils::count_bool())
#define SerSizeMapwstringBool(val) SerUtils::getMapSize((val), SerUtils::count_wstring(), SerUtils::count_bool())


////////////////////////////////////////////////////////////////////////////////////
// Outputter functions
////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////
// Native outputters
///////////////////////////////////
#define SerWriteDouble(out,val) SerUtils::write((out), (val), SerUtils::output_double())
#define SerWriteLong(out,val) SerUtils::write((out), (val), SerUtils::output_long())
#define SerWriteInt(out,val) SerUtils::write((out), (val), SerUtils::output_int())
#define SerWriteShort(out,val) SerUtils::write((out), (val), SerUtils::output_short())
#define SerWriteFloat(out,val) SerUtils::write((out), (val), SerUtils::output_float())
#define SerWriteBool(out,val) SerUtils::write((out), (val), SerUtils::output_bool())

#define SerWriteString(out,val) SerUtils::write((out), (val), SerUtils::output_String())
#define SerWritestring(out,val) SerUtils::write((out), (val), SerUtils::output_string())
#define SerWritewstring(out,val) SerUtils::write((out), (val), SerUtils::output_wstring())

///////////////////////////////////
// Vector outputters for native types
///////////////////////////////////
#define SerWriteVectorDouble(out,val) SerUtils::writeVector((out), (val), SerUtils::output_double())
#define SerWriteVectorLong(out,val) SerUtils::writeVector((out), (val), SerUtils::output_long())
#define SerWriteVectorInt(out,val) SerUtils::writeVector((out), (val), SerUtils::output_int())
#define SerWriteVectorShort(out,val) SerUtils::writeVector((out), (val), SerUtils::output_short())
#define SerWriteVectorFloat(out,val) SerUtils::writeVector((out), (val), SerUtils::output_float())
#define SerWriteVectorBool(out,val) SerUtils::writeVector((out), (val), SerUtils::output_bool())

#define SerWriteVectorString(out,val) SerUtils::writeVector((out), (val), SerUtils::output_String())
#define SerWriteVectorstring(out,val) SerUtils::writeVector((out), (val), SerUtils::output_string())
#define SerWriteVectorwstring(out,val) SerUtils::writeVector((out), (val), SerUtils::output_wstring())


///////////////////////////////////
// Map outputters for native types
///////////////////////////////////
// cslib String key
#define SerWriteMapStringString(out,val) SerUtils::writeMap((out), (val), SerUtils::output_String(), SerUtils::output_String())
#define SerWriteMapStringDouble(out,val) SerUtils::writeMap((out), (val), SerUtils::output_String(), SerUtils::output_double())
#define SerWriteMapStringLong(out,val) SerUtils::writeMap((out), (val), SerUtils::output_String(), SerUtils::output_long())
#define SerWriteMapStringInt(out,val) SerUtils::writeMap((out), (val), SerUtils::output_String(), SerUtils::output_int())
#define SerWriteMapStringShort(out,val) SerUtils::writeMap((out), (val), SerUtils::output_String(), SerUtils::output_short())
#define SerWriteMapStringFloat(out,val) SerUtils::writeMap((out), (val), SerUtils::output_String(), SerUtils::output_float())
#define SerWriteMapStringBool(out,val) SerUtils::writeMap((out), (val), SerUtils::output_String(), SerUtils::output_bool())

// std::string key
#define SerWriteMapstringstring(out,val) SerUtils::writeMap((out), (val), SerUtils::output_string(), SerUtils::output_string())
#define SerWriteMapstringDouble(out,val) SerUtils::writeMap((out), (val), SerUtils::output_string(), SerUtils::output_double())
#define SerWriteMapstringLong(out,val) SerUtils::writeMap((out), (val), SerUtils::output_string(), SerUtils::output_long())
#define SerWriteMapstringInt(out,val) SerUtils::writeMap((out), (val), SerUtils::output_string(), SerUtils::output_int())
#define SerWriteMapstringShort(out,val) SerUtils::writeMap((out), (val), SerUtils::output_string(), SerUtils::output_short())
#define SerWriteMapstringFloat(out,val) SerUtils::writeMap((out), (val), SerUtils::output_string(), SerUtils::output_float())
#define SerWriteMapstringBool(out,val) SerUtils::writeMap((out), (val), SerUtils::output_string(), SerUtils::output_bool())

// std::wstrings
#define SerWriteMapwstringwstring(out,val) SerUtils::writeMap((out), (val), SerUtils::output_wstring(), SerUtils::output_wstring())
#define SerWriteMapwstringDouble(out,val) SerUtils::writeMap((out), (val), SerUtils::output_wstring(), SerUtils::output_double())
#define SerWriteMapwstringLong(out,val) SerUtils::writeMap((out), (val), SerUtils::output_wstring(), SerUtils::output_long())
#define SerWriteMapwstringInt(out,val) SerUtils::writeMap((out), (val), SerUtils::output_wstring(), SerUtils::output_int())
#define SerWriteMapwstringShort(out,val) SerUtils::writeMap((out), (val), SerUtils::output_wstring(), SerUtils::output_short())
#define SerWriteMapwstringFloat(out,val) SerUtils::writeMap((out), (val), SerUtils::output_wstring(), SerUtils::output_float())
#define SerWriteMapwstringBool(out,val) SerUtils::writeMap((out), (val), SerUtils::output_wstring(), SerUtils::output_bool())


////////////////////////////////////////////////////////////////////////////////////
// inputter functions
////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////
// Native inputters
///////////////////////////////////
#define SerReadDouble(in,val) SerUtils::read((in), (val), SerUtils::input_double())
#define SerReadLong(in,val) SerUtils::read((in), (val), SerUtils::input_long())
#define SerReadInt(in,val) SerUtils::read((in), (val), SerUtils::input_int())
#define SerReadShort(in,val) SerUtils::read((in), (val), SerUtils::input_short())
#define SerReadFloat(in,val) SerUtils::read((in), (val), SerUtils::input_float())
#define SerReadBool(in,val) SerUtils::read((in), (val), SerUtils::input_bool())

#define SerReadString(in,val) SerUtils::read((in), (val), SerUtils::input_String())
#define SerReadstring(in,val) SerUtils::read((in), (val), SerUtils::input_string())
#define SerReadwstring(in,val) SerUtils::read((in), (val), SerUtils::input_wstring())

///////////////////////////////////
// Vector inputters for native types
///////////////////////////////////
#define SerReadVectorDouble(in,val) SerUtils::readVector((in), (val), SerUtils::input_double())
#define SerReadVectorLong(in,val) SerUtils::readVector((in), (val), SerUtils::input_long())
#define SerReadVectorInt(in,val) SerUtils::readVector((in), (val), SerUtils::input_int())
#define SerReadVectorShort(in,val) SerUtils::readVector((in), (val), SerUtils::input_short())
#define SerReadVectorFloat(in,val) SerUtils::readVector((in), (val), SerUtils::input_float())
#define SerReadVectorBool(in,val) SerUtils::readVector((in), (val), SerUtils::input_bool())

#define SerReadVectorString(in,val) SerUtils::readVector((in), (val), SerUtils::input_String())
#define SerReadVectorstring(in,val) SerUtils::readVector((in), (val), SerUtils::input_string())
#define SerReadVectorwstring(in,val) SerUtils::readVector((in), (val), SerUtils::input_wstring())

///////////////////////////////////
// Map inputters for native types
///////////////////////////////////
// cslib String key
#define SerReadMapStringString(out,val) SerUtils::readMap((out), (val), SerUtils::input_String(), SerUtils::input_String())
#define SerReadMapStringDouble(out,val) SerUtils::readMap((out), (val), SerUtils::input_String(), SerUtils::input_double())
#define SerReadMapStringLong(out,val) SerUtils::readMap((out), (val), SerUtils::input_String(), SerUtils::input_long())
#define SerReadMapStringInt(out,val) SerUtils::readMap((out), (val), SerUtils::input_String(), SerUtils::input_int())
#define SerReadMapStringShort(out,val) SerUtils::readMap((out), (val), SerUtils::input_String(), SerUtils::input_short())
#define SerReadMapStringFloat(out,val) SerUtils::readMap((out), (val), SerUtils::input_String(), SerUtils::input_float())
#define SerReadMapStringBool(out,val) SerUtils::readMap((out), (val), SerUtils::input_String(), SerUtils::input_bool())

// std::string key
#define SerReadMapstringstring(out,val) SerUtils::readMap((out), (val), SerUtils::input_string(), SerUtils::input_string())
#define SerReadMapstringDouble(out,val) SerUtils::readMap((out), (val), SerUtils::input_string(), SerUtils::input_double())
#define SerReadMapstringLong(out,val) SerUtils::readMap((out), (val), SerUtils::input_string(), SerUtils::input_long())
#define SerReadMapstringInt(out,val) SerUtils::readMap((out), (val), SerUtils::input_string(), SerUtils::input_int())
#define SerReadMapstringShort(out,val) SerUtils::readMap((out), (val), SerUtils::input_string(), SerUtils::input_short())
#define SerReadMapstringFloat(out,val) SerUtils::readMap((out), (val), SerUtils::input_string(), SerUtils::input_float())
#define SerReadMapstringBool(out,val) SerUtils::readMap((out), (val), SerUtils::input_string(), SerUtils::input_bool())

// std::wstring key
#define SerReadMapwstringwstring(out,val) SerUtils::readMap((out), (val), SerUtils::input_wstring(), SerUtils::input_wstring())
#define SerReadMapwstringDouble(out,val) SerUtils::readMap((out), (val), SerUtils::input_wstring(), SerUtils::input_double())
#define SerReadMapwstringLong(out,val) SerUtils::readMap((out), (val), SerUtils::input_wstring(), SerUtils::input_long())
#define SerReadMapwstringInt(out,val) SerUtils::readMap((out), (val), SerUtils::input_wstring(), SerUtils::input_int())
#define SerReadMapwstringShort(out,val) SerUtils::readMap((out), (val), SerUtils::input_wstring(), SerUtils::input_short())
#define SerReadMapwstringFloat(out,val) SerUtils::readMap((out), (val), SerUtils::input_wstring(), SerUtils::input_float())
#define SerReadMapwstringBool(out,val) SerUtils::readMap((out), (val), SerUtils::input_wstring(), SerUtils::input_bool())

// SerStdLib_H_LOADED
#endif
