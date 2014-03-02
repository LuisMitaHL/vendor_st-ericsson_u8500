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

#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define SYSLOG_COND g_tat_modem_syslog_level
#define SYSLOGLOC(prio) do { if (SYSLOG_COND >= prio) printf("TATMODEMLIB,%s; %s,%d(%d); ", __func__, __FILE__, __LINE__, prio); } while (0);
#define SYSLOGSTR(prio, fmt , __args__...) do { if (SYSLOG_COND >= prio) printf (fmt, ##__args__); } while (0);
#define SYSLOG(prio, fmt , __args__...) do { if (SYSLOG_COND >= prio) printf ("TATMODEMLIB,%s; %s,%d(%d)\n\t"fmt"\n\n", __func__, __FILE__, __LINE__, prio, ##__args__); } while (0);

#define TAT_MODEM_SERVICES      "modem"

#include "tatmodemlib.h"

#if defined(CONFIG_PHONET)

#include "type_def_wm_isi_if.h"
#include "pn_const_modem_ext.h"
#include "isi_common_lib.h"
#include "isi_driver_lib.h"
#include "Modem_Lib.h"

/* system log */

static int g_tat_modem_syslog_level = LOG_ERR;

/* type: isi driver error code */
typedef int32_t isidrivererr_t;

/* type: modem lib error code */
typedef uint32_t modemliberr_t;

static char *modem_modes[] = { "local",
								"normal",
								"unknown",
								"in reset",
								"error" };

static char *modem_isi_states[] = { "offline", "online" };

/* modem mode */
static int g_tat_modem_mode = TAT_MODEM_MODE_UNKNOWN;

/* modem connection status */
static int g_tat_modem_state = TAT_MODEM_OFFLINE;

/* owner of modem connection when in local mode (informative only) */
static char g_tat_modem_owner[TAT_MODEM_OWNER_LEN] = "";

/* global buffer for messages reading */
static uint8_t g_isi_buffer[ISI_DRIVER_MAX_MESSAGE_LENGTH];

/* buffer len initialized after each call to tat_modem_read_msg */
static int32_t g_isi_buffer_len;

void tat_modem_set_log(int level)
{
	g_tat_modem_syslog_level = level;
}

void tat_modem_set_owner(char *buf)
{
	int len = strlen(buf);
	if (len >= TAT_MODEM_OWNER_LEN)
		len = TAT_MODEM_OWNER_LEN - 1;

	if (strcmp(buf, g_tat_modem_owner) != 0) {
		strncpy(g_tat_modem_owner, buf, len);
		g_tat_modem_owner[len] = 0;

		SYSLOG(LOG_DEBUG, "modem owner: %s", g_tat_modem_owner);
	}
}

void tat_modem_print_isi(uint8_t *bytes, size_t size, int prio)
{
	size_t i;
	SYSLOGSTR(prio, "\n");
	for (i = 0; i < size; i++) {
		if (i > 0)
			SYSLOGSTR(prio, ",");

		SYSLOGSTR(prio, "%02X", bytes[i]);
	}
	SYSLOGSTR(prio, "\n%u bytes.\n\n", size);
}

uint8_t *tat_modem_marsh8(uint8_t * val, TAT_MODEM_ISI_MARSH op)
{
	uint8_t *ptr = val + 1;
	if (op == ISI_MARSH)
		isi_common_Set8bit(val, 0, *val);
	else
		*val = isi_common_Get8bit(val, 0);

	return ptr;
}

uint16_t *tat_modem_marsh16(uint16_t * val, TAT_MODEM_ISI_MARSH op)
{
	uint16_t *ptr = val + 1;
	if (op == ISI_MARSH)
		isi_common_Set16bit((uint8_t *) val, 0, *val);
	else
		*val = isi_common_Get16bit((uint8_t *) val, 0);

	return ptr;
}

uint32_t *tat_modem_marsh32(uint32_t * val, TAT_MODEM_ISI_MARSH op)
{
	uint32_t *ptr = val + 1;
	if (op == ISI_MARSH)
		isi_common_Set32bit((uint8_t *) val, 0, *val);
	else
		*val = isi_common_Get32bit((uint8_t *) val, 0);

	return ptr;
}

int tat_modem_grant(char *feature)
{
	int vl_err = ECONNREFUSED;

	/* modem has no restriction access */
	if (strcasecmp(TAT_MODEM_SERVICES, feature) == 0) {
		vl_err = 0;
	} else {
		/* case of other features */
		int vl_mode = TAT_MODEM_MODE_UNKNOWN;
		tat_modem_get_mode(TAT_MODEM_SRCE_AUTO, &vl_mode);
		switch (vl_mode) {
		case TAT_MODEM_MODE_LOCAL:

			vl_err = 0;

			/* changing owner feature */
			if (strcasecmp(g_tat_modem_owner, feature) != 0)
				SYSLOG(LOG_INFO, "switching to feature to %s",
				       feature);
			break;

			/* any feature can access modem in normal mode */
		case TAT_MODEM_MODE_NORMAL:

			vl_err = 0;
			break;

		case TAT_MODEM_MODE_UNKNOWN:

			SYSLOG(LOG_ERR, "refused: not ready");
			break;

		default:

			SYSLOG(LOG_ERR, "refused: system in error state");
			break;
		}
	}

	/* update owner if successful */
	if (0 == vl_err)
		tat_modem_set_owner(feature);

	return vl_err;
}

void tat_modem_set_state(int state)
{
	switch (state) {
	case TAT_MODEM_OFFLINE:
	case TAT_MODEM_ONLINE:

		g_tat_modem_state = state;
		SYSLOG(LOG_NOTICE, "modem state changed: %s",
		       modem_isi_states[state]);

		if (TAT_MODEM_OFFLINE == state)
			/* loose ownership when closing */
			tat_modem_set_owner("");

		break;

	default:
		SYSLOG(LOG_ERR, "invalid modem state: %d", state);
	}
}

int tat_modem_open_resource(rsrcid_t vp_rsrc)
{
	int vl_err;

	isidrivererr_t vl_isi_err = isi_driver_open(vp_rsrc);
	if (ISID_R_SUCCESS == vl_isi_err) {
		/* success */
		vl_err = 0;
	} else {
		/* fail to open */
		SYSLOG(LOG_ERR, "fail to open resource %d", vp_rsrc);
		vl_err = ECONNREFUSED;
	}

	return vl_err;
}

int tat_modem_lib_init(void);

int tat_modem_prepare(char *feature)
{
	/* take the feature into account (will also get the modem mode if needed) */
	int vl_err = tat_modem_grant(feature);

	/* open isi link if not opened */
	if ((0 == vl_err) && (TAT_MODEM_OFFLINE == g_tat_modem_state)) {
		SYSLOG(LOG_INFO, "establishing modem connection...");

		/* init isi driver lib */
		isi_driver_init();

		/* tell online to avoid reentering in tat_modem_prepare
		 * (isi_driver_init section) */
		tat_modem_set_state(TAT_MODEM_ONLINE);

		/* init modem lib */
		vl_err = tat_modem_lib_init();
		if (0 == vl_err) {
			/* open session for all needed resources/servers.
			 * see pn_const_modem_ext.h for the definition of
			 * all resources/servers. */

			/* main server */
			SYSLOG(LOG_DEBUG, "opening MODEM TEST...");
			vl_err = tat_modem_open_resource(PN_MODEM_TEST);

			/* RF servers */
			if (0 == vl_err) {
				SYSLOG(LOG_DEBUG, "opening COMMON RF TEST...");
				vl_err = tat_modem_open_resource(PN_COMMON_RF_TEST);
			}
			if (0 == vl_err) {
				SYSLOG(LOG_DEBUG, "opening WCDMA RF TEST...");
				vl_err = tat_modem_open_resource(PN_WCDMA_RF_TEST);
			}

			/* SIM servers */
			/* only PN_MODEM_TEST but already opened */
		}

		/* final check */
		if (0 != vl_err) {
			/* close all resources because we fail to open at
			 * least one resource expected. */
			tat_modem_close();
		}
	}

	return vl_err;
}

int tat_modem_read_msg(char *feature, t_isi_header * hdr)
{
	int vl_err;
	isidrivererr_t vl_drverr;	/* internal ISI error code */

	vl_err = tat_modem_prepare(feature);
	if (0 == vl_err) {
		/* NB: read is blocking */
		vl_drverr = isi_driver_read(ISI_DRIVER_MAX_MESSAGE_LENGTH, g_isi_buffer,
				    &g_isi_buffer_len);
		if (ISID_R_SUCCESS == vl_drverr) {
			SYSLOGSTR(LOG_INFO, "From modem:\n");
			tat_modem_print_isi((uint8_t *) g_isi_buffer, g_isi_buffer_len, LOG_INFO);

			if ((size_t)g_isi_buffer_len > sizeof(t_isi_header)) {
				if (hdr) {
					memcpy(hdr, g_isi_buffer, sizeof(t_isi_header));
				}
				vl_err = 0;
			} else {
				SYSLOG(LOG_ERR, "only a few bytes were received!\n");
				vl_err = EPROTO;
			}
		} else {
			SYSLOG(LOG_ERR, "isi driver error on read: code %d",
			       vl_drverr);
			vl_err = EPROTO;
		}
	}

	return vl_err;
}

void tat_modem_on_complete(int32_t mlib_request_id, void *answer,
			   void *userdata)
{
	if ((userdata == NULL) && (answer == NULL) && (mlib_request_id == 0))
		return;
	/* does nothing */
}

void tat_modem_unsollicited_event(int32_t mlib_unsol_event_id, void *data)
{
	if ((mlib_unsol_event_id == 0) && (data == NULL))
		return;
	/* does nothing */
}

/*
 * Initialize modem library.
 */
int tat_modem_lib_init(void)
{
	int vl_err = 0;
	modemliberr_t vl_err_modem;
	int mode;
	t_isi_header vl_isi_hdr;

	SYSLOG(LOG_INFO, "initializing modemlib...");

	vl_err_modem = modem_lib_init_all(tat_modem_on_complete,
					  tat_modem_unsollicited_event);

	if (vl_err_modem == MLIB_R_SUCCESS) {
		SYSLOG(LOG_INFO, "modemlib is initialized.");

		/* init modem lib context */
		vl_err = 0;
		mode = TAT_MODEM_MODE_UNKNOWN;

		tat_modem_get_mode(TAT_MODEM_SRCE_MODEM, &mode);
		while (!vl_err && (TAT_MODEM_MODE_UNKNOWN == mode)) {
			vl_err =
			    tat_modem_read_msg(TAT_MODEM_SERVICES, &vl_isi_hdr);
			if (!vl_err) {
				modem_lib_treat_ISI_message(vl_isi_hdr.resourceID, g_isi_buffer_len,
				     g_isi_buffer);

				tat_modem_get_mode(TAT_MODEM_SRCE_MODEM, &mode);
			}
		}

		SYSLOG(LOG_NOTICE,
		       "tatmodemlib init ends with code %d. Modem mode: %s.",
		       vl_err, modem_modes[mode]);

		/* if the library is the owner, we need to set to 'no owner' so any
		 * feature can own the modem link else it would have its requests
		 * refused.
		 */
		if (strcmp(g_tat_modem_owner, TAT_MODEM_SERVICES) == 0)
			tat_modem_set_owner("");

	} else {
		/* error: fail to initialize */
		SYSLOG(LOG_ERR, "modem lib init failed (code %d)",
		       vl_err_modem);
		vl_err = EPROTO;
	}

	return vl_err;
}

/*
 * get modem operating mode by asking modemlib
 *
 * @param mode  [out] any operating mode
 *
 * @return 0 if the function succeeds
 * @return EBADRQC
 * @return EINPROGRESS if the modem is busy
 * @return EFAULT if library is not initialized
 */
int tat_modem_ask_mode(int *mode)
{
	int vl_err;

	/* prerequisite: must be initialized else we
	 * cannot ask the modem its state */
	vl_err = tat_modem_prepare(TAT_MODEM_SERVICES);

	if (0 == vl_err) {
		modemliberr_t vl_err_modem;
		uint32_t vl_modem_state;

		*mode = TAT_MODEM_MODE_UNKNOWN;

		vl_err_modem = modem_lib_request(MLIB_SOL_MCE_GET_MODEM_MODE,
						 NULL, &vl_modem_state, NULL);

		if (vl_err_modem == MLIB_R_SUCCESS) {
			switch (vl_modem_state) {
			case MLIB_MCE_MODEM_NORMAL:

				*mode = TAT_MODEM_MODE_NORMAL;
				break;

			case MLIB_MCE_MODEM_TEST:

				*mode = TAT_MODEM_MODE_LOCAL;
				break;

			case MLIB_MCE_MODEM_SW_RESET:
			case MLIB_MCE_MODEM_POWER_OFF:

				*mode = TAT_MODEM_MODE_RESET;
				break;

			default:	/* any other should be considered as an error state */

				*mode = TAT_MODEM_MODE_ERROR;
			}
		} else {
			SYSLOG(LOG_INFO, "modem_lib_request returned error code %u.",
			       vl_err_modem);
			switch (vl_err_modem) {
			case MLIB_R_MODEM_TRANSACTION_ONGOING:

				*mode = TAT_MODEM_MODE_UNKNOWN;
				vl_err = EINPROGRESS;
				break;

			case MLIB_R_NOT_INITIALIZED:

				*mode = TAT_MODEM_MODE_UNKNOWN;
				vl_err = EFAULT;
				break;

			default:	/* another modem lib error */

				*mode = TAT_MODEM_MODE_ERROR;
				break;
			}
		}
	}

	return vl_err;
}

/******************************************************************************
 PUBLIC INTERFACE
******************************************************************************/

int tat_modem_get_owner(char *buf)
{
	int len = strlen(g_tat_modem_owner);

	if (len >= TAT_MODEM_OWNER_LEN)
		len = TAT_MODEM_OWNER_LEN - 1;

	strncpy(buf, g_tat_modem_owner, len);
	buf[len] = 0;

	SYSLOG(LOG_DEBUG, "owner is %s", g_tat_modem_owner);

	return len;
}

int tat_modem_get_mode(int srce, int *mode)
{
	int vl_err = 0;

	if (TAT_MODEM_SRCE_AUTO == srce) {
		srce = TAT_MODEM_SRCE_LIB;
		if (TAT_MODEM_MODE_UNKNOWN == g_tat_modem_mode)
			srce = TAT_MODEM_SRCE_MODEM;
	}

	switch (srce) {
	case TAT_MODEM_SRCE_LIB:

		*mode = g_tat_modem_mode;
		break;

	case TAT_MODEM_SRCE_MODEM:

		vl_err = tat_modem_ask_mode(mode);
		if (0 == vl_err)
			g_tat_modem_mode = *mode;

		break;

	default:
		vl_err = EBADRQC;
	}

	return vl_err;
}

int tat_modem_set_mode(int mode)
{
	int vl_err = 0, vl_mode;
	modemliberr_t vl_err_modem;

	vl_mode = TAT_MODEM_MODE_UNKNOWN;
	tat_modem_get_mode(TAT_MODEM_SRCE_AUTO, &vl_mode);
	if (vl_mode != mode) {
		/* change in current modem operating mode */
		switch (mode) {
		case TAT_MODEM_MODE_LOCAL:

			SYSLOG(LOG_DEBUG, "set modem to %s", modem_modes[mode]);
			vl_err_modem = modem_lib_request(MLIB_SOL_MCE_SET_TEST_MODE, NULL,
					      NULL, NULL);

			if (vl_err_modem == MLIB_R_SUCCESS)
				vl_err = 0;
			else
				vl_err = EFAULT;
			break;

		case TAT_MODEM_MODE_NORMAL:

			SYSLOG(LOG_DEBUG, "set modem to %s", modem_modes[mode]);
			vl_err_modem = modem_lib_request(MLIB_SOL_MCE_SET_NORMAL_MODE,
					      NULL, NULL, NULL);

			if (vl_err_modem == MLIB_R_SUCCESS)
				vl_err = 0;
			else
				vl_err = EFAULT;
			break;

		default:

			SYSLOG(LOG_ERR, "not a nominal modem mode: %d", mode);
			vl_err = EBADRQC;
		}

		/* we can't assume in which state the modem really is after such call
		 * so ask it directly */
		if (0 == vl_err) {
			vl_mode = TAT_MODEM_MODE_RESET;
			tat_modem_get_mode(TAT_MODEM_SRCE_MODEM, &vl_mode);
		}
	} else {
		/* no change needed */
		SYSLOG(LOG_DEBUG, "set modem : no change needed");
	}

	return vl_err;
}

int tat_modem_get_state()
{
	return g_tat_modem_state;
}

int tat_modem_open(void)
{
	int vl_err = 0;

	int vl_state = tat_modem_get_state();
	switch (vl_state) {
	case TAT_MODEM_OFFLINE:

		vl_err = tat_modem_prepare(TAT_MODEM_SERVICES);
		break;

	case TAT_MODEM_ONLINE:

		SYSLOG(LOG_WARNING, "already connected: aborting.");
		break;

	default:

		SYSLOG(LOG_ERR, "invalid modem state: %d", vl_state);
		vl_err = EINVAL;
	}

	return vl_err;
}

int tat_modem_close(void)
{
	int vl_err = 0;
	isidrivererr_t vl_drverr;

	int vl_state = tat_modem_get_state();
	switch (vl_state) {
	case TAT_MODEM_ONLINE:
		vl_drverr = isi_driver_close();
		if (ISID_R_SUCCESS == vl_drverr) {
			tat_modem_set_state(TAT_MODEM_OFFLINE);
		} else {
			/* fail to close */
			SYSLOG(LOG_ERR,
			       "fail to close modem connection: error %d",
			       vl_drverr);
			vl_err = ECONNREFUSED;
		}
		break;

	case TAT_MODEM_OFFLINE:

		/* already opened: no effect */
		SYSLOG(LOG_WARNING, "already offline: aborting.");
		break;

	default:
		SYSLOG(LOG_ERR, "invalide modem state: %d", vl_state);
		vl_err = EINVAL;
	}

	return vl_err;
}

int tat_modem_send(char *feature, void *msg, size_t msg_len, rsrcid_t * rsc_id)
{
	int vl_err;
	isidrivererr_t vl_drverr;	/* Internal ISI error code */
	t_isi_header *vl_isi_hdr;

	vl_err = tat_modem_prepare(feature);
	if (0 == vl_err) {
		/* remember the request resourceID to identify its response */
		vl_isi_hdr = (t_isi_header *) msg;
		*rsc_id = vl_isi_hdr->resourceID;

		SYSLOG(LOG_DEBUG, "sending ISI message, resource id = %u...",
		       *rsc_id);

		/* send the request */
		SYSLOGSTR(LOG_INFO, "To modem:\n");
		tat_modem_print_isi(msg, msg_len, LOG_INFO);

		vl_drverr = isi_driver_send(msg_len, msg);
		if (ISID_R_SUCCESS != vl_drverr) {
			/* Communication error on send */
			SYSLOG(LOG_ERR, "isi driver error (code %d)",
			       vl_drverr);
			vl_err = ECOMM;
		}
	}

	return vl_err;
}

int tat_modem_read(char *feature, rsrcid_t rsc_id, void *resp,
		   size_t *resp_len)
{
	int vl_err;
	t_isi_header vl_isi_hdr;

	SYSLOG(LOG_DEBUG, "enter ISI read loop...");

	/* read ISI messages until response with expected rsc_id is
	 * received or an error occured */
	do {
		vl_err = tat_modem_read_msg(feature, &vl_isi_hdr);
		size_t msg_len = g_isi_buffer_len;

		if (0 == vl_err) {
			if (vl_isi_hdr.resourceID == rsc_id) {
				/* the one expected. copy data to resp. also check for
				 * potential buffer overflow */
				SYSLOG(LOG_DEBUG,
				       "expected response received.");

				/* return the response size in bytes */
				if (resp_len) {
					if (resp) {
						if (msg_len > *resp_len) {
							SYSLOG(LOG_WARNING,
							       "warning: response is larger than expected. Returned: %d bytes, expected: %d.",
							       msg_len, *resp_len);
							if (SYSLOG_COND < LOG_INFO)
								tat_modem_print_isi(g_isi_buffer, msg_len, LOG_WARNING);

							/* copy only enough to fill the buffer */
							msg_len = *resp_len;
						}

						/* save response in providen buffer */
						memcpy(resp, g_isi_buffer,
						       msg_len);
					}

					/* return response size, limited to buffer
					 * size if provided */
					*resp_len = msg_len;
				}
			} else {
				/* the message read is not the response expected. */
				/* it must be transferred to modem library to do
				 * some default handling */
				SYSLOG(LOG_DEBUG,
				       "retransmitting message with resource id = %u",
				       vl_isi_hdr.resourceID);
				modem_lib_treat_ISI_message
				    (vl_isi_hdr.resourceID, msg_len,
				     g_isi_buffer);
			}
		}
	} while ((0 == vl_err) && (vl_isi_hdr.resourceID != rsc_id));

	SYSLOG(LOG_DEBUG, "exiting read loop with code %d", vl_err);

	return vl_err;
}

#else /* CONFIG_PHONET */

/* modem libraries and drivers includes */
#include "type_def_wm_isi_if.h"
#include "isi_common_lib.h"
#include "isi_driver_lib.h"
#include "Modem_Lib.h"

/* system log */

static int g_tat_modem_syslog_level = LOG_ERR;

/* type: isi driver error code */
typedef int32_t isidrivererr_t;

/* type: modem lib error code */
typedef uint32_t modemliberr_t;

static char *modem_modes[] = { "local",
								"normal",
								"unknown",
								"in reset",
								"error" };

static char *modem_isi_states[] = { "closed", "opened" };

/* modem library initialization flag. 1 if intialized else 0. */
static int g_tat_modem_lib_init;

/* modem mode */
static int g_tat_modem_mode = TAT_MODEM_MODE_UNKNOWN;

/* isi libk status */
static int g_tat_modem_state = TAT_MODEM_DISCONNECTED;

/* owner of modem connection when in local mode */
static char g_tat_modem_owner[TAT_MODEM_OWNER_LEN] = "";

/* global buffer for messages reading */
static uint8_t g_isi_buffer[ISI_DRIVER_MAX_MESSAGE_LENGTH];

/* buffer len initialized after each call to tat_modem_read_msg */
static size_t g_isi_buffer_len;

void tat_modem_set_log(int level)
{
	g_tat_modem_syslog_level = level;
}

void tat_modem_set_owner(char *buf)
{
	int len = strlen(buf);
	if (len >= TAT_MODEM_OWNER_LEN)
		len = TAT_MODEM_OWNER_LEN - 1;

	if (strcmp(buf, g_tat_modem_owner) != 0) {
		strncpy(g_tat_modem_owner, buf, len);
		g_tat_modem_owner[len] = 0;

		SYSLOG(LOG_DEBUG, "modem owner: %s", g_tat_modem_owner);
	}
}

void tat_modem_print_isi(uint8_t *bytes, size_t size, int prio)
{
	size_t i;
	SYSLOGSTR(prio, "\n");
	for (i = 0; i < size; i++) {
		if (i > 0)
			SYSLOGSTR(prio, ",");

		SYSLOGSTR(prio, "%02X", bytes[i]);
	}
	SYSLOGSTR(prio, "\n%u bytes.\n\n", size);
}

uint8_t *tat_modem_marsh8(uint8_t * val, TAT_MODEM_ISI_MARSH op)
{
	uint8_t *ptr = val + 1;
	if (op == ISI_MARSH)
		isi_common_Set8bit(val, 0, *val);
	else
		*val = isi_common_Get8bit(val, 0);

	return ptr;
}

uint16_t *tat_modem_marsh16(uint16_t * val, TAT_MODEM_ISI_MARSH op)
{
	uint16_t *ptr = val + 1;
	if (op == ISI_MARSH)
		isi_common_Set16bit((uint8_t *) val, 0, *val);
	else
		*val = isi_common_Get16bit((uint8_t *) val, 0);

	return ptr;
}

uint32_t *tat_modem_marsh32(uint32_t * val, TAT_MODEM_ISI_MARSH op)
{
	uint32_t *ptr = val + 1;
	if (op == ISI_MARSH)
		isi_common_Set32bit((uint8_t *) val, 0, *val);
	else
		*val = isi_common_Get32bit((uint8_t *) val, 0);

	return ptr;
}

int tat_modem_grant(char *feature)
{
	int vl_err = ECONNREFUSED;

	/* modem has no restriction access */
	if (strcasecmp(TAT_MODEM_SERVICES, feature) == 0) {
		vl_err = 0;
	} else {
		/* case of other features */
		int vl_mode = TAT_MODEM_MODE_UNKNOWN;
		tat_modem_get_mode(TAT_MODEM_SRCE_AUTO, &vl_mode);
		switch (vl_mode) {
		case TAT_MODEM_MODE_LOCAL:
			vl_err = 0;
			/* changing owner feature */
			if (strcasecmp(g_tat_modem_owner, feature) != 0)
				SYSLOG(LOG_INFO, "switching to feature to %s",
				       feature);
			break;

			/* any feature can access modem in normal mode */
		case TAT_MODEM_MODE_NORMAL:

			vl_err = 0;
			break;

		case TAT_MODEM_MODE_UNKNOWN:

			SYSLOG(LOG_ERR, "refused: not ready");
			break;

		default:

			SYSLOG(LOG_ERR, "refused: system in error state");
			break;
		}
	}

	/* update owner if successful */
	if (0 == vl_err)
		tat_modem_set_owner(feature);

	return vl_err;
}

void tat_modem_set_state(int state)
{
	switch (state) {
	case TAT_MODEM_DISCONNECTED:
	case TAT_MODEM_CONNECTED:

		g_tat_modem_state = state;
		SYSLOG(LOG_NOTICE, "modem state changed: %s",
		       modem_isi_states[state]);

		if (TAT_MODEM_DISCONNECTED == state) {
			/* loose ownership when closing */
			tat_modem_set_owner("");
		}

		break;

	default:
		SYSLOG(LOG_ERR, "invalid modem state: %d", state);
	}
}

int tat_modem_open_if_closed()
{
	int vl_err = 0;

	if (TAT_MODEM_DISCONNECTED == g_tat_modem_state) {
		SYSLOG(LOG_DEBUG, "opening isi link...");
		isidrivererr_t vl_isi_err = isi_driver_open();
		if (ISID_R_SUCCESS == vl_isi_err) {
			/* now connected */
			tat_modem_set_state(TAT_MODEM_CONNECTED);
			vl_err = 0;
		} else {
			/* fail to open */
			SYSLOG(LOG_ERR, "fail to open isi link.");
			vl_err = ECONNREFUSED;
		}
	}

	return vl_err;
}

int tat_modem_prepare(char *feature)
{
	int vl_err;

	/* take the feature into account
	 * (will also get the modem mode if needed) */
	vl_err = tat_modem_grant(feature);

	/* open isi link if not opened */
	if (0 == vl_err)
		vl_err = tat_modem_open_if_closed();

	return vl_err;
}

int tat_modem_read_msg(char *feature, t_isi_header * hdr)
{
	int vl_err;
	int32_t vl_err_isi;	/* internal ISI error code */

	vl_err = tat_modem_prepare(feature);
	if (0 == vl_err) {
		/* NB: read is blocking */
		vl_err_isi =
		    isi_driver_read(ISI_DRIVER_MAX_MESSAGE_LENGTH, g_isi_buffer, &g_isi_buffer_len);
		if (ISID_R_SUCCESS == vl_err_isi) {
			SYSLOGSTR(LOG_INFO, "From modem:\n");
			tat_modem_print_isi((uint8_t *) g_isi_buffer,
					    g_isi_buffer_len, LOG_INFO);

			if (g_isi_buffer_len > (unsigned int)sizeof(t_isi_header)) {
				if (hdr)
					*hdr = *((t_isi_header *) g_isi_buffer);

				vl_err = 0;
			} else {
				SYSLOG(LOG_ERR,
				       "only a few bytes were received!\n");
				vl_err = EPROTO;
			}
		} else {
			SYSLOG(LOG_ERR, "isi driver error on read (code %d)",
			       vl_err_isi);
			vl_err = EPROTO;
		}
	}

	return vl_err;
}

void tat_modem_on_complete(int32_t mlib_request_id, void *answer,
			   void *userdata)
{
	/* does nothing */
}

void tat_modem_unsollicited_event(int32_t mlib_unsol_event_id, void *data)
{
	/* does nothing */
}

/*
 * Initialize modem library.
 */
int tat_modem_lib_init(void)
{
	int vl_err = 0;
	modemliberr_t vl_err_modem;
	int mode;
	t_isi_header vl_isi_hdr;

	SYSLOG(LOG_INFO, "initializing modemlib...");

	vl_err_modem = modem_lib_init_all(tat_modem_on_complete,
					  tat_modem_unsollicited_event);

	if (vl_err_modem == MLIB_R_SUCCESS) {
		SYSLOG(LOG_INFO, "modemlib is initialized.");

		/* do not renter in tat_modem_lib_init! */
		g_tat_modem_lib_init = 1;

		/* init modem lib context */
		vl_err = 0;
		mode = TAT_MODEM_MODE_UNKNOWN;

		tat_modem_get_mode(TAT_MODEM_SRCE_MODEM, &mode);
		while (!vl_err && (TAT_MODEM_MODE_UNKNOWN == mode)) {
			vl_err =
			    tat_modem_read_msg(TAT_MODEM_SERVICES, &vl_isi_hdr);
			if (!vl_err) {
				modem_lib_treat_ISI_message(vl_isi_hdr.resourceID, g_isi_buffer_len,
				     g_isi_buffer);

				tat_modem_get_mode(TAT_MODEM_SRCE_MODEM, &mode);
			}
		}
		SYSLOG(LOG_NOTICE,
		       "tatmodemlib init ends with code %d. Modem mode: %s.",
		       vl_err, modem_modes[mode]);

		/* if the library is the owner, we need to set to 'no owner' so any
		 * feature can own the modem link else it would have its requests
		 * refused.
		 */
		if (strcmp(g_tat_modem_owner, TAT_MODEM_SERVICES) == 0)
			tat_modem_set_owner("");
	} else {
		/* error: fail to initialize */
		SYSLOG(LOG_ERR, "modem lib init failed (code %d)",
		       vl_err_modem);
		vl_err = EPROTO;
	}

	return vl_err;
}

/*
 * get modem operating mode by asking modemlib
 *
 * @param mode  [out] any operating mode
 *
 * @return 0 if the function succeeds
 * @return EBADRQC
 * @return EINPROGRESS if the modem is busy
 * @return EFAULT if library is not initialized
 */
int tat_modem_ask_mode(int *mode)
{
	int vl_err = 0;

	/* prerequisite: isi link must be opened and modem lib initialized */
	vl_err = tat_modem_open_if_closed();

	if ((0 == vl_err) && !g_tat_modem_lib_init)
		vl_err = tat_modem_lib_init();

	if (0 == vl_err) {
		modemliberr_t vl_err_modem;
		uint32_t vl_modem_state;

		*mode = TAT_MODEM_MODE_UNKNOWN;

		vl_err_modem = modem_lib_request(MLIB_SOL_MCE_GET_MODEM_MODE,
						 NULL, &vl_modem_state, NULL);

		if (vl_err_modem == MLIB_R_SUCCESS) {
			switch (vl_modem_state) {
			case MLIB_MCE_MODEM_NORMAL:
				*mode = TAT_MODEM_MODE_NORMAL;
				break;
			case MLIB_MCE_MODEM_TEST:
				*mode = TAT_MODEM_MODE_LOCAL;
				break;
			case MLIB_MCE_MODEM_SW_RESET:
			case MLIB_MCE_MODEM_POWER_OFF:
				*mode = TAT_MODEM_MODE_RESET;
				break;
			default:	/* any other should be considered as an error state */
				*mode = TAT_MODEM_MODE_ERROR;
			}
		} else {
			SYSLOG(LOG_INFO,
			       "modem_lib_request returned error code %u.",
			       vl_err_modem);
			switch (vl_err_modem) {
			case MLIB_R_MODEM_TRANSACTION_ONGOING:
				*mode = TAT_MODEM_MODE_UNKNOWN;
				vl_err = EINPROGRESS;
				break;

			case MLIB_R_NOT_INITIALIZED:
				*mode = TAT_MODEM_MODE_UNKNOWN;
				vl_err = EFAULT;
				break;

			default:	/* another modem lib error */
				*mode = TAT_MODEM_MODE_ERROR;
				break;
			}
		}
	}

	return vl_err;
}

/*****************************************************************************
 PUBLIC INTERFACE
******************************************************************************/

int tat_modem_get_owner(char *buf, size_t buf_size)
{
	int len = strlen(g_tat_modem_owner);

	if (buf_size == 0)
		printf("buf size = 0\n");

	if (len >= TAT_MODEM_OWNER_LEN)
		len = TAT_MODEM_OWNER_LEN - 1;

	strncpy(buf, g_tat_modem_owner, len);
	buf[len] = 0;

	SYSLOG(LOG_DEBUG, "owner is %s", g_tat_modem_owner);

	return len;
}

int tat_modem_get_mode(int srce, int *mode)
{
	int vl_err = 0;

	if (TAT_MODEM_SRCE_AUTO == srce) {
		srce = TAT_MODEM_SRCE_LIB;
		if (TAT_MODEM_MODE_UNKNOWN == g_tat_modem_mode)
			srce = TAT_MODEM_SRCE_MODEM;
	}

	switch (srce) {
	case TAT_MODEM_SRCE_LIB:
		*mode = g_tat_modem_mode;
		break;

	case TAT_MODEM_SRCE_MODEM:
		vl_err = tat_modem_ask_mode(mode);
		if (0 == vl_err)
			g_tat_modem_mode = *mode;
		break;

	default:
		vl_err = EBADRQC;
	}

	return vl_err;
}

int tat_modem_set_mode(int mode)
{
	int vl_err = 0, vl_mode;
	modemliberr_t vl_err_modem;

	vl_mode = TAT_MODEM_MODE_UNKNOWN;
	tat_modem_get_mode(TAT_MODEM_SRCE_LIB, &vl_mode);
	if (vl_mode != mode) {
		/* change in current modem operating mode */
		switch (mode) {
		case TAT_MODEM_MODE_LOCAL:
			SYSLOG(LOG_DEBUG, "set modem to %s", modem_modes[mode]);
			vl_err_modem =
			    modem_lib_request(MLIB_SOL_MCE_SET_TEST_MODE, NULL,
					      NULL, NULL);

			if (vl_err_modem == MLIB_R_SUCCESS)
				vl_err = 0;
			else
				vl_err = EFAULT;
			break;

		case TAT_MODEM_MODE_NORMAL:
			SYSLOG(LOG_DEBUG, "set modem to %s", modem_modes[mode]);
			vl_err_modem = modem_lib_request(MLIB_SOL_MCE_SET_NORMAL_MODE,
					      NULL, NULL, NULL);

			if (vl_err_modem == MLIB_R_SUCCESS)
				vl_err = 0;
			else
				vl_err = EFAULT;
			break;
		default:
			SYSLOG(LOG_ERR, "not a nominal modem mode: %d", mode);
			vl_err = EBADRQC;
		}

		/* we can't assume in which state the modem really is after such call
		 * so ask it directly */
		if (0 == vl_err) {
			vl_mode = TAT_MODEM_MODE_RESET;
			tat_modem_get_mode(TAT_MODEM_SRCE_MODEM, &vl_mode);
		}
	} else {
		/* no change needed */
		vl_err = ENOEXEC;
	}

	return vl_err;
}

int tat_modem_get_state()
{
	return g_tat_modem_state;
}

int tat_modem_open(void)
{
	int vl_err = 0;

	int vl_state = tat_modem_get_state();
	switch (vl_state) {
	case TAT_MODEM_DISCONNECTED:
		vl_err = tat_modem_open_if_closed();
		break;
	case TAT_MODEM_CONNECTED:
		SYSLOG(LOG_WARNING, "isi link already opened. aborting.");
		break;
	default:
		SYSLOG(LOG_ERR, "invalid modem state: %d", vl_state);
		vl_err = EINVAL;
	}

	return vl_err;
}

int tat_modem_close(void)
{
	int vl_err = 0;

	int vl_state = tat_modem_get_state();
	switch (vl_state) {
	case TAT_MODEM_CONNECTED:
		isidrivererr_t vl_isi_err = isi_driver_close();
		if (ISID_R_SUCCESS == vl_isi_err) {
			tat_modem_set_state(TAT_MODEM_DISCONNECTED);
		} else {
			/* fail to close */
			SYSLOG(LOG_ERR, "fail to close isi link");
			vl_err = ECONNREFUSED;
		}
		break;
	case TAT_MODEM_DISCONNECTED:
		/* already opened: no effect */
		SYSLOG(LOG_WARNING, "isi link already close. aborting.");
		break;
	default:
		SYSLOG(LOG_ERR, "invalide modem state: %d", vl_state);
		vl_err = EINVAL;
	}

	return vl_err;
}

int tat_modem_send(char *feature, void *msg, size_t msg_len, uint32_t * rsc_id)
{
	int vl_err;
	int32_t vl_err_isi;	/* Internal ISI error code */
	t_isi_header *vl_isi_hdr;

	vl_err = tat_modem_prepare(feature);
	if (0 == vl_err) {
		/* remember the request resourceID to identify its response */
		vl_isi_hdr = (t_isi_header *) msg;
		*rsc_id = vl_isi_hdr->resourceID;

		SYSLOG(LOG_DEBUG, "sending ISI message, resource id = %u...",
		       *rsc_id);

		/* send the request */
		SYSLOGSTR(LOG_INFO, "To modem:\n");
		tat_modem_print_isi(msg, msg_len, LOG_INFO);

		vl_err_isi = isi_driver_send(msg_len, msg);
		if (ISID_R_SUCCESS != vl_err_isi) {
			/* Communication error on send */
			SYSLOG(LOG_ERR, "isi driver error (code %d)",
			       vl_err_isi);
			vl_err = ECOMM;
		}
	}

	return vl_err;
}

int tat_modem_read(char *feature, uint32_t rsc_id, void *resp,
		   size_t *resp_len)
{
	int vl_err;
	t_isi_header vl_isi_hdr;

	SYSLOG(LOG_DEBUG, "enter ISI read loop...");

	/* read ISI messages until response with expected rsc_id
	 * is received or an error occured */
	do {
		vl_err = tat_modem_read_msg(feature, &vl_isi_hdr);
		size_t msg_len = g_isi_buffer_len;

		if (0 == vl_err) {
			if (vl_isi_hdr.resourceID == rsc_id) {
				/* the one expected. copy data to resp.
				 * also check for potential buffer overflow */
				SYSLOG(LOG_DEBUG, "expected response received.");

				/* return the response size in bytes */
				if (resp_len) {
					if (resp) {
						if (msg_len > *resp_len) {
							SYSLOG(LOG_WARNING,
							       "warning: response is larger than expected. Returned: %d bytes, expected: %d.", msg_len, *resp_len);
							if (SYSLOG_COND < LOG_INFO)
								tat_modem_print_isi(g_isi_buffer, msg_len, LOG_WARNING);

							/* copy only enough to fill the buffer */
							msg_len = *resp_len;
						}

						/* save response in providen buffer */
						memcpy(resp, g_isi_buffer, msg_len);
					}
					/* return response size, limited to buffer size
					 * if provided */
					*resp_len = msg_len;
				}
			} else {
				/* the message read is not the response expected.
				 * it must be transferred to modem library
				 * to do some default handling */
				SYSLOG(LOG_DEBUG, "retransmitting message with resource id = %u",
				       vl_isi_hdr.resourceID);
				modem_lib_treat_ISI_message(vl_isi_hdr.resourceID, msg_len, g_isi_buffer);
			}
		}
	} while ((0 == vl_err) && (vl_isi_hdr.resourceID != rsc_id));

	SYSLOG(LOG_DEBUG, "exiting read loop with code %d", vl_err);

	return vl_err;
}
#endif /* CONFIG_PHONET */
