/*****************************************************************************/
/**
*  © ST-Ericsson, 2011 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \file	   tatlrf.h
* \brief   internal definitions and macros
* \author  ST-Ericsson
*/
/*****************************************************************************/
#ifndef TATLRF_H_
#define TATLRF_H_

#include "hatsrf.h"

#define RF_FEATURE              "RF"

#define MAX_LOCAL_STRING_SIZE   256
#define MAX_CHAR_PER_LINE       4096

/* RF_DATA_FILE_DIR is a path defined externaly */
#define RX_IQ_SAMPLES_FILE      RF_DATA_FILE_DIR"/IQSample.csv"
#define RX_IQ_SAMPLES_BACKUP    RF_DATA_FILE_DIR"/IQSample.bak"
#define RX_IQ_SAMPLES_SCRIPT    IQSAMPLE_SCRIPT_DIR"/iqsample.sh"

#define PATH_PARAMETER_ELEMENT  RF_DATA_FILE_DIR"/rfparams.csv"

enum {
	false,
	true,
};

typedef enum {
	RF_BAND_0 = 0,
	RF_BAND_1,
	RF_BAND_2,
	RF_BAND_3,
	RF_BAND_4,
	RF_BAND_5,
	RF_BAND_6,
	RF_BAND_7,
	RF_BAND_8,
	RF_BAND_9,
	RF_NO_BAND = 65535
} RF_BAND;

#define RF_TEST_MODEM_NOT_READY      C_TEST_NOT_AVAILABLE

#include "type_def_wm_isi_if.h"
#include "isi_common_lib.h"
#include <math.h>

/* RF_HAL */

#include "pn_const_modem_ext.h"
#include "common_dsp_test_isi.h"

#define PN_DSP_COMMON_TEST           PN_COMMON_RF_TEST
#define ISIFILLER8                   0x00
#define ISIFILLER16	                 0x55AA

#define TATLRF_COMMON_DSP_TEST_ISI_VERSION_Z   0
#define TATLRF_COMMON_DSP_TEST_ISI_VERSION_Y 160


#if (TATLRF_COMMON_DSP_TEST_ISI_VERSION_Z < COMMON_DSP_TEST_ISI_VERSION_Z)
#error ********* modem version and tatlrf implementation are not ALIGNED!!! [COMMON_DSP_TEST_ISI_VERSION_Z definition in common_dsp_test_init.h file and TATLRF_COMMON_DSP_TEST_ISI_VERSION_Z in rfdef.h mismatch!!!]  *********
#elif (TATLRF_COMMON_DSP_TEST_ISI_VERSION_Y < COMMON_DSP_TEST_ISI_VERSION_Y)
#error ********* modem version and tatlrf implementation are not ALIGNED!!! [COMMON_DSP_TEST_ISI_VERSION_Y definition in common_dsp_test_init.h file and TATLRF_COMMON_DSP_TEST_ISI_VERSION_Y in rfdef.h mismatch!!!]  *********
#endif


/* these headers are temporary ones. They should be replaced later by their
   definitive counterparts */
#include "isimiss.h"
#include "isiwcdma.h"

/* max: select maximum of two values */
#define max(a,b)      (((a) > (b)) ? (a) : (b))

/* min: select minimum of two values */
#define min(a,b)      (((a) < (b)) ? (a) : (b))

#define RF_Q_MULT(q)     (float)pow(2, (q))
#define RF_Q(x, p)       ( ((double)(x)) * RF_Q_MULT(p) )
#define RF_UNQ(x, p)     ( ((double)(x)) / RF_Q_MULT(p) )

#define SYSLOG					RF_SYSLOG
#define SYSLOGLOC				RF_SYSLOGLOC
#define SYSLOGSTR				RF_SYSLOGSTR

#define DTH_SET_SIGNED          RF_DTH_SET_SIGNED
#define DTH_SET_UNSIGNED        RF_DTH_SET_UNSIGNED
#define DTH_SET_FLOAT           RF_DTH_SET_FLOAT

#define DTH_GET_SIGNED          RF_DTH_GET_SIGNED
#define DTH_GET_UNSIGNED        RF_DTH_GET_UNSIGNED
#define DTH_GET_FLOAT           RF_DTH_GET_FLOAT

#define DEREF_PTR               RF_DEREF_PTR
#define DEREF_PTR_SET           RF_DEREF_PTR_SET

/* msg_expr can be either a string (e.g. "generic assertion raised") or a format
 * string and one or more args (e.g. "file not found: %s", filename).
 * The caller is responsible for passing valid string format and args. Passing
 * inappropriate args for string format may lead to critical failure.
 */
#define ASSERT(bool_cond, msg_expr)                                 \
    if ( !(bool_cond) ) {                                           \
        SYSLOG(LOG_WARNING, "ASSERTION FAILED: %s", (msg_expr));    \
    }

#include "tatmodemlib.h"

#endif /* TATLRF_H_ */
