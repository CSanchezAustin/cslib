//      utiltest.cpp
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
//              $Revision: 2.2 $
//              $State: Exp $
//              $Author: sanchezc $
//              $Date: 2002/09/26 16:55:03 $
//
//      Contents:	
//
#if defined(_WINDOWS) || defined(WIN32)
#pragma warning(disable: 4786)
#endif

#include <stdio.h>
#include <fstream>
#include <sstream>
#include <cmath>

#include "util/UtilMapping.h"
#include "util/UtilRefCounted.h"
#include "util/UtilHeapInlines.h"
#include "util/UtilAutoPtrInlines.h"
#include "util/UtilStringUtils.h"
#include "util/UtilStreaming.h"
#include "util/UtilStringTokenizer.h"
#include "util/UtilDllLoader.h"
#include "lang/Date.h"

// typedef for testing function test_dllloader()
#ifdef UNICODE
typedef void (*REVERSE)(wchar_t* MESSAGE);
#else
typedef void (*REVERSE)(char* MESSAGE);
#endif
typedef int (*GETMODULEVERSION)();


void test_autoptr();
void test_heap();
void test_mapping(const int argc, const char* argv[]);
void test_tokenizer();
void test_dllloader();
void test_utils();
void test_streaming();

int main(const int argc, const char* argv[])
{
  OFSTREAM out(NTEXT("debug-log"));
  OFSTREAM err(NTEXT("err.log"));
//    Debug::setOutputStream( out );
//    Debug::setErrStream( err );

        Debug::setCurrentModule( NTEXT("UTILTEST") );

    test_tokenizer();
    test_mapping(argc, argv);
    test_autoptr();
    test_utils();
    test_dllloader();
    test_streaming();
    test_heap();
 
    return 0;
}


//+++++++++++++++++++++++++++++++++++++
//
//  AutoPtr and RefCounted, InterlockedLong
//
//+++++++++++++++++++++++++++++++++++++

class Moo : public RefCounted
{
public:
    Moo() {}
    ~Moo() {}
    void doMoo() { COUT << NTEXT("Moo...") << std::endl; }
};


void test_autoptr()
{
    Moo *pm = new Moo;

    // AutoPtr();
    AutoPtr<Moo> defaultPtr;

    // explicit AutoPtr( T *ptr );
    AutoPtr<Moo> createdPtr(pm);
    AutoPtr<Moo> createdPtr2(new Moo);

    // AutoPtr( const AutoPtr& rhs );
    AutoPtr<Moo> constructedPtr( createdPtr );

    // AutoPtr& operator=( const AutoPtr& rhs );
    defaultPtr = constructedPtr;

    // AutoPtr& operator=( const T *rhs );
    defaultPtr = pm;

    // T* operator ->() const { return _ptr; }
    createdPtr->doMoo();

    // T& operator *()  const { return *_ptr; }
    Moo m( *createdPtr );

    // bool operator == (const T* rhs)  const
    ASSERT( createdPtr == pm );

    // bool operator != (const AutoPtr<T>& rhs) const
    ASSERT( createdPtr != createdPtr2 );

    // bool operator != (const T* rhs)  const
    ASSERT( createdPtr2 != pm );

    defaultPtr = createdPtr;
    defaultPtr = new Moo;

    defaultPtr = NULL;

    m.doMoo();
}


//+++++++++++++++++++++++++++++++++++++
//
//  Heap
//
//+++++++++++++++++++++++++++++++++++++

class FooTest
{
private:
    int i;

public:
    FooTest( const int value = 0 ) : i(value){}
    int Get() const { return i; }
    void Set(const int in) { i = in; }

    friend OSTREAM& operator<<( OSTREAM &o, const FooTest *test )
    {
        o << (*test).i;
        return o;
    }
};


class FooComparator : public std::binary_function<FooTest*, FooTest*, bool>
{
public:
    bool operator() ( const FooTest *a, const FooTest *b ) const
    {
        return a->Get() < b->Get();
    }
};

class KeyComparator :
    public std::binary_function<const String&, const String&, bool>
{
public:
    bool operator() ( const String& a, const String& b ) const
    {
        return a < b;
    }
};


void test_heap()
{
    FooTest *sixteen = new FooTest(16);
    FooTest *fifteen = new FooTest(15);
    FooTest *twelve = new FooTest(12);
    FooTest *four = new FooTest(4);

    Heap<String, FooTest*, FooComparator, KeyComparator> h;
    h.add( NTEXT("four"), four );
    h.print();
    h.add( NTEXT("one"), new FooTest(1) );
    h.print();
    h.add( NTEXT("three"), new FooTest(3) );
    h.print();
    h.add( NTEXT("two"), new FooTest(2) );
    h.print();
    h.add( NTEXT("sixteen"), sixteen );
    h.print();
    h.add( NTEXT("nine"), new FooTest(9) );
    h.print();
    h.add( NTEXT("ten"), new FooTest(10) );
    h.print();
    h.add( NTEXT("fourteen"), new FooTest(14) );
    h.print();
    h.add( NTEXT("eight"), new FooTest(8) );
    h.print();
    h.add( NTEXT("seven"), new FooTest(7) );
    h.print();
    h.add( NTEXT("twenty"), new FooTest(20) );
    h.print();
    h.add( NTEXT("eleven"), new FooTest(11) );
    h.print();
    h.add( NTEXT("twelve"), twelve );
    h.print();
    h.add( NTEXT("sixtysix"), new FooTest(66) );
    h.print();
    h.add( NTEXT("seventeen"), new FooTest(17) );
    h.print();
    h.add( NTEXT("thirty"), new FooTest(30) );
    h.print();
    h.add( NTEXT("fifteen"), fifteen );
    h.print();
    h.add( NTEXT("twentysix"), new FooTest(26) );
    h.print();
    h.add( NTEXT("seventy"), new FooTest(70) );
    h.print();

    COUT << h.pop() << std::endl;
    h.print();

    h.add( NTEXT("four"), four );
    h.print();

    h.remove( NTEXT("four") );
    h.print();

    COUT << h.pop() << std::endl;
    h.print();

    COUT << h.pop() << std::endl;
    h.print();

    h.push( NTEXT("onefourteen"), new FooTest(114) );
    h.print();
    h.push( NTEXT("onetwenty"), new FooTest(120) );
    h.print();
    h.push( NTEXT("oneeleven"), new FooTest(111) );
    h.print();
    h.push( NTEXT("onesixtysix"), new FooTest(166) );
    h.print();
    h.push( NTEXT("oneseventeen"), new FooTest(117) );
    h.print();
    h.push( NTEXT("onethirty"), new FooTest(130) );
    h.print();
    h.push( NTEXT("onetwentysix"), new FooTest(126) );
    h.print();
    h.push( NTEXT("oneseventy"), new FooTest(170) );
    h.print();

    h.build();
    h.print();

    sixteen->Set( 13 );
    h.print();
    h.heapify( NTEXT("sixteen") );
    h.print();

    twelve->Set( 27 );
    h.print();
    h.heapify( NTEXT("twelve") );
    h.print();

    fifteen->Set( 12 );
    h.print();
    h.heapify( NTEXT("fifteen") );
    h.print();
}

void 
test_mapping(const int argc, const char* argv[])
{
    Mapping io;
    io[NTEXT("one")] = NTEXT("one");
    io[NTEXT("one")] = NTEXT("one-one");
    io[2] = 2L;
    io[NTEXT("three")] = 3L;

    ASSERT( io.find(NTEXT("one")) != io.end() );
    ASSERT( io.find(2) != io.end() );
    ASSERT( io.find(NTEXT("three")) != io.end() );

    Mapping mp( NTEXT("velvet=\"black\" cat=\"french\"") );
    mp[NTEXT("true_bool")] = true;
    mp[NTEXT("false_bool")] = false;
    mp[NTEXT("state")] = NTEXT("Florida");
    mp[ 1 ] = NTEXT("One");

	String str = mp.toString();
    COUT << str.c_str() << std::endl;
	str = mp.toArgcString();
    COUT << str.c_str() << std::endl;

    Mapping mp2(argc, argv);
    COUT << mp2.toArgcString().c_str() << std::endl;
    COUT << mp2.toString().c_str() << std::endl;

    // test without section delimiters
	COUT << NTEXT("test WITHOUT section delimiters\n");
    IFSTREAM in(NTEXT("config.txt"));
    Mapping mp3(in);
    COUT << mp3.toString().c_str() << std::endl;

    int cnt = mp3.merge(mp2);
    String itsValue;
    bool res = mp3.getIfAvailable(NTEXT("number2"), itsValue);
	{
	COUT << NTEXT("test with section delimiters\n");
    String delims =  NTEXT("cslib-en cslib-fr");
    IFSTREAM in2(NTEXT("config-delim.txt"));
    Mapping mp4(in2, &delims);
    COUT << mp4.toString().c_str() << std::endl;
	}
	{
	COUT << NTEXT("test with ALL section delimiters\n");
    String delims =  NTEXT("");
    IFSTREAM in2(NTEXT("config-delim.txt"));
    Mapping mp4(in2, &delims);
    COUT << mp4.toString().c_str() << std::endl;
	}
}


void test_tokenizer()
{
    String data(
        NTEXT("\t\tthis is some\t\ttext;of a test\t of another_test\t\t"));
    String tokens(NTEXT(" \t;_"));
    
    // normal processing
    StringTokenizer b( data , tokens );
    while ( b.hasMoreTokens() )
    {
        String tok = b.getNextToken();
        COUT << tok.c_str() << std::endl;            
    }

    // do not skip delimiters
    StringTokenizer b2( data , tokens, false );
    while ( b2.hasMoreTokens() )
    {
        String tok = b2.getNextToken();
        COUT << tok.c_str() << std::endl;            
    }

    // test finding all tokens and not skipping delimiters
    data = NTEXT("POST / HTTP/1.1\r\nAccept: image/gif, image/x-xbitmap, image/jpeg, image/pjpeg, application/vnd.ms-powerpoint,application/vnd.ms-excel, application/msword, application/pdf, */*\r\n\r\nsome data\r\n");
    tokens = NTEXT("\r\n");
    StringTokenizer b3( data , tokens, false, true );
    while ( b3.hasMoreTokens() )
    {
        String tok = b3.getNextToken();
        COUT << tok.c_str() << std::endl;            
    }


    // test finding all tokens and skipping delimiters
    StringTokenizer b4( data , tokens, true, true );
    while ( b4.hasMoreTokens() )
    {
        String tok = b4.getNextToken();
        COUT << tok.c_str() << std::endl;            
    }

    StringTokenizer b5(NTEXT("A,B,C, D, E, F G"), NTEXT(","));
    while ( b5.hasMoreTokens() )
    {
        String tok = b5.getNextToken();
        COUT << tok.c_str() << std::endl;            
    }
}


void test_dllloader()
{
    REVERSE rev;
#ifdef WINDOWS
    DllLoader sample(NTEXT("sample.dll"));
#else
    DllLoader sample(NTEXT("sample.so"));
#endif
#ifdef UNICODE
    rev = (REVERSE) sample.getFacility(NTEXT("reverse_w"));
#else
    rev = (REVERSE) sample.getFacility(NTEXT("reverse_c"));
#endif
    if (rev != NULL) 
    {
#ifdef UNICODE
        wchar_t buff[100];
        ::wcscpy( buff, NTEXT("Test message only\n") );
#else
        char buff[100];
        ::strcpy( buff, NTEXT("Test message only\n") );
#endif
        COUT << NTEXT("Pre reverse string: ");
	COUT << buff << std::endl;
        (*rev) (buff);
        COUT << NTEXT("Post reverse string: ");
	COUT << buff << std::endl;
    }
    else 
        COUT << NTEXT(" Facility  foo is not found in the dll.");

    GETMODULEVERSION mod;
    DllLoader sample2 = sample;
    mod = (GETMODULEVERSION) sample2.getFacility(NTEXT("getModuleVersion"));
    if (mod != NULL)
    {
      int i = (*mod) ();
    }
}

void test_utils()
{
    String source = NTEXT("\\\\c:\\temp\\\\\\");
    String pattern = NTEXT("\\");

    StringUtils::trimFront( pattern, source );
    StringUtils::trimBack( pattern, source );

    source = NTEXT("a:b:c");
    pattern = NTEXT(":");

    String front, back;
    StringUtils::splitFront( pattern, source, &front, &back );
    StringUtils::splitBack( pattern, source, &front, &back );

    source = NTEXT("http://www.test.com:80");
    pattern = NTEXT("://");
    StringUtils::splitFront( pattern, source, &front, &back );
    pattern = NTEXT(":");
    StringUtils::splitBack( pattern, source, &front, &back );

    Date sourceDate( 2, 3, 2001 );
    String answerStr = StringUtils::toString( sourceDate );
    if ( String( NTEXT("2001-02-03") ) == answerStr )
    {
        COUT << NTEXT("\nDate toString successful\n");
    }
    else
    {
        CERR << NTEXT("\nDate toString failed\n");
    }

    String sourceStr( NTEXT("2002-05-12") );
    Date answerDate = StringUtils::toDate( sourceStr );

    if ( Date( 5, 12, 2002 ) == answerDate )
    {
        COUT << NTEXT("\nString toDate successful\n");
    }
    else
    {
        CERR << NTEXT("\nString toDate failed\n");
    }

    float sourceFloat = ( float )671.23;
    String answerString = StringUtils::toString( sourceFloat );
	COUT << NTEXT("\n671.23 converts to ") << answerString << NTEXT("\n");

    sourceStr = NTEXT("37.251");
    float answerFloat = StringUtils::toFloat( sourceStr );
    if ( 0.001 > fabs( answerFloat - 37.251 ) )
    {
        COUT << NTEXT("\nString toFloat successful\n");
    }
    else
    {
        CERR << NTEXT("\nString toFloat failed\n");
    }

    sourceStr = NTEXT("true");
    bool answerBool = StringUtils::toBool( sourceStr );
    if ( true == answerBool )
    {
        COUT << NTEXT("\nString toBool successful\n");
    }
    else
    {
        CERR << NTEXT("\nString toBool failed\n");
    }

    bool sourceBool = false;
    answerString = StringUtils::toString( sourceBool );
    if ( FALSE_STR == answerString )
    {
        COUT << NTEXT("\nBool toString successful\n");
    }
    else
    {
        CERR << NTEXT("\nBool toString failed\n");
    }

    double sourceDouble = 569432.338;
    answerString = StringUtils::toString( sourceDouble );
	COUT << NTEXT("\n569432.338 converts to ") << answerString << NTEXT("\n");

    sourceStr = NTEXT("0.44567");
    double answerDouble = StringUtils::toDouble( sourceStr );
    if ( 0.00001 > fabs( answerDouble - 0.44567 ) )
    {
        COUT << NTEXT("\nString toDouble successful\n");
    }
    else
    {
        CERR << NTEXT("\nString toDouble failed\n");
    }

	String mixedCase(NTEXT("This Is Camel Case"));
	ASSERT(StringUtils::upper(mixedCase) == NTEXT("THIS IS CAMEL CASE"));
	ASSERT(StringUtils::lower(mixedCase) == NTEXT("this is camel case"));

	ASSERT( StringUtils::compareCaseless(NTEXT("This is some Text"), NTEXT("this is some text")) == 0 );
	ASSERT( StringUtils::findCaseless(NTEXT("This is some Text"), NTEXT("some")) != String::npos );
}

void test_streaming()
{
	ScheduleTime sourceTime;
	Mapping mp;
	Mapping sourceMap;

	sourceMap[NTEXT("some.sub.key.int")] = 12L;
	sourceMap[NTEXT("some.sub.key.bool")] = true;
	sourceMap[NTEXT("some.sub.key.str")] = NTEXT("sub.text");

	sourceTime.break_time();

	String sourceString =  NTEXT("key");
	long sourceLong = 100L;
	StreamUtils::putKey(NTEXT("time_key"), sourceTime, mp );
	StreamUtils::putKey(NTEXT("int_key"), sourceLong, mp );
	StreamUtils::putKey(NTEXT("str_key"), sourceString, mp );
	StreamUtils::putKey(NTEXT("map_key"), sourceMap, mp );

	String str = mp.toString();
    COUT << str.c_str() << std::endl;

	String retrievedStr;
	long retrievedLong;
	ScheduleTime retrievedTime;
	Mapping retrievedMapping;

	StreamUtils::getKey(NTEXT("time_key"), mp, retrievedTime );
	StreamUtils::getKey(NTEXT("int_key"), mp, retrievedLong );
	StreamUtils::getKey(NTEXT("str_key"), mp, retrievedStr );
	StreamUtils::getKey(NTEXT("map_key"), mp, retrievedMapping );

	ASSERT( sourceTime == retrievedTime );
	ASSERT( sourceLong == retrievedLong );
	ASSERT( retrievedStr.compare(sourceString) == 0 );
	ASSERT( sourceMap == retrievedMapping );
}
