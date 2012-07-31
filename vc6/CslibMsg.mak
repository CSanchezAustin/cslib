# Microsoft Developer Studio Generated NMAKE File, Based on CslibMsg.dsp
!IF "$(CFG)" == ""
CFG=CslibMsg - Win32 Debug
!MESSAGE No configuration specified. Defaulting to CslibMsg - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "CslibMsg - Win32 Release" && "$(CFG)" != "CslibMsg - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "CslibMsg.mak" CFG="CslibMsg - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "CslibMsg - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "CslibMsg - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "CslibMsg - Win32 Release"

OUTDIR=.\..\bin\vc6\win32rel
INTDIR=.\..\obj\vc6\win32rel
# Begin Custom Macros
OutDir=.\..\bin\vc6\win32rel
# End Custom Macros

ALL : ".\..\h\log\nt\CslibMsg.h" "$(OUTDIR)\CslibMsg.dll"


CLEAN :
	-@erase "$(INTDIR)\CslibMsgr.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\Version.res"
	-@erase "$(OUTDIR)\CslibMsg.dll"
	-@erase "$(OUTDIR)\CslibMsg.exp"
	-@erase "$(OUTDIR)\CslibMsg.lib"
	-@erase ".\..\h\log\nt\CslibMsg.h"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /Fp"$(INTDIR)\CslibMsg.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

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

MTL=midl.exe
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32 
RSC=rc.exe
RSC_PROJ=/l 0x809 /fo"$(INTDIR)\Version.res" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\CslibMsg.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /incremental:no /pdb:"$(OUTDIR)\CslibMsg.pdb" /machine:I386 /out:"$(OUTDIR)\CslibMsg.dll" /implib:"$(OUTDIR)\CslibMsg.lib" 
LINK32_OBJS= \
	"$(INTDIR)\CslibMsgr.obj" \
	"$(INTDIR)\Version.res"

"$(OUTDIR)\CslibMsg.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "CslibMsg - Win32 Debug"

OUTDIR=.\..\bin\vc6\win32dbg
INTDIR=.\..\obj\vc6\win32dbg
# Begin Custom Macros
OutDir=.\..\bin\vc6\win32dbg
# End Custom Macros

ALL : ".\..\h\log\nt\CslibMsg.h" "$(OUTDIR)\CslibMsg.dll"


CLEAN :
	-@erase "$(INTDIR)\CslibMsgr.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(INTDIR)\Version.res"
	-@erase "$(OUTDIR)\CslibMsg.dll"
	-@erase "$(OUTDIR)\CslibMsg.exp"
	-@erase "$(OUTDIR)\CslibMsg.ilk"
	-@erase "$(OUTDIR)\CslibMsg.lib"
	-@erase "$(OUTDIR)\CslibMsg.pdb"
	-@erase ".\..\h\log\nt\CslibMsg.h"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /Fp"$(INTDIR)\CslibMsg.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

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

MTL=midl.exe
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32 
RSC=rc.exe
RSC_PROJ=/l 0x809 /fo"$(INTDIR)\Version.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\CslibMsg.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /incremental:yes /pdb:"$(OUTDIR)\CslibMsg.pdb" /debug /machine:I386 /out:"$(OUTDIR)\CslibMsg.dll" /implib:"$(OUTDIR)\CslibMsg.lib" /pdbtype:sept 
LINK32_OBJS= \
	"$(INTDIR)\CslibMsgr.obj" \
	"$(INTDIR)\Version.res"

"$(OUTDIR)\CslibMsg.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("CslibMsg.dep")
!INCLUDE "CslibMsg.dep"
!ELSE 
!MESSAGE Warning: cannot find "CslibMsg.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "CslibMsg - Win32 Release" || "$(CFG)" == "CslibMsg - Win32 Debug"
SOURCE=..\src\log\nt\CslibMsg.mc

!IF  "$(CFG)" == "CslibMsg - Win32 Release"

InputDir=\chriss\src\cslib-working\src\log\nt
ProjDir=.
InputPath=..\src\log\nt\CslibMsg.mc

"..\h\log\nt\CslibMsg.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	<<tempfile.bat 
	@echo off 
	mc.exe -r $(InputDir) -h $(ProjDir)\..\h\log\nt $(InputPath)
<< 
	

!ELSEIF  "$(CFG)" == "CslibMsg - Win32 Debug"

InputDir=\chriss\src\cslib-working\src\log\nt
ProjDir=.
InputPath=..\src\log\nt\CslibMsg.mc

"..\h\log\nt\CslibMsg.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	<<tempfile.bat 
	@echo off 
	mc.exe -r $(InputDir) -h $(ProjDir)\..\h\log\nt $(InputPath)
<< 
	

!ENDIF 

SOURCE=..\src\log\nt\CslibMsgr.c

"$(INTDIR)\CslibMsgr.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\src\log\nt\Version.rc

!IF  "$(CFG)" == "CslibMsg - Win32 Release"


"$(INTDIR)\Version.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) /l 0x809 /fo"$(INTDIR)\Version.res" /i "\chriss\src\cslib-working\src\log\nt" /d "NDEBUG" $(SOURCE)


!ELSEIF  "$(CFG)" == "CslibMsg - Win32 Debug"


"$(INTDIR)\Version.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) /l 0x809 /fo"$(INTDIR)\Version.res" /i "\chriss\src\cslib-working\src\log\nt" /d "_DEBUG" $(SOURCE)


!ENDIF 


!ENDIF 

