/*******************************************************************************
 * Copyright (C) ST-Ericsson SA 2011
 * License terms: 3-clause BSD license
 ******************************************************************************/
/**
 * @addtogroup ldr_communication_serv
 * @{
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "r_r15_family.h"
#include "r_r15_transport_layer.h"
#include "r_r15_network_layer.h"
#include "r_debug_macro.h"
/*******************************************************************************
 * File scope types, constants and variables
 ******************************************************************************/

/*******************************************************************************
 * Definition of external functions
 ******************************************************************************/

/*
 * R15 family protocols initialization.
 *
 * @param [in] Communication_p Communication module context.
 *
 * @retval  E_SUCCESS                   After successful execution.
 * @retval  E_INVALID_INPUT_PARAMETERS  Invalid input parameter.
 */
ErrorCode_e R15_Family_Init(Communication_t *Communication_p)
{
    ErrorCode_e ReturnValue = E_INVALID_INPUT_PARAMETERS;

    VERIFY(NULL != Communication_p, E_INVALID_INPUT_PARAMETERS);

    Communication_p->FamilyContext_p = malloc(sizeof(R15_FamilyContext_t));
    VERIFY(NULL != Communication_p->FamilyContext_p, E_ALLOCATE_FAILED);

#ifndef CFG_ENABLE_LOADER_TYPE
    Communication_p->CurrentFamilyHash = HASH_NONE;
#else

#ifdef SKIP_PAYLOAD_VERIFICATION
    Communication_p->CurrentFamilyHash = HASH_NONE;
#else
    Communication_p->CurrentFamilyHash = HASH_SHA256;
#endif

#endif

    R15_NETWORK(Communication_p)->Inbound.StopTransfer = FALSE;

    if (NULL != Communication_p->BackupCommBuffer_p) {
        if (Communication_p->BackupCommBufferSize < ALIGNED_HEADER_LENGTH) {
            memcpy(R15_NETWORK(Communication_p)->Inbound.Scratch, Communication_p->BackupCommBuffer_p, Communication_p->BackupCommBufferSize);
            R15_NETWORK(Communication_p)->Inbound.ReqData = ALIGNED_HEADER_LENGTH - Communication_p->BackupCommBufferSize;
            R15_NETWORK(Communication_p)->Inbound.ReqBuffOffset = Communication_p->BackupCommBufferSize;
            R15_NETWORK(Communication_p)->Inbound.RecBackupData = Communication_p->BackupCommBufferSize;
            Communication_p->BackupCommBufferSize = 0;
        } else {
            /* Copy content of backup buffer into scratch buffer */
            memcpy(R15_NETWORK(Communication_p)->Inbound.Scratch, Communication_p->BackupCommBuffer_p, ALIGNED_HEADER_LENGTH);
            /* Move rest of backup data at the beginning of the backup buffer */
            memcpy(Communication_p->BackupCommBuffer_p, Communication_p->BackupCommBuffer_p + ALIGNED_HEADER_LENGTH, Communication_p->BackupCommBufferSize - ALIGNED_HEADER_LENGTH);
            /* Update the size of the backup buffer */
            Communication_p->BackupCommBufferSize = Communication_p->BackupCommBufferSize - ALIGNED_HEADER_LENGTH;
            R15_NETWORK(Communication_p)->Inbound.RecBackupData = ALIGNED_HEADER_LENGTH;
        }

        C_(printf("r15_family.c(%d) BackupBuffer=0x%x Size=%d\n", __LINE__, Communication_p->BackupCommBuffer_p, Communication_p->BackupCommBufferSize);)
        R15_NETWORK(Communication_p)->Inbound.State = RECEIVE_HEADER;
        R15_NETWORK(Communication_p)->Inbound.Target_p = R15_NETWORK(Communication_p)->Inbound.Scratch;
        R15_NETWORK(Communication_p)->Inbound.LCM_Error = E_SUCCESS;
    } else {
        Communication_p->BackupCommBufferSize = 0;
    }

    ReturnValue = R15_Transport_Initialize(Communication_p);
    VERIFY_CONDITION(E_SUCCESS == ReturnValue);

    ReturnValue = R15_Network_Initialize(Communication_p);

ErrorExit:
    A_(printf("r15_family.c(%d) ErrorCode=%d\n", __LINE__, ReturnValue);)
    return ReturnValue;
}


/*
 * R15 family protocols shutdown.
 *
 * @param [in] Communication_p Communication module context.
 *
 * @retval  E_SUCCESS                   After successful execution.
 * @retval  E_INVALID_INPUT_PARAMETERS  Invalid input parameter.
 */
ErrorCode_e R15_Family_Shutdown(Communication_t *Communication_p)
{
    ErrorCode_e ReturnValue = E_INVALID_INPUT_PARAMETERS;

    VERIFY(NULL != Communication_p, E_INVALID_INPUT_PARAMETERS);

    if (NULL != Communication_p->BackupCommBuffer_p) {
        free(Communication_p->BackupCommBuffer_p);
        Communication_p->BackupCommBuffer_p = NULL;
    }

    Communication_p->BackupCommBuffer_p = (uint8 *)malloc(HEADER_LENGTH + BULK_EXTENDED_HEADER_LENGTH);
    VERIFY(NULL != Communication_p->BackupCommBuffer_p, E_ALLOCATE_FAILED);
    memcpy(Communication_p->BackupCommBuffer_p, R15_NETWORK(Communication_p)->Inbound.Scratch, HEADER_LENGTH + BULK_EXTENDED_HEADER_LENGTH);
    Communication_p->BackupCommBufferSize = HEADER_LENGTH + BULK_EXTENDED_HEADER_LENGTH;
    C_(printf("r15_family.c(%d) BackupBuffer=0x%x, Size=%d \n", __LINE__, Communication_p->BackupCommBuffer_p, Communication_p->BackupCommBufferSize);)

    Communication_p->CurrentFamilyHash = HASH_NONE;

    ReturnValue = R15_Transport_Shutdown(Communication_p);
    VERIFY_CONDITION(E_SUCCESS == ReturnValue);

    ReturnValue = R15_Network_Shutdown(Communication_p);
    VERIFY_CONDITION(E_SUCCESS == ReturnValue);

ErrorExit:

    if (NULL != Communication_p) {
        if (NULL != Communication_p->FamilyContext_p) {
            free(Communication_p->FamilyContext_p);
            Communication_p->FamilyContext_p = NULL;
        }
    }

    A_(printf("r15_family.c(%d) ErrorCode=%d\n", __LINE__, ReturnValue);)

    return ReturnValue;
}

/*
 * R15 Cancel Transmission.
 *
 * @param [in] Communication_p Communication module context.
 * @param [in] PacketsBeforeTransferStop Number of packets that will be transmitted before stopping the transmission.
 *
 * @retval  E_SUCCESS                   After successful execution.
 * @retval  E_INVALID_INPUT_PARAMETERS  Invalid input parameter.
 */
ErrorCode_e R15_CancelReceiver(Communication_t *Communication_p, uint8 PacketsBeforeReceiverStop)
{
    ErrorCode_e ReturnValue = E_SUCCESS;

    VERIFY(NULL != Communication_p, E_INVALID_INPUT_PARAMETERS);

    R15_NETWORK(Communication_p)->Inbound.PacketsBeforeReceiverStop = PacketsBeforeReceiverStop;
    R15_NETWORK(Communication_p)->Inbound.StopTransfer = TRUE;

ErrorExit:
    A_(printf("protrom_family.c(%d) ErrorCode=%d\n", __LINE__, ReturnValue);)
    return ReturnValue;
}

/** @} */
