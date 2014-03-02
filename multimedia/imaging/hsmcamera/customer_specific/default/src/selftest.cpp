/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
 
#include "selftest.h"
#include "OMX_CoreExt.h"
#include "omxcamera.h"
#include "camera.h"

CSelfTest::CSelfTest(ENS_Component* pENSComp, enumCameraSlot CamSlot, CDefferedEventMgr* pDeferredEvtMgr)
{
	// set pointers to COmxCamera config structs
	pSelfTestSelect = &(((COmxCamera*)pENSComp)->mSelfTestType);
	pTesting = &(((COmxCamera*)pENSComp)->mTestingType);
	pENSComponent = pENSComp;
	pDeferredEventMgr = pDeferredEvtMgr;

	// init structs
	pSelfTestSelect->nPortIndex = OMX_ALL;
	pSelfTestSelect->nFlashHwFaultRegister1 = 0;
	pSelfTestSelect->nFlashHwFaultRegister2 = 0;
	pSelfTestSelect->nTestSelectMaxSizeUsed = 0;
	for(int j=0 ; j<OMX_SYMBIAN_MAX_SELF_TESTS_PER_REQUEST ; j++)
	{
		pSelfTestSelect->eSelfTests[j].eTestId = OMX_SYMBIAN_CameraSelfTestNone;
		pSelfTestSelect->eSelfTests[j].eResult = OMX_ErrorUndefined;
	}
	pTesting->bEnabled = OMX_FALSE;

	// init local variables
	selftestError = OMX_ErrorUndefined;
	callbackEnabled = OMX_FALSE;
	eCamSlot = CamSlot;
	currentSelftest = OMX_SYMBIAN_CameraSelfTestNone;
}

CLoadedSelfTest::CLoadedSelfTest(ENS_Component* pENSComp, enumCameraSlot CamSlot)
{
	// set pointers to COmxCamera config structs
	pSelfTestSelect = &(((COmxCamera*)pENSComp)->mSelfTestType);
	pTesting = &(((COmxCamera*)pENSComp)->mTestingType);
	pENSComponent = pENSComp;
	eCamSlot = CamSlot;
}

void CLoadedSelfTest::StartFlashSelftests(void)
{
}

void CLoadedSelfTest::Flash_cb (TFlashSelftest aResult, void *apContext)
{
}
