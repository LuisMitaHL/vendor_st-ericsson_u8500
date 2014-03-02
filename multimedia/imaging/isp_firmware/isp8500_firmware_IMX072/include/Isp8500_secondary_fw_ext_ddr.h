
/*****************************************************************************/
/**
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
*/
/*****************************************************************************/

#ifndef __ISP8500_SECONDARY_FW_EXT_DDR_H
#define __ISP8500_SECONDARY_FW_EXT_DDR_H

#ifdef __cplusplus
    extern "C" {
#endif

#define ISP8500_SECONDARY_FW_EXT_DDR_SIZE 262144

extern char Isp8500_secondary_fw_ext_ddr_type[];
extern char Isp8500_secondary_fw_ext_ddr_version[];

extern unsigned char IMPORT_C Isp8500_secondary_fw_ext_ddr[ISP8500_SECONDARY_FW_EXT_DDR_SIZE];

#ifdef __cplusplus
    } /* extern C */
#endif

#endif /* __ISP8500_SECONDARY_FW_EXT_DDR_H */

