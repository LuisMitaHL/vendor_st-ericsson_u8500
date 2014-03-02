/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */


#ifndef _BPU_API_H_
#define _BPU_API_H_

/*****************************************************************************/
/*
 * Includes							       
 */
/*****************************************************************************/
#include "t1xhv_types.h"
#include "macros.h"                /* ASSERT */
#include "t1xhv_hamac_xbus_regs.h" /* register definition */
#include <stwdsp.h>
#include "t1xhv_retarget.h" 

/*****************************************************************************/
/*
 * Define
 */
/*****************************************************************************/

/*****************************************************************************/
/*
 * Variables							       
 */
/*****************************************************************************/
/* This is new in 8820, HW blocks memory mapping is no more handled
   through MMDSP Tools, now we get DMA_BASE from t1xhv_hamac_xbus_regs.h */
/* Two variants, with and without XIORDY */
#define HW_BPU(a) (*(volatile MMIO t_uint16 *)(BPU_BASE+a))
#define HW_BPU_WAIT(a) (*(volatile __XIO t_uint16 *)(BPU_BASE+a))


/**
 * Range between channel 1/2 and 2/3 register bank, used by BPU_WRITE_xxx
 * to compute register address from channel index
 */
#define BPU_CTRL_RANGE 0x30	

/*
 * Size of length and value when command is packed 
 */
#define BPU_SZE_LENGTH 4
#define BPU_SZE_VALUE  12


/**
 * Defines							       
 */

/*****************************************************************************/
/*
 * Types
 */
/*****************************************************************************/
typedef enum
{
    BPU_CHANNEL_HDR1  = 0, /**< \brief  channel 1 */
    BPU_CHANNEL_HDR2  = 1, /**< \brief  channel 2 */
    BPU_CHANNEL_DATA  = 2  /**< \brief  channel 3 */
} t_bpu_channel;

typedef enum
{
    JPEG_MODE_DISABLE = 0, /**< \brief No automatic stuffing of 00 after FF */
    JPEG_MODE_ENABLE  = 1, /**< \brief Automatic stuffing of 00 after FF    */
    H264_MODE_DISABLE = 2, /**< \brief No Automatic stuffing of 03  */
    H264_MODE_ENABLE  = 3  /**< \brief Automatic stuffing of 03  */
} t_stuff_mode_enable;


/*
 * Public functions
 */
/*****************************************************************************/
/**
 * \brief   Configure the stuff_0 bit for BPU
 * \author  Serge Backert
 *
 */
/*****************************************************************************/
#pragma inline
static void BPU_STUFF_MODE(t_stuff_mode_enable mode)
{
    HW_BPU_WAIT(BPU_CFG) = (mode);
}

/*****************************************************************************/
/**
 * \brief 	Read the ff_msb status bit
 * \author 	Serge Backert
 * \return 	status
 * 
 */
/*****************************************************************************/
#pragma inline
static t_uint16 BPU_READ_STUFF_STATUS()
{
    return(HW_BPU_WAIT(BPU_GTS));
}

/*****************************************************************************/
/**
 * \brief 	Reset the compressed data FIFO, packer and bit count for channel 1
 * \author 	Serge Backert
 *
 */
/*****************************************************************************/
#pragma inline
static void BPU_INIT_HDR1()
{
    HW_BPU_WAIT(BPU_RST1) = 0;
}
/*****************************************************************************/
/**
 * \brief 	Reset the SRAM controller and channel 2 and 3 packer and bit count
 * \author 	Serge Backert
 *
 */
/*****************************************************************************/
#pragma inline
static void BPU_INIT_HDR2_DATA()
{
    HW_BPU_WAIT(BPU_RST23456789)     = 0;
}

/*****************************************************************************/
/**
 * \brief   Reset the compressed data FIFO, packer and bit count for channel 1
 *          Reset the SRAM controller and channel 2 and 3 packer and bit count
 * \author  Serge Backert
 *
 */
/*****************************************************************************/
#pragma inline
static void BPU_INIT()
{
    BPU_INIT_HDR1();
    BPU_INIT_HDR2_DATA();
}

/*****************************************************************************/
/** 
 * \brief   Get the number of bits written into channel 1 since the last
 *          initialization	
 * \author  Serge Backert
 * \return  bit number
 */
/*****************************************************************************/
#pragma inline
static t_uint32 BPU_GET_BIT_NUMBER_HDR1()
{
    return((((t_uint32)HW_BPU_WAIT(BPU_GTBh))<<16) | (t_uint32)HW_BPU_WAIT(BPU_GTB1));
}

/*****************************************************************************/
/** 
 * \brief   Get the number of bits written into channel 2 since the last
 *          initialization	
 * \author  Serge Backert
 * \return  bit number
 */
/*****************************************************************************/
#pragma inline
static t_uint16 BPU_GET_BIT_NUMBER_HDR2()
{
    return((t_uint16)HW_BPU_WAIT(BPU_GTB2));
}

/*****************************************************************************/
/** 
 * \brief   Get the number of bits written into channel 3 since the last
 *          initialization	
 * \author  Serge Backert
 * \return  bit number
 */
/*****************************************************************************/
#pragma inline
static t_uint16 BPU_GET_BIT_NUMBER_DATA()
{
    return((t_uint16)HW_BPU_WAIT(BPU_GTB3));
}


/*****************************************************************************/
/** 
 * \brief   Get the number of bits written into BPU since the last 
 *          initialization	
 * \author  Serge Backert
 * \return  bit number
 */
/*****************************************************************************/
#pragma inline
static t_uint32 BPU_GET_BIT_NUMBER()
{
    return(  BPU_GET_BIT_NUMBER_HDR1() 
           + (t_uint32)BPU_GET_BIT_NUMBER_HDR2()
           + (t_uint32)BPU_GET_BIT_NUMBER_DATA()
          );
}


/*****************************************************************************/
/**
 * \brief   Write value on length bits in channel 1
 * \author  Serge Backert
 * \param   channel
 * \param   length
 * \param   value
 *
 * When called by m4e_vlc_mp4_dc_coeff(), the channel is not known by the
 * caller, it may be either be channel 1, 2 or 3. That is the reason why this
 * parameter has to be passed to the BPU_WRITE_xxx function instead of
 * defining BPU_WRITE_xxx_HDR1/HDR2/DATA()
 *  
 */
/*****************************************************************************/
#pragma inline
static void BPU_WRITE_CHANNEL_16(t_bpu_channel channel,t_uint16 length,t_uint16 value)
{
    ASSERT(length<16);
    switch(channel) {
        /*channel 1*/
    case 0:
        HW_BPU_WAIT(BPU_WR1 + length) = value;
        break;
      /*channel 2*/
    case 1:
        HW_BPU_WAIT(BPU_WR2 + length) = value;
        break;
   /*channel 3*/
    case 2:
        HW_BPU_WAIT(BPU_WR3 + length) = value;
        break;
        
    }

}
/*****************************************************************************/
/**
 * \brief   Write value on length bits in channel 1
 * \author  Sonia Marrec
 * \param   length
 * \param   value
 *  
 */
/*****************************************************************************/
#pragma inline
static void BPU_WRITE_CHANNEL1_16(t_uint16 length,t_uint16 value)
{
    ASSERT(length<16);
    HW_BPU_WAIT(BPU_WR1+1 + length-1) = value;

}
/*****************************************************************************/
/**
 * \brief   Write value on length bits in channel 2
 * \author  Rebecca Richard
 * \param   length
 * \param   value
 *  
 */
/*****************************************************************************/
#pragma inline
static void BPU_WRITE_CHANNEL2_16(t_uint16 length,t_uint16 value)
{
    ASSERT(length<16);
    HW_BPU_WAIT(BPU_CTRL_RANGE +BPU_WR1 + length) = value;

}
/*****************************************************************************/
/**
 * \brief   Write value on length bits in channel 1
 * \author  Sonia Marrec
 * \param   length
 * \param   value
 *  
 */
/*****************************************************************************/
#pragma inline
static void BPU_WRITE_CHANNEL1P(t_uint16 nbits,t_sint16 value)
{
  HW_BPU_WAIT(BPU_WR1) = winsert(nbits,0x40C,value);
}
/*****************************************************************************/
/**
 * \brief   Write value on length bits in channel
 * \author  Serge Backert
 * \param   channel
 * \param   length
 * \param   value
 *  
 */
/*****************************************************************************/
#pragma inline
static void BPU_WRITE_CHANNEL_32(t_bpu_channel channel,t_uint16 length,t_uint32 value)
{
    ASSERT(length<32);

    switch(channel) {
        /*channel 1*/
    case 0:
        /** Write MSB */
        HW_BPU_WAIT(BPU_WR1 + length - 16) = MS_SHORT(value);
        /** Write LSB */
        HW_BPU_WAIT(BPU_WR1 + 16)          = LS_SHORT(value);
        break;
        /*channel 2*/
    case 1:
        /** Write MSB */
        HW_BPU_WAIT(BPU_WR2 + length - 16) = MS_SHORT(value);
        /** Write LSB */
        HW_BPU_WAIT(BPU_WR2 + 16)          = LS_SHORT(value);        
        break;
        /*channel 3*/
    case 2:
        /** Write MSB */
        HW_BPU_WAIT(BPU_WR3 + length - 16) = MS_SHORT(value);
        /** Write LSB */
        HW_BPU_WAIT(BPU_WR3 + 16)          = LS_SHORT(value);      
        break;  
    }
}


/*****************************************************************************/
/**
 * \brief   Move n bits from channel 2 into channel 1
 * \author  Serge Backert
 * \param   length number of bits to move
 *  
 * The current implementation is limited to 8192 bit max
 */
/*****************************************************************************/
#pragma inline
static void BPU_MOVE_HDR2(t_uint16 length)
{
    HW_BPU_WAIT(BPU_MOV2) = length;
}

/*****************************************************************************/
/**
 * \brief   Move n bits from channel 3 into channel 1
 * \author  Serge Backert
 * \param   length number of bits to move
 *  
 * The current implementation is limited to 8192 bit max
 */
/*****************************************************************************/
#pragma inline
static void BPU_MOVE_DATA(t_uint16 length)
{
    ASSERT(length<8192);
    HW_BPU_WAIT(BPU_MOV3) = length;
}

/*****************************************************************************/
/**
 * \brief   Write value at position length in packer 1
 * \author  Serge Backert
 * \param   length
 * \param   value
 *  
 */
/*****************************************************************************/
#pragma inline
static void BPU_FLUSH_HDR1(t_uint16 length,t_uint16 value)
{
    HW_BPU_WAIT(BPU_FLU1 + length-1) = value;
}

/*****************************************************************************/
/**
 * \brief   Write value at position length in packer 2
 * \author  Serge Backert
 * \param   length
 * \param   value
 *  
 */
/*****************************************************************************/
#pragma inline
static void BPU_FLUSH_HDR2(t_uint16 length,t_uint16 value)
{
    HW_BPU_WAIT(BPU_FLU2 + length-1) = value;
}

/*****************************************************************************/
/**
 * \brief   Write value at position length in packer 3
 * \author  Serge Backert
 * \param   length
 * \param   value
 *  
 */
/*****************************************************************************/
#pragma inline
static void BPU_FLUSH_DATA(t_uint16 length,t_uint16 value)
{
    HW_BPU_WAIT(BPU_FLU3 + length-1) = value;
}


/*****************************************************************************/
/**
 * \brief   Define static vs dynamic behaviour of BPU 
 * \author  Maurizio COlombo
 * \param   0 = from FIFO, 1 = from REG
 *  
 */
/*****************************************************************************/
#pragma inline
static void BPU_SET_REG_DEF(t_uint16 def)
{
    HW_BPU_WAIT(BPU_REG_DEF) = def;
}


/*****************************************************************************/
/**
 * \brief   Program command register of BPU 
 * \author  Maurizio COlombo
 * \param   define the channel to pack in data coming from SDIF 
 *  
 */
/*****************************************************************************/
#pragma inline
static void BPU_SET_REG_CMD(t_uint16 cmd)
{
    HW_BPU_WAIT(BPU_REG_CMD) = cmd;
}


/*****************************************************************************/
/**
 * \brief   Program command FIFO of BPU 
 * \author  Maurizio Colombo
 * \param   define the channel to pack in data coming from SDIF 
 * \param   repeat 
 */
/*****************************************************************************/
#pragma inline
static void BPU_SET_FIFO_CMD(t_uint16 cmd, t_uint16 repeat)
{
    HW_BPU_WAIT(BPU_FIFO_CMD) = cmd | ((repeat-1)<<4);
}

/*****************************************************************************/
/**
 * \brief   Get status of BPU 
 * \author  Maurizio Colombo
 * \return  1 if idle, 0 if data is being packed 
 */
/*****************************************************************************/
#pragma inline
static t_uint16 BPU_GET_STATUS()
{
    return HW_BPU_WAIT(BPU_SDIF_STATUS);
}

/*****************************************************************************/
/**
 * \brief   Flush the several channels of BPU for H264 encoder
 * \brief   Macro dedicated to BPU for H264 encoder
 * \author  Rebecca RICHARD
 * \param   superlevel 
 * \param   CBP 
 *  
 */
/*****************************************************************************/
#pragma inline
static void BPU_FLUSH_H264(t_uint16 superlevel, t_uint16 CBP)
{
    HW_BPU_WAIT(BPU_FLUSH_H264_NEW) =
        (((CBP & 0x3F)              )
        |(superlevel            << 6));

}

/*****************************************************************************/
/**
 * \brief   Request BPU MB size
 * \brief   Macro dedicated to BPU for H264 encoder
 * \author  Thomas Costis
 * \param   superlevel 
 * \param   CBP 
 *  
 */
/*****************************************************************************/
#pragma inline
static void BPU_GET_MB_SIZE_CMD(t_uint16 superlevel, t_uint16 CBP)
{
    HW_BPU_WAIT(BPU_GET_MB_SIZE) = 
		( ((CBP & 0x3F)              ) | 
		  (superlevel            << 6));
}




t_uint16 bpu_flush_vp(t_uint16 bit_count2,t_uint16 bit_count3, t_ahb_address start_addr, t_uint16 offset);

t_uint16 bpu_flush_h264(t_uint16 bit_count2,t_uint16 bit_count3);

t_uint16 bpu_flush_frame(t_uint16 previous_offset);

void bpu_align_offset(t_uint16 offset_in_bits, t_uint32 ahb_address);

#endif /* _BPU_API_H_ */

