/*******************************************************************************
 * Copyright (C) ST-Ericsson SA 2011
 * License terms: 3-clause BSD license
 ******************************************************************************/
/**
 * @addtogroup loader_communication_module
 * @{
 *    @addtogroup a2_family
 *    @{
 *      @addtogroup a2_protocol
 *      @{
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "r_a2_family.h"
#include "r_a2_network.h"
#include "r_a2_header.h"
#include "r_a2_transport.h"
#include "r_debug_macro.h"

/*******************************************************************************
 * File scope types, constants and variables
 ******************************************************************************/

/*******************************************************************************
 * Definition of external functions
 ******************************************************************************/


/*
 * A2 family protocols initialization.
 *
 * @param [in] Communication_p Communication module context.
 *
 * @retval  E_SUCCESS                   After successful execution.
 * @retval  E_INVALID_INPUT_PARAMETERS  Invalid input parameter.
 */
ErrorCode_e A2_Family_Init(Communication_t *Communication_p)
{
    ErrorCode_e ReturnValue = E_INVALID_INPUT_PARAMETERS;

    VERIFY(NULL != Communication_p, E_INVALID_INPUT_PARAMETERS);

    Communication_p->FamilyContext_p = malloc(sizeof(A2_FamilyContext_t));
    VERIFY(NULL != Communication_p->FamilyContext_p, E_ALLOCATE_FAILED);

    A2_SPEEDFLASH(Communication_p)->State = A2_SPEEDFLASH_INACTIVE;
    A2_SPEEDFLASH(Communication_p)->LastBlock = FALSE;
    A2_SPEEDFLASH(Communication_p)->Outbound_p = NULL;
    A2_SPEEDFLASH(Communication_p)->OutboundSize = 0;
    Communication_p->CurrentFamilyHash = HASH_CRC16;

    if (NULL != Communication_p->BackupCommBuffer_p) {
        if (Communication_p->BackupCommBufferSize < A2_HEADER_LENGTH) {
            memcpy(A2_NETWORK(Communication_p)->Inbound.Scratch, Communication_p->BackupCommBuffer_p, Communication_p->BackupCommBufferSize);
            A2_NETWORK(Communication_p)->Inbound.ReqData = A2_HEADER_LENGTH - Communication_p->BackupCommBufferSize;
            A2_NETWORK(Communication_p)->Inbound.ReqBuffOffset = Communication_p->BackupCommBufferSize;
            A2_NETWORK(Communication_p)->Inbound.RecBackupData = Communication_p->BackupCommBufferSize;
            Communication_p->BackupCommBufferSize = 0;
        } else {
            /* Copy content of backup buffer into scratch buffer */
            memcpy(A2_NETWORK(Communication_p)->Inbound.Scratch, Communication_p->BackupCommBuffer_p, A2_HEADER_LENGTH);
            /* Move rest of backup data at the beginning of the backup buffer */
            memcpy(Communication_p->BackupCommBuffer_p, Communication_p->BackupCommBuffer_p + A2_HEADER_LENGTH, Communication_p->BackupCommBufferSize - A2_HEADER_LENGTH);
            /* Update the size of the backup buffer */
            Communication_p->BackupCommBufferSize = Communication_p->BackupCommBufferSize - A2_HEADER_LENGTH;
            A2_NETWORK(Communication_p)->Inbound.RecBackupData = A2_HEADER_LENGTH;
        }

        C_(printf("a2_family.c(%d) BackupBuffer=0x%x Size=%d\n", __LINE__, Communication_p->BackupCommBuffer_p, Communication_p->BackupCommBufferSize);)
        A2_NETWORK(Communication_p)->Inbound.State = A2_RECEIVE_HEADER;
        A2_NETWORK(Communication_p)->Inbound.Target_p = A2_NETWORK(Communication_p)->Inbound.Scratch;
        A2_NETWORK(Communication_p)->Inbound.LCM_Error = E_SUCCESS;
    } else {
        Communication_p->BackupCommBufferSize = 0;
    }

    A2_NETWORK(Communication_p)->Inbound.StopTransfer = FALSE;

    ReturnValue = A2_Network_Initialize(Communication_p);
    VERIFY_CONDITION(E_SUCCESS == ReturnValue);
ErrorExit:
    return ReturnValue;
}


/*
 * A2 family protocols shutdown.
 *
 * @param [in] Communication_p Communication module context.
 *
 * @retval  E_SUCCESS                   After successful execution.
 * @retval  E_INVALID_INPUT_PARAMETERS  Invalid input parameter.
 */
ErrorCode_e A2_Family_Shutdown(Communication_t *Communication_p)
{
    ErrorCode_e ReturnValue = E_INVALID_INPUT_PARAMETERS;

    VERIFY(NULL != Communication_p, E_INVALID_INPUT_PARAMETERS);

    if (NULL != Communication_p->BackupCommBuffer_p) {
        free(Communication_p->BackupCommBuffer_p);
        Communication_p->BackupCommBuffer_p = NULL;
    }

    Communication_p->BackupCommBuffer_p = (uint8 *)malloc(A2_HEADER_LENGTH);
    VERIFY(NULL != Communication_p->BackupCommBuffer_p, E_ALLOCATE_FAILED);
    memcpy(Communication_p->BackupCommBuffer_p, A2_NETWORK(Communication_p)->Inbound.Scratch, A2_HEADER_LENGTH);
    Communication_p->BackupCommBufferSize = A2_HEADER_LENGTH;

    C_(printf("a2_family.c(%d) BackupBuffer=0x%x, Size=%d \n", __LINE__, Communication_p->BackupCommBuffer_p, Communication_p->BackupCommBufferSize);)

    Communication_p->CurrentFamilyHash = HASH_NONE;

    ReturnValue = A2_Network_Shutdown(Communication_p);
    VERIFY_CONDITION(E_SUCCESS == ReturnValue);

ErrorExit:

    if (NULL != Communication_p) {
        if (NULL != Communication_p->FamilyContext_p) {
            free(Communication_p->FamilyContext_p);
            Communication_p->FamilyContext_p = NULL;
        }
    }

    A_(printf("a2_family.c(%d) ErrorCode=%d\n", __LINE__, ReturnValue);)

    return ReturnValue;
}

/*
 * A2 Cancel Transmission.
 *
 * @param [in] Communication_p Communication module context.
 * @param [in] PacketsBeforeTransferStop Number of packets that will be transmitted before stopping the transmission.
 *
 * @retval  E_SUCCESS                   After successful execution.
 * @retval  E_INVALID_INPUT_PARAMETERS  Invalid input parameter.
 */
ErrorCode_e A2_CancelReceiver(Communication_t *Communication_p, uint8 PacketsBeforeReceiverStop)
{
    ErrorCode_e ReturnValue = E_SUCCESS;

    VERIFY(NULL != Communication_p, E_INVALID_INPUT_PARAMETERS);

    A2_NETWORK(Communication_p)->Inbound.PacketsBeforeReceiverStop = PacketsBeforeReceiverStop;
    A2_NETWORK(Communication_p)->Inbound.StopTransfer = TRUE;

ErrorExit:
    A_(printf("protrom_family.c(%d) ErrorCode=%d\n", __LINE__, ReturnValue);)
    return ReturnValue;
}


/** @} */
/** @} */
/** @} */
