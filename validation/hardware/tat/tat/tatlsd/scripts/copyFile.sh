#!/bin/sh


if test -f $2; then
echo "remove file "$2
rm $2
fi

echo "copy file "$1 "to" $2 
cp $1 $2

if test -f $2; then
echo "ok">$3
echo "copy command is well executed!"
else
echo "ko">$3
echo "copy command is NOT well executed!"
fi
exit
