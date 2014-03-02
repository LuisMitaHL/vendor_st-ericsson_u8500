
/*****************************************************************************/
/**
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
*/
/*****************************************************************************/

#ifndef __ISP8500_SECONDARY_FW_EXT_H
#define __ISP8500_SECONDARY_FW_EXT_H

#ifdef __cplusplus
    extern "C" {
#endif

#define ISP8500_SECONDARY_FW_EXT_SIZE 61440

extern char Isp8500_secondary_fw_ext_type[];
extern char Isp8500_secondary_fw_ext_version[];

extern unsigned char IMPORT_C Isp8500_secondary_fw_ext[ISP8500_SECONDARY_FW_EXT_SIZE];

#ifdef __cplusplus
    } /* extern C */
#endif

#endif /* __ISP8500_SECONDARY_FW_EXT_H */

