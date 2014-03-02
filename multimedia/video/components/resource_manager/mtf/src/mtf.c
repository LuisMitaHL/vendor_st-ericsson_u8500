/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/* Include automatically built nmf file */
#include "../inc/local_nmf_mtf.h"

#include "mtf_api.h"
#include "mtf_hwloop.h"
/*------------------------------------------------------------------------
 * Global Variables
 *----------------------------------------------------------------------*/

/*------------------------------------------------------------------------
 * Private functions prototype
 *----------------------------------------------------------------------*/

/*------------------------------------------------------------------------
 * Methodes of component interface
 *----------------------------------------------------------------------*/


/*****************************************************************************/
/**
 * \brief   readThroughMTF
 * \brief 	Read data from firmware from memory (internal/external depending on
 * 		    bit0 of srcDataAddress i.e. 0:internal 1:external)
 * \author 	SVA FW Team (source is extracted from old scheduler)
 * \param   srcDataAddress Address location (no endianess to be handled as this
 * 			param is unmarshalled by NMF engine).
 * \param 	Size Data size to be read
 * \param   dstDataAddress Pointer on data to write
 *  
 * \brief	No concurent access has to be allowed. It has to be checked before.
 */
/*****************************************************************************/
#pragma force_dcumode
void METH(readThroughMTF)(t_uint32 srcDataAddress, t_uint16 Size, void *dstDataAddress)
{
    t_uint16 *saved_dstDataAddress;
    t_uint32 read_address;

    saved_dstDataAddress = dstDataAddress;

    /* Read external memory */    
    if ((srcDataAddress & 0x1) == 0x1)
    {
        /* Calc new address */
        read_address = srcDataAddress & ~(t_uint32)0x1;

        /* Read with MTF */
        mtf_read_table(read_address,saved_dstDataAddress,Size,0x1,MTF_NO_SWAP_BYTE_NO_SWAP_WORD64);  
    }
    else
    {
        t_uint16 *internal_address;
        t_uint16 i;

        /* Point on internal table */
        read_address = (HOST_CONVERT_ADDR(srcDataAddress));
        internal_address = (t_uint16*)read_address;

        /* Recopy data from internal to structure */
        for (i=0; i<Size; i++)
        {
            *saved_dstDataAddress = *internal_address;
            saved_dstDataAddress++;
            internal_address++;
        }            
    }
} /* end of readThroughMTF() function */

/*****************************************************************************/
/**
 * \brief   writeThroughMTF
 * \brief 	Write data from firmware to memory (internal/external depending on
 * 		    bit0 of dstDataAddress i.e. 0:internal 1:external)
 * \author 	SVA FW Team (source is extracted from old scheduler)
 * \param   dstDataAddress Address location (no endianess to be handled as this
 * 			param is unmarshalled by NMF engine).
 * \param 	Size Data size to be written
 * \param   srcDataAddress Pointer on data to read
 *  
 * \brief	No concurent access has to be allowed. It has to be checked before.
 */
/*****************************************************************************/
#ifndef BIT_TRUE
#pragma force_dcumode
void METH(writeThroughMTF)(t_uint32 dstDataAddress,t_uint16 Size,void *srcDataAddress)
{
    t_uint16 *saved_srcDataAddress;
    t_uint32 write_address;

    saved_srcDataAddress = srcDataAddress;

    /* Write external memory */
    if ((dstDataAddress & 0x1) == 0x1)
    {
        /* Calc new address */
        write_address = dstDataAddress & ~(t_uint32)0x1;

        /* Write data */
        mtf_write_table(write_address,saved_srcDataAddress,Size,0x1,MTF_NO_SWAP_BYTE_NO_SWAP_WORD64);          
    }
    /* Write internal memory */
    else
    {
        t_uint16 *internal_address;
		t_uint16 i;
    
        /* Point on internal table */
        write_address = (HOST_CONVERT_ADDR(dstDataAddress));
        internal_address = (t_uint16*)write_address;
                
        /* Copy data to structure */
        for (i=0; i<Size; i++)
        {
            *internal_address = *saved_srcDataAddress;
            saved_srcDataAddress++;
            internal_address++;
        }
    }
} /* end of writeThroughMTF() function */

/*****************************************************************************/
/**
 * \brief   mtf_write_table
 */
/*****************************************************************************/
#pragma force_dcumode
void METH(mtfWriteTable)(t_mtf_mode mode, t_uint32 addr_out, t_uint16 *pt_src, t_uint16 len, t_uint16 incr, t_uint16 swap_cfg){

  switch (mode){
    case TRANSFERT_MTF_ONLY :
      mtf_write_table(addr_out, pt_src, len, incr, swap_cfg);
      break;
    case TRANSFERT_MTF_HW_LOOP:
//      mtf_write_table_hwloop(addr_out, pt_src, len, incr, swap_cfg);
      break;
    default:
      break;
    }

} /* mtf_write_table() function. */
/*****************************************************************************/
/**
 * \brief   mtf_read_table
 */
/*****************************************************************************/
#pragma force_dcumode
void METH(mtfReadTable)(t_mtf_mode mode, t_uint32 addr_in, t_uint16 *pt_dest, t_uint16 len, t_uint16 incr, t_uint16 swap_cfg){

  switch (mode){
    case TRANSFERT_MTF_ONLY :
      mtf_read_table(addr_in, pt_dest, len, incr, swap_cfg);
    case TRANSFERT_MTF_HW_LOOP:
//      mtf_read_table_hwloop(addr_in, pt_dest, len, incr, swap_cfg);
      break;
    default:
      break;
    }

} /* End of mtf_read_table()) function. */

#endif
