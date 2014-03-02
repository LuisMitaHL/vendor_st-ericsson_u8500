@echo off
setlocal enableextensions enabledelayedexpansion
if not defined STE_FLASH_TOOL_BACKEND_HOME set STE_FLASH_TOOL_BACKEND_HOME=%~dp0

if [%1] == [/?] goto help

set config=%STE_FLASH_TOOL_BACKEND_HOME%\configuration
set loaders=%STE_FLASH_TOOL_BACKEND_HOME%\configuration
set profiles=%STE_FLASH_TOOL_BACKEND_HOME%\configuration
set logconfig=%STE_FLASH_TOOL_BACKEND_HOME%\configuration\log_cfg.xml
set doClose=false

set activeProfile=
set port=

set missingArg=

:loop
if [%1]==[] (
	goto continue
) else if [%1]==[/CONFIG] (
	shift
	goto setConfig
) else if [%1] == [/LOADERS] (
	shift
	goto setLoaders
) else if [%1] == [/PROFILES] (
	shift
	goto setProfiles
) else if [%1] == [/PORT] (
	shift
	goto setPort
) else if [%1] == [/ACTIVE_PROFILE] (
	shift
	goto setActiveProfile
) else if [%1] == [/DEBUG] (
	shift
	goto setDebug
) else if [%1] == [/CLOSE_ON_EXIT] (
    set doClose=true
	shift
) else (
	goto invalidFlag
)
goto loop

:setConfig
if [%1] == [] (
	set missingArg=/CONFIG
	goto missingValue
)
set config=%1%
set config=!config:"=!
REM ---- Checks if last char of the command is not \ char ----
if [%config:~-1%]==[\] (
REM ---- / char is removed from the string ----
set config=%config::~0,-1%
)

shift
goto loop

:setLoaders
if [%1] == [] (
	set missingArg=/LOADERS
	goto missingValue
)
set loaders=%1%
set loaders=!loaders:"=!
REM ---- Checks if last char of the command is not \ char ----
if [%loaders:~-1%]==[\] (
REM ---- / char is removed from the string ----
set loaders=%loaders::~0,-1%

)

shift
goto loop

:setProfiles
if [%1] == [] (
	set missingArg=/PROFILES
	goto missingValue
)
set profiles=%1%
set profiles=!profiles:"=!
REM ---- Checks if last char of the command is not \ char ----
if [%profiles:~-1%]==[\] (
REM ---- / char is removed from the string ----
set profiles=%profiles::~0,-1%
)

shift
goto loop

:setActiveProfile
if [%1] == [] (
	set missingArg=/ACTIVE_PROFILE
	goto missingValue
)
set activeProfile=%1%
shift
goto loop

:setPort
if [%1] == [] (
	set missingArg=/PORT
	goto missingValue
)
set port=%1%
shift
goto loop

:setDebug
set logconfig=%STE_FLASH_TOOL_BACKEND_HOME%\configuration\debug_log_cfg.xml
goto loop

:continue

mkdir .backend
set backDir="%CD%\.backend"

copy "%STE_FLASH_TOOL_BACKEND_HOME%\osgi\configuration\config.ini" %backDir%


if [%BACKEND_TEST_MODE%] == [TRUE] (
  set logconfig=%STE_FLASH_TOOL_BACKEND_HOME%\configuration\debug_log_cfg.xml
)


java -Xmx256M -Dosgi.framework.activeThreadType=normal -Dlog4j.configuration="file:///%logconfig%" -Djava.library.path="%STE_FLASH_TOOL_BACKEND_HOME%\native" -Dnative.root="%STE_FLASH_TOOL_BACKEND_HOME%\native" -Dconfigurations.root="%config%\backend.mesp" -Dloaders.root="%loaders%" -Dprofiles.root="%profiles%" -Dactive.profile=%activeProfile% -jar "%STE_FLASH_TOOL_BACKEND_HOME%\osgi\org.eclipse.osgi_3.5.0.jar" -configuration %backDir% -console %port%
goto end

:help
echo.
echo Usage: flash-tool-backend [/CONFIG CONFIGURATION DIR] [/LOADERS LOADER DIR] [/PROFILES PROFILE DIR] [/ACTIVE_PROFILE] [/PORT PORT] [/CLOSE_ON_EXIT]
echo.
echo   /CONFIG - Specify custom configuration root containing the backend.mesp file.
echo   /LOADERS - Specify custom loaders root containing loader files.
echo   /PROFILES - Specify custom profiles root containing profile files.
echo   /ACTIVE_PROFILE - Specify custom active profile alias.
echo   /PORT - Specify custom OSGI port (access OSGI console from telnet application).
echo   /DEBUG - Turn on DEBUG message logging
echo   /CLOSE_ON_EXIT - Force the script to exit when the backend is shut down.
goto end

:invalidFlag
echo.
echo Invalid flag: %1%
goto help

:missingValue
echo.
echo Missing value for flag: %missingArg%
goto end

:end
echo.
if [%doClose%] == [true] (
	exit
)
