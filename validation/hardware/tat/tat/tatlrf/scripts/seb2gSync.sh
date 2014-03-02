#! /bin/sh
. ./dthadv.sh ""

# inputs: tester, band, bcch arfcn, tch arfcn, data type, frame number, 
# channel type, codec, TS pattern, mode

#auto set of parameters
dth_set_auto $DTH9P/RF/BER/2G/Sync/In_Tester $1
dth_set_auto $DTH9P/RF/BER/2G/Sync/In_RfBand $2
dth_set_auto $DTH9P/RF/BER/2G/Sync/In_BcchArfcn $3
dth_set_auto $DTH9P/RF/BER/2G/Sync/In_TchArfcn $4
dth_set_auto $DTH9P/RF/BER/2G/Sync/In_TxDataType $5
dth_set_auto $DTH9P/RF/BER/2G/Sync/In_FnSync $6
dth_set_auto $DTH9P/RF/BER/2G/Sync/In_ChannelType $7
dth_set_auto $DTH9P/RF/BER/2G/Sync/In_Codec $8
dth_set_auto $DTH9P/RF/BER/2G/Sync/In_TsPattern $9
dth_set_auto $DTH9P/RF/BER/2G/Sync/In_CodedMode $10

