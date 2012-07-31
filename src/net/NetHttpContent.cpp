// [
// $        HttpContent.cpp
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
// 
//         RCS:
//                 $Revision:$
//                 $Author:$
//                 $Date:$
// 
// ]

#include <cslib.h>

#include <net/NetHttpContent.h>
#include <net/NetHttpUtils.h>
#include <util/UtilStringTokenizer.h>


HttpContent::HttpContent() :
    _contentIsForm(false),
    _msgType(HTTP_MSG_TYPE_NONE),
    _version(HttpUtils::HTTP_VERSION),
    _serverPath(HttpUtils::HTTP_DEFAULT_SVRPATH)
{
    setMimeType(HttpUtils::HTTP_MIME_PLAINTEXT);
    setMimeEncoding(HttpUtils::HTTP_ENC_UTF8);
}


HttpContent::~HttpContent()
{
    
}


/********************************************************************
    These functions are generic to both the request and response

********************************************************************/



/********************************************************************
    sets the mime type of the http content

********************************************************************/
void 
HttpContent::setMimeType(
    const String& mime) 
{
    _mimeType = mime;

    // add the content-type header
    String headerVal = _mimeType;
    if ( !_encoding.empty() )
    {
        headerVal += SEMICOLON;
        headerVal += SPACE;
        headerVal += HttpUtils::HTTP_CHARSET;
        headerVal += EQUAL;
        headerVal += _encoding;
    }

    setHeaderProperty( HttpUtils::HTTP_CONTENT_TYPE, headerVal );
}


/********************************************************************
    sets the encoding type of the http content

********************************************************************/
void 
HttpContent::setMimeEncoding(
    const String& enc) 
{
    _encoding = enc;

    // add the content-type header
    String headerVal = _mimeType;
    headerVal += SEMICOLON;
    headerVal += SPACE;
    headerVal += HttpUtils::HTTP_CHARSET;
    headerVal += EQUAL;
    headerVal += _encoding;

    setHeaderProperty( HttpUtils::HTTP_CONTENT_TYPE, headerVal );
}


/********************************************************************
    gets the a header property for the content

********************************************************************/
const String&
HttpContent::getHeaderProperty(
    const String& propName) const
{
    StringUtils::StringMap::const_iterator it = _headers.find( propName );
    if (it != _headers.end())
    {
        return (*it).second;
    }

    return EMPTYSTR;
}


/*********************************************************************
    generates a http content request

*********************************************************************/
bool
HttpContent::generate(
    String& header, 
    String& body) const
{
    header = generateCommand();
    if ( header.empty() )
    {
        return false;
    }

    header += HttpUtils::HTTP_ENDL;
    header += generateHeader();

    // before we add a line, we have to ensure that
    // we generate the body of the content and length header
    // if this is a request
    if ( _msgType == HTTP_MSG_TYPE_REQUEST )
    {
        body.erase();
        // flaten out the query string

        // if the application data is a form, then we
        // have to generate query strings that are url encoded
        if ( _contentIsForm )
        {
            for (QueryMap::const_iterator it = _queries.begin(); 
                 it != _queries.end(); 
                 it ++)
            {
                if ( !body.empty() )
                {
                    body += AMPERSAND;
                }

                String tempStr;
                HttpUtils::urlEncode( (*it).first, tempStr );
                body += tempStr;
                body += EQUAL;
                HttpUtils::urlEncode( (*it).second, tempStr );
                body += tempStr;
            }
        }
        else
        {
            body = _body;
        }

        // add the content-length header 
        header += HttpUtils::HTTP_CONTENT_LEN;
        header += COLON;
        header += SPACE;
        header += StringUtils::toString( (int)body.length() );
        header += HttpUtils::HTTP_ENDL;
    }
    else
    {
        // otherwise reference the raw body
        body = _body;
    }

    // Append end-of-line indicating end of headers
    header += HttpUtils::HTTP_ENDL;

    return true;
}


/********************************************************************
    generates the first line of a http message

********************************************************************/
String 
HttpContent::generateCommand() const
{
    String command;

    switch (_msgType)
    {
    case HTTP_MSG_TYPE_NONE:
    {
        // error
        return EMPTYSTR;
    }

    case HTTP_MSG_TYPE_REQUEST:
    {
        command = HttpUtils::HTTP_POST_METHOD;
        command += SPACE;

        // add server path
        command += _serverPath;
        command += SPACE;

        //add version
        command += HttpUtils::HTTP_VERSION_PREFIX;
        command += _version;

        break;
    }
    case HTTP_MSG_TYPE_RESPONSE:
    {
        command += HttpUtils::HTTP_VERSION_PREFIX;
        command += _version;
        command += SPACE;
        command += StringUtils::toString( _errorCode );
        if ( _errDesc.length() > 0 )
        {
            command += SPACE;
            command += _errDesc;
        }
    }
    }

    return command;
}


/********************************************************************
    generates all the headers needed for a http message

********************************************************************/
String 
HttpContent::generateHeader() const
{
    String headers;

    // Append the header properties ...
    for (StringUtils::StringMap::const_iterator it = _headers.begin(); 
         it != _headers.end(); 
         it++)
    {
        headers += (*it).first;
        headers += COLON;
        headers += SPACE;
        headers += (*it).second;
        headers += HttpUtils::HTTP_ENDL;
    }

    return headers;
}



/********************************************************************
    parses a the header into the http content from a buffer
    based on the contents of the buffer, the internal state
    of the http content is set to indicate whether or not the content
    represents a request or response.
********************************************************************/
bool 
HttpContent::parseHeader(
    const String& request)
{
    bool res = false;

    _headers.clear();
    
    StringTokenizer tok( request, HttpUtils::HTTP_ENDL );

    HttpUtils::ParseState state = HttpUtils::keCommand;
    while (tok.hasMoreTokens() && ( state != HttpUtils::keComplete ) )
    {
        String curTok = tok.getNextToken();

        // this section of code ensures that we transition from state
        // to state properly. The transitions are 
        // Current state    Next state  Trigger
        //  keCommand       keHeader    cr/lf
        //  keHeader        keBody      cr/lfcr/lf
        //  keBody          keComplete  cr/lf

        switch ( state )
        {
        case HttpUtils::keCommand:
            res = parseCommand( curTok );
            if ( res )
            {
                state = HttpUtils::keHeader;
            }
            else
            {
                state = HttpUtils::keComplete;
            }
            break;

        case HttpUtils::keHeader:
            res = parseOneHeader( curTok );
            if ( !res )
            {
                state = HttpUtils::keComplete;
            }
            break;

        default:
            DBG_LOGERR(NTEXT("HttpContent::parseHeader: got unknown state while parsing"));
            state = HttpUtils::keComplete;
            res = false;
            break;
        }
    }

    return res;
}


/********************************************************************
    Parses out the initial line of the http content. This initial
    line indicates what the type of the content is (request or response)

********************************************************************/
bool 
HttpContent::parseCommand(
    const String& command )
{
    StringTokenizer commandTok( command, SPACE );

    // first get the http command ( determine if this is a request
    // or a response
    String tempStr = commandTok.getNextToken();

    if ( tempStr == HttpUtils::HTTP_POST_METHOD )
    {
        _msgType = HTTP_MSG_TYPE_REQUEST;
    }
    else if ( tempStr.find(HttpUtils::HTTP_NAME) == 0 )
    {
        _msgType = HTTP_MSG_TYPE_RESPONSE;
    }
    else
    {
        _msgType = HTTP_MSG_TYPE_NONE;
        DBG_LOGERR(NTEXT("HttpContent::parseCommand: Unrecognized command"));
        return false;
    }

    if ( _msgType == HTTP_MSG_TYPE_REQUEST )
    {
        // then we expect the server path and the http version to be next

        // server path
        tempStr = commandTok.getNextToken();
        if ( tempStr.length() != 0 )
        {
            setServerPath( tempStr );
        }
        else
        {
            DBG_LOGERR(NTEXT("HttpContent::parseCommand: Http server path not found"));
            return false;
        }

        // version
        if ( StringUtils::splitFront( SLASH, commandTok.getNextToken(), NULL, &tempStr ) )
        {
            setHttpVersion( tempStr );
        }
        else
        {
            DBG_LOGERR(NTEXT("HttpContent::parseCommand: Http version not found"));            
            return false;
        }
    }
    else
    {
        // otherwise we expect the version, error code and error description to be next
        
        // first extract the version from the current token
        String version;
        if ( StringUtils::splitFront( SLASH, tempStr, NULL, &version ) )
        {
            setHttpVersion( version );
        }
        else
        {
            DBG_LOGERR(NTEXT("HttpContent::parseCommand: Http version not found"));            
            return false;
        }

        // error code
        tempStr = commandTok.getNextToken();
        if ( tempStr.length() != 0 )
        {
            setErrorCode( StringUtils::toInt( tempStr ) );
        }
        else
        {
            DBG_LOGERR(NTEXT("HttpContent::parseCommand: Http error code not found"));            
            return false;
        }

        // error description (optional)
        tempStr = commandTok.getNextToken();
        if ( tempStr.length() != 0 )
        {
            setErrorDesc( tempStr );
        }
    }

    return true;
}


/********************************************************************
    Parses out one single line of a http header and sets internal
    information based on the types of headers it understands

********************************************************************/
bool 
HttpContent::parseOneHeader(
    const String& header )
{
    // parse the header key from the
    String key;
    String value;
    if ( StringUtils::splitFront( COLON, header, &key, &value ) )
    {
        StringUtils::trimBoth( SPACE, key );
        StringUtils::trimBoth( SPACE, value );

        setHeaderProperty( key, value );

        bool isContent = 
            StringUtils::compareCaseless( key, HttpUtils::HTTP_CONTENT_TYPE ) == 0;
        // now check and see if it is an important type we need to know about
        if ( isContent && 
             ( value.find(HttpUtils::HTTP_FORM_APPLICATION_FORM) != String::npos ) )
        {
            setIsForm();
        }
        else if ( isContent )
        {
            // content type headers are in for format type/subtype[; charset=encoding]
            String mimeType;
            String encoding;
            if ( StringUtils::splitFront( SEMICOLON, value, &mimeType, &encoding ) )
            {
                setMimeType( mimeType );
                StringUtils::trimFront( SPACE, encoding );

                // now search for the equal sign (charset=)
                String charset;
                if ( StringUtils::splitFront( EQUAL, encoding, NULL, &charset ) )
                {
                    setMimeEncoding( charset );
                }
                else
                {
                    DBG_LOGERR(NTEXT("HttpContent::parseOneHeader: could not parse charset encoding"));
                    return false;
                }
            }
            else
            {
                // only the mime type is provided
                setMimeType( value );
            }
        }
    }
    else
    {
        DBG_LOGERR(NTEXT("HttpContent::parseOneHeader: improperly formatted http header"));            
        return false;
    }

    return true;
}


/********************************************************************
    parses a the header into the http content from a buffer
    The body is set depending on whether or not the http content
    represents a request or a response
********************************************************************/
bool 
HttpContent::parseBody(
    const String& body )
{
    bool res = false;

    _body.erase();
    _queries.clear();

    if (StringUtils::toInt(getHeaderProperty(HttpUtils::HTTP_CONTENT_LEN)) == (int)body.length())
    {
        // if the message type is a request, then we
        // parse out query strings
        if ( _msgType == HTTP_MSG_TYPE_REQUEST )
        {
            setRequestBody( body );
        }
        else
        {
            setResponseBody( body );
        }

        res = true;
    }
    else
    {
        DBG_LOGERR(NTEXT("readHttpData: invalid content length"));
        res = false;
    }

    return res;
}



/********************************************************************
    These sets of functions are specific to a content of type
    HTTP_MSG_TYPE_REQUEST. They include:
        setIsForm: indicates whether or not the content is in Web 
                    form format (query strings)
        getQueryString: retrieve a form query string
        setRequestBody: set the body (content or query string)
        getRequestBody: gets the content body of the request if not a form
        doesAcceptType: tells whether or not the request accepts a mime type
        getAcceptList: gets the list of mime types accepted
        getAcceptEncoding: the encodings accepted
        getAcceptLanguage: the language accepted
        getAcceptCharset: the charset accepted

********************************************************************/



/********************************************************************
    This is called to treat the content being read in as a form
    forms have query strings which are pairs of keys and values
    delimited by = and separated by &. The entire query string is 
    url encoded.
********************************************************************/
void HttpContent::setIsForm()
{
    ASSERT_D( _msgType == HTTP_MSG_TYPE_REQUEST );
    _contentIsForm = true;

    setHeaderProperty( HttpUtils::HTTP_CONTENT_TYPE, HttpUtils::HTTP_FORM_APPLICATION_FORM );
}


/********************************************************************
    get a query string from a form request http content
    @param key the query key
    @return the query value
********************************************************************/
const String& 
HttpContent::getQueryString(
    const String& key ) const
{
    ASSERT_D( _contentIsForm );
    ASSERT_D( _msgType == HTTP_MSG_TYPE_REQUEST );

    QueryMap::const_iterator it = _queries.find(key);
    if (it != _queries.end())
    {
        return (*it).second;
    }

    return EMPTYSTR;
}


/********************************************************************
    sets the body of a request depending on whether or 
    not the content is a form.
********************************************************************/
void 
HttpContent::setRequestBody(
    const String& body)
{
    // the content is a form, so set the appropriate data
    ASSERT_D( _msgType == HTTP_MSG_TYPE_REQUEST );

    if ( _contentIsForm )
    {
        StringTokenizer tok( body, AMPERSAND );
        while ( tok.hasMoreTokens() )
        {
            String key;
            String val;
            if ( StringUtils::splitFront( EQUAL, tok.getNextToken(), &key, &val ) )
            {
                String keyDecoded;
                String valueDecoded;

                HttpUtils::urlDecode( key, keyDecoded );
                HttpUtils::urlDecode( val, valueDecoded );

                setQueryString( keyDecoded, valueDecoded );
            }
        }
    }
    else
    {
        _body = body;
    }
}


/********************************************************************
    get the content body if the content is not a form

********************************************************************/
const String&
HttpContent::getRequestBody() const
{
    ASSERT_D( _msgType == HTTP_MSG_TYPE_REQUEST );

    if ( !_contentIsForm )
    {
        return _body;
    }

    return EMPTYSTR;
}


bool 
HttpContent::doesAcceptType( const String& theType ) const
{
    const String &acceptValue = getAcceptList();

    return
        (StringUtils::findCaseless(acceptValue, theType ) != String::npos) ||
        (StringUtils::findCaseless(acceptValue, HttpUtils::HTTP_ACCEPT_ALL ) != String::npos);
}


const String& 
HttpContent::getAcceptList() const
{
    return getHeaderProperty( HttpUtils::HTTP_ACCEPT_LIST );
}



const String& 
HttpContent::getAcceptEncoding() const
{
    return getHeaderProperty( HttpUtils::HTTP_ACCEPT_ENC );
}


const String& 
HttpContent::getAcceptLanguage() const
{
    return getHeaderProperty( HttpUtils::HTTP_ACCEPT_LANG );
}


const String& 
HttpContent::getAcceptCharset() const
{
    return getHeaderProperty( HttpUtils::HTTP_ACCEPT_CHARSET );
}


/********************************************************************
    These sets of functions are specific to a content of type
    HTTP_MSG_TYPE_RESPONSE. They include:
        setResponseBody: sets the content body of the response

********************************************************************/



/********************************************************************
    Sets the response body for a response http content

********************************************************************/
void 
HttpContent::setResponseBody(
    const String& body )
{
    ASSERT_D( _msgType == HTTP_MSG_TYPE_RESPONSE );

    _body = body;

    setHeaderProperty(  HttpUtils::HTTP_CONTENT_LEN, 
                        StringUtils::toString( (int)_body.size() ) );
}


const String&
HttpContent::getResponseBody() const 
{
    ASSERT_D( _msgType == HTTP_MSG_TYPE_RESPONSE );
    
    if ( _msgType == HTTP_MSG_TYPE_RESPONSE )
    {
        return _body; 
    }

    return EMPTYSTR;
}


/********************************************************************
    sets the http error code to be included in the 
    header of the content

********************************************************************/
void 
HttpContent::setErrorCode(
    const int code )
{
    ASSERT_D( _msgType == HTTP_MSG_TYPE_RESPONSE );

    _errorCode = code; 

    setErrorDesc( HttpUtils::getErrorDescription( _errorCode ) );
}
