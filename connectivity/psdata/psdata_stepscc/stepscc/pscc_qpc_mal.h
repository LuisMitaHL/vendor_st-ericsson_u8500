/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 * \brief  PS Connection Control parameter conversions for MAL
 *
 * Performs conversions between parameter formats according to TS 24.008 section 10.5.6.5
 */

#ifndef PSCC_QPC_MAL_H
#define PSCC_QPC_MAL_H

/********************************************************************************
 *
 * Include files
 *
 ********************************************************************************/
#include <stdint.h>
#include <stdbool.h>
#include "pscc_msg.h"
#include "mal_gpds.h"
#include "mpl_list.h"

/********************************************************************************
 *
 * Defines & Type definitions
 *
 ********************************************************************************/

/*******************************************************************************
 *
 * Functions and macros
 *
 *******************************************************************************/
/**
 * pscc_mqpc_mean_throughput_class_to_mal
 *
 * @mpl_mt_class - Value of mean throughput class coded by MPL
 *
 * Returns:
 * Value of mean throughput class coded in MAL
 */
mal_gpds_qos_mean_throughput_class_t pscc_mqpc_mean_throughput_class_to_mal(pscc_qos_mean_throughput_class_t mpl_mt_class);

/**
 * pscc_mqpc_res_ber_to_mal
 *
 * @mpl_res_ber - Value of residual BER coded by MPL
 *
 * Returns:
 * Value of residual BER coded in MAL
 */
mal_gpds_qos_residual_ber_t pscc_mqpc_res_ber_to_mal(pscc_qos_residual_ber_t mpl_res_ber);

/**
 * pscc_mqpc_res_ber_to_mpl
 *
 * @mal_res_ber - Value of residual BER coded by MAL
 *
 * Returns:
 * Value of residual BER coded in MPL
 */
pscc_qos_residual_ber_t pscc_mqpc_res_ber_to_mpl(mal_gpds_qos_residual_ber_t mal_res_ber);

/**
 * pscc_mqpc_sdu_err_ratio_to_mal
 *
 * @mpl_sdu_err - Value of SDU error coded by MPL
 *
 * Returns:
 * Value of SDU error coded in MAL
 */
mal_gpds_qos_sdu_error_ratio_t pscc_mqpc_sdu_err_ratio_to_mal(pscc_qos_sdu_error_ratio_t mpl_sdu_err);

/**
 * pscc_mqpc_sdu_err_ratio_to_mpl
 *
 * @mal_sdu_err - Value of SDU error coded by MAL
 *
 * Returns:
 * Value of SDU error coded in MPL
 */
pscc_qos_sdu_error_ratio_t pscc_mqpc_sdu_err_ratio_to_mpl(mal_gpds_qos_sdu_error_ratio_t mal_sdu_err);

/**
 * pscc_mqpc_bitrate_to_mal
 *
 * @base      Base bit rate (octet 7,8,12 or 13)
 * @extended  Extended bit rate (octet 15-18)
 *
 * Returns:
 * Value of bit rate coded in MAL
 */
uint16_t pscc_mqpc_bitrate_to_mal(uint8_t base, uint8_t extended);

/**
 * pscc_mqpc_bitrate_to_mpl
 *
 * @rate    Bitrate in kbps as returned by MAL
 * @base   [out] Base bit rate (octet 7,8,12 or 13)
 * @extended  [out] Extended bit rate (octet 15-18)
 */
void pscc_mqpc_bitrate_to_mpl(uint16_t rate, uint8_t* base, uint8_t* extended);

/**
 * pscc_mqpc_transfer_delay_to_mal
 *
 * @binary    Binary coded value of transfer delay
 *
 * Returns:
 * Value of transfer delay coded in MAL
 */
uint16_t pscc_mqpc_transfer_delay_to_mal(uint8_t binary);

/**
 * pscc_mqpc_transfer_delay_to_mpl
 *
 * @delay    Value of transfer delay (in milliseconds)
 *
 * Returns:
 * Value of transfer delay coded for mpl
 */
uint8_t pscc_mqpc_transfer_delay_to_mpl(uint16_t delay);

/**
 * pscc_mqpc_max_sdu_to_mal
 *
 * @binary  Value of SDU size, coded as in mpl (corresponding to the TS24.008 specification)
 *
 * Returns: value of SDU encoded in octets.
 */
uint16_t pscc_mqpc_max_sdu_to_mal(uint8_t binary);

/**
 * pscc_mqpc_max_sdu_to_mpl
 *
 * @octets Value of SDU size, coded in octets.
 *
 * Returns: Value of SDU size, coded as in mpl (corresponding to the TS24.008 specification)
 */
uint8_t pscc_mqpc_max_sdu_to_mpl(uint16_t octets);

#endif
