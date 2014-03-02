#!/bin/sh

eclipse_home="/usr/lib/eclipse"

java -jar $eclipse_home/plugins/org.eclipse.equinox.launcher_1.0.201.R35x_v20090715.jar -application org.eclipse.ant.core.antRunner -buildfile build-linux.xml -Dbuilder=$1