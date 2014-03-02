#!/bin/sh

echo "get"$2 "register value in" $1


sk-db R $2 | cut -f6 -d ' '>$1

echo " finished " 
exit
