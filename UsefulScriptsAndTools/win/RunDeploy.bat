@echo off

SET ORIGINAL_WORKING_DIR=%CD%
SET BAT_DIR=%~dp0

rem Switch the working directory to the this bat file's path (other bat's are relative to this dir)
cd /d %BAT_DIR%

rem Run deploy
python Deploy.py

rem Switch back to the original working directory
cd /d %ORIGINAL_WORKING_DIR%