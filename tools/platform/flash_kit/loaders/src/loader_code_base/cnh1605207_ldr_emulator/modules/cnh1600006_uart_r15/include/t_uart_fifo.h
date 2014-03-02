#ifndef INCLUSION_GUARD_T_UART_FIFO_H
#define INCLUSION_GUARD_T_UART_FIFO_H
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
#include "c_system.h"
#include "t_emulator.h"
#include "error_codes.h"

typedef struct PrivateData_s *PrivateData_t;

typedef struct Fifo_s {
    PrivateData_t Data;

    struct Fifo_s *This_p;

    ErrorCode_e(*Init)(struct Fifo_s *Fifo_p, uint32 FifoSize);
    ErrorCode_e(*Pop)(struct Fifo_s *Fifo_p, uint8 *OutData_p, uint32 DataSize);
    ErrorCode_e(*Push)(struct Fifo_s *Fifo_p, const uint8 *Data_p, uint32 DataSize);
    ErrorCode_e(*Print)(struct Fifo_s *Fifo_p);
    ErrorCode_e(*Flush)(struct Fifo_s *Fifo_p);
    boolean(*IsEmpty)(struct Fifo_s *Fifo_p);
    boolean(*IsFull)(struct Fifo_s *Fifo_p);
} Fifo_t;

/**     @}*/
/**@}*/
#endif //INCLUSION_GUARD_T_UART_FIFO_H
