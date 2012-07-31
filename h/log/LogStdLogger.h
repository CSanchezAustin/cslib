#ifndef LogStdLogger_H_LOADED
#define LogStdLogger_H_LOADED
// [
// $     LogStdLogger.h
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
// ?
//     Contents:	generic logger class
//
//    Usage Notes: 
//     This system component supports the following configuration directives
//     XML Root node: <std-logger>
//                     Name        Type        Default                 Description
//     Config Element: level       int         16                      Log level setting
//     Config Element: format      string      $0$ $1$ $2$:$3$\t$4$    line format for a log entry
//     Config Element: stderr      bool        false                   log to standard error
//     Config Element: stdout      bool        false                   log to standard output
//
//    Reference schema:
//      <xsd:element name="std-logger">
//  		<xsd:complexType content="elementOnly">
//      		<xsd:element name="level" type="xsd:integer" minOccurs="1" maxOccurs="1"/>
//    			<xsd:element name="format" type="xsd:string" minOccurs="1" maxOccurs="1"/>
//    			<xsd:element name="stderr" type="xsd:string" minOccurs="1" maxOccurs="1"/>
//    			<xsd:element name="stdout" type="xsd:string" minOccurs="1" maxOccurs="1"/>
//    			<xsd:element name="threaded" type="xsd:string" minOccurs="1" maxOccurs="1"/>
//      		<xsd:element name="rsrcfile" type="xsd:string" minOccurs="1" maxOccurs="1"/>
//			</xsd:complexType>
//		</xsd:element>
//
//    Example:
//      <std-logger>
//          <level>16</level>
//          <format>$0$ $1$ $2$:$3$\t$4$</format>
//          <stderr>false</stderr>
//          <stdout>false</stdout>
//          <threaded>false</threaded>
//          <rsrcfile>CslibMsg.dll</rsrcfile>
//          <impl name="std-file-logger">
//              <log-level>2</log-level>
//              <log-file>$log_dir\ControllerLog.txt</log-file>
//          </impl>
//       </std-logger>
//
// ?
// !        Changes History
//      $Header: $
//         
// ]

#include <vector>

#include <cslib.h>
#include <pat/PatRunnable.h>
#include <sys/iSysComponent.h>
#include <sys/SysComponentFactory.h>
#include <thd/ThdQueue.h>
#include <thd/ThdEvent.h>
#include <thd/ThdCriticalSection.h>
#include <util/UtilRefCountedPtr.h>
#include <util/UtilDllLoader.h>


class iLoggerImpl;
class StdFormatter;
class ThdThread;


class EXPORT_DLL StdLogger : public iSysComponent
{
public:
    enum
    {
        LOGC_ERR	= 0,
        LOGC_MSG	= 1,
        LOGC_WARN	= 2,
        /* ( 3-99 are user defined) */
        LOGC_DEBUG	= 100,
        LOGC_ALL    = 0xFFFF
    };

public:
    /**
       construct a Standard logger
       @param level level to log at
       @param stderr log to stderr also
       @param stdout log to std out
    */
    StdLogger(  const int level = LOGC_MSG, 
                const bool err = false, 
                const bool out = false );
    
    virtual ~StdLogger();
    
    /**
       set the logger format
       @param fmt the format string
    */
    void setFormat(const String& fmt);
    
    
    /**
       set the logger implementation
       @param imp the log implementation
    */
    void logImp( RefCountedPtr<iLoggerImpl> &imp );
    
    
    /**
       log to stderr
       @param stderr turn on error logging
    */
    void logStdErr(const bool err);
    
    
    /**
       log to stdout
       @param stdout turn on error logging
    */
    void logStdOut(const bool out);
    
    
    /**
       get the log error
    */
    int getLogLevel() const { return _mLogLevel; }
    
    
    /**
       sets the log level
       @param level new level to log at
    */
    void setLogLevel(const int newLevel)
    {
        _mLogLevel = newLevel;
    }
    
    
    /**
       this logs a message
       @param msg message to log
       @param level level to log at
    */
    void logMessage(const String& msg, const int level = LOGC_MSG);
    
    /**
       this logs a message
       @param msgid message id to log
       @param level level to log at
    */
    void logMessage(const int msgId, const int level = LOGC_MSG);
    
    /**
       this logs a message
       @param msgid message id to log
       @param level level to log at
    */
    void logMessage(const int msgId, const String& appendStr, const int level = LOGC_MSG);
    
    // overrides from iSysComponent
    int init( const DOMNode* config, RefCountedPtr<SysContext>& ctx );
    
    /**
      Called after initialization of the system context. this allows
      system components to utilize other components as necessary
    */
    virtual int postInit( const DOMNode* config, RefCountedPtr<SysContext>& ctx );
    
    /**
       used to close the system component
    */
    int close() { return 0; }
    
    /**
      called to shutdown a component. DOMNode is the same configuration
      node that was passed to init. The component should update the any
      data that it wants persisted
    */
    int update( DOMNode* /*config */) { return 0; }
    
    const String& getComponentName() { return LOG_CFGKEY; }
    
    static const String& getRegistryName() { return LOG_CFGKEY; }
    
    unsigned processMessages();
    
private:
    class LogMessage;
    friend class LoggerRunner;
    
    typedef ThdQueue< RefCountedPtr<LogMessage> > MessageQ;
    
    bool _logStdOut;
    bool _logStdErr;
    
    int                         _mLogLevel;
    std::vector< RefCountedPtr<iLoggerImpl> >  _mImpls;
    RefCountedPtr<StdFormatter> _mFmt;
    
    RefCountedPtr<ThdThread> _runner;   // is we are running in multi-threaded mode
    MessageQ            _mq;            // if we have a thread pool, then we queue log items
    ThdEvent            _exitEvent;     // the signal for the thread runner to exit
    ThdCriticalSection  _lock;          // if no thread pool, then serialize access while logging
    
	RefCountedPtr<DllLoader>		_rsrcFile;
	std::map<int,String>			_stringCache;
    ThdRWLock						_cacheLock;          // cache lock

private:
    void logOne( const String &logItem, const int level, const String &source );

	bool getCacheMessage( const int msgId, String& ioCachedString );

private:
    static const String LOG_CFGKEY;
    static const String LOG_LEVEL;
    static const String LOG_FORMAT;
    static const String LOG_STDERR;
    static const String LOG_RSRCFILE;
    static const String LOG_STDOUT;
    static const String LOG_THREADED;
    static const String LOG_IMPL;
    static const String LOG_IMPLNAMEATT;
    
    static const String LOG_FORMAT_D;
    static const String LOG_TRUE_D;

    // a log message class to be inserted into the shared queue
    // for the log runner to pick up and execute
    class LogMessage
    {
    public:
        LogMessage( 
            const String &message, 
            const int level, 
            const String &source ) :
            _message(message),
            _source(source),
            _level(level)
        {}
        
        ~LogMessage() {}
        
        LogMessage( const LogMessage &rhs )
        {
            *this = rhs;
        }
        
        LogMessage& operator=( const LogMessage &rhs )
        {
            if ( this != &rhs )
            {
                _message = rhs._message;
                _level = rhs._level;
            }
            
            return *this;
        }
        
        const String& getMessage() const { return _message; }
        const String& getSource() const { return _source; }

        const int getLevel() const { return _level; }

    private:
        String _message;
        String _source;
        int _level;

    private:
        LogMessage();
    };

    // this object manages logging data to the system
    class LoggerRunner : public iRunnable
    {
    private:
        StdLogger &_logger;

    public:
        LoggerRunner( StdLogger &logger ) :
            _logger(logger)
        {}

        ~LoggerRunner() 
        {
        }

        unsigned run()
        {
            return _logger.processMessages();
        }

    private:
        LoggerRunner();
        LoggerRunner(const LoggerRunner&);
        LoggerRunner& operator=(const LoggerRunner&);
    };

    StdLogger(const StdLogger&);
    StdLogger& operator=(const StdLogger&);

    DEFINE_SYS_COMPONENT(StdLogger)
};

#define CBLOGDBG(logger,msg) if (logger != NULL) logger->logMessage(msg, StdLogger::LOGC_DEBUG); else DBG_LOGDBG(msg);
#define CBLOGERR(logger,msg) if (logger != NULL) logger->logMessage(msg, StdLogger::LOGC_ERR); else DBG_LOGERR(msg);
#define CBLOGWRN(logger,msg) if (logger != NULL) logger->logMessage(msg, StdLogger::LOGC_WARN); else DBG_LOGWRN(msg);
#define CBLOGMSG(logger,msg) if (logger != NULL) logger->logMessage(msg, StdLogger::LOGC_MSG); else DBG_LOGMSG(msg);


#endif // LogStdLogger_H_LOADED
