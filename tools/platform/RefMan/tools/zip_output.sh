#!/bin/sh
# © Copyright ST-Ericsson 2010. All rights reserved.

OUTDIR=$1

if [ -z "$OUTDIR" ]
then
  SHOWPARAMS=yes
fi

if [ -n "$SHOWPARAMS" ]
then
    echo $0: Usage:
    echo "  $0 path_to_outDir"
    echo
    echo "  Zip the content of the eclipseplugin folder"
    exit 1
fi

cd $OUTDIR
zip -9 -r -q Reference_manual.zip *
cd -