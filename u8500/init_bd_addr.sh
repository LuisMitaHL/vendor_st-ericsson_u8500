#!/bin/sh

# NB: requires busybox

# NB: temporary solution

ADDRFILE=/etc/bd_addr
PARMFILE=/sys/module/ste_conn/parameters/ste_conn_bd_address

if [ ! -w $PARMFILE ] ; then
    echo "Can't find parameter file, module not loaded?";
    exit 1;
fi

if [ -f $ADDRFILE ] ; then
    if cat $ADDRFILE > $PARMFILE ; then
	# success
	exit 0;
    else
	echo "BD-address rejected, trying to generate a new";
    fi
else
    echo "No BD-address found, generating a new";
fi

# use urandom rather than random to protect against lock-up
dd if=/dev/urandom bs=1 count=4 2>/dev/null | hexdump -e '4/1 "0x%02x," "0x80,0x00\n"' >$ADDRFILE;

if cat $ADDRFILE > $PARMFILE ; then
    # success
    exit 0;
else
    echo "Failed to set BD-address";
    exit 1;
fi
