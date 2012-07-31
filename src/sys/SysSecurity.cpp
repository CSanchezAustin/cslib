/********
*
* SECURITY.CPP
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
*
* Copyright (c) 1996 Ralph P. Davis
*         Copyright (c) 1998-2000, Chris Sanchez
*
* All Rights Reserved
*
*****/
    
/*===== Includes =====*/

#include <cslib.h>

#include <sys/SysSecurity.h>
#include <sys/SysSecurityDescriptor.h>

SysAcl::SysAcl()
{
    _pACL = (PACL) new BYTE[16384];
	::memset( (void*)_pACL, 0x00, 16384 );

    if ( !::InitializeAcl( (PACL) _pACL, 16384, ACL_REVISION ) )
    {
        if ( ::GetLastError() != ERROR_CALL_NOT_IMPLEMENTED )
            throw ::GetLastError();
    }
}


SysAcl::~SysAcl()
{
   delete [] (BYTE*)_pACL;
}


SysDacl::SysDacl(
            const std::vector<String> &users,
            DWORD  dwAccess /*= STANDARD_RIGHTS_REQUIRED | GENERIC_ALL*/)
{
    std::vector<String>::size_type sz = users.size();
    for ( std::vector<String>::size_type i = 0; i < sz; ++i )
    {
        if ( !allowUserAccess( users[i], dwAccess ) )
        {
            if ( ::GetLastError() != ERROR_CALL_NOT_IMPLEMENTED )
                throw ::GetLastError();
            return;
        }
    }
}


BOOL 
SysDacl::allowUserAccess(
            const String &user,
            DWORD  dwAccess)
{
    SECURITY_INFORMATION SecurityInformation = DACL_SECURITY_INFORMATION;
    BYTE   bySIDBuffer[1024];
    DWORD  dwSIDSize = sizeof (bySIDBuffer);
    BOOL   bRetcode  = FALSE;

    dwSIDSize = sizeof (bySIDBuffer);

    if ( SysSecurityDescriptor::nameToSID( user, (PSID) bySIDBuffer, &dwSIDSize ) )
    {
        bRetcode = ::AddAccessAllowedAce( _pACL, ACL_REVISION, dwAccess, bySIDBuffer );
    }

    return bRetcode;
}


BOOL 
SysDacl::allowUserAccess(
            const std::vector<String> &users,
            DWORD  dwAccess /*= STANDARD_RIGHTS_REQUIRED | GENERIC_ALL*/)
{
    BOOL bRetcode = FALSE;

    std::vector<String>::size_type sz = users.size();
    for ( std::vector<String>::size_type i = 0; i < sz; ++i )
    {
        bRetcode |= allowUserAccess( users[i], dwAccess );
    }

   return bRetcode;
}


BOOL 
SysDacl::denyUserAccess(
            const String &user,
            DWORD  dwAccess)
{
    SECURITY_INFORMATION SecurityInformation = DACL_SECURITY_INFORMATION;
    BYTE   bySIDBuffer[1024];
    DWORD  dwSIDSize = sizeof (bySIDBuffer);
    BOOL   bRetcode  = FALSE;

    dwSIDSize = sizeof (bySIDBuffer);

    if ( SysSecurityDescriptor::nameToSID( user, (PSID) bySIDBuffer, &dwSIDSize ) )
    {
        bRetcode = ::AddAccessDeniedAce( _pACL, ACL_REVISION, dwAccess, bySIDBuffer );
    }

    return bRetcode;
}


BOOL 
SysDacl::denyUserAccess(
            const std::vector<String> &users,
            DWORD  dwAccess /*= STANDARD_RIGHTS_REQUIRED | GENERIC_ALL*/)
{
    BOOL bRetcode = FALSE;

    std::vector<String>::size_type sz = users.size();
    for ( std::vector<String>::size_type i = 0; i < sz; ++i )
    {
        bRetcode |= denyUserAccess( users[i], dwAccess );
    }

   return bRetcode;
}


SysSacl::SysSacl(
            const std::vector<String> &users,
            DWORD  dwAccess /*= STANDARD_RIGHTS_REQUIRED | GENERIC_ALL | ACCESS_SYSTEM_SECURITY */)
{
    std::vector<String>::size_type sz = users.size();
    for ( std::vector<String>::size_type i = 0; i < sz; ++i )
    {
        if ( !auditUserAccess( users[i], dwAccess ) )
        {
            if ( ::GetLastError() != ERROR_CALL_NOT_IMPLEMENTED )
                throw ::GetLastError();
            return;
        }
    }
}


SysSacl::~SysSacl()
{
   restorePrivileges();
}


BOOL 
SysSacl::auditUserAccess(
            const String &user,
            DWORD  dwAccess)
{
    SECURITY_INFORMATION SecurityInformation = SACL_SECURITY_INFORMATION;
    BYTE   bySIDBuffer[1024];
    DWORD  dwSIDSize = sizeof (bySIDBuffer);
    BOOL   bRetcode  = FALSE;

    // We have to enable the SE_SECURITY_NAME
    // privilege in order to play with SACLs
    turnPrivilegeOn( String(SE_SECURITY_NAME) );

    dwSIDSize = sizeof (bySIDBuffer);

    dwAccess |= ACCESS_SYSTEM_SECURITY;

    if ( SysSecurityDescriptor::nameToSID( user, (PSID) bySIDBuffer, &dwSIDSize ) )
    {
        bRetcode = ::AddAuditAccessAce( _pACL, ACL_REVISION, dwAccess, bySIDBuffer, TRUE, TRUE );
    }

    return bRetcode;
}


BOOL 
SysSacl::auditUserAccess(
            const std::vector<String> &users,
            DWORD  dwAccess /*= STANDARD_RIGHTS_REQUIRED | GENERIC_ALL | ACCESS_SYSTEM_SECURITY */)
{
    BOOL bRetcode = FALSE;

    std::vector<String>::size_type sz = users.size();
    for ( std::vector<String>::size_type i = 0; i < sz; ++i )
    {
        bRetcode |= auditUserAccess( users[i], dwAccess );
    }

   return bRetcode;
}


BOOL 
SysSacl::turnPrivilegeOn(
            const String &privilegeName )
{
    HANDLE hToken = SysSecurityDescriptor::getProcessToken();

    if ( hToken == NULL )
    {
      return FALSE;
    }

    LUID PrivilegeValue;

    // Get value of privilege name
    if ( !::LookupPrivilegeValue( NULL, privilegeName.c_str(), &PrivilegeValue ) )
    {
        return FALSE;
    }

    // Set TOKEN_PRIVILEGES structure
    // to enable the requested privilege
    TOKEN_PRIVILEGES NewPrivilegeSet;

    NewPrivilegeSet.PrivilegeCount = 1;
    NewPrivilegeSet.Privileges[0].Luid = PrivilegeValue;
    NewPrivilegeSet.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

    DWORD dwOutLength;

    ::AdjustTokenPrivileges( hToken, FALSE, 
                &NewPrivilegeSet, sizeof (TOKEN_PRIVILEGES),
                &m_CurrentPrivileges, &dwOutLength );

    return ( ::GetLastError() == NO_ERROR );
}


BOOL 
SysSacl::restorePrivileges()
{
   HANDLE hToken = SysSecurityDescriptor::getProcessToken();

   if ( hToken == NULL )
   {
      return FALSE;
    }

    ::AdjustTokenPrivileges( hToken, FALSE,
                &m_CurrentPrivileges, sizeof (TOKEN_PRIVILEGES), NULL, NULL);

    return ( ::GetLastError() == NO_ERROR );
}

