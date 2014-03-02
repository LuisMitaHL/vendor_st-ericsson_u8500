/*******************************************************************************
 * Copyright (C) ST-Ericsson SA 2011
 * License terms: 3-clause BSD license
 ******************************************************************************/
/**
 * @addtogroup ldr_communication_serv
 * @{
 *    @addtogroup Z_family
 *    @{
 *      @addtogroup Z_protocol
 *      @{
 */


/*******************************************************************************
 * Includes
 ******************************************************************************/
#include <stdlib.h>
#include <string.h>

#include "r_z_family.h"
#include "r_z_protocol.h"
#include "r_z_network.h"
#include "r_z_transport.h"
#include "r_debug_macro.h"

/*******************************************************************************
 * File scope types, constants and variables
 ******************************************************************************/

/*******************************************************************************
 * Definition of external functions
 ******************************************************************************/

/*
 * Z family protocols initialization.
 *
 * Z transport and Z network layer will be initialized.
 *
 * @param [in] Communication_p Communication module context.
 *
 * @retval  E_SUCCESS                   After successful execution.
 * @retval  E_INVALID_INPUT_PARAMETERS  Invalid input parameter.
 */
ErrorCode_e Z_Family_Init(Communication_t *Communication_p)
{
    ErrorCode_e ReturnValue = E_INVALID_INPUT_PARAMETERS;

    if (NULL == Communication_p) {
        return E_INVALID_INPUT_PARAMETERS;
    }

    if (NULL != Communication_p->BackupCommBuffer_p) {
        free(Communication_p->BackupCommBuffer_p);
        Communication_p->BackupCommBuffer_p = NULL;
    }

    Communication_p->BackupCommBufferSize = 0;

    Communication_p->FamilyContext_p = malloc(sizeof(Z_FamilyContext_t));

    if (NULL == Communication_p->FamilyContext_p) {
        return E_ALLOCATE_FAILED;
    }

    Communication_p->CurrentFamilyHash = HASH_NONE;
    Z_NETWORK(Communication_p)->Inbound.StopTransfer = FALSE;

    ReturnValue = Z_Transport_Initialize(Communication_p);
    VERIFY_CONDITION(E_SUCCESS == ReturnValue);

    ReturnValue = Z_Network_Initialize(Communication_p);

ErrorExit:
    return ReturnValue;
}

/*
 * Z family protocols shutdown.
 *
 * Z transport and Z network layer will be shutdown.
 *
 * @param [in] Communication_p Communication module context.
 *
 * @retval  E_SUCCESS                   After successful execution.
 * @retval  E_INVALID_INPUT_PARAMETERS  Invalid input parameter.
 */
ErrorCode_e Z_Family_Shutdown(Communication_t *Communication_p)
{
    ErrorCode_e ReturnValue = E_INVALID_INPUT_PARAMETERS;

    if (NULL == Communication_p) {
        return E_INVALID_INPUT_PARAMETERS;
    }

    ReturnValue = Z_Transport_Shutdown(Communication_p);
    VERIFY_CONDITION(E_SUCCESS == ReturnValue);

    ReturnValue = Z_Network_Shutdown(Communication_p);
    VERIFY_CONDITION(E_SUCCESS == ReturnValue);
    free(Communication_p->FamilyContext_p);
    Communication_p->FamilyContext_p = NULL;

    if (NULL != Communication_p->BackupCommBuffer_p) {
        free(Communication_p->BackupCommBuffer_p);
        Communication_p->BackupCommBuffer_p = NULL;
    }

    Communication_p->BackupCommBufferSize = 0;
    Communication_p->CurrentFamilyHash = HASH_NONE;

ErrorExit:

    return ReturnValue;
}

/*
 * Z family Cancel Transmission.
 *
 * @param [in] Communication_p Communication module context.
 * @param [in] PacketsBeforeTransferStop Number of packets that will be transmitted before stopping the transmission.
 *
 * @retval  E_SUCCESS                   After successful execution.
 * @retval  E_INVALID_INPUT_PARAMETERS  Invalid input parameter.
 */
ErrorCode_e Z_CancelReceiver(Communication_t *Communication_p, uint8 PacketsBeforeReceiverStop)
{
    ErrorCode_e ReturnValue = E_SUCCESS;

    VERIFY(NULL != Communication_p, E_INVALID_INPUT_PARAMETERS);

    Z_NETWORK(Communication_p)->Inbound.PacketsBeforeReceiverStop = PacketsBeforeReceiverStop;
    Z_NETWORK(Communication_p)->Inbound.StopTransfer = TRUE;

ErrorExit:
    A_(printf("protrom_family.c(%d) ErrorCode=%d\n", __LINE__, ReturnValue);)
    return ReturnValue;
}

/**         @} */
/**     @} */
/** @} */
