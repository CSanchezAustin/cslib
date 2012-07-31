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
//
//  Values are 32 bit values laid out as follows:
//
//   3 3 2 2 2 2 2 2 2 2 2 2 1 1 1 1 1 1 1 1 1 1
//   1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0
//  +---+-+-+-----------------------+-------------------------------+
//  |Sev|C|R|     Facility          |               Code            |
//  +---+-+-+-----------------------+-------------------------------+
//
//  where
//
//      Sev - is the severity code
//
//          00 - Success
//          01 - Informational
//          10 - Warning
//          11 - Error
//
//      C - is the Customer code flag
//
//      R - is a reserved bit
//
//      Facility - is the facility code
//
//      Code - is the facility's status code
//
//
// Define the facility codes
//


//
// Define the severity codes
//
#define STATUS_SEVERITY_WARNING          0x2
#define STATUS_SEVERITY_USER             0x1
#define STATUS_SEVERITY_SUCCESS          0x0
#define STATUS_SEVERITY_INFORMATIONAL    0x1
#define STATUS_SEVERITY_ERROR            0x3
#define STATUS_SEVERITY_DEBUG            0x1


//
// MessageId: CSLIBMSG_ERROR
//
// MessageText:
//
// %1
//
#define CSLIBMSG_ERROR                   ((DWORD)0xC0000000L)

//
// MessageId: CSLIBMSG_INFO
//
// MessageText:
//
// %1
//
#define CSLIBMSG_INFO                    ((DWORD)0x40000001L)

//
// MessageId: CSLIBMSG_WARNING
//
// MessageText:
//
// %1
//
#define CSLIBMSG_WARNING                 ((DWORD)0x80000002L)

//
// MessageId: CSLIBMSG_DEBUG
//
// MessageText:
//
// %1
//
#define CSLIBMSG_DEBUG                   ((DWORD)0x40000064L)

//
// MessageId: CSLIBMSG_USERDEFINED
//
// MessageText:
//
// %1
//
#define CSLIBMSG_USERDEFINED             ((DWORD)0x40000065L)

//
// MessageId: CSLIBMSG_WIN32_ERROR
//
// MessageText:
//
// %1
//
#define CSLIBMSG_WIN32_ERROR             ((DWORD)0xC0000066L)

