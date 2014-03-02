@echo off
cd %~dp0
ant -Dassemble-tool-cli.JUNIT_JAR="\\ws001463\Projects\EMP\Flash Kit\flash-kit-dependencies\assemble-tool-cli\lib\junit-4.6.jar" -Dassemble-tool-cli.KPI_FOLDER="\\ws001463\Projects\EMP\Flash Kit\flash-kit-dependencies\assemble-tool-cli\kpi" assemble-tool-cli.ALL_REPORTS
pause