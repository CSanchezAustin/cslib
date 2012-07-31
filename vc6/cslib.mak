# Microsoft Developer Studio Generated NMAKE File, Based on cslib.dsp
!IF "$(CFG)" == ""
CFG=cslib - Win32 Debug
!MESSAGE No configuration specified. Defaulting to cslib - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "cslib - Win32 Release" && "$(CFG)" != "cslib - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
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
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "cslib - Win32 Release"

OUTDIR=.\..\lib\vc6\win32rel
INTDIR=.\..\obj\vc6\win32rel
# Begin Custom Macros
OutDir=.\..\lib\vc6\win32rel
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\cslib.lib"

!ELSE 

ALL : "CslibMsg - Win32 Release" "$(OUTDIR)\cslib.lib"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"CslibMsg - Win32 ReleaseCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\CService.obj"
	-@erase "$(INTDIR)\CServiceInstall.obj"
	-@erase "$(INTDIR)\cslib.obj"
	-@erase "$(INTDIR)\cslib.pch"
	-@erase "$(INTDIR)\Date.obj"
	-@erase "$(INTDIR)\Debug.obj"
	-@erase "$(INTDIR)\EventLog.obj"
	-@erase "$(INTDIR)\FileAttributes.obj"
	-@erase "$(INTDIR)\FileIterator.obj"
	-@erase "$(INTDIR)\FileIteratorImp.obj"
	-@erase "$(INTDIR)\FileIterImpWin32.obj"
	-@erase "$(INTDIR)\FileUtils.obj"
	-@erase "$(INTDIR)\fwkBootstrap.obj"
	-@erase "$(INTDIR)\HandleObject.obj"
	-@erase "$(INTDIR)\LogImplementationFactory.obj"
	-@erase "$(INTDIR)\LogNTEventLogImpl.obj"
	-@erase "$(INTDIR)\LogStdFileImpl.obj"
	-@erase "$(INTDIR)\LogStdFormatter.obj"
	-@erase "$(INTDIR)\LogStdLogger.obj"
	-@erase "$(INTDIR)\Net.obj"
	-@erase "$(INTDIR)\NetHttpClientPost.obj"
	-@erase "$(INTDIR)\NetHttpContent.obj"
	-@erase "$(INTDIR)\NetHttpSerPost.obj"
	-@erase "$(INTDIR)\NetHttpUtils.obj"
	-@erase "$(INTDIR)\NetSocket.obj"
	-@erase "$(INTDIR)\NTServiceMain.obj"
	-@erase "$(INTDIR)\RpcAuthenticatorFactory.obj"
	-@erase "$(INTDIR)\RpcDispatcher.obj"
	-@erase "$(INTDIR)\RpcHttpAuthenticator.obj"
	-@erase "$(INTDIR)\RpcHttpTransport.obj"
	-@erase "$(INTDIR)\RpcInterface.obj"
	-@erase "$(INTDIR)\RpcMessage.obj"
	-@erase "$(INTDIR)\RpcServer.obj"
	-@erase "$(INTDIR)\RpcTransportFactory.obj"
	-@erase "$(INTDIR)\ScheduleTime.obj"
	-@erase "$(INTDIR)\SerFileInputSource.obj"
	-@erase "$(INTDIR)\SerFileOutputSource.obj"
	-@erase "$(INTDIR)\SerMemInputSource.obj"
	-@erase "$(INTDIR)\SerMemOutputSource.obj"
	-@erase "$(INTDIR)\SerSerializationMgr.obj"
	-@erase "$(INTDIR)\String.obj"
	-@erase "$(INTDIR)\SysComInitializer.obj"
	-@erase "$(INTDIR)\SysComponentFactory.obj"
	-@erase "$(INTDIR)\SysComponentLoader.obj"
	-@erase "$(INTDIR)\SysContext.obj"
	-@erase "$(INTDIR)\SysCounterMgr.obj"
	-@erase "$(INTDIR)\SysManager.obj"
	-@erase "$(INTDIR)\SysPathMgr.obj"
	-@erase "$(INTDIR)\SysSecurity.obj"
	-@erase "$(INTDIR)\SysSecurityAttributes.obj"
	-@erase "$(INTDIR)\SysSecurityDescriptor.obj"
	-@erase "$(INTDIR)\ThdCriticalSection.obj"
	-@erase "$(INTDIR)\ThdEvent.obj"
	-@erase "$(INTDIR)\ThdManager.obj"
	-@erase "$(INTDIR)\ThdMutex.obj"
	-@erase "$(INTDIR)\ThdPool.obj"
	-@erase "$(INTDIR)\ThdQueue.obj"
	-@erase "$(INTDIR)\ThdRWLock.obj"
	-@erase "$(INTDIR)\ThdScheduler.obj"
	-@erase "$(INTDIR)\ThdSemaphore.obj"
	-@erase "$(INTDIR)\ThdSyncObject.obj"
	-@erase "$(INTDIR)\ThdThread.obj"
	-@erase "$(INTDIR)\ThdWaitableTimer.obj"
	-@erase "$(INTDIR)\UtilDllLoader.obj"
	-@erase "$(INTDIR)\UtilMapping.obj"
	-@erase "$(INTDIR)\UtilPattern.obj"
	-@erase "$(INTDIR)\UtilStreaming.obj"
	-@erase "$(INTDIR)\UtilStringUtils.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\XmlDomUtils.obj"
	-@erase "$(OUTDIR)\cslib.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MD /W3 /GX /O2 /I "$(XERCESCROOT)\include" /I "$(XERCESCROOT)\include\xercesc" /I "..\h" /D "NDEBUG" /D "NOMINMAX" /D "WIN32" /D "_WINDOWS" /D _WIN32_WINNT=0x0400 /Fp"$(INTDIR)\cslib.pch" /Yu"cslib.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

.c{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

RSC=rc.exe
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\cslib.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"$(OUTDIR)\cslib.lib" 
LIB32_OBJS= \
	"$(INTDIR)\Debug.obj" \
	"$(INTDIR)\HandleObject.obj" \
	"$(INTDIR)\FileAttributes.obj" \
	"$(INTDIR)\FileIterator.obj" \
	"$(INTDIR)\FileIteratorImp.obj" \
	"$(INTDIR)\FileIterImpWin32.obj" \
	"$(INTDIR)\FileUtils.obj" \
	"$(INTDIR)\Date.obj" \
	"$(INTDIR)\ScheduleTime.obj" \
	"$(INTDIR)\String.obj" \
	"$(INTDIR)\EventLog.obj" \
	"$(INTDIR)\LogNTEventLogImpl.obj" \
	"$(INTDIR)\LogImplementationFactory.obj" \
	"$(INTDIR)\LogStdFileImpl.obj" \
	"$(INTDIR)\LogStdFormatter.obj" \
	"$(INTDIR)\LogStdLogger.obj" \
	"$(INTDIR)\RpcHttpAuthenticator.obj" \
	"$(INTDIR)\RpcHttpTransport.obj" \
	"$(INTDIR)\RpcAuthenticatorFactory.obj" \
	"$(INTDIR)\RpcDispatcher.obj" \
	"$(INTDIR)\RpcInterface.obj" \
	"$(INTDIR)\RpcMessage.obj" \
	"$(INTDIR)\RpcServer.obj" \
	"$(INTDIR)\RpcTransportFactory.obj" \
	"$(INTDIR)\SysComInitializer.obj" \
	"$(INTDIR)\SysComponentFactory.obj" \
	"$(INTDIR)\SysComponentLoader.obj" \
	"$(INTDIR)\SysContext.obj" \
	"$(INTDIR)\SysCounterMgr.obj" \
	"$(INTDIR)\SysManager.obj" \
	"$(INTDIR)\SysPathMgr.obj" \
	"$(INTDIR)\SysSecurity.obj" \
	"$(INTDIR)\SysSecurityAttributes.obj" \
	"$(INTDIR)\SysSecurityDescriptor.obj" \
	"$(INTDIR)\ThdCriticalSection.obj" \
	"$(INTDIR)\ThdEvent.obj" \
	"$(INTDIR)\ThdManager.obj" \
	"$(INTDIR)\ThdMutex.obj" \
	"$(INTDIR)\ThdPool.obj" \
	"$(INTDIR)\ThdQueue.obj" \
	"$(INTDIR)\ThdRWLock.obj" \
	"$(INTDIR)\ThdScheduler.obj" \
	"$(INTDIR)\ThdSemaphore.obj" \
	"$(INTDIR)\ThdSyncObject.obj" \
	"$(INTDIR)\ThdThread.obj" \
	"$(INTDIR)\ThdWaitableTimer.obj" \
	"$(INTDIR)\UtilDllLoader.obj" \
	"$(INTDIR)\UtilMapping.obj" \
	"$(INTDIR)\UtilPattern.obj" \
	"$(INTDIR)\UtilStreaming.obj" \
	"$(INTDIR)\UtilStringUtils.obj" \
	"$(INTDIR)\XmlDomUtils.obj" \
	"$(INTDIR)\Net.obj" \
	"$(INTDIR)\NetHttpClientPost.obj" \
	"$(INTDIR)\NetHttpContent.obj" \
	"$(INTDIR)\NetHttpSerPost.obj" \
	"$(INTDIR)\NetHttpUtils.obj" \
	"$(INTDIR)\NetSocket.obj" \
	"$(INTDIR)\fwkBootstrap.obj" \
	"$(INTDIR)\CService.obj" \
	"$(INTDIR)\CServiceInstall.obj" \
	"$(INTDIR)\NTServiceMain.obj" \
	"$(INTDIR)\SerFileInputSource.obj" \
	"$(INTDIR)\SerFileOutputSource.obj" \
	"$(INTDIR)\SerMemInputSource.obj" \
	"$(INTDIR)\SerMemOutputSource.obj" \
	"$(INTDIR)\SerSerializationMgr.obj" \
	"$(INTDIR)\cslib.obj" \
	"..\bin\vc6\win32rel\CslibMsg.lib"

"$(OUTDIR)\cslib.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ELSEIF  "$(CFG)" == "cslib - Win32 Debug"

OUTDIR=.\..\lib\vc6\win32dbg
INTDIR=.\..\obj\vc6\win32dbg
# Begin Custom Macros
OutDir=.\..\lib\vc6\win32dbg
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\cslib.lib" "$(OUTDIR)\cslib.bsc"

!ELSE 

ALL : "CslibMsg - Win32 Debug" "$(OUTDIR)\cslib.lib" "$(OUTDIR)\cslib.bsc"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"CslibMsg - Win32 DebugCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\CService.obj"
	-@erase "$(INTDIR)\CService.sbr"
	-@erase "$(INTDIR)\CServiceInstall.obj"
	-@erase "$(INTDIR)\CServiceInstall.sbr"
	-@erase "$(INTDIR)\cslib.obj"
	-@erase "$(INTDIR)\cslib.pch"
	-@erase "$(INTDIR)\cslib.sbr"
	-@erase "$(INTDIR)\Date.obj"
	-@erase "$(INTDIR)\Date.sbr"
	-@erase "$(INTDIR)\Debug.obj"
	-@erase "$(INTDIR)\Debug.sbr"
	-@erase "$(INTDIR)\EventLog.obj"
	-@erase "$(INTDIR)\EventLog.sbr"
	-@erase "$(INTDIR)\FileAttributes.obj"
	-@erase "$(INTDIR)\FileAttributes.sbr"
	-@erase "$(INTDIR)\FileIterator.obj"
	-@erase "$(INTDIR)\FileIterator.sbr"
	-@erase "$(INTDIR)\FileIteratorImp.obj"
	-@erase "$(INTDIR)\FileIteratorImp.sbr"
	-@erase "$(INTDIR)\FileIterImpWin32.obj"
	-@erase "$(INTDIR)\FileIterImpWin32.sbr"
	-@erase "$(INTDIR)\FileUtils.obj"
	-@erase "$(INTDIR)\FileUtils.sbr"
	-@erase "$(INTDIR)\fwkBootstrap.obj"
	-@erase "$(INTDIR)\fwkBootstrap.sbr"
	-@erase "$(INTDIR)\HandleObject.obj"
	-@erase "$(INTDIR)\HandleObject.sbr"
	-@erase "$(INTDIR)\LogImplementationFactory.obj"
	-@erase "$(INTDIR)\LogImplementationFactory.sbr"
	-@erase "$(INTDIR)\LogNTEventLogImpl.obj"
	-@erase "$(INTDIR)\LogNTEventLogImpl.sbr"
	-@erase "$(INTDIR)\LogStdFileImpl.obj"
	-@erase "$(INTDIR)\LogStdFileImpl.sbr"
	-@erase "$(INTDIR)\LogStdFormatter.obj"
	-@erase "$(INTDIR)\LogStdFormatter.sbr"
	-@erase "$(INTDIR)\LogStdLogger.obj"
	-@erase "$(INTDIR)\LogStdLogger.sbr"
	-@erase "$(INTDIR)\Net.obj"
	-@erase "$(INTDIR)\Net.sbr"
	-@erase "$(INTDIR)\NetHttpClientPost.obj"
	-@erase "$(INTDIR)\NetHttpClientPost.sbr"
	-@erase "$(INTDIR)\NetHttpContent.obj"
	-@erase "$(INTDIR)\NetHttpContent.sbr"
	-@erase "$(INTDIR)\NetHttpSerPost.obj"
	-@erase "$(INTDIR)\NetHttpSerPost.sbr"
	-@erase "$(INTDIR)\NetHttpUtils.obj"
	-@erase "$(INTDIR)\NetHttpUtils.sbr"
	-@erase "$(INTDIR)\NetSocket.obj"
	-@erase "$(INTDIR)\NetSocket.sbr"
	-@erase "$(INTDIR)\NTServiceMain.obj"
	-@erase "$(INTDIR)\NTServiceMain.sbr"
	-@erase "$(INTDIR)\RpcAuthenticatorFactory.obj"
	-@erase "$(INTDIR)\RpcAuthenticatorFactory.sbr"
	-@erase "$(INTDIR)\RpcDispatcher.obj"
	-@erase "$(INTDIR)\RpcDispatcher.sbr"
	-@erase "$(INTDIR)\RpcHttpAuthenticator.obj"
	-@erase "$(INTDIR)\RpcHttpAuthenticator.sbr"
	-@erase "$(INTDIR)\RpcHttpTransport.obj"
	-@erase "$(INTDIR)\RpcHttpTransport.sbr"
	-@erase "$(INTDIR)\RpcInterface.obj"
	-@erase "$(INTDIR)\RpcInterface.sbr"
	-@erase "$(INTDIR)\RpcMessage.obj"
	-@erase "$(INTDIR)\RpcMessage.sbr"
	-@erase "$(INTDIR)\RpcServer.obj"
	-@erase "$(INTDIR)\RpcServer.sbr"
	-@erase "$(INTDIR)\RpcTransportFactory.obj"
	-@erase "$(INTDIR)\RpcTransportFactory.sbr"
	-@erase "$(INTDIR)\ScheduleTime.obj"
	-@erase "$(INTDIR)\ScheduleTime.sbr"
	-@erase "$(INTDIR)\SerFileInputSource.obj"
	-@erase "$(INTDIR)\SerFileInputSource.sbr"
	-@erase "$(INTDIR)\SerFileOutputSource.obj"
	-@erase "$(INTDIR)\SerFileOutputSource.sbr"
	-@erase "$(INTDIR)\SerMemInputSource.obj"
	-@erase "$(INTDIR)\SerMemInputSource.sbr"
	-@erase "$(INTDIR)\SerMemOutputSource.obj"
	-@erase "$(INTDIR)\SerMemOutputSource.sbr"
	-@erase "$(INTDIR)\SerSerializationMgr.obj"
	-@erase "$(INTDIR)\SerSerializationMgr.sbr"
	-@erase "$(INTDIR)\String.obj"
	-@erase "$(INTDIR)\String.sbr"
	-@erase "$(INTDIR)\SysComInitializer.obj"
	-@erase "$(INTDIR)\SysComInitializer.sbr"
	-@erase "$(INTDIR)\SysComponentFactory.obj"
	-@erase "$(INTDIR)\SysComponentFactory.sbr"
	-@erase "$(INTDIR)\SysComponentLoader.obj"
	-@erase "$(INTDIR)\SysComponentLoader.sbr"
	-@erase "$(INTDIR)\SysContext.obj"
	-@erase "$(INTDIR)\SysContext.sbr"
	-@erase "$(INTDIR)\SysCounterMgr.obj"
	-@erase "$(INTDIR)\SysCounterMgr.sbr"
	-@erase "$(INTDIR)\SysManager.obj"
	-@erase "$(INTDIR)\SysManager.sbr"
	-@erase "$(INTDIR)\SysPathMgr.obj"
	-@erase "$(INTDIR)\SysPathMgr.sbr"
	-@erase "$(INTDIR)\SysSecurity.obj"
	-@erase "$(INTDIR)\SysSecurity.sbr"
	-@erase "$(INTDIR)\SysSecurityAttributes.obj"
	-@erase "$(INTDIR)\SysSecurityAttributes.sbr"
	-@erase "$(INTDIR)\SysSecurityDescriptor.obj"
	-@erase "$(INTDIR)\SysSecurityDescriptor.sbr"
	-@erase "$(INTDIR)\ThdCriticalSection.obj"
	-@erase "$(INTDIR)\ThdCriticalSection.sbr"
	-@erase "$(INTDIR)\ThdEvent.obj"
	-@erase "$(INTDIR)\ThdEvent.sbr"
	-@erase "$(INTDIR)\ThdManager.obj"
	-@erase "$(INTDIR)\ThdManager.sbr"
	-@erase "$(INTDIR)\ThdMutex.obj"
	-@erase "$(INTDIR)\ThdMutex.sbr"
	-@erase "$(INTDIR)\ThdPool.obj"
	-@erase "$(INTDIR)\ThdPool.sbr"
	-@erase "$(INTDIR)\ThdQueue.obj"
	-@erase "$(INTDIR)\ThdQueue.sbr"
	-@erase "$(INTDIR)\ThdRWLock.obj"
	-@erase "$(INTDIR)\ThdRWLock.sbr"
	-@erase "$(INTDIR)\ThdScheduler.obj"
	-@erase "$(INTDIR)\ThdScheduler.sbr"
	-@erase "$(INTDIR)\ThdSemaphore.obj"
	-@erase "$(INTDIR)\ThdSemaphore.sbr"
	-@erase "$(INTDIR)\ThdSyncObject.obj"
	-@erase "$(INTDIR)\ThdSyncObject.sbr"
	-@erase "$(INTDIR)\ThdThread.obj"
	-@erase "$(INTDIR)\ThdThread.sbr"
	-@erase "$(INTDIR)\ThdWaitableTimer.obj"
	-@erase "$(INTDIR)\ThdWaitableTimer.sbr"
	-@erase "$(INTDIR)\UtilDllLoader.obj"
	-@erase "$(INTDIR)\UtilDllLoader.sbr"
	-@erase "$(INTDIR)\UtilMapping.obj"
	-@erase "$(INTDIR)\UtilMapping.sbr"
	-@erase "$(INTDIR)\UtilPattern.obj"
	-@erase "$(INTDIR)\UtilPattern.sbr"
	-@erase "$(INTDIR)\UtilStreaming.obj"
	-@erase "$(INTDIR)\UtilStreaming.sbr"
	-@erase "$(INTDIR)\UtilStringUtils.obj"
	-@erase "$(INTDIR)\UtilStringUtils.sbr"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(INTDIR)\XmlDomUtils.obj"
	-@erase "$(INTDIR)\XmlDomUtils.sbr"
	-@erase "$(OUTDIR)\cslib.bsc"
	-@erase "$(OUTDIR)\cslib.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MDd /W3 /Gm /GX /ZI /Od /I "$(XERCESCROOT)\include" /I "$(XERCESCROOT)\include\xercesc" /I "..\h" /D "_DEBUG" /D "NOMINMAX" /D "WIN32" /D "_WINDOWS" /D _WIN32_WINNT=0x0400 /FR"$(INTDIR)\\" /Fp"$(INTDIR)\cslib.pch" /Yu"cslib.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

.c{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

RSC=rc.exe
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\cslib.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\Debug.sbr" \
	"$(INTDIR)\HandleObject.sbr" \
	"$(INTDIR)\FileAttributes.sbr" \
	"$(INTDIR)\FileIterator.sbr" \
	"$(INTDIR)\FileIteratorImp.sbr" \
	"$(INTDIR)\FileIterImpWin32.sbr" \
	"$(INTDIR)\FileUtils.sbr" \
	"$(INTDIR)\Date.sbr" \
	"$(INTDIR)\ScheduleTime.sbr" \
	"$(INTDIR)\String.sbr" \
	"$(INTDIR)\EventLog.sbr" \
	"$(INTDIR)\LogNTEventLogImpl.sbr" \
	"$(INTDIR)\LogImplementationFactory.sbr" \
	"$(INTDIR)\LogStdFileImpl.sbr" \
	"$(INTDIR)\LogStdFormatter.sbr" \
	"$(INTDIR)\LogStdLogger.sbr" \
	"$(INTDIR)\RpcHttpAuthenticator.sbr" \
	"$(INTDIR)\RpcHttpTransport.sbr" \
	"$(INTDIR)\RpcAuthenticatorFactory.sbr" \
	"$(INTDIR)\RpcDispatcher.sbr" \
	"$(INTDIR)\RpcInterface.sbr" \
	"$(INTDIR)\RpcMessage.sbr" \
	"$(INTDIR)\RpcServer.sbr" \
	"$(INTDIR)\RpcTransportFactory.sbr" \
	"$(INTDIR)\SysComInitializer.sbr" \
	"$(INTDIR)\SysComponentFactory.sbr" \
	"$(INTDIR)\SysComponentLoader.sbr" \
	"$(INTDIR)\SysContext.sbr" \
	"$(INTDIR)\SysCounterMgr.sbr" \
	"$(INTDIR)\SysManager.sbr" \
	"$(INTDIR)\SysPathMgr.sbr" \
	"$(INTDIR)\SysSecurity.sbr" \
	"$(INTDIR)\SysSecurityAttributes.sbr" \
	"$(INTDIR)\SysSecurityDescriptor.sbr" \
	"$(INTDIR)\ThdCriticalSection.sbr" \
	"$(INTDIR)\ThdEvent.sbr" \
	"$(INTDIR)\ThdManager.sbr" \
	"$(INTDIR)\ThdMutex.sbr" \
	"$(INTDIR)\ThdPool.sbr" \
	"$(INTDIR)\ThdQueue.sbr" \
	"$(INTDIR)\ThdRWLock.sbr" \
	"$(INTDIR)\ThdScheduler.sbr" \
	"$(INTDIR)\ThdSemaphore.sbr" \
	"$(INTDIR)\ThdSyncObject.sbr" \
	"$(INTDIR)\ThdThread.sbr" \
	"$(INTDIR)\ThdWaitableTimer.sbr" \
	"$(INTDIR)\UtilDllLoader.sbr" \
	"$(INTDIR)\UtilMapping.sbr" \
	"$(INTDIR)\UtilPattern.sbr" \
	"$(INTDIR)\UtilStreaming.sbr" \
	"$(INTDIR)\UtilStringUtils.sbr" \
	"$(INTDIR)\XmlDomUtils.sbr" \
	"$(INTDIR)\Net.sbr" \
	"$(INTDIR)\NetHttpClientPost.sbr" \
	"$(INTDIR)\NetHttpContent.sbr" \
	"$(INTDIR)\NetHttpSerPost.sbr" \
	"$(INTDIR)\NetHttpUtils.sbr" \
	"$(INTDIR)\NetSocket.sbr" \
	"$(INTDIR)\fwkBootstrap.sbr" \
	"$(INTDIR)\CService.sbr" \
	"$(INTDIR)\CServiceInstall.sbr" \
	"$(INTDIR)\NTServiceMain.sbr" \
	"$(INTDIR)\SerFileInputSource.sbr" \
	"$(INTDIR)\SerFileOutputSource.sbr" \
	"$(INTDIR)\SerMemInputSource.sbr" \
	"$(INTDIR)\SerMemOutputSource.sbr" \
	"$(INTDIR)\SerSerializationMgr.sbr" \
	"$(INTDIR)\cslib.sbr"

"$(OUTDIR)\cslib.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"$(OUTDIR)\cslib.lib" 
LIB32_OBJS= \
	"$(INTDIR)\Debug.obj" \
	"$(INTDIR)\HandleObject.obj" \
	"$(INTDIR)\FileAttributes.obj" \
	"$(INTDIR)\FileIterator.obj" \
	"$(INTDIR)\FileIteratorImp.obj" \
	"$(INTDIR)\FileIterImpWin32.obj" \
	"$(INTDIR)\FileUtils.obj" \
	"$(INTDIR)\Date.obj" \
	"$(INTDIR)\ScheduleTime.obj" \
	"$(INTDIR)\String.obj" \
	"$(INTDIR)\EventLog.obj" \
	"$(INTDIR)\LogNTEventLogImpl.obj" \
	"$(INTDIR)\LogImplementationFactory.obj" \
	"$(INTDIR)\LogStdFileImpl.obj" \
	"$(INTDIR)\LogStdFormatter.obj" \
	"$(INTDIR)\LogStdLogger.obj" \
	"$(INTDIR)\RpcHttpAuthenticator.obj" \
	"$(INTDIR)\RpcHttpTransport.obj" \
	"$(INTDIR)\RpcAuthenticatorFactory.obj" \
	"$(INTDIR)\RpcDispatcher.obj" \
	"$(INTDIR)\RpcInterface.obj" \
	"$(INTDIR)\RpcMessage.obj" \
	"$(INTDIR)\RpcServer.obj" \
	"$(INTDIR)\RpcTransportFactory.obj" \
	"$(INTDIR)\SysComInitializer.obj" \
	"$(INTDIR)\SysComponentFactory.obj" \
	"$(INTDIR)\SysComponentLoader.obj" \
	"$(INTDIR)\SysContext.obj" \
	"$(INTDIR)\SysCounterMgr.obj" \
	"$(INTDIR)\SysManager.obj" \
	"$(INTDIR)\SysPathMgr.obj" \
	"$(INTDIR)\SysSecurity.obj" \
	"$(INTDIR)\SysSecurityAttributes.obj" \
	"$(INTDIR)\SysSecurityDescriptor.obj" \
	"$(INTDIR)\ThdCriticalSection.obj" \
	"$(INTDIR)\ThdEvent.obj" \
	"$(INTDIR)\ThdManager.obj" \
	"$(INTDIR)\ThdMutex.obj" \
	"$(INTDIR)\ThdPool.obj" \
	"$(INTDIR)\ThdQueue.obj" \
	"$(INTDIR)\ThdRWLock.obj" \
	"$(INTDIR)\ThdScheduler.obj" \
	"$(INTDIR)\ThdSemaphore.obj" \
	"$(INTDIR)\ThdSyncObject.obj" \
	"$(INTDIR)\ThdThread.obj" \
	"$(INTDIR)\ThdWaitableTimer.obj" \
	"$(INTDIR)\UtilDllLoader.obj" \
	"$(INTDIR)\UtilMapping.obj" \
	"$(INTDIR)\UtilPattern.obj" \
	"$(INTDIR)\UtilStreaming.obj" \
	"$(INTDIR)\UtilStringUtils.obj" \
	"$(INTDIR)\XmlDomUtils.obj" \
	"$(INTDIR)\Net.obj" \
	"$(INTDIR)\NetHttpClientPost.obj" \
	"$(INTDIR)\NetHttpContent.obj" \
	"$(INTDIR)\NetHttpSerPost.obj" \
	"$(INTDIR)\NetHttpUtils.obj" \
	"$(INTDIR)\NetSocket.obj" \
	"$(INTDIR)\fwkBootstrap.obj" \
	"$(INTDIR)\CService.obj" \
	"$(INTDIR)\CServiceInstall.obj" \
	"$(INTDIR)\NTServiceMain.obj" \
	"$(INTDIR)\SerFileInputSource.obj" \
	"$(INTDIR)\SerFileOutputSource.obj" \
	"$(INTDIR)\SerMemInputSource.obj" \
	"$(INTDIR)\SerMemOutputSource.obj" \
	"$(INTDIR)\SerSerializationMgr.obj" \
	"$(INTDIR)\cslib.obj" \
	"..\bin\vc6\win32dbg\CslibMsg.lib"

"$(OUTDIR)\cslib.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ENDIF 


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("cslib.dep")
!INCLUDE "cslib.dep"
!ELSE 
!MESSAGE Warning: cannot find "cslib.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "cslib - Win32 Release" || "$(CFG)" == "cslib - Win32 Debug"
SOURCE=..\src\core\Debug.cpp

!IF  "$(CFG)" == "cslib - Win32 Release"


"$(INTDIR)\Debug.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\cslib.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "cslib - Win32 Debug"


"$(INTDIR)\Debug.obj"	"$(INTDIR)\Debug.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\cslib.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\src\core\HandleObject.cpp

!IF  "$(CFG)" == "cslib - Win32 Release"


"$(INTDIR)\HandleObject.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\cslib.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "cslib - Win32 Debug"


"$(INTDIR)\HandleObject.obj"	"$(INTDIR)\HandleObject.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\cslib.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\src\file\FileAttributes.cpp

!IF  "$(CFG)" == "cslib - Win32 Release"


"$(INTDIR)\FileAttributes.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\cslib.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "cslib - Win32 Debug"


"$(INTDIR)\FileAttributes.obj"	"$(INTDIR)\FileAttributes.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\cslib.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\src\file\FileIterator.cpp

!IF  "$(CFG)" == "cslib - Win32 Release"


"$(INTDIR)\FileIterator.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\cslib.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "cslib - Win32 Debug"


"$(INTDIR)\FileIterator.obj"	"$(INTDIR)\FileIterator.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\cslib.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\src\file\FileIteratorImp.cpp

!IF  "$(CFG)" == "cslib - Win32 Release"


"$(INTDIR)\FileIteratorImp.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\cslib.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "cslib - Win32 Debug"


"$(INTDIR)\FileIteratorImp.obj"	"$(INTDIR)\FileIteratorImp.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\cslib.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\src\file\FileIterImpWin32.cpp

!IF  "$(CFG)" == "cslib - Win32 Release"


"$(INTDIR)\FileIterImpWin32.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\cslib.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "cslib - Win32 Debug"


"$(INTDIR)\FileIterImpWin32.obj"	"$(INTDIR)\FileIterImpWin32.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\cslib.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\src\file\FileUtils.cpp

!IF  "$(CFG)" == "cslib - Win32 Release"


"$(INTDIR)\FileUtils.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\cslib.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "cslib - Win32 Debug"


"$(INTDIR)\FileUtils.obj"	"$(INTDIR)\FileUtils.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\cslib.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\src\lang\Date.cpp

!IF  "$(CFG)" == "cslib - Win32 Release"


"$(INTDIR)\Date.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\cslib.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "cslib - Win32 Debug"


"$(INTDIR)\Date.obj"	"$(INTDIR)\Date.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\cslib.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\src\lang\ScheduleTime.cpp

!IF  "$(CFG)" == "cslib - Win32 Release"


"$(INTDIR)\ScheduleTime.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\cslib.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "cslib - Win32 Debug"


"$(INTDIR)\ScheduleTime.obj"	"$(INTDIR)\ScheduleTime.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\cslib.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\src\lang\String.cpp

!IF  "$(CFG)" == "cslib - Win32 Release"


"$(INTDIR)\String.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\cslib.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "cslib - Win32 Debug"


"$(INTDIR)\String.obj"	"$(INTDIR)\String.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\cslib.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\src\log\nt\EventLog.cpp

!IF  "$(CFG)" == "cslib - Win32 Release"


"$(INTDIR)\EventLog.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\cslib.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "cslib - Win32 Debug"


"$(INTDIR)\EventLog.obj"	"$(INTDIR)\EventLog.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\cslib.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\src\log\nt\LogNTEventLogImpl.cpp

!IF  "$(CFG)" == "cslib - Win32 Release"


"$(INTDIR)\LogNTEventLogImpl.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\cslib.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "cslib - Win32 Debug"


"$(INTDIR)\LogNTEventLogImpl.obj"	"$(INTDIR)\LogNTEventLogImpl.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\cslib.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\src\log\LogImplementationFactory.cpp

!IF  "$(CFG)" == "cslib - Win32 Release"


"$(INTDIR)\LogImplementationFactory.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\cslib.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "cslib - Win32 Debug"


"$(INTDIR)\LogImplementationFactory.obj"	"$(INTDIR)\LogImplementationFactory.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\cslib.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\src\log\LogStdFileImpl.cpp

!IF  "$(CFG)" == "cslib - Win32 Release"


"$(INTDIR)\LogStdFileImpl.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\cslib.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "cslib - Win32 Debug"


"$(INTDIR)\LogStdFileImpl.obj"	"$(INTDIR)\LogStdFileImpl.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\cslib.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\src\log\LogStdFormatter.cpp

!IF  "$(CFG)" == "cslib - Win32 Release"


"$(INTDIR)\LogStdFormatter.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\cslib.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "cslib - Win32 Debug"


"$(INTDIR)\LogStdFormatter.obj"	"$(INTDIR)\LogStdFormatter.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\cslib.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\src\log\LogStdLogger.cpp

!IF  "$(CFG)" == "cslib - Win32 Release"


"$(INTDIR)\LogStdLogger.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\cslib.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "cslib - Win32 Debug"


"$(INTDIR)\LogStdLogger.obj"	"$(INTDIR)\LogStdLogger.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\cslib.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\src\rpc\http\RpcHttpAuthenticator.cpp

!IF  "$(CFG)" == "cslib - Win32 Release"


"$(INTDIR)\RpcHttpAuthenticator.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\cslib.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "cslib - Win32 Debug"


"$(INTDIR)\RpcHttpAuthenticator.obj"	"$(INTDIR)\RpcHttpAuthenticator.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\cslib.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\src\rpc\http\RpcHttpTransport.cpp

!IF  "$(CFG)" == "cslib - Win32 Release"


"$(INTDIR)\RpcHttpTransport.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\cslib.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "cslib - Win32 Debug"


"$(INTDIR)\RpcHttpTransport.obj"	"$(INTDIR)\RpcHttpTransport.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\cslib.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\src\rpc\RpcAuthenticatorFactory.cpp

!IF  "$(CFG)" == "cslib - Win32 Release"


"$(INTDIR)\RpcAuthenticatorFactory.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\cslib.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "cslib - Win32 Debug"


"$(INTDIR)\RpcAuthenticatorFactory.obj"	"$(INTDIR)\RpcAuthenticatorFactory.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\cslib.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\src\rpc\RpcDispatcher.cpp

!IF  "$(CFG)" == "cslib - Win32 Release"


"$(INTDIR)\RpcDispatcher.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\cslib.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "cslib - Win32 Debug"


"$(INTDIR)\RpcDispatcher.obj"	"$(INTDIR)\RpcDispatcher.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\cslib.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\src\rpc\RpcInterface.cpp

!IF  "$(CFG)" == "cslib - Win32 Release"


"$(INTDIR)\RpcInterface.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\cslib.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "cslib - Win32 Debug"


"$(INTDIR)\RpcInterface.obj"	"$(INTDIR)\RpcInterface.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\cslib.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\src\rpc\RpcMessage.cpp

!IF  "$(CFG)" == "cslib - Win32 Release"


"$(INTDIR)\RpcMessage.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\cslib.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "cslib - Win32 Debug"


"$(INTDIR)\RpcMessage.obj"	"$(INTDIR)\RpcMessage.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\cslib.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\src\rpc\RpcServer.cpp

!IF  "$(CFG)" == "cslib - Win32 Release"


"$(INTDIR)\RpcServer.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\cslib.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "cslib - Win32 Debug"


"$(INTDIR)\RpcServer.obj"	"$(INTDIR)\RpcServer.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\cslib.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\src\rpc\RpcTransportFactory.cpp

!IF  "$(CFG)" == "cslib - Win32 Release"


"$(INTDIR)\RpcTransportFactory.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\cslib.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "cslib - Win32 Debug"


"$(INTDIR)\RpcTransportFactory.obj"	"$(INTDIR)\RpcTransportFactory.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\cslib.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\src\sys\SysComInitializer.cpp

!IF  "$(CFG)" == "cslib - Win32 Release"


"$(INTDIR)\SysComInitializer.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\cslib.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "cslib - Win32 Debug"


"$(INTDIR)\SysComInitializer.obj"	"$(INTDIR)\SysComInitializer.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\cslib.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\src\sys\SysComponentFactory.cpp

!IF  "$(CFG)" == "cslib - Win32 Release"


"$(INTDIR)\SysComponentFactory.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\cslib.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "cslib - Win32 Debug"


"$(INTDIR)\SysComponentFactory.obj"	"$(INTDIR)\SysComponentFactory.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\cslib.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\src\sys\SysComponentLoader.cpp

!IF  "$(CFG)" == "cslib - Win32 Release"


"$(INTDIR)\SysComponentLoader.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\cslib.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "cslib - Win32 Debug"


"$(INTDIR)\SysComponentLoader.obj"	"$(INTDIR)\SysComponentLoader.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\cslib.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\src\sys\SysContext.cpp

!IF  "$(CFG)" == "cslib - Win32 Release"


"$(INTDIR)\SysContext.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\cslib.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "cslib - Win32 Debug"


"$(INTDIR)\SysContext.obj"	"$(INTDIR)\SysContext.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\cslib.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\src\sys\SysCounterMgr.cpp

!IF  "$(CFG)" == "cslib - Win32 Release"


"$(INTDIR)\SysCounterMgr.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\cslib.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "cslib - Win32 Debug"


"$(INTDIR)\SysCounterMgr.obj"	"$(INTDIR)\SysCounterMgr.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\cslib.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\src\sys\SysManager.cpp

!IF  "$(CFG)" == "cslib - Win32 Release"


"$(INTDIR)\SysManager.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\cslib.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "cslib - Win32 Debug"


"$(INTDIR)\SysManager.obj"	"$(INTDIR)\SysManager.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\cslib.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\src\sys\SysPathMgr.cpp

!IF  "$(CFG)" == "cslib - Win32 Release"


"$(INTDIR)\SysPathMgr.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\cslib.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "cslib - Win32 Debug"


"$(INTDIR)\SysPathMgr.obj"	"$(INTDIR)\SysPathMgr.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\cslib.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\src\sys\SysSecurity.cpp

!IF  "$(CFG)" == "cslib - Win32 Release"


"$(INTDIR)\SysSecurity.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\cslib.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "cslib - Win32 Debug"


"$(INTDIR)\SysSecurity.obj"	"$(INTDIR)\SysSecurity.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\cslib.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\src\sys\SysSecurityAttributes.cpp

!IF  "$(CFG)" == "cslib - Win32 Release"


"$(INTDIR)\SysSecurityAttributes.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\cslib.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "cslib - Win32 Debug"


"$(INTDIR)\SysSecurityAttributes.obj"	"$(INTDIR)\SysSecurityAttributes.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\cslib.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\src\sys\SysSecurityDescriptor.cpp

!IF  "$(CFG)" == "cslib - Win32 Release"


"$(INTDIR)\SysSecurityDescriptor.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\cslib.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "cslib - Win32 Debug"


"$(INTDIR)\SysSecurityDescriptor.obj"	"$(INTDIR)\SysSecurityDescriptor.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\cslib.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\src\thd\ThdCriticalSection.cpp

!IF  "$(CFG)" == "cslib - Win32 Release"


"$(INTDIR)\ThdCriticalSection.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\cslib.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "cslib - Win32 Debug"


"$(INTDIR)\ThdCriticalSection.obj"	"$(INTDIR)\ThdCriticalSection.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\cslib.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\src\thd\ThdEvent.cpp

!IF  "$(CFG)" == "cslib - Win32 Release"


"$(INTDIR)\ThdEvent.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\cslib.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "cslib - Win32 Debug"


"$(INTDIR)\ThdEvent.obj"	"$(INTDIR)\ThdEvent.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\cslib.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\src\thd\ThdManager.cpp

!IF  "$(CFG)" == "cslib - Win32 Release"


"$(INTDIR)\ThdManager.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\cslib.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "cslib - Win32 Debug"


"$(INTDIR)\ThdManager.obj"	"$(INTDIR)\ThdManager.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\cslib.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\src\thd\ThdMutex.cpp

!IF  "$(CFG)" == "cslib - Win32 Release"


"$(INTDIR)\ThdMutex.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\cslib.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "cslib - Win32 Debug"


"$(INTDIR)\ThdMutex.obj"	"$(INTDIR)\ThdMutex.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\cslib.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\src\thd\ThdPool.cpp

!IF  "$(CFG)" == "cslib - Win32 Release"


"$(INTDIR)\ThdPool.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\cslib.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "cslib - Win32 Debug"


"$(INTDIR)\ThdPool.obj"	"$(INTDIR)\ThdPool.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\cslib.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\src\thd\ThdQueue.cpp

!IF  "$(CFG)" == "cslib - Win32 Release"


"$(INTDIR)\ThdQueue.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\cslib.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "cslib - Win32 Debug"


"$(INTDIR)\ThdQueue.obj"	"$(INTDIR)\ThdQueue.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\cslib.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\src\thd\ThdRWLock.cpp

!IF  "$(CFG)" == "cslib - Win32 Release"


"$(INTDIR)\ThdRWLock.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\cslib.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "cslib - Win32 Debug"


"$(INTDIR)\ThdRWLock.obj"	"$(INTDIR)\ThdRWLock.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\cslib.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\src\thd\ThdScheduler.cpp

!IF  "$(CFG)" == "cslib - Win32 Release"


"$(INTDIR)\ThdScheduler.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\cslib.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "cslib - Win32 Debug"


"$(INTDIR)\ThdScheduler.obj"	"$(INTDIR)\ThdScheduler.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\cslib.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\src\thd\ThdSemaphore.cpp

!IF  "$(CFG)" == "cslib - Win32 Release"


"$(INTDIR)\ThdSemaphore.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\cslib.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "cslib - Win32 Debug"


"$(INTDIR)\ThdSemaphore.obj"	"$(INTDIR)\ThdSemaphore.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\cslib.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\src\thd\ThdSyncObject.cpp

!IF  "$(CFG)" == "cslib - Win32 Release"


"$(INTDIR)\ThdSyncObject.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\cslib.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "cslib - Win32 Debug"


"$(INTDIR)\ThdSyncObject.obj"	"$(INTDIR)\ThdSyncObject.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\cslib.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\src\thd\ThdThread.cpp

!IF  "$(CFG)" == "cslib - Win32 Release"


"$(INTDIR)\ThdThread.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\cslib.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "cslib - Win32 Debug"


"$(INTDIR)\ThdThread.obj"	"$(INTDIR)\ThdThread.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\cslib.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\src\thd\ThdWaitableTimer.cpp

!IF  "$(CFG)" == "cslib - Win32 Release"


"$(INTDIR)\ThdWaitableTimer.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\cslib.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "cslib - Win32 Debug"


"$(INTDIR)\ThdWaitableTimer.obj"	"$(INTDIR)\ThdWaitableTimer.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\cslib.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\src\util\UtilDllLoader.cpp

!IF  "$(CFG)" == "cslib - Win32 Release"


"$(INTDIR)\UtilDllLoader.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\cslib.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "cslib - Win32 Debug"


"$(INTDIR)\UtilDllLoader.obj"	"$(INTDIR)\UtilDllLoader.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\cslib.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\src\util\UtilMapping.cpp

!IF  "$(CFG)" == "cslib - Win32 Release"


"$(INTDIR)\UtilMapping.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\cslib.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "cslib - Win32 Debug"


"$(INTDIR)\UtilMapping.obj"	"$(INTDIR)\UtilMapping.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\cslib.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\src\util\UtilPattern.cpp

!IF  "$(CFG)" == "cslib - Win32 Release"


"$(INTDIR)\UtilPattern.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\cslib.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "cslib - Win32 Debug"


"$(INTDIR)\UtilPattern.obj"	"$(INTDIR)\UtilPattern.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\cslib.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\src\util\UtilStreaming.cpp

!IF  "$(CFG)" == "cslib - Win32 Release"


"$(INTDIR)\UtilStreaming.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\cslib.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "cslib - Win32 Debug"


"$(INTDIR)\UtilStreaming.obj"	"$(INTDIR)\UtilStreaming.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\cslib.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\src\util\UtilStringUtils.cpp

!IF  "$(CFG)" == "cslib - Win32 Release"


"$(INTDIR)\UtilStringUtils.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\cslib.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "cslib - Win32 Debug"


"$(INTDIR)\UtilStringUtils.obj"	"$(INTDIR)\UtilStringUtils.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\cslib.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\src\xml\XmlDomUtils.cpp

!IF  "$(CFG)" == "cslib - Win32 Release"


"$(INTDIR)\XmlDomUtils.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\cslib.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "cslib - Win32 Debug"


"$(INTDIR)\XmlDomUtils.obj"	"$(INTDIR)\XmlDomUtils.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\cslib.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\src\net\Net.cpp

!IF  "$(CFG)" == "cslib - Win32 Release"


"$(INTDIR)\Net.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\cslib.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "cslib - Win32 Debug"


"$(INTDIR)\Net.obj"	"$(INTDIR)\Net.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\cslib.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\src\net\NetHttpClientPost.cpp

!IF  "$(CFG)" == "cslib - Win32 Release"


"$(INTDIR)\NetHttpClientPost.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\cslib.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "cslib - Win32 Debug"


"$(INTDIR)\NetHttpClientPost.obj"	"$(INTDIR)\NetHttpClientPost.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\cslib.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\src\net\NetHttpContent.cpp

!IF  "$(CFG)" == "cslib - Win32 Release"


"$(INTDIR)\NetHttpContent.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\cslib.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "cslib - Win32 Debug"


"$(INTDIR)\NetHttpContent.obj"	"$(INTDIR)\NetHttpContent.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\cslib.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\src\net\NetHttpSerPost.cpp

!IF  "$(CFG)" == "cslib - Win32 Release"


"$(INTDIR)\NetHttpSerPost.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\cslib.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "cslib - Win32 Debug"


"$(INTDIR)\NetHttpSerPost.obj"	"$(INTDIR)\NetHttpSerPost.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\cslib.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\src\net\NetHttpUtils.cpp

!IF  "$(CFG)" == "cslib - Win32 Release"


"$(INTDIR)\NetHttpUtils.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\cslib.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "cslib - Win32 Debug"


"$(INTDIR)\NetHttpUtils.obj"	"$(INTDIR)\NetHttpUtils.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\cslib.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\src\net\NetSocket.cpp

!IF  "$(CFG)" == "cslib - Win32 Release"


"$(INTDIR)\NetSocket.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\cslib.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "cslib - Win32 Debug"


"$(INTDIR)\NetSocket.obj"	"$(INTDIR)\NetSocket.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\cslib.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\src\fwk\fwkBootstrap.cpp

!IF  "$(CFG)" == "cslib - Win32 Release"


"$(INTDIR)\fwkBootstrap.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\cslib.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "cslib - Win32 Debug"


"$(INTDIR)\fwkBootstrap.obj"	"$(INTDIR)\fwkBootstrap.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\cslib.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\src\svr\nt\CService.cpp

!IF  "$(CFG)" == "cslib - Win32 Release"


"$(INTDIR)\CService.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\cslib.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "cslib - Win32 Debug"


"$(INTDIR)\CService.obj"	"$(INTDIR)\CService.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\cslib.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\src\svr\nt\CServiceInstall.cpp

!IF  "$(CFG)" == "cslib - Win32 Release"


"$(INTDIR)\CServiceInstall.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\cslib.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "cslib - Win32 Debug"


"$(INTDIR)\CServiceInstall.obj"	"$(INTDIR)\CServiceInstall.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\cslib.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\src\svr\nt\NTServiceMain.cpp

!IF  "$(CFG)" == "cslib - Win32 Release"


"$(INTDIR)\NTServiceMain.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\cslib.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "cslib - Win32 Debug"


"$(INTDIR)\NTServiceMain.obj"	"$(INTDIR)\NTServiceMain.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\cslib.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\src\ser\SerFileInputSource.cpp

!IF  "$(CFG)" == "cslib - Win32 Release"


"$(INTDIR)\SerFileInputSource.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\cslib.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "cslib - Win32 Debug"


"$(INTDIR)\SerFileInputSource.obj"	"$(INTDIR)\SerFileInputSource.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\cslib.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\src\ser\SerFileOutputSource.cpp

!IF  "$(CFG)" == "cslib - Win32 Release"


"$(INTDIR)\SerFileOutputSource.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\cslib.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "cslib - Win32 Debug"


"$(INTDIR)\SerFileOutputSource.obj"	"$(INTDIR)\SerFileOutputSource.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\cslib.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\src\ser\SerMemInputSource.cpp

!IF  "$(CFG)" == "cslib - Win32 Release"


"$(INTDIR)\SerMemInputSource.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\cslib.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "cslib - Win32 Debug"


"$(INTDIR)\SerMemInputSource.obj"	"$(INTDIR)\SerMemInputSource.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\cslib.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\src\ser\SerMemOutputSource.cpp

!IF  "$(CFG)" == "cslib - Win32 Release"


"$(INTDIR)\SerMemOutputSource.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\cslib.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "cslib - Win32 Debug"


"$(INTDIR)\SerMemOutputSource.obj"	"$(INTDIR)\SerMemOutputSource.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\cslib.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\src\ser\SerSerializationMgr.cpp

!IF  "$(CFG)" == "cslib - Win32 Release"


"$(INTDIR)\SerSerializationMgr.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\cslib.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "cslib - Win32 Debug"


"$(INTDIR)\SerSerializationMgr.obj"	"$(INTDIR)\SerSerializationMgr.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\cslib.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\src\cslib.cpp

!IF  "$(CFG)" == "cslib - Win32 Release"

CPP_SWITCHES=/nologo /MD /W3 /GX /O2 /I "$(XERCESCROOT)\include" /I "$(XERCESCROOT)\include\xercesc" /I "..\h" /D "NDEBUG" /D "NOMINMAX" /D "WIN32" /D "_WINDOWS" /D _WIN32_WINNT=0x0400 /Fp"$(INTDIR)\cslib.pch" /Yc"cslib.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\cslib.obj"	"$(INTDIR)\cslib.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "cslib - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /ZI /Od /I "$(XERCESCROOT)\include" /I "$(XERCESCROOT)\include\xercesc" /I "..\h" /D "_DEBUG" /D "NOMINMAX" /D "WIN32" /D "_WINDOWS" /D _WIN32_WINNT=0x0400 /FR"$(INTDIR)\\" /Fp"$(INTDIR)\cslib.pch" /Yc"cslib.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\cslib.obj"	"$(INTDIR)\cslib.sbr"	"$(INTDIR)\cslib.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

!IF  "$(CFG)" == "cslib - Win32 Release"

"CslibMsg - Win32 Release" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\CslibMsg.mak" CFG="CslibMsg - Win32 Release" 
   cd "."

"CslibMsg - Win32 ReleaseCLEAN" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\CslibMsg.mak" CFG="CslibMsg - Win32 Release" RECURSE=1 CLEAN 
   cd "."

!ELSEIF  "$(CFG)" == "cslib - Win32 Debug"

"CslibMsg - Win32 Debug" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\CslibMsg.mak" CFG="CslibMsg - Win32 Debug" 
   cd "."

"CslibMsg - Win32 DebugCLEAN" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\CslibMsg.mak" CFG="CslibMsg - Win32 Debug" RECURSE=1 CLEAN 
   cd "."

!ENDIF 


!ENDIF 

