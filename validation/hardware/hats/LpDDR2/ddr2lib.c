
#include "ddr2lib.h"

#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>		/* for close */
#include <sys/ioctl.h>
#include <stdio.h>

#include <stdlib.h>
#include <limits.h>
#include <sys/mman.h>

/* DB8500 V1 */
#define DMC_CTL_90          0x80156168
#define DMC_CTL_91          0x8015616c

#define MAP_SIZE 4096UL
#define MAP_MASK (MAP_SIZE - 1)

int ddr2_parse_address(const char *str, ddr2_addr_t *addr)
{
    int is_ok = 1;
    long long_addr;
    char* end = 0;

    long_addr = strtol(str, &end, 16);
    
    if ( ( *end != 0 ) 
        || (long_addr == LONG_MIN) 
        || (long_addr == LONG_MAX) 
        || (0 > long_addr) 
        || (255 < long_addr) )
    {
        /* input is not a 16 bits DDR2 address */
        is_ok = 0;
    }
    else
    {
        /* str is designating a valid DDR2 address */
        *addr = long_addr;
    }
    
    return is_ok;
}


int _bb_access(unsigned int mode, db_register *DB_reg)
{
	int fd;
	int ret = 0;
	void *map_base, *virt_addr;
	unsigned long writeval = DB_reg->u32_val;
	off_t target = DB_reg->addr;

	fd = open("/dev/mem", O_RDWR | O_SYNC);
	if (fd == -1)
		return -1;
	fflush(stdout);

	/* Map one page */
	map_base = mmap(0, MAP_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, target & ~MAP_MASK);
	if (map_base == (void *)-1)
		return -1;

	virt_addr = map_base + (target & MAP_MASK);

	switch (mode) {
	case 1:
		*((unsigned long *)virt_addr) = writeval;
		break;
	default:
		DB_reg->u32_val = *((unsigned long *)virt_addr);
		break;
	}

	if (munmap(map_base, MAP_SIZE) == -1)
		return -1;
	close(fd);

	return ret;
}

int ddr2_read(ddr2_addr_t addr, uint16_t *val)
{
    int ret = 0;

    /* read register DMC_CTL_91 */
	db_register reg;
	reg.addr = DMC_CTL_91;
	reg.u32_val = 0;

    ret = _bb_access(DEVMEM_READ, &reg);
    if ( ret >= 0 )
    {
        /* set bit 0..7 and bit 1 of register DMC_CTL_91 */
        reg.u32_val &= ~0x000100ff;
        reg.u32_val |= 0x00010000 | addr;

		ret = _bb_access(DEVMEM_WRITE, &reg);
    }

    if ( ret >= 0 )
    {
        /* read register DMC_CTL_90 */
        reg.addr = DMC_CTL_90;
        reg.u32_val = 0;
        ret = _bb_access(DEVMEM_READ, &reg);
    }
    
    if ( ret >= 0 )
    {
        *val = reg.u32_val >> 16;
    }
    
    return ret;
}

