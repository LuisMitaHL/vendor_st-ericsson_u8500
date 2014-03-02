#!/bin/bash

cd `dirname $0`
JAR_PATH=`pwd -P`
cd -

java -classpath "${JAR_PATH}/flash-tool-cli.jar:${JAR_PATH}/commons-cli-1.2.jar:${JAR_PATH}/brp.jar" com.stericsson.sdk.cli.FlashtoolCLI "$@"
