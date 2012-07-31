//      CService.cpp
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
// Portions Copyright (c) 2000 Kevin Miller
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
//      Contents:	
//	
#include <cslib.h>

#include <fstream>

#include <svr/nt/CService.h>
#include <svr/nt/NTServiceMain.h>
#include <svr/iSvrService.h>
#include <thd/ThdManager.h>
#include <log/LogStdLogger.h>
#include <log/nt/EventLog.h>
#include <util/UtilRefCountedPtrInlines.h>
#include <sys/SysComInitializer.h>

const DWORD kDefaultWaitTime = 60 * 1000; // default is 1 minute

CService* CService::m_pThis = NULL;



CService::CService(
            RefCountedPtr<iSvrService>& theService,
            const Mapping& params,
            DWORD dwType ) : 
        _theService(theService),
        _params(params),
        m_dwType(dwType)
{
	m_pThis = this;

	m_hServiceStatus = NULL;
   m_dwRequestedControl = 0;

   // Control Events
   m_hWatcherThread = NULL;

   m_dwState = 0;
   m_dwControlsAccepted = 0;
   m_dwCheckpoint = 0;
   m_dwWaitHint = 0;

   // Initialize event handles to NULL
   for(int i = 0 ; i < NUMEVENTS ; i++)
      m_hEvents[i] = NULL;

   // Copy string names

   // Set up class critical section
   InitializeCriticalSection(&m_cs);
}

CService::~CService()
{
   DeleteCriticalSection(&m_cs);
}

void CService::PreInit()
{
    // set up the system context
    

   // Initialize Events
   for(int i = 0 ; i < NUMEVENTS ; i++)
   {
      m_hEvents[i] = CreateEvent(NULL, FALSE, FALSE, NULL);
      if(!m_hEvents[i])
         ErrorHandler(NTEXT("CreateEvent"));
   }
}

void CService::DeInit()
{
   // Wait for the watcher thread to terminate
   if(m_hWatcherThread)
   {
      // Wait a reasonable amount of time
      WaitForSingleObject(m_hWatcherThread, 10000);
      CloseHandle(m_hWatcherThread);
   }

   // Uninitialize any resources created in Init()
   for(int i = 0 ; i < NUMEVENTS ; i++)
   {
      if(m_hEvents[i])
         CloseHandle(m_hEvents[i]);
   }
}

void CService::ServiceMainMember(DWORD argc, LPTSTR* argv, LPHANDLER_FUNCTION pf, LPTHREAD_START_ROUTINE pfnWTP)
{
    DWORD dwErr = 0;

   __try
    {
        PreInit();
        SetupHandlerInside(pf);
        LaunchWatcherThread(pfnWTP);
        SetStatus(SERVICE_START_PENDING, 1, kDefaultWaitTime );
        if ( NTService::InitializeService( _theService, _params, false ) == 0 )
        {
            SetStatus( SERVICE_RUNNING, 0, 0, SERVICE_ACCEPT_STOP );
            _theService->start();
        }
        else
        {
            CERR << NTEXT("Error initializing service") << std::endl;
        }
    }
   __except(dwErr = GetExceptionCode(), EXCEPTION_EXECUTE_HANDLER)
   {
      if(m_hServiceStatus)
         SetStatus(SERVICE_STOPPED, 0, 0, 0, dwErr, 0);
   }

   DeInit();
   SetStatus(SERVICE_STOPPED, 0, 0, 0, dwErr, 0);
}

// Register the control handler for the service
bool CService::SetupHandlerInside(LPHANDLER_FUNCTION lpHandlerProc)
{
   m_hServiceStatus = RegisterServiceCtrlHandler(m_szName.c_str(), lpHandlerProc);
   if(!m_hServiceStatus)
   {
      ErrorHandler(NTEXT("RegisterServiceCtrlHandler"));
      return false;
   }

   return true;
}

void CService::HandlerMember(DWORD dwControl)
{
   // Keep an additional control request of the same type
   //  from coming in when you're already handling it
   if(m_dwRequestedControl == dwControl)
      return;

   switch(dwControl)
   {
   case SERVICE_CONTROL_STOP:
      m_dwRequestedControl = dwControl;

      // Notify the service to stop...
      SetEvent(m_hEvents[STOP]);
      break;

   case SERVICE_CONTROL_PAUSE:
      m_dwRequestedControl = dwControl;

      // Notify the service to pause...
      SetEvent(m_hEvents[PAUSE]);
      break;

   case SERVICE_CONTROL_CONTINUE:
      if(GetStatus() != SERVICE_RUNNING)
      {
         m_dwRequestedControl = dwControl;

         // Notify the service to continue...
         SetEvent(m_hEvents[CONTINUE]);
      }
      break;

   case SERVICE_CONTROL_SHUTDOWN:
      m_dwRequestedControl = dwControl;

      SetEvent(m_hEvents[SHUTDOWN]);
      break;

   case SERVICE_CONTROL_INTERROGATE:
      // Return current status on interrogation
      SetStatus(GetStatus());
      break;

   default: // User Defined
      m_dwRequestedControl = dwControl;
      HandleUserDefined(dwControl);
   }
}

void CService::LaunchWatcherThread(LPTHREAD_START_ROUTINE pfnWTP)
{
   DWORD tid = 0;

   m_hWatcherThread = (HANDLE)_beginthreadex(0, 0, (unsigned (WINAPI*)(void*))pfnWTP, 0, 0, (unsigned int*)&tid);
   if(!m_hWatcherThread)
      ErrorHandler(NTEXT("_beginthreadex"));
}

DWORD CService::WatcherThreadMemberProc()
{
   DWORD dwWait = 0;
   bool bControlWait = true;

   // Wait for any events to signal
   while(bControlWait)
   {
      dwWait = WaitForMultipleObjects(NUMEVENTS, m_hEvents, FALSE, INFINITE);

      switch(dwWait - WAIT_OBJECT_0)
      {
      case STOP:
         OnStop();
         bControlWait = false;
         break;

      case PAUSE:
         OnPause();
         ResetEvent(m_hEvents[PAUSE]);
         break;

      case CONTINUE:
         OnContinue();
         ResetEvent(m_hEvents[CONTINUE]);
         break;

      case SHUTDOWN:
         OnShutdown();
         bControlWait = false;
         break;
      }
   }
   return 0;
}

void CService::SetStatus(DWORD dwNewState, DWORD dwNewCheckpoint,
                         DWORD dwNewHint,  DWORD dwNewControls,
                         DWORD dwExitCode, DWORD dwSpecificExit)
{
   // The only state that can set Exit Codes is STOPPED
   // Fix if necessary, just in case not set properly.
   if(dwNewState != SERVICE_STOPPED)
   {
      dwExitCode = S_OK;
      dwSpecificExit = 0;
   }

   // Only pending states can set checkpoints or wait hints,
   //  and pending states *must* set wait hints
   if(dwNewState == SERVICE_STOPPED || dwNewState == SERVICE_PAUSED || dwNewState == SERVICE_RUNNING)
   {
      // Requires hint and checkpoint == 0
      // Fix it so that NO_CHANGE from previous state doesn't cause nonzero
      dwNewHint = 0;
      dwNewCheckpoint = 0;
   }
   else
   {
      // Requires hint and checkpoint != 0
      if(dwNewHint <= 0 || dwNewCheckpoint <=0)
      {
         ErrorHandler(NTEXT("CService::SetStatus: Pending statuses require a hint and checkpoint"), true, true, 0);
      }
   }

   // Function can be called by multiple threads - protect member data
   EnterCriticalSection(&m_cs);

   // Alter states if changing
   m_dwState = dwNewState;

   if(dwNewCheckpoint != STATE_NO_CHANGE)
      m_dwCheckpoint = dwNewCheckpoint;

   if(dwNewHint != STATE_NO_CHANGE)
      m_dwWaitHint = dwNewHint;

   if(dwNewControls != STATE_NO_CHANGE)
      m_dwControlsAccepted = dwNewControls;

   SERVICE_STATUS ss = { m_dwType, m_dwState, m_dwControlsAccepted,
                   dwExitCode, dwSpecificExit, m_dwCheckpoint, m_dwWaitHint };

   LeaveCriticalSection(&m_cs);

   if(!SetServiceStatus(m_hServiceStatus, &ss))
      ErrorHandler(NTEXT("SetServiceStatus"));
}

//Generic error handler which gathers the last error, looks up the description string, and optionally
//prints the string to the event log and/or raises an exception to stop the service.
DWORD CService::ErrorHandler(const TCHAR* psz, bool bPrintEvent, bool bRaiseException, DWORD dwErr)
{
   if(bPrintEvent)
   {
        CERR << psz << std::endl;
//      CEventLog log(m_szName, m_szName );
//      log.LogWin32Error(SERVICE_WIN32_ERROR, psz, dwErr);
   }

   if(bRaiseException)
      RaiseException(dwErr, EXCEPTION_NONCONTINUABLE, 0, 0);

   return dwErr;
}


void 
CService::OnPause()
{
}


void 
CService::OnContinue()
{
}


void 
CService::OnShutdown()
{
    RefCountedPtr<iSvrService> localService = _theService;

    localService->stop();

    DWORD waitHint = 1;

    while ( true )
    {
        SetStatus( SERVICE_STOP_PENDING, waitHint++, 2000 );
        if ( !localService->isStopped() )
        {
            ThdManager::Sleep( 1000 );
        }
        else
        {
            break;
        }
    }    
}


void 
CService::HandleUserDefined(
            DWORD dwControl )
{
}


void 
CService::OnStop()
{
    RefCountedPtr<iSvrService> localService = _theService;

    localService->stop();

    DWORD waitHint = 1;

    while ( true )
    {
        SetStatus( SERVICE_STOP_PENDING, waitHint++, 2000 );
        if ( !localService->isStopped() )
        {
            ThdManager::Sleep( 1000 );
        }
        else
        {
            break;
        }
    }    
}



void WINAPI 
CService::Main(
            DWORD argc, 
            LPTSTR* argv )
{ 
    SysComInitializer comInitter;

    m_pThis->ServiceMainMember(argc, argv,
                   (LPHANDLER_FUNCTION)Handler,
                   (LPTHREAD_START_ROUTINE)WatcherThreadProc);
}


void WINAPI 
CService::Handler( 
            DWORD dwControl )
{
    SysComInitializer comInitter;

    m_pThis->HandlerMember( dwControl );
}


DWORD WINAPI 
CService::WatcherThreadProc(
            LPVOID lpParameter )
{
    SysComInitializer comInitter;

    return m_pThis->WatcherThreadMemberProc();
}
