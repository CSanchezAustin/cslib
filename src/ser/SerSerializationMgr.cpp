//      SerSerializationMgr.cpp
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
//
//      Contents:	Implementation of the system context registry
//                  this registry keeps commonly used objects that
//                  are passed to other library components
//	
#include <cslib.h>

#include <iostream>
#include <sstream>
#include <memory>

#include <xercesc/dom/DOMNode.hpp>
#include <xercesc/dom/DOMNodeList.hpp>
#include <xercesc/dom/DOMElement.hpp>
#include <xercesc/dom/DOMDocument.hpp>

#include <ser/SerSerializationMgr.h>
#include <ser/SerStdLib.h>
#include <log/LogStdLogger.h>
#include <sys/SysContext.h>
#include <sys/SysPathMgr.h>
#include <util/UtilStringUtils.h>
#include <util/UtilRefCountedPtrInlines.h>
#include <xml/XmlDomUtils.h>


const String SerSerializationMgr::SER_MGR_NAME = NTEXT("ser-mgr");
int SerSerializationMgr::SER_FORMAT_SENTINEL = 0xB0B0F0F0;
int SerSerializationMgr::SER_FORMAT_CURVER = 0x00010000;
int SerSerializationMgr::SER_FORMAT_MINVER = 0x00010000;
int SerSerializationMgr::SER_FORMAT_MAXVER = 0x00010000;

DECLARE_SYS_COMPONENT(SerSerializationMgr)

SerSerializationMgr::SerSerializationMgr()
{
}


SerSerializationMgr::~SerSerializationMgr()
{
}


int 
SerSerializationMgr::postInit( 
    const DOMNode* config, 
    RefCountedPtr<SysContext>& ctx )
{
    if ( _logger == NULL )
    {
        REFCOUNTED_CAST(iSysComponent, StdLogger, ctx->getComponent(StdLogger::getRegistryName()), _logger);
    }

    if ( _paths == NULL )
    {
        REFCOUNTED_CAST(iSysComponent, SysPathMgr, ctx->getComponent(SysPathMgr::getRegistryName()), _paths);
    }

    return 0;

}

/**
   serialize
   write out theWorld object to the given output source
*/
int 
SerSerializationMgr::serialize( 
    iSerializable& theWorld, 
    std::ostream& out )
{
    // make sure we have exclusive access to the world
    StSerializableLocker lock(theWorld);

    // get the output source
    if ( !out.good() )
    {
        CBLOGERR( _logger, 
                  NTEXT("SerSerializationMgr::serialize: failed to get the output stream") );
        return SER_BAD_STREAM;
    }

    //  write out our sentinel to the stream
    // so that we can verify the stream when 
    // we try to deserialize the source
    if ( !SerWriteInt( out, SER_FORMAT_SENTINEL ).good() )
    {
        CBLOGERR( _logger, 
                  NTEXT("SerSerializationMgr::serialize: failed to write out the sentinel") );
        return SER_WRITE_ERR;
    }

    // write out the version info
    if ( !SerWriteInt( out, SER_FORMAT_CURVER).good() )
    {
        CBLOGERR( _logger, 
                  NTEXT("SerSerializationMgr::serialize: failed to write out the version") );
        return SER_WRITE_ERR;
    }

    // then write out the world
    if ( !theWorld.serialize( out ).good() )
    {
        CBLOGERR( _logger, 
                  NTEXT("SerSerializationMgr::serialize: failed to serialize the world") );
        return SER_WRITE_ERR;
    }
    
    return SER_NOERR;
}


/**
   deserialize
   read in the  theWorld object from the given input source
*/
int 
SerSerializationMgr::deserialize( 
    iSerializable& theWorld, 
    std::istream& in )
{
    // make sure we have exclusive access to the world
    StSerializableLocker lock(theWorld);

    if ( !in.good() )
    {
        CBLOGERR( _logger, 
                  NTEXT("SerSerializationMgr::deserialize: failed to get the input stream") );
        return SER_BAD_STREAM;
    }

    // read in the sentinel to see if this looks like
    // a good stream
    int sentinel = 0;
    if ( !SerReadInt( in, sentinel ).good() )
    {
        CBLOGERR( _logger, 
                  NTEXT("SerSerializationMgr::deserialize: failed to read the sentinel") );
        return SER_READ_ERR;
    }

    if ( sentinel != SER_FORMAT_SENTINEL )
    {
        CBLOGERR( _logger, 
                  NTEXT("SerSerializationMgr::deserialize: the sentinel doesn't match") );
        return SER_INVALID_FORMAT;
    }

    // now the version information
    int version = 0;
    if ( !SerReadInt( in, version).good() )
    {
        CBLOGERR( _logger, 
                  NTEXT("SerSerializationMgr::deserialize: failed to read the version") );
        return SER_READ_ERR;
    }

    if  ( version  < SER_FORMAT_MINVER || version > SER_FORMAT_MAXVER )
    {
        CBLOGERR( _logger, 
                  NTEXT("SerSerializationMgr::deserialize: the version is out of range") );
        return SER_INVALID_FORMAT;
    }

    // then write out the world
    if ( !theWorld.deserialize( in ).good() )
    {
        CBLOGERR( _logger, 
                  NTEXT("SerSerializationMgr::deserialize: failed to deserialize the world") );
        return SER_READ_ERR;
    }

    return SER_NOERR;
}


/**
   copy
   copy one serializable world to another. Start by locking the world,
   creating a memory buffer and stream the data to the buffer. Then
   create a new memory input buffer and stream in the data to the
   output world
*/
int 
SerSerializationMgr::copy( 
    iSerializable& theInputWorld, 
    iSerializable& theOutputWorld )
{
    // make sure we have exclusive access to the world
    StSerializableLocker lock( theInputWorld );
    StSerializableLocker lock2( theOutputWorld );

    std::ostringstream out;
    int res = serialize( theInputWorld, out );
    if ( res != SER_NOERR )
    {
        return res;
    }

    std::istringstream in(out.str());
    return deserialize( theOutputWorld, in );
}


size_t 
SerSerializationMgr::getMgrMem()
{
    return 
        SerSizeInt(SER_FORMAT_SENTINEL) +
        SerSizeInt(SER_FORMAT_CURVER);
}
