/*******************************************************************************
 * Copyright (C) ST-Ericsson SA 2011
 * License terms: 3-clause BSD license
 ******************************************************************************/
#ifndef _R_R15_HEADER_H_
#define _R_R15_HEADER_H_
/**
 *  @addtogroup ldr_communication_serv
 *  @{
 *    @addtogroup r15_family
 *    @{
 *      @addtogroup ldr_r15_header R15 header
 *      Functionalities for serialization and de-serialization header from R15
 *      protocol family.
 *
 *      @{
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "t_basicdefinitions.h"
#include "t_r15_header.h"
#include "t_r15_network_layer.h"

/*******************************************************************************
 * Declaration of functions
 ******************************************************************************/

/**
 * Get packet length in bytes given the information in Header_p.
 *
 * @param [in] Header_p Pointer to the header structure.
 *
 * @return  The length of the packet in bytes.
 */
uint32 R15_GetPacketLength(const R15_Header_t *Header_p);

/**
 * Deserialize the header stored in network format to a structure.
 *
 * @param [out] Header_p  Pointer to the header structure where the
 *                        header data should be placed.
 * @param [in]  Data_p    Pointer to the buffer with received header.
 *
 * @return none.
 */
void R15_DeserializeHeader(R15_Header_t *Header_p, void *Data_p);

/**
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
void R15_SerializeHeader(void *Data_p, const R15_Header_t *Header_p);

/**
 * Deserialize the extended header stored in network format to a structure.
 *
 * @param [out] ExtendedHeader_p  Pointer to the extended header structure where the
 *                                header data should be placed.
 * @param [in]  Data_p            Pointer to the buffer with received header.
 *
 * @return none.
 */
void R15_DeserializeBulkExtendedHeader(BulkExtendedHeader_t *ExtendedHeader_p, void *Data_p);

/**
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
void R15_SerializeExtendedHeader(void *Data_p, uint8 ProtocolType, const void *ExtHeader_p, uint8 *ExtHdrChecksum_p);

/**
 * Deserialize the command extended header stored in network format to a structure.
 *
 * @param [out] CommandExtendedHeader_p  Pointer to the command extended header structure where the
 *                                       header data should be placed.
 * @param [in]  Data_p                   Pointer to the buffer with received header.
 *
 * @return none.
 */
void R15_DeserializeCommandExtendedHeader(CommandExtendedHeader_t *CommandExtendedHeader_p, void *Data_p);

/**
 * Determines whether the first HEADER_LENGTH bytes of Data_p contains a
 * valid header.
 *
 * @param [in] Data_p Pointer to the header candidate.
 *
 * @retval  TRUE  If header is valid.
 * @retval  FALSE If header is not valid.
 */
boolean R15_IsValidHeader(const void *Data_p);

/**
 * Verify received extended header.
 *
 * @param [in] Data_p   Pointer to the buffer.
 * @param [in] Length   Length of the buffer.
 * @param [in] Checksum Checksum of the buffer.
 *
 * @retval TRUE   If is extended header verified.
 * @retval FALSE  If extended header is not verified.
 */
boolean R15_IsValidExtendedHeader(const void *Data_p, const uint8 Length, const uint8 Checksum);

/**
 * Checks if new header is received.
 *
 * @param [in] In_p Pointer to the structure with receiver information.
 *
 * @retval TRUE   If header is received.
 * @retval FALSE  If header is not received.
 */
boolean R15_IsReceivedHeader(R15_Inbound_t *In_p);

/** @} */
/** @} */
/** @} */
#endif /* _R_R15_HEADER_H_ */
