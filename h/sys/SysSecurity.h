#ifndef __SysSecurity_H_LOADED
#define __SysSecurity_H_LOADED

/********
*
* SECURITY.H
*
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
* Copyright (c) 1996 Ralph P. Davis
         Copyright (c) 1998-2000, Chris Sanchez
*
* All Rights Reserved
*
*****/

#include <vector>

#include <cslib.h>

class EXPORT_DLL SysAcl
{
protected:
   PACL _pACL;

public:
   SysAcl();
   virtual ~SysAcl();

    PACL GetACL() { return _pACL; }
};


class EXPORT_DLL SysDacl : public SysAcl
{
public:
   SysDacl() {}
   SysDacl( 
            const std::vector<String> &users, 
            DWORD dwAccess = GENERIC_ALL | STANDARD_RIGHTS_REQUIRED);

   BOOL allowUserAccess(
           const String &user,
           DWORD  dwAccess = GENERIC_ALL | STANDARD_RIGHTS_REQUIRED);

   BOOL allowUserAccess(
            const std::vector<String> &users, 
             DWORD  dwAccess = GENERIC_ALL | STANDARD_RIGHTS_REQUIRED);
             
   BOOL denyUserAccess(
           const String &user,
           DWORD  dwAccess = GENERIC_ALL | STANDARD_RIGHTS_REQUIRED);

   BOOL denyUserAccess(
            const std::vector<String> &users, 
            DWORD  dwAccess = GENERIC_ALL | STANDARD_RIGHTS_REQUIRED);
         
};

class EXPORT_DLL SysSacl : public SysAcl
{
protected:
    TOKEN_PRIVILEGES m_CurrentPrivileges;
    BOOL             turnPrivilegeOn( const String &privilegeName );
    BOOL             restorePrivileges();

public:
    SysSacl(
            const std::vector<String> &users, 
            DWORD dwAccess = GENERIC_ALL | STANDARD_RIGHTS_REQUIRED | ACCESS_SYSTEM_SECURITY);

   virtual ~SysSacl();

    BOOL auditUserAccess(
           const String &user,
           DWORD  dwAccess = GENERIC_ALL | STANDARD_RIGHTS_REQUIRED | ACCESS_SYSTEM_SECURITY);
   
    BOOL auditUserAccess(
            const std::vector<String> &users, 
            DWORD  dwAccess = GENERIC_ALL | STANDARD_RIGHTS_REQUIRED | ACCESS_SYSTEM_SECURITY);
};

#endif // __SysSecurity_H_LOADED

