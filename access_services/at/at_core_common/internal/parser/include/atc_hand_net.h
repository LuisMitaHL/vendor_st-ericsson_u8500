/**
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */


#ifndef INCLUSION_GUARD_ATC_HANDL_NET_H
#define INCLUSION_GUARD_ATC_HANDL_NET_H

/*************************************************************************
 * Includes
 *************************************************************************/

/*************************************************************************
 * Types, constants and external variables
 *************************************************************************/

/*************************************************************************
 * Declaration of functions
 **************************************************************************/

/*
 * Handle CREG events sent from MAL. Compose an AT string and broadcast it.
 */
void handle_incoming_creg_event(void *data_p);

/*
 * Handle EREG unsolicited result code triggered by network registration, cell id, lac or act change
 */
void handle_incoming_ereg_event(void *data_p);

/*
 * Handle CGREG events sent from MAL. Compose an AT string and broadcast it .
 */
void handle_incoming_cgreg_event(void *data_p);

/*
 * Handle incoming ECME event.
 */
void handle_incoming_ecme_event(exe_ecme_response_t *response);

/*************************************************************************/
#endif /* INCLUSION_GUARD_ATC_HANDL_NET_H */
