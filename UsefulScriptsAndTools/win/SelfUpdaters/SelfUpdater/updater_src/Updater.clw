; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=CCheckForUpdatesProcess
LastTemplate=CWinThread
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "Updater.h"

ClassCount=21
Class1=CUpdaterApp
Class2=CUpdaterDlg

ResourceCount=28
Resource2=IDD_UPDATER_DIALOG (English (U.S.))
Resource1=IDR_MAINFRAME
Class5=CCheckVersion
Class6=CDownload
Class7=CUISilent
Class3=CUpdateProcess
Class4=CInstall
Resource3=IDD_LOGVIEWER
Resource4=IDD_FULL_PROPPAGE_SELECTUPDATE (English (U.S.))
Resource5=IDD_CRASH
Resource6=IDD_SELECTLANGUAGE
Resource7=IDD_FULL_PROPPAGE_CHECK (English (U.S.))
Resource8=IDD_ERROR
Resource9=IDD_PROXYSETTINGS
Resource10=IDD_WARNING
Class8=CCopyFile
Class9=CInstallFiles
Class10=CCopyFolder
Class11=CBackUp
Resource11=IDD_FULL_PROPPAGE_DOWNLOAD (English (U.S.))
Class12=CAboutDlg
Resource12=IDD_ABOUT (English (U.S.))
Resource13=IDD_AUTHENTICATION (English (U.S.))
Class13=CTreeUpdater
Resource14=IDD_SILENT (English (U.S.))
Resource15=IDD_RESTORE
Class14=CHyperlinkLabel
Resource16=IDD_HIDDEN (English (U.S.))
Class15=CConfirmationStopUpdatingDlg
Resource17=IDD_FORMVIEW (English (U.S.))
Resource18=IDD_CONFIRMATION
Resource19=IDD_CLOSEAPPLICATION
Resource20=IDD_INFORMATION
Resource21=IDD_FULL_PROPPAGE_ROLLBACK (English (U.S.))
Class16=CRestoreDlg
Resource22=IDD_FULL_PROPPAGE_PROTECTION (English (U.S.))
Class17=CProxySettingsDlg
Class18=CEditLicense
Resource23=IDD_FULL_PROPPAGE_FINISH (English (U.S.))
Class19=CSelectLanguageDlg
Resource24=IDD_FULL_PROPPAGE_INSTALL (English (U.S.))
Resource25=IDD_FULL_PROPPAGE_LICENSE (English (U.S.))
Resource26=IDD_FULL_PROPPAGE_HTMLMESSAGE (English (U.S.))
Class20=CLogViewerDlg
Class21=CCheckForUpdatesProcess
Resource27=IDD_FULL_PROPPAGE_WELCOME (English (U.S.))
Resource28=IDR_SILENT_POPUP (English (U.S.))

[CLS:CUpdaterApp]
Type=0
HeaderFile=Updater.h
ImplementationFile=Updater.cpp
Filter=N

[CLS:CUpdaterDlg]
Type=0
HeaderFile=UpdaterDlg.h
ImplementationFile=UpdaterDlg.cpp
Filter=D



[DLG:IDD_UPDATER_DIALOG (English (U.S.))]
Type=1
ControlCount=3
Control1=IDOK,button,1342242817
Control2=IDCANCEL,button,1342242816
Control3=IDC_STATIC,static,1342308352

[CLS:CUpdateProcess]
Type=0
HeaderFile=UpdateProcess.h
ImplementationFile=UpdateProcess.cpp
BaseClass=CWinThread
Filter=N

[CLS:CInstall]
Type=0
HeaderFile=Install.h
ImplementationFile=Install.cpp
BaseClass=CWinThread
Filter=N

[CLS:CCheckVersion]
Type=0
HeaderFile=CheckVersion.h
ImplementationFile=CheckVersion.cpp
BaseClass=CWinThread
Filter=N

[CLS:CDownload]
Type=0
HeaderFile=Download.h
ImplementationFile=Download.cpp
BaseClass=CWinThread
Filter=N

[CLS:CUISilent]
Type=0
HeaderFile=UISilent.h
ImplementationFile=UISilent.cpp
BaseClass=generic CWnd
Filter=W

[DLG:IDD_SILENT (English (U.S.))]
Type=1
Class=?
ControlCount=1
Control1=lblExplanation,static,1342308352

[MNU:IDR_SILENT_POPUP (English (U.S.))]
Type=1
Class=?
Command1=ID_SILENT_POPUP_ABOUT
Command2=ID_SILENT_POPUP_EXIT
CommandCount=2

[DLG:IDD_FULL_PROPPAGE_WELCOME (English (U.S.))]
Type=1
Class=?
ControlCount=2
Control1=lblExplanation,static,1342308352
Control2=lblTitle,static,1342308364

[DLG:IDD_FULL_PROPPAGE_INSTALL (English (U.S.))]
Type=1
Class=?
ControlCount=7
Control1=lblFileProgress,static,1342308352
Control2=prgFileProgress,msctls_progress32,1350565888
Control3=lblTotalProgress,static,1342308352
Control4=prgTotalProgress,msctls_progress32,1350565888
Control5=lblExplanation,static,1342308352
Control6=lblFileProgressProcent,static,1342308354
Control7=lblTotalProgressProcent,static,1342308354

[DLG:IDD_FULL_PROPPAGE_FINISH (English (U.S.))]
Type=1
Class=?
ControlCount=3
Control1=lblExplanation,static,1342308352
Control2=chkCreateShortcuts,button,1342242819
Control3=chkUserData,button,1073807363

[DLG:IDD_FULL_PROPPAGE_DOWNLOAD (English (U.S.))]
Type=1
Class=?
ControlCount=7
Control1=lblFileProgress,static,1342308352
Control2=prgFileProgress,msctls_progress32,1350565888
Control3=lblFileProgressProcent,static,1342308354
Control4=lblTotalProgress,static,1342308352
Control5=prgTotalProgress,msctls_progress32,1350565888
Control6=lblTotalProgressProcent,static,1342308354
Control7=lblExplanation,static,1342308352

[DLG:IDD_FULL_PROPPAGE_CHECK (English (U.S.))]
Type=1
Class=?
ControlCount=8
Control1=lblStatus,static,1342308352
Control2=lblExplanation,static,1342308352
Control3=lblFeatures,static,1073872896
Control4=lblFeature1,static,1073872896
Control5=lblFeature2,static,1073872896
Control6=lblFeature3,static,1073872896
Control7=lblFeature4,static,1073872896
Control8=lblFeature5,static,1073872896

[DLG:IDD_FULL_PROPPAGE_ROLLBACK (English (U.S.))]
Type=1
Class=?
ControlCount=4
Control1=lblTotalProgress,static,1342308352
Control2=prgTotalProgress,msctls_progress32,1350565888
Control3=lblExplanation,static,1342308352
Control4=lblTotalProgressProcent,static,1342308354

[CLS:CCopyFile]
Type=0
HeaderFile=CopyFile.h
ImplementationFile=CopyFile.cpp
BaseClass=CWinThread
Filter=N

[CLS:CInstallFiles]
Type=0
HeaderFile=InstallFiles.h
ImplementationFile=InstallFiles.cpp
BaseClass=CWinThread
Filter=N

[CLS:CCopyFolder]
Type=0
HeaderFile=CopyFolder.h
ImplementationFile=CopyFolder.cpp
BaseClass=CWinThread
Filter=N

[CLS:CBackUp]
Type=0
HeaderFile=BackUp.h
ImplementationFile=BackUp.cpp
BaseClass=CWinThread
Filter=N

[CLS:CAboutDlg]
Type=0
HeaderFile=AboutDlg.h
ImplementationFile=AboutDlg.cpp
BaseClass=CDialog
Filter=D
LastObject=btnOK

[DLG:IDD_HIDDEN (English (U.S.))]
Type=1
Class=?
ControlCount=1
Control1=lblExplanation,static,1342308352

[DLG:IDD_ABOUT (English (U.S.))]
Type=1
Class=CAboutDlg
ControlCount=13
Control1=btnOK,button,1342242817
Control2=lblDeveloper,static,1342308352
Control3=lblWebsite,static,1342308352
Control4=lblUpdaterVersion,static,1342308352
Control5=lblLanguageVersion,static,1342308352
Control6=txtUpdaterVersion,edit,1350633600
Control7=txtLanguageVersion,edit,1350633600
Control8=picLogo,static,1342177294
Control9=lblLanguageUsed,static,1342308352
Control10=txtLanguageUsed,edit,1350633600
Control11=lblHyperlink,static,1342308610
Control12=lblSpecialThanks,static,1342308352
Control13=lstThanks,SysListView32,1350565889

[DLG:IDD_FULL_PROPPAGE_SELECTUPDATE (English (U.S.))]
Type=1
Class=?
ControlCount=5
Control1=treeUpdateSelection,SysTreeView32,1350635811
Control2=lblDescriptionTitle,static,1342308352
Control3=lblDescription,static,1342308352
Control4=lblExplanation,static,1342308352
Control5=lblMoreInfo,static,1342308352

[DLG:IDD_FULL_PROPPAGE_LICENSE (English (U.S.))]
Type=1
Class=?
ControlCount=3
Control1=txtLicense,edit,1352665220
Control2=rdLicenceAgree,button,1342390281
Control3=rdLicenceNotAgree,button,1342259209

[CLS:CTreeUpdater]
Type=0
HeaderFile=TreeUpdater.h
ImplementationFile=TreeUpdater.cpp
BaseClass=CTreeCtrl
Filter=W

[DLG:IDD_AUTHENTICATION (English (U.S.))]
Type=1
Class=?
ControlCount=10
Control1=txtUsername,edit,1350631552
Control2=txtPassword,edit,1350631584
Control3=btnOK,button,1342242817
Control4=btnCancel,button,1342242816
Control5=lblExplanation,static,1342308352
Control6=lblUsername,static,1342308352
Control7=lblPassword,static,1342308352
Control8=chkSaveCredentials,button,1342242819
Control9=bmpLogo,static,1342177294
Control10=lblTitle,static,1342308864

[DLG:IDD_FULL_PROPPAGE_HTMLMESSAGE (English (U.S.))]
Type=1
Class=?
ControlCount=1
Control1=htmlMessage,{8856F961-340A-11D0-A96B-00C04FD705A2},1342242816

[CLS:CHyperlinkLabel]
Type=0
HeaderFile=HyperlinkLabel.h
ImplementationFile=HyperlinkLabel.cpp
BaseClass=CStatic
Filter=W

[CLS:CConfirmationStopUpdatingDlg]
Type=0
HeaderFile=ConfirmationStopUpdatingDlg.h
ImplementationFile=ConfirmationStopUpdatingDlg.cpp
BaseClass=CDialog
Filter=D
LastObject=btnNo

[DLG:IDD_CONFIRMATION]
Type=1
Class=CConfirmationStopUpdatingDlg
ControlCount=6
Control1=btnYes,button,1342242817
Control2=btnNo,button,1342242816
Control3=lblExplanation,static,1342308352
Control4=lnSplitter,static,1342177287
Control5=lblTitle,static,1342308864
Control6=bmpLogo,static,1342177294

[DLG:IDD_ERROR]
Type=1
Class=?
ControlCount=8
Control1=btnOK,button,1342242817
Control2=btnMore,button,1342242816
Control3=lblExplanation,static,1342308352
Control4=lnSplitter,static,1342177287
Control5=lblTitle,static,1342308864
Control6=bmpLogo,static,1342177294
Control7=lblMore,static,1342308352
Control8=lnMoreSplitter,static,1342177288

[DLG:IDD_INFORMATION]
Type=1
Class=?
ControlCount=5
Control1=btnOK,button,1342242817
Control2=lblExplanation,static,1342308352
Control3=lnSplitter,static,1342177287
Control4=lblTitle,static,1342308864
Control5=bmpLogo,static,1342177294

[DLG:IDD_FULL_PROPPAGE_PROTECTION (English (U.S.))]
Type=1
Class=?
ControlCount=4
Control1=lblExplanation,static,1342308352
Control2=lblCheckLicense,static,1342308352
Control3=lblCheckFiles,static,1342308352
Control4=lblGetLicense,static,1342308352

[DLG:IDD_WARNING]
Type=1
Class=?
ControlCount=5
Control1=btnOK,button,1342242817
Control2=lblExplanation,static,1342308352
Control3=lnSplitter,static,1342177287
Control4=lblTitle,static,1342308864
Control5=bmpLogo,static,1342177294

[DLG:IDD_RESTORE]
Type=1
Class=CRestoreDlg
ControlCount=5
Control1=btnRestore,button,1342242817
Control2=btnClose,button,1342242816
Control3=lblExplanation,static,1342308352
Control4=lblProgress,static,1342308352
Control5=prgRestore,msctls_progress32,1350565888

[CLS:CRestoreDlg]
Type=0
HeaderFile=RestoreDlg.h
ImplementationFile=RestoreDlg.cpp
BaseClass=CDialog
Filter=D

[DLG:IDD_PROXYSETTINGS]
Type=1
Class=CProxySettingsDlg
ControlCount=11
Control1=rdDirectConnection,button,1342177289
Control2=rdAutoDetect,button,1342177289
Control3=rdManual,button,1342177289
Control4=txtHttpProxy,edit,1350631552
Control5=txtFtpProxy,edit,1350631552
Control6=rdAutoConfigURL,button,1342177289
Control7=txtAutoConfigURL,edit,1350631552
Control8=btnOK,button,1342242817
Control9=btnCancel,button,1342242816
Control10=lblHttpProxy,static,1342308352
Control11=lblFtpProxy,static,1342308352

[CLS:CProxySettingsDlg]
Type=0
HeaderFile=ProxySettingsDlg.h
ImplementationFile=ProxySettingsDlg.cpp
BaseClass=CDialog
Filter=D

[CLS:CEditLicense]
Type=0
HeaderFile=EditLicense.h
ImplementationFile=EditLicense.cpp
BaseClass=CEdit
Filter=W
VirtualFilter=WC

[DLG:IDD_SELECTLANGUAGE]
Type=1
Class=CSelectLanguageDlg
ControlCount=5
Control1=IDOK,button,1342242817
Control2=chkRemember,button,1342242819
Control3=cboLanguage,combobox,1344340227
Control4=IDC_STATIC,static,1342308352
Control5=bmpImage,static,1342177294

[CLS:CSelectLanguageDlg]
Type=0
HeaderFile=SelectLanguageDlg.h
ImplementationFile=SelectLanguageDlg.cpp
BaseClass=CDialog
Filter=D

[DLG:IDD_CRASH]
Type=1
Class=?
ControlCount=5
Control1=IDOK,button,1342242817
Control2=lblTitle,static,1342308352
Control3=lblDetails,static,1342308352
Control4=txtCrashInfo,edit,1350633604
Control5=chkSendErrorReport,button,1342242819

[DLG:IDD_FORMVIEW (English (U.S.))]
Type=1
Class=?
ControlCount=1
Control1=IDC_STATIC,static,1342308352

[DLG:IDD_LOGVIEWER]
Type=1
Class=CLogViewerDlg
ControlCount=2
Control1=btnOK,button,1342242817
Control2=txtLog,edit,1353779332

[CLS:CLogViewerDlg]
Type=0
HeaderFile=LogViewerDlg.h
ImplementationFile=LogViewerDlg.cpp
BaseClass=CDialog
Filter=D
LastObject=btnOK

[CLS:CCheckForUpdatesProcess]
Type=0
HeaderFile=CheckForUpdatesProcess.h
ImplementationFile=CheckForUpdatesProcess.cpp
BaseClass=CWinThread
Filter=N

[DLG:IDD_CLOSEAPPLICATION]
Type=1
Class=?
ControlCount=8
Control1=lblExplanation,static,1342308352
Control2=prgEnd,msctls_progress32,1350565888
Control3=btnCancel,button,1342242816
Control4=btnEndNow,button,1342242817
Control5=lblTitle,static,1342308864
Control6=lnSplitter,static,1342177287
Control7=IDC_STATIC,static,1342177294
Control8=icoApplication,static,1342177287

