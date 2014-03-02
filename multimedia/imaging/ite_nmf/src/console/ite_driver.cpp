/*****************************************************************************/
/**
* Copyright (C) ST-Ericsson SA 2010. All rights reserved.
* This code is ST-Ericsson proprietary and confidential.
* Any use of the code for whatever purpose is subject to
* specific written permission of ST-Ericsson SA.
*
* \brief   Terminates ITE  : Close resources, free allocated memory,...
* \author  ST-Ericsson
*/
/*****************************************************************************/
/*-----------------------------------------*/
/* INCLUDES                                */
/*-----------------------------------------*/
#ifdef __ARM_SYMBIAN


#include "MMIO_Camera.h"
#include "mmio_osal.h"
#include <los/api/los_api.h>

#include <inc/type.h>
#include "cm/inc/cm.h"
#include <cm/inc/cm_macros.h>
#include "ite_main.h"


extern t_nmf_channel nmfCallBackFifoID;

extern IMPORT_C void registerStubsAndSkels(void);
extern IMPORT_C void unregisterStubsAndSkels(void);

extern t_uint8 g_exit_CM_CALLBACK_THREAD;

/*-----------------------------------------*/
/* External Function                       */
/*-----------------------------------------*/

/** ------------------------------------------------------------------
 * \function ite_DeInit
 * \return   0 if successful
 *
 *  Terminates ITE : close resources, free allocated memory...
 * -----------------------------------------------------------------*/
EXPORT_C int ite_DeInit(void)
{
    OMX_ERRORTYPE error;
    t_cm_error err;

	LOS_Log("Release all ITE resources\n");

	LOS_Log("ite_DeInit: Release MMIO DeInit\n");
    // US 5/20 New API added in MMIO driver
    error =MMIO_Camera::desinitBoard();
	if (error != OMX_ErrorNone)
	{
        LOS_Log("ite_DeInit: Failed to desinitBoard error %d\n",error);
        return -1;
    } 

	LOS_Log("ite_DeInit: Release MMIO Driver\n");
	error = MMIO_Camera::releaseDriver();
	if (error != OMX_ErrorNone)
	{
        LOS_Log("ite_DeInit: Failed to release driver error %d\n",error);
        return -1;
    }


	LOS_Log("ite_DeInit: Closing CM Thread\n");
    g_exit_CM_CALLBACK_THREAD = 0;

	LOS_Sleep(100);
	LOS_ThreadExit();
	LOS_Sleep(100);

    ite_sia_deinit_redirection();
	

    LOS_Log("ite_DeInit: CMClose Channel\n");
    err = CM_CloseChannel(nmfCallBackFifoID);
	if(err != CM_OK)
	{
		LOS_Log("ite_DeInit: Failed to close channel error %d\n",err);
        return -1;
    }	

	LOS_Sleep(100);
    
    LOS_ThreadExit();
    LOS_Log("ite_DeInit: Waiting for child thread to exit\n");
    LOS_Sleep(500);

    return 0;
}

/** ------------------------------------------------------------------
 * \function ite_Init_Driver
 * \return   0 if successful
 *
 *  Initiates Camera driver...
 * -----------------------------------------------------------------*/
EXPORT_C int ite_Init_Driver(void)
{
    OMX_ERRORTYPE error;

   LOS_Log("ite_Init_Driver: Loading camera driver LDD \n");
   error = MMIO_Camera::initDriver();
   if (error != OMX_ErrorNone)
	{
   		LOS_Log("ite_Init_Driver:Can't initialize camera driver error %d\n",error);
        return -1;
    }
   
   return 0;
}

/* Added in CPP file */
EXPORT_C void ite_sia_init_redirection(void)
{
    //CM_REGISTER_STUBS_SKELS(imaging);
	registerStubsAndSkels();
}

EXPORT_C void ite_sia_deinit_redirection(void)
{
    //CM_REGISTER_STUBS_SKELS(imaging);
    unregisterStubsAndSkels();
}


#endif /*__ARM_SYMBIAN*/

