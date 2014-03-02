#ifndef NAND_MEMORY_H
#define NAND_MEMORY_H
/***********************************************************************
 * $Copyright Ericsson AB 2009 $
 **********************************************************************/
/**
 * \addtogroup ldr_emulator
 * @{
 *      \addtogroup NAND
 *      \brief Emulated NAND functions for R15 emulator
 *      @{
 */
//#include "c_pd_nand.h"
#include "t_pd_nand.h"
#include "t_emul_nand.h"

extern NAND_Memory_t NAND_Memory_Emul;

PD_NAND_Result_t NANDHW_Memory_Init(const PD_NAND_MemoryProperties_t *const MemProp_p);
void NANDHW_Memory_Reset(void);
void NANDHW_Memory_SetBlock_p(const uint32 Addr);
void NANDHW_Memory_SetRedundantArea_p(const uint32 Addr);
uint8 NANDHW_Memory_ManID(void);
uint8 NANDHW_Memory_DevID(void);

void Do_FlushPDNandEmulatorToFile(void);

/**     @}*/
/**@}*/
#endif //NAND_MEMORY_H
