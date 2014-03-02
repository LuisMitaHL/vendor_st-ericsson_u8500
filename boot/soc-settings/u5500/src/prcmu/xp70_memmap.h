#ifndef INCLUSION_GUARD_XP70_MEMMAP_H
#define INCLUSION_GUARD_XP70_MEMMAP_H

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
 * Author(s)   : Per-Inge TALLBERG                                            *
 *               Ulf MORLAND                                                  *
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

#include "t_basicdefinitions.h"
#include "numerics.h"



// definition of base addresses
#define NONSEC_PRCMU_REG_BASE 0x80157000 // B1 (in spec)
#define SEC_PRCMU_REG_BASE    0x80158000 // B2 (in spec)
#define TCPM_BASE             0x80160000 // B3 (in spec)
#define TCDM_BASE             0x80168000 // B4 (in spec)
#define TCDM_SIZE             0x5000   //(in spec)

// 8 bit Product (Maja v1 == 0, Maja v2 == 0x20),
// 24 bit sequence number
#define API_VERSION           (0x20000011)

// In order to optimize memory space the headers are grouped.
struct header {

    struct {
        u8      mb0;        // shall be assigned from enum req_mb_0_h
        u8      mb1;        // shall be assigned from enum req_mb_1_h
        u8      mb2;        // shall be assigned from enum req_mb_2_h
        u8      mb3;        // shall be assigned from enum req_mb_3_h
        u8      mb4;        // shall be assigned from enum req_mb_4_h
        u8      mb5;        // shall be assigned from enum req_mb_5_h
        u8      mb6;        // shall be assigned from enum req_mb_6_h
        u8      mb7;        // shall be assigned from enum req_mb_7_h
    } req;

    struct {
        u8      mb0;        // shall be assigned from enum ack_mb_0_h
        u8      mb1;        // shall be assigned from enum ack_mb_1_h
        u8      mb2;        // shall be assigned from enum req_mb_2_h
        u8      mb3;        // shall be assigned from enum req_mb_3_h
        u8      mb4;        // shall be assigned from enum ack_mb_4_h
        u8      mb5;        // shall be assigned from enum ack_mb_5_h
        u8      mb6;        // shall be assigned from enum ack_mb_6_h
        u8      mb7;        // shall be assigned from enum ack_mb_7_h
    } ack;
};


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////// List of the shared variables //
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////



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

// ApPwrSt pwr_current_state_var;

struct pwr_current_state_var {
    u8  pwr_current_state_var;  // assigned with values from enum ap_pwr_st
    u8  byte_fill[2];
    u8  boot_status_debug_var;  //
};



///////////////////////////////////////////////////////////////////////////////
// DDRInit
//
//          type : Shared Variable
//          associated IT : prcm_mbox_cpu_set[4] -> xp70 IT 16
//          communication direction : arm (Wr) -> prcmu (Rd)
//          width (Bytes) : 5*4+111*4
//          multiservice (using header) : n/a
//          service(s) : - Initialization of DDR Controller and PHY
///////////////////////////////////////////////////////////////////////////////

struct ddr_init {
    u32 MaskCfg0[5];
    u32 Cfg0[149];
};

struct ddr_init_mrr {
    u8 cfgadd[8];
    struct
    {
        u8 data_cs[8];
    } cs[2];
} ;

struct abb_init {
    u32     number_of_entries;
    struct {
        u8 slave_address;
        u8 reg_address;    // 0 = ignore
        u8 data;
    } regulator[80];
};

struct fuse_init {
    u32 temp;                   // Fuse are
};

struct memory_speed {
    u32 frequency;              // assign with values from enum ddr_frequency
};

struct frequency {
    u32 max_cpu_freq;           // assign with value from enum cpu_max_frequency
    u32 max_aclk_freq;          // assign with value from enum aclk_max_frequency
    u32 max_sga_freq;           // assign with value from enum sga_max_frequency
    u32 spare[12];
};

struct avs_abb_reg_setting {
    u8  abb_reg_setting[8];     // when assign with abb regulator register values, use index from enum AVS_...
                                // size 8 byte (2 32-bit words) allows future adding of more opp:s
                                //without change of mem map.
};

struct freq_ramp_up {
	u16 step;	// pulse increment
	u16 delay;	// delay between each increement
};

struct avs {
    struct avs_abb_reg_setting varm;    // AVS for A9, VARM. Use enum avs_varm_opp as index
    struct avs_abb_reg_setting vape;    // AVS for APE, VAPE. Use enum avs_vape_opp as index
    struct avs_abb_reg_setting vsafe;   // AVS for ALWAYS ON (VSafe). Use enum avs_vsafe_opp as index
    struct freq_ramp_up a9_ramp_up;	// a9 freq ramp up data, how fast and in what interval
};


struct wk_up_reason {
  u32   dbb_event;      // assigned with values from enum dbb_event

  u8    abb_event[32];
};


struct lpa_buffer {
    u32 status;             // assigned with values from enum lpa_status
    u32 read_ix;            // read index into .data[] (prcmu)
    u32 write_ix;           // write index into .data[] {arm)
    u32 element_size;       // number of u32 per element (expected to be 1=32 bits or 2=64 bits) (arm)
    u32 length;             // an even number of u32 (prcmu)
    u32 watermark;          // fire "need more data" interrupt when (read_ix+watermark % length)==write_ix is true (arm)
    u32 data[1];            // the data
};

struct share_info {
    u32 spare[15];
    u16 dbg_marker;
    u16 CurrentTemp;        // Current temperature in frequency, is updated every measure period
                            // (i.e. latest temp read from sensor)
};

struct thermal_sensor_data {
    u16 SensorMaxThresholdTemp;             // temperature limit for sensor (physical)
    u16 SensorAlertThresholdTemp;           // temperature limit for restart/reboot
    u16 SensorMaxPeriod;                    // max measuring period in ms (measuring atleast every X)
    u16 SensorMinPeriod;                    // min measuring period in ms
    u16 SensorMaxTempRange;                 // prcmu internal, max temp limit for TEMP_HIGH event
    u16 SensorMinTempRange;                 // prcmu internal, min temp limit for TEMP_LOW event
    u16 SensorCurrentTemp;                  // prcmu internal, current temperature (last read from sensor)
    u16 SensorMeasPeriod;                   // prcmu internal, current period in ms (MinPeriod < this < MaxPeriod)
};


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
//          width (Bytes) : 16
//          multiservice (using header) : true (disjoint)
//          service(s) : -1- power state transition requests
//                       -2- configuration of the wake up events (no ack)
//                       -3- wake-up reason read acknowledge (no ack)
///////////////////////////////////////////////////////////////////////////////

// List of the DBB relevant events to be used as a mask for wake-up config.


struct req_mb_0 {

    //---------------------------- -1- case header.req.mb0 == RMB0H_PWR_STATE_TRANS

    u8  ap_pwr_st_tr;               // assign with values from enum ap_pwr_st_tr
    u8  ap_ulp_pll_st;              // assign with values from enum on_off_st, respect in all states (even DS)
    u8  ap_pll_st_idle;             // assign with values from enum on_off_st, applicable only in idle
    u8  ddr_st_idle;                // assign with values from enum ddr_pwr_st
    u8  esram0_st;                  // assign with values from enum off_retention_st

    u8  bytefill[3];
    //---------------------------- -2- case header.req.mb0 == RMB0H_WAKE_UP_CFG

    u32 dbb_event;                  // assign with values from enum dbb_event
    u32 abb_event_reg;              // assign with values from enum abb_event_reg

    //---------------------------- -3- case header.req.mb0 == RMB0H_RD_WAKE_UP_ACK

};



///////////////////////////////////////////////////////////////////////////////
// AckMb0
//          type : Service Acknowledge Mailbox
//          associated IT : prcm_arm_it1_val[0]
//          communication direction : prcmu (write) -> arm (read)
//          width (Bytes) : 76
//          multiservice (using header) : true (disjoint)
//          service(s) : -0- wake-up reason transfer from low power states
///////////////////////////////////////////////////////////////////////////////

struct ack_mb_0 {

    // This variable is always updated before the req mb0 it is deasserted by PRCMU
    u8  ap_pwr_st_tr_status;    // assigned with values from enum ap_pwr_st_tr_status;

    //---------------------------- -0- case header.ack.mb0 == AMB0H_WAKE_UP

    // Read pointer to the ping pong mailbox used by arm
    // usage :
    // when the arm receives the IT of the wake-up reason, it reads
    // the rd_ix. The rd_ix (0 or 1) is then used as an index in .wk_up_reason
    // and points out ping (=0) or pong (=1). The wake up events are stored in
    // .wk_up_reason[ .rd_ix ]
    u8  rd_ix;                  // assigned with values from enum ping_pong

    u8  bytefill[2];
    // Hardware event fields
    // Fields in which the prcmu stores the dbb and abb hardware events
    // before transferring them to the arm via the IT associated to AckMb0.
    // Two fields have been defined in ordre to implement the pingpong mechanism.
    // wk_up_reason[0] corresp. to ping field
    // wk_up_reason[1] corresp. to pong field
    // Selection by arm : WkUpR[0x1 & rd_ix]
    struct wk_up_reason wk_up_reason[2];

};


///////////////////////////////////////////////////////////////////////////////
// ReqMb1
//          type : Service Request Mailbox
//          associated IT : prcm_mbox_cpu_set[1] -> xp70 IT 11
//          communication direction : arm (write) -> prcmu (read)
//          width (Bytes) : 12
//          multiservice (using header) : true (disjoint)
//          service(s) : -1- ARM operating point task acknowledge
//                       -2- APE operating point task acknowledge
//                       -3- ARM and APE operating point task acknowledge
//                       -4- Restore 100 OPP on ARM
//                       -5- Restore 100 OPP on APE
///////////////////////////////////////////////////////////////////////////////

struct req_mb_1 {

    //---------------------------- -1- case header.req.mb1 == MB1H_ARM_OPP
    //---------------------------- -3- case header.req.mb1 == MB1H_ARM_APE_OPP
    u8  arm_opp;                    // assign with values from enum arm_opp

    //---------------------------- -2- case header.req.mb1 == MB1H_APE_OPP
    //---------------------------- -3- case header.req.mb1 == MB1H_ARM_APE_OPP
    u8  ape_opp;                    // assign with values from enum ape_opp
    u8  byte_fill[2+4+4];           // padding
};

///////////////////////////////////////////////////////////////////////////////
// AckMb1
//          type : Service Acknowledge Mailbox
//          associated IT : prcm_arm_it1_val[1]
//          communication direction : prcmu (write) -> arm (read)
//          width (Bytes) : 8
//          multiservice (using header) : false
//          service(s) : -1- ARM operating point task acknowledge
//                       -2- APE operating point task acknowledge
//                       -3- ARM and APE operating point task acknowledge
//                       -4- Restore 100 OPP on ARM
//                       -5- Restore 100 OPP on APE
///////////////////////////////////////////////////////////////////////////////

struct ack_mb_1 {
    //---------------------------- -1- case header.ack.mb1 == MB1H_ARM_OPP
    //---------------------------- -2- case header.ack.mb1 == MB1H_APE_OPP
    //---------------------------- -3- case header.ack.mb1 == MB1H_ARM_APE_OPP
    u8  cur_arm_opp;                // assigned with values from enum arm_opp (MB1H_ARM_OPP, MB1H_ARM_APE_OPP)
    u8  cur_ape_opp;                // assigned with values from enum ape_opp (MB1H_APE_OPP, MB1H_ARM_APE_OPP)
    u8  arm_status;                 // assigned with values from enum return_code(MB1H_ARM_OPP, MB1H_ARM_APE_OPP)
    u8  ape_status;                 // assigned with values from enum return_code (MB1H_APE_OPP, MB1H_ARM_APE_OPP)
    u8  arm_req_pending;            //
    //---------------------------- -4- case header.ack.mb1 == MB1H_ARM_RESTORE_100_OPP
    //---------------------------- -5- case header.ack.mb1 == MB1H_APE_RESTORE_100_OPP
    u8 return_code;                 // assigned with values from enum return_code
    u8 byte_fill[2];                // padding
};


///////////////////////////////////////////////////////////////////////////////
// ReqMb2
//          type : Service Request Mailbox
//          associated IT : prcm_mbox_cpu_set[2] -> xp70 IT 12
//          communication direction : arm (write) -> prcmu (read)
//          width (Bytes) : 8
//          multiservice (using header) : true (shared)
//          service(s) : -1- Power
//                       -2- Clock
//                       -3- PLL
//
///////////////////////////////////////////////////////////////////////////////

struct req_mb_2 {

    //---------------------------- -1- case header.req.mb2 == MB2H_PWR_REQ
    u8  pwr_client;                 // assign with values from enum pwr_client
    u8  pwr_st;                     // assign with values from enum on_off_st except
                                    // for esram12 which should be assigned with
                                    // values from enum on_off_ret_st.
                                    // Requested esram12 | Vape | Actual esram12
                                    // state             |      | state
                                    // -----------------------------------------
                                    // Off               | Off  | Off
                                    // Ret               | Off  | Ret
                                    // On                | Off  | Ret
                                    // Off               | On   | on, but not accessible
                                    // Ret               | On   | on, but not accessible
                                    // On                | On   | on, accessible

    //---------------------------- -2- case header.req.mb2 == MB2H_CLK_REQ
    u8  clk_client;                 // assign with values from enum clk_client
    u8  clk_st;                     // assign with values from enum on_off_st

    //---------------------------- -3- case header.req.mb2 == MB2H_PLL_REQ
    u8  pll;                        // assign with values from enum pll
    u8  pll_st;                     // assign with values from enum on_off_st

    u8  byte_fill[2];
};

///////////////////////////////////////////////////////////////////////////////
// ACKMb2
//          type : Service Acknowledge Mailbox
//          associated IT : prcm_arm_it1_val[2]
//          communication direction : prcmu (write) -> arm (read)
//          width (Bytes) : 12
//          multiservice (using header) : true
//          service(s) : -1- Power
//                       -2- Clock
//                       -3- PLL
//
///////////////////////////////////////////////////////////////////////////////


struct ack_mb_2 {

    //---------------------------- -1- case header.ack.mb2 == MB2H_PWR_REQ
    u8  pwr_client;                 // assigned with values from enum pwr_client
    u8  pwr_st;                     // assigned with values from enum on_off_st
    u8  pwr_rc;                     // assigned with values from enum return_code
    u8  byte_fill1;                 //padding

    //---------------------------- -2- case header.ack.mb2 == MB2H_CLK_REQ
    u8  clk_client;                 // assigned with values from enum clk_client
    u8  clk_st;                     // assigned with values from enum on_off_st
    u8  clk_rc;                     // assigned with values from enum return_code
    u8  byte_fill2;                 //padding
    //---------------------------- -3- case header.ack.mb2 == MB2H_PLL_REQ
    u8  pll;                        // assigned with values from enum pll
    u8  pll_st;                     // assigned with values from enum on_off_st
    u8  pll_rc;                     // assigned with values from enum return_code
    u8  pll_req_pending;            // pll request pending
};


///////////////////////////////////////////////////////////////////////////////
// ReqMb3
//          type : Service Request Mailbox
//          associated IT : prcm_mbox_cpu_set[3] -> xp70 IT 13
//          communication direction : arm (write) -> prcmu (read)
//          width (Bytes) : 4
//          multiservice (using header) : true
//          service(s) : -1- RefClk
//
///////////////////////////////////////////////////////////////////////////////

struct req_mb_3 {

    //---------------------------- -1- case header.req.un.mb2 == MB3H_REF_CLK_REQ
    u8  ref_clk_st;                 // assign with values from enum on_off_st
    u8  byte_fill[3];
};

///////////////////////////////////////////////////////////////////////////////
// ACKMb3
//          type : Service Acknowledge Mailbox
//          associated IT : prcm_arm_it1_val[2]
//          communication direction : prcmu (write) -> arm (read)
//          width (Bytes) : 4
//          multiservice (using header) : true
//          service(s) : -1- RefClk
//
///////////////////////////////////////////////////////////////////////////////


struct ack_mb_3 {

    //---------------------------- -1- case header.ack.mb3 == MB3H_REF_CLK_REQ
    u8  ref_clk_st;                 // assign with values from enum on_off_st
    u8  ref_clk_req_pending;        // pending request
    u8  byte_fill[2];
};



///////////////////////////////////////////////////////////////////////////////
// ReqMb4
//          type : Service Request Mailbox
//          associated IT : prcm_mbox_cpu_set[4] -> xp70 IT 16
//          communication direction : arm (write) -> prcmu (read)
//          width (Bytes) : 40
//          multiservice (using header) : true (shared)
//          service(s) : -1- DDR Management
//                          -1.1-  initialization of ddr
//                          -1.2-  auto SR watermark
//                          -1.3-  auto LP watermark (DDR power down)
//                          -1.4-  partial self refresh cs1 (cs0 not supported)
//                          -1.5-  deep power down cs1 (cs0 not supported)
//                          -1.6-  QoS Port priority and bandwidth
//                          -1.7-  Pre initialization of ddr (MMR reading)
//                       -2- Temperature
//                          -2.1-  Hotdog. Platform will be shutdown at this temperature
//                          -2.2-  Hotmon. Platform will be informed to be able to perform proper action.
//                          -2.3-  Hotperiod. P
//                       -3- ABB
//                          -3.1-  ABB Init. Configuration from meminit.abb_init will be transmitted to abb.
//                          -3.2-  AVS_ACTIVATE. Avs structure is copied to an internal variable.
//                                 The settings are propagated into the abb regulators.
///////////////////////////////////////////////////////////////////////////////

struct req_mb_4 {

    //---------------------------- -1.1- case header.req.mb4 == RMB4H_DDR_INIT

    // configure the DDR controller and PHY by copying DDRInit shared variable from mem init


    //---------------------------- -1.2- case header.req.mb4 == RMB4H_CFG_AUTO_SR_WATERMARK
    // = mode 5 of the DDR Controller
    // cycle count before DDR automatically goes in self-refresh
    // No answer
    u16 auto_sr_cycle_watermark;

    u8  byte_fill1[2];

    //---------------------------- -1.3- case header.req.mb4 == RMB4H_CFG_AUTO_LP_WATERMARK
    // = mode 2 of the DDR Controller
    // cycle count before DDR automatically goes in power down (LP), not Deep Power Down DPD
    // No answer
    u16 auto_lp_cycle_watermark;

    u8  byte_fill2[2];

    //---------------------------- -1.4- case header.req.mb4 == RMB4H_CFG_PASR_CS1
    // variable defining the MR16 and MR17 value for CS1 (CS0 is not supported)
    // Answer
    u16 mr16_cs1;
    u16 mr17_cs1;

    //---------------------------- -1.5- case header.req.mb4 == RMB4H_CFG_DPD_CS1
    // Answer
    u8  ddr_dpd_st_cs1;             // assign with values from enum ddr_dpd_st
    u8  byte_fill3[3];

    //---------------------------- -1.6- case header.req.mb4 == RMB4H_CFG_QOS_PORT
    // Answer
    u8  port_prio_wr[4];            // assign with wr priority (2 bits)
    u8  port_prio_rd[4];            // assign with rd priority (2 bits)
    u8  port_bw[4];                 // assign with band width (0-100%, 7 bits)
    u8  port_mask;                  // assign with bit mask. Ex. bit0 and bit2 true,  means that data for index 0 and 2 contains valid data (to be written).
    u8  byte_fill4[3];              // padding
    //---------------------------- -1.7- case header.req.mb4 == RMB4H_DDR_PRE_INIT
    //
    //---------------------------- -2.1- case header.req.mb4 == RMB4H_CFG_HOTDOG
    u16 hotdog;                     // assign with hotdog temperature given in frequency
    u16 hotperiod;                  // assign with number of millisec between temp reads.
    //---------------------------- -2.2- case header.req.mb4 == RMB4H_CFG_HOTMON
    u16  hotmon_min;                //assign with hotdog min temperature given in frequency
    u16  hotmon_max;                //assign with hotdog max temperature given in frequency
    //---------------------------- -3.1- case header.req.mb4 == RMB4H_ABB_INIT
    // configure the ABB with regulator setting and DBB on. Default written by PRCMU FW at fw boot, and possibly updated
    // by A9 before requesting ABB_INIT is so needed.
    //---------------------------- -3.2- case header.req.mb4 == RMB4H_AVS_ACTIVATE
    // enable avs settings, force update of regulators.
    //---------------------------- -1.7- case header.req.mb4 == RMB4H_DDR_PRE_INIT
};


///////////////////////////////////////////////////////////////////////////////
// AckMb4
//          type : Service Acknowledge Mailbox
//          associated IT : prcm_arm_it1_val[4]
//          communication direction : prcmu (write) -> arm (read)
//          width (Bytes) : 4
//          multiservice (using header) : false
//          service(s) : <unused>
///////////////////////////////////////////////////////////////////////////////
struct ack_mb_4 {

    //---------------------------- -1- case header.ack.mb4 == AMB4H_DDR_INIT
    //---------------------------- -2- case header.ack.mb4 == AMB4H_PASR_CS1,
    //---------------------------- -3- case header.ack.mb4 == AMB4H_DPD_CS1,
    //---------------------------- -4- case header.ack.mb4 == AMB4H_QOS_PORT,
    //---------------------------- -5- case header.ack.mb4 == AMB4H_CFG_HOTDOG,
    //---------------------------- -6- case header.ack.mb4 == AMB4H_CFG_HOTMON,
    //---------------------------- -7- case header.ack.mb4 == AMB4H_ABB_INIT
    //---------------------------- -8- case header.ack.mb4 == AMB4H_DDRPREINIT
    //---------------------------- -9- case header.ack.mb4 == AMB4H_AVS_ACTIVATE

    u8  rc;                         // assigned with values from enum return_code

    u8  byte_fill[3];
};




///////////////////////////////////////////////////////////////////////////////
// ReqMb5
//          type : Service Request Mailbox
//          associated IT : prcm_mbox_cpu_set[5] -> xp70 IT 17
//          communication direction : arm (write) -> prcmu (read)
//          width (Bytes) : 8
//          multiservice (using header) : false
//          service(s) : generic I2C transfer
///////////////////////////////////////////////////////////////////////////////

struct req_mb_5 {
    //
    //---------------------------- -1- case header.req.mb5 == RMB5H_I2C_WRITE
    //---------------------------- -2- case header.req.mb5 == RMB5H_I2C_READ
    //
    u8  slave_addr;                 // 7 bit;
    u8  reg_addr;                   // 8 bit reg
    u8  data_len;                   // 1-4 bytes possible to write or read

    u8  byte_fill;                  // padding

    //---------------------------- -1- only for case header.req.mb5 == RMB5H_I2C_WRITE
    u8  data[4];
};


///////////////////////////////////////////////////////////////////////////////
// AckMb5
//          type : Service Acknowledge Mailbox
//          associated IT : prcm_arm_it1_val[5]
//          communication direction : prcmu (write) -> arm (read)
//          width (Bytes) : 8
//          multiservice (using header) : false
//          service(s) : <unused>
///////////////////////////////////////////////////////////////////////////////

struct ack_mb_5 {
    //---------------------------- -1- case header.ack.mb5 == RMB5H_I2C_WRITE
    //---------------------------- -2- case header.ack.mb5 == RMB5H_I2C_READ
    //
    u8  return_code;                // assigned with a value from enum return_code
    u8  byte_fill[3];               //padding
    //---------------------------- -2- case header.ack.mb5 == RMB5H_I2C_READ
    u8  data[4];

};


///////////////////////////////////////////////////////////////////////////////
// ReqMb6
//          type : Service Request Mailbox
//          associated IT : prcm_mbox_cpu_set[6] -> xp70 IT 18
//          communication direction : arm (write) -> prcmu (read)
//          width (Bytes) : 4
//          multiservice (using header) : true
//          service(s) :  low power audio play back
///////////////////////////////////////////////////////////////////////////////

struct req_mb_6 {
    //
    //---------------------------- -1- case header.req.mb6 == RMB6H_GET_BUF
    u32 req_size;                   //requested audio buffer size in bytes

    //---------------------------- -2- case header.req.mb6 == RMB6H_DO_START
    //---------------------------- -3- case header.req.mb6 == RMB6H_DO_STOP
    //---------------------------- -4- case header.req.mb6 == RMB6H_DO_ABORT

};


///////////////////////////////////////////////////////////////////////////////
// AckMb6
//          type : Service Acknowledge Mailbox
//          associated IT : prcm_arm_it1_val[6]
//          communication direction : prcmu (write) -> arm (read)
//          width (Bytes) : 8
//          multiservice (using header) : false
//          service(s) : <unused>
///////////////////////////////////////////////////////////////////////////////

struct ack_mb_6 {
    //---------------------------- -1- case header.ack.mb6 == AMB6H_GET_BUF
    //
    u32 offset;                     // returns the offset to the audio buffer structure from the prcmu baseline,
                                    // arm to calc arm address.
    //---------------------------- -2- case header.ack.mb6 == AMB6H_LPA_DO_CMD
    u8 return_code;                 // assigned with a value from enum return_code
    u8 byte_fill[3];                // padding

};


///////////////////////////////////////////////////////////////////////////////
// ReqMb7
//          type : Service Request Mailbox
//          associated IT : prcm_mbox_cpu_set[7] -> xp70 IT 19
//          communication direction : arm (write) -> prcmu (read)
//          width (Bytes) : 8
//          multiservice (using header) : false
//          service(s) : CLOCKOUT
///////////////////////////////////////////////////////////////////////////////
struct req_mb_7 {
    //
    //---------------------------- -1- case header.req.mb7 == CLK_OUT_CONFIG_SW_EN
    //---------------------------- -2- case header.req.mb7 == CLK_OUT_CONFIG_REQ_EN
    //---------------------------- -3- case header.req.mb7 == CLK_OUT_CONFIG_SW_DIS
    //---------------------------- -4- case header.req.mb7 == CLK_OUT_CONFIG_REQ_DIS
    u16 clk_out_source;             // assigned with a value from enum clk_out_source
    u8  clk_out_div;                // assigned with a value from enum clk_out_div,
                                    // Ignored unless clk_out_source member ends with a _DIVIDED
    u8  clk_out_oe;
    u8  clk_out_nbr;                // which clockout
    u8  byte_fill[3];
    //---------------------------- -5- case header.req.mb7 == CLOCKOUT_CONFIG_CLKOUT_REQ_ACTIVE_HIGH
    //---------------------------- -6- case header.req.mb7 == CLOCKOUT_CONFIG_CLKOUT_REQ_ACTIVE_LOW
};

///////////////////////////////////////////////////////////////////////////////
// AckMb7
//          type : Service Acknowledge Mailbox
//          associated IT : prcm_arm_it1_val[7]
//          communication direction : prcmu (write) -> arm (read)
//          width (Bytes) : 4
//          multiservice (using header) : false
//          service(s) : <unused>
///////////////////////////////////////////////////////////////////////////////

struct ack_mb_7 {
    //
    //---------------------------- -1- case header.req.mb7 == CLOCKOUT_CONFIG_SW_EN
    //---------------------------- -2- case header.req.mb6 == CLOCKOUT_CONFIG_REQ_EN
    //---------------------------- -3- case header.req.mb7 == CLOCKOUT_CONFIG_SW_DIS
    //---------------------------- -4- case header.req.mb6 == CLOCKOUT_CONFIG_REQ_DIS
    //
    u8  rc;                         // assigned with values from enum return_code
    u8  byte_fill[3];               // byte fill
};





///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////// TCPM & TCDM Memory Map /////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

struct n_sec_map {
    struct share_info share_info;//B4 + 0xEC8 {  64   bytes)
    struct ack_mb_7 ack_mb_7;   // B4 + 0xF08 ( 4 bytes)
    struct ack_mb_6 ack_mb_6;   // B4 + 0xF0C ( 8 bytes)
    struct ack_mb_5 ack_mb_5;   // B4 + 0xF14 ( 8 bytes)
    struct ack_mb_4 ack_mb_4;   // B4 + 0xF1C ( 4 bytes)
    struct ack_mb_3 ack_mb_3;   // B4 + 0xF20 ( 4 bytes)
    struct ack_mb_2 ack_mb_2;   // B4 + 0xF24 ( 12  bytes)
    struct ack_mb_1 ack_mb_1;   // B4 + 0xF30 ( 8 bytes)
    struct ack_mb_0 ack_mb_0;   // B4 + 0xF38 ( 76  bytes) 36*2+2+2= 72+4 = 76 (+24 0x18)
    struct req_mb_7 req_mb_7;   // B4 + 0xF84 ( 8 bytes)
    struct req_mb_6 req_mb_6;   // B4 + 0xF8C ( 4 bytes)
    struct req_mb_5 req_mb_5;   // B4 + 0xF90 ( 8 bytes)
    struct req_mb_4 req_mb_4;   // B4 + 0xF98 ( 40  bytes)
    struct req_mb_3 req_mb_3;   // B4 + 0xFC0 ( 4 bytes)
    struct req_mb_2 req_mb_2;   // B4 + 0xFC4 ( 8 bytes)
    struct req_mb_1 req_mb_1;   // B4 + 0xFCC ( 12  bytes)
    struct req_mb_0 req_mb_0;   // B4 + 0xFD8 ( 16  bytes)
    struct header   header;     // B4 + 0xFE8 ( 16  bytes)
  u32    reserved_NVM;      // B4 + 0xFF8   ( 4 bytes)
  struct pwr_current_state_var ShVar1;        // B4 + 0xFFC ( 4 bytes)
}; // Total 248 bytes


struct sec_map {
  struct ddr_init       ddr_init;
  struct memory_speed   memory_speed;
  struct fuse_init      fuse_init;
  struct abb_init       abb_init;
    u32    spare[4];
  struct ddr_init_mrr       ddr_init_mrr;
    u32    spare2[4];
  struct frequency          frequency;
    u32    spare3[4];
  struct avs                avs;
    u32    spare4[3];
  struct thermal_sensor_data   thermal_sensor_data;
};

// romcode communication field in secure TCPM
// #define romcode_mb  ((volatile RomcodeMb *) TCPM_BASE + 0x28)
//#define romcode_mb_arm  ((volatile RomcodeMb *) TCPM_BASE + 0xa0)
#define romcode_mb  ((volatile struct romcode_mbox *) (TCPM_BASE + 0xa0))

/* defines the version of the prcmu_fw. */
#define fw_version  ((volatile u32 *) (TCPM_BASE + 0xa4))

/* defines the version of the prcmu_fw. */
#define api_version  ((volatile u32 *) (TCPM_BASE + 0xa8))


// shared variables in secure TCDM section (begin of TCDM)
#define TCDMsec     ((volatile struct sec_map *) (TCDM_BASE + 0x0))

// shared variables and mailboxes in non-secure TCDM section (end of TCDM)
#define TCDMnSec    ((volatile struct n_sec_map *) (TCDM_BASE + 0xEC8))


//#endif /* !defined(XP70_MEMMAP_H) */


// Details abouts addresses and sizes meant for people in validation
// and people coding on rtl testbenches.

/*
///////////////////// from map file????
*/

#endif /* ifndef INCLUSION_GUARD_XP70_MEMMAP_H */
