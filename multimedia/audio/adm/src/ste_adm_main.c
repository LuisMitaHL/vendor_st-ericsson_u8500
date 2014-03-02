/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <string.h>
#include <errno.h>

#include "ste_adm_dbg.h"
#include "ste_adm_db.h"
#include "ste_adm_omx_log.h"
#include "ste_adm_api_devset.h"
#include "ste_adm_srv.h"
#include "ste_adm_dev.h"
#include "ste_adm_server.h"

#include "ste_adm_api_cscall.h"
#include "ste_adm_api_modem.h"
#include "ste_adm_api_d2d.h"
#include "ste_adm_api_devset.h"
#include "ste_adm_api_tg.h"
#include "ste_adm_api_util.h"
#include "ste_adm_omx_core.h"
#include "ste_adm_platform_adaptions.h"

#if defined(ADM_DBG_X86)
#include "ste_adm_hw_handler.h"
#else
#include "alsactrl_hwh.h"
#include "alsactrl_alsa.h"
#endif


#define ADM_LOG_FILENAME "main"

int ste_adm_server_create(void)
{
    int exitstatus = -1;
    int listen_fd = -1;
    sqlite3* db_h = NULL;

    dbg_set_logs_from_properties();

    if (signal(SIGPIPE, SIG_IGN) == SIG_ERR) {
        ALOG_ERR("signal(SIGPIPE, SIG_IGN) failed\n");
        goto cleanup;
    }

    ALOG_STATUS("ADM booting, pid=%d, %s %s. Mix/split=%s\n",
        getpid(), __TIME__, __DATE__, adaptions_internal_mixsplit() ? "ADM" : "AFM");

    // Initialize database subsystem
    if (adm_db_init() != STE_ADM_RES_OK) {
        ALOG_ERR("adm_db_init() failed!\n");
        goto cleanup;
    }

    // Init hw handler settings
#if defined(ADM_DBG_X86)
    if (ste_adm_hw_handler_init() != STE_ADM_RES_OK) {
       ALOG_ERR("ste_adm_hw_handler_init() failed!\n");
       goto cleanup;
    }
#else
    if(adm_db_get_handle(&db_h) != STE_ADM_RES_OK) {
       ALOG_ERR("adm_db_get_handle() failed!\n");
       goto cleanup;
    }
    if (Alsactrl_Hwh_Init(db_h, adm_db_toplevel_mapping_get) != STE_ADM_RES_OK) {
       ALOG_ERR("Alsactrl_Hwh_Init(db_h) failed!\n");
       goto cleanup;
    }
#endif

    /* Initialize OMX */
    if (OMX_ErrorNone != il_tool_log_Init()) {
        ALOG_ERR("il_tool_log_Init() failed!");
        goto cleanup;
    }

    ste_adm_debug_logbuf_init();

    if (srv_init() != STE_ADM_RES_OK) {
        ALOG_ERR("srv_init() failed\n");
        goto cleanup;
    }

    // Should be called after server is initialized; might need server
    // services (e.g. CSCall component sends IL callbacks that
    // we need to collect and dispatch once the main loop starts).
    dev_init();
    csapi_init();
    ste_adm_omx_core_init();
    ste_adm_omx_io_init();

    srv_mainloop();
    exitstatus = 0;

cleanup:
    /* Close the socket (disallow further receptions and transmissions) */
    if (listen_fd >= 0 && shutdown(listen_fd, SHUT_RDWR) < 0) {
        ALOG_ERR("shutdown() %s\n", strerror(errno));
    }

    /* unlink the the FS descriptor */
    if (unlink(STE_ADM_SOCK_PATH) < 0) {
        ALOG_ERR("unlink() %s\n", strerror(errno));
    }

    if (adm_db_disconnect() != STE_ADM_RES_OK) {
        ALOG_ERR("adm_db_disconnect() failed\n");
    }

    if(adm_get_modem_type()!=ADM_SLIM_MODEM) {
        SRV_DBG_ASSERT_IS_NOT_WORKER_THREAD;
        modem_deinit();
    }
    csapi_destroy();
    OMX_Deinit();
    ste_adm_omx_core_deinit();
    ste_adm_omx_io_deinit();
    return exitstatus;
}


