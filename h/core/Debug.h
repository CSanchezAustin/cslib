#ifndef Debug_H_LOADED
#define Debug_H_LOADED
// [
// $        Debug.h
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
//         RCS:
//                 $Revision: 2.2 $
//                 $Author: sanchezc $
//                 $Date: 2002/09/26 17:19:31 $
// ?
//         Contents: Debug macros
//
//         Purpose:
//
//         Usage Notes:
//
//                 
// ?
// !        Changes History
//     $Header: /cvs/common/cslib/h/core/Debug.h,v 2.2 2002/09/26 17:19:31 sanchezc Exp $
//         
// ]


#include <iostream>
#include <cstdlib>
#include <stdio.h>
#include <signal.h>

#include <cslib.h>

// debug macros
namespace EXPORT_DLL Debug
{

    // Error handling defs
    typedef enum
    {
        ERROR_TYPE = -1,
        WARN_TYPE = 0,
        MSG_TYPE = 1,
        DEBUG_TYPE = 2
        
    } LogType;

    /**
       Called to set the debug output device
       By default this will be cout and cerr
       This is to be used in conjunction with the
       COUT and CERR macros

       NOTE: DO NOT CLOSE OR DESTROY THE STREAM
       WITHOUT RESETTING THE DEBUG STREAM TO A NEW
       STREAM
     */
    void setOutputStream( OSTREAM &out );
    OSTREAM& getOutputStream();

    void setInputStream( ISTREAM &in );
    ISTREAM& getInputStream();
    
    void setErrStream( OSTREAM &out );
    OSTREAM& getErrStream();
    
    /**
       Called to get/set the runtime debug level
       that applications can use to selectively 
       enable debugging output
    */
    int  getLevel();
    void setLevel( const int newLevel );
    
    /**
       called to set a debugger breakpoint
    */
    void breakpoint();
    
    /**
       These functions get and set the module name of
       processes that is interfacing with CSLIB
    */
    const String& getCurrentModule();
    void setCurrentModule(const String& mod);

    void traceout(
            const String& fileName, 
            const int lineNum, 
            const LogType sev, 
            const String& msg);
}


/**
   COUT and CERR macros are to be used 
   by applications to control debug output
*/
#define COUT Debug::getOutputStream()
#define CIN  Debug::getInputStream()
#define CERR Debug::getErrStream()

/**
   Used to check the debug level of the application
*/
#define ISDBG(x) (Debug::getLevel() > (x))

/**
   Used to break into the debugger if the runtime
   level allows
 */
#define DBGBREAK(x) \
    if ( Debug::getLevel() >= (x) ) \
    {    Debug::breakpoint(); }

//
// MACROS
//

// Error handling
// Here's the levels for which information will be logged in cslib
// Errors always get logged
// warnings will get logged of level > 0
// messages will be logged if level > 100
// debugging will be logged if level > 200
#define DBG_LOGERR(msg) Debug::traceout(NTEXT(__FILE__), __LINE__, Debug::ERROR_TYPE, msg)
#define DBG_LOGWRN(msg) if ( Debug::getLevel() > (Debug::WARN_TYPE*100)) { Debug::traceout(NTEXT(__FILE__), __LINE__, Debug::WARN_TYPE, msg); }
#define DBG_LOGMSG(msg) if ( Debug::getLevel() > (Debug::MSG_TYPE*100)) { Debug::traceout(NTEXT(__FILE__), __LINE__, Debug::MSG_TYPE, msg); }
#define DBG_LOGDBG(msg) if ( Debug::getLevel() > (Debug::DEBUG_TYPE*100)) { Debug::traceout(NTEXT(__FILE__), __LINE__, Debug::DEBUG_TYPE, msg); }


// Debug_H_LOADED
#endif
