/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
 
/*!
 * \brief Component Manager Communication Type.
 */

#ifndef PROXY_COMMUNICATION_INTERNAL_H
#define PROXY_COMMUNICATION_INTERNAL_H

#include <nmf/inc/channel_type.h>
#include <cm/proxy/inc/osal.h>
#include <cm/proxy/common/utils/inc/list.h>
#include <cm/engine/api/channel_engine.h>

/*!
 * \brief Structure used to store a service callback and its context
 *
 * There is one list of service callback per channel.
 * A same duo (service-callback, context) can be registered on several channels:
 * In this case, the entry is duplicated in each channel list. (Later, this entry
 * could be shared through each channel).
 * 'count' is an "usage count": when equal to zero, the entry can be freed.
 *
 * \ingroup CM_INTERNAL_API
 */
typedef struct {
	t_uint32              count;
	t_nmf_serviceCallback service;
	void *                serviceCtx;
	t_list_entry          this_entry;
} t_service_entry;

/*!
 * \brief There is one such entry per opened channel.
 *
 * There is a list of service-callback per channel.
 * 'count' is an "usage count": when equal to zero, the entry can be freed.
 *
 * \ingroup CM_INTERNAL_API
 */
typedef struct {
	t_uint32               count;
	t_os_channel           channel;
	t_list_entry           service_list;
	t_nmf_osal_sync_handle service_lock;
	char                   msg[1024];
} t_channel_entry;

extern t_channel_entry *shared_channel;

/*!
 * \brief Very early init of the CM Proxy
 *
 * The purpose of the function is to:
 * - initialize the CM User Proxy library.
 *
 * It must be called once, before the main() function of the user program is called
 *
 * \ingroup CM_INTERNAL_API
 */
PUBLIC void CM_INTERNAL_ProxyInit(void);

/*!
 * \brief Destroy of the CM Proxy
 *
 * The purpose of the function is to:
 * - destroy the cery last ressources used in the CM User Proxy library.
 *
 * It must be called once, at exit() of the user program is called
 *
 * \ingroup CM_INTERNAL_API
 */
PUBLIC void CM_INTERNAL_ProxyDestroy(void);

/*!
 * \brief Initialize the CM Proxy
 *
 * The purpose of the function is to:
 * - initialize the CM User Proxy.
 *
 * It must be called once, by the OS Integrator, at the first \ref CM_CreateChannel() call.
 * It's up to the OS Integrator to manage concurrent access to this call.
 *
 * \warning It MUST be called only once, unless CM_INTERNAL_Destroy() has been called before.
 *
 * \exception CM_NO_MORE_MEMORY Not enough memory to create the callback Channel.
 * \exception CM_INTEGRATION_ERROR OS specific error.
 *
 * \ingroup CM_INTERNAL_API
 */
PUBLIC t_nmf_error CM_INTERNAL_Init(void);

/*!
 * \brief Destroy the CM Proxy
 *
 * The purpose of the function is to:
 * - destroy all ressources allocated by the CM User Proxy.
 *
 * It must be called, once, by the OS Integrator at the last \ref CM_CloseChannel() call.
 * It's up to the OS Integrator to manage concurrent access to this call.
 *
 * \warning It MUST be called only once, if a CM_INTERNAL_Init() has been called before, once all
 *          previously allocated ressources have been freed
 *
 * \ingroup CM_INTERNAL_API
 */
PUBLIC void CM_INTERNAL_Destroy(void);

/*!
 * \brief Create/get a channel.
 *
 * The purpose of the function is to create a channel or get a channel a channel entry
 * in the common User Proxy Part, regarding the flag parameter
 * This function must be called by \ref CM_CreateChannel()
 *
 * \param[in]  flags   Whether the caller want to create a new channel (\ref NMF_CHANNEL_PRIVATE)
 *                     or use the shared one (\ref NMF_CHANNEL_SHARED) (it will be created
 *                     if it does not yet exist)
 * \param[out] channel Channel number.
 * \param[out] OSchannel If *OSchannel is not NULL, the OS Integrator has to create an os_channel and set **OSchannel.
 *
 * \exception CM_NO_MORE_MEMORY Not enough memory to create the callback Channel.
 * \exception CM_INVALID_PARAMETER The specified flags is invalid.
 * \exception CM_INTEGRATION_ERROR OS specific error.
 *
 * \ingroup CM_INTERNAL_API
 */
PUBLIC t_nmf_error CM_INTERNAL_CreateChannel(t_nmf_channel_flag flags,
					     t_nmf_channel *channel,
					     t_os_channel **OSchannel);

/*!
 * \brief Flush a channel.
 *
 * The purpose of this function is to return OSchannel need by OS Integration. In case of shared channel if
 * reference counter will not be zero on next close then we return 0 in OSchannel, in that case OS integration
 * must not generate CM_FLUSH_MESSAGE.
 *
 * \param[in]  channel    Channel number:
 * \param[out] OSchannel  Is *OSchannel is not NULL, the OS integrator has to generate a CM_FLUSH_MESSAGE error in CM_GetMessage().
 *
 * \ingroup CM_INTERNAL_API
 */
PUBLIC t_nmf_error CM_INTERNAL_FlushChannel(t_nmf_channel channel,
					    t_os_channel *OSchannel);

/*!
 * \brief Close a channel
 *
 * The purpose of the function is to destroy or clear a reference to a channel from the user to the CM engine
 * in the common User Proxy Part. This function must be called by \ref CM_CloseChannel()
 *
 *
 *
 * \param[in]  channel    Channel number:
 * \param[out] pOSchannel If successful, *pOSchannel is filled. If *pOSchannel is not 0, the OS Integration have to destroy the channel pointed by pOSchannel
 *
 * \exception CM_INVALID_PARAMETER The specified channel is invalid or some callback are still registered.
 *
 * \ingroup CM_INTERNAL_API
 */
PUBLIC t_nmf_error CM_INTERNAL_CloseChannel(t_nmf_channel channel,
					    t_os_channel *pOSchannel);
#endif /* PROXY_COMMUNICATION_INTERNAL_H */
