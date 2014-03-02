#! /bin/sh

cd /tat/tatlsim/tstu
echo "TEST ALL COMMANDS CARD"
./Card_Activation.sh 1
sleep 2
./Card_Deactivation.sh
sleep 2
./Card_Activation.sh 2
sleep 2
./Set_PPS.sh 9 4 0
sleep 2
./Set_Clock_Frequency.sh 1
sleep 2
./Snd_CMD.sh 160 164 0 0 2 0 0 63 0
sleep 2
./Snd_CMD.sh 160 192 0 0 0 34 0 0
sleep 2
./Card_Clock_Stop.sh 1
sleep 2
./Card_Clock_Start.sh
sleep 2
./Snd_CMD.sh 160 164 0 0 2 0 0 63 0
sleep 2
./Card_Deactivation.sh


echo "FINISH"
