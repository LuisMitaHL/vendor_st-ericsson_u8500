/*******************************************************************************
 * Copyright (C) ST-Ericsson SA 2011
 * License terms: 3-clause BSD license
 ******************************************************************************/
/**
 *  @addtogroup ldr_communication_serv
 *  @{
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
#include "r_protrom_protocol.h"
#include "r_protrom_network.h"
#include "r_protrom_transport.h"
#include "r_communication_service.h"
#include "r_debug_macro.h"

/*******************************************************************************
 * File scope types, constants and variables
 ******************************************************************************/

/*******************************************************************************
 * Definition of external functions
 ******************************************************************************/

/*
 * Decode received command, and execute.
 *
 * @param [in] Communication_p Communication module context.
 * @param [in] Packet_p        Pointer to the received buffer.
 *
 * @retval E_SUCCESS           After successful execution.
 */
ErrorCode_e Protrom_Process(const Communication_t *const Communication_p, Protrom_Packet_t *Packet_p)
{
    ErrorCode_e ReturnValue = E_SUCCESS;
    CommandData_t CmdData;
    uint8 *DataTmp_p;

    if (NULL == Packet_p) {
        return E_INVALID_INPUT_PARAMETERS;
    }

    //if(PROTO_PROTROM == ((Protrom_Packet_t *)Packet_p)->Header.Protocol)
    //  return E_INVALID_INPUT_PARAMETERS;

    if (NULL != Communication_p->Do_CEH_Call_Fn) {
        CmdData.Payload.Size = Packet_p->Header.PayloadLength + PROTROM_HEADER_LENGTH + PROTROM_CRC_LENGTH;
        DataTmp_p = (uint8 *)malloc(CmdData.Payload.Size);

        if (NULL == DataTmp_p) {
            return E_ALLOCATE_FAILED;
        }

        CmdData.Payload.Data_p = DataTmp_p;
        memcpy(CmdData.Payload.Data_p, Packet_p->Buffer_p, CmdData.Payload.Size);

        free(Packet_p->Buffer_p);
        free(Packet_p);

        ReturnValue = Communication_p->Do_CEH_Call_Fn(OBJECT_CEH_CALL(Communication_p), &CmdData);

        if (NULL != CmdData.Payload.Data_p) {
            free(CmdData.Payload.Data_p);
            CmdData.Payload.Data_p = NULL;
        }
    }

    return ReturnValue;
}

/** @} */
/** @} */
/** @} */
