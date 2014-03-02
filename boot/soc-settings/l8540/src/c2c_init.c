/* C2C SoC-Settings table: standard config (not mirror-loopback)
 * Macro LE(value) is expected to convert 32bit value into
 * LittleEndian 32-bit value.
 *
 * Note for later optimizations:
 *  RAM optimization could be used to reduce this table to.
 *  It would use xp70 native 16-bit addressing mode:
 *  {// portcfg  mirror  irqsetHi  irqsetLo  fclk     rxmax
 *      0x0002,     0x0000, 0x0000,   0x0000,   0x00D5,  0x00D5,
 *   // geni_control    geni_mask       geno_interrupt      geno_level
 *      0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000
 *  };
 *  This 28-byte table could be moved to some unsecured xp70 TCDM area.
 */
#include <types.h>
#include <soc_settings.h>

/* Type = 4 means C2C structure*/
__attribute__((section(".type")))
  const u32 c2c_type = SECTION_TYPE_C2C_INIT;

extern u32 linker_payload_length;

__attribute__((section(".length")))
const u32 payload_length = (u32)&linker_payload_length;

__attribute__((section(".payload_c2c")))

const unsigned int C2CInit_table[] = {
 0x30030418,  /* maskCfg */
 0x00000006,  /* maskCfg */
 0x00000000,      /* C2C_REVISION, unused */
 0x00000000,      /* C2C_SYSCONFIG, unused */
 0x00000000,      /* C2C_SYSSTATUS, unused */
 0x00000002,  /* C2C_PORTCONFIG, 8-bit value used at C2C wake/sleep */
 0x00000000,  /* C2C_MIRRORMODE, 1-bit value used at C2C wake/sleep */
 0x00000000,      /* C2C_IRQ_RAW_STATUS_0, unused */
 0x00000000,      /* C2C_IRQ_RAW_STATUS_1, unused */
 0x00000000,      /* C2C_IRQ_ENABLE_STATUS_0, unused */
 0x00000000,      /* C2C_IRQ_ENABLE_STATUS_1, unused */
 0x00000000,      /* C2C_IRQ_ENABLE_SET_0, unused */
 0x00000000,  /* C2C_IRQ_ENABLE_SET_1, 32-bit value used at C2C wake/sleep */
 0x00000000,      /* C2C_IRQ_ENABLE_CLEAR_0, unused */
 0x00000000,      /* C2C_IRQ_ENABLE_CLEAR_1, unused */
 0x00000000,      /* C2C_IRQ_EOI, unused */
 0x00000000,      /* 32-bit gap */
 0x00000000,      /* 32-bit gap */
 0x000000D5,  /* C2C_FCLK_FREQ, 10-bit value used at C2C wake/sleep */
 0x000000D5,  /* C2C_RX_MAX_FREQ, 10-bit value used at C2C wake/sleep */
 0x00000000,      /* C2C_TX_MAX_FREQ, unused */
 0x00000000,      /* C2C_RX_MAX_FREQ_ACK, unused */
 0x00000000,      /* C2C_WAKE_REQ, unused */
 0x00000000,      /* C2C_WAKE_ACK, unused */
 0x00000000,      /* 32-bit gap */
 0x00000000,      /* 32-bit gap */
 0x00000000,      /* C2C_STANDBY, unused */
 0x00000000,      /* C2C_STANDBY_IN, unused */
 0x00000000,      /* C2C_WAIT, unused */
 0x00000000,      /* 32-bit gap */
 0x00000000,  /* C2C_GENI_CONTROL, 32-bit value used at C2C wake/sleep */
 0x00000000,  /* C2C_GENI_MASK, 32-bit value used at C2C wake/sleep */
 0x00000000,      /* 32-bit gap */
 0x00000000,      /* 32-bit gap */
 0x00000000,      /* C2C_GENO_STATUS, unused */
 0x00000000,  /* C2C_GENO_INTERRUPT, 32-bit value used at C2C wake/sleep */
 0x00000000,  /* C2C_GENO_LEVEL, 32-bit value used at C2C wake/sleep */
};


