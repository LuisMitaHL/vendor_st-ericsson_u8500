/* 
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved. 
 * This code is ST-Ericsson proprietary and confidential. 
 * Any use of the code for whatever purpose is subject to 
 * specific written permission of ST-Ericsson SA. 
 */ 
 
/**
 \defgroup  SleepManager Sleep Manager

 \details   This module performs all the tasks that are required
            to be done in context of a SLEEP command and WAKE command
            to the Host Interface.

*/
#ifndef SLEEPMANAGER_H_
#   define SLEEPMANAGER_H_

/**
 \if INCLUDE_IN_HTML_ONLY
 \file      SleepManager.h

 \brief     This file exposes an interface to allow other modules of the device
            to access the Sleep Manager module.

 \ingroup   SleepManager
 \endif
*/
extern void SleepManager_Sleep (void);
extern void SleepManager_Wakeup (void);
#endif /*SLEEPMANAGER_H_*/

