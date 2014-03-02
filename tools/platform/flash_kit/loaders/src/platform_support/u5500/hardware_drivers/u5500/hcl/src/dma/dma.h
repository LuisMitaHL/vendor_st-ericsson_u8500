/*
* --------------------------------------------------------------------------------------------
* STEricsson
* Reproduction and Communication of this document is strictly prohibited
* unless specifically authorized in writing by STEricsson
* --------------------------------------------------------------------------------------------
* File Name: dma.h
* This file holds function definitions and data structures exported by HCL functions.
*
* --------------------------------------------------------------------------------------------*/

#ifndef __DMA_HEADER
#define __DMA_HEADER

#ifdef    __cplusplus
extern "C" 
{                  /* To allow C++ to use this header */
#endif

#include "hcl_defs.h"
#include "debug.h"

/* -------- DMA HCL VERSION --------- */

#define DMA_HCL_VERSION_ID    1
#define DMA_HCL_MAJOR_ID      0
#define DMA_HCL_MINOR_ID      0


/* --------------------------- ENUMERATIONS & STRUCTURES ----------------------------------------*/

#define DMA_DIVISOR                 2
#define DMA_PIPE_DIVISOR            16
#define DMA_MAX_PIPE_ID             31
#define DMA_MASK_WORD               32
#define DMA_NULL_ADDRESS            0x00000000
#define DMA_LCSP                    0xFFFFFF00
#define DMA_SET_LOGICAL_TERM_CNT    0x00004000
#define DMA_SET_LOG_TERM_PARA       0xFFFFBFFF

/*------------------------ Common Definitions --------------------*/
/* Error definitons */

typedef enum 
{
    DMA_SRC_DEVICE_NOT_CONFIGURED           = (HCL_MAX_ERROR_VALUE - 20),
    DMA_DEST_DEVICE_NOT_CONFIGURED          = (HCL_MAX_ERROR_VALUE - 19),
    DMA_DEVICE_NOT_CONFIGURABLE             = (HCL_MAX_ERROR_VALUE - 18),
    DMA_NO_MORE_HW_CHANNELS_AVAILABLE       = (HCL_MAX_ERROR_VALUE - 17),
    DMA_MISALIGNED_ADDRESS                  = (HCL_MAX_ERROR_VALUE - 16),
    DMA_MEMORY_BUFFER_NOT_INITIALIZED       = (HCL_MAX_ERROR_VALUE - 15),
    DMA_NO_MORE_MEMORY_BUFFERS_AVAILABLE    = (HCL_MAX_ERROR_VALUE - 14),
    DMA_MEMORY_BUFFER_MAX_UPDATION_DONE     = (HCL_MAX_ERROR_VALUE - 13),
    DMA_MEMORY_BUFFER_ALREADY_FREED         = (HCL_MAX_ERROR_VALUE - 12),
    DMA_CHANNEL_UNAVAILABLE                 = (HCL_MAX_ERROR_VALUE - 11), 
    DMA_PIPE_NOT_OPENED                     = (HCL_MAX_ERROR_VALUE - 10),
    DMA_UNKNOWN_CHAN_ID                     = (HCL_MAX_ERROR_VALUE - 9),
    DMA_UNSUPPORTED_DEVICE                  = (HCL_MAX_ERROR_VALUE - 8),
    DMA_NO_MEMORY_PIPES_AVAILABLE           = (HCL_MAX_ERROR_VALUE - 7),
    DMA_NO_MORE_LLIS_AVAILABLE              = (HCL_MAX_ERROR_VALUE - 6),
    DMA_INCOMPATIBLE_TRANSFER_SIZE          = (HCL_MAX_ERROR_VALUE - 5),
    DMA_UNSUPPORTED_TRANSFER                = (HCL_MAX_ERROR_VALUE - 4), 
    DMA_DEVICE_ALREADY_USED                 = (HCL_MAX_ERROR_VALUE - 3), 
    DMA_INCOMPATIBLE_CONFIGURATION_SIZE     = (HCL_MAX_ERROR_VALUE - 2),
    DMA_MISSING_CONFIGURATION_PARAMETER     = (HCL_MAX_ERROR_VALUE - 1),
    DMA_UNKNOWN_DEVICE                      = HCL_MAX_ERROR_VALUE,
    DMA_INTERNAL_ERROR                      = HCL_INTERNAL_ERROR,        
    DMA_NOT_CONFIGURED                      = HCL_NOT_CONFIGURED,        
    DMA_REQUEST_PENDING                     = HCL_REQUEST_PENDING,       
    DMA_REQUEST_NOT_APPLICABLE              = HCL_REQUEST_NOT_APPLICABLE,
    DMA_INVALID_PARAMETER                   = HCL_INVALID_PARAMETER,    /* Invalid parameter across all platforms */    
    DMA_UNSUPPORTED_FEATURE                 = HCL_UNSUPPORTED_FEATURE,  /* Invalid parameter for specific platforms */
    DMA_UNSUPPORTED_HW                      = HCL_UNSUPPORTED_HW,       /* From DMA_Init if the peripheral ids do not match */
    DMA_ERROR                               = HCL_ERROR,                 
    DMA_OK                                  = HCL_OK,                   /* No error */    
    DMA_INTERNAL_EVENT                      = HCL_INTERNAL_EVENT,
    DMA_REMAINING_PENDING_EVENTS            = HCL_REMAINING_PENDING_EVENTS,
    DMA_REMAINING_FILTER_PENDING_EVENTS     = HCL_REMAINING_FILTER_PENDING_EVENTS,
    DMA_NO_MORE_PENDING_EVENT               = HCL_NO_MORE_PENDING_EVENT,
    DMA_NO_MORE_FILTER_PENDING_EVENT        = HCL_NO_MORE_FILTER_PENDING_EVENT,
    DMA_NO_PENDING_EVENT_ERROR              = HCL_NO_PENDING_EVENT_ERROR
}t_dma_error;

typedef t_uint8 t_dma_evt_grp_num;
typedef t_uint8 t_dma_chan_num;
typedef t_uint8 t_dma_chan_index;
typedef t_uint8 t_dma_pipe_num;
typedef t_uint8 t_dma_event_num;
typedef t_uint8 t_dma_device_num;
typedef t_uint8 t_dma_irq_num;

/* Channel Definitions */
typedef enum
{
    DMA_CHAN_0,             
    DMA_CHAN_1,
    DMA_CHAN_2,             
    DMA_CHAN_3,         
    DMA_CHAN_4,             
    DMA_CHAN_5,
    DMA_CHAN_6,             
    DMA_CHAN_7
}t_dma_chan_id;

/* enum for the master port*/
typedef enum
{
    DMA_MASTER_0 ,
    DMA_MASTER_1         
}t_dma_master;

/* enum to configure physical channel in standard or extended mode  */
typedef enum
{
    DMA_STANDARD                =         0x1,
    DMA_EXTENDED                =         0x2       
}t_dma_channel_type;


/* enum to configure physical channel in secure  or non-secure mode  */
typedef enum
{
    DMA_SECURE                  =         0x1,
    DMA_NONSECURE               =         0x2     
}t_dma_channel_security;

typedef enum
{
    DMA_PHYSICAL                =         0x1,
    DMA_LOGICAL                 =         0x2,
    DMA_OPERATION               =         0x3
}t_dma_channel_mode;

typedef enum
{
    DMA_CHAN_PHYSICAL_NOT_SECURE,
    DMA_CHAN_LOGICAL_NOT_SECURE,
    DMA_CHAN_PHYSICAL_SECURE,
    DMA_CHAN_LOGICAL_SECURE    
}t_dma_chan_type;

typedef enum                
{
    DMA_BASIC                    =         0x1, 
    DMA_MODULO                   =         0x2, 
    DMA_DOUBLE_DEST              =         0x3, 
    DMA_SRC_PHY_DEST_LOG         =         0x1, 
    DMA_SRC_LOG_DEST_PHY         =         0x2, 
    DMA_SRC_LOG_DEST_LOG         =         0x3  
}t_dma_channel_option;                            

typedef    enum
{
    DMA_EVENT_GROUP_0,
    DMA_EVENT_GROUP_1,
    DMA_EVENT_GROUP_2,
    DMA_EVENT_GROUP_3,
    DMA_NO_EVENT_GROUP
}t_dma_event_group;

/* enum to check if the event type is src or destination*/
typedef enum
{
    DMA_SRC_EVENT,
    DMA_DEST_EVENT
}t_dma_event_type;

/* enum to set the data width of the transfer*/
typedef enum 
{
    DMA_BYTE_WIDTH,        
    DMA_HALFWORD_WIDTH,        
    DMA_WORD_WIDTH,        
    DMA_DOUBLEWORD_WIDTH    
} t_dma_data_width;


/* enum to set the burst size of the transfer*/
typedef enum                        
{
    DMA_BURST_SIZE_1,         
    DMA_BURST_SIZE_4,        
    DMA_BURST_SIZE_8,        
    DMA_BURST_SIZE_16,        
    DMA_NO_BURST
}t_dma_burst_size;

typedef enum                            
{
    DMA_PRI_LOW,
    DMA_PRI_HIGH    
}t_dma_priority;

typedef enum                            
{
    DMA_NO_SYNC,
    DMA_PACKET_SYNC,
    DMA_FRAME_SYNC,
    DMA_BLOCK_SYNC
}t_dma_tx_mode;


/* enum to set the transfer as little endian or big endian*/
typedef enum 
{
    DMA_LITTLE,            
    DMA_BIG                 
}t_dma_endian;  


/* enum for prelink and post link operations */
typedef enum 
{
    DMA_POST_LINK,
    DMA_PRE_LINK          
}t_dma_relink_ctrl;  

typedef enum                                    /* Source or Destinaton half-channel    :    SD */
{
    DMA_SOURCE_HALF_CHANNEL,
    DMA_DEST_HALF_CHANNEL 
}t_dma_half_chan;

typedef enum                                    /* Source or Destinaton half-channel    :    SD */
{
    DMA_STOP,
    DMA_RUN,
    DMA_SUSPEND
}t_dma_active_status;

typedef enum
{
    DMA_MEM_TO_MEM,
    DMA_MEM_TO_PER,
    DMA_PER_TO_MEM,
    DMA_PER_TO_PER
}t_dma_transfer_type;

typedef enum
{
    DMA_EOT,
    DMA_RELINK
}t_dma_intr_gen;

typedef enum                            /* Event Enable (Logical Channel) */
{
    DMA_EVT_STATE_DISABLE,
    DMA_EVT_STATE_ENABLE,     
    DMA_EVT_STATE_ROUND     
}t_dma_event_state;

typedef enum  
{
    DMA_NOT_INIT_EXCHANGE, 
    DMA_ONGOING_EXCHANGE,
    DMA_SUSPENDED_EXCHANGE, 
    DMA_ERROR_ENDED_EXCHANGE,
    DMA_TC_ENDED_EXCHANGE,
    DMA_HALTED_EXCHANGE, 
    DMA_ABORTED_EXCHANGE 
} t_dma_exchange_state; 

typedef enum
{
    DMA_INIT_LLI_STATE,
    DMA_RELINK_LLI_STATE,
    DMA_APPEND_LLI_STATE,
    DMA_QUEUE_LLI_STATE,
    DMA_FREEZE_LLI_STATE
}t_dma_lli_state;

/* enum for source device types*/

typedef enum 
{
    DMA_SPI0_Rx_DEVICE                  =    0,
    DMA_SPI1_Rx_DEVICE			    =    1,
    DMA_SPI2_Rx_DEVICE	                =    2,
    DMA_SPI3_Rx_DEVICE                  =    3,
    DMA_USB_OTG_IEP1_DEVICE             =    4,
    DMA_USB_OTG_IEP9_DEVICE             =    DMA_USB_OTG_IEP1_DEVICE,
    DMA_USB_OTG_IEP2_DEVICE             =    5,
    DMA_USB_OTG_IEP10_DEVICE            =    DMA_USB_OTG_IEP2_DEVICE,
    DMA_USB_OTG_IEP3_DEVICE             =    6,
    DMA_USB_OTG_IEP11_DEVICE 	          =    DMA_USB_OTG_IEP3_DEVICE, 
    DMA_IRDA_RFS_Rx_DEVICE              =    7,
    DMA_IRDA_FIFO_Rx_DEVICE             =    8,
    DMA_MSP0_Rx_DEVICE                  =    9,
    DMA_MSP1_Rx_DEVICE                  =    10,
    DMA_MSP2_Rx_DEVICE                  =    11,
    DMA_UART0_Rx_DEVICE                 =    12,
    DMA_UART1_Rx_DEVICE                 =    13,
    DMA_UART2_Rx_DEVICE                 =    14,
    DMA_UART3_Rx_DEVICE                 =    15,
    DMA_USB_OTG_IEP8_DEVICE             =    16,    
    DMA_USB_OTG_IEP1_1_DEVICE             =    17,
    DMA_USB_OTG_IEP9_1_DEVICE             =    DMA_USB_OTG_IEP1_DEVICE,    
    DMA_USB_OTG_IEP2_1_DEVICE             =    18,
    DMA_USB_OTG_IEP10_1_DEVICE            =    DMA_USB_OTG_IEP2_DEVICE,
    DMA_USB_OTG_IEP3_1_DEVICE             =    19,
    DMA_USB_OTG_IEP11_1_DEVICE            =    DMA_USB_OTG_IEP3_DEVICE,
    DMA_USB_OTG_IEP4_DEVICE             =    20,
    DMA_USB_OTG_IEP12_DEVICE            =    DMA_USB_OTG_IEP4_DEVICE,
    DMA_USB_OTG_IEP5_DEVICE             =    21,
    DMA_USB_OTG_IEP13_DEVICE            =    DMA_USB_OTG_IEP5_DEVICE,
    DMA_USB_OTG_IEP6_DEVICE             =    22,
    DMA_USB_OTG_IEP14_DEVICE            =    DMA_USB_OTG_IEP6_DEVICE,
    DMA_USB_OTG_IEP7_DEVICE             =    23,
    DMA_USB_OTG_IEP15_DEVICE            =    DMA_USB_OTG_IEP7_DEVICE,
    DMA_SD_MMC0_Rx_DEVICE               =    24,
    DMA_SD_MMC1_Rx_DEVICE               =    25,
    DMA_SD_MMC2_Rx_DEVICE               =    26,
    DMA_SD_MMC3_Rx_DEVICE               =    27,
    DMA_SD_MMC4_Rx_DEVICE               =    28,
    DMA_RESERVED29_Rx_DEVICE            =    29,
    DMA_RESERVED30_Rx_DEVICE            =    30,
    DMA_RESERVED31_Rx_DEVICE            =    31,
    DMA_RESERVED32_Rx_DEVICE            =    32,
    DMA_SD_MMC0_Rx_1_DEVICE               =    33,
    DMA_SD_MMC1_Rx_1_DEVICE               =    34,
    DMA_SD_MMC2_Rx_1_DEVICE               =    35,
    DMA_SD_MMC3_Rx_1_DEVICE               =    36,
    DMA_SD_MMC4_Rx_1_DEVICE               =    37,
    DMA_USB_OTG_IEP8_1_DEVICE             =    38,    
    DMA_USB_OTG_IEP1_2_DEVICE             =    39,
    DMA_USB_OTG_IEP9_2_DEVICE             =    DMA_USB_OTG_IEP1_DEVICE,    
    DMA_USB_OTG_IEP2_2_DEVICE             =    40,
    DMA_USB_OTG_IEP10_2_DEVICE            =    DMA_USB_OTG_IEP2_DEVICE,
    DMA_USB_OTG_IEP3_2_DEVICE             =    41,
    DMA_USB_OTG_IEP11_2_DEVICE            =    DMA_USB_OTG_IEP3_DEVICE,
    DMA_USB_OTG_IEP4_1_DEVICE             =    42,
    DMA_USB_OTG_IEP12_1_DEVICE            =    DMA_USB_OTG_IEP4_DEVICE,
    DMA_USB_OTG_IEP5_1_DEVICE             =    43,
    DMA_USB_OTG_IEP13_1_DEVICE            =    DMA_USB_OTG_IEP5_DEVICE,
    DMA_USB_OTG_IEP6_1_DEVICE             =    44,
    DMA_USB_OTG_IEP14_1_DEVICE            =    DMA_USB_OTG_IEP6_DEVICE,
    DMA_USB_OTG_IEP7_1_DEVICE             =    45,
    DMA_USB_OTG_IEP15_1_DEVICE            =    DMA_USB_OTG_IEP7_DEVICE,
    DMA_RESERVED46_Rx_DEVICE            =    46,
    DMA_MCDE_Rx_DEVICE                  =    47,
    DMA_CRYPTO1_Rx_DEVICE               =    48,
    DMA_RESERVED49_Rx_DEVICE            =    49,
    DMA_RESERVED50_Rx_DEVICE            =    50,
    DMA_I2C1_Rx_DEVICE                  =    51,
    DMA_I2C3_Rx_DEVICE                  =    52,
    DMA_I2C2_Rx_DEVICE                  =    53,

    DMA_RESERVED54_Rx_DEVICE            =    54,
    DMA_RESERVED55_Rx_DEVICE            =    55,
    DMA_RESERVED56_Rx_DEVICE            =    56,
    DMA_RESERVED57_Rx_DEVICE            =    57,

    DMA_RESERVED58_Rx_DEVICE            =    58,
    DMA_RESERVED59_Rx_DEVICE            =    59,
    DMA_RESERVED60_Rx_DEVICE            =    60,

    DMA_CRYPTO0_Rx_DEVICE               =    61,
    DMA_RESERVED62_Rx_DEVICE            =    62,
    DMA_RESERVED63_Rx_DEVICE            =    63,
    DMA_MEMORY_SRC			    =    64	

}t_dma_src_device;


/* enum for destination device types*/

typedef enum 
{
    DMA_SPI0_Tx_DEVICE                  =    0,
    DMA_SPI1_Tx_DEVICE			    =    1,
    DMA_SPI2_Tx_DEVICE	                =    2,
    DMA_SPI3_Tx_DEVICE                  =    3,
    DMA_USB_OTG_OEP1_DEVICE             =    4,
    DMA_USB_OTG_OEP9_DEVICE             =    DMA_USB_OTG_OEP1_DEVICE,
    DMA_USB_OTG_OEP2_DEVICE             =    5,
    DMA_USB_OTG_OEP10_DEVICE            =    DMA_USB_OTG_OEP2_DEVICE,
    DMA_USB_OTG_OEP3_DEVICE             =    6,
    DMA_USB_OTG_OEP11_DEVICE 	          =    DMA_USB_OTG_OEP3_DEVICE, 
    DMA_IRRC_Tx_DEVICE                  =    7,
    DMA_IRDA_FIFO_Tx_DEVICE             =    8,
    DMA_MSP0_Tx_DEVICE                  =    9,
    DMA_MSP1_Tx_DEVICE                  =    10,
    DMA_MSP2_Tx_DEVICE                  =    11,
    DMA_UART0_Tx_DEVICE                 =    12,
    DMA_UART1_Tx_DEVICE                 =    13,
    DMA_UART2_Tx_DEVICE                 =    14,
    DMA_UART3_Tx_DEVICE                 =    15,
    DMA_USB_OTG_OEP8_DEVICE             =    16,    
    DMA_USB_OTG_OEP1_1_DEVICE             =    17,
    DMA_USB_OTG_OEP9_1_DEVICE             =    DMA_USB_OTG_OEP1_DEVICE,    
    DMA_USB_OTG_OEP2_1_DEVICE             =    18,
    DMA_USB_OTG_OEP10_1_DEVICE            =    DMA_USB_OTG_OEP2_DEVICE,
    DMA_USB_OTG_OEP3_1_DEVICE             =    19,
    DMA_USB_OTG_OEP11_1_DEVICE            =    DMA_USB_OTG_OEP3_DEVICE,
    DMA_USB_OTG_OEP4_DEVICE             =    20,
    DMA_USB_OTG_OEP12_DEVICE            =    DMA_USB_OTG_OEP4_DEVICE,
    DMA_USB_OTG_OEP5_DEVICE             =    21,
    DMA_USB_OTG_OEP13_DEVICE            =    DMA_USB_OTG_OEP5_DEVICE,
    DMA_USB_OTG_OEP6_DEVICE             =    22,
    DMA_USB_OTG_OEP14_DEVICE            =    DMA_USB_OTG_OEP6_DEVICE,
    DMA_USB_OTG_OEP7_DEVICE             =    23,
    DMA_USB_OTG_OEP15_DEVICE            =    DMA_USB_OTG_OEP7_DEVICE,
    DMA_SD_MMC0_Tx_DEVICE               =    24,
    DMA_SD_MMC1_Tx_DEVICE               =    25,
    DMA_SD_MMC2_Tx_DEVICE               =    26,
    DMA_SD_MMC3_Tx_DEVICE               =    27,
    DMA_SD_MMC4_Tx_DEVICE               =    28,
    DMA_RESERVED29_Tx_DEVICE            =    29,
    DMA_RESERVED30_Tx_DEVICE            =    30,
    DMA_RESERVED31_Tx_DEVICE            =    31,
    DMA_FSMC_Tx_DEVICE                  =    32,
    DMA_SD_MMC0_Tx_1_DEVICE               =    33,
    DMA_SD_MMC1_Tx_1_DEVICE               =    34,
    DMA_SD_MMC2_Tx_1_DEVICE               =    35,
    DMA_SD_MMC3_Tx_1_DEVICE               =    36,
    DMA_SD_MMC4_Tx_1_DEVICE               =    37,
    DMA_USB_OTG_OEP8_1_DEVICE             =    38,    
    DMA_USB_OTG_OEP1_2_DEVICE             =    39,
    DMA_USB_OTG_OEP9_2_DEVICE             =    DMA_USB_OTG_OEP1_DEVICE,    
    DMA_USB_OTG_OEP2_2_DEVICE             =    40,
    DMA_USB_OTG_OEP10_2_DEVICE            =    DMA_USB_OTG_OEP2_DEVICE,
    DMA_USB_OTG_OEP3_2_DEVICE             =    41,
    DMA_USB_OTG_OEP11_2_DEVICE            =    DMA_USB_OTG_OEP3_DEVICE,
    DMA_USB_OTG_OEP4_1_DEVICE             =    42,
    DMA_USB_OTG_OEP12_1_DEVICE            =    DMA_USB_OTG_OEP4_DEVICE,
    DMA_USB_OTG_OEP5_1_DEVICE             =    43,
    DMA_USB_OTG_OEP13_1_DEVICE            =    DMA_USB_OTG_OEP5_DEVICE,
    DMA_USB_OTG_OEP6_1_DEVICE             =    44,
    DMA_USB_OTG_OEP14_1_DEVICE            =    DMA_USB_OTG_OEP6_DEVICE,
    DMA_USB_OTG_OEP7_1_DEVICE             =    45,
    DMA_USB_OTG_OEP15_1_DEVICE            =    DMA_USB_OTG_OEP7_DEVICE,
    DMA_RESERVED46_Tx_DEVICE            =    46,
    DMA_STM_Tx_DEVICE                   =    47,
    DMA_CRYPTO1_Tx_DEVICE               =    48,
    DMA_CRYP_HASH1_Tx_DEVICE             =    49,
    DMA_HASH1_Tx_DEVICE                 =    50,

    DMA_I2C1_Tx_DEVICE                  =    51,
    DMA_I2C3_Tx_DEVICE                  =    52,
    DMA_I2C2_Tx_DEVICE                  =    53,

    DMA_RESERVED54_Tx_DEVICE            =    54,
    DMA_RESERVED55_Tx_DEVICE            =    55,

    DMA_MEM_TRANSFER1_Tx_DEVICE         =    56,
    DMA_MEM_TRANSFER2_Tx_DEVICE         =    57,
    DMA_MEM_TRANSFER3_Tx_DEVICE         =    58,
    DMA_MEM_TRANSFER4_Tx_DEVICE         =    59,
    DMA_MEM_TRANSFER5_Tx_DEVICE         =    60,

    DMA_CRYPTO0_Tx_DEVICE               =    61,
    DMA_CRYP_HASH0_Tx_DEVICE            =    62,
    DMA_HASH0_Tx_DEVICE                 =    63,
    DMA_MEMORY_DEST                     =    64

}t_dma_dest_device;

typedef struct
{
   t_dma_chan_num            chan_num;
   t_dma_channel_type        type;
   t_dma_channel_security    security;
   t_dma_channel_mode        mode;
   t_dma_channel_option      option;
}t_dma_channel_config;

typedef struct
{
    t_dma_chan_num      chan_num; 
    t_dma_pipe_num      src_pipe_id;
    t_dma_pipe_num      dest_pipe_id;
	t_dma_irq_num       irq_num;
}t_dma_pipe_config;

typedef struct 
{
    t_uint32 ChanConfig;
    t_uint32 ChanElement;
    t_uint32 ChanPointer;
    t_uint32 ChanLink;
}t_dma_relink_desc;

/* Logical Standard Channel Parameters */

typedef struct
{
    t_uint32 dmac_lcsp0;
    t_uint32 dmac_lcsp1;
    t_uint32 dmac_lcsp2;
    t_uint32 dmac_lcsp3;
}t_std_log_memory_param;

typedef struct
{
    t_uint32 dmac_lcsp0;
    t_uint32 dmac_lcsp1;
}t_std_src_log_memory_param;

typedef struct
{
    t_uint32 dmac_lcsp2;
    t_uint32 dmac_lcsp3;
}t_std_dest_log_memory_param;

/* Interrupt related enums*/
typedef struct
{
    t_bool tc_intr;
    t_bool err_intr;    
}t_dma_irq_status;

typedef struct
{
    t_dma_chan_type  channel_type;
    t_dma_irq_num    irq_num;
}t_dma_event_desc;

/* Define the filter to selectively choose the interrupt events for processing */
typedef t_uint32 t_dma_filter_mode;


typedef enum 
{
	DMA_SPI0_Tx_LINE                  = 0,
	DMA_SPI0_Rx_LINE                = 1,
		    
	DMA_SPI1_Tx_LINE	              = 2,
	DMA_SPI1_Rx_LINE	              = 3,
	DMA_SPI2_Tx_LINE	              = 4,
	DMA_SPI2_Rx_LINE				  = 5,
 	DMA_SPI3_Tx_LINE                = 6,
	DMA_SPI3_Rx_LINE                = 7,
	DMA_USB_OTG_OEP1_LINE           = 8,
	DMA_USB_OTG_IEP1_LINE           = 9,
    DMA_USB_OTG_OEP9_LINE           =  DMA_USB_OTG_OEP1_LINE,
	DMA_USB_OTG_IEP9_LINE           =  DMA_USB_OTG_IEP1_LINE,
	DMA_USB_OTG_OEP2_LINE           = 10,
	DMA_USB_OTG_IEP2_LINE           = 11,

    DMA_USB_OTG_OEP10_LINE          =  DMA_USB_OTG_OEP2_LINE,
	DMA_USB_OTG_IEP10_LINE          =  DMA_USB_OTG_IEP2_LINE,
	

	
	DMA_USB_OTG_OEP3_LINE           = 12,
	DMA_USB_OTG_IEP3_LINE           = 13,
    DMA_USB_OTG_OEP11_LINE 	      =    DMA_USB_OTG_OEP3_LINE, 
	DMA_USB_OTG_IEP11_LINE 	      =    DMA_USB_OTG_IEP3_LINE, 
	DMA_IRRC_Tx_LINE                = 14,
	DMA_IRDA_RFS_Rx_LINE            = 15,  
	DMA_IRDA_FIFO_Tx_LINE           = 16,    
	DMA_IRDA_FIFO_Rx_LINE           = 17,
	DMA_MSP0_Tx_LINE                = 18,
	DMA_MSP0_Rx_LINE                = 19,  
	DMA_MSP1_Tx_LINE                = 20, 
	DMA_MSP1_Rx_LINE                = 21,  
	DMA_MSP2_Tx_LINE                = 22,
	DMA_MSP2_Rx_LINE                = 23,  
	DMA_UART0_Tx_LINE               = 24,
	DMA_UART0_Rx_LINE               = 25,  
	DMA_UART1_Tx_LINE               = 26,
	DMA_UART1_Rx_LINE               = 27,  
    DMA_UART2_Tx_LINE               = 28,
	DMA_UART2_Rx_LINE               = 29,  
    DMA_UART3_Tx_LINE               = 30,
	DMA_UART3_Rx_LINE               = 31,  
    DMA_USB_OTG_OEP8_LINE           = 32,
	DMA_USB_OTG_IEP8_LINE           = 33,  
    DMA_USB_OTG_OEP1_1_LINE           = 34,
    DMA_USB_OTG_IEP1_1_LINE           = 35,  
    DMA_USB_OTG_OEP9_1_LINE             =    DMA_USB_OTG_OEP1_LINE,    
    DMA_USB_OTG_IEP9_1_LINE             =    DMA_USB_OTG_IEP1_LINE,    
       
    DMA_USB_OTG_OEP2_1_LINE           = 36,
    DMA_USB_OTG_IEP2_1_LINE           = 37,  
   DMA_USB_OTG_OEP10_1_LINE            =    DMA_USB_OTG_OEP2_LINE,
    DMA_USB_OTG_IEP10_1_LINE           =    DMA_USB_OTG_IEP2_LINE,
       
	DMA_USB_OTG_OEP3_1_LINE            = 38,
	DMA_USB_OTG_IEP3_1_LINE            = 39, 
	DMA_USB_OTG_OEP11_1_LINE           =    DMA_USB_OTG_OEP3_LINE,
	DMA_USB_OTG_IEP11_1_LINE           =    DMA_USB_OTG_IEP3_LINE,
    DMA_USB_OTG_OEP4_LINE            = 40,
	DMA_USB_OTG_IEP4_LINE            = 41, 
  DMA_USB_OTG_OEP12_LINE             =    DMA_USB_OTG_OEP4_LINE,
	DMA_USB_OTG_IEP12_LINE           =    DMA_USB_OTG_IEP4_LINE,
	DMA_USB_OTG_OEP5_LINE            = 42,
	DMA_USB_OTG_IEP5_LINE            = 43,
 	DMA_USB_OTG_OEP13_LINE            =    DMA_USB_OTG_OEP5_LINE,
  	DMA_USB_OTG_IEP13_LINE            =    DMA_USB_OTG_IEP5_LINE,
    
    DMA_USB_OTG_OEP6_LINE            = 44, 
    DMA_USB_OTG_IEP6_LINE            = 45,
  DMA_USB_OTG_OEP14_LINE             =    DMA_USB_OTG_OEP6_LINE,
   DMA_USB_OTG_IEP14_LINE            =    DMA_USB_OTG_IEP6_LINE,
  
    DMA_USB_OTG_OEP7_LINE            = 46,
    DMA_USB_OTG_IEP7_LINE            = 47, 
    DMA_USB_OTG_OEP15_LINE            =    DMA_USB_OTG_OEP7_LINE,
    DMA_USB_OTG_IEP15_LINE            =    DMA_USB_OTG_IEP7_LINE,
    DMA_SD_MMC0_Tx_LINE               =   48,
    DMA_SD_MMC0_Rx_LINE               =  49,
    DMA_SD_MMC1_Tx_LINE               =  50,
    DMA_SD_MMC1_Rx_LINE               =  51,
    DMA_SD_MMC2_Tx_LINE               =  52,
    DMA_SD_MMC2_Rx_LINE               =  53,
    DMA_SD_MMC3_Tx_LINE               =  54,
    DMA_SD_MMC3_Rx_LINE               =  55,
    DMA_SD_MMC4_Tx_LINE               =  56,
    DMA_SD_MMC4_Rx_LINE               =  57,
    DMA_RESERVED29_Tx_LINE            =  58,
    DMA_RESERVED29_Rx_LINE            =  59,
    DMA_RESERVED30_Tx_LINE            =  60,
    DMA_RESERVED30_Rx_LINE            =  61, 
    DMA_RESERVED31_Tx_LINE            =  62,
    DMA_RESERVED31_Rx_LINE            =  63,
    DMA_FSMC_Tx_LINE                  =  64,
    DMA_RESERVED32_Rx_LINE            =  65,
    DMA_SD_MMC0_Tx_1_LINE               =  66,
    DMA_SD_MMC0_Rx_1_LINE               =  67,
    DMA_SD_MMC1_Tx_1_LINE               =  68,
    DMA_SD_MMC1_Rx_1_LINE               =  69,
    DMA_SD_MMC2_Tx_1_LINE               =  70,
    DMA_SD_MMC2_Rx_1_LINE               =  71,
    DMA_SD_MMC3_Tx_1_LINE               =  72,
    DMA_SD_MMC3_Rx_1_LINE               =  73,
    DMA_SD_MMC4_Tx_1_LINE               =  74,
    DMA_SD_MMC4_Rx_1_LINE               =  75,
    DMA_USB_OTG_OEP8_1_LINE             =  76,    
    DMA_USB_OTG_IEP8_1_LINE             =  77,
    DMA_USB_OTG_OEP1_2_LINE             =  78,
    DMA_USB_OTG_IEP1_2_LINE             =  79,
    DMA_USB_OTG_OEP9_2_LINE             =   DMA_USB_OTG_OEP1_LINE,    
    DMA_USB_OTG_IEP9_2_LINE             =   DMA_USB_OTG_IEP1_LINE,
    DMA_USB_OTG_OEP2_2_LINE             =  80,
    DMA_USB_OTG_IEP2_2_LINE             =  81,
    DMA_USB_OTG_OEP10_2_LINE            =    DMA_USB_OTG_OEP2_LINE,
    DMA_USB_OTG_IEP10_2_LINE            =    DMA_USB_OTG_IEP2_LINE,
    DMA_USB_OTG_OEP3_2_LINE             =  82,
    DMA_USB_OTG_IEP3_2_LINE             =  83,
    DMA_USB_OTG_OEP11_2_LINE            =    DMA_USB_OTG_OEP3_LINE,
    DMA_USB_OTG_IEP11_2_LINE            =    DMA_USB_OTG_IEP3_LINE,
    DMA_USB_OTG_OEP4_1_LINE             =  84,
    DMA_USB_OTG_IEP4_1_LINE             =  85,
    DMA_USB_OTG_OEP12_1_LINE            =    DMA_USB_OTG_OEP4_LINE,
     DMA_USB_OTG_IEP12_1_LINE           =    DMA_USB_OTG_IEP4_LINE,
    DMA_USB_OTG_OEP5_1_LINE             =   86,
    DMA_USB_OTG_IEP5_1_LINE             =   87,
    DMA_USB_OTG_OEP13_1_LINE            =    DMA_USB_OTG_OEP5_LINE,
    DMA_USB_OTG_IEP13_1_LINE            =    DMA_USB_OTG_IEP5_LINE,
    DMA_USB_OTG_OEP6_1_LINE             =   88,
    DMA_USB_OTG_IEP6_1_LINE             =   89,
    DMA_USB_OTG_OEP14_1_LINE            =    DMA_USB_OTG_OEP6_LINE,
    DMA_USB_OTG_IEP14_1_LINE            =    DMA_USB_OTG_IEP6_LINE,
    DMA_USB_OTG_OEP7_1_LINE             =   90,
    DMA_USB_OTG_IEP7_1_LINE             =   91,
    DMA_USB_OTG_OEP15_1_LINE            =    DMA_USB_OTG_OEP7_LINE,
    DMA_USB_OTG_IEP15_1_LINE            =    DMA_USB_OTG_IEP7_LINE,
    DMA_RESERVED46_Tx_LINE            =   92,
    DMA_RESERVED46_Rx_LINE            =   93,
    DMA_STM_Tx_LINE                   =   94,
    DMA_MCDE_Rx_LINE                  =   95,
    DMA_CRYPTO1_Tx_LINE               =   96,
    DMA_CRYPTO1_Rx_LINE               =   97,
    DMA_CRYP_HASH1_Tx_LINE            =   98,
    DMA_RESERVED49_Rx_LINE            =   99,
    DMA_HASH1_Tx_LINE                 =   100,
    DMA_RESERVED50_Rx_LINE            =   101,


    DMA_I2C1_Tx_LINE                  =   102,
    DMA_I2C1_Rx_LINE                  =   103,
    DMA_I2C3_Tx_LINE                  =   104,
    DMA_I2C3_Rx_LINE                  =   105,
    DMA_I2C2_Tx_LINE                  =   106,
    DMA_I2C2_Rx_LINE                  =   107,


    DMA_RESERVED54_Tx_LINE            =   108,
    DMA_RESERVED54_Rx_LINE            =   109,
    DMA_RESERVED55_Tx_LINE            =   110,
	DMA_RESERVED55_Rx_LINE            =   111,
    DMA_MEM_TRANSFER1_Tx_LINE         =   112,
    DMA_RESERVED56_Rx_LINE            =   113,
    DMA_MEM_TRANSFER2_Tx_LINE         =   114,
    DMA_RESERVED57_Rx_LINE            =   115,
    DMA_MEM_TRANSFER3_Tx_LINE         =   116,
    DMA_RESERVED58_Rx_LINE            =   117,
    DMA_MEM_TRANSFER4_Tx_LINE         =   118,
    DMA_RESERVED59_Rx_LINE            =   119,
    DMA_MEM_TRANSFER5_Tx_LINE         =   120,
    DMA_RESERVED60_Rx_LINE            =   121,
               

    DMA_CRYPTO0_Tx_LINE               =    122,
    DMA_CRYPTO0_Rx_LINE               =   123, 
    DMA_CRYP_HASH0_Tx_LINE            =   124,
    DMA_RESERVED62_Rx_LINE               =   125,
    DMA_HASH0_Tx_LINE                 =   126,
    DMA_RESERVED63_Rx_LINE            =   127,


	DMA_MEMORY_SRC_LINE                 =	 128,	
	
    DMA_MEMORY_DEST_LINE                     =    129
    
}t_dma_line_num;


/*------------------------------------- DMA HCL APIs ----------------------------------------------*/

/*--------- Initilisation  APIs ------------*/
PUBLIC t_dma_error DMA_Init(IN t_logical_address);
PUBLIC t_dma_error DMA_GetVersion(OUT t_version *);
PUBLIC t_dma_error DMA_SetDbgLevel(IN t_dbg_level);
PUBLIC t_dma_error DMA_InitRelinkMemory(IN t_system_address * , IN t_uint32);

/*--------- Enhanced Functional  APIs -----*/

PUBLIC t_dma_error DMA_OpenChannel(IN t_dma_chan_id  ,  OUT t_dma_channel_config  *);
PUBLIC t_dma_error DMA_OpenPipe(IN t_dma_channel_config , IN t_dma_src_device , IN t_dma_dest_device , OUT t_dma_pipe_config *);
PUBLIC t_dma_error DMA_SetChannelType(IN t_dma_channel_type , IN t_dma_channel_security , INOUT t_dma_channel_config *);
PUBLIC t_dma_error DMA_SetChannelModeOption(IN t_dma_channel_mode, IN t_dma_channel_option, INOUT t_dma_channel_config *);
PUBLIC t_dma_error DMA_ConfigSrcDevice(IN t_dma_pipe_config, t_uint32, IN t_dma_data_width , IN t_dma_burst_size, IN t_uint16, IN t_uint32,IN t_uint16);
PUBLIC t_dma_error DMA_ConfigDestDevice(IN t_dma_pipe_config , t_uint32, t_dma_data_width , IN t_dma_burst_size,  IN t_uint16, IN t_uint32, IN t_uint16);
PUBLIC t_dma_error DMA_UpdateSrcMemoryBuffer(IN t_dma_channel_config , IN t_dma_pipe_config);
PUBLIC t_dma_error DMA_UpdateDestMemoryBuffer(IN t_dma_channel_config , IN t_dma_pipe_config);
PUBLIC t_dma_error DMA_SetTCInterruptLogic(IN t_dma_pipe_config , IN t_dma_intr_gen, IN t_bool, IN t_bool);
PUBLIC t_dma_error DMA_SetMasterPort(IN t_dma_pipe_config, IN t_dma_master, IN t_dma_master);
PUBLIC t_dma_error DMA_SetTerminalCountPulse(IN t_dma_pipe_config, IN t_bool, IN t_bool);
PUBLIC t_dma_error DMA_TransferActivate(IN t_dma_channel_config  , IN t_dma_active_status);
PUBLIC t_dma_error DMA_ClosePipe(IN t_dma_channel_config , IN t_dma_pipe_config);
PUBLIC t_dma_error DMA_CloseChannel(IN t_dma_channel_config);
PUBLIC t_dma_error DMA_ResetChannel(IN t_dma_channel_config);
PUBLIC t_dma_error DMA_FilterProcessIRQSrc(IN t_dma_irq_status * , IN t_dma_event_desc  *, IN t_dma_filter_mode );


#ifdef __cplusplus
} /* To allow C++ to use this header */
#endif    /* __cplusplus */

#endif /* __DMA_HEADER */



