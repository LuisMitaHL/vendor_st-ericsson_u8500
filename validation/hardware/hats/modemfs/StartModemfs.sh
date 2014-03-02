#!/bin/sh
HATS_MODEMFS=$1
MCE_FILEMANE_PATH="/CONF/MCE"
MCE_FILEMANE=MCE.CFG
	# create initial modem file system
	if [ ! -d $HATS_MODEMFS ]; then
		mkdir -p  $HATS_MODEMFS
    	fi
	################################################
    	#Preparation of the Filesystem
    	################################################
	# cp -a {} modem provided by nokia 
	#Building the modem file system tree
	rm -rf $HATS_MODEMFS/*
	mkdir -p $HATS_MODEMFS/APPL
	mkdir -p $HATS_MODEMFS/BOOT
	mkdir -p $HATS_MODEMFS/CONF
	#	mkdir $(MODEM_FS_ROOT)/CONF/MCE
	#		cp conf/MCE.CFG $(MODEM_FS_ROOT)/CONF/MCE
	mkdir -p $HATS_MODEMFS/DO
	mkdir -p $HATS_MODEMFS/LOG
	mkdir -p $HATS_MODEMFS/PERM
	mkdir -p $HATS_MODEMFS/PERM/PLAIN
	mkdir -p $HATS_MODEMFS/PERM/PROT
	mkdir -p $HATS_MODEMFS/TMP
	mkdir -p $HATS_MODEMFS/RFHAL
	mkdir -p $HATS_MODEMFS/SECURE
	# create Secure directory in RootFS if not existing
	cp -rf ./conf/* $HATS_MODEMFS/SECURE
	#create MCE.CFG file
	if [ ! -d  $HATS_MODEMFS/$MCE_FILEMANE_PATH/$MCE_FILEMANE ]; then 
		mkdir -p $HATS_MODEMFS/$MCE_FILEMANE_PATH
		chmod 777 $HATS_MODEMFS/$MCE_FILEMANE_PATH
		cp -f ../modemservices/modem_lib/$MCE_FILEMANE $HATS_MODEMFS/$MCE_FILEMANE_PATH
		chmod 777 $HATS_MODEMFS/$MCE_FILEMANE_PATH/$MCE_FILEMANE
	fi
	if [ -d filesystem/CONF/MCE ];then
		cp filesystem/CONF/MCE/* $HATS_MODEMFS/$MCE_FILEMANE_PATH
	fi
	chmod 777 -R $HATS_MODEMFS
