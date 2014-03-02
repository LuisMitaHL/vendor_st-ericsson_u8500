/*******************************************************************************
 * Copyright (C) ST-Ericsson SA 2011
 * License terms: 3-clause BSD license
 ******************************************************************************/
#ifndef _R_A2_NETWORK_H_
#define _R_A2_NETWORK_H_
/**
 *  @addtogroup ldr_communication_serv
 *  @{
 *    @addtogroup a2_family
 *    @{
 *      @addtogroup ldr_network_layer A2 Network Layer
 *      A2 network layer support receiving and sending A2 packets.
 *
 *      @{
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "error_codes.h"
#include "t_basicdefinitions.h"
#include "t_a2_network.h"
#include "t_communication_service.h"

/*******************************************************************************
 * Declaration of functions
 ******************************************************************************/
/**
 * Initializes the A2 family network layer.
 *
 * @param [in,out] Communication_p Communication module context.
 *
 * @retval E_SUCCESS                   After successful execution.
 * @retval E_FAILED_TO_INIT_COM_DEVICE Fail to initialize the communication
 *                                     device.
 */
ErrorCode_e A2_Network_Initialize(Communication_t *Communication_p);

/**
 * Shut down the A2 network layer.
 *
 * @param [in,out] Communication_p Communication module context.
 *
 * @retval  E_SUCCESS After successful execution.
 */
ErrorCode_e A2_Network_Shutdown(const Communication_t *const Communication_p);

/**
 * Cancel retransmission of packets.
 *
 * @param [in,out] Communication_p Communication module context.
 * @param [in]     UniqueKey       Unique key used for identification of packet
 *                                 for retransmission.
 *
 * @return none.
 */
ErrorCode_e A2_Network_CancelRetransmission(const Communication_t *const Communication_p, uint32 UniqueKey);

/**
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
void A2_Network_ReadCallback(const void *Data_p, const uint32 Length, void *Param_p);

/**
 * Handler function that is called after successful transmission
 * of a packet.
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
void A2_Network_WriteCallback(const void *Data_p, const uint32 Length, void *Param_p);

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
ErrorCode_e A2_Network_ReceiverHandler(Communication_t *Communication_p);

/**
 * Handler for transmitting available packets.
 *
 * This function transmits packet from outgoing queue.
 *
 * @param [in,out] Communication_p Communication module context.
 *
 * @retval E_SUCCESS                    After successful execution.
 * @retval E_RETRANSMITION_FAILED       If packet sending fails.
 */
ErrorCode_e A2_Network_TransmiterHandler(Communication_t *Communication_p);

/**
 * Network packet allocation in A2 protocol family.
 *
 * @param [in,out] Communication_p Communication module context.
 * @param [in]     BufferSize      Size of buffer used for network packet.
 *
 * @retval Pointer Pointer to allocated packet meta info.
 * @retval NULL    if allocation fail.
 */
A2_PacketMeta_t *A2_Network_PacketAllocate(const Communication_t *const Communication_p, int BufferSize);

/**
 * Network packet release in A2 protocol family.
 *
 * @param [in,out] Communication_p Communication module context.
 * @param [in]     Meta_p          Meta info for used network packet.
 *
 * @retval E_SUCCESS                   Successful network packet release.
 * @retval E_INVALID_INPUT_PARAMETERS  Invalid input parameter.
 */
ErrorCode_e A2_Network_PacketRelease(const Communication_t *const Communication_p, A2_PacketMeta_t *Meta_p);

/** @} */
/** @} */
/** @} */
#endif /*_R_A2_NETWORK_H_*/
