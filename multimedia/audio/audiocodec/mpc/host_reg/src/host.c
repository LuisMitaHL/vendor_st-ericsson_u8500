
/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   host.c
 * \brief  
 * \author ST-Ericsson
 */
/*****************************************************************************/
#include <audiocodec/mpc/host_reg.nmf>
#include <archi.h>

#define HOST_BASE_ADDR 0xEE00

#define HOST_udata0 (*((volatile HOSTMEM int  *) ( HOST_BASE_ADDR + 0x20)))
#define HOST_udata1 (*((volatile HOSTMEM int  *) ( HOST_BASE_ADDR + 0x21)))
#define HOST_udata2 (*((volatile HOSTMEM int  *) ( HOST_BASE_ADDR + 0x22)))
#define HOST_udata3 (*((volatile HOSTMEM int  *) ( HOST_BASE_ADDR + 0x23)))
#define HOST_udata4 (*((volatile HOSTMEM int  *) ( HOST_BASE_ADDR + 0x24)))
#define HOST_udata5 (*((volatile HOSTMEM int  *) ( HOST_BASE_ADDR + 0x25)))
#define HOST_udata6 (*((volatile HOSTMEM int  *) ( HOST_BASE_ADDR + 0x26)))
#define HOST_udata7 (*((volatile HOSTMEM int  *) ( HOST_BASE_ADDR + 0x27)))
#define HOST_uaddrl (*((volatile HOSTMEM int  *) ( HOST_BASE_ADDR + 0x28)))
#define HOST_uaddrh (*((volatile HOSTMEM int  *) ( HOST_BASE_ADDR + 0x29)))
#define HOST_uaddrm (*((volatile HOSTMEM int  *) ( HOST_BASE_ADDR + 0x36)))
#define HOST_ucmd   (*((volatile HOSTMEM int  *) ( HOST_BASE_ADDR + 0x2A)))

static volatile int nop =0;



void write_indirect(unsigned long addr, unsigned char tab[8] ){
    HOST_uaddrl = addr       & 0xFF;
    HOST_uaddrm = (addr>>8)  & 0xFF;
    HOST_uaddrh = (addr>>16) & 0xFF;

    HOST_udata0 = tab[0];
    HOST_udata1 = tab[1];
    HOST_udata2 = tab[2];
    HOST_udata3 = tab[3];
    HOST_udata4 = tab[4];
    HOST_udata5 = tab[5];
    HOST_udata6 = tab[6];
    HOST_udata7 = tab[7];

    HOST_ucmd = 0x10 ; // write cache control reg

    nop++;
    nop++;
    nop++;
}

void setSTBUS_ExtIOAddr(unsigned long addrbase, unsigned long addrtop){
    char tab[8];
    
    tab[0] = (addrtop >> 0) & 0xFFUL;
    tab[1] = (addrtop >> 8) & 0xFFUL;
    tab[2] = (addrtop >> 16) & 0xFFUL;
    tab[3] = (addrtop >> 24) & 0xFFUL;
    tab[4] = (addrbase >> 0) & 0xFFUL;
    tab[5] = (addrbase >> 8) & 0xFFUL;
    tab[6] = (addrbase >> 16) & 0xFFUL;
    tab[7] = (addrbase >> 24) & 0xFFUL;

    write_indirect(0xB, tab);
}

void read_indirect(unsigned long addr, unsigned char tab_result[8] ){
    HOST_uaddrl = addr       & 0xFF;
    HOST_uaddrm = (addr>>8)  & 0xFF;
    HOST_uaddrh = (addr>>16) & 0xFF;

    HOST_ucmd = 0x14 ; // read cache control reg 

    nop++;
    nop++;
    nop++;
    
    tab_result[0] = HOST_udata0;
    tab_result[1] = HOST_udata1;
    tab_result[2] = HOST_udata2;
    tab_result[3] = HOST_udata3;
    tab_result[4] = HOST_udata4;
    tab_result[5] = HOST_udata5;
    tab_result[6] = HOST_udata6;
    tab_result[7] = HOST_udata7;
}


unsigned long read_indirect_msb32(unsigned long addr){
    unsigned long ret =0;
    unsigned char tab_result[8];
    read_indirect( addr, tab_result );
    ret =  ((unsigned long)tab_result[4]) & 0xFF;
    ret += (((unsigned long)tab_result[5]) & 0xFF) << 8;
    ret += (((unsigned long)tab_result[6]) & 0xFF) << 16;
    ret += (((unsigned long)tab_result[7]) & 0xFF) << 24;
    return ret;
}

unsigned long read_indirect_lsb32(unsigned long addr){
    unsigned long ret =0;
    unsigned char tab_result[8];
    read_indirect( addr, tab_result );
    ret =  ((unsigned long)tab_result [0]) & 0xFF;
    ret += (((unsigned long)tab_result[1]) & 0xFF) << 8;
    ret += (((unsigned long)tab_result[2]) & 0xFF) << 16;
    ret += (((unsigned long)tab_result[3]) & 0xFF) << 24;
    return ret;
}

unsigned long read_indirect_8(unsigned long addr, int offset_in_byte){
    unsigned long ret =0;
    unsigned char tab_result[8];
    read_indirect( addr, tab_result );
    ret =  ((unsigned long)tab_result [offset_in_byte]) & 0xFF;
    return ret;
}

unsigned long read_indirect_10(unsigned long addr, int offset_in_byte){
    unsigned long ret =0;
    unsigned char tab_result[8];
    read_indirect( addr, tab_result );
    ret =  ((unsigned long)tab_result [offset_in_byte]) & 0xFF;
    ret += (((unsigned long)tab_result[offset_in_byte+1]) & 0x3) << 8;
    return ret;
}


struct mmdspAddrBase{
    unsigned long ExtBase_24_1 ; 
    unsigned long ExtBase_24_2 ; 
    unsigned long ExtBase_24_3 ; 
    unsigned long ExtBase_24_4 ; 
    unsigned long ExtBase_16_1 ; 
    unsigned long ExtBase_16_2 ; 
    unsigned long ExtBase_16_3 ; 
    unsigned long ExtBase_16_4 ; 

    unsigned long ExtTop_24_1  ; 
    unsigned long ExtTop_24_2  ; 
    unsigned long ExtTop_24_3  ; 
    unsigned long ExtTop_24_4  ; 
    unsigned long ExtTop_16_1  ; 
    unsigned long ExtTop_16_2  ; 
    unsigned long ExtTop_16_3  ; 
    unsigned long ExtTop_16_4  ; 

    unsigned long MmdspBase_24_1 ;
    unsigned long MmdspBase_24_2 ; 
    unsigned long MmdspBase_24_3 ; 
    unsigned long MmdspBase_24_4 ; 
    unsigned long MmdspBase_16_1 ;
    unsigned long MmdspBase_16_2 ; 
    unsigned long MmdspBase_16_3 ; 
    unsigned long MmdspBase_16_4 ; 
};
typedef struct mmdspAddrBase mmdspAddrBase;

static inline void fillBases(mmdspAddrBase * bases){
    bases->ExtBase_24_1 = read_indirect_lsb32(0x0A); 
    bases->ExtBase_24_2 = read_indirect_lsb32(0x0C); 
    bases->ExtBase_24_3 = read_indirect_lsb32(0x1B); 
    bases->ExtBase_24_4 = read_indirect_lsb32(0x1C); 
    bases->ExtBase_16_1 = read_indirect_msb32(0x0A); 
    bases->ExtBase_16_2 = read_indirect_msb32(0x0C); 
    bases->ExtBase_16_3 = read_indirect_msb32(0x1B); 
    bases->ExtBase_16_4 = read_indirect_msb32(0x1C); 

    bases->ExtTop_24_1  = read_indirect_lsb32(0x14); 
    bases->ExtTop_24_2  = read_indirect_lsb32(0x15); 
    bases->ExtTop_24_3  = read_indirect_lsb32(0x1D); 
    bases->ExtTop_24_4  = read_indirect_lsb32(0x1E); 
    bases->ExtTop_16_1  = read_indirect_msb32(0x14); 
    bases->ExtTop_16_2  = read_indirect_msb32(0x15); 
    bases->ExtTop_16_3  = read_indirect_msb32(0x1D); 
    bases->ExtTop_16_4  = read_indirect_msb32(0x1E); 

    bases->MmdspBase_24_1 = 0x010000UL; 
    bases->MmdspBase_24_2 = read_indirect_8(0x0F, 0) << 16; 
    bases->MmdspBase_24_3 = read_indirect_8(0x0F, 1) << 16; 
    bases->MmdspBase_24_4 = read_indirect_8(0x0F, 2) << 16; 
    bases->MmdspBase_16_1 = 0x800000UL; 
    bases->MmdspBase_16_2 = read_indirect_8(0x0F, 4) << 16; 
    bases->MmdspBase_16_3 = read_indirect_8(0x0F, 5) << 16; 
    bases->MmdspBase_16_4 = read_indirect_8(0x0F, 6) << 16; 
}

static inline void * internal_getMmdspAddr16(unsigned long ArmPhyAddr, mmdspAddrBase * bases){
    if((ArmPhyAddr >= bases->ExtBase_16_1) && (ArmPhyAddr < bases->ExtTop_16_1)){
        return (void *)(((ArmPhyAddr - bases->ExtBase_16_1) >> 1) + bases->MmdspBase_16_1);
    }
    else if((ArmPhyAddr >= bases->ExtBase_16_2) && (ArmPhyAddr < bases->ExtTop_16_2)){
        return (void *)(((ArmPhyAddr - bases->ExtBase_16_2) >> 1) + bases->MmdspBase_16_2);
    }
    else if((ArmPhyAddr >= bases->ExtBase_16_3) && (ArmPhyAddr < bases->ExtTop_16_3)){
        return (void *)(((ArmPhyAddr - bases->ExtBase_16_3) >> 1) + bases->MmdspBase_16_3);
    }
    else if((ArmPhyAddr >= bases->ExtBase_16_4) && (ArmPhyAddr < bases->ExtTop_16_4)){
        return (void *)(((ArmPhyAddr - bases->ExtBase_16_4) >> 1) + bases->MmdspBase_16_4);
    }
    return NULL;
}

static inline void * internal_getMmdspAddr24(unsigned long ArmPhyAddr, mmdspAddrBase * bases){
    if((ArmPhyAddr >= bases->ExtBase_24_1) && (ArmPhyAddr < bases->ExtTop_24_1)){
        return (void *)(((ArmPhyAddr - bases->ExtBase_24_1) >> 2) + bases->MmdspBase_24_1);
    }
    else if((ArmPhyAddr >= bases->ExtBase_24_2) && (ArmPhyAddr < bases->ExtTop_24_2)){
        return (void *)(((ArmPhyAddr - bases->ExtBase_24_2) >> 2) + bases->MmdspBase_24_2);
    }
    else if((ArmPhyAddr >= bases->ExtBase_24_3) && (ArmPhyAddr < bases->ExtTop_24_3)){
        return (void *)(((ArmPhyAddr - bases->ExtBase_24_3) >> 2) + bases->MmdspBase_24_3);
    }
    else if((ArmPhyAddr >= bases->ExtBase_24_4) && (ArmPhyAddr < bases->ExtTop_24_4)){
        return (void *)(((ArmPhyAddr - bases->ExtBase_24_4) >> 2) + bases->MmdspBase_24_4);
    }
    return NULL;
}

void * getMmdspAddr16(unsigned long ArmPhyAddr){
    mmdspAddrBase bases;
    fillBases(&bases);
    return internal_getMmdspAddr16(ArmPhyAddr, &bases);
}

void * getMmdspAddr24(unsigned long ArmPhyAddr){
    mmdspAddrBase bases;
    fillBases(&bases);
    return internal_getMmdspAddr24(ArmPhyAddr, &bases);
}

void * getMmdspAddr(unsigned long ArmPhyAddr){
    void * ret ;
    mmdspAddrBase bases;
    fillBases(&bases);
    ret = internal_getMmdspAddr24(ArmPhyAddr, &bases);
    if(ret){
        return ret;
    }
    return internal_getMmdspAddr16(ArmPhyAddr, &bases);
}

unsigned long getArmPhysicalAddr(unsigned int * dspAddr){
    unsigned long dspAddrTmp = (unsigned long) dspAddr;

    unsigned long ExtBase_24_1 = read_indirect_lsb32(0x0A); 
    unsigned long ExtBase_24_2 = read_indirect_lsb32(0x0C); 
    unsigned long ExtBase_24_3 = read_indirect_lsb32(0x1B); 
    unsigned long ExtBase_24_4 = read_indirect_lsb32(0x1C); 
    unsigned long ExtBase_16_1 = read_indirect_msb32(0x0A); 
    unsigned long ExtBase_16_2 = read_indirect_msb32(0x0C); 
    unsigned long ExtBase_16_3 = read_indirect_msb32(0x1B); 
    unsigned long ExtBase_16_4 = read_indirect_msb32(0x1C); 
    
    unsigned long MmdspBase_24_1 = 0x010000UL; 
    unsigned long MmdspBase_24_2 = read_indirect_8(0x0F, 0) << 16; 
    unsigned long MmdspBase_24_3 = read_indirect_8(0x0F, 1) << 16; 
    unsigned long MmdspBase_24_4 = read_indirect_8(0x0F, 2) << 16; 
    unsigned long MmdspBase_16_1 = 0x800000UL; 
    unsigned long MmdspBase_16_2 = read_indirect_8(0x0F, 4) << 16; 
    unsigned long MmdspBase_16_3 = read_indirect_8(0x0F, 5) << 16; 
    unsigned long MmdspBase_16_4 = read_indirect_8(0x0F, 6) << 16; 


    if(dspAddrTmp >= MmdspBase_16_4){
        return ((dspAddrTmp - MmdspBase_16_4) * 2) + ExtBase_16_4;
    }
    else if(dspAddrTmp >= MmdspBase_16_3){
        return ((dspAddrTmp - MmdspBase_16_3) * 2) + ExtBase_16_3;
    }
    else if(dspAddrTmp >= MmdspBase_16_2){
        return ((dspAddrTmp - MmdspBase_16_2) * 2) + ExtBase_16_2;
    }
    else if(dspAddrTmp >= MmdspBase_16_1){
        return ((dspAddrTmp - MmdspBase_16_1) * 2) + ExtBase_16_1;
    }
    else if(dspAddrTmp >= MmdspBase_24_4){
        return ((dspAddrTmp - MmdspBase_24_4) * 4) + ExtBase_24_4;
    }
    else if(dspAddrTmp >= MmdspBase_24_3){
        return ((dspAddrTmp - MmdspBase_24_3) * 4) + ExtBase_24_3;
    }
    else if(dspAddrTmp >= MmdspBase_24_2){
        return ((dspAddrTmp - MmdspBase_24_2) * 4) + ExtBase_24_2;
    }
    else if(dspAddrTmp >= MmdspBase_24_1){
        return ((dspAddrTmp - MmdspBase_24_1) * 4) + ExtBase_24_1;
    }

    return 0;  
}

