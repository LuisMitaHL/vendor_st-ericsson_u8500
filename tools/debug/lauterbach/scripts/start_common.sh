#!/bin/bash


echo "environment:"
echo "- T32SYS      = $T32SYS"
echo "- T32TMP      = $T32TMP"
echo "- t32marm     = $(which t32marm)"

# Check if all eviroment variables is set before running script
if [ "$T32SYS" == '' ] || [ "$T32TMP" == '' ] || [ "$(which t32marm)" == '' ]; then
	echo
	echo "You need to set the following enviroment variables:"
	echo -e "\033[1m\$T32SYS:\033[0m To point to the install directory for Trace32"
	echo -e "\033[1m\$T32TMP:\033[0m To a directory where you have write access"
	echo -e "\033[1m\$PATH:\033[0m    Add the path to t32marm"
	echo
	echo "More info can be found in the README file"
	echo
	exit
fi

# Check if License file is specified using $T32LICENSE, replace in all config files.
if [ "$T32LICENSE" != "" ]; then
	echo "- T32LICENSE  = $T32LICENSE"
	for f in $CONFIGPATH/config*.t32; do
		if ! grep -q LICENSE=$T32LICENSE $f; then
			#delete old LICENSE Line
			perl -i -nle 'print if !/^LICENSE\b/' $f
			#add LICENSE line
			echo LICENSE=$T32LICENSE >> $f
		fi
	done
fi
