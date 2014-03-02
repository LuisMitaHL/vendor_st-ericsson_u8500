@echo off
cd %~dp0
ant -Dflash-tool-backend.BUILD_DEPENDENCIES_DIR="\\ws001463\Projects\EMP\Flash Kit\flash-kit-dependencies\flash-tool-backend" -DEASYMOCK_JAR="\\ws001463\Projects\EMP\Flash Kit\flash-kit-dependencies\flash-tool-backend\easymock\easymock.jar" flash-tool-backend.RELEASE_WITH_KPI
pause