@echo off
set JARPATH=%~dp0

java -Xmx256M -Dlog4j.configuration="file:///%JARPATH%/log4j.properties" -classpath "%JARPATH%\com.stericsson.sdk.common.jar;%JARPATH%\commons-cli-1.2.jar;%JARPATH%\assemble-tool-cli.jar;%JARPATH%\org.apache.log4j.jar" com.stericsson.sdk.assembling.cli.Assemble u8500 %*
