#ifndef SMOTADP_H
#define SMOTADP_H
/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
/**************************************************************************
 *
 * DESCRIPTION:
 *
 *  Short Message Over The Air De-Personalisation handler include file.
 *
 *************************************************************************/

typedef enum {
    OTADP_NEW_SHORT_MESSAGE = 1,
} OTADP_Event_t;

typedef struct OTADPSystemObject *OTADPSystem_p;

void OTADP_HandleEvent(
    OTADP_Event_t,
    EventData_p);

#endif                          // SMOTADP_H
