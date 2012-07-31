# Microsoft Developer Studio Project File - Name="cslib" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=cslib - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "cslib.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "cslib.mak" CFG="cslib - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "cslib - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "cslib - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName "Perforce Project"
# PROP Scc_LocalPath "."
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "cslib - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\lib\vc6\win32rel"
# PROP Intermediate_Dir "..\obj\vc6\win32rel"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "$(XERCESCROOT)\include" /I "$(XERCESCROOT)\include\xercesc" /I "..\h" /D "NDEBUG" /D "NOMINMAX" /D "WIN32" /D "_WINDOWS" /D _WIN32_WINNT=0x0400 /Yu"cslib.h" /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "cslib - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\lib\vc6\win32dbg"
# PROP Intermediate_Dir "..\obj\vc6\win32dbg"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "$(XERCESCROOT)\include" /I "$(XERCESCROOT)\include\xercesc" /I "..\h" /D "_DEBUG" /D "NOMINMAX" /D "WIN32" /D "_WINDOWS" /D _WIN32_WINNT=0x0400 /FR /Yu"cslib.h" /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ENDIF 

# Begin Target

# Name "cslib - Win32 Release"
# Name "cslib - Win32 Debug"
# Begin Group "h"

# PROP Default_Filter "*.h"
# Begin Group "core"

# PROP Default_Filter "*.h"
# Begin Source File

SOURCE=..\h\core\Assert.h
# End Source File
# Begin Source File

SOURCE=..\h\core\Debug.h
# End Source File
# Begin Source File

SOURCE=..\h\core\HandleObject.h
# End Source File
# End Group
# Begin Group "file "

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\h\file\FileAttributes.h
# End Source File
# Begin Source File

SOURCE=..\h\file\FileIterator.h
# End Source File
# Begin Source File

SOURCE=..\h\file\FileIteratorImp.h
# End Source File
# Begin Source File

SOURCE=..\h\file\FileIterImpWin32.h
# End Source File
# Begin Source File

SOURCE=..\h\file\FileUtils.h
# End Source File
# End Group
# Begin Group "lang "

# PROP Default_Filter "*.h"
# Begin Source File

SOURCE=..\h\lang\Date.h
# End Source File
# Begin Source File

SOURCE=..\h\lang\InterLockedLong.h
# End Source File
# Begin Source File

SOURCE=..\h\lang\ScheduleTime.h
# End Source File
# Begin Source File

SOURCE=..\h\lang\String.h
# End Source File
# End Group
# Begin Group "log "

# PROP Default_Filter ""
# Begin Group "nt "

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\h\log\nt\EventLog.h
# End Source File
# Begin Source File

SOURCE=..\h\log\nt\LogNTEventLogImpl.h
# End Source File
# End Group
# Begin Source File

SOURCE=..\h\log\iLoggerImpl.h
# End Source File
# Begin Source File

SOURCE=..\h\log\LogImplementationFactory.h
# End Source File
# Begin Source File

SOURCE=..\h\log\LogStdFileImpl.h
# End Source File
# Begin Source File

SOURCE=..\h\log\LogStdFormatter.h
# End Source File
# Begin Source File

SOURCE=..\h\log\LogStdLogger.h
# End Source File
# End Group
# Begin Group "rpc "

# PROP Default_Filter ""
# Begin Group "http"

# PROP Default_Filter "*.h"
# Begin Source File

SOURCE=..\h\rpc\http\RpcHttpAuthenticator.h
# End Source File
# Begin Source File

SOURCE=..\h\rpc\http\RpcHttpTransport.h
# End Source File
# End Group
# Begin Source File

SOURCE=..\h\rpc\iRpcAuthenticator.h
# End Source File
# Begin Source File

SOURCE=..\h\rpc\iRpcMethod.h
# End Source File
# Begin Source File

SOURCE=..\h\rpc\iRpcServer.h
# End Source File
# Begin Source File

SOURCE=..\h\rpc\iRpcTransport.h
# End Source File
# Begin Source File

SOURCE=..\h\rpc\RpcAddress.h
# End Source File
# Begin Source File

SOURCE=..\h\rpc\RpcAuthenticatorFactory.h
# End Source File
# Begin Source File

SOURCE=..\h\rpc\RpcDispatcher.h
# End Source File
# Begin Source File

SOURCE=..\h\rpc\RpcErrorHandler.h
# End Source File
# Begin Source File

SOURCE=..\h\rpc\RpcInterface.h
# End Source File
# Begin Source File

SOURCE=..\h\rpc\RpcMessage.h
# End Source File
# Begin Source File

SOURCE=..\h\rpc\RpcServer.h
# End Source File
# Begin Source File

SOURCE=..\h\rpc\RpcTransportFactory.h
# End Source File
# Begin Source File

SOURCE=..\h\rpc\RpcTypes.h
# End Source File
# End Group
# Begin Group "pat "

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\h\pat\PatComparable.h
# End Source File
# Begin Source File

SOURCE=..\h\pat\PatDebuggable.h
# End Source File
# Begin Source File

SOURCE=..\h\pat\PatRunnable.h
# End Source File
# Begin Source File

SOURCE=..\h\pat\PatSchedulable.h
# End Source File
# Begin Source File

SOURCE=..\h\pat\PatSerializable.h
# End Source File
# End Group
# Begin Group "sys "

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\h\sys\iSysComponent.h
# End Source File
# Begin Source File

SOURCE=..\h\sys\SysComInitializer.h
# End Source File
# Begin Source File

SOURCE=..\h\sys\SysComponentFactory.h
# End Source File
# Begin Source File

SOURCE=..\h\sys\SysComponentLoader.h
# End Source File
# Begin Source File

SOURCE=..\h\sys\SysContext.h
# End Source File
# Begin Source File

SOURCE=..\h\sys\SysCounterMgr.h
# End Source File
# Begin Source File

SOURCE=..\h\sys\SysManager.h
# End Source File
# Begin Source File

SOURCE=..\h\sys\SysObjectFactory.h
# End Source File
# Begin Source File

SOURCE=..\h\sys\SysObjectFactoryInlines.h
# End Source File
# Begin Source File

SOURCE=..\h\sys\SysPathMgr.h
# End Source File
# Begin Source File

SOURCE=..\h\sys\SysSecurity.h
# End Source File
# Begin Source File

SOURCE=..\h\sys\SysSecurityAttributes.h
# End Source File
# Begin Source File

SOURCE=..\h\sys\SysSecurityDescriptor.h
# End Source File
# End Group
# Begin Group "thd "

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\h\thd\ThdAutoLock.h
# End Source File
# Begin Source File

SOURCE=..\h\thd\ThdCriticalSection.h
# End Source File
# Begin Source File

SOURCE=..\h\thd\ThdDeque.h
# End Source File
# Begin Source File

SOURCE=..\h\thd\ThdEvent.h
# End Source File
# Begin Source File

SOURCE=..\h\thd\ThdManager.h
# End Source File
# Begin Source File

SOURCE=..\h\thd\ThdMutex.h
# End Source File
# Begin Source File

SOURCE=..\h\thd\ThdPool.h
# End Source File
# Begin Source File

SOURCE=..\h\thd\ThdQueue.h
# End Source File
# Begin Source File

SOURCE=..\h\thd\ThdRunner.h
# End Source File
# Begin Source File

SOURCE=..\h\thd\ThdRWLock.h
# End Source File
# Begin Source File

SOURCE=..\h\thd\ThdScheduler.h
# End Source File
# Begin Source File

SOURCE=..\h\thd\ThdSemaphore.h
# End Source File
# Begin Source File

SOURCE=..\h\thd\ThdSyncObject.h
# End Source File
# Begin Source File

SOURCE=..\h\thd\ThdThread.h
# End Source File
# Begin Source File

SOURCE=..\h\thd\ThdTypes.h
# End Source File
# Begin Source File

SOURCE=..\h\thd\ThdWaitableTimer.h
# End Source File
# End Group
# Begin Group "util "

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\h\util\UtilAutoPtr.h
# End Source File
# Begin Source File

SOURCE=..\h\util\UtilAutoPtrInlines.h
# End Source File
# Begin Source File

SOURCE=..\h\util\UtilDllLoader.h
# End Source File
# Begin Source File

SOURCE=..\h\util\UtilHeap.h
# End Source File
# Begin Source File

SOURCE=..\h\util\UtilHeapInlines.h
# End Source File
# Begin Source File

SOURCE=..\h\util\UtilMapping.h
# End Source File
# Begin Source File

SOURCE=..\h\util\UtilPattern.h
# End Source File
# Begin Source File

SOURCE=..\h\util\UtilRefCounted.h
# End Source File
# Begin Source File

SOURCE=..\h\util\UtilRefCountedPtr.h
# End Source File
# Begin Source File

SOURCE=..\h\util\UtilRefCountedPtrInlines.h
# End Source File
# Begin Source File

SOURCE=..\h\util\UtilStreaming.h
# End Source File
# Begin Source File

SOURCE=..\h\util\UtilStringTokenizer.h
# End Source File
# Begin Source File

SOURCE=..\h\util\UtilStringUtils.h
# End Source File
# End Group
# Begin Group "xml "

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\h\xml\XmlDomUtils.h
# End Source File
# Begin Source File

SOURCE=..\h\xml\XmlStreamFormatTarget.h
# End Source File
# Begin Source File

SOURCE=..\h\xml\XmlStringFormatTarget.h
# End Source File
# Begin Source File

SOURCE=..\h\xml\XmlStXmlOpener.h
# End Source File
# End Group
# Begin Group "net "

# PROP Default_Filter "*.h"
# Begin Source File

SOURCE=..\h\net\Net.h
# End Source File
# Begin Source File

SOURCE=..\h\net\NetHttpClientPost.h
# End Source File
# Begin Source File

SOURCE=..\h\net\NetHttpContent.h
# End Source File
# Begin Source File

SOURCE=..\h\net\NetHttpSerPost.h
# End Source File
# Begin Source File

SOURCE=..\h\net\NetHttpUtils.h
# End Source File
# Begin Source File

SOURCE=..\h\net\NetSocket.h
# End Source File
# End Group
# Begin Group "fwk"

# PROP Default_Filter "*.h"
# Begin Source File

SOURCE=..\h\fwk\fwkBootstrap.h
# End Source File
# Begin Source File

SOURCE=..\h\fwk\fwkConduit.h
# End Source File
# Begin Source File

SOURCE=..\h\fwk\ifwkConduitListener.h
# End Source File
# End Group
# Begin Group "svr"

# PROP Default_Filter "*.h"
# Begin Group "nt   "

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\h\svr\nt\CService.h
# End Source File
# Begin Source File

SOURCE=..\h\svr\nt\CServiceInstall.h
# End Source File
# Begin Source File

SOURCE=..\h\svr\nt\NTServiceMain.h
# End Source File
# End Group
# Begin Source File

SOURCE=..\h\svr\iSvrService.h
# End Source File
# End Group
# Begin Group "ser"

# PROP Default_Filter "*.h"
# Begin Source File

SOURCE=..\h\ser\iSerInputSource.h
# End Source File
# Begin Source File

SOURCE=..\h\ser\iSerOutputSource.h
# End Source File
# Begin Source File

SOURCE=..\h\ser\SerCounters.h
# End Source File
# Begin Source File

SOURCE=..\h\ser\SerFileInputSource.h
# End Source File
# Begin Source File

SOURCE=..\h\ser\SerFileOutputSource.h
# End Source File
# Begin Source File

SOURCE=..\h\ser\SerInputters.h
# End Source File
# Begin Source File

SOURCE=..\h\ser\SerMemInputSource.h
# End Source File
# Begin Source File

SOURCE=..\h\ser\SerMemOutputSource.h
# End Source File
# Begin Source File

SOURCE=..\h\ser\SerOutputters.h
# End Source File
# Begin Source File

SOURCE=..\h\ser\SerSerializationMgr.h
# End Source File
# Begin Source File

SOURCE=..\h\ser\SerStdLib.h
# End Source File
# Begin Source File

SOURCE=..\h\ser\SerUtils.h
# End Source File
# End Group
# Begin Source File

SOURCE=..\h\cslib.h
# End Source File
# End Group
# Begin Group "src"

# PROP Default_Filter ""
# Begin Group "core "

# PROP Default_Filter "*.cpp"
# Begin Source File

SOURCE=..\src\core\Debug.cpp
# End Source File
# Begin Source File

SOURCE=..\src\core\HandleObject.cpp
# End Source File
# End Group
# Begin Group "file"

# PROP Default_Filter "*.cpp"
# Begin Source File

SOURCE=..\src\file\FileAttributes.cpp
# End Source File
# Begin Source File

SOURCE=..\src\file\FileIterator.cpp
# End Source File
# Begin Source File

SOURCE=..\src\file\FileIteratorImp.cpp
# End Source File
# Begin Source File

SOURCE=..\src\file\FileIterImpWin32.cpp
# End Source File
# Begin Source File

SOURCE=..\src\file\FileUtils.cpp
# End Source File
# End Group
# Begin Group "lang"

# PROP Default_Filter "*.cpp"
# Begin Source File

SOURCE=..\src\lang\Date.cpp
# End Source File
# Begin Source File

SOURCE=..\src\lang\ScheduleTime.cpp
# End Source File
# Begin Source File

SOURCE=..\src\lang\String.cpp
# End Source File
# End Group
# Begin Group "log"

# PROP Default_Filter "*.cpp"
# Begin Group "nt"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\src\log\nt\EventLog.cpp
# End Source File
# Begin Source File

SOURCE=..\src\log\nt\LogNTEventLogImpl.cpp
# End Source File
# End Group
# Begin Source File

SOURCE=..\src\log\LogImplementationFactory.cpp
# End Source File
# Begin Source File

SOURCE=..\src\log\LogStdFileImpl.cpp
# End Source File
# Begin Source File

SOURCE=..\src\log\LogStdFormatter.cpp
# End Source File
# Begin Source File

SOURCE=..\src\log\LogStdLogger.cpp
# End Source File
# End Group
# Begin Group "pat"

# PROP Default_Filter "*.pat"
# End Group
# Begin Group "rpc"

# PROP Default_Filter "*.cpp"
# Begin Group "http "

# PROP Default_Filter "*.cpp"
# Begin Source File

SOURCE=..\src\rpc\http\RpcHttpAuthenticator.cpp
# End Source File
# Begin Source File

SOURCE=..\src\rpc\http\RpcHttpTransport.cpp
# End Source File
# End Group
# Begin Source File

SOURCE=..\src\rpc\RpcAuthenticatorFactory.cpp
# End Source File
# Begin Source File

SOURCE=..\src\rpc\RpcDispatcher.cpp
# End Source File
# Begin Source File

SOURCE=..\src\rpc\RpcInterface.cpp
# End Source File
# Begin Source File

SOURCE=..\src\rpc\RpcMessage.cpp
# End Source File
# Begin Source File

SOURCE=..\src\rpc\RpcServer.cpp
# End Source File
# Begin Source File

SOURCE=..\src\rpc\RpcTransportFactory.cpp
# End Source File
# End Group
# Begin Group "sys"

# PROP Default_Filter "*.cpp"
# Begin Source File

SOURCE=..\src\sys\SysComInitializer.cpp
# End Source File
# Begin Source File

SOURCE=..\src\sys\SysComponentFactory.cpp
# End Source File
# Begin Source File

SOURCE=..\src\sys\SysComponentLoader.cpp
# End Source File
# Begin Source File

SOURCE=..\src\sys\SysContext.cpp
# End Source File
# Begin Source File

SOURCE=..\src\sys\SysCounterMgr.cpp
# End Source File
# Begin Source File

SOURCE=..\src\sys\SysManager.cpp
# End Source File
# Begin Source File

SOURCE=..\src\sys\SysPathMgr.cpp
# End Source File
# Begin Source File

SOURCE=..\src\sys\SysSecurity.cpp
# End Source File
# Begin Source File

SOURCE=..\src\sys\SysSecurityAttributes.cpp
# End Source File
# Begin Source File

SOURCE=..\src\sys\SysSecurityDescriptor.cpp
# End Source File
# End Group
# Begin Group "thd"

# PROP Default_Filter "*.cpp"
# Begin Source File

SOURCE=..\src\thd\ThdCriticalSection.cpp
# End Source File
# Begin Source File

SOURCE=..\src\thd\ThdEvent.cpp
# End Source File
# Begin Source File

SOURCE=..\src\thd\ThdManager.cpp
# End Source File
# Begin Source File

SOURCE=..\src\thd\ThdMutex.cpp
# End Source File
# Begin Source File

SOURCE=..\src\thd\ThdPool.cpp
# End Source File
# Begin Source File

SOURCE=..\src\thd\ThdQueue.cpp
# End Source File
# Begin Source File

SOURCE=..\src\thd\ThdRWLock.cpp
# End Source File
# Begin Source File

SOURCE=..\src\thd\ThdScheduler.cpp
# End Source File
# Begin Source File

SOURCE=..\src\thd\ThdSemaphore.cpp
# End Source File
# Begin Source File

SOURCE=..\src\thd\ThdSyncObject.cpp
# End Source File
# Begin Source File

SOURCE=..\src\thd\ThdThread.cpp
# End Source File
# Begin Source File

SOURCE=..\src\thd\ThdWaitableTimer.cpp
# End Source File
# End Group
# Begin Group "util"

# PROP Default_Filter "*.cpp"
# Begin Source File

SOURCE=..\src\util\UtilDllLoader.cpp
# End Source File
# Begin Source File

SOURCE=..\src\util\UtilMapping.cpp
# End Source File
# Begin Source File

SOURCE=..\src\util\UtilPattern.cpp
# End Source File
# Begin Source File

SOURCE=..\src\util\UtilStreaming.cpp
# End Source File
# Begin Source File

SOURCE=..\src\util\UtilStringUtils.cpp
# End Source File
# End Group
# Begin Group "xml"

# PROP Default_Filter "*.xml"
# Begin Source File

SOURCE=..\src\xml\XmlDomUtils.cpp
# End Source File
# End Group
# Begin Group "internal"

# PROP Default_Filter "*.h"
# Begin Source File

SOURCE=..\src\internal\XmlDomUtilsInternal.h
# End Source File
# End Group
# Begin Group "net"

# PROP Default_Filter "cpp"
# Begin Source File

SOURCE=..\src\net\Net.cpp
# End Source File
# Begin Source File

SOURCE=..\src\net\NetHttpClientPost.cpp
# End Source File
# Begin Source File

SOURCE=..\src\net\NetHttpContent.cpp
# End Source File
# Begin Source File

SOURCE=..\src\net\NetHttpSerPost.cpp
# End Source File
# Begin Source File

SOURCE=..\src\net\NetHttpUtils.cpp
# End Source File
# Begin Source File

SOURCE=..\src\net\NetSocket.cpp
# End Source File
# End Group
# Begin Group "fwk "

# PROP Default_Filter "*.cpp"
# Begin Source File

SOURCE=..\src\fwk\fwkBootstrap.cpp
# End Source File
# End Group
# Begin Group "svr "

# PROP Default_Filter "*.cpp"
# Begin Group "nt  "

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\src\svr\nt\CService.cpp
# End Source File
# Begin Source File

SOURCE=..\src\svr\nt\CServiceInstall.cpp
# End Source File
# Begin Source File

SOURCE=..\src\svr\nt\NTServiceMain.cpp
# End Source File
# End Group
# End Group
# Begin Group "ser "

# PROP Default_Filter "*.cpp"
# Begin Source File

SOURCE=..\src\ser\SerFileInputSource.cpp
# End Source File
# Begin Source File

SOURCE=..\src\ser\SerFileOutputSource.cpp
# End Source File
# Begin Source File

SOURCE=..\src\ser\SerMemInputSource.cpp
# End Source File
# Begin Source File

SOURCE=..\src\ser\SerMemOutputSource.cpp
# End Source File
# Begin Source File

SOURCE=..\src\ser\SerSerializationMgr.cpp
# End Source File
# End Group
# Begin Source File

SOURCE=..\src\cslib.cpp
# ADD CPP /Yc"cslib.h"
# End Source File
# End Group
# End Target
# End Project
