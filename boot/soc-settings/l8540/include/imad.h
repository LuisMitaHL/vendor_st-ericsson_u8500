/*
 * Copyright (C) ST-Ericsson SA 2011
 *
 * Author: Tristan BOYER <tristan.boyer at stericsson.com>
 *  for ST-Ericsson.
 *
 * Reproduction and Communication of this document is strictly prohibited
 * unless specifically authorized in writing by ST-Ericsson.
 */

#ifndef __IMAD_H__
#define __IMAD_H__

typedef enum
{
  IMAD_SEQUENCE_READ_AVS_FUSES = 0x0,
  IMAD_SEQUENCE_WRITE_AVS_IN_PRCMU_TDCM,
  IMAD_SEQUENCE_SET_APE_MIN_DDR_BW,
  IMAD_SEQUENCE_SET_DDR1_GFX_MIN_BW,
  IMAD_SEQUENCE_SHARED_DDR_L2CC_SCU_CONFIGS,
  IMAD_SEQUENCE_DDR0_INIT,
  IMAD_SEQUENCE_DDR1_INIT,
  IMAD_SEQUENCE_INTERLEAVING,
  IMAD_SEQUENCE_C2C_INIT,
  IMAD_SEQUENCE_WRITE_PASR_SEG,
  IMAD_SEQUENCE_WRITE_LVL0_FIREWALL
} t_imad_sequence;

typedef enum
{
  IMAD_TYPE_WRITE_EMBEDDED = 0x0,
  IMAD_TYPE_WRITE_BITMAP_EMBEDDED,
  IMAD_TYPE_WRITE_EXTERNAL,
  IMAD_TYPE_READ_TO_EXTERNAL,
  IMAD_TYPE_VERIFY_EMBEDDED,
  IMAD_TYPE_VERIFY_EXTERNAL
} t_imad_type;

#endif /* __IMAD_H__ */
