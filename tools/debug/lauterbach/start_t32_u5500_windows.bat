@ECHO OFF

REM Set env variables to TRACE32 and script
SET T32SYS=C:\T32
SET T32TMP=C:\Temp
SET CURRDIR=%CD%
SET SCRIPTPATH=%CURRDIR%\scripts
SET CONFIGPATH=%SCRIPTPATH%\ux500\u5500

REM Check if T32LICENSE is set, if so remove old LICENSE line in config.
IF "%T32LICENSE%" == "" GOTO End
TYPE %CONFIGPATH%\config_app.t32 | findstr /v LICENSE > %CONFIGPATH%\config_app.tmp
MOVE %CONFIGPATH%\config_app.tmp %CONFIGPATH%\config_app.t32
TYPE %CONFIGPATH%\config_acc.t32 | findstr /v LICENSE > %CONFIGPATH%\config_acc.tmp
MOVE %CONFIGPATH%\config_acc.tmp %CONFIGPATH%\config_acc.t32
TYPE %CONFIGPATH%\config_ceva.t32 | findstr /v LICENSE > %CONFIGPATH%\config_ceva.tmp
MOVE %CONFIGPATH%\config_ceva.tmp %CONFIGPATH%\config_ceva.t32

REM Add new LICENSE line into config files
echo LICENSE=%T32LICENSE% >> %CONFIGPATH%\config_app.t32
echo LICENSE=%T32LICENSE% >> %CONFIGPATH%\config_acc.t32
echo LICENSE=%T32LICENSE% >> %CONFIGPATH%\config_ceva.t32
:End

rem Clear all selections
SET APP=
SET ACC=
SET CEVA=
:Again
ECHO Choose the configuration that you would like to use:
ECHO 1 - APP
ECHO 2 - APP+ACC
ECHO 3 - APP+ACC+CEVA
SET /p choice=Type the number: 
IF NOT '%choice%'=='' SET choice=%choice:~0,1%

IF %choice% LEQ 3 SET APP=1
IF %choice% EQU 2 SET ACC=1
IF %choice% EQU 3 SET ACC=1
IF %choice% EQU 3 SET CEVA=1


IF NOT '%APP%%ACC%%CEVA%'=='' goto Continue
ECHO Entry "%choice%" is not valid, please try again
ECHO.
goto Again

:Continue


IF DEFINED APP start /Dc:\ %T32SYS%\t32marm.exe -c %CONFIGPATH%\config_app.t32,%SCRIPTPATH%\init.cmm [U5500_APP]
rem TIMEOUT /T 1 - not supported on XP
rem This is a trick to emulate SLEEP with PING
ping -n 6 localhost > nul
IF DEFINED ACC start /Dc:\ %T32SYS%\t32marm.exe -c %CONFIGPATH%\config_acc.t32,%SCRIPTPATH%\init.cmm [U5500_ACC]
rem TIMEOUT /T 1 - not supported on XP
ping -n 6 localhost > nul
IF DEFINED CEVA start /Dc:\ %T32SYS%\t32mceva.exe -c %CONFIGPATH%\config_ceva.t32,%SCRIPTPATH%\init.cmm [U5500_CEVA]
