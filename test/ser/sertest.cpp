//      sertest.cpp
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
//              $Author: $
//              $Date: $
//
//      Contents:	
//
#if defined(_WINDOWS) || defined(WIN32)
#pragma warning(disable: 4786)
#endif

#include <stdio.h>
#include <fstream>
#include <sstream>

#include "ser/SerSerializationMgr.h"
#include "ser/SerOutputters.h"
#include "ser/SerInputters.h"
#include "ser/SerCounters.h"
#include "ser/SerStdLib.h"
#include "pat/PatSerializable.h"
#include "sys/SysContext.h"
#include "xml/XmlDomUtils.h"
#include "xml/XmlStXmlOpener.h"
#include "util/UtilRefCountedPtrInlines.h"
#include "util/UtilStringUtils.h"

// a few macros to control the number of iterations to 
// create data with and to control printing the containers
//#define NUM_TIMES 3
#define NUM_TIMES 50000
//#define PRINTOUTPUT
//#define USE_MAPS
#define USE_PTRS


void test_serializable();
void test_maps();

int main()
{
  OFSTREAM out(NTEXT("debug-log"));
  OFSTREAM err(NTEXT("err.log"));
//    Debug::setOutputStream( out );
//    Debug::setErrStream( err );

    Debug::setCurrentModule( NTEXT("SERTEST") );

    ::srand(::time(NULL));

    test_maps();
    test_serializable();
    return 0;
}


/*********************************************************************/

class SharedPtr : public iSerializable
{
public:
    int value;

    SharedPtr() :
        value(0)
    {
    }

    SharedPtr(const SharedPtr& rhs) :
        value(rhs.value)
    {
    }

    SharedPtr(const int i) : 
        value(i)
    {
    }

    SharedPtr& operator=(const SharedPtr& rhs)
    {
        if ( this != &rhs )
        {
            value = rhs.value;
        }

        return *this;
    }

    bool lock() { return true; }
    
    bool unlock() { return true; }

    size_t size() const
    {
        return
            SerSizeInt(value);
    }

    std::ostream& serialize(std::ostream& out) const
    {
        SerWriteInt(out, value);

        return out;
    }

	std::istream& deserialize(std::istream& in)
    {
        SerReadInt(in, value);

        return in;
    }
};

typedef SerUtils::__vector_counter_serializable_ptr__<SharedPtr> count_sharedptr;
typedef SerUtils::__output_serializable_ptr__<SharedPtr> output_sharedptr;
typedef SerUtils::__input_serializable_ptr__<SharedPtr> input_sharedptr;


class MasterStruct : public iSerializable
{
public:
    int one_int;
    int two_int;
    bool one_bool;
    bool two_bool;
    std::vector<double> somedoubles;
    std::string somestring;
    std::wstring somewstring;
#ifdef USE_MAPS
    std::map<String, int> somemap;
#endif
#ifdef USE_PTRS
    std::vector<SharedPtr*> someptrs;
#endif

    MasterStruct() : 
        one_int(0),
        two_int(0),
        one_bool(false),
        two_bool(false)
    {
    }

    MasterStruct(const MasterStruct& rhs) : 
        one_int(rhs.one_int),
        two_int(rhs.two_int),
        one_bool(rhs.one_bool),
        two_bool(rhs.two_bool),
        somedoubles(rhs.somedoubles),
        somestring(rhs.somestring),
        somewstring(rhs.somewstring)
#ifdef USE_MAPS
        , somemap(rhs.somemap)
#endif
    {
#ifdef USE_PTRS
        for (std::vector<SharedPtr*>::const_iterator it = rhs.someptrs.begin();
             it != rhs.someptrs.end();
             it++ )
        {
            someptrs.push_back(new SharedPtr( *(*it) ));
        }
#endif
    }

    ~MasterStruct()
    {
#ifdef USE_PTRS
        for (std::vector<SharedPtr*>::const_iterator it = someptrs.begin();
             it != someptrs.end();
             it++ )
        {
            delete (*it);
        }
#endif
    }

    void init(int oneint, int twoint)
    {
        one_int = oneint;
        two_int = twoint;
        one_bool = (oneint&1)==0;
        two_bool = (twoint&1)==1;
        
        int upper = (oneint % 10) + 1;
        for (int i = 0; i < upper; i++ )
        {
            somedoubles.push_back( (double)(i * 3.1415967) );
#ifdef USE_MAPS
            somemap[NTEXT("string index-")+StringUtils::toString(i)] = i;
#endif
#ifdef USE_PTRS
            someptrs.push_back(new SharedPtr(i));
#endif
        }

        somestring = "regular string";
        somewstring = L"wide string";
    }

   MasterStruct& operator=(const MasterStruct& rhs)
   {
       if ( this != &rhs )
       {
           one_int = rhs.one_int;
           two_int = rhs.two_int;
           one_bool = rhs.one_bool;
           two_bool = rhs.two_bool;
           somedoubles = rhs.somedoubles;
           somestring = rhs.somestring;
           somewstring = rhs.somewstring;
#ifdef USE_MAPS
           somemap = rhs.somemap;
#endif
#ifdef USE_PTRS
           for (std::vector<SharedPtr*>::const_iterator it = rhs.someptrs.begin();
                it != rhs.someptrs.end();
                it++ )
           {
               someptrs.push_back(new SharedPtr( *(*it) ));
           }
#endif
       }
       
       return *this;
   }

    bool lock() { return true; }
    
    bool unlock() { return true; }

    size_t size() const
    {
        size_t total = 0;

        total += SerSizeInt(one_int) * 2;
        total += SerSizeBool(one_bool) * 2;
        total += SerSizeVectorDouble(somedoubles);
        total += SerSizestring(somestring);
        total += SerSizewstring(somewstring);
#ifdef USE_MAPS
        total += SerSizeMapStringInt(somemap);
#endif
#ifdef USE_PTRS
        total += SerUtils::getVectorPtrSize(someptrs, count_sharedptr());
#endif
        return total;
    }

	std::ostream& serialize(std::ostream& out) const
    {
        SerWriteInt(out, one_int);
        SerWriteInt(out, two_int);
        SerWriteBool(out, one_bool);
        SerWriteBool(out, two_bool);
        SerWriteVectorDouble(out, somedoubles);
        SerWritestring(out, somestring);
        SerWritewstring(out, somewstring);
#ifdef USE_MAPS
        SerWriteMapStringInt(out, somemap);
#endif
#ifdef USE_PTRS
        SerUtils::writeVectorPtr(out, someptrs, output_sharedptr());
#endif
        return out;
    }

	std::istream& deserialize(std::istream& in)
    {
        SerReadInt(in, one_int);
        SerReadInt(in, two_int);
        SerReadBool(in, one_bool);
        SerReadBool(in, two_bool);
        SerReadVectorDouble(in, somedoubles);
        SerReadstring(in, somestring);
        SerReadwstring(in, somewstring);
#ifdef USE_MAPS
        SerReadMapStringInt(in, somemap);
#endif
#ifdef USE_PTRS
        SerUtils::readVectorPtr(in, someptrs, input_sharedptr());
#endif
        return in;
    }
};

typedef SerUtils::__vector_counter_serializable__<MasterStruct> MasterStructCounter;
typedef SerUtils::__input_serializable__<MasterStruct> MasterStructInputter;
typedef SerUtils::__output_serializable__<MasterStruct> MasterStructOutputter;


class MasterStructContainer : public iSerializable
{
public:
    MasterStructContainer(){}
    ~MasterStructContainer(){}

    MasterStruct& getStruct(const int index)
    {
        return structs[index];
    }

    void reserve(const int sz)
    {
        structs.resize(sz);
    }

    bool lock() { return true; }
    
    bool unlock() { return true; }

    size_t size() const
    {
        return SerUtils::getVectorSize( structs, MasterStructCounter() );
    }

	std::ostream& serialize(std::ostream& out) const
    {
        return SerUtils::writeVector( out, structs, MasterStructOutputter() );
    }

	std::istream& deserialize(std::istream& in)
    {
        return SerUtils::readVector( in, structs, MasterStructInputter() );

    }

    std::vector<MasterStruct> structs;
};

typedef SerUtils::__vector_counter_serializable_ptr__<MasterStructContainer> MasterStructContainerCounter;
typedef SerUtils::__input_serializable_ptr__<MasterStructContainer> MasterStructContainerInputter;
typedef SerUtils::__output_serializable_ptr__<MasterStructContainer> MasterStructContainerOutputter;


void print( const MasterStructContainer& theContainer )
{
#ifdef PRINTOUTPUT
    for ( std::vector<MasterStruct>::const_iterator 
              it = theContainer.structs.begin() ;
          it != theContainer.structs.end(); it++ )
    {
        const MasterStruct& aStruct = *it;

        COUT << NTEXT("one_int: ") << aStruct.one_int << std::endl;
        COUT << NTEXT("two_int: ") << aStruct.two_int << std::endl;

        if ( aStruct.one_bool )
            COUT << NTEXT("one_bool: ") <<  NTEXT("true") 
                      << std::endl;
        else
            COUT << NTEXT("one_bool: ") << NTEXT("false") 
                      << std::endl;

        if ( aStruct.two_bool )
            COUT << NTEXT("two_bool: ") <<  NTEXT("true") 
                      << std::endl;
        else
            COUT << NTEXT("two_bool: ") << NTEXT("false") 
                      << std::endl;

        COUT << NTEXT("somedoubles") << std::endl;
        for ( std::vector<double>::const_iterator it2 = 
                  aStruct.somedoubles.begin() ;
              it2 != aStruct.somedoubles.end(); it2++ )
        {
            COUT << NTEXT("\tvalue: ") << (*it2) << std::endl;
        }

#ifdef USE_MAPS
        COUT << NTEXT("somemap") << std::endl;
        for ( std::map<String, int>::const_iterator it3 = 
                  aStruct.somemap.begin() ;
              it3 != aStruct.somemap.end(); it3++ )
        {
            COUT << NTEXT("\tkey: ") << (*it3).first.c_str()
                      << NTEXT("\tvalue: ") << (*it3).second << std::endl;
        }
#endif

#ifdef USE_PTRS
        COUT << NTEXT("someptrs") << std::endl;
        for (std::vector<SharedPtr*>::const_iterator it4 = aStruct.someptrs.begin();
             it4 != aStruct.someptrs.end();
             it4++ )
        {
            COUT << NTEXT("\tvalue: ") << (*it4)->value << std::endl;
        }
#endif
        COUT << NTEXT("somestring: ") << 
            aStruct.somestring.c_str() << std::endl;

        COUT << NTEXT("somewstring: ") << 
            aStruct.somewstring.c_str() << std::endl;
    }
#endif
}


void createData(
    MasterStructContainer& theOutputContainer)
{
    time_t startt, endt;

    startt = ::time(NULL);

    theOutputContainer.reserve( NUM_TIMES );
    
    for (int i = 0; i < NUM_TIMES; i++ )
    {
        MasterStruct &aStruct = theOutputContainer.getStruct(i);
        aStruct.init(::rand(), ::rand());
    }

    endt = ::time(NULL);
    COUT << NTEXT("Allocated structures in: ") << 
        endt-startt << NTEXT(" seconds") << std::endl;
}

void test_serializable()
{
    StXmlOpener opener;

    RefCountedPtr<SysContext> ctx(new SysContext);

    DOMDocument* doc = DomUtils::getDocument( NTEXT("sertest.xml") );
    if ( doc != NULL )
    {
        // construct the system counter manager
        SerSerializationMgr theMgr;

        // load the xml counter document
        DOMElement* root = doc->getDocumentElement();

        int res = theMgr.init( root, ctx );
        res = theMgr.postInit( root, ctx );
        if ( res == 0 )
        {
            // allocate the source data
            MasterStructContainer sourceContainer;
            createData( sourceContainer );

			COUT << NTEXT("---------------- reference data --------------") << std::endl;
            print( sourceContainer );
			COUT << NTEXT("---------------- reference data --------------") << std::endl;
            
            // finally, test the copy function
            MasterStructContainer theCopyContainer;
            time_t startt, endt;
            startt = ::time(NULL);
            if ( theMgr.copy( sourceContainer, theCopyContainer ) == 0 )
            {
                endt = ::time(NULL);
                COUT << 
                    NTEXT("Copied a serializable world in: ") <<
                    endt-startt << NTEXT(" seconds") << std::endl;
                print( theCopyContainer );
            }
            else
            {
                CERR << NTEXT("problem copying the object world") << std::endl;
            }

	    delete doc;
	}
        else
        {
            CERR << NTEXT("problem initializing the SerSerializationMgr object") << std::endl;
        }
    }
    else
    {
        CERR << NTEXT("Could not parse: 'sertest.xml'") << std::endl;
    }
}


void test_maps()
{
#ifdef USE_MAPS
    time_t startt, endt;
    std::map<String, MasterStruct*> somemap;

    startt = ::time(NULL);

    MasterStruct *aStruct = new MasterStruct;
    aStruct->init(::rand(), ::rand());

    for (int i = 0; i < NUM_TIMES; i++ )
    {
//        MasterStruct *aStruct = nMasterStruct;
//        aStruct->init(::rand(), ::rand());
		somemap[ StringUtils::toString(i) ] = aStruct;
    }

    delete aStruct;

    endt = ::time(NULL);
    COUT << NTEXT("Allocated map structures in: ") << 
        endt-startt << NTEXT(" seconds") << std::endl;

//    for (std::map<std::string, MasterStruct*>::const_iterator it = somemap.begin();
//         it != somemap.end(); it++ )
//    {
//        delete it->second;
//    }
#endif
}

