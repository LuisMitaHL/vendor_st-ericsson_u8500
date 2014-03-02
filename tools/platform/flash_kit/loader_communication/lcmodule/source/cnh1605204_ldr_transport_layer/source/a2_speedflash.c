/*******************************************************************************
 * Copyright (C) ST-Ericsson SA 2011
 * License terms: 3-clause BSD license
 ******************************************************************************/
/**
 *  @addtogroup ldr_communication_serv
 *  @{
 *    @addtogroup a2_family
 *    @{
 *      @addtogroup a2_protocol
 *      @{
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "t_basicdefinitions.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "r_communication_service.h"
#include "r_debug.h"
#include "r_debug_macro.h"
#include "r_a2_speedflash.h"
#include "t_a2_family.h"
#include "r_a2_family.h"
#include "r_a2_network.h"

/*******************************************************************************
 * Declaration of file local functions
 ******************************************************************************/
static void A2_Speedflash_WriteCallback(const void *Data_p, const uint32 Length, void *Param_p);
static void A2_Speedflash_ReadCallback(const void *Data_p, const uint32 Length, void *Param_p);

/**
 * Set the LCM in A2 Speedflash Mode.
 *
 * @param [in] Communication_p      Communication module context.
 * @param [in] State                State of the speedflash (TRUE - on, FALSE - off).
 *
 * @retval  E_SUCCESS                  After successful execution.
 * @retval
 */
void Do_A2_Speedflash_Start(Communication_t *Communication_p)
{
    A2_SPEEDFLASH(Communication_p)->State = A2_SPEEDFLASH_START;

    C_(printf("a2_speedflash.c (%d): Do_A2_Speedflash_Start\n", __LINE__);)
}

void Do_A2_Speedflash_SetLastBlock(Communication_t *Communication_p)
{
    A2_SPEEDFLASH(Communication_p)->LastBlock = TRUE;

    C_(printf("a2_speedflash.c (%d): Do_A2_Speedflash_SetLastBlock\n", __LINE__);)
}

/**
 * Writes A2 speedflash sub-block on the comm device.
 *
 * @param [in] Communication_p  Communication module context.
 * @param [in] Buffer           Buffer containing the data of the sub-block.
 * @param [in] BufferSize       Size of the speedflash sub-blocks.
 *
 * @retval  E_SUCCESS           After successful execution.
 * @retval
 */
ErrorCode_e Do_A2_Speedflash_WriteBlock(Communication_t *Communication_p, const void *Buffer, const uint32 BufferSize)
{
    A2_SPEEDFLASH(Communication_p)->Outbound_p = (void *)Buffer;
    A2_SPEEDFLASH(Communication_p)->OutboundSize = BufferSize;

    C_(printf("a2_speedflash.c (%d): Do_A2_Speedflash_WriteBlock\n", __LINE__);)

    return E_SUCCESS;
}

ErrorCode_e A2_Speedflash_Poll(Communication_t *Communication_p)
{
    ErrorCode_e ReturValue = E_SUCCESS;

    switch (A2_SPEEDFLASH(Communication_p)->InboundState) {
    case A2_SPEEDFLASH_READ_REQ:
        A2_SPEEDFLASH(Communication_p)->InboundState = A2_SPEEDFLASH_WAIT_READ_REQ;

#ifdef CFG_ENABLE_LOADER_TYPE

        if (E_SUCCESS != Communication_p->CommunicationDevice_p->Read((void *)A2_SPEEDFLASH(Communication_p)->Scratch,
                A2_SPEEDFLASH_REQ_SIZE, A2_Speedflash_ReadCallback, Communication_p->CommunicationDevice_p)) {
            A2_SPEEDFLASH(Communication_p)->InboundState = A2_SPEEDFLASH_READ_REQ;
        }

#else
        (void)Communication_p->CommunicationDevice_p->Read((void *)A2_SPEEDFLASH(Communication_p)->Scratch,
                A2_SPEEDFLASH_REQ_SIZE, A2_Speedflash_ReadCallback, Communication_p->CommunicationDevice_p);
#endif
        break;

    case A2_SPEEDFLASH_WAIT_READ_REQ:
        /* nothing to do */
        break;

    case A2_SPEEDFLASH_WRITE_BLOCK:

        if (NULL != A2_SPEEDFLASH(Communication_p)->Outbound_p) {
            A2_SPEEDFLASH(Communication_p)->InboundState = A2_SPEEDFLASH_WAIT_WRITE_BLOCK;

            if (E_SUCCESS != Communication_p->CommunicationDevice_p->Write(A2_SPEEDFLASH(Communication_p)->Outbound_p,
                    A2_SPEEDFLASH(Communication_p)->OutboundSize, A2_Speedflash_WriteCallback, Communication_p->CommunicationDevice_p)) {
                A2_SPEEDFLASH(Communication_p)->InboundState = A2_SPEEDFLASH_WRITE_BLOCK;
            }
        }

        break;

    case A2_SPEEDFLASH_WAIT_WRITE_BLOCK:
        /* nothing to do */
        break;
    }

    return ReturValue;
}

void A2_Speedflash_WriteCallback(const void *Data_p, const uint32 Length, void *Param_p)
{
    Communication_t *Communication_p = (Communication_t *)(((CommunicationDevice_t *)Param_p)->Object_p);

    A2_SPEEDFLASH(Communication_p)->Outbound_p = NULL;
    A2_SPEEDFLASH(Communication_p)->OutboundSize = 0;

    if (A2_SPEEDFLASH(Communication_p)->LastBlock) {
        A2_SPEEDFLASH(Communication_p)->LastBlock = FALSE;
        A2_SPEEDFLASH(Communication_p)->State = A2_SPEEDFLASH_INACTIVE;

        /* put A2 state machine in error state to reinitialize */
        A2_NETWORK(Communication_p)->Inbound.State = A2_RECEIVE_ERROR;
    } else {
        A2_SPEEDFLASH(Communication_p)->InboundState = A2_SPEEDFLASH_READ_REQ;
    }
}

void A2_Speedflash_ReadCallback(const void *Data_p, const uint32 Length, void *Param_p)
{
    Communication_t *Communication_p = (Communication_t *)(((CommunicationDevice_t *)Param_p)->Object_p);
    A2_CommandData_t CmdData;

    CmdData.Type = A2_SPEEDFLASH_GR;
    CmdData.Payload.Size = A2_SPEEDFLASH_REQ_SIZE;
    CmdData.Payload.Data_p = (uint8 *)malloc(A2_SPEEDFLASH_REQ_SIZE);

    if (NULL == CmdData.Payload.Data_p) {
        A_(printf("a2_speedflash.c(%d): memory allocation failed", __LINE__);)
        return;
    }

    A2_SPEEDFLASH(Communication_p)->InboundState = A2_SPEEDFLASH_WRITE_BLOCK;

    memcpy(CmdData.Payload.Data_p, Data_p, Length);
    Communication_p->Do_CEH_Call_Fn(OBJECT_CEH_CALL(Communication_p), (CommandData_t *)&CmdData);
    free(CmdData.Payload.Data_p);
}

/** @} */
/** @} */
/** @} */
