#! /bin/sh
. ./dthadv.sh ""

#inputs: tester, band, arfcn, ovsf, scrambling UL, pri scrambling, frame number,
#data type, tx level

#auto set of parameters
dth_set_auto $DTH9P/RF/BER/3G/Sync/In_Tester $1
dth_set_auto $DTH9P/RF/BER/3G/Sync/In_Band $2
dth_set_auto $DTH9P/RF/BER/3G/Sync/In_Arfcn $3
dth_set_auto $DTH9P/RF/BER/3G/Sync/In_Ovsf $4
dth_set_auto $DTH9P/RF/BER/3G/Sync/In_UlScramblingCode $5
dth_set_auto $DTH9P/RF/BER/3G/Sync/In_PriScramblingCode $6
dth_set_auto $DTH9P/RF/BER/3G/Sync/In_FrameNo $7
dth_set_auto $DTH9P/RF/BER/3G/Sync/In_DataType $8
dth_set_auto $DTH9P/RF/BER/3G/Sync/In_TxLevel $9

