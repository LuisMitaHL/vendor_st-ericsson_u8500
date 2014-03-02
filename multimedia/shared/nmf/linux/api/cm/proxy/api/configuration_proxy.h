/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
 
/*!
 * \brief Component Manager Configuration API.
*/
/*!
 * \defgroup CM_CONFIGURATION_API CM Proxy Configuration API
 * \ingroup CM_PROXY_MODULE
 */

#ifndef COMMON_CONFIGURATION_WRAPPER_H
#define COMMON_CONFIGURATION_WRAPPER_H

#include <nmf/inc/channel_type.h>
#include <nmf/inc/service_type.h>
#include <cm/engine/configuration/inc/configuration_type.h>
#include <cm/engine/communication/inc/communication_type.h>

/*!
 * \brief Create/get a channel.
 *
 * The purpose of the function is to
 * create a channel or get a channel, regarding the flag parameter
 *
 * \param[in]  flags   Whether the caller want to create a new channel (\ref NMF_CHANNEL_PRIVATE)
 *                     or use the shared one (\ref NMF_CHANNEL_SHARED) (it will be created
 *                     if it does not yet exist)
 * \param[out] channel Channel number.
 *
 * \exception CM_NO_MORE_MEMORY Not enough memory to create the callback Channel.
 * \exception CM_INVALID_PARAMETER The specified flags is invalid.
 * \exception CM_INTEGRATION_ERROR OS specific error.
 *
 * \ingroup CM_CONFIGURATION_API
 */
PUBLIC IMPORT_SHARED t_nmf_error CM_CreateChannel(t_nmf_channel_flag flags,
						  t_nmf_channel *channel);

/*!
 * \brief Flush a channel to allow user to safely close it.
 *
 * The purpose of the function is to allow safe call of CM_CloseChannel() later on. Calling
 * CM_FlushChannel() will allow a blocking call to CM_GetMessage() to exit with an error
 * CM_FLUSH_MESSAGE. After CM_GetMessage() has exit with such a value user must no more
 * call CM_GetMessage() and can safely call CM_CloseChannel() that will destroy channel.
 * In case of the share channel CM_FlushChannel() will return false for isFlushMessageGenerated if
 * it's internal reference counter is not zero, in that case no CM_FLUSH_MESSAGE error is return 
 * by CM_GetMessage() and user can immediatly call CM_CloseChannel().
 * In case user know that no usage of channel is done when he want to destroy channel, call to this api
 * is optionnal and user can safely call CM_CloseChannel().
 *
 * \param[in]  channel                  Channel number
 * \param[out] isFlushMessageGenerated  Allow user to know if it must wait for CM_FLUSH_MESSAGE return
 *                                      of CM_GetMessage() before calling CM_CloseChannel()
 *
 * \exception NMF_INVALID_PARAMETER The specified flags is invalid.
 *
 * \ingroup CM_CONFIGURATION_API
 */
PUBLIC IMPORT_SHARED t_nmf_error CM_FlushChannel(t_nmf_channel channel, t_bool *isFlushMessageGenerated);

/*!
 * \brief Close a channel
 *
 * The purpose of the function is to
 * destroy a channel from the user to the CM engine.
 *
 * The user must call CM_CloseChannel() as many time as \ref CM_CreateChannel().
 * At the last CM_CloseChannel() call, the channel is closed and definitely destroyed.
 * All service callback must be unregistered first.
 *
 * \param[in] channel    Channel number:
 *
 * \exception CM_INVALID_PARAMETER The specified channel is invalid or some callback are still registered.
 *
 * \ingroup CM_CONFIGURATION_API
 */
PUBLIC IMPORT_SHARED t_nmf_error CM_CloseChannel(t_nmf_channel channel);

/*!
 * \brief Register a service callback to this channel.
 *
 * \param[in] channel The channel on which the callback must be registered.
 * \param[in] handler The given callback.
 * \param[in] ctx     The context associated with this callback (the one passed to the handler).
 *
 * The handler must be a function with the following declaration
 * \code
 *    void service(void *contextHandler, t_nmf_service_type serviceType, t_nmf_service_data *serviceData)
 *      {
 *	switch (servicetype) {
 *	case NMF_SERVICE_PANIC:
 *	        t_nmf_panic_data *panic = &serviceData->panic;
 *               ...
 *		break;
 *	default:
 *		break;
 *	}
 *
 *     }
 * \endcode
 *
 * This 'service' function will be called from \ref CM_ExecuteMessage() for each incoming 'service messages'
 * - The 'contextHandler' passed is the context given when registering this service callback
 * - 'serviceType' specifies the service
 *
 * \exception CM_INVALID_PARAMETER The channel doesn't exist or the callback is NULL
 * \exception CM_NO_MORE_MEMORY Not enough memory to associate service with the Channel.
 *
 * \ingroup CM_CONFIGURATION_API
 */
PUBLIC IMPORT_SHARED t_nmf_error CM_RegisterService(t_nmf_channel channel,
						    t_nmf_serviceCallback handler,
						    void *ctx);

/*!
 * \brief Unregister a service callback from this channel.
 *
 * \param[in] channel The channel on which the callback must be registered.
 * \param[in] handler The given callback.
 * \param[in] ctx     The context associated with this callback.
 *
 * \exception CM_INVALID_PARAMETER The channel or the callback doesn't exist.
 *
 * \ingroup CM_CONFIGURATION_API
 */
PUBLIC IMPORT_SHARED t_nmf_error CM_UnregisterService(t_nmf_channel channel,
						      t_nmf_serviceCallback handler,
						      void *ctx);

/*!
 * \brief Set the mode of the Component Manager engine.
 *
 * According the (\ref t_cm_cmd_id) value, this routine allows to modify dynamically the behavior of the CM-engine.
 *
 * \param[in] aCmdID Command ID.
 * \param[in] aParam Parameter of command ID if required.
 *
 * \ingroup CM_CONFIGURATION_API
 */

PUBLIC IMPORT_SHARED t_cm_error CM_SetMode(t_cm_cmd_id aCmdID, t_sint32 aParam);

PUBLIC IMPORT_SHARED const char * CM_StringError(t_nmf_error error);
#endif // COMMON_CONFIGURATION_WRAPPER_H

