@echo off
set JARPATH=%~dp0

java -classpath "%JARPATH%\commons-cli-1.2.jar;%JARPATH%\flash-tool-cli.jar;%JARPATH%\brp.jar" com.stericsson.sdk.cli.FlashtoolCLI %*
