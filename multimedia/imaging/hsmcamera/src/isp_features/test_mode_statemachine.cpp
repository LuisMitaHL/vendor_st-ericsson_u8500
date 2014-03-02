/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */


#undef OMXCOMPONENT
#define OMXCOMPONENT "TEST_MODE"

#include "hsmcam.h"
#include "osi_trace.h"

#undef OMX_TRACE_UID
#define OMX_TRACE_UID 0x8

// OST framework
#include "OstTraceDefinitions_ste.h"
#include "OpenSystemTrace_ste.h"

#ifdef OST_TRACE_COMPILER_IN_USE
#include "hsmcamera_src_isp_features_test_mode_statemachineTraces.h"
#endif

//#define CAM_TRACE_TEST_MODE



#define I2C_WRITTING_MAX_SIZE 32
ts_I2CReg  sTestModeConfig[I2C_WRITTING_MAX_SIZE];

#include "VhcElementDefs.h"
IFM_CONFIG_WRITEPETYPE sTestModeConfigPE;

SCF_STATE CAM_SM::SettingTestMode(s_scf_event const *e)
{
	switch (e->sig)
	{
		case SCF_STATE_ENTRY_SIG:
			IMG_LOG("SettingTestMode-ENTRY;\n",NULL,NULL,NULL,NULL,NULL,NULL);
			MSG0("SettingTestMode-ENTRY;\n");
			OstTraceFiltStatic0(TRACE_DEBUG, "SettingTestMode-ENTRY;", (&mENSComponent));
			return 0;
		case SCF_STATE_EXIT_SIG:
			IMG_LOG("SettingTestMode-EXIT;\n",NULL,NULL,NULL,NULL,NULL,NULL);
			MSG0("SettingTestMode-EXIT;\n");
			OstTraceFiltStatic0(TRACE_DEBUG, "SettingTestMode-EXIT;", (&mENSComponent));
			return 0;
		case SCF_STATE_INIT_SIG:
			IMG_LOG("SettingTestMode-INIT;\n",NULL,NULL,NULL,NULL,NULL,NULL);
			MSG0("SettingTestMode-INIT;\n");
			OstTraceFiltStatic0(TRACE_DEBUG, "SettingTestMode-INIT;", (&mENSComponent));
			SCF_INIT(&CAM_SM::ProcessTestMode);
			return 0;
		case Q_DEFERRED_EVENT_SIG:
			IMG_LOG("SettingTestMode-DEFERRED_EVENT;\n",NULL,NULL,NULL,NULL,NULL,NULL);
			MSG0("SettingTestMode-DEFERRED_EVENT;\n");
			OstTraceFiltStatic0(TRACE_DEBUG, "SettingTestMode-DEFERRED_EVENT;", (&mENSComponent));
			return 0;
		default:
			IMG_LOG("SettingTestMode- default : e->sig = %d;\n",e->sig,NULL,NULL,NULL,NULL,NULL);
	}

	IMG_LOG("call SCF_STATE_PTR(&CAM_SM::SetFeature)\n",NULL,NULL,NULL,NULL,NULL,NULL);
	return SCF_STATE_PTR(&CAM_SM::SetFeature);
}

SCF_STATE CAM_SM::ProcessTestMode(s_scf_event const *e)
{
	t_uint32 indexPE = 0;
	enumCameraSlot sensorSlot = ePrimaryCamera;

	sensorSlot = pSensor->GetCameraSlot();
	switch (sensorSlot)
	{
		case ePrimaryCamera:
			IMG_LOG("\t\t sensorSlot == ePrimaryCamera\n",NULL,NULL,NULL,NULL,NULL,NULL);
			MSG0("\t\t sensorSlot == ePrimaryCamera \n");
			OstTraceFiltStatic0(TRACE_DEBUG, "\t\t sensorSlot == ePrimaryCamera ", (&mENSComponent));
		   break;
		case eSecondaryCamera:
			IMG_LOG("\t\t sensorSlot == ePrimaryCamera\n",NULL,NULL,NULL,NULL,NULL,NULL);
			MSG0("\t\t sensorSlot == ePrimaryCamera \n");
			OstTraceFiltStatic0(TRACE_DEBUG, "\t\t sensorSlot == ePrimaryCamera ", (&mENSComponent));
		   break;
		default :
			IMG_LOG("\t\t sensorSlot == UNKNOWN\n",NULL,NULL,NULL,NULL,NULL,NULL);
			MSG0("\t\t sensorSlot == UNKNOWN \n");
			OstTraceFiltStatic0(TRACE_DEBUG, "\t\t sensorSlot == UNKNOWN ", (&mENSComponent));
		   break;
	}

	switch (e->sig)
	{
		case SCF_STATE_ENTRY_SIG:
			IMG_LOG("ProcessTestMode-ENTRY;\n",NULL,NULL,NULL,NULL,NULL,NULL);
			MSG0("ProcessTestMode-ENTRY;\n");
			OstTraceFiltStatic0(TRACE_DEBUG, "ProcessTestMode-ENTRY;", (&mENSComponent));
			IMG_LOG("ask a PE to be waked up in pong\n",NULL,NULL,NULL,NULL,NULL,NULL);
			pResourceSharerManager->mRSPing.Ping(0);
			return 0;
		case SCF_STATE_EXIT_SIG:
			IMG_LOG("ProcessTestMode-EXIT;\n",NULL,NULL,NULL,NULL,NULL,NULL);
			MSG0("ProcessTestMode-EXIT;\n");
			OstTraceFiltStatic0(TRACE_DEBUG, "ProcessTestMode-EXIT;", (&mENSComponent));
			return 0;
		case Q_PONG_SIG:
			IMG_LOG("ProcessTestMode-Q_PONG_SIG;\n",NULL,NULL,NULL,NULL,NULL,NULL);
			MSG0("ProcessTestMode-Q_PONG_SIG;\n");
			OstTraceFiltStatic0(TRACE_DEBUG, "ProcessTestMode-Q_PONG_SIG;", (&mENSComponent));


            //SM_PUSH_STATE_STATIC(&CAM_SM::ProcessPendingEvents);

			switch(pTestMode->eCamTestMode)
			{
				case OMX_STE_TestModeNone:
					IMG_LOG("\t\t case OMX_STE_TestModeNone\n",NULL,NULL,NULL,NULL,NULL,NULL);
					sTestModeConfigPE.nPEList_addr[indexPE] = TestPattern_Ctrl_e_TestPattern_Byte0;
					sTestModeConfigPE.nPEList_data[indexPE] = TestPattern_e_Normal;
					indexPE++;
					sTestModeConfigPE.nPEList_addr[indexPE] = 0x0;
					sTestModeConfigPE.nPEList_data[indexPE] = 0x0;
					indexPE++;

					break;
				case OMX_STE_TestModeColorBar:
					IMG_LOG("\t\t case OMX_STE_TestModeColorBar\n",NULL,NULL,NULL,NULL,NULL,NULL);
					sTestModeConfigPE.nPEList_addr[indexPE] = TestPattern_Ctrl_e_TestPattern_Byte0;
					sTestModeConfigPE.nPEList_data[indexPE] = TestPattern_e_SolidColourBars;
					indexPE++;
					sTestModeConfigPE.nPEList_addr[indexPE] = 0x0;
					sTestModeConfigPE.nPEList_data[indexPE] = 0x0;
					indexPE++;

					break;
				case OMX_STE_TestModeSolidBar_Black:
					IMG_LOG("\t\t case OMX_STE_TestModeSolidBar_Black\n",NULL,NULL,NULL,NULL,NULL,NULL);
					sTestModeConfigPE.nPEList_addr[indexPE] = TestPattern_Ctrl_e_TestPattern_Byte0;
					sTestModeConfigPE.nPEList_data[indexPE] = TestPattern_e_SolidColour;
					indexPE++;
					sTestModeConfigPE.nPEList_addr[indexPE] = TestPattern_Ctrl_u16_test_data_red_Byte0;
					sTestModeConfigPE.nPEList_data[indexPE] = 0x0000;
					indexPE++;
					sTestModeConfigPE.nPEList_addr[indexPE] = TestPattern_Ctrl_u16_test_data_greenR_Byte0;
					sTestModeConfigPE.nPEList_data[indexPE] = 0x0000;
					indexPE++;
					sTestModeConfigPE.nPEList_addr[indexPE] = TestPattern_Ctrl_u16_test_data_blue_Byte0;
					sTestModeConfigPE.nPEList_data[indexPE] = 0x0000;
					indexPE++;
					sTestModeConfigPE.nPEList_addr[indexPE] = TestPattern_Ctrl_u16_test_data_greenB_Byte0;
					sTestModeConfigPE.nPEList_data[indexPE] = 0x0000;
					indexPE++;
					sTestModeConfigPE.nPEList_addr[indexPE] = 0x0;
					sTestModeConfigPE.nPEList_data[indexPE] = 0x0;
					indexPE++;

					break;
				case OMX_STE_TestModeSolidBar_Blue:
					IMG_LOG("\t\t case OMX_STE_TestModeSolidBar_Blue\n",NULL,NULL,NULL,NULL,NULL,NULL);
					sTestModeConfigPE.nPEList_addr[indexPE] = TestPattern_Ctrl_e_TestPattern_Byte0;
					sTestModeConfigPE.nPEList_data[indexPE] = TestPattern_e_SolidColour;
					indexPE++;
					sTestModeConfigPE.nPEList_addr[indexPE] = TestPattern_Ctrl_u16_test_data_red_Byte0;
					sTestModeConfigPE.nPEList_data[indexPE] = 0x0000;
					indexPE++;
					sTestModeConfigPE.nPEList_addr[indexPE] = TestPattern_Ctrl_u16_test_data_greenR_Byte0;
					sTestModeConfigPE.nPEList_data[indexPE] = 0x0000;
					indexPE++;
					sTestModeConfigPE.nPEList_addr[indexPE] = TestPattern_Ctrl_u16_test_data_blue_Byte0;
					sTestModeConfigPE.nPEList_data[indexPE] = 0xFFFF;
					indexPE++;
					sTestModeConfigPE.nPEList_addr[indexPE] = TestPattern_Ctrl_u16_test_data_greenB_Byte0;
					sTestModeConfigPE.nPEList_data[indexPE] = 0x0000;
					indexPE++;
					sTestModeConfigPE.nPEList_addr[indexPE] = 0x0;
					sTestModeConfigPE.nPEList_data[indexPE] = 0x0;
					indexPE++;

					break;
				case OMX_STE_TestModeSolidBar_Cyan:
					// bleu + green
					IMG_LOG("\t\t case OMX_STE_TestModeSolidBar_Cyan\n",NULL,NULL,NULL,NULL,NULL,NULL);
					sTestModeConfigPE.nPEList_addr[indexPE] = TestPattern_Ctrl_e_TestPattern_Byte0;
					sTestModeConfigPE.nPEList_data[indexPE] = TestPattern_e_SolidColour;
					indexPE++;
					sTestModeConfigPE.nPEList_addr[indexPE] = TestPattern_Ctrl_u16_test_data_red_Byte0;
					sTestModeConfigPE.nPEList_data[indexPE] = 0x0000;
					indexPE++;
					sTestModeConfigPE.nPEList_addr[indexPE] = TestPattern_Ctrl_u16_test_data_greenR_Byte0;
					sTestModeConfigPE.nPEList_data[indexPE] = 0xffff;
					indexPE++;
					sTestModeConfigPE.nPEList_addr[indexPE] = TestPattern_Ctrl_u16_test_data_blue_Byte0;
					sTestModeConfigPE.nPEList_data[indexPE] = 0xFFFF;
					indexPE++;
					sTestModeConfigPE.nPEList_addr[indexPE] = TestPattern_Ctrl_u16_test_data_greenB_Byte0;
					sTestModeConfigPE.nPEList_data[indexPE] = 0xFFFF;
					indexPE++;
					sTestModeConfigPE.nPEList_addr[indexPE] = 0x0;
					sTestModeConfigPE.nPEList_data[indexPE] = 0x0;
					indexPE++;

					break;
				case OMX_STE_TestModeSolidBar_Green:
					IMG_LOG("\t\t case OMX_STE_TestModeSolidBar_Green\n",NULL,NULL,NULL,NULL,NULL,NULL);
					sTestModeConfigPE.nPEList_addr[indexPE] = TestPattern_Ctrl_e_TestPattern_Byte0;
					sTestModeConfigPE.nPEList_data[indexPE] = TestPattern_e_SolidColour;
					indexPE++;
					sTestModeConfigPE.nPEList_addr[indexPE] = TestPattern_Ctrl_u16_test_data_red_Byte0;
					sTestModeConfigPE.nPEList_data[indexPE] = 0x0000;
					indexPE++;
					sTestModeConfigPE.nPEList_addr[indexPE] = TestPattern_Ctrl_u16_test_data_greenR_Byte0;
					sTestModeConfigPE.nPEList_data[indexPE] = 0xFFFF;
					indexPE++;
					sTestModeConfigPE.nPEList_addr[indexPE] = TestPattern_Ctrl_u16_test_data_blue_Byte0;
					sTestModeConfigPE.nPEList_data[indexPE] = 0x0000;
					indexPE++;
					sTestModeConfigPE.nPEList_addr[indexPE] = TestPattern_Ctrl_u16_test_data_greenB_Byte0;
					sTestModeConfigPE.nPEList_data[indexPE] = 0xFFFF;
					indexPE++;
					sTestModeConfigPE.nPEList_addr[indexPE] = 0x0;
					sTestModeConfigPE.nPEList_data[indexPE] = 0x0;
					indexPE++;

					break;
				case OMX_STE_TestModeSolidBar_Magenta:
					// bleu + red
					IMG_LOG("\t\t case OMX_STE_TestModeSolidBar_Magenta\n",NULL,NULL,NULL,NULL,NULL,NULL);
					sTestModeConfigPE.nPEList_addr[indexPE] = TestPattern_Ctrl_e_TestPattern_Byte0;
					sTestModeConfigPE.nPEList_data[indexPE] = TestPattern_e_SolidColour;
					indexPE++;
					sTestModeConfigPE.nPEList_addr[indexPE] = TestPattern_Ctrl_u16_test_data_red_Byte0;
					sTestModeConfigPE.nPEList_data[indexPE] = 0xFFFF;
					indexPE++;
					sTestModeConfigPE.nPEList_addr[indexPE] = TestPattern_Ctrl_u16_test_data_greenR_Byte0;
					sTestModeConfigPE.nPEList_data[indexPE] = 0x0000;
					indexPE++;
					sTestModeConfigPE.nPEList_addr[indexPE] = TestPattern_Ctrl_u16_test_data_blue_Byte0;
					sTestModeConfigPE.nPEList_data[indexPE] = 0xFFFF;
					indexPE++;
					sTestModeConfigPE.nPEList_addr[indexPE] = TestPattern_Ctrl_u16_test_data_greenB_Byte0;
					sTestModeConfigPE.nPEList_data[indexPE] = 0x0000;
					indexPE++;
					sTestModeConfigPE.nPEList_addr[indexPE] = 0x0;
					sTestModeConfigPE.nPEList_data[indexPE] = 0x0;
					indexPE++;

					break;
				case OMX_STE_TestModeSolidBar_Red:
					IMG_LOG("\t\t case OMX_STE_TestModeSolidBar_Red\n",NULL,NULL,NULL,NULL,NULL,NULL);
					sTestModeConfigPE.nPEList_addr[indexPE] = TestPattern_Ctrl_e_TestPattern_Byte0;
					sTestModeConfigPE.nPEList_data[indexPE] = TestPattern_e_SolidColour;
					indexPE++;
					sTestModeConfigPE.nPEList_addr[indexPE] = TestPattern_Ctrl_u16_test_data_red_Byte0;
					sTestModeConfigPE.nPEList_data[indexPE] = 0xFFFF;
					indexPE++;
					sTestModeConfigPE.nPEList_addr[indexPE] = TestPattern_Ctrl_u16_test_data_greenR_Byte0;
					sTestModeConfigPE.nPEList_data[indexPE] = 0x0000;
					indexPE++;
					sTestModeConfigPE.nPEList_addr[indexPE] = TestPattern_Ctrl_u16_test_data_blue_Byte0;
					sTestModeConfigPE.nPEList_data[indexPE] = 0x0000;
					indexPE++;
					sTestModeConfigPE.nPEList_addr[indexPE] = TestPattern_Ctrl_u16_test_data_greenB_Byte0;
					sTestModeConfigPE.nPEList_data[indexPE] = 0x0000;
					indexPE++;
					sTestModeConfigPE.nPEList_addr[indexPE] = 0x0;
					sTestModeConfigPE.nPEList_data[indexPE] = 0x0;
					indexPE++;

					break;
				case OMX_STE_TestModeSolidBar_White:
					IMG_LOG("\t\t case OMX_STE_TestModeSolidBar_White\n",NULL,NULL,NULL,NULL,NULL,NULL);
					sTestModeConfigPE.nPEList_addr[indexPE] = TestPattern_Ctrl_e_TestPattern_Byte0;
					sTestModeConfigPE.nPEList_data[indexPE] = TestPattern_e_SolidColour;
					indexPE++;
					sTestModeConfigPE.nPEList_addr[indexPE] = TestPattern_Ctrl_u16_test_data_red_Byte0;
					sTestModeConfigPE.nPEList_data[indexPE] = 0xFFFF;
					indexPE++;
					sTestModeConfigPE.nPEList_addr[indexPE] = TestPattern_Ctrl_u16_test_data_greenR_Byte0;
					sTestModeConfigPE.nPEList_data[indexPE] = 0xFFFF;
					indexPE++;
					sTestModeConfigPE.nPEList_addr[indexPE] = TestPattern_Ctrl_u16_test_data_blue_Byte0;
					sTestModeConfigPE.nPEList_data[indexPE] = 0xFFFF;
					indexPE++;
					sTestModeConfigPE.nPEList_addr[indexPE] = TestPattern_Ctrl_u16_test_data_greenB_Byte0;
					sTestModeConfigPE.nPEList_data[indexPE] = 0xFFFF;
					indexPE++;
					sTestModeConfigPE.nPEList_addr[indexPE] = 0x0;
					sTestModeConfigPE.nPEList_data[indexPE] = 0x0;
					indexPE++;

					break;
				case OMX_STE_TestModeSolidBar_Yellow:
					// red + green
					IMG_LOG("\t\t case OMX_STE_TestModeSolidBar_Yellow\n",NULL,NULL,NULL,NULL,NULL,NULL);
					sTestModeConfigPE.nPEList_addr[indexPE] = TestPattern_Ctrl_e_TestPattern_Byte0;
					sTestModeConfigPE.nPEList_data[indexPE] = TestPattern_e_SolidColour;
					indexPE++;
					sTestModeConfigPE.nPEList_addr[indexPE] = TestPattern_Ctrl_u16_test_data_red_Byte0;
					sTestModeConfigPE.nPEList_data[indexPE] = 0xFFFF;
					indexPE++;
					sTestModeConfigPE.nPEList_addr[indexPE] = TestPattern_Ctrl_u16_test_data_greenR_Byte0;
					sTestModeConfigPE.nPEList_data[indexPE] = 0xFFFF;
					indexPE++;
					sTestModeConfigPE.nPEList_addr[indexPE] = TestPattern_Ctrl_u16_test_data_blue_Byte0;
					sTestModeConfigPE.nPEList_data[indexPE] = 0x0000;
					indexPE++;
					sTestModeConfigPE.nPEList_addr[indexPE] = TestPattern_Ctrl_u16_test_data_greenB_Byte0;
					sTestModeConfigPE.nPEList_data[indexPE] = 0xffff;
					indexPE++;
					sTestModeConfigPE.nPEList_addr[indexPE] = 0x0;
					sTestModeConfigPE.nPEList_data[indexPE] = 0x0;
					indexPE++;

					break;

				case OMX_STE_TestModePN9:
					IMG_LOG("\t\t case OMX_STE_TestModePN9\n",NULL,NULL,NULL,NULL,NULL,NULL);
					
					sTestModeConfigPE.nPEList_addr[indexPE] = TestPattern_Ctrl_e_TestPattern_Byte0;
					sTestModeConfigPE.nPEList_data[indexPE] = TestPattern_e_PN9;
					indexPE++;
					sTestModeConfigPE.nPEList_addr[indexPE] = 0x0;
					sTestModeConfigPE.nPEList_data[indexPE] = 0x0;
					indexPE++;

					break;
				case OMX_STE_TestModeGreyScale:
					IMG_LOG("\t\t case OMX_STE_TestModeGreyScale\n",NULL,NULL,NULL,NULL,NULL,NULL);
					
					sTestModeConfigPE.nPEList_addr[indexPE] = TestPattern_Ctrl_e_TestPattern_Byte0;
					sTestModeConfigPE.nPEList_data[indexPE] = TestPattern_e_SolidColourBarsFade;
					indexPE++;
					sTestModeConfigPE.nPEList_addr[indexPE] = 0x0;
					sTestModeConfigPE.nPEList_data[indexPE] = 0x0;
					indexPE++;

					break;
				default:
					IMG_LOG("\t\t case default : %d\n",pTestMode->eCamTestMode,NULL,NULL,NULL,NULL,NULL);
					MSG1("\t\t case default : %d\n",pTestMode->eCamTestMode);
					OstTraceFiltStatic1(TRACE_DEBUG, "\t\t case default : %d", (&mENSComponent),pTestMode->eCamTestMode);
					break;
			}

            if( 0 != indexPE)
            {
                pTrace->resetWriteListPe();
                pTrace->iListOfPeToWrite = sTestModeConfigPE;
                while(pTrace->iListOfPeToWrite.nPEList_addr[pTrace->iListOfPeToWrite.nPEToWrite]!=0)
                {
                	pTrace->iListOfPeToWrite.nPEToWrite++;
                	DBC_ASSERT(pTrace->iListOfPeToWrite.nPEToWrite < MAX_NUMBER_OF_PE_IN_A_LIST);
                }
                pTrace->iPEMode=WRITE_LIST_OF_PE;
                pTrace->bCallbackNeeded=OMX_FALSE;
                IMG_LOG("\n\n Ask to writte %d PEs !!!\n\n",pTrace->iListOfPeToWrite.nPEToWrite,NULL,NULL,NULL,NULL,NULL);
                SCF_TRANSIT_TO(&CAM_SM::SHARED_Debugging);
            }
            else
            {
                IMG_LOG("nothing to writte on pe\n",NULL,NULL,NULL,NULL,NULL,NULL);
                SCF_TRANSIT_TO(&CAM_SM::ProcessPendingEvents);
            }

			return 0;
		default:
			IMG_LOG("ProcessTestMode- default : e->sig = %d;\n",e->sig,NULL,NULL,NULL,NULL,NULL);
	}

	IMG_LOG("default call SCF_STATE_PTR(&CAM_SM::SettingTestMode)\n",NULL,NULL,NULL,NULL,NULL,NULL);
	return SCF_STATE_PTR(&CAM_SM::SettingTestMode);
}

