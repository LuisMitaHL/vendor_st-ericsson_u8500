/*****************************************************************************/
/**
*  � ST-Ericsson, 2009 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* This module provides some support routines for the MultiMedia Card
* SD-card host Interface.
*
* author : ST-Ericsson
*/
/*****************************************************************************/

#ifndef _MMC_H_
#define _MMC_H_

#ifdef __cplusplus
extern "C"
{
#endif

/*---------------------------------------------------------------------------
 * Includes
 *---------------------------------------------------------------------------*/
#include "debug.h"
#include "mmc_irq.h"
#include "t_basicdefinitions.h"

/*---------------------------------------------------------------------------
 * Define
 *---------------------------------------------------------------------------*/
/*Defines for Version */
#define MMC_HCL_VERSION_ID   1
#define MMC_HCL_MAJOR_ID     6
#define MMC_HCL_MINOR_ID     0


/*---------------------------------------------------------------------------
 * Enums
 *---------------------------------------------------------------------------*/
typedef enum
{
    /* MMC specific error defines */
    MMC_CMD_CRC_FAIL                    = (HCL_MAX_ERROR_VALUE - 1),            /* Command response received (but CRC check failed) */
    MMC_DATA_CRC_FAIL                   = (HCL_MAX_ERROR_VALUE - 2),            /* Data bock sent/received (CRC check Failed) */
    MMC_CMD_RSP_TIMEOUT                 = (HCL_MAX_ERROR_VALUE - 3),            /* Command response timeout */
    MMC_DATA_TIMEOUT                    = (HCL_MAX_ERROR_VALUE - 4),            /* Data time out*/
    MMC_TX_UNDERRUN                     = (HCL_MAX_ERROR_VALUE - 5),            /* Transmit FIFO under-run */
    MMC_RX_OVERRUN                      = (HCL_MAX_ERROR_VALUE - 6),            /* Receive FIFO over-run */
    MMC_START_BIT_ERR                   = (HCL_MAX_ERROR_VALUE - 7),            /* Start bit not detected on all data signals in widE bus mode */
    MMC_CMD_OUT_OF_RANGE                = (HCL_MAX_ERROR_VALUE - 8),            /* CMD's argument was out of range.*/
    MMC_ADDR_MISALIGNED                 = (HCL_MAX_ERROR_VALUE - 9),            /* Misaligned address */
    MMC_BLOCK_LEN_ERR                   = (HCL_MAX_ERROR_VALUE - 10),           /* Transferred block length is not allowed for the card or the number of transferred bytes does not match the block length */
    MMC_ERASE_SEQ_ERR                   = (HCL_MAX_ERROR_VALUE - 11),           /* An error in the sequence of erase command occurs.*/
    MMC_BAD_ERASE_PARAM                 = (HCL_MAX_ERROR_VALUE - 12),           /* An Invalid selection for erase groups */
    MMC_WRITE_PROT_VIOLATION            = (HCL_MAX_ERROR_VALUE - 13),           /* Attempt to program a write protect block */
    MMC_LOCK_UNLOCK_FAILED              = (HCL_MAX_ERROR_VALUE - 14),           /* Sequence or password error has been detected in unlock command or if there was an attempt to access a locked card */
    MMC_COM_CRC_FAILED                  = (HCL_MAX_ERROR_VALUE - 15),           /* CRC check of the previous command failed */
    MMC_ILLEGAL_CMD                     = (HCL_MAX_ERROR_VALUE - 16),           /* Command is not legal for the card state */
    MMC_CARD_ECC_FAILED                 = (HCL_MAX_ERROR_VALUE - 17),           /* Card internal ECC was applied but failed to correct the data */
    MMC_CC_ERROR                        = (HCL_MAX_ERROR_VALUE - 18),           /* Internal card controller error */
    MMC_GENERAL_UNKNOWN_ERROR           = (HCL_MAX_ERROR_VALUE - 19),           /* General or Unknown error */
    MMC_STREAM_READ_UNDERRUN            = (HCL_MAX_ERROR_VALUE - 20),           /* The card could not sustain data transfer in stream read operation. */
    MMC_STREAM_WRITE_OVERRUN            = (HCL_MAX_ERROR_VALUE - 21),           /* The card could not sustain data programming in stream mode */
    MMC_CID_CSD_OVERWRITE               = (HCL_MAX_ERROR_VALUE - 22),           /* CID/CSD overwrite error */
    MMC_WP_ERASE_SKIP                   = (HCL_MAX_ERROR_VALUE - 23),           /* only partial address space was erased */
    MMC_CARD_ECC_DISABLED               = (HCL_MAX_ERROR_VALUE - 24),           /* Command has been executed without using internal ECC */
    MMC_ERASE_RESET                     = (HCL_MAX_ERROR_VALUE - 25),           /* Erase sequence was cleared before executing because an out of erase sequence command was received */
    MMC_AKE_SEQ_ERROR                   = (HCL_MAX_ERROR_VALUE - 26),           /* Error in sequence of authentication. */
    MMC_INVALID_VOLTRANGE               = (HCL_MAX_ERROR_VALUE - 27),
    MMC_ADDR_OUT_OF_RANGE               = (HCL_MAX_ERROR_VALUE - 28),
    MMC_SWITCH_ERROR                    = (HCL_MAX_ERROR_VALUE - 29),
    MMC_SDIO_DISABLED                   = (HCL_MAX_ERROR_VALUE - 30),
    MMC_SDIO_FUNCTION_BUSY              = (HCL_MAX_ERROR_VALUE - 31),
    MMC_SDIO_FUNCTION_FAILED            = (HCL_MAX_ERROR_VALUE - 32),
    MMC_PARTITION_NOT_SUPPORTED         = (HCL_MAX_ERROR_VALUE - 33),
    MMC_PARTITION_ALREADY_DONE			= (HCL_MAX_ERROR_VALUE - 34),
    MMC_SDIO_UNKNOWN_FUNCTION           = HCL_MAX_ERROR_VALUE,

    /* standard error defines */
    MMC_INTERNAL_ERROR                  = HCL_INTERNAL_ERROR,                   /*(-8)*/
    MMC_NOT_CONFIGURED                  = HCL_NOT_CONFIGURED,                   /*(-7)*/
    MMC_REQUEST_PENDING                 = HCL_REQUEST_PENDING,                  /*(-6) */
    MMC_REQUEST_NOT_APPLICABLE          = HCL_REQUEST_NOT_APPLICABLE,           /*(-5)*/
    MMC_INVALID_PARAMETER               = HCL_INVALID_PARAMETER,                /*(-4)*/
    MMC_UNSUPPORTED_FEATURE             = HCL_UNSUPPORTED_FEATURE,              /*(-3)*/
    MMC_UNSUPPORTED_HW                  = HCL_UNSUPPORTED_HW,                   /*(-2)*/
    MMC_ERROR                           = HCL_ERROR,                            /*(-1)*/
    MMC_OK                              = HCL_OK,                               /*0*/
    MMC_INTERNAL_EVENT                  = HCL_INTERNAL_EVENT,                   /*(1)*/
    MMC_REMAINING_PENDING_EVENTS        = HCL_REMAINING_PENDING_EVENTS,         /*(2)*/
    MMC_REMAINING_FILTER_PENDING_EVENTS = HCL_REMAINING_FILTER_PENDING_EVENTS,  /*(3)*/
    MMC_NO_MORE_PENDING_EVENT           = HCL_NO_MORE_PENDING_EVENT,            /*(4)*/
    MMC_NO_MORE_FILTER_PENDING_EVENT    = HCL_NO_MORE_FILTER_PENDING_EVENT,     /*(5)*/
    MMC_NO_PENDING_EVENT_ERROR          = HCL_NO_PENDING_EVENT_ERROR            /*(7)*/
} t_mmc_error;

TYPEDEF_ENUM
{
    /* Result defines */
    OP_OK             = 0x0000,  /* Operation OK */
    GEN_FAILURE       = 0x0001,  /* General failure */
    AUTH_FAILURE      = 0x0002,  /* Authentication failure (MAC comparison not matching, MAC calculation failure) */
    CNT_FAILURE       = 0x0003,  /* Counter failure (counters not matching in comparison, counter incrementing failure) */
    ADDR_FAILURE      = 0x0004,  /* Address failure (address out of range, wrong address alignment) */
    WRITE_FAILURE     = 0x0005,  /* Write failure (data/counter/result write failure) */
    READ_FAILURE      = 0x0006,  /* Read failure (data/counter/result read failure) */
    AUTH_KEY_NOT_PRG  = 0x0007,  /* Authentication Key not yet programmed */
    WRITE_CNT_EXPIRED = 0x0080  /* Write Counter has expired, all error codes are shifted for 0x80 */
}ENUM16(t_rpmb_error);

typedef struct
{
    t_mmc_error  mmc_error;
    t_rpmb_error rpmb_error;
} t_emmc_error;

typedef enum
{
    MMC_DISABLE                         = 0,
    MMC_ENABLE
} t_mmc_state;

typedef enum
{
    MMC_1_BIT_WIDE                      = 0,
    MMC_4_BIT_WIDE,
    MMC_8_BIT_WIDE
} t_mmc_wide_mode;

#if defined(__MMCI_HCL_ELEMENTARY)
typedef enum
{
    MMC_POWER_OFF                       = 0x0,
    MMC_POWER_UP                        = 0x2,
    MMC_POWER_ON                        = 0x3
} t_mmc_power_state;

typedef enum
{
    MMC_PUSH_PULL                       = 0,
    MMC_OPEN_DRAIN
} t_mmc_bus_mode;

typedef enum
{
    MMC_GO_IDLE_STATE                       = 0,
    MMC_SEND_OP_COND                        = 1,
    MMC_ALL_SEND_CID                        = 2,
    MMC_SET_REL_ADDR                        = 3,                                /* MMC_SEND_REL_ADDR for SD-card.*/
    MMC_SET_DSR                             = 4,
    MMC_SDIO_SEN_OP_COND                    = 5,
    MMC_HS_SWITCH                           = 6,
    MMC_SEL_DESEL_CARD                      = 7,
    MMC_HS_SEND_EXT_CSD                     = 8,
    MMC_SEND_CSD                            = 9,
    MMC_SEND_CID                            = 10,
    MMC_READ_DAT_UNTIL_STOP                 = 11,                               /* SD Card doesn't support it.*/
    MMC_STOP_TRANSMISSION                   = 12,
    MMC_SEND_STATUS                         = 13,
    MMC_HS_BUSTEST_READ                     = 14,
    MMC_GO_INACTIVE_STATE                   = 15,
    MMC_SET_BLOCKLEN                        = 16,
    MMC_READ_SINGLE_BLOCK                   = 17,
    MMC_READ_MULT_BLOCK                     = 18,
    MMC_HS_BUSTEST_WRITE                    = 19,
    MMC_WRITE_DAT_UNTIL_STOP                = 20,                               /* SD Card doesn't support it.*/
    MMC_SET_BLOCK_COUNT                     = 23,                               /* SD Card doesn't support it.*/
    MMC_WRITE_SINGLE_BLOCK                  = 24,
    MMC_WRITE_MULT_BLOCK                    = 25,
    MMC_PROG_CID                            = 26,                               /* reserved for manufacturers.*/
    MMC_PROG_CSD                            = 27,
    MMC_SET_WRITE_PROT                      = 28,
    MMC_CLR_WRITE_PROT                      = 29,
    MMC_SEND_WRITE_PROT                     = 30,
    MMC_SD_ERASE_GRP_START                  = 32,                               /*To set the address of the first write 
       										  block to be erased. (For SD card only)*/
    MMC_SD_ERASE_GRP_END                    = 33,                               /*To set the address of the last write 
	   										  block of the continuous range to be erased. (For SD card only)*/
    MMC_ERASE_GRP_START                     = 35,                               /*To set the address of the first write block to be erased.
       										 (For MMC card only spec 3.31)*/

    MMC_ERASE_GRP_END                       = 36,                               /*To set the address of the last write block of the
	   										  continuous range to be erased. (For MMC card only spec 3.31)*/

    MMC_ERASE                               = 38,
    MMC_FAST_IO                             = 39,                               /* SD Card doesn't support it.*/
    MMC_GO_IRQ_STATE                        = 40,                               /* SD Card doesn't support it.*/
    MMC_LOCK_UNLOCK                         = 42,
    MMC_APP_CMD                             = 55,
    MMC_GEN_CMD                             = 56,
    MMC_NO_CMD                              = 64,

    /*Following commands are SD Card Specific commands. MMC_APP_CMD should be sent before sending these commands.*/
    MMC_APP_SD_SET_BUSWIDTH                 = 6,                                /* For SD Card only.*/
    MMC_SD_APP_STAUS                        = 13,                               /* For SD Card only.*/
    MMC_SD_APP_SEND_NUM_WRITE_BLOCKS        = 22,                               /* For SD Card only.*/
    MMC_SD_APP_OP_COND                      = 41,                               /* For SD Card only.*/
    MMC_SD_APP_SET_CLR_CARD_DETECT          = 42,                               /* For SD Card only.*/
    MMC_SD_APP_SEND_SCR                     = 51,                               /* For SD Card only.*/

    MMC_SDIO_RW_DIRECT                      = 52,                               /* For SDIO Card only. */
    MMC_SDIO_RW_EXTENDED                    = 53,                               /* For SDIO Card only. */

    /* Following commands are SD Card Specific security commands.MMC_APP_CMD should be sent before sending these commands.*/
    MMC_SD_APP_GET_MKB                      = 43,                               /* For SD Card only.*/
    MMC_SD_APP_GET_MID                      = 44,                               /* For SD Card only.*/
    MMC_SD_APP_SET_CER_RN1                  = 45,                               /* For SD Card only.*/
    MMC_SD_APP_GET_CER_RN2                  = 46,                               /* For SD Card only.*/
    MMC_SD_APP_SET_CER_RES2                 = 47,                               /* For SD Card only.*/
    MMC_SD_APP_GET_CER_RES1                 = 48,                               /* For SD Card only.*/
    MMC_SD_APP_SECURE_READ_MULTIPLE_BLOCK   = 18,                               /* For SD Card only.*/
    MMC_SD_APP_SECURE_WRITE_MULTIPLE_BLOCK  = 25,                               /* For SD Card only.*/
    MMC_SD_APP_SECURE_ERASE                 = 38,                               /* For SD Card only.*/
    MMC_SD_APP_CHANGE_SECURE_AREA           = 49,                               /* For SD Card only.*/
    MMC_SD_APP_SECURE_WRITE_MKB             = 48                                /* For SD Card only.*/
} t_mmc_command_index;

typedef enum
{
    MMC_SHORT_RESP                          = 0,
    MMC_LONG_RESP
} t_mmc_response_type;

typedef enum
{
    MMC_WRITE                               = 0,
    MMC_READ
} t_mmc_transfer_direction;

typedef enum
{
    MMC_BLOCK                               = 0,
    MMC_STREAM
} t_mmc_transfer_type;

typedef enum
{
    MMC_BYTE_ADDRESSABLE 					= 0,
	MMC_SECT_ADDRESSABLE
}t_mmc_address_mode;
/* For SDIO card support */
typedef enum
{
    MMC_READ_WAIT_CONTROLLING_CLK           = 0,
    MMC_READ_WAIT_CONTROLLING_DAT2
} t_mmc_read_wait_mode;
#endif
#if defined(__MMCI_HCL_ENHANCED)
typedef enum
{
    MMC_POLLING_MODE                        = 0,
    MMC_INTERRUPT_MODE,
    MMC_DMA_MODE
} t_mmc_device_mode;
#endif

/*---------------------------------------------------------------------------
 * Structures
 *---------------------------------------------------------------------------*/
typedef struct
{
    t_uint32    card_ready_after_init;
    t_uint32    no_of_io_funcs;
    t_uint32    memory_present;
    t_uint32    op_cond_register;
} t_mmc_sdio_resp4;

#if defined(__MMCI_HCL_ELEMENTARY)
typedef struct
{
    t_mmc_state pwr_save;
    t_mmc_state bypass;
	t_mmc_state hs_enable;
} t_mmc_clock_control;

typedef struct
{
    t_bool      is_resp_expected;
    t_bool      is_long_resp;
    t_bool      is_interrupt_mode;
    t_bool      is_pending;
    t_mmc_state cmd_path;
} t_mmc_command_control;
#endif
#if defined(__MMCI_HCL_ENHANCED)
typedef t_uint32    t_mmc_event;

#define t_mmc_filter_mode   t_uint32
#define NO_FILTER_MODE      0

typedef enum
{
    MMC_SDIO_READ                           = 0,
    MMC_SDIO_WRITE
} t_mmc_sdio_data_dir;

typedef struct
{
    t_uint32            card_addr;
    t_uint16            no_of_bytes;
    t_uint16            no_of_blocks;
    t_uint16            block_size;
    t_uint8             function_number;
    t_mmc_state         read_after_write;
    t_mmc_state         block_mode;
    t_mmc_state         incremental_transfer;
    t_mmc_sdio_data_dir read_write;
} t_mmc_sdio_transfer_info;

typedef enum
{
    MMC_IRQ_STATE_NEW                       = 0,
    MMC_IRQ_STATE_OLD
} t_mmc_irq_state;

typedef struct
{
    t_mmc_irq_state irq_state;
    t_uint32        initial_irq;
    t_uint32        pending_irq;
} t_mmc_irq_status;

typedef enum
{
    MMC_MULTIMEDIA_CARD,
    MMC_SECURE_DIGITAL_CARD,
    MMC_SECURE_DIGITAL_IO_CARD,
    MMC_HIGH_SPEED_MULTIMEDIA_CARD,
    MMC_SECURE_DIGITAL_IO_COMBO_CARD,
    MMC_HIGH_CAPACITY_SD_CARD,
    MMC_HIGH_CAPACITY_MMC_CARD
} t_mmc_card_type;

typedef struct
{
    t_uint32        cid[4];
    t_uint32        csd[4];
    t_uint16        rca;
    t_mmc_card_type card_type;
    t_uint8         sdio_cccr[4];   /* I/O ready, CCCR/SDIO revision, SD Specification revision, and Card Capability registers */
} t_mmc_card_info;

typedef struct
{
    t_uint16    transferred_bytes;
    t_mmc_error transfer_error;
} t_mmc_last_transfer_info;

typedef enum
{
    MMC_NO_TRANSFER                         = 0,
    MMC_TRANSFER_IN_PROGRESS
} t_mmc_transfer_state;

typedef enum
{
    MMC_WRITE_PROT_WHOLE_CARD_TEMP          = 0,
    MMC_WRITE_PROT_WHOLE_CARD_PERM,
    MMC_WRITE_PROT_SINGLE_GROUP
} t_mmc_write_protect_type;
#endif

/*---------------------------------------------------------------------------
 *  Functions Prototype                                                   
 *---------------------------------------------------------------------------*/
PUBLIC t_mmc_error              MMC_Init(t_logical_address, t_mmc_device_id);
PUBLIC t_mmc_error              MMC_SetDbgLevel(t_dbg_level);
PUBLIC t_mmc_error              MMC_GetVersion(t_version *);
PUBLIC t_mmc_error              MMC_SetClockFrequency(t_uint8 ,t_mmc_device_id);
PUBLIC t_mmc_error              MMC_SetFeedBackClock(t_mmc_state,t_mmc_device_id);
PUBLIC t_mmc_error              MMC_EnableDirSignals(t_mmc_state,t_mmc_device_id);

PUBLIC t_mmc_error              MMC_SaveDeviceContext(t_mmc_device_id);
PUBLIC t_mmc_error              MMC_RestoreDeviceContext(t_mmc_device_id);

#if defined(__MMCI_HCL_ELEMENTARY)
PUBLIC t_mmc_error              MMC_SetWideBus(t_mmc_wide_mode, t_mmc_device_id);
PUBLIC t_mmc_error              MMC_SetPowerState(t_mmc_power_state,t_mmc_device_id);
PUBLIC t_mmc_power_state        MMC_GetPowerState(t_mmc_device_id);
PUBLIC t_mmc_error              MMC_SetOperatingVoltage(t_uint8);
PUBLIC t_mmc_error              MMC_GetOperatingVoltage(t_uint8 *);
PUBLIC t_mmc_error              MMC_ConfigBus(t_mmc_bus_mode, t_mmc_device_id);

PUBLIC t_mmc_error              MMC_SetClockState(t_mmc_state, t_mmc_device_id);
PUBLIC t_mmc_error              MMC_ConfigClockControl(t_mmc_clock_control, t_mmc_device_id);

PUBLIC t_mmc_error              MMC_SendCommand(t_mmc_command_index, t_uint32, t_mmc_command_control, t_mmc_device_id);
PUBLIC t_mmc_command_index      MMC_GetCommandResponse(t_mmc_device_id);
PUBLIC t_mmc_error              MMC_GetResponse(t_mmc_response_type,t_mmc_device_id, t_uint32 *);

PUBLIC t_mmc_error              MMC_SetDataPath(t_mmc_state, t_mmc_device_id);
PUBLIC t_mmc_error              MMC_SetDataTimeOut(t_uint32,t_mmc_device_id);
PUBLIC t_mmc_error              MMC_SetDataLength(t_uint32,t_mmc_device_id);
#if (!((defined __PEPS_8500_V2)||(defined ST_8500V2)||(defined ST_HREFV2)))
PUBLIC t_mmc_error              MMC_SetDataBlockLength(t_uint8,t_mmc_device_id);
#else
PUBLIC t_mmc_error              MMC_SetDataBlockLength(t_uint16,t_mmc_device_id);
#endif
PUBLIC t_mmc_error              MMC_SetTransferDirection(t_mmc_transfer_direction,t_mmc_device_id);
PUBLIC t_mmc_error              MMC_SetTransferType(t_mmc_transfer_type,t_mmc_device_id);
PUBLIC t_mmc_error              MMC_HandleDMA(t_mmc_state,t_mmc_device_id);
PUBLIC t_uint16                 MMC_GetDataCounter(t_mmc_device_id);

PUBLIC t_mmc_error              MMC_ReadFIFO(t_mmc_device_id,t_uint32 *);
PUBLIC t_mmc_error              MMC_WriteFIFO(t_uint32, t_mmc_device_id);
PUBLIC t_uint16                 MMC_GetFIFOCount(t_mmc_device_id);

PUBLIC t_mmc_error              MMC_StartSdioReadWait(t_mmc_device_id);
PUBLIC t_mmc_error              MMC_StopSdioReadWait(t_mmc_device_id);
PUBLIC t_mmc_error              MMC_SetSdioReadWaitMode(t_mmc_read_wait_mode,t_mmc_device_id);
PUBLIC t_mmc_error              MMC_ResetSdioReadWait(t_mmc_device_id);
PUBLIC t_mmc_error              MMC_SetSdioOperation(t_mmc_state,t_mmc_device_id);
PUBLIC t_mmc_error              MMC_SendSdioSuspendCmd(t_mmc_state,t_mmc_device_id);

PUBLIC t_mmc_error              MMC_SetBOOTMode(t_mmc_state,t_mmc_device_id);
PUBLIC t_mmc_error				MMC_SetBUSYMode(IN t_mmc_state,t_mmc_device_id);
PUBLIC t_mmc_error              MMC_EnableHWFlowControl(IN t_mmc_state, IN t_mmc_device_id);
PUBLIC t_mmc_address_mode  		MMC_GetAddressingMode(t_mmc_device_id mmc_device_id);

#ifndef ST_8500_ED
PUBLIC t_mmc_error MMC_SetInvClock(IN t_mmc_state invert_clock, t_mmc_device_id mmc_device_id);
PUBLIC t_mmc_error MMC_EnableNegEdgeClock(IN t_mmc_state edge_status, t_mmc_device_id mmc_device_id);
PUBLIC t_mmc_error MMC_EnPowerReset(IN t_mmc_state reset_state, t_mmc_device_id mmc_device_id);
PUBLIC t_mmc_error MMC_EnableDDRMode(IN t_mmc_state ddr_mode_state, t_mmc_device_id mmc_device_id);
#endif
#endif
#ifdef __MMCI_HCL_ENHANCED
PUBLIC t_mmc_error              MMC_PowerON(t_mmc_device_id);
PUBLIC t_mmc_error              MMC_PowerOFF(t_mmc_device_id);
PUBLIC t_mmc_error              MMC_InitializeCards(t_mmc_device_id);
PUBLIC t_uint8                  MMC_GetNumOfActivecards(t_mmc_device_id);
PUBLIC t_mmc_error              MMC_GetCardInfo(t_uint8, t_mmc_device_id , t_mmc_card_info *);
PUBLIC t_mmc_error              MMC_EnableWideBusOperation(t_uint8, t_mmc_wide_mode, t_mmc_device_id);

PUBLIC t_mmc_error              MMC_SetIRQMode(t_mmc_state,t_mmc_device_id);
PUBLIC t_mmc_error              MMC_SetDeviceMode(t_mmc_device_mode,t_mmc_device_id);
PUBLIC t_mmc_error              MMC_SendStatus(t_uint8, t_uint32 *,t_mmc_device_id);

PUBLIC t_mmc_error              MMC_ReadBytes(t_uint8, t_uint32, t_uint32 *, t_uint32,t_mmc_device_id);
PUBLIC t_mmc_error              MMC_ReadBlocks(t_uint8, t_uint32, t_uint32 *, t_uint16, t_uint32,t_mmc_device_id);
PUBLIC t_mmc_error              MMC_WriteBytes(t_uint8, t_uint32, t_uint32 *, t_uint32,t_mmc_device_id);
PUBLIC t_mmc_error              MMC_WriteBlocks(t_uint8, t_uint32, t_uint32 *, t_uint16, t_uint32,t_mmc_device_id, t_bool);
PUBLIC t_mmc_error              MMC_StopTransfer(t_mmc_device_id);
PUBLIC t_mmc_transfer_state     MMC_GetTransferState(t_mmc_device_id);
PUBLIC t_mmc_last_transfer_info MMC_LastTransferInfo(t_mmc_device_id);

PUBLIC t_mmc_error              MMC_Erase(t_uint8, t_uint32, t_uint32,t_mmc_device_id);
PUBLIC t_mmc_error 				MMC_Trim(t_uint8, t_uint32, t_uint32,t_mmc_device_id);
PUBLIC t_mmc_error 				MMC_ReliableWriteMultiBlocks
(
    IN t_uint8          cardno,
    IN t_uint32         addr,
    IN t_uint32         *p_writebuff,
    IN t_uint16         blocksize,
    IN t_uint32         no_of_blocks,
    IN t_mmc_device_id  mmc_device_id
);


PUBLIC t_mmc_error              MMC_SetWriteProtect(t_uint8, t_mmc_write_protect_type, t_uint32,t_mmc_device_id);
PUBLIC t_mmc_error              MMC_ClearFullWriteProtect(t_uint8,t_mmc_device_id);
PUBLIC t_mmc_error              MMC_ClearWriteProtect(t_uint8, t_uint32,t_mmc_device_id);
PUBLIC t_mmc_error              MMC_SendWriteProtectStatus(t_uint8, t_uint32, t_uint32 *,t_mmc_device_id);
PUBLIC t_mmc_error              MMC_SetPassword(t_uint8, t_uint8, t_uint8 *,t_mmc_device_id);
PUBLIC t_mmc_error              MMC_ClearPassword(t_uint8, t_uint8, t_uint8 *,t_mmc_device_id);
PUBLIC t_mmc_error              MMC_ChangePassword(t_uint8, t_uint8, t_uint8 *, t_uint8, t_uint8 *,t_mmc_device_id);
PUBLIC t_mmc_error              MMC_LockCard(t_uint8, t_uint8, t_uint8 *,t_mmc_device_id);
PUBLIC t_mmc_error              MMC_UnlockCard(t_uint8, t_uint8, t_uint8 *,t_mmc_device_id);
PUBLIC t_mmc_error              MMC_ForceErase(t_uint8,t_mmc_device_id);
PUBLIC t_mmc_error              MMC_GetSCR(t_uint8, t_uint32 *,t_mmc_device_id);
PUBLIC t_mmc_error              MMC_SendSDStatus(t_uint8, t_uint32 *,t_mmc_device_id);
PUBLIC t_mmc_error              MMC_GetExtCSD(t_uint8, t_uint32 *,t_mmc_device_id);
PUBLIC t_mmc_error              MMC_SetHighSpeedModeTiming(t_uint8, t_mmc_state,t_mmc_device_id);
PUBLIC t_mmc_error              MMC_SetPowerClass(t_uint8, t_uint8,t_mmc_device_id);

PUBLIC t_mmc_error              MMC_SDIOReadWriteData(t_uint8, t_mmc_sdio_transfer_info const *, t_uint32 *,t_mmc_device_id);
PUBLIC t_mmc_error              MMC_SDIOResumeFunction(t_uint8, t_uint8, t_uint8 *,t_mmc_device_id);
PUBLIC t_mmc_error              MMC_SDIOSuspendFunction(t_uint8,t_mmc_device_id);
PUBLIC t_mmc_error              MMC_SendSdioOpCond(OUT t_mmc_sdio_resp4 *,t_mmc_device_id);

/*New Interrupt strategy(M1 functions) */
PUBLIC void                     MMC_GetIRQSrcStatus(t_mmc_irq_src, t_mmc_irq_status *,t_mmc_device_id);
PUBLIC t_bool                   MMC_IsIRQSrcActive(t_mmc_irq_src, t_mmc_irq_status *,t_mmc_device_id);
PUBLIC t_mmc_error              MMC_ProcessIRQSrc(t_mmc_irq_status *,t_mmc_device_id);
PUBLIC t_mmc_error              MMC_FilterProcessIRQSrc(t_mmc_irq_status *, t_mmc_event *, t_mmc_filter_mode,t_mmc_device_id);
PUBLIC t_bool                   MMC_IsEventActive(t_mmc_event *,t_mmc_device_id);
PUBLIC void                     MMC_AcknowledgeEvent(t_mmc_event *,t_mmc_device_id);

PUBLIC t_mmc_error MMC_EnableEraseGroup (IN t_mmc_device_id mmc_device_id);
PUBLIC t_mmc_error MMC_SetEnhancedConfig(t_uint32, t_uint32, t_uint8, t_mmc_device_id);

#endif /* __MMCI_HCL_ENHANCED*/

#ifdef __cplusplus
}   /* allow C++ to use these headers */
#endif /* __cplusplus */
#endif /* _MMC_H_ */


