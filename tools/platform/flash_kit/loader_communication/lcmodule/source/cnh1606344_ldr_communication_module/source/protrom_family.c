/*******************************************************************************
 * Copyright (C) ST-Ericsson SA 2011
 * License terms: 3-clause BSD license
 ******************************************************************************/
/**
 * @addtogroup ldr_communication_serv
 * @{
 *    @addtogroup protrom_family
 *    @{
 *      @addtogroup protrom_protocol
 *      @{
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include <stdlib.h>
#include <string.h>

#include "r_protrom_family.h"
#include "r_protrom_network.h"
#include "r_protrom_header.h"
#include "r_protrom_transport.h"
#include "r_debug_macro.h"

/*******************************************************************************
 * File scope types, constants and variables
 ******************************************************************************/

/*******************************************************************************
 * Definition of external functions
 ******************************************************************************/

/*
 * Protrom family protocols initialization.
 *
 * @param [in] Communication_p Communication module context.
 *
 * @retval  E_SUCCESS                   After successful execution.
 * @retval  E_INVALID_INPUT_PARAMETERS  Invalid input parameter.
 */
ErrorCode_e Protrom_Family_Init(Communication_t *Communication_p)
{
    ErrorCode_e ReturnValue = E_INVALID_INPUT_PARAMETERS;

    VERIFY(NULL != Communication_p, E_INVALID_INPUT_PARAMETERS);

    Communication_p->FamilyContext_p = malloc(sizeof(Protrom_FamilyContext_t));
    VERIFY(NULL != Communication_p->FamilyContext_p, E_ALLOCATE_FAILED);

    Communication_p->CurrentFamilyHash = HASH_CRC16;
    PROTROM_NETWORK(Communication_p)->Inbound.StopTransfer = FALSE;

    if (NULL != Communication_p->BackupCommBuffer_p) {
        if (Communication_p->BackupCommBufferSize < PROTROM_HEADER_LENGTH) {
            memcpy(PROTROM_NETWORK(Communication_p)->Inbound.Scratch, Communication_p->BackupCommBuffer_p, Communication_p->BackupCommBufferSize);
            PROTROM_NETWORK(Communication_p)->Inbound.ReqData = PROTROM_HEADER_LENGTH - Communication_p->BackupCommBufferSize;
            PROTROM_NETWORK(Communication_p)->Inbound.ReqBuffOffset = Communication_p->BackupCommBufferSize;
            PROTROM_NETWORK(Communication_p)->Inbound.RecBackupData = Communication_p->BackupCommBufferSize;
            Communication_p->BackupCommBufferSize = 0;
        } else {
            /* Copy content of backup buffer into scratch buffer */
            memcpy(PROTROM_NETWORK(Communication_p)->Inbound.Scratch, Communication_p->BackupCommBuffer_p, PROTROM_HEADER_LENGTH);
            /* Move rest of backup data at the beginning of the backup buffer */
            memcpy(Communication_p->BackupCommBuffer_p, Communication_p->BackupCommBuffer_p + PROTROM_HEADER_LENGTH, Communication_p->BackupCommBufferSize - PROTROM_HEADER_LENGTH);
            /* Update the size of the backup buffer */
            Communication_p->BackupCommBufferSize = Communication_p->BackupCommBufferSize - PROTROM_HEADER_LENGTH;
            PROTROM_NETWORK(Communication_p)->Inbound.RecBackupData = PROTROM_HEADER_LENGTH;
        }

        C_(printf("protrom_family.c(%d) BackupBuffer=0x%x Size=%d\n", __LINE__, Communication_p->BackupCommBuffer_p, Communication_p->BackupCommBufferSize);)
        PROTROM_NETWORK(Communication_p)->Inbound.State = PROTROM_RECEIVE_HEADER;
        PROTROM_NETWORK(Communication_p)->Inbound.Target_p = PROTROM_NETWORK(Communication_p)->Inbound.Scratch;
    } else {
        Communication_p->BackupCommBufferSize = 0;
    }

    ReturnValue = Protrom_Network_Initialize(Communication_p);

ErrorExit:
    A_(printf("protrom_family.c(%d) ErrorCode=%d\n", __LINE__, ReturnValue);)
    return ReturnValue;
}

/*
 * Protrom family protocols shutdown.
 *
 * @param [in] Communication_p Communication module context.
 *
 * @retval  E_SUCCESS                   After successful execution.
 * @retval  E_INVALID_INPUT_PARAMETERS  Invalid input parameter.
 */
ErrorCode_e Protrom_Family_Shutdown(Communication_t *Communication_p)
{
    ErrorCode_e ReturnValue = E_SUCCESS;

    VERIFY(NULL != Communication_p, E_INVALID_INPUT_PARAMETERS);

    if (NULL != Communication_p->BackupCommBuffer_p) {
        free(Communication_p->BackupCommBuffer_p);
    }

    Communication_p->BackupCommBuffer_p = (uint8 *)malloc(PROTROM_HEADER_LENGTH);
    VERIFY(NULL != Communication_p->BackupCommBuffer_p, E_ALLOCATE_FAILED);
    memcpy(Communication_p->BackupCommBuffer_p, PROTROM_NETWORK(Communication_p)->Inbound.Scratch, PROTROM_HEADER_LENGTH);
    Communication_p->BackupCommBufferSize = PROTROM_HEADER_LENGTH;
    C_(printf("protrom_family.c(%d) BackupBuffer=0x%x Size=%d\n", __LINE__, Communication_p->BackupCommBuffer_p, Communication_p->BackupCommBufferSize);)

    Communication_p->CurrentFamilyHash = HASH_NONE;

    ReturnValue = Protrom_Network_Shutdown(Communication_p);
    VERIFY_CONDITION(E_SUCCESS == ReturnValue);

ErrorExit:

    if (NULL != Communication_p) {
        if (NULL != Communication_p->FamilyContext_p) {
            free(Communication_p->FamilyContext_p);
            Communication_p->FamilyContext_p = NULL;
        }
    }

    A_(printf("protrom_family.c(%d) ErrorCode=%d\n", __LINE__, ReturnValue);)

    return ReturnValue;
}

/*
 * Protrom Cancel Transmission.
 *
 * @param [in] Communication_p Communication module context.
 * @param [in] PacketsBeforeTransferStop Number of packets that will be transmitted before stopping the transmission.
 *
 * @retval  E_SUCCESS                   After successful execution.
 * @retval  E_INVALID_INPUT_PARAMETERS  Invalid input parameter.
 */
ErrorCode_e Protrom_CancelReceiver(Communication_t *Communication_p, uint8 PacketsBeforeReceiverStop)
{
    ErrorCode_e ReturnValue = E_SUCCESS;

    A_(printf("Protrom_CancelReceiver called ...\n");)

    VERIFY(NULL != Communication_p, E_INVALID_INPUT_PARAMETERS);

    PROTROM_NETWORK(Communication_p)->Inbound.PacketsBeforeReceiverStop = PacketsBeforeReceiverStop;
    PROTROM_NETWORK(Communication_p)->Inbound.StopTransfer = TRUE;

ErrorExit:
    A_(printf("protrom_family.c(%d) ErrorCode=%d\n", __LINE__, ReturnValue);)
    return ReturnValue;
}

/**         @} */
/**     @} */
/** @} */
