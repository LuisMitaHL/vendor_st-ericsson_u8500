#!/bin/sh

ram=esram
#ram=sdram

echo "#define  DELIVERY_DFM_FTR"                       >> $1include/config.h
#echo "#define  DELIVERY_VALID_FTR"                    >> $1include/config.h
echo "#define  TERMINAL_UART_FTR"                      >> $1include/config.h
#echo "#define  TERMINAL_JTAG_FTR"                     >> $1include/config.h
echo "#define  DB8500_HDW 30"                          >> $1include/config.h
echo "#define  AB8500_HDW 33"                          >> $1include/config.h
echo "#define  RF_HDW 30"                              >> $1include/config.h
echo "#define  GCAP"                                   >> $1include/config.h
#echo "#define  LION"                                  >> $1include/config.h
echo "#define  BATSEL_VBAT"                            >> $1include/config.h
#echo "#define  BATSEL_GND"                            >> $1include/config.h
echo "#define  HREF_PLUS_22_10_BOARD"                  >> $1include/config.h
#echo "#define  HREF_PLUS_20_60_BOARD"                 >> $1include/config.h
echo "#define  I2CAPE"                                 >> $1include/config.h
#echo "#define  SSP0"                                  >> $1include/config.h
echo "#define  PRCMU_I2CAPE_REGULAR"                   >> $1include/config.h
#echo "#define  PRCMU_I2CAPE_WORKAROUND"               >> $1include/config.h
#echo "#define  PRCMU_NO"                              >> $1include/config.h
#echo "#define  C1V2D_PIN_CONNECTED"                   >> $1include/config.h
echo "#define  MICANA1_OFF"                            >> $1include/config.h
#echo "#define  MICANA1_ON"                            >> $1include/config.h
#echo "#define  MICANA1_MOUNTED"                       >> $1include/config.h

if [ $ram = "esram" ]
then echo "#define  TEXT_BASE 0x40016000"              >> $1include/config.h
echo "ESRAM = TRUE"                                      > $1board/st-ericsson/u8500/config.tmp
echo "TEXT_BASE = 0x40016000"                          >> $1board/st-ericsson/u8500/config.tmp
else echo "#define  TEXT_BASE 0x05608000"               >> $1include/config.h
echo "TEXT_BASE = 0x05608000"                          >> $1board/st-ericsson/u8500/config.tmp
fi