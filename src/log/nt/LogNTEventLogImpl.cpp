//      LogNTEventLogImpl.cpp
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

#include <cslib.h>

#include <log/nt/LogNTEventLogImpl.h>
#include <log/nt/CslibMsg.h>
#include <log/LogStdLogger.h>
#include <sys/SysPathMgr.h>
#include <sys/SysContext.h>
#include <util/UtilStringUtils.h>
#include <util/UtilRefCountedPtrInlines.h>
#include <xml/XmlDomUtils.h>

const String NTEventLogImpl::NAME = NTEXT("std-ntevent-logger");
const String NTEventLogImpl::LOG_LEVEL = NTEXT("log-level");
const String NTEventLogImpl::LOG_SERVER = NTEXT("log-server");
const String NTEventLogImpl::LOG_SOURCE = NTEXT("log-source");
const String NTEventLogImpl::LOG_MSGDLL = NTEXT("log-msg-dll");

NTEventLogImpl::Initializer NTEventLogImpl::_init;


NTEventLogImpl::NTEventLogImpl() :
    _logAll(false)
{
}


NTEventLogImpl::~NTEventLogImpl()
{
}



int 
NTEventLogImpl::init( 
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

        // get the log server [required]
        DOMElement* logServerElement = NULL;
        if ( DomUtils::selectSingleNode( configElem, LOG_SERVER, (DOMNode**)&logServerElement ) )
        {
            if ( DomUtils::getNodeValue( logServerElement, &val ) )
            {
                _serverName = val;
            }
        }
        
        // get the log source [required]
        DOMElement* logSourceElement = NULL;
        if ( DomUtils::selectSingleNode( configElem, LOG_SOURCE, (DOMNode**)&logSourceElement ) )
        {
            if ( DomUtils::getNodeValue( logSourceElement, &val ) )
            {
                _sourceName = val;
            }
        }

        // get the log message DLL [required]
        DOMElement* logMsgDllElement = NULL;
        if ( DomUtils::selectSingleNode( configElem, LOG_MSGDLL, (DOMNode**)&logMsgDllElement ) )
        {
            if ( DomUtils::getNodeValue( logMsgDllElement, &val ) )
            {
                _sourceMsgDll = val;
            }
        }

		if ( _serverName.length() && _sourceName.length() && _sourceMsgDll.length() )
		{
			_logger = RefCountedPtr<CEventLog>(new CEventLog(_sourceName, _serverName));
			ASSERT_D(_logger != NULL );
			_logger->RegisterLog( _sourceMsgDll.c_str() );
			res = 0;
		}	
	}
    
    return res;
}


/*
	Writes a message to the log file
    @param msg the message to write
*/
size_t 
NTEventLogImpl::writeLogEntry( 
            const String &msg, 
            const int level,
            const String &source )
{
    if ( _logAll || ( _levels.find( level ) != _levels.end() ) )
    {
        DWORD eventId;
        WORD eventType;
        switch ( level )
        {
            case StdLogger::LOGC_ERR:
                eventType = EVENTLOG_ERROR_TYPE;
                eventId = CSLIBMSG_ERROR;
                break;

            case StdLogger::LOGC_WARN:
                eventType = EVENTLOG_WARNING_TYPE;
                eventId = CSLIBMSG_WARNING;
                break;

            case StdLogger::LOGC_DEBUG:
                eventType = EVENTLOG_INFORMATION_TYPE;
                eventId = CSLIBMSG_DEBUG;
                break;

            case StdLogger::LOGC_MSG:
                eventType = EVENTLOG_INFORMATION_TYPE;
                eventId = CSLIBMSG_INFO;
                break;

            default:
                eventType = EVENTLOG_INFORMATION_TYPE;
                eventId = CSLIBMSG_USERDEFINED;
                break;
        }

        _logger->LogEvent( eventId, msg.c_str(), eventType );
    }

    return msg.size();
}

