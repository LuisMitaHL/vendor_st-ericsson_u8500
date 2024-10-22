#!/bin/sh

# Creates a symlink to appropriate modules
KERNELRELEASE_FULL=`cat $KERNEL_OUTPUT/include/config/kernel.release`
KERNELRELEASE_STRIPPED=`echo $KERNELRELEASE_FULL | sed "s/-.*$//"`
MODPATH_FULL=$INSTALL_MOD_PATH/lib/modules/$KERNELRELEASE_FULL
MODPATH_STRIPPED=$INSTALL_MOD_PATH/lib/modules/$KERNELRELEASE_STRIPPED

if [ $MODPATH_STRIPPED != $MODPATH_FULL ] ; then
	if [ -d $MODPATH_STRIPPED -a ! -L $MODPATH_STRIPPED ] ; then
		cp -r $MODPATH_STRIPPED/* $MODPATH_FULL
		rm -rf $MODPATH_STRIPPED
	else
		rm -f $MODPATH_STRIPPED
	fi

	ln -s $KERNELRELEASE_FULL $INSTALL_MOD_PATH/lib/modules/$KERNELRELEASE_STRIPPED
fi

rm -f $INSTALL_MOD_PATH/lib/modules/$KERNELRELEASE_FULL/build
rm -f $INSTALL_MOD_PATH/lib/modules/$KERNELRELEASE_FULL/source
