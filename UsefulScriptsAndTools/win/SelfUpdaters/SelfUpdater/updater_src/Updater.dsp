# Microsoft Developer Studio Project File - Name="Updater" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=Updater - Win32 Debug restore version
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Updater.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Updater.mak" CFG="Updater - Win32 Debug restore version"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Updater - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "Updater - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE "Updater - Win32 Beta release" (based on "Win32 (x86) Application")
!MESSAGE "Updater - Win32 Release without compression" (based on "Win32 (x86) Application")
!MESSAGE "Updater - Win32 Debug proxy settings" (based on "Win32 (x86) Application")
!MESSAGE "Updater - Win32 Debug restore version" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Updater - Win32 Release"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 5
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "ZLIB_WINAPI" /D "UNICODE" /Yu"stdafx.h" /FD /c
# SUBTRACT CPP /Fr
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x413 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 shell32.lib shlwapi.lib ws2_32.lib zlib\zlibstat.lib winhttp.lib msimg32.lib /nologo /entry:"wWinMainCRTStartup" /subsystem:windows /machine:I386
# SUBTRACT LINK32 /profile /pdb:none /incremental:yes /map /debug /nodefaultlib
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=exe\upx --best Release\updater.exe
# End Special Build Tool

!ELSEIF  "$(CFG)" == "Updater - Win32 Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 5
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "ZLIB_WINAPI" /D "UNICODE" /FR /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x413 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 shlwapi.lib ws2_32.lib zlib\zlibstat.lib winhttp.lib msimg32.lib /nologo /entry:"wWinMainCRTStartup" /subsystem:windows /map /debug /machine:I386 /pdbtype:sept
# SUBTRACT LINK32 /profile /pdb:none /nodefaultlib

!ELSEIF  "$(CFG)" == "Updater - Win32 Beta release"

# PROP BASE Use_MFC 5
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Updater___Win32_Beta_release"
# PROP BASE Intermediate_Dir "Updater___Win32_Beta_release"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 5
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Beta"
# PROP Intermediate_Dir "Beta"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "ZLIB_WINAPI" /Yu"stdafx.h" /FD /c
# SUBTRACT BASE CPP /Fr
# ADD CPP /nologo /MT /W3 /GX /O2 /D "_BETA" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "ZLIB_WINAPI" /D "UNICODE" /Yu"stdafx.h" /FD /c
# SUBTRACT CPP /Fr
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x413 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 shell32.lib shlwapi.lib ws2_32.lib zlib\zlibstat.lib winhttp.lib /nologo /subsystem:windows /machine:I386
# SUBTRACT BASE LINK32 /pdb:none /incremental:yes /map /debug /nodefaultlib
# ADD LINK32 shell32.lib shlwapi.lib ws2_32.lib zlib\zlibstat.lib winhttp.lib msimg32.lib /nologo /entry:"wWinMainCRTStartup" /subsystem:windows /machine:I386
# SUBTRACT LINK32 /profile /pdb:none /incremental:yes /map /debug /nodefaultlib
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=exe\upx --best Beta\updater.exe
# End Special Build Tool

!ELSEIF  "$(CFG)" == "Updater - Win32 Release without compression"

# PROP BASE Use_MFC 5
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Updater___Win32_Release_without_compression"
# PROP BASE Intermediate_Dir "Updater___Win32_Release_without_compression"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 5
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Updater___Win32_Release_without_compression"
# PROP Intermediate_Dir "Updater___Win32_Release_without_compression"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "ZLIB_WINAPI" /Yu"stdafx.h" /FD /c
# SUBTRACT BASE CPP /Fr
# ADD CPP /nologo /MT /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "ZLIB_WINAPI" /D "UNICODE" /Yu"stdafx.h" /FD /c
# SUBTRACT CPP /Fr
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 shell32.lib shlwapi.lib ws2_32.lib zlib\zlibstat.lib winhttp.lib /nologo /subsystem:windows /machine:I386
# SUBTRACT BASE LINK32 /profile /pdb:none /incremental:yes /map /debug /nodefaultlib
# ADD LINK32 shell32.lib shlwapi.lib ws2_32.lib zlib\zlibstat.lib winhttp.lib msimg32.lib /nologo /entry:"wWinMainCRTStartup" /subsystem:windows /machine:I386
# SUBTRACT LINK32 /profile /pdb:none /incremental:yes /map /debug /nodefaultlib

!ELSEIF  "$(CFG)" == "Updater - Win32 Debug proxy settings"

# PROP BASE Use_MFC 5
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Updater___Win32_Debug_proxy_settings"
# PROP BASE Intermediate_Dir "Updater___Win32_Debug_proxy_settings"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 5
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Updater___Win32_Debug_proxy_settings"
# PROP Intermediate_Dir "Updater___Win32_Debug_proxy_settings"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "ZLIB_WINAPI" /FR /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "ZLIB_WINAPI" /D "UNICODE" /FR /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 shell32.lib shlwapi.lib ws2_32.lib zlib\zlibstat.lib winhttp.lib /nologo /subsystem:windows /map /debug /machine:I386 /pdbtype:sept
# SUBTRACT BASE LINK32 /profile /pdb:none /nodefaultlib
# ADD LINK32 shell32.lib shlwapi.lib ws2_32.lib zlib\zlibstat.lib winhttp.lib msimg32.lib /nologo /entry:"wWinMainCRTStartup" /subsystem:windows /map /debug /machine:I386 /pdbtype:sept
# SUBTRACT LINK32 /profile /pdb:none /nodefaultlib

!ELSEIF  "$(CFG)" == "Updater - Win32 Debug restore version"

# PROP BASE Use_MFC 5
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Updater___Win32_Debug_restore_version"
# PROP BASE Intermediate_Dir "Updater___Win32_Debug_restore_version"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 5
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Updater___Win32_Debug_restore_version"
# PROP Intermediate_Dir "Updater___Win32_Debug_restore_version"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "ZLIB_WINAPI" /FR /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "ZLIB_WINAPI" /D "UNICODE" /FR /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 shell32.lib shlwapi.lib ws2_32.lib zlib\zlibstat.lib winhttp.lib /nologo /subsystem:windows /map /debug /machine:I386 /pdbtype:sept
# SUBTRACT BASE LINK32 /profile /pdb:none /nodefaultlib
# ADD LINK32 shell32.lib shlwapi.lib ws2_32.lib zlib\zlibstat.lib winhttp.lib msimg32.lib /nologo /entry:"wWinMainCRTStartup" /subsystem:windows /map /debug /machine:I386 /pdbtype:sept
# SUBTRACT LINK32 /profile /pdb:none /nodefaultlib

!ENDIF 

# Begin Target

# Name "Updater - Win32 Release"
# Name "Updater - Win32 Debug"
# Name "Updater - Win32 Beta release"
# Name "Updater - Win32 Release without compression"
# Name "Updater - Win32 Debug proxy settings"
# Name "Updater - Win32 Debug restore version"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Group "3rdParty_cpp"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\CustomBitmapButton.cpp
# End Source File
# Begin Source File

SOURCE=.\FileVersion.cpp
# End Source File
# Begin Source File

SOURCE=.\GetOpt.cpp
# End Source File
# Begin Source File

SOURCE=.\Markup.cpp
# End Source File
# Begin Source File

SOURCE=.\md5.cpp
# End Source File
# Begin Source File

SOURCE=.\Registry.cpp
# End Source File
# Begin Source File

SOURCE=.\Rijndael.cpp
# End Source File
# Begin Source File

SOURCE=.\SystemTray.cpp
# End Source File
# Begin Source File

SOURCE=.\ThemeUtil.cpp
# End Source File
# Begin Source File

SOURCE=.\Unzipper.cpp
# End Source File
# Begin Source File

SOURCE=.\XHTMLStatic.cpp
# End Source File
# Begin Source File

SOURCE=.\XHyperLink.cpp
# End Source File
# Begin Source File

SOURCE=.\XNamedColors.cpp
# End Source File
# End Group
# Begin Group "Actions_cpp"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\ActionPoints.cpp
# End Source File
# End Group
# Begin Group "Copy_cpp"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\CopyFile.cpp
# End Source File
# Begin Source File

SOURCE=.\CopyFolder.cpp
# End Source File
# End Group
# Begin Group "Data_cpp"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\EventData.cpp
# End Source File
# Begin Source File

SOURCE=.\FileData.cpp
# End Source File
# Begin Source File

SOURCE=.\IniData.cpp
# End Source File
# Begin Source File

SOURCE=.\RegistryData.cpp
# End Source File
# Begin Source File

SOURCE=.\SectionData.cpp
# End Source File
# Begin Source File

SOURCE=.\ShortcutData.cpp
# End Source File
# End Group
# Begin Group "Info_cpp"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\EventInfo.cpp
# End Source File
# Begin Source File

SOURCE=.\RestoreInfo.cpp
# End Source File
# Begin Source File

SOURCE=.\RollbackInfo.cpp
# End Source File
# Begin Source File

SOURCE=.\SelfUpdateInfo.cpp
# End Source File
# Begin Source File

SOURCE=.\UpdateInfo.cpp
# End Source File
# End Group
# Begin Group "Internet_cpp"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\ConnectionTest.cpp
# End Source File
# Begin Source File

SOURCE=.\FtpFileObject.cpp
# End Source File
# Begin Source File

SOURCE=.\HttpFileObject.cpp
# End Source File
# Begin Source File

SOURCE=.\Internet.cpp
# End Source File
# Begin Source File

SOURCE=.\NetworkFileObject.cpp
# End Source File
# Begin Source File

SOURCE=.\Proxy.cpp
# End Source File
# Begin Source File

SOURCE=.\ProxyInfo.cpp
# End Source File
# Begin Source File

SOURCE=.\ProxyWPAD.cpp
# End Source File
# End Group
# Begin Group "Service_cpp"

# PROP Default_Filter "cpp"
# End Group
# Begin Group "Singleton_cpp"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Functions.cpp
# End Source File
# Begin Source File

SOURCE=.\GarbageCollector.cpp
# End Source File
# Begin Source File

SOURCE=.\InternalData.cpp
# End Source File
# Begin Source File

SOURCE=.\Language.cpp
# End Source File
# Begin Source File

SOURCE=.\Log.cpp
# End Source File
# Begin Source File

SOURCE=.\Path.cpp
# End Source File
# Begin Source File

SOURCE=.\RiskFunctions.cpp
# End Source File
# Begin Source File

SOURCE=.\Sections.cpp
# End Source File
# Begin Source File

SOURCE=.\Settings.cpp
# End Source File
# Begin Source File

SOURCE=.\StringFunctions.cpp
# End Source File
# Begin Source File

SOURCE=.\UserDataCollection.cpp
# End Source File
# End Group
# Begin Group "Tasks_cpp"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\BackUp.cpp
# End Source File
# Begin Source File

SOURCE=.\CheckVersion.cpp
# End Source File
# Begin Source File

SOURCE=.\Download.cpp
# End Source File
# Begin Source File

SOURCE=.\Install.cpp
# End Source File
# Begin Source File

SOURCE=.\InstallFiles.cpp
# End Source File
# Begin Source File

SOURCE=.\InstallIni.cpp
# End Source File
# Begin Source File

SOURCE=.\InstallRegistry.cpp
# End Source File
# Begin Source File

SOURCE=.\Protection.cpp
# End Source File
# Begin Source File

SOURCE=.\Restore.cpp
# End Source File
# Begin Source File

SOURCE=.\Rollback.cpp
# End Source File
# Begin Source File

SOURCE=.\SelectUpdate.cpp
# End Source File
# End Group
# Begin Group "UI_cpp"

# PROP Default_Filter "cpp"
# Begin Group "Controls_cpp"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\EditLicense.cpp
# End Source File
# Begin Source File

SOURCE=.\HyperlinkLabel.cpp
# End Source File
# Begin Source File

SOURCE=.\ResultLabel.cpp
# End Source File
# Begin Source File

SOURCE=.\TransparentBitmap.cpp
# End Source File
# Begin Source File

SOURCE=.\TransparentIcon.cpp
# End Source File
# Begin Source File

SOURCE=.\TransparentStatic.cpp
# End Source File
# Begin Source File

SOURCE=.\TreeUpdater.cpp
# End Source File
# Begin Source File

SOURCE=.\WebBrowser.cpp
# End Source File
# End Group
# Begin Group "Dialogs_cpp"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\AboutDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\AuthenticationDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\CloseApplicationDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\ConfirmationDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\ErrorDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\InformationDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\LogViewerDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\ModeFullPropCheck.cpp
# End Source File
# Begin Source File

SOURCE=.\ModeFullPropDownload.cpp
# End Source File
# Begin Source File

SOURCE=.\ModeFullPropFinish.cpp
# End Source File
# Begin Source File

SOURCE=.\ModeFullPropHtmlMessage.cpp
# End Source File
# Begin Source File

SOURCE=.\ModeFullPropInstall.cpp
# End Source File
# Begin Source File

SOURCE=.\ModeFullPropLicense.cpp
# End Source File
# Begin Source File

SOURCE=.\ModeFullPropProtection.cpp
# End Source File
# Begin Source File

SOURCE=.\ModeFullPropRollback.cpp
# End Source File
# Begin Source File

SOURCE=.\ModeFullPropSelectUpdate.cpp
# End Source File
# Begin Source File

SOURCE=.\ModeFullPropWelcome.cpp
# End Source File
# Begin Source File

SOURCE=.\ModeFullSheet.cpp
# End Source File
# Begin Source File

SOURCE=.\ModeHidden.cpp
# End Source File
# Begin Source File

SOURCE=.\ModeSilent.cpp
# End Source File
# Begin Source File

SOURCE=.\NotifyDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\ProxySettingsDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\RestoreDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\SelectLanguageDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\WarningDlg.cpp
# End Source File
# End Group
# End Group
# Begin Group "Process_cpp"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\CheckForUpdatesProcess.cpp
# End Source File
# Begin Source File

SOURCE=.\UpdateProcess.cpp
# End Source File
# End Group
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\Updater.cpp
# End Source File
# Begin Source File

SOURCE=.\Updater.rc
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Group "3rdParty_h"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\CustomBitmapButton.h
# End Source File
# Begin Source File

SOURCE=.\FileVersion.h
# End Source File
# Begin Source File

SOURCE=.\GetOpt.h
# End Source File
# Begin Source File

SOURCE=.\Markup.h
# End Source File
# Begin Source File

SOURCE=.\md5.h
# End Source File
# Begin Source File

SOURCE=.\Registry.h
# End Source File
# Begin Source File

SOURCE=.\Rijndael.h
# End Source File
# Begin Source File

SOURCE=.\SystemTray.h
# End Source File
# Begin Source File

SOURCE=.\ThemeUtil.h
# End Source File
# Begin Source File

SOURCE=.\Unzipper.h
# End Source File
# Begin Source File

SOURCE=.\XHTMLStatic.h
# End Source File
# Begin Source File

SOURCE=.\XHyperLink.h
# End Source File
# Begin Source File

SOURCE=.\XNamedColors.h
# End Source File
# End Group
# Begin Group "Actions_h"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\ActionPoints.h
# End Source File
# Begin Source File

SOURCE=.\Actions.h
# End Source File
# Begin Source File

SOURCE=.\EventActions.h
# End Source File
# Begin Source File

SOURCE=.\RestoreActions.h
# End Source File
# Begin Source File

SOURCE=.\RollbackActions.h
# End Source File
# End Group
# Begin Group "Copy_h"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\CopyFile.h
# End Source File
# Begin Source File

SOURCE=.\CopyFolder.h
# End Source File
# End Group
# Begin Group "Data_h"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\EventData.h
# End Source File
# Begin Source File

SOURCE=.\FileData.h
# End Source File
# Begin Source File

SOURCE=.\IniData.h
# End Source File
# Begin Source File

SOURCE=.\PopupMenuData.h
# End Source File
# Begin Source File

SOURCE=.\RegistryData.h
# End Source File
# Begin Source File

SOURCE=.\SectionData.h
# End Source File
# Begin Source File

SOURCE=.\ShortcutData.h
# End Source File
# End Group
# Begin Group "Info_h"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\EventInfo.h
# End Source File
# Begin Source File

SOURCE=.\RestoreInfo.h
# End Source File
# Begin Source File

SOURCE=.\RollbackInfo.h
# End Source File
# Begin Source File

SOURCE=.\SelfUpdateInfo.h
# End Source File
# Begin Source File

SOURCE=.\UpdateInfo.h
# End Source File
# End Group
# Begin Group "Internet_h"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\ConnectionTest.h
# End Source File
# Begin Source File

SOURCE=.\FtpFileObject.h
# End Source File
# Begin Source File

SOURCE=.\HttpFileObject.h
# End Source File
# Begin Source File

SOURCE=.\Internet.h
# End Source File
# Begin Source File

SOURCE=.\NetworkFileObject.h
# End Source File
# Begin Source File

SOURCE=.\Proxy.h
# End Source File
# Begin Source File

SOURCE=.\ProxyInfo.h
# End Source File
# Begin Source File

SOURCE=.\ProxyWPAD.h
# End Source File
# End Group
# Begin Group "Service_h"

# PROP Default_Filter "h"
# End Group
# Begin Group "Singleton_h"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Functions.h
# End Source File
# Begin Source File

SOURCE=.\GarbageCollector.h
# End Source File
# Begin Source File

SOURCE=.\InternalData.h
# End Source File
# Begin Source File

SOURCE=.\Language.h
# End Source File
# Begin Source File

SOURCE=.\Log.h
# End Source File
# Begin Source File

SOURCE=.\Path.h
# End Source File
# Begin Source File

SOURCE=.\RiskFunctions.h
# End Source File
# Begin Source File

SOURCE=.\Sections.h
# End Source File
# Begin Source File

SOURCE=.\Settings.h
# End Source File
# Begin Source File

SOURCE=.\StringFunctions.h
# End Source File
# Begin Source File

SOURCE=.\UserDataCollection.h
# End Source File
# End Group
# Begin Group "Tasks_h"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\BackUp.h
# End Source File
# Begin Source File

SOURCE=.\CheckVersion.h
# End Source File
# Begin Source File

SOURCE=.\Download.h
# End Source File
# Begin Source File

SOURCE=.\Install.h
# End Source File
# Begin Source File

SOURCE=.\InstallFiles.h
# End Source File
# Begin Source File

SOURCE=.\InstallIni.h
# End Source File
# Begin Source File

SOURCE=.\InstallRegistry.h
# End Source File
# Begin Source File

SOURCE=.\Protection.h
# End Source File
# Begin Source File

SOURCE=.\Restore.h
# End Source File
# Begin Source File

SOURCE=.\Rollback.h
# End Source File
# Begin Source File

SOURCE=.\SelectUpdate.h
# End Source File
# End Group
# Begin Group "UI_h"

# PROP Default_Filter ""
# Begin Group "Controls_h"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\EditLicense.h
# End Source File
# Begin Source File

SOURCE=.\HyperlinkLabel.h
# End Source File
# Begin Source File

SOURCE=.\ResultLabel.h
# End Source File
# Begin Source File

SOURCE=.\TransparentBitmap.h
# End Source File
# Begin Source File

SOURCE=.\TransparentIcon.h
# End Source File
# Begin Source File

SOURCE=.\TransparentStatic.h
# End Source File
# Begin Source File

SOURCE=.\TreeUpdater.h
# End Source File
# Begin Source File

SOURCE=.\WebBrowser.h
# End Source File
# End Group
# Begin Group "Dialogs_h"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\AboutDlg.h
# End Source File
# Begin Source File

SOURCE=.\AuthenticationDlg.h
# End Source File
# Begin Source File

SOURCE=.\CloseApplicationDlg.h
# End Source File
# Begin Source File

SOURCE=.\ConfirmationDlg.h
# End Source File
# Begin Source File

SOURCE=.\ErrorDlg.h
# End Source File
# Begin Source File

SOURCE=.\InformationDlg.h
# End Source File
# Begin Source File

SOURCE=.\LogViewerDlg.h
# End Source File
# Begin Source File

SOURCE=.\ModeFullPropCheck.h
# End Source File
# Begin Source File

SOURCE=.\ModeFullPropDownload.h
# End Source File
# Begin Source File

SOURCE=.\ModeFullPropFinish.h
# End Source File
# Begin Source File

SOURCE=.\ModeFullPropHtmlMessage.h
# End Source File
# Begin Source File

SOURCE=.\ModeFullPropInstall.h
# End Source File
# Begin Source File

SOURCE=.\ModeFullPropLicense.h
# End Source File
# Begin Source File

SOURCE=.\ModeFullPropProtection.h
# End Source File
# Begin Source File

SOURCE=.\ModeFullPropRollback.h
# End Source File
# Begin Source File

SOURCE=.\ModeFullPropSelectUpdate.h
# End Source File
# Begin Source File

SOURCE=.\ModeFullPropWelcome.h
# End Source File
# Begin Source File

SOURCE=.\ModeFullSheet.h
# End Source File
# Begin Source File

SOURCE=.\ModeHidden.h
# End Source File
# Begin Source File

SOURCE=.\ModeSilent.h
# End Source File
# Begin Source File

SOURCE=.\NotifyDlg.h
# End Source File
# Begin Source File

SOURCE=.\ProxySettingsDlg.h
# End Source File
# Begin Source File

SOURCE=.\RestoreDlg.h
# End Source File
# Begin Source File

SOURCE=.\SelectLanguageDlg.h
# End Source File
# Begin Source File

SOURCE=.\WarningDlg.h
# End Source File
# End Group
# End Group
# Begin Group "Process_h"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\CheckForUpdatesProcess.h
# End Source File
# Begin Source File

SOURCE=.\UpdateProcess.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\Constants.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\Updater.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Group "Icons"

# PROP Default_Filter ""
# Begin Source File

SOURCE=".\res\Check icons\Check_1.ico"
# End Source File
# Begin Source File

SOURCE=".\res\Check icons\Check_2.ico"
# End Source File
# Begin Source File

SOURCE=".\res\Check icons\Check_3.ico"
# End Source File
# Begin Source File

SOURCE=".\res\Check icons\Check_4.ico"
# End Source File
# Begin Source File

SOURCE=".\res\Check icons\Check_5.ico"
# End Source File
# Begin Source File

SOURCE=".\res\Download icons\Download_1.ico"
# End Source File
# Begin Source File

SOURCE=".\res\Download icons\Download_2.ico"
# End Source File
# Begin Source File

SOURCE=".\res\Download icons\Download_3.ico"
# End Source File
# Begin Source File

SOURCE=".\res\Download icons\Download_4.ico"
# End Source File
# Begin Source File

SOURCE=".\res\Install icons\v2\Install_1_1.ico"
# End Source File
# Begin Source File

SOURCE=".\res\Install icons\v2\Install_2_1.ico"
# End Source File
# Begin Source File

SOURCE=".\res\Install icons\v2\Install_3_1.ico"
# End Source File
# Begin Source File

SOURCE=".\res\Install icons\v2\Install_4_1.ico"
# End Source File
# Begin Source File

SOURCE=".\res\Install icons\v2\Install_5_1.ico"
# End Source File
# Begin Source File

SOURCE=".\res\Install icons\v2\Install_6_1.ico"
# End Source File
# Begin Source File

SOURCE=".\res\Install icons\v2\Install_7_1.ico"
# End Source File
# Begin Source File

SOURCE=".\res\Install icons\v2\Install_8_1.ico"
# End Source File
# Begin Source File

SOURCE=.\res\LogViewer.ico
# End Source File
# Begin Source File

SOURCE=.\res\Restore.ico
# End Source File
# Begin Source File

SOURCE=.\res\Result_error.ico
# End Source File
# Begin Source File

SOURCE=.\res\Result_good.ico
# End Source File
# Begin Source File

SOURCE=.\res\Updater.ico
# End Source File
# End Group
# Begin Group "Bitmaps"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\res\about.bmp
# End Source File
# Begin Source File

SOURCE=.\res\authentication.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bmpQuestionMark.bmp
# End Source File
# Begin Source File

SOURCE=.\res\confirmation.bmp
# End Source File
# Begin Source File

SOURCE=.\res\error.bmp
# End Source File
# Begin Source File

SOURCE=.\res\information.bmp
# End Source File
# Begin Source File

SOURCE=.\res\language_select.bmp
# End Source File
# Begin Source File

SOURCE=.\res\logol.bmp
# End Source File
# Begin Source File

SOURCE=.\res\logos.bmp
# End Source File
# Begin Source File

SOURCE=.\res\updater.bmp
# End Source File
# Begin Source File

SOURCE=.\res\updater_neg.bmp
# End Source File
# Begin Source File

SOURCE=.\res\warning.bmp
# End Source File
# Begin Source File

SOURCE=.\res\wizard_header.bmp
# End Source File
# End Group
# Begin Group "Cursors"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\res\Hand.cur
# End Source File
# End Group
# Begin Group "Executables"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\res\UpdaterCopy.exe
# End Source File
# End Group
# Begin Source File

SOURCE=.\res\close_application.bmp
# End Source File
# Begin Source File

SOURCE=.\res\Updater.rc2
# End Source File
# End Group
# End Target
# End Project
# Section Updater : {D30C1661-CDAF-11D0-8A3E-00C04FC9E26E}
# 	2:5:Class:CWebBrowser
# 	2:10:HeaderFile:webbrowser.h
# 	2:8:ImplFile:webbrowser.cpp
# End Section
# Section Updater : {8856F961-340A-11D0-A96B-00C04FD705A2}
# 	2:21:DefaultSinkHeaderFile:webbrowser.h
# 	2:16:DefaultSinkClass:CWebBrowser
# End Section
