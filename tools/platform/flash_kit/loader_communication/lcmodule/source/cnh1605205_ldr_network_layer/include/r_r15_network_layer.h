/*******************************************************************************
 * Copyright (C) ST-Ericsson SA 2011
 * License terms: 3-clause BSD license
 ******************************************************************************/
#ifndef _R_R15_NETWORK_LAYER_H_
#define _R_R15_NETWORK_LAYER_H_
/**
 *  @addtogroup ldr_communication_serv
 *  @{
 *    @addtogroup r15_family
 *    @{
 *      @addtogroup ldr_r15_network_layer R15 network layer
 *      R15 network layer support receiving and sending packets from
 *      R15 protocols.
 *
 *      @{
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "error_codes.h"
#include "t_basicdefinitions.h"
#include "t_r15_network_layer.h"

/*******************************************************************************
 * Declaration of functions
 ******************************************************************************/
/**
 * Initializes the r15 network layer.
 *
 * @param [in,out] Communication_p Communication module context.
 *
 * @retval  E_SUCCESS                   After successful execution.
 * @retval  E_FAILED_TO_INIT_COM_DEVICE Fail to initialize the communication
 *                                      device.
 */
ErrorCode_e R15_Network_Initialize(Communication_t *Communication_p);

/**
 * Shut down the r15 network layer.
 *
 * @param [in,out] Communication_p Communication module context.
 *
 * @retval  E_SUCCESS After successful execution.
 */
ErrorCode_e R15_Network_Shutdown(const Communication_t *const Communication_p);

/**
 * Cancel retransmission of packets.
 *
 * @param [in,out] Communication_p Communication module context.
 * @param [in]     UniqueKey       Unique key used for identification of packet
 *                                 for retransmission.
 *
 * @return none.
 */
ErrorCode_e R15_Network_CancelRetransmission(const Communication_t *const Communication_p, uint32 UniqueKey);

/**
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
uint32 R15_Network_CreateUniqueKey(const PacketMeta_t *const Packet_p, const uint8 ExternalKey);

/**
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
void R15_Network_ReadCallback(const void *Data_p, const uint32 Length, void *Param_p);

/**
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
void R15_Network_WriteCallback(const void *Data_p, const uint32 Length, void *Param_p);

/**
 * Handler for receiving new data.
 *
 * This function checks if new data has been received.
 *
 * @param [in,out] Communication_p Communication module context.
 *
 * @retval E_SUCCESS                    After successful execution.
 * @retval E_FAILED_TO_FIND_COMM_BUFFER Fail to allocate the communication buffer.
 */
ErrorCode_e R15_Network_ReceiverHandler(Communication_t *Communication_p);

/**
 * Handler for sending data.
 *
 * This function checks if new data is available for sending and handles the transmission.
 *
 * @param [in,out] Communication_p Communication module context.
 *
 * @retval E_SUCCESS              After successful execution.
 * @retval E_RETRANSMITION_FAILED Packet retransmission failed.
 */
ErrorCode_e R15_Network_TransmiterHandler(Communication_t *Communication_p);

/**
 * Network packet allocation in R15 protocol family.
 *
 * @param [in,out] Communication_p Communication module context.
 * @param [in]     BufferSize      Size of buffer used for network packet.
 *
 * @retval  Pointer Pointer to allocated packet meta info.
 * @retval  NULL    if allocation fail.
 */
PacketMeta_t *R15_Network_PacketAllocate(const Communication_t *const Communication_p, int BufferSize);

/**
 * Network packet release in R15 protocol family.
 *
 * @param [in,out] Communication_p Communication module context.
 * @param [in]     Meta_p          Meta info for used network packet.
 *
 * @retval  E_SUCCESS                   Successful network packet release.
 * @retval  E_INVALID_INPUT_PARAMETERS  Invalid input parameter.
 */
ErrorCode_e R15_Network_PacketRelease(const Communication_t *const Communication_p, PacketMeta_t *Meta_p);

/** @} */
/** @} */
/** @} */
#endif /*_R_R15_NETWORK_LAYER_H_*/
