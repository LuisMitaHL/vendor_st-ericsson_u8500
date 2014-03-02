@echo off
cd %~dp0
ant -Dflash-tool-cli.JUNIT_JAR="\\ws001463\Projects\EMP\Flash Kit\flash-kit-dependencies\flash-tool-cli\lib\junit-4.6.jar" -Dflash-tool-cli.KPI_FOLDER="\\ws001463\Projects\EMP\Flash Kit\flash-kit-dependencies\flash-tool-cli\kpi" flashtool_cli.ALL_REPORTS
pause