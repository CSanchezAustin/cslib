// File EventLog.cpp Implements the CEventLog class
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

#include <TCHAR.H>

#include <log/nt/EventLog.h>

// Construction
CEventLog::CEventLog(const String& pszSource, const String& pszServerName)
{
   m_hes = NULL;
   m_pszSource = pszSource;
   m_pszServerName = pszServerName;
}

// Simple Version
BOOL CEventLog::LogEvent(DWORD dwEventID,   WORD     wEventType,
                         WORD  wCategory,   PSID     lpUserSid)
{
   Register();
   BOOL b = ReportEvent(m_hes, wEventType, wCategory,
                                      dwEventID, lpUserSid, 0, 0, NULL, NULL);
   Deregister();
   return b;
}

// Does anything version
BOOL CEventLog::LogEvent(DWORD dwEventID,   LPCTSTR* lpStrings,
                         WORD  wNumStrings, LPVOID   lpRawData,
                         DWORD dwDataSize,  WORD     wEventType,
                         WORD  wCategory,   PSID     lpUserSid)
{
   if(wNumStrings == 0 && lpStrings != NULL)
      return FALSE;
   if(lpStrings == NULL && wNumStrings != 0)
      return FALSE;

   Register();
   BOOL b = ReportEvent(m_hes, wEventType, wCategory, dwEventID, lpUserSid,
                               wNumStrings, dwDataSize, lpStrings, lpRawData);
   Deregister();
   return b;
}

// Single String Version
BOOL CEventLog::LogEvent(DWORD dwEventID,   LPCTSTR  lpString,
                         WORD  wEventType,  WORD     wCategory,
                         PSID  lpUserSid)
{
   const TCHAR* rgsz[] = {lpString};

   Register();
   BOOL b = ReportEvent(m_hes, wEventType, wCategory, dwEventID,
                                                 lpUserSid, 1, 0, rgsz, NULL);
   Deregister();
   return b;
}

// Multiple String Version
BOOL CEventLog::LogEvent(DWORD dwEventID,   LPCTSTR* lpStrings,
                         WORD wNumStrings,  WORD     wEventType,
                         WORD wCategory,    PSID     lpUserSid)
{
   Register();
   BOOL b = ReportEvent(m_hes, wEventType, wCategory, dwEventID,
                                  lpUserSid, wNumStrings, 0, lpStrings, NULL);
   Deregister();
   return b;
}

// Send Data Version
BOOL CEventLog::LogEvent(DWORD  dwEventID,  DWORD    dwDataSize,
                         LPVOID lpRawData,  WORD     wEventType,
                         WORD   wCategory,  PSID     lpUserSid)
{
   Register();
   BOOL b = ReportEvent(m_hes, wEventType, wCategory, dwEventID,
                                   lpUserSid, 0, dwDataSize, NULL, lpRawData);
   Deregister();
   return b;
}

// Log a Win32 error
BOOL CEventLog::LogWin32Error(DWORD dwEventID,
                                           LPCTSTR szString, DWORD dwErrorNum)
{
   LPVOID lpvMsgBuf;
   TCHAR szErrorDesc[1024];
   if(!FormatMessage(
                  FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER,
                  0, dwErrorNum, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                  (LPTSTR)&lpvMsgBuf, 0, 0))
   {
      if(szString != NULL)
         wsprintf(szErrorDesc,
                  NTEXT("Function: %s returned Win32 Error: %d, Unknown Error"),
                  szString, dwErrorNum);
      else
         wsprintf(szErrorDesc,
                  NTEXT("Win32 Error: %d, Unknown Error"),
                  dwErrorNum);
   }
   else
   {
      if(szString != NULL)
         wsprintf(szErrorDesc,
                  NTEXT("Function: %s returned Win32 Error: %d Description: %s"),
                  szString, dwErrorNum, lpvMsgBuf);
      else
         wsprintf(szErrorDesc,
                  NTEXT("Win32 Error: %d Description: %s"),
                  dwErrorNum, lpvMsgBuf);
      LocalFree(lpvMsgBuf);
   }

   const TCHAR* rgsz[] = { szErrorDesc };

   Register();
   BOOL b = ReportEvent(m_hes, EVENTLOG_ERROR_TYPE, NULL, dwEventID,
                                                      NULL, 1, 0, rgsz, NULL);
   Deregister();
   return b;
}

BOOL CEventLog::RegisterLog(LPCTSTR szPath)
{
   HKEY hk;
   DWORD dwData;
   TCHAR szBuf[256];
   TCHAR szKey[256];
   LONG	lResult;
   DWORD dwDisposition ;
   wsprintf(szKey,
         NTEXT("SYSTEM\\CurrentControlSet\\Services\\EventLog\\Application\\%s"),
         m_pszSource.c_str());

   lResult = RegCreateKeyEx(HKEY_LOCAL_MACHINE, szKey, 0, 
							NULL, REG_OPTION_NON_VOLATILE, 
							KEY_WRITE|KEY_ALL_ACCESS, NULL, &hk, &dwDisposition);
   if(lResult != ERROR_SUCCESS)
   {
	   FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, 0, 0, szBuf, 256, NULL);
		return FALSE;
   }

   if ( dwDisposition == REG_OPENED_EXISTING_KEY )
	   return TRUE;

   _tcscpy_s(szBuf, szPath);

   // EventMessageFile subkey
   lResult = RegSetValueEx(hk, NTEXT("EventMessageFile"), 0, REG_EXPAND_SZ,
      (LPBYTE)szBuf, ((_tcslen(szBuf) + 1) * sizeof(TCHAR)));
   if(lResult != ERROR_SUCCESS)
   {
	   FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, 0, 0, szBuf, 256, NULL);
      return FALSE;
   }

   // TypesSupported
   dwData = EVENTLOG_ERROR_TYPE |
            EVENTLOG_WARNING_TYPE | EVENTLOG_INFORMATION_TYPE;

   lResult = RegSetValueEx(hk, NTEXT("TypesSupported"), 0, REG_DWORD, (LPBYTE)&dwData,
                                              sizeof(DWORD));
   if(lResult != ERROR_SUCCESS)
   {
	   FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, 0, 0, szBuf, 256, NULL);
      return FALSE;
   }

   RegCloseKey(hk);
   return TRUE;
}
