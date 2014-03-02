/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
#ifndef ITE_PLATFORM_H_
#   define ITE_PLATFORM_H_

#   if !(defined(__PEPS8500_SIA) || defined(_SVP_))

//For Linux
//#include "MMIO_Camera.h"
#   endif
#   include <inc/type.h>

#   include "ite_init.h"
#   include "ite_sia_init.h"
#   include "ite_main.h"

#   ifdef __cplusplus
extern "C"
{
#   endif
Result_te        ITE_InitBoard (void);
Result_te        ITE_InitPrimary (void);
Result_te        ITE_InitSecondary (void);
Result_te        ITE_DeInitPrimary (void);
Result_te        ITE_DeInitSecondary (void);
Result_te        ITE_PlatformMmioTest (int choice);
Result_te        DoPowerAction (t_uint32 u32_power_command);

int         IspRegWrite_Wrapper (unsigned long t1_dest, const long int *logical_addr, int count);
int         IspRegRead_Wrapper (unsigned long t1_dest, const long int *logical_addr, int count);
t_uint32    IspRegRead_Value (unsigned long t1_dest, const long int *logical_addr, int count);

#   ifdef __cplusplus
}


#   endif
#endif /* ITE_PLATFORM_H_*/

