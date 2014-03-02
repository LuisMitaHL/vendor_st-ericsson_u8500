#ifndef INCLUSION_GUARD_T_EMUL_NAND_H
#define INCLUSION_GUARD_T_EMUL_NAND_H
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

#include "c_pd_nand.h"

#define NANDHW_ECC_OFFSET        6

/* You can change these default values in DescrExtra now.
 *
 * Example:
 * !set -f CNH1605207_DEFAULT_MANUFACTURER_ID = T_PD_NAND_CONF_MAN_ID_SAMSUNG
 * !set -f CNH1605207_DEFAULT_DEVICE_ID = T_PD_NAND_CONF_DEV_ID_128
 */
#ifndef DEFAULT_MANUFACTURER_ID
#define DEFAULT_MANUFACTURER_ID  T_PD_NAND_CONF_MAN_ID_SAMSUNG
#endif

#ifndef DEFAULT_DEVICE_ID
#define DEFAULT_DEVICE_ID        T_PD_NAND_CONF_DEV_ID_SAMSUNG_K9F1208B0B
#endif


typedef struct {
    uint8 ManufacturerID;
    uint8 DeviceID;
    uint8 *MemBuffer;
    uint8 *RedundantAreaBuff;
    uint8 *Block_p;
    uint8 *RedundantArea_p;
    uint8 *ECC_p;
    uint32 NANDHW_EccShadow[4];
    PD_NAND_CONF_DeviceInfo_t DeviceInfo;

} NAND_Memory_t;

/**     @}*/
/**@}*/
#endif //INCLUSION_GUARD_T_EMUL_NAND_H
