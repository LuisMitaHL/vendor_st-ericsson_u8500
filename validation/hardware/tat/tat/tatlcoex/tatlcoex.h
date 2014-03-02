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

#ifndef TATLCOEX_H_
#define TATLCOEX_H_

#include <pthread.h>
#include <sys/mount.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#include "tatl11coexgen.h"
#include "dthrf.h"
#include "hatsrf.h"
#include "dthsim.h"
#include "dthvideo.h"
#include "tatl02videogen.h"
#include "tatl01videofunc.h"
#include "ConnServices.h"
#include "hats_audio.h"

#include <libi2c.h>
#include <libab.h>

/* ----------------------------------------------------------------------- */
/* Typedefs                                                                */
/* ----------------------------------------------------------------------- */

/**
 * List of available orders supported by a process.
 */
typedef enum{
	START=0,	/**< to start a process. */
	STOP=1,   /**< to stop a process. */
	STANDBY
}DTH_COEX_ORDER;

/**
 * List of available state supported by a process
 */
typedef enum{
	DEACTIVATED=0,	/**< process is stopped. */
	ACTIVATED=1,  /**< process is running. */
	ON_STANDBY
}DTH_COEX_STATE;

#include "tatl01misc.h"
#include "tatl02memory.h"
#include "tatl03display.h"
#include "tatl04pwm.h"
#include "tatl05sensor.h"
#include "tatl06i2c.h"
#include "tatl07victim.h"
#include "tatl08audio.h"
#include "tatl09modi2c.h"
#include "tatl10apei2c.h"
#include "tatl12conn.h"

/* ----------------------------------------------------------------------- */
/* Constants                                                               */
/* ----------------------------------------------------------------------- */
#define MAX_STRNG_SIZE	1000
#define NO_ERROR        "No error"

/* ----------------------------------------------------------------------- */
/* Enumerations                                                            */
/* ----------------------------------------------------------------------- */
enum {
false,
true,
};

#define SYSLOG					COEX_SYSLOG
#define SYSLOGLOC				COEX_SYSLOGLOC
#define SYSLOGSTR				COEX_SYSLOGSTR

#endif /* TATLCOEX_H_*/
