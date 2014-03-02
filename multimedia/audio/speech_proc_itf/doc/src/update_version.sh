#! /bin/sh
# to be called from doc directory

VERSION=`grep \\version ../include/speech_proc_interface.h | perl -e '<STDIN>=~/([0-9.]+)/ && print "$1\n"'`
perl -p -i -e  "s/(PROJECT_NUMBER\s*=).*/\1 v$VERSION/" src/Doxyfile
