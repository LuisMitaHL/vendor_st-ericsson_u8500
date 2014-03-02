#! /bin/sh
#	Copyright (C) ST-Ericsson 2011
#	This script executes HATS RF regression tests related to PMM access.
#	It writes and reads values for a set of RF parameters, not for all
#	parameters.
#	Author:	alain.crouzet-nonst@stericsson.com

# make use of dthadv.sh
. ./dthadv.sh


# TEST INIT

local PMMACCESS=$DTH9P/RF/Parameters/MemoryMgt/Action/Memory_access

# set mode WRITE else we may be enable to write data to PMM
dth_get $PMMACCESS
local OLDPMMACCESS=$VAR_GET_RESULT
dth_set $PMMACCESS 1


# TEST CORE

# scalar U8
# no match

# scalar S8
# no match

# scalar U16
./testdtha.sh "RF/Param/2G_Transmitter/Band/GMSK/GSM900/GMSK_TX_EER_PHASE_SHIFT/GSM900_GERAN_GSM_PHASESHIFT"

# scalar S16
./testdtha.sh "RF/Param/2G_Transmitter/Band/GMSK/GSM1800/GMSK_PA_COMPRESSION/GSM1800_GERAN_GSM_PA_COMPRESSION"

# scalar U32
# no match

# scalar S32
./testdtha.sh "RF/Param/3G_Transmitter/Fix_Parameters/IQ/slope_factors/UTRAN_SLOPE_FACTOR_A_HIGH"

# scalar U64
# no match

# scalar S64
# no match


# array U8 (DTH but coded U16 in PMM)
./testdtha.sh "RF/Param/RfcConfig/edge_tx/900/RF_CTRL_GSM_EDGE_TX_CONFIGURATION"

# array S8
# no match

# array U16
./testdtha.sh "RF/Param/2G_Receiver/Band/GSM1900/RX_gain/GSM1900_GERAN_RX_GAIN_FREQ_COMPENSATION_CHANNEL_TABLE"

# array S16
./testdtha.sh "RF/Param/2G_Receiver/Band/GSM1800/RX_gain/GSM1800_GERAN_RX_GAIN_FREQ_COMPENSATION_TABLE"

# array U32
# no match

# array S32
# no match

# array U64
# no match

# array S64
# no match


# END OF TEST

# restore PMM access mode
dth_set $PMMACCESS $OLDPMMACCESS

# END OF FILE
