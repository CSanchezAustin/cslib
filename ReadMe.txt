Overview:
The following source code provides libraries for some common programming
tasks. The following is a list of utility interfaces.

core: Core functions of the library
	Assert: assert macros
	Debug: debug macros
	HandleObject: generic reference counted handle object (Windows implemented currently)

file: Utilities to iterate directories and manipulate files
	FileAttributes: a class describing all attributes of a file
	FileIterator: generic class for iterating a directory
	FileIteratorImp: Interface for the actual iterator implementation
	FileIteratorImpwin32: Windows iterator implementation
	FileUtils: file manipulation utilities

fwk: framework interfaces
	fwkConduit: an bi-directional synchronous broadcast mechanism
	ifwkConduitListener: interface for a listener of a conduit

lang: language utilities
	Date: a generic date class representing operations on dates
	InterlockedLong: long integer whose operations are guaranteed to be atomic
	ScheduleTime: a fine resolution timer (Windows implemented currently)
	String: wrapper for a string or wstring based on unicode settings

log: logging subsystem
	iLoggerImpl: Plugin module interface to extend the StdLogger.
	LogImplementationFactory: logger implementation registry for dynamic creation of log implementations
	LogStdFileImpl: file logger implementation
	LogStdFormatter: object for formatting a line of text for the logger
	LogStdLogger: System log component

log/nt: NT Evelt log subsystem
	LogNTEventLogImpl: NT event log implementation

net: network subsystem
	NetHttpClientPost: client-side HTTP post
	NetHttpContent: HTTP content object
	NetHttpSerPost: server-side HTTP post
	NetHttpUtils: HTTP utilities
	NetSocket: socket class

pat: interfaces for various patterns used in the code

	iComparable: Interface to compare two classes
	iDebuggable: interface supporting system debugging (rarely used anymore)
	iRunnable: interface for objects that need to be run by the thread pool
	iSchedulable: Interface for objects that need to be scheduled to 
			execute by the thread scheduler
	iSerializable: interface for serializing to a DOM_Node

rpc: an XML rpc system

	iRpcAddressImpl: interface for a transport specific address
	iRpcAuthenticator: interface for an authentication object. Derivitives of this object
			should register themselves with RpcAuthenticatorFactory for dynamic creation
			by the RpcServer
	iRpcClientTransport: client transport interface
	iRpcMethod: interface representing a function for the RPC mechanism to call
			contain one or more interfaces.1
	iRpcServer: RPC server interface
	iRpcTransport: interface for a transport object. Derivitives of this object
			should register themselves with RpcTransportFactory for dynamic creation
			by the RpcServer
	RpcAddress: RPC address
	RpcAddressImpl: implementation of an rpc address (unused)
	RpcAuthenticatorFactory: global registry of creator functions for objects of type "iRpcAuthenticator"
	RpcClient: RPC client class
	RpcClientTransportFactory: RPC client registry
	RpcDispatcher: the dispatch mechanism to call an interface and method based on
			an XML document's processing instructions. The dispatcher can
	RpcErrorHandler: DOM rpc error catcher
	RpcInterface: Class containing multiple one or more methods that can be called 
			by the dispatcher.
	RpcMessage: a message class containing a request header and data
	RpcServer: the main class responsible for managing the dispatcher, transport and authenticators
	RpcTransportFactory: global registry of creator functions for objects of type "iRpcTransport"
	enum enumRpcCallResult: enumeration of results of the RPC syste, (see RpcTypes.h)
	enum eRpcStatus: RPC server status (see RpcTypes.h)
	enum eRpcControl: RPC transport control (see RpcTypes.h)

rpc/http: HTTP transport
	RpcHttpTransport: server HTTP transport implementation
	RpcHttpClientTransport: client HTTP transport implementation
	RpcHttpAuthenticator: HTTP authenticator

svr: server subsystem
	iSvrService: generic system service interface

svr/nt: NT SCM implementation
	CService: C++ service wrapper
	CServiceInstall: service installer
	NTService: NT services interface

sys: System level components
	iSysComponent: A dynamically loaded component used by the system loader
	SysComInitializer: COM initializer
	SysComponentFactory: factory for the dynamic component loader (iSysComponent>
	SysComponentLoader: creates and initialized components from an XML config file
	SysContext: system registry
	SysCounterMgr: a system component used to control access to runtime counters
	SysManager: various utility functions
	SysObjectFactory<T>: template factory for the dynamic creation of objects T
	SysObjectFactoryInlines: implementation of SysObjectFactory
	SysPathMgr: path resolution service
	SysAcl: access control list (Windows only)
	SysDacl: class to add/deny access to a SysAcl (Windows only)
	SysSacl: audit interfaces (Windows only)
	SysSecurityAttributes: C++ wrapper for Windows objects (Windows only)
	SysSecurityDescriptor: C++ wrapper for Windows objects (Windows only)

thd: Threading objects
	ThdAutoLock: stack-based access control to other synchronization objects
	ThdAutoCriticalSection: stack-based access control to a ThdCriticalSection
	ThdCriticalSection: interface for a lightweight critical secion (Windows implemented currently)
	ThdDeque: double ended queue
	ThdEvent: Interface for an event synchronization object (Windows implemented currently)
	ThdManager: interface for creating new threads
	ThdMutex: Interface for an mutex synchronization object (Windows implemented currently)
	ThdPool: Thread pool system component. Can add permanent threads to execute or can submit 
			objects of type iRunnable to execute in various execution modes (once, immediate)
	ThdQueue<class T>: A sharable queue for threads to communicate passing objects of type T
	ThdRunner<class T>: Wrapper implementing the iRunnable interface enabling a C++ class method
			to be called directly.
	ThdRWLock: single writer/multiple reader lock
	ThdScheduler: Class enabling objects of type iSchedulable to be scheduled with high precision. This class
			reduces overall system requirements for active threads required to handle large 
			number of active items requiring processor time.
	ThdSemaphore: Interface for an semaphore synchronization object (Windows implemented currently)
	ThdSyncObject: base class containing a shared synchronization object (Windows implemented currently)
	ThdThread: Class implementing a system thread (Windows implemented currently)
	ThdWaitableTimer: Interface for an timer synchronization object (Windows implemented currently)

util: Common utilities

	AutoPtr<class T>: an invasive reference counted pointer. class T must be 
			derived from RefCounted
	DllLoader: DLL loader mechanism
	Heap<class Key, class Type, class KeyCompare, class TypeCompare>: A priority queue providing
			quick lookup and manipulation of objects.
	Mapping: an extension to a STL map<> with functionality to support different keys
			and data types
	MappingRef: the structure used by the Mapping to represent multiple data types
	RefCounted: base class for AutoPtr<>
	RefCountedPtr<class T>: a non-invasive reference counted pointer. Can be used to
			reference count any type of pointer.
	StreamUtils: stream utilities
	StringTokenizer: crude implementation of a tokenizer
	StringUtils: string conversion utilities

xml: Utilities for making manipulation of the DOM easier
	DomUtils: Utilities to make manipulating and iterating DOM easier.
	StreamFormatTarget: stream formatter
	StringFormatTarget: string formatter
	StXmlOpener: Stack-based class to initialize the XML sub-system


Licensing:
See License.txt for details


Building:
The following code has been compiled and tested with Microsoft Visual C++ 6.

It has the following dependencies:

STL Port 4.0 (www.stlport.org)
Xerces C++ Parser (www.apache.org)

The following directives control the behavior of the code.

Directive	Purpose
_DEBUG		Debug mode
NDEBUG		Release mode
_WINDOWS	Windows platform build (currently the only supported)
WIN32		Windows platform build (currently the only supported)
UNICODE		if set String will be a wstring rather than a string object
USE_DLL		The library is being built as a DLL. If not defined the following do not apply
BUILD_DLL	Whether or not building a DLL or using the DLL interface. 
		Used to import or import definitions (Sets EXPORT_DLL)


The following environment variables are required to be set when building the code.

Variable	Purpose
CS_STL		Path to STL headers
CS_XMLPARSER	Path to Xerces directory


The following macros are defined by the source

Macro		Purpose
NTEXT		Handle inline text conversion to unicode if UNICODE is defined.
ASSERT		Assert in all modes
ASSERT_D	Assert in debug mode
EXPORT_DLL	Platform specific export/import declarations 
NULL		define null

