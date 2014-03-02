/**
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */


#ifndef INCLUSION_GUARD_PAS_AT_DEFS_H
#define INCLUSION_GUARD_PAS_AT_DEFS_H


/*This file was called pas_at_defs.h before*/

/*************************************************************************
 * Includes
 *************************************************************************/

/* ATC common related header files */
#include <atc_command_list.h>

/* ATC backend related header files */
#include <atc_config.h>
#include "atc_parser.h"

#ifdef EXE_USE_ATC_CUSTOMER_EXTENSIONS
#include <atc_extended_handlers.h>
#endif

/*************************************************************************
 * Types, constants and external variables
 *************************************************************************/

#define AT_END_OF_TABLE 0xFF
#define MAX_CID 25
#define MIN_CID 1
#define MAX_SIZE_OF_PDP_TYPE 10
#define MAX_SIZE_OF_PDP_ADDRESS 46 /*INET6_ADDRSTRLEN (45)+trailing null character*/



/*************************************************************************
 * Declaration of functions
 **************************************************************************/

AT_Command_e AT_AND_C_Handle(AT_ParserState_s *parser_p,
                             AT_CommandLine_t info_text,
                             AT_ResponseMessage_t message);

AT_Command_e AT_AND_D_Handle(AT_ParserState_s *parser_p,
                             AT_CommandLine_t info_text,
                             AT_ResponseMessage_t message);

AT_Command_e AT_AND_F_Handle(AT_ParserState_s *parser_p,
                             AT_CommandLine_t info_text,
                             AT_ResponseMessage_t message);

AT_Command_e AT_E_Handle(AT_ParserState_s *parser_p,
                         AT_CommandLine_t info_text,
                         AT_ResponseMessage_t message);

AT_Command_e AT_PLUS_CLIP_Handle(AT_ParserState_s *parser_p,
                                 AT_CommandLine_t result_text,
                                 AT_ResponseMessage_t message);

AT_Command_e AT_PLUS_CLIR_Handle(AT_ParserState_s *parser_p,
                                 AT_CommandLine_t result_text,
                                 AT_ResponseMessage_t message);

AT_Command_e AT_PLUS_CR_Handle(AT_ParserState_s *parser_p,
                               AT_CommandLine_t info_text,
                               AT_ResponseMessage_t message);

AT_Command_e AT_PLUS_CRC_Handle(AT_ParserState_s *parser_p,
                                AT_CommandLine_t info_text,
                                AT_ResponseMessage_t message);

AT_Command_e AT_CommandHandlerFunction(AT_ParserState_s *parser_p,
                                       AT_CommandLine_t result_text, AT_ResponseMessage_t message);

AT_Command_e AT_STAR_Handle(AT_ParserState_s *parser_p,
                            AT_CommandLine_t result_text,
                            AT_ResponseMessage_t message);

AT_Command_e AT_PLUS_CMER_Handle(AT_ParserState_s *parser_p,
                                 AT_CommandLine_t info_text,
                                 AT_ResponseMessage_t message);

AT_Command_e AT_PLUS_CLAC_Handle(AT_ParserState_s *parser_p,
                                 AT_CommandLine_t info_text,
                                 AT_ResponseMessage_t message);

AT_Command_e AT_H_Handle(AT_ParserState_s *parser_p,
                         AT_CommandLine_t info_text,
                         AT_ResponseMessage_t message);

AT_Command_e AT_D_Handle(AT_ParserState_s *parser_p,
                         AT_CommandLine_t info_text,
                         AT_ResponseMessage_t message);

AT_Command_e AT_L_Handle(AT_ParserState_s *parser_p,
                         AT_CommandLine_t info_text,
                         AT_ResponseMessage_t message);

AT_Command_e AT_S0_Handle(AT_ParserState_s *parser_p,
                          AT_CommandLine_t info_text,
                          AT_ResponseMessage_t message);

AT_Command_e AT_S3_Handle(AT_ParserState_s *parser_p,
                          AT_CommandLine_t info_text,
                          AT_ResponseMessage_t message);

AT_Command_e AT_S4_Handle(AT_ParserState_s *parser_p,
                          AT_CommandLine_t info_text,
                          AT_ResponseMessage_t message);

AT_Command_e AT_S5_Handle(AT_ParserState_s *parser_p,
                          AT_CommandLine_t info_text,
                          AT_ResponseMessage_t message);

AT_Command_e AT_S6_Handle(AT_ParserState_s *parser_p,
                          AT_CommandLine_t info_text,
                          AT_ResponseMessage_t message);

AT_Command_e AT_S7_Handle(AT_ParserState_s *parser_p,
                          AT_CommandLine_t info_text,
                          AT_ResponseMessage_t message);

AT_Command_e AT_S8_Handle(AT_ParserState_s *parser_p,
                          AT_CommandLine_t info_text,
                          AT_ResponseMessage_t message);

AT_Command_e AT_S10_Handle(AT_ParserState_s *parser_p,
                           AT_CommandLine_t info_text,
                           AT_ResponseMessage_t message);

AT_Command_e AT_Q_Handle(AT_ParserState_s *parser_p,
                         AT_CommandLine_t info_text,
                         AT_ResponseMessage_t message);

AT_Command_e AT_V_Handle(AT_ParserState_s *parser_p,
                         AT_CommandLine_t info_text,
                         AT_ResponseMessage_t message);

AT_Command_e AT_X_Handle(AT_ParserState_s *parser_p,
                         AT_CommandLine_t info_text,
                         AT_ResponseMessage_t message);

AT_Command_e AT_Z_Handle(AT_ParserState_s *parser_p,
                         AT_CommandLine_t info_text,
                         AT_ResponseMessage_t message);

AT_Command_e AT_PLUS_BLDN_Handle(AT_ParserState_s *parser_p,
                                 AT_CommandLine_t info_text,
                                 AT_ResponseMessage_t message);

AT_Command_e AT_PLUS_BINP_Handle(AT_ParserState_s *parser_p,
                                 AT_CommandLine_t info_text,
                                 AT_ResponseMessage_t message);

AT_Command_e AT_PLUS_NREC_Handle(AT_ParserState_s *parser_p,
                                 AT_CommandLine_t info_text,
                                 AT_ResponseMessage_t Response);

AT_Command_e AT_PLUS_CLCC_Handle(AT_ParserState_s *parser_p,
                                 AT_CommandLine_t info_text,
                                 AT_ResponseMessage_t response_p);

AT_Command_e AT_PLUS_CSCC_Handle(AT_ParserState_s *parser_p,
                                 AT_CommandLine_t info_text,
                                 AT_ResponseMessage_t message);

AT_Command_e AT_PLUS_CSCS_Handle(AT_ParserState_s *parser_p,
                                 AT_CommandLine_t info_text,
                                 AT_ResponseMessage_t message);

AT_Command_e AT_PLUS_WS46_Handle(AT_ParserState_s *parser_p,
                                 AT_CommandLine_t info_text,
                                 AT_ResponseMessage_t message);

AT_Command_e AT_PLUS_VTD_Handle(AT_ParserState_s *parser_p,
                                AT_CommandLine_t info_text,
                                AT_ResponseMessage_t message);

AT_Command_e AT_PLUS_VTS_Handle(AT_ParserState_s *parser_p,
                                AT_CommandLine_t info_text,
                                AT_ResponseMessage_t message);

AT_Command_e AT_PLUS_CNUM_Handle(AT_ParserState_s *parser_p,
                                 AT_CommandLine_t info_text,
                                 AT_ResponseMessage_t message);

AT_Command_e AT_PLUS_COLP_Handle(AT_ParserState_s *parser_p,
                                 AT_CommandLine_t info_text,
                                 AT_ResponseMessage_t message);

AT_Command_e AT_PLUS_COLR_Handle(AT_ParserState_s *parser_p,
                                 AT_CommandLine_t info_text,
                                 AT_ResponseMessage_t message);

AT_Command_e AT_PLUS_CAOC_Handle(AT_ParserState_s *parser_p,
                                 AT_CommandLine_t info_text,
                                 AT_ResponseMessage_t message);

AT_Command_e AT_PLUS_CCWE_Handle(AT_ParserState_s *parser_p,
                                 AT_CommandLine_t info_text,
                                 AT_ResponseMessage_t message);

AT_Command_e AT_PLUS_CCFC_Handle(AT_ParserState_s *parser_p,
                                 AT_CommandLine_t info_text,
                                 AT_ResponseMessage_t message);

AT_Command_e AT_PLUS_CCWA_Handle(AT_ParserState_s *parser_p,
                                 AT_CommandLine_t info_text,
                                 AT_ResponseMessage_t message);

AT_Command_e AT_PLUS_CHLD_Handle(AT_ParserState_s *parser_p,
                                 AT_CommandLine_t info_text,
                                 AT_ResponseMessage_t message);

AT_Command_e AT_PLUS_CLCK_Handle(AT_ParserState_s *parser_p,
                                 AT_CommandLine_t info_text,
                                 AT_ResponseMessage_t message);

AT_Command_e AT_PLUS_CTFR_Handle(AT_ParserState_s *parser_p,
                                 AT_CommandLine_t info_text,
                                 AT_ResponseMessage_t message);

AT_Command_e AT_PLUS_CAMM_Handle(AT_ParserState_s *parser_p,
                                 AT_CommandLine_t info_text,
                                 AT_ResponseMessage_t message);

AT_Command_e AT_PLUS_VGM_Handle(AT_ParserState_s *parser_p,
                                AT_CommandLine_t info_text,
                                AT_ResponseMessage_t message);

AT_Command_e AT_PLUS_VGS_Handle(AT_ParserState_s *parser_p,
                                AT_CommandLine_t info_text,
                                AT_ResponseMessage_t message);

AT_Command_e AT_STAR_EALS_Handle(AT_ParserState_s *parser_p,
                                 AT_CommandLine_t info_text,
                                 AT_ResponseMessage_t message);

AT_Command_e AT_STAR_ESLN_Handle(AT_ParserState_s *parser_p,
                                 AT_CommandLine_t info_text,
                                 AT_ResponseMessage_t message);

AT_Command_e AT_PLUS_CUSD_Handle(AT_ParserState_s *parser_p,
                                 AT_CommandLine_t info_text,
                                 AT_ResponseMessage_t message);

AT_Command_e AT_PLUS_CUUS1_Handle(AT_ParserState_s *parser_p,
                                  AT_CommandLine_t info_text,
                                  AT_ResponseMessage_t message);

AT_Command_e AT_STAR_EIAAUW_Handle(AT_ParserState_s *parser_p,
                                   AT_CommandLine_t info_text,
                                   AT_ResponseMessage_t message);

AT_Command_e AT_STAR_EPPSD_Handle(AT_ParserState_s *parser_p,
                                  AT_CommandLine_t info_text,
                                  AT_ResponseMessage_t message);

AT_Command_e AT_STAR_ELIN_Handle(AT_ParserState_s *parser_p,
                                 AT_CommandLine_t info_text,
                                 AT_ResponseMessage_t message);

AT_Command_e AT_PLUS_CBC_Handle(AT_ParserState_s *parser_p,
                                AT_CommandLine_t info_text,
                                AT_ResponseMessage_t message);

AT_Command_e AT_PLUS_CIND_Handle(AT_ParserState_s *parser_p,
                                 AT_CommandLine_t info_text,
                                 AT_ResponseMessage_t message);

AT_Command_e AT_STAR_ESBL_Handle(AT_ParserState_s *parser_p,
                                 AT_CommandLine_t info_text,
                                 AT_ResponseMessage_t message);

AT_Command_e AT_STAR_ESOM_Handle(AT_ParserState_s *parser_p,
                                 AT_CommandLine_t info_text,
                                 AT_ResponseMessage_t message);

AT_Command_e AT_STAR_ESIMR_Handle(AT_ParserState_s *parser_p,
                                  AT_CommandLine_t info_text,
                                  AT_ResponseMessage_t message);

AT_Command_e AT_STAR_EFSIM_Handle(AT_ParserState_s *parser_p,
                                  AT_CommandLine_t info_text,
                                  AT_ResponseMessage_t message);

AT_Command_e AT_PLUS_CSMS_Handle(AT_ParserState_s *parser_p,
                                 AT_CommandLine_t info_text,
                                 AT_ResponseMessage_t message);

AT_Command_e AT_PLUS_CPMS_Handle(AT_ParserState_s *parser_p,
                                 AT_CommandLine_t info_text,
                                 AT_ResponseMessage_t message);

AT_Command_e AT_PLUS_CMGF_Handle(AT_ParserState_s *parser_p,
                                 AT_CommandLine_t info_text,
                                 AT_ResponseMessage_t message);

AT_Command_e AT_PLUS_CSCA_Handle(AT_ParserState_s *parser_p,
                                 AT_CommandLine_t info_text,
                                 AT_ResponseMessage_t message);

AT_Command_e AT_PLUS_CSCB_Handle(AT_ParserState_s *parser_p,
                                 AT_CommandLine_t info_text,
                                 AT_ResponseMessage_t message);

AT_Command_e AT_PLUS_CNMI_Handle(AT_ParserState_s *parser_p,
                                 AT_CommandLine_t info_text,
                                 AT_ResponseMessage_t message);

AT_Command_e  AT_PLUS_CNMA_Handle(AT_ParserState_s *parser_p,
                                  AT_CommandLine_t info_text,
                                  AT_ResponseMessage_t message);

AT_Command_e AT_PLUS_CREG_Handle(AT_ParserState_s *parser_p,
                                 AT_CommandLine_t info_text,
                                 AT_ResponseMessage_t message);

AT_Command_e AT_PLUS_COPN_Handle(AT_ParserState_s *parser_p,
                                 AT_CommandLine_t info_text,
                                 AT_ResponseMessage_t message);

AT_Command_e AT_PLUS_COPS_Handle(AT_ParserState_s *parser_p,
                                 AT_CommandLine_t info_text,
                                 AT_ResponseMessage_t message);

AT_Command_e AT_PLUS_CPOL_Handle(AT_ParserState_s *parser_p,
                                 AT_CommandLine_t info_text,
                                 AT_ResponseMessage_t message);

AT_Command_e AT_PLUS_CRES_Handle(AT_ParserState_s *parser_p,
                                 AT_CommandLine_t info_text,
                                 AT_ResponseMessage_t message);

AT_Command_e AT_PLUS_CSAS_Handle(AT_ParserState_s *parser_p,
                                 AT_CommandLine_t info_text,
                                 AT_ResponseMessage_t message);

AT_Command_e AT_PLUS_CMGS_Handle(AT_ParserState_s *parser_p,
                                 AT_CommandLine_t info_text,
                                 AT_ResponseMessage_t message);

AT_Command_e AT_PLUS_CMGD_Handle(AT_ParserState_s *parser_p,
                                 AT_CommandLine_t info_text,
                                 AT_ResponseMessage_t message);

AT_Command_e AT_PLUS_CMGL_Handle(AT_ParserState_s *parser_p,
                                 AT_CommandLine_t info_text,
                                 AT_ResponseMessage_t message);

AT_Command_e AT_PLUS_CMGR_Handle(AT_ParserState_s *parser_p,
                                 AT_CommandLine_t info_text,
                                 AT_ResponseMessage_t message);

AT_Command_e AT_PLUS_CMGW_Handle(AT_ParserState_s *parser_p,
                                 AT_CommandLine_t info_text,
                                 AT_ResponseMessage_t message);

AT_Command_e AT_PLUS_CMSS_Handle(AT_ParserState_s *parser_p,
                                 AT_CommandLine_t info_text,
                                 AT_ResponseMessage_t message);

AT_Command_e AT_PLUS_CGSMS_Handle(AT_ParserState_s *parser_p,
                                  AT_CommandLine_t info_text,
                                  AT_ResponseMessage_t message);

AT_Command_e AT_PLUS_CMMS_Handle(AT_ParserState_s *parser_p,
                                 AT_CommandLine_t info_text,
                                 AT_ResponseMessage_t message);

AT_Command_e AT_STAR_ECAS_Handle(AT_ParserState_s *parser_p,
                                 AT_CommandLine_t info_text,
                                 AT_ResponseMessage_t message);

AT_Command_e AT_STAR_ESDG_Handle(AT_ParserState_s *parser_p,
                                 AT_CommandLine_t info_text,
                                 AT_ResponseMessage_t message);

AT_Command_e AT_STAR_ESGR_Handle(AT_ParserState_s *parser_p,
                                 AT_CommandLine_t info_text,
                                 AT_ResponseMessage_t message);

AT_Command_e AT_STAR_ESAG_Handle(AT_ParserState_s *parser_p,
                                 AT_CommandLine_t info_text,
                                 AT_ResponseMessage_t message);

AT_Command_e AT_STAR_EGIR_Handle(AT_ParserState_s *parser_p,
                                 AT_CommandLine_t info_text,
                                 AT_ResponseMessage_t message);

AT_Command_e AT_STAR_ESDI_Handle(AT_ParserState_s *parser_p,
                                 AT_CommandLine_t info_text,
                                 AT_ResponseMessage_t message);

AT_Command_e AT_PLUS_CSDF_Handle(AT_ParserState_s *parser_p,
                                 AT_CommandLine_t info_text,
                                 AT_ResponseMessage_t message);

AT_Command_e AT_PLUS_CCLK_Handle(AT_ParserState_s *parser_p,
                                 AT_CommandLine_t info_text,
                                 AT_ResponseMessage_t message);

AT_Command_e AT_PLUS_CALA_Handle(AT_ParserState_s *parser_p,
                                 AT_CommandLine_t info_text,
                                 AT_ResponseMessage_t message);

AT_Command_e AT_PLUS_CALD_Handle(AT_ParserState_s *parser_p,
                                 AT_CommandLine_t info_text,
                                 AT_ResponseMessage_t message);

AT_Command_e AT_PLUS_CIMI_Handle(AT_ParserState_s *parser_p,
                                 AT_CommandLine_t info_text,
                                 AT_ResponseMessage_t message);

AT_Command_e AT_STAR_ECUR_Handle(AT_ParserState_s *parser_p,
                                 AT_CommandLine_t info_text,
                                 AT_ResponseMessage_t message);

AT_Command_e AT_STAR_EREG_Handle(AT_ParserState_s *parser_p,
                                 AT_CommandLine_t info_text,
                                 AT_ResponseMessage_t message);

AT_Command_e AT_STAR_ESIMSR_Handle(AT_ParserState_s *parser_p,
                                   AT_CommandLine_t info_text,
                                   AT_ResponseMessage_t message);

AT_Command_e AT_STAR_ESIMRF_Handle(AT_ParserState_s *parser_p,
                                   AT_CommandLine_t info_text,
                                   AT_ResponseMessage_t message);

AT_Command_e AT_STAR_EPEE_Handle(AT_ParserState_s *parser_p,
                                 AT_CommandLine_t info_text,
                                 AT_ResponseMessage_t message);

AT_Command_e AT_STAR_ESMSFULL_Handle(AT_ParserState_s *parser_p,
                                     AT_CommandLine_t info_text,
                                     AT_ResponseMessage_t message);

AT_Command_e AT_STAR_EPSB_Handle(AT_ParserState_s *parser_p,
                                 AT_CommandLine_t info_text,
                                 AT_ResponseMessage_t message);

AT_Command_e AT_STAR_ENNIR_Handle(AT_ParserState_s *parser_p,
                                  AT_CommandLine_t info_text,
                                  AT_ResponseMessage_t message);

AT_Command_e AT_STAR_EVIB_Handle(AT_ParserState_s *parser_p,
                                 AT_CommandLine_t info_text,
                                 AT_ResponseMessage_t message);

AT_Command_e AT_STAR_ECHD_Handle(AT_ParserState_s *parser_p,
                                 AT_CommandLine_t info_text,
                                 AT_ResponseMessage_t message);

AT_Command_e AT_STAR_ESNU_Handle(AT_ParserState_s *parser_p,
                                 AT_CommandLine_t info_text,
                                 AT_ResponseMessage_t message);

AT_Command_e AT_STAR_EQVL_Handle(AT_ParserState_s *parser_p,
                                 AT_CommandLine_t info_text,
                                 AT_ResponseMessage_t message);

AT_Command_e AT_STAR_EXVC_Handle(AT_ParserState_s *parser_p,
                                 AT_CommandLine_t info_text,
                                 AT_ResponseMessage_t message);

AT_Command_e AT_STAR_EBCA_Handle(AT_ParserState_s *parser_p,
                                 AT_CommandLine_t info_text,
                                 AT_ResponseMessage_t message);

AT_Command_e AT_STAR_ELAT_Handle(AT_ParserState_s *parser_p,
                                 AT_CommandLine_t info_text,
                                 AT_ResponseMessage_t message);

AT_Command_e AT_STAR_ETCH_Handle(AT_ParserState_s *parser_p,
                                 AT_CommandLine_t info_text,
                                 AT_ResponseMessage_t message);

AT_Command_e AT_STAR_ETTY_Handle(AT_ParserState_s *parser_p,
                                 AT_CommandLine_t info_text,
                                 AT_ResponseMessage_t response_p);

AT_Command_e AT_STAR_ETZR_Handle(AT_ParserState_s *parser_p,
                                 AT_CommandLine_t info_text,
                                 AT_ResponseMessage_t message);

AT_Command_e AT_STAR_ECLKM_Handle(AT_ParserState_s *parser_p,
                                  AT_CommandLine_t info_text,
                                  AT_ResponseMessage_t message);

AT_Command_e AT_STAR_EASY_Handle(AT_ParserState_s *parser_p,
                                 AT_CommandLine_t info_text,
                                 AT_ResponseMessage_t message);

AT_Command_e AT_STAR_ZPPI_Handle(AT_ParserState_s *parser_p,
                                 AT_CommandLine_t info_text,
                                 AT_ResponseMessage_t message);

AT_Command_e AT_PLUS_CACM_Handle(AT_ParserState_s *parser_p,
                                 AT_CommandLine_t info_text,
                                 AT_ResponseMessage_t message);

AT_Command_e AT_PLUS_CPAS_Handle(AT_ParserState_s *parser_p,
                                 AT_CommandLine_t info_text,
                                 AT_ResponseMessage_t response_p);

AT_Command_e AT_PLUS_CPUC_Handle(AT_ParserState_s *parser_p,
                                 AT_CommandLine_t info_text,
                                 AT_ResponseMessage_t response_p);

AT_Command_e AT_PLUS_CPWD_Handle(AT_ParserState_s *parser_p,
                                 AT_CommandLine_t info_text,
                                 AT_ResponseMessage_t response_p);

AT_Command_e AT_PLUS_CKPD_Handle(AT_ParserState_s *parser_p,
                                 AT_CommandLine_t info_text,
                                 AT_ResponseMessage_t message);

AT_Command_e AT_STAR_EKSE_Handle(AT_ParserState_s *parser_p,
                                 AT_CommandLine_t info_text,
                                 AT_ResponseMessage_t message);

AT_Command_e AT_STAR_EACS_Handle(AT_ParserState_s *parser_p,
                                 AT_CommandLine_t info_text, AT_ResponseMessage_t message);

AT_Command_e AT_STAR_ECSCA_Handle(AT_ParserState_s *parser_p,
                                  AT_CommandLine_t info_text,
                                  AT_ResponseMessage_t message);

AT_Command_e AT_A_Handle(AT_ParserState_s *parser_p,
                         AT_CommandLine_t info_text,
                         AT_ResponseMessage_t response_p);

AT_Command_e AT_PLUS_CPBW_Handle(AT_ParserState_s *parser_p,
                                 AT_CommandLine_t info_text,
                                 AT_ResponseMessage_t message);

AT_Command_e AT_PLUS_CPBR_Handle(AT_ParserState_s *parser_p,
                                 AT_CommandLine_t info_text,
                                 AT_ResponseMessage_t message);

AT_Command_e AT_PLUS_CPBF_Handle(AT_ParserState_s *parser_p,
                                 AT_CommandLine_t info_text,
                                 AT_ResponseMessage_t message);

AT_Command_e AT_STAR_EPHD_Handle(AT_ParserState_s *parser_p,
                                 AT_CommandLine_t info_text,
                                 AT_ResponseMessage_t message);


AT_Command_e AT_PLUS_CSVM_Handle(AT_ParserState_s *parser_p,
                                 AT_CommandLine_t info_text,
                                 AT_ResponseMessage_t message);

AT_Command_e AT_PLUS_CPIN_Handle(AT_ParserState_s *parser_p,
                                 AT_CommandLine_t info_text,
                                 AT_ResponseMessage_t message);

AT_Command_e AT_PLUS_CPOL_Handle(AT_ParserState_s *parser_p,
                                 AT_CommandLine_t info_text,
                                 AT_ResponseMessage_t message);

AT_Command_e AT_STAR_EPINR_Handle(AT_ParserState_s *parser_p,
                                  AT_CommandLine_t info_text,
                                  AT_ResponseMessage_t message);

AT_Command_e AT_STAR_ENAP_Handle(AT_ParserState_s *parser_p,
                                 AT_CommandLine_t info_text,
                                 AT_ResponseMessage_t message);

AT_Command_e AT_STAR_EUPLINK_Handle(AT_ParserState_s *parser_p,
                                    AT_CommandLine_t info_text,
                                    AT_ResponseMessage_t message);

AT_Command_e AT_STAR_EIBA_Handle(AT_ParserState_s *parser_p,
                                 AT_CommandLine_t info_text,
                                 AT_ResponseMessage_t message);

AT_Command_e AT_PLUS_CFUN_Handle(AT_ParserState_s *parser_p,
                                 AT_CommandLine_t info_text,
                                 AT_ResponseMessage_t message);

AT_Command_e AT_PLUS_CGACT_Handle(AT_ParserState_s *parser_p,
                                  AT_CommandLine_t info_text,
                                  AT_ResponseMessage_t message);

AT_Command_e AT_PLUS_CGATT_Handle(AT_ParserState_s *parser_p,
                                  AT_CommandLine_t info_text,
                                  AT_ResponseMessage_t message);

AT_Command_e AT_PLUS_CMOD_Handle(AT_ParserState_s *parser_p,
                                 AT_CommandLine_t info_text,
                                 AT_ResponseMessage_t message);

AT_Command_e AT_PLUS_CGCMOD_Handle(AT_ParserState_s *parser_p,
                                   AT_CommandLine_t info_text,
                                   AT_ResponseMessage_t message);

AT_Command_e AT_PLUS_CGDCONT_Handle(AT_ParserState_s *parser_p,
                                    AT_CommandLine_t info_text,
                                    AT_ResponseMessage_t message);

AT_Command_e AT_PLUS_CGEQMIN_Handle(AT_ParserState_s *parser_p,
                                    AT_CommandLine_t info_text,
                                    AT_ResponseMessage_t message);

AT_Command_e AT_PLUS_CGEQNEG_Handle(AT_ParserState_s *parser_p,
                                    AT_CommandLine_t info_text,
                                    AT_ResponseMessage_t message);

AT_Command_e AT_PLUS_CGEQREQ_Handle(AT_ParserState_s *parser_p,
                                    AT_CommandLine_t info_text,
                                    AT_ResponseMessage_t message);

AT_Command_e AT_PLUS_CGQMIN_Handle(AT_ParserState_s *parser_p,
                                   AT_CommandLine_t info_text,
                                   AT_ResponseMessage_t message);

AT_Command_e AT_PLUS_CGQREQ_Handle(AT_ParserState_s *parser_p,
                                   AT_CommandLine_t info_text,
                                   AT_ResponseMessage_t message);

AT_Command_e AT_PLUS_CGEREP_Handle(AT_ParserState_s *parser,
                                   AT_CommandLine_t info_text,
                                   AT_ResponseMessage_t message);

AT_Command_e AT_PLUS_CGEREP_Handle(AT_ParserState_s *parser_p,
                                   AT_CommandLine_t info_text,
                                   AT_ResponseMessage_t message);

AT_Command_e AT_PLUS_CGMI_Handle(AT_ParserState_s *parser_p,
                                 AT_CommandLine_t info_text,
                                 AT_ResponseMessage_t message);

AT_Command_e AT_PLUS_CGMM_Handle(AT_ParserState_s *parser_p,
                                 AT_CommandLine_t info_text,
                                 AT_ResponseMessage_t message);

AT_Command_e AT_PLUS_CGMR_Handle(AT_ParserState_s *parser_p,
                                 AT_CommandLine_t info_text,
                                 AT_ResponseMessage_t message);

AT_Command_e AT_PLUS_CGPADDR_Handle(AT_ParserState_s *parser_p,
                                    AT_CommandLine_t info_text,
                                    AT_ResponseMessage_t message);

AT_Command_e AT_PLUS_CGREG_Handle(AT_ParserState_s *parser_p,
                                  AT_CommandLine_t info_text,
                                  AT_ResponseMessage_t message);

AT_Command_e AT_PLUS_CGSN_Handle(AT_ParserState_s *parser_p,
                                 AT_CommandLine_t info_text,
                                 AT_ResponseMessage_t message);

AT_Command_e AT_STAR_EACE_Handle(AT_ParserState_s *parser_p,
                                 AT_CommandLine_t info_text,
                                 AT_ResponseMessage_t message);

AT_Command_e AT_STAR_EBAT_Handle(AT_ParserState_s *parser_p,
                                 AT_CommandLine_t info_text,
                                 AT_ResponseMessage_t message);

AT_Command_e AT_STAR_ECAM_Handle(AT_ParserState_s *parser_p,
                                 AT_CommandLine_t info_text,
                                 AT_ResponseMessage_t message);

AT_Command_e AT_STAR_ECME_Handle(AT_ParserState_s *parser_p,
                                 AT_CommandLine_t info_text,
                                 AT_ResponseMessage_t message);

AT_Command_e AT_STAR_CPI_Handle(AT_ParserState_s *parser_p,
                                AT_CommandLine_t info_text,
                                AT_ResponseMessage_t message);

AT_Command_e AT_PLUS_CSSN_Handle(AT_ParserState_s *parser_p,
                                 AT_CommandLine_t info_text,
                                 AT_ResponseMessage_t message);

AT_Command_e AT_PLUS_CSQ_Handle(AT_ParserState_s *parser_p,
                                AT_CommandLine_t info_text,
                                AT_ResponseMessage_t message);

AT_Command_e AT_PLUS_CMUT_Handle(AT_ParserState_s *parser_p,
                                 AT_CommandLine_t info_text,
                                 AT_ResponseMessage_t message);

AT_Command_e AT_PLUS_CCHC_Handle(AT_ParserState_s *parser_p,
                                 AT_CommandLine_t info_text,
                                 AT_ResponseMessage_t message);

AT_Command_e AT_PLUS_CCHO_Handle(AT_ParserState_s *parser_p,
                                 AT_CommandLine_t info_text,
                                 AT_ResponseMessage_t message);

AT_Command_e AT_PLUS_CGLA_Handle(AT_ParserState_s *parser_p,
                                 AT_CommandLine_t info_text,
                                 AT_ResponseMessage_t message);

AT_Command_e AT_PLUS_CRSM_Handle(AT_ParserState_s *parser_p,
                                 AT_CommandLine_t info_text,
                                 AT_ResponseMessage_t message);

AT_Command_e AT_STAR_STKC_Handle(AT_ParserState_s *parser_p,
                                 AT_CommandLine_t info_text,
                                 AT_ResponseMessage_t message);

AT_Command_e AT_STAR_STKE_Handle(AT_ParserState_s *parser_p,
                                 AT_CommandLine_t info_text,
                                 AT_ResponseMessage_t message);

AT_Command_e AT_STAR_STKR_Handle(AT_ParserState_s *parser_p,
                                 AT_CommandLine_t info_text,
                                 AT_ResponseMessage_t message);

AT_Command_e AT_STAR_ESHLVOCR_Handle(AT_ParserState_s *parser_p,
                                     AT_CommandLine_t info_text,
                                     AT_ResponseMessage_t message);

AT_Command_e AT_PLUS_CMEE_Handle(AT_ParserState_s *parser_p,
                                 AT_CommandLine_t info_text,
                                 AT_ResponseMessage_t response_p);

AT_Command_e AT_PLUS_CMEC_Handle(AT_ParserState_s *parser_p,
                                 AT_CommandLine_t info_text,
                                 AT_ResponseMessage_t response_p);

AT_Command_e AT_STAR_ESAP_Handle(AT_ParserState_s *parser_p,
                                 AT_CommandLine_t info_text,
                                 AT_ResponseMessage_t response_p);

AT_Command_e AT_PLUS_CLVL_Handle(AT_ParserState_s *parser_p,
                                 AT_CommandLine_t info_text,
                                 AT_ResponseMessage_t response_p);

AT_Command_e AT_PLUS_CEAP_Handle(AT_ParserState_s *parser_p,
                                 AT_CommandLine_t info_text,
                                 AT_ResponseMessage_t response_p);

AT_Command_e AT_PLUS_CERP_Handle(AT_ParserState_s *parser_p,
                                 AT_CommandLine_t info_text,
                                 AT_ResponseMessage_t response_p);

AT_Command_e AT_PLUS_CUAD_Handle(AT_ParserState_s *parser_p,
                                 AT_CommandLine_t info_text,
                                 AT_ResponseMessage_t response_p);

AT_Command_e AT_PLUS_DS_Handle(AT_ParserState_s *parser_p,
                               AT_CommandLine_t info_text,
                               AT_ResponseMessage_t message);

AT_Command_e AT_STAR_EHNET_Handle(AT_ParserState_s *parser_p,
                                  AT_CommandLine_t info_text,
                                  AT_ResponseMessage_t response_p);

AT_Command_e AT_STAR_EPBC_Handle(AT_ParserState_s *parser_p,
                                 AT_CommandLine_t info_text,
                                 AT_ResponseMessage_t response_p);

AT_Command_e AT_STAR_ERFSTATE_Handle(AT_ParserState_s *parser_p,
                                     AT_CommandLine_t info_text,
                                     AT_ResponseMessage_t response_p);

AT_Command_e AT_PLUS_CSIM_Handle(AT_ParserState_s *parser_p,
                                 AT_CommandLine_t info_text,
                                 AT_ResponseMessage_t response_p);

AT_Command_e AT_PLUS_GCAP_Handle(AT_ParserState_s *parser_p,
                                 AT_CommandLine_t info_text,
                                 AT_ResponseMessage_t message);

AT_Command_e AT_PLUS_IFC_Handle(AT_ParserState_s *parser_p,
                                AT_CommandLine_t info_text,
                                AT_ResponseMessage_t message);

AT_Command_e AT_PLUS_CEER_Handle(AT_ParserState_s *parser_p,
                                 AT_CommandLine_t info_text,
                                 AT_ResponseMessage_t response_p);
AT_Command_e AT_STAR_EMSR_Handle(AT_ParserState_s *parser_p,
                                 AT_CommandLine_t info_text,
                                 AT_ResponseMessage_t response_p);
AT_Command_e AT_STAR_EHSTACT_Handle(AT_ParserState_s *parser_p,
                                    AT_CommandLine_t info_text,
                                    AT_ResponseMessage_t message);

AT_Command_e AT_STAR_ECPSCHBLKR_Handle(AT_ParserState_s *parser_p,
                                       AT_CommandLine_t info_text,
                                       AT_ResponseMessage_t message);

AT_Command_e AT_STAR_ECPSAUTHU_Handle(AT_ParserState_s *parser_p,
                                      AT_CommandLine_t info_text,
                                      AT_ResponseMessage_t message);

AT_Command_e AT_STAR_ECPSAUTHDU_Handle(AT_ParserState_s *parser_p,
                                       AT_CommandLine_t info_text,
                                       AT_ResponseMessage_t message);

AT_Command_e AT_STAR_ECPSIMEIW_Handle(AT_ParserState_s *parser_p,
                                      AT_CommandLine_t info_text,
                                      AT_ResponseMessage_t message);

AT_Command_e AT_STAR_ECPSLCW_Handle(AT_ParserState_s *parser_p,
                                    AT_CommandLine_t info_text,
                                    AT_ResponseMessage_t message);

AT_Command_e AT_STAR_ECPSSIMLOCK_Handle(AT_ParserState_s *parser_p,
                                        AT_CommandLine_t info_text,
                                        AT_ResponseMessage_t message);

AT_Command_e AT_STAR_ECPSSIMLOCKD_Handle(AT_ParserState_s *parser_p,
        AT_CommandLine_t info_text,
        AT_ResponseMessage_t message);

AT_Command_e AT_STAR_ECRAT_Handle(AT_ParserState_s *parser_p,
                                  AT_CommandLine_t info_text,
                                  AT_ResponseMessage_t message);

AT_Command_e AT_STAR_ECSPSAW_Handle(AT_ParserState_s *parser_p,
                                    AT_CommandLine_t info_text,
                                    AT_ResponseMessage_t message);

AT_Command_e AT_STAR_ECSPSAR_Handle(AT_ParserState_s *parser_p,
                                    AT_CommandLine_t info_text,
                                    AT_ResponseMessage_t message);

AT_Command_e AT_STAR_EGNCI_Handle(AT_ParserState_s *parser_p,
                                  AT_CommandLine_t info_text,
                                  AT_ResponseMessage_t message);

AT_Command_e AT_STAR_ESLEEP_Handle(AT_ParserState_s *parser_p,
                                   AT_CommandLine_t info_text,
                                   AT_ResponseMessage_t message);

AT_Command_e AT_STAR_EWNCI_Handle(AT_ParserState_s *parser_p,
                                  AT_CommandLine_t info_text,
                                  AT_ResponseMessage_t message);

AT_Command_e AT_STAR_EGSCI_Handle(AT_ParserState_s *parser_p,
                                  AT_CommandLine_t info_text,
                                  AT_ResponseMessage_t message);

AT_Command_e AT_STAR_EWSCI_Handle(AT_ParserState_s *parser_p,
                                  AT_CommandLine_t info_text,
                                  AT_ResponseMessage_t message);

AT_Command_e AT_STAR_EPWRRED_Handle(AT_ParserState_s *parser_p,
                                    AT_CommandLine_t info_text,
                                    AT_ResponseMessage_t message);

AT_Command_e AT_STAR_EEMPAGE_Handle(AT_ParserState_s *parser_p,
                                    AT_CommandLine_t info_text,
                                    AT_ResponseMessage_t message);

AT_Command_e AT_STAR_ECPSAUTHPE_Handle(AT_ParserState_s *parser_p,
                                       AT_CommandLine_t info_text,
                                       AT_ResponseMessage_t message);

AT_Command_e AT_STAR_ECPSDEAUTHPE_Handle(AT_ParserState_s *parser_p,
        AT_CommandLine_t info_text,
        AT_ResponseMessage_t message);

AT_Command_e AT_STAR_ECPSLOCKBOOTP_Handle(AT_ParserState_s *parser_p,
        AT_CommandLine_t info_text,
        AT_ResponseMessage_t message);

AT_Command_e AT_STAR_ECPSINITARB_Handle(AT_ParserState_s *parser_p,
                                        AT_CommandLine_t info_text,
                                        AT_ResponseMessage_t message);

AT_Command_e AT_STAR_ECPSDTW_Handle(AT_ParserState_s *parser_p,
                                    AT_CommandLine_t info_text,
                                    AT_ResponseMessage_t message);

AT_Command_e AT_STAR_ESVN_Handle(AT_ParserState_s *parser_p,
                                 AT_CommandLine_t info_text,
                                 AT_ResponseMessage_t message);
								 
AT_Command_e AT_PLUS_RADIOVER_Handle(AT_ParserState_s *parser_p,
                                     AT_CommandLine_t info_text,
                                     AT_ResponseMessage_t message);

void handle_incoming_ECAV_event(exe_ecav_response_t *response_p);

void handle_incoming_ETZV_event(exe_etzv_response_t *response_p);

void handle_incoming_CUSD_event(exe_cusd_response_t *response_p);

void ECAV_build_report(AT_CommandLine_t info_text,
                       unsigned char ccid,
                       exe_call_status_t call_status,
                       exe_call_type_t call_type,
                       unsigned char process_id,
                       unsigned char exit_cause,
                       unsigned char *subscriber_number_p,
                       int subscriber_number_addr_type);

void CGEV_build_report(AT_CommandLine_t info_text,
                       exe_cgev_response_t *response_p);

void handle_incoming_CCWA_event(exe_ccwa_response_t *response_p);

void handle_incoming_ciev_signal_event(void *response_p);

void handle_incoming_ciev_storage_event(void *response_p);

void handle_incoming_cmt_event(exe_sms_mt_t *response_p);

void handle_incoming_cmti_event(exe_sms_mt_t *response_p);

void handle_incoming_cds_event(exe_sms_status_report_t *response_p);

void handle_incoming_cdsi_event(exe_sms_status_report_t *response_p);

void handle_incoming_cbm_event(exe_cbm_t *response_p);

void handle_incoming_CLIP_event(exe_clip_response_t *response_p);

AT_Command_e AT_PLUS_CNAP_Handle(AT_ParserState_s *parser_p,
                                 AT_CommandLine_t result_text,
                                 AT_ResponseMessage_t message);

void handle_incoming_CNAP_event(exe_cnap_response_t *response_p);


void handle_incoming_CSSI_event(exe_cssi_response_t *response_p);

void handle_incoming_CSSU_event(exe_cssu_response_t *response_p);

void handle_incoming_CUUS1I_event(exe_cuus1_response_t *response_p);

void handle_incoming_CUUS1U_event(exe_cuus1_response_t *response_p);

void handle_incoming_RING_event(exe_call_type_t *call_type_p);

void handle_incoming_call_response_event(exe_call_response_t *call_response_p);

void handle_incoming_CGEV_event(exe_cgev_response_t *response_p);

void handle_incoming_CR_event(void);

void handle_incoming_ecrat_event(exe_ecrat_ind_t *response_p);

void handle_incoming_ennir_event(exe_ennir_ind_t *response_p);

void handle_incoming_elat_event(exe_elat_response_t *response_p);

void handle_incoming_ESIMSR_event(exe_esimsr_sim_state_t *response_p);

void handle_incoming_esimrf_event(exe_esimrf_unsol_t *esimrf_data_p);

void handle_incoming_epev_event();

void handle_incoming_ETZV_event(exe_etzv_response_t *response_p);

void handle_incoming_stkn_event(exe_apdu_data_t *response_p);

void handle_incoming_stki_event(exe_apdu_data_t *response_p);

void handle_incoming_stkend_event();

void handle_incoming_ATD_event(void);

void handle_incoming_erfstate_event(void *data_p);

void atc_hand_free_cgact_set_list(exe_cgact_set_request_t *cgact_p);

void handle_incoming_RSSI_event(void *response_p);

void handle_incoming_EEMPAGE_event(exe_empage_urc_t *response_p);

bool convert_ascii_hex_to_bin_buf(const char *const data_p,
                                  uint8_t *const bin_buf_p,
                                  const uint16_t length);

bool service_availability_check_required(uint16_t sim_file_id);


#ifdef EXE_USE_ATC_CUSTOMER_EXTENSIONS
#include <atc_extended_handlers.h>
#endif

/*************************************************************************/
#endif /* INCLUSION_GUARD_PAS_AT_DEFS_H */
