@echo off

REM ------------
REM Configurations

set uvBuildDirectory= "E:\AW_Dev\Source\EasyAppsClientProject\EasyAppsClientProject-build-desktop"
set uvSafeCleanPythonScriptDirectory= "E:\AW_Dev\UsefulScriptsAndTools\win"
set uvSafeCleanPythonScriptTargetDirectory= "E:\AW_Dev\Source\EasyAppsClientProject\EasyAppsClientProject-build-desktop"

REM ------------

cd /d %uvBuildDirectory%

rd 3rdParty /s /q
rd EasyAppsClient /s /q
rd Updater /s /q
DEL Makefile

cd /d %uvSafeCleanPythonScriptDirectory%
python SafeCleanTheBuild.py %uvSafeCleanPythonScriptTargetDirectory%