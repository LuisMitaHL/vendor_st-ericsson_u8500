@echo off
if [%1]==[] (
	goto help
) else if [%1]==[Assemble_Tool_CLI] (
	shift
  explorer.exe .\doc\assemble-tool-cli.html
  goto end
)

:help
echo.
echo Usage: man Assemble_Tool_CLI
echo.

:end
