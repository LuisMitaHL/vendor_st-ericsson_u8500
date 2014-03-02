/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
/**
 *
 * \file bip_error.h
 * \author ST Ericsson
 *
 * This header file provides BIP debug traces and error codes.
 *
 * @{
 */
/*---------------------------------------------------------------------------*/
#ifndef _BIP_ERROR_H
#define _BIP_ERROR_H


/*------------------------------------------------------------------------
 * Function definition
 *----------------------------------------------------------------------*/
void XLDBG_SendTrace(
        t_pub_rom_loader_info   *p_loaderInfo,
        t_address               pdata,
        t_size                  size);

/* Xloader Debug Trace Codes                         */
/* ------------------------------------------------- */
#define FIND_FLASH              0x20001
#define EXECUTE_PA              0x20002

#define GET_RESET_REASON        0x20100
/*
 * GET_RESET_REASON is followed by
 * 4 bytes contents of PRCMU status reg (0x8015803C 8500 B0)
 * 4 bytes t_boot_last_reset enum from boot_types.h
 */

#define SEARCH_MEM_INIT_FUNC    0x20200
#define CHECK_MEM_INIT_FUNC     0x20201
#define EXECUTE_MEM_INIT_FUNC   0x20202

#define SEARCH_PWR_MGT          0x20300
#define LOAD_PWR_MGT            0x20301
#define CONFIGURE_4500          0x20302
#define GET_4500_CUT_ID         0x20302
#define GET_8500_CUT_ID         0x20302
#define START_XP70              0x20303
#define XP70_STARTED            0x20304

#define SET_MODEM_PLL           0x20400

#define SEARCH_MODEM            0x20500
#define CHECK_MODEM             0x20501
#define LOAD_MODEM              0x20502

#define SEARCH_IPL              0x20600
#define CHECK_IPL               0x20601
#define LOAD_IPL                0x20602

#define SEARCH_APPLI            0x20700
#define CHECK_APPLI             0x20701
#define EXECUTE_NORMAL          0x20702
#define EXECUTE_PRODUCTION      0x20703
#define EXECUTE_ADL             0x20704

#define DISABLE_CACHE_MMU       0x20800
#define ENABLE_L2CC             0x20900

/* Xloader Error code                                */
/* ------------------------------------------------- */
#define	XL_ERROR_BAD_CUT_ID			0xC0010001 /**< Xloader cannot executes on this cutID  */
#define	XL_ERROR_XL_NOT_FOUND                   0xC0010002 /**< Xloader cannot find XLOADER in TOC        */
#define	XL_ERROR_PWR_MGT_NOT_FOUND              0xC0010003 /**< Xloader cannot find PWR_MGT in TOC        */
#define	XL_ERROR_XP70_TIMEOUT_NOT_STARTED       0xC0010004 /**< xP70 not stared, timeout        */
#define	XL_ERROR_BAD_PWR_MGT_VERSION            0xC0010005 /**< PWR_MGT cannot run on this chip, bad cutID  */
#define	XL_ERROR_MEM_INIT_NOT_FOUND             0xC0010006 /**< Xloader cannot find MEM_INIT in TOC        */
#define	XL_ERROR_IPL_NOT_FOUND                  0xC0010007 /**< Xloader cannot find IPL in TOC        */
#define	XL_ERROR_MODEM_NOT_FOUND                0xC0010008 /**< Xloader cannot find MODEM in TOC        */
#define	XL_ERROR_NORMAL_NOT_FOUND               0xC0010009 /**< Xloader cannot find NORMAL in TOC        */
#define	XL_ERROR_PRODUCTION_NOT_FOUND           0xC001000A /**< Xloader cannot find PRODUCTION in TOC        */
#define	XL_ERROR_ADL_NOT_FOUND                  0xC001000B /**< Xloader cannot find ADL in TOC        */
#define	XL_ERROR_MODEM_WRONG_LOAD_ADDRESS       0xC001000C /**< Xloader: Wrong Load Address of MODEM detected   */

#define MEM_INIT_INTERNAL_ERROR	0xA0010022

#endif /* _bip_error_H*/
/* end of bip_error.h */
/** @} */
