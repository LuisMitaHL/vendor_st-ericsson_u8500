/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 * Description: This file gathers the SIM file paths and
 *              file ID's used by various UICC transaction
 *              handlers.
 */

#define SMSP_USIM_PATH "3F007FFF"
#define SMSP_GSM_PATH "3F007F10"
#define SMSP_FILE_ID 0x6F42

#define PLMN_USIM_PATH "3F007FFF"
#define PLMN_GSM_PATH "3F007F20"
#define PLMN_WACT_FILE_ID 0x6F60
#define PLMN_SEL_FILE_ID 0x6F30

#define GSM_EF_ADN_FILE_ID 0x6F3A
#define GSM_EF_FDN_FILE_ID 0x6F3B

#define SERVICE_TABLE_UST_USIM_PATH "3F007FFF"
#define SERVICE_TABLE_EST_USIM_PATH "3F007FFF"
#define SERVICE_TABLE_GSM_FILE_PATH "3F007F20"

#define USIM_SERVICE_TABLE_UST_FILE_ID 0x6F38
#define USIM_SERVICE_TABLE_EST_FILE_ID 0x6F56
#define GSM_SERVICE_TABLE_FILE_ID 0x6F38

#define FDN_FILE_PATH "3F007F10"
#define ADN_FILE_PATH "3F007F10"

#define EF_RAT_USIM_PATH "3F007F665F30"
#define EF_RAT_ALTERNATE_USIM_PATH "3F007FFF7F665F30"
#define EF_RAT_FILE_ID 0x4F36

/**
 * For Read Subscriber Number Transaction.
 * File Identifiers for Read Subscriber Number Transaction.
 */
#define  UICCD_FILE_ID_FDN    0x6F40
// #define  UICCD_FILE_ID_CCP2   0x6F4F
#define  UICCD_FILE_ID_EXT2   0x6F4B


#define EF_AD_FILE_ID 0x6FAD

#define EF_IMSI_USIM_PATH "3F007FFF"
#define EF_IMSI_GSM_PATH "3F007F20"
#define EF_IMSI_FILE_ID 0x6F07

#define EF_SST_GSM_PATH "3F007F20"
#define EF_SST_FILE_ID 0x6F38
