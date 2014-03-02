#!/bin/sh
# get or set a feature debug level
# syntax: $0 feature [level]
if [ $# -eq 1 ]
then
	sh dthadv.sh Debug/$1/TraceLevel g
elif [ $# -eq 2 ]; then
	sh dthadv.sh Debug/$1/TraceLevel s $2
else
	echo "$0 feature [level=0-7]"
fi
