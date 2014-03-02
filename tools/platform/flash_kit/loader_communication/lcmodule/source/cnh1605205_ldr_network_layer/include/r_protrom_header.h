/*******************************************************************************
 * Copyright (C) ST-Ericsson SA 2011
 * License terms: 3-clause BSD license
 ******************************************************************************/
#ifndef R_PROTROM_HEADER_H_
#define R_PROTROM_HEADER_H_
/**
 *  @addtogroup ldr_communication_serv
 *  @{
 *    @addtogroup protrom_family
 *    @{
 *      @addtogroup ldr_protrom_header PROTROM header
 *      Functionalities for serialization and de-serialization PROTROM header.
 *
 *      @{
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include <t_basicdefinitions.h>
#include "t_protrom_network.h"

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
uint32 Protrom_GetPacketLength(const Protrom_Header_t *Header_p);

/**
 * Converts the header stored in network format to a structure.
 *
 * @param [out] Header_p  Pointer to the header structure where the
 *                        header data should be placed.
 * @param [in]  Data_p    Pointer to the buffer with received header.
 *
 * @return none.
 */
void Protrom_DeserializeHeader(Protrom_Header_t *Header_p, void *Data_p);

/**
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
void Protrom_SerializeHeader(void *Data_p, const Protrom_Header_t *Header_p);

/**
 * Determines whether the first PROTROM_HEADER_LENGTH bytes of Data_p contains a
 * valid header.
 *
 * @param [in] Data_p Pointer to the header candidate.
 *
 * @retval  TRUE  If header is valid.
 * @retval  FALSE If header is not valid.
 */
boolean Protrom_IsValidHeader(const void *Data_p);

/**
 * Checks if new header is received.
 *
 * @param [in] In_p Pointer to the structure with receiver information.
 *
 * @retval TRUE   If header is received.
 * @retval FALSE  If header is not received.
 */
boolean Protrom_IsReceivedHeader(Protrom_Inbound_t *In_p);

/** @} */
/** @} */
/** @} */
#endif /*R_PROTROM_HEADER_H_*/
