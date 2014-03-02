/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */


#ifndef _CACHE_API_H_
#define _CACHE_API_H_

/*****************************************************************************/
/*
 * Includes							       
 */
/*****************************************************************************/

#include <stwdsp.h>	                  /* Standard include for MMDSP  */
#include <archi.h>                    /* for EMU_unit_maskit */
#include "t1xhv_types.h"              /* Type definition                      */
//#include "macros.h"
#include "new_regs.h" 
#include "mmdsp_api.h"
/*****************************************************************************/
/*
 * Types							       
 */
/*****************************************************************************/


/* Emulation host registers used for the cache system */
#define UDATA_0 0x20
#define UDATA_1 0x21
#define UDATA_2 0x22
#define UDATA_3 0x23
#define UDATA_4 0x24
#define UDATA_5 0x25
#define UDATA_6 0x26
#define UDATA_7 0x27
#define UADDRL  0x28
#define UADDRM  0x29
#define UADDRH  0x36
#define UCMD    0x2A

typedef enum
{
  cache_24_1 = 0,
  cache_24_2 = 1,
  cache_16_1 = 2,
  cache_16_2 = 3,
  cache_16_3 = 4,
  cache_16_4 = 5
} t_cache_base;

/* 31:0 */
#pragma inline
static void SET_REG_EMUL_32L(t_uint16 reg, t_uint32 value)
{
      /* data */ 
    HOST[UDATA_0] = (value    )&0x000000FFUL;
    HOST[UDATA_1] = (value>> 8)&0x000000FFUL;
    HOST[UDATA_2] = (value>>16)&0x000000FFUL;
    HOST[UDATA_3] = (value>>24)&0x000000FFUL;
      /* addr */
    HOST[UADDRH] = 0x0; /* H 23-16 */
    HOST[UADDRM] = (reg>>16)&0xFF; /* M 15- 8 */
    HOST[UADDRL] = (reg&0xFF); /* L  7- 0 */
      /* cmd */
    HOST[UCMD] = 0x10; /**/
    
}
/* 63:32 */
#pragma inline
static void SET_REG_EMUL_32H(t_uint16 reg, t_uint32 value)
{
      /* data */ 
    HOST[UDATA_4] = (value    )&0x000000FFUL;
    HOST[UDATA_5] = (value>> 8)&0x000000FFUL;
    HOST[UDATA_6] = (value>>16)&0x000000FFUL;
    HOST[UDATA_7] = (value>>24)&0x000000FFUL;
      /* addr */
    HOST[UADDRH] = 0x0; /* H 23-16 */
    HOST[UADDRM] = (reg>>16)&0xFF; /* M 15- 8 */
    HOST[UADDRL] = (reg&0xFF); /* L  7- 0 */
      /* cmd */
    HOST[UCMD] = 0x10; /**/
}
#pragma inline
static t_uint32 GET_REG_EMUL_32L(t_uint16 reg)
{
    t_uint32 value;
      /* addr */
    HOST[UADDRH] = 0x0; /* H 23-16 */
    HOST[UADDRM] = (reg>>16)&0xFF; /* M 15- 8 */
    HOST[UADDRL] = (reg&0xFF); /* L  7- 0 */
      /* cmd */
    HOST[UCMD] = 0x14; /**/
    MMDSP_NOP();
    MMDSP_NOP();
    MMDSP_NOP();
    MMDSP_NOP();
      /* data */ 
    value =               HOST[UDATA_3];
    value = (value <<8) + HOST[UDATA_2];
    value = (value <<8) + HOST[UDATA_1];
    value = (value <<8) + HOST[UDATA_0];
    return value;
    
}
/* 63:32 */
#pragma inline
static t_uint32 GET_REG_EMUL_32H(t_uint16 reg)
{
    t_uint32 value;
      /* addr */
    HOST[UADDRH] = 0x0; /* H 23-16 */
    HOST[UADDRM] = (reg>>16)&0xFF; /* M 15- 8 */
    HOST[UADDRL] = (reg&0xFF); /* L  7- 0 */
      /* cmd */
    HOST[UCMD] = 0x14; /**/
    MMDSP_NOP();
    MMDSP_NOP();
    MMDSP_NOP();
    MMDSP_NOP();
      /* data */ 
    value =               HOST[UDATA_7];
    value = (value <<8) + HOST[UDATA_6];
    value = (value <<8) + HOST[UDATA_5];
    value = (value <<8) + HOST[UDATA_4];
    return value;
}
#pragma inline
static void SET_AHB_EXTMEM24_BASE_1(t_uint32 value)
{
    SET_REG_EMUL_32L(0xA,value);
}
#pragma inline
static void SET_AHB_EXTMEM24_TOP_1(t_uint32 value)
{
    SET_REG_EMUL_32L(0x14,value);
}
#pragma inline
static void SET_AHB_EXTMEM24_BASE_2(t_uint32 value)
{
    SET_REG_EMUL_32L(0xC,value);
}
#pragma inline
static void SET_AHB_EXTMEM24_TOP_2(t_uint32 value)
{
    SET_REG_EMUL_32L(0x15,value);
}
#pragma inline
static void SET_AHB_EXTMEM16_BASE_1(t_uint32 value)
{
    SET_REG_EMUL_32H(0xA,value);
}
static void SET_AHB_EXTMEM16_TOP_1(t_uint32 value)
{
    SET_REG_EMUL_32H(0x14,value);
}
#pragma inline
static void SET_AHB_EXTMEM16_BASE_2(t_uint32 value)
{
    SET_REG_EMUL_32H(0xC,value);
}
#pragma inline
static void SET_AHB_EXTMEM16_TOP_2(t_uint32 value)
{
    SET_REG_EMUL_32H(0x15,value);
}
#pragma inline
static void SET_XBUS_CACHE_BASE16_2(t_uint32 base16_2)
{
#ifdef MMDSPSIM_WA
#else
      /* data */  /* base address = 0xXX0000 */
    ASSERT (base16_2&0xFF00FFFFUL ==0);
    HOST[UDATA_4] = (base16_2>>16);
      /* addr */
      /* data2_1624_xa_base */
    HOST[UADDRH] = 0x0; /* H 23-16 */
    HOST[UADDRM] = 0x0; /* M 15- 8 */
    HOST[UADDRL] = 0xF; /* L  7- 0 */
      /* cmd */
    HOST[UCMD] = 0x10; /**/
#endif
}
#pragma inline
static void SET_XBUS_CACHE_BASE24_2(t_uint32 base24_2)
{
#ifdef MMDSPSIM_WA
#else
      /* data */  /* base address = 0xXX0000 */
    ASSERT (base24_2&0xFF00FFFFUL ==0);
    HOST[UDATA_0] = (base24_2&0xFF);
      /* addr */
      /* data2_1624_xa_base */
    HOST[UADDRH] = 0x0; /* H 23-16 */
    HOST[UADDRM] = 0x0; /* M 15- 8 */
    HOST[UADDRL] = 0xF; /* L  7- 0 */
      /* cmd */
    HOST[UCMD] = 0x10; /**/
#endif
}
#pragma inline
static void SET_AHB_EXTMEM_ACTIV(void)
{      /* data */ 
    HOST[UDATA_0] = 1;
      /* addr */
    HOST[UADDRH] = 0x0; /* H 23-16 */
    HOST[UADDRM] = 0x0; /* M 15- 8 */
    HOST[UADDRL] = 0x17; /* L  7- 0 */
      /* cmd */
    HOST[UCMD] = 0x10; /**/

    
}
#pragma inline
static t_uint32 GET_AHB_EXTMEM16_BASE_1(void)
{
    return (GET_REG_EMUL_32H(0xA));
}
#pragma inline
static t_uint32 GET_AHB_EXTMEM16_TOP_1(void)
{
    return (GET_REG_EMUL_32H(0x14));
}
#pragma inline
static t_uint32 GET_AHB_EXTMEM16_BASE_2(void)
{
    return (GET_REG_EMUL_32H(0xC));
}
#pragma inline
static t_uint32 GET_AHB_EXTMEM16_TOP_2(void)
{
    return (GET_REG_EMUL_32H(0x15));
}
#pragma inline
static t_uint32 GET_AHB_EXTMEM24_BASE_1(void)
{
    return (GET_REG_EMUL_32L(0xA));
}
#pragma inline
static t_uint32 GET_AHB_EXTMEM24_TOP_1(void)
{
    return (GET_REG_EMUL_32L(0x14));
}
#pragma inline
static t_uint32 GET_AHB_EXTMEM24_BASE_2(void)
{
    return (GET_REG_EMUL_32L(0xC));
}
#pragma inline
static t_uint32 GET_AHB_EXTMEM24_TOP_2(void)
{
    return (GET_REG_EMUL_32L(0x15));
}

/**************/
/* New in V81 */
/**************/
#pragma inline
static t_uint32 GET_AHB_EXTMEM16_BASE_3(void)
{
    return (GET_REG_EMUL_32H(0x1B));
}
#pragma inline
static t_uint32 GET_AHB_EXTMEM16_TOP_3(void)
{
    return (GET_REG_EMUL_32H(0x1D));
}

#pragma inline
static t_uint32 GET_AHB_EXTMEM16_BASE_4(void)
{
    return (GET_REG_EMUL_32H(0x1C));
}
#pragma inline
static t_uint32 GET_AHB_EXTMEM16_TOP_4(void)
{
    return (GET_REG_EMUL_32H(0x1E));
}
#pragma inline
static t_uint32 GET_XBUS_BASE16_3(void)
{
    t_uint32 xbus_base16_base;
	// in function GET_REG_EMUL_32H, we add some nop after CMD,
	// we read UDATA 7 to 4, and we deal with value,
	// which add some instructions between reads of UDATA.
	// All this is needed to work.
	xbus_base16_base = (GET_REG_EMUL_32H(0xF)<<8)&0xFF0000UL;
  return xbus_base16_base;
}

#pragma inline
static t_uint32 GET_XBUS_BASE16_4(void)
{
    t_uint32 xbus_base16_base;
	// in function GET_REG_EMUL_32H, we add some nop after CMD,
	// we read UDATA 7 to 4, and we deal with value,
	// which add some instructions between reads of UDATA.
	// All this is needed to work.
	xbus_base16_base = (GET_REG_EMUL_32H(0xF))&0xFF0000UL;
  return xbus_base16_base;
}



#pragma inline
static t_uint32 GET_XBUS_BASE24_1(void)
{
    return (0x10000UL);
}
#pragma inline
static t_uint32 GET_XBUS_BASE24_2(void)
{
    t_uint32 xbus_base24_base;
#ifdef MMDSPSIM_WA
    xbus_base24_base = 0x400000UL;
#else
	// in function GET_REG_EMUL_32L, we add some nop after CMD,
	// we read UDATA 3 to 0, and we deal with value,
	// which add some instructions between reads of UDATA.
	// All this is needed to work.
	xbus_base24_base = GET_REG_EMUL_32L(0xF)<<16;
#endif
    return xbus_base24_base;
}
#pragma inline
static t_uint32 GET_XBUS_BASE16_1(void)
{
    return (0x800000UL);
}
#pragma inline
static t_uint32 GET_XBUS_BASE16_2(void)
{
    t_uint32 xbus_base16_base;
#ifdef MMDSPSIM_WA
    xbus_base16_base = 0xBC0000UL;
#else
	// in function GET_REG_EMUL_32H, we add some nop after CMD,
	// we read UDATA 7 to 4, and we deal with value,
	// which add some instructions between reads of UDATA.
	// All this is needed to work.
	xbus_base16_base = (GET_REG_EMUL_32H(0xF)<<16)&0xFF0000UL;
#endif
    return xbus_base16_base;
}
#pragma inline
static t_uint32 GET_XBUS_EXTMEM_BASE(t_cache_base base)
{
    switch (base)
    {
        case cache_24_1:
          return (GET_XBUS_BASE24_1());
          break;
        case cache_24_2:
          return (GET_XBUS_BASE24_2());
          break;
        case cache_16_1:
          return (GET_XBUS_BASE16_1());  /* by default it is 0x800000 */
          break;
        case cache_16_2:
          return (GET_XBUS_BASE16_2());  /* by default it is 0x820000 */
          break;
        case cache_16_3:
          return (GET_XBUS_BASE16_3());  /* by default it is 0xF00000 */ 
          break;
        case cache_16_4:
          return (GET_XBUS_BASE16_4());  /* by default it is 0xF20000 */ 
          break;
        /* no default : only 4 bases */
        default:
          return 0;
          break;
    }
}
#pragma inline
static t_uint32 GET_AHB_EXTMEM_BASE(t_cache_base base)
{
    switch (base)
    {
        case cache_24_1:
          return (GET_AHB_EXTMEM24_BASE_1());
          break;
        case cache_24_2:
          return (GET_AHB_EXTMEM24_BASE_2());
          break;
        case cache_16_1:
          return (GET_AHB_EXTMEM16_BASE_1());
          break;
        case cache_16_2:
          return (GET_AHB_EXTMEM16_BASE_2());
          break;
        /* no default : only 4 bases */
        default:
          return 0;
          break;
    }
}

#ifndef __T1XHV_NMF_ENV
#pragma inline
static t_uint32 CONVERT_AHB_TO_EXTMEM(t_uint32 ahb_address)
#else
typedef enum
{
  mem24,
  mem16
} t_mem_type;
#pragma inline
static t_uint32 CONVERT_AHB_TO_EXTMEM(t_uint32 ahb_address, t_mem_type mem_type)
#endif
{
  t_uint32 xbus_address;
  t_uint32 ahb_base_address;
  t_uint32 ahb_base24_1_address;
  t_uint32 ahb_top24_1_address;
  t_uint32 ahb_base24_2_address;
  t_uint32 ahb_top24_2_address;
  t_uint32 ahb_base16_1_address;
  t_uint32 ahb_top16_1_address;
  t_uint32 ahb_base16_2_address;
  t_uint32 ahb_top16_2_address;
#ifdef __T1XHV_NMF_ENV
  t_uint32 ahb_base16_3_address;
  t_uint32 ahb_top16_3_address;
  t_uint32 ahb_base16_4_address;
  t_uint32 ahb_top16_4_address;
#endif  
  /* WORK AROUND : init registers that should be written by the host for TLM */
#ifdef CACHE_WA
  /* Activate second data bases */
//  SET_AHB_EXTMEM_ACTIV();
//  /* BASE ADDRESS 1 */
//  SET_AHB_EXTMEM24_BASE_1(0x00660000UL);
//  /* TOP ADDRESS 1 */
//  SET_AHB_EXTMEM24_TOP_1(0x0067FFFFUL);
//  /* BASE ADDRESS 1 */
//  SET_AHB_EXTMEM16_BASE_1(0x00680000UL);
//  /* TOP ADDRESS 1 */
//  SET_AHB_EXTMEM16_TOP_1(0x0075FFFFUL);
//  /* BASE ADDRESS 2 */
//  SET_AHB_EXTMEM16_BASE_2(0x20070000UL);
//  /* TOP ADDRESS 2 */
//  SET_AHB_EXTMEM16_TOP_2(0x2007FF00UL);
//  /* BASE ADDRESS 2 */
//  SET_AHB_EXTMEM16_BASE_2(0x20040000UL);
//  /* TOP ADDRESS 2 */
//  SET_AHB_EXTMEM16_TOP_2(0x2006FFFFUL);
  /* data_16(24)_xa_base */
  SET_XBUS_CACHE_BASE24_2(0x400000UL);
  /* data_16(24)_xa_base */
  SET_XBUS_CACHE_BASE16_2(0xBC0000UL);
#endif
  /* address given is in AHB_ZONE_24_1 or AHB_ZONE_16_1 ? */
  /* get base and top addresses */
  ahb_base24_1_address = GET_AHB_EXTMEM24_BASE_1();
  ahb_top24_1_address  = GET_AHB_EXTMEM24_TOP_1();
  ahb_base24_2_address = GET_AHB_EXTMEM24_BASE_2();
  ahb_top24_2_address  = GET_AHB_EXTMEM24_TOP_2();
  ahb_base16_1_address = GET_AHB_EXTMEM16_BASE_1();
  ahb_top16_1_address  = GET_AHB_EXTMEM16_TOP_1();
  ahb_base16_2_address = GET_AHB_EXTMEM16_BASE_2();
  ahb_top16_2_address  = GET_AHB_EXTMEM16_TOP_2();
  
#ifdef __T1XHV_NMF_ENV
  ahb_base16_3_address = GET_AHB_EXTMEM16_BASE_3();
  ahb_top16_3_address  = GET_AHB_EXTMEM16_TOP_3();
  ahb_base16_4_address = GET_AHB_EXTMEM16_BASE_4();
  ahb_top16_4_address  = GET_AHB_EXTMEM16_TOP_4();
#endif  
    
  /* guess in which zone is the address */
#ifdef __T1XHV_NMF_ENV
  if (mem_type == mem24) {
#endif
  /* BASE24_1 */
  if ( (ahb_address >= (ahb_base24_1_address)) &&
       (ahb_address <= (ahb_top24_1_address)) )
  {
       xbus_address = ((ahb_address - ahb_base24_1_address) >> 2) + GET_XBUS_EXTMEM_BASE(cache_24_1);
       return xbus_address;
  }
  /* BASE24_2 */
  if ( (ahb_address >= (ahb_base24_2_address)) &&
       (ahb_address <= (ahb_top24_2_address)) )
  {
       xbus_address = ((ahb_address - ahb_base24_2_address) >> 2) + GET_XBUS_EXTMEM_BASE(cache_24_2);
       return xbus_address;
  }
#ifdef __T1XHV_NMF_ENV
  }
  if (mem_type == mem16) {
#endif

  /* BASE16_1 */
  if ( (ahb_address >= (ahb_base16_1_address)) &&
       (ahb_address <= (ahb_top16_1_address)) )
  {
       xbus_address = ((ahb_address - ahb_base16_1_address) >> 1) + GET_XBUS_EXTMEM_BASE(cache_16_1);
       return xbus_address;
  }
  /* BASE16_2 */
  if ( (ahb_address >= (ahb_base16_2_address)) &&
       (ahb_address <= (ahb_top16_2_address)) )
  {
       xbus_address = ((ahb_address - ahb_base16_2_address) >> 1) + GET_XBUS_EXTMEM_BASE(cache_16_2);
       return xbus_address;
  }
#ifdef __T1XHV_NMF_ENV
  /* BASE16_3 */
  if ( (ahb_address >= (ahb_base16_3_address)) &&
       (ahb_address <= (ahb_top16_3_address)) )
  {
       xbus_address = ((ahb_address - ahb_base16_3_address) >> 1) + GET_XBUS_EXTMEM_BASE(cache_16_3);
       return xbus_address;
  }
  
    /* BASE16_4 */
  if ( (ahb_address >= (ahb_base16_4_address)) &&
       (ahb_address <= (ahb_top16_4_address)) )
  {
       xbus_address = ((ahb_address - ahb_base16_4_address) >> 1) + GET_XBUS_EXTMEM_BASE(cache_16_4);
       return xbus_address;
  }
  

  }
#endif

  return 0;
}

#pragma inline
static void FLUSH_D_CACHE(void)
{

    t_uint16  MMIO *p_reg = (t_uint16 MMIO *)0xEC09U;
    *p_reg = 0x7;

}
#endif /* _CACHE_API_H_ */
