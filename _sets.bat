@echo off
if "%A7_LOCAL_SETS%" == "1" goto A7_P_settings_accepted
set A7_LOCAL_SETS=1
set A7_PATH_MINGW=F:/A5/MinGW
set A7_PATH_GIT=F:/PROGRAMS/GIT/cmd
set PATH=%PATH%;%A7_PATH_MINGW%/bin;%A7_PATH_MINGW%/msys/1.0/bin;%A7_PATH_GIT%
:A7_P_settings_accepted
