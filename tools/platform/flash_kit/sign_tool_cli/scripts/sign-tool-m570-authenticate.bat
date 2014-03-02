@echo off
set JARPATH=%~dp0

java -Dlog4j.configuration="file:///%JARPATH%/log4j.properties" -classpath "%JARPATH%\bcprov-ext-jdk15-143.jar;%JARPATH%\com.stericsson.sdk.common.jar;%JARPATH%\com.stericsson.sdk.signing.jar;%JARPATH%\log4j-1.2.15.jar;%JARPATH%\commons-cli-1.2.jar;%JARPATH%\com.stericsson.sdk.signing.cli.jar" com.stericsson.sdk.signing.cli.Sign a2 --target=app --mac-mode=Config --interactive=static --app-sec=on --hdr-sec=verifyOnly --disable-etx-real --disable-etx-header --addr-format=pages --sw-version=0 --sw-type=generic %*