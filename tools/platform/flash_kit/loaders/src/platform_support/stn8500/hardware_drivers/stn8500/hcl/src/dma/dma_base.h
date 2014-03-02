/* --------------------------------------------------------------------------------------------
* STEricsson
* Reproduction and Communication of this document is strictly prohibited
* unless specifically authorized in writing by STEricsson
* --------------------------------------------------------------------------------------------
* File Name: dma_base.h
* Contains definitions for enums, structs and functions used in dma_base.h
* -------------------------------------------------------------------------------------------- */

#ifndef __DMA_BASE_HEADER
#define __DMA_BASE_HEADER

#ifdef	__cplusplus
extern "C" 
{                  /* To allow C++ to use this header */
#endif

#include "hcl_defs.h"
#include "debug.h"

#define DMA_BASE_DIVISOR_VALUE       0x02
#define DMA_BASE_EVENT_VALUE         0x10
#define DMA_BASE_WORD_VALUE          0x20

/* Changes for HREFV1 */
#define DMA_ACTIVE_MASK  0xFFFFFFFF

typedef enum
{
    DMA_DISABLE = 0x0,
    DMA_ENABLE = 0x1        
}t_dma_ctrl;

typedef enum
{
    DMA_MASTER_PORT_0 = 0x0,
    DMA_MASTER_PORT_1 = 0x1        
}t_dma_master_port;

typedef enum
{
	DMA_CHANNEL_0 = 0x0,     		/* PR0 & PR1 can be concatenated to form an Extended PR0 */
	DMA_CHANNEL_1 = 0x1,
	DMA_CHANNEL_2 = 0x2,         	/* PR2 & PR3 can be concatenated to form an Extended PR1 */
	DMA_CHANNEL_3 = 0x3,         
	DMA_CHANNEL_4 = 0x4,         	/* PR4 & PR5 can be concatenated to form an Extended PR2 */
	DMA_CHANNEL_5 = 0x5,
	DMA_CHANNEL_6 = 0x6,         	/* PR6 & PR7 can be concatenated to form an Extended PR3 */
	DMA_CHANNEL_7 = 0x7, 
	DMA_CHANNEL_8					/* not used */
}t_dma_channel;

typedef enum
{
	DMA_EVT_LINE_0	=	0,
	DMA_EVT_LINE_1	=	1,
	DMA_EVT_LINE_2	=	2,
	DMA_EVT_LINE_3	=	3,
	DMA_EVT_LINE_4	=	4,
	DMA_EVT_LINE_5	=	5,
	DMA_EVT_LINE_6	=	6,
	DMA_EVT_LINE_7	=	7,
	DMA_EVT_LINE_8	=	8,
	DMA_EVT_LINE_9	=	9,
	DMA_EVT_LINE_10	=	10,
	DMA_EVT_LINE_11	=	11,
	DMA_EVT_LINE_12	=	12,
	DMA_EVT_LINE_13	=	13,
	DMA_EVT_LINE_14	=	14,
	DMA_EVT_LINE_15	=	15,
	DMA_EVT_LINE_16	=	16,
	DMA_EVT_LINE_17	=	17,
	DMA_EVT_LINE_18	=	18,
	DMA_EVT_LINE_19	=	19,
	DMA_EVT_LINE_20	=	20,
	DMA_EVT_LINE_21	=	21,
	DMA_EVT_LINE_22	=	22,
	DMA_EVT_LINE_23	=	23,
	DMA_EVT_LINE_24	=	24,
	DMA_EVT_LINE_25	=	25,
	DMA_EVT_LINE_26	=	26,
	DMA_EVT_LINE_27	=	27,
	DMA_EVT_LINE_28	=	28,
	DMA_EVT_LINE_29	=	29,
	DMA_EVT_LINE_30	=	30,
	DMA_EVT_LINE_31	=	31,
	DMA_EVT_LINE_32	=	32,
	DMA_EVT_LINE_33	=	33,
	DMA_EVT_LINE_34	=	34,
	DMA_EVT_LINE_35	=	35,
	DMA_EVT_LINE_36	=	36,
	DMA_EVT_LINE_37	=	37,
	DMA_EVT_LINE_38	=	38,
	DMA_EVT_LINE_39	=	39,
	DMA_EVT_LINE_40	=	40,
	DMA_EVT_LINE_41	=	41,
	DMA_EVT_LINE_42	=	42,
	DMA_EVT_LINE_43	=	43,
	DMA_EVT_LINE_44	=	44,
	DMA_EVT_LINE_45	=	45,
	DMA_EVT_LINE_46	=	46,
	DMA_EVT_LINE_47	=	47,
	DMA_EVT_LINE_48	=	48,
	DMA_EVT_LINE_49	=	49,
	DMA_EVT_LINE_50	=	50,
	DMA_EVT_LINE_51	=	51,
	DMA_EVT_LINE_52	=	52,
	DMA_EVT_LINE_53	=	53,
	DMA_EVT_LINE_54	=	54,
	DMA_EVT_LINE_55	=	55,
	DMA_EVT_LINE_56	=	56,
	DMA_EVT_LINE_57	=	57,
	DMA_EVT_LINE_58	=	58,
	DMA_EVT_LINE_59	=	59,
	DMA_EVT_LINE_60	=	60,
	DMA_EVT_LINE_61	=	61,
	DMA_EVT_LINE_62	=	62,
	DMA_EVT_LINE_63	=	63,
	DMA_NO_EVT	=	64
}t_dma_event;

/* ---------------------------- Description Register Wise ------------------------------------------*/

/*--------------------------------------------------------------------------------------------------*/
/*----------------------------- Enumerations for Global Clock Control Registers---------------------*/
/*--------------------------------------------------------------------------------------------------*/

typedef enum                     
{                                
	DMA_GLOBAL         = 0x1,   	/* Src Event(RX) lines 0-15 with Physical Resource 0,1,8,9,16,17,24,25 		*/
	DMA_SRC_EVT_GRP_0  = 0x100, 	/* Src Event(RX) lines 16-31 with Physical Resource 2,3,10,11,18,19,26,27 	*/
	DMA_DEST_EVT_GRP_0 = 0x200, 	/* Src Event(RX) lines 32-47 with Physical Resource 4,5,12,13,20,21,28,29 	*/
	DMA_SRC_EVT_GRP_1  = 0x400, 	/* Src Event(RX) lines 47-63 with Physical Resource 6,7,14,15,22,23,30,31 	*/
	DMA_DEST_EVT_GRP_1 = 0x800,  	/* Dest Event(TX) lines 0-15 with Physical Resource 0,1,8,9,16,17,24,25 	*/
	DMA_SRC_EVT_GRP_2  = 0x1000, 	/* Dest Event(TX) lines 16-31 with Physical Resource 2,3,10,11,18,19,26,27 	*/
	DMA_DEST_EVT_GRP_2 = 0x2000, 	/* Dest Event(TX) lines 32-47 with Physical Resource 4,5,12,13,20,21,28,29 	*/
	DMA_SRC_EVT_GRP_3  = 0x4000,	/* Dest Event(TX) lines 47-63 with Physical Resource 6,7,14,15,22,23,30,31 	*/
	DMA_DEST_EVT_GRP_3 = 0x8000,   /* Entire DMA Kernel Logic */
	DMA_ALL_EVT_GRP    = 0xFF01 
}t_dma_evt_grp;

/*---------------------------------------------------------------------------------------------------------------*/
/*---- Enumerations for DMAC Physical Resource Type Registers : DMAC_PRTYP/PRSME/PRSMO/PRMSE/PRMSO/PRMOE/PRMOO --*/
/*---------------------------------------------------------------------------------------------------------------*/

typedef enum				/* DMAC_PRTYP 				: CPx_TYP    x = 0..15 	*/		
{
	DMA_PR_NOT_CONCATENATED	 	= 0x1,
	DMA_PR_CONCATENATED		 	= 0x2
}t_dma_pr_type;

typedef enum				/* DMAC_PRSME & DMAC_PRSMO	:  PRx_SEC    x = 0..31 */
{
	DMA_PR_SECURE		 		= 0x1,
	DMA_PR_NONSECURE	 		= 0x2
}t_dma_pr_security;

typedef enum				/* DMAC_PRMSE & DMAC_PRMSO	:  PRx_MOD    x = 0..31	*/
{
	DMA_PR_MODE_BASIC		 	= 0x1,
	DMA_PR_MODE_LOGICAL		 	= 0x2,
	DMA_PR_MODE_OPERATION	 	= 0x3
}t_dma_pr_mode;

typedef enum				/* DMAC_PRMOE & DMAC_PRMOO 	:	PRx_OPT	  x = 0..31	*/
{
	DMA_PR_MODE_OPTION_BASIC					= 		0x1, /* Option for PR in basic mode */
	DMA_PR_MODE_OPTION_MODULO					= 		0x2, /* Option for PR in basic mode */
	DMA_PR_MODE_OPTION_DOUBLE_DEST				= 		0x3, /* Option for PR in basic mode */
	DMA_PR_MODE_OPTION_SRC_PHY_DEST_LOG 		= 		0x1, /* Option for PR in logical mode */
	DMA_PR_MODE_OPTION_SRC_LOG_DEST_PHY 		= 		0x2, /* Option for PR in logical mode */
	DMA_PR_MODE_OPTION_SRC_LOG_DEST_LOG	 		= 		0x3  /* Option for PR in logical mode */
	/* Option for PR in Operation mode is currently reserved for future */	
}t_dma_pr_mode_option;                            


/*------------------------------------------------------------------------------------------*/
/*-----  Enumerations for Logical Channel Address : DMAC_LCPA/LCLA/SLCPA/SLCLA -------------*/
/*------------------------------------------------------------------------------------------*/

typedef enum	
{
	DMA_BASE_ADDR_PARAM,
    DMA_BASE_ADDR_LINK
}t_dma_base_addr_type;

typedef struct			/* DMAC_LCPA & DMAC_LCLA 	:	LCPA & LCLA	*/
{
	t_uint32 param_base_addr;
	t_uint32 link_base_addr;
}t_dma_base_addr;


/*------------------------------------------------------------------------------------------*/
/*---------- Enumerations for Secure Event Group : DMAC_SSEG 1..4 / SCEG 1..4 --------------*/
/*------------------------------------------------------------------------------------------*/

typedef enum
{
	DMA_SRC_EVT_LINE,  /* RX */
	DMA_DEST_EVT_LINE  /* TX */
}t_dma_evt_line_type;

typedef enum
{
	DMA_EVT_LINE_DISABLE, 
	DMA_EVT_LINE_ENABLE   
}t_dma_evt_line_ctrl;

/*------------------------------------------------------------------------------------------*/
/*-------- Enumerations for Physical Resource Activation Command : DMAC_ACTIVE/ACTIVO ------*/
/*------------------------------------------------------------------------------------------*/

typedef enum				/*	PRx_AS  x = 1..31*/
{
	DMA_ACTIVE_STOP			 	= 0x0,
	DMA_ACTIVE_RUN			 	= 0x1,
	DMA_ACTIVE_SUSPEND_REQ   	= 0x2,
	DMA_ACTIVE_SUSPENDED 	 	= 0x3
}t_dma_active_state;

/*------------------------------------------------------------------------------------------*/
/*------------------- Enumerations for Statistics Functions : DMAC_STFU --------------------*/
/*------------------------------------------------------------------------------------------*/

typedef enum
{
	DMA_STAT_DISABLE, 
	DMA_STAT_ENABLE   
}t_dma_stat_feature;

typedef enum									/* Statistic Mode 			: 	MODE*/
{
	DMA_MASTER_BANDWITH			 	= 0x0,
	DMA_MASTER_WAIT_CYCLE_COST	 	= 0x1,
	DMA_MASTER_WORKLOAD 		 	= 0x2,
	DMA_HALF_CHAN_TRANSFER_COST	 	= 0x4,
	DMA_HALF_CHAN_WAIT_CYCLE_COST 	= 0x5	
}t_dma_stat_mode;


typedef enum									/* Source or Destinaton half-channel	:	SD */
{
	DMA_SRC_HALF_CHAN ,
	DMA_DEST_HALF_CHAN 
}t_dma_half_chan_type;

typedef struct									/* User defined functionality 		*/
{
	t_dma_stat_feature	 stat_feature;
	t_dma_stat_mode		 stat_mode;
	t_dma_master_port	 dma_master;
	t_dma_channel		 dma_channel;
	t_dma_half_chan_type half_chan_type;
	t_uint8				 logical_chan_evt_line;
}t_dma_stat_config;

/*------------------------------------------------------------------------------------------*/
/*---- Enumerations for Implementation Configuration Registers Functions : DMAC_ICFG -------*/
/*------------------------------------------------------------------------------------------*/

typedef enum									/*	Modulo Enable : MODEN */
{
    MODULO_AVAILABLE 		= 0x0,	
    MODULO_NOT_AVAILABLE 	= 0x1
}t_dma_modulo_enable;

typedef enum									/*	PTYPE Management : PYP */
{
    STBUS_T3_ATTRB 			= 0x0,	
    NEW_ATTRB		 		= 0x1
}t_dma_ptype_mngmt;

typedef enum									/*	Master x Interface data Bus Width : MxSZ x = 1..4*/
{
    DMA_MASTER_32_BIT 		= 0x0,	
    DMA_MASTER_64_BIT 		= 0x1
}t_dma_data_bus_width;

typedef enum									/*	Data Buffer Size : DBUFSZ */
{
	DMA_BUF_16_BYTES 		= 0x0,
	DMA_BUF_32_BYTES 		= 0x1,
	DMA_BUF_64_BYTES 		= 0x2,
	DMA_BUF_128_BYTES		= 0x3	
}t_dma_data_buf_size;

typedef enum									/*	DMAC Master Number : MNB */
{
	ONE_DMAC_MASTER_IF 		= 0x0,
	TWO_DMAC_MASTER_IF 		= 0x1,
	THREE_DMAC_MASTER_IF 	= 0x2,
	FOUR_DMAC_MASTER_IF 	= 0x3	
}t_dma_master_port_number;


typedef enum									/*	Standard Channel Number : SCHNB	*/
{
	DMA_4_STD_PR,
	DMA_8_STD_PR,
	DMA_12_STD_PR,
	DMA_16_STD_PR,
	DMA_20_STD_PR,
	DMA_24_STD_PR,
	DMA_28_STD_PR,
	DMA_32_STD_PR	
}t_dma_std_chan_num;

typedef struct
{
	t_dma_modulo_enable 		master1_modulo;
	t_dma_modulo_enable 		master2_modulo;
	t_dma_modulo_enable 		master3_modulo;
	t_dma_modulo_enable 		master4_modulo;
	t_dma_ptype_mngmt 			ptyp_asymm_cmd;
	t_dma_data_bus_width 		master1_if_data_width;
	t_dma_data_bus_width 		master2_if_data_width;
	t_dma_data_bus_width 		master3_if_data_width;
	t_dma_data_bus_width 		master4_if_data_width;
	t_dma_data_bus_width 		cpu_if_data_width;	
	t_dma_data_buf_size 		data_buf_size;
	t_dma_master_port_number			num_of_master_if;
	t_dma_std_chan_num			std_chan_num;
}t_dma_icfg_config;

/*------------------------------------------------------------------------------------------*/
/*------- Enumerations for Standard Channel Configuration Registers : DMAC_SxCFG ----------*/
/*------------------------------------------------------------------------------------------*/


typedef enum							/*   Interrupt Mask 	: TIM	*/
{
	INTR_DISABLE = 0x0,
	INTR_ENABLE  = 0x1 
}t_dma_interrupt_mask;

typedef enum							/*   Packet Enable (physical channel) 	: PEN	*/
{
	DISABLE  = 0x0,
	ENABLE= 0x1
}t_dma_pen_incr;

typedef enum							/*   Address Increment (logical channel) : INCR	*/
{
	ADDR_NOT_INCR 	= 0x0,
	ADDR_INCR_ESIZE 
}t_dma_addr_incr;

typedef enum							/*   Packet Size : PSIZE	*/
{
	PACKET_02_ELEM = 0x0,
	PACKET_04_ELEM = 0x1,
	PACKET_08_ELEM = 0x2,
	PACKET_16_ELEM = 0x3
}t_dma_packet_size;

typedef enum							/*   Element Size : ESIZE	*/
{
	ELEM_08_BIT_WIDE = 0x0,
	ELEM_16_BIT_WIDE = 0x1,
	ELEM_32_BIT_WIDE = 0x2,
	ELEM_64_BIT_WIDE = 0x3
}t_dma_element_size;

typedef enum							/*   Priority	: PRI	*/
{
    PRIORITY_LOW 	= 0x0,
    PRIORITY_HIGH	= 0x1	
}t_dma_pr_priority;

typedef enum 
{
	NO_CHANGE 		 = 0x0,				/*   Endian		: LBE	*/
	CHANGE_ENDIANESS = 0x1	         	
} t_dma_endianess;                  	
	
typedef enum							/*   Transfer Mode : TM 	*/
{
    NON_SYNC_TRANSFER		 = 0x0,
    SYNC_TRANSFER_ONE_PACKET = 0x1,
    SYNC_TRANSFER_ONE_FRAME	 = 0x2,
    SYNC_TRANSFER_ONE_BLOCK	 = 0x3
}t_dma_transfer_mode;

typedef struct			/*Standard channel SxCFG configuration*/
{	
	t_dma_master_port	 	master_port; 					/* Master Port Selection 				*/
    t_dma_interrupt_mask 	tc_intr_mask;          			/* Terminal Count Interrupt mask 		*/
    t_dma_interrupt_mask 	err_intr_mask;         			/* Error Interrupt Mask					*/
    t_dma_pen_incr			pen_incr;    					/* Packet Enable / Addr Incr (Logical)	*/
    t_dma_packet_size 		packet_size;   					/* Packet Size							*/
    t_dma_element_size 		element_size;   				/* Element Size		 					*/
    t_dma_pr_priority 		priority_level; 				/* Priority Level 						*/
    t_dma_endianess			switch_endian;      			/* Endianess Conversion					*/
    t_dma_transfer_mode 	transfer_mode; 					/* Transfer Mode	 					*/
    t_uint8 				event_line;              		/* Event Line		 					*/
	t_dma_interrupt_mask	global_intr_mask;				/* Global Interrupt Mask (Logical)		*/
	t_dma_master_port		master_par_fetch;				/* Master Param Fetch/Update (Logical)	*/		
}t_dma_sxcfg_config;


/*------------------------------------------------------------------------------------------*/
/*------- Enumerations for Standard Channel Element Registers : DMAC_SxELT -----------------*/
/*------------------------------------------------------------------------------------------*/

typedef enum 
{
	NO_TCP_ASSERTED = 0x0,				/*   Terminal Count Pulse : TCP	*/
	TCP_ASSERTED
} t_dma_tcp_assert;


typedef struct			/*Standard channel SxELT configuration*/
{
	t_uint16 				element_counter;       			/* Element Counter		 				*/
    t_uint16 				element_index;             		/* Element Index						*/
    t_uint8 				link_index;             		/* Link Index							*/
	t_uint8 				link_offset;             		/* Link Offset							*/
	t_dma_tcp_assert		tcp_assert;
}t_dma_sxelt_config;


/*------------------------------------------------------------------------------------------*/
/*------- Enumerations for Standard Channel Link Registers : DMAC_SxLNK -------------------*/
/*------------------------------------------------------------------------------------------*/

typedef enum 
{
	RELINK_AFTER_TRANSFER 	= 0x0,		/*   PRE-LINK			: PRE	*/
	RELINK_BEFORE_TRANSFER 	= 0x1
} t_dma_pre_link;

typedef enum 
{
	LINK_MASTER_PORT_0 = 0x0,			/*   Link Master Port	: LMP	*/
	LINK_MASTER_PORT_1
} t_dma_link_master_port;


typedef enum							/* Event Enable (Logical Channel) */
{
	DMA_EVT_DISABLE = 	0x0,
	DMA_EVT_ENABLE 	= 	0x1,
	DMA_EVT_ROUND 	= 	0x2
}t_dma_evt_ctrl;

typedef struct
{
	t_uint32 				link_addr;            			/* Source/Destination Link Address		*/
	t_dma_pre_link			pre_link;           			/* Pre - Link 		 					*/
	t_dma_master_port	 	link_master_port;  				/* Link Master Port 					*/
	t_dma_tcp_assert		tcp_assert;            			/* Terminal Count Pulse 				*/
}t_dma_sxlnk_config;

/*---------------------------------------End----------------------------------------------------*/

/* Standard Channel parameter registers */
typedef volatile struct
{	
    t_uint32 dmac_sscfg;  			/* 		Standand channel Source Configuration Register 		*/
    t_uint32 dmac_sselt;  			/* 		Standand channel Source Element Register  			*/
    t_uint32 dmac_ssptr;  			/* 		Standand channel Source Pointer Register 			*/
    t_uint32 dmac_sslnk;  			/* 		Standand channel Source Link Register  				*/
    t_uint32 dmac_sdcfg;  			/* 		Standand channel Destination Configuration 			*/
    t_uint32 dmac_sdelt;  			/* 		Standand channel Destination Element Register  		*/
    t_uint32 dmac_sdptr;  			/* 		Standand channel Destination Pointer Register  		*/
    t_uint32 dmac_sdlnk;			/* 		Standand channel Destination Link Register  		*/
}t_dma_std_channel_param;

/* Extended Channel parameter registers */
typedef volatile struct
{	
    t_uint32 dmac_escfg;  			/*  	Extended channel Configuration Register				*/
    t_uint32 dmac_eselt;  			/*  	Extended channel Element Register 					*/
    t_uint32 dmac_esptr;  			/*  	Extended channel Pointer Register					*/
    t_uint32 dmac_eslnk;  			/*  	Extended channel Link Register 						*/
    t_uint32 dmac_esexc;  			/*  	Extended channel Extra Configuration Register		*/
    t_uint32 dmac_esfrm;  			/*  	Extended channel Frame Register 					*/
    t_uint32 dmac_esrld;  			/* 		Extended channel Reload Register 					*/
    t_uint32 dmac_esblk;			/* 		Extended channel Block Register 					*/
}t_dma_ext_channel_param;

/*------------------------------------- DMA HCL APIs ----------------------------------------------*/

/*--------- Initilisation & Supplemenatary APIs ------------*/
PUBLIC t_bool dmaBase_Init(IN t_logical_address);

/*--------- Set/Get Information APIs ----------*/

PUBLIC void dmaBase_SetClockControl(IN t_dma_evt_grp , IN t_dma_ctrl);
PUBLIC void dmaBase_GetClockControl(IN t_dma_evt_grp , OUT t_dma_ctrl *);

PUBLIC void dmaBase_SetPRType(IN t_dma_channel , IN t_dma_pr_type );
PUBLIC void dmaBase_SetPRSecurity(IN t_dma_channel , IN t_dma_pr_security );
PUBLIC void dmaBase_SetPRModeSelect(IN t_dma_channel , IN t_dma_pr_mode );
PUBLIC void dmaBase_SetPRModeOption(IN t_dma_channel , IN t_dma_pr_mode_option );
PUBLIC void dmaBase_SetPRActiveStatus(IN t_dma_channel , IN t_dma_active_state );

PUBLIC t_dma_pr_type 		dmaBase_GetPRType(IN t_dma_channel);
PUBLIC t_dma_pr_security 	dmaBase_GetPRSecurity(IN t_dma_channel);
PUBLIC t_dma_pr_mode 		dmaBase_GetPRModeSelect(IN t_dma_channel );
PUBLIC t_dma_pr_mode_option dmaBase_GetPRModeOption(IN t_dma_channel );
PUBLIC t_dma_active_state 	dmaBase_GetPRActiveStatus(IN t_dma_channel );

PUBLIC void dmaBase_SetLogicalChanBaseAddr(IN t_dma_pr_security , IN t_dma_base_addr *);
PUBLIC void dmaBase_GetLogicalChanBaseAddr(IN t_dma_pr_security , OUT t_dma_base_addr *);
       
PUBLIC void dmaBase_SetEventLineSecurity(IN t_dma_pr_security , IN t_dma_evt_line_type , IN t_dma_event);
PUBLIC void dmaBase_GetEventLineSecurity(IN t_dma_evt_line_type ,IN t_dma_event ,OUT t_dma_pr_security *);
PUBLIC void dmaBase_ClearEventLineSecurity(IN t_dma_pr_security ,IN t_dma_evt_line_type , IN t_dma_event);       

PUBLIC void dmaBase_SetForceSrcEvtBurst(t_uint8);
PUBLIC void dmaBase_GetForceSrcEvtBurst(t_uint8,t_dma_ctrl *);
       
PUBLIC void dmaBase_SetStatistics(IN t_dma_stat_config *);
PUBLIC void dmaBase_GetStatistics(OUT t_dma_stat_config * , OUT t_uint16 *);
       
PUBLIC void dmaBase_GetHwConfig(OUT t_dma_icfg_config *);
PUBLIC void dmaBase_GetMasterPlugStatus(IN t_dma_master_port , IN t_dma_channel , OUT t_dma_ctrl *);
       
PUBLIC void dmaBase_SetChannelConfig(IN t_dma_channel , IN t_dma_pr_mode , IN t_dma_half_chan_type, IN t_dma_sxcfg_config *);
PUBLIC void dmaBase_SetChannelElement(IN t_dma_channel , IN t_dma_pr_mode , IN t_dma_half_chan_type , IN t_dma_sxelt_config *);
PUBLIC void dmaBase_SetChannelPointer(IN t_dma_channel , IN t_dma_half_chan_type, IN t_uint32 );
PUBLIC void dmaBase_SetChannelLink(IN t_dma_channel , IN t_dma_half_chan_type, IN t_dma_sxlnk_config *);
PUBLIC void dmaBase_GetPhyLinkStatus(IN t_dma_channel ,IN t_dma_half_chan_type , IN t_uint32 *);
PUBLIC void dmaBase_ResetChannel(IN t_dma_channel);
       
PUBLIC void dmaBase_SetLogChannelEvent(IN t_dma_channel , IN t_dma_evt_line_type , IN t_dma_event , IN t_dma_evt_ctrl );
PUBLIC void dmaBase_GetLogChannelEvent(IN t_dma_channel , IN t_dma_evt_line_type , IN t_dma_event , OUT t_dma_evt_ctrl *);

PUBLIC t_uint32 dmaBase_GetChannelTCIntStatus(t_bool secure);
PUBLIC t_uint32 dmaBase_GetChannelErrorIntStatus(t_bool secure);

#ifdef __cplusplus
} /* To allow C++ to use this header */
#endif	/* __cplusplus */

#endif /* __dmaBase_HEADER */



