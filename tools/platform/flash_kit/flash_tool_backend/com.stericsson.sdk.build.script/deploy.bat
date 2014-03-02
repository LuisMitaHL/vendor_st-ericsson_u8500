set ECLIPSE_HOME=c:/eclipse
set ECLIPSE_EQUINOX_LAUNCHER_PLUGIN_PATH=%ECLIPSE_HOME%/plugins/org.eclipse.equinox.launcher_1.0.201.R35x_v20090715.jar

java -jar %ECLIPSE_EQUINOX_LAUNCHER_PLUGIN_PATH% -application org.eclipse.ant.core.antRunner -buildfile build-win.xml -Dbuilder=%~dp0
