/*****************************************************************************/
/**
*  © ST-Ericsson, 2011 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \brief   This module provides routines to manage ab dth functions
* \author  ST-Ericsson
*/
/*****************************************************************************/

/* DB8500 is 32 bits based */
typedef uint32_t regDB_addr_t;
typedef uint32_t regDB_val_t;

typedef struct {
  uint32_t addr;/* address */
  uint32_t u32_val;
} db_register;

int db_u32_read(regDB_addr_t addr, regDB_val_t *val);
int db_u32_write(regDB_addr_t addr, regDB_val_t val);

/* Tells if addr is in virtual address space or not.
 * @param addr          in, a valid address for DB8500
 * @param iomap_index   out, non NULL pointer to a long filled with iomap index found.
 *                      Its value is -1 if addr is not in virtual space else the value is a 0-based index into ioremap table
 * @return 0 if the function succeeds, -1 if an error occured when opening /dev/hwreg, > 0 : standard error code.
 */
int db_check_addr(regDB_addr_t addr, long *iomap_index);

