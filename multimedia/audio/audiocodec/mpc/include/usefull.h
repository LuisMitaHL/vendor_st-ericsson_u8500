/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   usefull.h
 * \brief  
 * \author ST-Ericsson
 */
/*****************************************************************************/
#ifndef  _USEFULL_H_
#define  _USEFULL_H_
#include <archi.h>
#include <inc/type.h>
//-------------------------------
// Long (48-32)  Bit Handling
//-------------------------------
static inline unsigned int getBitLong(unsigned long value, unsigned int offset){
    return ((value >> offset) & 1UL);
}

static inline void setBitLong(unsigned long * value, unsigned int offset){
    *value |= 1UL << offset;
}

static inline void resetBitLong(unsigned long * value, unsigned int offset){
    *value &= ~(1UL << offset);
}

static inline unsigned long getBitFieldLong(unsigned long value, unsigned int offset, unsigned int nbbit){
    unsigned long mask = (1UL << nbbit)-1;
    return (value >> offset) & mask;
}

//-------------------------------
// Short (24-16)  Bit Handling
//-------------------------------
static inline unsigned int getBitShort(unsigned int value, unsigned int offset){
    return (value >> offset) & 1U;
}

static inline void setBitShort(unsigned int * value, unsigned int offset){
    *value |= 1U << offset;
}

static inline void resetBitShort(unsigned int * value, unsigned int offset){
    *value &= ~(1U << offset);
}

static inline unsigned int getBitFieldShort(unsigned int value, unsigned int offset, unsigned int nbbit){
    unsigned int mask = (1L << nbbit)-1;
    return (value >> offset) & mask;
}


#define SET_BIT(a,b)   ((a) |=  (1<<(b)))
#define RESET_BIT(a,b) ((a) &= ~(1<<(b)))

static inline void AUDIO_MASK_IT(unsigned int it){    
    it < 16 ? SET_BIT(ITREMAP_interf_itmskl, (it)) : SET_BIT(ITREMAP_interf_itmskh, (it)-16);
    ITREMAP_interf_itmskl;
    ITREMAP_interf_itmskl;
    ITREMAP_interf_itmskl; 
}

static inline void AUDIO_UNMASK_IT(unsigned int it){
    (it) < 16 ? RESET_BIT(ITREMAP_interf_itmskl, (it)) : RESET_BIT(ITREMAP_interf_itmskh, ((it)-16));
}

typedef volatile __MMIO struct {
    t_uword mode;
    t_uword control;
    t_uword way;
    t_uword line;
    t_uword command;
    t_uword status;
    t_uword cptr1l;
    t_uword cptr1h;
    t_uword cptr2l;
    t_uword cptr2h;
    t_uword cptr3l;
    t_uword cptr3h;
    t_uword cptrsel;
    t_uword flushl_min_w;
    t_uword flushm_min_w;
    t_uword flushl_max_w;
    t_uword flushm_max_w;
} t_experimental_dcache_regs;

#define CACHE_FLUSH_WAIT_CYCLE 140

static inline void DCACHE_Flush(void){
    int i;
    Dcache_command = 0x7;
    for(i = 0; i < CACHE_FLUSH_WAIT_CYCLE; i++) {
        wnop();
    }
    Dcache_command = 0x0;
}

#endif   // _USEFULL_H_
