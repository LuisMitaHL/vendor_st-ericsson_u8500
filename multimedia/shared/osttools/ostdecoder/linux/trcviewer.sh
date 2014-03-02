#!/bin/sh
export BASEDIR=$(cd $(dirname $0); pwd -P)

## Uncomment if you want to use a custom ostdecoder in OSTViewer
#export DECODER=/your/path/to/ostdecoder

## Uncomment if you want to use a custom TraceServer in OSTViewer
#export TRACESERVER=/your/path/to/TraceServer

## Add JVM used by OSTViewer to the path (this must be a 32 bits jvm!)
export PATH=/sw/st/gnu_compil/gnu/Linux-RH-WS-3/jre/bin:${PATH}

## Add bin to LD_LIBRARY_PATH (stmprobe)
export LD_LIBRARY_PATH=${BASEDIR}/bin:${LD_LIBRARY_PATH}

#"${BASEDIR}/trcviewer/trcviewer" -clean --clear-workspace-preferences -data @noDefault $@ &
"${BASEDIR}/trcviewer/trcviewer" -clean -data @noDefault $@ &
