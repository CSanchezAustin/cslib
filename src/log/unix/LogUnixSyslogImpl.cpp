//      LogUnixSyslogImpl.cpp
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
//              $Revision: 2.1 $
//              $Author: sanchezc $
//              $Date: 2002/09/20 21:30:18 $
//
//      Contents:	file log implementation
//	
//      Current Info:
//      $Header: /cvs/common/cslib/src/log/unix/LogUnixSyslogImpl.cpp,v 2.1 2002/09/20 21:30:18 sanchezc Exp $

#include <cslib.h>

#include <log/unix/LogUnixSyslogImpl.h>
#include <log/LogStdLogger.h>
#include <sys/SysPathMgr.h>
#include <sys/SysContext.h>
#include <util/UtilStringUtils.h>
#include <util/UtilRefCountedPtrInlines.h>
#include <xml/XmlDomUtils.h>

#include <syslog.h>

const String UnixSyslogImpl::NAME = NTEXT("std-syslog-logger");
const String UnixSyslogImpl::LOG_LEVEL = NTEXT("log-level");
const String UnixSyslogImpl::LOG_SERVER = NTEXT("log-server");
const String UnixSyslogImpl::LOG_STDERR = NTEXT("log-stderr");
const String UnixSyslogImpl::LOG_STDOUT = NTEXT("log-stdout");
const String UnixSyslogImpl::LOG_PROCID = NTEXT("log-pid");
const String UnixSyslogImpl::LOG_TRUE_D = NTEXT("true");

UnixSyslogImpl::Initializer UnixSyslogImpl::_init;


UnixSyslogImpl::UnixSyslogImpl() :
    _logAll(false)
{
}


UnixSyslogImpl::~UnixSyslogImpl()
{
}



int 
UnixSyslogImpl::init( 
            const DOMNode* config, 
            RefCountedPtr<SysContext> &ctx )
{
    int res = -1;

    int option = 0;

    if ( (config != NULL) && (config->getNodeType() == DOMNode::ELEMENT_NODE) )
    {
        const DOMElement* configElem = (const DOMElement*)config;

        String val;

        // Get the log level setting [optional]
        DOMNodeList* levels = DomUtils::getNodeList( configElem, LOG_LEVEL );
	if (levels != NULL)
	{
	  for ( unsigned int i = 0, sz = levels->getLength(); i < sz; i++ )
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
		  res = 0;
		}
	    }
	}

        // get the log server [optional]
        DOMElement* logServerElement = NULL;
        if ( DomUtils::selectSingleNode( configElem, LOG_SERVER, (DOMNode**)&logServerElement ) )
        {
            if ( DomUtils::getNodeValue( logServerElement, &val ) )
            {
                _serverName = val;
                res = 0;
            }
        }

        // now stderr logging
        DOMElement* logStdErrNode = NULL;
        if ( DomUtils::selectSingleNode( configElem, LOG_STDERR, (DOMNode**)&logStdErrNode ) )
        {
            if ( DomUtils::getNodeValue( logStdErrNode, &val ) )
            {
                if (LOG_TRUE_D.compare( val ) == 0)
                {
#ifdef LINUX
                    option |= LOG_PERROR;
#endif
                }
                res = 0;
            }
        }
        
        // now stdout logging
        DOMElement* logStdOutNode = NULL;
        if ( DomUtils::selectSingleNode( configElem, LOG_STDOUT, (DOMNode**)&logStdOutNode ) )
        {
            if ( DomUtils::getNodeValue( logStdOutNode, &val ) )
            {
                if (LOG_TRUE_D.compare( val ) == 0)
                {
                    option |= LOG_CONS;
                }
                res = 0;
            }
        }
        
        // now pid
        DOMElement* logPIDNode = NULL;
        if ( DomUtils::selectSingleNode( configElem, LOG_PROCID, (DOMNode**)&logPIDNode ) )
        {
            if ( DomUtils::getNodeValue( logPIDNode, &val ) )
            {
                if (LOG_TRUE_D.compare( val ) == 0)
                {
                    option |= LOG_PID;
                }
                res = 0;
            }
        }        
    }

    // use the options to open the system log
    if ( _serverName.length() > 0 )
    {
        ::openlog( _serverName.c_str(), option, LOG_USER );
        ::setlogmask( LOG_UPTO(LOG_DEBUG) );
    }
    
    return res;
}


/*
	Writes a message to the log file
    @param msg the message to write
*/
size_t 
UnixSyslogImpl::writeLogEntry( 
            const String &msg, 
            const int level,
            const String &source )
{
    if ( _logAll || ( _levels.find( level ) != _levels.end() ) )
    {
        int sysLevel = LOG_ERR;

        switch ( level )
        {
            case StdLogger::LOGC_ERR:
                sysLevel = LOG_ERR;
                break;

            case StdLogger::LOGC_WARN:
                sysLevel = LOG_WARNING;
                break;

            case StdLogger::LOGC_DEBUG:
                sysLevel = LOG_DEBUG;
                break;

            case StdLogger::LOGC_MSG:
                sysLevel = LOG_INFO;
                break;

            default:
                sysLevel = LOG_ERR;
                break;
        }

        ::syslog( sysLevel, NTEXT("%s"), msg.c_str() );
    }

    return msg.size();
}

