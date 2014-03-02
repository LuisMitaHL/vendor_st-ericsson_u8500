/*
* --------------------------------------------------------------------------------------------
* STEricsson
* Reproduction and Communication of this document is strictly prohibited
* unless specifically authorized in writing by STEricsson
* --------------------------------------------------------------------------------------------
* File Name: dma_p.h
* This file contains the private defintions.
* -------------------------------------------------------------------------------------------- */


#ifndef __DMA_PRIVATE_HEADER
#define __DMA_PRIVATE_HEADER

#include "dma.h"

/* DMA Peripheral IDs and Primecell ID */

#define DMA_PERIPH_ID_0 0x40
#define DMA_PERIPH_ID_1 0x00

#if defined(ST_8500ED) || defined(ST_HREFED) || defined(__PEPS_8500) 
#define DMA_PERIPH_ID_2 0x08

#elif  defined(__PEPS_8500_V2) || defined(ST_8500V2) || defined(ST_HREFV2)

#if defined(STE_AP9540) || defined(STE_AP8540)
#define DMA_PERIPH_ID_2 0x48
#else
#define DMA_PERIPH_ID_2 0x38
#endif

#else
#define DMA_PERIPH_ID_2 0x28

#endif

#define DMA_PERIPH_ID_3 0x00

#define DMA_P_CELL_ID_0 0x0D
#define DMA_P_CELL_ID_1 0xF0
#define DMA_P_CELL_ID_2 0x05
#define DMA_P_CELL_ID_3 0xB1

/* Channel Defines*/
#define DMA_MASTER_PORTS_MAX             2
#define DMA_PHY_CHANNEL_MAX              32
#define DMA_NO_OF_PHYSICAL_CHANNELS      8
#define DMA_NO_OF_LOGICAL_CHANNELS       32 /* no of logical channels on one physical resource */
#define DMA_STANDARD_CHANNELS_AVBL       (DMA_NO_OF_PHYSICAL_CHANNELS)
#define DMA_EXTENDED_CHANNELS_AVBL       (DMA_STANDARD_CHANNELS_AVBL / 2)
#define DMA_NO_OF_DEVICES                128
#define DMA_NO_OF_EVENT_GROUPS           4
#define DMA_PHY_CHAN_PER_EVT_GRP         2
#define DMA_DEVICES_PER_PHY_CHANNEL      (DMA_NO_OF_DEVICES / DMA_NO_OF_EVENT_GROUPS)
#define DMA_SRC_DEVICES_PER_PHY_CHAN     (DMA_DEVICES_PER_PHY_CHANNEL / 2)
#define DMA_DEST_DEVICES_PER_PHY_CHAN    (DMA_DEVICES_PER_PHY_CHANNEL / 2)
#define DMA_MEM_DEVICES_PER_PHY_CHAN     16
#define DMA_PIPES_PER_PHY_CHAN           (DMA_SRC_DEVICES_PER_PHY_CHAN + DMA_MEM_DEVICES_PER_PHY_CHAN)

/* Relink Memory Defines */
#define DMA_TOTAL_RELINK_MEMORY_SIZE     (514 * 1024)
#define DMA_PHYSICAL_RELINK_MEMORY_SIZE  (256 * 1024)
#define DMA_LOGICAL_RELINK_MEMORY_SIZE   (256 * 1024)
#define DMA_LOGICAL_PARAM_MEMORY_SIZE    (2 * 1024)
#define DMA_PHY_CHAN_SIZE_OF_LINK        (16)
#define DMA_ONE_EVT_GRP_MEM_SIZE         (DMA_PHYSICAL_RELINK_MEMORY_SIZE / DMA_NO_OF_EVENT_GROUPS) /*64K*/
#define DMA_ONE_PHY_CHAN_MEM_SIZE        (DMA_ONE_EVT_GRP_MEM_SIZE / DMA_PHY_CHAN_PER_EVT_GRP)      /*32K*/
#define DMA_ONE_HALF_CHAN_MEM_SIZE       (DMA_ONE_PHY_CHAN_MEM_SIZE / 2)                            /*16K*/
#define DMA_PHY_CHAN_DEST_MEM_OFFSET     (DMA_ONE_PHY_CHAN_MEM_SIZE / 2)                            /*16K*/
#define DMA_PHY_NO_OF_LINKS_MAX          (DMA_ONE_HALF_CHAN_MEM_SIZE / DMA_PHY_CHAN_SIZE_OF_LINK)
#define DMA_LOG_NO_OF_LINKS_MAX          (128)

#define DMA_ELEMENT_TRANSFER_MAX         ((1<<16)-1) /* 65535 */


/*******************************************************************************/
/*            Macros used for accessing DMA Hardware register bitfield         */
/*******************************************************************************/

/*---------------------------------- DMAC Controller Registers------------------------------ */

/*******************************************/
/*            Global Clock Enable            */
/*******************************************/

#define DMA_CLKEN_MASK            MASK_BIT0                                /* Global Clock Enable         */
#define DMA_CLKEN_SHIFT           0 

#define DMA_SE_0_SHIFT            8                                        /*Source Event Clock Enable    */
#define DMA_SE_1_SHIFT            10
#define DMA_SE_2_SHIFT            12
#define DMA_SE_3_SHIFT            14
#define DMA_SE_0_MASK             MASK_BIT8
#define DMA_SE_1_MASK             MASK_BIT10
#define DMA_SE_2_MASK             MASK_BIT12
#define DMA_SE_3_MASK             MASK_BIT14

#define DMA_DE_0_SHIFT            9                                        /*Destination Event Clock Enable */
#define DMA_DE_1_SHIFT            11
#define DMA_DE_2_SHIFT            13
#define DMA_DE_3_SHIFT            15
#define DMA_DE_0_MASK             MASK_BIT9
#define DMA_DE_1_MASK             MASK_BIT11
#define DMA_DE_2_MASK             MASK_BIT13
#define DMA_DE_3_MASK             MASK_BIT15

/*******************************************/
/*            Physical Resource Register   */
/*******************************************/

#define DMA_CP_TYP_MASK           0x3                                      /* Physical Resource Type               */
#define DMA_PR_SEC_MASK           0x3                                      /* Physical Resource Security Mode      */
#define DMA_PR_MOD_MASK           0x3                                      /* Physical Resource Mode               */
#define DMA_PR_OPT_MASK           0x3                                      /* Physical Resource Option             */
#define DMA_PR_AS_MASK            0x3                                      /* Physical Resource Activation status  */

/*******************************************/
/*             Secure Event Group Register */
/*******************************************/

#define DMA_EVT_SEL_MASK  MASK_BIT0                                        /* Event Line Selection*/


/*******************************************/
/*            Statistic Function Register    */
/*******************************************/

#define DMA_STAT_ENA_SHIFT       31                                        /* Enable */
#define DMA_STAT_ENA_MASK        MASK_BIT31 

#define DMA_STAT_MODE_SHIFT      28                                        /* Mode Select    */
#define DMA_STAT_MODE_MASK       0x70000000 

#define DMA_STAT_MSTSEL_SHIFT    26                                        /* Master Select */
#define DMA_STAT_MSTSEL_MASK     0x0C000000

#define DMA_STAT_PRSEL_SHIFT     21 
#define DMA_STAT_PRSEL_MASK      0x03E00000                                /* Physical Resource select */

#define DMA_STAT_SD_SHIFT        20
#define DMA_STAT_SD_MASK         MASK_BIT20                                /* Source or Destination Half channel */

#define DMA_STAT_LCELNB_SHIFT    16                                        /* Logical Channel Event Line number */
#define DMA_STAT_LCELNB_MASK     0x000F0000   

#define DMA_STAT_RESULT_MASK     0xFFFF                                    /* Statistic Result */

/*************************************************/
/*      Implementation Configuration Register    */
/*************************************************/

#define DMA_ICFG_MODEN1_MASK      MASK_BIT16                               /* Master Modulo Enable */
#define DMA_ICFG_MODEN2_MASK      MASK_BIT17
#define DMA_ICFG_MODEN3_MASK      MASK_BIT18
#define DMA_ICFG_MODEN4_MASK      MASK_BIT19
#define DMA_ICFG_MODEN1_SHIFT     16
#define DMA_ICFG_MODEN2_SHIFT     17
#define DMA_ICFG_MODEN3_SHIFT     18
#define DMA_ICFG_MODEN4_SHIFT     19

#define DMA_ICFG_PTYP_SHIFT       12                                       /* ST-Bus ptype */
#define DMA_ICFG_PTYP_MASK        MASK_BIT12 

#define DMA_ICFG_MSZ1_MASK        MASK_BIT8                                /* Master interface data bus width */
#define DMA_ICFG_MSZ2_MASK        MASK_BIT9
#define DMA_ICFG_MSZ3_MASK        MASK_BIT10
#define DMA_ICFG_MSZ4_MASK        MASK_BIT11
#define DMA_ICFG_MSZ1_SHIFT       8
#define DMA_ICFG_MSZ2_SHIFT       9
#define DMA_ICFG_MSZ3_SHIFT       10
#define DMA_ICFG_MSZ4_SHIFT       11
   
#define DMA_ICFG_CPSZ_SHIFT       7                                        /* CPU interface data bus width */
#define DMA_ICFG_CPSZ_MASK        MASK_BIT7 


#define DMA_ICFG_DBUFSZ_SHIFT     5                                        /* Data Buffer Size */
#define DMA_ICFG_DBUFSZ_MASK      (0x03 <<  DMA_ICFG_DBUFSZ_SHIFT)

#define DMA_ICFG_MNB_SHIFT        3                                        /* Number of master */
#define DMA_ICFG_MNB_MASK         (0x03  <<  DMA_ICFG_MNB_SHIFT)

#define DMA_ICFG_SCHNB_MASK       0x07                                     /* Standard Channel Number */


/*************************************************/
/*        Master Plug Status Register            */
/*************************************************/

#define DMA_MPLUG_MASK MASK_BIT0                                           /* Master Plug Status */


/*-------------------------- Standard  & Extended  Channel Parameters---------------------- */


/*************************************************/
/*      Configuration Register DMAC_SxCFG        */
/*************************************************/

/*---------------Physical Channel-----------------*/

#define DMA_SxCFG_MST_MASK          MASK_BIT15                             /* Master Port Seletion */
#define DMA_SxCFG_MST_SHIFT         15                                       
#define DMA_SxCFG_TIM_MASK          MASK_BIT14                             /* Terminal Count Interrupt mask */
#define DMA_SxCFG_TIM_SHIFT         14                                         
#define DMA_SxCFG_EIM_MASK          MASK_BIT13                             /* Error Interrupt mask */
#define DMA_SxCFG_EIM_SHIFT         13
#define DMA_SxCFG_PEN_MASK          MASK_BIT12                             /* Packet Enable */
#define DMA_SxCFG_PEN_SHIFT         12
#define DMA_SxCFG_PSIZE_MASK        (MASK_BIT11 | MASK_BIT10)              /* Packet Size */
#define DMA_SxCFG_PSIZE_SHIFT       10
#define DMA_SxCFG_ESIZE_MASK        (MASK_BIT9 | MASK_BIT8)                /* Element Size */
#define DMA_SxCFG_ESIZE_SHIFT       8
#define DMA_SxCFG_PRI_MASK          MASK_BIT7                              /* Priority Level of Physical resource */
#define DMA_SxCFG_PRI_SHIFT         7
#define DMA_SxCFG_LBE_MASK          MASK_BIT6                              /* Little/Big Endian */
#define DMA_SxCFG_LBE_SHIFT         6
#define DMA_SxCFG_TM_MASK           (MASK_BIT5 | MASK_BIT4)                /* Transfer Mode */
#define DMA_SxCFG_TM_SHIFT          4
#define DMA_SxCFG_EVTL_MASK         0xF                                    /* Event Line selection */

/*---------------Logical Channel-----------------*/

#define DMA_SxCFG_INCR_MASK         MASK_BIT12                             /* Address Increment */
#define DMA_SxCFG_INCR_SHIFT        12

#define DMA_SxCFG_GIM_MASK          MASK_BIT5                              /* Global Interrupt Mask  */
#define DMA_SxCFG_GIM_SHIFT         5

#define DMA_SxCFG_MFU_MASK          MASK_BIT4                              /* Master Parameter Fetch Update */
#define DMA_SxCFG_MFU_SHIFT         4



/*************************************************/
/*        Element Register DMAC_SxELT            */
/*************************************************/

/*-----------Physical Channel--------------------*/

#define DMA_SxELT_ECNT_MASK           0xFFFF0000
#define DMA_SxELT_ECNT_SHIFT          16                                   /* Element Counter */
#define DMA_SxELT_EIDX_MASK           0x0000FFFF                           /* Element Index */

/*-----------Logical Channel--------------------*/

#define DMA_SxELT_LIDX_MASK           0x0000FF00                           /* Link Index */
#define DMA_SxELT_LIDX_SHIFT          8                                         

/*************************************************/
/*        Link Register DMAC_SxLNK               */
/*************************************************/

/*-----------Physical Channel--------------------*/

#define DMA_SxLNK_LINK_MASK           0xFFFFFFF8                           /* Source/Destination Link Address */
#define DMA_SxLNK_LINK_SHIFT          3
#define DMA_SxLNK_PRE_MASK            MASK_BIT2                            /* PRE-LINK */
#define DMA_SxLNK_PRE_SHIFT           2
#define DMA_SxLNK_LMP_MASK            MASK_BIT1                            /* Link Master Port */
#define DMA_SxLNK_LMP_SHIFT           1
#define DMA_SxLNK_TCP_MASK            MASK_BIT0                            /* Terminal Count Pulse */

/*-----------Logical Channel--------------------*/

#define DMA_SxLNK_EE_MASK            (MASK_BIT0 | MASK_BIT1)                /* event enable for logical channels */

/*************************************************/
/*        Logical Channel Standard Parameters    */
/*************************************************/

#define DMA_LCSP0_ECNT_SHIFT            16
#define DMA_LCSP0_PTRLSB_MASK           0x0000FFFF

#define DMA_LCSP1_PTRMSB_SHIFT          16
#define DMA_LCSP1_PTRMSB_MASK           0xFFFF0000
#define DMA_LCSP1_CFG_MASK              0x00000FF00
#define DMA_LCSP1_LOS_SHIFT             1
 
#define DMA_LCSP2_ECNT_SHIFT            16
#define DMA_LCSP2_PTRLSB_MASK           0x0000FFFF

#define DMA_LCSP3_PTRMSB_MASK           0xFFFF0000
#define DMA_LCSP3_PTRMSB_SHIFT          16
#define DMA_LCSP3_CFG_MASK              0x00000FF00
#define DMA_LCSP3_LOS_SHIFT             1

#define DMA_SLOS_ZERO_MASK              0xFFFFFF01
#define DMA_DLOS_ZERO_MASK              0xFFFFFF01


/*************************************************/
/*        Miscellaneous Defines                  */
/*************************************************/

#define DMA_NO_EVENT_REQUEST  0xFF

/*************************************************/
/*        Private Structures and Enums           */
/*************************************************/

typedef t_uint8 t_dma_device;

typedef struct
{
    t_uint16   curr_src_lli_num;
	t_uint32   curr_src_lli_ptr;
	t_dma_lli_state src_phy_lli_state;
    t_uint16   curr_dest_lli_num;
    t_uint32   curr_dest_lli_ptr;
	t_dma_lli_state dest_phy_lli_state;
}t_dma_phy_lli_info;

typedef struct
{
    t_physical_address   base_phy_addr;
    t_logical_address    base_log_addr;
    t_uint8              link_offset;
}t_dma_log_lli_info;

typedef struct
{
   t_bool                    is_channel_open;
   t_dma_channel_type        type;
   t_dma_channel_security    security;
   t_dma_channel_mode        mode;
   t_dma_channel_option      option;
   t_dma_transfer_type       transfer_type;
   t_dma_phy_lli_info        phy_link_info;
   t_dma_log_lli_info        log_link_info;
   t_dma_exchange_state      exchange_state;
}t_dma_channel_info;

typedef struct
{
    t_physical_address       chan_link_phy_ptr;
    t_logical_address        chan_link_log_ptr;
    t_physical_address       chan_param_phy_ptr;     /* only for logical channels */
    t_logical_address        chan_param_log_ptr;     /* only for logical channels */
}t_dma_link_ptr;

typedef struct
{
    t_bool                  is_device_open;
    t_dma_device            device;
    t_dma_event_group       event_group;
    t_uint32                address;
	t_uint32                curr_addr;
    t_uint32                transfer_size;
    t_uint16                block_size;
	t_uint16                element_count;
    t_dma_master            master_port;
    t_dma_intr_gen          intr_gen;
    t_bool                  is_tc_intr;
    t_bool                  is_err_intr;
    t_bool                  is_pen;
    t_bool                  is_addr_incr;
    t_bool                  tcp_assert;
	t_dma_data_width        data_width;
    t_dma_burst_size        burst_size;
    t_dma_priority          priority;
    t_dma_endian            endian;
    t_dma_tx_mode           tx_mode;
    t_uint8                 event_line;
    t_uint16                element_index;
    t_dma_relink_ctrl       relink_ctrl;
    t_dma_event_type        event_type;
	t_uint8                 event_num;      
    t_dma_link_ptr          link_addr;
	t_dma_lli_state         log_lli_state;
    t_uint8                 link_id;
	t_uint8                 no_of_links;
}t_dma_device_info;


typedef struct
{
    t_dma_channel_info     channel_config[DMA_PHY_CHAN_PER_EVT_GRP];
    t_dma_device_info      src_device_config[DMA_PIPES_PER_PHY_CHAN];
    t_dma_device_info      dest_device_config[DMA_PIPES_PER_PHY_CHAN];
}t_dma_controller_desc;

typedef struct
{
    t_physical_address       physical_lli_base_addr;		
    t_logical_address        logical_lli_base_addr;
    t_physical_address       phy_chan_param_phy_addr;
    t_logical_address        phy_chan_param_log_addr;
	t_sint32                 log_to_phy_offset;
    t_physical_address       log_chan_link_phy_addr;
    t_logical_address        log_chan_link_log_addr;
    t_physical_address       log_chan_param_phy_addr;
    t_logical_address        log_chan_param_log_addr;
}t_dma_relink_config;


/*************************************************/
/*        System Context Variable                  */
/*************************************************/

/* Standard Channel parameter registers */
typedef struct
{    
    t_uint32 dmac_sscfg_excfg;  /* Standand channel Source Configuration Register        */
    t_uint32 dmac_sselt_exelt;  /* Standand channel Source Element Register              */
    t_uint32 dmac_ssptr_exptr;  /* Standand channel Source Pointer Register              */
    t_uint32 dmac_sslnk_exlnk;  /* Standand channel Source Link Register                 */
    t_uint32 dmac_sdcfg_exexc;  /* Standand channel Destination Configuration            */
    t_uint32 dmac_sdelt_exfrm;  /* Standand channel Destination Element Register         */
    t_uint32 dmac_sdptr_exrld;  /* Standand channel Destination Pointer Register         */
    t_uint32 dmac_sdlnk_exblk;  /* Standand channel Destination Link Register            */
}t_dma_channel_param;


/* DMA Registers */
typedef volatile struct
{
    t_uint32 dmac_gcc;          /*  Global Clocks Control                                         */
    t_uint32 dmac_prtyp;        /* Physical Resource Type                                         */
    t_uint32 dmac_prsme;        /* Physical Resource Secure Mode Even                             */
    t_uint32 dmac_prsmo;        /* Physical Resource Secure Mode Odd                              */
    t_uint32 dmac_prmse;        /* Physical Resource Mode Selection Even                          */
    t_uint32 dmac_prmso;        /* Physical Resource Mode Selection Odd                           */
    t_uint32 dmac_prmoe;        /* Physical Resource Mode Option Even                             */
    t_uint32 dmac_prmoo;        /* Physical Resource Mode Option Odd                              */
    t_uint32 dmac_lcpa;         /* Logical Channels Parameter base Address                        */
    t_uint32 dmac_lcla;         /* Logical Channels Link base Address                             */
    t_uint32 dmac_slcpa;        /* Secure Logical Channels Parameter base Address                 */
    t_uint32 dmac_slcla;        /* Secure Logical Channels Link base Address                      */
    t_uint32 dmac_sseg[4];   	/* Secure Set for Event Group 1(lines 0..15)                      */   
    t_uint32 dmac_sceg[4];   	/* Secure Clear for Event Group 1(lines 0..15)                    */   
    t_uint32 dmac_active;       /* Physical channels Active Even Command                          */
    t_uint32 dmac_activo;       /* Physical channels Active Odd Command                           */
    t_uint32 dmac_fsebs1;       /* Force Source Event Burst line Selection 1 (lines 0..31)        */
    t_uint32 dmac_fsebs2;       /* Force Source Event Burst line Selection 2 (lines 32..63)       */   
    t_uint32 dmac_pcmis;        /* Physical Channels Masked Interrupt Status                      */
    t_uint32 dmac_pcicr;        /* Physical Channels Interrupt Clear Register                     */
    t_uint32 dmac_pctis;        /* Physical Channels Terminal count Raw Interrupt Status          */
    t_uint32 dmac_pceis;      	/* Physical Channels Error raw Interrupt Status                   */
    t_uint32 dmac_spcmis;      	/* Secure Physical Channels Masked Interrupt Status               */
    t_uint32 dmac_spcicr;       /* Secure Physical Channels Interrupt Clear Register              */
    t_uint32 dmac_spctis;       /* Secure Physical Channels T. count Raw Interrupt Status         */
    t_uint32 dmac_spceis;       /* Secure Physical Channels Error Raw Interrupt Status            */
    t_uint32 dmac_lcmis[4];     /* Logical Channels Masked Interrupt Status (1,2,3,4)             */
    t_uint32 dmac_lcicr[4];     /* Logical Channels Interrupt Clear Register 1                    */
    t_uint32 dmac_lctis[4];     /* Logical Channels Terminal count Raw Interrupt Status 1         */
    t_uint32 dmac_lceis[4];     /* Logical Channels Error raw Interrupt Status 1                  */
    t_uint32 dmac_slcmis[4];    /* Secure Logical Channels Masked Interrupt Status 1              */
    t_uint32 dmac_slcicr[4];    /* Secure Logical Channels Interrupt Clear Register 1             */
    t_uint32 dmac_slctis[4];    /* Secure Logical Channels Terminal count Raw Int Status 1        */
    t_uint32 dmac_slceis[4];    /* Secure Logical Channels Error raw Interrupt Status 1           */
#if defined(ST_8500ED) || defined(ST_HREFED) || defined(__PEPS_8500) 
    t_uint32 dmac_unused1[(0x400 - 0x100) >> 2];

#elif defined(ST_8500V1) || defined(ST_HREFV1)|| defined(__PEPS_8500_V1)|| defined(__PEPS_8500_V2)
    t_uint32 dmac_fsess[2];     /* Force Source Event Source Line Selection Register              */
    t_uint32 dmac_fsebs[2];     /* Force Source Event Burst Line Selection Register               */
    t_uint32 dmac_unused1[(0x400 - 0x110) >> 2];
#else
	t_uint32 dmac_fsess[2];     /* Force Source Event Source Line Selection Register              */
    t_uint32 dmac_fsebs[2];     /* Force Source Event Burst Line Selection Register               */
    t_uint32 dmac_pseg1;		/* Priority set for Event Group 1 */
    t_uint32 dmac_pseg2;		/* Priority set for Event Group 2 */
    t_uint32 dmac_pseg3;		/* Priority set for Event Group 3 */
    t_uint32 dmac_pseg4;		/* Priority set for Event Group 4 */
    t_uint32 dmac_pceg1;		/* Priority Clear for Event Group 1 */
    t_uint32 dmac_pceg2;		/* Priority Clear for Event Group 2 */
    t_uint32 dmac_pceg3;		/* Priority Clear for Event Group 3 */	
    t_uint32 dmac_pceg4;		/* Priority Clear for Event Group 4 */
    t_uint32 dmac_rseg1;		/* Realtime set for Event Group 1 */
    t_uint32 dmac_rseg2;		/* Realtime set for Event Group 2 */
    t_uint32 dmac_rseg3;		/* Realtime set for Event Group 3 */
    t_uint32 dmac_rseg4;		/* Realtime set for Event Group 4 */
    t_uint32 dmac_rceg1;		/* Realtime clear for Event Group 1 */
    t_uint32 dmac_rceg2;		/* Realtime clear for Event Group 2 */
    t_uint32 dmac_rceg3;        /* Realtime clear for Event Group 3 */    
    t_uint32 dmac_rceg4;        /* Realtime clear for Event Group 4 */
    t_uint32 dmac_unused1[(0x400 - 0x150) >> 2];
#endif
    t_dma_channel_param dma_channel_param[DMA_PHY_CHANNEL_MAX]; /* Channel parameters register    */     
    t_uint32 dmac_unused2[(0xFC8 - 0x800) >> 2];
    t_uint32 dmac_stfu;         /* DMAC Statistic Functions                                       */
    t_uint32 dmac_icfg;         /* DMAC Implementation Configuration                              */
    t_uint32 dmac_mplug[4];     /* Master 1,2,3,4 pyhsical plug Configuration                     */    
    t_uint32 dma_periphid0;     /* Peripheral identifcation register 0                            */
    t_uint32 dma_periphid1;     /* Peripheral identifcation register 1                            */
    t_uint32 dma_periphid2;     /* Peripheral identifcation register 2                            */
    t_uint32 dma_periphid3;     /* Peripheral identifcation register 3                            */
    t_uint32 dma_cellid0;       /* IP Cell identifcation register 0                               */
    t_uint32 dma_cellid1;       /* IP Cell identifcation register 1                               */
    t_uint32 dma_cellid2;       /* IP Cell identifcation register 2                               */ 
    t_uint32 dma_cellid3;       /* IP Cell identifcation register 3                               */
}t_dma_register;

typedef struct
{
     t_dma_register *p_dma_register; /* Pointer which stores the DMA base address */
}t_dma_system_context;

#endif /* __DMA_PRIVATE_HEADER */



