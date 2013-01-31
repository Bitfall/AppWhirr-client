@echo off

SET ORIGINAL_WORKING_DIR=%CD%
SET BAT_DIR=%~dp0

rem Switch the working directory to the this bat file's path (other bat's are relative to this dir)
cd /d %BAT_DIR%

rem Run the post-process bat scripts
START /B updater_project_compile_finished_growl_notify.bat

rem Switch back to the original working directory
cd /d %ORIGINAL_WORKING_DIR%