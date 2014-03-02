/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   prcmu_registers.h
 * \brief  
 * \author ST-Ericsson
 */
/*****************************************************************************/
#ifndef  _PRCMU_REGISTERS_H_
#define  _PRCMU_REGISTERS_H_

#define PRCMU_FW_IT 15

#define EXT_MMIO_BASE   0xF80000UL

#define PRCMU_DATA_ARM_OFFSET  0xB8000UL
#define PRCMU_ALT_DATA_ARM_OFFSET  0xBA000UL

#define PRCMU_MB7_HEADER_ADDR_OLD ( EXT_MMIO_BASE + ( (PRCMU_DATA_ARM_OFFSET + 0xFEEUL) >> 1) )
#define PRCMU_MB7_HEADER_ADDR     ( EXT_MMIO_BASE + ( (PRCMU_DATA_ARM_OFFSET + 0xFF6UL) >> 1) )
#define PRCMU_MB7_DATA_ADDR       ( EXT_MMIO_BASE + ( (PRCMU_DATA_ARM_OFFSET + 0xE3CUL) >> 1) )
#define PRCMU_MB7_DATA_MSB_ADDR   ( EXT_MMIO_BASE + ( (PRCMU_DATA_ARM_OFFSET + 0xE3EUL) >> 1) )

#define PRCMU_MB_GEN_IT           ( EXT_MMIO_BASE + ( 0x57100UL >> 1) )
#define PRCMU_SIAMMDSP_IT         ( EXT_MMIO_BASE + ( 0x57248UL >> 1) )
#define PRCMU_SIAMMDSP_IT_CLR     ( EXT_MMIO_BASE + ( 0x5724CUL >> 1) )

static volatile __EXTMMIO unsigned int   * PRCMUFW_Mb7_header_old  = (volatile __EXTMMIO unsigned int *)  PRCMU_MB7_HEADER_ADDR_OLD;
static volatile __EXTMMIO unsigned int   * PRCMUFW_Mb7_header      = (volatile __EXTMMIO unsigned int *)  PRCMU_MB7_HEADER_ADDR;
static volatile __EXTMMIO unsigned long  * PRCMUFW_Mb7_data        = (volatile __EXTMMIO unsigned long *) PRCMU_MB7_DATA_ADDR;
static volatile __EXTMMIO unsigned int   * PRCMUFW_Mb7_data_msb    = (volatile __EXTMMIO unsigned int *)  PRCMU_MB7_DATA_MSB_ADDR;
static volatile __EXTMMIO unsigned long  * PRCMUFW_generate_It19   = (volatile __EXTMMIO unsigned long *) PRCMU_MB_GEN_IT;
static volatile __EXTMMIO unsigned int   * PRCMUFW_generate_sia_It = (volatile __EXTMMIO unsigned int *)  PRCMU_SIAMMDSP_IT;
static volatile __EXTMMIO unsigned int   * PRCMUFW_clear_sia_It    = (volatile __EXTMMIO unsigned int *)  PRCMU_SIAMMDSP_IT_CLR;

static inline void load_prcmu_pointers(int target) {

    switch (target) {
        case 0:
            break;

        default:
            PRCMUFW_Mb7_header_old  = (volatile __EXTMMIO unsigned int *)  ( EXT_MMIO_BASE + ( (PRCMU_ALT_DATA_ARM_OFFSET + 0xFEEUL) >> 1) );
            PRCMUFW_Mb7_header      = (volatile __EXTMMIO unsigned int *)  ( EXT_MMIO_BASE + ( (PRCMU_ALT_DATA_ARM_OFFSET + 0xFF6UL) >> 1) );
            PRCMUFW_Mb7_data        = (volatile __EXTMMIO unsigned long *) ( EXT_MMIO_BASE + ( (PRCMU_ALT_DATA_ARM_OFFSET + 0xE3CUL) >> 1) );
            PRCMUFW_Mb7_data_msb    = (volatile __EXTMMIO unsigned int *)  ( EXT_MMIO_BASE + ( (PRCMU_ALT_DATA_ARM_OFFSET + 0xE3EUL) >> 1) );
            break;
    }
}

static inline int readAb8500RegThroughPrcmu(unsigned int addr, unsigned char * value){
    t_uint16 i2cCmd[2]={0,0};
    int error = 0x0;

    i2cCmd[0] |= 1; // read access [0]
    i2cCmd[0] |= ((addr&0xFF00)>>8) << 1; // slave addr [7-1]
    // already = 0: i2cCmd |= 0 << 8 ; // Tsize [9-8]
    // already = 0: i2cCmd |= 0 << 10; // HWGCEN [10]
    // already = 0: i2cCmd |= 0 << 11; // STOPEN [11]
    i2cCmd[1] |=  (addr&0xFF) ;      // reg addr [23-16]
    // already = 0: i2cCmd |= 0 << 24; // reg data [24-31]

    //FIXME remove old header
    *PRCMUFW_Mb7_header_old     = 0x000200;  //FIXME bug this write for sia and sva
    *PRCMUFW_Mb7_header         = 0x2;  //FIXME bug this write for sia and sva
    
    *PRCMUFW_Mb7_data      = i2cCmd[0] ;
    *PRCMUFW_Mb7_data_msb  = i2cCmd[1] ;

    *PRCMUFW_generate_It19 = 1UL << 7;

    //wait until i2c write ok = 0x2  
    while (((*PRCMUFW_Mb7_data & 0x00FF00)>>8) != 0x2){
        //check time out
        if (((*PRCMUFW_Mb7_data & 0x00FF00)>>8) == 0x11){
            error = 1;
            return error;
        }
    }

    *value =  (char) ((*PRCMUFW_Mb7_data_msb & 0xFF00)>>8);
    return error;
}

static inline int writeAb8500RegThroughPrcmu(unsigned int addr, unsigned char value){
    t_uint16 i2cCmd[2]={0,0};
    int error = 0x0;    

    i2cCmd[0] |= 0; // write access [0]
    i2cCmd[0]  |= ((addr&0xFF00)>>8) << 1; // slave addr [7-1]
    // already = 0: i2cCmd |= 0 << 8 ; // Tsize [9-8]
    // already = 0: i2cCmd |= 0 << 10; // HWGCEN [10]
    // commented to set STOPEN to 0 
    // i2cCmd |= 1 << 11;                 // STOPEN [11]
    i2cCmd[1]  |=  (addr&0xFF);      // reg addr [23-16]
    i2cCmd[1]  |= value << 8; // reg data [24-31]

    //FIXME remove old header    
    *PRCMUFW_Mb7_header_old     = 0x000200;  //FIXME bug this write for sia and sva
    *PRCMUFW_Mb7_header         = 0x2;  //FIXME bug this write for sia and sva
    *PRCMUFW_Mb7_data           = i2cCmd[0] ;
    *PRCMUFW_Mb7_data_msb       = i2cCmd[1] ;

    *PRCMUFW_generate_It19      = 1UL << 7;

    //wait until i2c write ok = 0x1
    while (((*PRCMUFW_Mb7_data & 0x0000FF00)>>8) != 0x1){
        //check time out
        if (((*PRCMUFW_Mb7_data & 0x00FF00)>>8) == 0x11){
            error = 1;
            return error;
        }
    }
    return error;
}

//Ask PRCMU FW to (un)mask ITfifo
static inline void ItFiFoConfigurationThroughPrcmu(int enableIt){
    int value = 0;
    if(enableIt){
        value = 1;

        // Clear possible Pending Interrupt TODO: I think that this is useless
        * PRCMUFW_clear_sia_It = 1;
    }

    *PRCMUFW_Mb7_header         = 0x000001;
    *PRCMUFW_Mb7_data_msb       = value ;
    *PRCMUFW_generate_It19      |= 1UL << 7;
}
#endif   // _PRCMU_REGISTERS_H_
