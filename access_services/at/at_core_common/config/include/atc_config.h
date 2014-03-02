/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef ATC_CONFIG_H_
#define ATC_CONFIG_H_

/* Maximum AT command buffer length. The buffer is dynamically allocated as the need arises.
 * This setting only defines a sanity level to prevent outrageous buffer allocation. If it
 * is too small, just increase it and the parser will automatically allow longer AT commands.
 *
 * Note that the parser will add one byte to the buffer to leave room for a terminating
 * character.
 */
#define AT_COMMAND_BUFFER_MAX_SIZE  (4095)

/* Enabled commands */
#define AT_A_ENABLED
#define AT_AND_C_ENABLED
#define AT_AND_D_ENABLED
#define AT_D_ENABLED
#define AT_E_ENABLED
#define AT_AND_F_ENABLED
#define AT_H_ENABLED
#define AT_PLUS_CCFC_ENABLED
#define AT_PLUS_CCHC_ENABLED
#define AT_PLUS_CCHO_ENABLED
#define AT_PLUS_CGLA_ENABLED
#define AT_PLUS_CCWA_ENABLED
#define AT_PLUS_CEER_ENABLED
#define AT_PLUS_CFUN_ENABLED
#define AT_PLUS_CGACT_ENABLED
#define AT_PLUS_CGATT_ENABLED
#define AT_PLUS_CGCMOD_ENABLED
#define AT_PLUS_CGDCONT_ENABLED
#define AT_PLUS_CGEQMIN_ENABLED
#define AT_PLUS_CGEQREQ_ENABLED
#define AT_PLUS_CGEQNEG_ENABLED
#define AT_PLUS_CGQMIN_ENABLED
#define AT_PLUS_CGQREQ_ENABLED
#define AT_PLUS_CGEREP_ENABLED
#define AT_PLUS_CGMI_ENABLED
#define AT_PLUS_CGMM_ENABLED
#define AT_PLUS_CGMR_ENABLED
#define AT_PLUS_CGPADDR_ENABLED
#define AT_PLUS_CGREG_ENABLED
#define AT_PLUS_CGSMS_ENABLED
#define AT_PLUS_CGSN_ENABLED
#define AT_PLUS_CHLD_ENABLED
#define AT_PLUS_CHUP_ENABLED
#define AT_PLUS_CIMI_ENABLED
#define AT_PLUS_CLCC_ENABLED
#define AT_PLUS_CLCK_ENABLED
#define AT_PLUS_CLIP_ENABLED
#define AT_PLUS_CLIR_ENABLED
#define AT_PLUS_CMEC_ENABLED
#define AT_PLUS_CMEE_ENABLED
#define AT_PLUS_CMER_ENABLED
#define AT_PLUS_CMGD_ENABLED
#define AT_PLUS_CMGF_ENABLED
#define AT_PLUS_CMGL_ENABLED
#define AT_PLUS_CMGR_ENABLED
#define AT_PLUS_CMGS_ENABLED
#define AT_PLUS_CMGW_ENABLED
#define AT_PLUS_CMMS_ENABLED
#define AT_PLUS_CMOD_ENABLED
#define AT_PLUS_CMSS_ENABLED
#define AT_PLUS_CMUT_ENABLED
#define AT_PLUS_CNAP_ENABLED
#define AT_PLUS_CNMA_ENABLED
#define AT_PLUS_CNMI_ENABLED
#define AT_PLUS_CNUM_ENABLED
#define AT_PLUS_COPS_ENABLED
#define AT_PLUS_COLP_ENABLED
#define AT_PLUS_COLR_ENABLED
#define AT_PLUS_CPIN_ENABLED
#define AT_PLUS_CPOL_ENABLED
#define AT_PLUS_CPMS_ENABLED
#define AT_PLUS_CPWD_ENABLED
#define AT_PLUS_CRC_ENABLED
#define AT_PLUS_CREG_ENABLED
#define AT_PLUS_CRLA_ENABLED
#define AT_PLUS_CR_ENABLED
#define AT_PLUS_CRSM_ENABLED
#define AT_PLUS_CUAD_ENABLED
#define AT_PLUS_CSIM_ENABLED
#define AT_PLUS_CSCA_ENABLED
#define AT_PLUS_CSAS_ENABLED
#define AT_PLUS_CRES_ENABLED
#define AT_PLUS_CSCB_ENABLED
#define AT_PLUS_CSCS_ENABLED
#define AT_PLUS_CSMS_ENABLED
#define AT_PLUS_CSQ_ENABLED
#define AT_PLUS_CSSN_ENABLED
#define AT_PLUS_CUSD_ENABLED
#define AT_PLUS_CUUS1_ENABLED
#define AT_PLUS_DS_ENABLED
#define AT_PLUS_GCAP_ENABLED
#define AT_PLUS_GMM_ENABLED
#define AT_PLUS_GMR_ENABLED
#define AT_PLUS_IFC_ENABLED
#define AT_PLUS_VTD_ENABLED
#define AT_PLUS_VTS_ENABLED
#define AT_Q_ENABLED
#define AT_S0_ENABLED
#define AT_STAR_ECAM_ENABLED
/* ECEXPIN is an extension of CPIN
 * To make use of ECEXPIN, AT_PLUS_CPIN_ENABLED must be set as well */
#define AT_STAR_ECEXPIN_ENABLED
#define AT_STAR_ECME_ENABLED
#define AT_STAR_ECPSCHBLKR_ENABLE
#define AT_STAR_ECPSAUTHPE_ENABLE
#define AT_STAR_ECPSDEAUTHPE_ENABLE
#define AT_STAR_ECPSLOCKBOOTP_ENABLE
#define AT_STAR_ECPSINITARB_ENABLE
#define AT_STAR_ECPSDTW_ENABLE
#define AT_STAR_ECPSAUTHU_ENABLE
#define AT_STAR_ECPSAUTHDU_ENABLE
#define AT_STAR_ECPSIMEIW_ENABLE
#define AT_STAR_ECPSLCW_ENABLE
#define AT_STAR_ECPSSIMLOCK_ENABLE
#define AT_STAR_ECPSSIMLOCKD_ENABLE
#define AT_STAR_ECSPSAW_ENABLE
#define AT_STAR_ECSPSAR_ENABLE
#define AT_STAR_ECRAT_ENABLE
#ifdef ENABLE_FTD
#define AT_STAR_EEMPAGE_ENABLED
#endif
#define AT_STAR_EHSTACT_ENABLED
#define AT_STAR_EIAAUW_ENABLED
#define AT_STAR_ESMSFULL_ENABLED
/* Removed until specification work is done  #define AT_STAR_EFSIM_ENABLED */
#define AT_STAR_ELAT_ENABLED
#define AT_STAR_ELIN_ENABLED
#define AT_STAR_ENABLED
#define AT_STAR_ENAP_ENABLED
#define AT_STAR_EPEE_ENABLED
#define AT_STAR_EPINR_ENABLED
#define AT_STAR_EPPSD_ENABLED
#define AT_STAR_EPSB_ENABLED
#define AT_STAR_ENNIR_ENABLED
#define AT_STAR_EPWRRED_ENABLED
#define AT_STAR_EREG_ENABLED
#define AT_STAR_ERFSTATE_ENABLED
#define AT_STAR_ESHLVOCR_ENABLED
#define AT_STAR_ESIMRF_ENABLED
#define AT_STAR_ESIMSR_ENABLED
#define AT_STAR_ESVN_ENABLED
#define AT_STAR_ETTY_ENABLED
#define AT_STAR_ETZR_ENABLED
#define AT_STAR_EUPLINK_ENABLED
#define AT_STAR_STKC_ENABLED
#define AT_STAR_STKE_ENABLED
#define AT_STAR_STKR_ENABLED
#define AT_STAR_EGNCI_ENABLED
#define AT_STAR_EWNCI_ENABLED
#define AT_STAR_EGSCI_ENABLED
#define AT_STAR_EWSCI_ENABLED
#define AT_S3_ENABLED
#define AT_S4_ENABLED
#define AT_S5_ENABLED
#define AT_S6_ENABLED
#define AT_S7_ENABLED
#define AT_S8_ENABLED
#define AT_S10_ENABLED
#define AT_V_ENABLED
#define AT_X_ENABLED
#define AT_Z_ENABLED
#define AT_STAR_EMSR_ENABLED
#define AT_STAR_ESLEEP_ENABLED
#define AT_PLUS_RADIOVER_ENABLED

/* FOR DEBUGGING */
#define AT_STAR_EACS_ENABLED

#define ATC_SOCKET_PATH "/dev/socket/at_core"

/* Defines for CSCS character sets. */
#ifdef AT_PLUS_CSCS_ENABLED
#define AT_PLUS_CSCS_CHARSET_UTF_8_ENABLED
#define AT_PLUS_CSCS_CHARSET_HEX_ENABLED
/*#define AT_PLUS_CSCS_CHARSET_GSM_ENABLED*/
/*#define AT_PLUS_CSCS_CHARSET_IRA_ENABLED*/
/*#define AT_PLUS_CSCS_CHARSET_8859_1_ENABLED*/
/*#define AT_PLUS_CSCS_CHARSET_UCS2_ENABLED*/
#endif

/* Must be at the bottom of the file to give the ability to override common
 * settings in the extension file.
 */
#ifdef EXE_USE_ATC_CUSTOMER_EXTENSIONS
#include <atc_extended_config.h>
#endif


#endif /* ATC_CONFIG_H_ */
