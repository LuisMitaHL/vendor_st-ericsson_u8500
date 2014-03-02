/*****************************************************************************/
/**
*  © ST-Ericsson, 2009 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
*  Public Header file of HASH Processor
*  Specification release related to this implementation: A_V2.2
*  AUTHOR :  ST-Ericsson
*/
/*****************************************************************************/

#ifndef _HASH_H_
#define _HASH_H_

#ifdef __cplusplus
extern "C"
{
#endif

/*--------------------------------------------------------------------------*
 * Includes                                                                 *
 *--------------------------------------------------------------------------*/
#include "debug.h"

/*--------------------------------------------------------------------------*
 * Defines                                                                  *
 *--------------------------------------------------------------------------*/
/* Number of bytes in the message digest */
#define HASH_MSG_DIGEST_SIZE    32
#define HASH_BLOCK_SIZE         64

typedef struct
{
    t_uint32    hash_high_word;
    t_uint32    hash_low_word;
} t_hash_uint64;

/* HASH device ID */
typedef enum
{
    HASH_DEVICE_ID_0 = 0,
    HASH_DEVICE_ID_1  
} t_hash_device_id;


/*------------------*
 * HASH Data Format *   
 *------------------*/
typedef enum
{
    HASH_DATA_32_BITS   = 0x0,
    HASH_DATA_16_BITS   = 0x1,
    HASH_DATA_8_BITS    = 0x2,
    HASH_DATA_1_BIT     = 0x3
} t_hash_data_format;

/* Device state */
typedef enum
{
    HASH_STATE_DISABLE  = 0,
    HASH_STATE_ENABLE   = 1
} t_hash_device_state;

/* Device protection configuration */
typedef struct
{
    t_hash_device_state hash_privilege_access;
    t_hash_device_state hash_secure_access;
} t_hash_protection_config;

/* Data Input flag status */
typedef enum
{
    HASH_DIN_EMPTY  = 0,
    HASH_DIN_FULL   = 1
} t_hash_dinf_status;

/* Number of words already pushed */
typedef enum
{
    HASH_NBW_00 = 0x00,
    HASH_NBW_01 = 0x01,
    HASH_NBW_02 = 0x02,
    HASH_NBW_03 = 0x03,
    HASH_NBW_04 = 0x04,
    HASH_NBW_05 = 0x05,
    HASH_NBW_06 = 0x06,
    HASH_NBW_07 = 0x07,
    HASH_NBW_08 = 0x08,
    HASH_NBW_09 = 0x09,
    HASH_NBW_10 = 0x0A,
    HASH_NBW_11 = 0x0B,
    HASH_NBW_12 = 0x0C,
    HASH_NBW_13 = 0x0D,
    HASH_NBW_14 = 0x0E,
    HASH_NBW_15 = 0x0F
} t_hash_nbw_status;

/* Device status ie DINF, NBW, and NBLW bit fields */
typedef struct
{
    t_hash_dinf_status  hash_dinf_status;   /* HASH_DIN Full Flag */
    t_hash_nbw_status   hash_nbw_status;    /* Number of words already pushed */
    t_uint8             hash_nblw_status;   /* Number of Valid Bits in Last Word of the Message */
} t_hash_device_status;

/* Enumeration for HASH DMA Request types */
typedef enum
{
    HASH_DISABLE_DMA_REQ    = 0x0,
    HASH_ENABLE_DMA_REQ     = 0x1
} t_hash_dma_req;

/* Enumeration for Digest calculation */
typedef enum
{
    HASH_DISABLE_DCAL   = 0x0,
    HASH_ENABLE_DCAL    = 0x1
} t_hash_dcal_state;


/* Enumeration for selecting between SHA-1 or SHA2 algorithm */
typedef enum
{
    HASH_ALGO_SHA1  = 0x0,
    HASH_ALGO_SHA2  = 0x1
} t_hash_algorithm;

/* Enumeration for selecting between HASH or HMAC mode for the selected algorithm */
typedef enum
{
    HASH_OPER_MODE_HASH = 0x0,
    HASH_OPER_MODE_HMAC = 0x1
} t_hash_oper_mode;

/*------------------------------------------------------*
 * Enumeration for selecting between short key          *
 * (<= 64 bytes) or long key (>64 bytes) in HMAC mode   *
 *------------------------------------------------------*/
typedef enum
{
    HASH_SHORT_KEY  = 0x0,
    HASH_LONG_KEY   = 0x1
} t_hash_hamac_key;

/*----------------------------------*
 * Configuration of HASH Controller *
 *----------------------------------*/
typedef struct
{
    t_hash_data_format  hash_config_data_format;    /* Format of data entered in the HASH_DIN reg */
    t_hash_algorithm    hash_config_algorithm;      /* Algorithm Selection bit */
    t_hash_oper_mode    hash_config_oper_mode;      /* Operating Mode Selection bit */
    t_hash_hamac_key    hash_config_hmac_key;       /* Long Key Selection bit in HMAC mode */
} t_hash_config;

/*--------------------------------------------------------------------------*
 * WARNING: "hash_state_index" IS THE  MEMBER OF THE STRUCTURE, TO BE SURE  *
 * THAT "hash_state_buffer" IS ALIGNED ON A 4-BYTES BOUNDARY.               *
 * THIS IS ****HIGHLY**** IMPLEMENTATION DEPENDENT, AND                     *
 * ****MUST**** BE CHECKED WHENEVER THIS CODE IS PORTED ON NEW PLATFORMS    *
 *--------------------------------------------------------------------------*/
typedef struct
{
    t_uint32        hash_state_temp_cr;                         /* Temperory HASH Control Register */
    t_uint32        hash_state_str_reg;                         /* HASH Start Register */
    t_uint32        hash_state_din_reg;                         /* HASH Data Input Register */
    t_uint32        hash_state_csr[52];                         /* HASH Context Swap Registers 0-39 */
    t_uint32        hash_state_csfull;                           /* HASH Context Swap Registers 40 ie Status flags */
    t_uint32        hash_state_csdatain;                           /* HASH Context Swap Registers 41 ie Input data */

    /*------------------------------------------------------------------*
     * 'hash_state_buffer' contains part of the message already copied  *
     *  from the user but not already involved in the HW computation    *
     *------------------------------------------------------------------*/
    t_uint32        hash_state_buffer[HASH_BLOCK_SIZE / sizeof(t_uint32)];

    /* Length of the part of the message hashed so far ( floor(N/64) * 64) */
    t_hash_uint64   hash_state_length;

    /*------------------------------------------------------------------*
     * Valid number of bytes in buffer (N % 64, where N is the number   *
     * of bytes of the message passed by the user so far)               *
     *------------------------------------------------------------------*/
    t_uint8         hash_state_index;

    /*------------------------------------------------------------------*
     * Valid number of bits in buffer (N % 8, where N is the number     *
     * of bits of the message passed by the user so far)                *
     *------------------------------------------------------------------*/
    t_uint8         hash_state_bit_index;
} t_hash_state;

/* Errors related values *
 *-----------------------*/
typedef enum
{
    HASH_MSG_LENGTH_OVERFLOW    = HCL_MAX_ERROR_VALUE,          /* Value : (-65) */
    HASH_INTERNAL_ERROR         = HCL_INTERNAL_ERROR,           /* Value :  (-8) */
    HASH_NOT_CONFIGURED         = HCL_NOT_CONFIGURED,           /* Value :  (-7) */
    HASH_REQUEST_PENDING        = HCL_REQUEST_PENDING,          /* Value :  (-6) */
    HASH_REQUEST_NOT_APPLICABLE = HCL_REQUEST_NOT_APPLICABLE,   /* Value :  (-5) */
    HASH_INVALID_PARAMETER      = HCL_INVALID_PARAMETER,        /* Value :  (-4) */
    HASH_UNSUPPORTED_FEATURE    = HCL_UNSUPPORTED_FEATURE,      /* Value :  (-3) */
    HASH_UNSUPPORTED_HW         = HCL_UNSUPPORTED_HW,           /* Value :  (-2) */
    HASH_OK                     = HCL_OK                        /* Value :  ( 0) */
} t_hash_error;

/*--------------------------------------------------------------------------*
 * Functions                                                                *
 *--------------------------------------------------------------------------*/
/* Initialization functions */
PUBLIC t_hash_error         HASH_Init(IN t_hash_device_id hash_device_id, IN t_logical_address hash_base_address);
/* Version Management functions */
PUBLIC t_hash_error         HASH_GetVersion(OUT t_version *p_version);

#ifdef __DEBUG
/* Debugging functions */
PUBLIC t_hash_error         HASH_SetDbgLevel(IN t_dbg_level debug_level);
PUBLIC t_hash_error         HASH_GetDbgLevel(OUT t_dbg_level *p_debug_level);
#endif /* End __DEBUG */

/* Reset functions */
PUBLIC t_hash_error         HASH_Reset(IN t_hash_device_id hash_devive_id);
/* Configuration and Mode setting functions */
PUBLIC t_hash_error         HASH_ConfigureDmaRequest(IN t_hash_device_id hash_device_id, IN t_hash_dma_req request_state);
PUBLIC t_hash_error         HASH_ConfigureLastValidBits(IN t_hash_device_id hash_device_id, IN t_uint8 nblw_val);
PUBLIC t_hash_error         HASH_ConfigureDigestCal(IN t_hash_device_id hash_device_id, IN t_hash_dcal_state hash_dcal_state);
PUBLIC t_hash_error         HASH_ConfigureProtection(IN t_hash_device_id hash_device_id, IN t_hash_protection_config *p_protect_config);
PUBLIC t_hash_error         HASH_SetConfiguration(IN t_hash_device_id hash_device_id, IN t_hash_config *p_config);
PUBLIC t_hash_error         HASH_Begin(IN t_hash_device_id hash_device_id);
PUBLIC t_hash_error         HASH_ReadMessageDigest(IN t_hash_device_id hash_device_id, OUT t_uint8 digest[HASH_MSG_DIGEST_SIZE]);
PUBLIC t_hash_error         HASH_ClockGatingOff(IN t_hash_device_id hash_device_id);

/* Status functions */
PUBLIC t_hash_device_status HASH_GetDeviceStatus(IN t_hash_device_id hash_device_id);
PUBLIC t_bool               HASH_IsDcalOngoing(IN t_hash_device_id hash_device_id);

#ifdef __HASH_ELEMENTARY    /* Elementary layer APIs */
PUBLIC t_hash_error         HASH_SetInputData(IN t_hash_device_id hash_device_id, IN t_uint32 input_data);
PUBLIC t_hash_error         HASH_GetDataInput(IN t_hash_device_id hash_device_id, OUT t_uint32 *p_data_input);
#endif /* End __HASH_ELEMENTARY */

#ifdef __HASH_ENHANCED      /* Enhanced layer APIs */
PUBLIC t_hash_error         HASH_Update(IN t_hash_device_id hash_device_id, IN const t_uint8 *p_data_buffer, IN t_uint32 msg_length);
PUBLIC t_hash_error         HASH_End(IN t_hash_device_id hash_device_id, OUT t_uint8 digest[HASH_MSG_DIGEST_SIZE]);
PUBLIC t_hash_error         HASH_Compute
                            (
                                IN t_hash_device_id hash_device_id,
                                IN const t_uint8    *p_data_buffer,
                                IN t_uint32         msg_length,
                                IN t_hash_config    *p_hash_config,
                                OUT t_uint8         digest[HASH_MSG_DIGEST_SIZE]
                            );
#endif /* End __HASH_ENHANCED */

/* Power Management functions */
PUBLIC t_hash_error         HASH_SaveDeviceState(IN t_hash_device_id hash_device_id, OUT t_hash_state *hash_state);
PUBLIC t_hash_error         HASH_ResumeDeviceState(IN t_hash_device_id hash_device_id, IN const t_hash_state *hash_state);
#ifdef __cplusplus
}       /* Allow C++ to use these headers */
#endif  /* __cplusplus*/
#endif  /* _HASH_H_ */

/* End of file - hash.h */

