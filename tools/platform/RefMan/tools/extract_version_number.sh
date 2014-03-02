#!/bin/sh
# © Copyright ST-Ericsson 2010. All rights reserved.

PROJECTROOTDIR=$1
TEMPOUTDIR=$2
WERROR=$3

VERSIONNUMBERFILE=$TEMPOUTDIR/versionNumber.txt

if [ -z "$PROJECTROOTDIR" ]
then
  SHOWPARAMS=yes
fi
if [ -z "$TEMPOUTDIR" ]
then
  SHOWPARAMS=yes
fi

if [ -n "$SHOWPARAMS" ]
then
    echo $0: Usage:
    echo "  $0 path_to_refmandir path_to_tempoutdir [--ignore-errors]"
    echo
    echo "  Obtains the latest platform tag from the manifest git"
    exit 1
fi

if [ "$WERROR" = "--ignore-errors" ]
then
  ABORT_ON_ERRORS=
else
  ABORT_ON_ERRORS=yes
fi

cd $PROJECTROOTDIR/.repo/manifests
VERSIONS=`git log --decorate --pretty=oneline | cut -d '(' -f2 | cut -d ',' -f2 | cut -d ')' -f1`
cd -

if [ -n "$ABORT_ON_ERRORS" ]
then
  if [ -z "$VERSIONS" ]
  then
    exit 1
  fi
fi

for VERSION in $VERSIONS
do
	echo >> $VERSIONNUMBERFILE $VERSION
done
