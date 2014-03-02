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

#include <stdint.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>		/* close */
#include "libdb.h"

#include <sys/mman.h>

#define MAP_SIZE 4096UL
#define MAP_MASK (MAP_SIZE - 1)

#define DEVMEM_READ 0
#define DEVMEM_WRITE 1

int db_cmd(unsigned int mode, db_register *DB_reg)
{
	int fd;
	int ret = 0;
	void *map_base, *virt_addr;
	unsigned long writeval = DB_reg->u32_val;
	off_t target = DB_reg->addr;

	fd = open("/dev/mem", O_RDWR | O_SYNC);
	if (fd == -1) {
		ret = -1;
	} else {
		fflush(stdout);
		/* Map one page */
		map_base = mmap(0, MAP_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, target & ~MAP_MASK);
		if (map_base == (void *) -1) {
			ret = -1;
		} else {
			fflush(stdout);
			virt_addr = map_base + (target & MAP_MASK);
			switch (mode) {
			case DEVMEM_READ:
				DB_reg->u32_val = *((unsigned long *) virt_addr);
				break;
			case DEVMEM_WRITE:
				*((unsigned long *) virt_addr) = writeval;
				break;
			case 2:
				printf("tat_bb_check_addr is not implemented\n");
				break;
			default:
				printf("tat_bb_cmd called with unknown mode\n");
			}
			fflush(stdout);
			if (munmap(map_base, MAP_SIZE) == -1) {
				ret = -1;
			}
		}
		close(fd);
	}

	return ret;
}

int db_u32_read(regDB_addr_t addr, regDB_val_t *val)
{
	int ret;
	db_register DB_reg;

	DB_reg.addr = addr;

	ret = db_cmd(0, &DB_reg);
	if (ret >= 0)
		*val = DB_reg.u32_val;

	return ret;
}

int db_u32_write(regDB_addr_t addr, regDB_val_t val)
{
	db_register DB_reg;

	DB_reg.addr = addr;
	DB_reg.u32_val = val;

	return db_cmd(1, &DB_reg);
}

int db_check_addr(regDB_addr_t addr, long *iomap_index)
{
	int ret;
	db_register DB_reg;
	DB_reg.addr = addr;

	ret = db_cmd(2, &DB_reg);
	if (ret == 0) {
		/* index returned is 1-based index, 0 is for not found. */
		*iomap_index = (long)(DB_reg.u32_val - 1);
	} else {
		/* Error: CHECK_ADDR failed with code ret */
		printf("HWREG CHECK_ADDR failed with code %d\n", ret);
	}

	return ret;
}
