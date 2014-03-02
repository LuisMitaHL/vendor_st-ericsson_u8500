/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
 
/*!
 * \brief CM executive engine management Engine API.
 *
 * This file contains the Component Manager executive engine management Engine API.
 */
#ifndef CM_EXECUTIVE_ENGINE_MANAGEMENT_OS_H_
#define CM_EXECUTIVE_ENGINE_MANAGEMENT_OS_H_

#include <cm/inc/cm_type.h>

/*!
 * \brief Return executive engine handle for given core
 *
 * \param[in]  domainId The domain identifier for which we want executive engine handle.
 * \param[out] executiveEngineHandle executive engine instance (null if the executive engine is not loaded)
 *
 * \ingroup CM_OS_API
 */
PUBLIC t_cm_error CM_OS_GetExecutiveEngineHandle(
        t_cm_domain_id        domainId,
        t_cm_instance_handle *executiveEngineHandle);

#endif /*CM_EXECUTIVE_ENGINE_MANAGEMENT_OS_H_*/
