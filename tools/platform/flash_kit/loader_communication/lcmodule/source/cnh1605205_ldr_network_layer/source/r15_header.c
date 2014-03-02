/*******************************************************************************
 * Copyright (C) ST-Ericsson SA 2011
 * License terms: 3-clause BSD license
 ******************************************************************************/
/**
 *  @addtogroup ldr_communication_serv
 *  @{
 *    @addtogroup r15_family
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
#include "r_r15_header.h"
#include "r_serialization.h"
#include "t_bulk_protocol.h"
#include "t_r15_network_layer.h"

/*******************************************************************************
 * Declaration of file local functions
 ******************************************************************************/
static uint8 memxor(const uint8 *Buf_p, uint32 Length);
static uint8 R15_FindHeaderPattern(R15_Inbound_t *In_p, uint32 *StartInBuffer_p);

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
void R15_DeserializeHeader(R15_Header_t *Header_p, void *Data_p)
{
    void *Iter_p = Data_p;

    Header_p->HeaderPattern = get_uint8(&Iter_p);
    Header_p->Protocol = get_uint8(&Iter_p);
    Header_p->HeaderPatternExtension = get_uint16(&Iter_p);
    Header_p->Flags = get_uint8(&Iter_p);
    Header_p->ExtendedHeaderLength = get_uint8(&Iter_p);
    Header_p->ExtendedHeaderChecksum = get_uint8(&Iter_p);
    Header_p->PayloadLength = get_uint32_le(&Iter_p);
    Header_p->PayloadChecksum = get_uint32_le(&Iter_p);
    Header_p->HeaderChecksum = get_uint8(&Iter_p);
}

/**
 * Deserialize the extended header stored in network format to a structure.
 *
 * @param [out] Header_p  Pointer to the extended header structure where the
 *                        header data should be placed.
 * @param [in]  Data_p    Pointer to the buffer with received header.
 *
 * @return none.
 */
void R15_DeserializeBulkExtendedHeader(BulkExtendedHeader_t *ExtendedHeader_p, void *Data_p)
{
    void *Iter_p = Data_p;

    ExtendedHeader_p->Session = get_uint16_le(&Iter_p);
    ExtendedHeader_p->TypeFlags = get_uint8(&Iter_p);
    ExtendedHeader_p->AcksChunk = get_uint8(&Iter_p);
    ExtendedHeader_p->ChunkSize = get_uint32_le(&Iter_p);
    ExtendedHeader_p->Offset = get_uint64_le(&Iter_p);
    ExtendedHeader_p->Length = get_uint32_le(&Iter_p);
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
void R15_SerializeHeader(void *Data_p, const R15_Header_t *header_p)
{
    void *Iter_p = Data_p;

    put_uint8(&Iter_p, HEADER_PATTERN);
    put_uint8(&Iter_p, header_p->Protocol);
    put_uint8(&Iter_p, HEADER_PATTERN_EXTENSION);
    put_uint8(&Iter_p, HEADER_PATTERN_EXTENSION);
    put_uint8(&Iter_p, header_p->Flags);

    put_uint8(&Iter_p, header_p->ExtendedHeaderLength);
    put_uint8(&Iter_p, header_p->ExtendedHeaderChecksum);

    put_uint32_le(&Iter_p, header_p->PayloadLength);
    put_uint32_le(&Iter_p, header_p->PayloadChecksum);

    put_uint8(&Iter_p, memxor((uint8 *)Data_p, HEADER_LENGTH - 1));
}


/*
 * Converts the extended header structure to network format.
 *
 * @param [out] Data_p            Pointer to the buffer where the
 *                                serialized header should be placed.
 * @param [in]  ProtocolType      The type of the protocol used for
 *                                communication.
 * @param [in]  ExtHeader_p       Pointer to the source header structure.
 * @param [out] ExtHdrChecksum_p  Calculated checksum for extended header data.
 *
 * @return  none.
 */
void R15_SerializeExtendedHeader(void *Data_p, uint8 ProtocolType, const void *ExtHeader_p, uint8 *ExtHdrChecksum_p)
{
    void *Iter_p = Data_p;

    if (COMMAND_PROTOCOL == ProtocolType) {
        put_uint16_le(&Iter_p, ((CommandExtendedHeader_t *)ExtHeader_p)->SessionState);
        put_uint8(&Iter_p, ((CommandExtendedHeader_t *)ExtHeader_p)->Command);
        put_uint8(&Iter_p, ((CommandExtendedHeader_t *)ExtHeader_p)->CommandGroup);
        *ExtHdrChecksum_p = memxor((uint8 *)ExtHeader_p, COMMAND_EXTENDED_HEADER_LENGTH);
    } else {
        put_uint16_le(&Iter_p, ((BulkExtendedHeader_t *)ExtHeader_p)->Session);
        put_uint8(&Iter_p, ((BulkExtendedHeader_t *)ExtHeader_p)->TypeFlags);
        put_uint8(&Iter_p, ((BulkExtendedHeader_t *)ExtHeader_p)->AcksChunk);
        put_uint32_le(&Iter_p, ((BulkExtendedHeader_t *)ExtHeader_p)->ChunkSize);
        put_uint64_le(&Iter_p, ((BulkExtendedHeader_t *)ExtHeader_p)->Offset);
        put_uint32_le(&Iter_p, ((BulkExtendedHeader_t *)ExtHeader_p)->Length);
        *ExtHdrChecksum_p = memxor((uint8 *)ExtHeader_p, BULK_EXTENDED_HEADER_LENGTH);
    }
}

/**
 * Deserialize the command extended header stored in network format to a structure.
 *
 * @param [out] CommandExtendedHeader_p  Pointer to the command extended header structure where the
 *                                       header data should be placed.
 * @param [in]  Data_p                   Pointer to the buffer with received header.
 *
 * @return none.
 */
void R15_DeserializeCommandExtendedHeader(CommandExtendedHeader_t *CommandExtendedHeader_p, void *Data_p)
{
    void *Iter_p = Data_p;

    CommandExtendedHeader_p->SessionState = get_uint16_le(&Iter_p);
    CommandExtendedHeader_p->Command = get_uint8(&Iter_p);
    CommandExtendedHeader_p->CommandGroup = get_uint8(&Iter_p);
}

/*
 * Get packet length in bytes given the information in Header_p.
 *
 * @param [in] Header_p Pointer to the header structure.
 *
 * @return  The length of the packet in bytes.
 */
uint32 R15_GetPacketLength(const R15_Header_t *Header_p)
{
    return HEADER_LENGTH + Header_p->ExtendedHeaderLength +
           Header_p->PayloadLength;
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
boolean R15_IsValidHeader(const void *Data_p)
{
    if (memxor((uint8 *)Data_p, HEADER_LENGTH) == 0) {
        /** @todo It's possible to check other things here */
        return TRUE;
    }

    return FALSE;
}

/*
 * Checks if new header is received.
 *
 * @param [in] In_p Pointer to the structure with receiver information.
 *
 * @retval TRUE   If header is received.
 * @retval FALSE  If header is not received.
 */
boolean R15_IsReceivedHeader(R15_Inbound_t *In_p)
{
    uint32 StartHeaderInBuffer = 0;
    uint8 *TmpPointer_p = NULL;
    uint8 Res = 0;

    Res = R15_FindHeaderPattern(In_p, &StartHeaderInBuffer);

    if (HEADER_PATTERN_CANDIDATE == Res) {
        /* call for receiving the rest bytes in header */
        In_p->ReqData = StartHeaderInBuffer + ALIGNED_HEADER_LENGTH - In_p->RecData;
        TmpPointer_p = In_p->Target_p + StartHeaderInBuffer;
        memcpy(In_p->Target_p, TmpPointer_p, In_p->RecData - StartHeaderInBuffer);
        In_p->ReqBuffOffset = In_p->RecData - StartHeaderInBuffer;
    } else {
        if (HEADER_PATTERN_MATCH == Res) {
            /* Check start point of header in received data */
            if (StartHeaderInBuffer == 0) {
                return TRUE;
            } else {
                In_p->ReqData = StartHeaderInBuffer + ALIGNED_HEADER_LENGTH - In_p->RecData;
                TmpPointer_p = In_p->Target_p + StartHeaderInBuffer;
                memcpy(In_p->Target_p, TmpPointer_p, In_p->RecData - StartHeaderInBuffer);
                In_p->ReqBuffOffset = In_p->RecData - StartHeaderInBuffer;
            }
        } else {
            /* Sync the header. */
            In_p->RecData       = 0;
            In_p->ReqBuffOffset = 0;
            In_p->State         = RECEIVE_HEADER;
            In_p->ReqData       = ALIGNED_HEADER_LENGTH;
            In_p->Target_p      = In_p->Scratch;
        }
    }

    return FALSE;
}


/*
 * Verify received extended header.
 *
 * @param [in] Data_p   Pointer to the buffer.
 * @param [in] Length   Length of the buffer.
 * @param [in] Checksum Checksum of the buffer.
 *
 * @retval TRUE   If is extended header verified.
 * @retval FALSE  If extended header is not verified.
 */
boolean R15_IsValidExtendedHeader(const void *Data_p, const uint8 Length, const uint8 Checksum)
{
    if (memxor((uint8 *)Data_p, Length) == Checksum) {
        return TRUE;
    }

    return FALSE;
}


static uint8 R15_FindHeaderPattern(R15_Inbound_t *In_p, uint32 *StartInBuffer_p)
{
    uint8 Res = NO_HEADER_PATTERN;
    uint32 Offset = 0;
    uint8 *HeaderData_p = In_p->Scratch;

    do {
        Res = NO_HEADER_PATTERN;

        if (*((uint8 *)HeaderData_p + Offset) == HEADER_PATTERN) {
            Res = HEADER_PATTERN_CANDIDATE;

            if (Offset < In_p->RecData - 1) {
                if ((*((uint8 *)HeaderData_p + Offset + 1) == PROTO_COMMAND) || (*((uint8 *)HeaderData_p + Offset + 1) == PROTO_BULK)) {
                    if (Offset < In_p->RecData - 2) {
                        if (*((uint8 *)HeaderData_p + Offset + 2) == HEADER_PATTERN_EXTENSION) {
                            if (Offset < In_p->RecData - 3) {
                                if (*((uint8 *)HeaderData_p + Offset + 3) == HEADER_PATTERN_EXTENSION) {
                                    Res = HEADER_PATTERN_MATCH;
                                    break;
                                } else {
                                    Offset = Offset + 2;
                                    Res = NO_HEADER_PATTERN;
                                }
                            } else {
                                Res = HEADER_PATTERN_CANDIDATE;
                                break;
                            }
                        } else {
                            Offset ++;
                            Res = NO_HEADER_PATTERN;
                        }
                    } else {
                        Res = HEADER_PATTERN_CANDIDATE;
                        break;
                    }
                } else {
                    Res = NO_HEADER_PATTERN;
                }
            } else {
                Res = HEADER_PATTERN_CANDIDATE;
                break;
            }
        }

        Offset++;
    } while (Offset < In_p->RecData);

    *StartInBuffer_p = Offset;

    return Res;
}


static uint8 memxor(const uint8 *Buf_p, uint32 Length)
{
    uint8 Value = 0;
    const uint8 *Stop_p = Buf_p + Length;

    /** @todo Optimize for 32 bit access? */
    do {
        Value ^= *Buf_p++;
    } while (Buf_p < Stop_p);

    return Value;
}

/** @} */
/** @} */
/** @} */
