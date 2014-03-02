#!/bin/bash

cd `dirname $0`
FLASH_TOOL_BACKEND_HOME=`pwd -P`
cd -

config="$FLASH_TOOL_BACKEND_HOME/configuration"
loaders="$FLASH_TOOL_BACKEND_HOME/configuration"
profiles="$FLASH_TOOL_BACKEND_HOME/configuration"
logcfg="$FLASH_TOOL_BACKEND_HOME/configuration/log_cfg.xml"
activeProfile=""
port=""
currentDir=`pwd`

while [ $# -gt 0 ]
do
  case $1
  in
    -c | --config)
      config=$2
      shift 2
    ;;

    -l | --loaders)
      loaders=$2
      shift 2
    ;;

    -r | --profiles)
      profiles=$2
      shift 2
    ;;

    -a | --activeProfile)
      activeProfile=$2
      shift 2
    ;;

    -d | --debug)
     logcfg="$FLASH_TOOL_BACKEND_HOME/configuration/debug_log_cfg.xml"
      shift 1
     ;;

    -p | --port)
      port=$2
      shift 2
    ;;

    *)
      echo "Usage: ./flash-tool-backend [-c CONFIGURATION DIR] [-l LOADER DIR] [-r PROFILE DIR] [-p PORT]"
	  echo "	-c, --config <arg>	Specify custom configuration root containing the backend.mesp file."
	  echo "	-l, --loaders <arg>		Specify custom loaders root containing loader files."
	  echo "	-r, --profiles <arg>		Specify custom profiles root containing profile files."
	  echo "	-a, --activeProfile <arg>		Specify custom active profile alias."
	  echo   "-d, --debug  Turn on DEBUG message logging "
	  echo "	-p, --port <arg>		Specify custom OSGI port (access OSGI console from telnet application)."
      exit 1;
    ;;
  esac
done

mkdir -p .backend

cp -f $FLASH_TOOL_BACKEND_HOME/osgi/configuration/config.ini $currentDir/.backend

java -Dosgi.framework.activeThreadType=normal -Dlog4j.configuration="file://$logcfg" -Dconfigurations.root="$config/backend.mesp" -Dloaders.root="$loaders" -Dprofiles.root="$profiles" -Dactive.profile="$activeProfile" -Djava.library.path=$FLASH_TOOL_BACKEND_HOME/native -Dnative.root=$FLASH_TOOL_BACKEND_HOME/native -jar $FLASH_TOOL_BACKEND_HOME/osgi/org.eclipse.osgi_3.5.0.jar -configuration $currentDir/.backend -console $port
