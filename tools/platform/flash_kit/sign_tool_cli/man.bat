@echo off
if [%1]==[] (
	goto help
) else if [%1]==[Signing] (
	shift
  explorer.exe .\doc\signing.html
  goto end
)else if [%1]==[Sign_Tool_CLI_A2] (
	shift
  explorer.exe .\doc\sign-tool-cli-a2.html
  goto end
) else if [%1]==[Sign_Tool_CLI_M570_Authenticate] (
	shift
  explorer.exe .\doc\sign-tool-cli-m570-authenticate.html
  goto end
) else if [%1]==[Sign_Tool_CLI_U5500_U8500] (
	shift
  explorer.exe .\doc\sign-tool-cli-u5500-u8500.html
  goto end
) else if [%1]==[Sign_Tool_CLI_Load_Modules] (
	shift
  explorer.exe .\doc\sign-tool-cli-load-modules.html
  goto end
)

:help
echo.
echo Usage: man [OPTIONS]
echo.
echo [OPTIONS]:
echo.
echo Signing - sign server setup
echo Sign_Tool_CLI_A2 - signing SW for A2
echo Sign_Tool_CLI_M570_Authenticate - signing SW for M570
echo Sign_Tool_CLI_U5500_U8500 - signing SW for u5500/u8500
echo Sign_Tool_CLI_Load_Modules - signing ELF load modules SW for u5500/u8500/Thorium
echo.

:end