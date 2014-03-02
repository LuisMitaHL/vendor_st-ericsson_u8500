#! /bin/sh

if [ $# -le 7 ]         
then                    
	echo "arg0 : CLASS"
	echo "arg1 : INS"
	echo "arg2 : P1" 
	echo "arg3 : P2"
	echo "arg4 : LC"
	echo "arg5 arg6: LeLSB LeMSB"
	echo "arg7 to argn => data: data to write in the DATA to send to SIM."
exit
fi

rm /tat/tatlsim/toto
nbarg=$5
echo "Test Command APDU"

echo $1 | dthfilter -w u8 > /mnt/DTH/SMC/Snd_Cmd/SendAPDU/Class/value
echo $2 | dthfilter -w u8 > /mnt/DTH/SMC/Snd_Cmd/SendAPDU/Instruction/value
echo $3 | dthfilter -w u8 > /mnt/DTH/SMC/Snd_Cmd/SendAPDU/Parameter1/value
echo $4 | dthfilter -w u8 > /mnt/DTH/SMC/Snd_Cmd/SendAPDU/Parameter2/value
echo $5 | dthfilter -w u8 > /mnt/DTH/SMC/Snd_Cmd/SendAPDU/Lc/value
echo $6 | dthfilter -w u8 > /mnt/DTH/SMC/Snd_Cmd/SendAPDU/LeLSB/value
echo $7 | dthfilter -w u8 > /mnt/DTH/SMC/Snd_Cmd/SendAPDU/LeMSB/value

shift 7
for i in $*
do
echo $i
echo $i | dthfilter -w u8 >> /tat/tatlsim/toto
done


for i in $(seq $nbarg 1 255)
do
echo 255 | dthfilter -w u8 >> /tat/tatlsim/toto
done

cat /tat/tatlsim/toto > /mnt/DTH/SMC/Snd_Cmd/SendAPDU/Data/value

cat /mnt/DTH/SMC/Snd_Cmd/SendAPDU/value

echo "END TEST"
