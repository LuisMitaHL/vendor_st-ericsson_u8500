/*****************************************************************************/
/**
*  © ST-Ericsson, 2009 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
*  This module provides prototype for CRYP Public functions and structrue
*  AUTHOR :  ST-Ericsson
*/
/*****************************************************************************/

#ifndef _CRYP_H_
#define _CRYP_H_
    
#ifdef __cplusplus
extern "C"
{	/* In case C++ needs to use this header.*/
#endif

#include "cryp_irq.h"

/*---------------------------------------------------------------------------
	Modules Defines
-----------------------------------------------------------------------------*/
#define CRYP_MODULE_NAME	"CRYP HCL Module"

/* Cryp HCL version */
#define CRYP_HCL_VERSION_ID	1
#define CRYP_HCL_MAJOR_ID  	1
#define CRYP_HCL_MINOR_ID	6


/*---------------------------------------------------------------------------
	Error Types
-----------------------------------------------------------------------------*/
typedef enum
{
        CRYP_OK				    = HCL_OK,
        CRYP_UNSUPPORTED_HW		= HCL_UNSUPPORTED_HW,
        CRYP_INVALID_PARAMETER	= HCL_INVALID_PARAMETER,
        CRYP_UNSUPPORTED_FEATURE= HCL_UNSUPPORTED_FEATURE,
        CRYP_LOGIC_BUSY			= HCL_MAX_ERROR_VALUE - 1
} t_cryp_error;

/* CRYP Enable/disable*/
typedef enum
{
    CRYP_ACTIVITY_DISABLE	= 0,
    CRYP_ACTIVITY_ENABLE	= 1
}t_cryp_activity;

/* CRYP Start Computation Enable/Disable*/
typedef enum
{
    CRYP_START_DISABLE	= 0,
    CRYP_START_ENABLE	= 1
}t_cryp_start;

/* CRYP Init Signal Enable/disable*/
typedef enum
{
    CRYP_INIT_DISABLE	= 0,
    CRYP_INIT_ENABLE	= 1
}t_cryp_init;

/* Cryp State Enable/Disable */
typedef enum 
{
    CRYP_STATE_DISABLE	= 0,
    CRYP_STATE_ENABLE 	= 1
}t_cryp_state;

/* Key preparation bit enable */
typedef  enum
{
    KSE_DISABLED,
    KSE_ENABLED
}t_cryp_key_prep;

/* Key size */
typedef  enum
{
    CRYP_KEY_SIZE_128,
    CRYP_KEY_SIZE_192,
    CRYP_KEY_SIZE_256
} t_cryp_key_size;

/* Data type Swap */
typedef  enum
{
    CRYP_DATA_TYPE_32BIT_SWAP = 0x00,
    CRYP_DATA_TYPE_16BIT_SWAP = 0x01,
    CRYP_DATA_TYPE_8BIT_SWAP  = 0x02,
    CRYP_DATA_TYPE_BIT_SWAP   = 0x03
} t_cryp_data_type;

/* AES modes */
typedef  enum
{
    CRYP_ALGO_TDES_ECB,
    CRYP_ALGO_TDES_CBC,
    CRYP_ALGO_DES_ECB,
    CRYP_ALGO_DES_CBC,
    CRYP_ALGO_AES_ECB,
    CRYP_ALGO_AES_CBC,
    CRYP_ALGO_AES_CTR,
    CRYP_ALGO_AES_XTS
}t_cryp_algo_mode;

/* Cryp Encryption or Decryption */
typedef  enum
{
    CRYP_ALGORITHM_ENCRYPT,
    CRYP_ALGORITHM_DECRYPT
}t_cryp_algorithm_dir;

/* CRYP configuration structure to be passed to set configuration */
typedef struct
{
    t_cryp_state   		 key_access;
    t_cryp_key_size 	 key_size;
    t_cryp_data_type     data_type;
    t_cryp_algo_mode     algo_mode;
    t_cryp_algorithm_dir encrypt_or_decrypt;
}t_cryp_config;

/* Protection configuration structure for setting privilage access */
typedef struct
{
    t_cryp_state    privilege_access;
    t_cryp_state    secure_access;
} t_cryp_protection_config;

/* Cryp status */
typedef enum
{
    CRYP_STATUS_BUSY			        = 0x10,
    CRYP_STATUS_OUTPUT_FIFO_FULL 	    = 0x08,
    CRYP_STATUS_OUTPUT_FIFO_NOT_EMPTY   = 0x04,
    CRYP_STATUS_INPUT_FIFO_NOT_FULL     = 0x02,
    CRYP_STATUS_INPUT_FIFO_EMPTY        = 0x01
} t_cryp_status_id;

/* Cryp device status */
typedef t_uint32 t_cryp_status;

/* Cryp DMA interface */
typedef enum 
{
    CRYP_DMA_DISABLE_BOTH           = 0x0,
    CRYP_DMA_ENABLE_IN_DATA         = 0x1,
    CRYP_DMA_ENABLE_OUT_DATA        = 0x2,
    CRYP_DMA_ENABLE_BOTH_DIRECTION  = 0x3
}t_cryp_dma_req_type;

/* Key registers */
typedef enum
{
	CRYP_KEY_REG_1,
    CRYP_KEY_REG_2,
    CRYP_KEY_REG_3,
    CRYP_KEY_REG_4
}t_cryp_key_reg_index;

/* Key register left and right */
typedef struct
{	 
	t_uint32	key_value_left;
	t_uint32	key_value_right;	
}t_cryp_key_value;

/* Cryp Initialization structure */
typedef enum
{	 
	CRYP_INIT_VECTOR_INDEX_0,
	CRYP_INIT_VECTOR_INDEX_1
}t_cryp_init_vector_index;

typedef struct
{	 
	t_uint32	init_value_left;
	t_uint32	init_value_right;
}t_cryp_init_vector_value;

/* Stealing mode for AES_XTS / AES_CBC mode (computing for non-multiple of 128 bits) */
typedef enum
{
    CRYP_STEALING_DISABLE	= 0,
    CRYP_STEALING_ENABLE	= 1
}t_cryp_stealing;

/* indicates 2nd last input block for AES_XTS */
typedef enum
{
    CRYP_SECOND_LAST_DISABLE = 0,
    CRYP_SECOND_LAST_ENABLE	 = 1
}t_cryp_secondlast;

/* AES Keys Lock - to protect key registers against un-authorized access  */
typedef enum
{
    CRYP_KEY_LOCK_DISABLE	= 0,
    CRYP_KEY_LOCK_ENABLE	= 1
}t_cryp_key_lock;


/*-----------------------------------------------------------------------------
	Initialization functions
-----------------------------------------------------------------------------*/

PUBLIC	t_cryp_error CRYP_Init (IN t_cryp_device_id, IN t_logical_address);
PUBLIC	t_cryp_error CRYP_SetDebugLevel		(IN t_dbg_level);
PUBLIC  t_cryp_error CRYP_GetVersion		(OUT t_version *);
PUBLIC  void CRYP_Reset						(IN t_cryp_device_id);
PUBLIC  t_cryp_error CRYP_Activity      	(IN t_cryp_device_id,IN t_cryp_activity);
PUBLIC  t_cryp_error CRYP_Start         	(IN t_cryp_device_id,IN t_cryp_start);
PUBLIC  t_cryp_error CRYP_InitSignal      	(
                                            IN t_cryp_device_id,
											IN t_cryp_init
									     	);

PUBLIC  t_cryp_error CRYP_KeyPreparation   (IN t_cryp_device_id, 
                                             IN t_cryp_key_prep
                                            );
									     	
PUBLIC  t_cryp_error CRYP_FlushInOutFifo	(IN t_cryp_device_id);

/*-----------------------------------------------------------------------------
    Configuration funtion
-----------------------------------------------------------------------------*/

PUBLIC	t_cryp_error    CRYP_SetConfiguration	(
												IN t_cryp_device_id,
												IN t_cryp_config *
												);
												
PUBLIC	t_cryp_error    CRYP_GetConfiguration	(
												IN t_cryp_device_id,
												IN t_cryp_config *
												);
												
PUBLIC	t_cryp_error    CRYP_ConfigureForDma    (
                                                IN t_cryp_device_id,
                                                IN  t_cryp_dma_req_type
                                                );
                                                
PUBLIC	t_cryp_error	CRYP_ConfigureKeyValues (
                                                IN t_cryp_device_id,
                                                IN t_cryp_key_reg_index,
                                                IN t_cryp_key_value
                                                );
                                                
PUBLIC	t_cryp_error	CRYP_ConfigureInitVector(
                                                IN t_cryp_device_id,
                                                IN t_cryp_init_vector_index,
                                                IN t_cryp_init_vector_value
                                                );  

PUBLIC  t_cryp_error	CRYP_ConfigureProtection(
                                                IN t_cryp_device_id,
                                                IN t_cryp_protection_config *
                                                );

#if ((defined ST_HREFV2) || (defined ST_8500V2) || (defined __PEPS_8500_V2))
PUBLIC  t_cryp_error  	CRYP_SetKeyLock			(
												IN t_cryp_device_id cryp_device_id,
												IN t_cryp_key_lock cryp_keylock
												);
#endif

PUBLIC t_cryp_error CRYP_SetStealingMode		(
												IN t_cryp_device_id cryp_device_id,
												t_cryp_stealing cryp_stealing
												);
												
PUBLIC t_cryp_error CRYP_SetSecondLastBlock		(
												IN t_cryp_device_id cryp_device_id, 
												t_cryp_secondlast cryp_secondlast
												);

/*-----------------------------------------------------------------------------
    Power management funtions
-----------------------------------------------------------------------------*/
PUBLIC	void		    CRYP_SaveDeviceContext      (IN t_cryp_device_id);
PUBLIC	void		    CRYP_RestoreDeviceContext   (IN t_cryp_device_id);

/*-----------------------------------------------------------------------------
    Data transfer and status bits.
-----------------------------------------------------------------------------*/
												
PUBLIC  t_bool 	        CRYP_IsLogicBusy        (IN t_cryp_device_id);
PUBLIC	t_cryp_status   CRYP_GetStatus	        (IN t_cryp_device_id);
PUBLIC	t_cryp_error    CRYP_WriteInData	    (IN t_cryp_device_id, IN t_uint32);
PUBLIC	t_cryp_error    CRYP_ReadInData	        (IN t_cryp_device_id, OUT t_uint32 *);
PUBLIC	t_cryp_error    CRYP_ReadOutData	    (IN t_cryp_device_id, OUT t_uint32 *);

#ifdef __cplusplus
} /*Allow C++ to use this header */
#endif  /* __cplusplus   */

#endif	/* _CRYP_H_*/
