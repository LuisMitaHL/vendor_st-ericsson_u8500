@echo off
cd %~dp0
ant -Dsign-tool-cli_kpi_folder="\\ws001463\Projects\EMP\Flash Kit\flash-kit-dependencies\sign-tool-cli\kpi" -Dsign-tool-cli_junit_jar="\\ws001463\Projects\EMP\Flash Kit\flash-kit-dependencies\sign-tool-cli\lib\junit-4.6.jar" -Dsign-tool-cli_easymock_jar="\\ws001463\Projects\EMP\Flash Kit\flash-kit-dependencies\sign-tool-cli\lib\easymock.jar"  sign-tool-cli.ALL_REPORTS
pause