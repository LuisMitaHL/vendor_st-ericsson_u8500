/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */


#ifndef _ITE_VPIP_H
#define _ITE_VPIP_H



#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <inc/type.h>
#include "grab_types.idt.h"

#include "ite_sensorinfo.h"
#include "ite_boardinfo.h"
#include "ite_pipeinfo.h"

#include "ispctl_types.idt.h"

#ifdef __cplusplus
extern "C"
{
#endif

typedef enum
{
    FrameRateMode_Manual,
    FrameRateMode_Auto
} FrameRateMode_e;


void ITE_Log_RW_pageelements_enable(void);
void ITE_Log_RW_pageelements_disable(void);
void ITE_InitMMDSPTimer(void);
void ITE_writeListPE(ts_PageElement tab_pe[], t_uint16);

t_uint32 ITE_readPE(t_uint16);
float ITE_float_readPE(t_uint16 addr);
t_uint32 ITE_WaitExpectedPeValue(t_uint16 addr, t_uint32 value_expected, t_uint32 polling_frequency, t_uint32 timeout);
void ITE_writePE(t_uint16 , t_uint32 );

int ITE_SendCommands(t_uint32 data, enum e_ispctlInfo trigEvent);
int ITE_Do_SmiaPP_PowerUp_Sequence();
int ITE_Do_SmiaPP_PowerDown_Sequence();
int ITE_Do_SENSOR_TUNNING_Sequence_Before_Boot(t_uint32 data, enum e_ispctlInfo trigEvent);
int ITE_DoBootSequence(int smiapp_power_sequence, int sensor_tunning_before_boot);
t_uint16 ITE_readListPE(ts_PageElement tab_pe[], t_uint16 number_of_pe);

int STOPVPIP(void);
int STOPVPIP_BML(void);
int STARTVPIP(void);
int STARTVPIP_BML(void);
int PREPAREVPIP(void);
int SLEEPVPIP(void);
int WAKEUPVPIP(void);


int STARTVPIPMULTIFRAME(enum e_grabPipeID pipe,int nb_frame);
int ITE_IsVPIPRunning(void);
int ITE_IsVPIPStopped(void);



#ifdef __cplusplus
}
#endif

#endif   /* _ITE_VPIP_H */
