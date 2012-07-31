//      CServiceInstall.cpp
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
// Copyright (c) 2001 Chris Sanchez. All rights reserved.
// Portions Copyright (c) 2000 Kevin Miller
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
//
//      Contents:	
//	

#include <cslib.h>

#include <svr/nt/CServiceInstall.h>
#include <util/UtilMapping.h>

CServiceInstall::CServiceInstall( 
            const String& szServiceName, 
            const String& szDisplay ) :
        m_service(szServiceName ),
        m_display(szDisplay )
{
}

void CServiceInstall::Install(
            const Mapping& parameters, 
            DWORD dwType, 
            DWORD dwStart,
            LPCTSTR lpDepends, 
            LPCTSTR lpName,
            LPCTSTR lpPassword )
{
	SC_HANDLE hSCM = NULL;
	SC_HANDLE hService = NULL;
    HKEY hk;
    String params;
    String szKey;
	String filePath;

	if( IsInstalled() == TRUE )
		return;
	
	hSCM = OpenSCManager( NULL,
							NULL,
							SC_MANAGER_CREATE_SERVICE );
	if ( !hSCM )
	{
		ErrorPrinter( NTEXT("OpenSCManager") );
		goto cleanup;
	}

    TCHAR szFilePath[_MAX_PATH];
    ::GetModuleFileName(NULL, szFilePath, _MAX_PATH);
    filePath = szFilePath;
    if ( ( filePath.find( SPACE, 0 ) != String::npos ) &&
         ( filePath.find( QUOTE, 0 ) != 0 )  )
    {
        // we must quote the file path to make sure that it is properly 
        //  interpreted by the SCM
        filePath = QUOTE + filePath + QUOTE;
    }

	hService = CreateService( 
		hSCM,
		m_service.c_str(),
		m_display.c_str(),
		SERVICE_ALL_ACCESS,
		dwType,
		dwStart,
		SERVICE_ERROR_NORMAL,
		filePath.c_str(),
		NULL,
		NULL,
		lpDepends,
		lpName,
		lpPassword );

	if ( !hService ) 
	{
		ErrorPrinter( NTEXT("CreateService") );
		goto cleanup;
	}
	else
	{
        CERR << m_service.c_str() << NTEXT(" Created") << std::endl;
//		_tprintf(NTEXT("%s Created\n"), m_service);
	}


    // now prepare the save off the command line parameters
    params = parameters.toString();
    szKey = NTEXT("SYSTEM\\CurrentControlSet\\Services\\");
    szKey += m_service;

    // Does the key already exist?
    if(RegOpenKey(HKEY_LOCAL_MACHINE, szKey.c_str(), &hk) == ERROR_SUCCESS)
    {
        if(RegCreateKey(HKEY_LOCAL_MACHINE, szKey.c_str(), &hk) == ERROR_SUCCESS)
        {
            RegSetValueEx(  hk, NTEXT("params"), 0, 
                            REG_SZ, (LPBYTE)params.c_str(),
                            params.length() );
            RegCloseKey(hk);
        }
    }

cleanup:
	if (hService) CloseServiceHandle( hService );
	if (hSCM) CloseServiceHandle( hSCM );
	
	return;
}

void CServiceInstall::Remove( BOOL bForce )
{
	SC_HANDLE hSCM = NULL;
	SC_HANDLE hService = NULL;
	BOOL bSuccess = FALSE;

	hSCM = ::OpenSCManager( NULL,
							  NULL,
							  SC_MANAGER_CONNECT );
	if ( !hSCM )
	{
		ErrorPrinter( NTEXT("OpenSCManager") );
		return;
	}
	
	hService = ::OpenService(
		hSCM,
		m_service.c_str(),
		DELETE );						
	if ( !hService ) 
	{
		ErrorPrinter( NTEXT("OpenService") );
		goto cleanup;
	}
	
	//Force the service to stop 
	if( bForce == TRUE ) 
	{
		SERVICE_STATUS status;
		::ControlService(hService, SERVICE_CONTROL_STOP, &status);
        CERR << m_service.c_str() << NTEXT(" stopped") << std::endl;
//		_tprintf(NTEXT("%s stopped\n"), m_service);
	}

	bSuccess = ::DeleteService( hService );

	if (bSuccess)
    {
        CERR << m_service.c_str() << NTEXT(" removed") << std::endl;
//		_tprintf(NTEXT("%s removed\n"), m_service); 
    }
	else
    {
		ErrorPrinter( NTEXT("DeleteService") );
    }

cleanup:
	if (hService) CloseServiceHandle( hService );
	if (hSCM) CloseServiceHandle( hSCM );
	
	return;
}

BOOL CServiceInstall::IsInstalled( )
{
    BOOL bRes = FALSE;

    SC_HANDLE hSCM = ::OpenSCManager(NULL, NULL, SC_MANAGER_CONNECT);

    if (hSCM != NULL)
	{
        SC_HANDLE hService = ::OpenService(hSCM, m_service.c_str(), SERVICE_QUERY_CONFIG);
        if (hService != NULL)
		{
            bRes = TRUE;
            ::CloseServiceHandle(hService);
        }
        ::CloseServiceHandle(hSCM);
    }
    return bRes;
}

DWORD CServiceInstall::ErrorPrinter( const TCHAR* psz, DWORD dwErr )
{
	LPVOID lpvMsgBuf;
    if ( !FormatMessage( FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER,
		0, dwErr, 
		MAKELANGID( LANG_NEUTRAL, SUBLANG_DEFAULT ),
		(LPTSTR) &lpvMsgBuf, 0, 0 ) )
	{
        CERR << psz << NTEXT(" failed: Unknown error ") << dwErr << std::endl;
//		_tprintf( NTEXT("%s failed: Unknown error %x\n"), psz, dwErr );
	}
	else {
        CERR << psz << NTEXT(" failed: ") << (LPTSTR)lpvMsgBuf << std::endl;
//		_tprintf( NTEXT("%s failed: %s\n"), psz, (LPTSTR)lpvMsgBuf );
	}

	LocalFree( lpvMsgBuf );
	return dwErr;
}


Mapping
CServiceInstall::GetParameters()
{
    String params;

    HKEY hKey;
    String szKey = NTEXT("SYSTEM\\CurrentControlSet\\Services\\");
    szKey += m_service;

    // Does the key already exist?
    if( RegOpenKey(HKEY_LOCAL_MACHINE, szKey.c_str(), &hKey) == ERROR_SUCCESS )
    {
        DWORD nSize;
	    if ( RegQueryValueEx(hKey, 
			         NTEXT("params"),		/* key name */
			         NULL,		/* reserved */
			         NULL,		/* type */
			         NULL,		/* for value */
			         &nSize) == ERROR_SUCCESS )		/* for size of "value" */
        {
            String::value_type *buff = new String::value_type[ nSize ];
	        if ( RegQueryValueEx(hKey, 
			             NTEXT("params"),		/* key name */
			             NULL,		/* reserved */
			             NULL,		/* type */
			             (LPBYTE)buff,		/* for value */
			             &nSize) == ERROR_SUCCESS )		/* for size of "value" */
            {
                params = buff;
            }

            delete [] buff;
        }

        RegCloseKey(hKey);
    }

    return Mapping(params);
}
