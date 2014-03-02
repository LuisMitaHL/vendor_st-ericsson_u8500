/*
 * Copyright (C) ST-Ericsson SA 2011
 *
 * Author: Tristan BOYER <tristan.boyer at stericsson.com>
 *  for ST-Ericsson.
 *
 * Reproduction and Communication of this document is strictly prohibited
 * unless specifically authorized in writing by ST-Ericsson.
 */


#include <bass_app.h>
#include <prcmu.h>
#include <imad.h>
#include <nomadik_mapping.h>
#include <nomadik_registers.h>
#include <soc_settings.h>

#if 0
typedef struct
{
  t_imad_sequence     seq_id;
  t_imad_type         type;
  uint32_t *          address;
  uint32_t            length;
  uint8_t             data[];
} t_imad_element __attribute__ ((aligned (8)));

typedef struct
{
  uint32_t            version;
  t_imad_element      imad_element[];
} t_imad __attribute__ ((aligned (8)));
#endif

__attribute__((section(".type")))
const uint32_t type = SECTION_TYPE_IMAD;

extern uint32_t linker_payload_length;

__attribute__((section(".length")))
const uint32_t payload_length = (uint32_t)&linker_payload_length;

__attribute__((section(".payload_imad")))


const uint32_t imad[] = {
    0x44414D49, /* magic: "IMAD" */

#ifdef IMAD_CONFIG_READ_FUSES
    /* sequence READ_AVS_FUSES: read of AVS fuses */
    (uint32_t)IMAD_SEQUENCE_READ_AVS_FUSES,
    (uint32_t)IMAD_TYPE_READ_TO_EXTERNAL,
    (uint32_t)(&(((t_secure_registers *)SEC_REG_START_ADDR)->FuseValue[4])), /* @ of AVS fuses */
    (uint32_t)4*sizeof(uint32_t),
    /* No data */
    /* No padding */

    /* sequence READ_AVS_FUSES: read of AVS fuses */
    (uint32_t)IMAD_SEQUENCE_READ_AVS_FUSES,
    (uint32_t)IMAD_TYPE_READ_TO_EXTERNAL,
    (uint32_t)(&(((t_secure_registers *)SEC_REG_START_ADDR)->FuseValue[21])), /* @ of AVS fuses */
    (uint32_t)2*sizeof(uint32_t),
    /* No data */
    /* No padding */
#endif

    /* sequence WRITE_AVS_IN_PRCMU_TDCM: Write AVS data structure inside PRCMU TDCM */
    (uint32_t)IMAD_SEQUENCE_WRITE_AVS_IN_PRCMU_TDCM,
    (uint32_t)IMAD_TYPE_WRITE_EXTERNAL,
    (uint32_t)(&(((t_SecMap *)TCDMsec)->InitOppData)), /* PRCMU init OPP area */
    (uint32_t)sizeof(sPrcmuApi_InitOppData_t),
    /* No data */
    /* No padding */

    /* sequence SET_APE_MIN_DDR_BW: Set the APE minimum bandwidth for DDR controllersw*/
    (uint32_t)IMAD_SEQUENCE_SET_APE_MIN_DDR_BW,
    (uint32_t)IMAD_TYPE_WRITE_EXTERNAL,
    (uint32_t)(&(((t_prcmu_hw_registers *)PRCMU_REG_START_ADDR)->prcm_ddrsubsys_ape_minbw)), /* set minimum bandwidth requested for DDR0 by APE */
    (uint32_t)sizeof(uint32_t),
    /* No data */
    /* No padding */

    (uint32_t)IMAD_SEQUENCE_SET_APE_MIN_DDR_BW,
    (uint32_t)IMAD_TYPE_WRITE_EXTERNAL,
    (uint32_t)((((uint8_t*)(&(((t_prcmu_hw_registers *)PRCMU_REG_START_ADDR)->prcm_ddrsubsys_ape_minbw)))) + 0x2000), /* set minimum bandwidth requested for DDR1 by APE */
    (uint32_t)sizeof(uint32_t),
    /* No data */
    /* No padding */

    (uint32_t)IMAD_SEQUENCE_SET_DDR1_GFX_MIN_BW,
    (uint32_t)IMAD_TYPE_WRITE_EMBEDDED,
    (uint32_t)((uint32_t*)(PRCMU_REG_START_ADDR + 0x2450)), /* set minimum bandwidth requested for DDR1 by GFX */
    (uint32_t)sizeof(uint32_t),
    (uint32_t)0x2,
    /* No padding */

    /* sequence SHARED_DDR_L2CC_SCU_CONFIGS: init to be done before DDR inits */
    (uint32_t)IMAD_SEQUENCE_SHARED_DDR_L2CC_SCU_CONFIGS,
    (uint32_t)IMAD_TYPE_WRITE_EMBEDDED,
    (uint32_t)(&(((t_secure_registers *)SEC_REG_START_ADDR)->DDRShareHighMaxSpaceAddrCheck)), /* reset modem/APE shared DDR configuration */
    (uint32_t)sizeof(uint32_t),
    /* data */
    (uint32_t)0xFFFFFFFF,
    /* No padding */

    /* ARM recommends that you program the Address Filtering End Register before the Address
     * Filtering Start Register to avoid unpredictable behavior between the two writes. */
    (uint32_t)IMAD_SEQUENCE_SHARED_DDR_L2CC_SCU_CONFIGS,
    (uint32_t)IMAD_TYPE_WRITE_BITMAP_EMBEDDED,
    (uint32_t)(&(((t_l2cc_registers *)L2CC_REG_START_ADDR)->AddressFilterEnd)), /* set L2 cache filtering end address */
    (uint32_t)2*sizeof(uint32_t),
    /* data */
    (uint32_t)0xFFF00000, /* bitmask */
    (uint32_t)0xC0000000, /* end address bits[19-0] should be zero */
    /* No padding */

    (uint32_t)IMAD_SEQUENCE_SHARED_DDR_L2CC_SCU_CONFIGS,

    (uint32_t)IMAD_TYPE_WRITE_BITMAP_EMBEDDED,
    (uint32_t)(&(((t_l2cc_registers *)L2CC_REG_START_ADDR)->AddressFilterStart)), /* set L2 cache filtering start address */
    (uint32_t)2*sizeof(uint32_t),
    /* data */
    (uint32_t)0xFFF00001, /* bitmask */
    (uint32_t)0x40000001, /* start address bits[19-1] should be zero */

     /* sequence DDR0_INIT: Set DDR0 init data */
    (uint32_t)IMAD_SEQUENCE_DDR0_INIT,
    (uint32_t)IMAD_TYPE_WRITE_EXTERNAL,
    (uint32_t)(&(((t_SecMap *)TCDMsec)->DDRCtrl0Init)), /* DDR0 init data */
    (uint32_t)sizeof(t_DDRInit),
    /* No data */
    /* No padding */

    /* sequence DDR1_INIT: Set DDR1 init data */
    (uint32_t)IMAD_SEQUENCE_DDR1_INIT,
    (uint32_t)IMAD_TYPE_WRITE_EXTERNAL,
    (uint32_t)(&(((t_SecMap *)TCDMsec)->DDRCtrl1Init)), /* DDR1 init data */
    (uint32_t)sizeof(t_DDRInit),
    /* No data */
    /* No padding */

    /* sequence INTERLEAVING: Set interleaving config */
    /* TODO: to be adapted with real final value: if fata is no managed as embedded data
       disable/set/enable could not be done in the same sequence... */
    (uint32_t)IMAD_SEQUENCE_INTERLEAVING,
    (uint32_t)IMAD_TYPE_WRITE_BITMAP_EMBEDDED,
    (uint32_t)(&(((t_secure_registers *)SEC_REG_START_ADDR)->Intlv)),/* interleaving */
    (uint32_t)2*sizeof(uint32_t),
    /* data */
    (uint32_t)0x00000001, /* bitmask */
    (uint32_t)0x00000000, /* value */
    /* No padding */

    (uint32_t)IMAD_SEQUENCE_INTERLEAVING,
    (uint32_t)IMAD_TYPE_WRITE_EXTERNAL,
    (uint32_t)(&(((t_secure_registers *)SEC_REG_START_ADDR)->Intlv)), /* set interleaving config */
    (uint32_t)sizeof(uint32_t),
    /* data */
    /* No padding */

    (uint32_t)IMAD_SEQUENCE_INTERLEAVING,
    (uint32_t)IMAD_TYPE_WRITE_EXTERNAL,
    (uint32_t)(&(((t_secure_registers *)SEC_REG_START_ADDR)->Intlv)), /* Re-enable if needed */
    (uint32_t)sizeof(uint32_t),
    /* data */
    /* No padding */

    /* sequence PASR_AT_BOOT: Send PASR segment mask config at boot to PRMCU Fw */
    (uint32_t)IMAD_SEQUENCE_WRITE_PASR_SEG,
    (uint32_t)IMAD_TYPE_WRITE_EXTERNAL,
    (uint32_t)(&(((t_SecMap *)TCDMsec)->securePasrData)),
    (uint32_t)sizeof(sPrcmuApi_securePasrData_t),
    /* No data */
    /* No padding */

#ifdef IMAD_CONFIG_LVL0_FIREWALL
    /* sequence LEVEL0_FIREWALLING: Configure Level0 firewalling */
    (uint32_t)IMAD_SEQUENCE_WRITE_LVL0_FIREWALL,
    (uint32_t)IMAD_TYPE_WRITE_BITMAP_EMBEDDED,
    (uint32_t)(&(((t_secure_registers *)SEC_REG_START_ADDR)->ScramblerControl)),
    (uint32_t)2*sizeof(uint32_t),
    /* data */
    (uint32_t)0x00000010, /* bitmask */
    (uint32_t)0x00000010, /* value */
    /* No padding */

    /* sequence LEVEL0_FIREWALLING: Configure Level0 firewalling */
    (uint32_t)IMAD_SEQUENCE_WRITE_LVL0_FIREWALL,
    (uint32_t)IMAD_TYPE_WRITE_EXTERNAL,
    (uint32_t)(&(((t_secure_registers *)SEC_REG_START_ADDR)->DDRModemAddrCheck)),
    (uint32_t)sizeof(uint32_t),
    /* data */
    /* No padding */

    /* sequence LEVEL0_FIREWALLING: Configure Level0 firewalling */
    (uint32_t)IMAD_SEQUENCE_WRITE_LVL0_FIREWALL,
    (uint32_t)IMAD_TYPE_WRITE_EXTERNAL,
    (uint32_t)(&(((t_secure_registers *)SEC_REG_START_ADDR)->DDRA9OnlyAddrCheck)),
    (uint32_t)sizeof(uint32_t),
    /* data */
    /* No padding */

    /* sequence LEVEL0_FIREWALLING: Configure Level0 firewalling */
    (uint32_t)IMAD_SEQUENCE_WRITE_LVL0_FIREWALL,
    (uint32_t)IMAD_TYPE_WRITE_EXTERNAL,
    (uint32_t)(&(((t_secure_registers *)SEC_REG_START_ADDR)->DDRShareHighMaxSpaceAddrCheck)),
    (uint32_t)sizeof(uint32_t),
    /* data */
    /* No padding */

    /* sequence LEVEL0_FIREWALLING: Configure Level0 firewalling */
    (uint32_t)IMAD_SEQUENCE_WRITE_LVL0_FIREWALL,
    (uint32_t)IMAD_TYPE_WRITE_BITMAP_EMBEDDED,
    (uint32_t)(&(((t_secure_registers *)SEC_REG_START_ADDR)->ScramblerControl)),
    (uint32_t)2*sizeof(uint32_t),
    /* data */
    (uint32_t)0x00000010, /* bitmask */
    (uint32_t)0x00000000, /* value */
#endif
};

