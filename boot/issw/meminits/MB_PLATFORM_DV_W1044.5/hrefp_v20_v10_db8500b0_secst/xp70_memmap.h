#ifndef XP70_MEMMAP_H
#define XP70_MEMMAP_H

/*----------------------------------------------------------------------------*
 *      STEricsson, Incorporated. All rights reserved.                        *
 *                                                                            *
 *        STEricsson, INCORPORATED PROPRIETARY INFORMATION                    *
 * This software is supplied under the terms of a license agreement or non-   *
 * disclosure agreement with STEricsson and may not be copied or disclosed    *
 * except in accordance with the terms of that agreement.                     *
 *----------------------------------------------------------------------------*
 * System      : STxP70 Processor                                             *
 * File Name   : xp70_MemMap.h                                                *
 * Purpose     : defines the TCDM and TCPM memory mapping of the xp70         *
 * Author(s)   : Didier MOULIN                                                *
 *               Helder DA SILVA VALENTE                                      *
 * History     : 2009/10/26 - First draft implementation.  (Helder)           *
 *----------------------------------------------------------------------------*/


// Contents :
//      List of the shared variables
//      List of the Request mailboxes
//      List of the Acknowledge Mailboxes
//      TCPM & TCDM Memory Map


// Legend :
//      [ ] = optional
//      [ x | y | z ] = optional choice between x, y and z
//      n/a = not applicable
//      Rd / Wr = Read / Write
//      <Text> = to be replaced with the meaning of the Text
///////////////////////////////////////////////////////////////////////////////
// <name (not type) of the shared variable or mailbox>
//
//          type : [ Shared Variable | Sevice Request Mailbox |
//                   Service Acknowledge Mailbox ]
//
//          associated IT : <name of the hw register which generates the IT>
//                          [ -> <associated xp70 IT if applicable> ]
//
//          communication direction : <description of the type and the blocs
//                                     involved in the com. with the prcmu>
//
//          width (Bytes) : <nb of bytes reserved in memory>
//
//          multiservice (using header) : true | false [ (disjoint) | (shared) ]
//                                        if more than one service is associated
//                                        to the mailbox a header destinguishes
//                                        the services = multiservice mode.
//                                        In multiservice mode, memory can be :
//                           [(disjoint)] All services have their own memory
//                                        spaces. Thus usually a copy to local
//                                        variables is not needed.
//                             [(shared)] All the services share a same
//                                        memory space.
//
//          service(s) : [-<service number>-] <short description of service>
//                       [(fwCopy)] [(<ack. procedure if diff. from default>)]
//            [(fwCopy)] When executing the current service the firmware starts
//                       by copying the memory area to a local variable. Note,
//                       this can also happen in single service mode or in dis-
//                       joint multiservice mode.
//
///////////////////////////////////////////////////////////////////////////////
//
//  PRCMU API USER MANUAL 
//                       (for arm developers)
//
//  How does the prcmu behave when a service ReqMbj(SiH) has been requested ?
//  -----------------------------------------------------------------------
//  - prcmu enters the service IT routine.
//  - it potentially copies the mailbox data (fwcopy).
//  - it asserts prcm_mbox_cpu[j] (requesting IT) back to '0'.
//  - if the service implies an atomic action (like generic I2C), it masks 
//    the service requesting ITs and/or other ITs.
//  - it executes the requested service.
//  - If an acknowledge to the service is planned, a success or error 
//    message (status) or other requested data are written into AckMbj 
//    (respectively AckMbj(SiH) if it is a  multiservice acknowledge mailbox).
//  - It generates the IT prcm_arm_it1[j].
//
//  How to request the service SiH with the requesting mailbox ReqMbj ?
//  -----------------------------------------------------------------
//  The prcmu services can be called using two different methods, which are 
//  detailed afterwards (in pseudo code manner): 
//  (A) polling the prcm_mbox_cpu[j] until its value is '0' with a timeout
//  (B) Using the AckMbj(SiH) to assert that ReqMbj(SiH) is accepted. If
//      before service request ReqMbj(SiH), prcm_mbox_cpu[j] is '1' an 
//      error is issued.
//
//  Method (B) can obviously only be chosen when the ReqMbj(SiH) service is 
//  systematically acknowledged by AckMbj(SiH). Further (B) avoids having 
//  the arm polling on the prcm_mbox_cpu[j] register and is consequently 
//  more interesting to be implemented for services, which on prcmu side 
//  will keep prcm_mbox_cpu[j] to '1' a long time. This is the case for 
//  instance for the generic I2C service, which masks the ITs when accessing 
//  the I2C. 
//
//  (Dis)Advantages of Method (A):
//  + most straightforward method, which can be implemented as the first solution
//    for all the services. Method (B) can always be planned later on
//    where method (A) was proven inadequate. 
//  + The time out being programmable, it can be used for testing the 
//    reactiveness (resp. robustness) of the prcmu to the service requests. 
//  + No acknowledge IT of a service is required.
//  - Arm resources can be monopolized in a register polling, which can be 
//    of an unacceptable amount of time especially regarding the frequencies
//    of both cores. 
//
//  (Dis)Advantages of Method (B):
//  + polling of the arm on prcm_mbox_cpu[j] register is avoided.
//  - It must be kept track the requested services.
//
//  Finally, method (B) is more interesting to be implemented for services, 
//  which on prcmu side might keep prcm_mbox_cpu[j] to '1' for a long time. 
//  This is for instance the case for the generic I2C service, which masks 
//  the service ITs when accessing the I2C. A suggestion of which method to 
//  use might be in the mailbox comments.
//  
//
//  (A) Sequence for service request using time out
//  --------------------------------------------------------------------------
//  if (prcm_mbox_cpu[j] == 0) 
//      request service ReqMbj(SiH)
//  else
//      Wait for (prcm_mbox_cpu[j] == 1) or (timeout TOUT1)
//      if timeout occured
//          send back error
//      endif
//      if (prcm_mbox_cpu[j] == 1)
//          request service ReqMbj(SiH) 
//      endif
// 
//
// (B) Sequence for service request using acknowledge mechanism
// -------------------------------------------------------------------------- 
//  if (prcm_mbox_cpu[j] == 0) 
//      request service ReqMbj(SiH)
//  else
//      if (ReqMbj(SiH) previously requested)
//          Wait for (AckMbj(SiH) interrupt)
//          if (prcm_mbox_cpu[j] == 0) 
//              request service ReqMbj(SiH)
//          else
//              send back error
//          endif
//      else
//          send back error
//      endif
//
//
//
//
//  request service ReqMbj(SiH) consists in :
//        (1) writing the service related data into the mailbox
//        (2) generating mailbox IT by setting prcm_mbox_cpu[j] to '1'
//  Note that (1) must be written AFTER having asserted prcm_mbox_cpu[j] low.
//
///////////////////////////////////////////////////////////////////////////////




#include "numerics.h"



// definition of base addresses
#define NONSEC_PRCMU_REG_BASE 0x80157000 // B1 (in spec)
#define SEC_PRCMU_REG_BASE    0x80158000 // B2 (in spec)
#define TCPM_BASE             0x801B0000 // B3 (in spec)
#define TCDM_BASE             0x801B8000 // B4 (in spec)
#define TCDM_SIZE             0x1000 	 //(in spec)


// In order to optimize memory space the headers are grouped.
typedef struct {

    union {
        struct {
            t_ReqMb0H     Mb0 : 8;
            t_ReqMb1H     Mb1 : 8;
            t_ReqMb2H     Mb2 : 8;
            t_ReqMb3H     Mb3 : 8;
            t_ReqMb4H     Mb4 : 8;
            unsigned char Mb5 : 8;
            unsigned char Mb6 : 8; // unused
            t_ReqMb7H		Mb7 : 8;
        } un;
        unsigned char Mb[8];
    } Req;
    
    union {
        struct {
            t_ReqMb0H     Mb0 : 8;
            unsigned char Mb1 : 8; // unused
            unsigned char Mb2 : 8; // unused
            unsigned char Mb3 : 8; // unused
            t_ReqMb4H		Mb4 : 8;
            unsigned char Mb5 : 8; // unused
            unsigned char Mb6 : 8; // unused
            unsigned char Mb7 : 8; // unused
        } un;
        unsigned char Mb[8];
    } Ack;
    
} t_Header;


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////// List of the shared variables //
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
// romcode_mb
//
//          type : Shared Variable
//          associated IT : n/a
//          communication direction : arm (Rd/Wr) <-> prcmu (Wr/Rd)
//          width (Bytes) : 4
//          multiservice (using header) : n/a
//          service(s) : communication with romcode
///////////////////////////////////////////////////////////////////////////////

// t_RomcodeMb romcode_mb;



///////////////////////////////////////////////////////////////////////////////
// boot_status_debug_var
//
//          type : Shared Variable
//          associated IT : n/a
//          communication direction : arm (Rd) <- prcmu (Wr)
//          width (Bytes) : 1
//          multiservice (using header) : n/a
//          service(s) : - synchronization with arm during xp70 launch
//                       - for internal debug
///////////////////////////////////////////////////////////////////////////////

// Note: List of debug codes not provided in shared .h files !

// unsigned char boot_status_debug_var;


///////////////////////////////////////////////////////////////////////////////
// pwr_current_state_var
//
//          type : Shared Variable
//          associated IT : n/a
//          communication direction : arm (Rd) <- prcmu (Wr)
//          width (Bytes) : 1
//          multiservice (using header) : n/a
//          service(s) : Stores the current power state (for internal use)
///////////////////////////////////////////////////////////////////////////////

// t_ApPwrSt pwr_current_state_var;

typedef struct {
    t_ApPwrSt     pwr_current_state_var : 8;
    unsigned char byteFill[2];
    unsigned char boot_status_debug_var;
} t_ShVar1;



///////////////////////////////////////////////////////////////////////////////
// DDRInit
//
//          type : Shared Variable
//          associated IT : prcm_mbox_cpu_set[4] -> xp70 IT 16
//          communication direction : arm (Wr) -> prcmu (Rd)
//          width (Bytes) : 464
//          multiservice (using header) : n/a
//          service(s) : - Initialization of DDR Controller and PHY
// 						 - The three Msb of MaskCfg0[4] are used to store the DDR size value
///////////////////////////////////////////////////////////////////////////////

typedef struct {
    unsigned int MaskCfg0[5];
    unsigned int Cfg0[111];
} t_DDRInit;


///////////////////////////////////////////////////////////////////////////////
// unsecure DDR header
//
//          type : Shared Variable
//          associated IT : prcm_mbox_cpu_set[4] -> xp70 IT 16
//          communication direction : prcmu (Wr) -> arm (Rd)
//          width (Bytes) : 4
//          multiservice (using header) : n/a
//          service(s) : - copy of secDDRhead to unsecDDRhead during DDR init
///////////////////////////////////////////////////////////////////////////////

typedef struct {
    unsigned int unsecDDRhead;
} t_ShVar2;


///////////////////////////////////////////////////////////////////////////////
// perfmeter timer msb
//
//          type : Shared Variable
//          associated IT :  n/a
//          communication direction : prcmu (Wr) -> arm (Rd)
//          width (Bytes) : 4
//          multiservice (using header) : n/a
//          service(s) : - perfmeter timer4 msb value 
///////////////////////////////////////////////////////////////////////////////

typedef struct {
    unsigned int timer4_msb;
} t_ShVar3;


///////////////////////////////////////////////////////////////////////////////
// primary watchdog 
//
//          type : Shared Variable
//          associated IT :  n/a
//          communication direction : prcmu (Wr) -> arm (Rd)
//          width (Bytes) : 4
//          multiservice (using header) : n/a
//          service(s) : 
///////////////////////////////////////////////////////////////////////////////

typedef struct {
    unsigned char MainWDogCtrlVar ;
    unsigned char MainWDogTimerVar;
    unsigned char byteFill[2];
} t_ShVar4;



///////////////////////////////////////////////////////////////////////////////
// Adaptive Voltage Scaling (AVS) data
//
//          type : Shared Variable
//          associated IT : prcm_mbox_cpu_set[4] -> xp70 IT 16
//          communication direction : voltage tables : bias, comp, dflt : 
//                                                     arm (Wr) -> prcmu (Rd)
//                                    status field : stat, pend : 
//                                                   prcmu(Rd/Wr) -> arm(Rd) 
//                                                   (arm potential read)
//          width (Bytes) : 
//          multiservice (using header) : n/a
//          service(s) : - pass compensated or default voltage values from the
//                         ARM to the prcmu and update of status field by prcmu
///////////////////////////////////////////////////////////////////////////////
#define AVS_STRUCT_SIZE_BYTES   13

// index of different voltages to be used when accessing AVSData structure as a t_uint8 tab 
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


// voltage supply. bit 6 and 7 are only relevant for vARM max OPP
// IsSlowStartup : copy of the OTP_VarmVapeSlowStartup fuse.
// IsModeEnabled : ARM MAX opp is enabled when this bit is set.
typedef struct {
    char Voltage        : 6;
    char IsSlowStartup  : 1;
    char IsModeEnabled  : 1;    // only usefull for ARM OPP0 (max OPP)

} t_VoltSupply;


// bias voltages in signed format in order to deal arithmetically 
// with the voltage values
typedef struct {    
        signed char Vbbn    : 4;
        signed char Vbbp    : 4;

} t_VoltBB;


// Structure which holds the data relative to the AVS.
// Data may be accessed via Voltage[] tab or using the Fields structure.
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
    } Fields;
    
    char Voltage[AVS_STRUCT_SIZE_BYTES];
} t_AVSData;


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////// List of the Request mailboxes //
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
// ReqMb0
//          type : Service Request Mailbox
//          associated IT : prcm_mbox_cpu_set[0] -> xp70 IT 10
//          communication direction : arm (write) -> prcmu (read)
//          width (Bytes) : 12
//          multiservice (using header) : true (disjoint)
//          service(s) : -1- power state transition requests
//                       -2.1- configuration of the wake up events for execute and idle power mode    (no ack)(shared with 2.2)(fwcopy)
//                       -2.2- configuration of the wake up events for deepsleep and sleep power mode (no ack)(shared with 2.1)(fwcopy)
//                       -3- wake-up reason read acknowledge (no ack)
///////////////////////////////////////////////////////////////////////////////

// List of the 8500 relevant Hw events to be used as a mask for wake-up config.
typedef union {
    struct {
        // If not otherwise mentionned (by <HwEvent>_fall) only rising edge is
        // taken into consideration. Alignement is done with PRCM_HOLD_EVT register.
        // Format :
        //  <type> <HwEvent> : <nb of bits> ;  <position in CompletField> 
        //                                       <HwEvent assoc. status register>
        // '0' : Hw Event is masked
        // '1' : Hw Event is unmasked
        
        unsigned int RTC               : 1; // 0  PRCM_STATUS0[0]
        unsigned int RTT0              : 1; // 1  PRCM_STATUS0[1]
        unsigned int RTT1              : 1; // 2  PRCM_STATUS0[2]
        unsigned int HSI0              : 1; // 3  PRCM_STATUS0[3]
        unsigned int HSI1              : 1; // 4  PRCM_STATUS0[4]
        unsigned int cawakereq         : 1; // 5  PRCM_STATUS0[5] note: in some doc 'cawakereq' is also denoted as 'MOD_WAKE_HOST_IRQ'
        unsigned int USBWkup           : 1; // 6  PRCM_STATUS0[6]
        unsigned int ape_int_4500      : 1; // 7  PRCM_STATUS0[7]
        unsigned int fifo4500it        : 1; // 8  PRCM_STATUS1[0] //2do:DM: do not implement for the moment
        
        unsigned int bitFill1          : 8; // stuffing bits
        
        // ITs from arm enabled by ARMit_maskxp70_it
        // at position of swreset in PRCM_HOLD_EVT register
        unsigned int ARM_it_mgt        : 1; // 17 no status bit available
        
        unsigned int bitFill2          : 2; // stuffing bits
        
        unsigned int mod_sw_reset_req  : 1; // 20 PRCM_STATUS4[3]
        
        unsigned int bitFill3          : 2; // stuffing bits
        
        unsigned int gpio              : 9; // 31:23 PRCM_STATUS5[8:0]
        
    } event;

    unsigned int CompletField;

} t_8500HwEvent;

// Mask applied to the 16 bytes (1bit/byte) of hw events read from 4500
// These bytes correspond to the IT_LATCH (itl) registers in AB8500 (4500)
// '0' : register will not be read
// '1' : register will be read
typedef union {
    struct {
        unsigned int itl1     : 1;
        unsigned int itl2     : 1;
        unsigned int itl3     : 1;
        unsigned int itl4     : 1;
        unsigned int itl5     : 1;
        unsigned int itl6     : 1;
        unsigned int itl7     : 1;
        unsigned int itl8     : 1;
        unsigned int itl9     : 1;
        unsigned int itl10    : 1;
        unsigned int bitFill2 : 8;
        unsigned int itl19    : 1;
        unsigned int itl20    : 1;
        unsigned int itl21    : 1;
        unsigned int itl22    : 1;
        unsigned int itl23    : 1;
        unsigned int itl24    : 1;
        unsigned int bitFill3 : 8;
    } event;
    unsigned int CompletField;
} t_4500HwEvent;


typedef struct {

    //---------------------------- -1- case ReqMb0H.header == PwrStTrH
    union {
        struct {
            t_ApPwrStTr   ApPwrStTr : 8;
            t_OnOffSt     ApPllSt   : 8;
            t_OnOffSt     UlpClkSt  : 8;
            unsigned char byteFill  ;
        } un;
        unsigned int CompletField;
    } PwrStTr;
    
    //---------------------------- -2.1- case ReqMb0H.header == CfgWkUpsHExe or 
    //---------------------------- -2.2- case ReqMb0H.header == CfgWkUpsHSleep
    t_8500HwEvent WkUp8500;
    t_4500HwEvent WkUp4500;
    
    //---------------------------- -3- case ReqMb0H.header == RdWkUpAckH
    // toggle read pointer (RdP) in AckMb0

} t_ReqMb0;


///////////////////////////////////////////////////////////////////////////////
// ReqMb1
//          type : Service Request Mailbox
//          associated IT : prcm_mbox_cpu_set[1] -> xp70 IT 11
//          communication direction : arm (write) -> prcmu (read)
//          width (Bytes) : 12
//          multiservice (using header) : true (disjoint)
//          service(s) : -1- Ap operating point task (fwCopy)
//                       -2- configure hw events that restore 100% opp
///////////////////////////////////////////////////////////////////////////////

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


///////////////////////////////////////////////////////////////////////////////
// ReqMb2
//          type : Service Request Mailbox
//          associated IT : prcm_mbox_cpu_set[2] -> xp70 IT 12
//          communication direction : arm (write) -> prcmu (read)
//          width (Bytes) : 16
//          multiservice (using header) : true (shared)
//          service(s) : -1- DPS on hardware accelerators (fwCopy)
//                       -2- autonomous power management
///////////////////////////////////////////////////////////////////////////////

/* hardware accelerator denomination list */
typedef enum {
    svammdsp = 0,
    svapipe  = 1,
    siammdsp = 2,
    siapipe  = 3,
    sga      = 4,
    b2r2mcde = 5,
    esram12  = 6,
    esram34  = 7
} t_HwAccIndex;

typedef union {
    struct {
        unsigned char HSEM       : 1;
        unsigned char Fifo4500it : 1;
        unsigned char bitFill    : 6;
    } un;
    unsigned char CompletField;
} t_SiaSvaPwrOnEnable;

typedef union {
	struct {
    	// sia power policy
    	t_SiaSvaPwrPolicy siaPwrPolicy : 4;
    	// sva power policy
    	t_SiaSvaPwrPolicy svaPwrPolicy : 4;
    	// sia power ON conditions HSEM or 4500 fifo interrupt
    	t_SiaSvaPwrOnEnable siaPwrOnEnable;
    	// sva power ON conditions HSEM or 4500 fifo interrupt
    	t_SiaSvaPwrOnEnable svaPwrOnEnable;
    	// global Enable for autonomous power management
    	t_OnOffSt AutoPwrEnable : 8;
	} un;
    unsigned int  CompletField;
} t_SiaSvaAutoPwr;

typedef struct {
    // power policy applied by PRCMU when apSleep transition request
    t_SiaSvaAutoPwr   apSleepSiaSvaAutoPwr;
    // power policy applied by PRCMU when apIdle  transition request
    // not supported as a first step
    t_SiaSvaAutoPwr   apIdleSiaSvaAutoPwr;
    // To be clarified: mutually exclusive ?
    // If not what is the power policy to be applied by the PRCMU on MMDSP interrupt
    // while AP is in apExecute ?
} t_autoPwr;

typedef struct {

    //---------------------------- -1- case ReqMb2H.header == dpsH
    // info: bit-uns with tables, is not supported
    //   t_HwAccSt HwAccSt[8] : 8; // HwAccSt[hwAccIndex]
    union {
        struct {
            t_HwAccSt svammdsp : 8;
            t_HwAccSt svapipe  : 8;
            t_HwAccSt siammdsp : 8;
            t_HwAccSt siapipe  : 8;
            t_HwAccSt sga      : 8;
            t_HwAccSt b2r2mcde : 8;
            t_HwAccSt esram12  : 8;
            t_HwAccSt esram34  : 8;
        } un;
        unsigned char Acc[8]; // Hw.Acc[HwAccIndex]
    } Hw;

    //---------------------------- -2- case ReqMb2H.header == autoPwrH
    t_autoPwr autoPwr;

} t_ReqMb2;




///////////////////////////////////////////////////////////////////////////////
// ReqMb3
//          type : Service Request Mailbox
//          associated IT : prcm_mbox_cpu_set[3] -> xp70 IT 13
//          communication direction : arm (write) -> prcmu (read)
//          width (Bytes) : 372
//          multiservice (using header) : true (disjoint)
//          service(s) : -1- ANC filter coefficient update
//                       -2- sidetone filter coefficient update
///////////////////////////////////////////////////////////////////////////////

typedef struct {

  //---------------------------- -1- case ReqMb3H.header == AncH
  struct {

	/* 15 x 16-bit coefficient for the FIR */
    unsigned char ANCFIRCoeff[30];	/* ANCFIRCoeff[even]=ANCFIRCoeffMSB &  ANCShifter[odd]=ANCFIRCoeffLSB */
    unsigned short byteFill0;
	
    /* 32 x 16-bit coefficient for the IIR */
    unsigned char ANCIIRCoeff[64];	/* ANCFIRCoeff[even]=ANCIIRCoeffMSB &  ANCShifter[odd]=ANCIIRCoeffLSB */

    /* 3 x 8-bit shifter*/
	unsigned char ANCShifter[3];	/* ANCShifter[0]=ANCInShift &  ANCShifter[1]=ANCFIROutShift &  ANCShifter[2]=ANCShiftOut */
	unsigned char byteFill1;

	/* 2 x 8-bit WarpDel */
	unsigned char ANCWarp[2];	/* ANCWarp[0]=ANCWarpDelMSB &  ANCWarp[1]=ANCWarpDelLSB */
	unsigned char byteFill2[2];
		
  } ANC;

  //---------------------------- -2- case ReqMb3H.header == SidetoneH
  struct {

	/* 2 x 8-bit SidFIRGain*/
	unsigned char SidFIRGain[2];	/* SidFIRGain[0]=FIRSid1Gain &  SidFIRGain[1]=FIRSid2Gain */
	unsigned char byteFill0[2];

	/* 128 x 16-bit coefficient for the SidFIRCoef */
	unsigned char SidFIRCoeff[256];	/* SidFIRCoeff[even]=FIRSidCoeffMSB &  SidFIRCoeff[odd]=FIRSidCoeffLSB */

  } sidetone;

  /* Not used */
  unsigned char byteFill[8];

} t_ReqMb3;



///////////////////////////////////////////////////////////////////////////////
// ReqMb4
//          type : Service Request Mailbox
//          associated IT : prcm_mbox_cpu_set[4] -> xp70 IT 16
//          communication direction : arm (write) -> prcmu (read)
//          width (Bytes) : 4
//          multiservice (using header) : true (shared)
//          service(s) : -1- DDR Management
//                          -1.1- initialization of ddr
//                          -1.2- ddr and esram power states (partly fwCopy)
//                          -1.3- auto self refresh
//                          -1.4- auto power down
//                          -1.5- partial self refresh
//                          -1.6- power management per chip select (CS)
//                          -1.7- QoSPortiH
//                       -2- Sysclk management
//                       -3- Adaptive Voltage Scaling (AVS)
//                       -4- Primary Watchdog settings (primWDsettingsH)
//
///////////////////////////////////////////////////////////////////////////////


typedef struct {

  union {

    //---------------------------- -1.1- case ReqMb4H.header == ddrInitH

    // configure the DDR controller and PHY by copying DDRInit shared variable


    //---------------------------- -1.2- case ReqMb4H.header == memStH
    struct {
        /* expected DDR state depending the AP Power State (LowLatency/highlatency) */
        t_DdrPwrSt DdrStApIdle  		: 4; // (fwCopy)
        t_DdrPwrSt DdrStApSleep 		: 4; // (fwCopy)
        t_DdrPwrSt DdrStApDeepIdle 	: 4; // (fwCopy)
        unsigned int bitFill1			: 12; // stuffing bits
        
        // expected secure eSRAM  in deepsleep mode(Nochgt:0 or OFF:1 or retention:2 or ON:3)
        t_OnOffRetSt eSRAM0St : 8;
    } memSt;


    //---------------------------- -1.3- case ReqMb4H.header == autoSrefreshH
    // = mode 5 of the DDR Controller
    // not yet implemented - do not use !
    struct {
        // cycle count before DDR automatically goes in self-refresh
        unsigned short lowPower_internal_cnt;
        unsigned char  byteFillUnion[2];
    } autoSrefresh;


    //---------------------------- -1.4- case ReqMb4H.header == autoPwrDnH
    // = mode 2 of the DDR Controller
    // not yet implemented - do not use !
    struct {
        // cycle count before DDR automatically goes in power down
        unsigned short Lowpower_power_down_cnt;
        unsigned char  byteFillUnion[2];
    } autoPwrDn;


    //---------------------------- -1.5- case ReqMb4H.header == partialSrefreshCS0H
    //                                                          partialSrefreshCS1H
    struct {
        /* variable defining the MR16 value for CS0 and CS1 */
        unsigned short MR16;
        /* variable defining the MR17 value for CS0 and CS1 */
        unsigned short MR17;
    } partialSrefresh;


    //---------------------------- -1.6- case ReqMb4H.header == csPwrMgtH
    // not yet implemented - do not use !
    t_csPwrMgt csPwrMgt;

    //---------------------------- -1.7- case ReqMb4H.header == QoSPort0H
    //                                                          QoSPort1H
    //                                                          QoSPort2H
    //                                                          QoSPort3H
    struct {
        unsigned char PortPtyWr;
        unsigned char PortPtyRd;
        unsigned char PortBw;
	    /* Allow to select the field to be write:
	    MaskVal(0) is the PortPtyWr mask, 
	    MaskVal(1) is the PortPtyRd mask, 
	    MaskVal(2) is the PortBw mask, */
        unsigned char MaskVal;
    } QoSPort;


    //---------------------------- -2- case ReqMb4H.header == sysClkH
    t_OnOffSt SysClkMgt;


    //---------------------------- -3- case ReqMb4H.header == AvsH

    // no parameter needed for AVS service.
    // This service will only copy voltage values given in AVSData structure (copied 
    // by ARM ) into AB8500 dedicated registers.
    // This service can only be called when FW is in apBoot state, otherwise 
    // an error is issued on AckMb4.
    
    //---------------------------- -4- case ReqMb4H.header == primWDsettingsH

    t_ShVar4 MainWDog;

  } un;
} t_ReqMb4;


///////////////////////////////////////////////////////////////////////////////
// ReqMb5
//          type : Service Request Mailbox
//          associated IT : prcm_mbox_cpu_set[5] -> xp70 IT 17
//          communication direction : arm (write) -> prcmu (read)
//          width (Bytes) : 4
//          multiservice (using header) : true
//          service(s) : -1- generic I2C transfer (read or write)
//                       -2- Read modify write generic I2C transfer
///////////////////////////////////////////////////////////////////////////////
//
//---------------------------- -1- case ReqMb5H.header == 0 => generic I2C transfer (read or write)
//
// use the genI2c service in reading when I2CopType=1 or
// use the genI2c service in writing when I2CopType=0 
//
//---------------------------- -2- case ReqMb5H.header != 0 => Read modify write generic I2C transfer
//
// the ReqMb5H.header value is used as writing mask:
// when '0' into a mask bit => this bit has to be write
// when '1' into a mask bit => the bit has to read and rewrite
//
//----------------------------
typedef union {
    struct {
        t_I2CopType I2CopType: 1;
        unsigned int SlaveAdd: 7;
        unsigned int Tsize	 : 2;
        unsigned int HwGCEn	 : 1;
        unsigned int StopEn	 : 1;
        unsigned int Reserved: 4;
        unsigned int RegAdd	 : 8;
        unsigned int RegVal	 : 8;
    } un;
    unsigned int CompletField;
} t_ReqMb5;


///////////////////////////////////////////////////////////////////////////////
// ReqMb6
//          type : Service Request Mailbox
//          associated IT : prcm_mbox_cpu_set[6] -> xp70 IT 18
//          communication direction : SvaMmdsp (write) -> prcmu (read)
//          width (Bytes) : 4
//          multiservice (using header) : false
//          service(s) :  <unused> 
///////////////////////////////////////////////////////////////////////////////

typedef struct {
    unsigned char byteFill[4];
} t_ReqMb6;


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////// List of the Acknowledge Mailboxes //
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////



///////////////////////////////////////////////////////////////////////////////
// AckMb0
//          type : Service Acknowledge Mailbox
//          associated IT : prcm_arm_it1_val[0]
//          communication direction : prcmu (write) -> arm (read)
//          width (Bytes) : 52
//          multiservice (using header) : true (disjoint)
//          service(s) : -1- power state transition acknowledge
//                       -2.1- wake-up reason transfer from low power states for execute and idle power mode(shared with 2.2)
//                       -2.2- wake-up reason transfer from low power states for deepsleep and sleep power mode(shared with 2.1)
///////////////////////////////////////////////////////////////////////////////

typedef struct {
    t_8500HwEvent WkUp8500;
    
    // For 8500V1 wake-up the addresses of the mailboxes are frozen, thus 
    // no optimization is done concerning the following fields.
    // WkUp4500[0]  = IT_LATCH1    masked by   ReqMb0.WkUp4500.event.itl1
    // WkUp4500[1]  = IT_LATCH2    masked by   ReqMb0.WkUp4500.event.itl2
    // WkUp4500[2]  = IT_LATCH3    masked by   ReqMb0.WkUp4500.event.itl3
    // WkUp4500[3]  = IT_LATCH4    masked by   ReqMb0.WkUp4500.event.itl4
    // WkUp4500[4]  = IT_LATCH5    masked by   ReqMb0.WkUp4500.event.itl5
    // WkUp4500[5]  = IT_LATCH6    masked by   ReqMb0.WkUp4500.event.itl6
    // WkUp4500[6]  = IT_LATCH7    masked by   ReqMb0.WkUp4500.event.itl7
    // WkUp4500[7]  = IT_LATCH8    masked by   ReqMb0.WkUp4500.event.itl8
    // WkUp4500[8]  = IT_LATCH9    masked by   ReqMb0.WkUp4500.event.itl9
    // WkUp4500[9]  = IT_LATCH10   masked by   ReqMb0.WkUp4500.event.itl10
    // WkUp4500[10] = IT_LATCH19   masked by   ReqMb0.WkUp4500.event.itl19
    // WkUp4500[11] = IT_LATCH20   masked by   ReqMb0.WkUp4500.event.itl20
    // WkUp4500[12] = IT_LATCH21   masked by   ReqMb0.WkUp4500.event.itl21
    // WkUp4500[13] = IT_LATCH22   masked by   ReqMb0.WkUp4500.event.itl22
    // WkUp4500[14] = IT_LATCH23   masked by   ReqMb0.WkUp4500.event.itl23
    // WkUp4500[15] = IT_LATCH24   masked by   ReqMb0.WkUp4500.event.itl24
    // WkUp4500[16:19] = <unused>
    unsigned char WkUp4500[20];
} t_WkUpReason;

typedef struct {

    //---------------------------- -1- case ReqMb0H.header == PwrStTrH
    
    t_ApPwrStTrStatus ApPwrStTrStatus : 8;
    
    //---------------------------- -2.1- case ReqMb0H.header == WkUpHExe or
    //---------------------------- -2.2- case ReqMb0H.header == WkUpHSleep
    
    // Read pointer to the ping pong mailbox used by arm
    // usage :
    // when the arm receives the IT of the wake-up reason, it reads 
    // 'WkUpR_ping' if RdP == ping (resp. 'WkUpR_pong' if RdP == pong) 
    t_PingPong RdP : 8;
    
    // In order to do the correct transfer of the hardware events to the arm,
    // the prcmu has to manage the states of the hardware event fields.
    // init value : EvtRd
    // WkUpR_st[0] corresp. to ping field
    // WkUpR_st[1] corresp. to pong field
    // t_WkUpReasonFdSt WkUpR_st[2] :8;
    unsigned char WkUpR_st[2];
    
    // Hardware event fields
    // Fields in which the prcmu stores the 8500 and 4500 hardware events 
    // before transferring them to the arm via the IT associated to AckMb0.
    // Two fields have been defined in ordre to implement the pingpong mechanism.
    // WkUpR[0] corresp. to ping field
    // WkUpR[1] corresp. to pong field
    // Selection by arm : WkUpR[0x1 & RdP]
    t_WkUpReason WkUpR[2];
    
} t_AckMb0;

///////////////////////////////////////////////////////////////////////////////
// AckMb1
//          type : Service Acknowledge Mailbox
//          associated IT : prcm_arm_it1_val[1]
//          communication direction : prcmu (write) -> arm (read)
//          width (Bytes) : 4
//          multiservice (using header) : false
//          service(s) : -1- Ap operating point task acknowledge
///////////////////////////////////////////////////////////////////////////////

typedef struct {

    union {
        struct {
            t_ArmOpp  currArmOpp    : 8;
            t_ApeOpp  currApeOpp    : 8;
            t_OnOffSt currBoostMode : 8;
            t_DvfsStatus ArmDvfsStatus : 4; //2do:DM: name and type name changed
            t_DvfsStatus ApeDvfsStatus : 4; //2do:DM: name and type name changed
        } un;
        unsigned int CompletField;
    } dvfs;

} t_AckMb1;


///////////////////////////////////////////////////////////////////////////////
// AckMb2
//          type : Service Acknowledge Mailbox
//          associated IT : prcm_arm_it1_val[2]
//          communication direction : prcmu (write) -> arm (read)
//          width (Bytes) : 4
//          multiservice (using header) : false
//          service(s) : <unused>
///////////////////////////////////////////////////////////////////////////////

typedef struct {
    t_DpsStatus DpsStatus : 8; //2do:DM: name and type name changed
    unsigned char  byteFill[3];
} t_AckMb2;


///////////////////////////////////////////////////////////////////////////////
// AckMb3
//          type : Service Acknowledge Mailbox
//          associated IT : prcm_arm_it1_val[3]
//          communication direction : prcmu (write) -> arm (read)
//          width (Bytes) : 4
//          multiservice (using header) : false
//          service(s) : ANC/sidetone coefficient update completion
///////////////////////////////////////////////////////////////////////////////

typedef struct {
    t_AckMb3Status AckMb3Status : 8;
    unsigned char  byteFill[3];
} t_AckMb3;


///////////////////////////////////////////////////////////////////////////////
// AckMb4
//          type : Service Acknowledge Mailbox
//          associated IT : prcm_arm_it1_val[4]
//          communication direction : prcmu (write) -> arm (read)
//          width (Bytes) : 4
//          multiservice (using header) : false
//          service(s) : <unused>
///////////////////////////////////////////////////////////////////////////////
typedef union {
    struct {
        t_AckMb4Status AckMb4Status : 8;
        unsigned char  byteFill[3];
    } field;
    unsigned int CompletField;
} t_AckMb4;


///////////////////////////////////////////////////////////////////////////////
// AckMb5
//          type : Service Acknowledge Mailbox
//          associated IT : prcm_arm_it1_val[5]
//          communication direction : prcmu (write) -> arm (read)
//          width (Bytes) : 4SlaveAdd
//          multiservice (using header) : true
//          service(s) : -1- generic I2C transfer (read or write)
//                       -2- Read modify write generic I2C transfer
///////////////////////////////////////////////////////////////////////////////
//
//---------------------------- -1- case ReqMb5H.header == 0 => generic I2C transfer is read mode 
//
// update AckMb5.
// write I2C_Rd_Ok status into AckMb5Status field
// The asked read value is found into RegVal field
//
//---------------------------- -1- case ReqMb5H.header == 0 => generic I2C transfer is write mode 
// update AckMb5.
// write I2C_Wr_Ok status into into AckMb5Status field
//
//---------------------------- -2- case ReqMb5H.header != 0 => Read modify write generic I2C transfer
//
// update AckMb5.
// write I2C_Wr_Ok status into into AckMb5Status field 
//
//---------------------------- 
typedef union {
    struct {
        t_I2CopType 		I2CopType	: 1;
        unsigned int 		SlaveAdd	: 7;
        t_AckMb5Status      AckMb5Status: 8;
        unsigned int 		RegAdd	    : 8;
        unsigned int 		RegVal	    : 8;
    } un;
    unsigned int CompletField;
} t_AckMb5;


///////////////////////////////////////////////////////////////////////////////
// AckMb6
//          type : Service Acknowledge Mailbox
//          associated IT : prcm_arm_it1_val[6]
//          communication direction : prcmu (write) -> arm (read)
//          width (Bytes) : 4
//          multiservice (using header) : false
//          service(s) : <unused>
///////////////////////////////////////////////////////////////////////////////

typedef struct {
    unsigned char  byteFill[4];
} t_AckMb6;


///////////////////////////////////////////////////////////////////////////////
// ReqMb7
//          type : Service Request Mailbox
//          associated IT : prcm_mbox_cpu_set[7] -> xp70 IT 19
//          communication direction : SiaMmdsp (write) -> prcmu (read)
//          width (Bytes) : 4
//          multiservice (using header) : true (disjoint/shared)
//          service(s) : -1.1- SiaMmdsp completion task (unused Reqmb7)
//                       -1.2- Configure fifo4500it signal as wake up event
//                       -2- Geni2c access
///////////////////////////////////////////////////////////////////////////////

typedef struct {
    
    union {
        struct {
            unsigned char byteFill1[2];
            //---------------------------- -1.2- case ReqMb7H.header == Fifo4500ItCfgH
            t_OnOffSt Fifo4500ItCfg : 8;
            //----------------------------
            unsigned char byteFill2[1];
        } Sleep_4500It;
        
        //---------------------------- -2- case ReqMb7H.header == GenI2cH
        struct {
            t_ReqMb5  Req;
        } GenI2C1;
        
        struct {
            t_AckMb5  Ack;
        } GenI2C2;
    } un;
} t_ReqMb7;


///////////////////////////////////////////////////////////////////////////////
// AckMb7
//          type : Service Acknowledge Mailbox
//          associated IT : prcm_arm_it1_val[7]
//          communication direction : prcmu (write) -> arm (read)
//          width (Bytes) : 4
//          multiservice (using header) : false
//          service(s) : system reset and IPC
///////////////////////////////////////////////////////////////////////////////

typedef struct {
    t_AckMb7Status AckMb7Status : 8;
    unsigned char  byteFill[3];
} t_AckMb7;

///////////////////////////////////////////////////////////////////////////////
// romcode_mbox
//          type : /
//          associated IT : /
//          communication direction : 	romcode (write) -> prcmu (read) or 
//										prcmu (write) -> romcode (read)
//          width (Bytes) : 4
//          multiservice (using header) : false
//          service(s) : used to exchange information between romcode and prcmufw
///////////////////////////////////////////////////////////////////////////////

typedef union {
    struct {
        t_RomcodeMb Msg	 			: 8;
        t_TState 	DmaHashOnGoing	: 8;
        t_TState	DmaCrypOnGoing	: 8;
        unsigned int SaveSecMemory	: 8;
    } un;
    unsigned int CompletField;
} t_romcode_mbox;


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////// TCPM & TCDM Memory Map /////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

typedef struct {
	unsigned int    GPIO_RstSettings;   // B4 + 0xDDC	(	4	bytes)       
	t_ShVar4     	ShVar4;       	    // B4 + 0xDE0	(	4	bytes)   
	t_ShVar3     	ShVar3;       	    // B4 + 0xDE4	(	4	bytes)
	t_ShVar2     	ShVar2;       	    // B4 + 0xDE8	(	4	bytes)
  	t_AckMb7     	AckMb7;       	    // B4 + 0xDEC	(	4	bytes)
	t_AckMb6     	AckMb6;       	    // B4 + 0xDF0	(	4	bytes)
	t_AckMb5     	AckMb5;       	    // B4 + 0xDF4	(	4	bytes)
	t_AckMb4     	AckMb4;       	    // B4 + 0xDF8	(	4	bytes)
	t_AckMb3     	AckMb3;       	    // B4 + 0xDFC	(	4	bytes)
	t_AckMb2     	AckMb2;       	    // B4 + 0xE00	(	4	bytes)
	t_AckMb1     	AckMb1;       		// B4 + 0xE04	(	4	bytes)
	t_AckMb0     	AckMb0;       		// B4 + 0xE08	(	52	bytes)
	t_ReqMb7     	ReqMb7;       		// B4 + 0xE3C	(	4	bytes)
	t_ReqMb6     	ReqMb6;       		// B4 + 0xE40	(	4	bytes)
	t_ReqMb5     	ReqMb5;       		// B4 + 0xE44	(	4	bytes)
	t_ReqMb4     	ReqMb4;       		// B4 + 0xE48	(	4	bytes)
	t_ReqMb3     	ReqMb3;       		// B4 + 0xE4C	(	372	bytes)
	t_ReqMb2     	ReqMb2;       		// B4 + 0xFC0	(	16	bytes)
	t_ReqMb1     	ReqMb1;       		// B4 + 0xFD0	(	12	bytes)
	t_ReqMb0     	ReqMb0;       		// B4 + 0xFDC	(	12	bytes)
	t_Header     	Header;       		// B4 + 0xFE8	(	16	bytes)
	unsigned int 	Reserved_NVM; 		// B4 + 0xFF8	(	4	bytes)
	t_ShVar1     	ShVar1;       		// B4 + 0xFFC	(	4	bytes)
} t_nSecMap; // Total 532	bytes				


typedef struct {
    t_DDRInit DDRInit;
} t_SecMap; // Total 	464	bytes


typedef struct s_initTCDM {
    unsigned int RamData[1024];
} s_initTCDM;


typedef struct{
    unsigned char MMDSPData[136];
} t_MMDSP;


// romcode communication field in secure TCPM
#define romcode_mb  ((volatile t_RomcodeMb *) (TCPM_BASE + 0x28))
#define romcode_mb_arm  ((volatile t_RomcodeMb *) (TCPM_BASE + 0xa0))

// shared variables in secure TCDM section (begin of TCDM)
#define TCDMsec     ((volatile t_SecMap *) (TCDM_BASE + 0x0))

// shared variables and mailboxes in non-secure TCDM section (end of TCDM)
#define TCDMnSec    ((volatile t_nSecMap *) (TCDM_BASE + 0xDDC))


#endif /* !defined(XP70_MEMMAP_H) */


// Details abouts addresses and sizes meant for people in validation
// and people coding on rtl testbenches.

/*

OUT>                 Types: Size
OUT> ---------------------------------------------------------------------------------
OUT>             t_nSecMap: 532
OUT>              t_SecMap: 464
OUT>             t_ReqMb0H:   4
OUT>         t_8500HwEvent:   4
OUT>         t_4500HwEvent:   4
OUT>              t_ReqMb3: 372
OUT>   t_SiaSvaPwrOnEnable:   1
OUT>       t_SiaSvaAutoPwr:   4
OUT>             t_autoPwr:   8
OUT>          t_WkUpReason:  24
OUT> 
OUT>           t_RomcodeMb:   4
OUT>             t_DDRInit: 464
OUT> 
OUT>              t_ShVar1:   4
OUT>                   int:   4
OUT>              t_Header:  16
OUT>              t_ReqMb0:  12
OUT>              t_ReqMb1:  12
OUT>              t_ReqMb2:  16
OUT>              t_ReqMb3: 372
OUT>              t_ReqMb4:   4
OUT>              t_ReqMb5:   4
OUT>              t_ReqMb6:   4
OUT>              t_ReqMb7:   4
OUT>              t_AckMb0:  52
OUT>              t_AckMb1:   4
OUT>              t_AckMb2:   4
OUT>              t_AckMb3:   4
OUT>              t_AckMb5:   4
OUT>              t_AckMb7:   4
OUT>              t_ShVar2:   4
OUT>              t_ShVar3:   4
OUT>              t_ShVar4:   4
OUT>              t_uint32:   4
OUT> 
OUT> 
OUT> Members of aggregate types                              Address   
OUT> ---------------------------------------------------------------------------------
OUT> vTCDMnSec                                                    0x00000DE8
OUT> vTCDMnSec.ShVar1                                             0x00000FFC
OUT> vTCDMnSec.ShVar1.boot_status_debug_var                       0x00000FFF
OUT> vTCDMnSec.Reserved_NVM                                       0x00000FF8
OUT> vTCDMnSec.Header                                             0x00000FE8
OUT> vTCDMnSec.Header.Req                                         0x00000FE8
OUT> vTCDMnSec.Header.Req.Mb[0]                                   0x00000FE8
OUT> vTCDMnSec.Header.Req.Mb[1]                                   0x00000FE9
OUT> vTCDMnSec.Header.Req.Mb[2]                                   0x00000FEA
OUT> vTCDMnSec.Header.Req.Mb[3]                                   0x00000FEB
OUT> vTCDMnSec.Header.Req.Mb[4]                                   0x00000FEC
OUT> vTCDMnSec.Header.Req.Mb[5]                                   0x00000FED
OUT> vTCDMnSec.Header.Req.Mb[6]                                   0x00000FEE
OUT> vTCDMnSec.Header.Req.Mb[7]                                   0x00000FEF
OUT> vTCDMnSec.Header.Ack                                         0x00000FF0
OUT> vTCDMnSec.Header.Ack.Mb[0]                                   0x00000FF0
OUT> vTCDMnSec.Header.Ack.Mb[1]                                   0x00000FF1
OUT> vTCDMnSec.Header.Ack.Mb[2]                                   0x00000FF2
OUT> vTCDMnSec.Header.Ack.Mb[3]                                   0x00000FF3
OUT> vTCDMnSec.Header.Ack.Mb[4]                                   0x00000FF4
OUT> vTCDMnSec.Header.Ack.Mb[5]                                   0x00000FF5
OUT> vTCDMnSec.Header.Ack.Mb[6]                                   0x00000FF6
OUT> vTCDMnSec.Header.Ack.Mb[7]                                   0x00000FF7
OUT> vTCDMnSec.ReqMb0                                             0x00000FDC
OUT> vTCDMnSec.ReqMb0.PwrStTr                                     0x00000FDC
OUT> vTCDMnSec.ReqMb0.WkUp8500                                    0x00000FE0
OUT> vTCDMnSec.ReqMb0.WkUp4500                                    0x00000FE4
OUT> vTCDMnSec.ReqMb1                                             0x00000FD0
OUT> vTCDMnSec.ReqMb1.dvfs                                        0x00000FD0
OUT> vTCDMnSec.ReqMb1.apeOpp100Restore                            0x00000FD4
OUT> vTCDMnSec.ReqMb1.armOpp100Restore                            0x00000FD8
OUT> vTCDMnSec.ReqMb2                                             0x00000FC0
OUT> vTCDMnSec.ReqMb2.Hw.Acc[0]                                   0x00000FC0
OUT> vTCDMnSec.ReqMb2.Hw.Acc[1]                                   0x00000FC1
OUT> vTCDMnSec.ReqMb2.Hw.Acc[2]                                   0x00000FC2
OUT> vTCDMnSec.ReqMb2.Hw.Acc[3]                                   0x00000FC3
OUT> vTCDMnSec.ReqMb2.Hw.Acc[4]                                   0x00000FC4
OUT> vTCDMnSec.ReqMb2.Hw.Acc[5]                                   0x00000FC5
OUT> vTCDMnSec.ReqMb2.Hw.Acc[6]                                   0x00000FC6
OUT> vTCDMnSec.ReqMb2.Hw.Acc[7]                                   0x00000FC7
OUT> vTCDMnSec.ReqMb2                                             0x00000FC0
OUT> vTCDMnSec.ReqMb2.autoPwr                                     0x00000FC8
OUT> vTCDMnSec.ReqMb2.autoPwr.apSleepSiaSvaAutoPwr                0x00000FC8
OUT> vTCDMnSec.ReqMb2.autoPwr.apSleepSiaSvaAutoPwr.siaPwrOnEnable 0x00000FC9
OUT> vTCDMnSec.ReqMb2.autoPwr.apSleepSiaSvaAutoPwr.svaPwrOnEnable 0x00000FCA
OUT> vTCDMnSec.ReqMb2.autoPwr.apIdleSiaSvaAutoPwr                 0x00000FCC
OUT> vTCDMnSec.ReqMb2.autoPwr.apIdleSiaSvaAutoPwr.siaPwrOnEnable  0x00000FCD
OUT> vTCDMnSec.ReqMb2.autoPwr.apIdleSiaSvaAutoPwr.svaPwrOnEnable  0x00000FCE
OUT> vTCDMnSec.ReqMb3                                             0x00000E4C
OUT> vTCDMnSec.ReqMb3.ANC                                         0x00000E4C
OUT> vTCDMnSec.ReqMb3.ANC.ANCFIRCoeff                             0x00000E4C
OUT> vTCDMnSec.ReqMb3.ANC.ANCIIRCoeff                             0x00000E6A
OUT> vTCDMnSec.ReqMb3.ANC.ANCShifter                              0x00000EBA
OUT> vTCDMnSec.ReqMb3.sidetone                                    0x00000EBE
OUT> vTCDMnSec.ReqMb3.sidetone.FIRCoeff                           0x00000EBE
OUT> vTCDMnSec.ReqMb4                                             0x00000E48
OUT> vTCDMnSec.ReqMb4.un.memSt                                    0x00000E48
OUT> vTCDMnSec.ReqMb4.un.autoSrefresh                             0x00000E48
OUT> vTCDMnSec.ReqMb4.un.autoSrefresh.lowPower_internal_cnt       0x00000E48
OUT> vTCDMnSec.ReqMb4.un.autoPwrDn                                0x00000E48
OUT> vTCDMnSec.ReqMb4.un.autoPwrDn.Lowpower_power_down_cnt        0x00000E48
OUT> vTCDMnSec.ReqMb4.un.partialSrefresh                          0x00000E48
OUT> vTCDMnSec.ReqMb4.un.partialSrefresh.MR16                     0x00000E48
OUT> vTCDMnSec.ReqMb4.un.partialSrefresh.MR17                     0x00000E4A
OUT> vTCDMnSec.ReqMb4.un.csPwrMgt                                 0x00000E48
OUT> vTCDMnSec.ReqMb4.un.SysClkMgt                                0x00000E48
OUT> vTCDMnSec.ReqMb5                                             0x00000E44
OUT> vTCDMnSec.ReqMb5.CompletField                                0x00000E44
OUT> vTCDMnSec.ReqMb6                                             0x00000E40
OUT> vTCDMnSec.ReqMb7                                             0x00000E3C
OUT> vTCDMnSec.AckMb0                                             0x00000E08
OUT> vTCDMnSec.AckMb0.WkUpR_ping                                  0x00000E0C
OUT> vTCDMnSec.AckMb0.WkUpR_ping.WkUp8500                         0x00000E0C
OUT> vTCDMnSec.AckMb0.WkUpR_ping.WkUp4500                         0x00000E10
OUT> vTCDMnSec.AckMb0.WkUpR_pong                                  0x00000E24
OUT> vTCDMnSec.AckMb0.WkUpR_pong.WkUp8500                         0x00000E24
OUT> vTCDMnSec.AckMb0.WkUpR_pong.WkUp4500                         0x00000E28
OUT> vTCDMnSec.AckMb1                                             0x00000E04
OUT> vTCDMnSec.AckMb2                                             0x00000E00
OUT> vTCDMnSec.AckMb3                                             0x00000DFC
OUT> vTCDMnSec.AckMb5                                             0x00000DF4
OUT> vTCDMnSec.AckMb5.CompletField                                0x00000DF4
OUT> vTCDMnSec.AckMb6                                             0x00000DF0
OUT> vTCDMnSec.AckMb7                                             0x00000DEC
OUT> vTCDMnSec.ShVar2                                             0x00000DE8
OUT> vTCDMnSec.ShVar3                                             0x00000DE4
OUT> vTCDMnSec.ShVar4                                             0x00000DE0
OUT> vTCDMnSec.GPIO_RstSettings                                   0x00000DDC
OUT> 
OUT> vTCDMSec                                                     0x00000000
OUT> vTCDMSec.DDRInit                                             0x00000000
OUT> 
OUT> vRomcodeMb                                                   0x004000A0

*/
