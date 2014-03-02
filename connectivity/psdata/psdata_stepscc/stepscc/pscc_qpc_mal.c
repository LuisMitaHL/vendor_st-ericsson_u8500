/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 * \brief  PS Connection Control parameter conversions for MAL
 */

/********************************************************************************
 * Include Files
 ********************************************************************************/
#include <assert.h>
#include <stdlib.h>
#include "pscc_bearer.h"
#include "pscc_qpc_mal.h"
#include "psccd.h"

/********************************************************************************
 *
 * Defines & Type definitions
 *
 ********************************************************************************/
/* add defines here */

/* Defines for the bitrate conversions */
#define cutoff_1_kbps ((uint8_t)0b01000000)
#define cutoff_8_kbps ((uint8_t)0b10000000)
#define cutoff_64_kbps ((uint8_t)0b11111110)
#define cutoff_1_Mbps ((uint8_t)0b01001010)
#define cutoff_2_Mbps ((uint8_t)0b10111010)
#define maxLimit ((uint8_t)0b11111010)
#define Mbps ((uint16_t)1024)

#define base_octet_offset_64 ((uint16_t)64)
#define base_octet_offset_576 ((uint16_t)576)
#define extended_octet_offset_8600 ((uint16_t)8600)
#define extended_octet_offset_16 ((uint16_t)16)
#define extended_octet_offset_128 ((uint16_t)128)
#define extended_octet_maxval ((uint16_t)256)

#define minimum_speed ((uint16_t)1)

/* Defines for transfer delay conversions */
#define cutoff_td_150 ((uint8_t)0b10000)
#define cutoff_td_950 ((uint8_t)0b100000)
#define cutoff_td_max ((uint8_t)0b111110)
#define offset_td_200 ((uint16_t)200)
#define offset_td_1000 ((uint16_t)1000)

/* Defines for SDU error conversions */
#define sdu_mpl_1502 ((uint8_t)0b10010111)
#define sdu_mpl_1510 ((uint8_t)0b10011000)
#define sdu_mpl_1520 ((uint8_t)0b10011001)

/********************************************************************************
 *
 * Private function prototypes
 *
 ********************************************************************************/

/*******************************************************************************
 *
 * Public Functions
 *
 *******************************************************************************/
mal_gpds_qos_mean_throughput_class_t pscc_mqpc_mean_throughput_class_to_mal(pscc_qos_mean_throughput_class_t mpl_mt_class)
{
  int conversion[] = { MAL_GPDS_QOS_MEAN_THROUGHPUT_CLASS_SUBSCRIBED,
                       MAL_GPDS_QOS_MEAN_THROUGHPUT_CLASS_100, MAL_GPDS_QOS_MEAN_THROUGHPUT_CLASS_200, MAL_GPDS_QOS_MEAN_THROUGHPUT_CLASS_500,
                       MAL_GPDS_QOS_MEAN_THROUGHPUT_CLASS_1000, MAL_GPDS_QOS_MEAN_THROUGHPUT_CLASS_2000, MAL_GPDS_QOS_MEAN_THROUGHPUT_CLASS_5000,
                       MAL_GPDS_QOS_MEAN_THROUGHPUT_CLASS_10000, MAL_GPDS_QOS_MEAN_THROUGHPUT_CLASS_20000, MAL_GPDS_QOS_MEAN_THROUGHPUT_CLASS_50000,
                       MAL_GPDS_QOS_MEAN_THROUGHPUT_CLASS_100000, MAL_GPDS_QOS_MEAN_THROUGHPUT_CLASS_200000, MAL_GPDS_QOS_MEAN_THROUGHPUT_CLASS_500000,
                       MAL_GPDS_QOS_MEAN_THROUGHPUT_CLASS_1000000, MAL_GPDS_QOS_MEAN_THROUGHPUT_CLASS_2000000, MAL_GPDS_QOS_MEAN_THROUGHPUT_CLASS_5000000,
                       MAL_GPDS_QOS_MEAN_THROUGHPUT_CLASS_10000000, MAL_GPDS_QOS_MEAN_THROUGHPUT_CLASS_20000000, MAL_GPDS_QOS_MEAN_THROUGHPUT_CLASS_50000000,
                       MAL_GPDS_QOS_MEAN_THROUGHPUT_CLASS_BEST_EFFORT };

  if (mpl_mt_class >= pscc_number_of_qos_MEAN_THROUGHPUT_CLASS) {
    PSCC_DBG_TRACE(LOG_ERR, "Value of pscc_qos_mean_throughput_class_t out of bounds. Value=%u\n", mpl_mt_class);
    return 0xFF;
  } else {
    return (mal_gpds_qos_mean_throughput_class_t) conversion[mpl_mt_class];
  }

}

mal_gpds_qos_residual_ber_t pscc_mqpc_res_ber_to_mal(pscc_qos_residual_ber_t mpl_res_ber)
{
  int conversion[] = { MAL_GPDS_QOS_RES_BER_SUBSCRIBED, MAL_GPDS_QOS_RES_BER_1, MAL_GPDS_QOS_RES_BER_2, MAL_GPDS_QOS_RES_BER_3,
      MAL_GPDS_QOS_RES_BER_4, MAL_GPDS_QOS_RES_BER_5, MAL_GPDS_QOS_RES_BER_6, MAL_GPDS_QOS_RES_BER_7, MAL_GPDS_QOS_RES_BER_8,
      MAL_GPDS_QOS_RES_BER_9, };

  if (mpl_res_ber >= pscc_number_of_qos_RESIDUAL_BER) {
    PSCC_DBG_TRACE(LOG_ERR, "Value of pscc_qos_residual_ber_t out of bounds. Value=%u\n", mpl_res_ber);
    return 0xFF;
  } else {
    return (mal_gpds_qos_residual_ber_t) conversion[mpl_res_ber];
  }

}

pscc_qos_residual_ber_t pscc_mqpc_res_ber_to_mpl(mal_gpds_qos_residual_ber_t mal_res_ber)
{
  int i = 0;
  for (i = 0; i < pscc_number_of_qos_RESIDUAL_BER; i++) {
    if (pscc_mqpc_res_ber_to_mal(i) == mal_res_ber) {
      return (pscc_qos_residual_ber_t) i;
    }
  }

  PSCC_DBG_TRACE(LOG_ERR, "Value of mal_gpds_qos_residual_ber_t out of bounds. Value=%u\n", mal_res_ber);
  return (pscc_qos_residual_ber_t) 0;
}

mal_gpds_qos_sdu_error_ratio_t pscc_mqpc_sdu_err_ratio_to_mal(pscc_qos_sdu_error_ratio_t mpl_sdu_err)
{
  int conversion[] = { MAL_GPDS_QOS_SDU_ERROR_RATIO_SUBSCRIBED, MAL_GPDS_QOS_SDU_ERROR_RATIO_1, MAL_GPDS_QOS_SDU_ERROR_RATIO_2,
      MAL_GPDS_QOS_SDU_ERROR_RATIO_3, MAL_GPDS_QOS_SDU_ERROR_RATIO_4, MAL_GPDS_QOS_SDU_ERROR_RATIO_5, MAL_GPDS_QOS_SDU_ERROR_RATIO_6,
      MAL_GPDS_QOS_SDU_ERROR_RATIO_7 };

  if (mpl_sdu_err >= pscc_number_of_qos_SDU_ERROR_RATIO) {
    PSCC_DBG_TRACE(LOG_ERR, "Value of pscc_qos_sdu_error_ratio_t out of bounds. Value=%u\n", mpl_sdu_err);
    return (mal_gpds_qos_sdu_error_ratio_t) 0xFF;
  }
  return (mal_gpds_qos_sdu_error_ratio_t) conversion[mpl_sdu_err];
}

pscc_qos_sdu_error_ratio_t pscc_mqpc_sdu_err_ratio_to_mpl(mal_gpds_qos_sdu_error_ratio_t mal_sdu_err)
{
  int i = 0;
  for (i = 0; i < pscc_number_of_qos_SDU_ERROR_RATIO; i++) {
    if (pscc_mqpc_sdu_err_ratio_to_mal(i) == mal_sdu_err) {
      return (pscc_qos_sdu_error_ratio_t) i;
    }
  }

  PSCC_DBG_TRACE(LOG_ERR, "Value of mal_gpds_qos_sdu_error_ratio_t out of bounds. Value=%u\n", mal_sdu_err);
  return (pscc_qos_sdu_error_ratio_t) 0;
}

uint16_t pscc_mqpc_bitrate_to_mal(uint8_t base, uint8_t extended)
{
  uint32_t returnvalue = 0;

  if (base < cutoff_1_kbps) {
    returnvalue = base;
  } else if (base < cutoff_8_kbps) {
    returnvalue = ((base - cutoff_1_kbps) * 8 + base_octet_offset_64);
  } else if (base <= cutoff_64_kbps) {
    returnvalue = ((base - cutoff_8_kbps) * 64 + base_octet_offset_576);

    if (base == cutoff_64_kbps) {
      /* handle extended values: */
      if (extended == 0) {
        /* extended byte = 0 means ignore it */
      } else if (extended <= cutoff_1_Mbps) {
        returnvalue = (extended_octet_offset_8600 + extended * 100); /* 100 is a constant from 3GPP spec 24.008 */
      } else if (extended <= cutoff_2_Mbps) {
        returnvalue = ((extended_octet_offset_16 + extended - cutoff_1_Mbps) * Mbps);
      } else if (extended <= maxLimit) {
        returnvalue = ((extended_octet_offset_128 + (extended - cutoff_2_Mbps) * 2) * Mbps);
      } else {
        returnvalue = (extended_octet_maxval * Mbps); /* 256 is a constant from 3GPP spec 24.008 (maximum value) */
      }
    }
  } else {
    returnvalue = minimum_speed; /* base byte 0xff means speed = 0, which is illegal for mal */
  }

  if (returnvalue >= UINT16_MAX) {
    return (uint16_t) UINT16_MAX;
  } else {
    return (uint16_t) returnvalue;
  }
}

void pscc_mqpc_bitrate_to_mpl(uint16_t rate, uint8_t* base, uint8_t* extended)
{

  if (rate >= 8700) { /* 8700 is a constant from 3GPP spec 24.008 (lower limit for first interval with extended octet)
   rate is beyond the base encoding. Must use extended octet */
    *base = cutoff_64_kbps;

    /*As long as the input value is a uint16, the maximum values will never be used as input values. Consider the removed code below as documentation only!*/
    if (rate > extended_octet_offset_16 * Mbps) {
      *extended = rate / Mbps - extended_octet_offset_16 + cutoff_1_Mbps; // 16 is a constant from 3GPP spec 24.008
    } else {
      *extended = (rate - extended_octet_offset_8600) / 100;
    }
  } else {
    *extended = 0;

    if (rate >= base_octet_offset_576) {
      *base = (rate - base_octet_offset_576) / 64 + cutoff_8_kbps;
    } else if (rate >= base_octet_offset_64) {
      *base = (rate - base_octet_offset_64) / 8 + cutoff_1_kbps;
    } else if (rate > 0) {
      *base = rate;
    } else { /* NB: A value of "0" means default, i.e. 0. There is no "0kbps", as minimum is 1kbps */
      *base = 0;
    }
  }
}

uint16_t pscc_mqpc_transfer_delay_to_mal(uint8_t binary)
{
  uint16_t returnvalue = 0;

  if (binary < cutoff_td_150) {
    returnvalue = binary * 10;
  } else if (binary < cutoff_td_950) {
    returnvalue = ((binary - cutoff_td_150) * 50 + offset_td_200);
  } else if (binary <= cutoff_td_max) {
    returnvalue = ((binary - cutoff_td_950) * 100 + offset_td_1000);
  } else {
    PSCC_DBG_TRACE(LOG_ERR, "Converted value is out of bounds. Base value=%u\n", binary);
  }

  return returnvalue;
}

uint8_t pscc_mqpc_transfer_delay_to_mpl(uint16_t delay)
{
  uint8_t returnvalue = 0;
  if (delay > 4000) {
    returnvalue = cutoff_td_max;
    PSCC_DBG_TRACE(LOG_ERR, "Converted value is out of bounds. Delay value=%u, max is 4000\n", delay);
  } else if (delay >= offset_td_1000) {
    returnvalue = (delay - offset_td_1000) / 100 + cutoff_td_950;
  } else if (delay >= offset_td_200) {
    returnvalue = (delay - offset_td_200) / 50 + cutoff_td_150;
  } else {
    returnvalue = delay / 10;
  }

  return returnvalue;
}

uint16_t pscc_mqpc_max_sdu_to_mal(uint8_t binary)
{
  uint16_t returnvalue = 0;

  if (binary < sdu_mpl_1502) {
    returnvalue = binary * 10;
  } else if (binary == sdu_mpl_1502) {
    returnvalue = 1502;
  } else if (binary == sdu_mpl_1510) {
    returnvalue = 1510;
  } else if (binary == sdu_mpl_1520) {
    returnvalue = 1520;
  } else {
    PSCC_DBG_TRACE(LOG_ERR, "pscc_mqpc_max_sdu_to_mal input value out of bounds. Value:%u is larger than the stipulated %u. \n", binary, sdu_mpl_1520);
    returnvalue = 1520;
  }

  return returnvalue;
}

uint8_t pscc_mqpc_max_sdu_to_mpl(uint16_t octets)
{
  uint8_t returnvalue = 0;
  if (octets == 1520) {
    returnvalue = sdu_mpl_1520;
  } else if (octets == 1510) {
    returnvalue = sdu_mpl_1510;
  } else if (octets == 1502) {
    returnvalue = sdu_mpl_1502;
  } else if (octets <= 1500) {
    returnvalue = octets / 10;
  } else {
    PSCC_DBG_TRACE(LOG_ERR, "pscc_mqpc_max_sdu_to_mpl input value out of bounds. Value:%u. \n", octets);
  }

  return returnvalue;
}


