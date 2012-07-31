// EventLog.h file
// CEventLog Class - Implements event logging functions
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

#include <cslib.h>

class CEventLog
{
public:
   CEventLog(const String& pszSource, const String& pszServerName = EMPTYSTR );
   ~CEventLog() { }

   // Simple Version
   BOOL LogEvent(DWORD    dwEventID,
                 WORD     wEventType  = EVENTLOG_ERROR_TYPE,
                 WORD     wCategory   = 0,
                 PSID     lpUserSid   = NULL);

   // Does anything version
   BOOL LogEvent(DWORD dwEventID,
                 LPCTSTR* lpStrings   = NULL,
                 WORD     wNumStrings = 0,
                 LPVOID   lpRawData   = 0,
                 DWORD    dwDataSize  = 0,
                 WORD     wEventType  = EVENTLOG_ERROR_TYPE,
                 WORD     wCategory   = 0,
                 PSID     lpUserSid   = NULL);

   // Single String Version
   BOOL LogEvent(DWORD    dwEventID,
                 LPCTSTR  lpString,
                 WORD     wEventType  = EVENTLOG_ERROR_TYPE,
                 WORD     wCategory   = 0,
                 PSID     lpUserSid   = NULL);

   // Multiple String Version
   BOOL LogEvent(DWORD    dwEventID,
                 LPCTSTR* lpStrings,
                 WORD     wNumStrings,
                 WORD     wEventType  = EVENTLOG_ERROR_TYPE,
                 WORD     wCategory   = 0,
                 PSID     lpUserSid   = NULL);

   // Send Data Version
   BOOL LogEvent(DWORD    dwEventID,
                 DWORD    dwDataSize,
                 LPVOID   lpRawData,
                 WORD     wEventType  = EVENTLOG_ERROR_TYPE,
                 WORD     wCategory   = 0,
                 PSID     lpUserSid   = NULL);

   // Log a Win32 error
   BOOL LogWin32Error(DWORD   dwEventID,
                      LPCTSTR szString   = NULL,
                      DWORD   dwErrorNum = GetLastError());

   BOOL RegisterLog(LPCTSTR szPath);

private:
   HANDLE  m_hes;
   String m_pszSource;
   String m_pszServerName;

   void Register()
   {
      m_hes = RegisterEventSource(
            ( m_pszServerName.length() > 0 ) ? m_pszServerName.c_str() : NULL,
            ( m_pszSource.length() > 0 ) ? m_pszSource.c_str() : NULL );
   }

   void Deregister()
   {
      DeregisterEventSource(m_hes);
   }

// Event Types are one of the following
   // EVENTLOG_ERROR_TYPE;
   // EVENTLOG_WARNING_TYPE;
   // EVENTLOG_INFORMATION_TYPE;
};
