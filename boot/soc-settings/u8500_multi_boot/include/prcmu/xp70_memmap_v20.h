#ifndef XP70_MEMMAP_H
#define XP70_MEMMAP_H

/*----------------------------------------------------------------------------*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *----------------------------------------------------------------------------*
 * System      : STxP70 Processor                                             *
 * File Name   : xp70_MemMap.h                                                *
 * Purpose     : defines the TCDM and TCPM memory mapping of the xp70         *
 * Author(s)   : Didier MOULIN                                                *
 *               Helder DA SILVA VALENTE                                      *
 * History     : 2009/10/26 - First draft implementation.  (Helder)           *
 *----------------------------------------------------------------------------*/

#include "numerics_v20.h"

/* definition of base addresses */
#define TCDM_BASE             0x801B8000 /* B4 (in spec) */

/* In order to optimize memory space the headers are grouped. */
typedef struct {

    union {
        struct {
            t_ReqMb0H     Mb0 : 8;              /**< Request mailbox 0 */
            t_ReqMb1H     Mb1 : 8;              /**< Request mailbox 1 */
            t_ReqMb2H     Mb2 : 8;              /**< Request mailbox 2 */
            t_ReqMb3H     Mb3 : 8;              /**< Request mailbox 3 */
            t_ReqMb4H     Mb4 : 8;              /**< Request mailbox 4 */
            unsigned char Mb5 : 8;              /**< Request mailbox 5 */
            unsigned char bitFill1 : 8;         /**< Reserved */
            unsigned char bitFill2 : 8;         /**< Reserved */
        } un;                                   /**< To access request mailboxes through typed fields */
        unsigned char Mb[8];                    /**< To access request mailboxes through an array of char */
    } Req;                                      /**< Union permitting to access request mailboxes either through an array or through typed fields */

    union {
        struct {
            t_MbPingPongH     MbPingPong : 8;   /**< Acknowledge header used during ping/pong mechanism */
            unsigned char     bitFill3   : 8;   /**< Reserved */
            unsigned char     bitFill4   : 8;   /**< Reserved */
            unsigned char     bitFill5   : 8;   /**< Reserved */
        } un;                                   /**< To access acknowledge header mailboxes through typed fields */
        unsigned char Mb[4];                    /**< To access acknowledge header mailboxes through an array of char */
    } Ack;                                      /**< Union permitting to access acknoledge header mailboxes either through an array or through typed fields */

    union {
        struct {
            t_ReqMb6H     	  Mb6        : 8;  /**< Request mailbox 6 */
            t_ReqMb7H     	  Mb7        : 8;  /**< Request mailbox 7 */
        } un;                                   /**< To access DSP request mailboxes through typed fields */
        unsigned short Mb[2];                   /**< To access DSP request mailboxes through an array of char */
    } ReqDsp;                                   /**< Union permitting to access DSP request mailboxes either through an array or through typed fields */

} t_Header;

typedef struct {
    t_ApPwrSt     pwr_current_state_var : 8;
    unsigned char byteFill[2];
    unsigned char boot_status_debug_var;
} t_ShVar1;

/*------------------------------------------------------------------------*
 * DDRInit
 *
 *          type : Shared Variable
 *          associated IT : prcm_mbox_cpu_set[4] -> xp70 IT 16
 *          communication direction : arm (Wr) -> prcmu (Rd)
 *          width (Bytes) : 464
 *          multiservice (using header) : n/a
 *          service(s) : - Initialization of DDR Controller and PHY
 *		- The three Msb of MaskCfg0[4] are used to store the
 *			DDR size value
*------------------------------------------------------------------------*/
typedef struct {
    unsigned int MaskCfg0[5];   /**< Used to not configure masked DDR controler registers */
    unsigned int Cfg0[111];     /**< DDR controler register value config0 */
    unsigned int Cfg1;          /**< DDR controler register value config1*/
} t_DDRInit;

/*------------------------------------------------------------------------*
 * Adaptive Voltage Scaling (AVS) data
 *
 *          type : Shared Variable
 *          associated IT : prcm_mbox_cpu_set[4] -> xp70 IT 16
 *          communication direction : voltage tables : bias, comp, dflt :
 *                                                     arm (Wr) -> prcmu (Rd)
 *                                    status field : stat, pend :
 *                                                   prcmu(Rd/Wr) -> arm(Rd)
 *                                                   (arm potential read)
 *          width (Bytes) :
 *          multiservice (using header) : n/a
 *          service(s) : - pass compensated or default voltage values from the
 *                         ARM to the prcmu and update of status field by prcmu
*------------------------------------------------------------------------*/
#define AVS_STRUCT_SIZE_BYTES   14

/*------------------------------------------------------------------------*
 * index of different voltages to be used when accessing
 *AVSData structure as a t_uint8 tab
*------------------------------------------------------------------------*/
#define AVS_VBB_RET           0
#define AVS_VBB_MAX_OPP       1
#define AVS_VBB_100_OPP       2
#define AVS_VBB_50_OPP        3
#define AVS_VARM_MAX_OPP      4
#define AVS_VARM_100_OPP      5
#define AVS_VARM_50_OPP       6
#define AVS_VARM_RET          7
#define AVS_VAPE_100_OPP      8
#define AVS_VAPE_50_OPP       9
#define AVS_VMOD_100_OPP      10
#define AVS_VMOD_50_OPP       11
#define AVS_VSAFE             12
#define AVS_VSAFE_RET	      13

/* voltage supply. bit 6 and 7 are only relevant for vARM max OPP */
/* IsSlowStartup : copy of the OTP_VarmVapeSlowStartup fuse. */
/* IsModeEnabled : ARM MAX opp is enabled when this bit is set. */
typedef struct {
	char Voltage:6;
	char IsSlowStartup:1;
	char IsModeEnabled:1; /* only usefull for ARM OPP0 (max OPP) */
} t_VoltSupply;

/* bias voltages in signed format in order to deal arithmetically  */
/* with the voltage values */
typedef struct {
	signed char Vbbn:4;
	signed char Vbbp:4;
} t_VoltBB;

/* Structure which holds the data relative to the AVS. */
/* Data may be accessed via Voltage[] tab or using the Fields structure. */
typedef union {
    struct {
	t_VoltBB VbbRet;
	t_VoltBB VbbMaxOpp;
	t_VoltBB Vbb100Opp;
	t_VoltBB Vbb50Opp;
	t_VoltSupply VarmMaxOPP;
	t_VoltSupply Varm100OPP;
	t_VoltSupply Varm50OPP;
	t_VoltSupply VarmRetOPP;
	t_VoltSupply Vape100OPP;
	t_VoltSupply Vape50OPP;
	t_VoltSupply Vmod100OPP;
	t_VoltSupply Vmod50OPP;
	t_VoltSupply VSafe;
	t_VoltSupply VSafeRet;
    } Fields;
    char Voltage[AVS_STRUCT_SIZE_BYTES];
} t_AVSData;

/*------------------------------------------------------------------------*
* If not otherwise mentionned (by <HwEvent>_fall) only rising edge is
* taken into consideration. Alignement is done with PRCM_HOLD_EVT register.
* Format :
*  <type> <HwEvent> : <nb of bits> ;  <position in CompletField>
*                                       <HwEvent assoc. status register>
* '0' : Hw Event is masked
* '1' : Hw Event is unmasked
*------------------------------------------------------------------------*/
/* List of the 8500 relevant Hw events to be used as mask for wake-up config. */
typedef union {
/**
    If not otherwise mentionned (by <HwEvent>_fall) only rising edge is
    taken into consideration. Alignement is done with PRCM_HOLD_EVT register.\n
    Format :\n
     - <type> <HwEvent> : <nb of bits> ;  <position in CompletField>  <HwEvent assoc. status register>
     - '0' : Hw Event is masked
     - '1' : Hw Event is unmasked
*/
    struct {
        unsigned int RTC               : 1; /**< b[0] : PRCM_STATUS0[0] */
        unsigned int RTT0              : 1; /**< b[1] : PRCM_STATUS0[1] */
        unsigned int RTT1              : 1; /**< b[2] : PRCM_STATUS0[2] */
        unsigned int HSI0              : 1; /**< b[3] : PRCM_STATUS0[3] */
        unsigned int HSI1              : 1; /**< b[4] : PRCM_STATUS0[4] */
        unsigned int cawakereq         : 1; /**< b[5] : PRCM_STATUS0[5] note: in some doc 'cawakereq' is also denoted as 'MOD_WAKE_HOST_IRQ' */
        unsigned int USBWkup           : 1; /**< b[6] : PRCM_STATUS0[6] */
        unsigned int ape_int_4500      : 1; /**< b[7] : PRCM_STATUS0[7] */
        unsigned int fifo4500it        : 1; /**< b[8] : PRCM_STATUS1[0] */
        unsigned int SysClkOk          : 1; /**< b[9] : Ack from ReqMb3(sysClkH) */
        unsigned int CaSleepReq        : 1; /**< b[10]: Forward of falling edge of MOD_WAKE_HOST_IRQ signal */
        unsigned int AcWakeAck         : 1; /**< b[11]: Forward of rising edge of MOD_HOST_PORT_AVAILABLE signal */
        unsigned int SideToneOk        : 1; /**< b[12]: Forward Side tone OK */
        unsigned int AncOk             : 1; /**< b[13]: Forward ANC OK */
        unsigned int SwErr             : 1; /**< b[14]: Forward Software error */
        unsigned int AcSleepAck        : 1; /**< b[15]: Forward of falling edge of MOD_HOST_PORT_AVAILABLE signal */
        unsigned int bitFill2          : 1; /**< b[16]: Reserved */

        // ITs from arm enabled by ARMit_maskxp70_it
        // at position of swreset in PRCM_HOLD_EVT register
        unsigned int ARM_it_mgt        : 1; /**< b[17]: No status bit available */

        unsigned int HotMonL           : 1; /**< b[18]: Temperature lower than the min specified */
        unsigned int HotMonH           : 1; /**< b[19]: Temperature greater than the max specified */

        unsigned int mod_sw_reset_req  : 1; /**< b[20]: PRCM_STATUS4[3] */

        unsigned int bitFill3          : 2; /**< b[21:22]: Reserved */

        unsigned int gpio              : 9; /**< b[31:23]: PRCM_STATUS5[8:0] */

    } event;                                /**< asynchronous event description */

    unsigned int CompletField;              /**< To access HW event through 32bit variable */

} t_8500HwEvent;                            /**< Hardware events structure */

/*----------------------------------------------------------------------------*
 * Mask applied to the 16 bytes (1bit/byte) of hw events read from 4500
 * These bytes correspond to the IT_LATCH (itl) registers in AB8500 (4500)
 * '0' : register will not be read
 * '1' : register will be read
 *----------------------------------------------------------------------------*/
typedef union {
    struct {
	unsigned int itl1:1;
	unsigned int itl2:1;
	unsigned int itl3:1;
	unsigned int itl4:1;
	unsigned int itl5:1;
	unsigned int itl6:1;
	unsigned int itl7:1;
	unsigned int itl8:1;
	unsigned int itl9:1;
	unsigned int itl10:1;
	unsigned int bitFill1:1;
	unsigned int itl12:1;
	unsigned int bitFill2:6;
	unsigned int itl19:1;
	unsigned int itl20:1;
	unsigned int itl21:1;
	unsigned int itl22:1;
	unsigned int itl23:1;
	unsigned int itl24:1;
	unsigned int bitFill3:8;
    } event;
    unsigned int CompletField;
} t_4500HwEvent;

typedef struct {

    /** 1 case ReqMb0H.header == PwrStTrH */
    union {
        struct {
            t_ApPwrStTr   ApPwrStTr : 8;    /**< Power state requested */
            t_OnOffSt     ApPllSt   : 8;    /**< PLL state requested */
            t_OnOffSt     UlpClkSt  : 8;    /**< ULP clock requested */
            t_OnOffSt     DoNotWfi  : 8  ;  /**< A9 WFI requested when going to low power state */
        } un;                               /**< To access power state through bitfield */
        unsigned int CompletField;          /**< To access power state through 32bits variable */
    } PwrStTr;                              /**< Power state transition structure */

    /** 2.1 case ReqMb0H.header == CfgWkUpsHExe or\n
        2.2 case ReqMb0H.header == CfgWkUpsHSleep */
    t_8500HwEvent WkUp8500;                 /**< DB8500 Wakeup events configuration */
    t_4500HwEvent WkUp4500;                 /**< AB850 Wakeup events configuration */

    /** 3 case ReqMb0H.header == RdWkUpAckH toggle read pointer (RdP) in AckMb0 */
    /** 4 case ReqMb0H.header == DummyService */

} t_ReqMb0; /**< Request mailbox 0 */

/*----------------------------------------------------------------------------*
* ReqMb1
*          type : Service Request Mailbox
*          associated IT : prcm_mbox_cpu_set[1] -> xp70 IT 11
*          communication direction : arm (write) -> prcmu (read)
*          width (Bytes) : 12
*          multiservice (using header) : true (disjoint)
*          service(s) : -1- Ap operating point task (fwCopy)
*                       -2- configure hw events that restore 100% opp
*----------------------------------------------------------------------------*/
typedef struct {
  //---------------------------- -1- case ReqMb1H.header == ArmApeOppH
  union {
    struct {
      t_ArmOpp  ArmOpp    : 8;
      t_ApeOpp  ApeOpp    : 8;
      t_OnOffSt BoostMode : 8;
      unsigned char byteFill;
    } un;
    unsigned int CompletField;
  } dvfs;

  //---------------------------- -2- case ReqMb1H.header == ArmApeRestore100OppH
  // On selected hw events restore 100 OPP on Ape
  t_8500HwEvent apeOpp100Restore;
  // On selected hw events restore 100 OPP on Arm
  t_8500HwEvent armOpp100Restore;

} t_ReqMb1;

/*----------------------------------------------------------------------------*
 * ReqMb5
 *         type : Service Request Mailbox
 *         associated IT : prcm_mbox_cpu_set[5] -> xp70 IT 17
 *         communication direction : arm (write) -> prcmu (read)
 *         width (Bytes) : 4
 *         multiservice (using header) : true
 *         service(s) : -1- generic I2C transfer (read or write)
 *                      -2- Read modify write generic I2C transfer
 * -1- case ReqMb5H.header == 0 => generic I2C transfer (read or write)
 * use the genI2c service in reading when I2CopType=1 or
 * use the genI2c service in writing when I2CopType=0
 * -2- case ReqMb5H.header != 0 => Read modify write generic I2C transfer
 * the ReqMb5H.header value is used as writing mask:
 * when '0' into a mask bit => this bit has to be write
 * when '1' into a mask bit => the bit has to read and rewrite
*----------------------------------------------------------------------------*/
typedef union {
    struct {
	t_I2CopType	I2CopType : 1;
	unsigned int	SlaveAdd:7;
	unsigned int	Tsize:2;
	unsigned int	HwGCEn:1;
	unsigned int	StopEn:1;
	unsigned int	Reserved:4;
	unsigned int	RegAdd:8;
	unsigned int	RegVal:8;
    } un;
    unsigned int CompletField;
} t_ReqMb5;

/*----------------------------------------------------------------------------*
*AckMb1
*    - type : Service Acknowledge Mailbox
*    - associated IT : prcm_arm_it1_val[1]
*    - communication direction : prcmu (write) -> arm (read)
*    - width (Bytes) : 4
*    - multiservice (using header) : true
*    - service(s) :
*        - 1 Ap operating point task acknowledge
*        - 2 Restore A9/APE 100% opp on USBWkup hw events
*----------------------------------------------------------------------------*/
typedef struct {

    /** 1 case ReqMb1H.header == ArmApeOppH */
    union {
        struct {
            t_ArmOpp  currArmOpp                  : 8;     /**< Current ARM OPP */
            t_ApeOpp  currApeOpp                  : 8;     /**< Current APE OPP */
            t_ape_voltage_status ApeVoltageStatus : 1;     /**< APE Voltage  status */
            t_OnOffSt Bytefill                    : 7;     /**< Reserved */
            t_DvfsStatus ArmDvfsStatus            : 4;     /**< ARM DVFS status */
            t_DvfsStatus ApeDvfsStatus            : 4;     /**< APE DVFS status */
        } un;                                   /**< To access DVFS status through bitfield */
        unsigned int CompletField;              /**< To access DVFS status through 32bits variable */
    } dvfs;                                     /**< DVFS configuration */

	/** 2 case ReqMb1H.header == ArmApeRestore100OppH
	    depracated*/

	/** 3 case ReqMb1H.header == ResetModemH
	    depracated*/

	/** 4 case ReqMb1H.header == Ape100OPPVoltRequestH
        100% voltage is required APE side */

	/** 5 case ReqMb1H.header == Ape100OPPVoltReleaseH
        100% voltage is no more required APE side */

	/** 6 case ReqMb1H.header == UsbWakeupReleaseH
        USB indicating that is no more operating  */

	/** 7 case ReqMb1H.header == ResetModemH
	    depracated*/

} t_AckMb1; /**< Acknowledge mailbox 1 */

/*----------------------------------------------------------------------------*
 * AckMb4
 *          type : Service Acknowledge Mailbox
 *          associated IT : prcm_arm_it1_val[4]
 *          communication direction : prcmu (write) -> arm (read)
 *          width (Bytes) : 4
 *          multiservice (using header) : false
 *          service(s) : <unused>
*----------------------------------------------------------------------------*/
typedef union {
    struct {
	t_AckMb4Status AckMb4Status	: 8;
	unsigned char  byteFill[3];
    } field;
    unsigned int CompletField;
} t_AckMb4;

/*----------------------------------------------------------------------------*
 * AckMb5
 *         type : Service Acknowledge Mailbox
 *         associated IT : prcm_arm_it1_val[5]
 *         communication direction : prcmu (write) -> arm (read)
 *         width (Bytes) : 4SlaveAdd
 *         multiservice (using header) : true
 *         service(s) : -1- generic I2C transfer (read or write)
 *                      -2- Read modify write generic I2C transfer
 * -1- case ReqMb5H.header == 0 => generic I2C transfer is read mode
 * update AckMb5.
 * write I2C_Rd_Ok status into AckMb5Status field
 * The asked read value is found into RegVal field
 * -2- case ReqMb5H.header != 0 => Read modify write generic I2C transfer
 * update AckMb5.
 * write I2C_Wr_Ok status into into AckMb5Status field
*----------------------------------------------------------------------------*/
typedef union {
    struct {
	t_I2CopType	I2CopType : 1;
	unsigned int	SlaveAdd:7;
	t_AckMb5Status	AckMb5Status : 8;
	unsigned int	RegAdd:8;
	unsigned int	RegVal:8;
    } un;
    unsigned int CompletField;
} t_AckMb5;

typedef struct
{
    unsigned char CfgAdd[8];
    struct
    {
        unsigned char DataCS[8];
    } cs[2];
} t_DDRInitData;


#endif /* !defined(XP70_MEMMAP_H) */
