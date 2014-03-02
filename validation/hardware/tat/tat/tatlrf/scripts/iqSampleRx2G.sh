#! /bin/sh

if [ $# -ne 0 ]         
then                    
	echo "$0 RX2G IQ sample read: this command needs no parameter"
	exit 1
fi

root=/mnt/9p/DTH/RF/Rx2G5/IQSample/Start
here=`pwd`

cd $root

cat value

echo "RF 2GRX IQ sample read"
echo "status: "`cat Status/value | dthfilter -r u16`
echo "freq: "`cat Freq/value | dthfilter -r u16`
echo "gain index: "`cat RxGainIndex/value | dthfilter -r u16`
echo "nb data: "`cat AmountOfData/value | dthfilter -r u16`

cd $here
