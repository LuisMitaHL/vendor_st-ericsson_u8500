/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */ 

#ifndef __HALGPSXOSGPS_HIP__
#define __HALGPSXOSGPS_HIP__
/**
* \file halgpsxosgps.hip
* \date 26/05/2009
* \version 1.0
*
* <B>Compiler:</B> ARM ADS\n
*
* <B>Description:</B> This file contain all prototype used by halgps8xosgps.c.\n
* 
* <TABLE>
*     <TR>
*             <TD> Date</TD><TD> Author</TD><TD> Description</TD>
*     </TR>
*     <TR>
*             <TD> 17.07.08</TD><TD> Archana.B </TD><TD> Creation </TD>
*     </TR>
* </TABLE>
*/


/**
*\fn bool HALGPS8_00XosgpsInit(void)
*\brief initialisation of HALGPS module XOSGPS part
*
* initialises XOSGPS
*\return TRUE if initialisation was successful, FALSE otherwise
*/
bool HALGPS8_00XosgpsInit(void);


/**
*\fn uint32_t HALGPS8_02XosgpsRcv(uint8_t * pp_rxBuffer, uint32_t vp_length)
*\brief get data from XOSGPS
*
*\param pp_rxBuffer pointer where the data will be copied
*\param vp_length desired data length
*\return copied data length
*/
uint32_t HALGPS8_02XosgpsRcv(uint8_t * pp_rxBuffer, uint32_t vp_length);

/**
*\fn uint32_t HALGPS8_02XosgpsSend(uint8_t * pp_txBuffer, uint32_t  vp_length)
*\brief send data to XOSGPS
*
*\param pp_txBuffer buffer containing data
*\param vp_length data length
*\return the sent data length
*/
uint32_t HALGPS8_02XosgpsSend(uint8_t * pp_txBuffer, uint32_t  vp_length);

/**
*\fn bool HALGPS8_03XosgpsCtrl( e_halgps_xosgps_ctrl vp_CtrlChar )
*\brief Xosgps Ctrl
*\return TRUE if initialisation was successful, FALSE otherwise
*/
bool HALGPS8_03XosgpsCtrl( e_halgps_xosgps_ctrl vp_CtrlChar );


#endif //__HALGPSXOSGPS_HIP__
