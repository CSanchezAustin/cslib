#ifndef RpcAddressImpl_H_LOADED
#define RpcAddressImpl_H_LOADED
// [
// $        RpcAddressImpl.h
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
//         RCS:
//                 $Revision: 2.0 $
//                 $Author: $
//                 $Date: $
// ?
//         Contents: RpcAddressImpl class
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

#include <cslib.h>
#include <rpc/RpcTypes.h>
#include <rpc/iRpcAddressImpl.h>


class EXPORT_DLL RpcAddressImpl : public iRpcAddressImpl
{
public:
	RpcAddressImpl();
	RpcAddressImpl(const String& url);
    ~RpcAddressImpl(){};
	
	/**
		To Get and Set the Transport Name
	**/
	String getTransportName() const;

	/**
	 To parse the Transport name and the server add
	 the format for Sstring should be
	 <transportname>:<ServerAdd>
	 make sure that transport name should not contain any Colon
	**/
	void setTransportName(const String& transname);

	/**
		To get and Set the Server Url
	**/
	String getServerAdd() const;
	void setServerAdd(const String& serurl);
private:
	/**
		To storoe the Transport Name
	**/
	String _transportname;

	/**
		To store the Server Url
	**/
	String _serverurl;

	 // Unimplemented standard methods
    RpcAddressImpl( const RpcAddressImpl& );
    RpcAddressImpl& operator=(const RpcAddressImpl&);

};

// RpcAddressImpl_H_LOADED
#endif
