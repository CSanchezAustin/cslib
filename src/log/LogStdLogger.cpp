//      LogStdLogger.cpp
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
//      Contents:	standard logger implementation
//	
//      Current Info:
//      $Header:$

#include <cslib.h>

#include <xercesc/dom/DOMNode.hpp>
#include <xercesc/dom/DOMElement.hpp>
#include <xercesc/dom/DOMNodeList.hpp>

#include <log/LogStdLogger.h>
#include <log/LogStdFormatter.h>
#include <log/iLoggerImpl.h>
#include <log/LogImplementationFactory.h>
#include <sys/SysContext.h>
#include <thd/ThdPool.h>
#include <thd/ThdAutoLock.h>
#include <thd/ThdThread.h>
#include <thd/ThdManager.h>
#include <util/UtilMapping.h>
#include <util/UtilStringUtils.h>
#include <util/UtilRefCountedPtrInlines.h>
#include <xml/XmlDomUtils.h>

const String StdLogger::LOG_CFGKEY = NTEXT("std-logger");
const String StdLogger::LOG_LEVEL = NTEXT("level");
const String StdLogger::LOG_FORMAT = NTEXT("format");
const String StdLogger::LOG_STDERR = NTEXT("stderr");
const String StdLogger::LOG_STDOUT = NTEXT("stdout");
const String StdLogger::LOG_THREADED = NTEXT("threaded");
const String StdLogger::LOG_RSRCFILE = NTEXT("rsrcfile");
const String StdLogger::LOG_IMPL = NTEXT("impl");
const String StdLogger::LOG_IMPLNAMEATT = NTEXT("name");

const String StdLogger::LOG_FORMAT_D = NTEXT("$0$-$1$ ($3$:$2$) $5$:$4$");
const String StdLogger::LOG_TRUE_D = NTEXT("true");

DECLARE_SYS_COMPONENT(StdLogger)

/**
    construct a Standard logger
    @param level level to log at
    @param stderr log to stderr also
    @param stdout log to std out
*/
StdLogger::StdLogger(
            const int level, 
            const bool err, 
            const bool out ) :
    _mLogLevel(level),
    _mFmt(new StdFormatter(LOG_FORMAT_D)),
    _exitEvent(NULL, true, true) // make sure it's initially signalled
{
    logStdErr(err);
    logStdOut(out);
}


StdLogger::~StdLogger()
{
    // shutdown the message queue and wait for the thread to exit
    _mq.down();
    _exitEvent.wait();
}


/**
    set the logger format
    @param fmt the format string
*/
void 
StdLogger::setFormat(
            const String &fmt )
{
    _mFmt = new StdFormatter(fmt);
}


/**
    set the logger implementation
    @param imp the log implementation
*/
void 
StdLogger::logImp(
            RefCountedPtr<iLoggerImpl> &imp )
{
    _mImpls.push_back( imp );
}


/**
    log to stderr
    @param stderr turn on error logging
*/
void 
StdLogger::logStdErr(
            const bool err )
{
    _logStdErr = err;
}


/**
    log to stdout
    @param stdout turn on error logging
*/
void 
StdLogger::logStdOut(
            const bool out )
{
    _logStdOut = out;
}



/**
    log a single line item
*/
void 
StdLogger::logOne( 
    const String &logItem,
    const int level,
    const String &source )
{
    if ( _mImpls.size() > 0 )
    {
        for ( std::vector< RefCountedPtr<iLoggerImpl> >::iterator it = _mImpls.begin();
              it != _mImpls.end(); it++ )
        {
            (*it)->writeLogEntry( logItem, level, source );
        }
    }
    
    if ( _logStdErr )
    {
		CERR << logItem.c_str() << std::endl << std::flush;
    }
    
    if ( _logStdOut )
    {
        COUT << logItem.c_str() << std::endl <<std::flush;
    }
}


/**
    the threaded logging function
*/
unsigned 
StdLogger::processMessages()
{
    _exitEvent.reset();

    RefCountedPtr<LogMessage> item;

    for ( enumSyncOpResult res = _mq.remove( item );
          res == SYNCOP_SUCCESS;
          item = NULL, res = _mq.remove( item ) )
    {
        logOne( item->getMessage(), item->getLevel(), item->getSource() );
    }

    _exitEvent.signal();

    return 0;
}


/**
	this logs a message
    @param msg message to log
    @param level level to log at
*/
void 
StdLogger::logMessage(
            const String &msg,
            const int level )
{
    if ( _mLogLevel >= level )
    {
        String logItem = _mFmt->format(msg, level);
        if ( _runner != NULL )
        {
            RefCountedPtr<LogMessage> theMessage(
                new LogMessage( logItem, level, Debug::getCurrentModule() ) );
            _mq.insert( theMessage );
        }
        else
        {
            ThdAutoCriticalSection guard(_lock);
            logOne( logItem, level, Debug::getCurrentModule() );
        }
    }
}


/**
	this logs a message
    @param msgid message id to log
    @param level level to log at
*/
void 
StdLogger::logMessage( 
            const int msgId, 
            const int level )
{
	String logStr;
	bool bFound = getCacheMessage( msgId, logStr );
	if ( bFound )
	{
		logMessage( logStr, level );
	}
}



/**
	this logs a message
    @param msgid message id to log
	@param appendStr is a string to append to the resource string
    @param level level to log at
*/
void 
StdLogger::logMessage( 
            const int msgId, 
            const String& appendStr,
            const int level )
{	
	String resMsg;
	bool bFound = getCacheMessage( msgId, resMsg );
	if ( bFound )
	{
		logMessage( resMsg + appendStr, level );
	}
}


/**
	this function manages a cache of strings. If the string is found 
	in the cache, it is simply returned. If not found, then it is retrieved
	and put in the cache. In both cases the string is returned and the 
	result of the function is true (success)
    @param msgid message id to retrieve if already in the cache
    @param ioCachedString is the output string
*/bool StdLogger::getCacheMessage(
				const int msgId, 
				String& ioCachedString )
{
	ASSERT_D ( _rsrcFile != NULL );

	bool bFound = false;

	if (_cacheLock.acquireReadLock())
	{
		std::map<int,String>::iterator it = _stringCache.find( msgId );
		if ( it != _stringCache.end() )
		{
			bFound = true;
			ioCachedString = (*it).second;
			_cacheLock.releaseReadLock();
		}
		else
		{
			_cacheLock.releaseReadLock();

			// retrieve the message
			ioCachedString = _rsrcFile->getString( msgId );
			if (ioCachedString.length() > 0 )
			{
				// and insert it into the cache
				ThdAutoWriteLock writeGuard( _cacheLock );
				_stringCache[ msgId ] = ioCachedString;
				bFound = true;
			}
			else
			{
				DBG_LOGERR(NTEXT("Did not find string"));
			}
		}
	}


	return bFound;
}


int 
StdLogger::init( 
            const DOMNode* config, 
            RefCountedPtr<SysContext>& ctx )
{
 
    if (( config != NULL ) && ( config->getNodeType() == DOMNode::ELEMENT_NODE ))
    {
        const DOMElement* configElem = (const DOMElement*)config;

        // first look for the log level setting
        String val;
        DOMNodeList* nodes = DomUtils::getNodeList( configElem, LOG_LEVEL );
        if ( DomUtils::getNodeValue( (const DOMElement*)nodes->item( 0 ), &val ) )
        {
            setLogLevel( StringUtils::toInt( val ) );
        }

        // now format
        val.erase();
        nodes = DomUtils::getNodeList( configElem, LOG_FORMAT );
        if ( DomUtils::getNodeValue( (const DOMElement*)nodes->item( 0 ), &val ) )
        {
            setFormat( val );
        }

        // now stderr logging
        val.erase();
        nodes = DomUtils::getNodeList( configElem, LOG_STDERR );
        if ( DomUtils::getNodeValue( (const DOMElement*)nodes->item( 0 ), &val ) )
        {
            logStdErr( LOG_TRUE_D.compare( val ) == 0 );
        }

        // now stdout logging
        val.erase();
        nodes = DomUtils::getNodeList( configElem, LOG_STDOUT );
        if ( DomUtils::getNodeValue( (const DOMElement*)nodes->item( 0 ), &val ) )
        {
            logStdOut( LOG_TRUE_D.compare( val ) == 0 );
        }

        // use of the thread pool is optional
        val.erase();
        nodes = DomUtils::getNodeList( configElem, LOG_THREADED );
        if ( DomUtils::getNodeValue( (const DOMElement*)nodes->item( 0 ), &val ) )
        {
            if( LOG_TRUE_D.compare( val ) == 0 )
            {
                RefCountedPtr<iRunnable> logRunner(new LoggerRunner( *this ));
                _runner = ThdManager::newWorkerThread( logRunner );
            }
        }
		
        // use of the resource file is optional
        val.erase();
        nodes = DomUtils::getNodeList( configElem, LOG_RSRCFILE );
        if ( DomUtils::getNodeValue( (const DOMElement*)nodes->item( 0 ), &val ) )
        {
			_rsrcFile = RefCountedPtr<DllLoader>(new DllLoader(val));
		}
    }

    return 0; // the logger can always operate with some defaults
}


/*
    Called after initialization of the system context. this allows
    system components to utilize other components as necessary
*/
int 
StdLogger::postInit( 
            const DOMNode* config, 
            RefCountedPtr<SysContext>& ctx )
{
    int res = -1;

    if (( config != NULL ) && ( config->getNodeType() == DOMNode::ELEMENT_NODE ))
    {
        const DOMElement* configElem = (const DOMElement*)config;

        DOMNodeList* impls = DomUtils::getNodeList( configElem, LOG_IMPL );
        for ( XMLSize_t i = 0, sz = impls->getLength(); i < sz; i++ )
        {
            Mapping attrs;
            const DOMElement* implElem = (const DOMElement*)impls->item(i);
            if ( DomUtils::getNodeValue( implElem, NULL, &attrs ) )
            {
                String implName;
                attrs.getIfAvailable(LOG_IMPLNAMEATT, implName);

                RefCountedPtr<iLoggerImpl> theImpl( LogImplementationFactory::createObject( implName ) );
                if ( theImpl != NULL && theImpl->init( implElem, ctx ) == 0 )
                {
                    logImp( theImpl );
                    res = 0;
                }
            }
        }
    }

    return res;
}
