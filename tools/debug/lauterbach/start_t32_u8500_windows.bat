@ECHO OFF

REM Set env variables to TRACE32 and script
SET T32SYS=C:\T32
SET T32TMP=%TEMP%
SET CURRDIR=%CD%
SET SCRIPTPATH=%CURRDIR%\scripts
SET CONFIGPATH=%SCRIPTPATH%\ux500\u8500
REM Check if T32LICENSE is set, if so remove old LICENSE line in config.

REM manage new bin directory since Trace32 2011 june version
SET T32BIN=C:\T32
IF NOT EXIST %T32BIN%\t32marm.exe (
SET T32BIN=C:\T32\bin\windows
)

IF "%T32LICENSE%" == "" GOTO End
TYPE %CONFIGPATH%\config_U8500-ED.t32 | findstr /v LICENSE > %CONFIGPATH%\config_U8500-ED.t32.tmp
MOVE %CONFIGPATH%\config_U8500-ED.t32.tmp %CONFIGPATH%\config_U8500-ED.t32
TYPE %CONFIGPATH%\config_U8500-V1.t32 | findstr /v LICENSE > %CONFIGPATH%\config_U8500-V1.t32.tmp
MOVE %CONFIGPATH%\config_U8500-V1.t32.tmp %CONFIGPATH%\config_U8500-V1.t32
TYPE %CONFIGPATH%\config_U8500-V2.t32 | findstr /v LICENSE > %CONFIGPATH%\config_U8500-V2.t32.tmp
MOVE %CONFIGPATH%\config_U8500-V2.t32.tmp %CONFIGPATH%\config_U8500-V2.t32

REM Add new LICENSE line into config files
echo LICENSE=%T32LICENSE% >> %CONFIGPATH%\config_U8500-ED.t32
echo LICENSE=%T32LICENSE% >> %CONFIGPATH%\config_U8500-V1.t32
echo LICENSE=%T32LICENSE% >> %CONFIGPATH%\config_U8500-V2.t32
:End


SET choice=%1
IF DEFINED choice Goto TestPlatform

:AgainPlatform
ECHO Please select platform:
ECHO      Chip          Comment
ECHO 1 -  U8500_V2      V2
ECHO 2 -  U8500_V1      V1.0 and V1.1
ECHO 3 -  U8500_ED      Early Drop
SET /p choice=Type the number:
IF '%choice%'=='' (SET choice=1) ELSE (SET choice=%choice:~0,1%)

:TestPlatform
IF %choice% LSS 1 Goto AgainPlatform
IF %choice% GTR 3 Goto AgainPlatform

SET simu=%2
IF DEFINED simu Goto TestMode

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

SET modem=%3
IF DEFINED modem Goto TestModem

:AgainModem
ECHO Debug modem:
ECHO 0 - No
ECHO 1 - Only L2 core
ECHO 2 - Both L2 and L1 cores
SET /p modem=Type the number:
IF '%modem%'=='' (SET modem=0) ELSE (SET modem=%modem:~0,1%)

:TestModem
IF %modem% LSS 0 Goto AgainModem
IF %modem% GTR 2 Goto AgainModem

set sxa=%4
IF DEFINED sxa Goto TestSxa

:AgainSxa
ECHO Debug SXA:
ECHO 0 - No
ECHO 1 - SIA
ECHO 2 - SVA
ECHO 3 - SIA + SVA
SET /p sxa=Type the number:
IF '%sxa%'=='' (SET sxa=0) ELSE (SET sxa=%sxa:~0,1%)

:TestSxa
IF %sxa% LSS 0 Goto AgainSxa
IF %sxa% GTR 3 Goto AgainSxa


set T32PORT1="30000"
set T32PORT2="30001"
set T32PORT3="30002"
set T32PORT4="30003"


IF %choice% EQU 1 (
	echo Debugging: [U8500-V2_APP]
	set CPU=U8500-V2
    start /Dc:\ %T32BIN%\t32marm.exe -c %CONFIGPATH%\config_U8500-V2%SIM%.t32 -s "%SCRIPTPATH%\init.cmm [U8500-V2_APP]"
) ELSE IF %choice% EQU 2 (
	echo Debugging: [U8500-V1_APP]
	set CPU=U8500-V1
    start /Dc:\ %T32BIN%\t32marm.exe -c %CONFIGPATH%\config_U8500-V1%SIM%.t32 -s "%SCRIPTPATH%\init.cmm [U8500-V1_APP]"
) ELSE IF %choice% EQU 3 (
	echo Debugging: [U8500-ED_APP]
	set CPU=U8500-ED
    start /Dc:\ %T32BIN%\t32marm.exe -c %CONFIGPATH%\config_U8500-V1%SIM%.t32 -s "%SCRIPTPATH%\init.cmm [U8500-ED_APP]"
)

IF NOT %modem% EQU 0 (
  rem TIMEOUT /T 1 - not supported on XP => This is a trick to emulate SLEEP with PING
  ping 10.10.10.10 -n 1 -w 6000 > nul
  echo Debugging: [U8500-MODEM_CORE_L2]
  start /Dc:\ %T32BIN%\t32marm.exe -c %CONFIGPATH%\config_modem1%SIM%.t32
  IF %modem% EQU 2 (
    ping 10.10.10.10 -n 1 -w 6000 > nul
    echo Debugging: [U8500-MODEM_CORE_L1]
    start /Dc:\ %T32BIN%\t32marm.exe -c %CONFIGPATH%\config_modem2%SIM%.t32
  )
)


IF %sxa% EQU 1 (
  ping 10.10.10.10 -n 1 -w 5000 > nul
  echo "Debugging: [%CPU%_MMDSP] SIA"
  start /Dc:\ %T32BIN%\t32mmdsp.exe -c %CONFIGPATH%\config_sia%SIM%.t32 -s "%SCRIPTPATH%\init.cmm [%CPU%_MMDSP]"
) ELSE IF %sxa% EQU 2 (
  ping 10.10.10.10 -n 1 -w 8000 > nul
  echo "Debugging: [%CPU%_MMDSP] SVA"
  start /Dc:\ %T32BIN%\t32mmdsp.exe -c %CONFIGPATH%\config_sva%SIM%.t32 -s "%SCRIPTPATH%\init.cmm [%CPU%_MMDSP]"
) ELSE IF %sxa% EQU 3 (
  ping 10.10.10.10 -n 1 -w 8000 > nul
  echo "Debugging: [%CPU%_MMDSP] SIA + SVA"
  start /Dc:\ %T32BIN%\t32mmdsp.exe -c %CONFIGPATH%\config_sia%SIM%.t32 -s "%SCRIPTPATH%\init.cmm [%CPU%_MMDSP]"
  start /Dc:\ %T32BIN%\t32mmdsp.exe -c %CONFIGPATH%\config_sva%SIM%.t32 -s "%SCRIPTPATH%\init.cmm [%CPU%_MMDSP]"
)

