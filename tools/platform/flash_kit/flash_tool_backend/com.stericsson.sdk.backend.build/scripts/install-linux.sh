#!/bin/bash

INSTALL=`which install`
MKDIR=`which mkdir`

if [ -z "$1" ]; then
BINPATH=/usr/bin
MODE="-o root -g root"
else
bash -c "echo $1"
BINPATH=`bash -c "echo $1"`
fi

STE_FLASH_TOOL_BACKEND_HOME=${BINPATH}/flash-tool-backend

echo "Installing flash tool backend in ${BINPATH}"
echo "Please ensure that you have write permissions in those directories"
echo "Add ${BINPATH} to your PATH environment variable"
 
${MKDIR} -p ${BINPATH}
${MKDIR} -p ${STE_FLASH_TOOL_BACKEND_HOME}
${MKDIR} -p ${STE_FLASH_TOOL_BACKEND_HOME}/configuration
${MKDIR} -p ${STE_FLASH_TOOL_BACKEND_HOME}/native
${MKDIR} -p ${STE_FLASH_TOOL_BACKEND_HOME}/osgi
${MKDIR} -p ${STE_FLASH_TOOL_BACKEND_HOME}/osgi/configuration

${INSTALL} ${MODE} -v -m 0644 -t ${STE_FLASH_TOOL_BACKEND_HOME} *.jar
${INSTALL} ${MODE} -v -m 0644 -t ${STE_FLASH_TOOL_BACKEND_HOME}/configuration configuration/backend-default.mesp
${INSTALL} ${MODE} -v -m 0644 -t ${STE_FLASH_TOOL_BACKEND_HOME}/configuration configuration/profile-default.mesp
${INSTALL} ${MODE} -v -m 0644 -t ${STE_FLASH_TOOL_BACKEND_HOME}/configuration configuration/loader.ldr
${INSTALL} ${MODE} -v -m 0644 -t ${STE_FLASH_TOOL_BACKEND_HOME}/configuration configuration/log4jRelease.xml
${INSTALL} ${MODE} -v -m 0644 -t ${STE_FLASH_TOOL_BACKEND_HOME}/osgi/configuration configuration/config.ini
${INSTALL} ${MODE} -v -m 0644 -t ${STE_FLASH_TOOL_BACKEND_HOME}/native native/*.so
${INSTALL} ${MODE} -v -m 0755 -t ${BINPATH} ste-ftb

echo "STE_FLASH_TOOL_BACKEND_HOME=${STE_FLASH_TOOL_BACKEND_HOME}" > $HOME/.flashtoolbackendinstallpath
echo "If you got permission denied try: ./install.sh <path>"

