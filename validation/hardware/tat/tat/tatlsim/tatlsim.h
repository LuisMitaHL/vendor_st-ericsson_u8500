/*****************************************************************************/
/**
*  ST-Ericsson, 2011 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \file    tatlsim.h
* \brief   HATS SIM private definitions
* \author  ST-Ericsson
*/
/*****************************************************************************/

#ifndef TATLSIM_H_
#define TATLSIM_H_

#include "hatssim.h"

#define SYSLOG					SMC_SYSLOG
#define SYSLOGLOC				SMC_SYSLOGLOC
#define SYSLOGSTR				SMC_SYSLOGSTR

#define TATSIM_FEATURE          "SIM"

typedef enum {LSB_POSS, MSB_POSS} t_position;

#define	MAX_CHAR_PER_LINE	    4096
#define	SEP			        ","
#define DATASMCSIZE		        256

#include "pn_const_modem_ext.h"
#define PN_DSP_SIM_TEST         PN_MODEM_TEST
#define PN_DSP_COMMON_TEST      PN_COMMON_RF_TEST

#include "type_def_wm_isi_if.h"
#include "tatmodemlib.h"
#include "sim_hal_t_api.h"

/* these infos should be removed once source files have been reworked in order
 * to not use such strucutre/define */

#define ISI_COMMON_PADDING_BYTE    0x00

/* TATSIM_TEST_MASK */
#define TATSIM_INITIAL_TEST         0x00000000

#define TATSIM_ACTIVATION_MASK      0x0000000F
#define TATSIM_TEST_ACTIVATION      0x00000001
#define TATSIM_TEST_DEACTIVATION    0x00000002

#define TATSIM_SIM_SETTINGS_MASK    0x000000F0
#define TATSIM_TEST_PPS             0x00000010

#define TATSIM_SIM_TEST_MASK        0x00FFFF00
#define TATSIM_TEST_SET_FREQUENCY   0x00000100
#define TATSIM_TEST_CLOCK_STOP      0x00000200
#define TATSIM_TEST_CLOCK_START     0x00000400
#define TATSIM_TEST_SEND_CMD        0x00000800

#define TATSIM_SELFTEST_MASK        0xFF000000
#define TATSIM_TEST_UICC_SELFTEST   0x10000000
#define TATSIM_TEST_I2C_SELFTEST    0x20000000

#define TATSIM_ALL_TEST_MASKS       0xFFFFFFFF
/* TATSIM_TEST_MASK to remember the tests done */
extern u32 v_tatsim_testsMask;

#endif /* TATLSIM_H_ */
