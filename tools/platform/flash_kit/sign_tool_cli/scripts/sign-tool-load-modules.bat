@echo off
set JARPATH=%~dp0

java -Xmx256M -Dlog4j.configuration="file:///%JARPATH%/log4j.properties" -classpath "%JARPATH%/bcprov-ext-jdk15-143.jar;%JARPATH%\com.stericsson.sdk.common.jar;%JARPATH%\com.stericsson.sdk.signing.jar;%JARPATH%/log4j-1.2.15.jar;%JARPATH%/commons-cli-1.2.jar;%JARPATH%/com.stericsson.sdk.signing.cli.jar" com.stericsson.sdk.signing.cli.Sign load-modules %*
