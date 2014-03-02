/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
/*------------------------------------------------------------------------
 *                               TPA / MPU
 *                   MultiMedia Technology Center (MMTC)
 *------------------------------------------------------------------------
 * Use this module to declare the Nomadik Full registers.
 *------------------------------------------------------------------------
 *
 ******************************************************************************/

#ifndef _NOMADIK_REGISTERS_H
#define _NOMADIK_REGISTERS_H

/*------------------------------------------------------------------------
 * Includes
 *----------------------------------------------------------------------*/

#include "boot_secure_types.h"

/*------------------------------------------------------------------------
 * Definitions
 *----------------------------------------------------------------------*/
 
#define DMC_NUM_OF_QOS                     16
#define DMC_NUM_OF_CHIPS                   4
 


/*------------------------------------------------------------------------
 * Flexible Static Memory Controller (FSMC)
 *----------------------------------------------------------------------*/


/* PC-Card/NAND-Flash Control register */
typedef struct {
    t_bitfield prst:1;          /* PC-CArd Reset pin                        */
    t_bitfield pwaiten:1;       /* Wait feature enable bit                  */
    t_bitfield pbken:1;         /* PC-Card/NAND flash chip-Select enable    */  
    t_bitfield ptyp:1;          /* Memory type                              */
    t_bitfield pwid:2;          /* NAND-Flash Data bus width                */
    t_bitfield eccen:1;         /* ECC computation logic enable bit         */
    t_bitfield eccplen:1;       /* ECC page Length                          */
    t_bitfield adlow:1;         /* Address Low Bit delivery                 */   
    t_bitfield tclr:4;          /* CLE to RE delay                          */ 
    t_bitfield tar:4;           /* ALE to RE delay                          */ 
    t_bitfield eccps:3;         /* ECC page size                            */ 
    t_bitfield unused:12;   
} t_fsmc_pcr_reg;

/* Common Memory Space Timing Register */
typedef struct {
    t_bitfield memset:8;        /* common memory Set up time            */
    t_bitfield memwait:8;       /* common memory wait time              */
    t_bitfield memhold:8;       /* common memory hold time              */  
    t_bitfield memHiZ:8;        /* common memory data bus HiZ time      */
} t_fsmc_pmem_reg;

/* Common Attribute Memory Space Timing Register */
typedef struct {
    t_bitfield attset:8;        /* attribute memory Set up time         */
    t_bitfield attwait:8;       /* attribute memory wait time           */
    t_bitfield atthold:8;       /* attribute memory hold time           */ 
    t_bitfield attHiz:8;        /* attribute memory data bus HiZ time   */  
} t_fsmc_patt_reg;

/* SRAM/NOR-Flash chip-select Control Register */
typedef struct {
    t_bitfield mbken:1;         /* memory chip select enable bit        */
    t_bitfield muxen:1;         /* Address/Data Mus=xing Enable bit     */
    t_bitfield mtyp:2;          /* memory type                          */
    t_bitfield mwid:2;          /* memory data bus width                */
    t_bitfield frstlvl:1;       /* flash reset pin level                */
    t_bitfield fwprlvl:1;       /* flash write protection pin level     */
    t_bitfield bursten:1;       /* burst enable bit                     */
    t_bitfield waitpol:1;       /* wait signal polarity bit             */
    t_bitfield wrapmod:1;       /* wrapped burst mode support           */
    t_bitfield waitcfg:1;       /* wait timing configuration            */
    t_bitfield wren:1;          /* write enable bit                     */
    t_bitfield waiten:1;        /* wait enable bit                      */
    t_bitfield unused:18;
} t_fsmc_bcr_reg;

/* SRAM/NOR-Flash chip-select Timing Register */
typedef struct {
    t_bitfield addset:4;        /* address setup phase duration         */
    t_bitfield addhld:4;        /* address hold phase duration          */
    t_bitfield datast:8;        /* data phase duration                  */
    t_bitfield busturn:4;       /* bus turn around phase duration       */
    t_bitfield clkdiv:4;        /* clock divide ratio (for SMCKO signal */
    t_bitfield datlat:4;        /* data latency for synchro burst NOR   */
    t_bitfield unused:4;
} t_fsmc_btr_reg;

typedef struct{
    /*Type   Name               Description                         Offset      */
    t_fsmc_pcr_reg PCR;         /* PCcard/NAND-Flash Ctrl Reg       0x040       */
    t_uint32 unused;            /*                                  0x44        */
    t_fsmc_pmem_reg PMEM;       /* Common Memory Space Timing Reg   0x048       */
    t_fsmc_patt_reg PATT;       /* Attribute Memory Space Timing    0x04C       */
    t_uint32 PIO;               /* I/O Space Timing Reg             0x050       */
    t_uint32 ECC;               /* ECC result Reg                   0x054       */
    t_uint32 unused_3[2];       
} t_fsmc_nand_reg;


typedef struct{
    /*Type   Name               Description                         Offset      */
    t_fsmc_bcr_reg FSMC_BCR;    /* SRAM/NOR-Flash CS Ctrl Reg       0x000       */
    t_fsmc_btr_reg FSMC_BTR;    /* SRAM/NOR-Flash CS Timing Reg     0x004       */
} t_fsmc_nor_reg;


typedef struct {
    //Type   Name                       /* Description                      Offset      */
    t_fsmc_nor_reg nor_reg[3];          /* SRAM/NOR-Flash Registers         0x000       */
    t_uint32 unused_1[(0x40-0x18)>>2];  /* Reserved                         0x018-0x03C */
    t_fsmc_nand_reg nand_reg[2];        /* PCcard/NAND-Flash Registers      0x040-0x07C */
    t_uint32 unused_4[16];              /* 0x80-0xBC */
    t_uint32 itcr;                      /* Integration Test Control Reg     0x0C0       */
    t_uint32 itip0;                     /* Integration Test Input Reg 0     0x0C4       */   
} t_fsmc_registers;


/*------------------------------------------------------------------------
 * Global interrupt controller (GIC)
 *----------------------------------------------------------------------*/
/* INTID = SPI(NB) + 32    */
#define SPI_OFFSET_ID       32
#define RTT_IT_LINE_NB      0x9
#define DMA_IT_LINE_NB      0x18
#define PRCMU_IT_LINE_NB    0x2E

/* STI: cpu mask selection */
#define STI_CPU_FILT_ALL_BUT_ME 0x1


 typedef struct {
    t_bitfield Enable:1;        /* to enable NonSecure IT         */
    t_bitfield Reserved:31;     /* reserved                       */
} t_gic_ctrl_ns_reg;

typedef struct {
    t_bitfield EnableS:1;        /* to enable Secure IT                          */
    t_bitfield EnableNS:1;       /* to enable NonSecure IT                       */
    t_bitfield AckCtl:1;         /* control secure access to int_ack and eoi reg */
    t_bitfield FiqEn:1;          /* to use FIQ when sending Secure IT            */
    t_bitfield Sbpr:1;           /* to control binary ptr reg                    */
    t_bitfield reserved:27;      
} t_gic_ctrl_s_reg;

typedef union {
    t_gic_ctrl_ns_reg  CtrlNS;   /* Control Reg when doing a non secure access */
    t_gic_ctrl_s_reg  CtrlS;     /* Control Reg when doing a secure access     */
}t_gic_ctrl_reg;

typedef union {
    struct {                    
        t_bitfield IntId:10;       /* INTID of the interrupt                        */
        t_bitfield SrcCpuId:3;     /* source CPUID of the STI (b000 for PPIs,SPIs)  */
        t_bitfield Reserved:19;
        } Bit;
    t_uint32  Reg;     
}t_gic_int_mgt;

typedef union {
    struct {                    
        t_bitfield IntId:4;       /* STI INTID of the STI  to trigger              */
        t_bitfield Reserved:11;    
        t_bitfield Satt:1;         /* Security attribute (secure access) of the STI */    
        t_bitfield CpuTargetList:8;/* Select the CPU(s) to receive the STI */
        t_bitfield TargetListFilter:2;/* Select CPU(s) or use of target list        */
        t_bitfield Reserved2:6;    
        } Bit;
    t_uint32  Reg; 
}t_gic_sti_trig;

typedef struct
{                    
        t_bitfield TimerEn:1;               /* Timer enable                             */
        t_bitfield CompEn:1;                /* Comparator enable                        */
        t_bitfield ItEn:1;                  /* Interrupt generation enable              */
        t_bitfield AutoIncr:1;              /* Auto Increment (after timer expiration)  */
        t_bitfield Reserved:4;    
        t_bitfield Prescaler:8;             /* Clock divider                            */
        t_bitfield Reserved2:16;    
}t_timer_ctrl_reg;

typedef struct
{                    
        t_bitfield EventFlag:1;             /* Timer interrupt status                   */
        t_bitfield Reserved:31;    
}t_timer_it_stat_reg;

typedef struct {
    t_uint8 Byte[4];
}t_gic_bytes;

typedef union {
    struct {                    
        t_bitfield SOrNs:1;       /* Enable S or Ns                        */
        t_bitfield Ns:1;          /* Enable NS only for ENABLE_S reg */
        t_bitfield Reserved:30;
        } Bit;
    t_uint32  Reg;     
}t_gic_distrib_enable;


typedef struct {
    //Type   Name                       /* Description                          Offset     */
    /* PROCESSOR INTERFACE REGISTERS                                                       */
    t_gic_ctrl_reg CtrlReg;             /* CONTROL_S or CONTROL_NS register     0x100      */
    t_uint32 PriorityMask;              /* PRIORITY_MASK [7:3]    register      0x104      */
    t_uint32 BinPtSorNS;                /* Binary point [2:0] register secure or non-secure  0x108 */
    t_gic_int_mgt IntAck;               /* Interrupt Acknowledge [9:0] register 0x10C       */
    t_gic_int_mgt Eoi;                  /* End of interrupt [9:0] register      0x110       */
    t_uint32 unused2[3];                /* RunPriority, HiPend, AliasBinPtNs    0x114-0x11C */   
    t_uint32 unused3[(0x200-0x120)>>2]; /* Reserved - cpuIdent (0x1FC)          0x120-0x1FC */

    /* GLOBAL TIMER                                                                        */
    t_uint32 GlobTimerLowValue;         /* Low 32 bits of timer value           0x200      */
    t_uint32 GlobTimerHighValue;        /* High 32 bits of timer value          0x204      */
    t_timer_ctrl_reg GlobTimerCtrl;     /* CONTROL_S or CONTROL_NS register     0x208      */
    t_timer_it_stat_reg GlobTimerItStat;/* CONTROL_S or CONTROL_NS register     0x20C      */
    t_uint32 GlobTimerLowComp;          /* Low 32 bits of timer comparator      0x210      */
    t_uint32 GlobTimerHighComp;         /* High 32 bits of timer comparator     0x214      */
    t_uint32 GlobTimerCompIncr;         /* Increment for comparator             0x218      */
    t_uint32 unused4[(0x600-0x21C)>>2]; /* Reserved                             0x21C-0x5FC */

    /* PRIVATE TIMER                                                                        */
    t_uint32 PrivTimerLoadValue;        /* Low 32 bits of timer value           0x600      */
    t_uint32 PrivTimerCountValue;       /* High 32 bits of timer value          0x604      */
    t_timer_ctrl_reg PrivTimerCtrl;     /* CONTROL_S or CONTROL_NS register     0x608      */
    t_timer_it_stat_reg PrivTimerItStat;/* CONTROL_S or CONTROL_NS register     0x60C      */
    t_uint32 unused5[(0x1000-0x610)>>2];/* Reserved                             0x610-0xFFC */

    /* DISTRIBUTOR INTERFACE REGISTERS                                                        */
    t_gic_distrib_enable EnableSorNS;   /* ENABLE_S or ENABLE_NS [0] according to
                                           secure or non-secure access          0x1000        */
    t_uint32 unused6[2];                /* itCtrlType, DistribImpId Reg         0x1004-0x1008 */
    t_uint32 unused7[(0x1080-0x100C)>>2];/* Reserved                            0x100C-0x107C */
/*For following registers: INTID[0:31]is STI[15:0] + PPI[31:28] and INTID[32:255] is SPI[223:0] + unused*/
    t_uint32 ItSec[32];               /* Interrupt Security Reg                 0x1080-0x10FC */
    t_uint32 EnableSet[32];             /* Enabling of interrupt                0x1100-0x117C */
    t_uint32 EnableClr[32];             /* Disabling of interrupt               0x1180-0x11FC */
    t_uint32 PendSet[32];               /* Set interrupt pending                0x1200-0x127C */
    t_uint32 PendClr[32];               /* Clear interrupt pending              0x1280-0x12FC */
    t_uint32 ActStat[32];               /* Give active status                   0x1300-0x137C */
    t_uint32 unused8[(0x1400-0x1380)>>2];/* Reserved                            0x1380-0x13FC */
    t_gic_bytes PriorityLvl[64];         /* Give priority level                 0x1400-0x14FC */
    t_uint32 unused9[(0x1800-0x1500)>>2];/* Reserved                            0x1500-0x17FC */    
    t_gic_bytes SpiTarget[64];           /* SPI target (CPU target for each SPI INTID 32 to 255 SPI[223:0],
                                        SPI target not used for STI and PPI)    0x1800-0x18FC */
    t_uint32 unused10[(0x1C00-0x1900)>>2];/* Reserved                            0x1900-0x1BFC */
    t_uint32 ItConfig[64];              /* Interrupt Configuration sensitivity (2bits/it) 
                                                                                0x1C00-0x1CFC */

    t_uint32 PpiStat;                /* PPI status [15:12]                      0x1D00        */
    t_uint32 SpiStat[7];             /* SPI Status (Access to status) 
                                        for INTID 32 to 255 (SPI[223:0])        0x1D04-0x1D1C */
    t_uint32 unused11[(0x1F00-0x1D20)>>2];/* Reserved                           0x1D20-0x1EFC */
    t_gic_sti_trig StiTrigger;       /* STI trigger: to issue SW interrupt      0x1F00        */      
} t_gic_registers;




/*------------------------------------------------------------------------
 * Power, Reset, Clock Managment Unit (PRCMU)
 *----------------------------------------------------------------------*/
 // PRCMU control register 

// xxCLK_mgt_reg
typedef union {
    struct {       
        t_bitfield  PLLDIV          :5;
        t_bitfield  PLLSW           :3;    
        t_bitfield  CLKEN           :1;
        t_bitfield  CLK38           :1;
        t_bitfield  CLK38SRC        :1;
        t_bitfield  CLK38DIV        :1;
        const t_bitfield unused     :20;
        } Bit;
    t_uint32  Reg;     
} t_prcmu_clk_mgt_reg;

// PLL settings reg
typedef union {
    struct {       
        t_bitfield  D               :8; // [ 7: 0]
        t_bitfield  N               :6; // [13: 8]   
        t_bitfield  unused1         :2; // [15:14]
        t_bitfield  R               :3; // [18:16]
        t_bitfield  unused2         :5; // [23:19]
        t_bitfield  seldiv2         :1; // [24]    
        t_bitfield  div2en          :1; // [25]                
        const t_bitfield unused3    :6; // [31:26]
        } Bit;
    t_uint32  Reg;     
} t_prcmu_pll_freq_reg;

// PLL lock status reg
typedef union {
    struct {       
        t_bitfield  lockp3          :1; // [0]
        t_bitfield  lockp10         :1; // [1]     
        const t_bitfield unused3    :30; // [31:2]
        } Bit;
    t_uint32  Reg;     
} t_prcmu_pll_lock_reg;

// CLK009_mgt_reg
typedef union {
    struct {       
        t_bitfield  div          :3; // [ 2: 0]
        t_bitfield  clk38src     :1; // [3]
        const t_bitfield unused  :28;
        } Bit;
    t_uint32  Reg;     
} t_prcmu_clk009_mgt_reg;

// CLK32K_mgt_reg
typedef union {
    struct {       
        t_bitfield  enable      :1; // [0]
        t_bitfield  dir         :1; // [1]
        t_bitfield  gate        :1; // [1]   
        const t_bitfield unused  :29;
        } Bit;
    t_uint32  Reg;     
} t_prcmu_clk32k_mgt_reg;

// Reset Status Regi
typedef union {
    struct {       
        t_bitfield  a9cpu0_wdg_reset :1; // [0]
        t_bitfield  a9cpu1_wdg_reset :1; // [1]
        t_bitfield  apesw_reset      :1; // [2]    
        t_bitfield  ape_reset        :1; // [3]  
        t_bitfield  sec_wdg_reset    :1; // [4]      
        t_bitfield  por              :1; // [5]  
        t_bitfield  a9_deep_sleep    :1; // [6]
        t_bitfield  ape_deep_sleep   :1; // [7]          
        const t_bitfield unused      :24;
        } Bit;
    t_uint32  Reg;     
}t_prcmu_reset_status_reg;



// XP70 CFG CR Register
typedef union {
    struct {       
        t_bitfield  xp70_p_fetch_en  :1; // [0]
        t_bitfield  xp70_wake_up     :1; // [1]
        t_bitfield  xp70_except_vb   :1; // [2]    
        t_bitfield  xp70_idle_ack    :1; // [3]           
        const t_bitfield unused      :28;
        } Bit;
    t_uint32  Reg;     
}t_prcmu_xp70cfgcr_reg ;

typedef struct
{       
    t_bitfield  secure_ongoing  :1;
    const t_bitfield unused     :31;
} t_prcmu_secure_status_reg;

// ARM CHGCLKREQ Register
typedef union {
    struct {       
        t_bitfield  arm_chgclkreq  :1; // [0]
        const t_bitfield  unused1  :15; // [1:15]
        t_bitfield  arm_divsel     :1; // [16]            
        const t_bitfield unused2   :15;
        } Bit;
    t_uint32  Reg;     
}t_prcmu_arm_chgclkreq_reg ;

// ARM CHGCLKREQ Register
typedef union {
    struct {
	t_bitfield  dbg_modemrf_ctrl_cmd1  :1;  // [0]
	t_bitfield  dbg_modemrf_ctrl_cmd0  :1;  // [1]
	t_bitfield  dbg_modemrf_int_cmd1   :1;  // [2]
	t_bitfield  dbg_modemrf_int_cmd0   :1;  // [3]
	t_bitfield  mod_ddr_space          :2;  // [4:5]
	t_bitfield  mod_gpiocr_spare       :10; // [6:15]
        const t_bitfield  unused1          :16; // [16:31]
        } Bit;
	t_uint32  Reg;
}t_prcmu_gpiocr2_reg;

// PRCMU HW registers
typedef struct {
//  Type                        Name                    Offset Description
//                  SOC CLOCKS Managment
    t_prcmu_clk_mgt_reg         prcm_armclkfix_mgt;     // 0x0000
    t_prcmu_clk_mgt_reg         prcm_aclk_mgt;          // 0x0004
    t_uint32                    prcm_svammdspclk_mgt;   // 0x0008
    t_uint32                    prcm_siammdspclk_mgt;   // 0x000C
    t_uint32                    prcm_saammdspclk_mgt;   // 0x0010
    t_prcmu_clk_mgt_reg         prcm_sgaclk_mgt;        // 0x0014
    t_prcmu_clk_mgt_reg         prcm_uartclk_mgt;       // 0x0018
    t_prcmu_clk_mgt_reg         prcm_mspclk_mgt;        // 0x001C
    t_prcmu_clk_mgt_reg         prcm_i2cclk_mgt;        // 0x0020 
    t_prcmu_clk_mgt_reg         prcm_sdmmcclk_mgt;      // 0x0024
    t_prcmu_clk_mgt_reg         prcm_slimclk_mgt;       // 0x0028
    t_prcmu_clk_mgt_reg         prcm_per1clk_mgt;       // 0x002C
    t_prcmu_clk_mgt_reg         prcm_per2clk_mgt;       // 0x0030
    t_prcmu_clk_mgt_reg         prcm_per3clk_mgt;       // 0x0034
    t_prcmu_clk_mgt_reg         prcm_per5clk_mgt;       // 0x0038
    t_prcmu_clk_mgt_reg         prcm_per6clk_mgt;       // 0x003C
    t_prcmu_clk_mgt_reg         prcm_per7clk_mgt;       // 0x0040
    t_prcmu_clk_mgt_reg         prcm_lcdclk_mgt;        // 0x0044
    t_uint32                    unused_1;
    t_prcmu_clk_mgt_reg         prcm_bmlclk_mgt;        // 0x004c
    t_prcmu_clk_mgt_reg         prcm_hsitxclk_mgt;      // 0x0050
    t_prcmu_clk_mgt_reg         prcm_hsirxclk_mgt;      // 0x0054
    t_prcmu_clk_mgt_reg         prcm_hdmiclk_mgt;       // 0x0058
    t_prcmu_clk_mgt_reg         prcm_apeatclk_mgt;      // 0x005C
    t_uint32                    prcm_apetraceclk_mgt;   // 0x0060
    t_prcmu_clk_mgt_reg         prcm_mcdeclk_mgt;       // 0x0064
    t_prcmu_clk_mgt_reg         prcm_ipi2cclk_mgt;      // 0x0068
    t_prcmu_clk_mgt_reg         prcm_dsialtclk_mgt;     // 0x006C
    t_prcmu_clk_mgt_reg         prcm_spareclk_mgt;      // 0x0070
    t_prcmu_clk_mgt_reg         prcm_dmaclk_mgt;        // 0x0074
    t_prcmu_clk_mgt_reg         prcm_b2r2clk_mgt;       // 0x0078
    t_prcmu_clk_mgt_reg         prcm_tvclk_mgt;         // 0x007C
//                   SOC  PLL Managment    
    t_prcmu_pll_freq_reg        prcm_pllsoc0_freq;      // 0x0080
    t_prcmu_pll_freq_reg        prcm_pllsoc1_freq;      // 0x0084
    t_prcmu_pll_freq_reg        prcm_pllarm_freq;       // 0x0088
    t_prcmu_pll_freq_reg        prcm_pllddr_freq;       // 0x008C
    t_uint32                    prcm_pllsoc0_enable;    // 0x0090
    t_uint32                    prcm_pllsoc1_enable;    // 0x0094
    t_uint32                    prcm_pllarm_enable;     // 0x0098
    t_uint32                    prcm_pllddr_enable;     // 0x009C
    t_prcmu_pll_lock_reg        prcm_pllsoc0_lockp;     // 0x00A0
    t_prcmu_pll_lock_reg        prcm_pllsoc1_lockp;     // 0x00A4
    t_prcmu_pll_lock_reg        prcm_pllarm_lockp;      // 0x00A8
    t_prcmu_pll_lock_reg        prcm_pllddr_lockp;      // 0x00AC    
    t_uint32                    prcm_xp70clk_mgt;       // 0x00B0
    t_uint32                    prcm_timer_0_ref;       // 0xB4
    t_uint32                    prcm_timer_0_dwncnt;    // 0xB8
    t_uint32                    prcm_timer_0_mode;      // 0xBC
    t_uint32                    prcm_timer_1_ref;       // 0xC0
    t_uint32                    prcm_timer_1_dwncnt;    // 0xC4
    t_uint32                    prcm_timer_1_mode;      // 0xC8
    t_uint32                    prcm_timer_2_ref;       // 0xCC
    t_uint32                    prcm_timer_2_dwncnt;    // 0xD0
    t_uint32                    prcm_timer_2_mode;      // 0xD4

    t_uint32                    unused_2[(0xE4-0xD8)>>2];

    t_prcmu_clk009_mgt_reg      prcm_clk009_mgt;        // 0x00E4  
    t_uint32                    prcm_modeclk;           // 0x00E8
    t_uint32                    unused_3;               // 0x00EC
    t_uint32                    prcm_arm_it_reg;        // 0x00F0
    t_uint32                    prcm_arm_it_clr_reg;    // 0x00F4
    t_uint32                    prcm_4500_clk_req;      // 0x00F8
    t_uint32                    prcm_mbox_cpu_val;      // 0x00FC
    t_uint32                    prcm_mbox_cpu_set;      // 0x0100
    t_uint32                    prcm_mbox_cpu_clr;      // 0x0104
    t_uint32                    unused_4;               // 0x0108
    t_prcmu_clk32k_mgt_reg      prcm_pll32k_enable;     // 0x010C
    t_uint32                    prcm_pll32k_lockp;      // 0x0110
    t_prcmu_arm_chgclkreq_reg   prcm_arm_chgclkreq;     // 0x0114
    t_uint32                    prcm_arm_plldivps;      // 0x0118
    t_uint32                    prcm_armitmsk31to0;     // 0x011C
    t_uint32                    prcm_armitmsk63to32;    // 0x0120
    t_uint32                    prcm_armitmsk95to64;    // 0x0124
    t_uint32                    prcm_armitmsk127to96;   // 0x0128
    t_uint32                    prcm_armitmskxp70_it;   // 0x012C
    t_uint32                    prcm_armstandby_stat;   // 0x0130
    t_uint32                    prcm_cgating_bypass;    // 0x0134
    t_uint32                    prcm_gpiocr;            // 0x0138
    t_uint32                    prcm_lemi_lowemi;       // 0x013C
    t_uint32                    prcm_compcr;            // 0x0140
    t_uint32                    prcm_compsta;           // 0x0144
    t_uint32                    prcm_itstatus0;         // 0x0148
    t_uint32                    prcm_itclr0;            // 0x014C
    t_uint32                    prcm_itstatus1;         // 0x0150
    t_uint32                    prcm_itclr1;            // 0x0154
    t_uint32                    prcm_itstatus2;         // 0x0158
    t_uint32                    prcm_itclr2;            // 0x015C
    t_uint32                    prcm_itstatus3;         // 0x0160
    t_uint32                    prcm_itclr3;            // 0x0164
    t_uint32                    prcm_itstatus4;         // 0x0168
    t_uint32                    prcm_itclr4;            // 0x016C
    t_uint32                    prcm_line_value;        // 0x0170
    t_uint32                    prcm_hold_evt;          // 0x0174  
    t_uint32                    prcm_edge_sens_l;       // 0x0178
    t_uint32                    prcm_edge_sens_h;       // 0x017C
    t_uint32                    prcm_ddr_srefresh_reg;  // 0x0180
    t_uint32                    prcm_ddr_phy_dll_stat_reg; // 0x0184  
    t_uint32                    unused_5;                // 0x0188
    t_uint32                    unused_6;                // 0x018C
    t_uint32                    prcm_debug_ctrl_val;     // 0x0190
    t_uint32                    prcm_debug_no_pwrdown_val; // 0x0194  
    t_uint32                    prcm_debug_ctrl_ack;       // 0x0198
    t_uint32                    prcm_a9pl_force_clkeb_reg; // 0x019C
    t_uint32                    prcm_tpiu_flushin_req;   // 0x01A0
    t_uint32                    prcm_tpiu_flushin_ack;   // 0x01A4  
    t_uint32                    prcm_stp_flushin_req;    // 0x01A8
    t_uint32                    prcm_stp_flushin_ack;    // 0x01AC
    t_uint32                    prcm_hwi2c_div;          // 0x01B0
    t_uint32                    prcm_hwi2c_go;           // 0x01B4  
    t_uint32                    prcm_hwi2c_cmd;          // 0x01B8
    t_uint32                    prcm_hwi2c_data123;      // 0x01BC
    t_uint32                    prcm_hwi2c_sr;           // 0x01C0
    t_uint32                    prcm_remap_cr;           // 0x01C4  
    t_uint32                    prcm_tcr;                // 0x01C8
    t_uint32                    prcm_clkocr;             // 0x01CC
    t_uint32                    prcm_itstatus_dbg;       // 0x01D0
    t_uint32                    prcm_itclr_dbg;          // 0x01D4  
    t_uint32                    prcm_line_value_dbg;     // 0x01D8
    t_uint32                    prcm_dbg_hold;           // 0x01DC
    t_uint32                    prcm_edge_sens_dbg;      // 0x01E0
    t_uint32                    prcm_ape_resetn_set;     // 0x01E4  
    t_uint32                    prcm_ape_resetn_clr;     // 0x01E8
    t_uint32                    prcm_ape_resetn_val;     // 0x01EC
    t_uint32                    prcm_a9_resetn_set;      // 0x01F0
    t_uint32                    prcm_a9_resetn_clr;      // 0x01F4  
    t_uint32                    prcm_a9_resetn_val;      // 0x01F8
    t_uint32                    prcm_mod_resetn_set;     // 0x01FC
    t_uint32                    prcm_mod_resetn_clr;     // 0x0200  
    t_uint32                    prcm_mod_resetn_val;     // 0x0204    
    t_uint32                    prcm_gpio_resetn_set;    // 0x0208
    t_uint32                    prcm_gpio_resetn_clr;    // 0x020C  
    t_uint32                    prcm_gpio_resetn_val;    // 0x0210 
    t_uint32                    prcm_4500_resetn_set;    // 0x0214
    t_uint32                    prcm_4500_resetn_clr;    // 0x0218  
    t_uint32                    prcm_4500_resetn_val;    // 0x021C     
    t_uint32                    prcm_swd_resetn_cfg;     // 0x220
    t_uint32                    prcm_hsi_softrst;        // 0x0224  
    t_uint32                    prcm_ape_softrst;        // 0x0228    
    t_uint32                    prcm_periph4_resetn_set; // 0x022C
    t_uint32                    prcm_periph4_resetn_clr; // 0x0230
    t_uint32                    prcm_periph4_resetn_val; // 0x0234  
    t_uint32                    prcm_swd_rst_tempo;      // 0x038   
    t_uint32                    prcm_rst_4500_tempo;     // 0x023C
    t_uint32                    prcm_svammdsp_it;        // 0x0240
    t_uint32                    prcm_svammdsp_it_clr;    // 0x0244  
    t_uint32                    prcm_siammdsp_it;        // 0x0248   
    t_uint32                    prcm_siammdsp_it_clr;    // 0x024C
    t_uint32                    prcm_stm_data;           // 0x0250  
    t_uint32                    prcm_power_state_set;    // 0x0254    
    t_uint32                    prcm_power_state_clr;    // 0x0258
    t_uint32                    prcm_power_state_val;    // 0x025C
    t_uint32                    prcm_armitvalue31to0;    // 0x0260  
    t_uint32                    prcm_armitvalue63to32;   // 0x0264    
    t_uint32                    prcm_armitvalue95to64;   // 0x0268
    t_uint32                    prcm_armitvalue127to96;  // 0x026C
    t_uint32                    prcm_redun_load;         // 0x0270
    t_uint32                    prcm_redun_status;       // 0x0274  
    t_uint32                    prcm_uniproclk_mgt;      // 0x0278   
    t_uint32                    prcm_uicclk_mgt;         // 0x027C
    t_uint32                    prcm_sspclk_mgt;         // 0x0280
    t_prcmu_clk_mgt_reg         prcm_rngclk_mgt;         // 0x0284  
    t_uint32                    prcm_msp1clk_mgt;        // 0x0288       
    t_uint32                    unused_7[(0x2A0-0x28C)>>2]; // 0x28C - 0x29C     
    t_uint32                    prcm_dap_resetn_set;      // 0x02A0
    t_uint32                    prcm_dap_resetn_clr;      // 0x02A4  
    t_uint32                    prcm_dap_resetn_val;      // 0x02A8
    t_uint32                    unused_8[(0x300-0x2AC)>>2]; // 0x02AC-0x2FC
    t_uint32                    prcm_sram_dedcstov;       // 0x0300
    t_uint32                    prcm_sram_ls_sleep;       // 0x0304  
    t_uint32                    prcm_sram_a9;             // 0x0308
    t_uint32                    prcm_arm_ls_clamp;        // 0x030C
    t_uint32                    prcm_iocr;                // 0x0310
    t_uint32                    prcm_modem_sysclk_reg;    // 0x0314  
    t_uint32                    prcm_sysclk_delay;        // 0x0318
    t_uint32                    prcm_sysclkstatus;        // 0x031C
    t_uint32                    unused_9;                 // 0x0320
    t_uint32                    prcm_dsi_sw_reset;        // 0x0324  
    t_uint32                    prcm_a9_msk_req;          // 0x0328
    t_uint32                    prcm_a9_msk_ack;          // 0x032C   
    t_uint32                    unused_10;                // 0x0330
    t_uint32                    prcm_host_access_req;     // 0x0334  
    t_uint32                    prcm_timer_3_ref;         // 0x0338
    t_uint32                    prcm_timer_3_dwncnt;      // 0x033C
    t_uint32                    prcm_timer_3_mode;        // 0x0340
    t_uint32                    prcm_pmb[(0x400-0x344)>>2]; //0x344-3FC PMB
    t_uint32                    prcm_sem;                 // 0x0400
    t_uint32                    prcm_ape_epod_cfg;        // 0x0404  
    t_uint32                    prcm_ddr_epod_cfg;        // 0x0408
    t_uint32                    unused_11;                // 0x040C
    t_uint32                    prcm_epod_c_set;          // 0x0410
    t_uint32                    prcm_epod_c_clr;          // 0x0414
    t_uint32                    prcm_epod_c_val;          // 0x0418  
    t_uint32                    prcm_epod_vok;            // 0x041C
    t_uint32                    prcm_mmip_ls_clamp_set;   // 0x0420
    t_uint32                    prcm_mmip_ls_clamp_clr;   // 0x0424
    t_uint32                    prcm_mmip_ls_clamp_val;   // 0x0428  
    t_uint32                    prcm_vsafe_ls_clamp_set;  // 0x042C
    t_uint32                    prcm_vsafe_ls_clamp_clr;  // 0x0430   
    t_uint32                    prcm_vsafe_ls_clamp_val;  // 0x0434
    t_uint32                    prcm_ddrsubsys_ape_minbw; // 0x0438  
    t_uint32                    prcm_ddrsubsys_status;    // 0x043C
    t_uint32                    prcm_ddrsubsys_ctrl;      // 0x0440
    t_uint32                    prcm_ddrsubsys_highleakcond;// 0x0444
    t_uint32                    prcm_ddrsubsys_config;    // 0x0448  
    t_uint32                    prcm_ddrsubsys_config_ack;// 0x044C   
    t_uint32                    prcm_timer_4_ref;         // 0x0450
    t_uint32                    prcm_timer_4_dwncnt;      // 0x0454
    t_uint32                    prcm_timer_4_mode;        // 0x0458
    t_uint32                    prcm_timer_5_ref;         // 0x045C
    t_uint32                    prcm_timer_5_dwncnt;      // 0x0460
    t_uint32                    prcm_timer_5_mode;        // 0x0464
    t_uint32                    unused_12;                // 0x0468 
    t_uint32                    prcm_sem_a9_it_clr;       // 0x046C   
    t_uint32                    prcm_ape_mem_req;         // 0x0470
    t_uint32                    prcm_dbg_force_ape_mem_req;// 0x0474
    t_uint32                    prcm_ape_mem_wfx_en;      // 0x0478
    t_uint32                    prcm_ape_mem_latency;     // 0x047C
    t_uint32                    prcm_ape_mem_ack;         // 0x0480
    t_uint32                    prcm_itsatus5;            // 0x0484
    t_uint32                    prcm_itclr5;              // 0x0488
    t_uint32                    prcm_arm_it1_clr;         // 0x048C
    t_uint32                    prcm_arm_it1_set;         // 0x0490
    t_uint32                    prcm_arm_it1_val;         // 0x0494
    t_uint32                    prcm_mod_pwr_ok;          // 0x0498
    t_uint32                    prcm_mod_auxclkok;        // 0x049C
    t_uint32                    prcm_mod_awake_status;    // 0x04A0
    t_uint32                    prcm_mod_swreset_irq_ack; // 0x04A4
    t_uint32                    prcm_mod_swreset_ack;     // 0x04A8
    t_uint32                    prcm_dbg_pwrctl;          // 0x04AC
    t_uint32                    prcm_hwobs_h;             // 0x04B0
    t_uint32                    prcm_hwobs_l;             // 0x04B4
    t_uint32                    unused_13[(0x500-0x4B8)>>2]; //0x4B8-4FC 
    t_uint32                    prcm_plldsi_freq;          // 0x0500
    t_uint32                    prcm_plldsi_enable;        // 0x0504  
    t_uint32                    prcm_plldsi_lockp;         // 0x0508
    t_uint32                    prcm_rng_enable;           // 0x050C
    t_uint32                    prcm_yyclken0_mgt_set;     // 0x0510
    t_uint32                    prcm_yyclken1_mgt_set;     // 0x0514
    t_uint32                    prcm_yyclken0_mgt_clr;     // 0x0518  
    t_uint32                    prcm_yyclken1_mgt_clr;     // 0x051C
    t_uint32                    prcm_yyclken0_mgt_val;     // 0x0520
    t_uint32                    prcm_yyclken1_mgt_val;     // 0x0524
    t_uint32                    prcm_xp70clk_mgt2;         // 0x0528  
    t_uint32                    prcm_dsitvclk_div;         // 0x052C
    t_uint32                    prcm_dsi_pllout_sel;       // 0x0530   
    t_uint32                    prcm_dsi_glitchfree_en;    // 0x0534
    t_uint32                    prcm_clkactiv;             // 0x0538  
    t_uint32                    prcm_sia_mmdsp_mem_mgt;    // 0x053C
    t_uint32                    prcm_sva_mmdsp_mem_mgt;     // 0x0540
    t_uint32                    unused_14a[(0x574-0x544)>>2];// 0x0544-0x570
    t_prcmu_gpiocr2_reg         prcm_gpiocr2;               // 0x0574
    t_uint32                    unused_14b[(0x1000-0x578)>>2];// 0x0578-0xFFC
    t_uint32                    prcm_secnonsewm;           // 0x1000  
    t_uint32                    prcm_esram0_initn;         // 0x1004    
    t_uint32                    unused_15[(0x101C-0x1008)>>2];  // 0x1008-1018
    t_uint32                    prcm_esram0_epod_cfg;      // 0x101C  
    t_uint32                    prcm_esram0_epod_c_val;    // 0x1020 
    t_uint32                    prcm_esram0_epod_c_set;    // 0x1024
    t_uint32                    prcm_esram0_epod_c_clr;    // 0x1028  
    t_uint32                    prcm_esram0_epod_vok;      // 0x102C     
    t_uint32                    prcm_esram0_ls_sleep;      // 0x1030
    t_prcmu_secure_status_reg   prcm_secure_status;        // 0x1034  
    t_uint32                    prcm_i2c_secure;           // 0x1038
    t_prcmu_reset_status_reg    prcm_reset_status;         // 0x103C 
    t_uint32                    unused_16[(0x10D8-0x1040)>>2];// 0x1040-0x10D4    
    t_uint32                    prcm_pipelinedcr;           // 0x10D8
    t_uint32                    prcm_volatileiocr;          // 0x10DC
    t_prcmu_xp70cfgcr_reg       prcm_xp70cfgcr;             // 0x10E0
} t_prcmu_hw_registers;


// PRCMU FW mailbox
typedef union {
    struct{
        t_bitfield  FiqOnGoingCpu0  :1;
        t_bitfield  FiqOnGoingCpu1  :1;
        t_bitfield  Reserved        :6;
   } Bit;
   t_uint8 Reg;    
} t_pcmu_fiq_on_going;

typedef struct
{       
    t_uint8 cmd;
    t_uint8 IsCrypDmaOngoing;
    t_uint8 IsHashDmaOngoing;
    t_uint8 IsDirtySecMemory;
} t_prcmu_fw_mailbox;

// PRCMU FW registers
typedef struct {
//  Type                        Name                    Offset Description
    t_uint32                    unused[(0xA0)>>2];      // 0x00-0xA0
    t_prcmu_fw_mailbox          prcm_fw_mailbox;        // 0xA0
} t_prcmu_fw_registers;


/*------------------------------------------------------------------------
 * Peripheral reset and clock control (PRCC)
 *----------------------------------------------------------------------*/
// PRCC registers
typedef struct {
//  Type                        Name                    Offset Description
    t_uint32                    pcken;             // 0x00
    t_uint32                    pckdis;            // 0x04
    t_uint32                    kcken;             // 0x08
    t_uint32                    kckdis;            // 0x0c
    t_uint32                    pcksr;             // 0x10
    t_uint32                    pkcksr;            // 0x14
} t_prcc_registers;


/*------------------------------------------------------------------------
 * Secure Control Registers (SECR)
 *----------------------------------------------------------------------*/

// scr_CR register depiction
typedef struct
{
    t_bitfield  APEDbgEn0       :1;
    t_bitfield  APENiDEn0       :1;
    t_bitfield  SPNiDEn0        :1;
    t_bitfield  SPIDEn0         :1;
    t_bitfield  APEDbgEn1       :1;
    t_bitfield  APENiDEn1       :1;
    t_bitfield  SPNiDEn1        :1;
    t_bitfield  SPIDEn1         :1;
    t_bitfield  TOPDebugEn      :1;
    const t_bitfield Reserved1  :1;
    t_bitfield  APESTRCEn       :1;
    t_bitfield  APESTMEn        :1;
    t_bitfield  SIADebugEn      :1;
    t_bitfield  APEDebugEn      :1;
    t_bitfield  SPRstEn0        :1;
    t_bitfield  SPRstEn1        :1;
    t_bitfield  PrcmuDebugEn    :1;
    t_bitfield  SFRDis          :1;
    t_bitfield  ResetEn         :1;
    const t_bitfield Reserved2  :4;
    t_bitfield  ModDis          :1;    
    t_bitfield  FBValDis        :1;
    t_bitfield  ScRdAFDis       :1;   
    t_bitfield  SecWDClk        :1;
    t_bitfield  Reserved3       :1;
    t_bitfield  CompJTAGEn      :1;
    t_bitfield  RemapDis        :1;
    t_bitfield  Reserved4       :1;
    t_bitfield  OpenDevn        :1;
} t_scr_cr_reg;

typedef t_uint32 t_scr_wdr_reg;         // Secure watchdog timer value
typedef t_uint32 t_scr_wlr_reg;         // Secure watchdog timer load value

typedef struct
{
    t_bitfield  SWdTDis     :1;         // Secure watchdog timer disable
    t_bitfield  SWdTLDis    :1;         // Secure watchdog timer load disable
    t_bitfield  Reserved    :30;
} t_scr_wcr_reg;

typedef t_uint32 t_scr_fvr_reg;         // Anti-fuse value
typedef t_uint32 t_scr_ramp_reg;        // esRam protection register
typedef t_uint32 t_scr_spr_reg;         // Scratch pad

// TrustZone Secure Region Size Register

typedef union{
    struct{
        t_bitfield RomSize:10;
        const t_bitfield unused1:2; 
        t_bitfield BkpRamSize:10;
        const t_bitfield unused2:10;
   } Bit;
   t_uint32 Reg;
} t_scr_sec_region_size_reg;


// Decode Protection Register
typedef union{
    struct{
        t_bitfield Protec:24;
        const t_bitfield unused:8; 
    } Bit;
   t_uint32 Reg;
}  t_scr_protec_reg;

typedef struct
{
    t_bitfield  CRC0            :8;      
    t_bitfield  CRC1            :8;      
    t_bitfield  CRC2            :8;      
    const t_bitfield Reserved   :8;
} t_scr_crc_fuse_reg;

typedef struct
{
    t_bitfield  ScrEn           :1;      
    const t_bitfield Reserved1  :3;
    t_bitfield  DDRHold         :1;      
    const t_bitfield Reserved2  :27;
} t_scr_scrc_reg;

typedef struct
{
    t_bitfield Info             :8;     // Information (designer, part number, revision, cell id...)
    const t_bitfield Reserved   :24;
} t_scr_id_reg;


typedef struct
{
    t_bitfield ModDbgEn         :1;
    t_bitfield ModNidEn         :1;
    t_bitfield ModSpidEn        :1;        
    t_bitfield ModSpnidEn       :1;
    const t_bitfield Reserved1  :7;
    t_bitfield ModStmDis        :1;
    const t_bitfield Reserved2  :1;
    t_bitfield TopModDbgEn      :1;
    const t_bitfield Reserved3  :2;
    t_bitfield ModTxDisable     :1;
    const t_bitfield Reserved4  :15;                                
} t_scr_modem_reg;



typedef struct
{
    // Type             Name                            // Offset
    t_scr_cr_reg        Control;                        // 0x0000
    t_scr_wdr_reg       WatchdogData;                   // 0x0004
    t_scr_wlr_reg       WatchdogLoad;                   // 0x0008
    t_scr_wcr_reg       WatchdogControl;                // 0x000C
    const t_scr_fvr_reg FuseValue[24];                  // 0x0010...0x006C
    t_scr_ramp_reg      ESRamProtection[6];             // 0x0070...0x0084
    t_uint32            Reserved0[(0x90-0x88)>>2];      // 0x0088
    t_scr_spr_reg       ScratchPad[2];                  // 0x0090...0x0054
    t_scr_sec_region_size_reg SecRegionSize;            // 0x0098
    t_scr_protec_reg    DecProtecReg;                   // 0x009C
    t_uint32            Reserved1[(0x0AC-0x0A0)>>2];    // 0x00A0
    t_uint32            Reserved2[(0x0B0-0x0AC)>>2];    // 0x00AC
    t_scr_crc_fuse_reg  CRCFuseResult;                  // 0x00B0
    t_uint32            Reserved3[(0x0C0-0x0B4)>>2];    // 0x00B4
    t_scr_scrc_reg      ScramblerControl;               // 0x00C0
    t_uint32            ScramblerKeyLow;                // 0x00C4
    t_uint32            ScramblerKeyHigh;               // 0x00C8
    t_uint32            Reserved4[(0x0D0-0x0CC)>>2];    // 0x00CC
    t_uint32            DDRSecOnlyLowAddrCheck;         // 0x00D0     
    t_uint32            DDRSecOnlyHighAddrCheck;        // 0x00D4
    t_uint32            DDRA9OnlyLowAddrCheck;          // 0x00D8  
    t_uint32            DDRA9OnlyHighAddrCheck;         // 0x00DC  
    t_uint32            DDRModemLowAddrCheck;           // 0x00E0     
    t_uint32            DDRModemHighAddrCheck;          // 0x00E4
    t_uint32            DDRShareHighAddrCheck;          // 0x00E8    
    t_uint32            DDRMaxSpaceAddrCheck;           // 0x00EC    
    t_uint32            Reserved5[(0x100-0x0F0)>>2];    // 0x00F0
	t_scr_modem_reg     ModemSecReg;                    // 0x0100
    t_uint32            Reserved6[(0xFE0-0x104)>>2];    // 0x0104
    const t_scr_id_reg  PeriphId[4];                    // 0x0FE0...0x0FEC
    const t_scr_id_reg  PCellId0[4];                    // 0x0FF0...0x0FFC
} t_secure_registers;

/*------------------------------------------------------------------------
 * CRYP processor
 *----------------------------------------------------------------------*/

// CRYP Control Register 
typedef struct
{
    t_bitfield unused_0:1;// deprecated
    t_bitfield PrivN:1;
    t_bitfield AlgoDir:1;
    t_bitfield AlgoMode:3;
    t_bitfield DataType:2;
    t_bitfield KeySize:2;
    t_bitfield KeyReadEn:1;
    t_bitfield KSE:1;
    t_bitfield EXD:1;
    t_bitfield Init:1;
    t_bitfield FFlush:1;
    t_bitfield CrypEn:1;  
    t_bitfield SecondLast:1;          
    t_bitfield unused_1:3;
    t_bitfield ENS:1;     
    t_bitfield unused_2:11;                     
} t_cryp_control_reg;


// CRYP Status Register 
typedef struct
{
    const t_bitfield InFifoEmpty:1;
    const t_bitfield InFifoNotFull:1;
    const t_bitfield OutFifoNotEmpty:1;
    const t_bitfield OutFifoFull:1;
    const t_bitfield Busy:1;
    const t_bitfield unused:27;
} t_cryp_status_reg;


// CRYP DMA Control Register 
typedef struct
{
    t_bitfield InEn:1;
    t_bitfield OutEn:1;
    t_bitfield unused:30;
} t_cryp_dma_control_reg;


// CRYP Interupt Mask Register
typedef struct
{
    t_bitfield InFifoItMask:1;
    t_bitfield OutFifoItMask:1;
    const t_bitfield unused:30; 
} t_cryp_it_mask_reg;


// CRYP Raw Interrupt Status Register
typedef struct
{
    t_bitfield InFifoItRaw:1;
    t_bitfield OutFifoItRaw:1;
    const t_bitfield unused:30; 
} t_cryp_it_raw_reg;


// CRYP Masked Interupt Status Register
typedef struct
{
    t_bitfield InFifoMaskedIt:1;
    t_bitfield OutFifoMaskedIt:1;
    const t_bitfield unused:30; 
} t_cryp_masked_it_reg;


// CRYP Registers
typedef struct {
//  Type                    Name               Offset Description
    t_cryp_control_reg      Control;        // 0x000  Control Register 
    t_cryp_status_reg       Status;         // 0x004  Status Register 
    t_uint32                DataIn;         // 0x008  Data Input Register
    t_uint32                DataInSize;     // 0x00C  Data Input Size Register
    const t_uint32          DataOut;        // 0x010  Data Output Register
    t_uint32                DataOutSize;    // 0x014  Data Output Size Register
    t_cryp_dma_control_reg  DmaControl;     // 0x018  DMA Control Register 
    t_cryp_it_mask_reg      ItMask;         // 0x01C  Interupt Mask Register
    t_cryp_it_raw_reg       RawItStatus;    // 0x020  Raw Interrupt Status Register
    t_cryp_masked_it_reg    MaskedItSTatus; // 0x024  Masked Interupt Status Register
    t_uint32                Key1L;          // 0x028  Key 1, Left word
    t_uint32                Key1R;          // 0x02C  Key 1,            Right word
    t_uint32                Key2L;          // 0x030  Key 2, Left word
    t_uint32                Key2R;          // 0x034  Key 2,            Right word
    t_uint32                Key3L;          // 0x038  Key 3, Left word
    t_uint32                Key3R;          // 0x03C  Key 3,            Right word
    t_uint32                Key4L;          // 0x040  Key 4, Left word
    t_uint32                Key4R;          // 0x044  Key 4,            Right word
    t_uint32                InitVect0L;     // 0x048  Initialization Vector 0, Left  word
    t_uint32                InitVect0R;     // 0x04C  Initialization Vector 0, Right word
    t_uint32                InitVect1L;     // 0x050  Initialization Vector 1, Left  word
    t_uint32                InitVect1R;     // 0x054  Initialization Vector 1, Right word
    const t_uint32          reserved_1[(0x080-0x058)>>2];
    t_uint32                TestControl;    // 0x080: Integration Test Control register
    t_uint32                TestInput;      // 0x084: Integration Test Input register
    t_uint32                TestOutput;     // 0x088: Integration Test Output register
    const t_uint32          reserved_2[(0xFE0-0x08C)>>2];
    const t_uint32          PeriphID[4];    // 0xFE0...0XFEC: Peripheral id registers
    const t_uint32          PCellID[4];     // 0xFF0...0XFFC: PrimeCell id registers
} t_cryp_registers;



/*------------------------------------------------------------------------
 * PKA processor
 *----------------------------------------------------------------------*/

// PKA Control Register 
typedef struct
{
    t_bitfield PkaGo:1;
    t_bitfield PkaStop:1;
    t_bitfield PkaRestore:1;
    t_bitfield unused:29;
} t_pka_control_reg;


// PKA Configuration Register 
typedef struct
{
    t_bitfield unused_0:1;// deprecated
    t_bitfield PrivN:1;    
    t_bitfield AlgoMode:1;
    t_bitfield AlgoCalc:2;
    t_bitfield SecLevel:2;
    t_bitfield DataSwap:2;
    t_bitfield unused:23;
} t_pka_configuration_reg;


// PKA Status Register 
typedef struct
{
    const t_bitfield Busy:1;
    const t_bitfield StatusStop:1;
    const t_bitfield StatusRestore:1;
    const t_bitfield unused:29;
} t_pka_status_reg;


// PKA Interupt Mask Register
typedef struct
{
    t_bitfield ItMask:1;
    const t_bitfield unused:31; 
} t_pka_it_mask_reg;


// PKA Raw Interrupt Status Register
typedef struct
{
    t_bitfield ItRaw:1;
    const t_bitfield unused:31; 
} t_pka_it_raw_reg;


// PKA Masked Interupt Status Register
typedef struct
{
    t_bitfield MaskedIt:1;
    const t_bitfield unused:31; 
} t_pka_masked_it_reg;


// PKA Interrupt Clear Register
typedef struct
{
    t_bitfield ItClear:1;
    const t_bitfield unused:31; 
} t_pka_it_clear_reg;


// PKA Registers
typedef struct {
//  Type                    Name               Offset Description
    t_pka_control_reg       Control;        // 0x000  Control Register 
    t_pka_configuration_reg Config;         // 0x004  Configuration Register
    t_pka_status_reg        Status;         // 0x008  Status Register 
    t_pka_it_mask_reg       ItMask;         // 0x00C  Interrupt Mask Set/Clear register
    t_pka_it_raw_reg        RawItStatus;    // 0x010  Raw Status Interrupt Register
    t_pka_masked_it_reg     MaskedItSTatus; // 0x014  Masked Status Interrupt Register 
    t_pka_it_clear_reg      ItClear;        // 0x018  Interrupt Clear Register
    const t_uint32          reserved_1[(0x800-0x01C)>>2];
    t_uint32                TestControl;    // 0x800: Integration Test Control register
    t_uint32                TestInput;      // 0x804: Integration Test Input register
    t_uint32                TestOutput;     // 0x808: Integration Test Output register
    const t_uint32          reserved_2[(0xFE0-0x80C)>>2];
    const t_uint32          PeriphID[4];    // 0xFE0...0XFEC: Peripheral id registers
    const t_uint32          PCellID[4];     // 0xFF0...0XFFC: PrimeCell id registers
    const t_uint32          reserved_3[(0x1100-0x1000)>>2];
} t_pka_registers;



/*------------------------------------------------------------------------
 * Random Number Generator (RNG)
 *----------------------------------------------------------------------*/
typedef struct
{
    t_bitfield unused_0:1;// deprecated
    t_bitfield PrivN:1;
    t_bitfield Disable:1;
    t_bitfield TestClock:1;
    const t_bitfield unused:28;
} t_rng_control_reg;

typedef struct
{
    t_bitfield ValueReady:1;
    t_bitfield ClockReveal:1;
    t_bitfield FaultReveal:1;
    const t_bitfield unused:29;
} t_rng_status_reg;

typedef struct
{
    const t_bitfield Value:16;
    const t_bitfield unused:16;
} t_rng_rand_val_reg;

typedef struct
{
    t_rng_control_reg   Control;                    // 0x000 : configuration register
    t_rng_status_reg    Status;                     // 0x004 : status register
    t_rng_rand_val_reg  Rand;                       // 0x008 : value register
    const t_uint32      unused_0[(0x80-0x0C)>>2];
    t_uint32            TestControl;                // 0x080
    t_uint32            TestInput;                  // 0x084
    const t_uint32      unused_1[(0xFE0-0x088)>>2];
    const t_uint32      PeriphID[4];                // 0xFE0...0XFEC
    const t_uint32      PCellID[4];                 // 0xFF0...0xFFC
} t_rng_registers;


/*------------------------------------------------------------------------
 * HASH processor
 *----------------------------------------------------------------------*/
typedef struct
{
    t_bitfield unused_0:1;// deprecated
    t_bitfield PrivN:1;
    t_bitfield Init:1;
    t_bitfield DmaEn:1;
    t_bitfield DataFormat:2;
    t_bitfield Mode:1;
    t_bitfield Algo:1;
    t_bitfield NbWord:4;
    t_bitfield DataInFull:1;
    t_bitfield unused_1:3;
    t_bitfield LongKey:1;
    t_bitfield unused_2:3;
    t_bitfield EmptyMsg:1;
    t_bitfield unused_3:11;
} t_hash_control_reg;

typedef struct
{
    t_bitfield NbValidBits:5;
    t_bitfield unused_1:3;  
    t_bitfield DigestCalc:1;
    t_bitfield unused_2:23;
} t_hash_start_reg;

typedef struct
{   t_uint32    Flags;                      // 0x0F8: Context swapping: flags
    t_uint32    DataIn;                     // 0x0FC: Context swapping: input data
    t_uint32    ControlStatus;              // 0x100: Context swapping: control status 
    t_uint32    HashStatus[5];              // 0x104...0x114: Context swapping: Hash status
    t_uint32    Register0;                  // 0x118: Context swapping: Working register 0
    t_uint32    Register1;                  // 0x11C: Context swapping: Working register 1
    t_uint32    Register2;                  // 0x120: Context swapping: Working register 2
    t_uint32    Register3;                  // 0x124: Context swapping: Working register 3
    t_uint32    Register4;                  // 0x128: Context swapping: Working register 4
    t_uint32    MessSched0;                 // 0x12C: Context swapping: Message scheduler 0
    t_uint32    MessSched1;                 // 0x130: Context swapping: Message scheduler 1
    t_uint32    MessSched2;                 // 0x134: Context swapping: Message scheduler 2
    t_uint32    MessSched3;                 // 0x138: Context swapping: Message scheduler 3
    t_uint32    MessSched4;                 // 0x13C: Context swapping: Message scheduler 4
    t_uint32    MessSched5;                 // 0x140: Context swapping: Message scheduler 5
    t_uint32    MessSched6;                 // 0x144: Context swapping: Message scheduler 6
    t_uint32    MessSched7;                 // 0x148: Context swapping: Message scheduler 7
    t_uint32    MessSched8;                 // 0x14C: Context swapping: Message scheduler 8
    t_uint32    MessSched9;                 // 0x150: Context swapping: Message scheduler 9
    t_uint32    MessSched10;                // 0x154: Context swapping: Message scheduler 10
    t_uint32    MessSched11;                // 0x158: Context swapping: Message scheduler 11
    t_uint32    MessSched12;                // 0x15C: Context swapping: Message scheduler 12
    t_uint32    MessSched13;                // 0x160: Context swapping: Message scheduler 13
    t_uint32    MessSched14;                // 0x164: Context swapping: Message scheduler 14
    t_uint32    MessSched15;                // 0x168: Context swapping: Message scheduler 15
    t_uint32    HashConstant;               // 0x16C: Context swapping: Hash constant
    t_uint32    LowBitCount;                // 0x170: Context swapping: Cumulated bit count (LSW)
    t_uint32    tHighBitCount;              // 0x174: Context swapping: Cumulated bit count (HSW)
    t_uint32    CurHmacKey0;                // 0x178: Context swapping: Digest of the current HMAC key 0
    t_uint32    CurHmacKey1;                // 0x17C: Context swapping: Digest of the current HMAC key 1
    t_uint32    CurHmacKey2;                // 0x180: Context swapping: Digest of the current HMAC key 2
    t_uint32    CurHmacKey3;                // 0x184: Context swapping: Digest of the current HMAC key 3
    t_uint32    CurHmacKey4;                // 0x188: Context swapping: Digest of the current HMAC key 4
    t_uint32    StorageHmac0;               // 0x18C: Context swapping: Storage area for HMAC computation 0
    t_uint32    StorageHmac1;               // 0x190: Context swapping: Storage area for HMAC computation 1
    t_uint32    StorageHmac2;               // 0x194: Context swapping: Storage area for HMAC computation 2
    t_uint32    StorageHmac3;               // 0x198: Context swapping: Storage area for HMAC computation 3
    t_uint32    StorageHmac4;               // 0x19C: Context swapping: Storage area for HMAC computation 4
} t_hash_context_regs;


typedef struct
{
//  Type                Name                                   Offset           Description
    t_hash_control_reg  Control;                            // 0x000:           Control register 
    t_uint32            DataIn;                             // 0x004:           Input data
    t_hash_start_reg    Start;                              // 0x008:           Computation start register
    t_uint32            Digest[8];                          // 0x00C...0x028:   Data output register
    const t_uint32      reserved_1[(0x80-0x2C)>>2];         // 0x02C...0x080:   Reserved
    t_uint32            TestControl;                        // 0x080:           Integration Test Control register
    t_uint32            TestInput;                          // 0x084:           Integration Test Input register
    t_uint32            TestOutput;                         // 0x088:           Integration Test Output register
    const t_uint32      reserved_2[(0xF8-0x8C)>>2];         // 0x08C...0x0F4:   Reserved
    t_uint32            ContextSwapReg[54];                 // 0x0F8...0x1CC:   Context Swap reg Xsters
    const t_uint32      reserved_3[(0xFE0-0x1D0)>>2];       // 0x1A0...0xFDC:   Reserved
    const t_uint32      PeriphID[4];                        // 0xFE0...0xFEC:   Peripheral id registers
    const t_uint32      CellID[4];                          // 0xFF0...0xFFC:   PrimeCell id registers
} t_hash_registers;


/*------------------------------------------------------------------------
 * DMA controller
 *----------------------------------------------------------------------*/

#define TOTAL_CHANNELS 8

typedef union{
    struct{
        t_bitfield GlobalClkEn :1;
        t_bitfield unused_0 :7;
        t_bitfield SrcEventGr0ClkEn :1;
        t_bitfield DstEventGr0ClkEn :1;
        t_bitfield SrcEventGr1ClkEn :1;
        t_bitfield DstEventGr1ClkEn :1;
        t_bitfield SrcEventGr2ClkEn :1;
        t_bitfield DstEventGr2ClkEn :1;
        t_bitfield SrcEventGr3ClkEn :1;
        t_bitfield DstEventGr3ClkEn :1;
        t_bitfield unused_1 :16;
   } Bit;
   t_uint32 Reg;

}t_dma_global_clk_ctrl_reg;

/*------------------------------------------------------------------------
 * t_dma_pr_type_reg type covers:      
 *  - Physical resources type
 *----------------------------------------------------------------------*/
typedef union{
    struct{
        t_bitfield CP0Type :2;
        t_bitfield CP1Type :2;
        t_bitfield CP2Type :2;
        t_bitfield CP3Type :2;
        t_bitfield CP4Type :2;
        t_bitfield CP5Type :2;
        t_bitfield CP6Type :2;
        t_bitfield CP7Type :2;
        t_bitfield CP8Type :2;
        t_bitfield CP9Type :2;
        t_bitfield CP10Type :2;
        t_bitfield CP11Type :2;
        t_bitfield CP12Type :2;
        t_bitfield CP13Type :2;
        t_bitfield CP14Type :2;
        t_bitfield CP15Type :2;
    } Bit;
    t_uint32 Reg;
       
}t_dma_pr_type_reg;

/*------------------------------------------------------------------------
 * t_dma_logical_reg type covers:      
 *  - Logical Channels Parameter Base Address
 *  - Logical Channels Link Base Address
 *  - Secure Logical Channels Parameter Base Address
 *  - Secure Logical Channels Link Base Address
 *----------------------------------------------------------------------*/
typedef union
{
    struct {            
        t_bitfield Unused   :10;
        t_bitfield Address  :22;
    } Bit;
    t_uint32 Reg;
}t_dma_logical_reg;

/*------------------------------------------------------------------------
 * t_dma_pr_reg type covers:
 *  - Secure Mode Registers - Secure/not secure...
 *  - Resource Mode Selection Registers - Basic/Logical...
 *  - Resource Mode Option Registers - Basic/Modulo addressing
 *
 * WARNING: these registers all work in pairs; a first one for even 
 *          physical ressources and a 2nd one for odd pr.
 *----------------------------------------------------------------------*/
typedef t_uint32 t_dma_pr_mgmt_reg;

typedef t_uint32 t_dma_int_reg;

/*------------------------------------------------------------------------
 * t_dma_stat_reg type covers:
 *  - Statistics Functions
 *----------------------------------------------------------------------*/
typedef struct
{
    t_bitfield Result   :16;
    t_bitfield LogChSel :4;
    t_bitfield HalfChSel :1;
    t_bitfield PRSel    :5;
    t_bitfield MasterSel :2;
    t_bitfield Mode     :3;
    t_bitfield Enable   :1;
} t_dma_stat_reg;

/*------------------------------------------------------------------------
 * t_dma_imp_cfg_reg type covers:
 *  - DMAC Implementation Configuration Register
 *----------------------------------------------------------------------*/
typedef struct
{
    t_bitfield StdChNb :3;
    t_bitfield MasterNb :2;
    t_bitfield DataBufSize :2;
    t_bitfield CPUBusWidth :1;
    t_bitfield Mst1BusWdth :1;
    t_bitfield Mst2BusWdth :1;
    t_bitfield Mst3BusWdth :1;
    t_bitfield Mst4BusWdth :1;
    t_bitfield PType   :1;
    t_bitfield Unused1 :3;
    t_bitfield ModEn   :4; 
    t_bitfield Unused2 :12;
} t_dma_imp_cfg_reg;

/*----------------------------------------------------------------------*/
/* t_dma_mst_plug_reg type covers:                   */
/* Master Plug Status Registers 0-4                 */
/*----------------------------------------------------------------------*/
typedef t_uint32 t_dma_mst_plug_reg;

/*------------------------------------------------------------------------
 * t_dma_std_cfg_reg type covers:
 *  - Standard Channel Config 0- a possible 31(8 in our case)
 *----------------------------------------------------------------------*/
typedef struct
{
    t_bitfield Evtl    :4;
    t_bitfield TrfMode :2;
    t_bitfield Endian  :1;
    t_bitfield Pri     :1;
    t_bitfield ElSize  :2;
    t_bitfield Psize   :2;
    t_bitfield Pen     :1;
    t_bitfield ErrItMask    :1;
    t_bitfield TCItMask     :1;
    t_bitfield Master       :1;
    t_bitfield Unused :16;
} t_dma_std_cfg_reg;

/*----------------------------------------------------------------------
 * t_dma_std_element_reg type covers:
 *  - Standard Channel Counter 0- a possible 31(6/8 in our case)
 *----------------------------------------------------------------------*/
typedef struct
{
    t_bitfield Increment :16;
    t_bitfield Counter   :16;
    
} t_dma_std_element_reg;

/*------------------------------------------------------------------------
 * t_dma_std_ptr_reg type covers:
 *  - Source and Destination Pointers for SMALL channels
 *----------------------------------------------------------------------*/
typedef t_uint32 t_dma_std_ptr_reg;

/*------------------------------------------------------------------------
 * t_dma_std_lnk_reg type covers:
 *  - Standard Channel Link Register
 *----------------------------------------------------------------------*/
typedef struct
{
    t_bitfield TCPulse :1;
    t_bitfield LnkMasterPort :1;
    t_bitfield Prelink :1;
    t_bitfield NextItemAddr :29;
} t_dma_std_lnk_reg;

/*------------------------------------------------------------------------
 * t_dma_log_cfg_reg type covers:
 *  - Standard Channel Config 0- a possible 31(8 in our case)
 *----------------------------------------------------------------------*/
typedef struct
{
    t_bitfield Unused_0         :4;
    t_bitfield FetchMasterSel   :1;        
    t_bitfield GlobalItMask     :1;
    t_bitfield Endian           :1;
    t_bitfield Pri              :1;
    t_bitfield ElSize           :2;
    t_bitfield Psize            :2;
    t_bitfield AddrIncrement    :1;
    t_bitfield ErrItMask        :1;
    t_bitfield TCItMask         :1;
    t_bitfield Master           :1;
    t_bitfield Unused_1         :16;
} t_dma_log_cfg_reg;

/*----------------------------------------------------------------------
 * t_dma_log_element_reg type covers:
 *  - Standard Channel Counter 0- a possible 31(8 in our case)
 *----------------------------------------------------------------------*/
typedef struct
{
    t_bitfield TCPulse      :1;
    t_bitfield LinkOffset   :7;
    t_bitfield LinkIndex    :8;
    t_bitfield Counter  :16;
    
} t_dma_log_element_reg;

/*------------------------------------------------------------------------
 * t_dma_log_ptr_reg type covers:
 *  - Source and Destination Pointers for SMALL channels
 *----------------------------------------------------------------------*/
typedef t_uint32 t_dma_log_ptr_reg;

/*------------------------------------------------------------------------
 * t_dma_log_lnk_reg type covers:
 *  - Standard Channel Link Register
 *----------------------------------------------------------------------*/
typedef union
{
    struct {    
        t_bitfield Event0 :2;
        t_bitfield Event1 :2;
        t_bitfield Event2 :2;
        t_bitfield Event3 :2;
        t_bitfield Event4 :2;
        t_bitfield Event5 :2;
        t_bitfield Event6 :2;
        t_bitfield Event7 :2;
        t_bitfield Event8 :2;
        t_bitfield Event9 :2;
        t_bitfield Event10 :2;
        t_bitfield Event11 :2;
        t_bitfield Event12 :2;
        t_bitfield Event13 :2;
        t_bitfield Event14 :2;
        t_bitfield Event15 :2;
    } Bit;
    t_uint32 Reg;
} t_dma_log_lnk_reg;


/*------------------------------------------------------------------------
 * t_dma_periph_id_reg  covers:
 * PeriphID0-3
 *----------------------------------------------------------------------*/
typedef t_uint32 t_dma_periph_id_reg;

/*------------------------------------------------------------------------
 * t_dma_cell_id_reg  covers:
 * CellID0-3
 *----------------------------------------------------------------------*/
typedef t_uint32 t_dma_cell_id_reg ;

/*------------------------------------------------------------------------
 * Logical channel standard param (Even params: param0 and param2)
 *----------------------------------------------------------------------*/
typedef union
{
    struct 
    {
        t_bitfield  PtrLSB       :16;
        t_bitfield  ElemCount    :16;
    } Bit;
    t_uint32 Reg;
} t_dma_log_even_params;

/*------------------------------------------------------------------------
 * Logical channel standard param (Odd params: param1 and param3)
 *----------------------------------------------------------------------*/
typedef union 
{
    struct 
    {
        t_bitfield  TCPulse      :1;
        t_bitfield  LinkOffset   :7;
        t_bitfield  ElemSize     :2;
        t_bitfield  PackSize     :2;
        t_bitfield  Incr         :1;            
        t_bitfield  ErrorItMask  :1;                        
        t_bitfield  TermItMask   :1;
        t_bitfield  MasterPort   :1;                        
        t_bitfield  PtrMSB       :16;            
    } Bit;
    t_uint32 Reg;
} t_dma_log_odd_params;

typedef struct
{
    t_dma_log_even_params   Param0;
    t_dma_log_odd_params    Param1;    
    t_dma_log_even_params   Param2;    
    t_dma_log_odd_params    Param3;        
} t_dma_log_param;

/*------------------------------------------------------------------------
 * t_dma_std_channel_desc type covers:
 *  - Standard Channel Descriptor Registers    (0 - 7)
 *----------------------------------------------------------------------*/
typedef struct
{
    t_dma_std_cfg_reg       StdSrcConfig;
    t_dma_std_element_reg   StdSrcElement;
    t_dma_std_ptr_reg       StdSrcPtr;
    t_dma_std_lnk_reg       StdSrcLnk;
    t_dma_std_cfg_reg       StdDestConfig;
    t_dma_std_element_reg   StdDestElement;
    t_dma_std_ptr_reg       StdDestPtr;
    t_dma_std_lnk_reg       StdDestLnk;
} t_dma_std_channel_desc;

/*------------------------------------------------------------------------
 * t_dma_log_channel_desc type covers:
 *  - Logical Channel Descriptor Registers    (0 - 7)
 *----------------------------------------------------------------------*/
typedef struct
{
    t_dma_log_cfg_reg       LogSrcConfig;
    t_dma_log_element_reg   LogSrcElement;
    t_dma_log_ptr_reg       LogSrcPtr;
    t_dma_log_lnk_reg       LogSrcLnk;
    t_dma_log_cfg_reg       LogDestConfig;
    t_dma_log_element_reg   LogDestElement;
    t_dma_log_ptr_reg       LogDestPtr;
    t_dma_log_lnk_reg       LogDestLnk;
} t_dma_log_channel_desc;


typedef union {
     t_dma_std_channel_desc StdChannels[TOTAL_CHANNELS];
     t_dma_log_channel_desc LogChannels[TOTAL_CHANNELS];
}t_dma_channel_param_regs;  


 typedef struct
{
//  Type                        Name                            Relative Address
    t_dma_global_clk_ctrl_reg   GlobalClkControl;               // 0x000
    t_dma_pr_type_reg           PhysicalResourceType;           // 0x004
    t_dma_pr_mgmt_reg           PRSecureMode[2];                // 0x008
    t_dma_pr_mgmt_reg           PRModeSel[2];                   // 0x010
    t_dma_pr_mgmt_reg           PRModeOptions[2];               // 0x018
    t_dma_logical_reg           LogParamBaseAdr;                // 0x020
    t_dma_logical_reg           LogLnkBaseAdr;                  // 0x024
    t_dma_logical_reg           SecureLogParamBaseAdr;          // 0x028
    t_dma_logical_reg           SecureLogLnkBaseAdr;            // 0x02C
    t_uint32                    SecSetGrp[4];                   // 0x030
    t_uint32                    SecClrGrp[4];                   // 0x040
    t_dma_pr_mgmt_reg           PRActivation[2];                // 0x050
    const t_uint32              reserved[2];                    // 0x058
    t_dma_int_reg               MaskIntStat;                    // 0x060
    t_dma_int_reg               IntClr;                         // 0x064
    t_dma_int_reg               TermCountIntStat;               // 0x068
    t_dma_int_reg               ErrIntStat;                     // 0x06C
    t_dma_int_reg               SecMaskIntStat;                 // 0x070
    t_dma_int_reg               SecIntClr;                      // 0x074
    t_dma_int_reg               SecTermCountIntStat;            // 0x078
    t_dma_int_reg               SecErrIntStat;                  // 0x07C
    t_dma_int_reg               LogMaskIntStat[4];              // 0x080
    t_dma_int_reg               LogIntClr[4];                   // 0x090
    t_dma_int_reg               LogTermCountIntStat[4];         // 0x0A0
    t_dma_int_reg               LogErrIntStat[4];               // 0x0B0
    t_dma_int_reg               LogSecMaskIntStat[4];           // 0x0C0
    t_dma_int_reg               LogSecIntClr[4];                // 0x0D0
    t_dma_int_reg               LogSecTermCountIntStat[4];      // 0x0E0
    t_dma_int_reg               LogSecErrIntStat[4];            // 0x0F0
    t_uint32                    ForceSrcEvLineSel[2];           // 0x100
    t_uint32                    ForceSrcEvBurstSel[2];          // 0x108
    const t_uint32              reserved_1[(0x400-0x110)>>2];   // 0x110
    t_dma_channel_param_regs    Channels;                       // 0x400
    const t_uint32              reserved_2[(0xFC8-0x500)>>2];   // 0xC00
    t_dma_stat_reg              Statistics;                     // 0xFC8
    t_dma_imp_cfg_reg           DmaImpConfig;                   // 0xFCC
    t_dma_mst_plug_reg          DmaMstPlug[4];                  // 0xFD0
    t_dma_periph_id_reg         PeriphID[4];                    // 0xFE0
    t_dma_cell_id_reg           CellID[4];                      // 0xFF0
    
} t_dma_registers;



/*------------------------------------------------------------------------
 * Serial Synchronous Receiver Interface (SSI RX)
 *----------------------------------------------------------------------*/
/* SSIRX Configuration Register */ 
typedef struct{
        t_bitfield Mode:3;
        t_bitfield FrameSize:5;
        t_bitfield NbOfChannel:2;
        const t_bitfield unused:14;
        t_bitfield FifoRaS0 : 1;        
        t_bitfield FifoRaS1 : 1;        
        t_bitfield FifoRaS2 : 1;        
        t_bitfield FifoRaS3 : 1;        
        t_bitfield FifoRaS4 : 1;        
        t_bitfield FifoRaS5 : 1;        
        t_bitfield FifoRaS6 : 1;        
        t_bitfield FifoRaS7 : 1;    
}t_ssirx_conf_reg;   

/* SSIRX Buffer Status Register */
typedef struct{
        t_bitfield FifoFull0:1;
        t_bitfield FifoFull1:1;
        t_bitfield FifoFull2:1;
        t_bitfield FifoFull3:1;
        t_bitfield FifoFull4:1;
        t_bitfield FifoFull5:1;
        t_bitfield FifoFull6:1;
        t_bitfield FifoFull7:1;
        const t_bitfield unused1:8;
        t_bitfield FifoEmpty0:1;
        t_bitfield FifoEmpty1:1;
        t_bitfield FifoEmpty2:1;
        t_bitfield FifoEmpty3:1;
        t_bitfield FifoEmpty4:1;
        t_bitfield FifoEmpty5:1;
        t_bitfield FifoEmpty6:1;
        t_bitfield FifoEmpty7:1;        
        t_bitfield unused2:8;
}t_ssirx_bsr_reg;   

/* SSIRX Exception Status Register */
typedef struct{
        t_bitfield unused1 : 16;
        t_bitfield Break : 1;
        t_bitfield Timeout : 1;
        t_bitfield Signal : 1;
        t_bitfield unused2 : 13;
}t_ssirx_esr_reg;

/*----------------------------------------------------------------------*/
/* SSIRX Interrupt Mask Set/Clear Register */
typedef struct{
        t_bitfield Int0:1;
        t_bitfield Int1:1;
        t_bitfield Int2:1;
        t_bitfield Int3:1;
        t_bitfield Int4:1;
        t_bitfield Int5:1;
        t_bitfield Int6:1;
        t_bitfield Int7:1;                                              
        t_bitfield unused1:8;
        t_bitfield ExceptionInt0:1;
        t_bitfield ExceptionInt1:1;
        t_bitfield unused2:14;
}t_ssirx_int_reg;

typedef struct
{
    t_ssirx_conf_reg CFR0;  /* configuration register*/             /*0x000*/
    t_uint32 TOR0;          /* timeout register */                  /*0x004*/
    t_uint32 RSR;           /* reeiver status register*/            /*0x008*/
    t_ssirx_bsr_reg BSR;    /* buffer status register*/             /*0x00C*/
    t_ssirx_esr_reg ESR0;   /* Exception status register 0*/        /*0x010*/
    t_uint32 DMACR;         /* DMA ontrol register*/                /*0x014*/
    t_ssirx_conf_reg CFR1;  /* configuration register*/             /*0x018*/
    t_uint32        TOR1;   /* timeout register */                  /*0x01C*/
    t_ssirx_int_reg IMSC;   /* interrupt mask set/clear reg*/       /*0x020*/
    t_ssirx_int_reg RIS;    /* Raw interrupt status register*/      /*0x024*/
    t_ssirx_int_reg MIS;    /* masked interrupt status register*/   /*0x028*/
    t_uint32 IDSR;          /* Interrupt destination delection reg*//*0x2C*/
    t_ssirx_esr_reg ESR1;   /* Exception status register 1*/        /*0x030*/
    t_uint32 reserved1[(0x040-0x034)>>2];                           /*reserved*/
    t_uint32 RDB[8];        /*SSIR receive data buffer*/            /*0x040*/
    t_uint32 reserved2[(0x080 - 0x060)>>2];                         /*reserved*/
    t_uint32 RSDB[8];       /*SSIR receive swapped data buffer*/    /*0x080*/
    t_uint32 reserved3[(0x0C0 - 0x0A0)>>2];                         /*reserved*/
    t_uint32 RHSDB[8];      /*SSIR half word swapped data buffer*/  /*0x0C0*/
    t_uint32 reserved4[(0x100-0x0E0)>>2];                               /*reserved*/
    t_uint32 TSTCR;         /*SSIR test control register*/              /*0x100*/
    t_uint32 ITIP;          /*SSIR test input register*/                /*0x104*/
    t_uint32 ITOP1;         /*SSIR integration test output 1 register*/ /*0x108*/
    t_uint32 ITOP2;         /*SSIR integration test output2 register */ /*0x10C*/
    t_uint32 reserved6[(0xFE0 - 0x110)>>2];                             /*reserved*/
    t_uint32 PeriphID0;     /* Peripheral Identification reg  */ /*0xFE0*/
    t_uint32 PeriphID1;     /* Peripheral Identification reg  */                                            /*0xFE4*/
    t_uint32 PeriphID2;     /* Peripheral Identification reg  */                                            /*0xFE8*/
    t_uint32 PeriphID3;     /* Peripheral Identification reg  */                                            /*0xFEC*/
    t_uint32 PCellID0;      /* Peripheral Identification reg bit 7:0 */                                            /*0xFF0*/
    t_uint32 PCellID1;      /* Peripheral Identification reg bit 7:0 */                                            /*0xFF4*/
    t_uint32 PCellID2;      /* Peripheral Identification reg bit 7:0 */                                            /*0xFF8*/
    t_uint32 PCellID3;      /* Peripheral Identification reg bit 7:0 */                                            /*0xFFC*/    
} t_ssirx_registers;


/*------------------------------------------------------------------------
 * Serial Synchronous Transmit Interface (SSI TX)
 *----------------------------------------------------------------------*/
/* SSITX Configuration Register */
typedef struct{
        t_bitfield Mode:3;
        t_bitfield FrameSize:5;
        t_bitfield NbOfChannel:2;
        t_bitfield unused1:1;
        t_bitfield ArbitrationPolicy:1;
        t_bitfield IdleStateHoldPeriod:4;
        t_bitfield unused2:8;
        t_bitfield FifoTaS0 : 1;        
        t_bitfield FifoTaS1 : 1;        
        t_bitfield FifoTaS2 : 1;        
        t_bitfield FifoTaS3 : 1;        
        t_bitfield FifoTaS4 : 1;        
        t_bitfield FifoTaS5 : 1;        
        t_bitfield FifoTaS6 : 1;        
        t_bitfield FifoTaS7 : 1;    
}t_ssitx_conf_reg;  
 

/*----------------------------------------------------------------------*/
/* SSITX Buffer Status Register */
typedef struct{
        t_bitfield TxChannel0FifoFull:1;
        t_bitfield TxChannel1FifoFull:1;
        t_bitfield TxChannel2FifoFull:1;
        t_bitfield TxChannel3FifoFull:1;
        t_bitfield TxChannel4FifoFull:1;
        t_bitfield TxChannel5FifoFull:1;
        t_bitfield TxChannel6FifoFull:1;
        t_bitfield TxChannel7FifoFull:1;
        t_bitfield unused1:8;
        t_bitfield TxChannel0FifoEmpty:1;
        t_bitfield TxChannel1FifoEmpty:1;
        t_bitfield TxChannel2FifoEmpty:1;
        t_bitfield TxChannel3FifoEmpty:1;
        t_bitfield TxChannel4FifoEmpty:1;
        t_bitfield TxChannel5FifoEmpty:1;
        t_bitfield TxChannel6FifoEmpty:1;
        t_bitfield TxChannel7FifoEmpty:1;       
        t_bitfield unused2:8;
}t_ssitx_bsr_reg;   

/*----------------------------------------------------------------------*/
/* SSITX Interrupt Mask Set/Clear Register */
typedef struct{
        t_bitfield TxChannel0Int:1;
        t_bitfield TxChannel1Int:1;
        t_bitfield TxChannel2Int:1;
        t_bitfield TxChannel3Int:1;
        t_bitfield TxChannel4Int:1;
        t_bitfield TxChannel5Int:1;
        t_bitfield TxChannel6Int:1;
        t_bitfield TxChannel7Int:1;                                             
        t_bitfield unused:24;
}t_ssitx_int_reg;

/*----------------------------------------------------------------------*/
/* SSITX Bit Rate divisor register */
typedef struct{
        t_uint16 BDR0;
        t_uint16 BDR1;
}t_ssitx_brd_reg;
 
typedef struct
{
    t_ssitx_conf_reg CFR0;  /*SSIT configuration register 0 */          /*0x000*/
    t_ssitx_brd_reg BRD;            /*SSIT bit rate divisor register*/  /*0x004*/
    t_uint32 TSR;           /*SSIT transmitter status register*/        /*0x008*/
    t_ssitx_bsr_reg BSR;    /*SSIT buffer status register */            /*0x00C*/
    t_uint32 BCR;           /*SSIT break count register */              /*0x010*/
    t_uint32 DMACR;         /*SSIT DMA control register */              /*0x014*/
    t_ssitx_conf_reg CFR1;  /*SSIT configuration register 1*/           /*0x018*/
    t_uint32 reserved1;         /*reserved*/
    t_ssitx_int_reg IMSC;   /*SSIT interrupt set/clear register*/       /*0x020*/
    t_ssitx_int_reg RIS;    /*SSIT raw interrupt status register*/      /*0x024*/
    t_ssitx_int_reg MIS;    /*SSIT masked interrupt status register*/   /*0x028*/
    t_ssitx_int_reg IDSR;   /*Interrupt destination selection register*//*0x02C*/
    t_uint32 reserved2[4];      /*reserved*/
    t_uint32 TDB[8];        /*SSIT transmit data buffer */              /*0x040*/
    t_uint32 reserved3[8/*(0x080 - 0x060)>>2*/];        /*reserved*/
    t_uint32 TBSDB[8];      /*SSIT transmit byte swapped buffer */      /*0x080*/
    t_uint32 reserved4[8/*(0x0C0 - 0x0A0)>>2*/];        /*reserved*/
    t_uint32 THSDB[8];      /*SSIT half word swapped data buffer*/      /*0x0C0*/
    t_uint32 reserved5[8/*(0x100-0x0E0)>>2*/];/*reserved*/
    t_uint32 TSTCR;         /*SSIT test control register */             /*0x100*/
    t_uint32 ITIP;          /*SSIT integration test input register */   /*0x104*/
    t_uint32 ITOP1;         /*SSIT Integration test output1 register */ /*0x108*/
    t_uint32 ITOP2;         /*SSIT Integration  test output2 register*/ /*0x10C*/
    t_uint32 reserved6[(0xFE0 - 0x110)>>2];  /*reserved*/
    t_uint32 PeriphID0;     /* Peripheral Identification reg bit 7:0 */  /* 0xFE0*/    
    t_uint32 PeriphID1;     /* Peripheral Identification reg bit 15:8*/  /* 0xFE4*/
    t_uint32 PeriphID2;     /* Peripheral Identification reg bit 23:16*/ /* 0xFE8*/
    t_uint32 PeriphID3;     /* Peripheral Identification reg bit 31:24*/ /* 0xFEC*/
    t_uint32 PCellID0;      /* PrimeCell id register */                  /* 0xFF0*/
    t_uint32 PCellID1;      /* PrimeCell id register */                  /* 0xFF4*/
    t_uint32 PCellID2;      /* PrimeCell id register */                  /* 0xFF8*/
    t_uint32 PCellID3;      /* PrimeCell id register */                  /* 0xFFC*/
} t_ssitx_registers;


/*------------------------------------------------------------------------
 * Universal asynchronous REcevier and Transmitter (UART 0)
 *----------------------------------------------------------------------*/

/* UART Data register Register */
typedef struct{
        t_bitfield Data:8;
        t_bitfield FramingError:1;
        t_bitfield ParityError:1;
        t_bitfield BreakError:1;
        t_bitfield OverunError:1;
        t_bitfield unused:20;
}t_uart_dr_reg; 

/* UART Flag Register */
typedef struct {                    
    t_bitfield ClearToSend:1;
    t_bitfield DataSetReady:1;
    t_bitfield DataCarrierDetect:1;
    t_bitfield Busy:1;
    t_bitfield ReceiveFifoEmpty:1;
    t_bitfield TransmitFifoFull:1;
    t_bitfield ReceiveFifoFull:1;
    t_bitfield TransmitFifoEmpty:1;
    t_bitfield RingIndicator:1;
    t_bitfield DCTS:1;
    t_bitfield DDSR:1;
    t_bitfield DDCD:1;
    t_bitfield TERI:1;
    t_bitfield RTXDIS:1;
    t_bitfield unused:18;
}t_uart_flag_reg;   


/* UART Line Control Register */

typedef union {
    struct {                    
        t_bitfield SendBreak:1;
        t_bitfield ParityEnable:1;
        t_bitfield EvenParitySelect:1;
        t_bitfield TwoStopBitsSelect:1; 
        t_bitfield FifoEnable:1;        
        t_bitfield WordLength:2;    
        t_bitfield StickParitySelect:1; 
        t_bitfield unused:24;   
        }Bit;
    t_uint32 Reg;   
}t_uart_lcr_reg;    


/* UART Control Register */ 
typedef union {
    struct {                    
        t_bitfield UartEnable:1;
        t_bitfield Unused1:2;
        t_bitfield OversamplingFactor:1;
        t_bitfield Unused2:3;
        t_bitfield LoopBackEnable:1;        
        t_bitfield TransmitEnable:1;    
        t_bitfield ReceiveEnable:1; 
        t_bitfield DataTransmitReady:1; 
        t_bitfield RequestToSend:1; 
        t_bitfield Unused3:1;           
        t_bitfield Unused4:1;   
        t_bitfield RTSEnable:1;     
        t_bitfield CTSEnable:1; 
        t_bitfield unused3:16;
    } Bit;
    t_uint32 Reg;               
} t_uart_cr_reg;  
 
 
/* UART Interrupt Clear/Mask Register */ 
typedef union {
     struct {
        t_bitfield RingIndicatorModem:1;
        t_bitfield ClearToSendModem:1;
        t_bitfield DataCarrierDetectModem:1;        
        t_bitfield DataSetReadyModem:1;
        t_bitfield Received:1;
        t_bitfield Transmit:1;
        t_bitfield ReceivedTimeout:1;   
        t_bitfield FramingError:1;
        t_bitfield ParityError:1;
        t_bitfield BreakError:1;
        t_bitfield OverrunError:1;
        t_bitfield XOFFInterrupt:1;
        t_bitfield unused:20;   
    }Bit;
    t_uint32 Reg;
}t_uart_int_reg;     
 
/* UART Interrupt Clear/Mask Register */ 
typedef struct {
    t_bitfield TxIntFifoLevel:3;
    t_bitfield RxIntFifoLevel:3;    
    t_bitfield unused2:26;      
}t_uart_ifls_reg;
 
 
typedef struct
{
    t_uart_dr_reg   DATA;   /* Rx and Tx fifo data register         */      /*0x000*/
    t_uint32        RSR;    /* Receive status register              */      /*0x004*/
    
    t_uint32 unused_1[(0x018-0x008)>>2];    

    t_uart_flag_reg FR;     /* Flag register */                             /*0x018*/
    t_uart_lcr_reg  LCRH_RX; /* Line Control register */                 /*0x01C*/
    t_uint32 unused_2[(0x024-0x020)>>2];    
    
    t_uint32        IBRD;   /* Integer baud rate divisor register */        /*0x024*/   
    t_uint32        FBRD;   /* Fractional baud rate divisor register */     /*0x028*/
    t_uart_lcr_reg  LCRH_TX; /* Line Control register */             /*0x02C*/
    t_uart_cr_reg   CR; /* Control register */                      /*0x030*/
    t_uart_ifls_reg IFLS;   /* Interrupt Fifo level select register */      /*0x034*/
    t_uart_int_reg  IMSC;   /* Interrupt mask set/clear register */ /*0x038*/
    t_uint32        RIS;    /* Raw Interrupt status register */     /*0x03C*/
    t_uart_int_reg  MIS;    /* Masked Interrupt status register */  /*0x040*/
    t_uart_int_reg  ICR;    /* Interrupt Clear register */          /*0x044*/
    t_uint32        DMACR;  /* Dma Control register */              /*0x048*/           
    
    t_uint32 unused_3;
    
    t_uint32    XFCR;     /* Additional control reg for sw flow control*/  /*0x50 */
    t_uint32    XON1;     /* Xon1 character for sw flow control*/   /*0x54 */
    t_uint32    XON2;     /* Xon2 character for sw flow control*/   /*0x58 */
    t_uint32    XOFF1;    /* Xoff1 character for sw flow control*/  /*0x5C */
    t_uint32    XOFF2;    /* Xoff2 character for sw flow control*/  /*0x60 */
    
    t_uint32 unused_4[(0x080-0x064)>>2];
    
    t_uint32 ITCR;      /* Intergration Test Control register */    /*0x080*/
    t_uint32 ITIP;      /* Integration Test Input register */       /*0x084*/
    t_uint32 ITOP;      /* Integration Test Output register */      /*0x088*/
    t_uint32 TDR;       /* Test Data register */                    /*0x08C*/
    
    t_uint32 unused_5[(0x100-0x090)>>2];
    
    t_uint32 ABCR;      /* Autobaud Control register */             /*0x100*/
    t_uint32 ABSR;      /* Autobaud Status register */              /*0x104*/
    t_uint32 ABFMT; /* Autobaud Format register */                  /*0x108*/
    
    t_uint32 unused_6[(0x150-0x10C)>>2];
	
    t_uint32 ABBDR; /* Autobaud Baud Divisor register */            /*0x150*/
    t_uint32 ABBDFR;    /* Autobaud Baud Divisor Fraction register *//*0x154*/
    t_uint32 ABBMR; /* Autobaud Baud Measurement register */        /*0x158*/
    t_uint32 ABIMSC;    /* Autobaud Baud Interrupt Mask register */ /*0x15C*/
    t_uint32 ABRIS; /* Autobaud Baud Raw status register */         /*0x160*/
    t_uint32 ABMIS; /* Autobaud Baud Masked Status register */      /*0x164*/
    t_uint32 ABICR; /* Autobaud Baud Interrupt Clear register */    /*0x168*/       
    
    t_uint32 unused_7[(0xFE0-0x16C)>>2];
    
    t_uint32 PeriphID0; /* Peripheral id register: bits 7:0 */          /*0xFE0*/
    t_uint32 PeriphID1; /* Peripheral id register: bits 15:8 */         /*0xFE4*/
    t_uint32 PeriphID2; /* Peripheral id register: bits 23:16 */        /*0xFE8*/
    t_uint32 PeriphID3; /* Peripheral id register: bits 31:24 */        /*0xFEC*/
    t_uint32 PCellID0;  /* PrimeCell id register: bits 7:0 */           /*0xFF0*/
    t_uint32 PCellID1;  /* PrimeCell id register: bits 15:8 */          /*0xFF4*/
    t_uint32 PCellID2;  /* PrimeCell id register: bits 23:16 */         /*0xFF8*/
    t_uint32 PCellID3;  /* PrimeCell id register: bits 31:24 */         /*0xFFC*/
} t_uart_registers;



/*------------------------------------------------------------------------
 * General Purpose IOs 0...15 (GPIO0)
 *----------------------------------------------------------------------*/
typedef struct
{   //Type   Name           /* Description                              Offset  */
    t_uint32 DAT;           /*  Data register                           0x000   */
    t_uint32 DATS;          /*  Data Set register                       0x004   */
    t_uint32 DATC;          /*  Data Clear register                     0x008   */
    t_uint32 PDIS;          /*  Pull Disable register                   0x00C   */
    t_uint32 DIR;           /*  Direction register                      0x010   */
    t_uint32 DIRS;          /*  Direction set register                  0x014   */
    t_uint32 DIRC;          /*  Direction Clear register                0x018   */
    t_uint32 SLPM;          /*  Sleep Mode register                     0x01C   */
    t_uint32 AFSLA;         /*  Alternate Function Select register A    0x020   */
    t_uint32 AFSLB;         /*  Alternate Function Select register B    0x024   */
    t_uint32 LOWEMI;        /*  reserved                                0x028   */
    t_uint32 unused1;       /*  reserved                                0x02C   */
    t_uint32 unused2;       /*  reserved                                0x030   */
    t_uint32 unused3;       /*  reserved                                0x034   */
    t_uint32 unused4;       /*  reserved                                0x038   */
    t_uint32 unused5;       /*  reserved                                0x03C   */
    t_uint32 RIMSC;         /*  Interrupt Mask Set Clear register       0x040   */
    t_uint32 FIMSC;         /*  Interrupt Mask Set Clear register       0x044   */
    t_uint32 IS;            /*  Interrupt Sense register                0x048   */
    t_uint32 IC;            /*  Interrupt clear register                0x04C   */
    t_uint32 RWMSC;         /*  Interrupt Mask Set Clear register       0x050   */
    t_uint32 FWMSC;         /*  Interrupt Mask Set Clear register       0x054   */
    t_uint32 WKS;           /*  Interrupt Mask Set Clear register       0x058   */
} t_gpio_registers; 


/*------------------------------------------------------------------------
 * Timers Unit (MTU)
 *----------------------------------------------------------------------*/
typedef struct
{
    t_bitfield OneShot:1;
    t_bitfield Size:1;
    t_bitfield Prescaler:2;
    t_bitfield unused1:2;       
    t_bitfield Mode:1;  
    t_bitfield Enable:1;
    t_bitfield unused2:24;          
}t_tmr_ctrl_reg;

typedef struct
{
    t_uint32        IMSC;       /* @0x00 */
    t_uint32        RIS;        /* @0x04 */
    t_uint32        MIS;        /* @0x08 */
    t_uint32        IntClr;     /* @0x0C */
 
    t_uint32        TMR1Load;       /* @0x10 */
    t_uint32        TMR1Value;      /* @0x14 */
    t_tmr_ctrl_reg  TMR1Control;    /* @0x18 */
    t_uint32        TMR1BGLoad;     /* @0x1C */

    t_uint32        TMR2Load;       /* @0x20 */
    t_uint32        TMR2Value;      /* @0x24 */
    t_tmr_ctrl_reg  TMR2Control;    /* @0x28 */
    t_uint32        TMR2BGLoad;     /* @0x2C */

    t_uint32        TMR3Load;       /* @0x30 */
    t_uint32        TMR3Value;      /* @0x34 */
    t_tmr_ctrl_reg  TMR3Control;    /* @0x38 */
    t_uint32        TMR3BGLoad;     /* @0x3C */

    t_uint32        TMR4Load;       /* @0x40 */
    t_uint32        TMR4Value;      /* @0x44 */
    t_tmr_ctrl_reg  TMR4Control;    /* @0x48 */
    t_uint32        TMR4BGLoad;     /* @0x4C */

    t_uint32        unused_1[(3840-80)/sizeof(t_uint32)];

    t_uint32        ITCR;           /* @0xF00 */
    t_uint32        ITOP;           /* @0xF04*/

    t_uint32        unused_2[(4064-3848)/sizeof(t_uint32)];

    t_uint32        PeriphID0;      /* @0xFE0 */
    t_uint32        PeriphID1;      /* @0xFE4 */
    t_uint32        PeriphID2;      /* @0xFE8 */
    t_uint32        PeriphID3;      /* @0xFEC */
    t_uint32        PCellID0;       /* @0xFF0 */
    t_uint32        PCellID1;       /* @0xFF4 */
    t_uint32        PCellID2;       /* @0xFF8 */
    t_uint32        PCellID3;       /* @0xFFC */
} t_mtu_registers;


/*------------------------------------------------------------------------
 * Real time clock (RTC)
 *----------------------------------------------------------------------*/
typedef struct
{
    t_bitfield ClockWatch:1;
    t_bitfield Timer:1;
    t_bitfield unused:30;       // Reserved
}t_rtc_twobits_reg;

typedef struct
{
    t_uint32            CWDR;       // 0x00 
    t_uint32            CWDMR;      // 0x04 
    t_uint32            CWDLR;      // 0x08 
    t_uint32            CTCR;       // 0x0C 
    t_rtc_twobits_reg   IMSC;       // 0x10 
    t_rtc_twobits_reg   RIS;        // 0x14 
    t_rtc_twobits_reg   MIS;        // 0x18 
    t_rtc_twobits_reg   ICR;        // 0x1C 
    
    t_uint32            TDR;        // 0x20 
    t_uint32            TLR1;       // 0x24 
	
    t_rtc_twobits_reg   TCR;        // 0x28
    t_uint32            TLR2;       // 0x2C 

    t_uint32            CWYR;       // 0x30
    t_uint32            CWYMR;      // 0x34
    t_uint32            CWYLR;      // 0x38
	
    t_uint32            TPR1;       // 0x3C 
    t_uint32            TPR2;       // 0x40 
    t_uint32            TPR3;       // 0x44 
    t_uint32            TPR4;       // 0x48 
    
    t_uint32            TIN;        // 0x4C 
    t_uint32            TWK;        // 0x50
    t_uint32            SMIN;       // 0x54
    
    t_uint32            unused_2[(0x80-0x58)/sizeof(t_uint32)];
    t_uint32            ITCR;       // 0x80 
    t_uint32            ITIR;       // 0x84 
    t_uint32            ITOP;       // 0x88 
    t_uint32            unused_3[(0xFE0-0x8C)/sizeof(t_uint32)];
    t_uint32            PeriphID0;  // 0xFE0 
    t_uint32            PeriphID1;  // 0xFE4 
    t_uint32            PeriphID2;  // 0xFE8 
    t_uint32            PeriphID3;  // 0xFEC 
    t_uint32            PCellID0;   // 0xFF0 
    t_uint32            PCellID1;   // 0xFF4 
    t_uint32            PCellID2;   // 0xFF8 
    t_uint32            PCellID3;   // 0xFFC 
} t_rtc_registers;


/*------------------------------------------------------------------------
 * Real time timer (RTT)
 *----------------------------------------------------------------------*/
typedef struct
{
    t_bitfield CKDIV:16;        // Clock divider
    t_bitfield unused:16;       // Reserved
}t_rtt_trim_ctrl_reg;

typedef struct
{
    t_bitfield RTTOS:1;           // OneShot
    t_bitfield RTTEN:1;           // EnableSize
    t_bitfield RTTENS:1;          // EnableSizeStatus (Read only)
    t_bitfield RTTByPassGated:1;  // Clock (no need to be changed, reset value = 0)        
    t_bitfield unused:28;         // Reserved
}t_rtt_ctrl_reg;

typedef struct
{
    t_bitfield Command:1;
    t_bitfield unused:31;       // Reserved
}t_rtt_onebit_reg;

typedef struct
{
    t_rtt_trim_ctrl_reg CTCR;               // 0x00 : counter trim and control reg
    t_rtt_onebit_reg    IMSC;               // 0x04 : interrupt mask reg
    t_rtt_onebit_reg    RIS;                // 0x08 : raw interrupt status reg
    t_rtt_onebit_reg    MIS;                // 0x0C : masked interrupt status reg
    t_rtt_onebit_reg    ICR;                // 0x10 : interupt clear reg
    t_uint32            DR;                 // 0x14 : data reg
    t_uint32            LR;                 // 0x18 : time load reg
    t_rtt_ctrl_reg      CR;                 // 0x1C : control reg
    t_uint32            unused_1[(0x80-0x20)/sizeof(t_uint32)];
    t_uint32            ITCR;               // 0x80 : integration test control reg
    t_uint32            ITOP;               // 0x84 : integration test output reg
    t_uint32            unused_2[(0xFE0-0x88)/sizeof(t_uint32)];
    t_uint32            PeriphID0;          // 0xFE0 
    t_uint32            PeriphID1;          // 0xFE4 
    t_uint32            PeriphID2;          // 0xFE8 
    t_uint32            PeriphID3;          // 0xFEC 
    t_uint32            PCellID0;           // 0xFF0 
    t_uint32            PCellID1;           // 0xFF4 
    t_uint32            PCellID2;           // 0xFF8 
    t_uint32            PCellID3;           // 0xFFC 
} t_rtt_registers;


/*------------------------------------------------------------------------
 * USB-OTG module for DEVICE
 *----------------------------------------------------------------------*/

/* Power Register bit definition */
#define USB_PWR_SFCN       MASK_BIT6    /* Soft connect */
#define USB_PWR_HSEN       MASK_BIT5    /* HS enable    */
#define USB_PWR_HSMD       MASK_BIT4    /* HS mode      */
#define USB_PWR_SUSPM      MASK_BIT0    /* Enable suspend M */

/* Interrupt registers bit definition    */
#define USB_INTEP0         MASK_BIT0
#define USB_INTEP1         MASK_BIT1
#define USB_INTEP2         MASK_BIT2
#define USB_INTEP3         MASK_BIT3
#define USB_INTEP4         MASK_BIT4
#define USB_INTEP5         MASK_BIT5
#define USB_INTEP6         MASK_BIT6
#define USB_INTEP7         MASK_BIT7
#define USB_INTEP8         MASK_BIT8
#define USB_INTEP9         MASK_BIT9
#define USB_INTEP10        MASK_BIT10
#define USB_INTEP11        MASK_BIT11
#define USB_INTEP12        MASK_BIT12
#define USB_INTEP13        MASK_BIT13
#define USB_INTEP14        MASK_BIT14
#define USB_INTEP15        MASK_BIT15
#define USB_INTUSB_RST     MASK_BIT2    /* Reset          */
#define USB_INTUSB_SOF     MASK_BIT3    /* Start of Frame */

/* Endpoint index mask  */
#define USB_ENDPNO_INDX    0x0F

/* RX Control / Status register bit definition */
#define USB_RXCSR_CLR             MASK_BIT7    /* clear data toggle */
#define USB_RXCSR_SNT             MASK_BIT6    /* Sent Stall       */
#define USB_RXCSR_SND             MASK_BIT5     /* Send Stall       */
#define USB_RXCSR_OR              MASK_BIT2    /* Overrun bit      */
#define USB_RXCSR_ERR             MASK_BIT3    /* Data Error       */
#define USB_RXCSR_RRDY            MASK_BIT0    /* RX Packet Ready  */

/* TX Control/Status register bit definition */
#define USB_TXCSR_MD              MASK_BIT13    /* Mode direction TX or RX */ 
#define USB_TXCSR_CLR             MASK_BIT6    /* clear data toggle */
#define USB_TXCSR_SNT             MASK_BIT5     /* Sent Stall       */
#define USB_TXCSR_SND             MASK_BIT4     /* Send Stall       */
#define USB_TXCSR_UND             MASK_BIT2     /* Clear Stall      */ 
#define USB_TXCSR_TRDY            MASK_BIT0     /* TX packet ready  */
    
/* TX Control and Status register bit definition */
#define USB_TXCSR_RRDY            MASK_BIT0

/* Endpoint 0 Control and Status register bit definition */
#define USB_CSR0_RRDY           MASK_BIT0 /* RX packet ready  */  
#define USB_CSR0_TRDY           MASK_BIT1 /* TX packet ready  */ 
#define USB_CSR0_SNTSTALL       MASK_BIT2 /* Sent Stall       */
#define USB_CSR0_DATAEND        MASK_BIT3   
#define USB_CSR0_SERRDY         MASK_BIT6 /* serviced RX Packet Ready */
#define USB_CSR0_SNDSTALL       MASK_BIT5 /* Send Stall   */ 
#define USB_CSR0_SETUPEND       MASK_BIT4  
#define USB_CSR0_SERSETEND      MASK_BIT7  /* Service setup end    */

/* Max Packet Size register (TX and Rx) bit defintion */
#define USB_MAXP           0x7FF

/* Test mode register   */
#define USB_TM_TSTNAK       MASK_BIT0
#define USB_TM_TSTJ         MASK_BIT1
#define USB_TM_TSTK         MASK_BIT2
#define USB_TM_TSTPKT       MASK_BIT3
#define USB_TM_FHS          MASK_BIT4
#define USB_TM_FFS          MASK_BIT5
#define USB_TM_FIFOA        MASK_BIT6
#define USB_TM_FRH          MASK_BIT7

/* INTUSBEN register*/
#define USB_INTUSBEN_SSPEN      MASK_BIT0 //suspend
#define USB_INTUSBEN_RESEN      MASK_BIT1 // resume
#define USB_INTUSBEN_RSTEN      MASK_BIT2 // reset
#define USB_INTUSBEN_SOFEN      MASK_BIT3 //SOF
#define USB_INTUSBEN_CONEN      MASK_BIT4 // connect
#define USB_INTUSBEN_DSCEN      MASK_BIT5 // diconnect
#define USB_INTUSBEN_SREQN      MASK_BIT6 // session request
#define USB_INTUSBEN_VBEN       MASK_BIT7 // vbus error

/* Byte Received Counter for RX EPs */
#define USB_RXCNT          0x1FFF

/* Device control register bit definition */
#define USB_DEVCTL_VBUS    MASK_BIT4
#define ABOVE_AVALID       0x10


/* USBOTG  Top Control register   */
typedef struct {                   
        t_bitfield ModeUlpi:1;
        t_bitfield Reserved:2;
        t_bitfield Xgate:1;
        t_bitfield I2Coff:1;        
        t_bitfield HDev:1;  
        t_bitfield VBusLo:1;    
        t_bitfield unused:25;        
}t_usb_topctrl_reg;

#define USB_TOPCRTL_FS  0x3     /* Standard full speed interface */
#define USB_TOPCTRL_T3W 0x1     /* transceiver-less 3-wire interface */
#define USB_TOPCTRL_T5W 0x2     /* transceiver-less 3-wire interface */
#define USB_SOFTRST  0x3        /* Val of SOFTRST to reset the USB*/

/* Target Address Register Structure */
typedef struct
{
    t_uint8 TXFAD;      /* Tx Function address register */
    t_uint8 FILL0;
    t_uint8 TXHAD;      /* TX Hub Address register      */
    t_uint8 TXHP;       /* TX Hub Port register         */
    t_uint8 RXFAD;      /* TX Function Address register */
    t_uint8 FILL1;  
    t_uint8 RXHAD;      /* RX Hub address register      */
    t_uint8 RXHP;       /* RX Hub Port register         */

}t_usb_ta_ctrl_reg;

/* Control Status register structure */
typedef struct
{
    t_uint16 TXMAXP;    /* TX Max Packet Size Register     */ 
    t_uint16 CSR;       /* TX Control/Status register      */
    t_uint16 RXMAXP;    /* RX Max PAcket Size register     */
    t_uint16 RXCSR;     /* RX Control/Status register      */ 
    t_uint16 RXCNT;     /* RX Bytes Received Counter       */
    t_uint8 TXTYPE;     /* TX Transaction Control register */
    t_uint8 TXINTV;     /* TX Polling Interval register    */ 
    t_uint8 RXTYPE;     /* RX Transaction Control register */
    t_uint8 RXINTV;     /* RX Polling Interval register    */
    t_uint8 FILL35;
    t_uint8 CFD;        /* Core configuration regsiter     */
}t_usb_csr_reg;

typedef struct {

    t_uint8  FADDR;     /* Function Address Register                     0x00 */
    t_uint8  PWR;       /* Power Management Register                     0x01 */
    t_uint16 INTTX;     /* TX Interrupt register (EP0 & TX EPs)          0x02 */
    t_uint16 INTRX;     /* RX Interrupt register (RX EPs)                0x04 */
    t_uint16 INTTXEN;   /* Interrupt enable register for USB_INTTX       0x06 */
    t_uint16 INTRXEN;   /* Interrupt enable register for USB_INTRX       0x08 */   
    t_uint8  INTUSB;    /* Interrupt register for common USB interrupts  0x0A */
    t_uint8  INTUSBEN;  /* Interrupt enable register for USB_INTUSB      0x0B */
    t_uint16 FMNO;      /* Frame number register                         0x0C */
    t_uint8  INDX;      /* Index register                                0x0E */
    t_uint8  TM;        /* Register to enable the USB2.0 test modes      0x0F */

    /* Indexed Endpoint control/status registers */
    t_uint16 TXMAXP;    /* 0x10 */
    t_uint16 TXCSR;     /* 0x12 */
    t_uint16 RXMAXP;    /* 0x14 */
    t_uint16 RXCSR;     /* 0x16 */
    t_uint16 RXCNT;     /* 0x18 */
    t_uint8  FILL0[5];  /* 0x1A - 0x1E */
    t_uint8  CFD;       /* 0x1F */

    /* FIFOs for endpoints */
    t_uint32 FIFO[16];  /* 0x20 - 0x5C */

    /* Additional device control and status registers */
    t_uint8  DEVCTL;    /* 0x60 */
    t_uint8  FILL1;     /* 0x61 */
    t_uint8  TXFSZ;     /* 0x62 */
    t_uint8  RXFSZ;     /* 0x63 */
    t_uint16 TXFA;      /* 0x64 */
    t_uint16 RXFA;      /* 0x66 */
    t_uint32 VCNTL;     /* 0x68 */
    t_uint16 HWVER;     /* 0x6C */
    t_uint16 FILL2[6];  /* 0x6E */
    t_uint8  LINKINFO;  /* 0x7A */
    t_uint8  FILL3[4];
    t_uint8  SOFTRST;   /* 0x7F */

    /* Target Address control registers for endpoint 0-15*/
    t_usb_ta_ctrl_reg   TA_REG[16]; /* 0x80 -> 0xFF */

    /* Control Status register for Endpoint 0-15 (HOST mode) */
    t_usb_csr_reg       CSR_REG[16];    /* 0x100 -> 0x1FF */

//    t_uint32 DMASEL;    /*OTG DMA Selector register*/
    t_uint32 FILL4;
    t_usb_topctrl_reg  TOPCTRL;

} t_usb_register;


/*------------------------------------------------------------------------
 * MMC registers
 *----------------------------------------------------------------------*/

typedef union {
    struct {                     
        t_bitfield Pwrctrl:2;
        t_bitfield Dat2dirEn:1;
        t_bitfield CmdDirEn:1;
        t_bitfield Dat0dirEn:1;
        t_bitfield Dat31dirEn:1;
        t_bitfield Opd:1;
        t_bitfield FbClkEn:1;
        t_bitfield Dat74dirEn:1;
        t_bitfield Reserved:23;
        } Bit;
        t_uint32 Reg;
  } t_mmc_power_reg;

typedef union {
    struct {                     
        t_bitfield ClkDiv:8;
        t_bitfield ClkEn:1;
        t_bitfield PwrSav:1;
        t_bitfield Bypass:1;
        t_bitfield WidBus:2;
        t_bitfield NegEdge:1;
        t_bitfield HwFc:1;
        t_bitfield ClkOutInv:1;
        t_bitfield ClkGateEn:1;
        t_bitfield Reserved:15;
        } Bit;
        t_uint32 Reg;
   }t_mmc_clk_reg;
    
typedef union {
    struct {                    
        t_bitfield CmdIndex:6;
        t_bitfield WaitResp:1;
        t_bitfield LongResp:1;
        t_bitfield WaitIT:1;
        t_bitfield WaitPend:1;
        t_bitfield CpsmEn:1;
        t_bitfield Unused:5; /* sdioSusp, enCmdCompl, nIEn(interrupt ce-ata),ce-ataCmd,cBootModeEn*/
        t_bitfield Reserved:16;
        } Bit;
        t_uint32 Reg;
    } t_mmc_cmd_reg;
    
    
typedef struct {                   
    t_bitfield CmdCrcFail:1;
    t_bitfield DataCrcFail:1;
    t_bitfield CmdTimeout:1;
    t_bitfield DataTimeout:1;
    t_bitfield TxUnderrunErr:1;
    t_bitfield RxOverrunErr:1;
    t_bitfield CmdRespEnd:1;
    t_bitfield CmdSent:1;
    t_bitfield DataEnd:1;
    t_bitfield StartBitErr:1;
    t_bitfield DataBlkEnd:1;
    t_bitfield CmdAct:1;
    t_bitfield TxAct:1;
    t_bitfield RxAct:1;
    t_bitfield TxFifoBurstWable:1; //Transmit FIFO Burst Writable: at least a burst (8 words) can be written in the FIFO.
    t_bitfield RxFifoBurstRable:1; //Receive FIFO Burst Readable: there is at least a burst (8 words) in the FIFO.
    t_bitfield TxFifoFull:1;
    t_bitfield RxFifoFull:1;
    t_bitfield TxFifoEmp:1;
    t_bitfield RxFifoEmp:1;
    t_bitfield TxDataAvail:1;
    t_bitfield RxDataAvail:1;
    t_bitfield Unused:6; // sdioInt,ce-ataEnd,cardBusy,bootMode,bootAckErr,bootAckTimeout
    t_bitfield Reserved:4;
} t_mmc_status_reg; 


typedef union {
    struct {                    
        t_bitfield Reserved:8;
        t_bitfield ReadyForData:1;
        t_bitfield State:4;
        t_bitfield EraseReset:1;
        t_bitfield Unused:5;
        t_bitfield LastCmdErr:1;
        t_bitfield InternalErr:1;
        t_bitfield CardEccFail:1;
        t_bitfield IllegalCmd:1;
        t_bitfield CmdCrcErr:1;
        t_bitfield LockFail:1;
        t_bitfield LockedCard:1;
        t_bitfield WPViolation:1;
        t_bitfield ErraseErr:2;
        t_bitfield BlkLenErr:1;
        t_bitfield MisalignAddr:1;
        t_bitfield OutRangeAddr:1;
        } R1Bit;
        t_uint32 Reg;
} t_mmc_resp0_reg;  

typedef union {
    struct {                        
        t_bitfield CmdCrcFail:1;
        t_bitfield DataCrcFail:1;
        t_bitfield CmdTimeout:1;
        t_bitfield DataTimeout:1;
        t_bitfield TxUnderrunErr:1;
        t_bitfield RxOverrunErr:1;
        t_bitfield CmdRespEnd:1;
        t_bitfield CmdSent:1;
        t_bitfield DataEnd:1;
        t_bitfield StartBitErr:1;
        t_bitfield DataBlkEnd:1;
        t_bitfield Reserved:21;
        }Bit;
        t_uint32 Reg;
} t_mmc_clear_reg;  


typedef  union {
        struct {                    
        t_bitfield DataTransEn:1;
        t_bitfield DataTransDir:1;
        t_bitfield DataTransMode:1;
        t_bitfield DmaEn:1;
        t_bitfield Unused1:4;
        t_bitfield Unused2:8;/*rwStart,rwStop,rwmod,sdioEn,dmareqCtl,dBootModeEn,busyMod, ddr_mode*/
        t_bitfield DataBlkSize:15;
        t_bitfield Reserved:1; 
        }Bit;
        t_uint32 Reg;
} t_mmc_data_ctrl_reg;
/* MMC registers */

typedef struct {
t_mmc_power_reg PWR;         /* Power Control Reg    0x00 */
t_mmc_clk_reg CLKCR;         /* Clock Control Reg    0x04 */
t_uint32 ARG;                /* Argument Reg         0x08 */
t_mmc_cmd_reg CMD;           /* Command Reg          0x0c */
t_uint32 RESPCMD;            /* Response Command reg 0x10 */
const t_mmc_resp0_reg RESP0; /* Response Reg 0       0x14 */
const t_uint32 RESP1;        /* Response Reg 1       0x18 */
const t_uint32 RESP2;        /* Response Reg 2       0x1c */
const t_uint32 RESP3;        /* Response Reg 3       0x20 */
t_uint32 DTIMER;             /* Data Timer Reg       0x24 */
t_uint32 DLEN;               /* Data Length Reg      0x28 */
t_mmc_data_ctrl_reg DCTRL;   /* Data Control Reg     0x2c */
t_uint32 DCNT;               /* Data counter Reg     0x30 */
const t_mmc_status_reg STATUS;  /* Status Reg        0x34 */
t_mmc_clear_reg ICR;         /* Interrupt Clear Reg  0x38 */
t_uint32 MASK;               /* Interrupt Mask Reg   0x3c */
t_uint32 reserved1;          /* 0x40 */
t_uint32 reserved2;          /* 0x44 */
t_uint32 FIFOCNT;            /* FIFO Counter Reg     0x48 */
t_uint32 reserved3[(0x80-0x4C)>>2]; 
t_uint32 FIFO[16];           /* Data FIFO Reg        0x80-0xBC */
t_uint32 DBTIMER;            /* Boot ack timer Reg   0xC0 */
t_uint32 reserved4[(0xFE0-0x84)>>2]; 
const t_uint32 PeriphID0;    /* Peripheral Identification Reg 0xFE0 */
const t_uint32 PeriphID1;    /* 0xFE4 */
const t_uint32 PeriphID2;    /* 0xFE8 */
const t_uint32 PeriphID3;    /* 0xFEC */

const t_uint32 PCellID0;     /* Cell Identification Reg  0xFF0 */
const t_uint32 PCellID1;     /* 0xFF4 */
const t_uint32 PCellID2;     /* 0xFF8 */
const t_uint32 PCellID3;     /* 0xFFc */
}t_mmc_register;


/*------------------------------------------------------------------------
 * SCU Snoop Control Unit 
 *----------------------------------------------------------------------*/

// SCU control Register
typedef struct
{
    t_bitfield Enable:1;
    t_bitfield AddrFiltEn:1;
    t_bitfield PartyOn:1; 
    t_bitfield unused:29;    
} t_scu_control_reg;

// SCU configuration Register
typedef struct
{
    t_bitfield CpuNumber:2;
    t_bitfield Sbz:2;
    t_bitfield CpuSmp:4; 
    t_bitfield TagRamSizes:8;
    t_bitfield unused:16;    
} t_scu_config_reg;

// SCU cpu power status Register
typedef struct
{
    t_bitfield Cpu0Satus:2;
    t_bitfield Cpu1Satus:2;
    t_bitfield Cpu2Satus:2;
    t_bitfield Cpu3Satus:2;
    t_bitfield unused:24;    
} t_scu_power_status_reg;

// SCU invalidate all in non secure state Registe
typedef struct
{
    t_bitfield Cpu0Ways:4;
    t_bitfield Cpu1Ways:4;
    t_bitfield Cpu2Ways:4;
    t_bitfield Cpu3Ways:4;
    t_bitfield unused:16;    
} t_scu_inval_non_sec_reg;

// SCU invalidate all in secure state Register
typedef struct
{
    t_bitfield Cpu0Ways:4;
    t_bitfield Cpu1Ways:4;
    t_bitfield Cpu2Ways:4;
    t_bitfield Cpu3Ways:4;
    t_bitfield unused:16;    
} t_scu_inval_sec_reg;

// SCU filtering start Register
typedef struct
{
    t_bitfield unused:20; 
    t_bitfield StartAddr:12;   
} t_scu_filt_start_addr_reg;

// SCU filtering end Register
typedef struct
{
    t_bitfield unused:20; 
    t_bitfield EndAddr:12;   
} t_scu_filt_end_addr_reg;

// SCU access control Register
typedef struct
{
    t_bitfield Cpu0:1;
    t_bitfield Cpu1:1;
    t_bitfield Cpu2:1;
    t_bitfield Cpu3:1;    
    t_bitfield unused:28;    
} t_scu_access_ctrl_reg;

// SCU secure access control Register
typedef struct
{
    t_bitfield Cpu0:1;
    t_bitfield Cpu1:1;
    t_bitfield Cpu2:1;
    t_bitfield Cpu3:1;  
    t_bitfield Cpu0PrivateTimer:1;
    t_bitfield Cpu1PrivateTimer:1;
    t_bitfield Cpu2PrivateTimer:1;
    t_bitfield Cpu3PrivateTimer:1;      
    t_bitfield Cpu0GlobalTimer:1;
    t_bitfield Cpu1GlobalTimer:1;
    t_bitfield Cpu2GlobalTimer:1;
    t_bitfield Cpu3GlobalTimer:1;              
    t_bitfield unused:20;   
} t_scu_sec_access_ctrl_reg;

// SCU Registers
typedef struct {
    //  Type                    Name                Offset Description
    t_scu_control_reg           Control;            // 0x000  Controle Register
    t_scu_config_reg            Config;             // 0x004  Configuration Register   
    t_scu_power_status_reg      CpuPowerStatus;     // 0x008  Cpu power status Register
    t_scu_inval_non_sec_reg     Invalidate;         // 0x00C  invalidate all in non secure state Registe
    t_uint32                    reserved[(0x40-0x10)>>2];
    t_scu_filt_start_addr_reg   FilterStartAddr;    // 0x040  filtering start Register
    t_scu_filt_end_addr_reg     FilterEndAddr;      // 0x044  filtering end Register
    t_uint32                    reserved_1[(0x50-0x48)>>2];
    t_scu_access_ctrl_reg       AccessCtrl;         // 0x050  access control Register
    t_scu_sec_access_ctrl_reg   SecAccessCtrl;      // 0x054  secure access control Register    
} t_scu_registers;

/*------------------------------------------------------------------------
 * STM system trace module
 *----------------------------------------------------------------------*/


// STM control Register
typedef struct
{
    t_bitfield PrioPort0:1;
    t_bitfield PrioPort1:1;
    t_bitfield PrioPort2:1;        
    t_bitfield TSnTS:1; 
    t_bitfield DwnG:2;
    t_bitfield XCkDiv:3;
    t_bitfield SwapPRCMU:1;
    const t_bitfield64 unused:54;    
} t_stm_control_reg;


// STM MIPI modes control Register
typedef struct
{
    t_bitfield HWnSW0:1;
    t_bitfield HWnSW1:1;
    t_bitfield HWnSW2:1;
    t_bitfield HWnSW3:1;
    t_bitfield HWnSW4:1;
    t_bitfield HWnSW5:1;        
    const t_bitfield64 unused:58; 
} t_stm_mipi_control_reg;


// STM trace enable Register
typedef struct
{
    t_bitfield TEn0:1;
    const t_bitfield unused:1;
    t_bitfield TEn2:1;
    t_bitfield TEn3:1;
    t_bitfield TEn4:1;
    t_bitfield TEn5:1;
    const t_bitfield unused2:3;         
    t_bitfield TEn9:1;                
    const t_bitfield64 unused3:54; 
} t_stm_trace_enable_reg;


// STM trace disable status Register
typedef struct
{
    t_bitfield TDis0:1;
    t_bitfield TDis1:1;
    t_bitfield TDis2:1;
    t_bitfield TDis3:1;
    t_bitfield TDis4:1;
    t_bitfield TDis5:1;        
    const t_bitfield unused:3;                 
    t_bitfield TDis9:1;        
    const t_bitfield64 unused2:54; 
} t_stm_trace_dis_status_reg;


// STM overflow status Register
typedef struct
{
    t_bitfield OvF0:1;
    t_bitfield OvF1:1;
    t_bitfield OvF2:1;
    t_bitfield OvF3:1;
    t_bitfield OvF4:1;
    t_bitfield OvF5:1;
    const t_bitfield unused:3;        
    t_bitfield OvF9:1;        
    t_bitfield SecError:1;        
    t_bitfield DecError:1;                        
    t_bitfield AddrError:5;         
    const t_bitfield64 unused2:47; 
} t_stm_overflow_status_reg;


// STM transmit fifo Status Register
typedef struct
{
    t_bitfield RAME:1;
    t_bitfield RAMF:1;
    t_bitfield PRCMUE:1;        
    t_bitfield PRCMUF:1;        
    t_bitfield ChgConfig:1;                        
    const t_bitfield64 unused:59; 
} t_stm_trans_fifo_status_reg;


// STM sbag status Register
typedef struct
{
    t_bitfield SBFE:1;
    t_bitfield SBFF:1;
    t_bitfield SBOv:1;
    t_bitfield SBOvNb:5;
    const t_bitfield unused:6;                  
    const t_bitfield64 unused2:50; 
} t_stm_sbag_status_reg;


// STM Registers
typedef struct {
//  Type                        Name               Offset Description
    t_stm_control_reg           Control;        // 0x000  control Register
    t_stm_mipi_control_reg      MIPIControl;    // 0x008  MIPI modes control Register
    t_stm_trace_enable_reg      TraceEnable;    // 0x010  trace enable Register 
    t_stm_trace_dis_status_reg  TraceDisStatus; // 0x018  trace disable status Register
    t_stm_overflow_status_reg   OverflowStatus; // 0x020  overflow status Register
    t_stm_trans_fifo_status_reg TransFIFOStatus;// 0x028  transmit fifo Status Register
    t_stm_sbag_status_reg       SbagStatus;     // 0x030  sbag status Register
    const t_uint32              reserved_2[(0xFC0-0x038)>>2];
    const t_uint32              PeriphID[4];    // 0xFC0... : Peripheral id registers
    const t_uint32              PCellID[4];     // 0xFE0... : PrimeCell  id registers   
} t_stm_registers;


/*------------------------------------------------------------------------
 * SBAG ST bus analyser generic
 *----------------------------------------------------------------------*/
// SBAG enable disable Register
typedef struct
{
    t_bitfield64 Enable:1;
    t_bitfield64 unused:63; 
} t_sbag_enable_reg;


// SBAG Define modes Register 
typedef struct
{
    t_bitfield64 Mode:3;
    t_bitfield64 unused:61; 
} t_sbag_mode_reg;


// SBAG Define pace for output data Register
typedef struct
{
    t_bitfield64 PortPace:16;
    t_bitfield64 unused:48; 
} t_sbag_msg_port_pace_reg;


// SBAG FIFO status Register
typedef struct
{
    t_bitfield64 FFFlags:2;
    t_bitfield64 unused:62; 
} t_sbag_status_reg;

// SBAG absolute time register
typedef struct
{
    t_bitfield64 time:48;
    t_bitfield64 unused:16; 
} t_sbag_abs_time_reg;

// SBAG watchpoint control register
typedef struct
{
    t_bitfield64 responseSourceMask:8;
    t_bitfield64 responseSourceValue:8;
    t_bitfield64 sourceMask:8;
    t_bitfield64 sourceValue:8;
    t_bitfield64 opcodeMask:8;
    t_bitfield64 opcodeValue:8;
    t_bitfield64 outOfRange:1;
    t_bitfield64 unused:15;
} t_sbag_wp_ctrl_reg;

// SBAG watchpoint extended control register
typedef struct
{
    t_bitfield64 bufferStatusControl:1;
    t_bitfield64 unused:63;
} t_sbag_wp_ex_ctrl_reg;

// SBAG watchpoint extended control register
typedef struct
{
    t_bitfield64 unused0:2;
    t_bitfield64 startAddr:22;
    t_bitfield64 unused1:8;
    t_bitfield64 unused2:32;
} t_sbag_wp_match_addr_reg;

// SBAG watchpoint status register
typedef struct
{
    t_bitfield64 packetSource:8;
    t_bitfield64 tid:8;
    t_bitfield64 opcode:8;
    t_bitfield64 unused:8;
    t_bitfield64 addr:32;
} t_sbag_wp_status_reg;

// SBAG watchpoint enable register
typedef struct
{
    t_bitfield64 enable:1;
    t_bitfield64 unused:63;
} t_sbag_wp_enable_reg;

// SBAG watchpoint registers
typedef struct
{
    t_sbag_wp_ctrl_reg control;
    t_sbag_wp_ex_ctrl_reg extendedControl;
    t_sbag_wp_match_addr_reg addrStart;
    t_sbag_wp_match_addr_reg addrEnd;
    t_sbag_wp_status_reg status;
    t_sbag_wp_enable_reg enable;
    const t_uint32 reserved[(0x480-0x430)>>2];
} t_sbag_watch_point;

// SBAG ref message rate register
typedef struct
{
    t_bitfield64 rate:32;
    t_bitfield64 unused:32;
} t_sbag_ref_rate_reg;

// SBAG fast printf message register
typedef struct
{
    t_bitfield64 payload:32;
    t_bitfield64 unused:32;
} t_sbag_fpf_msg_reg;

// SBAG fast printf trace control register
typedef struct
{
    t_bitfield64 data:1;
    t_bitfield64 unused0:47;
    t_bitfield64 time:1;
    t_bitfield64 unused1:14;
    t_bitfield64 enable:1;
} t_sbag_fpf_trace_ctrl_reg;

// SBAG pi trace control register
typedef struct
{
    t_bitfield64 data:16;
    t_bitfield64 unused0:32;
    t_bitfield64 time:1;
    t_bitfield64 itEn:1;
    t_bitfield64 excIt:1;
    t_bitfield64 itf:1;
    t_bitfield64 unused1:11;
    t_bitfield64 enable:1;
} t_sbag_pi_trace_ctrl_reg;

// SBAG watch point trace control register
typedef struct
{
    t_bitfield64 opc:1;
    t_bitfield64 src:1;
    t_bitfield64 tid:1;
    t_bitfield64 be:1;
    t_bitfield64 unused0:4;
    t_bitfield64 add:1;
    t_bitfield64 unused1:7;
    t_bitfield64 data:1;
    t_bitfield64 unused2:7;
    t_bitfield64 dwidth:2;
    t_bitfield64 unused3:22;
    t_bitfield64 time:1;
    t_bitfield64 unused4:14;
    t_bitfield64 enable:1;
} t_sbag_wp_trace_ctrl_reg;

// SBAG error trace control register
typedef struct
{
    t_bitfield64 flags0:1;
    t_bitfield64 unused0:7;
    t_bitfield64 flags1:1;
    t_bitfield64 flags2:1;
    t_bitfield64 unused1:6;
    t_bitfield64 flags3:1;
    t_bitfield64 unused2:46;
    t_bitfield64 enable:1;
} t_sbag_error_trace_ctrl_reg;

// SBAG tm trace enable register
typedef struct
{
    t_bitfield64 enablePort:13;
    t_bitfield64 unused:51;
} t_sbag_tm_trace_enable_reg;

// SBAG tm trace control register
typedef struct
{
    t_bitfield64 lmetric1:1;
    t_bitfield64 lmetric2:1;
    t_bitfield64 lmetric3:1;
    t_bitfield64 unused0:5;
    t_bitfield64 lerr:1;
    t_bitfield64 unused1:55;
} t_sbag_tm_trace_ctrl_reg;

// SBAG ref trace control register
typedef struct
{
    t_bitfield64 unused:63;
    t_bitfield64 enable:1;
} t_sbag_ref_trace_ctrl_reg;

// SBAG port filter register
typedef struct
{
    t_bitfield64 sourceValue:8;
    t_bitfield64 opcodeValue:8;
    t_bitfield64 sourceMask:8;
    t_bitfield64 opcodeMask:8;
    t_bitfield64 unused0:24;
    t_bitfield64 sourceEnable:1;
    t_bitfield64 opcodeEnable:1;
    t_bitfield64 unused1:6;
} t_sbag_port_filter_reg;

// SBAG port metric1 register
typedef struct
{
    t_bitfield64 metric:16;
    t_bitfield64 unused:48;
} t_sbag_port_metric1_reg;

// SBAG port metric2 register
typedef struct
{
    t_bitfield64 metric:24;
    t_bitfield64 unused:40;
} t_sbag_port_metric2_reg;

// SBAG port metric3 register
typedef struct
{
    t_bitfield64 metric:16;
    t_bitfield64 unused:48;
} t_sbag_port_metric3_reg;

// SBAG pi edge ctrl register
typedef struct
{
    t_bitfield64 enable:16;
    t_bitfield64 unused:48;
} t_sbag_pi_edge_reg;

// SBAG Registers
typedef struct {
//  Type                        Name               Offset Description
    t_sbag_enable_reg           Enable;         // 0x000  enable disable Register 
    t_sbag_mode_reg             Mode;           // 0x008  Define modes Register 
    t_sbag_msg_port_pace_reg    MsgPortPace;    // 0x010  Define pace for output data Register
    t_sbag_status_reg           Status;         // 0x018  FIFO status Register
    const t_uint32              reserved_0[(0x200-0x020)>>2];
    t_sbag_abs_time_reg         AbsoluteTime;   // 0x200  Absolute time counter
    const t_uint32              reserved_1[(0x400-0x208)>>2];
    t_sbag_watch_point          WatchPoint[2];  // 0x400
    const t_uint32              reserved_2[(0x600-0x500)>>2];
    t_sbag_ref_rate_reg         RefRate;        // 0x600
    t_sbag_fpf_msg_reg          FpfMsg;         // 0x608
    t_sbag_fpf_trace_ctrl_reg   FpfCtrl;        // 0x610
    t_sbag_pi_trace_ctrl_reg    PiCtrl;         // 0x618
    t_sbag_wp_trace_ctrl_reg    WpCtrl[2];      // 0x620
    t_sbag_error_trace_ctrl_reg ErrorCtrl;      // 0x630
    t_sbag_tm_trace_enable_reg  TmTraceEnable;  // 0x638
    t_sbag_tm_trace_ctrl_reg    TmTraceCtrl;    // 0x640
    const t_uint32              reserved_3[(0x660-0x648)>>2];
    t_sbag_ref_trace_ctrl_reg   RefCtrl;        // 0x660
    const t_uint32              reserved_4[(0x800-0x668)>>2];
    t_sbag_port_filter_reg      PortFilter[13]; // 0x800
    const t_uint32              reserved_5[(0x880-0x868)>>2];
    t_sbag_port_metric1_reg     PortMetric1[13];// 0x880
    const t_uint32              reserved_6[(0x900-0x8e8)>>2];
    t_sbag_port_metric2_reg     PortMetric2[13];// 0x900
    const t_uint32              reserved_7[(0x980-0x968)>>2];
    t_sbag_port_metric3_reg     PortMetric3[13];// 0x980
    const t_uint32              reserved_8[(0xa00-0x9e8)>>2];
    t_sbag_pi_edge_reg          Rising;         // 0xa00
    t_sbag_pi_edge_reg          Falling;        // 0xa08
} t_sbag_registers;

/*------------------------------------------------------------------------
 * Level 2 cache controller
 *----------------------------------------------------------------------*/
#define WAY_NB          8

// L2CC identification register 
typedef struct {                    
    t_bitfield Revision:6;
    t_bitfield PartNumber:4;
    t_bitfield CacheId:6;
    t_bitfield unused:8;
    t_bitfield Designer:8;
} t_l2cc_id_reg;

// L2CC cache type register 
typedef struct {                    
    t_bitfield unused_1:6;
    t_bitfield InstrAssoc:1;
    t_bitfield unused_2:1;
    t_bitfield InstrWaySize:3;
    t_bitfield unused_3:7;
    t_bitfield DataAssoc:1;
    t_bitfield unused_4:1;
    t_bitfield DataWay:3;
    t_bitfield unused_5:2;
    t_bitfield CType:4;
    t_bitfield unused_6:3;
} t_l2cc_type_reg;

// L2CC control register 
typedef struct {                    
    t_bitfield Enable:1;
    t_bitfield unused:31;
} t_l2cc_control_reg;

// L2CC auxiliairy control register 
typedef struct {
    t_bitfield unused_2:12;                
    t_bitfield Exclusive:1;
    t_bitfield unused_3:3;
    t_bitfield Associativity:1;
    t_bitfield WaySize:3;
    t_bitfield EventBusEnable:1;
    t_bitfield ParityEnable:1;
    t_bitfield SharedAttrOverEnable:1;
    t_bitfield ForceWriteAlloc:2;
    t_bitfield unused_4:1;
    t_bitfield NSLockEnable:1;
    t_bitfield NSIntCtrl:1;
    t_bitfield DPrefetchEnable:1;    
    t_bitfield IPrefetchEnable:1;  
    t_bitfield unused_5:2;
} t_l2cc_aux_control_reg;

// L2CC Tag and RAM latency control register 
typedef struct {
    t_bitfield RamSetupLatency:3;
    t_bitfield unused_1:1;
    t_bitfield RamReadLatency:3;
    t_bitfield unused_2:1;
    t_bitfield RamWriteLatency:3;
    t_bitfield unused_3:21;
} t_l2cc_latency_control_reg;

// L2CC Event Counter Control register
typedef struct {
    t_bitfield Enable:1;
    t_bitfield counter0Reset:1;
    t_bitfield counter1Reset:1;
    t_bitfield unused:29;
} t_l2cc_evt_cnt_control_reg;

// L2CC Event Counter Configuration register
typedef struct {
    t_bitfield ItConfiguration:2;
    t_bitfield Source:4;
    t_bitfield unused:26;
} t_l2cc_evt_cnt_conf_reg;

// L2CC interrupt register
typedef struct {
    t_bitfield EventCounterOverflow:1;
    t_bitfield L2TagRamParityError:1;
    t_bitfield L2DataRamParityError:1;
    t_bitfield L2TagRamWrError:1;
    t_bitfield L2DataRamWrError:1;
    t_bitfield L2TagRamRdError:1;
    t_bitfield L2DataRamRdError:1;
    t_bitfield SLVERR:1;
    t_bitfield DECERR:1;
    t_bitfield unused:23;
} t_l2cc_it_reg;

// L2CC choice by way
typedef struct {                    
    t_bitfield Ways:8;
    t_bitfield unused:24;
} t_l2cc_by_way_reg;

// L2CC address filtering start register
typedef struct {
    t_bitfield Enable:1;
    t_bitfield unused:19;
    t_bitfield Address:12;
} t_l2cc_filter_start_reg;

// L2CC address filtering start register
typedef struct {
    t_bitfield unused:20;
    t_bitfield Address:12;
} t_l2cc_filter_end_reg;

// L2CC debug control
typedef struct {                    
    t_bitfield Dcl:1;
    t_bitfield Dwb:1;
    t_bitfield SPNIDEN:1;
    t_bitfield unused:29;
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

/*------------------------------------------------------------------------
 * I2C high-speed controller (I2CHS)
 *----------------------------------------------------------------------*/
 
//  I2C control register
typedef struct
{
    t_bitfield PeriphEnable:1;     // Peripheral enabled/disabled
    t_bitfield OperatingMode:2;    // Operating mode
    t_bitfield SlaveAddrMode:1;    // Slave addressing mode
    t_bitfield SpeedMode:2;        // Speed mode
    t_bitfield SlaveGenCallMode:1; // Slave general call mode
    t_bitfield FlushTx:1;          // Flush transmit
    t_bitfield FlushRx:1;          // Flush receive
    t_bitfield DmaTxEnable1;       // DMA Tx enable
    t_bitfield DmaRxEnable:1;      // DMA Rx enable
    t_bitfield DmaSLE:1;           // DMA synchronization logic enable
    t_bitfield LoopbackMode:1;     // Loopback mode
    t_bitfield FilteringOn:2;      // Filtering On
    t_bitfield ForceStopEnable:1;  // Force stop enable bit
    const t_bitfield Reserved:16;  // Reserved
} t_i2c_control_reg;

//  I2C slave control register
typedef struct
{
    t_bitfield SlaveAddr7:7;          // Slave address 7-bit
    t_bitfield ExtSlaveAddr10:3;      // Extended slave address 10-bit
    const t_bitfield Reserved:6;      // Reserved
    t_bitfield SlaveDataSetupTime:16; // Slave data setup time
} t_i2c_slave_control_reg;

//  I2C high-speed master code register
typedef struct
{
    t_bitfield MasterCode:3;      // Master code
    const t_bitfield Reserved:29; // Reserved
} t_i2c_hs_master_code_reg;

//  I2C master control register
typedef struct
{
    t_bitfield Operation:1;      // Operation
    t_bitfield Addr:7;           // Address
    t_bitfield ExtAddr:3;        // Extended address
    t_bitfield StartByte:1;      // Start byte procedure        
    t_bitfield AddrType:2;       // Address type
    t_bitfield P:1;              // Stop condition (defined as P in I2C norm)    
    t_bitfield Length:11;        // Transaction length
    const t_bitfield Reserved:6; // Reserved        
} t_i2c_master_control_reg;

//  I2C transmit FIFO register
typedef union
{
	struct
	{
	    t_bitfield TxData:8;          // Transmission data
	    const t_bitfield Reserved:24; // Reserved    
	} Bit;
	t_uint32 Reg;
} t_i2c_tx_fifo_reg;

//  I2C status register
typedef struct
{
    t_bitfield Operation:2;       // Operation
    t_bitfield Status:2;          // Controler status
    t_bitfield Cause:3;           // Abort cause
    t_bitfield Type:2;            // Receive type
    t_bitfield Length:11;         // Transfer length                
    const t_bitfield Reserved:12; // Reserved    
} t_i2c_status_reg;

//  I2C receive FIFO register
typedef struct
{
    t_bitfield RxData:8;          // Receive data
    const t_bitfield Reserved:24; // Reserved    
} t_i2c_rx_fifo_reg;

//  I2C transmit FIFO threshold register
typedef struct
{
    t_bitfield ThresholdTx:4;     // Threshold Tx
    const t_bitfield Reserved:28; // Reserved    
} t_i2c_tx_threshold_reg;

//  I2C receive FIFO threshold register
typedef struct
{
    t_bitfield ThresholdRx:4;     // Threshold Rx
    const t_bitfield Reserved:28; // Reserved    
} t_i2c_rx_threshold_reg;

//  I2C DMA register
typedef struct
{
    t_bitfield SrcBurstSize:3;      // Source burst size Rx
    t_bitfield BurstRx:1;           // Type of DMA request Rx
    const t_bitfield Reserved_1:4;  //  Type of DMA request    
    t_bitfield DestBurstSize:3;     // Destination burst size Tx
    t_bitfield BurstTx:1;           // Type of DMA request Tx 	        
    const t_bitfield Reserved_2:4;  // Reserved    
    const t_bitfield Reserved_3:16; // Reserved
} t_i2c_dma_reg;

//  I2C baud rate counter register
typedef struct
{
    t_bitfield BaudRateCount1:16; // Baud rate counter 1 
    t_bitfield BaudRateCount2:16; // Baud rate counter 2    
} t_i2c_br_counter_reg;

//  I2C interrupt mask set/clear register
typedef union 
{
    struct
    {
        t_bitfield TxFifoEmptyMask:1;           // Tx FIFO empty mask
        t_bitfield TxFifoNearlyEmptyMask:1;     // Tx FIFO neraly empty mask
        t_bitfield TxFifoFullMask:1;            // Tx FIFO full mask    
        t_bitfield TxFifoOverunMask:1;          // Tx FIFO overun mask
        t_bitfield RxFifoEmptyMask:1;           // Rx FIFO empty mask
        t_bitfield RxFifoNearlyFullMask:1;      // Rx FIFO nearly full mask
        t_bitfield RxFifoFullMask:1;            // Rx FIFO full mask
        const t_bitfield Reserved_1:9;          // Reserved    
        t_bitfield ReadFromSlaveReqMask:1;      // Read-from-Slave request mask	           
        t_bitfield ReadFromSlaveEmptyMask:1;    // Read-from-Slave empty mask
        t_bitfield WriteToSlaveReqMask:1;       // Write-to-Slave request mask
        t_bitfield MasterTransacDoneMask:1;     // Master transaction done
        t_bitfield SlaveTransacDoneMask:1;      // Slave transaction done
        const t_bitfield Reserved_2:3;          // Reserved                        
        t_bitfield MasterArbitrationLostMask:1; // Master arbitration lost
        t_bitfield BusErrMask:1;                // Bus error mask
        const t_bitfield Reserved_3:2;          // Reserved
        t_bitfield MasterTransWithoutStop:1;    // Master Transaction Done Without Stop Mask.
        const t_bitfield Reserved_4:3;          // Reserved
    } Bit;
    t_uint32 Reg;                    
} t_i2c_it_mask_reg;

//  I2C raw interrupt status register
typedef struct
{
    t_bitfield TxFifoEmpty:1;           // Tx FIFO empty mask
    t_bitfield TxFifoNearlyEmpty:1;     // Tx FIFO neraly empty mask
    t_bitfield TxFifoFull:1;            // Tx FIFO full mask    
    t_bitfield TxFifoOverun:1;          // Tx FIFO overun mask
    t_bitfield RxFifoEmpty:1;           // Rx FIFO empty mask
    t_bitfield RxFifoNearlyFull:1;      // Rx FIFO nearly full mask
    t_bitfield RxFifoFull:1;            // Rx FIFO full mask
    const t_bitfield Reserved_1:9;      // Reserved    
    t_bitfield ReadFromSlaveReq:1;      // Read-from-Slave request mask	           
    t_bitfield ReadFromSlaveEmpty:1;    // Read-from-Slave empty mask
    t_bitfield WriteToSlaveReq:1;       // Write-to-Slave request mask
    t_bitfield MasterTransacDone:1;     // Master transaction done
    t_bitfield SlaveTransacDone:1;      // Slave transaction done
    const t_bitfield Reserved_2:3;      // Reserved  
    t_bitfield MasterArbitrationLost:1; // Master arbitration lost
    t_bitfield BusErr:1;                // Bus error mask
    const t_bitfield Reserved_3:2;      // Reserved
    t_bitfield MasterTransWithoutStop:1;// Master Transaction Done Without Stop Mask.
    const t_bitfield Reserved_4:3;      // Reserved
} t_i2c_it_status_reg;

//  I2C masked interrupt status register
typedef union
{
    struct
    {
        t_bitfield TxFifoEmptyMaskStatus:1;           // Tx FIFO empty mask status
        t_bitfield TxFifoNearlyEmptyStatus:1;         // Tx FIFO neraly empty mask
        t_bitfield TxFifoFullMaskStatus:1;            // Tx FIFO full mask    
        t_bitfield TxFifoOverunMaskStatus:1;          // Tx FIFO overun mask
        t_bitfield RxFifoEmptyMaskStatus:1;           // Rx FIFO empty mask
        t_bitfield RxFifoNearlyFullMaskStatus:1;      // Rx FIFO nearly full mask
        t_bitfield RxFifoFullMaskStatus:1;            // Rx FIFO full mask
        const t_bitfield Reserved_1:9;                // Reserved    
        t_bitfield ReadFromSlaveReqMaskStatus:1;      // Read-from-Slave request mask	           
        t_bitfield ReadFromSlaveEmptyMaskStatus:1;    // Read-from-Slave empty mask
        t_bitfield WriteToSlaveReqMaskStatus:1;       // Write-to-Slave request mask
        t_bitfield MasterTransacDoneMask:1;           // Master transaction done
        t_bitfield SlaveTransacDoneMaskStatus:1;      // Slave transaction done
        const t_bitfield Reserved_2:3;                // Reserved                
        t_bitfield MasterArbitrationLostMaskStatus:1; // Master arbitration lost
        t_bitfield BusErrMaskStatus:1;                // Bus error mask
        const t_bitfield Reserved_3:2;                // Reserved
        t_bitfield MasterTransWithoutStop:1;          // Master Transaction Done Without Stop Mask.
        const t_bitfield Reserved_4:3;                // Reserved
    } Bit;
    t_uint32 Reg;    
} t_i2c_it_masked_status_reg;

//  I2C interrupt clear register
typedef union
{ 
    struct
    {
        const t_bitfield Reserved_1:3;               // Reserved	
        t_bitfield TxFifoOverunClear:1;              // Clear Tx FIFO overun
        const t_bitfield Reserved_2:12;              // Reserved    
        t_bitfield ReadFromSlaveReqClear:1;          // Clear Read-from-Slave	           
        t_bitfield ReadFromSlaveEmptyClear:1;        // Clear Read-from-Slave
        t_bitfield WriteToSlaveReqMaskClear:1;       // Clear Write-to-Slave request
        t_bitfield MasterTransacDoneClear:1;         // clear Master transaction                
        t_bitfield SlaveTransacDoneMaskClear:1;      // Clear Slave transaction
        const t_bitfield Reserved_3:3;               // Reserved    
        t_bitfield MasterArbitrationLostMaskClear:1; // Clear Master arbitration lost
        t_bitfield BusErrMaskClear:1;                // Clear Bus error
        const t_bitfield Reserved_4:2;               // Reserved
        t_bitfield MasterTransWithoutStop:1;         // Master Transaction Done Without Stop Mask.
        const t_bitfield Reserved_5:3;               // Reserved
    } Bit;
        
    t_uint32 Reg;    
} t_i2c_it_clear_reg;


//  I2C integration test control register
typedef struct
{
    t_bitfield TestEn:1;                // Integration test enable
    t_bitfield FifoTest:1;              // FIFO test
    const t_bitfield Reserved:30;       // Reserved         
} t_i2c_test_ctrl_reg;

//  I2C integration test input register
typedef struct
{
    t_bitfield SdaIn:1;                 // I2C data input value
    t_bitfield SclIn:1;                 // Clock input value
    const t_bitfield Reserved_0:2;      // Reserved 
    t_bitfield DmaClrRx:1;              // receive DMA clear input value
    t_bitfield DmaTCRx:1;               // receive DMA terminal count input value
    t_bitfield DmaClrTx:1;              // transmit DMA clear input value
    t_bitfield DmaTCTx:1;               // transmit DMA terminal count input value
    const t_bitfield Reserved_1:7;      // Reserved 
    t_bitfield TestClock:1;             // test edge clock input value
    t_bitfield HwSAddr:10;              // HW slave addr input value
    const t_bitfield Reserved_2:6;      // Reserved           
} t_i2c_test_input_reg;

//  I2C integration test output register
typedef struct
{
    t_bitfield SdaOut:1;                // I2C data output value
    t_bitfield SclOut:1;                // Clock output value
    t_bitfield EnCSOut:1;               // Enable Current source output value
    const t_bitfield Reserved_0:1;      // Reserved 
    t_bitfield DmaBurstRx:1;            // receive burst resquest output value
    t_bitfield DmaSingRx:1;             // receive single resquest output value
    t_bitfield DmaBurstTx:1;            // transmit burst resquest output value
    const t_bitfield Reserved_1:8;      // Reserved 
    t_bitfield IntGbl:1;                // global interrupt output value
    const t_bitfield Reserved_2:16;     // Reserved         
} t_i2c_test_output_reg;

//  I2C test data register
typedef struct
{
    t_bitfield TestData:32;           
} t_i2c_test_data_reg;

//  I2C peripheral indentification register 0
typedef struct
{
    t_bitfield PartNum0:8;        // Part number 0
    const t_bitfield Reserved:24; // Reserved    
} t_i2c_periph_id0_reg;

//  I2C peripheral indentification register 1
typedef struct
{
    t_bitfield PartNum1:4;        // Part number 1
    t_bitfield Designer0:4;       // Designer 0    
    const t_bitfield Reserved:24; // Reserved    
} t_i2c_periph_id1_reg;

//  I2C peripheral indentification register 2
typedef struct
{
    t_bitfield Designer1:4;       // Desiger 1
    t_bitfield Revision:4;        // Revision    
    const t_bitfield Reserved:24; // Reserved    
} t_i2c_periph_id2_reg;

//  I2C peripheral indentification register 3
typedef struct
{
    t_bitfield Configuration:8;   // Configuarion
    const t_bitfield Reserved:24; // Reserved    
} t_i2c_periph_id3_reg;

//  I2C PCell indentification register 0
typedef struct
{
    t_bitfield I2cPCellId0:8;     // I2C PCell indentification register 0
    const t_bitfield Reserved:24; // Reserved    
} t_i2c_pcell_id0_reg;

//  I2C PCell indentification register 1
typedef struct
{
    t_bitfield I2cPCellId1:8;     // I2C PCell indentification register 1
    const t_bitfield Reserved:24; // Reserved    
} t_i2c_pcell_id1_reg;

//  I2C PCell indentification register 2
typedef struct
{
    t_bitfield I2cPCellId2:8;     // I2C PCell indentification register 2
    const t_bitfield Reserved:24; // Reserved    
} t_i2c_pcell_id2_reg;

//  I2C PCell indentification register 3
typedef struct
{
    t_bitfield I2cPCellId3:8;     // I2C PCell indentification register 3
    const t_bitfield Reserved:24; // Reserved    
} t_i2c_pcell_id3_reg;

// I2C Registers
typedef struct 
{
//  Type                        Name               Offset Description
    t_i2c_control_reg           Control;                // 0x00
    t_i2c_slave_control_reg     SlaveControl;           // 0x04
    t_i2c_hs_master_code_reg    HsMasterCode;           // 0x08        
    t_i2c_master_control_reg    MasterControl;          // 0x0C      
    t_i2c_tx_fifo_reg           TransmitFifo;           // 0x10    
    t_i2c_status_reg            Status;                 // 0x14   
    t_i2c_rx_fifo_reg           ReceiveFifo;            // 0x18
    t_i2c_tx_threshold_reg      TransmitFifoThreshold;  // 0x1C
    t_i2c_rx_threshold_reg      ReceiveFifoThreshold;   // 0x20  
    t_i2c_dma_reg               Dma;                    // 0x24  
    t_i2c_br_counter_reg        BaudRateCounter;        // 0x28
    t_i2c_it_mask_reg           ItMaskSetClear;         // 0x2C
    t_i2c_it_status_reg         RawItStatus;            // 0x30
    t_i2c_it_masked_status_reg  MaskedItStatus;         // 0x34
    t_i2c_it_clear_reg          ItClear;                // 0x38
    t_i2c_test_ctrl_reg         TestCtrl;               // 0x3C
    t_i2c_test_input_reg        TestInput;              // 0x40
    t_i2c_test_output_reg       TestOutput;             // 0x44
    t_i2c_test_data_reg         TestData;               // 0x48
    const t_uint32              reserved[(0xFE0-0x04C)>>2];
    t_i2c_periph_id0_reg        PeriphId0;              // 0xFE0    
    t_i2c_periph_id1_reg        PeriphId1;              // 0xFE4
    t_i2c_periph_id2_reg        PeriphId2;              // 0xFE8
    t_i2c_periph_id3_reg        PeriphId3;              // 0xFEC
    t_i2c_pcell_id0_reg         PCellId0;               // 0xFF0                
    t_i2c_pcell_id1_reg         PCellId1;               // 0xFF4    
    t_i2c_pcell_id2_reg         PCellId2;               // 0xFF8
    t_i2c_pcell_id3_reg         PCellId3;               // 0xFFC                                        
} t_i2c_registers;


/*------------------------------------------------------------------------
 * Dynamic Memory Controller (DMC)
 *----------------------------------------------------------------------*/
 
// DMC control register
typedef struct
{
    t_bitfield ActiveAging:1;       // Enable command aging in the command queue
    const t_bitfield Reserved0:7;   // Reserved  
    t_bitfield AddrCmpEn:1;         // enable address collision detection for cmd queue placement logic
    const t_bitfield Reserved1:7;   // Reserved     
    t_bitfield AutoPrecharge:1;     // enable auto precharge mode of controller
    const t_bitfield Reserved2:7;   // Reserved  
    t_bitfield Arefresh:1;          // at boundary specified by AUTO_REFRESH_ MODE
    const t_bitfield Reserved3:7;   // Reserved       
} t_dmc_ctl_00_reg;


// DMC Registers
typedef struct
{
//  Type                        Name                Offset Description
    t_dmc_ctl_00_reg            Ctl_00;             // 0x00
    t_uint32                    Ctl_01;             // 0x04
    t_uint32                    Ctl_02;             // 0x08
    t_uint32                    Ctl_03;             // 0x0C
    t_uint32                    Ctl_04;             // 0x10
    t_uint32                    Ctl_05;             // 0x14
    t_uint32                    Ctl_06;             // 0x18
    t_uint32                    Ctl_07;             // 0x1C
    t_uint32                    Ctl_08;             // 0x20
    t_uint32                    Ctl_09;             // 0x24    
    t_uint32                    Ctl_10;             // 0x28
    t_uint32                    Ctl_11;             // 0x2C    
    t_uint32                    Ctl_12;             // 0x30
    t_uint32                    Ctl_13;             // 0x34    
    t_uint32                    Ctl_14;             // 0x38
    t_uint32                    Ctl_15;             // 0x3C   
    t_uint32                    Ctl_16;             // 0x40
    t_uint32                    Ctl_17;             // 0x44    
    t_uint32                    Ctl_18;             // 0x48
    t_uint32                    Ctl_19;             // 0x4C 
    t_uint32                    Ctl_20;             // 0x50
    t_uint32                    Ctl_21;             // 0x54    
    t_uint32                    Ctl_22;             // 0x58
    t_uint32                    Ctl_23;             // 0x5C 
    t_uint32                    Ctl_24;             // 0x60
    t_uint32                    Ctl_25;             // 0x64    
    t_uint32                    Ctl_26;             // 0x68
    t_uint32                    Ctl_27;             // 0x6C 
    t_uint32                    Ctl_28;             // 0x70
    t_uint32                    Ctl_29;             // 0x74    
    t_uint32                    Ctl_30;             // 0x78
    t_uint32                    Ctl_31;             // 0x7C 
    t_uint32                    Ctl_32;             // 0x80
    t_uint32                    Ctl_33;             // 0x84    
    t_uint32                    Ctl_34;             // 0x88
    t_uint32                    Ctl_35;             // 0x8C 
    t_uint32                    Ctl_36;             // 0x90
    t_uint32                    Ctl_37;             // 0x94    
    t_uint32                    Ctl_38;             // 0x98
    t_uint32                    Ctl_39;             // 0x9C 
    t_uint32                    Ctl_40;             // 0xA0
    t_uint32                    Ctl_41;             // 0xA4    
    t_uint32                    Ctl_42;             // 0xA8
    t_uint32                    Ctl_43;             // 0xAC 
    t_uint32                    Ctl_44;             // 0xB0
    t_uint32                    Ctl_45;             // 0xB4    
    t_uint32                    Ctl_46;             // 0xB8
    t_uint32                    Ctl_47;             // 0xBC     
    t_uint32                    Ctl_48;             // 0xC0
    t_uint32                    Ctl_49;             // 0xC4    
    t_uint32                    Ctl_50;             // 0xC8
    t_uint32                    Ctl_51;             // 0xCC     
    t_uint32                    Ctl_52;             // 0xD0
    t_uint32                    Ctl_53;             // 0xD4    
    t_uint32                    Ctl_54;             // 0xD8
    t_uint32                    Ctl_55;             // 0xDC     
    t_uint32                    Ctl_56;             // 0xE0
    t_uint32                    Ctl_57;             // 0xE4    
    t_uint32                    Ctl_58;             // 0xE8
    t_uint32                    Ctl_59;             // 0xEC      
    t_uint32                    Ctl_60;             // 0xF0
    t_uint32                    Ctl_61;             // 0xF4    
    t_uint32                    Ctl_62;             // 0xF8
    t_uint32                    Ctl_63;             // 0xFC    

} t_dmc_registers;

/*------------------------------------------------------------------------
 * Configuration registers (CR)
 *----------------------------------------------------------------------*/
 
// Configuration register 0
typedef struct
{
    t_bitfield VpipPwm8Mux:3;       // VPIP PWM output 8 pin mux
    t_bitfield Msp0Wr:1;            // MSP0 wired as MSP1 and MSP2
    t_bitfield Hc0Dma:1;            // DMA log channel 62 configuration
    t_bitfield Hc1Dma:1;            // DMA log channel 49 configuration        
    t_bitfield TrigIo:1;            // DDR trig IO
    t_bitfield ModemIt:1;           // ModemSS IT mask
    t_bitfield ModemDataSize:5;     // Data size of the ModemSS ETM
    t_bitfield ModemPctl:1;         // Modem trace Ctrl pin
    t_bitfield Reserved:2;          // Reserved
    t_bitfield SxAGpio:1;           // SVA SGA GPIO selection
    t_bitfield Reserved_1:1;        // Reserved        
    t_bitfield SbagData:4;          // SBAG data
    t_bitfield SbagTrig:4;          // SBAG trigger
    t_bitfield HTimEn:1;            // Imaging, Audio, video accelerator timer enable
    t_bitfield Reserved_2:1;        // Reserved
    t_bitfield ModemHostAddrOff:4;  // ModemSS Host Port Address Offset
} t_cr_0_reg;
 
typedef struct
{
//  Type                        Name                Offset Description    
    t_cr_0_reg                  CrReg0;             // 0x00
    t_uint32                    CrReg1;             // 0x04
    t_uint32                    CrReg2;             // 0x08
    t_uint32                    CrReg3;             // 0x0C
    t_uint32                    Reserved[0x14-0x10];
    t_uint32                    CrReg5;             // 0x14
    t_uint32                    Reserved_1[0x20-0x18];
    t_uint32                    CrXdbg;             // 0x20
    t_uint32                    Reserved_2[0x2C-0x24];    
    t_uint32                    CrUsb;              // 0x2C
    t_uint32                    CrRxDphy0;          // 0x30
    t_uint32                    CrRxDphy1;          // 0x34
    t_uint32                    CrRxDphy2;          // 0x38
    t_uint32                    CrRxDphy3;          // 0x3C
    t_uint32                    CrRxDphy4;          // 0x40
    t_uint32                    CrRxDphy5;          // 0x44
    t_uint32                    CrRxDphy6;          // 0x48
    t_uint32                    CrRxDphy7;          // 0x4C
    t_uint32                    CrRxDphy8;          // 0x50
    t_uint32                    CompRxDphy1;        // 0x54
    t_uint32                    CompRxDphy2;        // 0x58
    t_uint32                    SrRxDphy;           // 0x5C
} t_cr_registers;

/*----------------------------------------------------------------------*/

#endif // _NOMADIK_REGISTERS_H

// End of file nomadik_registers.h
