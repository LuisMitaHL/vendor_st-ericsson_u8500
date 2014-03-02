#! /bin/sh
# to be called from doc directory

X_VERSION=`grep SPEECH_PROC_MAJOR ../inc/speech_proc_config.h | perl -e '<STDIN>=~/(\d+)/ && print "$1\n"'`
Y_VERSION=`grep SPEECH_PROC_MINOR ../inc/speech_proc_config.h | perl -e '<STDIN>=~/(\d+)/ && print "$1\n"'`
Z_VERSION=`grep SPEECH_PROC_REVISION ../inc/speech_proc_config.h | perl -e '<STDIN>=~/(\d+)/ && print "$1\n"'`

VERSION="$X_VERSION.$Y_VERSION.$Z_VERSION";

perl -p -i -e  "s/(PROJECT_NUMBER\s*=).*/\1 v$VERSION/" src/Doxyfile
