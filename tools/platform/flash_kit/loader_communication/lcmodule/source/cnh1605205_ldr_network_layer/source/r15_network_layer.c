/*******************************************************************************
 * Copyright (C) ST-Ericsson SA 2011
 * License terms: 3-clause BSD license
 ******************************************************************************/
/**
 *  @addtogroup ldr_communication_serv
 *  @{
 *    @addtogroup r15_family
 *    @{
 *      @addtogroup ldr_network_layer
 *      @{
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include <stdlib.h>
#include <string.h>
#include "r_r15_network_layer.h"
#include "r_basicdefinitions.h"
#include "r_debug.h"
#include "r_debug_macro.h"
#include "c_system_v2.h"
#include "r_r15_transport_layer.h"
#include "r_r15_family.h"
#include "r_r15_header.h"
#include "r_bulk_protocol.h"
#include "r_command_protocol.h"
#include "r_critical_section.h"

#ifdef  CFG_ENABLE_MEASUREMENT_TOOL
#include "r_measurement_tool.h"
#include "r_time_utilities.h"
#endif

#ifdef  CFG_ENABLE_MEASUREMENT_TOOL
extern  Measurement_t *Measurement_p;
#endif

static PacketMeta_t PacketMetaInfo[COMMAND_BUFFER_COUNT + BULK_BUFFER_COUNT] = {{0}}; /* Packet Meta Info vector*/

/*******************************************************************************
 * Declaration of file local functions
 ******************************************************************************/
static ErrorCode_e R15_Network_ReceiveHeader(const Communication_t *const Communication_p);
static ErrorCode_e R15_Network_ReceiveExtendedHeader(Communication_t *Communication_p);
static ErrorCode_e R15_Network_ReceivePayload(Communication_t *Communication_p);
static ErrorCode_e R15_Network_RegisterRetransmission(Communication_t *Communication_p, PacketMeta_t *Packet_p);
static void R15_Network_RetransmissionCallback(Communication_t *Communication_p, const void *const Timer_p, void *Data_p);
static void R15_InHashCallback(const void *const Data_p, uint32 Length, const uint8 *const Hash_p, void *Param_p);
#ifdef  CFG_ENABLE_LOADER_TYPE
static void R15_QueueOutCallback(const void *const Queue_p, void *Param_p);
static void R15_QueueInCallback(const void *const Queue_p, void *Param_p);
#endif
static PacketMeta_t *R15_Network_GetAvailableMetaPacket(void);


/*******************************************************************************
 * File scope types, constants and variables
 ******************************************************************************/

#define RESET_INBOUND(c, s) do { (c)->ReqData = 0; (c)->RecData = 0; (c)->ReqBuffOffset = 0; (c)->Target_p = NULL; (c)->State = (s); } while(0);
#define SYNC_HEADER(c, d, t) do { (c)->ReqData = d; (c)->Target_p = t; } while(0);
#define SET_INBOUND(c, s, d) do { (c)->ReqData = d; (c)->RecData = 0; (c)->ReqBuffOffset = 0; (c)->State = (s); } while(0);

/*******************************************************************************
 * Definition of external functions
 ******************************************************************************/

/*
 * Initializes the r15 network layer.
 *
 * @param [in,out] Communication_p Communication module context.
 *
 * @retval  E_SUCCESS                   After successful execution.
 * @retval  E_FAILED_TO_INIT_COM_DEVICE Fail to initialize the communication
 *                                      device.
 */
ErrorCode_e R15_Network_Initialize(Communication_t *Communication_p)
{
    memset(R15_NETWORK(Communication_p), 0, sizeof(R15_NetworkContext_t));
    R15_NETWORK(Communication_p)->Outbound.TxCriticalSection = Do_CriticalSection_Create();

    /* Simulate a finished read to get the inbound state-machine going. */
    R15_Network_ReadCallback(NULL, 0, Communication_p->CommunicationDevice_p);
#ifdef  CFG_ENABLE_LOADER_TYPE
    (void)QUEUE(Communication_p, Fifo_SetCallback_Fn)(OBJECT_QUEUE(Communication_p), Communication_p->Outbound_p, QUEUE_NONEMPTY, R15_QueueOutCallback, Communication_p);
    (void)QUEUE(Communication_p, Fifo_SetCallback_Fn)(OBJECT_QUEUE(Communication_p), Communication_p->Inbound_p, QUEUE_NONEMPTY, R15_QueueInCallback, Communication_p);
#endif
    R15_NETWORK(Communication_p)->Inbound.LCM_Error  = E_SUCCESS;
    R15_NETWORK(Communication_p)->Outbound.LCM_Error = E_SUCCESS;

    return E_SUCCESS;
}

/*
 * Shutdown the R15 Network layer.
 *
 * @param [in,out] Communication_p Communication module context.
 *
 * @retval  E_SUCCESS After successful execution.
 */
ErrorCode_e R15_Network_Shutdown(const Communication_t *const Communication_p)
{
    ErrorCode_e ReturnValue = E_SUCCESS;

    (void)QUEUE(Communication_p, Fifo_SetCallback_Fn)(OBJECT_QUEUE(Communication_p), Communication_p->Outbound_p, QUEUE_NONEMPTY, NULL, NULL);
    (void)QUEUE(Communication_p, Fifo_SetCallback_Fn)(OBJECT_QUEUE(Communication_p), Communication_p->Outbound_p, QUEUE_EMPTY, NULL, NULL);

    /* Wait until the all packets in the queue has released. */
    while (!QUEUE(Communication_p, Fifo_IsEmpty_Fn)(OBJECT_QUEUE(Communication_p), Communication_p->Outbound_p)) {
        PacketMeta_t *Packet_p = (PacketMeta_t *)QUEUE(Communication_p, FifoDequeue_Fn)(OBJECT_QUEUE(Communication_p), Communication_p->Outbound_p);
        BulkExtendedHeader_t ExtendedHeader = {0};

        R15_DeserializeBulkExtendedHeader(&ExtendedHeader, Packet_p->ExtendedHeader_p);

        if (Packet_p->Header.Protocol != BULK_PROTOCOL || (ExtendedHeader.TypeFlags & MASK_BULK_COMMAND_SELECT) != CMD_BULK_DATA) {
            ReturnValue = R15_Network_PacketRelease(Communication_p, Packet_p);
            VERIFY_CONDITION(E_SUCCESS == ReturnValue);
        }
    }

    (void)QUEUE(Communication_p, Fifo_SetCallback_Fn)(OBJECT_QUEUE(Communication_p), Communication_p->Inbound_p, QUEUE_NONEMPTY, NULL, NULL);
    (void)QUEUE(Communication_p, Fifo_SetCallback_Fn)(OBJECT_QUEUE(Communication_p), Communication_p->Inbound_p, QUEUE_EMPTY, NULL, NULL);

    /* Wait until the all packets in the queue has released. */
    while (!QUEUE(Communication_p, Fifo_IsEmpty_Fn)(OBJECT_QUEUE(Communication_p), Communication_p->Inbound_p)) {
        ReturnValue = R15_Network_PacketRelease(Communication_p, (PacketMeta_t *)QUEUE(Communication_p, FifoDequeue_Fn)(OBJECT_QUEUE(Communication_p), Communication_p->Inbound_p));
    }

    Do_CriticalSection_Destroy(&(R15_NETWORK(Communication_p)->Outbound.TxCriticalSection));

ErrorExit:
    return ReturnValue;
}


/*
 * Handler for received packets in R15 protocol family.
 *
 * This callback function handles the received packets.
 *
 * @param [in] Data_p  Pointer to the received data.
 * @param [in] Length  Length of the received data.
 * @param [in] Param_p Parameters;
 *
 * @return none.
 */
void R15_Network_ReadCallback(const void *Data_p, const uint32 Length, void *Param_p)
{
    Communication_t *Communication_p = (Communication_t *)(((CommunicationDevice_t *)Param_p)->Object_p);

    C_(printf("r15_network_layer.c (%d) RecLength(%d) RecBackupData (%d)\n", __LINE__, Length, R15_NETWORK(Communication_p)->Inbound.RecBackupData);)
    R15_NETWORK(Communication_p)->Inbound.RecData = Length + R15_NETWORK(Communication_p)->Inbound.RecBackupData;
    R15_NETWORK(Communication_p)->Inbound.RecBackupData = 0;

    if (R15_NETWORK(Communication_p)->Inbound.ReqData == 0) {
        ErrorCode_e ReturnValue = E_GENERAL_COMMUNICATION_ERROR;

        switch (R15_NETWORK(Communication_p)->Inbound.State) {
        case RECEIVE_HEADER:
            ReturnValue = R15_Network_ReceiveHeader(Communication_p);
            break;

        case RECEIVE_EXTENDED_HEADER:
            ReturnValue = R15_Network_ReceiveExtendedHeader(Communication_p);
            break;

        case RECEIVE_PAYLOAD:
            ReturnValue = R15_Network_ReceivePayload(Communication_p);
            break;

        default:
            R15_NETWORK(Communication_p)->Inbound.State = RECEIVE_HEADER;
            R15_NETWORK(Communication_p)->Inbound.RecData = 0;
            R15_NETWORK(Communication_p)->Inbound.ReqData = ALIGNED_HEADER_LENGTH;
            R15_NETWORK(Communication_p)->Inbound.Target_p = R15_NETWORK(Communication_p)->Inbound.Scratch;
            R15_NETWORK(Communication_p)->Inbound.LCM_Error = E_SUCCESS;
            ReturnValue = E_SUCCESS;
            break;
        }

#ifdef  CFG_ENABLE_LOADER_TYPE
        ReturnValue = R15_Network_ReceiverHandler(Communication_p);
#endif

        if (E_SUCCESS != ReturnValue) {
            R15_NETWORK(Communication_p)->Inbound.State = RECEIVE_ERROR;
        }

        R15_NETWORK(Communication_p)->Inbound.LCM_Error = ReturnValue;
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
ErrorCode_e R15_Network_ReceiverHandler(Communication_t *Communication_p)
{
    uint32 ReqData;
    uint32 ReqBufferOffset;
    R15_Inbound_t *In_p = &(R15_NETWORK(Communication_p)->Inbound);

    /* new data for receiving ? */
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

                C_(printf("r15_network_layer.c (%d) ReqData(%d) RecData(%d)\n", __LINE__, ReqData, In_p->RecData);)
                C_(printf("r15_network_layer.c (%d) Communication_p->BackupCommBufferSize(%d) RecBackupData (%d)\n", __LINE__, Communication_p->BackupCommBufferSize, In_p->RecBackupData);)

#ifdef CFG_ENABLE_LOADER_TYPE

                if (E_SUCCESS != Communication_p->CommunicationDevice_p->Read(
                    In_p->Target_p + ReqBufferOffset + In_p->RecBackupData,
                    ReqData - In_p->RecBackupData, R15_Network_ReadCallback,
                    Communication_p->CommunicationDevice_p)) {
                    /* Read failed! Return to previous state. */
                    A_(printf("r15_network_layer.c (%d) R15_Network_ReceiverHandler() Read Failed!\n", __LINE__);)
                    In_p->ReqData = ReqData;
                    In_p->ReqBuffOffset = ReqBufferOffset;
                    Communication_p->BackupCommBufferSize = In_p->RecBackupData;
                    In_p->RecBackupData = 0;
                }

#else
                (void)Communication_p->CommunicationDevice_p->Read(
                    In_p->Target_p + ReqBufferOffset + In_p->RecBackupData,
                    ReqData - In_p->RecBackupData, R15_Network_ReadCallback,
                    Communication_p->CommunicationDevice_p);

#endif
            } else {
                /* Copy content of backup buffer into receive buffer */
                memcpy(In_p->Target_p + In_p->ReqBuffOffset, Communication_p->BackupCommBuffer_p, In_p->ReqData);
                /* Move rest of backup data at the beginning of the backup buffer */
                memcpy(Communication_p->BackupCommBuffer_p, Communication_p->BackupCommBuffer_p + In_p->ReqData, Communication_p->BackupCommBufferSize - In_p->ReqData);
                /* Update the size of the backup buffer */
                Communication_p->BackupCommBufferSize = Communication_p->BackupCommBufferSize - In_p->ReqData;

                ReqData = In_p->ReqData;
                In_p->ReqData = 0;
                ReqBufferOffset = In_p->ReqBuffOffset;
                In_p->ReqBuffOffset = 0;
                In_p->RecData = 0;
                R15_Network_ReadCallback(In_p->Target_p + ReqBufferOffset, ReqData, Communication_p->CommunicationDevice_p);
            }
        } else {
            ReqData = In_p->ReqData;
            In_p->ReqData = 0;
            ReqBufferOffset = In_p->ReqBuffOffset;
            In_p->ReqBuffOffset = 0;
            In_p->RecData = 0;

            C_(printf("r15_network_layer.c (%d) ReqData(%d) RecData(%d) \n", __LINE__, ReqData, In_p->RecData);)
            C_(printf("r15_network_layer.c (%d) Communication_p->BackupCommBufferSize(%d) RecBackupData (%d)\n", __LINE__, Communication_p->BackupCommBufferSize, In_p->RecBackupData);)

#ifdef CFG_ENABLE_LOADER_TYPE

            if (E_SUCCESS != Communication_p->CommunicationDevice_p->Read(
                In_p->Target_p + ReqBufferOffset, ReqData, R15_Network_ReadCallback,
                Communication_p->CommunicationDevice_p)) {
                /* Read failed! Return to previous state. */
                A_(printf("r15_network_layer.c (%d) R15_Network_ReceiverHandler() Read Failed!\n", __LINE__);)
                In_p->ReqData = ReqData;
                In_p->ReqBuffOffset = ReqBufferOffset;
            }

#else
            (void)Communication_p->CommunicationDevice_p->Read(
                In_p->Target_p + ReqBufferOffset, ReqData, R15_Network_ReadCallback,
                Communication_p->CommunicationDevice_p);
#endif
        }
    }

    /* check for receiver synchronization */
    if (In_p->State == RECEIVE_ERROR) {
        In_p->ReqData = 0;
        In_p->RecData = 0;
        In_p->ReqBuffOffset = 0;
#ifdef CFG_ENABLE_LOADER_TYPE

        A_(printf("r15_network_layer.c (%d) R15_Network_ReceiverHandler() Receive Error ! \n\n", __LINE__);)

        if (E_SUCCESS == Communication_p->CommunicationDevice_p->Read(In_p->Target_p,
        ALIGNED_HEADER_LENGTH, R15_Network_ReadCallback,
        Communication_p->CommunicationDevice_p)) {
            In_p->State = RECEIVE_HEADER;
        }

#else
        (void)Communication_p->CommunicationDevice_p->Read(In_p->Target_p,
        ALIGNED_HEADER_LENGTH, R15_Network_ReadCallback,
        Communication_p->CommunicationDevice_p);
        In_p->State = RECEIVE_HEADER;
#endif
    }

    return R15_NETWORK(Communication_p)->Inbound.LCM_Error;
}

ErrorCode_e R15_Network_TransmiterHandler(Communication_t *Communication_p)
{
    ErrorCode_e ReturnValue = E_SUCCESS;
    R15_Outbound_t *Out_p = &(R15_NETWORK(Communication_p)->Outbound);
    uint8  *ExHeaderStartInBuffer_p = NULL;
#ifdef CFG_ENABLE_LOADER_TYPE
    uint8  *HeaderStartInBuffer_p = NULL;
    boolean IsBufferContinuous = FALSE;
    static R15_OutboundState_t SavedState = SEND_IDLE;
    static boolean ReRun = FALSE;
#endif // CFG_ENABLE_LOADER_TYPE
    uint32  ContinuousBufferLength = 0;
    boolean RegisterRetransmission = FALSE;
    uint32  ExtHdrLen = 0;
    uint32  Aligned_Length = 0;

#ifdef CFG_ENABLE_LOADER_TYPE
StartTramsmitter:
#endif // CFG_ENABLE_LOADER_TYPE

    if (!Do_CriticalSection_Enter(Out_p->TxCriticalSection)) {
#ifdef CFG_ENABLE_LOADER_TYPE
        ReRun = TRUE;
        SavedState = Out_p->State;
        A_(printf("r15_network_layer.c (%d) Failed to lock CriticalSection code! \n", __LINE__);)
        return E_CS_LOCK_FAILED;
#else
        return ReturnValue;
#endif // CFG_ENABLE_LOADER_TYPE
    }

    switch (Out_p->State) {
    case SEND_IDLE:
        /* check retransmission count before send */
        Out_p->Packet_p = (PacketMeta_t *)QUEUE(Communication_p, FifoDequeue_Fn)(OBJECT_QUEUE(Communication_p), Communication_p->Outbound_p);

        if ((NULL == Out_p->Packet_p) || (CHECK_PACKET_FLAGS(Out_p->Packet_p, BUF_FREE)) || (CHECK_PACKET_FLAGS(Out_p->Packet_p, BUF_TX_DONE))) {
            break;
        } else if (Out_p->Packet_p->Resend < MAX_RESENDS) {
            Out_p->Packet_p->Resend++;
            /* get next packet for transmitting */
            Out_p->State = SEND_HEADER;
        } else {
            //Do_CommunicationInternalErrorHandler(E_RETRANSMITION_FAILED);
            A_(printf("r15_network_layer.c (%d) Packet Retransmission Failed! Polling will stop!\n", __LINE__);)
            ReturnValue = E_RETRANSMITION_FAILED;
            break;
        }

        /* FALLTHROUGH */
    case SEND_HEADER:
#ifdef CFG_ENABLE_LOADER_TYPE
        HeaderStartInBuffer_p = Out_p->Packet_p->Buffer_p;

        if (Out_p->Packet_p->Header.ExtendedHeaderLength == COMMAND_EXTENDED_HEADER_LENGTH) {
            ExtHdrLen = ALIGNED_COMMAND_EXTENDED_HEADER_LENGTH;
        } else {
            ExtHdrLen = ALIGNED_BULK_EXTENDED_HEADER_LENGTH;
        }

        if ((HeaderStartInBuffer_p + ALIGNED_HEADER_LENGTH + ExtHdrLen) == Out_p->Packet_p->Payload_p) {
            /* end of the header and beginning of the payload are same */
            IsBufferContinuous = TRUE;
        }

        /* set next state before calling the communication device, to avoid race condition
           where write callback is called before new state is set */
        if (IsBufferContinuous || Out_p->Packet_p->Header.PayloadLength == 0) {
            /* we have a packet without payload or header and payload are contained in one
               continuous buffer so it can be sent with one write request */
            ContinuousBufferLength = ALIGNED_HEADER_LENGTH + ExtHdrLen + Out_p->Packet_p->Header.PayloadLength;
            ContinuousBufferLength = (ContinuousBufferLength + (ALIGN_SIZE - 1)) & (~(ALIGN_SIZE - 1));

            Out_p->State = SENDING_PAYLOAD;
            RegisterRetransmission = TRUE;
        } else {
            ContinuousBufferLength = ALIGNED_HEADER_LENGTH + ExtHdrLen;
            Out_p->State = SENDING_HEADER;
        }

#else
        ContinuousBufferLength = ALIGNED_HEADER_LENGTH;
        Out_p->State = SENDING_HEADER;

#endif

        if (E_SUCCESS == Communication_p->CommunicationDevice_p->Write((Out_p->Packet_p->Buffer_p),
        ContinuousBufferLength, R15_Network_WriteCallback, Communication_p->CommunicationDevice_p)) {
            C_(printf("r15_network_layer.c (%d) Header Sent to comm device! \n", __LINE__);)
        } else {
            Out_p->State = SEND_HEADER;
            RegisterRetransmission = FALSE;
            C_(printf("r15_network_layer.c (%d) Error sending header to comm device! \n", __LINE__);)
        }

        break;

    case SENDING_HEADER:
        /* nothing to do, wait until sending is finished and state changed by write callback */
        break;

    case SEND_EX_HEADER:
        ExHeaderStartInBuffer_p = Out_p->Packet_p->Buffer_p + ALIGNED_HEADER_LENGTH;

        if (Out_p->Packet_p->Header.ExtendedHeaderLength == COMMAND_EXTENDED_HEADER_LENGTH) {
            ExtHdrLen = ALIGNED_COMMAND_EXTENDED_HEADER_LENGTH;
        } else {
            ExtHdrLen = ALIGNED_BULK_EXTENDED_HEADER_LENGTH;
        }

        if (Out_p->Packet_p->Header.PayloadLength != 0) {
            Out_p->State = SENDING_EX_HEADER;
        } else {
            /* if there is no payload, just go directly to SENDING_PAYLOAD state */
            Out_p->State = SENDING_PAYLOAD;
            RegisterRetransmission = TRUE;
        }

        if (E_SUCCESS == Communication_p->CommunicationDevice_p->Write(ExHeaderStartInBuffer_p, ExtHdrLen,
        R15_Network_WriteCallback, Communication_p->CommunicationDevice_p)) {
            C_(printf("r15_network_layer.c (%d) ExHeader Sent to comm device! \n", __LINE__);)
        } else {
            Out_p->State = SEND_EX_HEADER;
            RegisterRetransmission = FALSE;
            C_(printf("r15_network_layer.c (%d) Error sending ex_header to comm device! \n", __LINE__);)
        }

        break;

    case SENDING_EX_HEADER:
        /* nothing to do, wait until sending is finished and state changed by write callback */
        break;

    case SEND_PAYLOAD:
        Out_p->State = SENDING_PAYLOAD;

        Aligned_Length = (Out_p->Packet_p->Header.PayloadLength + (ALIGN_SIZE - 1)) & (~(ALIGN_SIZE - 1));

        if (E_SUCCESS == Communication_p->CommunicationDevice_p->Write(Out_p->Packet_p->Payload_p,
        Aligned_Length,
        R15_Network_WriteCallback, Communication_p->CommunicationDevice_p)) {
            RegisterRetransmission = TRUE;
            C_(printf("r15_network_layer.c (%d) Payload Sent to comm device! \n", __LINE__);)
        } else {
            Out_p->State = SEND_PAYLOAD;
            C_(printf("r15_network_layer.c (%d) Error sending payload to comm device! \n", __LINE__);)
        }

        break;

    case SENDING_PAYLOAD:
        /* nothing to do, wait until sending is finished and state changed by write callback */
        break;
    }

    if (RegisterRetransmission) {
        if (0 != Out_p->Packet_p->Timer.Time) {
            C_(printf("r15_network_layer.c (%d) Register retransmission\n", __LINE__);)
            (void)R15_Network_RegisterRetransmission(Communication_p, Out_p->Packet_p);
        }
    }

    Do_CriticalSection_Leave(Out_p->TxCriticalSection);

#ifdef CFG_ENABLE_LOADER_TYPE

    if (TRUE == ReRun) {
        ReRun = FALSE;
        Out_p->State = SavedState;

        ExHeaderStartInBuffer_p = NULL;
        HeaderStartInBuffer_p = NULL;
        IsBufferContinuous = FALSE;
        ContinuousBufferLength = 0;
        RegisterRetransmission = FALSE;
        ExtHdrLen = 0;
        Aligned_Length = 0;

        goto StartTramsmitter;
    }

#endif // CFG_ENABLE_LOADER_TYPE

    return ReturnValue;
}

/*
 * Cancel retransmission of packets.
 *
 * @param [in,out] Communication_p Communication module context.
 * @param [in]     UniqueKey       Unique key used for identification of packet
 *                                 for retransmission.
 *
 * @return none.
 */
ErrorCode_e R15_Network_CancelRetransmission(const Communication_t *const Communication_p, uint32 UniqueKey)
{
    ErrorCode_e ReturnValue = E_NOT_FOUND_ELEMENT_IN_RETRANSMISSION_LIST;
    uint32 Index = 0;

    do {
        if ((TRUE == R15_NETWORK(Communication_p)->RetransmissionList[Index].InUse) && (R15_NETWORK(Communication_p)->RetransmissionList[Index].Key == UniqueKey)) {
            R15_NETWORK(Communication_p)->RetransmissionList[Index].InUse = FALSE;

            (void)TIMER(Communication_p, TimerRelease_Fn)(OBJECT_TIMER(Communication_p), R15_NETWORK(Communication_p)->RetransmissionList[Index].TimerKey);

            memset(&(R15_NETWORK(Communication_p)->RetransmissionList[Index].Packet_p->Timer), 0, sizeof(Timer_t));

            ReturnValue = R15_Network_PacketRelease(Communication_p, R15_NETWORK(Communication_p)->RetransmissionList[Index].Packet_p);

            if (E_SUCCESS != ReturnValue) {
                A_(printf("r15_network_layer.c(%d): Packet release failed!\n", __LINE__);)
                return ReturnValue;
            }

            R15_NETWORK(Communication_p)->RetransmissionListCount--;

            for (; Index < R15_NETWORK(Communication_p)->RetransmissionListCount; Index++) {
                R15_NETWORK(Communication_p)->RetransmissionList[Index] = R15_NETWORK(Communication_p)->RetransmissionList[Index + 1];
            }

            ReturnValue = E_SUCCESS;
            break;
        }

        Index++;
    } while (Index < R15_NETWORK(Communication_p)->RetransmissionListCount);

    return ReturnValue;
}

/*
 * Create unique key.
 *
 * This function combine the session number and protocol type in one
 * unique key for command packet. For the bulk packet the unique key is the
 * combination of protocol type, session number and command.
 *
 * @param [in] Packet_p    Pointer to the packet.
 * @param [in] ExternalKey External key for marking the packet for retransmission.
 *
 * @return Unique key.
 */
uint32 R15_Network_CreateUniqueKey(const PacketMeta_t *const Packet_p, const uint8 ExternalKey)
{
    uint32 Key = Packet_p->Header.Protocol;

    if (Key == PROTO_COMMAND) {
        return((Key << 16) | (*(Packet_p->ExtendedHeader_p) & MASK_CLR_STATE));
    } else {
        return ((Key << 16) | ((*(Packet_p->ExtendedHeader_p) & 0x00FF) << 8) | ExternalKey);
    }
}


/*
 * Network packet allocation in R15 protocol family.
 *
 * @param [in,out] Communication_p Communication module context.
 * @param [in]     BufferSize      Size of buffer used for network packet.
 *
 * @retval  Pointer Pointer to allocated packet meta info.
 * @retval  NULL    if allocation fail.
 */
PacketMeta_t *R15_Network_PacketAllocate(const Communication_t *const Communication_p, int BufferSize)
{
    PacketMeta_t *Meta_p = NULL;
    void *Buffer_p = NULL;
    int  BuffersNr = 0;

    /* Find the first unallocated buffers. */
    Buffer_p = BUFFER(Communication_p, BufferAllocate_Fn)(OBJECT_BUFFER(Communication_p), BufferSize);

    if (NULL == Buffer_p) {
        A_(printf("r15_network_layer.c (%d): ** Buffer allocation fail! **\n", __LINE__);)
        goto ErrorExit;
    }

    B_(printf("r15_network_layer.c (%d): Buffer allocate (0x%x)! **\n", __LINE__, (uint32)Buffer_p);)

    /* packet meta info allocate, get free packet meta structure */

    Meta_p = R15_Network_GetAvailableMetaPacket();

    if (NULL == Meta_p) {
        R15_NETWORK(Communication_p)->Inbound.LCM_Error = E_ALLOCATE_FAILED;
        goto ErrorExit;
    }

    /* packet meta info setup */
    C_(

        if (BULK_BUFFER_SIZE > BufferSize)
        printf("CmdBuffGet:%x\n", Buffer_p);
    else {
        printf("ChunkBuffGet:%x\n", Buffer_p);
        })

    Meta_p->Buffer_p = (uint8 *)Buffer_p;
    Meta_p->BufferSize = BufferSize;
    SET_PACKET_FLAGS(Meta_p, PACKET_ALLOCATION_STATE_MASK, BUF_ALLOCATED);

    do {
        if (NULL == R15_NETWORK(Communication_p)->MetaInfoList[BuffersNr]) {
            R15_NETWORK(Communication_p)->MetaInfoList[BuffersNr] = Meta_p;
            break;
        }

        BuffersNr ++;
    } while (BuffersNr < (COMMAND_BUFFER_COUNT + BULK_BUFFER_COUNT));

ErrorExit:
    return Meta_p;
}


/*
 * Network packet release in R15 protocol family.
 *
 * @param [in,out] Communication_p Communication module context.
 * @param [in]     Meta_p          Meta info for used network packet.
 *
 * @retval  E_SUCCESS                   Successful network packet release.
 * @retval  E_INVALID_INPUT_PARAMETERS  Invalid input parameter.
 */
ErrorCode_e R15_Network_PacketRelease(const Communication_t *const Communication_p, PacketMeta_t *Meta_p)
{
    ErrorCode_e ReturnValue = E_SUCCESS;
    int  BuffersNr = 0;

    VERIFY(NULL != Meta_p, E_INVALID_INPUT_PARAMETERS);

    /* remove the meta info data from list */
    do {
        if (Meta_p == R15_NETWORK(Communication_p)->MetaInfoList[BuffersNr]) {
            R15_NETWORK(Communication_p)->MetaInfoList[BuffersNr] = NULL;
            /* release buffer */
            B_(printf("r15_network_layer.c (%d): Buffer release (0x%x)! **\n", __LINE__, (uint32)(Meta_p->Buffer_p));)
#ifndef  CFG_ENABLE_LOADER_TYPE
            ReturnValue = BUFFER(Communication_p, BufferRelease_Fn)(OBJECT_BUFFER(Communication_p), Meta_p->Buffer_p, Meta_p->BufferSize);
            VERIFY_CONDITION(E_SUCCESS == ReturnValue);
            B_(printf("r15_network_layer.c (%d): Buffer released! **\n", __LINE__);)
#else
            C_(

                if (BULK_BUFFER_SIZE > Meta_p->BufferSize)
                printf("CmdBuffRel:%x\n", Meta_p->Buffer_p);
            else {
                printf("ChunkBuffRel:%x\n", Meta_p->Buffer_p);
                })
            ReturnValue = BUFFER(Communication_p, BufferRelease_Fn)(OBJECT_BUFFER(Communication_p), Meta_p->Buffer_p, Meta_p->BufferSize);
            VERIFY_CONDITION(E_SUCCESS == ReturnValue);
            B_(printf("r15_network_layer.c (%d): Buffer released! **\n", __LINE__);)
#endif

            memset(Meta_p, 0, sizeof(PacketMeta_t));
            Meta_p = NULL;
            break;
        }

        BuffersNr ++;
    } while (BuffersNr < (COMMAND_BUFFER_COUNT + BULK_BUFFER_COUNT));

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
void R15_Network_WriteCallback(const void *Data_p, const uint32 Length, void *Param_p)
{
    Communication_t *Communication_p = (Communication_t *)(((CommunicationDevice_t *)Param_p)->Object_p);
    R15_Outbound_t *Out_p = &(R15_NETWORK(Communication_p)->Outbound);
    BulkExtendedHeader_t ExtendedHeader = {0};
#ifdef CFG_ENABLE_LOADER_TYPE
    ErrorCode_e ReturnValue = E_GENERAL_COMMUNICATION_ERROR;
    R15_OutboundState_t SavedState = Out_p->State;
#endif // CFG_ENABLE_LOADER_TYPE
    B_(printf("r15_network_layer.c (%d): Device write finished!! \n", __LINE__);)

    if (SENDING_HEADER == Out_p->State) {
        Out_p->State = SEND_EX_HEADER;
    } else if (SENDING_EX_HEADER == Out_p->State) {
        Out_p->State = SEND_PAYLOAD;
    } else if (SENDING_PAYLOAD == Out_p->State) {
        if (0 == Out_p->Packet_p->Timer.Time) {
            if (Out_p->Packet_p->Header.Protocol == BULK_PROTOCOL) {
                R15_DeserializeBulkExtendedHeader(&ExtendedHeader, Out_p->Packet_p->ExtendedHeader_p);
            }

            if (!((Out_p->Packet_p->Header.Protocol == BULK_PROTOCOL) && ((ExtendedHeader.TypeFlags & MASK_BULK_COMMAND_SELECT) == CMD_BULK_DATA))) {
                (void)R15_Network_PacketRelease(Communication_p, Out_p->Packet_p);
            }
        }

        Out_p->State = SEND_IDLE;
        SET_PACKET_FLAGS(Out_p->Packet_p, PACKET_TX_STATE_MASK, BUF_TX_SENT);
    }

#ifdef CFG_ENABLE_LOADER_TYPE

    ReturnValue = R15_Network_TransmiterHandler(Communication_p);

    if (E_CS_LOCK_FAILED == ReturnValue) {
        Out_p->State = SavedState;
    } else if (E_SUCCESS != ReturnValue) {
        R15_NETWORK(Communication_p)->Outbound.LCM_Error = E_GENERAL_COMMUNICATION_ERROR;
    }

#endif
}


/*******************************************************************************
 * Definition of internal functions
 ******************************************************************************/

static ErrorCode_e R15_Network_ReceiveHeader(const Communication_t *const Communication_p)
{
    R15_Inbound_t *In_p = &(R15_NETWORK(Communication_p)->Inbound);
    A_(static uint8 print_header = 1;)

    if (In_p->RecData == 0) {
        A_(printf("r15_network_layer.c (%d) Synchronize for Receiving Header!\n", __LINE__);)
        In_p->ReqData = ALIGNED_HEADER_LENGTH;
        In_p->Target_p = In_p->Scratch;
        In_p->ReqBuffOffset = 0;
    } else {
        if (R15_IsReceivedHeader(In_p)) {
            if (R15_IsValidHeader(In_p->Scratch)) {
                A_(print_header = 1;)

                R15_DeserializeHeader(&In_p->Header, In_p->Scratch);
                In_p->Target_p += ALIGNED_HEADER_LENGTH;

                if (In_p->Header.ExtendedHeaderLength == COMMAND_EXTENDED_HEADER_LENGTH) {
                    SET_INBOUND(In_p, RECEIVE_EXTENDED_HEADER, ALIGNED_COMMAND_EXTENDED_HEADER_LENGTH);
                } else {
                    SET_INBOUND(In_p, RECEIVE_EXTENDED_HEADER, ALIGNED_BULK_EXTENDED_HEADER_LENGTH);
                }
            } else {
                A_(printf("Not valid header!\n");)
                /* Sync the header. */
                RESET_INBOUND(In_p, RECEIVE_HEADER);
                SYNC_HEADER(In_p, ALIGNED_HEADER_LENGTH, In_p->Scratch);
            }
        } else {
            A_(

            if (print_header) {
            uint32 Counter = 0;
            print_header = 0;
            printf("Invalid header! \n");

                for (Counter = 0; Counter < 16; Counter++) {
                    printf(" %02X", In_p->Scratch[Counter]);
                }

                printf("\n\n");
            }
            )
        }
    }

    return E_SUCCESS;
}

static ErrorCode_e R15_Network_ReceiveExtendedHeader(Communication_t *Communication_p)
{
    R15_Inbound_t *In_p = &(R15_NETWORK(Communication_p)->Inbound);
    ErrorCode_e ReturnValue = E_SUCCESS;
    R15_Header_t R15Header = {0};
    BulkExtendedHeader_t BulkExtendedHeader = {0};
    boolean IsValidHeader = FALSE;

    VERIFY(NULL != In_p, E_GENERAL_FATAL_ERROR);

    R15_DeserializeHeader(&R15Header, In_p->Scratch);
    R15_DeserializeBulkExtendedHeader(&BulkExtendedHeader, In_p->Target_p);

    IsValidHeader = R15_IsValidExtendedHeader(In_p->Target_p,
    In_p->Header.ExtendedHeaderLength,
    In_p->Header.ExtendedHeaderChecksum);

    if (IsValidHeader) {
        if (R15Header.Protocol == BULK_PROTOCOL &&
        (BulkExtendedHeader.TypeFlags & MASK_BULK_COMMAND_SELECT) == CMD_BULK_DATA) {
            In_p->Packet_p = R15_Network_PacketAllocate(Communication_p, BULK_BUFFER_SIZE);
        } else {
            In_p->Packet_p = R15_Network_PacketAllocate(Communication_p, COMMAND_BUFFER_SIZE);
        }

        VERIFY(NULL != In_p->Packet_p, E_FAILED_TO_FIND_COMM_BUFFER);

        In_p->Packet_p->Header = R15Header;
        In_p->Packet_p->ExtendedHeader_p = In_p->Packet_p->Buffer_p + ALIGNED_HEADER_LENGTH;
        SET_PACKET_FLAGS(In_p->Packet_p, PACKET_RX_STATE_MASK, BUF_HDR_CRC_OK);

        if (In_p->Packet_p->Header.ExtendedHeaderLength == COMMAND_EXTENDED_HEADER_LENGTH) {
            In_p->Packet_p->Payload_p = In_p->Packet_p->ExtendedHeader_p + ALIGNED_COMMAND_EXTENDED_HEADER_LENGTH;
        } else {
            In_p->Packet_p->Payload_p = In_p->Packet_p->ExtendedHeader_p + ALIGNED_BULK_EXTENDED_HEADER_LENGTH;
        }

        memcpy(In_p->Packet_p->Buffer_p, &In_p->Packet_p->Header, HEADER_LENGTH);
        memcpy(In_p->Packet_p->ExtendedHeader_p, In_p->Target_p, In_p->Header.ExtendedHeaderLength);

        In_p->Target_p = In_p->Packet_p->Payload_p;

        /* check for expected payload */
        if (In_p->Packet_p->Header.PayloadLength != 0) {
            uint32 Aligned_Size = 0;
            Aligned_Size = (In_p->Packet_p->Header.PayloadLength + (ALIGN_SIZE - 1)) & (~(ALIGN_SIZE - 1));

            SET_INBOUND(In_p, RECEIVE_PAYLOAD, Aligned_Size);
        } else {
#ifdef SKIP_PAYLOAD_VERIFICATION
            SET_PACKET_FLAGS(In_p->Packet_p, PACKET_RX_STATE_MASK, BUF_RX_READY);
            (void)QUEUE(Communication_p, FifoEnqueue_Fn)(OBJECT_QUEUE(Communication_p), Communication_p->Inbound_p, In_p->Packet_p);
#else
            PacketMeta_t *Packet_p = In_p->Packet_p;
            Packet_p->Communication_p = Communication_p;
            SET_PACKET_FLAGS(In_p->Packet_p, PACKET_RX_STATE_MASK, BUF_RX_READY);
            // Copy the original packet checksum to avoid hash mismatch
            memcpy(Packet_p->Hash, &Packet_p->Header.PayloadChecksum, 4);

            Communication_p->HashDevice_p->Calculate(OBJECT_HASH(Communication_p),
            HASH_NONE,
            Packet_p->Payload_p, Packet_p->Header.PayloadLength,
            Packet_p->Hash, (HashCallback_t)R15_InHashCallback,
            (void *)Packet_p);
#endif

            In_p->Packet_p = NULL;
            RESET_INBOUND(In_p, RECEIVE_HEADER);
            SYNC_HEADER(In_p, ALIGNED_HEADER_LENGTH, In_p->Scratch);
            C_(printf("r15_network_layer.c (%d) ReqData(%d) RecData(%d) \n", __LINE__, In_p->ReqData, In_p->RecData);)
        }
    } else {
        A_(
            uint32 Counter = 0;
            printf("Invalid extended header! \n");

        for (Counter = 0; Counter < 16; Counter++) {
        printf(" %02X", In_p->Target_p[Counter]);
        }
        printf("\n\n");
        )
        RESET_INBOUND(In_p, RECEIVE_HEADER);
        SYNC_HEADER(In_p, ALIGNED_HEADER_LENGTH, In_p->Scratch);
    }

ErrorExit:
    return ReturnValue;
}

static ErrorCode_e R15_Network_ReceivePayload(Communication_t *Communication_p)
{
    R15_Inbound_t *In_p = &(R15_NETWORK(Communication_p)->Inbound);
    PacketMeta_t *Packet_p = In_p->Packet_p;

    Packet_p->Communication_p = Communication_p;
    SET_PACKET_FLAGS(In_p->Packet_p, PACKET_RX_STATE_MASK, BUF_RX_READY);

#ifndef CFG_ENABLE_LOADER_TYPE
    Communication_p->CurrentFamilyHash = (HashType_e)Packet_p->Header.Flags;
#endif

    if (HASH_NONE != Packet_p->Header.Flags) {
#ifdef  CFG_ENABLE_MEASUREMENT_TOOL

        if (In_p->Packet_p->Header.Protocol == BULK_PROTOCOL) {
            BulkExtendedHeader_t ExtendedHeader = {0};
            R15_DeserializeBulkExtendedHeader(&ExtendedHeader, Packet_p->ExtendedHeader_p);

            if ((ExtendedHeader.TypeFlags & MASK_BULK_COMMAND_SELECT) == CMD_BULK_DATA) {
                (void)MP(Measurement_p, ExtendedHeader.AcksChunk, START_HASHINGCHUNK_TIME);
            }
        }

#endif
        Communication_p->HashDevice_p->Calculate(OBJECT_HASH(Communication_p),
        Communication_p->CurrentFamilyHash,
        Packet_p->Payload_p, Packet_p->Header.PayloadLength,
        Packet_p->Hash, (HashCallback_t)R15_InHashCallback,
        (void *)Packet_p);
    } else {
        SET_PACKET_FLAGS(Packet_p, PACKET_RX_STATE_MASK, BUF_PAYLOAD_CRC_OK);
        (void)QUEUE((Packet_p->Communication_p), FifoEnqueue_Fn)(OBJECT_QUEUE(Packet_p->Communication_p), Packet_p->Communication_p->Inbound_p, Packet_p);
    }

    In_p->Packet_p = NULL;
    RESET_INBOUND(In_p, RECEIVE_HEADER);
    SYNC_HEADER(In_p, ALIGNED_HEADER_LENGTH, In_p->Scratch);
    return E_SUCCESS;
}

static ErrorCode_e R15_Network_RegisterRetransmission(Communication_t *Communication_p, PacketMeta_t *Packet_p)
{
    int i = 0;
    int Index = 0;
    ErrorCode_e ReturnValue = E_RETRANSMISSION_LIST_FULL;

    if (R15_NETWORK(Communication_p)->RetransmissionListCount < MAX_SIZE_RETRANSMISSION_LIST) {
        if (NULL == Packet_p->Timer.HandleFunction_p) {
            Packet_p->Timer.HandleFunction_p = (HandleFunction_t)R15_Network_RetransmissionCallback;
            Packet_p->Timer.Param_p = Communication_p;
        }

        do {
            if (TRUE == R15_NETWORK(Communication_p)->RetransmissionList[Index].InUse) {
                if (Packet_p->Timer.Time < TIMER(Communication_p, ReadTime_Fn)(OBJECT_TIMER(Communication_p), R15_NETWORK(Communication_p)->RetransmissionList[Index].TimerKey)) {
                    i = R15_NETWORK(Communication_p)->RetransmissionListCount;

                    for (; Index < i; i--) {
                        R15_NETWORK(Communication_p)->RetransmissionList[i] = R15_NETWORK(Communication_p)->RetransmissionList[i - 1];
                    }

                    break;
                }

                Index++;
            } else {
                break;
            }
        } while (Index < MAX_SIZE_RETRANSMISSION_LIST);

        if (Index != MAX_SIZE_RETRANSMISSION_LIST) {
            R15_NETWORK(Communication_p)->RetransmissionList[Index].TimerKey = TIMER(Communication_p, TimerGet_Fn)(OBJECT_TIMER(Communication_p), &(Packet_p->Timer));
            R15_NETWORK(Communication_p)->RetransmissionList[Index].Timeout = Packet_p->Timer.Time;
            R15_NETWORK(Communication_p)->RetransmissionList[Index].Packet_p = Packet_p;
            R15_NETWORK(Communication_p)->RetransmissionList[Index].Key = R15_Network_CreateUniqueKey(Packet_p, (uint8)(*(Packet_p->ExtendedHeader_p + sizeof(uint16))));
            R15_NETWORK(Communication_p)->RetransmissionList[Index].InUse = TRUE;
            R15_NETWORK(Communication_p)->RetransmissionListCount++;
            C_(printf("r15_network_layer.c (%d) Key(%d) TKey(%d) \n", __LINE__, (R15_NETWORK(Communication_p)->RetransmissionList[Index].Key), (R15_NETWORK(Communication_p)->RetransmissionList[Index].TimerKey));)
        }

        ReturnValue = E_SUCCESS;

    } else {
        A_(printf("r15_network_layer.c (%d) ** Err: Retransmission List is full! ** \n", __LINE__);)
    }

    return ReturnValue;
}


static void R15_Network_RetransmissionCallback(Communication_t *Communication_p, const void *const Timer_p, void *Packet_p)
{
    uint32 Index = 0;

    /* get first in list of packets for retransmission and remove */
    if (TRUE == R15_NETWORK(Communication_p)->RetransmissionList[Index].InUse) {
        R15_NETWORK(Communication_p)->RetransmissionList[Index].InUse = FALSE;
    }

    if (R15_NETWORK(Communication_p)->RetransmissionListCount > 0) {
        C_(printf("r15_network_layer.c (%d) RetransmissionListCount(%d) \n", __LINE__, R15_NETWORK(Communication_p)->RetransmissionListCount);)
        R15_NETWORK(Communication_p)->RetransmissionListCount--;

        for (; Index < R15_NETWORK(Communication_p)->RetransmissionListCount; Index++) {
            R15_NETWORK(Communication_p)->RetransmissionList[Index] = R15_NETWORK(Communication_p)->RetransmissionList[Index + 1];
        }

        R15_NETWORK(Communication_p)->RetransmissionList[Index].InUse = FALSE;

        /* enqueue the packet for retransmission */
        (void)QUEUE(Communication_p, FifoEnqueue_Fn)(OBJECT_QUEUE(Communication_p), Communication_p->Outbound_p, Packet_p);
    }
}


static void R15_InHashCallback(const void *const Data_p, uint32 Length, const uint8 *const Hash_p, void *Param_p)
{
    PacketMeta_t *Packet_p = (PacketMeta_t *)Param_p;

#ifdef  CFG_ENABLE_MEASUREMENT_TOOL

    if (Packet_p->Header.Protocol == BULK_PROTOCOL) {
        BulkExtendedHeader_t ExtendedHeader = {0};
        R15_DeserializeBulkExtendedHeader(&ExtendedHeader, Packet_p->ExtendedHeader_p);

        if ((ExtendedHeader.TypeFlags & MASK_BULK_COMMAND_SELECT) == CMD_BULK_DATA) {
            (void)MP(Measurement_p, ExtendedHeader.AcksChunk, END_HASHINGCHUNK_TIME);
        }
    }

#endif

    if (memcmp(Hash_p, &Packet_p->Header.PayloadChecksum, 4) == 0) {
        SET_PACKET_FLAGS(Packet_p, PACKET_RX_STATE_MASK, BUF_PAYLOAD_CRC_OK);
        (void)QUEUE((Packet_p->Communication_p), FifoEnqueue_Fn)(OBJECT_QUEUE(Packet_p->Communication_p), Packet_p->Communication_p->Inbound_p, Packet_p);
    } else {
        /* Invalid packet */
        ErrorCode_e ReturnValue = E_GENERAL_FATAL_ERROR;

        if (E_SUCCESS != (ReturnValue = R15_Network_PacketRelease((Communication_t *)Packet_p->Communication_p, Packet_p))) {
            R15_NETWORK((Communication_t *)Packet_p->Communication_p)->Inbound.LCM_Error = ReturnValue;
        }
    }
}

#ifdef  CFG_ENABLE_LOADER_TYPE
static void R15_QueueOutCallback(const void *const Queue_p, void *Param_p)
{
    if (E_SUCCESS != R15_Network_TransmiterHandler((Communication_t *)Param_p)) {
        R15_NETWORK((Communication_t *)Param_p)->Outbound.LCM_Error = E_GENERAL_COMMUNICATION_ERROR;
    }
}

static void R15_QueueInCallback(const void *const Queue_p, void *Param_p)
{
    ErrorCode_e ReturnValue = E_GENERAL_FATAL_ERROR;
    Communication_t *Communication_p = (Communication_t *)Param_p;
    PacketMeta_t *Packet_p = NULL;

    while ((Packet_p = (PacketMeta_t *)QUEUE(Communication_p, FifoDequeue_Fn)(OBJECT_QUEUE(Communication_p), Communication_p->Inbound_p)) != NULL) {
        switch (Packet_p->Header.Protocol) {
        case PROTO_BULK:
            ReturnValue = R15_Bulk_Process(Communication_p, Packet_p);

            if (E_SUCCESS != ReturnValue) {
                R15_NETWORK(Communication_p)->Inbound.LCM_Error = ReturnValue;
            }

            break;

        case PROTO_COMMAND:
            ReturnValue = R15_Command_Process(Communication_p, Packet_p);

            if (E_SUCCESS != ReturnValue) {
                R15_NETWORK(Communication_p)->Inbound.LCM_Error = ReturnValue;
            }

            break;

        default:
            R15_NETWORK(Communication_p)->Inbound.LCM_Error = E_SUCCESS;
            break;
        }
    }
}
#endif

static PacketMeta_t *R15_Network_GetAvailableMetaPacket(void)
{
    uint8 i = 0;
    PacketMeta_t *PacketMeta_p = NULL;

    do {
        if (NULL == PacketMetaInfo[i].Buffer_p) {
            PacketMeta_p = &PacketMetaInfo[i];
            break;
        }

        i++;
    } while (i < COMMAND_BUFFER_COUNT + BULK_BUFFER_COUNT);

    return PacketMeta_p;
}

/** @} */
/** @} */
/** @} */
