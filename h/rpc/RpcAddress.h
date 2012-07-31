#ifndef RpcAddress_H_LOADED
#define RpcAddress_H_LOADED
// [
// $        RpcAddress.h
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
//         Contents: RpcAddress class
//
//         Purpose: 
//
//         Usage Notes:
//              This class represents an address to an RPC entity in the system
//              an address is represented by the following URL
//              transport:address
//              Where:
//              transport = the transport identifier (e.g. rpc-http-client)
//              address = transport specific address (e.g. 204.152.12.12:8080 / IP address and port)
//                 
// ?
// !        Changes History
//     $Header: $
//         
// ]

#include <cslib.h>
#include <rpc/RpcTypes.h>
#include <util/UtilStringUtils.h>


class EXPORT_DLL RpcAddress
{
public:
    RpcAddress() {}
    RpcAddress( const String& url)
    {
    	StringUtils::splitFront(COLON, url, &_transport, &_address);
    }

    RpcAddress(const RpcAddress& rhs)
    {
        *this = rhs;
    }

    virtual ~RpcAddress(){}

    RpcAddress& operator=( const RpcAddress& rhs )
    {
        if ( this != &rhs )
        {
            _transport = rhs._transport;
            _address = rhs._address;
        }

        return *this;
    }

    const String&  getTransport() const
    {
	    return _transport;
    }

    /**
	    To Get the Transport Name
    **/
    void setTransport(const String& transport)
    {
	    _transport = transport;
    }

    /**
	    To get the Server Url
    **/
    const String& getAddress() const
    {
	    return _address;
    }

    /**
	    To set the Server Url
    **/
    void setAddress(const String& address)
    {
	    _address = address;
    }

    String toString() const 
    {
        String res = _transport;
        res += COLON;
        res += _address;

        return res;
    }

	String getURL() const
	{
        String res = _transport;
        res += COLON;
        res += _address;

        return res;
	}

private:
    String _transport;
    String _address;
};

// RpcAddress_H_LOADED
#endif
