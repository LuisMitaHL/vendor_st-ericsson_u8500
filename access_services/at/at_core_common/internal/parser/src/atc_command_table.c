/**
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */


/*************************************************************************
* Includes
*************************************************************************/

/* ATC related header files */
#include <atc_config.h>
#include <atc_command_table.h>
#include <atc_handlers.h>


/*************************************************************************
* Types, constants and external variables
*************************************************************************/

const AT_CommandTableRecord_s AT_CommandTable[] = {
#ifdef AT_AND_C_ENABLED
    { AT_AND_C,        "&C",       AT_AND_C_Handle,            AT_MODE_DO | AT_MODE_READ | AT_MODE_TEST,    AT_CLASS_BASIC },
#endif
#ifdef AT_AND_D_ENABLED
    { AT_AND_D,        "&D",       AT_AND_D_Handle,            AT_MODE_DO | AT_MODE_READ | AT_MODE_TEST,    AT_CLASS_BASIC },
#endif
#ifdef AT_AND_F_ENABLED
    { AT_AND_F,        "&F",       AT_AND_F_Handle,            AT_MODE_DO | AT_MODE_SET | AT_MODE_TEST,     AT_CLASS_BASIC },
#endif
#ifdef AT_STAR_ENABLED
    { AT_STAR,         "*",        AT_STAR_Handle,             AT_MODE_DO | AT_MODE_TEST,                   AT_CLASS_ERICSSON },
#endif
#ifdef AT_STAR_CPI_ENABLED
    { AT_STAR_CPI,     "*CPI",     AT_STAR_CPI_Handle,         AT_MODE_SET | AT_MODE_READ | AT_MODE_TEST,   AT_CLASS_ERICSSON },
#endif
#ifdef AT_STAR_EACE_ENABLED
    { AT_STAR_EACE,    "*EACE",    AT_STAR_EACE_Handle,        AT_MODE_SET | AT_MODE_READ | AT_MODE_TEST,   AT_CLASS_ERICSSON },
#endif
#ifdef AT_STAR_EACS_ENABLED
    { AT_STAR_EACS,    "*EACS",    AT_STAR_EACS_Handle,        AT_MODE_SET | AT_MODE_READ | AT_MODE_TEST,   AT_CLASS_ERICSSON },
#endif
#ifdef AT_STAR_EALS_ENABLED
    { AT_STAR_EALS,    "*EALS",    AT_STAR_EALS_Handle,        AT_MODE_DO  | AT_MODE_TEST,                  AT_CLASS_ERICSSON },
#endif
#ifdef AT_STAR_EASY_ENABLED
    { AT_STAR_EASY,    "*EASY",    AT_STAR_EASY_Handle,        AT_MODE_SET | AT_MODE_READ | AT_MODE_TEST,   AT_CLASS_ERICSSON },
#endif
#ifdef AT_STAR_EBAT_ENABLED
    { AT_STAR_EBAT,    "*EBAT",    AT_STAR_EBAT_Handle,        AT_MODE_SET | AT_MODE_TEST,                  AT_CLASS_ERICSSON },
#endif
#ifdef AT_STAR_EBCA_ENABLED
    { AT_STAR_EBCA,    "*EBCA",    AT_STAR_EBCA_Handle,        AT_MODE_SET | AT_MODE_READ | AT_MODE_TEST,   AT_CLASS_ERICSSON },
#endif
#ifdef AT_STAR_ECAM_ENABLED
    { AT_STAR_ECAM,    "*ECAM",    AT_STAR_ECAM_Handle,        AT_MODE_SET | AT_MODE_READ | AT_MODE_TEST,   AT_CLASS_ERICSSON },
#endif
#ifdef AT_STAR_ECME_ENABLED
    { AT_STAR_ECME,    "*ECME",    AT_STAR_ECME_Handle,        AT_MODE_SET | AT_MODE_READ | AT_MODE_TEST,   AT_CLASS_ERICSSON },
#endif
#ifdef AT_STAR_ECPSCHBLKR_ENABLE
    { AT_STAR_ECPSCHBLKR, "*ECPSCHBLKR", AT_STAR_ECPSCHBLKR_Handle,   AT_MODE_DO | AT_MODE_TEST,            AT_CLASS_ERICSSON },
#endif
#ifdef AT_STAR_ECPSAUTHPE_ENABLE
    { AT_STAR_ECPSAUTHPE, "*ECPSAUTHPE", AT_STAR_ECPSAUTHPE_Handle,          AT_MODE_DO | AT_MODE_TEST,         AT_CLASS_ERICSSON },
#endif
#ifdef AT_STAR_ECPSDEAUTHPE_ENABLE
    { AT_STAR_ECPSDEAUTHPE, "*ECPSDEAUTHPE", AT_STAR_ECPSDEAUTHPE_Handle,    AT_MODE_DO | AT_MODE_TEST,         AT_CLASS_ERICSSON },
#endif
#ifdef AT_STAR_ECPSLOCKBOOTP_ENABLE
    { AT_STAR_ECPSLOCKBOOTP, "*ECPSLOCKBOOTP", AT_STAR_ECPSLOCKBOOTP_Handle, AT_MODE_DO | AT_MODE_TEST,         AT_CLASS_ERICSSON },
#endif
#ifdef AT_STAR_ECPSINITARB_ENABLE
    { AT_STAR_ECPSINITARB, "*ECPSINITARB", AT_STAR_ECPSINITARB_Handle, AT_MODE_SET | AT_MODE_TEST,              AT_CLASS_ERICSSON },
#endif
#ifdef AT_STAR_ECPSDTW_ENABLE
    { AT_STAR_ECPSDTW, "*ECPSDTW", AT_STAR_ECPSDTW_Handle,     AT_MODE_SET | AT_MODE_TEST,                  AT_CLASS_ERICSSON },
#endif

#ifdef AT_STAR_ECPSAUTHU_ENABLE
    { AT_STAR_ECPSAUTHU, "*ECPSAUTHU", AT_STAR_ECPSAUTHU_Handle, AT_MODE_SET | AT_MODE_TEST,                AT_CLASS_ERICSSON },
#endif
#ifdef AT_STAR_ECPSAUTHDU_ENABLE
    { AT_STAR_ECPSAUTHDU, "*ECPSAUTHDU", AT_STAR_ECPSAUTHDU_Handle, AT_MODE_SET | AT_MODE_TEST,             AT_CLASS_ERICSSON },
#endif
#ifdef AT_STAR_ECPSIMEIW_ENABLE
    { AT_STAR_ECPSIMEIW, "*ECPSIMEIW", AT_STAR_ECPSIMEIW_Handle, AT_MODE_SET | AT_MODE_TEST,                AT_CLASS_ERICSSON },
#endif
#ifdef AT_STAR_ECPSLCW_ENABLE
    { AT_STAR_ECPSLCW, "*ECPSLCW", AT_STAR_ECPSLCW_Handle,     AT_MODE_SET | AT_MODE_TEST,                  AT_CLASS_ERICSSON },
#endif
#ifdef AT_STAR_ECPSSIMLOCK_ENABLE
    { AT_STAR_ECPSSIMLOCK, "*ECPSSIMLOCK", AT_STAR_ECPSSIMLOCK_Handle, AT_MODE_SET | AT_MODE_READ | AT_MODE_TEST, AT_CLASS_ERICSSON },
#endif
#ifdef AT_STAR_ECPSSIMLOCKD_ENABLE
    { AT_STAR_ECPSSIMLOCKD, "*ECPSSIMLOCKD", AT_STAR_ECPSSIMLOCKD_Handle, AT_MODE_SET | AT_MODE_TEST,       AT_CLASS_ERICSSON },
#endif
#ifdef AT_STAR_ECRAT_ENABLE
    { AT_STAR_ECRAT, "*ECRAT",     AT_STAR_ECRAT_Handle,       AT_MODE_SET | AT_MODE_READ | AT_MODE_TEST,  AT_CLASS_ERICSSON },
#endif
#ifdef AT_STAR_ECSPSAW_ENABLE
    { AT_STAR_ECSPSAW, "*ECSPSAW", AT_STAR_ECSPSAW_Handle,     AT_MODE_SET | AT_MODE_TEST,                 AT_CLASS_ERICSSON },
#endif
#ifdef AT_STAR_ECSPSAR_ENABLE
    { AT_STAR_ECSPSAR, "*ECSPSAR", AT_STAR_ECSPSAR_Handle,     AT_MODE_SET | AT_MODE_TEST,                 AT_CLASS_ERICSSON },
#endif
#ifdef AT_STAR_ECAS_ENABLED
    { AT_STAR_ECAS,    "*ECAS",    AT_STAR_ECAS_Handle,        AT_MODE_SET | AT_MODE_READ | AT_MODE_TEST,   AT_CLASS_ERICSSON },
#endif
#ifdef AT_STAR_ECLKM_ENABLED
    { AT_STAR_ECLKM,   "*ECLKM",   AT_STAR_ECLKM_Handle,       AT_MODE_DO  | AT_MODE_TEST,                  AT_CLASS_ERICSSON },
#endif
#ifdef CFG_ENABLE_COPS_CSCA
#ifdef AT_STAR_ECSCA_ENABLED
    { AT_STAR_ECSCA,   "*ECSCA",   AT_STAR_ECSCA_Handle,       AT_MODE_DO  | AT_MODE_TEST,                  AT_CLASS_ERICSSON },
#endif
#endif
#ifdef AT_STAR_ECHD_ENABLED
    { AT_STAR_ECHD,    "*ECHD",    AT_STAR_ECHD_Handle,        AT_MODE_SET | AT_MODE_TEST,                  AT_CLASS_ERICSSON },
#endif
#ifdef AT_STAR_ECUR_ENABLED
    { AT_STAR_ECUR,    "*ECUR",    AT_STAR_ECUR_Handle,        AT_MODE_SET | AT_MODE_TEST,                  AT_CLASS_ERICSSON },
#endif
#ifdef AT_STAR_EGIR_ENABLED
    { AT_STAR_EGIR,    "*EGIR",    AT_STAR_EGIR_Handle,        AT_MODE_SET | AT_MODE_TEST,                  AT_CLASS_ERICSSON },
#endif
#ifdef AT_STAR_EHNET_ENABLED
    { AT_STAR_EHNET,   "*EHNET",   AT_STAR_EHNET_Handle,       AT_MODE_SET | AT_MODE_TEST,                  AT_CLASS_ERICSSON },
#endif
#ifdef AT_STAR_EHSTACT_ENABLED
    { AT_STAR_EHSTACT, "*EHSTACT", AT_STAR_EHSTACT_Handle,     AT_MODE_SET | AT_MODE_TEST,                  AT_CLASS_ERICSSON },
#endif
#ifdef AT_STAR_EIAAUW_ENABLED
    { AT_STAR_EIAAUW,  "*EIAAUW",  AT_STAR_EIAAUW_Handle,      AT_MODE_SET | AT_MODE_TEST,                  AT_CLASS_ERICSSON },
#endif
#ifdef AT_STAR_EIBA_ENABLED
    { AT_STAR_EIBA,    "*EIBA",    AT_STAR_EIBA_Handle,        AT_MODE_SET | AT_MODE_TEST,                  AT_CLASS_ERICSSON },
#endif
#ifdef AT_STAR_EKSE_ENABLED
    { AT_STAR_EKSE,    "*EKSE",    AT_STAR_EKSE_Handle,        AT_MODE_SET | AT_MODE_TEST,                  AT_CLASS_ERICSSON },
#endif
#ifdef AT_STAR_ELAT_ENABLED
    { AT_STAR_ELAT,    "*ELAT",    AT_STAR_ELAT_Handle,        AT_MODE_SET | AT_MODE_READ | AT_MODE_TEST,   AT_CLASS_ERICSSON },
#endif
#ifdef AT_STAR_ELIN_ENABLED
    { AT_STAR_ELIN,    "*ELIN",    AT_STAR_ELIN_Handle,        AT_MODE_SET | AT_MODE_READ | AT_MODE_TEST,   AT_CLASS_ERICSSON },
#endif
#ifdef AT_STAR_EMSR_ENABLED
    { AT_STAR_EMSR,    "*EMSR",    AT_STAR_EMSR_Handle,        AT_MODE_DO | AT_MODE_TEST,                   AT_CLASS_ERICSSON },
#endif
#ifdef AT_STAR_ENAP_ENABLED
    { AT_STAR_ENAP,    "*ENAP",    AT_STAR_ENAP_Handle,        AT_MODE_SET | AT_MODE_READ | AT_MODE_TEST,   AT_CLASS_ERICSSON },
#endif
#ifdef AT_STAR_EPBC_ENABLED
    { AT_STAR_EPBC,    "*EPBC",    AT_STAR_EPBC_Handle,        AT_MODE_DO  | AT_MODE_TEST,                  AT_CLASS_ERICSSON },
#endif
#ifdef AT_STAR_EPEE_ENABLED
    { AT_STAR_EPEE,    "*EPEE",    AT_STAR_EPEE_Handle,        AT_MODE_SET | AT_MODE_READ | AT_MODE_TEST,   AT_CLASS_ERICSSON },
#endif
#ifdef AT_STAR_EPHD_ENABLED
    { AT_STAR_EPHD,    "*EPHD",    AT_STAR_EPHD_Handle,        AT_MODE_SET | AT_MODE_READ | AT_MODE_TEST,   AT_CLASS_ERICSSON },
#endif
#ifdef AT_STAR_EPINR_ENABLED
    { AT_STAR_EPINR,   "*EPINR",   AT_STAR_EPINR_Handle,       AT_MODE_SET | AT_MODE_READ | AT_MODE_TEST,   AT_CLASS_ERICSSON },
#endif
#ifdef AT_STAR_EPPSD_ENABLED
    { AT_STAR_EPPSD,   "*EPPSD",   AT_STAR_EPPSD_Handle,       AT_MODE_SET | AT_MODE_TEST,                  AT_CLASS_ERICSSON },
#endif
#ifdef AT_STAR_EPSB_ENABLED
    { AT_STAR_EPSB,   "*EPSB",     AT_STAR_EPSB_Handle,        AT_MODE_SET | AT_MODE_READ | AT_MODE_TEST,   AT_CLASS_ERICSSON },
#endif
#ifdef AT_STAR_ENNIR_ENABLED
    { AT_STAR_ENNIR,   "*ENNIR",     AT_STAR_ENNIR_Handle,        AT_MODE_SET | AT_MODE_READ | AT_MODE_TEST,   AT_CLASS_ERICSSON },
#endif
#ifdef AT_STAR_EPWRRED_ENABLED
    { AT_STAR_EPWRRED, "*EPWRRED",  AT_STAR_EPWRRED_Handle,    AT_MODE_SET | AT_MODE_TEST,                  AT_CLASS_ERICSSON },
#endif
#ifdef AT_STAR_EQVL_ENABLED
    { AT_STAR_EQVL,    "*EQVL",    AT_STAR_EQVL_Handle,        AT_MODE_SET | AT_MODE_READ | AT_MODE_TEST,   AT_CLASS_ERICSSON },
#endif
#ifdef AT_STAR_EREG_ENABLED
    { AT_STAR_EREG,    "*EREG",    AT_STAR_EREG_Handle,        AT_MODE_SET | AT_MODE_READ | AT_MODE_TEST,   AT_CLASS_ERICSSON },
#endif
#ifdef AT_STAR_ERFSTATE_ENABLED
    { AT_STAR_ERFSTATE, "*ERFSTATE", AT_STAR_ERFSTATE_Handle,    AT_MODE_SET | AT_MODE_READ | AT_MODE_TEST,   AT_CLASS_ERICSSON },
#endif
#ifdef AT_STAR_ESIMRF_ENABLED
    { AT_STAR_ESIMRF,  "*ESIMRF",  AT_STAR_ESIMRF_Handle,      AT_MODE_SET | AT_MODE_READ | AT_MODE_TEST,   AT_CLASS_ERICSSON },
#endif
#ifdef AT_STAR_ESIMSR_ENABLED
    { AT_STAR_ESIMSR,  "*ESIMSR",  AT_STAR_ESIMSR_Handle,      AT_MODE_SET | AT_MODE_READ | AT_MODE_TEST,   AT_CLASS_ERICSSON },
#endif
#ifdef AT_STAR_ESAG_ENABLED
    { AT_STAR_ESAG,    "*ESAG",    AT_STAR_ESAG_Handle,        AT_MODE_SET | AT_MODE_TEST,                  AT_CLASS_ERICSSON },
#endif
#ifdef AT_STAR_ESAP_ENABLED
    { AT_STAR_ESAP,    "*ESAP",    AT_STAR_ESAP_Handle,        AT_MODE_SET | AT_MODE_TEST,                  AT_CLASS_ERICSSON },
#endif
#ifdef AT_STAR_ESBL_ENABLED
    { AT_STAR_ESBL,    "*ESBL",    AT_STAR_ESBL_Handle,        AT_MODE_SET | AT_MODE_READ | AT_MODE_TEST,   AT_CLASS_ERICSSON },
#endif
#ifdef AT_STAR_ESDG_ENABLED
    { AT_STAR_ESDG,    "*ESDG",    AT_STAR_ESDG_Handle,        AT_MODE_SET | AT_MODE_TEST,                  AT_CLASS_ERICSSON },
#endif
#ifdef AT_STAR_ESDI_ENABLED
    { AT_STAR_ESDI,    "*ESDI",    AT_STAR_ESDI_Handle,        AT_MODE_SET | AT_MODE_TEST,                  AT_CLASS_ERICSSON },
#endif
#ifdef AT_STAR_ESGR_ENABLED
    { AT_STAR_ESGR,    "*ESGR",    AT_STAR_ESGR_Handle,        AT_MODE_DO  | AT_MODE_TEST,                  AT_CLASS_ERICSSON },
#endif
#ifdef AT_STAR_ESHLVOCR_ENABLED
    { AT_STAR_ESHLVOCR, "*ESHLVOCR", AT_STAR_ESHLVOCR_Handle,  AT_MODE_SET  | AT_MODE_TEST,                 AT_CLASS_ERICSSON },
#endif
#ifdef AT_STAR_ESIMR_ENABLED
    { AT_STAR_ESIMR,   "*ESIMR",   AT_STAR_ESIMR_Handle,       AT_MODE_DO  | AT_MODE_SET | AT_MODE_TEST,    AT_CLASS_ERICSSON },
#endif
#ifdef AT_STAR_ESKL_ENABLED
    { AT_STAR_ESKL,    "*ESKL",    AT_CommandHandlerFunction,  AT_MODE_SET | AT_MODE_READ | AT_MODE_TEST,   AT_CLASS_ERICSSON },
#endif
#ifdef AT_STAR_ESKS_ENABLED
    { AT_STAR_ESKS,    "*ESKS",    AT_CommandHandlerFunction,  AT_MODE_SET | AT_MODE_READ | AT_MODE_TEST,   AT_CLASS_ERICSSON },
#endif
#ifdef AT_STAR_ESLEEP_ENABLED
    { AT_STAR_ESLEEP,  "*ESLEEP",  AT_STAR_ESLEEP_Handle,      AT_MODE_SET | AT_MODE_TEST,                  AT_CLASS_ERICSSON },
#endif
#ifdef AT_STAR_ESLN_ENABLED
    { AT_STAR_ESLN,    "*ESLN",    AT_STAR_ESLN_Handle,        AT_MODE_SET | AT_MODE_READ | AT_MODE_TEST,   AT_CLASS_ERICSSON },
#endif
#ifdef AT_STAR_ESNU_ENABLED
    { AT_STAR_ESNU,    "*ESNU",    AT_STAR_ESNU_Handle,        AT_MODE_SET | AT_MODE_READ | AT_MODE_TEST,   AT_CLASS_ERICSSON },
#endif
#ifdef AT_STAR_ESOM_ENABLED
    { AT_STAR_ESOM,    "*ESOM",    AT_STAR_ESOM_Handle,        AT_MODE_SET | AT_MODE_TEST,                  AT_CLASS_ERICSSON },
#endif
#ifdef AT_STAR_ESVN_ENABLED
    { AT_STAR_ESVN,    "*ESVN",    AT_STAR_ESVN_Handle,        AT_MODE_READ,   AT_CLASS_ERICSSON },
#endif
#ifdef AT_STAR_ETCH_ENABLED
    { AT_STAR_ETCH,    "*ETCH",    AT_STAR_ETCH_Handle,        AT_MODE_DO  | AT_MODE_TEST,                  AT_CLASS_ERICSSON },
#endif
#ifdef AT_STAR_ETTY_ENABLED
    { AT_STAR_ETTY,    "*ETTY",    AT_STAR_ETTY_Handle,        AT_MODE_SET | AT_MODE_READ | AT_MODE_TEST,   AT_CLASS_ERICSSON },
#endif
#ifdef AT_STAR_ETZR_ENABLED
    { AT_STAR_ETZR,    "*ETZR",    AT_STAR_ETZR_Handle,        AT_MODE_SET | AT_MODE_READ | AT_MODE_TEST,   AT_CLASS_ERICSSON },
#endif
#ifdef AT_STAR_EUPLINK_ENABLED
    { AT_STAR_EUPLINK, "*EUPLINK", AT_STAR_EUPLINK_Handle,     AT_MODE_SET,                                 AT_CLASS_ERICSSON },
#endif
#ifdef AT_STAR_EVIB_ENABLED
    { AT_STAR_EVIB,    "*EVIB",    AT_STAR_EVIB_Handle,        AT_MODE_SET |    AT_MODE_TEST,               AT_CLASS_ERICSSON },
#endif
#ifdef AT_STAR_EXVC_ENABLED
    { AT_STAR_EXVC,    "*EXVC",    AT_STAR_EXVC_Handle,        AT_MODE_SET | AT_MODE_READ | AT_MODE_TEST,   AT_CLASS_ERICSSON },
#endif
#ifdef AT_STAR_STKC_ENABLED
    { AT_STAR_STKC,    "*STKC",    AT_STAR_STKC_Handle,        AT_MODE_SET | AT_MODE_READ | AT_MODE_TEST,   AT_CLASS_ERICSSON },
#endif
#ifdef AT_STAR_STKE_ENABLED
    { AT_STAR_STKE,    "*STKE",    AT_STAR_STKE_Handle,        AT_MODE_SET | AT_MODE_TEST,                  AT_CLASS_ERICSSON },
#endif
#ifdef AT_STAR_STKR_ENABLED
    { AT_STAR_STKR,    "*STKR",    AT_STAR_STKR_Handle,        AT_MODE_SET | AT_MODE_TEST,                  AT_CLASS_ERICSSON },
#endif
#ifdef AT_STAR_ZPPI_ENABLED
    { AT_STAR_ZPPI,    "*ZPPI",    AT_STAR_ZPPI_Handle,        AT_MODE_SET | AT_MODE_TEST,                  AT_CLASS_ERICSSON },
#endif
#ifdef AT_STAR_EGNCI_ENABLED
    { AT_STAR_EGNCI,   "*EGNCI",   AT_STAR_EGNCI_Handle,       AT_MODE_DO | AT_MODE_TEST,                   AT_CLASS_ERICSSON },
#endif
#ifdef AT_STAR_EWNCI_ENABLED
    { AT_STAR_EWNCI,   "*EWNCI",   AT_STAR_EWNCI_Handle,       AT_MODE_DO | AT_MODE_TEST,                   AT_CLASS_ERICSSON },
#endif
#ifdef AT_STAR_EGSCI_ENABLED
    { AT_STAR_EGSCI,   "*EGSCI",   AT_STAR_EGSCI_Handle,       AT_MODE_DO | AT_MODE_TEST,                   AT_CLASS_ERICSSON },
#endif
#ifdef AT_STAR_EWSCI_ENABLED
    { AT_STAR_EWSCI,   "*EWSCI",   AT_STAR_EWSCI_Handle,       AT_MODE_DO | AT_MODE_TEST,                   AT_CLASS_ERICSSON },
#endif
#ifdef AT_PLUS_BINP_ENABLED
    { AT_PLUS_BINP,    "+BINP",    AT_PLUS_BINP_Handle,        AT_MODE_SET | AT_MODE_TEST,                  AT_CLASS_GSM_BASIC },
#endif
#ifdef AT_PLUS_BLDN_ENABLED
    { AT_PLUS_BLDN,    "+BLDN",    AT_PLUS_BLDN_Handle,        AT_MODE_DO  | AT_MODE_TEST,                  AT_CLASS_GSM_BASIC },
#endif
#ifdef AT_PLUS_CACM_ENABLED
    { AT_PLUS_CACM,    "+CACM",    AT_PLUS_CACM_Handle,        AT_MODE_SET | AT_MODE_READ | AT_MODE_TEST,   AT_CLASS_GSM_BASIC },
#endif
#ifdef AT_PLUS_CALA_ENABLED
    { AT_PLUS_CALA,    "+CALA",    AT_PLUS_CALA_Handle,        AT_MODE_SET | AT_MODE_READ | AT_MODE_TEST,   AT_CLASS_GSM_BASIC },
#endif
#ifdef AT_PLUS_CALD_ENABLED
    { AT_PLUS_CALD,    "+CALD",    AT_PLUS_CALD_Handle,        AT_MODE_SET | AT_MODE_TEST,                  AT_CLASS_GSM_BASIC },
#endif
#ifdef AT_PLUS_CAMM_ENABLED
    { AT_PLUS_CAMM,    "+CAMM",    AT_PLUS_CAMM_Handle,        AT_MODE_SET | AT_MODE_READ | AT_MODE_TEST,   AT_CLASS_GSM_BASIC },
#endif
#ifdef AT_PLUS_CAOC_ENABLED
    { AT_PLUS_CAOC,    "+CAOC",    AT_PLUS_CAOC_Handle,        AT_MODE_DO  | AT_MODE_SET | AT_MODE_READ | AT_MODE_TEST,    AT_CLASS_GSM_BASIC },
#endif
#ifdef AT_PLUS_CBC_ENABLED
    { AT_PLUS_CBC,     "+CBC",     AT_PLUS_CBC_Handle,         AT_MODE_DO  | AT_MODE_READ | AT_MODE_TEST,   AT_CLASS_GSM_BASIC },
#endif
#ifdef AT_PLUS_CCFC_ENABLED
    { AT_PLUS_CCFC,    "+CCFC",    AT_PLUS_CCFC_Handle,        AT_MODE_SET | AT_MODE_TEST,                  AT_CLASS_GSM_BASIC },
#endif
#ifdef AT_PLUS_CCLK_ENABLED
    { AT_PLUS_CCLK,    "+CCLK",    AT_PLUS_CCLK_Handle,        AT_MODE_SET | AT_MODE_READ | AT_MODE_TEST,   AT_CLASS_GSM_BASIC },
#endif
#ifdef AT_PLUS_CCWA_ENABLED
    /* TODO Add AT_CLASS_ABORTABLE to CCWA when it is clear what MAL function should be called in request_abort_call_waiting_network_operation */
    { AT_PLUS_CCWA,    "+CCWA",    AT_PLUS_CCWA_Handle,        AT_MODE_SET | AT_MODE_READ | AT_MODE_TEST,   AT_CLASS_GSM_BASIC },
#endif
#ifdef AT_PLUS_CCWE_ENABLED
    { AT_PLUS_CCWE,    "+CCWE",    AT_PLUS_CCWE_Handle,        AT_MODE_SET | AT_MODE_READ | AT_MODE_TEST,   AT_CLASS_GSM_BASIC },
#endif
#ifdef AT_PLUS_CEAP_ENABLED
    { AT_PLUS_CEAP,    "+CEAP",    AT_PLUS_CEAP_Handle,        AT_MODE_SET | AT_MODE_TEST,                  AT_CLASS_GSM_BASIC },
#endif
#ifdef AT_PLUS_CEER_ENABLED
    { AT_PLUS_CEER,   "+CEER",     AT_PLUS_CEER_Handle,        AT_MODE_DO | AT_MODE_TEST,                    AT_CLASS_GSM_BASIC },
#endif
#ifdef AT_PLUS_CERP_ENABLED
    { AT_PLUS_CERP,    "+CERP",    AT_PLUS_CERP_Handle,        AT_MODE_SET | AT_MODE_TEST,                  AT_CLASS_GSM_BASIC },
#endif
#ifdef AT_PLUS_CFUN_ENABLED
    { AT_PLUS_CFUN,    "+CFUN",    AT_PLUS_CFUN_Handle,        AT_MODE_SET | AT_MODE_READ | AT_MODE_TEST,   AT_CLASS_GSM_BASIC },
#endif
#ifdef AT_PLUS_CGACT_ENABLED
    { AT_PLUS_CGACT,   "+CGACT",   AT_PLUS_CGACT_Handle,       AT_MODE_SET | AT_MODE_READ | AT_MODE_TEST,   AT_CLASS_GSM_BASIC },
#endif
#ifdef AT_PLUS_CGATT_ENABLED
    { AT_PLUS_CGATT,   "+CGATT",   AT_PLUS_CGATT_Handle,       AT_MODE_DO  | AT_MODE_SET | AT_MODE_READ | AT_MODE_TEST,   AT_CLASS_ABORTABLE | AT_CLASS_GSM_BASIC},
#endif
#ifdef AT_PLUS_CGCMOD_ENABLED
    { AT_PLUS_CGCMOD,  "+CGCMOD",  AT_PLUS_CGCMOD_Handle,      AT_MODE_SET | AT_MODE_TEST,                  AT_CLASS_GSM_BASIC },
#endif
#ifdef AT_PLUS_CGDCONT_ENABLED
    { AT_PLUS_CGDCONT, "+CGDCONT", AT_PLUS_CGDCONT_Handle,     AT_MODE_SET  | AT_MODE_READ | AT_MODE_TEST,  AT_CLASS_GSM_BASIC },
#endif
#ifdef AT_PLUS_CGEQMIN_ENABLED
    { AT_PLUS_CGEQMIN, "+CGEQMIN", AT_PLUS_CGEQMIN_Handle,     AT_MODE_SET | AT_MODE_READ | AT_MODE_TEST,   AT_CLASS_GSM_BASIC },
#endif
#ifdef AT_PLUS_CGEQNEG_ENABLED
    { AT_PLUS_CGEQNEG, "+CGEQNEG", AT_PLUS_CGEQNEG_Handle,     AT_MODE_SET | AT_MODE_TEST,   AT_CLASS_GSM_BASIC },
#endif
#ifdef AT_PLUS_CGEQREQ_ENABLED
    { AT_PLUS_CGEQREQ, "+CGEQREQ", AT_PLUS_CGEQREQ_Handle,     AT_MODE_SET | AT_MODE_READ | AT_MODE_TEST,   AT_CLASS_GSM_BASIC },
#endif
#ifdef AT_PLUS_CGQMIN_ENABLED
    { AT_PLUS_CGQMIN, "+CGQMIN", AT_PLUS_CGQMIN_Handle,     AT_MODE_SET | AT_MODE_READ | AT_MODE_TEST,   AT_CLASS_GSM_BASIC },
#endif
#ifdef AT_PLUS_CGQREQ_ENABLED
    { AT_PLUS_CGQREQ, "+CGQREQ", AT_PLUS_CGQREQ_Handle,     AT_MODE_SET | AT_MODE_READ | AT_MODE_TEST,   AT_CLASS_GSM_BASIC },
#endif
#ifdef AT_PLUS_CGEREP_ENABLED
    { AT_PLUS_CGEREP,  "+CGEREP",  AT_PLUS_CGEREP_Handle,      AT_MODE_SET | AT_MODE_READ | AT_MODE_TEST,   AT_CLASS_GSM_BASIC },
#endif
#ifdef AT_PLUS_CGMI_ENABLED
    { AT_PLUS_CGMI,    "+CGMI",    AT_PLUS_CGMI_Handle,        AT_MODE_DO  | AT_MODE_TEST,                  AT_CLASS_GSM_BASIC },
#endif
#ifdef AT_PLUS_CGMM_ENABLED
    { AT_PLUS_CGMM,    "+CGMM",    AT_PLUS_CGMM_Handle,        AT_MODE_DO  | AT_MODE_TEST,                  AT_CLASS_GSM_BASIC },
#endif
#ifdef AT_PLUS_CGMR_ENABLED
    { AT_PLUS_CGMR,    "+CGMR",    AT_PLUS_CGMR_Handle,        AT_MODE_DO  | AT_MODE_TEST,                  AT_CLASS_GSM_BASIC },
#endif
#ifdef AT_PLUS_CGPADDR_ENABLED
    { AT_PLUS_CGPADDR, "+CGPADDR", AT_PLUS_CGPADDR_Handle,     AT_MODE_SET | AT_MODE_TEST,                  AT_CLASS_GSM_BASIC },
#endif
#ifdef AT_PLUS_CGREG_ENABLED
    { AT_PLUS_CGREG,   "+CGREG",   AT_PLUS_CGREG_Handle,       AT_MODE_SET | AT_MODE_READ | AT_MODE_TEST,   AT_CLASS_GSM_BASIC },
#endif
#ifdef AT_PLUS_CGSMS_ENABLED
    { AT_PLUS_CGSMS,   "+CGSMS",   AT_PLUS_CGSMS_Handle,       AT_MODE_SET | AT_MODE_READ | AT_MODE_TEST,   AT_CLASS_GSM_BASIC },
#endif
#ifdef AT_PLUS_CGSN_ENABLED
    { AT_PLUS_CGSN,    "+CGSN",    AT_PLUS_CGSN_Handle,        AT_MODE_DO  | AT_MODE_TEST,                  AT_CLASS_GSM_BASIC },
#endif
#ifdef AT_PLUS_CHLD_ENABLED
    { AT_PLUS_CHLD,    "+CHLD",    AT_PLUS_CHLD_Handle,        AT_MODE_SET | AT_MODE_TEST,                  AT_CLASS_GSM_BASIC },
#endif
#ifdef AT_PLUS_CHUP_ENABLED
    { AT_PLUS_CHUP,    "+CHUP",    AT_H_Handle,                AT_MODE_DO  | AT_MODE_TEST,                  AT_CLASS_GSM_BASIC },
#endif
#ifdef AT_PLUS_CIMI_ENABLED
    { AT_PLUS_CIMI,    "+CIMI",    AT_PLUS_CIMI_Handle,        AT_MODE_DO  | AT_MODE_TEST,                  AT_CLASS_GSM_BASIC },
#endif
#ifdef AT_PLUS_CIND_ENABLED
    { AT_PLUS_CIND,    "+CIND",    AT_PLUS_CIND_Handle,        AT_MODE_READ | AT_MODE_TEST,                 AT_CLASS_GSM_BASIC },
#endif
#ifdef AT_PLUS_CKPD_ENABLED
    { AT_PLUS_CKPD,    "+CKPD",    AT_PLUS_CKPD_Handle,        AT_MODE_SET | AT_MODE_TEST,                  AT_CLASS_GSM_BASIC },
#endif
#ifdef AT_PLUS_CLAC_ENABLED
    { AT_PLUS_CLAC,    "+CLAC",    AT_PLUS_CLAC_Handle,        AT_MODE_DO  | AT_MODE_TEST,                  AT_CLASS_GSM_BASIC },
#endif
#ifdef AT_PLUS_CLCC_ENABLED
    { AT_PLUS_CLCC,    "+CLCC",    AT_PLUS_CLCC_Handle,        AT_MODE_DO  | AT_MODE_TEST,                  AT_CLASS_GSM_BASIC },
#endif
#ifdef AT_PLUS_CLCK_ENABLED
    { AT_PLUS_CLCK,    "+CLCK",    AT_PLUS_CLCK_Handle,        AT_MODE_SET | AT_MODE_TEST,                  AT_CLASS_ABORTABLE | AT_CLASS_GSM_BASIC },
#endif
#ifdef AT_PLUS_CLIP_ENABLED
    { AT_PLUS_CLIP,    "+CLIP",    AT_PLUS_CLIP_Handle,        AT_MODE_SET | AT_MODE_READ | AT_MODE_TEST,   AT_CLASS_GSM_BASIC },
#endif
#ifdef AT_PLUS_CLIR_ENABLED
    { AT_PLUS_CLIR,    "+CLIR",    AT_PLUS_CLIR_Handle,        AT_MODE_SET | AT_MODE_READ | AT_MODE_TEST,   AT_CLASS_GSM_BASIC },
#endif
#ifdef AT_PLUS_CLVL_ENABLED
    { AT_PLUS_CLVL,    "+CLVL",    AT_PLUS_CLVL_Handle,        AT_MODE_SET | AT_MODE_READ | AT_MODE_TEST,   AT_CLASS_GSM_BASIC },
#endif
#ifdef AT_PLUS_CMEC_ENABLED
    { AT_PLUS_CMEC,    "+CMEC",    AT_PLUS_CMEC_Handle,        AT_MODE_SET | AT_MODE_READ | AT_MODE_TEST,   AT_CLASS_GSM_BASIC },
#endif
#ifdef AT_PLUS_CMEE_ENABLED
    { AT_PLUS_CMEE,    "+CMEE",    AT_PLUS_CMEE_Handle,        AT_MODE_SET | AT_MODE_READ | AT_MODE_TEST,   AT_CLASS_GSM_BASIC },
#endif
#ifdef AT_PLUS_CMER_ENABLED
    { AT_PLUS_CMER,    "+CMER",    AT_PLUS_CMER_Handle,        AT_MODE_SET | AT_MODE_READ | AT_MODE_TEST,   AT_CLASS_GSM_BASIC },
#endif
#ifdef AT_PLUS_CMGC_ENABLED
    { AT_PLUS_CMGC,    "+CMGC",    AT_PLUS_CMGC_Handle,        AT_MODE_SET | AT_MODE_TEST,                  AT_CLASS_ABORTABLE | AT_CLASS_GSM_BASIC },
#endif
#ifdef AT_PLUS_CMGD_ENABLED
    { AT_PLUS_CMGD,    "+CMGD",    AT_PLUS_CMGD_Handle,        AT_MODE_SET | AT_MODE_TEST,                  AT_CLASS_GSM_BASIC },
#endif
#ifdef AT_PLUS_CMGF_ENABLED
    { AT_PLUS_CMGF,    "+CMGF",    AT_PLUS_CMGF_Handle,        AT_MODE_SET | AT_MODE_READ | AT_MODE_TEST,   AT_CLASS_GSM_BASIC },
#endif
#ifdef AT_PLUS_CMGL_ENABLED
    { AT_PLUS_CMGL,    "+CMGL",    AT_PLUS_CMGL_Handle,        AT_MODE_DO  | AT_MODE_SET | AT_MODE_TEST,    AT_CLASS_GSM_BASIC },
#endif
#ifdef AT_PLUS_CMGR_ENABLED
    { AT_PLUS_CMGR,    "+CMGR",    AT_PLUS_CMGR_Handle,        AT_MODE_SET | AT_MODE_TEST,                  AT_CLASS_GSM_BASIC },
#endif
#ifdef AT_PLUS_CMGS_ENABLED
    { AT_PLUS_CMGS,    "+CMGS",    AT_PLUS_CMGS_Handle,        AT_MODE_SET | AT_MODE_TEST,                  AT_CLASS_GSM_BASIC },
#endif
#ifdef AT_PLUS_CMGW_ENABLED
    { AT_PLUS_CMGW,    "+CMGW",    AT_PLUS_CMGW_Handle,        AT_MODE_SET | AT_MODE_TEST,                  AT_CLASS_GSM_BASIC },
#endif
#ifdef AT_PLUS_CMMS_ENABLED
    { AT_PLUS_CMMS,    "+CMMS",    AT_PLUS_CMMS_Handle,        AT_MODE_SET | AT_MODE_READ | AT_MODE_TEST,   AT_CLASS_GSM_BASIC },
#endif
#ifdef AT_PLUS_CMOD_ENABLED
    { AT_PLUS_CMOD,    "+CMOD",    AT_PLUS_CMOD_Handle,        AT_MODE_SET | AT_MODE_READ | AT_MODE_TEST,   AT_CLASS_GSM_BASIC },
#endif
#ifdef AT_PLUS_CMSS_ENABLED
    { AT_PLUS_CMSS,    "+CMSS",    AT_PLUS_CMSS_Handle,        AT_MODE_SET | AT_MODE_TEST,                  AT_CLASS_ABORTABLE | AT_CLASS_GSM_BASIC },
#endif
#ifdef AT_PLUS_CMUT_ENABLED
    { AT_PLUS_CMUT,    "+CMUT",    AT_PLUS_CMUT_Handle,        AT_MODE_SET | AT_MODE_READ | AT_MODE_TEST,   AT_CLASS_GSM_BASIC },
#endif
#ifdef AT_PLUS_CNAP_ENABLED
    { AT_PLUS_CNAP,    "+CNAP",    AT_PLUS_CNAP_Handle,        AT_MODE_SET | AT_MODE_READ | AT_MODE_TEST,   AT_CLASS_GSM_BASIC },
#endif
#ifdef AT_PLUS_CNMA_ENABLED
    { AT_PLUS_CNMA,    "+CNMA",    AT_PLUS_CNMA_Handle,        AT_MODE_SET | AT_MODE_TEST,                  AT_CLASS_GSM_SMS },
#endif
#ifdef AT_PLUS_CNMI_ENABLED
    { AT_PLUS_CNMI,    "+CNMI",    AT_PLUS_CNMI_Handle,        AT_MODE_SET | AT_MODE_READ | AT_MODE_TEST,   AT_CLASS_GSM_SMS },
#endif
#ifdef AT_PLUS_CNUM_ENABLED
    { AT_PLUS_CNUM,    "+CNUM",    AT_PLUS_CNUM_Handle,        AT_MODE_DO  | AT_MODE_TEST,                  AT_CLASS_GSM_BASIC },
#endif
#ifdef AT_PLUS_COLP_ENABLED
    { AT_PLUS_COLP,    "+COLP",    AT_PLUS_COLP_Handle,        AT_MODE_SET | AT_MODE_TEST | AT_MODE_READ,   AT_CLASS_GSM_BASIC },
#endif
#ifdef AT_PLUS_COLR_ENABLED
    { AT_PLUS_COLR,    "+COLR",    AT_PLUS_COLR_Handle,        AT_MODE_DO | AT_MODE_TEST,                  AT_CLASS_GSM_BASIC },
#endif
#ifdef AT_PLUS_COPN_ENABLED
    { AT_PLUS_COPN,    "+COPN",    AT_PLUS_COPN_Handle,        AT_MODE_DO  | AT_MODE_TEST,                  AT_CLASS_GSM_BASIC },
#endif
#ifdef AT_PLUS_COPS_ENABLED
    { AT_PLUS_COPS,    "+COPS",    AT_PLUS_COPS_Handle,        AT_MODE_SET | AT_MODE_READ | AT_MODE_TEST,   AT_CLASS_ABORTABLE | AT_CLASS_GSM_BASIC },
#endif
#ifdef AT_PLUS_CPAS_ENABLED
    { AT_PLUS_CPAS,    "+CPAS",    AT_PLUS_CPAS_Handle,        AT_MODE_DO  | AT_MODE_TEST,                  AT_CLASS_GSM_BASIC },
#endif
#ifdef AT_PLUS_CPBF_ENABLED
    { AT_PLUS_CPBF,    "+CPBF",    AT_PLUS_CPBF_Handle,        AT_MODE_SET | AT_MODE_TEST,                  AT_CLASS_GSM_BASIC },
#endif
#ifdef AT_PLUS_CPBR_ENABLED
    { AT_PLUS_CPBR,    "+CPBR",    AT_PLUS_CPBR_Handle,        AT_MODE_SET | AT_MODE_TEST,                  AT_CLASS_GSM_BASIC },
#endif
#ifdef AT_PLUS_CPBW_ENABLED
    { AT_PLUS_CPBW,    "+CPBW",    AT_PLUS_CPBW_Handle,        AT_MODE_SET | AT_MODE_TEST,                  AT_CLASS_GSM_BASIC },
#endif
#ifdef AT_PLUS_CPIN_ENABLED
    { AT_PLUS_CPIN,    "+CPIN",    AT_PLUS_CPIN_Handle,        AT_MODE_SET | AT_MODE_READ | AT_MODE_TEST,   AT_CLASS_GSM_BASIC },
#ifdef AT_STAR_ECEXPIN_ENABLED
    { AT_STAR_ECEXPIN, "*ECEXPIN", AT_PLUS_CPIN_Handle,        AT_MODE_SET | AT_MODE_TEST,                  AT_CLASS_ERICSSON },
#endif
#endif
#ifdef AT_PLUS_CPOL_ENABLED
    { AT_PLUS_CPOL,    "+CPOL",    AT_PLUS_CPOL_Handle,        AT_MODE_SET | AT_MODE_READ | AT_MODE_TEST,   AT_CLASS_GSM_BASIC },
#endif
#ifdef AT_STAR_EFSIM_ENABLED
    { AT_STAR_EFSIM,   "*EFSIM",   AT_STAR_EFSIM_Handle,       AT_MODE_READ | AT_MODE_TEST, AT_CLASS_ERICSSON },
#endif
#ifdef AT_PLUS_CPMS_ENABLED
    { AT_PLUS_CPMS,    "+CPMS",    AT_PLUS_CPMS_Handle,        AT_MODE_SET | AT_MODE_READ | AT_MODE_TEST,   AT_CLASS_GSM_BASIC },
#endif
#ifdef AT_PLUS_CPOL_ENABLED
    { AT_PLUS_CPOL,    "+CPOL",    AT_PLUS_CPOL_Handle,        AT_MODE_SET | AT_MODE_READ | AT_MODE_TEST,   AT_CLASS_GSM_BASIC },
#endif
#ifdef AT_PLUS_CPUC_ENABLED
    { AT_PLUS_CPUC,    "+CPUC",    AT_PLUS_CPUC_Handle,        AT_MODE_SET | AT_MODE_READ | AT_MODE_TEST,   AT_CLASS_GSM_BASIC },
#endif
#ifdef AT_PLUS_CPWD_ENABLED
    { AT_PLUS_CPWD,    "+CPWD",    AT_PLUS_CPWD_Handle,        AT_MODE_SET | AT_MODE_TEST,                  AT_CLASS_GSM_BASIC },
#endif
#ifdef AT_PLUS_CR_ENABLED
    { AT_PLUS_CR,      "+CR",      AT_PLUS_CR_Handle,          AT_MODE_SET | AT_MODE_READ | AT_MODE_TEST,   AT_CLASS_GSM_BASIC },
#endif
#ifdef AT_PLUS_CRC_ENABLED
    { AT_PLUS_CRC,     "+CRC",     AT_PLUS_CRC_Handle,         AT_MODE_SET | AT_MODE_READ | AT_MODE_TEST,   AT_CLASS_GSM_BASIC },
#endif
#ifdef AT_PLUS_CREG_ENABLED
    { AT_PLUS_CREG,    "+CREG",    AT_PLUS_CREG_Handle,        AT_MODE_SET | AT_MODE_READ | AT_MODE_TEST,   AT_CLASS_GSM_BASIC },
#endif
#ifdef AT_PLUS_CRES_ENABLED
    { AT_PLUS_CRES,    "+CRES",    AT_PLUS_CRES_Handle,        AT_MODE_DO  | AT_MODE_SET | AT_MODE_TEST,    AT_CLASS_GSM_BASIC },
#endif
#ifdef AT_PLUS_CCHC_ENABLED
    { AT_PLUS_CCHC,    "+CCHC",    AT_PLUS_CCHC_Handle,        AT_MODE_SET | AT_MODE_TEST,    AT_CLASS_GSM_BASIC },
#endif
#ifdef AT_PLUS_CCHO_ENABLED
    { AT_PLUS_CCHO,    "+CCHO",    AT_PLUS_CCHO_Handle,        AT_MODE_SET | AT_MODE_TEST,    AT_CLASS_GSM_BASIC },
#endif
#ifdef AT_PLUS_CGLA_ENABLED
    { AT_PLUS_CGLA,    "+CGLA",    AT_PLUS_CGLA_Handle,        AT_MODE_SET | AT_MODE_TEST,    AT_CLASS_GSM_BASIC },
#endif
#ifdef AT_PLUS_CRSM_ENABLED
    { AT_PLUS_CRSM,    "+CRSM",    AT_PLUS_CRSM_Handle,        AT_MODE_DO  | AT_MODE_SET | AT_MODE_TEST,    AT_CLASS_GSM_BASIC },
#endif
#ifdef AT_PLUS_CSAS_ENABLED
    { AT_PLUS_CSAS,    "+CSAS",    AT_PLUS_CSAS_Handle,        AT_MODE_DO  | AT_MODE_SET | AT_MODE_TEST,    AT_CLASS_GSM_BASIC },
#endif
#ifdef AT_PLUS_CSCA_ENABLED
    { AT_PLUS_CSCA,    "+CSCA",    AT_PLUS_CSCA_Handle,        AT_MODE_SET | AT_MODE_READ | AT_MODE_TEST,    AT_CLASS_GSM_BASIC },
#endif
#ifdef AT_PLUS_CSCB_ENABLED
    { AT_PLUS_CSCB,    "+CSCB",    AT_PLUS_CSCB_Handle,        AT_MODE_SET | AT_MODE_READ | AT_MODE_TEST,   AT_CLASS_GSM_BASIC },
#endif
#ifdef AT_PLUS_CSCC_ENABLED
    { AT_PLUS_CSCC,    "+CSCC",    AT_PLUS_CSCC_Handle,        AT_MODE_SET | AT_MODE_READ | AT_MODE_TEST,   AT_CLASS_GSM_BASIC },
#endif
#ifdef AT_PLUS_CSCS_ENABLED
    { AT_PLUS_CSCS,    "+CSCS",    AT_PLUS_CSCS_Handle,        AT_MODE_SET | AT_MODE_READ | AT_MODE_TEST,   AT_CLASS_GSM_BASIC },
#endif
#ifdef AT_PLUS_CSDF_ENABLED
    { AT_PLUS_CSDF,    "+CSDF",    AT_PLUS_CSDF_Handle,        AT_MODE_SET | AT_MODE_READ | AT_MODE_TEST,   AT_CLASS_GSM_BASIC },
#endif
#ifdef AT_PLUS_CSIL_ENABLED
    { AT_PLUS_CSIL,    "+CSIL",    AT_CommandHandlerFunction,  AT_MODE_SET | AT_MODE_READ | AT_MODE_TEST,   AT_CLASS_GSM_BASIC },
#endif
#ifdef AT_PLUS_CSIM_ENABLED
    { AT_PLUS_CSIM,    "+CSIM",    AT_PLUS_CSIM_Handle,        AT_MODE_SET | AT_MODE_TEST,                  AT_CLASS_GSM_BASIC },
#endif
#ifdef AT_PLUS_CSMS_ENABLED
    { AT_PLUS_CSMS,    "+CSMS",    AT_PLUS_CSMS_Handle,        AT_MODE_SET | AT_MODE_READ | AT_MODE_TEST,   AT_CLASS_GSM_BASIC },
#endif
#ifdef AT_PLUS_CSQ_ENABLED
    { AT_PLUS_CSQ,     "+CSQ",     AT_PLUS_CSQ_Handle,         AT_MODE_DO  | AT_MODE_TEST,                  AT_CLASS_GSM_BASIC },
#endif
#ifdef AT_PLUS_CSSN_ENABLED
    { AT_PLUS_CSSN,    "+CSSN",    AT_PLUS_CSSN_Handle,        AT_MODE_SET | AT_MODE_READ | AT_MODE_TEST,   AT_CLASS_GSM_BASIC },
#endif
#ifdef AT_PLUS_CSVM_ENABLED
    { AT_PLUS_CSVM,    "+CSVM",    AT_PLUS_CSVM_Handle,        AT_MODE_SET | AT_MODE_READ | AT_MODE_TEST,   AT_CLASS_GSM_BASIC },
#endif
#ifdef AT_PLUS_CTFR_ENABLED
    { AT_PLUS_CTFR,    "+CTFR",    AT_PLUS_CTFR_Handle,        AT_MODE_SET | AT_MODE_TEST,                  AT_CLASS_GSM_BASIC },
#endif
#ifdef AT_PLUS_CUAD_ENABLED
    { AT_PLUS_CUAD,    "+CUAD",    AT_PLUS_CUAD_Handle,        AT_MODE_DO  | AT_MODE_TEST,                  AT_CLASS_GSM_BASIC },
#endif
#ifdef AT_PLUS_CUSD_ENABLED
    { AT_PLUS_CUSD,    "+CUSD",    AT_PLUS_CUSD_Handle,        AT_MODE_SET | AT_MODE_READ | AT_MODE_TEST,   AT_CLASS_ABORTABLE | AT_CLASS_GSM_SS },
#endif
#ifdef AT_PLUS_CUUS1_ENABLED
    { AT_PLUS_CUUS1,   "+CUUS1",   AT_PLUS_CUUS1_Handle,       AT_MODE_SET | AT_MODE_READ | AT_MODE_TEST,   AT_CLASS_ABORTABLE | AT_CLASS_GSM_SS },
#endif
#ifdef AT_PLUS_DS_ENABLED
    { AT_PLUS_DS,      "+DS",      AT_PLUS_DS_Handle,          AT_MODE_SET,                                 AT_CLASS_BASIC },
#endif
#ifdef AT_STAR_EEMPAGE_ENABLED
    { AT_STAR_EEMPAGE,   "*EEMPAGE", AT_STAR_EEMPAGE_Handle,      AT_MODE_SET | AT_MODE_TEST,               AT_CLASS_ERICSSON },
#endif
#ifdef AT_STAR_ESMSFULL_ENABLED
    { AT_STAR_ESMSFULL, "*ESMSFULL", AT_STAR_ESMSFULL_Handle,  AT_MODE_SET | AT_MODE_TEST,                  AT_CLASS_ERICSSON },
#endif
#ifdef AT_PLUS_GCAP_ENABLED
    { AT_PLUS_GCAP,    "+GCAP",    AT_PLUS_GCAP_Handle,        AT_MODE_DO  | AT_MODE_TEST,                  AT_CLASS_BASIC },
#endif
#ifdef AT_PLUS_GMM_ENABLED
    { AT_PLUS_GMM,    "+GMM",      AT_PLUS_CGMM_Handle,        AT_MODE_DO  | AT_MODE_TEST,                  AT_CLASS_BASIC },
#endif
#ifdef AT_PLUS_GMR_ENABLED
    { AT_PLUS_GMR,    "+GMR",      AT_PLUS_CGMR_Handle,        AT_MODE_DO  | AT_MODE_TEST,                  AT_CLASS_BASIC },
#endif
#ifdef AT_PLUS_IFC_ENABLED
    { AT_PLUS_IFC,    "+IFC",      AT_PLUS_IFC_Handle,         AT_MODE_SET,                                 AT_CLASS_BASIC },
#endif
#ifdef AT_PLUS_NREC_ENABLED
    { AT_PLUS_NREC,    "+NREC",    AT_PLUS_NREC_Handle,        AT_MODE_SET | AT_MODE_READ | AT_MODE_TEST,   AT_CLASS_GSM_BASIC },
#endif
#ifdef AT_PLUS_VGM_ENABLED
    { AT_PLUS_VGM,     "+VGM",     AT_PLUS_VGM_Handle,         AT_MODE_SET | AT_MODE_READ | AT_MODE_TEST,   AT_CLASS_GSM_BASIC },
#endif
#ifdef AT_PLUS_VGS_ENABLED
    { AT_PLUS_VGS,     "+VGS",     AT_PLUS_VGS_Handle,         AT_MODE_SET | AT_MODE_READ | AT_MODE_TEST,   AT_CLASS_GSM_BASIC },
#endif
#ifdef AT_PLUS_VTD_ENABLED
    { AT_PLUS_VTD,     "+VTD",     AT_PLUS_VTD_Handle,         AT_MODE_SET | AT_MODE_READ | AT_MODE_TEST,   AT_CLASS_GSM_BASIC },
#endif
#ifdef AT_PLUS_VTS_ENABLED
    { AT_PLUS_VTS,     "+VTS",     AT_PLUS_VTS_Handle,         AT_MODE_SET | AT_MODE_TEST,                  AT_CLASS_GSM_BASIC },
#endif
#ifdef AT_PLUS_WS46_ENABLED
    { AT_PLUS_WS46,    "+WS46",    AT_PLUS_WS46_Handle,        AT_MODE_SET | AT_MODE_READ | AT_MODE_TEST,   AT_CLASS_GSM_BASIC },
#endif
#ifdef AT_A_ENABLED
    { AT_A,            "A",        AT_A_Handle,                AT_MODE_DO,                                  AT_CLASS_BASIC },
#endif
#ifdef AT_D_ENABLED
    { AT_D,            "D",        AT_D_Handle,                AT_MODE_DO  | AT_MODE_TEST,                  AT_CLASS_BASIC },
#endif
#ifdef AT_E_ENABLED
    { AT_E,            "E",        AT_E_Handle,                AT_MODE_DO  | AT_MODE_SET  | AT_MODE_READ | AT_MODE_TEST,   AT_CLASS_BASIC },
#endif
#ifdef AT_H_ENABLED
    { AT_H,            "H",        AT_H_Handle,                AT_MODE_DO  | AT_MODE_SET  | AT_MODE_TEST,   AT_CLASS_BASIC },
#endif
#ifdef AT_L_ENABLED
    { AT_L,            "L",        AT_L_Handle,                AT_MODE_DO  | AT_MODE_READ | AT_MODE_TEST,   AT_CLASS_BASIC },
#endif
#ifdef AT_Q_ENABLED
    { AT_Q,            "Q",        AT_Q_Handle,                AT_MODE_DO | AT_MODE_SET  | AT_MODE_READ | AT_MODE_TEST,   AT_CLASS_BASIC },
#endif
#ifdef AT_S0_ENABLED
    { AT_S0,           "S0",       AT_S0_Handle,               AT_MODE_SET | AT_MODE_READ | AT_MODE_TEST,   AT_CLASS_BASIC },
#endif
#ifdef AT_S10_ENABLED
    { AT_S10,          "S10",      AT_S10_Handle,              AT_MODE_SET | AT_MODE_READ | AT_MODE_TEST,   AT_CLASS_BASIC },
#endif
#ifdef AT_S3_ENABLED
    { AT_S3,           "S3",       AT_S3_Handle,               AT_MODE_SET | AT_MODE_READ | AT_MODE_TEST,   AT_CLASS_BASIC },
#endif
#ifdef AT_S4_ENABLED
    { AT_S4,           "S4",       AT_S4_Handle,               AT_MODE_SET | AT_MODE_READ | AT_MODE_TEST,   AT_CLASS_BASIC },
#endif
#ifdef AT_S5_ENABLED
    { AT_S5,           "S5",       AT_S5_Handle,               AT_MODE_SET | AT_MODE_READ | AT_MODE_TEST,   AT_CLASS_BASIC },
#endif
#ifdef AT_S6_ENABLED
    { AT_S6,           "S6",       AT_S6_Handle,               AT_MODE_SET | AT_MODE_READ | AT_MODE_TEST,   AT_CLASS_BASIC },
#endif
#ifdef AT_S7_ENABLED
    { AT_S7,           "S7",       AT_S7_Handle,               AT_MODE_SET | AT_MODE_READ | AT_MODE_TEST,   AT_CLASS_BASIC },
#endif
#ifdef AT_S8_ENABLED
    { AT_S8,           "S8",       AT_S8_Handle,               AT_MODE_SET | AT_MODE_READ | AT_MODE_TEST,   AT_CLASS_BASIC },
#endif
#ifdef AT_V_ENABLED
    { AT_V,            "V",        AT_V_Handle,                AT_MODE_SET | AT_MODE_DO  | AT_MODE_READ | AT_MODE_TEST,   AT_CLASS_BASIC },
#endif
#ifdef AT_X_ENABLED
    { AT_X,            "X",        AT_X_Handle,                AT_MODE_SET | AT_MODE_DO  | AT_MODE_READ | AT_MODE_TEST,   AT_CLASS_BASIC },
#endif
#ifdef AT_Z_ENABLED
    { AT_Z,            "Z",        AT_Z_Handle,                AT_MODE_SET | AT_MODE_DO  | AT_MODE_TEST,   AT_CLASS_BASIC },
#endif
#ifdef AT_PLUS_RADIOVER_ENABLED
    { AT_PLUS_RADIOVER, "+RADIOVER", AT_PLUS_RADIOVER_Handle,  AT_MODE_READ | AT_MODE_TEST,                AT_CLASS_ERICSSON },
#endif
    /* Include commands from extionsions */
#ifdef EXE_USE_ATC_CUSTOMER_EXTENSIONS
#include <atc_extended_command_table.h>
#endif
    { AT_THE_END,      "ZZZZ",     AT_CommandHandlerFunction,  AT_MODE_NONE,                                AT_CLASS_DEBUG }

};

/* The number of entries in the AT command table is constant. */
const AT_TableSize_t AT_COMMAND_TABLE_LENGTH = (sizeof(AT_CommandTable) / sizeof(AT_CommandTableRecord_s));

/*************************************************************************
* Declaration of functions
**************************************************************************/

