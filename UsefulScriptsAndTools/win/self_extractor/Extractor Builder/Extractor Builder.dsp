# Microsoft Developer Studio Project File - Name="Extractor Builder" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=Extractor Builder - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Extractor Builder.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Extractor Builder.mak" CFG="Extractor Builder - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Extractor Builder - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "Extractor Builder - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Extractor Builder - Win32 Release"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "../Shared" /I "../Zlib" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /YX"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x809 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x809 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 /nologo /subsystem:windows /machine:I386 /out:"../Release/Extractor Builder.exe"
# SUBTRACT LINK32 /debug

!ELSEIF  "$(CFG)" == "Extractor Builder - Win32 Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "../Shared ../Zlib" /I "../Shared" /I "../Zlib" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# SUBTRACT CPP /Fr
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x809 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x809 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 /nologo /subsystem:windows /debug /machine:I386 /out:"../Debug/Extractor Builder.exe" /pdbtype:sept

!ENDIF 

# Begin Target

# Name "Extractor Builder - Win32 Release"
# Name "Extractor Builder - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\Shared\DirDialog.cpp
# ADD CPP /Yu"stdafx.h"
# End Source File
# Begin Source File

SOURCE=".\Extractor Builder.cpp"
# End Source File
# Begin Source File

SOURCE=".\Extractor Builder.rc"
# End Source File
# Begin Source File

SOURCE=".\Extractor BuilderDlg.cpp"
# End Source File
# Begin Source File

SOURCE=..\Shared\SEFileInfo.cpp
# ADD CPP /Yu"stdafx.h"
# End Source File
# Begin Source File

SOURCE=..\Shared\SelfExtractor.cpp
# ADD CPP /Yu"stdafx.h"
# End Source File
# Begin Source File

SOURCE=..\Shared\ShellFileOp.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=..\Shared\TextProgressCtrl.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\Shared\DirDialog.h
# End Source File
# Begin Source File

SOURCE=".\Extractor Builder.h"
# End Source File
# Begin Source File

SOURCE=".\Extractor BuilderDlg.h"
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=..\Shared\SEFileInfo.h
# End Source File
# Begin Source File

SOURCE=..\Shared\SelfExtractor.h
# End Source File
# Begin Source File

SOURCE=..\Shared\ShellFileOp.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=..\Shared\TextProgressCtrl.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;cnt;rtf;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=".\res\Extractor Builder.ico"
# End Source File
# Begin Source File

SOURCE=".\res\Extractor Builder.rc2"
# End Source File
# Begin Source File

SOURCE=".\res\Self Extractor.ico"
# End Source File
# End Group
# Begin Group "Zlib"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\Zlib\adler32.c

!IF  "$(CFG)" == "Extractor Builder - Win32 Release"

!ELSEIF  "$(CFG)" == "Extractor Builder - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\Zlib\compress.c

!IF  "$(CFG)" == "Extractor Builder - Win32 Release"

!ELSEIF  "$(CFG)" == "Extractor Builder - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\Zlib\crc32.c

!IF  "$(CFG)" == "Extractor Builder - Win32 Release"

!ELSEIF  "$(CFG)" == "Extractor Builder - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\Zlib\deflate.c

!IF  "$(CFG)" == "Extractor Builder - Win32 Release"

!ELSEIF  "$(CFG)" == "Extractor Builder - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\Zlib\gzio.c

!IF  "$(CFG)" == "Extractor Builder - Win32 Release"

!ELSEIF  "$(CFG)" == "Extractor Builder - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\Zlib\infblock.c

!IF  "$(CFG)" == "Extractor Builder - Win32 Release"

!ELSEIF  "$(CFG)" == "Extractor Builder - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\Zlib\infcodes.c

!IF  "$(CFG)" == "Extractor Builder - Win32 Release"

!ELSEIF  "$(CFG)" == "Extractor Builder - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\Zlib\inffast.c

!IF  "$(CFG)" == "Extractor Builder - Win32 Release"

!ELSEIF  "$(CFG)" == "Extractor Builder - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\Zlib\inflate.c

!IF  "$(CFG)" == "Extractor Builder - Win32 Release"

!ELSEIF  "$(CFG)" == "Extractor Builder - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\Zlib\inftrees.c

!IF  "$(CFG)" == "Extractor Builder - Win32 Release"

!ELSEIF  "$(CFG)" == "Extractor Builder - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\Zlib\infutil.c

!IF  "$(CFG)" == "Extractor Builder - Win32 Release"

!ELSEIF  "$(CFG)" == "Extractor Builder - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\Zlib\trees.c

!IF  "$(CFG)" == "Extractor Builder - Win32 Release"

!ELSEIF  "$(CFG)" == "Extractor Builder - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\Zlib\uncompr.c

!IF  "$(CFG)" == "Extractor Builder - Win32 Release"

!ELSEIF  "$(CFG)" == "Extractor Builder - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\Zlib\zutil.c

!IF  "$(CFG)" == "Extractor Builder - Win32 Release"

!ELSEIF  "$(CFG)" == "Extractor Builder - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# End Group
# Begin Source File

SOURCE=..\Release\Extractor.exe
# End Source File
# End Target
# End Project
