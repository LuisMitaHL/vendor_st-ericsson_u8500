/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
 
/* rtos/itdispatcher/src/dispatcher.c - EE.
 *
 * Copyright (c) 2006, 2007, 2008 STMicroelectronics.
 *
 * Reproduction and Communication of this document is strictly prohibited
 * unless specifically authorized in writing by STMicroelectronics.
 * 
 * Written by NMF team.
 */
#include <rtos/itdispatcher.nmf>

#include <archi.h>
#include <rtos/common/inc/stub-requiredapi.h>

#pragma inline
static void jumpIT(Irtos_itdispatcher_handler *pHandle)
{
    void *oldTHIS = THIS;
    THIS = pHandle->THIS;
    pHandle->Execute();
    THIS = oldTHIS;
}

#define IT(n) _INTERRUPT void IT##n(void) { jumpIT(&it##n); }

IT(1)
IT(2)
IT(3)
IT(4)
IT(5)
IT(6)
IT(7)
IT(8)
IT(9)
IT(10)
//IT(11) Not provided here, already used by NMF communication mechanisms
IT(12)
IT(13)
IT(14)
IT(15)
IT(16)
IT(17)
IT(18)
IT(19)
IT(20)
IT(21)
IT(22)
IT(23)
IT(24)
IT(25)
IT(26)
IT(27)
IT(28)
IT(29)
IT(30)
IT(31)
