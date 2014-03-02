/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 * \brief  Routing Control Daemon service functions
 */

#ifndef STERCD_CAIF_H
#define STERCD_CAIF_H

#ifdef  __cplusplus
extern "C" {
#endif

    /********************************************************************************
     *
     * Defines & Type definitions
     *
     ********************************************************************************/

    typedef enum {
        tt_caif,
        tt_unknown
    }
    sterc_transport_type_t;

    typedef struct NAPRequest {
        uint8_t request;
        uint8_t cid;
        uint8_t orig_channel;
        uint8_t state;
        size_t datalen;
        struct NAPRequest *next;
    } NAPRequest;

    typedef struct NAPRequestQueue {
        pthread_mutex_t lock;
        pthread_cond_t cond;
        NAPRequest *requestList;
    } NAPRequestQueue;

    /* Info from connected PDN */
    typedef struct PdpContextParam {
        char *ipAddrStr;
        char *subnetMaskStr;
        char *mtuStr;
        char *primDnsStr;
        char *secDnsStr;
    } PdpContextParam;

#ifdef  __cplusplus
}
#endif

#endif
