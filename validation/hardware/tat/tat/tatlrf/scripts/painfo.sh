#! /bin/sh
#	Copyright (C) ST-Ericsson 2011
#	get or save PA information.
#	This script is member of HATS unitary test tools.
#	Author:	alain.crouzet-nonst@stericsson.com

usage()
{
	echo "$0 access system"
	echo "access: read or save"
    echo "system: gsm, wcdma or utran"
}

. dthadv.sh ""

local path="$DTH9P/RF/PAInfo/Run"

if [ $# -ne 2 ]
then
	usage
	exit 1
fi

local access=$1
local system=$2
local system_code=0

case $system in
    gsm)
        system_code=1
        ;;
    wcdma)
        system_code=2
        ;;
    eutran)
        system_code=4
        ;;
    *)
        echo "invalid system info"
        exit 1
        ;;
esac

# $1 is the page number. if not defined, prints all pages.
case $access in
read)

    dth_set "$path/In_System" "$system_code"
    dth_set "$path/In_SaveOrUpdate" "0"
	dth_exec "$path"
    dth_get_children "$path" "name"
    ;;

save)
    dth_set "$path/In_System" "$system_code"
    dth_set "$path/In_SaveOrUpdate" "1"
	dth_exec "$path"
    ;;
*)
    echo "invalid access type"
    exit 1
    ;;
esac

