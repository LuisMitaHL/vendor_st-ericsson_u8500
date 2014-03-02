/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
 
/*!
 * \brief Configuration Component Manager User OS API.
 *
 * This file contains the Configuration CM OS API for manipulating CM.
 *
 */

#ifndef CONFIGURATION_OS_H
#define CONFIGURATION_OS_H

#include <cm/engine/configuration/inc/configuration_type.h>

/*!
 * \brief Dynamically set some debug parameters of the CM
 *
 * \param[in] aCmdID The command for the parameter to update
 * \param[in] aParam The actual value to set for the given command
 *
 * \ingroup CM_OS_API
 */
PUBLIC t_cm_error CM_OS_SetMode(t_cm_cmd_id aCmdID, t_sint32 aParam);

#endif /* CONFIGURATION_OS_H */
