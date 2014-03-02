@echo off

call "..\flashkit\flash-tool-cli\flash-tool.bat" -port 8088 -host localhost set_active_profile -profile_name STE_DB8500_normalboot_bootdebug

exit /B %ERRORLEVEL%
