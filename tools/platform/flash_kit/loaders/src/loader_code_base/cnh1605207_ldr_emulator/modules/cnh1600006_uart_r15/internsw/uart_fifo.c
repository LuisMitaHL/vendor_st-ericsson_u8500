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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#ifdef WIN32
#include <windows.h>
#endif //WIN32
#include "error_codes.h"
#include "t_basicdefinitions.h"
#include "t_emulator.h"
#include "r_debug.h"
#define EMP_Printf printf   /**< @todo remove in final version */
#include "r_uart_fifo.h"

/*
 * This is a structure containing the private members of a fifo.
 */
struct PrivateData_s {
    /* Buffer for holding the data, this is where data is stored*/
    uint8  *Buffer_p;
    /* Pointer to the head of the fifo*/
    uint8  *Head_p;
    /* Pointer to the tail of the fifo*/
    uint8  *Tail_p;
    /* Size of the fifo*/
    uint32  Size;
    /* Free space in the fifo*/
    uint32  Available;
    /* Critical section needed for multithreaded applications*/
    CRITICAL_SECTION Fifo_cs;
};

/*
 * This function initializes fifo object.
 * param[in] Fifo_p pointer to Fifo object.
 * param[in] FifoSize is the size of the fifo.
 * @return see \c ErrorCode_e for detailed description of the error codes.
 */
static ErrorCode_e Fifo_Init(Fifo_t *Fifo_p, uint32 FifoSize);

/*
 * This function pops elements from the fifo.
 * param[in] Fifo_p pointer to Fifo object.
 * param[in] OutData_p pointer to a buffer where the data from the fifo
 * should be placed.
 * param[in] DataSize is the size of data that should be poped from the fifo.
 * @return see \c ErrorCode_e for detailed description of the error codes.
 */
static ErrorCode_e Fifo_Pop(Fifo_t *Fifo_p, uint8 *OutData_p, uint32 DataSize);

/*
 * This function pushes elements into the fifo.
 * param[in] Fifo_p pointer to Fifo object.
 * param[in] Data_p pointer to a buffer containing data thata should be
 * placed on the fifo.
 * param[in] DataSize is the size of data that should be placed in the fifo.
 * @return see \c ErrorCode_e for detailed description of the error codes.
 */
static ErrorCode_e Fifo_Push(Fifo_t *Fifo_p, const uint8 *Data_p, uint32 DataSize);

/*
 * This function prints the elements in the fifo.
 * param[in] Fifo_p pointer to Fifo object.
 * @return see \c ErrorCode_e for detailed description of the error codes.
 */
static ErrorCode_e Fifo_Print(Fifo_t *Fifo_p);

/*
 * This function flushes the fifo.
 * param[in] Fifo_p pointer to Fifo object.
 * @return see \c ErrorCode_e for detailed description of the error codes.
 */
static ErrorCode_e Fifo_Flush(Fifo_t *Fifo_p);

/*
 * This function checks whether fifo is empty or not.
 * param[in] Fifo_p pointer to Fifo object.
 * @return boolean TRUE if fifo is empty else FALSE.
 */
static boolean Fifo_IsEmpty(Fifo_t *Fifo_p);

/*
 * This function checks whether fifo is full or not.
 * param[in] Fifo_p pointer to Fifo object.
 * @return boolean TRUE if fifo is full else FALSE.
 */
static boolean Fifo_IsFull(Fifo_t *Fifo_p);

/*
 * This function creates fifo object.
 * param[out] Fifo_pp pointer to pointer to Fifo object. *Fifo_pp must point to NULL.
 *            After the function returns, *Fifo_pp points to the newly created Fifo object.
 * @return see \c ErrorCode_e for detailed description of the error codes.
 */
ErrorCode_e CreateFifoObject(Fifo_t **Fifo_pp)
{
    if (NULL == Fifo_pp) {
        A_(printf("uart_fifo.c (%d): Error pointer is NULL\n", __LINE__);)
        return E_INVALID_INPUT_PARAMETERS;
    }

    if (NULL != *Fifo_pp) {
        A_(printf("uart_fifo.c (%d): Error pointer is not null\n", __LINE__);)
        return E_POINTER_NOT_NULL;
    }

    *Fifo_pp = (Fifo_t *) malloc(sizeof(Fifo_t));

    if (NULL == *Fifo_pp) {
        A_(printf("uart_fifo.c (%d): Error malloc failed to allocate memory\n", __LINE__);)
        return E_ALLOCATE_FAILED;
    }

    (*Fifo_pp)->Data = (struct PrivateData_s *)malloc(sizeof(struct PrivateData_s));

    if (NULL == (*Fifo_pp)->Data) {
        A_(printf("uart_fifo.c (%d): Error malloc failed to allocate memory\n", __LINE__);)
        return E_ALLOCATE_FAILED;
    }

    InitializeCriticalSection(&(*Fifo_pp)->Data->Fifo_cs);

    EnterCriticalSection(&(*Fifo_pp)->Data->Fifo_cs);
    (*Fifo_pp)->This_p   = *Fifo_pp;
    (*Fifo_pp)->Init     = Fifo_Init;
    (*Fifo_pp)->Pop      = Fifo_Pop;
    (*Fifo_pp)->Push     = Fifo_Push;
    (*Fifo_pp)->Print    = Fifo_Print;
    (*Fifo_pp)->Flush    = Fifo_Flush;
    (*Fifo_pp)->IsEmpty  = Fifo_IsEmpty;
    (*Fifo_pp)->IsFull   = Fifo_IsFull;
    LeaveCriticalSection(&(*Fifo_pp)->Data->Fifo_cs);
    return E_SUCCESS;
}

/*
 * This function destroys fifo object.
 * param[out] Fifo_pp pointer to pointer to Fifo object. *Fifo_pp must point to valid Fifo object.
 *            After the function returns, *Fifo_pp points to NULLt.
 * @return see \c ErrorCode_e for detailed description of the error codes.
 */
ErrorCode_e DestroyFifoObject(Fifo_t **Fifo_pp)
{
    if (NULL == Fifo_pp) {
        A_(printf("uart_fifo.c (%d): Error pointer is NULL\n", __LINE__);)
        return E_INVALID_INPUT_PARAMETERS;
    }

    if (NULL == *Fifo_pp) {
        A_(printf("uart_fifo.c (%d): Error object is NULL\n", __LINE__);)
        return E_OBJECT_NULL;
    }

    EnterCriticalSection(&(*Fifo_pp)->Data->Fifo_cs);

    if (NULL != (*Fifo_pp)->Data->Buffer_p) {
        free((*Fifo_pp)->Data->Buffer_p);
        (*Fifo_pp)->Data->Buffer_p = NULL;
    }

    LeaveCriticalSection(&(*Fifo_pp)->Data->Fifo_cs);
    free(*Fifo_pp);
    *Fifo_pp = NULL;
    return E_SUCCESS;
}

/*
 * This function initializes fifo object.
 * param[in] Fifo_p pointer to Fifo object.
 * param[in] FifoSize is the size of the fifo.
 * @return see \c ErrorCode_e for detailed description of the error codes.
 */
static ErrorCode_e Fifo_Init(Fifo_t *Fifo_p, uint32 FifoSize)
{
    if (NULL == Fifo_p) {
        A_(printf("uart_fifo.c (%d): Error object is NULL\n", __LINE__);)
        return E_OBJECT_NULL;
    }

    EnterCriticalSection(&Fifo_p->Data->Fifo_cs);

    Fifo_p->Data->Buffer_p      = NULL;
    Fifo_p->Data->Head_p        = NULL;
    Fifo_p->Data->Tail_p        = NULL;
    Fifo_p->Data->Size        = 0;
    Fifo_p->Data->Available   = 0;

    Fifo_p->Data->Buffer_p = (uint8 *)malloc(FifoSize);

    if (NULL == Fifo_p->Data->Buffer_p) {
        LeaveCriticalSection(&Fifo_p->Data->Fifo_cs);
        A_(printf("uart_fifo.c (%d): Error malloc failed to allocate memory\n", __LINE__);)
        return E_ALLOCATE_FAILED;
    }

    memset(Fifo_p->Data->Buffer_p, 0x00, Fifo_p->Data->Size);
    Fifo_p->Data->Size = FifoSize;
    Fifo_p->Data->Available = FifoSize;
    Fifo_p->Data->Head_p = Fifo_p->Data->Buffer_p;
    Fifo_p->Data->Tail_p = Fifo_p->Data->Buffer_p;
    LeaveCriticalSection(&Fifo_p->Data->Fifo_cs);
    return E_SUCCESS;
}

/*
 * This function pops elements from the fifo.
 * param[in] Fifo_p pointer to Fifo object.
 * param[in] OutData_p pointer to a buffer where the data from the fifo
 * should be placed.
 * param[in] DataSize is the size of data that should be poped from the fifo.
 * @return see \c ErrorCode_e for detailed description of the error codes.
 */
static ErrorCode_e Fifo_Pop(Fifo_t *Fifo_p, uint8 *OutData_p, uint32 DataSize)
{
    struct PrivateData_s *Data_p;

    if (NULL == Fifo_p) {
        A_(printf("uart_fifo.c (%d): Error object is NULL\n", __LINE__);)
        return E_OBJECT_NULL;
    }

    EnterCriticalSection(&Fifo_p->Data->Fifo_cs);
    Data_p = Fifo_p->Data;

    if (NULL == OutData_p) {
        LeaveCriticalSection(&Fifo_p->Data->Fifo_cs);
        A_(printf("uart_fifo.c (%d): Error parametar out_data_p is NULL\n", __LINE__);)
        return E_INVALID_INPUT_PARAMETERS;
    }

    if ((0 == DataSize) || (Data_p->Size < DataSize)) {
        LeaveCriticalSection(&Fifo_p->Data->Fifo_cs);
        A_(printf("uart_fifo.c (%d): Error invalid argument DataSize\n", __LINE__);)
        return E_INVALID_INPUT_PARAMETERS;
    }

    if (DataSize > (Data_p->Size - Data_p->Available)) {
        LeaveCriticalSection(&Fifo_p->Data->Fifo_cs);
        A_(printf("uart_fifo.c (%d): Error Fifo underflow\n");)
        return E_FIFO_UNDERFLOW;
    }

    if (Data_p->Size - (uint32)(Data_p->Tail_p - Data_p->Buffer_p) >= DataSize) {
        memcpy(OutData_p, Data_p->Tail_p, DataSize);
        Data_p->Tail_p += DataSize;
        Data_p->Available += DataSize;

        if (Data_p->Head_p == Data_p->Tail_p) {
            Data_p->Head_p = Data_p->Buffer_p;
            Data_p->Tail_p = Data_p->Buffer_p;

            assert(Data_p->Size == Data_p->Available);
        } else {
            assert(Data_p->Size > Data_p->Available);
        }

        assert(Data_p->Available <= Data_p->Size);
        assert((uint32)abs(Data_p->Head_p - Data_p->Buffer_p) <= Data_p->Size);
        assert((uint32)abs(Data_p->Tail_p - Data_p->Buffer_p) <= Data_p->Size);
        LeaveCriticalSection(&Fifo_p->Data->Fifo_cs);
        return E_SUCCESS;
    } else {
        memcpy(OutData_p, Data_p->Tail_p, Data_p->Size - (uint32)(Data_p->Tail_p - Data_p->Buffer_p));
        Data_p->Available += Data_p->Size - (uint32)(Data_p->Tail_p - Data_p->Buffer_p);
        DataSize -= Data_p->Size - (uint32)(Data_p->Tail_p - Data_p->Buffer_p);
        OutData_p += Data_p->Size - (uint32)(Data_p->Tail_p - Data_p->Buffer_p);
        Data_p->Tail_p = Data_p->Buffer_p;

        memcpy(OutData_p, Data_p->Tail_p, DataSize);
        Data_p->Tail_p += DataSize;
        Data_p->Available += DataSize;

        if (Data_p->Head_p == Data_p->Tail_p) {
            Data_p->Head_p = Data_p->Buffer_p;
            Data_p->Tail_p = Data_p->Buffer_p;

            assert(Data_p->Size == Data_p->Available);
        } else {
            assert(Data_p->Size > Data_p->Available);
        }

        assert(Data_p->Available <= Data_p->Size);
        assert((uint32)abs(Data_p->Head_p - Data_p->Buffer_p) <= Data_p->Size);
        assert((uint32)abs(Data_p->Tail_p - Data_p->Buffer_p) <= Data_p->Size);
        LeaveCriticalSection(&Fifo_p->Data->Fifo_cs);
        return E_SUCCESS;
    }

    return E_GENERAL_FATAL_ERROR;
}

/*
 * This function pushes elements into the fifo.
 * param[in] Fifo_p pointer to Fifo object.
 * param[in] Data_p pointer to a buffer containing data thata should be
 * placed on the fifo.
 * param[in] DataSize is the size of data that should be placed in the fifo.
 * @return see \c ErrorCode_e for detailed description of the error codes.
 */
static ErrorCode_e Fifo_Push(Fifo_t *Fifo_p, const uint8 *Data_p, uint32 DataSize)
{
    if (NULL == Fifo_p) {
        A_(printf("uart_fifo.c (%d): Error object is NULL\n", __LINE__);)
        return E_OBJECT_NULL;
    }

    if (NULL == Data_p) {
        A_(printf("uart_fifo.c (%d): Error parametar data_p is NULL\n", __LINE__);)
        return E_INVALID_INPUT_PARAMETERS;
    }

    EnterCriticalSection(&Fifo_p->Data->Fifo_cs);

    if ((0 == DataSize) || (Fifo_p->Data->Size < DataSize)) {
        LeaveCriticalSection(&Fifo_p->Data->Fifo_cs);
        A_(printf("uart_fifo.c (%d): Error invalid argument DataSize\n");)
        return E_INVALID_INPUT_PARAMETERS;
    }

    if (Fifo_p->Data->Available < DataSize) {
        LeaveCriticalSection(&Fifo_p->Data->Fifo_cs);
        A_(printf("uart_fifo.c (%d): Error Fifo overflow\n", __LINE__);)
        return E_FIFO_OVERFLOW;
    }

    if (Fifo_p->Data->Size - (uint32)(Fifo_p->Data->Head_p - Fifo_p->Data->Buffer_p) >= DataSize) {
        memcpy(Fifo_p->Data->Head_p, Data_p, DataSize);

        Fifo_p->Data->Head_p += DataSize;
        Fifo_p->Data->Available -= DataSize;

        if ((Fifo_p->Data->Head_p == Fifo_p->Data->Tail_p) || (Fifo_p->Data->Head_p == (Fifo_p->Data->Tail_p + Fifo_p->Data->Size))) {
            assert(0 == Fifo_p->Data->Available);
        } else {
            assert(Fifo_p->Data->Available > 0);
        }

        assert(Fifo_p->Data->Available <= Fifo_p->Data->Size);
        assert((uint32)abs(Fifo_p->Data->Head_p - Fifo_p->Data->Buffer_p) <= Fifo_p->Data->Size);
        assert((uint32)abs(Fifo_p->Data->Tail_p - Fifo_p->Data->Buffer_p) <= Fifo_p->Data->Size);
        LeaveCriticalSection(&Fifo_p->Data->Fifo_cs);
        return E_SUCCESS;
    } else {
        memcpy(Fifo_p->Data->Head_p, Data_p, Fifo_p->Data->Size - (uint32)(Fifo_p->Data->Head_p - Fifo_p->Data->Buffer_p));

        Fifo_p->Data->Available -= Fifo_p->Data->Size - (uint32)(Fifo_p->Data->Head_p - Fifo_p->Data->Buffer_p);
        Data_p += Fifo_p->Data->Size - (uint32)(Fifo_p->Data->Head_p - Fifo_p->Data->Buffer_p);
        DataSize -= Fifo_p->Data->Size - (uint32)(Fifo_p->Data->Head_p - Fifo_p->Data->Buffer_p);
        Fifo_p->Data->Head_p = Fifo_p->Data->Buffer_p;

        memcpy(Fifo_p->Data->Head_p, Data_p, DataSize);
        Fifo_p->Data->Head_p += DataSize;
        Fifo_p->Data->Available -= DataSize;

        if (Fifo_p->Data->Head_p == Fifo_p->Data->Tail_p) {
            assert(0 == Fifo_p->Data->Available);
        }

        assert(Fifo_p->Data->Available <= Fifo_p->Data->Size);
        assert((uint32)abs(Fifo_p->Data->Head_p - Fifo_p->Data->Buffer_p) <= Fifo_p->Data->Size);
        assert((uint32)abs(Fifo_p->Data->Tail_p - Fifo_p->Data->Buffer_p) <= Fifo_p->Data->Size);
        LeaveCriticalSection(&Fifo_p->Data->Fifo_cs);
        return E_SUCCESS;
    }

    return E_GENERAL_FATAL_ERROR;
}

/*
 * This function prints the elements in the fifo.
 * param[in] Fifo_p pointer to Fifo object.
 * @return see \c ErrorCode_e for detailed description of the error codes.
 */
static ErrorCode_e Fifo_Print(Fifo_t *Fifo_p)
{
    uint8 *Temp_p = NULL;

    if (NULL == Fifo_p) {
        A_(printf("uart_fifo.c (%d): Error object is NULL\n", __LINE__);)
        return E_OBJECT_NULL;
    }

    EnterCriticalSection(&Fifo_p->Data->Fifo_cs);

    if ((NULL == Fifo_p->Data->Tail_p) || (NULL == Fifo_p->Data->Head_p) || (NULL == Fifo_p->Data->Buffer_p)) {
        LeaveCriticalSection(&Fifo_p->Data->Fifo_cs);
        A_(printf("uart_fifo.c (%d): Error object is not complete\n", __LINE__);)
        return E_GENERAL_FATAL_ERROR;
    }

    Temp_p = Fifo_p->Data->Tail_p;

    while (Temp_p != Fifo_p->Data->Head_p) {
        if (Temp_p == (Fifo_p->Data->Buffer_p + Fifo_p->Data->Size)) {
            Temp_p = Fifo_p->Data->Buffer_p;
        } else {
            printf("0x%02x\n", *Temp_p);
            Temp_p ++;
        }
    }

    LeaveCriticalSection(&Fifo_p->Data->Fifo_cs);
    return E_SUCCESS;
}

/*
 * This function flushes the fifo.
 * param[in] Fifo_p pointer to Fifo object.
 * @return see \c ErrorCode_e for detailed description of the error codes.
 */
static ErrorCode_e Fifo_Flush(Fifo_t *Fifo_p)
{
    EnterCriticalSection(&Fifo_p->Data->Fifo_cs);
    Fifo_p->Data->Tail_p      = Fifo_p->Data->Buffer_p;
    Fifo_p->Data->Head_p      = Fifo_p->Data->Buffer_p;
    Fifo_p->Data->Available   = Fifo_p->Data->Size;
    memset(Fifo_p->Data->Buffer_p, 0x00, Fifo_p->Data->Size);
    LeaveCriticalSection(&Fifo_p->Data->Fifo_cs);
    return E_SUCCESS;
}

/*
 * This function checks whether fifo is empty or not.
 * param[in] Fifo_p pointer to Fifo object.
 * @return boolean TRUE if fifo is empty else FALSE.
 */
static boolean Fifo_IsEmpty(Fifo_t *Fifo_p)
{
    EnterCriticalSection(&Fifo_p->Data->Fifo_cs);

    if (Fifo_p->Data->Size == Fifo_p->Data->Available) {
        LeaveCriticalSection(&Fifo_p->Data->Fifo_cs);
        return TRUE;
    }

    LeaveCriticalSection(&Fifo_p->Data->Fifo_cs);
    return FALSE;
}

/*
 * This function checks whether fifo is full or not.
 * param[in] Fifo_p pointer to Fifo object.
 * @return boolean TRUE if fifo is full else FALSE.
 */
static boolean Fifo_IsFull(Fifo_t *Fifo_p)
{
    EnterCriticalSection(&Fifo_p->Data->Fifo_cs);

    if (0 == Fifo_p->Data->Available) {
        LeaveCriticalSection(&Fifo_p->Data->Fifo_cs);
        return TRUE;
    }

    LeaveCriticalSection(&Fifo_p->Data->Fifo_cs);
    return FALSE;
}
/**     @}*/
/**@}*/