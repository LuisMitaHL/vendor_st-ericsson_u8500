/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef _SELFTEST_H_
#define _SELFTEST_H_

#include "OMX_Component.h"
#include "host/grab/api/cmd.hpp"
#include "hsm.h"
#include "sensor.h"
#include "IFM_Types.h"
#include "flash_bitfields.h"
#include "flash_api.h"


#ifdef __SYMBIAN32__
NONSHARABLE_CLASS(CSelfTest);
#endif
class CSelfTest
{
	public :
		CSelfTest(ENS_Component* pENSComp, enumCameraSlot CamSlot, CDefferedEventMgr* pDeferredEventMgr);
		OMX_SYMBIAN_CONFIG_CAMERASELFTESTSELECTTYPE* pSelfTestSelect;
		OMX_CONFIG_BOOLEANTYPE* pTesting;
		ENS_Component* pENSComponent;
		enumCameraSlot eCamSlot;
		CDefferedEventMgr* pDeferredEventMgr;

		struct s_grabParams grabParams;
		OMX_ERRORTYPE selftestError;
		OMX_BOOL callbackEnabled; /* OMX_TRUE if a callback must be sent at end of selftests */
		OMX_SYMBIAN_CAMERASELFTESTTYPE currentSelftest;
};

#ifdef __SYMBIAN32__
NONSHARABLE_CLASS(CLoadedSelfTest);
#endif
class CLoadedSelfTest
{
	public :
		CLoadedSelfTest(ENS_Component* pENSComp, enumCameraSlot CamSlot);
		OMX_SYMBIAN_CONFIG_CAMERASELFTESTSELECTTYPE* pSelfTestSelect;
		OMX_CONFIG_BOOLEANTYPE* pTesting;
		ENS_Component* pENSComponent;
		enumCameraSlot eCamSlot;

		void StartFlashSelftests(void);
		static void Flash_cb (TFlashSelftest aResult, void *apContext);
		OMX_BOOL callbackEnabled;
};

#endif /* _SELFTEST_H_ */
