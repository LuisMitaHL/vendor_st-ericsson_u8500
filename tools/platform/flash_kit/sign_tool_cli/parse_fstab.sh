#!/bin/bash

FSTAB="$1"

function print_toc()
{
	echo "write_toc_partition(package_extract_file(\"${1}\"), \"${2}\");"
}

function print_emmc()
{
	echo "package_extract_file(\"${1}\", \"${2}\");"
}

if [ ! -f "$FSTAB" ]; then
	echo "Cannot find fstab: $FSTAB" >&2
	exit 1
fi

while read MP TYPE DEV DEV2; do
	IMAGE=""
	case "$MP" in
		/modem)
			IMAGE=device/st-ericsson/modem.fs
			continue;
		;;
		/modemfs)
			IMAGE=device/st-ericsson/modemfs.img
			continue;
		;;
		/data)
			echo "mount(\"ext4\", \"EMMC\", \"$DEV\", \"/data\");"
		;;
	esac
	[ -z "$IMAGE" ] && continue
	case "$TYPE" in
		toc)
			print_toc "$IMAGE" "$DEV"
		;;
		emmc)
			print_emmc "$IMAGE" "$DEV"
		;;
	esac
done < "$FSTAB"

echo 'delete("/data/misc/cns.nvd", "/data/adm.sqlite", "/system/fw.flash");'
echo 'unmount("/data");'
