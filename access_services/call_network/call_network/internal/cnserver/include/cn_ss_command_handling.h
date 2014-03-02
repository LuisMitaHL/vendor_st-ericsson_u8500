/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef __cn_ss_command_handling_h__
#define __cn_ss_command_handling_h__ (1)

#include <stdio.h>
#include "cn_data_types.h"


/********************************************************************************
 *                               TYPE DEFINITIONS                               *
 ********************************************************************************/

typedef enum {
    CN_SS_PROCEDURE_TYPE_ACTIVATION,
    CN_SS_PROCEDURE_TYPE_DEACTIVATION,
    CN_SS_PROCEDURE_TYPE_INTERROGATION,
    CN_SS_PROCEDURE_TYPE_REGISTRATION,
    CN_SS_PROCEDURE_TYPE_ERASURE,
    CN_SS_PROCEDURE_TYPE_NR_OF_TYPES,
    CN_SS_PROCEDURE_TYPE_UNKNOWN
} cn_ss_procedure_type_t;

typedef struct {
    cn_ss_procedure_type_t type;
    char *pattern_p;
} cn_ss_pattern_entry_t;

typedef enum {
    CN_SI_TYPE_SIA,
    CN_SI_TYPE_SIB,
    CN_SI_TYPE_SIC,
    CN_SI_TYPE_UNKNOWN,
} cn_si_type_t;

typedef enum { /* SS service codes from 3GPP TS 22.030, Annex B (entries without codes omitted)  */
    CN_SS_TYPE_ENHANCED_MULTILEVEL_PRECEDENCE_AND_PREEMPTION  = 75,  /* eMLPP, 3GPP TS 22.067    */
    CN_SS_TYPE_ENHANCED_MULTILEVEL_PRECEDENCE_AND_PREEMPTION0 = 750, /* eMLPP, 3GPP TS 22.067    */
    CN_SS_TYPE_ENHANCED_MULTILEVEL_PRECEDENCE_AND_PREEMPTION1 = 751, /* eMLPP, 3GPP TS 22.067    */
    CN_SS_TYPE_ENHANCED_MULTILEVEL_PRECEDENCE_AND_PREEMPTION2 = 752, /* eMLPP, 3GPP TS 22.067    */
    CN_SS_TYPE_ENHANCED_MULTILEVEL_PRECEDENCE_AND_PREEMPTION3 = 753, /* eMLPP, 3GPP TS 22.067    */
    CN_SS_TYPE_ENHANCED_MULTILEVEL_PRECEDENCE_AND_PREEMPTION4 = 754, /* eMLPP, 3GPP TS 22.067    */
    CN_SS_TYPE_CALL_DEFLECTION                                = 66,  /* CD,    3GPP TS 22.072    */
    CN_SS_TYPE_CALLING_LINE_IDENTIFICATION_PRESENTATION       = 30,  /* CLIP,  3GPP TS 22.081    */
    CN_SS_TYPE_CALLING_LINE_IDENTIFICATION_RESTRICTION        = 31,  /* CLIR,  3GPP TS 22.081    */
    CN_SS_TYPE_CONNECTED_LINE_IDENTIFICATION_PRESENTATION     = 76,  /* COLP,  3GPP TS 22.081    */
    CN_SS_TYPE_CONNECTED_LINE_IDENTIFICATION_RESTRICTION      = 77,  /* COLR,  3GPP TS 22.081    */
    CN_SS_TYPE_CALL_FORWARDING_UNCONDITIONAL                  = 21,  /* CF,    3GPP TS 22.082    */
    CN_SS_TYPE_CALL_FORWARDING_BUSY                           = 67,  /* CF,    3GPP TS 22.082    */
    CN_SS_TYPE_CALL_FORWARDING_NO_REPLY                       = 61,  /* CF,    3GPP TS 22.082    */
    CN_SS_TYPE_CALL_FORWARDING_NOT_REACHABLE                  = 62,  /* CF,    3GPP TS 22.082    */
    CN_SS_TYPE_CALL_FORWARDING_ALL                            = 002, /* CF,    3GPP TS 22.082    */
    CN_SS_TYPE_CALL_FORWARDING_ALL_CONDITIONAL                = 004, /* CF,    3GPP TS 22.082    */
    CN_SS_TYPE_CALL_WAITING                                   = 43,  /* CW,    3GPP TS 22.083    */
    CN_SS_TYPE_USER_TO_USER_SIGNALLING_SERVICE_1              = 361, /* UUS,   3GPP TS 22.087    */
    CN_SS_TYPE_USER_TO_USER_SIGNALLING_SERVICE_2              = 362, /* UUS,   3GPP TS 22.087    */
    CN_SS_TYPE_USER_TO_USER_SIGNALLING_SERVICE_3              = 363, /* UUS,   3GPP TS 22.087    */
    CN_SS_TYPE_USER_TO_USER_SIGNALLING_ALL                    = 360, /* UUS,   3GPP TS 22.087    */
    CN_SS_TYPE_CALL_BARRING_BAOC                              = 33,  /*        3GPP TS 22.088    */
    CN_SS_TYPE_CALL_BARRING_BAOIC                             = 331, /*        3GPP TS 22.088    */
    CN_SS_TYPE_CALL_BARRING_BAOIC_EXCL_HOME                   = 332, /*        3GPP TS 22.088    */
    CN_SS_TYPE_CALL_BARRING_BAIC                              = 35,  /*        3GPP TS 22.088    */
    CN_SS_TYPE_CALL_BARRING_BAIC_ROAMING                      = 351, /*        3GPP TS 22.088    */
    CN_SS_TYPE_CALL_BARRING_ALL                               = 330, /*        3GPP TS 22.088    */
    CN_SS_TYPE_CALL_BARRING_ALL_OUTGOING                      = 333, /*        3GPP TS 22.088    */
    CN_SS_TYPE_CALL_BARRING_ALL_INCOMING                      = 353, /*        3GPP TS 22.088    */
    CN_SS_TYPE_EXPLICIT_CALL_TRANSFER                         = 96,  /* ECT,   3GPP TS 22.091    */
    CN_SS_TYPE_CALL_COMPLETION_TO_BUSY_SUBSCRIBER             = 37,  /* CCBS,  3GPP TS 22.093    */
    CN_SS_TYPE_FOLLOW_ME                                      = 214, /* FM,    3GPP TS 22.094    */
    CN_SS_TYPE_CALLING_NAME_PRESENTATION                      = 300, /* CNAP,  3GPP TS 22.096    */
    CN_SS_TYPE_MULTIPLE_SUBSCRIBER_PROFILE                    = 59,  /* MSP    3GPP TS 22.097    */
    CN_SS_TYPE_MULTIPLE_SUBSCRIBER_PROFILE1                   = 591, /* MSP    3GPP TS 22.097    */
    CN_SS_TYPE_MULTIPLE_SUBSCRIBER_PROFILE2                   = 592, /* MSP    3GPP TS 22.097    */
    CN_SS_TYPE_MULTIPLE_SUBSCRIBER_PROFILE3                   = 593, /* MSP    3GPP TS 22.097    */
    CN_SS_TYPE_MULTIPLE_SUBSCRIBER_PROFILE4                   = 594, /* MSP    3GPP TS 22.097    */
    CN_SS_TYPE_MULTICALL                                      = 88,  /* MC,    3GPP TS 22.135    */
    CN_SS_TYPE_UNKNOWN                                        = 999
} cn_ss_type_t;

#define SI_ENTRY_MAX_SIZE 100
typedef struct {
    cn_ss_procedure_type_t procedure_type;
    cn_ss_type_t           ss_type;
    char                   supplementary_info_a[SI_ENTRY_MAX_SIZE];
    char                   supplementary_info_b[SI_ENTRY_MAX_SIZE];
    char                   supplementary_info_c[SI_ENTRY_MAX_SIZE];
} cn_ss_string_compounds_t;



/********************************************************************************
 *                               FUNCTION PROTOTYPES                            *
 ********************************************************************************/

/**
 * \fn cn_ss_string_compounds_t *decode_ss_string(char *ss_string_p)
 *
 * \brief Decode SS string
 *
 * \n This function decodes a given SS string into a \ref cn_ss_string_compounds_t
 * \n structure containing the SS command's subcomponents. This way the information
 * \n is easily accessed for a SS handler function.
 * \n
 * \n See 3GPP TS 22.030 for more information about the MMI/SS strings.
 *
 * \param [in] ss_string_p     Pointer to NULL terminated SS string.
 *
 * \return                     Pointer to \ref cn_ss_string_compounds_t.
 *                             The caller need to free the heap buffer.
 */
cn_ss_string_compounds_t *decode_ss_string(char *ss_string_p);


/**
 * \fn char *encode_ss_string(cn_ss_string_compounds_t *command_p)
 *
 * \brief Encode SS string
 *
 * \n This function encodes a given \ref cn_ss_string_compounds_t structure
 * \n to a SS string. It is the opposite of the decoder.
 *
 * \n See 3GPP TS 22.030 for more information about the MMI/SS strings.
 *
 *
 * \param [in] ss_string_p     Pointer to a \ref cn_ss_string_compounds_t structure.
 *
 * \return                     Pointer to a NULL terminated SS string.
 *                             The caller need to free the heap buffer.
 */
char *encode_ss_string(cn_ss_string_compounds_t *command_p);



#endif /* __cn_ss_command_handling_h__ */
