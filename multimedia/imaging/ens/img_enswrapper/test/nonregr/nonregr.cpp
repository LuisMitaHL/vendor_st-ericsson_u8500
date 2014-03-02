/*****************************************************************************
* Copyright (C) ST-Ericsson SA 2010. All rights reserved.
* This code is ST-Ericsson proprietary and confidential.
* Any use of the code for whatever purpose is subject to
* specific written permission of ST-Ericsson SA.
* Author:     : laurent.regnier@st.com
* Filename:   : nonregr.cpp
* Description : test suite for imgenswrapper
*               Use a test component named omximgens_test
*****************************************************************************/
#define _CRT_SECURE_NO_WARNINGS
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "osi_toolbox_lib.h"
#include "omx_toolbox_lib.h"
#include "Img_EnsWrapper_Lib.h"
#include "omxapi_loader.h" //Bellagio osal

using namespace ImgEns;

extern "C"  OMX_ERRORTYPE setup_component_loader(BOSA_COMPONENTLOADER * pLoader);

enum
{
	ePort0,
	ePort1,
};

int Test_ImgLoader     (NonRegressionTest &Parent);
int Test_BosaInterface (NonRegressionTest &Parent);
int Test_BasicComponentUse (NonRegressionTest &Parent, const char *ComponentName, unsigned int nb_port);

int main (int /*argc*/, const char ** /*argv*/)
//******************************************************************************************
{
#ifdef _MSC_VER
	_crtBreakAlloc=98;
#endif
	NonRegressionTest NoRegr;
	NoRegr.Name="Wrapper non regression";
	Test_BosaInterface (NoRegr);
	Test_ImgLoader (NoRegr);
	Test_BasicComponentUse (NoRegr, "OMX.STE.WOMDEMOCPT"  , 2);
	Test_BasicComponentUse (NoRegr, "OMX.ST.ArmIVProc"    , 2);
	Test_BasicComponentUse (NoRegr, "OMX.ST.Fakesink"     , 1);
/*
*/
	printf(  "\n\n End of Tests, display statistics \n");
	DisplayOverallResult(NoRegr);

	return(NoRegr.GetNbErrors());
}

int Test_ImgLoader (NonRegressionTest &Parent)
//******************************************************************************************
{
	NonRegressionTest &Test=*Parent.CreateChild();
	Test.Name="Test_ImgLoader";
	int result=S_OK;
	OMX_ERRORTYPE error;
	BOSA_COMPONENTLOADER ImgLoaderInterface;
	error=setup_component_loader(&ImgLoaderInterface);
	steTEST(Test, error == OMX_ErrorNone);
	if (error != OMX_ErrorNone)
	{
		printf("Cannot setup loader (error=%d),", error);
		return(-1);
	}
	size_t nbr=0;
	STE_Img_Loader &Loader=Get_ImgLoader_Instance();
	nbr=Loader.GetModuleCount();
	steTEST(Test,nbr==0);
	error=ImgLoaderInterface.BOSA_InitComponentLoader(&ImgLoaderInterface);
	steTEST(Test, error == OMX_ErrorNone);
	if (error != OMX_ErrorNone)
	{
		printf("Cannot InitComponentLoader (%s=%d)\n", GetStringOmxError(error), error);
		return(-2);
	}

	OMX_COMPONENTTYPE*Handle;
	OMX_PTR           pAppData  = NULL;
	OMX_CALLBACKTYPE  CallBacks = { NULL, NULL, NULL};
	char ComponentName[]= "OMX.ST.ImgEns_Demo0";
	OMX_ERRORTYPE status;
	OMX_STATETYPE state;
	size_t Retry=100;
	do
	{
		error= ImgLoaderInterface.BOSA_CreateComponent(&ImgLoaderInterface, (OMX_HANDLETYPE *)&Handle, ComponentName , pAppData, &CallBacks);
		steTEST(Test, error == OMX_ErrorNone /* BOSA_CreateComponent */);
		if (error != OMX_ErrorNone)
		{
			printf("Cannot CreateComponent %s (%s=%d)\n", ComponentName, GetStringOmxError(error), error );
			break;
		}
		// Idle
		status=Handle->SendCommand( Handle, OMX_CommandStateSet, OMX_StateIdle, NULL);
		steTEST(Test, error == OMX_ErrorNone);
		OMX_BUFFERHEADERTYPE* pBufferHdr;

		// Allocate buffersn must be in Transcient IDLE state
		//const size_t BuffSize=1024;
		//unsigned char buff[BuffSize];
		//status=Handle->UseBuffer( Handle, &pBufferHdr, ePort0, NULL, BuffSize, buff);
		status=Handle->AllocateBuffer( Handle, &pBufferHdr, ePort0, NULL, 98304);
		steTEST(Test, status == OMX_ErrorNone);

		while (Handle->GetState( Handle, &state), state != OMX_StateIdle)
		{ //Wait reaching state idle
			SleepMillisecond(1000);
			if (Retry > 0)
				--Retry;
			else
				break;
		}

		//Executing state
		status=Handle->SendCommand( Handle, OMX_CommandStateSet, OMX_StateExecuting, NULL);
		steTEST(Test, status == OMX_ErrorNone);
		if (1)
		{

			while (Handle->GetState( Handle, &state), state != OMX_StateExecuting)
			{ //Wait reaching state idle
				SleepMillisecond(1000);
				if (Retry > 0)
					--Retry;
				else
					break;
			}

			status=Handle->SendCommand( Handle, OMX_CommandStateSet, OMX_StateIdle, NULL);
			steTEST(Test, status == OMX_ErrorNone);
			while (Handle->GetState( Handle, &state), state != OMX_StateIdle)
			{ //Wait reaching state idle
				SleepMillisecond(1000);
				if (Retry > 0)
					--Retry;
				else
					break;
			}

			status=Handle->FreeBuffer( Handle, ePort0, pBufferHdr); //Free buffer must be done is IDLE
			steTEST(Test, status == OMX_ErrorNone);
			status=Handle->SendCommand( Handle, OMX_CommandStateSet, OMX_StateLoaded, NULL);
			steTEST(Test, status == OMX_ErrorNone);

			while (Handle->GetState( Handle, &state), state != OMX_StateLoaded)
			{ //Wait reaching state idle
				SleepMillisecond(1000);
				if (Retry > 0)
					--Retry;
				else
					break;
			}
		}
		error= ImgLoaderInterface.BOSA_DestroyComponent(& ImgLoaderInterface, Handle);
		steTEST(Test, error == OMX_ErrorNone);
	} while(0);

	error=ImgLoaderInterface.BOSA_DeInitComponentLoader(&ImgLoaderInterface);
	steTEST(Test, error == OMX_ErrorNone);
	if (error != OMX_ErrorNone)
	{
		printf("Cannot DeInitComponentLoader (%s=%d)\n", GetStringOmxError(error), error);
		result=-10;
	}
	return(Test.GetNbErrors());
};

int Test_BasicComponentUse (NonRegressionTest &Parent, const char *ComponentName, unsigned int nb_port)
//******************************************************************************************
{
	NonRegressionTest &Test=*Parent.CreateChild();
	Test.Name="Test_ImgLoader";
	int result=S_OK;
	OMX_ERRORTYPE error;
	BOSA_COMPONENTLOADER ImgLoaderInterface;
	error=setup_component_loader(&ImgLoaderInterface);
	steTEST(Test, error == OMX_ErrorNone);
	OMX_BUFFERHEADERTYPE** pBufferHdr=NULL;
	if (error != OMX_ErrorNone)
	{
		printf("Cannot setup loader (error=%d),", error);
		return(-1);
	}
	size_t nbr=0;
	STE_Img_Loader &Loader=Get_ImgLoader_Instance();
	nbr=Loader.GetModuleCount();
	steTEST(Test,nbr==0);
	error=ImgLoaderInterface.BOSA_InitComponentLoader(&ImgLoaderInterface);
	steTEST(Test, error == OMX_ErrorNone);
	if (error != OMX_ErrorNone)
	{
		printf("Cannot InitComponentLoader (%s=%d)\n", GetStringOmxError(error), error);
		return(-2);
	}

	OMX_COMPONENTTYPE*Handle;
	OMX_PTR           pAppData  = NULL;
	OMX_CALLBACKTYPE  CallBacks = { NULL, NULL, NULL};

	OMX_ERRORTYPE status;
	OMX_STATETYPE state;
	size_t Retry=100;
	do
	{
		error= ImgLoaderInterface.BOSA_CreateComponent(&ImgLoaderInterface, (OMX_HANDLETYPE *)&Handle, (OMX_STRING)ComponentName , pAppData, &CallBacks);
		steTEST(Test, error == OMX_ErrorNone /* BOSA_CreateComponent */);
		if (error != OMX_ErrorNone)
		{
			printf("Cannot CreateComponent %s (%s=%d)\n", ComponentName, GetStringOmxError(error), error );
			break;
		}
		// Idle
		status=Handle->SendCommand( Handle, OMX_CommandStateSet, OMX_StateIdle, NULL);
		steTEST(Test, error == OMX_ErrorNone);
		pBufferHdr= new OMX_BUFFERHEADERTYPE *[nb_port];

		// Allocate buffersn must be in Transcient IDLE state
		//const size_t BuffSize=1024;
		//unsigned char buff[BuffSize];
		//status=Handle->UseBuffer( Handle, &pBufferHdr, ePort0, NULL, BuffSize, buff);
		for (unsigned int n=0; n < nb_port; ++n)
		{
			status=Handle->AllocateBuffer( Handle, &pBufferHdr[n], n, NULL, 98304);
		}
		steTEST(Test, status == OMX_ErrorNone);

		while (Handle->GetState( Handle, &state), state != OMX_StateIdle)
		{ //Wait reaching state idle
			SleepMillisecond(1000);
			if (Retry > 0)
				--Retry;
			else
				break;
		}

		//Executing state
		status=Handle->SendCommand( Handle, OMX_CommandStateSet, OMX_StateExecuting, NULL);
		steTEST(Test, status == OMX_ErrorNone);
		if (1)
		{

			while (Handle->GetState( Handle, &state), state != OMX_StateExecuting)
			{ //Wait reaching state idle
				SleepMillisecond(1000);
				if (Retry > 0)
					--Retry;
				else
					break;
			}

			status=Handle->SendCommand( Handle, OMX_CommandStateSet, OMX_StateIdle, NULL);
			steTEST(Test, status == OMX_ErrorNone);
			while (Handle->GetState( Handle, &state), state != OMX_StateIdle)
			{ //Wait reaching state idle
				SleepMillisecond(1000);
				if (Retry > 0)
					--Retry;
				else
					break;
			}

			for (unsigned int n=0; n < nb_port; ++n)
			{
				status=Handle->FreeBuffer( Handle, n, pBufferHdr[n]); //Free buffer must be done is IDLE
			}
			steTEST(Test, status == OMX_ErrorNone);
			status=Handle->SendCommand( Handle, OMX_CommandStateSet, OMX_StateLoaded, NULL);
			steTEST(Test, status == OMX_ErrorNone);

			while (Handle->GetState( Handle, &state), state != OMX_StateLoaded)
			{ //Wait reaching state idle
				SleepMillisecond(1000);
				if (Retry > 0)
					--Retry;
				else
					break;
			}
		}
		error= ImgLoaderInterface.BOSA_DestroyComponent(& ImgLoaderInterface, Handle);
		steTEST(Test, error == OMX_ErrorNone);
	} while(0);

	error=ImgLoaderInterface.BOSA_DeInitComponentLoader(&ImgLoaderInterface);
	steTEST(Test, error == OMX_ErrorNone);
	if (error != OMX_ErrorNone)
	{
		printf("Cannot DeInitComponentLoader (%s=%d)\n", GetStringOmxError(error), error);
		result=-10;
	}

	delete [] pBufferHdr;
	return(Test.GetNbErrors());
};

int Test_BosaInterface (NonRegressionTest &Parent)
//******************************************************************************************
{ // Test all functionalities of the BOSA interface (use a test component)
	NonRegressionTest &Test=*Parent.CreateChild();
	Test.Name=__FUNCTION__;
	const size_t BuffSize=256;
	const size_t MaxNames=10;
	char Buff[BuffSize];
	char Names[MaxNames][BuffSize];
	char *pNames[MaxNames];
	OMX_U32 Nbr=0;
	for (size_t i=0; i < MaxNames; ++i)
	{
		pNames[i] = Names[i]; //Initialise pointers
	}
	BOSA_COMPONENTLOADER ImgLoaderInterface;
	OMX_ERRORTYPE omxerror= setup_component_loader(&ImgLoaderInterface);
	steTEST(Test, omxerror==OMX_ErrorNone); //Init the bosa interface
	if (omxerror!=OMX_ErrorNone)
		return(Test.GetNbErrors());

	steTEST(Test, ImgLoaderInterface.BOSA_InitComponentLoader(&ImgLoaderInterface)==OMX_ErrorNone);

	STE_Img_Loader &Loader=Get_ImgLoader_Instance();
	if (Loader.GetModuleCount()==0)
	{
		printf("\nNo component found in.directory '%s'.. skip test", Loader.GetDirectory());
	}
	else
	{
		//Test get name
		steTEST(Test, ImgLoaderInterface.BOSA_ComponentNameEnum(&ImgLoaderInterface, Buff, BuffSize, 0)   ==OMX_ErrorNone);
		steTEST(Test, ImgLoaderInterface.BOSA_ComponentNameEnum(&ImgLoaderInterface, Buff, BuffSize, 3)   ==OMX_ErrorNone);
		steTEST(Test, ImgLoaderInterface.BOSA_ComponentNameEnum(&ImgLoaderInterface, Buff, BuffSize, 1000)==OMX_ErrorNoMore);// No more component

		//Test get roles of component
		steTEST(Test, ImgLoaderInterface.BOSA_GetRolesOfComponent(&ImgLoaderInterface, (char*)"OMX.ST.ImgEns_Test3", &Nbr, NULL)   ==OMX_ErrorNone);
		steTEST(Test, Nbr==3);
		steTEST(Test, ImgLoaderInterface.BOSA_GetRolesOfComponent(&ImgLoaderInterface, (char*)"OMX.ST.ImgEns_Test3", &Nbr, (OMX_U8 **) pNames)   ==OMX_ErrorNone);

		//Test get component of roles 
		Nbr=0;
		steTEST(Test, ImgLoaderInterface.BOSA_GetComponentsOfRole(&ImgLoaderInterface, (char*)"nonexisting", &Nbr, NULL)   ==OMX_ErrorNone);
		steTEST(Test, Nbr==0);

		steTEST(Test, ImgLoaderInterface.BOSA_GetComponentsOfRole(&ImgLoaderInterface, (char*)"roles2.ImgEns_Test3", &Nbr, NULL)   ==OMX_ErrorNone);
		steTEST(Test, Nbr==1);
		Nbr=MaxNames;
		steTEST(Test, ImgLoaderInterface.BOSA_GetComponentsOfRole(&ImgLoaderInterface, (char*)"roles2.ImgEns_Test3", &Nbr, (OMX_U8 **) pNames)   ==OMX_ErrorNone);

		// 2 component have this role
		steTEST(Test, ImgLoaderInterface.BOSA_GetComponentsOfRole(&ImgLoaderInterface, (char*)"roles0.ImgEns_Test", &Nbr, NULL)   ==OMX_ErrorNone);
		steTEST(Test, Nbr==2);
		Nbr=MaxNames;
		steTEST(Test, ImgLoaderInterface.BOSA_GetComponentsOfRole(&ImgLoaderInterface, (char*)"roles0.ImgEns_Test", &Nbr, (OMX_U8 **) pNames)   ==OMX_ErrorNone);
	}

	// End 
	steTEST(Test, ImgLoaderInterface.BOSA_DeInitComponentLoader(&ImgLoaderInterface)==OMX_ErrorNone);
	return(Test.GetNbErrors());
}
