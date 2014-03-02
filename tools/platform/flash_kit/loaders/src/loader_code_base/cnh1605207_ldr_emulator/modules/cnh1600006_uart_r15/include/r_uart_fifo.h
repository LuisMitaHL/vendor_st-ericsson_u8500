#ifndef INCLUSION_GUARD_R_UART_FIFO_H
#define INCLUSION_GUARD_R_UART_FIFO_H
/***********************************************************************
 * $Copyright Ericsson AB 2008 $
 **********************************************************************/
/**
 * \addtogroup ldr_emulator
 * @{
 *      \addtogroup UART
 *      \brief Emulated UART functions for R15 emulator
 *      @{
 */

#include "error_codes.h"
#include "t_uart_fifo.h"

/**
 * This function creates fifo object.
 * param[out] Fifo_pp pointer to pointer to Fifo object. *Fifo_pp must point to NULL.
 *            After the function returns, *Fifo_pp points to the newly created Fifo object.
 * @return see \c ErrorCode_e for detailed description of the error codes.
 */
ErrorCode_e CreateFifoObject(Fifo_t **Fifo_pp);

/**
 * This function destroys fifo object.
 * param[out] Fifo_pp pointer to pointer to Fifo object. *Fifo_pp must point to valid Fifo object.
 *            After the function returns, *Fifo_pp points to NULLt.
 * @return see \c ErrorCode_e for detailed description of the error codes.
 */
ErrorCode_e DestroyFifoObject(Fifo_t **Fifo_pp);

/**     @}*/
/**@}*/
#endif //INCLUSION_GUARD_R_UART_FIFO_H