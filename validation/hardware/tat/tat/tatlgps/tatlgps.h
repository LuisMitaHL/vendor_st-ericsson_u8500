/*****************************************************************************/
/**
*  ST-Ericsson, 2011 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \brief   This module provides services for power services module
* \author  ST-Ericsson
*/
/*****************************************************************************/

#ifndef TATLGPS_H_
#define TATLGPS_H_

/* STANDARD INCLUDES */
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdint.h>
#include <math.h>

/* TAT INCLUDES */
#include <dthsrvhelper/dthsrvhelper.h>

/* MODEM INCLUDES */
#include "type_def_wm_isi_if.h"
#include "isi_common_lib.h"
#include "tatmodemlib.h"

/* TAT GPS INCLUDES */
#include "dthgps.h"
#include "tatl01selftest.h"
#include "tatl02misc.h"
#include "tatl03modem.h"

enum {
false,
true,
};

typedef int ETatGpsSelfTestArg; /* values are defined in tatlgpsdth.h */

/* MACROS */

#define SYSLOG					GPS_SYSLOG
#define SYSLOGLOC				GPS_SYSLOGLOC
#define SYSLOGSTR				GPS_SYSLOGSTR

/**
 * msg_expr can be either a string (e.g. "generic assertion raised") or a
 * format string and one or more args (e.g. "file not found: %s", filename)
 * The calling code is responsible for passing valid string format and args.
 * Passing inappropriate args for string format could lead to critical failure.
 **/
#define ASSERT(bool_cond, msg_expr)								\
	do {														\
		if (!(bool_cond))										\
			SYSLOG(LOG_ERR, "ASSERTION FAILED: %s", msg_expr);	\
	} while (0);

#endif /* define TATLGPS_H_ */

