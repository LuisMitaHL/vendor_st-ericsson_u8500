#!/bin/sh

echo "put register value in" $1


sk-ab R $2 | cut -f6 -d ' '>$1

echo "finished"
exit
