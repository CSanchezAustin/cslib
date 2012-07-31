//      SysSecurityDescriptor.cpp
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
//              Copyright (c) 1996 Ralph P. Davis
// 
//      RCS:
//                 $Revision:$
//                 $Author:$
//                 $Date:$
//
//      Contents:	
//	
#include <cslib.h>

#include <sys/SysSecurityDescriptor.h>
#include <sys/SysSecurity.h>


const String SysSecurityDescriptor::_gAuditUsers( NTEXT("Everyone") );

SysSecurityDescriptor::SysSecurityDescriptor(
            const BOOL bNotUsed )
{
    _pDACL = NULL;
    _pSACL = NULL;
    _pSD   = NULL;
}


SysSecurityDescriptor::SysSecurityDescriptor(
            const std::vector<String> &allowedUsers,
            const std::vector<String> &deniedUsers,
            const BOOL      bAuditAccess )
{
    _pDACL = NULL;
    _pSACL = NULL;

    _pSD = (SECURITY_DESCRIPTOR *) new BYTE[ 16384 ];
	::memset( (void*)_pSD, 0x00, 16384 );
    
    if ( !::InitializeSecurityDescriptor( _pSD,  SECURITY_DESCRIPTOR_REVISION ) )
    {
        // Windows 95?
        if ( ::GetLastError() != ERROR_CALL_NOT_IMPLEMENTED )
            throw ::GetLastError();
        return;
    }

   // Add denied ACEs to ACL first
    if ( deniedUsers.size() > 0 )
    {
        if ( _pDACL == NULL )
        {
            _pDACL = new SysDacl;
        }

        for ( std::vector<String>::size_type i = 0; i < deniedUsers.size(); ++i )
        {
            _pDACL->denyUserAccess( deniedUsers[ i ] );
        }
    }

    if ( allowedUsers.size() > 0  )
    {
        if ( _pDACL == NULL )
        {
            _pDACL = new SysDacl;
        }

        for ( std::vector<String>::size_type i = 0; i < allowedUsers.size(); ++i )
        {
            _pDACL->allowUserAccess( allowedUsers[ i ] );
        }
    }

    if ( ( deniedUsers.size() > 0 ) || ( allowedUsers.size() > 0 ) )
    {
        BOOL isSet = setDacl( _pDACL );
    }

    if ( bAuditAccess )
    {
        std::vector<String> v;
        v.push_back( _gAuditUsers );

        _pSACL = new SysSacl( v );
        setSacl( _pSACL );
    }
}


SysSecurityDescriptor::~SysSecurityDescriptor()
{
   delete _pDACL;
   delete _pSACL;
   delete [] _pSD;
}


SysSecurityDescriptor::operator SECURITY_DESCRIPTOR*()
{
    return _pSD;
}


BOOL 
SysSecurityDescriptor::nameToSID(
            const String &name,
            PSID pSID, 
            LPDWORD lpdwSIDSize)
{
   TCHAR szDomain[256];
   DWORD dwDomainSize = sizeof (szDomain);
   SID_NAME_USE SidNameUse;

   BOOL bRetcode = ::LookupAccountName(NULL, name.c_str(), pSID, 
          lpdwSIDSize, szDomain, &dwDomainSize, &SidNameUse);

   return bRetcode;
}


HANDLE 
SysSecurityDescriptor::getProcessToken()
{
    HANDLE hToken;
    BOOL   bRetval;
    DWORD  dwError;

    bRetval = ::OpenProcessToken( ::GetCurrentProcess(), TOKEN_ALL_ACCESS, &hToken );

    dwError = ::GetLastError();

    if ( !bRetval )
    {
        return NULL;
    }
    
    return hToken;
}


HANDLE 
SysSecurityDescriptor::getThreadToken()
{
    HANDLE hToken;
    BOOL   bRetval;
    DWORD  dwError;

    bRetval = ::OpenThreadToken( ::GetCurrentThread(), TOKEN_ALL_ACCESS, TRUE, &hToken );

    dwError = ::GetLastError();

    if ( !bRetval )
    {
        return NULL;
    }

    return hToken;
}


BOOL 
SysSecurityDescriptor::setDacl(
            SysDacl *pDacl          /*= NULL*/,
            BOOL   bDaclPresent   /*= TRUE*/,
            BOOL   bDaclDefaulted /*= FALSE*/)
{
   PACL pACL = NULL;

    if ( pDacl != NULL )
    {
        pACL = pDacl->GetACL();
    }

	BOOL res = ::SetSecurityDescriptorDacl( _pSD, bDaclPresent, pACL, bDaclDefaulted );
    return  res;
}


BOOL 
SysSecurityDescriptor::setSacl(
            SysSacl *pSacl,
            BOOL   bSaclPresent   /*= TRUE*/,
            BOOL   bSaclDefaulted /*= FALSE*/)
{
   PACL pACL = NULL;

    if (pSacl != NULL)
    {
        pACL = pSacl->GetACL();
    }

	BOOL res = ::SetSecurityDescriptorSacl( _pSD, bSaclPresent, pACL, bSaclDefaulted );
    return res;
}


SysSecurityDescriptor*
SysSecurityDescriptor::createPrivateObject(
            const std::vector<String>  &allowedUsers,
            const std::vector<String>  &deniedUsers,
            BOOL bAuditAccess     /* = FALSE*/)
{
    // Create a template SECURITY_DESCRIPTOR
    // from the input arguments
    SysSecurityDescriptor TemplateSD(
            allowedUsers, deniedUsers, bAuditAccess);

    SysSecurityDescriptor *pNewSD = NULL;
    PSECURITY_DESCRIPTOR pSecurityDescriptor;
    GENERIC_MAPPING      GenericMapping;
    HANDLE               hToken;

    hToken = SysSecurityDescriptor::getProcessToken();

    if ( hToken != NULL )
    {

        // Set generic-to-specific access mapping as if
        // this object were a file
        GenericMapping.GenericRead    = FILE_GENERIC_READ;
        GenericMapping.GenericWrite   = FILE_GENERIC_WRITE;
        GenericMapping.GenericExecute = FILE_GENERIC_EXECUTE;
        GenericMapping.GenericAll     = FILE_ALL_ACCESS;

        // When we call CreatePrivateObjectSecurity(),
        // we pass the second argument as a pointer to
        // our template security descriptor
        if ( CreatePrivateObjectSecurity( NULL, TemplateSD.getSecurityDescriptor(),
                        &pSecurityDescriptor, FALSE, hToken, &GenericMapping ) )
        {
            std::vector<String> v;

            pNewSD = new SysSecurityDescriptor( TRUE );
            pNewSD->setSecurityDescriptor( pSecurityDescriptor );
            if ( ( allowedUsers.size() > 0 )|| ( deniedUsers.size() > 0 ) )
            {
                // CreatePrivateObjectSecurity() has already
                // created ACEs
                // All we need is a SysDacl object
                pNewSD->setDacl( new SysDacl( v ) );
            }

            if ( bAuditAccess )
            {
                // CreatePrivateObjectSecurity() has already
                // create the system ACEs
                // All we need is a SysSacl object
                pNewSD->setSacl( new SysSacl( v ) );
            }
        }
    }

    return pNewSD;
}


BOOL 
SysSecurityDescriptor::destroyPrivateObject()
{
   PSECURITY_DESCRIPTOR pSecurityDescriptor = (PSECURITY_DESCRIPTOR) getSecurityDescriptor();

   BOOL bRetcode = 
      ::DestroyPrivateObjectSecurity( &pSecurityDescriptor );

   _pSD = NULL;
   delete this;

   return bRetcode;
}
