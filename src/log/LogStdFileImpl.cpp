//      LogStdFileImpl.cpp
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
//      Contents:	file log implementation
//	
//      Current Info:
//      $Header:$

#include <iostream>

#include <cslib.h>

#include <log/LogStdFileImpl.h>
#include <log/LogStdLogger.h>
#include <sys/SysPathMgr.h>
#include <sys/SysContext.h>
#include <util/UtilStringUtils.h>
#include <util/UtilRefCountedPtrInlines.h>
#include <xml/XmlDomUtils.h>

const String StdFileImpl::NAME = NTEXT("std-file-logger");
const String StdFileImpl::LOG_LEVEL = NTEXT("log-level");
const String StdFileImpl::FILE_NAME = NTEXT("log-file");
const String StdFileImpl::NUM_FILES = NTEXT("num-logs");
const String StdFileImpl::FILE_SIZE = NTEXT("log-size");


StdFileImpl::Initializer StdFileImpl::_init;


StdFileImpl::StdFileImpl() :
    _logAll(false)
{
		_SizePerLog=0;
}


StdFileImpl::~StdFileImpl()
{
    if ( _logFile.is_open() )
    {
        _logFile.close();
    }
}



int 
StdFileImpl::init( 
    const DOMNode* config, 
    RefCountedPtr<SysContext>& ctx )
{
    int res = -1;
    
    if ( (config != NULL) && (config->getNodeType() == DOMNode::ELEMENT_NODE) )
    {
        const DOMElement* configElem = (const DOMElement*)config;
        
        String val;
        
        // Get the log level setting [optional]
        DOMNodeList* levels = DomUtils::getNodeList( configElem, LOG_LEVEL );
        if ( levels != NULL )
        {
            for ( XMLSize_t i = 0, sz = levels->getLength(); i < sz; i++ )
            {
                if ( DomUtils::getNodeValue( (DOMElement*)levels->item( i ), &val ) )
                {
                    int level = StringUtils::toInt( val );
                    if ( level >= StdLogger::LOGC_ALL )
                    {
                        _logAll = true;
                    }
                    else
                    {
                        _levels.insert( level );
                    }
                }
            }
        }
        
        // get the log file path [required]
        DOMElement* logFileElement = NULL;
        String logFileName;
        if ( DomUtils::selectSingleNode( configElem, FILE_NAME, (DOMNode**)&logFileElement ) )
        {
            if ( DomUtils::getNodeValue( logFileElement, &val ) )
            {	
				RefCountedPtr<SysPathMgr> paths;
				REFCOUNTED_CAST(iSysComponent, SysPathMgr, ctx->getComponent(SysPathMgr::getRegistryName()), paths);
                if ( paths != NULL )
                {
                    logFileName = paths->getPath( val );
                }
                else
                {
                    logFileName = val;
                }

                DOMElement* numFilesElement = NULL;
                DOMElement* sizeFileElement = NULL;

                int numLogs = 0;
                _SizePerLog = 0;
                if ( DomUtils::selectSingleNode( configElem, NUM_FILES, (DOMNode**)&numFilesElement ) )
                {
                    if (DomUtils::getNodeValue( numFilesElement, &val ) )
                    {
                        numLogs = StringUtils::toInt(val);
                    }
                    else 
                    {
                        numLogs = 1;
                    }
                }
                if ( DomUtils::selectSingleNode( configElem, FILE_SIZE, (DOMNode**)&sizeFileElement ) )
                {
                    if (DomUtils::getNodeValue( sizeFileElement, &val ) )
                    {
                        _SizePerLog = StringUtils::toLong(val);
                    }				
                }
                
                if ( (numLogs <= 0) || (_SizePerLog <= 0) )
                {
#ifdef __GNUG__
                    _logFile.open( logFileName.c_str(), std::ios::out | std::ios::app );
#else
                    _logFile.open( logFileName.c_str(), std::ios_base::out | std::ios_base::app );
#endif
                    _logFileNames.push_back( logFileName );
                    _logIndex = 0;
                    _SizePerLog = 0;
                    res = 0;
                }
                else
                {
                    int i;
                    if ( numLogs == 1 )
                    {
                        _logFileNames.push_back( logFileName );
                    }
                    else
                    {
                        for ( i = 1; i <= numLogs; i++ )
                        {
                            _logFileNames.push_back( logFileName + StringUtils::toString(i)+ NTEXT(".txt") );
                        }
                    }

                    FileAttributes fattrs;
                    if ( !FileUtils::getAttributes( _logFileNames[0].c_str(), fattrs ) )
                    {
#ifdef __GNUG__
                        _logFile.open(_logFileNames[0].c_str(), std::ios::out | std::ios::app );
#else
                        _logFile.open(_logFileNames[0].c_str(), std::ios_base::out | std::ios_base::app );
#endif
                        _logIndex = 0;
                        res = 0;
                    }
                    else 
                    {
                        int useindex = 0;
                        struct tm filetime = fattrs.getModifyTime();
                        time_t currtime = ::time(NULL);
                        double besttime = ::difftime( currtime, mktime(&filetime) );
                        for (i = 1; i < numLogs; i++)
                        {
                            if ( FileUtils::getAttributes( _logFileNames[i].c_str(), fattrs ) )
                            {
                                filetime = fattrs.getModifyTime();
                                double thisdiff = ::difftime( currtime, ::mktime(&filetime) );
                                if ( thisdiff < besttime )
                                {
                                    besttime = thisdiff;
                                    useindex = i;
                                }
                            }
                        }

                        FileUtils::getAttributes( _logFileNames[useindex].c_str(), fattrs );
                        if ( (int) fattrs.getFileSize() < _SizePerLog )
                        {
#ifdef __GNUG__
                            _logFile.open(_logFileNames[useindex].c_str(), std::ios::out | std::ios::app );
#else
                            _logFile.open(_logFileNames[useindex].c_str(), std::ios_base::out | std::ios_base::app );
#endif
                            _logIndex = useindex;
                            res = 0;
                        }
                        else
                        {
                            if ( (useindex + 1) >= (int)_logFileNames.size() )
                            {
                                useindex = 0;
                            }
                            else
                            {
                                useindex += 1;
                            }
#ifdef __GNUG__
                            _logFile.open(_logFileNames[useindex].c_str(), std::ios::out | std::ios::trunc );
#else
                            _logFile.open(_logFileNames[useindex].c_str(), std::ios_base::out | std::ios_base::trunc );
#endif
                            _logIndex = useindex;
                            res = 0;
                            
                        }
                    }
                    
                }
            }
        }
    }
    
    return res;
}


/*
	Writes a message to the log file
    @param msg the message to write
*/
size_t 
StdFileImpl::writeLogEntry( 
            const String &msg, 
            const int level,
            const String &source )
{
    if ( _logAll || ( _levels.find( level ) != _levels.end() ) )
    {
        _logFile << msg.c_str() << std::endl <<std::flush;
    }

    if ( (_SizePerLog > 0) && (_logFile.tellp() > _SizePerLog) )
    {
        _logFile.close();

        if ( (_logIndex < 0) || ((_logIndex + 1) >= (int)_logFileNames.size()) )
        {
            _logIndex = 0;
        }
        else
        {
            _logIndex += 1;
        }

#ifdef __GNUG__
        _logFile.open(_logFileNames[_logIndex].c_str(), std::ios::out | std::ios::trunc );
#else
        _logFile.open(_logFileNames[_logIndex].c_str(), std::ios_base::out | std::ios_base::trunc );
#endif
	}

    return msg.size();
}
