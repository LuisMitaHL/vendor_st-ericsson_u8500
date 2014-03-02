
/*****************************************************************************/
/**
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
*/
/*****************************************************************************/

#ifndef __ISP8500_PRIMARY_FW_SPLIT_H
#define __ISP8500_PRIMARY_FW_SPLIT_H

#ifdef __cplusplus
    extern "C" {
#endif

#define ISP8500_PRIMARY_FW_SPLIT_SIZE 65536

extern char Isp8500_primary_fw_split_type[];
extern char Isp8500_primary_fw_split_version[];

extern unsigned char IMPORT_C Isp8500_primary_fw_split[ISP8500_PRIMARY_FW_SPLIT_SIZE];

#ifdef __cplusplus
    } /* extern C */
#endif

#endif /* __ISP8500_PRIMARY_FW_SPLIT_H */

