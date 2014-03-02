#!/bin/sh

if [ $# -lt 2 ]; then
  echo "Expected arguments: ./make_versions <tool_version_file_name> [tool_relative_path]+"
else
  TOOL_VERSION=$1
  ARG=1
  for i in $@ ; do
    if [ $ARG -gt 1 ] ; then
      cat $i/$1
    fi
    ARG=`expr $ARG + 1`
  done
fi
