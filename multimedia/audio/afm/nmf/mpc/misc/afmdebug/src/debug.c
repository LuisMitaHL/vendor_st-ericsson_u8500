/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   debug.c
 * \brief  
 * \author ST-Ericsson
 */
/*****************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <afmenum.idt>
#include <afmstate.idt>
#include <afmerror.idt>
#include <afmeos.idt>


const char debugInfoEnumType[AFM_NB__ENUM][32]=
{
	"AFM_ENUM_EOS",
	"AFM_ENUM_ERROR",
	"AFM_ENUM_STATE"
};

const char debugInfoEnumState[AFM_NB_STATE_ENUM][32]=
{
	"AFM_LoadedState",
	"AFM_ReadyState",
	"AFM_PlayingState",
	"AFM_PausedState",
	"AFM_PendingStopState"
};

const char debugInfoEnumEos[AFM_NB_EOS_ENUM][32]=
{
	"AFM_EOS_SENT",
	"AFM_EOS_RECEIVED"
};

const char debugInfoEnumError[AFM_NB_ERROR_ENUM][32]=
{
	"AFM_NO_ERROR"
};


void AfmPrintEnum(char *Name,int typeOfInfo, int info)
{
	AfmEnumType_e type = (AfmEnumType_e)typeOfInfo;
	switch(type)
	{
		case AFM_ENUM_EOS :
		{
			AfmEos_e eos = (AfmEos_e)info;
			printf("- AFM: %s\t  Type :%s\tValue : %s\n",Name,debugInfoEnumType[type],debugInfoEnumEos[eos]);
			break;
		}
		case AFM_ENUM_ERROR :
		{
			AfmError_e error = (AfmError_e)info;
			printf("- AFM: %s\t  Type :%s\tValue : %s\n",Name,debugInfoEnumType[type],debugInfoEnumError[error]);
			break;
		}
		case AFM_ENUM_STATE :
		{
			AfmState_e state = (AfmState_e)info;
			printf("- AFM: %s\t  Type :%s\tValue : %s\n",Name,debugInfoEnumType[type],debugInfoEnumState[state]);
			break;
		}
	}
}


