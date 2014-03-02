#! /bin/sh
#	Copyright (C) ST-Ericsson 2011
#	general functions for test status notification.
#	Author:	alain.crouzet-nonst@stericsson.com

test_PASS()
{
	local name=$1
	echo -e "Test \033[32mPASS\033[0m: $name"
}

test_FAIL()
{
	local name=$1
	local text=$2

	echo -e "Test \033[31mFAIL\033[0m: $name"

	if [ "$text" != "" ]
	then
		echo "$text"
	fi
}
