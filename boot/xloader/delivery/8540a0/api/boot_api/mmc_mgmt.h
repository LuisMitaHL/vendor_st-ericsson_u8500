/*-----------------------------------------------------------------------------
 * ST Ericsson
 * Reproduction and Communication of this document is strictly prohibited
 * unless specifically authorized in writing by ST Ericsson.
 *---------------------------------------------------------------------------*/
/** \addtogroup MMC
 * @{
 */ 
/*---------------------------------------------------------------------------*/
/** \file mmc_mgmt.h
 *  \author ST Ericsson
 *  \brief This module is the MMC driver header file
 */

/*---------------------------------------------------------------------------*/
#ifndef _MMC_MGMT_H_  
#define _MMC_MGMT_H_

/*------------------------------------------------------------------------
 * Includes
 *----------------------------------------------------------------------*/

#include "boot_error.h"
#include "boot_types.h"
#include "nomadik_registers.h"
#if 0
#include "string.h"
#endif
#include "dma.h"

/*------------------------------------------------------------------------
 * Defines
 *----------------------------------------------------------------------*/
/* For MMC clock 50MHz*/
#define MMC_CLKDIV_12MHZ_FOR_50MHZ 2 /**< Clkdiv to set to have fpp=12.5MHz when mmc_clk=50MHz (50MHz/(clkdiv+2))*/
#define MMC_CLKDIV_17MHZ_FOR_50MHZ 1 /**< Clkdiv to set to have fpp=16.7MHz when mmc_clk=50MHz (50MHz/(clkdiv+2))*/


#define MMC_TOC_COPY_OFFSET  (128*1024)  /** In MMC, TOC is copied 4 times at 128KB intervals */
/*------------------------------------------------------------------------
 * Global Variables
 *----------------------------------------------------------------------*/

/** SDI interface number */
typedef enum {
    MMC_SDI2, /**< (0) SDI2 interface chosen */
    MMC_SDI4  /**< (1) SDI4 interface chosen */
}t_MmcNb;

/** \internal SDI (MMC/SD interface) choice according to BSV/Antifuse MMC interface selection */
typedef enum {
    MMC2_MMC4=0, /** \internal < (0) Search must be done on SDI2 and if fail on SDI4 */
    MMC2=1,      /** \internal < (1) Search must be done on SDI2 only */
    MMC4=2       /** \internal < (2) Search must be done on SDI4 only */
}t_MmcPosition;

/** Addressing mode of the card */
typedef enum{
    MMC_DIRECT_ADDRESSING, /**< (0) for normal MMC/SD cards, address given in bytes */
    MMC_SECTOR_ADDRESSING  /**< (1) for SDHC (High Capacity SD card) and  
                                    for MMC with capacity>2GB (spec version>=4.2),
                                    address given in 512 bytes sector */
}t_mmc_addressing_type;

/** Bus configuration choice */
typedef enum {
    MMC_1BIT_BUS, /**< (0), default (only DAT0 used)*/
    MMC_4BIT_BUS, /**< (1), if chosen, GPIOs MCx_DAT1, DAT2 and DAT3 must be connected to the card*/
    MMC_8BIT_BUS  /**< (2), if chosen, GPIOs MCx_DAT1, ... DAT7 must be connected to the card*/
}t_mmc_bus_conf;

/** MMC/SD card description */
typedef struct{
t_boot_memType memType;         /**< Memory used to boot: MMC or SD card */
t_MmcNb mmcNb ;                 /**< MMC Interface number used to boot*/
t_mmc_addressing_type addrType; /**< Direct or sector addressing card */
t_mmc_bus_conf busConf;         /**< Indicate the current bus configuration after init*/
t_uint16  readBlkSize;          /**< Card memory block length */
t_uint16  rca;                  /**< MMC/SD card assigned RCA*/
t_uint8 mmcSpecVer;             /**< Give the MMC spec version if 4 -> highspeed MMC*/
t_uint8 filled;                 /**< unused */
t_uint16 clkDiv;                /**< Give the current set clkdiv ie the programmed card frequence (fpp)*/
t_uint32  mmcSize;              /**< Size of the MMC/SD card in bytes or in multiple of 512
                                     bytes in case of sector addressing MMC/SD (size > 2GB)\n
                                     in case of MMC >2GB, size will not be correct if busConf is 1bit (set to 2GB)*/
t_uint32  freqTranSpeed;        /**< Max card frequence in kHz calculated according to CSD register 
                                    (or EXT_CSD only if 4/8 bit busConf set,
                                     so for 26/52MHz MMC, it will not be correct if busConf is 1bit)*/
t_mmc_register * p_MmcRegister; /**< Pointer to the SDI command registers*/
t_uint32    dmaChannel;        /**<  DMA channel use for DMA transfer */
t_dma_std_channel_desc*   p_dmaLli;  /**<  pointer to the LLI to be used by the DMA */
t_uint32    lliMaxSize;         /**< Max size for LLI with MMC  */
}t_MmcType;

/** Init configuration choice */
typedef struct {
    t_MmcNb mmcNb;          /**< SDI interface the card to initialize is connected to. */
    t_bool  lowVoltageInit; /**< TRUE if 1.8V init is needed 
                               (always TRUE in case MMC_SDI2 is used)*/
    t_mmc_bus_conf mmcBusConf; /**< 1, 4 or 8bits (8bits not supported by SD card) \n
                            Init is done by default in 1bit then if card supports it other bus config (4/8bits) 
                            given in mmcBusConf will be set*/
    t_uint16 mmcFreqConf; /**< Transfer frequency to set expressed in clkdiv ( freq = mmc_clk/(clkdiv+2) )*/
}t_mmc_init_conf;


/*------------------------------------------------------------------------
 * Function definition
 *----------------------------------------------------------------------*/
/* PUBLIC */
PUBLIC t_boot_error MMC_DetectAndInit(const t_mmc_init_conf  *p_initConf,t_MmcType *p_mmcType);
PUBLIC void         MMC_Close(t_MmcNb mmcNb);
PUBLIC t_boot_error MMC_Read(const t_MmcType *p_mmcType, const t_address readAddress,
                                 t_uint8 *p_writeAddress,const t_size transferSize,
                                 t_dma_use dmaUse);
PUBLIC t_boot_error MMC_TocSearch(const t_MmcType *p_mmcType, t_address *p_tocStartAddress);    
PUBLIC t_boot_error MMC_FullTocSearch(const t_MmcType *p_mmcType, t_uint32 tocIndex, t_address *p_tocStartAddress,
                                      t_bool isCRCKNeeded, t_boot_toc_item *p_itemCRKC, t_bool * p_isCRCKFound, t_bool isSecureChip,
                                      const t_uart_number uartNumber, const t_boot_indication indication);
PUBLIC void         MMC_GPIOReset(t_MmcType *p_mmcType);
PUBLIC t_boot_error MMC_SetMmcWideBus(t_MmcType *p_mmcType, const t_mmc_init_conf *p_initConf);

#endif //_MMC_MGMT_H_
/** @} */
