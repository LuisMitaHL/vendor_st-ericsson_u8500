#ifndef NUMERICS_H
#define NUMERICS_H

/*----------------------------------------------------------------------------*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *----------------------------------------------------------------------------*
 * System      : STxP70 Processor                                             *
 * File Name   : numerics.h                                                   *
 * Purpose     : typedef definition                                           *
 * Author(s)   : Didier MOULIN                                                *
 *               Helder DA SILVA VALENTE                                      *
 * History     : 2009/10/26 - First draft implementation. (Helder)            *
 *----------------------------------------------------------------------------*/

/* general purpose On/Off states */
typedef enum {
  OffStat = 0,
  OnStat  = 1
} t_OnOffSt;

/* 8500 Ap power states */
typedef enum {
  NoPwrSt	= 0x00,
  ApBoot	= 0x01,
  ApExecute	= 0x02,
  ApDeepSleep	= 0x03,
  ApSleep	= 0x04,
  ApIdle	= 0x05,
  ApReset	= 0x06,
  ApDeepIdle	= 0x07
} t_ApPwrSt;

/* Header for ReqMb0 */
typedef enum {
  PwrStTrH		= 0,
  CfgWkUpsHExe		= 1,
  RdWkUpAckH		= 3,
  CfgWkUpsHSleep	= 4,
  DummyService		= 5
} t_ReqMb0H;

/* power state transitions */
/* 2do:DM: values changed */
typedef enum {
  NoTransition			= 0,
  ApExecuteToApSleep		= 1,
  ApIdleToApSleep		= 2,
  ApBootToApExecute		= 3,
  ApExecuteToApDeepSleep	= 4,
  ApExecuteToApIdle		= 5,
  ApExecuteToApDeepIdle		= 7,
  RomCode_Patch			= 8
} t_ApPwrStTr;

/* Header for ReqMb1 */
typedef enum {
  ArmApeOppH            = 0x00,
  ArmApeRestore100OppH  = 0x01,
  ResetModemH		= 0x02,
  Ape100OPPVoltRequestH = 0x03,
  Ape100OPPVoltReleaseH = 0x04,
  UsbWakeupReleaseH     = 0x05
} t_ReqMb1H;

/* Header for ReqMb2 */
typedef enum {
  dpsH     = 0x00,
  autoPwrH = 0x01
} t_ReqMb2H;

/* Header for ReqMb3  */
typedef enum {
  AncH      = 0x00,
  SidetoneH = 0x01,
  sysClkH   = 0x0E,
} t_ReqMb3H;

/* Header for ReqMb4 */
typedef enum {
  ddrInitH		= 0x00,
  memStH		= 0x01,
  autoSrefreshH		= 0x02,
  autoPwrDnH		= 0x03,
  partialSrefreshCS0H	= 0x04,
  partialSrefreshCS1H	= 0x05,
  csPwrdnH		= 0x06,
  SrefreshCS0H		= 0x07,
  SrefreshCS1H		= 0x08,
  SrefreshCSiStatusH	= 0x09,
  QoSPort0H		= 0x0A,
  QoSPort1H		= 0x0B,
  QoSPort2H		= 0x0C,
  QoSPort3H		= 0x0D,
  workAround2H		= 0x0F,
  AvsH                  = 0x10,
  primWDsettingsH       = 0x11,
  ThSensHotDogH         = 0x12,
  ThSensHotMonH         = 0x13,
  ThSensSetMeasurePeriodH = 0x14,
  ddrPreInitH             = 0x15,
} t_ReqMb4H;

/* Header for ReqMb6  */
typedef enum {
  initReqMb6H       = 0x00,
} t_ReqMb6H;

/* Header for ReqMb7  */
typedef enum {
  SleepBySIAMMDSPH	= 0x00,
  Fifo4500ItCfgH	= 0x01,
  GenI2cH		= 0x02
} t_ReqMb7H;

typedef enum {
  I2CWrite = 0,
  I2CRead  = 1
} t_I2CopType;

typedef enum {
  AckMb4Init		= 0,
  DDRFirstStarting_Ok   = 1,
  DDROn_Ok		= 2,
  partialSrefreshCS0_Ok = 3,
  partialSrefreshCS1_Ok = 4,
  SrefreshCS0_Ok	= 5,
  SrefreshCS1_Ok	= 6,
  SrefreshCSiStatus_OK	= 7,
  QoSPort0_Ok		= 8,
  QoSPort1_Ok		= 9,
  QoSPort2_Ok		= 10,
  QoSPort3_Ok		= 11,
  Avs_Ok		= 12,
  ThSens_Ok		= 13,
  csPwrdn_Ok            = 14,
  AvsReqInBadPowerState_Er      = 0x80,
  AvsI2cAccessFail_Er           = 0x81,
  ThSensParamOutofRange_Er      = 0x82,
  ThSensBadParameter_Er         = 0x83,
  UnknownService_Er             = 0xFF
} t_AckMb4Status;

/* ARM OPP STATES */
typedef enum {
  ArmOppInit= 0x00,
  ArmNoChg	= 0x01,
  Arm100Opp	= 0x02,
  Arm50Opp	= 0x03,
  ArmMaxOpp = 0x04,
  VArmMax_Freq100Opp= 0x05,
  ArmExtClk	= 0x07
} t_ArmOpp;

/* APE OPP STATES */
typedef enum {
  ApeOppInit= 0x00,
  ApeNoChg	= 0x01,
  Ape100Opp	= 0x02,
  Ape50Opp	= 0x03
} t_ApeOpp;

typedef enum {
	AckMb5Init  = 0x00,
	I2C_Wr_Ok   = 0x01,
	I2C_Rd_Ok   = 0x02,
	SysClk_Ok   = 0x03,
	I2C_TimeOut = 0x11,
	SysClk_Er   = 0x12,
	/* Error Status resent by PRCM_HWI2C_SR */
	/* PRCM_HWI2C_SR(4:2)="001", PRCM_HWI2C_SR(1:0)="11" */
	I2cWr_NAck_Data_Er			= 0x07,
	/* PRCM_HWI2C_SR(4:2)="010", PRCM_HWI2C_SR(1:0)="11" */
	I2cWr_NAck_Reg_Addr_Er		= 0x0B,
	/* PRCM_HWI2C_SR(4:2)="110", PRCM_HWI2C_SR(1:0)="11" */
	I2cRdWr_NAck_Data0123_Er		= 0x1B,
	/* PRCM_HWI2C_SR(4:2)="111", PRCM_HWI2C_SR(1:0)="11" */
	I2cWr_NAck_Addr_Er			= 0x1F,
	/* PRCM_HWI2C_SR(4:2)="011", PRCM_HWI2C_SR(1:0)="11" */
	I2cRd_NAck_Addr_Init_Er		= 0x0F,
	/* PRCM_HWI2C_SR(4:2)="100", PRCM_HWI2C_SR(1:0)="11" */
	I2cRd_NAck_Reg_Addr_Init_Er	= 0x13,
	/* PRCM_HWI2C_SR(4:2)="101", PRCM_HWI2C_SR(1:0)="11" */
	I2cRd_NAck_Addr_Er			= 0x17
} t_AckMb5Status;

//---------------------------- Header for MbPingPong
typedef enum {
  WkUpHExe       	= 2,
  WkUpHSleep       	= 5
} t_MbPingPongH;

typedef enum {
  VarmMaxOpp_disable		= 0x9,
  DVFSOk			= 0xA,
  DVFSReqMb1Err			= 0xB,
//  A9DVFSOk			= 0x0A,
//  A9DVFSReqMb1Err		= 0x01,
//  APEDVFSOk			= 0xA0,
//  APEDVFSReqMb1Err		= 0x10,
  InitDvfsStatus		= 0x0
} t_DvfsStatus;

typedef enum {
    ApeVoltageSuccess =   0,
    ApeVoltageFail      = 1
} t_ape_voltage_status;

#endif /* !defined(NUMERICS_H) */
