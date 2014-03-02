/*******************************************************************************
 * Copyright (C) ST-Ericsson SA 2011
 * License terms: 3-clause BSD license
 ******************************************************************************/
/**
 *  @addtogroup ldr_communication_serv
 *  @{
 *    @addtogroup z_family
 *    @{
 *      @addtogroup ldr_transport_layer
 *      @{
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include <stdlib.h>
#include <string.h>

#include "t_basicdefinitions.h"
#include "r_debug.h"
#include "r_z_transport.h"
#include "r_z_network.h"
#include "r_communication_service.h"
#include "r_z_protocol.h"

/*******************************************************************************
 * Declaration of file local functions
 ******************************************************************************/
static ErrorCode_e Z_Process(Communication_t *Communication_p);

/*******************************************************************************
 * File scope types, constants and variables
 ******************************************************************************/

/*******************************************************************************
 * Definition of external functions
 ******************************************************************************/

/*
 * Initializes the transport layer for Z protocol family.
 *
 * @param [in]  Communication_p Communication module context.
 *
 * @retval  E_SUCCESS After successful execution.
 * @retval E_FAILED_TO_INIT_TL unsuccessful initialization.
 * @retval E_ALLOCATE_FAILED failed to allocate memory space.
 */
ErrorCode_e Z_Transport_Initialize(const Communication_t *const Communication_p)
{
    return E_SUCCESS;
}

/*
 * Shut Down the transport layer for Z protocol family.
 *
 * @param [in]  Communication_p Communication module context.
 *
 * @retval E_SUCCESS  After successful execution.
 */
ErrorCode_e Z_Transport_Shutdown(const Communication_t *const Communication_p)
{
    return E_SUCCESS;
}

/*
 * Handles all registered TL processes for Z protocol family.
 *
 * @param [in]  Communication_p Communication module context.
 *
 * @retval  E_SUCCESS After successful execution.
 */
ErrorCode_e Z_Transport_Poll(Communication_t *Communication_p)
{
    return Z_Process(Communication_p);
}


/*
 * Function for sending packet in Z protocol family.
 *
 * @param [in]  Communication_p Communication module context.
 * @param [in]  InputData_p     Pointer to the input data.
 *
 * @retval E_SUCCESS                        After successful execution.
 * @retval E_FAILED_TO_ALLOCATE_COMM_BUFFER Failed to allocate communication
 * buffer.
 */
ErrorCode_e Z_Transport_Send(Communication_t *Communication_p, void *InputDataIn_p)
{
    Z_SendingContent_t SendingContent;

    SendingContent.Size = *((uint8 *)InputDataIn_p);
    SendingContent.Data_p = (uint8 *)InputDataIn_p + 1;

    return Z_Network_TransmiterHandler(Communication_p, &SendingContent);
}

/*******************************************************************************
 * Definition of internal functions
 ******************************************************************************/
/*
 * Handles all registered TL processes for Z protocol family.
 *
 * @param [in]  Communication_p Communication module context.
 *
 * @retval  E_SUCCESS After successful execution.
 */
static ErrorCode_e Z_Process(Communication_t *Communication_p)
{
    ErrorCode_e ReturnValue = E_SUCCESS;

    Z_Network_ReceiverHandler(Communication_p);

    return ReturnValue;
}

/** @} */
/** @} */
/** @} */
