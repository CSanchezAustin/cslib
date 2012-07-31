//      UtilMapping.cpp
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
#include <cslib.h>

#include <util/UtilMapping.h>
#include <util/UtilStringUtils.h>


// MappingRef
//  This is the individual reference of data in a Mapping
//  The data contained in this wrapper is not shared. Copies 
//  if the data are always made

MappingRef::MappingRef() :
    _type(keUnknown),
    _l(0)
{
}


MappingRef::MappingRef( 
            const String &value ) :
    _type(keString),
    _s(new String(value))
{
}

MappingRef::MappingRef( 
            const char* value ) :
    _type(keString),
    _s(new String(StringUtils::toString(value)))
{
}

MappingRef::MappingRef( 
            const wchar_t* value ) :
    _type(keString),
    _s(new String(StringUtils::toString(value)))
{
}

MappingRef::MappingRef( 
            const long value ) :
    _type(keLong),
    _l(value)
{
}


MappingRef::MappingRef( 
            const Mapping &value ) :
    _type(keMapping),
    _m(new Mapping(value))
{
}

MappingRef::MappingRef( 
            const bool value ) :
    _type(keBoolean),
    _b(value)
{
}


MappingRef::MappingRef( 
            const MappingRef &rhs ) :
    _type(keUnknown),
    _l(0)
{
    *this = rhs;
}


MappingRef& 
MappingRef::operator=( 
            const String &rhs )
{
    disposeOld();
    
    _type = keString;
    _s = new String( rhs );
    
    return *this;
}

MappingRef& 
MappingRef::operator=( 
            const char* rhs )
{
    disposeOld();
    
    _type = keString;
    _s = new String( StringUtils::toString(rhs) );

    return *this;
}

MappingRef& 
MappingRef::operator=( 
            const wchar_t* rhs )
{
    disposeOld();
    
    _type = keString;
    _s = new String( StringUtils::toString(rhs) );

    return *this;
}

MappingRef& 
MappingRef::operator=( 
            const long rhs )
{
    disposeOld();

    _type = keLong;
    _l = rhs;

    return *this;
}


MappingRef& 
MappingRef::operator=( 
            const Mapping &rhs )
{
    if ( _m != &rhs )
    {
        disposeOld();

        _type = keMapping;
        _m = new Mapping( rhs );
    }

    return *this;
}


MappingRef& 
MappingRef::operator=( 
            const bool rhs )
{
    disposeOld();

    _type = keBoolean;
    _b = rhs;

    return *this;
}


MappingRef& 
MappingRef::operator=( 
            const MappingRef &rhs )
{
    if ( this != &rhs )
    {
        disposeOld();

        _type = rhs._type;

        switch ( _type )
        {
            case keMapping:
                _m = new Mapping( *rhs._m );
                break;

            case keString:
                _s = new String( *rhs._s );
                break;

            case keLong:
                _l = rhs._l;
                break;

            case keBoolean:
                _b = rhs._b;
                break;

            case keUnknown:
                _l = 0;
                break;

            default:
                ASSERT_D(!NTEXT("MappingRef::operator="));
        }
    }

    return *this;
}

bool 
MappingRef::operator==(
	const MappingRef &rhs ) const
{
	if ( _type == rhs._type)
	{

		switch ( _type )
		{
			case keMapping:
				return *_m == *rhs._m;

			case keString:
				return *_s == *rhs._s;

			case keLong:
				return _l == rhs._l;

			case keBoolean:
				return _b == rhs._b;

			default:
				return false;
		}
	}

    return false;
}



MappingRef::~MappingRef()
{
    disposeOld();
}


void
MappingRef::disposeOld()
{
    switch ( _type )
    {
        case keMapping:
            delete _m;
            _m = NULL;
            break;

        case keString:
            delete _s;
            _s = NULL;
            break;

        case keBoolean:
            _b = false;
            break;

        case keLong:
        case keUnknown:
            _l = 0;
            break;

        default:
            ASSERT_D(!NTEXT("MappingRef::disposeOld"));
    }
}


MappingRef::operator const long() const
{
    ASSERT_D( _type == keLong );
    return _l;
}


MappingRef::operator const bool() const
{
    ASSERT_D( _type == keBoolean );
    return _b;
}


MappingRef::operator const String() const
{
    ASSERT_D( _type == keString );
    return *_s;
}


MappingRef::operator const Mapping() const
{
    ASSERT_D( _type == keMapping );
    return *_m;
}


// Mapping
//  This class provides a wrapper for an STL map, but
//  provides a bit more functionality

Mapping::Mapping()
{
}


/**
    parse a string with the following format:
    key="value" key2="value2"... to create a mapping
    Note: Sub-maps are not yet supported: BUGBUG: TODO
*/
Mapping::Mapping( 
            const String& inMapStr )
{
    String::size_type curPos = 0;
    while ( ( curPos != String::npos ) && ( curPos < inMapStr.size() ) )
    {
        String key;
        String value;

        // ignore whitespace preceeding the 
        while(  ( curPos < inMapStr.size() ) && 
                ( inMapStr.at( curPos ) == String::value_type(' ') ) )
            curPos++;

        String::size_type equalPos = inMapStr.find( NTEXT("="), curPos );
        if ( equalPos != String::npos )
        {
            // ignore white space preceeding the "=" sign
            String::size_type copyPos = equalPos;
            while( inMapStr.at( copyPos ) == String::value_type(' ') )
                copyPos--;

            key = inMapStr.substr( curPos, copyPos - curPos );
            curPos = equalPos++;

            // now look for everything in quotes
            String::size_type firstQ = inMapStr.find_first_of( NTEXT("\"'"), curPos );
            if ( firstQ != String::npos )
            {   // have the opening, now look for the close
                String::size_type secondQ = inMapStr.find_first_of( NTEXT("\"'"), firstQ + 1 );
                if ( secondQ != String::npos )
                {
                    value = inMapStr.substr( firstQ + 1, secondQ - firstQ - 1 );
                    (*this)[ key ] = value;
                    curPos = secondQ + 1;
                }
                else
                {
                    break;
                }
            }
            else
            {
                break;
            }
        }
    }    
}

Mapping::Mapping( 
    const int argc, 
    const wchar_t* argv[] )
{
    // 0 argument is always the executable name, so just skip over it
    
    int cnt = 1;
    while (cnt < argc)
    {
        // get the current argument and increment
        // the counter to point to the next argument

        const wchar_t* curArg = argv[cnt++];

        // remember that cnt now points to the next argument
        if ( curArg[0] == NTEXT('-'))	// BUGBUG TODO" if unicode is note defined then NTEXT will resolve to a char*. fix this
        {
            // the argument can either be a switch (no - for the next
            // argument) or can be a key value pair

            // get the current key by skipping past the -
            String key = StringUtils::toString(curArg + 1);
            String value;

            // now look at the next argument and see if it has a -
            if ( (cnt < argc) && (argv[cnt][0] != NTEXT('-')) )
            {
                curArg = argv[cnt];
                value = StringUtils::toString(curArg);
                (*this)[ key ] = value;
            }
            else
            {
                // was just a switch
                (*this)[ key ] = true;
            }
        }
    }
}

Mapping::Mapping( 
    const int argc, 
    const char* argv[] )
{
    // 0 argument is always the executable name, so just skip over it
    
    int cnt = 1;
    while (cnt < argc)
    {
        // get the current argument and increment
        // the counter to point to the next argument

        const char* curArg = argv[cnt++];

        // remember that cnt now points to the next argument
        if ( curArg[0] == '-')
        {
            // the argument can either be a switch (no - for the next
            // argument) or can be a key value pair

            // get the current key by skipping past the -
            String key = StringUtils::toString(curArg + 1);
            String value;

            // now look at the next argument and see if it has a -
            if ( (cnt < argc) && (argv[cnt][0] != '-') )
            {
                curArg = argv[cnt];
                value = StringUtils::toString(curArg);
                (*this)[ key ] = value;
            }
            else
            {
                // was just a switch
                (*this)[ key ] = true;
            }
        }
    }
}


Mapping::Mapping( IFSTREAM& in, const String* sectionDelimiters )
{
  readFrom(in, sectionDelimiters);
}


Mapping::Mapping( 
            const Mapping &rhs )
{
    *this = rhs;
}


Mapping& 
Mapping::operator=( 
            const Mapping &rhs )
{
    if ( this != &rhs )
    {
        BaseMapping::operator=(rhs);
    }

    return *this;
}

bool Mapping::operator==(const Mapping& rhs)
{
	// couple of quick checks
	if ( this == &rhs )
	{
		return true;
	}

	if (size() != rhs.size())
	{
		return false;
	}

	// iterate through the 'this' mapping to see if it has the equivalent amount
    for ( Mapping::const_iterator it = begin();
            it != end();
            it++ )
    {
		Mapping::const_iterator rhsIt = rhs.get_ref( (*it).first, (*it).second._type);
		if (rhsIt == rhs.end())
		{
			// not found at all in the RHS mapping
			return false;
		}

		// now check the values
		if ( !((*it).second == (*rhsIt).second) )
		{
			return false;
		}
    }


	return true;
}


Mapping::~Mapping()
{
}


Mapping::iterator 
Mapping::find( 
            const int value ) 
{
    return BaseMapping::find( StringUtils::toString( value ) ); 
}


Mapping::const_iterator 
Mapping::find( 
            const int value ) const 
{
    return BaseMapping::find( StringUtils::toString( value ) ); 
}


Mapping::size_type 
Mapping::erase(
            const int value )
{
    return BaseMapping::erase( StringUtils::toString( value ) ); 
}


int
Mapping::merge( 
            const Mapping &value )
{
    int result = 0;

    for ( Mapping::const_iterator it = value.begin();
            it != value.end();
            it++ )
    {
        (*this)[ (*it).first ] = (*it).second;
        result++;
    }

    return result;
}


bool
Mapping::check_type( 
            const String &value,
            const MappingRef::Type expectedType ) const
{
    return get_ref( value, expectedType ) != end();
}


Mapping::const_iterator 
Mapping::get_ref( 
            const String &value,
            const MappingRef::Type expectedType ) const
{
    Mapping::const_iterator it = find( value );
    if ( ( it != end() ) && 
         ( (*it).second._type != expectedType ) )
    {
        it = end();
    }

    return it;    
}


bool 
Mapping::getIfAvailable(
            const String &value,
            String& outValue ) const
{
    bool res = false;

    Mapping::const_iterator it = find( value );
    if ( ( it != end() ) && 
         ( (*it).second._type == MappingRef::keString ) )
    {
        outValue = *(it->second._s);
        res = true;
    }

    return res;
}

bool 
Mapping::getIfAvailable(
            const String &value,
            long& outValue ) const
{
    bool res = false;

    Mapping::const_iterator it = find( value );
    if ( ( it != end() ) && 
         ( (*it).second._type == MappingRef::keLong ) )
    {
        outValue = it->second._l;
        res = true;
    }

    return res;
}


bool 
Mapping::getIfAvailable(
            const String &value,
            bool& outValue ) const
{
    bool res = false;

    Mapping::const_iterator it = find( value );
    if ( ( it != end() ) && 
         ( (*it).second._type == MappingRef::keBoolean ) )
    {
        outValue = it->second._b;
        res = true;
    }

    return res;
}


bool 
Mapping::getIfAvailable(
            const String &value,
            Mapping& outValue ) const
{
    bool res = false;

    Mapping::const_iterator it = find( value );
    if ( ( it != end() ) && 
         ( (*it).second._type == MappingRef::keMapping ) )
    {
        outValue = *(it->second._m);
        res = true;
    }

    return res;
}


MappingRef& 
Mapping::operator[]( const String &index ) 
{
    return BaseMapping::operator[]( index );
}


MappingRef& 
Mapping::operator[]( 
            const int index )
{
    return BaseMapping::operator[]( StringUtils::toString(index) );
}


/**
    create a string with the following format:
    key="value" key2="value2"...
    Note: Sub-maps are not yet supported: BUGBUG: TODO
*/
String 
Mapping::toString() const
{
    bool needSpace = false;

    String result;
    for ( Mapping::const_iterator it = begin();
            it != end(); it++)
    {
        if ( needSpace )
            result += NTEXT(" ");

        result += (*it).first;  // put the key and then the value surrounded by quotes
        result += NTEXT("=\"");
        if ( (*it).second._type == MappingRef::keString )
        {
            result += *( (*it).second._s );
        }
        else if ( (*it).second._type == MappingRef::keLong )
        {
            result += StringUtils::toString( (*it).second._l );
        }
        else if ( (*it).second._type == MappingRef::keBoolean )
        {
            if ((*it).second._b)
            {
                result += TRUE_STR;
            }
            else
            {
                result += FALSE_STR;
            }
        }
        else if ( (*it).second._type == MappingRef::keMapping )
	{
	  result += (*it).second._m->toString();
	}

        result += NTEXT("\"");
        needSpace = true;
    }

    return result;
}


String 
Mapping::toArgcString() const
{
    bool needSpace = false;

    String result;
    for ( Mapping::const_iterator it = begin();
            it != end(); it++)
    {
        if ( needSpace )
            result += NTEXT(" ");

        if ( (*it).second._type == MappingRef::keString )
        {
            result += DASH;
            result += (*it).first;
            result += SPACE;
            result += *( (*it).second._s );
        }
        else if ( (*it).second._type == MappingRef::keLong )
        {
            result += DASH;
            result += (*it).first;
            result += SPACE;
            result += StringUtils::toString( (*it).second._l );
        }
        else if ( (*it).second._type == MappingRef::keBoolean )
        {
            if ((*it).second._b)
            {
                result += DASH;
                result += (*it).first;
            }
        }

        needSpace = true;
    }

    return result;
}

bool
Mapping::readFrom(IFSTREAM& in, const String* sectionDelimiters)
{
    if ( in.good() )
    {
        CHAR_T buffer[1024];
        Mapping* sectionMapPtr = NULL;
	Mapping emptyMap;
	bool skipSection = false;
        
        while( !in.eof() && in.good() )
        {                
            in.getline(buffer, 1024);

            String curLine(buffer);
            StringUtils::trimBoth( WS_PAT, curLine );

            if ( curLine.length() == 0 ||
                 curLine[0] == NTEXT('#'))
            {
                // the line is empty or is a comment
                continue;
            }

            // handle section delimiters
            if (sectionDelimiters != NULL)
            {
                // peek at the current line to see if it looks like a header, e.g [dddd]
                if ( (curLine[0] == NTEXT('[')) &&  (curLine[curLine.length()-1] == NTEXT(']')) )
                {
					// if we are already processing a section, then we have to stop that one and move
					// onto the next line
					sectionMapPtr = NULL;
                    
					// always strip the brackets off
					StringUtils::trimFront(NTEXT("["), curLine);
					StringUtils::trimBack(NTEXT("]"), curLine);
				  
					bool addIt = false;
					if (sectionDelimiters->length() == 0 )
					{
						// if the sections delimiters is empty, then add all section delimiters to the map
						addIt = true;
					}
					else
					{
						// otherwise, check to see if the current line is one of the delimiters
						addIt = sectionDelimiters->find( curLine ) != String::npos;
					}

					if ( addIt )
					{
						// set up the new section delimiter map
						(*this)[curLine] = emptyMap;
						sectionMapPtr = (*this)[curLine]._m;
						skipSection = false;
					}
					else
					{
						// if we have a delimiter section that doesn't match one
						// of our inputs, then we need to skip all the parameters
						// until we hit another section that matches our delimiters
						skipSection = true;
					}

					continue;
                }
            }

	    if (skipSection)
	    {
	      continue;
	    }

            String key;
            String value;
            StringUtils::splitFront( COLON, curLine, &key, &value );
            StringUtils::trimBoth( WS_PAT, key );
            StringUtils::trimBoth( WS_PAT, value );
            if ( key.length() == 0 || value.length() == 0 )
            {                   
                DBG_LOGERR( NTEXT("Error processing line: ") + curLine );
                continue;
            }
            
            if (sectionMapPtr != NULL)
            {
                (*sectionMapPtr)[ key ] = value;
               
            }
            else
            {
                (*this)[ key ] = value;
            }
        }

        return true;
    }
    else
    {
        DBG_LOGERR(NTEXT("input ifstream not valid"));
        return false;
    }

    return false;
}
