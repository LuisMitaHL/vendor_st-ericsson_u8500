#ifndef INCLUSION_GUARD_EMUL_NAND_H_
#define INCLUSION_GUARD_EMUL_NAND_H_
/***********************************************************************
 * $Copyright Ericsson AB 2008 $
 **********************************************************************/
/**
 * \addtogroup ldr_emulator
 * @{
 *      \addtogroup NAND
 *      \brief Emulated NAND functions for R15 emulator
 *      @{
 */

ErrorCode_e EMUL_Nand_Util_SmallPageImage(const sint8 *Buffer, uint32 BufferSize);
ErrorCode_e EMUL_Nand_Util_LargePageImage(const sint8 *Buffer, uint32 BufferSize);

/**     @}*/
/**@}*/
#endif /*INCLUSION_GUARD_EMUL_NAND_H_*/
