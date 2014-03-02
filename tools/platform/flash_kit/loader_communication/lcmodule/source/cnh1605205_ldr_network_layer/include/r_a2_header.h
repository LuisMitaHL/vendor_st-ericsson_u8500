/*******************************************************************************
 * Copyright (C) ST-Ericsson SA 2011
 * License terms: 3-clause BSD license
 ******************************************************************************/
#ifndef _R_A2_HEADER_H_
#define _R_A2_HEADER_H_
/**
 *  @addtogroup ldr_communication_serv
 *  @{
 *    @addtogroup a2_family
 *    @{
 *      @addtogroup ldr_header A2 Header
 *      Functionalities for serialization and de-serialization A2 header.
 *
 *      @{
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "t_basicdefinitions.h"
#include "t_a2_header.h"
#include "t_a2_network.h"

/*******************************************************************************
 * Declaration of functions
 ******************************************************************************/

/**
 * Calculates the expected total packet length in bytes given the in-
 * formation in Header_p.
 *
 * @param [in] Header_p Pointer to the header structure.
 *
 * @return  The length of the packet in bytes.
 */
uint32 A2_GetPacketLength(const A2_Header_t *Header_p);

/**
 * Converts the header stored in network format to a structure.
 *
 * @param [out] Header_p  Pointer to the header structure where the
 *                        header data should be placed.
 * @param [in]  Data_p    Pointer to the buffer with received header.
 *
 * @return Nothing.
 */
void A2_DeserializeHeader(A2_Header_t *Header_p, void *Data_p);

/**
 * Converts the header structure to network format.
 *
 * The data buffer must be of at least size A2_HEADER_LENGTH.
 *
 * @param [out] Data_p    Pointer to the buffer where the
 *                        serialized header should be placed.
 * @param [in]  Header_p  Pointer to the source header structure.
 *
 * @return none.
 */
void A2_SerializeHeader(void *Data_p, const A2_Header_t *Header_p);

/**
 * Determines whether the first HEADER_LENGTH bytes of Data_p contains a
 * valid header.
 *
 * @param [in] Data_p Pointer to the header candidate.
 *
 * @retval  TRUE  if header is valid.
 * @retval  FALSE if header is not valid.
 */
boolean A2_IsValidHeader(const void *Data_p);

/**
 * Checks if new header is received.
 *
 * @param [in] In_p Pointer to the structure with receiver information.
 *
 * @retval TRUE   If header is received.
 * @retval FALSE  If header is not received.
 */
boolean A2_IsReceivedHeader(A2_Inbound_t *In_p);

/** @} */
/** @} */
/** @} */
#endif /*_R_A2_HEADER_H_*/
