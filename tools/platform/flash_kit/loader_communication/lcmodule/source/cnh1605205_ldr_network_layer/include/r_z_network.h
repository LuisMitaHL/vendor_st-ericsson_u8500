/*******************************************************************************
 * Copyright (C) ST-Ericsson SA 2011
 * License terms: 3-clause BSD license
 ******************************************************************************/
#ifndef R_Z_NETWORK_H_
#define R_Z_NETWORK_H_
/**
 *  @addtogroup ldr_communication_serv
 *  @{
 *    @addtogroup z_family
 *    @{
 *      @addtogroup ldr_z_network_layer Z network layer
 *      Z network layer support receiving and sending bytes from
 *      Z protocol.
 *
 *      @{
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "error_codes.h"
#include "t_basicdefinitions.h"
#include "t_z_network.h"
#include "t_communication_service.h"
#include "t_z_protocol.h"

/*******************************************************************************
 * Declaration of functions
 ******************************************************************************/
/**
 * Initializes the Z family network layer.
 *
 * @param [in]  Communication_p Communication module context.
 *
 * @retval  E_SUCCESS                   After successful execution.
 * @retval  E_FAILED_TO_INIT_COM_DEVICE Failed to initialize the communication
 *                                      device.
 */
ErrorCode_e Z_Network_Initialize(Communication_t *Communication_p);

/**
 * Shut down the Z family network layer.
 *
 * @param [in]  Communication_p Communication module context.
 *
 * @retval  E_SUCCESS After successful execution.
 */
ErrorCode_e Z_Network_Shutdown(const Communication_t *const Communication_p);

/**
 * Handler for received packets in Z protocol family.
 *
 * This callback function handles the received packets.
 *
 * @param [in] Data_p  Pointer to the received data.
 * @param [in] Length  Length of the received data.
 * @param [in] Param_p Extra parameters.
 *
 * @return none.
 */
void Z_Network_ReadCallback(const void *Data_p, const uint32 Length, void *Param_p);

/**
 * Handler function that is called after successful transmission of a packet.
 *
 * If new packet is ready for transmitting it starts
 * the transmission of the packet.
 *
 * @param [in] Data_p   Pointer to the data for transmitting.
 * @param [in] Length   Length of the received data.
 * @param [in] Param_p  Extra parameters.
 *
 * @return none.
 */

void Z_Network_WriteCallback(const void *Data_p, const uint32 Length, void *Param_p);

/**
 * Handler for receiving new data in Z protocol family.
 *
 * This function checks if new data has been received.
 *
 * @param [in]  Communication_p Communication module context.
 *
 * @return  none.
 */
void Z_Network_ReceiverHandler(Communication_t *Communication_p);


/**
 * Handler for sending new data in Z protocol family.
 *
 * This function checks if new data has been transmitted.
 *
 * @param [in]  Communication_p  Communication module context.
 * @param [in]  SendingContent_p Pointer to the sending context.
 *
 * @return  none.
 */
ErrorCode_e Z_Network_TransmiterHandler(Communication_t *Communication_p, Z_SendingContent_t *SendingContent_p);

/** @} */
/** @} */
/** @} */
#endif /*R_Z_NETWORK_H_*/
