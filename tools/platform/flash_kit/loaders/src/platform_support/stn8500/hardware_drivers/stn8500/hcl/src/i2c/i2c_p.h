/*****************************************************************************/
/**
*  © ST-Ericsson, 2009 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \brief   Private Header file of I2C Controller module containing private APIs and constants
* \author  ST-Ericsson
*/
/*****************************************************************************/


#ifndef _i2c_H_
#define _i2c_H_



#include "i2c.h"
#include "i2c_irqp.h"



#ifdef __cplusplus
extern "C"
{	/* In case C++ needs to use this header.*/
#endif


/*Peripheral ID s  */

#define   I2C_P_ID_0         0x24
#define   I2C_P_ID_1         0x00
#define   I2C_P_ID_2         0x38
#define   I2C_P_ID_3         0x00
#define   I2C_CELL_ID_0      0x0D 
#define   I2C_CELL_ID_1      0xF0
#define   I2C_CELL_ID_2      0x05
#define   I2C_CELL_ID_3      0xB1

/*-----------------------------------------------------------------------------

	Constants

-----------------------------------------------------------------------------*/
typedef enum {
	I2C_NO_OPERATION	= 0xFF,
	I2C_WRITE			= 0x00,
	I2C_READ			= 0x01
} t_i2c_operation;

/*-----------------------------------------------------------------------------
	Typedefs
-----------------------------------------------------------------------------*/
typedef enum { 
	I2C_MAX_STANDARD_SCL    =   100000,	/* Max clock frequency (Hz) for Standard Mode.*/
	I2C_MAX_FAST_SCL	    =   400000,	/* Max clock frequency (Hz) for Fast Mode.*/
	I2C_MAX_FAST_PLUS_SCL	=  1000000,	/* Max clock frequency (Hz) for Fast Plus Mode.*/
	I2C_MAX_HIGH_SPEED_SCL  =  3400000  /* Max clock frequency (Hz) for HS Mode.*/
} I2C_MaxClocks;

typedef enum {
	I2C_DDC1,						/* DDC1 mode.*/
	I2C_DDC2B,						/* DD2 B mode.*/
	I2C_DDC2AB						/* DDC2 AB mode (I2C).*/
} t_i2c_ddc_mode;

typedef enum {
    I2C_IT_TXFE     = MASK_BIT0,     /* Tx fifo empty */
    I2C_IT_TXFNE    = MASK_BIT1,     /* Tx Fifo nearly empty */
    I2C_IT_TXFF     = MASK_BIT2,     /* Tx Fifo Full */
    I2C_IT_TXOVR    = MASK_BIT3,     /* Tx Fifo over run */
    I2C_IT_RXFE     = MASK_BIT4,     /* Rx Fifo Empty */
    I2C_IT_RXFNF    = MASK_BIT5,     /* Rx Fifo nearly empty */
    I2C_IT_RXFF     = MASK_BIT6,     /* Rx Fifo Full  */
    I2C_IT_RFSR     = MASK_BIT16,    /* Read From slave request */
    I2C_IT_RFSE     = MASK_BIT17,    /* Read from slave empty */
    I2C_IT_WTSR     = MASK_BIT18,    /* Write Slave request */
    I2C_IT_MTD      = MASK_BIT19,    /* Master Transcation Done */
    I2C_IT_STD      = MASK_BIT20,    /* Slave Transaction Done */
    I2C_IT_MAL      = MASK_BIT24,    /* Master Arbitation Lost */
    I2C_IT_BERR     = MASK_BIT25,    /* Bus Error */
    I2C_IT_MTDWS      = MASK_BIT28   /* Master Transcation Done Without Stop */
}t_i2c_interrupt;	/* IRQ source numbers.*/




typedef enum { 
	I2C_NO_REG_INDEX_OP,			/* Do not send any register index.*/
	I2C_8_BIT_REG_INDEX_OP,			/* Send a 8-bit register index.*/
	I2C_16_BIT_REG_INDEX_OP			/* Send a 16-bit register index.*/
} t_i2c_reg_op;

typedef enum {
	I2C_SINGLE_BYTE_TRANSFER,
	I2C_MULTIPLE_BYTE_TRANSFER
} t_i2c_transfer_data;

typedef t_uint32 t_i2c_device_context[1];

typedef struct {
    /*Device configuration*/
	t_i2c_registers	 		*p_i2c_registers;		/* I2C Controller Register*/
	t_uint32			    freq_scl;	        	/* The I2C bus SCL clock frequency (Hz).*/
	t_uint16			    own_address;	        /* The controller's slave address.*/
    t_i2c_device_context    i2c_device_context;
    t_i2c_digital_filter    digital_filter_control;
    t_i2c_control           dma_sync_logic_control;
    t_i2c_control           start_byte_procedure;
    t_uint8                 high_speed_master_code;
    t_uint16                slave_data_setup_time;
   
    /*Transfer configuration*/
	t_uint32			    freq_input;		        /* The controller's input clock frequency (Hz).*/
	t_i2c_freq_mode		    mode;			        /* Standard or Fast mode.*/
	t_i2c_operation		    operation;		        /* Write or read.*/
	t_i2c_bus_control_mode  bus_control_mode;
    t_i2c_control           i2c_loopback_mode;
    t_i2c_general_call_handling general_call_mode_handling;
    t_i2c_transfer_mode     index_transfer_mode;
    t_i2c_transfer_mode     data_transfer_mode;
    t_uint8                 i2c_transmit_interrupt_threshold;
    t_uint8                 i2c_receive_interrupt_threshold;
    t_uint8                 transmit_burst_length;
    t_uint8                 receive_burst_length;
    t_uint16                burst_length;
	t_uint16			    slave_address;	        /* The slave to talk to.*/
	t_uint16			    register_index;	        /* The index of the slave's registers*/
	t_i2c_index_format      index_format;
	t_bool                  multi_operation;

    /*Device Status*/
	t_bool				    enabled;		        /* True means controller is enabled.*/
	t_uint32			    count_data;		        /* The number of bytes to be transferred.*/
	t_uint32			    transfer_data;	        /* Number of transferred data bytes.*/
	t_uint8*			    databuffer;		        /* Pointer to the data buffer. Used in Multi operation.*/
	t_i2c_current_bus_configuration current_bus_config;
	t_i2c_device_status		    status;			        /* The controller's status.*/
	t_uint8				    data;			        /* Used in Single operation.*/
	t_i2c_event			    active_event;	        /* The current active event.*/
	t_bool                  std;                    /*This variable is used to store the STD interrupt   */
	                                                /*status for 10 bit slave transmitter case */
    
} t_i2c_system_context;


/*-----------------------------------------------------------------------------

	Private service functions
                 
-----------------------------------------------------------------------------*/
PRIVATE t_i2c_error i2c_SetBusClock(t_i2c_device_id id, t_uint32 fSCL, t_uint32 fIn);
PRIVATE t_bool i2c_AddressIsValid(t_uint16 address);
PRIVATE void i2c_Abort(t_i2c_device_id id);

#if((defined __I2C_ENHANCED) || (defined __I2C_GENERIC))
PRIVATE t_i2c_error i2c_ProcessIt(t_i2c_irq_status* status, t_i2c_active_event* event);
PRIVATE t_i2c_error  i2c_SlaveIndexReceive(t_i2c_device_id id );
PRIVATE t_i2c_error  i2c_TransmitDataPolling(t_i2c_device_id id, volatile t_uint8 *p_data,t_i2c_transfer_data data_type);
PRIVATE t_i2c_error  i2c_TransmitDataPollingSingle(t_i2c_device_id id, volatile t_uint8* p_data);
PRIVATE t_i2c_error  i2c_ReceiveDataPolling(t_i2c_device_id  id, t_uint8* p_data );
PRIVATE t_i2c_error  i2c_MasterIndexTransmit(t_i2c_device_id id );
#endif

#ifdef __I2C_GENERIC
PRIVATE t_i2c_error i2c_TransmitBufferPolling(t_i2c_device_id id, volatile t_uint8 *p_data);
PRIVATE t_i2c_error i2c_ReceiveBufferPolling(t_i2c_device_id id, t_uint8 *p_data);
PRIVATE t_i2c_error i2c_TransmitBufferDataTransfer(t_i2c_device_id id);
PRIVATE void i2c_StartReadRequestDisableIRQ(t_i2c_device_id id);
PRIVATE void i2c_StartReadRequestCheckMode(t_i2c_device_id id, t_uint16 slave_address, t_i2c_index_format index_format, t_uint32 count);
#endif

PRIVATE t_i2c_error  i2c_GetAbortCause(t_i2c_device_id id );


PRIVATE t_i2c_interrupt i2c_ProcessItTestIntBit(t_i2c_irq_status *status);
PRIVATE void i2c_ProcessItDisableIRQSrc(t_i2c_irq_status *status);
PRIVATE void i2c_ProcessItProcessTXFNE(t_i2c_irq_status *status);
PRIVATE void i2c_ProcessItProcessRFSE(t_i2c_irq_status *status);
PRIVATE void i2c_ProcessItProcessMTD(t_i2c_irq_status *status);
PRIVATE void i2c_ProcessItProcessSTD(t_i2c_irq_status *status);
PRIVATE void i2c_ProcessItProcessBERR(t_i2c_irq_status *status);
PRIVATE void i2c_ProcessItProcessMTDWS(t_i2c_irq_status *status);
PRIVATE void i2c_ProcessItProcessTX(t_i2c_interrupt interrupt_source, t_i2c_irq_status *status);
PRIVATE t_i2c_error i2c_ProcessItProcessRX(t_i2c_interrupt interrupt_source, t_i2c_irq_status *status);
PRIVATE void i2c_ProcessItProcessMisc(t_i2c_interrupt interrupt_source, t_i2c_irq_status *status);
PRIVATE t_i2c_error i2c_MasterIndexTransmitCheckTxFifo(t_i2c_device_id id);
PRIVATE t_i2c_error i2c_MasterIndexTransmitCheckIndex(t_i2c_device_id id);
PRIVATE t_i2c_error i2c_ReceiveDataPollingCheckRxFifo(t_i2c_device_id id);
PRIVATE t_i2c_error i2c_ReceiveDataPollingCheckForSTD(t_i2c_device_id id);
PRIVATE t_i2c_error i2c_TransmitDataPollingCheckTxFifo(t_i2c_device_id id);
PRIVATE t_i2c_error i2c_TransmitDataPollingCheckForSTD(t_i2c_device_id id);
PRIVATE void i2c_RWDataDisableIRQ(t_i2c_device_id id);
PRIVATE void i2c_WriteDataCheckMode(t_i2c_device_id id, t_uint16 slave_address, t_i2c_index_format index_format,t_uint32 count);
PRIVATE void i2c_WriteDataEnableIRQ(t_i2c_device_id id);
PRIVATE t_i2c_error i2c_RWDataErrorCheck(t_i2c_device_id id, t_uint16 slave_address, t_i2c_index_format index_format);
PRIVATE void i2c_ReadDataCheckMode(t_i2c_device_id id, t_uint16 slave_address, t_i2c_index_format index_format,t_uint32 count);
PRIVATE void i2c_ReadDataEnableIRQ(t_i2c_device_id id, t_i2c_index_format index_format);




#endif	/* _i2c_H_*/
