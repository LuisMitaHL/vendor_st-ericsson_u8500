#!/bin/sh
# Author: Guillaume Beauclair, Update: Guillaume Camus

rm /var/log/BB_registers.log

SAVELOG=/var/log/BB_registers.log
echo "*****************************************************************************" | tee -a $SAVELOG
echo "***************************** BaseBand Register *****************************" | tee -a $SAVELOG
echo "*****************************************************************************" | tee -a $SAVELOG
echo " " | tee -a $SAVELOG
export SAVELOG

echo " "
#B2R2 Failed (cannot read)
#. /usr/bin/HATS_validation/BaseBand_registers/BB_registers_B2R2.sh; cat /var/log/BB_registerB2R2.log | tee -a $SAVELOG; export SAVELOG
. /usr/bin/HATS_validation/BaseBand_registers/BB_registers_CLK.sh; cat /var/log/BB_registerCLK.log | tee -a $SAVELOG; export SAVELOG
. /usr/bin/HATS_validation/BaseBand_registers/BB_registers_.sh; cat /var/log/BB_registerCR.log | tee -a $SAVELOG; export SAVELOG
#CRYP0 secured
. /usr/bin/HATS_validation/BaseBand_registers/BB_registers_CRYP.sh; cat /var/log/BB_registerCRYP.log | tee -a $SAVELOG; export SAVELOG
. /usr/bin/HATS_validation/BaseBand_registers/BB_registers_CTI.sh; cat /var/log/BB_registerCTI.log | tee -a $SAVELOG; export SAVELOG
. /usr/bin/HATS_validation/BaseBand_registers/BB_registers_DBAHN.sh; cat /var/log/BB_registerDBAHN.log | tee -a $SAVELOG; export SAVELOG
. /usr/bin/HATS_validation/BaseBand_registers/BB_registers_DMAC.sh; cat /var/log/BB_registerDMAC.log | tee -a $SAVELOG; export SAVELOG
#DSI Failed (cannot read)
#. /usr/bin/HATS_validation/BaseBand_registers/BB_registers_DSI.sh; cat /var/log/BB_registerDSI.log | tee -a $SAVELOG; export SAVELOG
. /usr/bin/HATS_validation/BaseBand_registers/BB_registers_ETB11.sh; cat /var/log/BB_registerETB11.log | tee -a $SAVELOG; export SAVELOG
. /usr/bin/HATS_validation/BaseBand_registers/BB_registers_FSMC.sh; cat /var/log/BB_registerFSMC.log | tee -a $SAVELOG; export SAVELOG
. /usr/bin/HATS_validation/BaseBand_registers/BB_registers_GIC.sh; cat /var/log/BB_registerGIC.log | tee -a $SAVELOG; export SAVELOG
. /usr/bin/HATS_validation/BaseBand_registers/BB_registers_GPIO.sh; cat /var/log/BB_registerGPIO.log | tee -a $SAVELOG; export SAVELOG
#HASH0 secured
. /usr/bin/HATS_validation/BaseBand_registers/BB_registers_HASH.sh; cat /var/log/BB_registerHASH.log | tee -a $SAVELOG; export SAVELOG
. /usr/bin/HATS_validation/BaseBand_registers/BB_registers_HSEM.sh; cat /var/log/BB_registerHSEM.log | tee -a $SAVELOG; export SAVELOG
. /usr/bin/HATS_validation/BaseBand_registers/BB_registers_HSI2C.sh; cat /var/log/BB_registerHSI2C.log | tee -a $SAVELOG; export SAVELOG
. /usr/bin/HATS_validation/BaseBand_registers/BB_registers_HSIR.sh; cat /var/log/BB_registerHSIR.log | tee -a $SAVELOG; export SAVELOG
. /usr/bin/HATS_validation/BaseBand_registers/BB_registers_HSIT.sh; cat /var/log/BB_registerHSIT.log | tee -a $SAVELOG; export SAVELOG
. /usr/bin/HATS_validation/BaseBand_registers/BB_registers_ICN.sh; cat /var/log/BB_registerICN.log | tee -a $SAVELOG; export SAVELOG
#MCDE Failed (cannot read)
#. /usr/bin/HATS_validation/BaseBand_registers/BB_registers_MCDE.sh; cat /var/log/BB_registerMCDE.log | tee -a $SAVELOG; export SAVELOG
. /usr/bin/HATS_validation/BaseBand_registers/BB_registers_MCSBAG.sh; cat /var/log/BB_registerMCSBAG.log | tee -a $SAVELOG; export SAVELOG
. /usr/bin/HATS_validation/BaseBand_registers/BB_registers_MMCI.sh; cat /var/log/BB_registerMMCI.log | tee -a $SAVELOG; export SAVELOG
. /usr/bin/HATS_validation/BaseBand_registers/BB_registers_MSP.sh; cat /var/log/BB_registerMSP.log | tee -a $SAVELOG; export SAVELOG
. /usr/bin/HATS_validation/BaseBand_registers/BB_registers_MSSCSTF.sh; cat /var/log/BB_registerMSSCSTF.log | tee -a $SAVELOG; export SAVELOG
. /usr/bin/HATS_validation/BaseBand_registers/BB_registers_MSSCTI.sh; cat /var/log/BB_registerMSSCTI.log | tee -a $SAVELOG; export SAVELOG
. /usr/bin/HATS_validation/BaseBand_registers/BB_registers_MSSETB11.sh; cat /var/log/BB_registerMSSETB11.log | tee -a $SAVELOG; export SAVELOG
. /usr/bin/HATS_validation/BaseBand_registers/BB_registers_MSSETM11CS.sh; cat /var/log/BB_registerMSSETM11CS.log | tee -a $SAVELOG; export SAVELOG
. /usr/bin/HATS_validation/BaseBand_registers/BB_registers_MSSTPIU.sh; cat /var/log/BB_registerMSSTPIU.log | tee -a $SAVELOG; export SAVELOG
#PKA secured
#. /usr/bin/HATS_validation/BaseBand_registers/BB_registers_PKA.sh; cat /var/log/BB_registerPKA.log | tee -a $SAVELOG; export SAVELOG
. /usr/bin/HATS_validation/BaseBand_registers/BB_registers_PL310.sh; cat /var/log/BB_registerPL310.log | tee -a $SAVELOG; export SAVELOG
. /usr/bin/HATS_validation/BaseBand_registers/BB_registers_PRCMU.sh; cat /var/log/BB_registerPRCMU.log | tee -a $SAVELOG; export SAVELOG
. /usr/bin/HATS_validation/BaseBand_registers/BB_registers_PTM.sh; cat /var/log/BB_registerPTM.log | tee -a $SAVELOG; export SAVELOG
. /usr/bin/HATS_validation/BaseBand_registers/BB_registers_PWL.sh; cat /var/log/BB_registerPWL.log | tee -a $SAVELOG; export SAVELOG
. /usr/bin/HATS_validation/BaseBand_registers/BB_registers_RNG.sh; cat /var/log/BB_registerRNG.log | tee -a $SAVELOG; export SAVELOG
. /usr/bin/HATS_validation/BaseBand_registers/BB_registers_RTC.sh; cat /var/log/BB_registerRTC.log | tee -a $SAVELOG; export SAVELOG
#RTT secured
#. /usr/bin/HATS_validation/BaseBand_registers/BB_registers_RTT.sh; cat /var/log/BB_registerRTT.log | tee -a $SAVELOG; export SAVELOG
#SCR secured
#. /usr/bin/HATS_validation/BaseBand_registers/BB_registers_SCR.sh; cat /var/log/BB_registerSCR.log | tee -a $SAVELOG; export SAVELOG
. /usr/bin/HATS_validation/BaseBand_registers/BB_registers_SCU.sh; cat /var/log/BB_registerSCU.log | tee -a $SAVELOG; export SAVELOG
. /usr/bin/HATS_validation/BaseBand_registers/BB_registers_SKE.sh; cat /var/log/BB_registerSKE.log | tee -a $SAVELOG; export SAVELOG
. /usr/bin/HATS_validation/BaseBand_registers/BB_registers_SLIMBUS.sh; cat /var/log/BB_registerSLIMBUS.log | tee -a $SAVELOG; export SAVELOG
. /usr/bin/HATS_validation/BaseBand_registers/BB_registers_SPI.sh; cat /var/log/BB_registerSPI.log | tee -a $SAVELOG; export SAVELOG
. /usr/bin/HATS_validation/BaseBand_registers/BB_registers_SSP.sh; cat /var/log/BB_registerSSP.log | tee -a $SAVELOG; export SAVELOG
. /usr/bin/HATS_validation/BaseBand_registers/BB_registers_STM.sh; cat /var/log/BB_registerSTM.log | tee -a $SAVELOG; export SAVELOG
. /usr/bin/HATS_validation/BaseBand_registers/BB_registers_TIMER.sh; cat /var/log/BB_registerTIMER.log | tee -a $SAVELOG; export SAVELOG
. /usr/bin/HATS_validation/BaseBand_registers/BB_registers_TPIU.sh; cat /var/log/BB_registerTPIU.log | tee -a $SAVELOG; export SAVELOG
. /usr/bin/HATS_validation/BaseBand_registers/BB_registers_UART.sh; cat /var/log/BB_registerUART.log | tee -a $SAVELOG; export SAVELOG
. /usr/bin/HATS_validation/BaseBand_registers/BB_registers_USB.sh; cat /var/log/BB_registerUSB.log | tee -a $SAVELOG; export SAVELOG
. /usr/bin/HATS_validation/BaseBand_registers/BB_registers_USBOTGM.sh; cat /var/log/BB_registerUSBOTGM.log | tee -a $SAVELOG; export SAVELOG

echo " "
echo "*****************************************************************************" | tee -a $SAVELOG
echo "****************************** BaseBand Register ****************************"
echo "********************************** Test End *********************************"
echo "*****************************************************************************" | tee -a $SAVELOG
echo "********* Tests relsuts have been saved in /var/log/BB_registers.log ********"
export LOGFILE