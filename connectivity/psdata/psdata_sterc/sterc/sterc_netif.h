/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 * \brief  Routing Control
 */

/**
 * Returns 0 on success, sets errno and returns negative on error.
 * *ifindex is set on success, but not modified on error.
 * Note ifname is in/out and must be minimum size MAX_IFNAME_LEN.
 */
int rtnl_create_caif_interface(int type, int conn_id, char *ifname,
                               int *ifindex, char loop);

