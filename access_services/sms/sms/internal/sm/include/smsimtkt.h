#ifndef SMSIMTKT_H
#define SMSIMTKT_H
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
 *  The Short Message SIM toolkit(CAT) handler.
 *
 *************************************************************************/

typedef enum {
    SIMTKT_NEW_SHORT_MESSAGE = 1,
    SIMTKT_BUILD_DELIVERY_REPORT
} SIMTKT_Event_t;

typedef struct SIMTKTSystemObject *SIMTKTSystem_p;

void SIMTKT_HandleEvent(
    SIMTKT_Event_t,
    EventData_p);

void SIMTKT_SetEnvelopeData(
    uint8_t,
    char *);
uint8_t SIMTKT_GetEnvelopeStatus(
    void);

#define MAX_SIZE_ACK_DATA (128)
// BER TLV tags

#define SMS_PP_DOWNLOAD_TAG  (0xD1)


#endif                          // SMSIMTKT_H
