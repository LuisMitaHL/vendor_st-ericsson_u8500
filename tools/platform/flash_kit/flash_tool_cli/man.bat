@echo off
if [%1]==[] (
	goto help
) else if [%1]==[Flash_Tool_CLI] (
	shift
  explorer.exe .\doc\flash-tool-cli.html
  goto end
)

:help
echo.
echo Usage: man Flash_Tool_CLI
echo.

:end