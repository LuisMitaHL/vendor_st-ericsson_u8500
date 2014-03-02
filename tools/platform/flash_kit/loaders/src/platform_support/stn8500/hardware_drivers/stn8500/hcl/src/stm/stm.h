/*****************************************************************************/
/**
*  © ST-Ericsson, 2009 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \brief   Public Header file of System Trace Module (STM)          
* \author  ST-Ericsson
*/
/*****************************************************************************/


#ifndef _STM_H_
#define _STM_H_

/*--------------------------------------------------------------------------*
 * Includes																	*
 *--------------------------------------------------------------------------*/
#include "debug.h"
#include "hcl_defs.h"

#ifdef __cplusplus
extern "C"
{
#endif
extern  t_uint32 stm_message_counter;
/* STM Variables */
typedef t_uint32 t_stm_initiatormsg;
/*--------------------------------------------------------------------------*
 * Constants and new types													*
 *--------------------------------------------------------------------------*/
#define STM_HCL_VERSION 1
#define STM_HCL_MAJOR   2
#define STM_HCL_MINOR   5

#define STM_NO_OVERFLOW  0x00


/* STM protocol */
#define STT_END_CHANNEL 250

#define SET_BIT_STT_RESERVED	(1 << 7)
#define SET_BIT_VECTOR		    (1 << 6)
#define SET_BIT_SIGNED		    (1 << 5)

#define STT_PRLG_VERSION		(SET_BIT_STT_RESERVED | 0)
#define STT_PRLG_UNF_PRINTF		(SET_BIT_STT_RESERVED | 1)
#define STT_PRLG_STR			(SET_BIT_STT_RESERVED | 2)

#define STT_PRLG_T_U8			    (SET_BIT_STT_RESERVED | SET_BIT_VECTOR | 1)
#define STT_PRLG_T_U16			    (SET_BIT_STT_RESERVED | SET_BIT_VECTOR | 2)
#define STT_PRLG_T_U32			    (SET_BIT_STT_RESERVED | SET_BIT_VECTOR | 3)
#define STT_PRLG_T_U64			    (SET_BIT_STT_RESERVED | SET_BIT_VECTOR | 4)
#define STT_PRLG_T_U16_8		    (SET_BIT_STT_RESERVED | SET_BIT_VECTOR | 5)
#define STT_PRLG_T_U32_U32_24	    (SET_BIT_STT_RESERVED | SET_BIT_VECTOR | 6)
#define STT_PRLG_T_U32_U32		    (SET_BIT_STT_RESERVED | SET_BIT_VECTOR | 7)
#define STT_PRLG_T_U32_U16_8	    (SET_BIT_STT_RESERVED | SET_BIT_VECTOR | 8)
#define STT_PRLG_T_U32_U16		    (SET_BIT_STT_RESERVED | SET_BIT_VECTOR | 9)
#define STT_PRLG_T_U32_24		    (SET_BIT_STT_RESERVED | SET_BIT_VECTOR | 10)
#define STT_PRLG_T_U32_U16_U16_8    (SET_BIT_STT_RESERVED | SET_BIT_VECTOR | 11)

#define STT_PRLG_T_S8			    (SET_BIT_STT_RESERVED | SET_BIT_VECTOR | SET_BIT_SIGNED | 1)
#define STT_PRLG_T_S16			    (SET_BIT_STT_RESERVED | SET_BIT_VECTOR | SET_BIT_SIGNED | 2)
#define STT_PRLG_T_S32			    (SET_BIT_STT_RESERVED | SET_BIT_VECTOR | SET_BIT_SIGNED | 3)
#define STT_PRLG_T_S64			    (SET_BIT_STT_RESERVED | SET_BIT_VECTOR | SET_BIT_SIGNED | 4)
#define STT_PRLG_T_S16_8		    (SET_BIT_STT_RESERVED | SET_BIT_VECTOR | SET_BIT_SIGNED | 5)
#define STT_PRLG_T_S32_S32_24		(SET_BIT_STT_RESERVED | SET_BIT_VECTOR | SET_BIT_SIGNED | 6)
#define STT_PRLG_T_S32_S32		    (SET_BIT_STT_RESERVED | SET_BIT_VECTOR | SET_BIT_SIGNED | 7)
#define STT_PRLG_T_S32_S16_8		(SET_BIT_STT_RESERVED | SET_BIT_VECTOR | SET_BIT_SIGNED | 8)
#define STT_PRLG_T_S32_S16		    (SET_BIT_STT_RESERVED | SET_BIT_VECTOR | SET_BIT_SIGNED | 9)
#define STT_PRLG_T_S32_24		    (SET_BIT_STT_RESERVED | SET_BIT_VECTOR | SET_BIT_SIGNED | 10)
#define STT_PRLG_T_S32_S16_S16_8	(SET_BIT_STT_RESERVED | SET_BIT_VECTOR | SET_BIT_SIGNED | 11)


 /* STM Clock Divider */
 /* Freq = [AHBCLK/(XCKDIV+1)*2] */
 typedef enum
 {
 	STM_CLKDIVBY02       = 0,    /* STM ABH clock divided by 02  = 0x000 */
 	STM_CLKDIVBY04       = 1,    /* STM ABH clock divided by 04  = 0x001 */
	STM_CLKDIVBY06       = 2,    /* STM ABH clock divided by 08  = 0x010 */
 	STM_CLKDIVBY08       = 3,    /* STM ABH clock divided by 08  = 0x011 */
	STM_CLKDIVBY10       = 4,    /* STM ABH clock divided by 08  = 0x100 */
	STM_CLKDIVBY12       = 5,    /* STM ABH clock divided by 08  = 0x101 */
	STM_CLKDIVBY14       = 6,    /* STM ABH clock divided by 08  = 0x110 */
 	STM_CLKDIVBY16       = 7,    /* STM ABH clock divided by 16  = 0x111 */
 } t_stm_xckdiv;
  


/* Errors related values */
typedef enum
{
    STM_OK                      = HCL_OK,
    STM_ERROR                   = HCL_ERROR,
    STM_UNSUPPORTED_HW          = HCL_UNSUPPORTED_HW,
    STM_UNSUPPORTED_FEATURE     = HCL_UNSUPPORTED_FEATURE,
    STM_INVALID_PARAMETER       = HCL_INVALID_PARAMETER,
	STM_INTERNAL_ERROR			= HCL_INTERNAL_ERROR
} t_stm_error;

typedef enum
{
	STM_CLK_LOW                     = 0x00,   /* Select the value of the clock as low */
	STM_CLK_HIGH                    = 0x01   /* Select the value of STM clock as high */
}t_stm_test_stmclk;

/* Integration test output data 3 line */
typedef enum
{
	STM_DATA3_LOW                   = 0x00,   /* Select the value of Data 3 line as low  */  
	STM_DATA3_HIGH                  = 0x01   /* Select the value of Data 3 line as high */  
}t_stm_test_dat3;

/* Integration test output data 2 line */
typedef enum
{
	STM_DATA2_LOW                   = 0x00,   /* Select the value of Data 2 line as low  */  
	STM_DATA2_HIGH                  = 0x01   /* Select the value of Data 2 line as high */  
}t_stm_test_dat2;

/* Integration test output data 1 line */
typedef enum
{
	STM_DATA1_LOW                   = 0x00,   /* Select the value of Data 1 line as low  */  
	STM_DATA1_HIGH                  = 0x01   /* Select the value of Data 1 line as high */  
}t_stm_test_dat1;

/* Integration test output data 0 line */
typedef enum
{
	STM_DATA0_LOW                   = 0x00,   /* Select the value of Data 0 line as low  */  
	STM_DATA0_HIGH                  = 0x01   /* Select the value of Data 0 line as high */  
}t_stm_test_dat0;


/* Priority Level Port 0 */
typedef enum
{
    STM_LOWEST_PRIORITY       = 0x00,
    STM_HIGHEST_PRIORITY      = 0x01
}t_stm_plp0;

/* Priority Level Port 1 */
typedef enum
{
    STM_LOWEST_PRIORITY_WITH_PLP0   = 0x00,
    STM_HIGHEST_PRIORITY_WITH_PLP0  = 0x01
}t_stm_plp1;

/* Downgradable STM Data Lines */
typedef enum
{
    STM_4_BIT_DATA_OUTPUT           = 0x00,
    STM_2_BIT_DATA_OUTPUT           = 0x01,
    STM_1_BIT_DATA_OUTPUT           = 0x02,
    STM_UNUSED                      = 0x03
}t_stm_dwng;

typedef enum
{
	STM_LSN_FIRST	= 0x0,
	STM_MSN_FIRST 	= 0x1
}t_stm_prcmu;
/* STM Datawidth [D8/D16/D32/D64] Selection */
typedef enum
{
    STM_DATA_WIDTH_8               = 0x00,
    STM_DATA_WIDTH_16              = 0x01,
    STM_DATA_WIDTH_32              = 0x02,
    STM_DATA_WIDTH_64              = 0x03
}t_stm_datawidth;

/* STM MIPI Modes Control Register */
typedef enum
{
    STM_SOFTWARE_MODE               = 0x00,
    STM_HARDWARE_MODE               = 0x01
}t_stm_mipi_mode;

/* STM Trace enable\disable */
typedef enum
{
    STM_TRACE_DISABLE               = 0x00,
    STM_TRACE_ENABLE                = 0x01
}t_stm_trace;


/* STM encoding types */
typedef enum
{
	eSTT_Printf            = STT_PRLG_UNF_PRINTF,
	eSTT_String            = STT_PRLG_STR,
    eSTT_Printf_Fmt        = STT_PRLG_STR,
	eSTT_Int8_n            = STT_PRLG_T_S16_8,
	eSTT_UInt8_n           = STT_PRLG_T_U16_8,
	eSTT_Int16_n           = STT_PRLG_T_S16,
	eSTT_UInt16_n          = STT_PRLG_T_U16,
	eSTT_Int24_n           = STT_PRLG_T_S32_24,
	eSTT_UInt24_n          = STT_PRLG_T_U32_24,
	eSTT_Int32_n           = STT_PRLG_T_S32,
	eSTT_UInt32_n          = STT_PRLG_T_U32,
	eSTT_Int56             = STT_PRLG_T_S32_S16_S16_8, 
	eSTT_UInt56            = STT_PRLG_T_U32_U16_U16_8, 
	eSTT_Int48_n           = STT_PRLG_T_S32_S16_8,
	eSTT_UInt48_n          = STT_PRLG_T_U32_U16_8,
	eSTT_Int48_n_bis       = STT_PRLG_T_S32_S16, 
	eSTT_UInt48_n_bis      = STT_PRLG_T_U32_U16, 

	eSTT_Saa_hcl           = 0x100,	
	eSTT_Binary           ,

	eUnsuported            =0x10000,

	eSTT_Char             ,
	eSTT_Int8             ,
	eSTT_UInt8            ,
	eSTT_Int16            ,
	eSTT_UInt16           ,
	eSTT_Int32            ,
	eSTT_UInt32           ,
	eSTT_Int48            ,
	eSTT_UInt48           ,
	eSTT_Int64            ,
	eSTT_UInt64           ,
	eSTT_Double           ,
	eSTT_Float            ,
	eSTT_Int64_n          ,
	eSTT_UInt64_n         ,
	eSTT_Double_n         ,
	eSTT_Float_n          ,

	eSTT_Bad_start_value  =0x10000,  /* BAD_START_VALUE           */ 
	eSTT_Betty_lost_data  =0x20000,  /* BETTY_LOST_DATA           */ 
}t_stm_encoding;


typedef struct
{
	t_stm_initiatormsg stm_initiator[2];
	t_stm_initiatormsg stm_initiator_timestamp[2];
}t_stm_initiator_init;

typedef struct
{
    t_stm_initiator_init  stm_initiator_init[256];
}t_stm_initiator;

/* Select between the two SXA */


typedef struct
{
	t_uint8      partnum0;           /* Part number 0 in periphiral id 0 */
	t_uint8      partnum1;           /* Part number 1 in periphiral id 1 */
	t_uint8      designer0;          /* Designer    0 in periphiral id 1 */
	t_uint8      designer1;          /* Designer    1 in periphiral id 2 */
	t_uint8      revision;           /* Revision      in periphiral id 2 */
	t_uint8      configuration;      /* Configuration in periphiral id 3 */
}t_stm_periphiral;

/* STM registers */
 
/* STM Configuration */
typedef struct
{
    t_stm_plp0      stm_plp0;           /* STM priority level 0 */
    t_stm_plp1      stm_plp1;           /* STM priority level 1 */
    t_stm_dwng      stm_dwng;           /* STM Downgradable data lines */
	t_stm_prcmu     stm_swap_prcmu;    /* STM priority level 1 */
}t_stm_configuration;

/* STM Master configuration */
typedef struct
{
    t_bool          stm_hwnsw0;         /* Hardware not Software Master 0 Configuration */ 
    t_bool          stm_hwnsw1;         /* Hardware not Software Master 1 Configuration */ 
    t_bool          stm_hwnsw2;         /* Hardware not Software Master 2 Configuration */ 
    t_bool          stm_hwnsw3;         /* Hardware not Software Master 3 Configuration */ 
	t_bool          stm_hwnsw4;         /* Hardware not Software Master 4 Configuration */ 
	t_bool          stm_hwnsw5;         /* Hardware not Software Master 5 Configuration */ 
}t_stm_mmc;

/* STM Trace Enable */
typedef struct
{
    t_bool          stm_te0;             /* Trace Enable Master 0 */
    t_bool          stm_te2;             /* Trace Enable Master 2 */
    t_bool          stm_te3;             /* Trace Enable Master 3 */
    t_bool          stm_te4;             /* Trace Enable Master 4 */
	t_bool          stm_te5;             /* Trace Enable Master 5 */
	t_bool          stm_te9;             /* Trace Enable Master 9 */
}t_stm_ter;

/* STM Trace Disable Status Register */
typedef struct
{
    t_bool          stm_td0;             /* Trace Disable Master 0 */
    t_bool          stm_td1;             /* Trace Disable Master 1 */
    t_bool          stm_td2;             /* Trace Disable Master 2 */
    t_bool          stm_td3;             /* Trace Disable Master 3 */
    t_bool          stm_td4;             /* Trace Disable Master 4 */
	t_bool          stm_td5;             /* Trace Disable Master 5 */
	t_bool          stm_td9;             /* Trace Disable Master 9 */
}t_stm_tdsr;

/* STM Overflow Status Register */
typedef struct
{
    t_bool          stm_ovf0;            /* Master 0 overflow status bit */ 
    t_bool          stm_ovf1;            /* Master 1 overflow status bit */
    t_bool          stm_ovf2;            /* Master 2 overflow status bit */
    t_bool          stm_ovf3;            /* Master 3 overflow status bit */
    t_bool          stm_ovf4;            /* Master 4 overflow status bit */
	t_bool          stm_ovf5;            /* Master 5 overflow status bit */
	t_bool          stm_ovf9;            /* Master 9 overflow status bit */
    t_bool          stm_secerr;          /* Secure Error status bit      */
    t_bool          stm_decerr;          /* Decode Error status bit      */
    t_uint8         stm_adderr;          /* Illegal address error        */
}t_stm_ofsr;

/* STM SBAG Status Register */
typedef struct
{
    t_bool          stm_sbfe;            /* SBAG FIFO Empty status bit */
    t_bool          stm_sbff;            /* SBAG FIFO Full status bit  */
    t_bool          stm_sbov;            /* SBAG Overflow flag         */
    t_uint32        stm_sbovn;           /* SBAG Overflow Number       */
    t_bool          stm_miscnt;          /* SBAG Miscount Status bit   */
    t_bool          stm_bytenb;          /* SBAG Byte Nb Status bit    */
}t_stm_sbsr;

/* STM Transmit FIFO Status Register */
typedef struct
{
    t_bool          stm_rame;            /* Status of RAM Empty */
    t_bool          stm_ramf;            /* Status of RAM Full  */
	t_bool          stm_prcmuf;            /* Status of PRCMU FIFO Full  */
	t_bool          stm_prcmue;            /* Status of PRCMU FIFO empty  */
    t_bool          stm_chgconf;         /* Change configuration flag */
}t_stm_tfsr;

/* STM Debug Register */
typedef struct
{
    t_bool          stm_dbgen;           /* Status of Debug Enable */
    t_bool          stm_dbgm;            /* Status of Debug operating mode */
    t_uint8         stm_pticssel;        /* Status of PTI Calibration Sequence Selection */
    t_uint8         stm_ftfsel;          /* Status of Force STP FIFO Full Selection */
    t_bool          stm_fsbff;           /* Status of Force SBAG FIFO Full Selectio */
    t_uint8         stm_ptistate;        /* Status of PTI State */
}t_stm_dbg;

/*--------------------------------------------------------------------------------------------------
 * Functions declaration
 *--------------------------------------------------------------------------------------------------*/

PUBLIC t_stm_error STM_Init(IN t_logical_address);
PUBLIC t_stm_error STM_GetVersion(OUT t_version *);
PUBLIC t_stm_error STM_SetDbgLevel(IN t_dbg_level);
PUBLIC void STM_SetBaseAddress(IN t_logical_address);
PUBLIC t_stm_error STM_ConfigureFrequency(IN t_stm_xckdiv); 
PUBLIC void STM_SetInitiatorBaseAddress(IN t_stm_initiator *p_stm_initiator);

PUBLIC t_stm_error STM_Configuration(IN t_stm_configuration);
PUBLIC t_stm_error STM_MIPIModeControl(IN t_stm_mmc);
PUBLIC t_stm_error STM_TraceEnable(IN t_stm_ter);
PUBLIC t_stm_error STM_GetTraceStatus(OUT t_stm_tdsr *);
PUBLIC t_stm_error STM_GetOverflowStatus(OUT t_stm_ofsr *);
PUBLIC t_stm_error STM_GetTransmitFIFOStatus(OUT t_stm_tfsr *);
PUBLIC t_stm_error STM_GetSBAGStatus(OUT t_stm_sbsr *);
PUBLIC t_stm_error STM_GetTimeStamp(t_uint32 *);
PUBLIC t_stm_error STM_SetDebug(IN t_stm_dbg);
PUBLIC t_stm_error STM_GetDebug(OUT t_stm_dbg *);

PUBLIC t_stm_error STM_GetSleepClockCounter(t_uint32 *);


PUBLIC t_stm_error STM_SendMessage8(IN t_uint32, IN t_uint8);
PUBLIC t_stm_error STM_SendMessage16(IN t_uint32,IN t_uint16);
PUBLIC t_stm_error STM_SendMessage32(IN t_uint32,IN t_uint32);
PUBLIC t_stm_error STM_SendMessage64(IN t_uint32,IN t_uint32 *);
PUBLIC t_stm_error STM_SendBlockMessage8(IN t_uint32, IN t_uint8 *,IN t_uint32);
PUBLIC t_stm_error STM_SendBlockMessage16(IN t_uint32,IN t_uint16 *,IN t_uint32);
PUBLIC t_stm_error STM_SendBlockMessage32(IN t_uint32,IN t_uint32 *,IN t_uint32);
PUBLIC t_stm_error STM_SendBlockMessage64(IN t_uint32,IN t_uint32 *,IN t_uint32);
PUBLIC t_stm_error STM_SendTimeStampMessage8(IN t_uint32,IN t_uint8);
PUBLIC t_stm_error STM_SendTimeStampMessage16(IN t_uint32,IN t_uint16);
PUBLIC t_stm_error STM_SendTimeStampMessage32(IN t_uint32,IN t_uint32);
PUBLIC t_stm_error STM_SendTimeStampMessage64(IN t_uint32,IN t_uint32 *);
PUBLIC t_stm_error STM_SendBlockTimeStampMessage8(IN t_uint32,IN t_uint8 *,IN t_uint32);
PUBLIC t_stm_error STM_SendBlockTimeStampMessage16(IN t_uint32,IN t_uint16 *,IN t_uint32);
PUBLIC t_stm_error STM_SendBlockTimeStampMessage32(IN t_uint32,IN t_uint32 *,IN t_uint32);
PUBLIC t_stm_error STM_SendBlockTimeStampMessage64(IN t_uint32,IN t_uint32 *,IN t_uint32);




#define STM_CMD_PRINTF(channel)                  STM_SendMessage8(channel,eSTT_Printf)
#define STM_CMD_END_OF_CHANNEL(channel)          STM_SendTimeStampMessage8(channel,0x20)

#define STM_CMD_PRINTF_FMT(channel)              STM_SendMessage8(channel, eSTT_Printf_Fmt) 
//#define STM_CMD_STRING(channel)                  STM_SendMessage8(channel,eSTT_String)
//#define STM_CMD_STRING(channel)                  STM_SendMessage8(224,0xC3)
#define STM_CMD_STRING(channel)                  STM_SendMessage8(224,0x20)
#define STM_CMD_RAW(channel)                     STM_SendMessage8(channel, eSTT_Binary)
#define STM_CMD_SAA_HCL(channel)                 STM_SendMessage8(channel, eSTT_Saa_hcl)
#define STM_CMD_SND32(channel)                   STM_SendMessage8(channel, eSTT_UInt32)


#ifdef __cplusplus
}   /* allow C++ to use these headers	*/
#endif /* __cplusplus */
#endif /* _STM_H_ */




