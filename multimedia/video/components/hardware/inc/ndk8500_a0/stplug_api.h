/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */


#ifndef _STPLUG_API_H_
#define _STPLUG_API_H_


/*
 * Includes							       
 */
#include "t1xhv_types.h" 	         /* Include for MMDSP types */
#include "macros.h"                /* macros, intrinsics      */
#include "t1xhv_hamac_xbus_regs.h" /* All register addresses  */
#include <stwdsp.h>
#include "dma_api.h" 

/*
 * Defines							       
 */

/****************************************************************************/
/**
 * \brief 	Optimum programmation of STBUS plug registers
 * \param       
 * \author Maurizio Colombo
 *  
 * 
 **/
/****************************************************************************/
#pragma inline
static void STPLUG_PGM(void)
{
  /* DMA_PORT_CTRL_L/H */
  /* Configured for DDR only        */
  /* To be updated by each codec    */
  /*   when using eSRAM             */
  DMA_SET_REG(DMA_PORT_CTRL_L,0x0000U);
  DMA_SET_REG(DMA_PORT_CTRL_H,0x0000U);
  /* Default values */
  /* CUP_CTX_R */
  DMA_SET_REG(DMA_MAX_OPC_SZ0,6);
  DMA_SET_REG(DMA_MAX_MSSG_SZ0,0);
  DMA_SET_REG(DMA_MAX_CHCK_SZ0,0);

  /* MTF_R */
  DMA_SET_REG(DMA_MAX_OPC_SZ1,3);
  DMA_SET_REG(DMA_MAX_MSSG_SZ1,0);
  DMA_SET_REG(DMA_MAX_CHCK_SZ1,0);

  /* SWF_R */
  DMA_SET_REG(DMA_MAX_OPC_SZ2,5);
  DMA_SET_REG(DMA_MAX_MSSG_SZ2,0);
  DMA_SET_REG(DMA_MAX_CHCK_SZ2,0);

  /* CD_R */
  DMA_SET_REG(DMA_MAX_OPC_SZ3,5);
  DMA_SET_REG(DMA_MAX_MSSG_SZ3,0);
  DMA_SET_REG(DMA_MAX_CHCK_SZ3,0);

  /* VPP_L_R */
  DMA_SET_REG(DMA_MAX_OPC_SZ4,6);
  DMA_SET_REG(DMA_MAX_MSSG_SZ4,1);
  DMA_SET_REG(DMA_MAX_CHCK_SZ4,0);

  /* VPP_C_R */
  DMA_SET_REG(DMA_MAX_OPC_SZ5,6);
  DMA_SET_REG(DMA_MAX_MSSG_SZ5,0);
  DMA_SET_REG(DMA_MAX_CHCK_SZ5,0);

  /* VPP_PARAM_R */
  DMA_SET_REG(DMA_MAX_OPC_SZ6,4);
  DMA_SET_REG(DMA_MAX_MSSG_SZ6,0);
  DMA_SET_REG(DMA_MAX_CHCK_SZ6,0);

  /* IMC_R */
  DMA_SET_REG(DMA_MAX_OPC_SZ7,6);
  DMA_SET_REG(DMA_MAX_MSSG_SZ7,2);
  DMA_SET_REG(DMA_MAX_CHCK_SZ7,0);

  /* IME_CWL_R */
  DMA_SET_REG(DMA_MAX_OPC_SZ8,5);
  DMA_SET_REG(DMA_MAX_MSSG_SZ8,0);
  DMA_SET_REG(DMA_MAX_CHCK_SZ8,0);

  /* IME_TPL_R */
  DMA_SET_REG(DMA_MAX_OPC_SZ9,3);
  DMA_SET_REG(DMA_MAX_MSSG_SZ9,0);
  DMA_SET_REG(DMA_MAX_CHCK_SZ9,0);

  /* IME_SWL_R */
  DMA_SET_REG(DMA_MAX_OPC_SZ10,5);
  DMA_SET_REG(DMA_MAX_MSSG_SZ10,0);
  DMA_SET_REG(DMA_MAX_CHCK_SZ10,0);

  /* IME_MVF_R */
  DMA_SET_REG(DMA_MAX_OPC_SZ11,3);  
  DMA_SET_REG(DMA_MAX_MSSG_SZ11,0);
  DMA_SET_REG(DMA_MAX_CHCK_SZ11,0);

  /* IME_CCM_W */
  DMA_SET_REG(DMA_MAX_OPC_SZ12,3);  
  DMA_SET_REG(DMA_MAX_MSSG_SZ12,0);
  DMA_SET_REG(DMA_MAX_CHCK_SZ12,0);

  /* IPA_R */
  DMA_SET_REG(DMA_MAX_OPC_SZ13,3);  
  DMA_SET_REG(DMA_MAX_MSSG_SZ13,0);
  DMA_SET_REG(DMA_MAX_CHCK_SZ13,0);

  /* RECF_R */
  DMA_SET_REG(DMA_MAX_OPC_SZ14,5);  
  DMA_SET_REG(DMA_MAX_MSSG_SZ14,0);
  DMA_SET_REG(DMA_MAX_CHCK_SZ14,0);

  /* MTF_W */
  DMA_SET_REG(DMA_MAX_OPC_SZ15,3);  
  DMA_SET_REG(DMA_MAX_MSSG_SZ15,0);
  DMA_SET_REG(DMA_MAX_CHCK_SZ15,0);

  /* SWF_W */
  DMA_SET_REG(DMA_MAX_OPC_SZ16,5);  
  DMA_SET_REG(DMA_MAX_MSSG_SZ16,0);
  DMA_SET_REG(DMA_MAX_CHCK_SZ16,0);

  /* CD_W */
  DMA_SET_REG(DMA_MAX_OPC_SZ17,6);  
  DMA_SET_REG(DMA_MAX_MSSG_SZ17,0);
  DMA_SET_REG(DMA_MAX_CHCK_SZ17,0);

  /* VPP_L_W */
  DMA_SET_REG(DMA_MAX_OPC_SZ18,6);  
  DMA_SET_REG(DMA_MAX_MSSG_SZ18,2);
  DMA_SET_REG(DMA_MAX_CHCK_SZ18,0);

  /* VPP_C_W */
  DMA_SET_REG(DMA_MAX_OPC_SZ19,6);  
  DMA_SET_REG(DMA_MAX_MSSG_SZ19,0);
  DMA_SET_REG(DMA_MAX_CHCK_SZ19,0);

  /* REC_W */
  DMA_SET_REG(DMA_MAX_OPC_SZ20,6);  
  DMA_SET_REG(DMA_MAX_MSSG_SZ20,0);
  DMA_SET_REG(DMA_MAX_CHCK_SZ20,0);

  /* CUP_CTX_W */
  DMA_SET_REG(DMA_MAX_OPC_SZ21,6);  
  DMA_SET_REG(DMA_MAX_MSSG_SZ21,0);
  DMA_SET_REG(DMA_MAX_CHCK_SZ21,0);

  /* CUP_DBX_W */
  DMA_SET_REG(DMA_MAX_OPC_SZ22,4);  
  DMA_SET_REG(DMA_MAX_MSSG_SZ22,0);
  DMA_SET_REG(DMA_MAX_CHCK_SZ22,0);

  /* IME_MVF_W */
  DMA_SET_REG(DMA_MAX_OPC_SZ23,3);  
  DMA_SET_REG(DMA_MAX_MSSG_SZ23,0);
  DMA_SET_REG(DMA_MAX_CHCK_SZ23,0);

  /* RECF_W */
  DMA_SET_REG(DMA_MAX_OPC_SZ24,6);  
  DMA_SET_REG(DMA_MAX_MSSG_SZ24,0);
  DMA_SET_REG(DMA_MAX_CHCK_SZ24,0);

}
#endif /* _STPLUG_API_H_ */

