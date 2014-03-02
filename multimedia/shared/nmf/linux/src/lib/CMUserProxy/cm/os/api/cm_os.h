/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
 
/*!
 * \brief CM Operating System API.
 *
 * This file contains the Component Manager Operating System API.
 */

/*!
 * \defgroup CM_OS_MODULE CM Driver
 */

/*!
 * \defgroup CM_OS_API CM Operating System API (a.ka. CM Driver API)
 *
 * \note This API is not for user developers, this API is only an internal API.
 *
 * \warning All parameters in out from this API means that the parameter is a reference to a data that is complete by the call.
 *
 * This API is required by CM Proxy to call CM service and shall be implemented in driver user part.
 *
 * This API must be implemented by NMF OS integrator which must at the end call CM ENGINE API if existing.
 * If kernel space can't access user space, a copying to temporary data must be implement in driver.
 * \ingroup CM_OS_MODULE
 */

#include <cm/os/api/configuration_os.h>

#include <cm/os/api/domain_os.h>

#include <cm/os/api/component_os.h>

#include <cm/os/api/memory_os.h>

#include <cm/os/api/migration_os.h>

#include <cm/os/api/communication_os.h>

#include <cm/os/api/perfmeter_os.h>

#include <cm/os/api/executive_engine_mgt_os.h>

#include <cm/os/api/repository_mgt_os.h>
