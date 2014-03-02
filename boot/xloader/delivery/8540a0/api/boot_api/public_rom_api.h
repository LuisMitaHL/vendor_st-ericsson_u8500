/*-----------------------------------------------------------------------------
 * ST Ericsson
 * Reproduction and Communication of this document is strictly prohibited
 * unless specifically authorized in writing by ST Ericsson.
 *---------------------------------------------------------------------------*/
/** \addtogroup RPC
 * @{
 */ 
/*---------------------------------------------------------------------------*/
/** \file public_rom_api.h
 *  \author ST Ericsson
 *  \brief This header file provides function prototypes for RPC functions
 */
/*---------------------------------------------------------------------------*/
#ifndef _PUBLIC_ROM_API_H
#define _PUBLIC_ROM_API_H

#include "boot_types.h"
#include "boot_error.h"
#include "uart.h"
#include "usb.h"
#include "mmc_mgmt.h"
#include "nomadik_mapping.h"


/*------------------------------------------------------------------------
 * Defines
 *----------------------------------------------------------------------*/
/** \internal RPC index to call PUB_LoadImage() */
#define  PUB_RPC_LOAD_IMAGE  1

//following definition must be the same than in sec_init.s !!
#define  PUB_CPU1_JUMP_MAGIC_NUM      0xA1FEED01   /**< \internal Magic Num to check validity of jump address */
#define  PUB_CPU1_JUMP_MAGICNUM_ADDR   (BACKUP_RAM_START_ADDR + BACKUP_RAM_LENGTH - 0x10) 
#define  PUB_CPU1_JUMP_ADDR            (BACKUP_RAM_START_ADDR + BACKUP_RAM_LENGTH - 0xc) 



/*RPC related structure:*/

/** \internal RPC handler return value */
typedef enum
{
	BOOT_RPC_FAIL = 0,    /**< (0) failure in boot RPC handler management */
    BOOT_RPC_OK = 1       /**< (1) success  */
}t_rpc_error;

/** RPC feature flags */
typedef union
{
    struct{
		t_bitfield UseDma:1;       /**<  Bit[0]  Use DMA or not for the transfer  */ 
		t_bitfield unused1:31;     /**<  Bit[31:1] Unused         */
	}Bit;
	t_uint32 Reg;     /**<  contains the feature flags option*/
}t_rpc_feature_flags;

/** \internal Parameters to call PUB_LoadImage() with boot RPC handler */
typedef struct{ 
t_uint32 interface; /**< Define the interface that is used for loading the image(see t_load_if)*/
t_uint32 size; /**< For memory interface: size of the image to be read in bytes. \n
                              For peripheral interface: set to zero*/
t_uint32 readAddr; /**< For memory interface: Offset address of the image in memory.\n
                       For peripheral interface: set to zero*/
t_uint32 writeAddr; /**< Physical address where the image should be copied.*/                                  

t_address configInfo ; /**< For peripheral interface:  address to buffer that contains\n
                            configuration data to be sent to host as configuration info. \n
                            Size of the message (in bytes) is defined in the first uint32 \n
                            element in the buffer */
t_rpc_feature_flags featureFlags; /**< Bits for defining additional behaviour/features: \n
                                       Bit 0 ->  Enable/Disbale DMA usage for the transfer,\n 
                                       Bit 1 -> usb charging disable: unused */
t_uint32 timeout;  /**< timeout value in ms to apply for file reception on peripheral interface (USB or UART)
                        (if 0 no timeout) */
t_uint32 bootError;   /**< Boot error code (see t_boot_error) */
} t_pub_rom_rpc_load_param;

/* Union on all params structure to call all functions from RPC boot handler */
typedef union{
    t_pub_rom_rpc_load_param loadImage;
} t_pub_rom_rpc_params;


/** \internal Interface structure for shared params between boot ROM code and public loader application.*/
typedef struct{
    t_MmcType* p_mmcType;
    t_usb_type* p_usbType;
    t_uart_type* p_uartType; 
} t_pub_if_type;

/*Loader related structures:*/
/** \internal Structure for shared params between boot ROM code and public loader application.*/
typedef struct{
void* bootRomJmpTbl;  /**< Physical Address of the boot ROM jump table (needed to call boot ROM api functions)*/
t_uint32 pdebugInfo;  /**< Word that contain the physical address (in backup RAM) of boot debug trace, 
                            see t_boot_trace_info (gives interface used to boot). */
t_pub_if_type* pIfTypes;      /**<  Physical Address of the the structure t_pub_if_type that contains:
                            Physical Address of the MmcType (t_MmcType),
                             UsbType (t_usb_type), UartType (t_uart_type) structure, 
                            needed to call interface boot API ROM functions, or t_uart_type for UART.*/
t_uint32 bootInfo;   /**<  Boot info read from Antifuse/BSV, see t_boot_rom_conf */
t_uint32 bootIndication ;  /**< Boot indication received (needed for debug trace on uart). */
t_uint32 tocAddrMem; /**< TOC address in memory: flash or MMC */
} t_pub_rom_loader_info;

/*------------------------------------------------------------------------
 * Public functions
 *----------------------------------------------------------------------*/
PUBLIC t_rpc_error PUB_RpcHandler(t_uint32);
                         
PUBLIC t_boot_error PUB_SearchItemInToc (t_uint32, char *, t_boot_toc_item* );

PUBLIC t_boot_error PUB_SearchCRKCInToc (t_uint32, t_boot_toc_item* );
                                                                      
PUBLIC t_boot_error PUB_LoadImage ( const t_address readAddress,
                                    const t_size size,
                                    t_uint32* p_writeAddress,
                                    const t_load_if interface,
                                    t_uint32* p_configInfo,
                                    t_rpc_feature_flags featureFlags,
                                    t_uint32 timeout);  
                                    
                                                                            
PUBLIC void PUB_SlaveProcWakeUp(const t_address);
PUBLIC t_boot_error PUB_StartIssw ( t_boot_indication indication,
                                   t_boot_rom_conf* p_bootInfo,
                                   t_uart_number uartNumber,
                                   t_uint32 *p_isswHeader,
                                   t_bool isLastMemoryParsed);
                                   
PUBLIC t_boot_error PUB_CheckIsswHeader( t_boot_indication indication,
                                   t_boot_rom_conf* p_bootInfo,
                                   t_uart_number uartNumber,
                                   t_uint32 *p_isswHeader,
								   t_bool IsSpeedUpDis);
 
#endif /* _PUBLIC_ROM_API_H */
/* end of public_rom_api.h */
/** @} */
