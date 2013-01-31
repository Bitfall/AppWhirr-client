#
# !!! installer - licence
#	check in url, or in the client > options, AppWhirr Licence page
#

!define MUI_ICON "icon\appwhirr.ico"

SetCompressor /SOLID LZMA

# ----------------------------------
# --- general installer settings ---
# * check 'General Attributes':
# ** SetCompress (auto|force|off)
# ** SetCompressor (zlib|bzip2|lzma) /FINAL /SOLID
# ** BrandingText
# ** Caption
# ** TargetMinimalOS 5.0 (Win 2000) - important to generate Unicode installer (allowed for 5.0+)

# --------------------------------------------
# --- AppWhirr specific settings / configs ---

# * install dir: user dir - $PROFILE (C:\Documents and Settings\Foo)

# -----------------------------------------------------------
# --- info, can be presented on Windows - file properties ---
VIAddVersionKey /LANG=${LANG_ENGLISH} "ProductName" "AppWhirr client"
VIAddVersionKey /LANG=${LANG_ENGLISH} "Comments" "Official client for Windows"
VIAddVersionKey /LANG=${LANG_ENGLISH} "CompanyName" "BitFall"
VIAddVersionKey /LANG=${LANG_ENGLISH} "LegalTrademarks" "AppWhirr is a trademark of BitFall"
VIAddVersionKey /LANG=${LANG_ENGLISH} "LegalCopyright" "© BitFall"
VIAddVersionKey /LANG=${LANG_ENGLISH} "FileDescription" "AppWhirr official Windows client"
VIAddVersionKey /LANG=${LANG_ENGLISH} "FileVersion" "1.0.0"

VIProductVersion "1.0.0.0"


;--------------------------------
;Include Modern UI and functions
 
# Use Modern UI 2
  !include "MUI2.nsh"
  
  !include "WordFunc.nsh"
  !include "Library.nsh"
  !include "WinVer.nsh"
  !include "FileFunc.nsh"
  !include "Memento.nsh"
  

# -------------------------------
# --- installer code / script ---

# define name of installer
Name "AppWhirr"
OutFile "AppWhirr_inst.exe"
# define installation directory
InstallDir "$PROFILE\AppWhirr"

;Request application privileges for Windows Vista
  RequestExecutionLevel user

;--------------------------------
;Interface Settings
 
  !define MUI_ABORTWARNING
  
# run after install
!define MUI_FINISHPAGE_RUN "$INSTDIR\AppWhirr.exe"
  
/*Function .onInit
 
  ${Unless} ${AtLeastWin2000}
    MessageBox MB_YESNO|MB_ICONSTOP "Unsupported operating system.$\AppWhirr requires at least Windows XP and may not work correctly on your system.$\nDo you really want to continue with the installation?" /SD IDNO IDYES installonoldwindows
    Abort
installonoldwindows:
  ${EndUnless}
  
FunctionEnd*/

# Page texts
!define WELCOME_TITLE 'Welcome to AppWhirr Installer!'

!define WELCOME_THE_TEXT 'Welcome to the extra 3 lines test setup wizard. \
This page has extra space for the welcome title!'

!define FINISH_TITLE 'AppWhirr is ready to use!'

!define FINISH_THE_TEXT 'You can find the AppWhirr icon in your Desktop and in your Start Menu.'

# Pages
!define MUI_WELCOMEPAGE_TITLE '${WELCOME_TITLE}'
#!define MUI_WELCOMEPAGE_TITLE_3LINES
#!define MUI_WELCOMEPAGE_TEXT '${WELCOME_THE_TEXT}'
!insertmacro MUI_PAGE_WELCOME
!define MUI_LICENSEPAGE_CHECKBOX
!define MUI_LICENSEPAGE_CHECKBOX_TEXT 'I accept'
!define MUI_LICENSEPAGE_BUTTON 'Install!'
!insertmacro MUI_PAGE_LICENSE "license\license.txt"
!insertmacro MUI_PAGE_INSTFILES
!define MUI_FINISHPAGE_TITLE '${FINISH_TITLE}'
!define MUI_FINISHPAGE_TITLE_3LINES
!define MUI_FINISHPAGE_TEXT '${FINISH_THE_TEXT}'
!insertmacro MUI_PAGE_FINISH

# Lang
  !insertmacro MUI_LANGUAGE "English"

 
# start default section
Section "Install Section" SecDummy
 
    # set the installation directory as the destination for the following actions
    SetOutPath $INSTDIR
	
	File /r "app_files\*"
 
    # create a shortcut named "new shortcut" in the start menu programs directory
    # point the new shortcut at the program uninstaller
    CreateShortCut "$SMPROGRAMS\AppWhirr.lnk" "$INSTDIR\AppWhirr.exe"
	
	# create desktop shortcut
	CreateShortCut "$DESKTOP\AppWhirr.lnk" "$INSTDIR\AppWhirr.exe" ""

SectionEnd

;--------------------------------
;Descriptions

  ;Language strings
  LangString DESC_SecDummy ${LANG_ENGLISH} "A test section."

  ;Assign language strings to sections
  !insertmacro MUI_FUNCTION_DESCRIPTION_BEGIN
    !insertmacro MUI_DESCRIPTION_TEXT ${SecDummy} $(DESC_SecDummy)
  !insertmacro MUI_FUNCTION_DESCRIPTION_END