#! /bin/sh

if [ $# -ne 2 ]         
then                    
	echo "$0 WCDMA path new_samples"
	echo "path          0=main, 1=diversity"
	echo "new_samples   y or n"
	exit 1
fi

path=$1

root=/mnt/9p/DTH/RF/Rx3G/IQSample/Start
here=`pwd`

cd $root

echo "RF 3GRX IQ sample read"
case $2 in
	y)
		echo "0" | dthfilter -w u32 > IsNewMessage/value
		;;
	n)
		echo "1" | dthfilter -w u32 > IsNewMessage/value
		;;
	*)
		echo "wrong new_samples argument"
		exit 2
		;;
esac

echo "$path" | dthfilter -w u32 > Path/value

cat value

echo "status: "`cat Status/value | dthfilter -r u16`
echo "freq: "`cat Freq/value | dthfilter -r u16`
echo "gain index: "`cat RxGainIndex/value | dthfilter -r u16`
echo "nb data: "`cat AmountOfData/value | dthfilter -r u16`

cd $here
