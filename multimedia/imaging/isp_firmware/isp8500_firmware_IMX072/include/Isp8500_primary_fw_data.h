
/*****************************************************************************/
/**
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
*/
/*****************************************************************************/

#ifndef __ISP8500_PRIMARY_FW_DATA_H
#define __ISP8500_PRIMARY_FW_DATA_H

#ifdef __cplusplus
    extern "C" {
#endif

#define ISP8500_PRIMARY_FW_DATA_SIZE 8192

extern char Isp8500_primary_fw_data_type[];
extern char Isp8500_primary_fw_data_version[];

extern unsigned char IMPORT_C Isp8500_primary_fw_data[ISP8500_PRIMARY_FW_DATA_SIZE];

#ifdef __cplusplus
    } /* extern C */
#endif

#endif /* __ISP8500_PRIMARY_FW_DATA_H */

