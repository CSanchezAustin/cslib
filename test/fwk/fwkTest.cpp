//      fwkTest.cpp
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

#include "fwk/ifwkConduitListener.h"
#include "fwk/fwkConduit.h"

typedef enum enumMyEventTypes
{
    keTest1 = 1,
    keTest2 = 2
};

void test_conduit();

int main()
{
  OFSTREAM out(NTEXT("debug-log"));
  OFSTREAM err(NTEXT("err.log"));
//    Debug::setOutputStream( out );
//    Debug::setErrStream( err );

    Debug::setCurrentModule( NTEXT("FWKTEST") );

    test_conduit();
    return 0;
}



class MyConduitListener : public iConduitListener<enumMyEventTypes>
{
public:
    MyConduitListener(int num) :
        _num(num)
    {
    }
    
    ~MyConduitListener()
    {
    }

    /**
       notify interface
    */
    void doNotify( const enumMyEventTypes evtType, const void* data )
    {
        COUT << NTEXT("Handler: ") << _num <<  
            NTEXT(", Event type: ") << (int)evtType << 
            NTEXT(", data: ") << (*((String*)data)).c_str() << std::endl;
    }
private:
    int _num;
};

void
test_conduit()
{
    MyConduitListener myListener(1);
    MyConduitListener myListener2(2);
    String myMessage1(NTEXT("This is a message"));
    String myMessage2(NTEXT("This is a the second message"));
    Conduit<enumMyEventTypes> cond;

    cond.addListener( &myListener );
    cond.addListener( &myListener2 );
    cond.notify( keTest1, &myMessage1 );
    cond.notify( keTest2, &myMessage2 );

    cond.removeListener( &myListener );
    cond.notify( keTest1, &myMessage1 );
    cond.notify( keTest2, &myMessage2 );
}
