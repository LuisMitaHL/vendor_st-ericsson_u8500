/** ----------------------------------------------------------------------------
* \author  Mickael CHAVE
* \brief   This routine set a SIA register
* \param   reg_string
* \param   reg_offset
* \param   val
* \return  OK if correct init
---------------------------------------------------------------------------- */
#include "sia_register_fct.h"
#include "ite_main.h"

#define CHARAC

int hi_SetReg_with_string(char *reg_string, int reg_offset, int reg_width, t_uint32 val)
{
    UNUSED(reg_string);
#ifndef CHARAC
    unsigned int time = 0;
#endif
    switch (reg_width) {
    case  8 : hw_write8(SIA_MEMORY_SPACE_ADDR_L + reg_offset, val); break;
    case 16 : hw_write16(SIA_MEMORY_SPACE_ADDR_L + reg_offset, val); break;
    case 32 : hw_write32(SIA_MEMORY_SPACE_ADDR_L + reg_offset, val); break;
		  /*
          case  8 : hw_write8( ((uintptr_t) (SIA_MEMORY_SPACE_ADDR_L)) + reg_offset, val); break;
          case 16 : hw_write16(((uintptr_t) (SIA_MEMORY_SPACE_ADDR_L)) + reg_offset, val); break;
          case 32 : hw_write32(((uintptr_t) (SIA_MEMORY_SPACE_ADDR_L)) + reg_offset, val); break;
        */
    }
#ifndef CHARAC
    get_time(&time);
    switch (reg_width) {
    case  8 : PRINTF_LEVEL(PROG,("\ttime %d ns : Set %d bits Register %s - addr = 0x%08x - val=0x%02x\n", 
                  time, reg_width, reg_string, reg_offset, val)); break;
    case 16 : PRINTF_LEVEL(PROG,("\ttime %d ns : Set %d bits Register %s - addr = 0x%08x - val=0x%04x\n", 
                  time, reg_width, reg_string, reg_offset, val)); break;
    case 32 : PRINTF_LEVEL(PROG,("\ttime %d ns : Set %d bits Register %s - addr = 0x%08x - val=0x%08x\n", 
                  time, reg_width, reg_string, reg_offset, val)); break;
    }
#endif
    return (1);
}
/** ----------------------------------------------------------------------------
* \author  Mickael CHAVE
* \brief   This routine get a SIA register value
* \param   reg_string
* \param   reg_offset
* \param   reg_width
* \return  the register value
---------------------------------------------------------------------------- */

t_uint32 hi_GetReg_with_string(char *reg_string, int reg_offset, int reg_width)
{

    UNUSED(reg_string);
#ifndef CHARAC
    unsigned int time = 0;
#endif
    t_uint32 val = 0;  //__NO_WARNING__ being used before being set
    
    switch (reg_width) {
    case  8 : val = hw_read8(SIA_MEMORY_SPACE_ADDR_L + reg_offset); break;
    case 16 : val = hw_read16(SIA_MEMORY_SPACE_ADDR_L + reg_offset); break;
    case 32 : val = hw_read32(SIA_MEMORY_SPACE_ADDR_L + reg_offset); break;
    /*
    case  8 : val = hw_read8( ((uintptr_t) (SIA_MEMORY_SPACE_ADDR_L)) + reg_offset); break;
    case 16 : val = hw_read16(((uintptr_t) (SIA_MEMORY_SPACE_ADDR_L)) + reg_offset); break;
    case 32 : val = hw_read32(((uintptr_t) (SIA_MEMORY_SPACE_ADDR_L)) + reg_offset); break;
        */
    }
#ifndef CHARAC
    get_time(&time);
    switch (reg_width) {
    case  8 : PRINTF_LEVEL(PROG,("\ttime %d ns : Get %d bits Register %s - addr = 0x%08x - val=0x%02x\n", 
                  time, reg_width, reg_string, reg_offset, val)); break;
    case 16 : PRINTF_LEVEL(PROG,("\ttime %d ns : Get %d bits Register %s - addr = 0x%08x - val=0x%04x\n", 
                  time, reg_width, reg_string, reg_offset, val)); break;
    case 32 : PRINTF_LEVEL(PROG,("\ttime %d ns : Get %d bits Register %s - addr = 0x%08x - val=0x%08x\n", 
                  time, reg_width, reg_string, reg_offset, val)); break;
    }
#endif
    return (val);
}

