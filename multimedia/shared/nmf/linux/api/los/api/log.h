/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
 
#ifndef __INC_LOS_LOG_H
#define __INC_LOS_LOG_H

/*!
 * \defgroup LOS_LOG LOS Log API
 */

#include <inc/typedef.h>

/*!
 * \brief LOS_Log write any message on the debugger output window. (printf like)
 *
 * \param[in] format message (string) to write, including some parameters like \%d, \%s, \%x, etc.
 * \param[in] ... parameters
 *
 * \see <tt>man -S3 printf</tt> for more information about format...
 *
 * \ingroup LOS_LOG
 * */
PUBLIC IMPORT_SHARED void LOS_Log(
    const char *format, ...);

/*!
 * \brief LOS_Log_Err write any message on the debugger output window. (printf like)
 *
 * \param[in] format message (string) to write, including some parameters like \%d, \%s, \%x, etc.
 * \param[in] ... parameters
 *
 * \see <tt>man -S3 printf</tt> for more information about format...
 *
 * \ingroup LOS_LOG
 * */
PUBLIC IMPORT_SHARED void LOS_Log_Err(
    const char *format, ...);

/*!
 * \brief LOS_Scanf read any message on the debugger output window. (scanf like)
 *
 * \param[in] format message (string) to read, including some parameters like \%d, \%s, \%x, etc.
 * \param[in] ... parameters
 *
 * \see <tt>man -S3 scanf</tt> for more information about format...
 *
 * \ingroup LOS_LOG
 * */
PUBLIC IMPORT_SHARED void LOS_Scanf(
    const char *format, ...);

#endif /* __INC_LOS_LOG_H */
