/*******************************************************************************
 * Copyright (C) ST-Ericsson SA 2011
 * License terms: 3-clause BSD license
 ******************************************************************************/
/**
 *  @addtogroup ldr_communication_serv
 *  @{
 *    @addtogroup a2_family
 *    @{
 *      @addtogroup ldr_network_layer
 *      @{
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "c_system_v2.h"
#include "r_basicdefinitions.h"
#include "r_a2_family.h"
#include "r_a2_transport.h"
#include "r_a2_network.h"
#include "r_a2_protocol.h"
#include "r_a2_header.h"
#include "r_communication_service.h"
#include "r_debug.h"
#include "r_debug_macro.h"
#include "r_critical_section.h"

/*******************************************************************************
 * Declaration of file local functions
 ******************************************************************************/
static ErrorCode_e A2_Network_ReceiveHeader(const Communication_t *const Communication_p);
static ErrorCode_e A2_Network_ReceivePayload(Communication_t *Communication_p);
static ErrorCode_e A2_Network_RegisterRetransmission(Communication_t *Communication_p, A2_PacketMeta_t *Packet_p);
static void A2_Network_RetransmissionCallback(Communication_t *Communication_p, const void *const Timer_p, void *Data_p);
static void A2_InHashCallback(const void *const Data_p, uint32 Length, const uint8 *const Hash_p, void *Param_p);
#ifdef  CFG_ENABLE_LOADER_TYPE
static void A2_QueueCallback(const void *const Queue_p, void *Param_p);
#endif  //CFG_ENABLE_LOADER_TYPE

/*******************************************************************************
 * File scope types, constants and variables
 ******************************************************************************/
#define A2_RESET_INBOUND(c, s) do { (c)->ReqData = 0; (c)->RecData = 0; (c)->ReqBuffOffset = 0; (c)->Target_p = NULL; (c)->State = (s); } while(0);
#define A2_SYNC_HEADER(c, d, t) do { (c)->ReqData = d; (c)->Target_p = t; } while(0);
#define A2_SET_INBOUND(c, s, d) do { (c)->ReqData = d; (c)->RecData = 0; (c)->ReqBuffOffset = 0; (c)->State = (s); } while(0);

/*******************************************************************************
 * Definition of external functions
 ******************************************************************************/

/*
 * Initializes the A2 network layer.
 *
 * @param [in,out] Communication_p Communication module context.
 *
 * @retval  E_SUCCESS                   After successful execution.
 * @retval  E_FAILED_TO_INIT_COM_DEVICE Fail to initialize the communication
 *                                      device.
 */
ErrorCode_e A2_Network_Initialize(Communication_t *Communication_p)
{
    memset(A2_NETWORK(Communication_p), 0, sizeof(A2_NetworkContext_t));

    /* Simulate a finished read to get the inbound state-machine going. */
    A2_Network_ReadCallback(NULL, 0, Communication_p->CommunicationDevice_p);
    A2_NETWORK(Communication_p)->Outbound.TxCriticalSection = Do_CriticalSection_Create();

#ifdef  CFG_ENABLE_LOADER_TYPE
    (void)QUEUE(Communication_p, Fifo_SetCallback_Fn)(OBJECT_QUEUE(Communication_p), Communication_p->Outbound_p, QUEUE_NONEMPTY, A2_QueueCallback, Communication_p);
#endif
    A2_NETWORK(Communication_p)->Inbound.LCM_Error  = E_SUCCESS;
    A2_NETWORK(Communication_p)->Outbound.LCM_Error = E_SUCCESS;

    return E_SUCCESS;
}

/*
 * Shut down the A2 network layer.
 *
 * @param [in,out] Communication_p Communication module context.
 *
 * @retval  E_SUCCESS After successful execution.
 */
ErrorCode_e A2_Network_Shutdown(const Communication_t *const Communication_p)
{
    ErrorCode_e ReturnValue = E_SUCCESS;

    (void)QUEUE(Communication_p, Fifo_SetCallback_Fn)(OBJECT_QUEUE(Communication_p), Communication_p->Outbound_p, QUEUE_NONEMPTY, NULL, NULL);
    (void)QUEUE(Communication_p, Fifo_SetCallback_Fn)(OBJECT_QUEUE(Communication_p), Communication_p->Outbound_p, QUEUE_EMPTY, NULL, NULL);


    /* Wait until the all packets in the queue has released. */
    while (!QUEUE(Communication_p, Fifo_IsEmpty_Fn)(OBJECT_QUEUE(Communication_p), Communication_p->Outbound_p)) {
        A2_PacketMeta_t *Packet_p = (A2_PacketMeta_t *)QUEUE(Communication_p, FifoDequeue_Fn)(OBJECT_QUEUE(Communication_p), Communication_p->Outbound_p);

        if (Packet_p->Header.Protocol != A2_PROTOCOL) {
            ReturnValue = A2_Network_PacketRelease(Communication_p, Packet_p);
            VERIFY_CONDITION(E_SUCCESS == ReturnValue);
        }
    }

    (void)QUEUE(Communication_p, Fifo_SetCallback_Fn)(OBJECT_QUEUE(Communication_p), Communication_p->Inbound_p, QUEUE_NONEMPTY, NULL, NULL);
    (void)QUEUE(Communication_p, Fifo_SetCallback_Fn)(OBJECT_QUEUE(Communication_p), Communication_p->Inbound_p, QUEUE_EMPTY, NULL, NULL);

    /* Wait until the all packets in the queue has released. */
    while (!QUEUE(Communication_p, Fifo_IsEmpty_Fn)(OBJECT_QUEUE(Communication_p), Communication_p->Inbound_p)) {
        ReturnValue = A2_Network_PacketRelease(Communication_p, (A2_PacketMeta_t *)QUEUE(Communication_p, FifoDequeue_Fn)(OBJECT_QUEUE(Communication_p), Communication_p->Inbound_p));
    }

    Do_CriticalSection_Destroy(&(A2_NETWORK(Communication_p)->Outbound.TxCriticalSection));

ErrorExit:
    return ReturnValue;
}


/*
 * Handler for received packets in A2 protocol family.
 *
 * This callback function handles the received packets.
 *
 * @param [in] Data_p  Pointer to the received data.
 * @param [in] Length  Length of the received data.
 * @param [in] Param_p Parameters;
 *
 * @return none.
 */
void A2_Network_ReadCallback(const void *Data_p, const uint32 Length, void *Param_p)
{
    IDENTIFIER_NOT_USED(Data_p);
    Communication_t *Communication_p = (Communication_t *)(((CommunicationDevice_t *)Param_p)->Object_p);

    C_(printf("a2_network.c (%d) RecLength(%d) RecBackupData (%d)\n", __LINE__, Length, A2_NETWORK(Communication_p)->Inbound.RecBackupData);)
    A2_NETWORK(Communication_p)->Inbound.RecData += Length + A2_NETWORK(Communication_p)->Inbound.RecBackupData;
    A2_NETWORK(Communication_p)->Inbound.RecBackupData = 0;

    if (A2_NETWORK(Communication_p)->Inbound.ReqData == 0) {
        ErrorCode_e ReturnValue = E_GENERAL_COMMUNICATION_ERROR;

        switch (A2_NETWORK(Communication_p)->Inbound.State) {
        case A2_RECEIVE_HEADER:
            ReturnValue = A2_Network_ReceiveHeader(Communication_p);
            break;

        case A2_RECEIVE_PAYLOAD:
            ReturnValue = A2_Network_ReceivePayload(Communication_p);
            break;

        default:
            A2_NETWORK(Communication_p)->Inbound.State = A2_RECEIVE_HEADER;
            A2_NETWORK(Communication_p)->Inbound.RecData = 0;
            A2_NETWORK(Communication_p)->Inbound.ReqData = A2_HEADER_LENGTH;
            A2_NETWORK(Communication_p)->Inbound.Target_p = A2_NETWORK(Communication_p)->Inbound.Scratch;
            A2_NETWORK(Communication_p)->Inbound.LCM_Error = E_SUCCESS;
            ReturnValue = E_SUCCESS;
            break;
        }

#ifdef  CFG_ENABLE_LOADER_TYPE
        ReturnValue = A2_Network_ReceiverHandler(Communication_p);
#endif

        if (E_SUCCESS != ReturnValue) {
            A2_NETWORK(Communication_p)->Inbound.State = A2_RECEIVE_ERROR;
        }

        A2_NETWORK(Communication_p)->Inbound.LCM_Error = ReturnValue;
    }
}

/*
 * Handler for receiving new data.
 *
 * This function checks if new data has been received.
 *
 * @param [in,out] Communication_p Communication module context.
 *
 * @return  none.
 */
ErrorCode_e A2_Network_ReceiverHandler(Communication_t *Communication_p)
{
    uint32 ReqData;
    uint32 ReqBufferOffset;
    A2_Inbound_t *In_p = &(A2_NETWORK(Communication_p)->Inbound);

    if (In_p->ReqData > 0) {
        if (Communication_p->BackupCommBufferSize != 0) {
            if (Communication_p->BackupCommBufferSize < In_p->ReqData) {
                memcpy(In_p->Target_p + In_p->ReqBuffOffset, Communication_p->BackupCommBuffer_p, Communication_p->BackupCommBufferSize);
                ReqData = In_p->ReqData;
                In_p->ReqData = 0;
                ReqBufferOffset = In_p->ReqBuffOffset;
                In_p->ReqBuffOffset = 0;
                In_p->RecBackupData = Communication_p->BackupCommBufferSize;
                Communication_p->BackupCommBufferSize = 0;
                In_p->RecData = 0;

                C_(printf("a2_network.c (%d) ReqData(%d) RecData(%d)\n", __LINE__, ReqData, In_p->RecData);)
                C_(printf("a2_network.c (%d) Communication_p->BackupCommBufferSize(%d) RecBackupData (%d)\n", __LINE__, Communication_p->BackupCommBufferSize, In_p->RecBackupData);)

#ifdef CFG_ENABLE_LOADER_TYPE

                if (E_SUCCESS != Communication_p->CommunicationDevice_p->Read(
                            In_p->Target_p + ReqBufferOffset + In_p->RecBackupData,
                            ReqData - In_p->RecBackupData, A2_Network_ReadCallback,
                            Communication_p->CommunicationDevice_p)) {
                    /* Read failed! Return to previous state. */
                    In_p->ReqData = ReqData;
                    In_p->ReqBuffOffset = ReqBufferOffset;
                    Communication_p->BackupCommBufferSize = In_p->RecBackupData;
                    In_p->RecBackupData = 0;
                }

#else
                (void)Communication_p->CommunicationDevice_p->Read(
                    In_p->Target_p + ReqBufferOffset + In_p->RecBackupData,
                    ReqData - In_p->RecBackupData, A2_Network_ReadCallback,
                    Communication_p->CommunicationDevice_p);
#endif
            } else {
                /* Copy content of backup buffer into receive buffer */
                memcpy(In_p->Target_p + In_p->ReqBuffOffset, Communication_p->BackupCommBuffer_p, In_p->ReqData);
                /* Move rest of backup data at the beginning of the backup buffer. */
                memcpy(Communication_p->BackupCommBuffer_p, Communication_p->BackupCommBuffer_p + In_p->ReqData, Communication_p->BackupCommBufferSize - In_p->ReqData);
                /* Update the size of the backup buffer to handle only unprocessed data. */
                Communication_p->BackupCommBufferSize = Communication_p->BackupCommBufferSize - In_p->ReqData;

                ReqData = In_p->ReqData;
                In_p->ReqData = 0;
                ReqBufferOffset = In_p->ReqBuffOffset;
                In_p->ReqBuffOffset = 0;
                In_p->RecData = 0;
                A2_Network_ReadCallback(In_p->Target_p + ReqBufferOffset, ReqData, Communication_p->CommunicationDevice_p);
            }
        } else {
            ReqData = In_p->ReqData;
            In_p->ReqData = 0;
            ReqBufferOffset = In_p->ReqBuffOffset;
            In_p->ReqBuffOffset = 0;
            In_p->RecData = 0;

            C_(printf("a2_network.c (%d) ReqData(%d) RecData(%d) \n", __LINE__, ReqData, In_p->RecData);)
            C_(printf("a2_network.c (%d) Communication_p->BackupCommBufferSize(%d) RecBackupData (%d)\n", __LINE__, Communication_p->BackupCommBufferSize, In_p->RecBackupData);)

#ifdef CFG_ENABLE_LOADER_TYPE

            if (E_SUCCESS != Communication_p->CommunicationDevice_p->Read(
                        In_p->Target_p + ReqBufferOffset, ReqData, A2_Network_ReadCallback,
                        Communication_p->CommunicationDevice_p)) {
                /* Read failed! Return to previous state. */
                In_p->ReqData = ReqData;
                In_p->ReqBuffOffset = ReqBufferOffset;
            }

#else
            (void)Communication_p->CommunicationDevice_p->Read(
                In_p->Target_p + ReqBufferOffset, ReqData, A2_Network_ReadCallback,
                Communication_p->CommunicationDevice_p);
#endif
        }
    }

    /* check for receiver synchronization */
    if (In_p->State == A2_RECEIVE_ERROR) {
        A2_RESET_INBOUND(In_p, A2_RECEIVE_HEADER);
        A2_SYNC_HEADER(In_p, A2_HEADER_LENGTH, In_p->Scratch);
        //(void)Communication_p->CommunicationDevice_p->Read(In_p->Target_p, A2_HEADER_LENGTH, A2_Network_ReadCallback, Communication_p->CommunicationDevice_p);
    }

    return A2_NETWORK(Communication_p)->Inbound.LCM_Error;
}

/*
 * Cancel retransmission of packets.
 *
 * @param [in,out] Communication_p Communication module context.
 * @param [in] UniqueKey           Unique key used for identification of packet
 *                                 for retransmission.
 *
 * @return none.
 */
ErrorCode_e A2_Network_CancelRetransmission(const Communication_t *const Communication_p, uint32 UniqueKey)
{
    ErrorCode_e ReturnValue = E_SUCCESS;
    uint32 SequenceNumber = A2_NETWORK(Communication_p)->Outbound.Packet_p->Header.SequenceNumber;

    if (SequenceNumber != UniqueKey) {
        A_(printf("a2_network.c(%d): Cancel packet retransmission error: SequenceNumber = %u, UniqueKey = %u\n", __LINE__, SequenceNumber, UniqueKey);)
        return E_GENERAL_FATAL_ERROR;
    }

    (void)TIMER(Communication_p, TimerRelease_Fn)(OBJECT_TIMER(Communication_p), A2_NETWORK(Communication_p)->RetransmissionContext.TimerKey);
    free(A2_NETWORK(Communication_p)->Outbound.Packet_p->Timer_p);
    ReturnValue = A2_Network_PacketRelease(Communication_p, A2_NETWORK(Communication_p)->Outbound.Packet_p);

    if (E_SUCCESS != ReturnValue) {
        A_(printf("a2_network.c(%d): Packet release failed\n", __LINE__);)
        return ReturnValue;
    }

    A2_NETWORK(Communication_p)->Outbound.State = A2_SEND_IDLE;
    A2_NETWORK(Communication_p)->Outbound.Packet_p = NULL;

    return ReturnValue;
}

/*
 * Network packet allocation in A2 protocol family.
 *
 * @param [in,out] Communication_p Communication module context.
 * @param [in]     BufferSize      Size of buffer used for network packet.
 *
 * @retval  Pointer Pointer to allocated packet meta info.
 * @retval  NULL    if allocation fail.
 */
A2_PacketMeta_t *A2_Network_PacketAllocate(const Communication_t *const Communication_p, int BufferSize)
{
    A2_PacketMeta_t *Meta_p = NULL;
    void *Buffer_p = NULL;
    int  BuffersNr = 0;

    /* Find the first unallocated buffers. */
    Buffer_p = BUFFER(Communication_p, BufferAllocate_Fn)(OBJECT_BUFFER(Communication_p), BufferSize);

    if (NULL == Buffer_p) {
        A_(printf("a2_network.c (%d): ** Buffer allocation fail! **\n", __LINE__);)
        goto ErrorExit;
    }

    B_(printf("a2_network.c (%d): Buffer allocate (0x%x)! **\n", __LINE__, (uint32)Buffer_p);)

    /* packet meta info allocate */
    Meta_p = (A2_PacketMeta_t *)malloc(sizeof(A2_PacketMeta_t));

    if (NULL == Meta_p) {
        A2_NETWORK(Communication_p)->Inbound.LCM_Error = E_ALLOCATE_FAILED;
        goto ErrorExit;
    }

    /* packet meta info setup */
    Meta_p->Buffer_p = (uint8 *)Buffer_p;
    Meta_p->BufferSize = BufferSize;
    Meta_p->Flags = A2_BUF_ALLOCATED;

    do {
        if (NULL == A2_NETWORK(Communication_p)->MetaInfoList[BuffersNr]) {
            A2_NETWORK(Communication_p)->MetaInfoList[BuffersNr] = Meta_p;
            break;
        }

        BuffersNr++;
    } while (BuffersNr < (A2_COMMAND_BUFFER_COUNT));

ErrorExit:
    return Meta_p;
}

/*
 * Network packet release in A2 protocol family.
 *
 * @param [in,out] Communication_p Communication module context.
 * @param [in]     Meta_p          Meta info for used network packet.
 *
 * @retval  E_SUCCESS                   Successful network packet release.
 * @retval  E_INVALID_INPUT_PARAMETERS  Invalid input parameter.
 */
ErrorCode_e A2_Network_PacketRelease(const Communication_t *const Communication_p, A2_PacketMeta_t *Meta_p)
{
    ErrorCode_e ReturnValue = E_SUCCESS;
    int BuffersNr = 0;

    VERIFY(NULL != Meta_p, E_INVALID_INPUT_PARAMETERS);

    /* remove the meta info data from list */
    do {
        if (Meta_p == A2_NETWORK(Communication_p)->MetaInfoList[BuffersNr]) {
            /* release buffer */
            B_(printf("a2_network.c (%d): Buffer release (0x%x)! **\n", __LINE__, (uint32)(Meta_p->Buffer_p));)
            ReturnValue = BUFFER(Communication_p, BufferRelease_Fn)(OBJECT_BUFFER(Communication_p), Meta_p->Buffer_p, Meta_p->BufferSize);
            VERIFY_CONDITION(E_SUCCESS == ReturnValue);
            B_(printf("a2_network.c (%d): Buffer released! **\n", __LINE__);)

            A2_NETWORK(Communication_p)->MetaInfoList[BuffersNr] = NULL;
            memset(Meta_p, 0, sizeof(A2_PacketMeta_t));
            break;
        }

        BuffersNr++;
    } while (BuffersNr < (A2_COMMAND_BUFFER_COUNT));

ErrorExit:
    return ReturnValue;
}

/*
 * Handler function that is called after successful transmission of a packet.
 *
 * If new packet is ready for transmitting it starts
 * the transmission of the packet.
 *
 * @param [in] Data_p  Pointer to the data for transmitting.
 * @param [in] Length  Length of the received data.
 * @param [in] Param_p Parameters.
 *
 * @return none.
 */
void A2_Network_WriteCallback(const void *Data_p, const uint32 Length, void *Param_p)
{
    IDENTIFIER_NOT_USED(Data_p);
    IDENTIFIER_NOT_USED(Length);
    Communication_t *Communication_p = (Communication_t *)(((CommunicationDevice_t *)Param_p)->Object_p);
    A2_Outbound_t *Out_p = &(A2_NETWORK(Communication_p)->Outbound);

    if (A2_SENDING_PAYLOAD == Out_p->State) {
        if (NULL != Out_p->Packet_p->Timer_p) {
            (void)A2_Network_RegisterRetransmission(Communication_p, Out_p->Packet_p);
        } else {
            (void)A2_Network_PacketRelease(Communication_p, Out_p->Packet_p);
            Out_p->State = A2_SEND_IDLE;
            Out_p->Packet_p = NULL;
        }
    } else if (A2_SENDING_HEADER == Out_p->State) {
        Out_p->State = A2_SEND_PAYLOAD;
    } else {
        /* should never happen
           TODO: Error handling */
    }

#ifdef CFG_ENABLE_LOADER_TYPE

    if (E_SUCCESS != A2_Network_TransmiterHandler(Communication_p)) {
        A2_NETWORK(Communication_p)->Outbound.LCM_Error = E_GENERAL_COMMUNICATION_ERROR;
    }

#endif
}


/*******************************************************************************
 * Definition of internal functions
 ******************************************************************************/
static ErrorCode_e A2_Network_ReceiveHeader(const Communication_t *const Communication_p)
{
    A2_Inbound_t *In_p = &(A2_NETWORK(Communication_p)->Inbound);
    ErrorCode_e ReturnValue = E_SUCCESS;

    if (In_p->RecData == 0) {
        In_p->ReqData = A2_HEADER_LENGTH;
        In_p->Target_p = In_p->Scratch;
        In_p->ReqBuffOffset = 0;
    } else {
        if (A2_IsReceivedHeader(In_p)) {
            if (A2_IsValidHeader(In_p->Scratch)) {
                A2_DeserializeHeader(&In_p->Header, In_p->Scratch);
                In_p->Packet_p = A2_Network_PacketAllocate(Communication_p, A2_COMMAND_BUFFER_SIZE);
                VERIFY(NULL != In_p->Packet_p, E_FAILED_TO_FIND_COMM_BUFFER);
                A2_DeserializeHeader(&In_p->Packet_p->Header, In_p->Scratch);
                In_p->Packet_p->Flags = In_p->Packet_p->Flags | A2_BUF_HDR_CRC_OK;
                In_p->Target_p = In_p->Packet_p->Buffer_p;
                memcpy(In_p->Target_p, In_p->Scratch, A2_HEADER_LENGTH);
                In_p->Target_p += A2_HEADER_LENGTH;
                In_p->Packet_p->Payload_p = In_p->Packet_p->Buffer_p + A2_HEADER_LENGTH;

                /* check for expected payload */
                if (In_p->Header.DataLength != 0) {
                    A2_SET_INBOUND(In_p, A2_RECEIVE_PAYLOAD, In_p->Header.DataLength + A2_CRC_LENGTH);
                } else {
                    (void)QUEUE(Communication_p, FifoEnqueue_Fn)(OBJECT_QUEUE(Communication_p), Communication_p->Inbound_p, In_p->Packet_p);

                    /* Sync the header. */
                    In_p->Packet_p = NULL;
                    A2_RESET_INBOUND(In_p, A2_RECEIVE_HEADER);

                    if (A2_SPEEDFLASH_START != A2_SPEEDFLASH(Communication_p)->State) {
                        A2_SYNC_HEADER(In_p, A2_HEADER_LENGTH, In_p->Scratch);
                    }

                    C_(printf("a2_network.c (%d) ReqData(%d) RecData(%d) \n", __LINE__, In_p->ReqData, In_p->RecData);)
                }
            } else {
                /* Sync the header. */
                A2_RESET_INBOUND(In_p, A2_RECEIVE_HEADER);
                A2_SYNC_HEADER(In_p, A2_HEADER_LENGTH, In_p->Scratch);
            }
        }
    }

ErrorExit:
    return ReturnValue;
}

static ErrorCode_e A2_Network_ReceivePayload(Communication_t *Communication_p)
{
    A2_Inbound_t *In_p = &(A2_NETWORK(Communication_p)->Inbound);
    A2_PacketMeta_t *Packet_p = In_p->Packet_p;

    Packet_p->Communication_p = Communication_p;

    if (HASH_NONE != Communication_p->CurrentFamilyHash) {
        Communication_p->HashDevice_p->Calculate(OBJECT_HASH(Communication_p),
                Communication_p->CurrentFamilyHash,
                Packet_p->Buffer_p, Packet_p->Header.DataLength + A2_HEADER_LENGTH,
                Packet_p->CRC, (HashCallback_t)A2_InHashCallback,
                (void *)Packet_p);
    } else {
        Packet_p->Flags = Packet_p->Flags | A2_BUF_PAYLOAD_CRC_OK | A2_BUF_RX_READY;
        (void)QUEUE((Packet_p->Communication_p), FifoEnqueue_Fn)(OBJECT_QUEUE(Packet_p->Communication_p), Packet_p->Communication_p->Inbound_p, Packet_p);
    }

    In_p->Packet_p = NULL;
    A2_RESET_INBOUND(In_p, A2_RECEIVE_HEADER);
    A2_SYNC_HEADER(In_p, A2_HEADER_LENGTH, In_p->Scratch);
    return E_SUCCESS;
}

ErrorCode_e A2_Network_TransmiterHandler(Communication_t *Communication_p)
{
    ErrorCode_e ReturnValue = E_SUCCESS;
    A2_Outbound_t *Out_p = &(A2_NETWORK(Communication_p)->Outbound);

    if (!Do_CriticalSection_Enter(Out_p->TxCriticalSection)) {
        return ReturnValue;
    }

    switch (Out_p->State) {
    case A2_SEND_IDLE:
        /* check retransmission count before send */
        Out_p->Packet_p = (A2_PacketMeta_t *)QUEUE(Communication_p, FifoDequeue_Fn)(OBJECT_QUEUE(Communication_p), Communication_p->Outbound_p);

        if (NULL != Out_p->Packet_p) {
            if (Out_p->Packet_p->Resend < MAX_RESENDS) {
                Out_p->Packet_p->Resend++;
                /* get next packet for transmitting */
                Out_p->State = A2_SEND_HEADER;
            } else {
                return E_RETRANSMITION_FAILED;
            }
        } else {
            break;
        }

        /* FALLTHROUGH */
    case A2_SEND_HEADER:

        /* set next state before calling the communication device, to avoid race condition
           where write callback is called before new state is set */
        if (Out_p->Packet_p->Header.Protocol == PROTO_A2_ACK) {
            Out_p->State = A2_SENDING_PAYLOAD;
        } else {
            Out_p->State = A2_SENDING_HEADER;
        }

        if (E_SUCCESS == Communication_p->CommunicationDevice_p->Write((Out_p->Packet_p->Buffer_p),
                A2_HEADER_LENGTH, A2_Network_WriteCallback, Communication_p->CommunicationDevice_p)) {
            C_(printf("a2_network.c (%d) Header Sent to comm device! \n", __LINE__);)
        } else {
            Out_p->State = A2_SEND_HEADER;
            C_(printf("a2_network.c (%d) Error sending header to comm device! \n", __LINE__);)
        }

        break;

    case A2_SENDING_HEADER:
        /* nothing to do, wait until sending is finished and state changed in write callback */
        break;

    case A2_SEND_PAYLOAD:
        Out_p->State = A2_SENDING_PAYLOAD;

        if (E_SUCCESS == Communication_p->CommunicationDevice_p->Write(
                    (Out_p->Packet_p->Buffer_p + A2_HEADER_LENGTH),
                    Out_p->Packet_p->Header.DataLength + A2_CRC_LENGTH,
                    A2_Network_WriteCallback, Communication_p->CommunicationDevice_p)) {
            C_(printf("a2_network.c (%d) Payload Sent to comm device! \n", __LINE__);)
        } else {
            Out_p->State = A2_SEND_PAYLOAD;
            C_(printf("a2_network.c (%d) Error sending payload to comm device! \n", __LINE__);)
        }

        break;

    case A2_SENDING_PAYLOAD:
        /* nothing to do, wait until sending is finished and state changed when packet ACK is received */
        break;
    }

    Do_CriticalSection_Leave(Out_p->TxCriticalSection);

    return ReturnValue;
}

static ErrorCode_e A2_Network_RegisterRetransmission(Communication_t *Communication_p, A2_PacketMeta_t *Packet_p)
{
    ErrorCode_e ReturnValue = E_SUCCESS;

    if (NULL == Packet_p->Timer_p->HandleFunction_p) {
        Packet_p->Timer_p->HandleFunction_p = (HandleFunction_t)A2_Network_RetransmissionCallback;
        Packet_p->Timer_p->Param_p = Communication_p;
    }

    A2_NETWORK(Communication_p)->RetransmissionContext.TimerKey = TIMER(Communication_p, TimerGet_Fn)(OBJECT_TIMER(Communication_p), Packet_p->Timer_p);
    A2_NETWORK(Communication_p)->RetransmissionContext.Timeout = Packet_p->Timer_p->Time;

    return ReturnValue;
}

static void A2_Network_RetransmissionCallback(Communication_t *Communication_p, const void *const Timer_p, void *Data_p)
{
    (void)QUEUE(Communication_p, FifoEnqueue_Fn)(OBJECT_QUEUE(Communication_p), Communication_p->Outbound_p, Data_p);
}

static void A2_InHashCallback(const void *const Data_p, uint32 Length, const uint8 *const Hash_p, void *Param_p)
{
    IDENTIFIER_NOT_USED(Data_p);
    IDENTIFIER_NOT_USED(Length);
    A2_PacketMeta_t *Packet_p = (A2_PacketMeta_t *)Param_p;

    if (memcmp(Hash_p, Packet_p->Buffer_p + A2_HEADER_LENGTH + Packet_p->Header.DataLength, sizeof(uint16)) == 0) {
        Packet_p->Flags = Packet_p->Flags | A2_BUF_PAYLOAD_CRC_OK | A2_BUF_RX_READY;
        (void)QUEUE((Packet_p->Communication_p), FifoEnqueue_Fn)(OBJECT_QUEUE(Packet_p->Communication_p), Packet_p->Communication_p->Inbound_p, Packet_p);
    } else {
        /* Invalid packet */
        ErrorCode_e ReturnValue = E_GENERAL_FATAL_ERROR;

        if (E_SUCCESS != (ReturnValue = A2_Network_PacketRelease((Communication_t *)Packet_p->Communication_p, Packet_p))) {
            A2_NETWORK((Communication_t *)((A2_PacketMeta_t *)Param_p)->Communication_p)->Inbound.LCM_Error = ReturnValue;
        }
    }
}

#ifdef  CFG_ENABLE_LOADER_TYPE
static void A2_QueueCallback(const void *const Queue_p, void *Param_p)
{
    if (E_SUCCESS != A2_Network_TransmiterHandler((Communication_t *)Param_p)) {
        A2_NETWORK((Communication_t *)Param_p)->Outbound.LCM_Error = E_GENERAL_COMMUNICATION_ERROR;
    }
}
#endif //CFG_ENABLE_LOADER_TYPE

/** @} */
/** @} */
/** @} */
