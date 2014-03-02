/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
 
/*!
 * \brief CM Performance Meter OS API.
 *
 * This file contains the Component Manager Performance Meter OS API.
 */
#ifndef CM_ENGINE_PERFMETER_OS_H_
#define CM_ENGINE_PERFMETER_OS_H_

#include <cm/engine/perfmeter/inc/perfmeter_type.h>

/*!
 * \brief MPC cpu load
 *
 * \param[in] coreId identification of mpc from which we want cpu load
 * \param[out] mpcLoadCounter will contain mpc cpu load counters value if success
 *
 * \ingroup CM_OS_API
 */
PUBLIC t_cm_error CM_OS_getMpcLoadCounter(
        t_nmf_core_id coreId,
        t_cm_mpc_load_counter *mpcLoadCounter);

#endif /*CM_ENGINE_PERFMETER_OS_H_*/
