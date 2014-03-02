/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
 
#ifndef __INC_LOS_TIME_H
#define __INC_LOS_TIME_H
/*!
 * \defgroup LOS_TIME LOS Time API
 */


/*!
 * \brief Get the system time and returns it in microsecond
 *
 * The implementation of this function is owned by the OS implementation choice.
 *
 *
 * \return time (unsigned long long) in microsecond since the beggining of the OS boot
 *
 * \ingroup LOS_TIME
 * */
IMPORT_SHARED unsigned long long LOS_getSystemTime(void);

#endif
