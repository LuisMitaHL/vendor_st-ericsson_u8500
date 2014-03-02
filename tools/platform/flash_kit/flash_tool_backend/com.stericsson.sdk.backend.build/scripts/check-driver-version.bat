@echo off

setlocal EnableExtensions
setlocal EnableDelayedExpansion

echo.
echo Checking Flash Kit USB driver...

if "%SYSTEMROOT%" EQU "" (
  set WINDOWS_ROOT=c:\windows
) else (
  set WINDOWS_ROOT=%SYSTEMROOT%
)

set CONFIGURATION_FILE=%~f1
if "%CONFIGURATION_FILE%" EQU "" (
  set CONFIGURATION_FILE=%~dp0check-driver-version.config
)
if not exist "%CONFIGURATION_FILE%" (
  echo Configuration file %CONFIGURATION_FILE% not found
  exit /B 1
)

set DRIVER_VERSION=
for /F "usebackq eol=# tokens=1,2 delims==" %%i in ("%CONFIGURATION_FILE%") do (
  set PARAMETER_NAME=%%i
  set PARAMETER_NAME=!PARAMETER_NAME: =!
  set PARAMETER_VALUE=%%j
  set PARAMETER_VALUE=!PARAMETER_VALUE: =!

  if /I "!PARAMETER_NAME!" EQU "DriverVersion" (
    set DRIVER_VERSION=!PARAMETER_VALUE!
  )
)

if "%DRIVER_VERSION%" EQU "" (
  echo Missing DriverVersion in configuration file %CONFIGURATION_FILE%
  exit /B 1
)

set DRIVERS_LIST=
set DRIVERS_SEARCH_PATH=%WINDOWS_ROOT%\system32\drvstore\ste_usb_rom.inf
for /F %%i in ( 'dir /B /S %%DRIVERS_SEARCH_PATH%% 2^>NUL' ) do (
  if "!DRIVERS_LIST!" EQU "" (
    set DRIVERS_LIST=%%i
  ) else (
    set DRIVERS_LIST=!DRIVERS_LIST! %%i
  )
)

if "%DRIVERS_LIST%" EQU "" (
  set DRIVERS_SEARCH_PATH=%WINDOWS_ROOT%\system32\driverstore\filerepository\ste_usb_rom.inf
  for /F %%i in ( 'dir /B /S %%DRIVERS_SEARCH_PATH%% 2^>NUL' ) do (
    if "!DRIVERS_LIST!" EQU "" (
      set DRIVERS_LIST=%%i
    ) else (
      set DRIVERS_LIST=!DRIVERS_LIST! %%i
    )
  )
)

if "%DRIVERS_LIST%" EQU "" (
  echo Flash Kit USB driver is not installed on this computer
  exit /B 1
)

set INSTALLED_DRIVER_VERSION=
for /F %%i in ("%DRIVERS_LIST%") do (
  for /F "usebackq eol=; tokens=1,2 delims==" %%j in ("%%i") do (
    set PARAMETER_NAME=%%j
    set PARAMETER_NAME=!PARAMETER_NAME: =!
    set PARAMETER_VALUE=%%k
    set PARAMETER_VALUE=!PARAMETER_VALUE: =!
    set PARAMETER_VALUE=!PARAMETER_VALUE:~1,-1!
    if /I "!PARAMETER_NAME!" EQU "DriverVersion" (
      set INSTALLED_DRIVER_VERSION=!PARAMETER_VALUE!
    )
  )
)

set RETURN_CODE=0
echo Required driver version:  %DRIVER_VERSION%
if "%INSTALLED_DRIVER_VERSION%" EQU "" (
  echo Installed driver version: N/A
  set RETURN_CODE=1
) else (
  if /I "%INSTALLED_DRIVER_VERSION%" EQU "%DRIVER_VERSION%" (
    echo Installed driver version: %INSTALLED_DRIVER_VERSION% ^(the same as the required one^)
    set RETURN_CODE=0
  )
  if /I "%INSTALLED_DRIVER_VERSION%" LSS "%DRIVER_VERSION%" (
    echo Installed driver version: %INSTALLED_DRIVER_VERSION% ^(older than the required one^)
    set RETURN_CODE=1
  )
  if /I "%INSTALLED_DRIVER_VERSION%" GTR "%DRIVER_VERSION%" (
    echo Installed driver version: %INSTALLED_DRIVER_VERSION% ^(newer than the required one^)
    set RETURN_CODE=0
  )
)

if %RETURN_CODE% EQU 0 (
  echo.
  echo Installed Flash Kit USB driver is compatible
  echo.
) else (
  echo.
  echo.
  echo.
  echo.
  echo **************************************************
  echo *                                                *
  echo *  WARNING                                       *
  echo *                                                *
  echo *  Required version of the Flash Kit USB driver  *
  echo *  is not installed on this computer.            *
  echo *                                                *
  echo **************************************************
  echo.
  echo.
  echo.
  echo.
)

exit /B %RETURN_CODE%
