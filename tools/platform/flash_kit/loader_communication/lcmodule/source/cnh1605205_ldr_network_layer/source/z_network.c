/*******************************************************************************
 * Copyright (C) ST-Ericsson SA 2011
 * License terms: 3-clause BSD license
 ******************************************************************************/
/**
 *  @addtogroup ldr_communication_serv
 *  @{
 *    @addtogroup z_family
 *    @{
 *      @addtogroup ldr_network_layer
 *      @{
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include <stdlib.h>
#include <string.h>
#include "c_system_v2.h"
#include "t_basicdefinitions.h"
#include "r_z_family.h"
#include "r_z_transport.h"
#include "r_z_network.h"
#include "r_z_protocol.h"
#include "r_debug.h"
#include "r_communication_service.h"
#include "r_critical_section.h"

/*******************************************************************************
 * Declaration of file local functions
 ******************************************************************************/

/*******************************************************************************
 * File scope types, constants and variables
 ******************************************************************************/
#define Z_RESET_INBOUND(c, s) do { (c)->ReqData = 0; (c)->RecData = 0; (c)->Target_p = NULL; (c)->State = (s); } while(0);
#define Z_SYNC_HEADER(c, d, t) do { (c)->ReqData = d; (c)->Target_p = t; } while(0);
#define Z_SET_INBOUND(c, s, d) do { (c)->ReqData = d; (c)->RecData = 0; (c)->State = (s); } while(0);

/*******************************************************************************
 * Definition of external functions
 ******************************************************************************/

ErrorCode_e Z_Network_Initialize(Communication_t *Communication_p)
{
    if (NULL == Communication_p) {
        return E_INVALID_INPUT_PARAMETERS;
    }

    memset(Z_NETWORK(Communication_p), 0, sizeof(Z_NetworkContext_t));

    Z_NETWORK(Communication_p)->Outbound.TxCriticalSection = Do_CriticalSection_Create();

    /* Simulate a finished read to get the inbound state-machine going. */
    Z_Network_ReadCallback(NULL, 0, Communication_p->CommunicationDevice_p);

    return E_SUCCESS;
}

ErrorCode_e Z_Network_Shutdown(const Communication_t *const Communication_p)
{
    Do_CriticalSection_Destroy(&(Z_NETWORK(Communication_p)->Outbound.TxCriticalSection));

    return E_SUCCESS;
}

void Z_Network_ReadCallback(const void *Data_p, const uint32 Length, void *Param_p)
{
    Communication_t *Communication_p = (Communication_t *)(((CommunicationDevice_t *)Param_p)->Object_p);
    CommandData_t CmdData;

    Z_NETWORK(Communication_p)->Inbound.RecData += Length;

    if (Z_NETWORK(Communication_p)->Inbound.ReqData == 0) {
        Z_Inbound_t *In_p = &(Z_NETWORK(Communication_p)->Inbound);

        if (In_p->RecData == 0) {
            In_p->ReqData = Z_HEADER_LENGTH;
            In_p->Target_p = In_p->Scratch;
        } else {
            CmdData.Payload.Data_p = In_p->Scratch;
            (void)Communication_p->Do_CEH_Call_Fn(Communication_p->Object_p, &CmdData);
            Z_RESET_INBOUND(In_p, Z_RECEIVE_HEADER);
            Z_SYNC_HEADER(In_p, Z_HEADER_LENGTH, In_p->Scratch);
        }
    }
}

void Z_Network_ReceiverHandler(Communication_t *Communication_p)
{
    uint32 ReqData;
    Z_Inbound_t *In_p = &(Z_NETWORK(Communication_p)->Inbound);

    /* new data for receiving ? */
    if (In_p->ReqData > 0) {
        ReqData = In_p->ReqData;
        In_p->ReqData = 0;
        In_p->RecData = 0;
        C_(printf("z_network.c (%d) ReqData(%d) RecData(%d) \n", __LINE__, ReqData, In_p->RecData);)

#ifdef CFG_ENABLE_LOADER_TYPE

        if (E_SUCCESS != Communication_p->CommunicationDevice_p->Read(In_p->Target_p,
                ReqData, Z_Network_ReadCallback, Communication_p->CommunicationDevice_p)) {
            /* Read failed! Return to previous state. */
            In_p->ReqData = ReqData;
        }

#else
        (void)Communication_p->CommunicationDevice_p->Read(In_p->Target_p,
                ReqData, Z_Network_ReadCallback, Communication_p->CommunicationDevice_p);
#endif
    }

    /* check for receiver synchronization */
    if (In_p->State == Z_RECEIVE_ERROR) {
        In_p->ReqData = 0;
        In_p->RecData = 0;
#ifdef CFG_ENABLE_LOADER_TYPE

        if (TRUE == Communication_p->CommunicationDevice_p->Read(In_p->Target_p,
                Z_HEADER_LENGTH, Z_Network_ReadCallback,
                Communication_p->CommunicationDevice_p)) {
            In_p->State = Z_RECEIVE_HEADER;
        }

#else
        (void)Communication_p->CommunicationDevice_p->Read(In_p->Target_p,
                Z_HEADER_LENGTH, Z_Network_ReadCallback,
                Communication_p->CommunicationDevice_p);
        In_p->State = Z_RECEIVE_HEADER;
#endif
    }
}


void Z_Network_WriteCallback(const void *Data_p, const uint32 Length, void *Param_p)
{
}


ErrorCode_e Z_Network_TransmiterHandler(Communication_t *Communication_p, Z_SendingContent_t *SendingContent_p)
{
    uint8 *Data_p = SendingContent_p->Data_p;
    uint8  Size = SendingContent_p->Size;
    Z_Outbound_t *Out_p = &(Z_NETWORK(Communication_p)->Outbound);

    if (!Do_CriticalSection_Enter(Out_p->TxCriticalSection)) {
        return E_SUCCESS;
    }

    switch (Out_p->State) {
    case Z_SEND_IDLE:
        /* get next packet for transmitting */
        Out_p->State = Z_SEND_PACKET;

        /* FALLTHROUGH */
    case Z_SEND_PACKET:

        if (E_SUCCESS == Communication_p->CommunicationDevice_p->Write(Data_p,
                Size,
                Z_Network_WriteCallback, Communication_p->CommunicationDevice_p)) {
            Out_p->State = Z_SENDING_PACKET;
        } else {
            /* error state ?*/
        }

        /* FALLTHROUGH */
    case Z_SENDING_PACKET:
        Out_p->State = Z_SEND_IDLE;
        break;

    }

    Do_CriticalSection_Leave(Out_p->TxCriticalSection);

    return E_SUCCESS;
}

/*******************************************************************************
 * Definition of internal functions
 ******************************************************************************/


/** @} */
/** @} */
/** @} */
