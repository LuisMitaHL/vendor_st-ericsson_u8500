/*******************************************************************************
 * $Copyright ST-Ericsson 2009 $
 ******************************************************************************/
#ifndef _INCLUSION_GUARD_R_COMMUNICATION_SERVICE_H_
#define _INCLUSION_GUARD_R_COMMUNICATION_SERVICE_H_
/**
 * @addtogroup ldr_communication_module Loader communication module
 * The Loader Communication module is introduced with the intention of sharing
 * code and bridging the gap between Loaders and the tools using Loaders.
 * By having a module that exposes an interface to both Loaders and tools and
 * this interface are generated from a common specification, we can more easily
 * change and version control the interface.
 *
 * @{
 *
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "c_system.h"
#include "error_codes.h"
#include "t_communication_service.h"

#ifndef CFG_ENABLE_LOADER_TYPE

#ifdef LCM_EXPORTS
#define LCM_API __declspec(dllexport) // export DLL information
#else //LCM_EXPORTS
#define LCM_API __declspec(dllimport) // import DLL information
#endif // LCM_EXPORTS

#else

#define LCM_API

#endif  // CFG_ENABLE_LOADER_TYPE

#define QUEUE(x,y) x->Functions_p->QueueFunctions_p->y
#define TIMER(x,y) x->Functions_p->TimerFunctions_p->y
#define BUFFER(x,y) x->Functions_p->BufferFunctions_p->y
#define OBJECT_BUFFER(x) x->Functions_p->BufferFunctions_p->Object_p
#define OBJECT_TIMER(x) x->Functions_p->TimerFunctions_p->Object_p
#define OBJECT_QUEUE(x) x->Functions_p->QueueFunctions_p->Object_p
#define OBJECT_HASH(x) x->HashDevice_p->Object_p
#define OBJECT_CEH_CALL(x) x->Object_p

extern Communication_t GlobalCommunication;
#ifndef CFG_ENABLE_COMMUNICATION_SINGLETON
extern Communication_t GlobalCommunication2;
#endif

extern Communication_t DebugCommunication;

/*******************************************************************************
 * Declaration of functions
 ******************************************************************************/

#ifdef __cplusplus
extern "C"
{
#endif // #ifdef __cplusplus
    /**
     * Initialize an instance of the communication module.
     *
     * Given the initial family, already initialized hash device descriptor
     * and communication device descriptor, a new instance of the
     * communication module is created and bound to use the provided devices.
     *
     * @param [in,out] Communication_p       Communication module context.
     * @param [in]     Family                Initial protocol family.
     * @param [in]     HashDevice_p          The device to use for checksum
     *                                       calculations and verifications.
     * @param [in]    *CommunicationDevice_p The device to use for network
     *                                       transmission.
     * @param [in]     CommandCallback_p     Collback function for command handling.
     *
     * @retval  E_SUCCESS If the module instance is successfully
     *                    initialized. A protocol family dependant error
     *                    code otherwise.
     */
#ifdef CFG_ENABLE_LOADER_TYPE
    LCM_API ErrorCode_e Do_Communication_Initialize(Communication_t *Communication_p, Family_t Family, HashDevice_t *HashDevice_p, CommunicationDevice_t *CommunicationDevice_p, Do_CEH_Call_t CommandCallback_p);
#else
    LCM_API ErrorCode_e Do_Communication_Initialize(void *Object_p, Communication_t **Communication_pp, Family_t Family, HashDevice_t *HashDevice_p, CommunicationDevice_t *CommunicationDevice_p, Do_CEH_Call_t CommandCallback_p, BuffersInterface_t *Buffers_p, TimersInterface_t *Timers_p, QueueInterface_t *Queue_p);
#endif  // #ifdef CFG_ENABLE_LOADER_TYPE

    /**
     * Shutdown an instance of the communication module.
     *
     * Shutdown the initialized protocol family, cancel current transmission and
     * receiving data thru the communication device and shutdown the transport and
     * network layer.
     *
     * @param [in,out] Communication_p    Communication module context.
     *
     * @retval  E_SUCCESS If the module instance is successfully
     *                    shut down. A protocol family dependant error
     *                    code otherwise.
     */
    LCM_API ErrorCode_e Do_Communication_Shutdown(Communication_t **Communication_pp);

    /**
     * Poll the network for inbound packets.
     *
     * When a packet is received it is put on the inbound queue. This
     * function should be called whenever there is time to handle a new
     * packet. It passes all the elements in the queue to the family
     * specific polling function, one by one.
     *
     * @param [in] Communication_p  Communication module context.
     *
     *
     * @retval  E_SUCCESS If all packets are successfully handled. A
     *                    protocol family dependant error code otherwise.
     */
    LCM_API ErrorCode_e Do_Communication_Poll(Communication_t *Communication_p);

    /**
     * Change the current protocol family.
     *
     * Shuts down the old protocol family and initializes the new one.
     *
     * @param [in] Communication_p  Communication module context.
     * @param [in] Family           New protocol family identifier.
     * @param [in] CEHCallback      Callback that will handle the commands.
     *
     * @retval  E_SUCCESS If the family is successfully changed. A
     *                    protocol family dependant error code otherwise.
     */
    LCM_API ErrorCode_e Do_Communication_SetFamily(Communication_t *Communication_p, Family_t Family, Do_CEH_Call_t CEHCallback);

    /**
     * Function for sending packet.
     *
     * @param [in]  Communication_p Communication module context.
     * @param [in]  InputData_p     Pointer to the data for tranmission.
     *
     * @retval E_SUCCESS                        After successful execution.
     * @retval E_FAILED_TO_ALLOCATE_COMM_BUFFER Failed to allocate communication
     *                                          buffer.
     */
    LCM_API ErrorCode_e Do_Communication_Send(Communication_t *Communication_p, void *InputData_p);

    /**
     * Function for setting the timeouts in the protocols.
     *
     * Timeouts that should be set depend of the protocol family. Some protocol
     * family do not have timeouts.
     *
     * @param [in]  Communication_p Communication module context.
     * @param [in]  TimeoutData_p   Pointer to the data with all timeouts.
     *
     * @retval E_SUCCESS            After successful execution.
     */
    LCM_API ErrorCode_e Do_Communication_SetProtocolTimeouts(Communication_t *Communication_p, void *TimeoutData_p);

    /**
     * Function for getting the timeouts in the protocols.
     *
     * Timeouts that should be get depend of the protocol family. Some protocol
     * family do not have timeouts.
     *
     * @param [in]  Communication_p  Communication module context.
     * @param [out]  TimeoutData_pp  Return data with all timeouts.
     *
     * @retval E_SUCCESS             After successful execution.
     */
    LCM_API ErrorCode_e Do_Communication_GetProtocolTimeouts(Communication_t *Communication_p, void **TimeoutData_pp);

#ifdef __cplusplus
};
#endif // #ifdef __cplusplus

/**
 *
 */
void Do_CommunicationInternalErrorHandler(const ErrorCode_e IntError);

/** @} */
#endif // _INCLUSION_GUARD_R_COMMUNICATION_SERVICE_H_
