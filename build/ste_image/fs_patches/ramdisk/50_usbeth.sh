#!/bin/sh

# Add g_cdc module to modules.alias (if necessary)
for i in `find $INSTALL_MOD_PATH/lib/modules -name modules.alias`; do
	if ! grep -q usb0 "$i"; then
		echo "Patching $i"
		echo "alias usb0 g_cdc" >> "$i"
	fi
done

