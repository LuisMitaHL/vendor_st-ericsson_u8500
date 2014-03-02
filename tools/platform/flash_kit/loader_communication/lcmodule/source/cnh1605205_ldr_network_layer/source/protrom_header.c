/*******************************************************************************
 * Copyright (C) ST-Ericsson SA 2011
 * License terms: 3-clause BSD license
 ******************************************************************************/
/**
 *  @addtogroup ldr_communication_serv
 *  @{
 *    @addtogroup protrom_family
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
#include "r_protrom_header.h"
#include "r_serialization.h"

/*******************************************************************************
 * Declaration of file local functions
 ******************************************************************************/
static uint8 Protrom_FindHeaderPattern(const uint8 *const HeaderData_p, uint32 *StartInBuffer_p);


/*******************************************************************************
 * Definition of internal functions
 ******************************************************************************/

/*
 * Converts the header stored in network format to a structure.
 *
 * @param [out] Header_p  Pointer to the header structure where the
 *                        header data should be placed.
 * @param [in]  Data_p    Pointer to the buffer with received header.
 *
 * @return none.
 */
void Protrom_DeserializeHeader(Protrom_Header_t *Header_p, void *Data_p)
{
    void *Iter_p = Data_p;

    Header_p->HeaderPattern = get_uint8(&Iter_p);
    Header_p->Protocol = get_uint8(&Iter_p);
    Header_p->SourceAddress = get_uint8(&Iter_p);
    Header_p->DestinationAddress = get_uint8(&Iter_p);
    Header_p->ReservedField = get_uint8(&Iter_p);
    Header_p->PayloadLength = get_uint16(&Iter_p);
}


/*
 * Converts the header structure to network format.
 *
 * The data buffer must be of at least size PROTROM_HEADER_LENGTH.
 *
 * @param [out] Data_p    Pointer to the buffer where the
 *                        serialized header should be placed.
 * @param [in]  Header_p  Pointer to the source header structure.
 *
 * @return none.
 */
void Protrom_SerializeHeader(void *Data_p, const Protrom_Header_t *Header_p)
{
    void *Iter_p = Data_p;

    put_uint8(&Iter_p, PROTROM_HEADER_PATTERN);
    put_uint8(&Iter_p, PROTO_PROTROM);
    put_uint8(&Iter_p, PROTROM_SOURCE_ADDRESS);
    put_uint8(&Iter_p, PROTROM_DESTINATION_ADDRESS);
    put_uint8(&Iter_p, PROTROM_RESERVED_FIELD);
    put_uint16(&Iter_p, Header_p->PayloadLength);
}

/*
 * Get packet length in bytes given the information in Header_p.
 *
 * @param [in] Header_p Pointer to the header structure.
 *
 * @return  The length of the packet in bytes.
 */
uint32 Protrom_GetPacketLength(const Protrom_Header_t *Header_p)
{
    return PROTROM_HEADER_LENGTH + Header_p->PayloadLength;
}


/*
 * Determines whether the first PROTROM_HEADER_LENGTH bytes of Data_p contains a
 * valid header.
 *
 * @param [in] Data_p Pointer to the header candidate.
 *
 * @retval  TRUE  If header is valid.
 * @retval  FALSE If header is not valid.
 */
boolean Protrom_IsValidHeader(const void *Data_p)
{
    uint8 *Temp_p = (uint8 *)Data_p;
    uint16 ProtromVal = 0;

    if (PROTROM_HEADER_PATTERN != *Temp_p) {
        return FALSE;
    }

    if (PROTO_PROTROM != *(Temp_p + 1)) {
        return FALSE;
    }

    if (PROTROM_SOURCE_ADDRESS != *(Temp_p + 2)) {
        return FALSE;
    }

    if (PROTROM_DESTINATION_ADDRESS != *(Temp_p + 3)) {
        return FALSE;
    }

    if (PROTROM_RESERVED_FIELD != *(Temp_p + 4)) {
        return FALSE;
    }

    ProtromVal = (Temp_p[6] << 8) | Temp_p[5];

    if (ProtromVal < 1) {
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
boolean Protrom_IsReceivedHeader(Protrom_Inbound_t *In_p)
{
    uint32 StartHeaderInBuffer = 0;
    uint8 *TmpPointer_p = NULL;
    uint8 Res = 0;

    Res = Protrom_FindHeaderPattern(In_p->Scratch, &StartHeaderInBuffer);

    if (PROTROM_HEADER_PATTERN_CANDIDATE == Res) {
        /* call for receiving the rest bytes in header */
        In_p->ReqData = StartHeaderInBuffer + PROTROM_HEADER_LENGTH - In_p->RecData;
        TmpPointer_p = In_p->Target_p + StartHeaderInBuffer;
        memcpy(In_p->Target_p, TmpPointer_p, In_p->RecData - StartHeaderInBuffer);
        In_p->ReqBuffOffset = In_p->RecData - StartHeaderInBuffer;
    } else {
        if (PROTROM_HEADER_PATTERN_MATCH == Res) {
            /* Check start point of header in received data */
            if (StartHeaderInBuffer == 0) {
                return TRUE;
            } else {
                In_p->ReqData = StartHeaderInBuffer + PROTROM_HEADER_LENGTH - In_p->RecData;
                TmpPointer_p = In_p->Target_p + StartHeaderInBuffer;
                memcpy(In_p->Target_p, TmpPointer_p, In_p->RecData - StartHeaderInBuffer);
                In_p->ReqBuffOffset = In_p->RecData - StartHeaderInBuffer;
            }
        } else {
            In_p->ReqData = PROTROM_HEADER_LENGTH;
            In_p->ReqBuffOffset = 0;
        }
    }

    return FALSE;
}

/*
 * Find header pattern in the received data.
 *
 * @param [in] HeaderData_p     Pointer to the structure with receiver data.
 * @param [out] StartInBuffer_p Offset in the buffer where is find header.
 *
 * @retval TRUE   If header is received.
 * @retval FALSE  If header is not received.
 */
static uint8 Protrom_FindHeaderPattern(const uint8 *const HeaderData_p, uint32 *StartInBuffer_p)
{
    uint8 Res = NO_PROTROM_HEADER_PATTERN;
    uint32 Offset = 0;

    do {
        Res = NO_PROTROM_HEADER_PATTERN;

        if (*((uint8 *)HeaderData_p + Offset) == PROTROM_HEADER_PATTERN) {
            Res = PROTROM_HEADER_PATTERN_CANDIDATE;

            if (Offset < 6) {
                if (*((uint8 *)HeaderData_p + Offset + 1) == PROTO_PROTROM) {
                    if (Offset < 5) {
                        if (*((uint8 *)HeaderData_p + Offset + 2) == PROTROM_SOURCE_ADDRESS) {
                            if (Offset < 4) {
                                if (*((uint8 *)HeaderData_p + Offset + 3) == PROTROM_DESTINATION_ADDRESS) {
                                    Res = PROTROM_HEADER_PATTERN_MATCH;
                                    break;
                                } else {
                                    Offset = Offset + 2;
                                    Res = NO_PROTROM_HEADER_PATTERN;
                                }
                            } else {
                                Res = PROTROM_HEADER_PATTERN_CANDIDATE;
                                break;
                            }
                        } else {
                            Offset ++;
                            Res = NO_PROTROM_HEADER_PATTERN;
                        }
                    } else {
                        Res = PROTROM_HEADER_PATTERN_CANDIDATE;
                        break;
                    }
                } else {
                    Res = NO_PROTROM_HEADER_PATTERN;
                }
            } else {
                Res = PROTROM_HEADER_PATTERN_CANDIDATE;
                break;
            }
        }

        Offset++;
    } while (Offset < PROTROM_HEADER_LENGTH);

    *StartInBuffer_p = Offset;

    return Res;
}

/** @} */
/** @} */
/** @} */
