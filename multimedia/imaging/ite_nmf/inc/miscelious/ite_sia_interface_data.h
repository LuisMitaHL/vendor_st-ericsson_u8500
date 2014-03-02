/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */


#ifndef __INC_ITE_SIA_INTERFACE_DATA_H
#define __INC_ITE_SIA_INTERFACE_DATA_H


#include "ite_sia_interface.h"

/* ----------- SIA ------------------------------------------*/
/* Inputs of the grab component */
extern Igrab_api_cmd grabCommand;
extern Iapi_set_debug grabDebug;
/* Output of the grab component */
extern CBgrab_api_alert grabAlert[];

/* Inputs of the ispctl component */
extern Iapi_set_debug ispctlDebug;
extern Iispctl_api_cfg ispctlConfigure;
extern Iispctl_api_cmd ispctlCommand;
/* Output of the ispctl component */
extern CBispctl_api_alert ispctlAlert;

/* Component handles */
extern t_cm_instance_handle sia_adapter_handle;
extern t_cm_instance_handle sia_resource_manager_handle;
extern t_cm_instance_handle ispctl_handle;
extern t_cm_instance_handle grab_handle;


#endif /* __INC_ITE_SIA_INTERFACE_DATA_H */
