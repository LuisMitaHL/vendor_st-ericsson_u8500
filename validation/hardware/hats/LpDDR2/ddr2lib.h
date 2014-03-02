/* HATS DDR2 registers access library */
/* depends of tat-dd-lib library */

#ifndef HATS_DDR2_LIB_H__
#define HATS_DDR2_LIB_H__

#include <stdint.h>

typedef uint16_t ddr2_addr_t;

typedef struct {
  uint32_t addr;/* address */
  uint32_t u32_val;
} db_register;

enum{
  DEVMEM_READ,
  DEVMEM_WRITE
};


/* parse a DDR2 address passed as a string. */
/* @param str [in] a string representation of a 16 bits address. can be prefixed by '0x' or not */
/* @param addr [out] the converted register address if successfull */
/* @return if \a str is a valid representation for a DDR2 then non 0 value is returned. If it fails, 0 is returned. */
int ddr2_parse_address(const char *str, ddr2_addr_t *addr);

/* read a DDR2 register content */
/* @param addr [in] a DDR2 register address */
/* @param val [out] pointer to the data read from the DDR2 register if successfull. Cannot be null */
/* @return 0 or positive if the function succeeded. A negative value indicates an error occured. */
int ddr2_read(ddr2_addr_t reg, uint16_t *val);

#endif
