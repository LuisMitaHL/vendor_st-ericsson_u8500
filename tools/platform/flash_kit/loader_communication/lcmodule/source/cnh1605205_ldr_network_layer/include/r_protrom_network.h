/*******************************************************************************
 * Copyright (C) ST-Ericsson SA 2011
 * License terms: 3-clause BSD license
 ******************************************************************************/
#ifndef R_PROTROM_NETWORK_H_
#define R_PROTROM_NETWORK_H_
/**
 *  @addtogroup ldr_communication_serv
 *  @{
 *    @addtogroup protrom_family
 *    @{
 *      @addtogroup ldr_protrom_network_layer PROTROM network layer
 *      PROTROM network layer support receiving and sending PROTROM packets.
 *
 *      @{
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "error_codes.h"
#include "t_basicdefinitions.h"
#include "t_protrom_network.h"
#include "t_communication_service.h"

/*******************************************************************************
 * Declaration of functions
 ******************************************************************************/
/**
 * Initializes the PROTOROM network layer.
 *
 * @param [in]  Communication_p Communication module context.
 *
 * @retval  E_SUCCESS                   After successful execution.
 * @retval  E_FAILED_TO_INIT_COM_DEVICE Failed to initialize the communication
 *                                      device.
 */
ErrorCode_e Protrom_Network_Initialize(Communication_t *Communication_p);

/**
 * Shutdown the PROTROM network layer.
 *
 * @param [in]  Communication_p Communication module context.
 *
 * @retval  E_SUCCESS After successful execution.
 */
ErrorCode_e Protrom_Network_Shutdown(const Communication_t *const Communication_p);

/**
 * This callback function handles the received packets.
 *
 * @param [in] Data_p  Pointer to the received data.
 * @param [in] Length  Length of the received data.
 * @param [in] Param_p Extra parameters.
 *
 * @return none.
 */
void Protrom_Network_ReadCallback(const void *Data_p, const uint32 Length, void *Param_p);

/**
 * Handler function that is called after successful transmission
 * of a packet.
 *
 * If new packet is ready for transmitting it starts
 * the transmission of the packet.
 *
 * @param [in] Data_p  Pointer to the data for transmitting.
 * @param [in] Length  Length of the received data.
 * @param [in] Param_p Extra parameters.
 *
 * @return none.
 */
void Protrom_Network_WriteCallback(const void *Data_p, const uint32 Length, void *Param_p);

/**
 * This function checks if new data has been received.
 *
 * @param [in]  Communication_p Communication module context.
 *
 * @return  none.
 */
void Protrom_Network_ReceiverHandler(Communication_t *Communication_p);

/** @} */
/** @} */
/** @} */
#endif /*R_PROTROM_NETWORK_H_*/
