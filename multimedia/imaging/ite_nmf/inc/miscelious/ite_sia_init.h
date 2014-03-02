/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */


#ifndef __INC_ITE_SIA_INIT_H
#define __INC_ITE_SIA_INIT_H

//For Linux
//#include "MMIO_Camera.h"


#undef EXTERN_DEF
#ifdef ITE_SIA_INIT_C_
#define EXTERN_DEF
#else
#define EXTERN_DEF extern
#endif


#if !( defined(__ARM_LINUX) || defined(__ARM_SYMBIAN) || defined(__PEPS8500_SIA) || defined(_SVP_)) 
#include "mmte_b2r2_driver_8500.h"

EXTERN_DEF ts_b2r2_config g_B2R2_node ;

#endif


#define PMU_SWCR                 0x0030
 
/*--------------------------------------------------------------------------*
 * NMF Defines                                                              *
 *--------------------------------------------------------------------------*/
/* Define size of the communication fifo */
#define FIFO_COMS_SIZE  16

/* Define size of interace collection for grab components */
#define DD_INSTANCE_NUMBER 16

/* Define adapters registration */
#define CM_REGISTER_SIA_ADAPTER CM_REGISTER_STUBS_SKELS(imaging);

/* Define linked to component type name */
#define SIA_RESOURCE_MANAGER_COMPONENT_TYPE_NAME_STR   "sia_rm"
#define GRAB_COMPONENT_TYPE_NAME_STR "grab"
#define ISPCTL_COMPONENT_TYPE_NAME_STR "ispctl"

/*Wait on NMF event: reset also global at end*/
#define WAIT_EVENT(event) \
do{                       \
}while (event == FALSE)

#define RESET_EVENT(event) \
event = FALSE


#ifdef INFINITY
#undef INFINITY
#endif
#define INFINITY 255


#ifdef __cplusplus
extern "C"
{
#endif

/*--------------------------------------------------------------------------*
 * Functions                                                                *
 *--------------------------------------------------------------------------*/
int ITE_register_sia_components( void );
int ITE_unregister_sia_components( void );
void ITE_enableSiaClocks(void);
void ITE_SiaEnable(void);

#ifdef __cplusplus
}
#endif

#endif /* __INC_ITE_SIA_INIT_H */
