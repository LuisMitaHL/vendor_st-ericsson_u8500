/*****************************************************************************/
/**
*  © ST-Ericsson, 2009 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \brief   Public Header file of I2C Controller module
* \author  ST-Ericsson
*/
/*****************************************************************************/



#ifndef _I2C_H_
#define _I2C_H_


#include "hcl_defs.h"
#include "i2c_irq.h"
#include "debug.h"

#ifdef __cplusplus
extern "C"
{	/* In case C++ needs to use this header.*/
#endif


#define I2C_MODULE_NAME		"I2C HCL Module"

#define I2C_HCL_VERSION_ID  2
#define I2C_MAJOR_VERSION	1
#define I2C_MINOR_VERSION	0

/*-----------------------------------------------------------------------------
	Typedefs
-----------------------------------------------------------------------------*/
typedef enum {
    I2C_FREQ_MODE_STANDARD,	        	/* Standard mode.   */	
	I2C_FREQ_MODE_FAST,	                /* Fast mode.       */
    I2C_FREQ_MODE_HIGH_SPEED,            /* High Speed mode. */
    I2C_FREQ_MODE_FAST_PLUS	                /* Fast Plus mode.       */
} t_i2c_freq_mode;

typedef enum {
	I2C_BUS_SLAVE_MODE = 0,             /* Slave Mode               */
	I2C_BUS_MASTER_MODE,                /* Master Mode              */
	I2C_BUS_MASTER_SLAVE_MODE           /* Dual Configuration Mode  */
} t_i2c_bus_control_mode;

typedef enum {
	I2C_CURRENT_BUS_SLAVE_TRANSMITTER,
	I2C_CURRENT_BUS_SLAVE_RECEIVER,
	I2C_CURRENT_BUS_MASTER_TRANSMITTER,
	I2C_CURRENT_BUS_MASTER_RECEIVER
}t_i2c_current_bus_configuration;

typedef enum {
    I2C_TRANSFER_MODE_POLLING,
    I2C_TRANSFER_MODE_INTERRUPT,
    I2C_TRANSFER_MODE_DMA
}t_i2c_transfer_mode;

typedef enum {
    I2C_DISABLE,
    I2C_ENABLE
}t_i2c_control;

typedef enum {
	I2C_COMMAND_SEND_START,
	I2C_COMMAND_SEND_STOP,
	I2C_COMMAND_SEND_ACKNOWLEDGE,
	I2C_COMMAND_CLEAR_ACKNOWLEDGE,
	I2C_COMMAND_SET_TRANSMIT_DMA,
	I2C_COMMAND_CLEAR_TRANSMIT_DMA,
	I2C_COMMAND_SET_RECEIVE_DMA,
	I2C_COMMAND_CLEAR_RECEIVE_DMA
}t_i2c_command;


typedef enum {
#if ((defined __PEPS_8500_V2)||(defined ST_8500V2)||(defined ST_HREFV2))
	I2C_STATUS_MASTER_TRANSMITTER_MODE = 0x2,				/* Controller is in master transmitter mode.*/
	I2C_STATUS_MASTER_RECEIVER_MODE = 0x3,				/* Controller is in master receiver mode.*/
	I2C_STATUS_SLAVE_TRANSMITTER_MODE = 0x0,				/* Controller is in slave transmitter mode.*/
	I2C_STATUS_SLAVE_RECEIVER_MODE = 0x1,					/* Controller is in slave receiver mode.*/
#else
	I2C_STATUS_MASTER_TRANSMITTER_MODE = 0x3,				/* Controller is in master transmitter mode.*/
	I2C_STATUS_MASTER_RECEIVER_MODE = 0x2,				/* Controller is in master receiver mode.*/
	I2C_STATUS_SLAVE_TRANSMITTER_MODE = 0x0,				/* Controller is in slave transmitter mode.*/
	I2C_STATUS_SLAVE_RECEIVER_MODE = 0x1,					/* Controller is in slave receiver mode.*/
#endif
	I2C_STATUS_SLAVE_MODE = 0x4,					     	/* Controller is in slave mode.*/
	I2C_STATUS_MASTER_MODE = 0x5				        /* Controller is in master mode.*/
} t_i2c_device_status;

    
typedef enum {
    I2C_TRANSMIT_FIFO,
    I2C_RECEIVE_FIFO
}t_i2c_fifo;


typedef enum {
    I2C_DIGITAL_FILTERS_OFF,
    I2C_DIGITAL_FILTERS_1_CLK_SPIKES,
    I2C_DIGITAL_FILTERS_2_CLK_SPIKES,
    I2C_DIGITAL_FILTERS_4_CLK_SPIKES
}t_i2c_digital_filter;


typedef struct {
    t_i2c_digital_filter   i2c_digital_filter_control;
    t_i2c_control          i2c_dma_sync_logic_control;
    t_i2c_control          i2c_start_byte_procedure;       /*ONLY VALID FOR MASTER MODE TRANSACTIONS*/
    t_uint8                i2c_high_speed_master_code;     /*ONLY VALID FOR MASTER MODE TRANSACTIONS*/
    t_uint16               slave_data_setup_time;         /* Only valid for HS controller */
    t_uint16               controller_i2c_address;
    t_uint32               input_frequency;
} t_i2c_device_config;

typedef enum {
    I2C_NO_GENERAL_CALL_HANDLING,
    I2C_SOFTWARE_GENERAL_CALL_HANDLING,
    I2C_HARDWARE_GENERAL_CALL_HANDLING
} t_i2c_general_call_handling;
    
typedef struct {
    t_i2c_control               i2c_loopback_mode;
    t_i2c_general_call_handling i2c_slave_general_call_mode;
    t_i2c_transfer_mode         index_transfer_mode;
    t_i2c_transfer_mode         data_transfer_mode;
    t_i2c_bus_control_mode      bus_control_mode;
    t_uint8                     i2c_transmit_interrupt_threshold;
    t_uint8                     i2c_receive_interrupt_threshold;
    t_uint8                     transmit_burst_length;
    t_uint8                     receive_burst_length;
    t_uint32                    i2c_transfer_frequency;
} t_i2c_transfer_config;



typedef struct {
    t_logical_address	            base_address;       /* The controller's logical base address.       */
    t_i2c_device_id 	                    id;			/* The controller's id.                         */
    t_i2c_freq_mode	                freq_mode;          /* Standard ,Fast mode or Hs Mode.              */
    t_bool	                        is_enabled;		    /* True means controller is enabled.            */
    t_i2c_bus_control_mode	        bus_control_mode;
    t_i2c_current_bus_configuration	current_bus_config;
    t_i2c_general_call_handling 	general_call_handling;
    t_uint32	                    freq_scl;           /* The I2C bus SCL clock frequency (Hz).        */
    t_uint32	                    freq_input;		    /* The controller's input clock frequency (Hz). */
    t_uint32	                    own_address;        /* The controller's slave address.              */
} t_i2c_info;		                    /* Used to provide information to the user.     */	


typedef enum {
    /*Common to all platforms*/
    I2C_STATE_GENERAL_CALL_DETECTED     = MASK_BIT0,
    I2C_STATE_ARBITRATION_LOST          = MASK_BIT2,
    I2C_STATE_BUSY                      = MASK_BIT12,

    I2C_STATE_TRANSFER_COMPLETE         = MASK_BIT16,
    I2C_STATE_ABORT_NACK_ON_ADDRESS     = MASK_BIT17,
    I2C_STATE_ABORT_NACK_ON_DATA        = MASK_BIT18,
    I2C_STATE_ABORT_ACK_ON_MASTER_CODE  = MASK_BIT19,
    I2C_STATE_BUS_ERROR_DETECTED_START  = MASK_BIT20,
    I2C_STATE_BUS_ERROR_DETECTED_STOP   = MASK_BIT21,
    I2C_STATE_OVERFLOW                  = MASK_BIT22,
    I2C_STATE_HARDWARE_GENERAL_CALL     = MASK_BIT23
} t_i2c_device_states;


typedef enum {
	I2C_NO_PENDING_EVENT_ERROR			= HCL_NO_PENDING_EVENT_ERROR,
	I2C_NO_MORE_FILTER_PENDING_EVENT	= HCL_NO_MORE_FILTER_PENDING_EVENT,
	I2C_NO_MORE_PENDING_EVENT			= HCL_NO_MORE_PENDING_EVENT,
	I2C_REMAINING_FILTER_PENDING_EVENTS	= HCL_REMAINING_FILTER_PENDING_EVENTS,
	I2C_REMAINING_PENDING_EVENTS		= HCL_REMAINING_PENDING_EVENTS,
	I2C_INTERNAL_EVENT					= HCL_INTERNAL_EVENT,
	I2C_OK 								= HCL_OK,	                    /* No error.                                */
    I2C_INTERNAL_ERROR                  = HCL_INTERNAL_ERROR,          
    I2C_NOT_CONFIGURED                  = HCL_NOT_CONFIGURED,          
    I2C_REQUEST_PENDING                 = HCL_REQUEST_PENDING,         
    I2C_REQUEST_NOT_APPLICABLE          = HCL_REQUEST_NOT_APPLICABLE,  
    I2C_INVALID_PARAMETER               = HCL_INVALID_PARAMETER,       
    I2C_UNSUPPORTED_FEATURE             = HCL_UNSUPPORTED_FEATURE,     
    I2C_UNSUPPORTED_HW                  = HCL_UNSUPPORTED_HW,          
	I2C_HW_FAILED						= (HCL_MAX_ERROR_VALUE -1),		/* Generic hardware error.					*/
	I2C_SW_FAILED						= (HCL_MAX_ERROR_VALUE -2),		/* Generic software error.					*/
	I2C_CONTROLLER_BUSY					= (HCL_MAX_ERROR_VALUE -3),		/* Transfer on going.						*/
	I2C_LINE_FREQ_NOT_SUPPORTED			= (HCL_MAX_ERROR_VALUE -4),		/* SCL bus frequency not supported.			*/
	I2C_INPUT_FREQ_NOT_SUPPORTED		= (HCL_MAX_ERROR_VALUE -5),		/* Input frequency not supported.			*/
	I2C_DDC_MODE_NOT_SUPPORTED			= (HCL_MAX_ERROR_VALUE -6),		/* DDC mode not supported.					*/
	I2C_SLAVE_ADDRESS_NOT_VALID			= (HCL_MAX_ERROR_VALUE -7),		/* Slave address is reserved or not valid.  */

    I2C_BYTE_TRANSFER_FAILED 	        = (HCL_MAX_ERROR_VALUE -100),
    I2C_ADDRESS_MATCH_FAILED            = (HCL_MAX_ERROR_VALUE -101),
    I2C_START_BYTE_FAILED               = (HCL_MAX_ERROR_VALUE -102),
    I2C_ACKNOWLEDGE_FAILURE             = (HCL_MAX_ERROR_VALUE -103),
    I2C_STOP_FAILED 	                = (HCL_MAX_ERROR_VALUE -104),
    I2C_ARBITRATION_LOST                = (HCL_MAX_ERROR_VALUE -105),
    I2C_BUS_ERROR 	                    = (HCL_MAX_ERROR_VALUE -106),
    I2C_10_BIT_ADDRESS_FAILED           = (HCL_MAX_ERROR_VALUE -107),
    I2C_SCL_FALL_FAILED                 = (HCL_MAX_ERROR_VALUE -108),
    I2C_END_ADDRESS_FAILED              = (HCL_MAX_ERROR_VALUE -109),

    
    I2C_TRANSMIT_FIFO_FULL              = (HCL_MAX_ERROR_VALUE -200),
    I2C_RECEIVE_FIFO_EMPTY              = (HCL_MAX_ERROR_VALUE -201),
    I2C_ACK_FAIL_ON_ADDRESS             = (HCL_MAX_ERROR_VALUE -202),
    I2C_ACK_FAIL_ON_DATA                = (HCL_MAX_ERROR_VALUE -203),
    I2C_ACK_IN_HS_MODE                  = (HCL_MAX_ERROR_VALUE -204),
    I2C_BUS_ERROR_DETECTED_START        = (HCL_MAX_ERROR_VALUE -205),
    I2C_BUS_ERROR_DETECTED_STOP         = (HCL_MAX_ERROR_VALUE -206),
    I2C_OVERFLOW                        = (HCL_MAX_ERROR_VALUE -207)
} t_i2c_error;

typedef enum
{
    /*Common to all platforms*/
	I2C_NO_EVENT				                     = MASK_BIT0,	/* No activity.                                 */
	I2C_TRANSFER_OK_EVENT		                     = MASK_BIT1,	/* Transfer operation ended correctly.			*/
	I2C_CANCEL_EVENT			                     = MASK_BIT2,	/* Transfer operation cancelled by the user.	*/
	I2C_INTERNAL_ERROR_EVENT	                     = MASK_BIT3,	/* Internal error happened.						*/
	I2C_ARBITRATION_LOST_ERROR_EVENT                 = MASK_BIT4,	/* Arbitration Lost happened.					*/

	I2C_AF_ERROR_EVENT			                     = MASK_BIT5,	/* Acknowledge Failure happened.				*/
	I2C_BUS_ERROR_EVENT	                             = MASK_BIT6,	/* Bus Error happened.							*/
	I2C_START_EVENT			                         = MASK_BIT7,	/* START generated.								*/
	I2C_INDEX_TX_EVENT			                     = MASK_BIT8,	/* Register index byte transmitted.				*/
	I2C_DATA_TX_EVENT			                     = MASK_BIT9,	/* Data byte transmitted.						*/
	I2C_DATA_RX_EVENT			                     = MASK_BIT10,	/* Data byte received.							*/
	I2C_WAITING_DATA_RX_EVENT	                     = MASK_BIT11,	/* Waiting for a data byte.						*/

    
    I2C_TRANSMIT_FIFO_EMPTY_EVENT                    = MASK_BIT12,
    I2C_TRANSMIT_FIFO_NEARLY_EMPTY_EVENT             = MASK_BIT13,
    I2C_TRANSMIT_FIFO_FULL_EVENT                     = MASK_BIT14,
    I2C_TRANSMIT_FIFO_OVERRUN_EVENT                  = MASK_BIT15,
    I2C_RECEIVE_FIFO_EMPTY_EVENT                     = MASK_BIT16,
    I2C_RECEIVE_FIFO_NEARLY_FULL_EVENT               = MASK_BIT17,
    I2C_RECEIVE_FIFO_FULL_EVENT                      = MASK_BIT18,
    I2C_READ_FROM_SLAVE_REQUEST_EVENT                = MASK_BIT19,
    I2C_READ_FROM_SLAVE_EMPTY_EVENT                  = MASK_BIT20,
    I2C_WRITE_TO_SLAVE_REQUEST_EVENT                 = MASK_BIT21,
    I2C_MASTER_TRANSACTION_DONE_EVENT                = MASK_BIT22,
    I2C_SLAVE_TRANSACTION_DONE_EVENT                 = MASK_BIT23,
    I2C_ABORT_NACK_ON_ADDRESS_EVENT                  = MASK_BIT24,
    I2C_ABORT_NACK_ON_DATA_EVENT                     = MASK_BIT25,
    I2C_ABORT_ACK_ON_MASTER_CODE_EVENT               = MASK_BIT26,
    I2C_BUS_ERROR_DETECTED_START_EVENT               = MASK_BIT27,
    I2C_BUS_ERROR_DETECTED_STOP_EVENT                = MASK_BIT28,
    I2C_OVERFLOW_EVENT                               = MASK_BIT29,
    I2C_MASTER_TRANSACTION_DONE_WITHOUT_STOP_EVENT   = MASK_BIT30
} t_i2c_event;			               /* Inform the I2C HCL user about the last occurred event.*/


typedef enum
{
    I2C_NO_INDEX,                               /* Current transfer is non-indexed      */
    I2C_BYTE_INDEX,                             /* Current transfer uses 8-bit index    */
    I2C_HALF_WORD_LITTLE_ENDIAN,                /* Current transfer uses 16-bit index 
                                                   in little endian mode                */
    I2C_HALF_WORD_BIG_ENDIAN                    /* Current transfer uses 16-bit index 
                                                   in big endian mode                   */
}t_i2c_index_format;

typedef struct {
    t_i2c_device_id id;
	t_i2c_event		type;		    /* The active event.                            */
	t_uint32		transfer_data;	/* Number of data bytes actually transferred.   */
} t_i2c_active_event;

typedef t_i2c_device_id t_i2c_irq_status;

/*---------Time Period for I2C------------*/
typedef enum
{
        I2C_24MHZ_CLK_PERIOD = 42, 		/*It is actually 42ns*/
        I2C_38MHZ_CLK_PERIOD = 26, 		/*It is actually 26ns*/
        I2C_48MHZ_CLK_PERIOD = 21  		/*It is actually 21ns*/           
}t_i2c_time_period;

/*-----------------------------------------------------------------------------
	Configuration functions
-----------------------------------------------------------------------------*/
PUBLIC t_i2c_error I2C_Init            (IN t_i2c_device_id id, 
                                        IN t_logical_address address);

PUBLIC t_i2c_error I2C_SetDbgLevel     (IN t_dbg_level DebugLevel);

PUBLIC t_i2c_error I2C_GetVersion       (OUT t_version* version);

PUBLIC t_i2c_error I2C_SetDeviceConfiguration   (IN t_i2c_device_id id,
                                                IN t_i2c_device_config    *p_device_config);

PUBLIC t_i2c_error I2C_SetTransferConfiguration (IN t_i2c_device_id id,
                                                IN t_i2c_transfer_config  *p_transfer_config);

PUBLIC t_i2c_error I2C_SetTransferMode ( IN t_i2c_device_id id, 
                                         IN t_i2c_transfer_mode index_transfer_mode,
                                         IN t_i2c_transfer_mode data_transfer_mode);

PUBLIC t_i2c_error I2C_SetBusControlMode (IN t_i2c_device_id id, 
                                          IN t_i2c_bus_control_mode bus_control_mode);

PUBLIC t_i2c_error I2C_SendCommand          (   IN t_i2c_device_id,
                                                    IN t_i2c_command);

                                        
/*-----------------------------------------------------------------------------
	    Configuration functions
-----------------------------------------------------------------------------*/
PUBLIC t_i2c_error I2C_FlushFifo(IN t_i2c_device_id , IN t_i2c_fifo );

/*-----------------------------------------------------------------------------
	    Status functions
-----------------------------------------------------------------------------*/
PUBLIC t_i2c_error I2C_GetInfo                  (   IN t_i2c_device_id id, 
                                                    OUT t_i2c_info *p_info);
/*-----------------------------------------------------------------------------
        Operative functions
-----------------------------------------------------------------------------*/
	PUBLIC t_i2c_error I2C_Enable   (IN t_i2c_device_id id);

	PUBLIC t_i2c_error I2C_Disable  (IN t_i2c_device_id id);


#ifdef __I2C_ELEMENTARY                                            
    PUBLIC t_i2c_error I2C_GetDeviceStatus      (   IN t_i2c_device_id id, 
                                                    OUT t_i2c_device_status *p_device_status);
                                                
    PUBLIC t_i2c_error I2C_WriteData            (   IN t_i2c_device_id,
                                                    IN t_uint8 data);

    PUBLIC t_i2c_error I2C_ReadData             (   IN t_i2c_device_id,
                                                    OUT t_uint8 *p_data);
                                                    

    PUBLIC t_i2c_error I2C_ConfigureSlaveAddress(   IN t_i2c_device_id id,
                                                    IN t_uint16 i2c_slave_address);
    
    PUBLIC t_i2c_error I2C_ConfigureFrameLength(    IN t_i2c_device_id id,
                                                    IN t_uint16 i2c_frame_length);
                                                                                                                                                    

#elif (defined __I2C_ENHANCED)

    PUBLIC t_i2c_error I2C_WriteSingleData    (IN t_i2c_device_id             id,
                                                IN t_uint16             slave_address, 
                                                IN t_i2c_index_format   index_format,                                                
                                                IN t_uint16             index_value,
                                                IN t_uint8              data);

    PUBLIC t_i2c_error I2C_WriteMultipleData  (IN t_i2c_device_id             id, 
                                                IN t_uint16             slave_address, 
                                                IN t_i2c_index_format   index_format,                                                
                                                IN t_uint16             index_value,
                                                IN t_uint8              *p_data, 
                                                IN t_uint32             count);

    PUBLIC t_i2c_error I2C_ReadSingleData    (IN t_i2c_device_id             id, 
                                                IN t_uint16             slave_address, 
                                                IN t_i2c_index_format   index_format,
                                                IN t_uint16             index_value,
                                                IN t_uint8              *p_data);

    PUBLIC t_i2c_error I2C_ReadMultipleData (IN t_i2c_device_id             id, 
                                                IN t_uint16             slave_address, 
                                                IN t_i2c_index_format   index_format,
                                                IN t_uint16             index_value,
                                                IN t_uint8              *p_data, 
                                                IN t_uint32             count);


    PUBLIC t_i2c_error I2C_Cancel              (IN      t_i2c_device_id id, 
                                                INOUT   t_i2c_active_event *event);/*Only IT mode*/

    PUBLIC void 			I2C_GetIRQSrcStatus    (IN t_i2c_irq_src_id id, 
    											    OUT t_i2c_irq_status* status);

    PUBLIC t_i2c_error 		I2C_ProcessIRQSrc       (INOUT  t_i2c_irq_status* status);

    PUBLIC t_i2c_error 		I2C_FilterProcessIRQSrc (INOUT  t_i2c_irq_status* status, 
                                                     OUT    t_i2c_active_event* event);

    PUBLIC t_bool 			I2C_IsEventActive       (IN     t_i2c_active_event* event);

    PUBLIC t_bool 			I2C_AcknowledgeEvent    (IN   t_i2c_active_event* event);
                                                

#endif

#ifdef __I2C_GENERIC

    PUBLIC t_i2c_error I2C_StartWriteRequest   (IN t_i2c_device_id      id,
                                                IN t_uint16             slave_address,   
                                                IN t_uint32             count);
                                                
    PUBLIC t_i2c_error I2C_TransmitBuffer      (IN t_i2c_device_id      id,    
                                                IN t_uint8              *p_data,
                                                IN t_uint32             count);
                                                
    PUBLIC t_i2c_error I2C_EndWriteRequest     (IN t_i2c_device_id      id);
    
    PUBLIC t_i2c_error I2C_StartReadRequest    (IN t_i2c_device_id      id,
                                                IN t_uint16             slave_address,   
                                                IN t_uint32             count_totaldata,
                                                IN t_uint8              *p_data,
                                                IN t_uint8              count_index);
                                                
    PUBLIC t_i2c_error I2C_ReceiveBuffer       (IN t_i2c_device_id      id,    
                                                IN t_uint8              *p_data,
                                                IN t_uint32             count);
                                                
    PUBLIC t_i2c_error I2C_EndReadRequest      (IN t_i2c_device_id      id);                                                                                                

#endif

PUBLIC t_i2c_error I2C_GetInputClock(           IN t_i2c_device_id id, 
                                                OUT t_uint32 *p_fIn);
                                            
PUBLIC t_i2c_error I2C_GetBusClock(             IN t_i2c_device_id id, 
                                                OUT t_uint32 *p_fSCL);
                                        
PUBLIC t_i2c_error I2C_GetEnabled(              IN t_i2c_device_id id, 
                                                OUT t_bool *p_status);

PUBLIC t_i2c_error I2C_GetDeviceConfiguration(  IN t_i2c_device_id id,
                                                OUT t_i2c_device_config    *p_device_config);

PUBLIC t_i2c_error I2C_GetTransferConfiguration(IN t_i2c_device_id id,
                                                OUT t_i2c_transfer_config  *p_transfer_config); 

PUBLIC t_i2c_error I2C_GetTransferMode(         IN t_i2c_device_id id, 
                                                OUT t_i2c_transfer_mode *p_index_transfer_mode,
                                                OUT t_i2c_transfer_mode *p_data_transfer_mode);

PUBLIC t_i2c_error I2C_GetBusControlMode(       IN t_i2c_device_id id, 
                                                OUT t_i2c_current_bus_configuration 
                                                                   *p_i2c_current_transfer_mode,
                                                OUT t_i2c_bus_control_mode *p_bus_control_mode);


/*-----------------------------------------------------------------------------
		Power Management functions
-----------------------------------------------------------------------------*/
PUBLIC t_i2c_error   I2C_Reset(IN t_i2c_device_id id);

PUBLIC t_i2c_error   I2C_SaveDeviceContext   (IN t_i2c_device_id id);

PUBLIC t_i2c_error   I2C_RestoreDeviceContext(IN t_i2c_device_id id);

/*------------------------------------------------------------------------------
		Setup and Holdtime Related Functions 
-------------------------------------------------------------------------------*/
PUBLIC t_i2c_error I2C_SetDataHoldTime(t_i2c_device_id id, t_uint32 hold_time, t_uint32 pad_delay, t_i2c_time_period period );

PUBLIC t_i2c_error I2C_SetHoldTimeStart(t_i2c_device_id id, t_uint32 start_hold_time, t_i2c_time_period period);

PUBLIC t_i2c_error I2C_SetSetupTimeStart(t_i2c_device_id id, t_uint32 start_setup_time, t_i2c_time_period period);

#ifdef __cplusplus
}   /* Allow C++ to use this header */
#endif  /* __cplusplus              */

#endif	/* _I2CHCL_H_               */
