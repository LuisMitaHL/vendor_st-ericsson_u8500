/*******************************************************************************
 *                            C STMicroelectronics
 *    Reproduction and Communication of this document is strictly prohibited
 *      unless specifically authorized in writing by STMicroelectronics.
 *----------------------------------------------------------------------------*/

/*------------------------------------------------------------------------
 * Level 2 cache controller
 *----------------------------------------------------------------------*/
#ifndef _NOMADIK_REGISTERS_H
#define _NOMADIK_REGISTERS_H

#include "hcl_defs.h"

#define WAY_NB          8

// Level 2 Cache configuration registers (L220)//To become L310 0xA0414000 or 0xA0422000 ??
#define L2CC_REG_START_ADDR   0xA0412000
#define L2CC_REG_LENGTH       CNTRL_REG_SIZE
#define L2CC_REG_END_ADDR     (L2CC_REG_START_ADDR + L2CC_REG_LENGTH - 1)

// L2CC identification register
typedef struct {
    t_bitfield Revision: 6;
    t_bitfield PartNumber: 4;
    t_bitfield CacheId: 6;
    t_bitfield unused: 8;
    t_bitfield Designer: 8;
} t_l2cc_id_reg;

// L2CC cache type register
typedef struct {
    t_bitfield unused_1: 6;
    t_bitfield InstrAssoc: 1;
    t_bitfield unused_2: 1;
    t_bitfield InstrWaySize: 3;
    t_bitfield unused_3: 7;
    t_bitfield DataAssoc: 1;
    t_bitfield unused_4: 1;
    t_bitfield DataWay: 3;
    t_bitfield unused_5: 2;
    t_bitfield CType: 4;
    t_bitfield unused_6: 3;
} t_l2cc_type_reg;

// L2CC control register
typedef struct {
    t_bitfield Enable: 1;
    t_bitfield unused: 31;
} t_l2cc_control_reg;

// L2CC auxiliairy control register
typedef struct {
    t_bitfield unused_2: 12;
    t_bitfield Exclusive: 1;
    t_bitfield unused_3: 3;
    t_bitfield Associativity: 1;
    t_bitfield WaySize: 3;
    t_bitfield EventBusEnable: 1;
    t_bitfield ParityEnable: 1;
    t_bitfield SharedAttrOverEnable: 1;
    t_bitfield ForceWriteAlloc: 2;
    t_bitfield unused_4: 1;
    t_bitfield NSLockEnable: 1;
    t_bitfield NSIntCtrl: 1;
    t_bitfield DPrefetchEnable: 1;
    t_bitfield IPrefetchEnable: 1;
    t_bitfield unused_5: 2;
} t_l2cc_aux_control_reg;

// L2CC Tag and RAM latency control register
typedef struct {
    t_bitfield RamSetupLatency: 3;
    t_bitfield unused_1: 1;
    t_bitfield RamReadLatency: 3;
    t_bitfield unused_2: 1;
    t_bitfield RamWriteLatency: 3;
    t_bitfield unused_3: 21;
} t_l2cc_latency_control_reg;

// L2CC Event Counter Control register
typedef struct {
    t_bitfield Enable: 1;
    t_bitfield counter0Reset: 1;
    t_bitfield counter1Reset: 1;
    t_bitfield unused: 29;
} t_l2cc_evt_cnt_control_reg;

// L2CC Event Counter Configuration register
typedef struct {
    t_bitfield ItConfiguration: 2;
    t_bitfield Source: 4;
    t_bitfield unused: 26;
} t_l2cc_evt_cnt_conf_reg;

// L2CC interrupt register
typedef struct {
    t_bitfield EventCounterOverflow: 1;
    t_bitfield L2TagRamParityError: 1;
    t_bitfield L2DataRamParityError: 1;
    t_bitfield L2TagRamWrError: 1;
    t_bitfield L2DataRamWrError: 1;
    t_bitfield L2TagRamRdError: 1;
    t_bitfield L2DataRamRdError: 1;
    t_bitfield SLVERR: 1;
    t_bitfield DECERR: 1;
    t_bitfield unused: 23;
} t_l2cc_it_reg;

// L2CC choice by way
typedef struct {
    t_bitfield Ways: 8;
    t_bitfield unused: 24;
} t_l2cc_by_way_reg;

// L2CC address filtering start register
typedef struct {
    t_bitfield Enable: 1;
    t_bitfield unused: 19;
    t_bitfield Address: 12;
} t_l2cc_filter_start_reg;

// L2CC address filtering start register
typedef struct {
    t_bitfield unused: 20;
    t_bitfield Address: 12;
} t_l2cc_filter_end_reg;

// L2CC debug control
typedef struct {
    t_bitfield Dcl: 1;
    t_bitfield Dwb: 1;
    t_bitfield SPNIDEN: 1;
    t_bitfield unused: 29;
} t_l2cc_debug_cont_reg;

// L2CC registers
typedef struct {
    //  Type                            Name                           Offset   Description
    t_l2cc_id_reg                   Id;                         // 0x000    Identification
    t_l2cc_type_reg                 Type;                       // 0x004    Cache type
    const t_uint32                  reserved_1[(0x100-0x008)>>2];
    t_l2cc_control_reg              Control;                    // 0x100    Control
    t_l2cc_aux_control_reg          AuxControl;                 // 0x104    Auxiliary control
    t_l2cc_latency_control_reg      TagRamLatency;              // 0x108    Tag RAM latency control
    t_l2cc_latency_control_reg      DataRamLatency;             // 0x10c    Data RAM latency control
    const t_uint32                  reserved_2[(0x200-0x110)>>2];
    t_l2cc_evt_cnt_control_reg      EvtCntControl;              // 0x200    Event counter control
    t_l2cc_evt_cnt_conf_reg         EvtCnt1Configuration;       // 0x204    Event counter1 configuration
    t_l2cc_evt_cnt_conf_reg         EvtCnt0Configuration;       // 0x208    Event counter0 configuration
    t_uint32                        Counter1;                   // 0x20c    Event counter1
    t_uint32                        Counter0;                   // 0x210    Event counter0
    t_uint32                        ItMaskSetClear;             // 0x214    Interrupt mask set/clear
    t_uint32                        MaskedItStatus;             // 0x218    Masked interrupt status
    t_uint32                        RawItStatus;                // 0x21c    Raw interrupt status
    t_uint32                        ClearIt;                    // 0x220    Interrupt clear
    const t_uint32                  reserved_3[(0x730-0x224)>>2];
    t_uint32                        Sync;                       // 0x730    Cache synchronize
    const t_uint32                  reserved_4[(0x770-0x734)>>2];
    t_uint32                        InvLineByPhysAddr;          // 0x770    Invalidate by physical address
    const t_uint32                  reserved_5[2];
    t_l2cc_by_way_reg               InvByWay;                   // 0x77C    Invalidate by way
    const t_uint32                  reserved_6[(0x7B0-0x780)>>2];
    t_uint32                        CleanLineByPhysAddr;        // 0x7B0    Clean line by physical address
    const t_uint32                  reserved_7[1];
    t_uint32                        CleanLineByWay;             // 0x7B8    Clean line by way/index
    t_l2cc_by_way_reg               CleanByWay;                 // 0x7BC    Clean by way
    const t_uint32                  reserved_8[(0x7F0-0x7C0)>>2];
    t_uint32                        CleanInvLineByPhysAddr;     // 0x7F0    Clean and invalidate line by physical address
    const t_uint32                  reserved_9[(0x7F8-0x7F4)>>2];
    t_uint32                        CleanInvLineByWay;          // 0x7F8    Clean and invalidate line by way
    t_l2cc_by_way_reg               CleanInvByWay;              // 0x7FC    Clean and invalidate by way
    const t_uint32                  reserved_10[(0x900-0x800)>>2];
    t_l2cc_by_way_reg               DataLock0ByWay;             // 0x900    Lock way for the data side
    t_l2cc_by_way_reg               InstrLock0ByWay;            // 0x904    Lock way for the instruction side
    t_l2cc_by_way_reg               DataLock1ByWay;             // 0x908
    t_l2cc_by_way_reg               InstrLock1ByWay;            // 0x90C
    t_l2cc_by_way_reg               DataLock2ByWay;             // 0x910
    t_l2cc_by_way_reg               InstrLock2ByWay;            // 0x914
    t_l2cc_by_way_reg               DataLock3ByWay;             // 0x918
    t_l2cc_by_way_reg               InstrLock3ByWay;            // 0x91C
    t_l2cc_by_way_reg               DataLock4ByWay;             // 0x920
    t_l2cc_by_way_reg               InstrLock4ByWay;            // 0x924
    t_l2cc_by_way_reg               DataLock5ByWay;             // 0x928
    t_l2cc_by_way_reg               InstrLock5ByWay;            // 0x92C
    t_l2cc_by_way_reg               DataLock6ByWay;             // 0x930
    t_l2cc_by_way_reg               InstrLock6ByWay;            // 0x934
    t_l2cc_by_way_reg               DataLock7ByWay;             // 0x938
    t_l2cc_by_way_reg               InstrLock7ByWay;            // 0x93C
    const t_uint32                  reserved_11[(0x950-0x940)>>2];
    t_uint32                        LockByLineEnable;           // 0x950    Lockdown by line enable
    t_uint32                        UnlockAllLinesByWay;        // 0x954    Unlock all lines by way
    const t_uint32                  reserved_12[(0xC00-0x958)>>2];
    t_l2cc_filter_start_reg         AddressFilterStart;         // 0xC00    Address filtering start
    t_l2cc_filter_end_reg           AddressFilterEnd;           // 0xC04    Address filtering end
    const t_uint32                  reserved_13[(0xF40-0xC08)>>2];
    t_l2cc_debug_cont_reg           DebugControl;               // 0xF40    Debug Control
    const t_uint32                  reserved_14[(0x1000-0xF44)>>2];
} t_l2cc_registers;
#endif // _NOMADIK_REGISTERS_H

// End of file nomadik_registers.h
