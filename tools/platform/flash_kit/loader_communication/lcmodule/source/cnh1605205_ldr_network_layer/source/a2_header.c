/*******************************************************************************
 * Copyright (C) ST-Ericsson SA 2011
 * License terms: 3-clause BSD license
 ******************************************************************************/
/**
 *  @addtogroup ldr_communication_serv
 *  @{
 *    @addtogroup a2_family
 *    @{
 *      @addtogroup ldr_header
 *      @{
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "c_system_v2.h"
#include <string.h>
#include "t_basicdefinitions.h"
#include "r_a2_header.h"
#include "r_serialization.h"

/*******************************************************************************
 * Declaration of file local functions
 ******************************************************************************/
static uint8 A2_FindHeaderPattern(const uint8 *const HeaderData_p, uint32 *StartInBuffer_p);


/*******************************************************************************
 * Definition of internal functions
 ******************************************************************************/

/*
 * Deserialize the header stored in network format to a structure.
 *
 * @param [out] Header_p  Pointer to the header structure where the
 *                        header data should be placed.
 * @param [in]  Data_p    Pointer to the buffer with received header.
 *
 * @return none.
 */
void A2_DeserializeHeader(A2_Header_t *Header_p, void *Data_p)
{
    void *Iter_p = Data_p;

    Header_p->HeaderPattern = get_uint8(&Iter_p);
    Header_p->Protocol = get_uint8(&Iter_p);

    if (PROTO_A2_ACK == Header_p->Protocol) {
        Header_p->SourceAddress = get_uint8(&Iter_p);
        Header_p->DestinationAddress = get_uint8(&Iter_p);
        Header_p->SequenceNumber = get_uint8(&Iter_p);
        Header_p->Reserved[0] = get_uint8(&Iter_p);
        Header_p->Reserved[1] = get_uint8(&Iter_p);
        Header_p->Reserved[2] = get_uint8(&Iter_p);
        Header_p->DataLength = 0;
    } else {
        Header_p->SourceAddress = get_uint8(&Iter_p);
        Header_p->DestinationAddress = get_uint8(&Iter_p);
        Header_p->SequenceNumber = get_uint8(&Iter_p);
        Header_p->Reserved[0] = get_uint8(&Iter_p);
        Header_p->Reserved[1] = get_uint8(&Iter_p);
        Header_p->Reserved[2] = get_uint8(&Iter_p);
        Header_p->DataLength = get_uint32_le(&Iter_p);
    }
}

/*
 * Serialize the header structure to network format.
 *
 * The data buffer must be of at least size HEADER_LENGTH.
 *
 * @param [out] Data_p    Pointer to the buffer where the
 *                        serialized header should be placed.
 * @param [in]  Header_p  Pointer to the source header structure.
 *
 * @return none.
 */
void A2_SerializeHeader(void *Data_p, const A2_Header_t *Header_p)
{
    void *Iter_p = Data_p;

    if (PROTO_A2_ACK == Header_p->Protocol) {
        put_uint8(&Iter_p, A2_HEADER_PATTERN);
        put_uint8(&Iter_p, PROTO_A2_ACK);
        put_uint8(&Iter_p, Header_p->SourceAddress);
        put_uint8(&Iter_p, Header_p->DestinationAddress);
        put_uint8(&Iter_p, Header_p->SequenceNumber);
        put_uint8(&Iter_p, Header_p->Reserved[0]);
        put_uint8(&Iter_p, Header_p->Reserved[1]);
        put_uint8(&Iter_p, Header_p->Reserved[2]);
        put_uint16_le(&Iter_p, 0);
    } else {
        put_uint8(&Iter_p, A2_HEADER_PATTERN);
        put_uint8(&Iter_p, PROTO_A2);
        put_uint8(&Iter_p, Header_p->SourceAddress);
        put_uint8(&Iter_p, Header_p->DestinationAddress);
        put_uint8(&Iter_p, Header_p->SequenceNumber);
        put_uint8(&Iter_p, Header_p->Reserved[0]);
        put_uint8(&Iter_p, Header_p->Reserved[1]);
        put_uint8(&Iter_p, Header_p->Reserved[2]);
        put_uint32_le(&Iter_p, Header_p->DataLength);
    }
}


/*
 * Get packet length in bytes given the information in Header_p.
 *
 * @param [in] Header_p Pointer to the header structure.
 *
 * @return  The length of the packet in bytes.
 */
uint32 A2_GetPacketLength(const A2_Header_t *Header_p)
{
    return A2_HEADER_LENGTH + Header_p->DataLength;
}


/*
 * Determines whether the first HEADER_LENGTH bytes of Data_p contains a
 * valid header.
 *
 * @param [in] Data_p Pointer to the header candidate.
 *
 * @retval  TRUE  If header is valid.
 * @retval  FALSE If header is not valid.
 */
boolean A2_IsValidHeader(const void *Data_p)
{
    uint8 *Temp_p = (uint8 *)Data_p;

    if (A2_HEADER_PATTERN != *Temp_p) {
        return FALSE;
    }

    if ((PROTO_A2 != *(Temp_p + 1)) && (PROTO_A2_ACK != *(Temp_p + 1)) && (PROTO_CTRL_MSG != *(Temp_p + 1))) {
        return FALSE;
    }

    return TRUE;
}

/*
 * Checks if new header is received.
 *
 * @param [in] In_p Pointer to the structure with receiver information.
 *
 * @retval TRUE   If header is received.
 * @retval FALSE  If header is not received.
 */
boolean A2_IsReceivedHeader(A2_Inbound_t *In_p)
{
    uint32 StartHeaderInBuffer = 0;
    uint8 *TmpPointer_p = NULL;
    uint8 Res = 0;

    Res = A2_FindHeaderPattern(In_p->Scratch, &StartHeaderInBuffer);

    if (A2_HEADER_PATTERN_MATCH == Res) {
        /* Check start point of header in received data */
        if (StartHeaderInBuffer == 0) {
            return TRUE;
        } else {
            In_p->ReqData = StartHeaderInBuffer;
            TmpPointer_p = In_p->Target_p + StartHeaderInBuffer;
            memcpy(In_p->Target_p, TmpPointer_p, A2_HEADER_LENGTH - StartHeaderInBuffer);
            In_p->ReqBuffOffset = A2_HEADER_LENGTH - StartHeaderInBuffer;
        }
    } else {
        /* setup for receiving new packet */
        In_p->State = A2_RECEIVE_ERROR;
    }

    return FALSE;
}

static uint8 A2_FindHeaderPattern(const uint8 *const HeaderData_p, uint32 *StartInBuffer_p)
{
    uint8 Res = NO_A2_HEADER_PATTERN;
    uint32 Offset = 0;

    do {
        Res = NO_A2_HEADER_PATTERN;

        if ((*((uint8 *)HeaderData_p + Offset) == A2_HEADER_PATTERN)) {
            Res = A2_HEADER_PATTERN_MATCH;
            break;
        }

        Offset++;
    } while (Offset < (A2_HEADER_LENGTH - 1));

    *StartInBuffer_p = Offset;

    return Res;
}

/** @} */
/** @} */
/** @} */
