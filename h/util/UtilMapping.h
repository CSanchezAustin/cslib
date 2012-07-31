#ifndef UtilMapping_H_LOADED
#define UtilMapping_H_LOADED
// [
// $        UtilMapping.h
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
//                 $Revision: 2.2 $
//                 $Author: sanchezc $
//                 $Date: 2002/09/26 17:19:31 $
// ?
//         Contents: Mapping class
//
//         Purpose:
//
//         Usage Notes:
//
//                 
// ?
// !        Changes History
//     $Header: /cvs/common/cslib/h/util/UtilMapping.h,v 2.2 2002/09/26 17:19:31 sanchezc Exp $
//         
// ]

#include <map>
#include <fstream>

#include <cslib.h>

class Mapping;

struct EXPORT_DLL MappingRef
{
    enum Type { keUnknown, keString, keLong, keMapping, keBoolean };

    // create an Mapping
    MappingRef();
    explicit MappingRef( const String &value );
    explicit MappingRef( const char* value );
    explicit MappingRef( const wchar_t* value );
    explicit MappingRef( const long value );
    explicit MappingRef( const Mapping &value );
    explicit MappingRef( const bool value );

    MappingRef( const MappingRef &rhs );

    MappingRef& operator=( const String &rhs );
    MappingRef& operator=( const char* rhs );
    MappingRef& operator=( const wchar_t* rhs );
    MappingRef& operator=( const long rhs );
    MappingRef& operator=( const Mapping &rhs );
    MappingRef& operator=( const MappingRef &rhs );
    MappingRef& operator=( const bool rhs );

    bool operator==( const MappingRef &rhs ) const;

    operator const long() const;
    operator const String() const;
    operator const Mapping() const;
    operator const bool() const;

    ~MappingRef();

    Type _type;
    union 
    {
        String *_s;
        long _l;
        Mapping *_m;
        bool _b;
    };
private:
    void disposeOld();
};


typedef std::map<String, MappingRef> BaseMapping;

class EXPORT_DLL Mapping : public BaseMapping
{
public:

    // create a Mapping
    Mapping();
    /**
        parse a string with the following format:
        key="value" key2="value2"... to create a mapping
        Note: Sub-maps are not yet supported: BUGBUG: TODO
    */
    Mapping( const String& inMapStr );

    /**
       parse out a map from standard main()
       arguments for a c program
     */
    Mapping( const int argc, const char* argv[]);

	    /**
       parse out a map from standard main()
       arguments for a c program
     */
    Mapping( const int argc, const wchar_t* argv[]);


    /**
       parse out a map from a configuration file
       with the following format
       # comment
       key: value
    */
    Mapping( IFSTREAM& in, const String* sectionDelimiters = NULL );

    Mapping( const Mapping &rhs );
    Mapping& operator=( const Mapping &rhs );
	bool operator==(const Mapping& rhs);
        
    ~Mapping();

    bool readFrom(IFSTREAM& in, const String* sectionDelimiters = NULL);

    iterator find( const String& value ) { return BaseMapping::find( value ); }
    const_iterator find( const String& value ) const { return BaseMapping::find( value ); }

    iterator find( const int value );
    const_iterator find( const int value ) const;

    size_type erase( const String &value ) { return BaseMapping::erase( value ); }
    size_type erase( const int value );

    //  returns the number of items merged from the source mapping
    //  this does not represent size()
    int merge( const Mapping &value );

    bool check_type( 
        const String &value,
        const MappingRef::Type expectedType ) const;
    
    const_iterator get_ref( 
        const String &value,
        const MappingRef::Type expectedType ) const;
    
    bool getIfAvailable(
        const String &value,
        String& outValue ) const;

    bool getIfAvailable(
        const String &value,
        long& outValue ) const;

    bool getIfAvailable(
        const String &value,
        Mapping& outValue ) const;

    bool getIfAvailable(
        const String &value,
        bool& outValue ) const;

    MappingRef& operator[]( const int index );
    MappingRef& operator[]( const String &index );

    /**
        create a string with the following format:
        key="value" key2="value2"...
        Note: Sub-maps are not yet supported: BUGBUG: TODO
    */
    String toString() const;

    String toArgcString() const;

protected:

private:

    // Unimplemented standard methods
};

// UtilMapping_H_LOADED
#endif
