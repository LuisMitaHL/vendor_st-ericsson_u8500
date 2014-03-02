/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
#ifndef __mmte_nmf_h_
#define __mmte_nmf_h_

#ifndef __SYMBIAN32__
#define IMPORT_C
#define EXPORT_C
#endif



#ifdef __cplusplus
extern "C"
{
#endif


    /*-----------------------------------------*/
    /* INCLUDES           					   */
    /*-----------------------------------------*/
#ifndef HOST_ONLY
#include "cm/inc/cm.h"
#endif // HOST_ONLY

#include "cli.h"

#include "los/api/los_api.h"

#ifdef NMF_XTI
#include "xti.h"
#endif
    
#ifndef HOST_ONLY
#include "cm/engine/configuration/inc/configuration_type.h"
#endif

    /*-----------------------------------------*/
    /*  Exported Function prototypes           */
    /*-----------------------------------------*/
    IMPORT_C void   mmte_nmf_init(void);
    IMPORT_C void   mmte_nmf_terminate(void);
    IMPORT_C void   mmte_nmf_settracelevel(int level);
    
    /*-----------------------------------------*/
    /*  Internal Function prototypes           */
    /*-----------------------------------------*/
    void    nmf_memory_status_init  (void);
    void    nmf_memory_status_print (void);
    void    nmf_network_print       (t_bool all_components);
    t_bool  nmf_memory_status_check (void);

#if defined(__ARM_LINUX)
    extern t_nmf_channel nmfSharedChannel;
#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif

