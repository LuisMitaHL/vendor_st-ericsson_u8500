#!/bin/sh

echo "mount SD Card"
umount /mnt/SDCard
mount /mnt/SDCard 2> tempfile.txt
cat tempfile.txt | cut -f6 -d ' '>$1
rm tempfile.txt

echo "Finished"
exit
