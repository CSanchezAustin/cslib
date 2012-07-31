#ifndef CService_H_LOADED
#define CService_H_LOADED
// [
// $        CService.h
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
// Copyright (c) 2001 Chris Sanchez. All rights reserved.
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
//                 $Revision: 2.0 $
//                 $Author: $
//                 $Date: $
// ?
//         Contents: Service installer class
//
//         Purpose:
//
//         Usage Notes:
//
//                 
// ?
// !        Changes History
//     $Header: $
//         
// ]

#define STATE_NO_CHANGE 0xffffffff
#define NUMEVENTS 4
#define MAX_SERVICE_LEN 256

#include <cslib.h>
#include <util/UtilRefCountedPtr.h>
#include <util/UtilMapping.h>

class iSvrService;

///////////////////////////////////////////////////////
// CService Class
///////////////////////////////////////////////////////
/* Each service derives from this to do its own
   work. The application must instantiate the derived
   service class one and only one time. */
///////////////////////////////////////////////////////
class CService
{
    friend class CServiceExHandler;

public:
   CService(
            RefCountedPtr<iSvrService>& theService,
            const Mapping& params,
            DWORD dwType );

   ~CService();

   DWORD   GetStatus()      { return m_dwState; }
   DWORD   GetControls()    { return m_dwControlsAccepted; }
   LPCTSTR GetName()        { return m_szName.c_str(); }
   LPCTSTR GetDisplayName() { return m_szDisplay.c_str(); }

   // All derived class static ServiceMain functions are delegated to me
   void ServiceMainMember(DWORD argc, LPTSTR* argv,
                        LPHANDLER_FUNCTION pf, LPTHREAD_START_ROUTINE pfnWTP);

   // All derived class static handler functions are delegated to me
   void HandlerMember(DWORD dwControl);

   static void WINAPI Main(DWORD argc, LPTSTR* argv);

protected:
   bool SetupHandlerInside(LPHANDLER_FUNCTION lpHandlerProc);

   // Launches a thread to look for control requests
   virtual void LaunchWatcherThread(LPTHREAD_START_ROUTINE pfnWTP);
   virtual DWORD WatcherThreadMemberProc();

   void SetStatus(DWORD dwNewState,
                  DWORD dwNewCheckpoint = STATE_NO_CHANGE,
                  DWORD dwNewHint       = STATE_NO_CHANGE,
                  DWORD dwNewControls   = STATE_NO_CHANGE,
                  DWORD dwExitCode      = NO_ERROR,
                  DWORD dwSpecificExit  = 0);

   DWORD ErrorHandler(const TCHAR* pszFcn,
                      bool         bPrintEvent     = true,
                      bool         bRaiseException = true,
                      DWORD        dwErr           = GetLastError());

// Overrideables
protected:
   virtual void PreInit();     // If you override, call the base class version
   virtual void DeInit();      // If you override, call the base class version
   virtual void OnPause();
   virtual void OnContinue();
   virtual void OnShutdown();
   virtual void OnStop();
   virtual void HandleUserDefined(DWORD dwControl);

// Attributes
protected:
   CRITICAL_SECTION m_cs;

   // Status info
   SERVICE_STATUS_HANDLE m_hServiceStatus;
   DWORD m_dwState;
   DWORD m_dwControlsAccepted;
   DWORD m_dwCheckpoint;
   DWORD m_dwWaitHint;

   // Tracks state currently being worked on in Handler
   DWORD m_dwRequestedControl;

   // Control Events
   HANDLE m_hEvents[NUMEVENTS];
   HANDLE m_hWatcherThread;

   String m_szName;
   String m_szDisplay;
   DWORD m_dwType;

    Mapping _params;
    RefCountedPtr<iSvrService> _theService;

   enum EVENTS { STOP, PAUSE, CONTINUE, SHUTDOWN };

private:
   static CService* m_pThis;
   static void WINAPI Handler(DWORD dwControl);
   static DWORD WINAPI WatcherThreadProc(LPVOID lpParameter);
};

#endif // CService_H_LOADED
