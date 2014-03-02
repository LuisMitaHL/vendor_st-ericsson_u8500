#!/bin/sh
# Create version string for XLoader version identification

cd "${1:-.}"

printf '%s' "const char __attribute__((used)) gitinfo[]=\"\n\nXloader version: "
git log -1 --pretty=format:"%h %ci" | awk '{printf $1 " " $2 " " $3}'

if [ "`git diff --raw HEAD | wc -c`" != 0 ]; then
    printf '%s' " - modified"
fi

printf '%s' "\\n\";"
printf '\n'


