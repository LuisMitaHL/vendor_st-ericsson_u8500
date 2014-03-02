/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
/* \brief   DMA API/macro for SIA
*/
#ifndef _DMA_SIA_API_H_
#define _DMA_SIA_API_H_

/*
 * Includes
 */
#include "crm_mmdsp.h" /* register definition */
#include "dma_mmdsp.h" /* register definition */
#include "pictor_mmdsp.h" /* register definition */
#include "proj_mmdsp.h" /* register definition */
#include "sia_mmdsp.h" /* register definition */
#include "stbp_mmdsp.h" /* register definition */
#include <stwdsp.h>

/* This is new in 8820, HW blocks memory mapping is no more handled
   through MMDSP Tools, now we get IPP_BASE from t1xhv_hamac_xbus_regs.h */
#define HW_DMA_WAIT(a) (*(volatile __XIO t_uint16 *)a)

/* DMA processes mask, used in ENR/DIR/IMR */
#define IRP03_L_W         (0x1UL << 0)  /*< IRP Pipe0/3 Luma write */
#define IRP03_C_W         (0x1UL << 1)  /*< IRP Pipe0/3 Chroma write */
#define IRP03_C1_W        (0x1UL << 2)  /*< IRP Pipe0/3 Chroma1 write */
#define IRP03_C2_W        (0x1UL << 3)  /*< IRP Pipe0/3 Chroma2 write */
#define IRP03_GCF_L_W     (0x1UL << 4)  /*< IRP Pipe0/3 Grab Cache Luma write */
#define IRP03_GCF_C_W     (0x1UL << 5)  /*< IRP Pipe0/3 Grab Cache Chroma write */
#define IRP03_GCF_L_R     (0x1UL << 6)  /*< IRP Pipe0/3 Grab Cache Luma read */
#define IRP03_GCF_C_R     (0x1UL << 7)  /*< IRP Pipe0/3 Grab Cache Chroma write */
#define IRP03_VCP_W       (0x1UL << 8)  /*< IRP Pipe0/3 Vertical Accumulation Pixel write */
#define IRP03_HCP_W       (0x1UL << 9)  /*< IRP Pipe0/3 Horizontal Accumulation Pixel write */
#define IRP03_HCP_R       (0x1UL << 10) /*< IRP Pipe0/3 Horizontal Accumulation Pixel read */
#define IRP03_PRA_W       (0x1UL << 11) /*< IRP Pipe0/3 Pixel (raw data) Raster write */

#define IRP0_PRA_R       (0x1UL << 12) /*< IRP Pipe0 Pixel (raw data) Raster read */
#define IRP1_PRA_W       (0x1UL << 13) /*< IRP Pipe1 Pixel (raw data) Raster write */
#define IRP2_PRA_W       (0x1UL << 14) /*< IRP Pipe2 Pixel (raw data) Raster write */
#define IRP3_JPG_W       (0x1UL << 15) /*< IRP Pipe3 JPG write */

#define DMA_ROTATION        0x00
#define DMA_ROTATION_CW     0x01
#define DMA_ROTATION_CCW    0x03

/*****************************************************************************/
/** \brief  Read a register from DMA - 16 bits
 *  \param  addr    Address to read (offset)
 *  \return Read value
 */
/*****************************************************************************/
#define DMA_GET_REG(_Reg_) DMA_GET_REG_fct(DMA_##_Reg_##_OFFSET)

#pragma inline
static t_uint16 DMA_GET_REG_fct(t_uint16 addr)
{
    return HW_DMA_WAIT(addr);
}
/*****************************************************************************/
/** \brief  Read a register from DMA - 32 bits
 *  \param  addr_lsb    Address to read (offset) LSB
 *  \param  addr_msb    Address to read (offset) MSB
 *  \return Read value
 */
/*****************************************************************************/
#define DMA_GET_REG_32(_Reg_) DMA_GET_REG_32_fct(DMA_##_Reg_##_L_OFFSET,DMA_##_Reg_##_H_OFFSET)
#pragma inline
static t_uint32 DMA_GET_REG_32_fct(t_uint16 addr_lsb,
                               t_uint16 addr_msb)
{
    t_uint32 tmp =(t_uint32)HW_DMA_WAIT(addr_lsb);
    t_uint32 tmp2 =(t_uint32)((t_uint32)HW_DMA_WAIT(addr_msb) << 16L);

    return (t_uint32)((t_uint32)tmp | (t_uint32)tmp2);
}
#pragma inline

/*****************************************************************************/
/** \brief  Write a register from DMA - 16 bits
 *  \param  addr    Address to write (offset)
 *  \param  value   Value to write
 */
/*****************************************************************************/
#define DMA_SET_REG(_Reg_, _Val_) DMA_SET_REG_fct(DMA_##_Reg_##_OFFSET,_Val_)
#pragma inline
static void DMA_SET_REG_fct(t_uint16 addr, t_uint16 value)
{
    HW_DMA_WAIT(addr) = value;
}
/*****************************************************************************/
/** \brief  Write a register from DMA - 32 bits
 *  \param  addr_lsb    Address to read (offset) LSB
 *  \param  addr_msb    Address to read (offset) MSB
 *  \param  value   Value to write
 */
/*****************************************************************************/
#define DMA_SET_REG_32(_Reg_, _Val_) DMA_SET_REG_32_fct(DMA_##_Reg_##_L_OFFSET,DMA_##_Reg_##_H_OFFSET,_Val_)
#pragma inline
static void DMA_SET_REG_32_fct(t_uint16 addr_lsb,
                           t_uint16 addr_msb,
                           t_uint32 value)
{
    HW_DMA_WAIT(addr_lsb) = wextract_l(value);
    HW_DMA_WAIT(addr_msb) = wextract_h(value);
}

/*****************************************************************************/
/** \brief  Write a register from DMA - 32 bits
 *  \param  addr_lsb    Address to read (offset) LSB
 *  \param  addr_msb    Address to read (offset) MSB
 *  \param  value   Value1 to write MSB
  *  \param  value   Value2 to write LSB
 */
/*****************************************************************************/
#pragma inline
static void DMA_SET_REG_32_fct1(t_uint16 addr_lsb,
                           t_uint16 addr_msb,
                           t_uint16 value1, t_uint16 value2)
{
    HW_DMA_WAIT(addr_lsb) = value1;
    HW_DMA_WAIT(addr_msb) = value2;
}


/*****************************************************************************/
/**
 * \brief DMA reset process
 * \param process_name: process to reset
 * \return nothing
 *
 */
/*****************************************************************************/
#pragma inline
/*static   void DMA_SOFT_RESET (t_uint32 process_name)
{
    static volatile int nop;
    t_uint32 process_rst;
    t_uint16 i;

    do{
        process_rst = DMA_GET_REG_32(DMA_SOFT_RST_L, DMA_SOFT_RST_H);
        DMA_SET_REG_32(DMA_SOFT_RST_L, DMA_SOFT_RST_H, (process_rst| process_name) );
        DMA_SET_REG_32(DMA_SOFT_RST_L, DMA_SOFT_RST_H, process_rst);
    for (i=0; i<200; i++) nop;
    }
    while ( (DMA_GET_REG_32(DMA_ENR_L, DMA_ENR_H)&process_name) != 0 );
}
*/

#endif /* _DMA_SIA_API_H_ */

