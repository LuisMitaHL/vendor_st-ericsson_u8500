/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */


/* SIA FW NMF interfaces (component entry points) */
#include "ite_sia_interface_data.h"


/* --------------- SIA related -------------------------------------------------------------------------------*/

/* Inputs of the grab component */
Igrab_api_cmd grabCommand;
Iapi_set_debug grabDebug;
/* Output of the grab component */
extern CBgrab_api_alert grabAlert[];

/* Inputs of the ispctl component */
Iapi_set_debug ispctlDebug;
Iispctl_api_cfg ispctlConfigure;
Iispctl_api_cmd ispctlCommand;
/* Output of the ispctl component */
extern CBispctl_api_alert ispctlAlert;

/* Inputs of the sia rm component */
/* Output of the ispctl component */

/* Component handles */

t_cm_instance_handle sia_adapter_handle;
t_cm_instance_handle sia_resource_manager_handle;
t_cm_instance_handle ispctl_handle;
t_cm_instance_handle grab_handle;

