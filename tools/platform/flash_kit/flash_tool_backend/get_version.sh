#!/bin/sh

get_report(){
  cat .git/config | grep url | sed 's/.*= \(.*\)/Repository:\ \1/g' > $OUT_FILE
  git log --pretty=oneline | head -n 1 | sed 's/\(.*\)/\ \ \1/' >> $OUT_FILE
  echo >> $OUT_FILE
}

if [ $# -ne 2 ] ; then
  echo "Expected 2 arguments: ./get_version <path_to_git_repo> <tool_version_file_name>"
else
  if [ -d $1 ] ; then
    OUT_FILE=$2
    cd $1
    get_report
    cd -
  else
    echo "Cannot generate version information file: Path to '$1' doesn't exist!"
  fi
fi
