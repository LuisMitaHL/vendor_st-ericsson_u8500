/*                               -*- Mode: C -*- 
 * Copyright (C) 2010, ST-Ericsson
 * 
 * File name       : sim_file_path.c 
 * Description     : Sim file path structure 
 * 
 * Author          : Stefan Svenberg <stefan.svenberg@stericsson.com>
 * 
 */

/* The file contains df/ef structure according to spec.
 * SIM 51.011, USIM 31.102 UICC w.r.t 102.221. This can e.g. be used
 * to complete a file id with its file path. +CRSM AT command states path
 * is optional, while mal mandates the path. While it is possible for SIM client
 * to supply path at all times, CRSM should be supported. Hence this file.
 *
 * The file is done as follows: Each DF (gsm and usim) is specified in a table
 * of its EFs. Then the proper tables and file paths that apply for usim, and
 * gsm are composed in the appropriate configurations.
 *
 */
#include "mal_uicc.h"
#include "sim_file_path.h"
#include "sim_unused.h"
#include <assert.h>
#include <stdio.h>

#define DF_UNKNOWN               ""                /* Not recognized */

/* Names beginning with df_usim are for usim only, df_gsm for gsm only */
/* TODO: Verify contents towards specs! */
#define DF_MF                    "3F00"
static const uint16_t df_mf[] = {
  0x2F00, // SIM_EF_DIR,
  0x2F05, // SIM_EF_PL,
  0x2F06, // SIM_EF_NOT_SUPPORTED,
  0x2FE2, // SIM_EF_ICC_ID
};
#define DF_MF_SIZE (sizeof(df_mf)/sizeof(*df_mf))

static const char * df_mf_names[DF_MF_SIZE] = {
  "SIM_EF_DIR",
  "SIM_EF_PL",
  "SIM_EF_NOT_SUPPORTED",
  "SIM_EF_ICC_ID"
};

#define DF_USIM_ADF              "3F007FFF"
static const uint16_t df_usim_adf[] = {
  0x6F05, // SIM_EF_LP,
  0x6F06, // SIM_EF_NOT_SUPPORTED,
  0x6F07, // SIM_EF_IMSI,
  0x6F08, // SIM_EF_KEYS,
  0x6F09, // SIM_EF_KEYS_PS,
  0x6F11, // SIM_EF_CPHS_MWF,
  0x6F13, // SIM_EF_CPHS_CFF,
  0x6F14, // SIM_EF_CPHS_OPERATOR_NAME,
  0x6F15, // SIM_EF_CPHS_CSP,
  0x6F16, // SIM_EF_CPHS_INFO,
  0x6F17, // SIM_EF_CPHS_MAILBOX,
  0x6F18, // SIM_EF_CPHS_OPERATOR_NAME_SHORTFORM,
  0x6F19, // SIM_EF_CPHS_INFO_NUMBERS,
  0x6F2C, // SIM_EF_DCK,
  0x6F31, // SIM_EF_HPPLMN,
  0x6F32, // SIM_EF_CNL,
  0x6F37, // SIM_EF_ACM_MAX,
  0x6F38, // SIM_EF_SERVICE_TABLE,
  0x6F39, // SIM_EF_ACM,
  0x6F3B, // SIM_EF_FDN,
  0x6F3C, // SIM_EF_SMS,
  0x6F3E, // SIM_EF_GID_1,
  0x6F3F, // SIM_EF_GID_2,
  0x6F40, // SIM_EF_MSISDN,
  0x6F41, // SIM_EF_PUCT,
  0x6F42, // SIM_EF_SMSP,
  0x6F43, // SIM_EF_SMSS,
  0x6F45, // SIM_EF_CBMI,
  0x6F46, // SIM_EF_SPN,
  0x6F47, // SIM_EF_SMS_STATUS_REPORT,
  0x6F48, // SIM_EF_CBMID,
  0x6F49, // SIM_EF_SDN,
  0x6F4B, // SIM_EF_EXT_2,
  0x6F4C, // SIM_EF_EXT_3,
  0x6F4D, // SIM_EF_BDN,
  0x6F4E, // SIM_EF_EXT_5,
  0x6F4F, // SIM_EF_CCP2_ECCP,
  0x6F50, // SIM_EF_CBMIR,
  0x6F55, // SIM_EF_EXT_4,
  0x6F56, // SIM_EF_EST,
  0x6F57, // SIM_EF_ACL,
  0x6F58, // SIM_EF_CMI,
  0x6F5B, // SIM_EF_START_HFN,
  0x6F5C, // SIM_EF_THRESHOLD,
  0x6F60, // SIM_EF_PLMN_WACT,
  0x6F61, // SIM_EF_OPLMN_WACT,
  0x6F62, // SIM_EF_HPLMN_WACT,
  0x6F73, // SIM_EF_PS_LOCI,
  0x6F78, // SIM_EF_ACC,
  0x6F7B, // SIM_EF_FPLMN,
  0x6F7E, // SIM_EF_LOCI,
  0x6F80, // SIM_EF_ICI,
  0x6F81, // SIM_EF_OCI,
  0x6F82, // SIM_EF_ICT,
  0x6F83, // SIM_EF_OCT,
  0x6FAD, // SIM_EF_AD_USIM,
  0x6FB1, // SIM_EF_VGCS,
  0x6FB2, // SIM_EF_VGCSS,
  0x6FB3, // SIM_EF_VBS,
  0x6FB4, // SIM_EF_VBSS,
  0x6FB5, // SIM_EF_EMLPP,
  0x6FB6, // SIM_EF_AAEM,
  0x6FB7, // SIM_EF_ECC,
  0x6FC3, // SIM_EF_HIDDEN_KEY,
  0x6FC4, // SIM_EF_NETPAR,
  0x6FC5, // SIM_EF_PNN,
  0x6FC6, // SIM_EF_OPL,
  0x6FC7, // SIM_EF_MBDN,
  0x6FC8, // SIM_EF_EXT_6,
  0x6FC9, // SIM_EF_MBI,
  0x6FCA, // SIM_EF_MWIS,
  0x6FCB, // SIM_EF_CFIS,
  0x6FCC, // SIM_EF_EXT_7,
  0x6FCD, // SIM_EF_SPDI,
  0x6FCE, // SIM_EF_MMSN,
  0x6FCF, // SIM_EF_EXT_8,
  0x6FD0, // SIM_EF_MMS_ICP,
  0x6FD1, // SIM_EF_MMS_UP,
  0x6FD2, // SIM_EF_MMS_UCP,
  0x6FD3, // SIM_EF_NIA,
  0x6FD4, // SIM_EF_VGCSCA,
  0x6FD5, // SIM_EF_VBSCA,
  0x6FD6, // SIM_EF_GBA_BP_USIM,
  0x6FD7, // SIM_EF_MSK,
  0x6FD8, // SIM_EF_MUK,
  0x6FD9, // SIM_EF_EQUIVALENT_HPLMN,
  0x6FDA, // SIM_EF_GBA_NL
};

// By this define, compiler complains if sizes do not match.
#define DF_USIM_ADF_SIZE (sizeof(df_usim_adf)/sizeof(*df_usim_adf))

static const char * const df_usim_adf_names[DF_USIM_ADF_SIZE] = {
  "SIM_EF_LP",
  "SIM_EF_NOT_SUPPORTED",
  "SIM_EF_IMSI",
  "SIM_EF_KEYS",
  "SIM_EF_KEYS_PS",
  "SIM_EF_CPHS_MWF",
  "SIM_EF_CPHS_CFF",
  "SIM_EF_CPHS_OPERATOR_NAME",
  "SIM_EF_CPHS_CSP",
  "SIM_EF_CPHS_INFO",
  "SIM_EF_CPHS_MAILBOX",
  "SIM_EF_CPHS_OPERATOR_NAME_SHORTFORM",
  "SIM_EF_CPHS_INFO_NUMBERS",
  "SIM_EF_DCK",
  "SIM_EF_HPPLMN",
  "SIM_EF_CNL",
  "SIM_EF_ACM_MAX",
  "SIM_EF_SERVICE_TABLE",
  "SIM_EF_ACM",
  "SIM_EF_FDN",
  "SIM_EF_SMS",
  "SIM_EF_GID_1",
  "SIM_EF_GID_2",
  "SIM_EF_MSISDN",
  "SIM_EF_PUCT",
  "SIM_EF_SMSP",
  "SIM_EF_SMSS",
  "SIM_EF_CBMI",
  "SIM_EF_SPN",
  "SIM_EF_SMS_STATUS_REPORT",
  "SIM_EF_CBMID",
  "SIM_EF_SDN",
  "SIM_EF_EXT_2",
  "SIM_EF_EXT_3",
  "SIM_EF_BDN",
  "SIM_EF_EXT_5",
  "SIM_EF_CCP2_ECCP",
  "SIM_EF_CBMIR",
  "SIM_EF_EXT_4",
  "SIM_EF_EST",
  "SIM_EF_ACL",
  "SIM_EF_CMI",
  "SIM_EF_START_HFN",
  "SIM_EF_THRESHOLD",
  "SIM_EF_PLMN_WACT",
  "SIM_EF_OPLMN_WACT",
  "SIM_EF_HPLMN_WACT",
  "SIM_EF_PS_LOCI",
  "SIM_EF_ACC",
  "SIM_EF_FPLMN",
  "SIM_EF_LOCI",
  "SIM_EF_ICI",
  "SIM_EF_OCI",
  "SIM_EF_ICT",
  "SIM_EF_OCT",
  "SIM_EF_AD_USIM",
  "SIM_EF_VGCS",
  "SIM_EF_VGCSS",
  "SIM_EF_VBS",
  "SIM_EF_VBSS",
  "SIM_EF_EMLPP",
  "SIM_EF_AAEM",
  "SIM_EF_ECC",
  "SIM_EF_HIDDEN_KEY",
  "SIM_EF_NETPAR",
  "SIM_EF_PNN",
  "SIM_EF_OPL",
  "SIM_EF_MBDN",
  "SIM_EF_EXT_6",
  "SIM_EF_MBI",
  "SIM_EF_MWIS",
  "SIM_EF_CFIS",
  "SIM_EF_EXT_7",
  "SIM_EF_SPDI",
  "SIM_EF_MMSN",
  "SIM_EF_EXT_8",
  "SIM_EF_MMS_ICP",
  "SIM_EF_MMS_UP",
  "SIM_EF_MMS_UCP",
  "SIM_EF_NIA",
  "SIM_EF_VGCSCA",
  "SIM_EF_VBSCA",
  "SIM_EF_GBA_BP_USIM",
  "SIM_EF_MSK",
  "SIM_EF_MUK",
  "SIM_EF_EQUIVALENT_HPLMN",
  "SIM_EF_GBA_NL"
};

#define DF_USIM_PHONEBOOK_LOCAL  "3F007FFF5F3A"
static const uint16_t df_usim_phonebook_local[] = {
  0x4F22, // SIM_EF_LOCAL_PHONEBOOK_PSC,
  0x4F23, // SIM_EF_LOCAL_PHONEBOOK_CC,
  0x4F24, // SIM_EF_LOCAL_PHONEBOOK_PUID,
  0x4F30, // SIM_EF_LOCAL_PHONEBOOK_PBR
};

#define DF_USIM_PHONEBOOK_LOCAL_SIZE (sizeof(df_usim_phonebook_local)/sizeof(*df_usim_phonebook_local))

static const char * const df_usim_phonebook_local_names[] = {
  "SIM_EF_LOCAL_PHONEBOOK_PSC",
  "SIM_EF_LOCAL_PHONEBOOK_CC",
  "SIM_EF_LOCAL_PHONEBOOK_PUID",
  "SIM_EF_LOCAL_PHONEBOOK_PBR"
};

#define DF_USIM_GSM_ACCESS       "3F007FFF5F3B"
static const uint16_t df_usim_gsm_access[] = {
  0x4F20, // SIM_EF_KC,
  0x4F52, // SIM_EF_KC_GPRS,
  0x4F63, // SIM_EF_CPBCCH,
  0x4F64, // SIM_EF_INVSCAN
};

#define DF_USIM_GSM_ACCESS_SIZE (sizeof(df_usim_gsm_access)/sizeof(*df_usim_gsm_access))

static const char * const df_usim_gsm_access_names[DF_USIM_GSM_ACCESS_SIZE] = {
  "SIM_EF_KC",
  "SIM_EF_KC_GPRS",
  "SIM_EF_CPBCCH",
  "SIM_EF_INVSCAN"
};

#define DF_USIM_MEXE             "3F007FFF5F3C"
#define DF_GSM_MEXE              "7F205F3C" /* or 7F21 5F3C */
static const uint16_t df_mexe[] = {
  0x4F40, // SIM_EF_MEXE_ST,
  0x4F41, // SIM_EF_MEXE_ORPK,
  0x4F42, // SIM_EF_MEXE_ARPK,
  0x4F43, // SIM_EF_MEXE_TPRPK
};

#define DF_MEXE_SIZE (sizeof(df_mexe)/sizeof(*df_mexe))

static const char * const df_mexe_names[DF_MEXE_SIZE] = {
  "SIM_EF_MEXE_ST",
  "SIM_EF_MEXE_ORPK",
  "SIM_EF_MEXE_ARPK",
  "SIM_EF_MEXE_TPRPK"
};

#define DF_USIM_WLAN             "3F007FFF5F40"
static const uint16_t df_usim_wlan[] = {
  0x4F41, // SIM_EF_WLAN_PSEUDONYM,
  0x4F42, // SIM_EF_WLAN_USER_PLMN,
  0x4F43, // SIM_EF_WLAN_OPERATOR_PLMN,
  0x4F44, // SIM_EF_WLAN_USER_SPECIFIC_ID_LIST,
  0x4F45, // SIM_EF_WLAN_OPERATOR_SPECIFIC_ID_LIST,
  0x4F46, // SIM_EF_WLAN_REAUTHENTICATION_ID
};
#define DF_USIM_WLAN_SIZE (sizeof(df_usim_wlan)/sizeof(*df_usim_wlan))

static const char * const df_usim_wlan_names[DF_USIM_WLAN_SIZE] = {
  "SIM_EF_WLAN_PSEUDONYM",
  "SIM_EF_WLAN_USER_PLMN",
  "SIM_EF_WLAN_OPERATOR_PLMN",
  "SIM_EF_WLAN_USER_SPECIFIC_ID_LIST",
  "SIM_EF_WLAN_OPERATOR_SPECIFIC_ID_LIST",
  "SIM_EF_WLAN_REAUTHENTICATION_ID"
};

#define DF_USIM_SOLSA            "3F007FFF5F70"
#define DF_GSM_SOLSA             "7F205F70"/* or 7F21 5F70 */

static const uint16_t df_solsa[] = {
  0x4F30, // SIM_EF_SOLSA_ACCESS_INDICATOR,
  0x4F31, // SIM_EF_SOLSA_LSA_LIST
};

#define DF_SOLSA_SIZE (sizeof(df_solsa)/sizeof(*df_solsa))

static const char * const df_solsa_names[DF_SOLSA_SIZE] = {
  "SIM_EF_SOLSA_ACCESS_INDICATOR",
  "SIM_EF_SOLSA_LSA_LIST"
};

#define DF_TELECOM               "3F007F10"
static const uint16_t df_telecom[] = {
  0x6F06, // SIM_EF_NOT_SUPPORTED,
  0x6F3A, // SIM_EF_ADN,
  0x6F3B, // SIM_EF_FDN,
  0x6F3C, // SIM_EF_SMS,
  0x6F3D, // SIM_EF_CCP,
  0x6F40, // SIM_EF_MSISDN,
  0x6F42, // SIM_EF_SMSP,
  0x6F43, // SIM_EF_SMSS,
  0x6F44, // SIM_EF_LND,
  0x6F47, // SIM_EF_SMS_STATUS_REPORT,
  0x6F49, // SIM_EF_SDN,
  0x6F4A, // SIM_EF_EXT_1,
  0x6F4B, // SIM_EF_EXT_2,
  0x6F4C, // SIM_EF_EXT_3,
  0x6F4D, // SIM_EF_BDN,
  0x6F4E, // SIM_EF_EXT_4,
  0x6F4F, // SIM_EF_CCP2_ECCP,
  0x6F54, // SIM_EF_NOT_SUPPORTED,
  0x6F58, // SIM_EF_CMI
};

#define DF_TELECOM_SIZE (sizeof(df_telecom)/sizeof(*df_telecom))

static const char * const df_telecom_names[DF_TELECOM_SIZE] = {
  "SIM_EF_NOT_SUPPORTED",
  "SIM_EF_ADN",
  "SIM_EF_FDN",
  "SIM_EF_SMS",
  "SIM_EF_CCP",
  "SIM_EF_MSISDN",
  "SIM_EF_SMSP",
  "SIM_EF_SMSS",
  "SIM_EF_LND",
  "SIM_EF_SMS_STATUS_REPORT",
  "SIM_EF_SDN",
  "SIM_EF_EXT_1",
  "SIM_EF_EXT_2",
  "SIM_EF_EXT_3",
  "SIM_EF_BDN",
  "SIM_EF_EXT_4",
  "SIM_EF_CCP2_ECCP",
  "SIM_EF_NOT_SUPPORTED",
  "SIM_EF_CMI"
};

#define DF_USIM_PHONEBOOK_GLOBAL "3F007F105F3A"

static const uint16_t df_usim_phonebook_global[] = {
  0x4F22, // SIM_EF_GLOBAL_PHONEBOOK_PSC,
  0x4F23, // SIM_EF_GLOBAL_PHONEBOOK_CC,
  0x4F24, // SIM_EF_GLOBAL_PHONEBOOK_PUID,
  0x4F30, // SIM_EF_GLOBAL_PHONEBOOK_PBR
};

#define DF_USIM_PHONEBOOK_GLOBAL_SIZE (sizeof(df_usim_phonebook_global)/sizeof(*df_usim_phonebook_global))

static const char * const df_usim_phonebook_global_names[DF_USIM_PHONEBOOK_GLOBAL_SIZE] = {
  "SIM_EF_GLOBAL_PHONEBOOK_PSC",
  "SIM_EF_GLOBAL_PHONEBOOK_CC",
  "SIM_EF_GLOBAL_PHONEBOOK_PUID",
  "SIM_EF_GLOBAL_PHONEBOOK_PBR"
};

#define DF_USIM_MULTIMEDIA       "3F007F105F3B"

static const uint16_t df_usim_multimedia[] = {
  0x4F47, // SIM_EF_NOT_SUPPORTED,
  0x4F48, // SIM_EF_NOT_SUPPORTED
};

#define DF_USIM_MULTIMEDIA_SIZE (sizeof(df_usim_multimedia)/sizeof(*df_usim_multimedia))

static const char * const df_usim_multimedia_names[DF_USIM_MULTIMEDIA_SIZE] = {
  "SIM_EF_NOT_SUPPORTED",
  "SIM_EF_NOT_SUPPORTED"
};

#define DF_GRAPHICS              "3F007F105F50"
static const uint16_t df_graphics[] = {
  0x4F20 // SIM_EF_IMAGE_FILE
};

#define DF_GRAPHICS_SIZE (sizeof(df_graphics)/sizeof(*df_graphics))

static const char * const df_graphics_names[DF_GRAPHICS_SIZE] = {
  "SIM_EF_IMAGE_FILE"
};

#define DF_GSM                   "3F007F20" /* or 7F21 */
static const uint16_t df_gsm[] = {
  0x6F05, // SIM_EF_LP,
  0x6F07, // SIM_EF_IMSI,
  0x6F11, // SIM_EF_CPHS_MWF,
  0x6F13, // SIM_EF_CPHS_CFF,
  0x6F14, // SIM_EF_CPHS_OPERATOR_NAME,
  0x6F15, // SIM_EF_CPHS_CSP,
  0x6F16, // SIM_EF_CPHS_INFO,
  0x6F17, // SIM_EF_CPHS_MAILBOX,
  0x6F18, // SIM_EF_CPHS_OPERATOR_NAME_SHORTFORM,
  0x6F19, // SIM_EF_CPHS_INFO_NUMBERS,
  0x6F20, // SIM_EF_KC,
  0x6F2C, // SIM_EF_DCK,
  0x6F30, // SIM_EF_PLMN_SEL,
  0x6F31, // SIM_EF_HPPLMN,
  0x6F32, // SIM_EF_CNL,
  0x6F37, // SIM_EF_ACM_MAX,
  0x6F38, // SIM_EF_SERVICE_TABLE,
  0x6F39, // SIM_EF_ACM,
  0x6F3E, // SIM_EF_GID_1,
  0x6F3F, // SIM_EF_GID_2,
  0x6F41, // SIM_EF_PUCT,
  0x6F45, // SIM_EF_CBMI,
  0x6F46, // SIM_EF_SPN,
  0x6F48, // SIM_EF_CBMID,
  0x6F50, // SIM_EF_CBMIR,
  0x6F51, // SIM_EF_NIA,
  0x6F52, // SIM_EF_KC_GPRS,
  0x6F53, // SIM_EF_LOCI_GPRS,
  0x6F54, // SIM_EF_NOT_SUPPORTED,
  0x6F60, // SIM_EF_PLMN_WACT,
  0x6F61, // SIM_EF_OPLMN_WACT,
  0x6F62, // SIM_EF_HPLMN_WACT,
  0x6F63, // SIM_EF_CPBCCH,
  0x6F64, // SIM_EF_INVSCAN,
  0x6F74, // SIM_EF_BCCH,
  0x6F78, // SIM_EF_ACC,
  0x6F7B, // SIM_EF_FPLMN,
  0x6F7E, // SIM_EF_LOCI,
  0x6FAD, // SIM_EF_AD_USIM,
  0x6FAE, // SIM_EF_PHASE,
  0x6FB1, // SIM_EF_VGCS,
  0x6FB2, // SIM_EF_VGCSS,
  0x6FB3, // SIM_EF_VBS,
  0x6FB4, // SIM_EF_VBSS,
  0x6FB5, // SIM_EF_EMLPP,
  0x6FB6, // SIM_EF_AAEM,
  0x6FB7, // SIM_EF_ECC,
  0x6FC5, // SIM_EF_PNN,
  0x6FC6, // SIM_EF_OPL,
  0x6FC7, // SIM_EF_MBDN,
  0x6FC8, // SIM_EF_EXT_6,
  0x6FC9, // SIM_EF_MBI,
  0x6FCA, // SIM_EF_MWIS,
  0x6FCB, // SIM_EF_CFIS,
  0x6FCC, // SIM_EF_EXT_7,
  0x6FCD, // SIM_EF_SPDI,
  0x6FCE, // SIM_EF_MMSN,
  0x6FCF, // SIM_EF_EXT_8,
  0x6FD0, // SIM_EF_MMS_ICP,
  0x6FD1, // SIM_EF_MMS_UP,
  0x6FD2 // SIM_EF_MMS_UCP,
};

#define DF_GSM_SIZE (sizeof(df_gsm)/sizeof(*df_gsm))

static const char * const df_gsm_names[DF_GSM_SIZE] = {
  "SIM_EF_LP",
  "SIM_EF_IMSI",
  "SIM_EF_CPHS_MWF",
  "SIM_EF_CPHS_CFF",
  "SIM_EF_CPHS_OPERATOR_NAME",
  "SIM_EF_CPHS_CSP",
  "SIM_EF_CPHS_INFO",
  "SIM_EF_CPHS_MAILBOX",
  "SIM_EF_CPHS_OPERATOR_NAME_SHORTFORM",
  "SIM_EF_CPHS_INFO_NUMBERS",
  "SIM_EF_KC",
  "SIM_EF_DCK",
  "SIM_EF_PLMN_SEL",
  "SIM_EF_HPPLMN",
  "SIM_EF_CNL",
  "SIM_EF_ACM_MAX",
  "SIM_EF_SERVICE_TABLE",
  "SIM_EF_ACM",
  "SIM_EF_GID_1",
  "SIM_EF_GID_2",
  "SIM_EF_PUCT",
  "SIM_EF_CBMI",
  "SIM_EF_SPN",
  "SIM_EF_CBMID",
  "SIM_EF_CBMIR",
  "SIM_EF_NIA",
  "SIM_EF_KC_GPRS",
  "SIM_EF_LOCI_GPRS",
  "SIM_EF_NOT_SUPPORTED",
  "SIM_EF_PLMN_WACT",
  "SIM_EF_OPLMN_WACT",
  "SIM_EF_HPLMN_WACT",
  "SIM_EF_CPBCCH",
  "SIM_EF_INVSCAN",
  "SIM_EF_BCCH",
  "SIM_EF_ACC",
  "SIM_EF_FPLMN",
  "SIM_EF_LOCI",
  "SIM_EF_AD_USIM",
  "SIM_EF_PHASE",
  "SIM_EF_VGCS",
  "SIM_EF_VGCSS",
  "SIM_EF_VBS",
  "SIM_EF_VBSS",
  "SIM_EF_EMLPP",
  "SIM_EF_AAEM",
  "SIM_EF_ECC",
  "SIM_EF_PNN",
  "SIM_EF_OPL",
  "SIM_EF_MBDN",
  "SIM_EF_EXT_6",
  "SIM_EF_MBI",
  "SIM_EF_MWIS",
  "SIM_EF_CFIS",
  "SIM_EF_EXT_7",
  "SIM_EF_SPDI",
  "SIM_EF_MMSN",
  "SIM_EF_EXT_8",
  "SIM_EF_MMS_ICP",
  "SIM_EF_MMS_UP",
  "SIM_EF_MMS_UCP"
};

static const uint16_t * const sim_gsm_ef_tables[] = {
  df_mf,
  df_mexe,
  df_solsa,
  df_telecom,
  df_graphics,
  df_gsm
};
#define SIM_GSM_EF_TABLES_SIZE (sizeof(sim_gsm_ef_tables)/sizeof(*sim_gsm_ef_tables))

static const char * const * const sim_gsm_ef_names_tables[] = {
  df_mf_names,
  df_mexe_names,
  df_solsa_names,
  df_telecom_names,
  df_graphics_names,
  df_gsm_names
};

static const size_t sim_gsm_ef_tables_size[SIM_GSM_EF_TABLES_SIZE] = {
  DF_MF_SIZE,
  DF_MEXE_SIZE,
  DF_SOLSA_SIZE,
  DF_TELECOM_SIZE,
  DF_GRAPHICS_SIZE,
  DF_GSM_SIZE
};

static const char * const sim_gsm_df_file_paths[SIM_GSM_EF_TABLES_SIZE] = {
  DF_MF,
  DF_GSM_MEXE,
  DF_GSM_SOLSA,
  DF_TELECOM,
  DF_GRAPHICS,
  DF_GSM
};

static const uint16_t * const sim_usim_ef_tables[] = {
  df_mf,
  df_usim_adf,
  df_usim_phonebook_local,
  df_usim_gsm_access,
  df_mexe,
  df_usim_wlan,
  df_solsa,
  df_telecom,
  df_usim_phonebook_global,
  df_usim_multimedia,
  df_graphics
};

#define SIM_USIM_EF_TABLES_SIZE (sizeof(sim_usim_ef_tables)/sizeof(*sim_usim_ef_tables))

static const char * const * const sim_usim_ef_names_tables[SIM_USIM_EF_TABLES_SIZE] = {
  df_mf_names,
  df_usim_adf_names,
  df_usim_phonebook_local_names,
  df_usim_gsm_access_names,
  df_mexe_names,
  df_usim_wlan_names,
  df_solsa_names,
  df_telecom_names,
  df_usim_phonebook_global_names,
  df_usim_multimedia_names,
  df_graphics_names
};

static const size_t sim_usim_ef_tables_size[SIM_USIM_EF_TABLES_SIZE] = {
  DF_MF_SIZE,
  DF_USIM_ADF_SIZE,
  DF_USIM_PHONEBOOK_LOCAL_SIZE,
  DF_USIM_GSM_ACCESS_SIZE,
  DF_MEXE_SIZE,
  DF_USIM_WLAN_SIZE,
  DF_SOLSA_SIZE,
  DF_TELECOM_SIZE,
  DF_USIM_PHONEBOOK_GLOBAL_SIZE,
  DF_USIM_MULTIMEDIA_SIZE,
  DF_GRAPHICS_SIZE
};

static const char * const sim_usim_df_file_paths[SIM_USIM_EF_TABLES_SIZE] = {
  DF_MF,
  DF_USIM_ADF,
  DF_USIM_PHONEBOOK_LOCAL,
  DF_USIM_GSM_ACCESS,
  DF_USIM_MEXE,
  DF_USIM_WLAN,
  DF_USIM_SOLSA,
  DF_TELECOM,
  DF_USIM_PHONEBOOK_GLOBAL,
  DF_USIM_MULTIMEDIA,
  DF_GRAPHICS
};

static int some_path_in_table(file_fn_t *fn, void *user_data,
                              const uint16_t table[],
                              const char * const names[],
                              const char *path,
                              int len)
{
  int i;
  for (i = 0; i<len; i++) {
    if (fn(user_data, table[i], names[i], path))
      return 1;
  }
  return 0;
}

static int some_file_path(file_fn_t *fn, void *user_data,
                          const uint16_t * const tables[],
                          const char * const * const names[],
                          size_t size,
                          const size_t sizes[],
                          const char * const paths[])
{
  int i, len = (int)size;
  for (i = 0; i<len; i++) {
    if (some_path_in_table(fn, user_data, tables[i], names[i], paths[i], sizes[i]))
      return 1;
  }
  return 0;
}

// This uses types from MAL.
int sim_file_some_path(file_fn_t *fn, void *user_data,
                       uint8_t app_type) {
  int res = 0;
  switch(app_type) {
  case MAL_UICC_APPL_TYPE_ICC_SIM:
    res = some_file_path(fn, user_data, sim_gsm_ef_tables,
                         sim_gsm_ef_names_tables,
                         SIM_GSM_EF_TABLES_SIZE,
                         sim_gsm_ef_tables_size, sim_gsm_df_file_paths);
    break;
  case MAL_UICC_APPL_TYPE_UICC_USIM:
    res = some_file_path(fn, user_data, sim_usim_ef_tables,
                         sim_usim_ef_names_tables,
                         SIM_USIM_EF_TABLES_SIZE,
                         sim_usim_ef_tables_size,
                         sim_usim_df_file_paths);
    break;
  default:
    break;
  }
  return res;
}




#if 0
static const struct {
  int nr;
  const char *name;
  const char *usim_file_path;
  int usim_file_id;
  const char *gsm_file_path;
  int gsm_file_id;
} const sim_ef[] = {
  {0, "EF_NONE", "", 0, "", 0},
  {1, "EF_DIR", "3F00", 0x2F00, "3F00", 0x2F00},
  {2, "EF_PL", "3F00", 0x2F05, "3F00", 0x2F05},
  {3, "EF_ICC_ID", "3F00", 0x2FE2, "3F00", 0x2FE2},
  {4, "EF_LP", "3F007FFF", 0x6F05, "3F007F20", 0x6F05},
  {5, "EF_IMSI", "3F007FFF", 0x6F07, "3F007F20", 0x6F07},
  {6, "EF_KEYS", "3F007FFF", 0x6F08, "", 0x0},
  {7, "EF_KEYS_PS", "3F007FFF", 0x6F09, "", 0x0},
  {8, "EF_CPHS_MWF", "3F007FFF", 0x6F11, "3F007F20", 0x6F11},
  {9, "EF_CPHS_CFF", "3F007FFF", 0x6F13, "3F007F20", 0x6F13},
  {10, "EF_CPHS_OPERATOR_NAME", "3F007FFF", 0x6F14, "3F007F20", 0x6F14},
  {11, "EF_CPHS_CSP", "3F007FFF", 0x6F15, "3F007F20", 0x6F15},
  {12, "EF_CPHS_INFO", "3F007FFF", 0x6F16, "3F007F20", 0x6F16},
  {13, "EF_CPHS_MAILBOX", "3F007FFF", 0x6F17, "3F007F20", 0x6F17},
  {14, "EF_CPHS_OPERATOR_NAME_SHORTFORM", "3F007FFF", 0x6F18, "3F007F20", 0x6F18},
  {15, "EF_CPHS_INFO_NUMBERS", "3F007FFF", 0x6F19, "3F007F20", 0x6F19},
  {16, "EF_KC", "3F007FFF5F3B", 0x4F20, "3F007F20", 0x6F20},
  {17, "EF_DCK", "3F007FFF", 0x6F2C, "3F007F20", 0x6F2C},
  {18, "EF_PLMN_SEL", "", 0x0, "3F007F20", 0x6F30},
  {19, "EF_HPPLMN", "3F007FFF", 0x6F31, "3F007F20", 0x6F31},
  {20, "EF_CNL", "3F007FFF", 0x6F32, "3F007F20", 0x6F32},
  {21, "EF_ACM_MAX", "3F007FFF", 0x6F37, "3F007F20", 0x6F37},
  {22, "EF_SERVICE_TABLE", "3F007FFF", 0x6F38, "3F007F20", 0x6F38},
  {23, "EF_ACM", "3F007FFF", 0x6F39, "3F007F20", 0x6F39},
  {24, "EF_ADN", "3F007F10", 0x6F3A, "3F007F10", 0x6F3A},
  {25, "EF_FDN", "3F007FFF", 0x6F3B, "3F007F10", 0x6F3B},
  {26, "EF_SMS", "3F007FFF", 0x6F3C, "3F007F10", 0x6F3C},
  {27, "EF_CCP", "", 0x0, "3F007F10", 0x6F3D},
  {28, "EF_GID_1", "3F007FFF", 0x6F3E, "3F007F20", 0x6F3E},
  {29, "EF_GID_2", "3F007FFF", 0x6F3F, "3F007F20", 0x6F3F},
  {30, "EF_MSISDN", "3F007FFF", 0x6F40, "3F007F10", 0x6F40},
  {31, "EF_PUCT", "3F007FFF", 0x6F41, "3F007F20", 0x6F41},
  {32, "EF_SMSP", "3F007FFF", 0x6F42, "3F007F10", 0x6F42},
  {33, "EF_SMSS", "3F007FFF", 0x6F43, "3F007F10", 0x6F43},
  {34, "EF_LND", "", 0x0, "3F007F10", 0x6F44},
  {35, "EF_CBMI", "3F007FFF", 0x6F45, "3F007F20", 0x6F45},
  {36, "EF_SPN", "3F007FFF", 0x6F46, "3F007F20", 0x6F46},
  {37, "EF_SMS_STATUS_REPORT", "3F007FFF", 0x6F47, "3F007F10", 0x6F47},
  {38, "EF_CBMID", "3F007FFF", 0x6F48, "3F007F20", 0x6F48},
  {39, "EF_SDN", "3F007FFF", 0x6F49, "3F007F10", 0x6F49},
  {40, "EF_EXT_1", "3F007F10", 0x6F4A, "3F007F10", 0x6F4A},
  {41, "EF_EXT_2", "3F007FFF", 0x6F4B, "3F007F10", 0x6F4B},
  {42, "EF_EXT_3", "3F007FFF", 0x6F4C, "3F007F10", 0x6F4C},
  {43, "EF_BDN", "3F007FFF", 0x6F4D, "3F007F10", 0x6F4D},
  {44, "EF_EXT_5", "3F007FFF", 0x6F4E, "", 0x0},
  {45, "EF_CCP2_ECCP", "3F007F10", 0x6F4F, "3F007F10", 0x6F4F},
  {46, "EF_CBMIR", "3F007FFF", 0x6F50, "3F007F20", 0x6F50},
  {47, "EF_KC_GPRS", "3F007FFF5F3B", 0x4F52, "3F007F20", 0x6F52},
  {48, "EF_LOCI_GPRS", "", 0x0, "3F007F20", 0x6F53},
  {49, "EF_EXT_4", "3F007FFF", 0x6F55, "3F007F10", 0x6F4E},
  {50, "EF_EST", "3F007FFF", 0x6F56, "", 0x0},
  {51, "EF_ACL", "3F007FFF", 0x6F57, "", 0x0},
  {52, "EF_CMI", "3F007FFF", 0x6F58, "3F007F10", 0x6F58},
  {53, "EF_START_HFN", "3F007FFF", 0x6F5B, "", 0x0},
  {54, "EF_THRESHOLD", "3F007FFF", 0x6F5C, "", 0x0},
  {55, "EF_PLMN_WACT", "3F007FFF", 0x6F60, "3F007F20", 0x6F60},
  {56, "EF_OPLMN_WACT", "3F007FFF", 0x6F61, "3F007F20", 0x6F61},
  {57, "EF_HPLMN_WACT", "3F007FFF", 0x6F62, "3F007F20", 0x6F62},
  {58, "EF_CPBCCH", "3F007FFF5F3B", 0x4F63, "3F007F20", 0x6F63},
  {59, "EF_INVSCAN", "3F007FFF5F3B", 0x4F64, "3F007F20", 0x6F64},
  {60, "EF_PS_LOCI", "3F007FFF", 0x6F73, "", 0x0},
  {61, "EF_BCCH", "", 0x0, "3F007F20", 0x6F74},
  {62, "EF_ACC", "3F007FFF", 0x6F78, "3F007F20", 0x6F78},
  {63, "EF_FPLMN", "3F007FFF", 0x6F7B, "3F007F20", 0x6F7B},
  {64, "EF_LOCI", "3F007FFF", 0x6F7E, "3F007F20", 0x6F7E},
  {65, "EF_ICI", "3F007FFF", 0x6F80, "", 0x0},
  {66, "EF_OCI", "3F007FFF", 0x6F81, "", 0x0},
  {67, "EF_ICT", "3F007FFF", 0x6F82, "", 0x0},
  {68, "EF_OCT", "3F007FFF", 0x6F83, "", 0x0},
  {69, "EF_AD_USIM", "3F007FFF", 0x6FAD, "3F007F20", 0x6FAD},
  {70, "EF_PHASE", "", 0x0, "3F007F20", 0x6FAE},
  {71, "EF_VGCS", "3F007FFF", 0x6FB1, "3F007F20", 0x6FB1},
  {72, "EF_VGCSS", "3F007FFF", 0x6FB2, "3F007F20", 0x6FB2},
  {73, "EF_VBS", "3F007FFF", 0x6FB3, "3F007F20", 0x6FB3},
  {74, "EF_VBSS", "3F007FFF", 0x6FB4, "3F007F20", 0x6FB4},
  {75, "EF_EMLPP", "3F007FFF", 0x6FB5, "3F007F20", 0x6FB5},
  {76, "EF_AAEM", "3F007FFF", 0x6FB6, "3F007F20", 0x6FB6},
  {77, "EF_ECC", "3F007FFF", 0x6FB7, "3F007F20", 0x6FB7},
  {78, "EF_HIDDEN_KEY", "3F007FFF", 0x6FC3, "", 0x0},
  {79, "EF_NETPAR", "3F007FFF", 0x6FC4, "", 0x0},
  {80, "EF_PNN", "3F007FFF", 0x6FC5, "3F007F20", 0x6FC5},
  {81, "EF_OPL", "3F007FFF", 0x6FC6, "3F007F20", 0x6FC6},
  {82, "EF_MBDN", "3F007FFF", 0x6FC7, "3F007F20", 0x6FC7},
  {83, "EF_EXT_6", "3F007FFF", 0x6FC8, "3F007F20", 0x6FC8},
  {84, "EF_MBI", "3F007FFF", 0x6FC9, "3F007F20", 0x6FC9},
  {85, "EF_MWIS", "3F007FFF", 0x6FCA, "3F007F20", 0x6FCA},
  {86, "EF_CFIS", "3F007FFF", 0x6FCB, "3F007F20", 0x6FCB},
  {87, "EF_EXT_7", "3F007FFF", 0x6FCC, "3F007F20", 0x6FCC},
  {88, "EF_SPDI", "3F007FFF", 0x6FCD, "3F007F20", 0x6FCD},
  {89, "EF_MMSN", "3F007FFF", 0x6FCE, "3F007F20", 0x6FCE},
  {90, "EF_EXT_8", "3F007FFF", 0x6FCF, "3F007F20", 0x6FCF},
  {91, "EF_MMS_ICP", "3F007FFF", 0x6FD0, "3F007F20", 0x6FD0},
  {92, "EF_MMS_UP", "3F007FFF", 0x6FD1, "3F007F20", 0x6FD1},
  {93, "EF_MMS_UCP", "3F007FFF", 0x6FD2, "3F007F20", 0x6FD2},
  {94, "EF_NIA", "3F007FFF", 0x6FD3, "3F007F20", 0x6F51},
  {95, "EF_VGCSCA", "3F007FFF", 0x6FD4, "", 0x0},
  {96, "EF_VBSCA", "3F007FFF", 0x6FD5, "", 0x0},
  {97, "EF_GBA_BP_USIM", "3F007FFF", 0x6FD6, "", 0x0},
  {98, "EF_MSK", "3F007FFF", 0x6FD7, "", 0x0},
  {99, "EF_MUK", "3F007FFF", 0x6FD8, "", 0x0},
  {100, "EF_EQUIVALENT_HPLMN", "3F007FFF", 0x6FD9, "", 0x0},
  {101, "EF_GBA_NL", "3F007FFF", 0x6FDA, "", 0x0},
  {102, "EF_ZONE_CELL_INFO", "", 0x0, "3F007F20", 0xEA00},
  {103, "EF_INFO_NR", "", 0x0, "3F007F10", 0xEA01},
  {104, "EF_LOCAL_CALLING_DETAILS", "", 0x0, "3F007F20", 0xEA03},
  {105, "EF_LOCAL_PHONEBOOK_PSC", "3F007FFF5F3A", 0x4F22, "", 0x0},
  {106, "EF_LOCAL_PHONEBOOK_CC", "3F007FFF5F3A", 0x4F23, "", 0x0},
  {107, "EF_LOCAL_PHONEBOOK_PUID", "3F007FFF5F3A", 0x4F24, "", 0x0},
  {108, "EF_LOCAL_PHONEBOOK_PBR", "3F007FFF5F3A", 0x4F30, "", 0x0},
  {109, "EF_LOCAL_PHONEBOOK_FILE", "3F007FFF5F3A", 0x4FXX, "", 0x0},
  {110, "EF_MEXE_ST", "3F007FFF5F3C", 0x4F40, "3F007F205F3C", 0x4F40},
  {111, "EF_MEXE_ORPK", "3F007FFF5F3C", 0x4F41, "3F007F205F3C", 0x4F41},
  {112, "EF_MEXE_ARPK", "3F007FFF5F3C", 0x4F42, "3F007F205F3C", 0x4F42},
  {113, "EF_MEXE_TPRPK", "3F007FFF5F3C", 0x4F43, "3F007F205F3C", 0x4F43},
  {114, "EF_MEXE_DATA_FILE", "3F007FFF5F3C", 0x4FXX, "3F007F205F3C", 0x4FXX},
  {115, "EF_WLAN_PSEUDONYM", "3F007FFF5F40", 0x4F41, "", 0x0},
  {116, "EF_WLAN_USER_PLMN", "3F007FFF5F40", 0x4F42, "", 0x0},
  {117, "EF_WLAN_OPERATOR_PLMN", "3F007FFF5F40", 0x4F43, "", 0x0},
  {118, "EF_WLAN_USER_SPECIFIC_ID_LIST", "3F007FFF5F40", 0x4F44, "", 0x0},
  {119, "EF_WLAN_OPERATOR_SPECIFIC_ID_LIST", "3F007FFF5F40", 0x4F45, "", 0x0},
  {120, "EF_WLAN_REAUTHENTICATION_ID", "3F007FFF5F40", 0x4F46, "", 0x0},
  {121, "EF_SOLSA_ACCESS_INDICATOR", "3F007FFF5F70", 0x4F30, "3F007F205F70", 0x4F30},
  {122, "EF_SOLSA_LSA_LIST", "3F007FFF5F70", 0x4F31, "3F007F205F70", 0x4F31},
  {123, "EF_SOLSA_LSA_DESCRIPTOR", "3F007FFF5F70", 0x4FXX, "3F007F205F70", 0x4FXX},
  {124, "EF_GLOBAL_PHONEBOOK_PSC", "3F007F105F3A", 0x4F22, "", 0x0},
  {125, "EF_GLOBAL_PHONEBOOK_CC", "3F007F105F3A", 0x4F23, "", 0x0},
  {126, "EF_GLOBAL_PHONEBOOK_PUID", "3F007F105F3A", 0x4F24, "", 0x0},
  {127, "EF_GLOBAL_PHONEBOOK_PBR", "3F007F105F3A", 0x4F30, "", 0x0},
  {128, "EF_GLOBAL_PHONEBOOK_FILE", "3F007F105F3A", 0x4FXX, "", 0x0},
  {129, "EF_MULTIMEDIA_MSG_LIST", "3F007F105F3B", 0x4F47, "", 0x0},
  {130, "EF_MULTIMEDIA_MSG_DATA_FILE", "3F007F105F3B", 0x4F48, "", 0x0},
  {131, "EF_IMAGE_FILE", "3F007F105F50", 0x4F20, "3F007F105F50", 0x4F20},
  {132, "EF_IMAGE_INSTANCE_DATA_FILE", "3F007F105F50", 0x4FXX, "3F007F105F50", 0x4FXX},
  {133, "EF_ORANGE_CARD_TYPE", "3F007F40", 0x6F91, "3F007F40", 0x6F91},
  {134, "EF_DYNAMIC2_FLAGS", "3F007F40", 0x6F92, "3F007F40", 0x6F92},
  {135, "EF_ORANGE_SERVICE_CONTROL_TABLE", "3F007F40", 0x6F93, "3F007F40", 0x6F93},
  {136, "EF_ZONAL_INDICATOR_1", "3F007F40", 0x6F95, "3F007F40", 0x6F95},
  {137, "EF_ZONAL_INDICATOR_2", "3F007F40", 0x6F96, "3F007F40", 0x6F96},
  {138, "EF_CSP_2", "3F007F40", 0x6F98, "3F007F40", 0x6F98},
  {139, "EF_JACOB", "3F007F40", 0x6F99, "3F007F40", 0x6F99},
  {140, "EF_ORANGE_PARAMETERS", "3F007F40", 0x6F9B, "3F007F40", 0x6F9B},
  {141, "EF_ACM_LINE_2", "3F007F40", 0x6F9C, "3F007F40", 0x6F9C},
  {142, "EF_DYNAMIC_FLAGS", "3F007F40", 0x6F9F, "3F007F40", 0x6F9F},
  {143, "EF_HZ_PARAMETERS", "3F007F43", 0x6F60, "3F007F43", 0x6F60},
  {144, "EF_HZ_CELL_CACHE_1", "3F007F43", 0x6F61, "3F007F43", 0x6F61},
  {145, "EF_HZ_CELL_CACHE_2", "3F007F43", 0x6F62, "3F007F43", 0x6F62},
  {146, "EF_HZ_CELL_CACHE_3", "3F007F43", 0x6F63, "3F007F43", 0x6F63},
  {147, "EF_HZ_CELL_CACHE_4", "3F007F43", 0x6F64, "3F007F43", 0x6F64},
  {148, "EF_UHZ_TAGS", "3F007F43", 0x6F80, "3F007F43", 0x6F80},
  {149, "EF_UHZ_SUBSCRIBED_LAC_CI_1", "3F007F43", 0x6F81, "3F007F43", 0x6F81},
  {150, "EF_UHZ_SUBSCRIBED_LAC_CI_2", "3F007F43", 0x6F82, "3F007F43", 0x6F82},
  {151, "EF_UHZ_SUBSCRIBED_LAC_CI_3", "3F007F43", 0x6F83, "3F007F43", 0x6F83},
  {152, "EF_UHZ_SUBSCRIBED_LAC_CI_4", "3F007F43", 0x6F84, "3F007F43", 0x6F84},
  {153, "EF_UHZ_SETTINGS", "3F007F43", 0x6F87, "3F007F43", 0x6F87},
  {154, "EF_ACTING_HPLMN", "3F007F66 5F30", 0x4F34, "3F007F66 5F30", 0x4F34},
  {155, "EF_IMEI_UPLOAD", "3F007F8F", 0x6FF0, "", 0x0},
  {156, "EF_OLD_IMEI", "3F007F8F", 0x6FF1, "", 0x0},
  {157, "EF_RAT_MODE", "3F007F66 5F30", 0x4F36, "", 0x0 },
  {158, "EF_O2_PERSONALISATION", "3F007F43", 0x6F02, "3F007F43", 0x6F02},
  {159, "EF_RAT_MODE", "3F007F66 5F30", 0x4F36, "", 0x0},
};
#endif
