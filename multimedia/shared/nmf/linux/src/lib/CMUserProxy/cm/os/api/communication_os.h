/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
 
/*!
 * \brief Communication User OS API.
 *
 * This file contains the Communication OS API for manipulating components.
 *
 */
#ifndef COMMUNICATION_OS_H_
#define COMMUNICATION_OS_H_

#include <cm/engine/api/channel_engine.h>
#include <cm/engine/communication/inc/communication_type.h>

/*!
 * \brief Allocate Event buffer where parameters will be marshalled.
 *
 * In order to optimize call, this method don't need to be exported to user space,
 * but must be used by CM driver.
 *
 * See \ref HOST2MPC "Host->MPC binding" for seeing an integration example.
 *
 * \note This method is not called from user space!!!
 *
 * \ingroup CM_OS_API
 */
PUBLIC t_event_params_handle CM_OS_AllocEvent(t_cm_bf_host2mpc_handle host2mpcId);

/*!
 * \brief Push a event in Fifo.
 *
 * In order to optimize call, this method don't need to be exported to user space,
 * but must be used by CM driver.
 *
 * See \ref HOST2MPC "Host->MPC binding" for seeing an integration example.
 *
 * \note This method is not called from user space!!!
 *
 * \ingroup CM_OS_API
 */
PUBLIC t_cm_error CM_OS_PushEvent(t_cm_bf_host2mpc_handle host2mpcId, t_event_params_handle h, t_uint32 methodIndex);

/*!
 * \brief Push a event in Fifo.
 *
 * In order to optimize call, this method need to be exported to user space
 * and must be implemented by CM driver.
 *
 * See \ref HOST2MPC "Host->MPC binding" for seeing an integration example.
 *
 * \note No implementation of this method is provided in kernel CM engine!!!
 *
 * \ingroup CM_OS_API
 */
PUBLIC t_cm_error CM_OS_PushEventWithSize(t_cm_bf_host2mpc_handle host2mpcId, t_event_params_handle h, t_uint32 size, t_uint32 methodIndex);

/*!
 * \brief Aknowledge a Fifo that the received event has been demarshalled.
 *
 * In order to optimize call, this method don't need to be exported to user space,
 * but must be used by CM driver.
 *
 * See \ref MPC2HOST "MPC->Host binding" for seeing an integration example.
 *
 * \note This method is not called from user space!!!
 *
 * \ingroup CM_OS_API
 */

PUBLIC void CM_OS_AcknowledgeEvent(t_cm_bf_mpc2host_handle mpc2hostId);

/*!
 * \brief Get received message from specified fifo.
 *
 * \ingroup CM_OS_API
 */
PUBLIC t_cm_error CM_OS_GetMessage(
        t_os_channel                channel,
        t_os_message *              message,
        t_uint32                    messagesize,
        t_bool                      block);

#endif /*COMMUNICATION_OS_H_*/
