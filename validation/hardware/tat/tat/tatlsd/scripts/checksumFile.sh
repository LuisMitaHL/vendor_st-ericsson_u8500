#!/bin/sh

echo "make checksum of file "$1 in $2 


md5sum $1 | cut -f1 -d ' ' >$2

exit
