/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
 
/*!
 * \brief CM Performance Meter API.
 *
 * This file contains the Component Manager Performance Meter API.
 */
#ifndef COMMON_PERFMETER_WRAPPER_H
#define COMMON_PERFMETER_WRAPPER_H

#include <cm/engine/perfmeter/inc/perfmeter_type.h>

/*!
 * \brief MPC cpu load
 *
 * This routine will return counters that allow to compute mpc load.
 *
 * \param[in] coreId identification of mpc from which we want cpu load
 * \param[out] mpcLoadCounter will contain mpc cpu load counters value if success
 *
 * \exception CM_INVALID_PARAMETER provide coreId is not a valid mpc coreId
 *
 * \ingroup PERFMETER
 */

PUBLIC IMPORT_SHARED t_cm_error CM_getMpcLoadCounter(
        t_nmf_core_id coreId,
        t_cm_mpc_load_counter *mpcLoadCounter);


#endif /* COMMON_PERFMETER_WRAPPER_H */
