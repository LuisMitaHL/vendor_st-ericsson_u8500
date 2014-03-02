@echo off
if [%1]==[] (
	goto help
) else if [%1]==[Flash_Tool_Backend] (
	shift
  explorer.exe .\com.stericsson.sdk.backend.build\doc\flash-tool-backend.html
  goto end	
)

:help
echo.
echo Usage: man Flash_Tool_Backend
echo.

:end