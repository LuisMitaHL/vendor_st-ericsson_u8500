@ECHO OFF

REM Set env variables to TRACE32 and script
SET T32SYS=C:\T32
SET T32TMP=%TEMP%
SET CURRDIR=%CD%
SET SCRIPTPATH=%CURRDIR%\scripts
SET CONFIGPATH=%SCRIPTPATH%\ux500\l8540
REM Check if T32LICENSE is set, if so remove old LICENSE line in config.

REM manage new bin directory since Trace32 2011 june version
SET T32BIN=C:\T32
IF NOT EXIST %T32BIN%\t32marm.exe (
SET T32BIN=C:\T32\bin\windows
)

IF "%T32LICENSE%" == "" GOTO End
TYPE %CONFIGPATH%\config_app.t32 | findstr /v LICENSE > %CONFIGPATH%\config_app.tmp
MOVE %CONFIGPATH%\config_app.tmp %CONFIGPATH%\config_app.t32
TYPE %CONFIGPATH%\config_sia.t32 | findstr /v LICENSE > %CONFIGPATH%\config_sia.tmp
MOVE %CONFIGPATH%\config_sia.tmp %CONFIGPATH%\config_sia.t32

REM Add new LICENSE line into config files
echo LICENSE=%T32LICENSE% >> %CONFIGPATH%\config_app.t32
echo LICENSE=%T32LICENSE% >> %CONFIGPATH%\config_sia.t32
:End

SET choice=%1
IF DEFINED choice Goto TestPlatform

:Again
ECHO Choose the configuration that you would like to use:
ECHO 1 - APP
ECHO 2 - APP + SIA
SET /p choice=Type the number:
IF '%choice%'=='' (SET choice=1) ELSE (SET choice=%choice:~0,1%)

:TestPlatform
IF %choice% LSS 1 Goto Again
IF %choice% GTR 2 Goto Again

rem Clear all selections
set APP=
set SIA=

IF %choice% EQU 1 (
  ECHO "Debugging: [L8540_APP]"
  SET APP=1
)

IF %choice% EQU 2 (
  ECHO "Debugging: [L8540_APP+SIA]"
  SET APP=1
  SET SIA=1
)

SET mode=%2
IF DEFINED mode Goto TestMode

:AgainMode
ECHO Please select mode:
ECHO 1 - ICD
ECHO 2 - Simulator
SET /p simu=Type the number:
IF '%simu%'=='' (SET simu=1) ELSE (SET simu=%simu:~0,1%)

:TestMode
IF %simu% LSS 1 Goto AgainMode
IF %simu% GTR 2 Goto AgainMode

SET SIM=
IF %simu% EQU 2 (
  SET SIM=_sim
)

IF DEFINED APP start /Dc:\ %T32BIN%\t32marm.exe -c %CONFIGPATH%\config_app%SIM%.t32 -s "%SCRIPTPATH%\init.cmm [L8540_APP]"

IF DEFINED SIA (
ping -n 8 localhost > nul
start /Dc:\ %T32BIN%\t32mmdsp.exe -c %CONFIGPATH%\config_sia%SIM%.t32 -s "%SCRIPTPATH%\init.cmm [L8540_MMDSP]"
)

