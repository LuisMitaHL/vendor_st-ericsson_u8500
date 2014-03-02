/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include "hsmcam.h"
#include "nmf_eventhandler.h"
#include "osi_trace.h"
#include "host/ispctl/api/cmd.hpp" /* should not be usefull as included in ispmcu_component_manager.h, but needed for a unknwown reason in Symbian */
#include <cm/inc/cm.hpp> // needed for a unknwown reason in Symbian */

#include "MMIO_Camera.h"
#include "pictor.h"         //  LUT programming
#include "ImgConfig.h"

#include "cam_shared_memory.h"
#include "VhcElementDefs.h"
#include "camera.h"
#include "async_flash_api.h"

// OST framework
#include "OstTraceDefinitions_ste.h"
#include "OpenSystemTrace_ste.h"

#ifdef OST_TRACE_COMPILER_IN_USE
#include "hsmcamera_src_boot_statemachineTraces.h"
#endif

#ifdef TraceLogsControl_u32_BufferAddr_Byte0
/* firmware supports tracing */
#define FIRMWARE_TRACE_SUPPORT 1
#endif

bool chkSignal = true;

/* */
/* Boot state machine */
/**/
SCF_STATE CAM_SM::Booting(s_scf_event const *e) {
	switch (e->sig) {
		case SCF_STATE_ENTRY_SIG:  ENTRY; return 0;
		case SCF_STATE_EXIT_SIG:  EXIT;  return 0;
		case SCF_STATE_INIT_SIG:
		{
			INIT;
			SCF_INIT(&CAM_SM::InitingXp70WFA);
			return 0;
		}
		case SCF_PARENT_SIG:break;
		default:  break;
	}
return SCF_STATE_PTR(&CAM_SM::OMX_Executing);
}


SCF_STATE  CAM_SM::InitingXp70WFA(s_scf_event const *e){
	switch (e->sig) {
		case SCF_STATE_ENTRY_SIG:
		{
			ENTRY;
            MSG1("Lock %s\n", CError::stringRSRequesterId(CAMERA_VF));
            OstTraceFiltStatic0(TRACE_DEBUG, "Lock SCF_STATE_ENTRY_SIG", (&mENSComponent));
            //OstTraceFiltStatic1(TRACE_DEBUG, "Lock %s\n", CError::stringRSRequesterId(CAMERA_VF), (&mENSComponent));
			pResourceSharerManager->mRSLock.Lock(CAMERA_VF);
			return 0;
		}
		case Q_LOCK_OK_SIG:
		{
			MSG0("Boot ISP FW\n");
			OstTraceFiltStatic0(TRACE_DEBUG, "Boot ISP FW", (&mENSComponent));
			pIspctlCom->getIspctlCmd().initISP(0);
			return 0;
		}
		case Q_LOCK_KO_SIG:
		{
			DBC_ASSERT(0); // TBD : not yet managed !
			/* in that particular case it could not occur */
			break;
		}
		case SCF_STATE_EXIT_SIG:
		{
			EXIT;
			return 0;
		}
		case EVT_ISPCTL_INFO_SIG :
		{
			MSG0("InitingXp70WFA-EVT_ISPCTL_INFO_SIG\n");
			OstTraceFiltStatic0(TRACE_DEBUG, "InitingXp70WFA-EVT_ISPCTL_INFO_SIG", (&mENSComponent));
#ifdef FIRMWARE_TRACE_SUPPORT
			COmxCamera *omxcam = (COmxCamera*)&mENSComponent;
			csm_uint32_t trace_addr;
			csm_uint32_t trace_phy_addr;
			csm_uint32_t trace_log_addr;
			IFM_PARAM_FIRMWARETRACETYPE trace_config;
			if (pSharedMemory->getXp70TraceAddress(&trace_addr) == CAM_SHARED_MEM_ERR_NONE
			&& pSharedMemory->getXp70TraceLogAddress(&trace_log_addr) == CAM_SHARED_MEM_ERR_NONE
			&& omxcam->getParameter((OMX_INDEXTYPE)IFM_IndexParamFirmwareTrace, &trace_config) == OMX_ErrorNone
			&& trace_config.bEnabled == OMX_TRUE
			&& MMIO_Camera::setTraceBuffer(trace_log_addr, CAM_SHARED_MEM_TRACE_BUFFER_SIZE) == OMX_ErrorNone) {
				pIspctlCom->queuePE(TraceLogsControl_u32_BufferAddr_Byte0, (t_uint32)trace_addr);
				pIspctlCom->queuePE(TraceLogsControl_u32_BufferSize_Byte0, (t_uint32)CAM_SHARED_MEM_TRACE_BUFFER_SIZE);

				pIspctlCom->queuePE(TraceLogsControl_u8_LogEnable_Byte0, (int)trace_config.bEnabled);

				pIspctlCom->queuePE(TraceLogsControl_u8_TraceMechanismSelect_Byte0, (t_uint32)trace_config.eTraceMechanism);

				pIspctlCom->queuePE(TraceLogsControl_u32_LogLevels_Byte0,trace_config.nLogLevel);
				pIspctlCom->processQueue();
				MSG0("firmware traces enabled\n");
				OstTraceFiltStatic0(TRACE_DEBUG, "firmware traces enabled", (&mENSComponent));
		    } else {
			MSG0("firmware traces disabled\n");
			OstTraceFiltStatic0(TRACE_DEBUG, "firmware traces disabled", (&mENSComponent));
			/* TODO : to be fine-tuned on V1 (may need to un-subscribe to unexpected events */

			// FIXME: This event must eventually be removed from FW as the statistics data are reported only once the last parameter update is effective.
			pIspctlCom->getIspctlCmd().unsubscribeEvent(ISP_EXPOSURE_AND_WB_PARAMETERS_UPDATED ,0,0,SIA_CLIENT_CAMERA);

			SCF_TRANSIT_TO(&CAM_SM::PreBootingWFA);
		    }
			return 0;
		}
		case EVT_ISPCTL_LIST_INFO_SIG :
	    {
		  MSG0("InitingXp70WFA-EVT_ISPCTL_LIST_INFO_SIG\n");
		  OstTraceFiltStatic0(TRACE_DEBUG, "InitingXp70WFA-EVT_ISPCTL_LIST_INFO_SIG", (&mENSComponent));

		  if (e->type.ispctlInfo.info_id != ISP_WRITELIST_DONE) break;
#endif
			/* TODO : to be fine-tuned on V1 (may need to un-subscribe to unexpected events */

			// FIXME: This event must eventually be removed from FW as the statistics data are reported only once the last parameter update is effective.
			pIspctlCom->getIspctlCmd().unsubscribeEvent(ISP_EXPOSURE_AND_WB_PARAMETERS_UPDATED ,0,0,SIA_CLIENT_CAMERA);

			SCF_TRANSIT_TO(&CAM_SM::PreBootingWFA);
			return 0;
		}
		default: break;
	}
	return SCF_STATE_PTR(&CAM_SM::Booting);
}

SCF_STATE CAM_SM::PreBootingWFA(s_scf_event const *e) {
	switch (e->sig) {
		case SCF_STATE_ENTRY_SIG:
		{
			ENTRY;
			t_uint32 NbOfElementInSensorPreBootTable;
			ts_PageElement * SensorPePreBoot;
			SensorPePreBoot = pSensor->GetSensorPreBootTable(&NbOfElementInSensorPreBootTable);
			if(0 != NbOfElementInSensorPreBootTable)
			{
				for (unsigned int i=0; i < NbOfElementInSensorPreBootTable; i++) {
					pIspctlCom->queuePE(SensorPePreBoot[i].pe_addr, SensorPePreBoot[i].pe_data);
				}
			}
			unsigned int ip_gpio;
			OMX_ERRORTYPE e_ret = MMIO_Camera::getCameraFunction(MMIO_Camera::ePrimaryXSD, &ip_gpio);
			if (e_ret != OMX_ErrorNone) {
				DBGT_ERROR("CAM_SM::PreBooting - ERROR: could not get primary GPIO\n");
				OstTraceFiltStatic0(TRACE_ERROR, "CAM_SM::PreBooting - ERROR: could not get primary GPIO", (&mENSComponent));
				DBC_ASSERT(e_ret);
			}
			pIspctlCom->queuePE(GPIOControl_u8_Sensor0XShutdownGPO_Byte0, (int)ip_gpio);
			e_ret = MMIO_Camera::getCameraFunction(MMIO_Camera::eSecondaryXSD, &ip_gpio);
			if (e_ret != OMX_ErrorNone) {
				DBGT_ERROR("CAM_SM::PreBooting - ERROR: could not get secondary GPIO\n");
				OstTraceFiltStatic0(TRACE_ERROR, "CAM_SM::PreBooting - ERROR: could not get secondary GPIO", (&mENSComponent));
				DBC_ASSERT(e_ret);
			}
			pIspctlCom->queuePE(GPIOControl_u8_Sensor1XShutdownGPO_Byte0, (int)ip_gpio);
#ifdef CAMERA_ENABLE_OMX_3A_EXTENSION
			pIspctlCom->queuePE(Sensor_Tuning_Control_e_TypeNVMExport_Byte0, (int)TypeNVMExport_e_Raw);
#endif // CAMERA_ENABLE_OMX_3A_EXTENSION
			pIspctlCom->processQueue();
			return 0;
		}
		case SCF_STATE_EXIT_SIG:   EXIT;  return 0;

		case EVT_ISPCTL_LIST_INFO_SIG:
		{
			MSG0("PreBootingWFA - EVT_ISPCTL_LIST_INFO_SIG\n");
			OstTraceFiltStatic0(TRACE_DEBUG, "PreBootingWFA - EVT_ISPCTL_LIST_INFO_SIG", (&mENSComponent));
			if (e->type.ispctlInfo.info_id != ISP_WRITELIST_DONE) break;
            SM_PUSH_STATE_STATIC(&CAM_SM::PostBootingWFA);
			ControlingISP_ControlType = ControlingISP_SendBoot_WaitBootComplete;
			SCF_TRANSIT_TO(&CAM_SM::SHARED_ControlingISPState);
			return 0;
	   }
       default: break;
   }
   return SCF_STATE_PTR(&CAM_SM::Booting);
}


SCF_STATE CAM_SM::PostBootingWFA(s_scf_event const *e) {
	switch (e->sig) {
		case SCF_STATE_ENTRY_SIG:
		{
			ENTRY;
			t_uint32 NbOfElementInSensorPostBootTable;
			ts_PageElement * SensorPePostBoot;
			SensorPePostBoot = pSensor->GetSensorPostBootTable(&NbOfElementInSensorPostBootTable);
			if(NbOfElementInSensorPostBootTable == 0)
			{
				/* send a Dummy PE to trigger a signal */
				pResourceSharerManager->mRSPing.Ping(0);
				return 0;
			}
			/* else if there are PostBoot in the table */
			for (unsigned int i=0; i < NbOfElementInSensorPostBootTable; i++) {
				pIspctlCom->queuePE(SensorPePostBoot[i].pe_addr, SensorPePostBoot[i].pe_data);
			}
			pIspctlCom->processQueue();
			return 0;
		}
		case SCF_STATE_EXIT_SIG:  EXIT; return 0;
		case EVT_ISPCTL_LIST_INFO_SIG:
		case Q_PONG_SIG:
		{
			MSG0("PreBootingWFA - EVT_ISPCTL_LIST_INFO_SIG, Q_PONG_SIG\n");
			OstTraceFiltStatic0(TRACE_DEBUG, "PreBootingWFA - EVT_ISPCTL_LIST_INFO_SIG, Q_PONG_SIG", (&mENSComponent));
			SCF_TRANSIT_TO(&CAM_SM::SensorModes);
			return 0;
		}
		default: break;
	}
	return SCF_STATE_PTR(&CAM_SM::Booting);
}


SCF_STATE CAM_SM::SensorModes(s_scf_event const *e) {
    switch (e->sig) {
        case SCF_STATE_ENTRY_SIG:
        {
            ENTRY;
            MSG0("SensorModes-SCF_STATE_ENTRY_SIG\n");
            OstTraceFiltStatic0(TRACE_DEBUG, "SensorModes-SCF_STATE_ENTRY_SIG", (&mENSComponent));

            // Read the SensorModes size and the current coin value
            pIspctlCom->queuePE(ReadLLAConfig_Status_u32_sensor_Output_Mode_data_size_Byte0,0);
            pIspctlCom->queuePE(ReadLLAConfig_Status_e_Coin_ReadLLAConfigStatus_Byte0,0);
            pIspctlCom->queuePE(ReadLLAConfig_Status_u16_number_of_modes_Byte0,0);
            // also read number of non active lines of sensor (to check default values are ok)
            pIspctlCom->queuePE(CurrentFrameDimension_u16_NumberofNonActiveLinesAtTopEdge_Byte0,0);
            pIspctlCom->readQueue();
            return 0;
        }
        case EVT_ISPCTL_LIST_INFO_SIG:
        {
            MSG0("SensorModes-EVT_ISPCTL_LIST_INFO_SIG\n");
            OstTraceFiltStatic0(TRACE_DEBUG, "SensorModes-EVT_ISPCTL_LIST_INFO_SIG", (&mENSComponent));
            if(e->type.ispctlInfo.info_id == ISP_READLIST_DONE)
            {
                // This is the "PE Read" acknowledge
                t_uint16 iSensorModesSize    = (t_uint16)e->type.ispctlInfo.Listvalue[0].pe_data;
                iStatusCoin = (Coin_te) e->type.ispctlInfo.Listvalue[1].pe_data;
                MSG1("SensorModes Size: %d bytes\n", iSensorModesSize);
                OstTraceFiltStatic1(TRACE_DEBUG, "SensorModes Size: %d bytes", (&mENSComponent), iSensorModesSize);
                pSensor->setSensorModesNber((t_uint16)e->type.ispctlInfo.Listvalue[2].pe_data);

                // Sanity check
                DBC_ASSERT(iSensorModesSize);                                           // Must exist
                DBC_ASSERT(iSensorModesSize < CAM_SHARED_MEM_CHUNK_SENSOR_MODES_SIZE);  // Must not exceed max allocated size

                // Send the buffer address to XP70 (where SensorModes has to be copied) and toggle the coin.
                pIspctlCom->queuePE(ReadLLAConfig_Control_ptr32_Sensor_Output_Mode_Data_Address_Byte0, (t_uint32)pSensorModesShmChunk->ispLogicalAddress);
                pIspctlCom->queuePE(ReadLLAConfig_Control_e_Coin_ReadLLAConfigControl_Byte0, (t_uint32)(iStatusCoin==Coin_e_Heads ? Coin_e_Tails : Coin_e_Heads));
                pIspctlCom->processQueue();
                // Stay in this state in order to catch PE write notif
                
                pSensor->setNberOfNonActiveLines((t_uint8)e->type.ispctlInfo.Listvalue[3].pe_data);
                return 0;
            }
            else if (e->type.ispctlInfo.info_id == ISP_WRITELIST_DONE)
            {
                // This is the "PE write" acknowledge
                // Stay in this state to catch ISP_Sensor_Output_Mode_Export_Notification notification
                return 0;
            }
            else break;
        }
        case EVT_ISPCTL_INFO_SIG:
        {
            MSG0("SensorModes-EVT_ISPCTL_INFO_SIG\n");
            OstTraceFiltStatic0(TRACE_DEBUG, "SensorModes-EVT_ISPCTL_INFO_SIG", (&mENSComponent));

            // Here, we expect event ISP_Sensor_Output_Mode_Export_Notification from the firmware
            if (e->type.ispctlInfo.info_id != ISP_SENSOR_OUTPUT_MODE_EXPORT_NOTIFICATION) break;

            // Export notification
            MSG0("SensorModes: SensorModes export completed\n");
            OstTraceFiltStatic0(TRACE_DEBUG, "SensorModes: SensorModes export completed", (&mENSComponent));

            // Invalidate the cache so that we read consistent data from the shared memory
            camSharedMemError_t shmErr = CAM_SHARED_MEM_ERR_NONE;
            MSG0("SensorModes: Invalidating the shared memory cache\n");
            OstTraceFiltStatic0(TRACE_DEBUG, "SensorModes: Invalidating the shared memory cache", (&mENSComponent));
            shmErr = pSharedMemory->cacheInvalidate(CAM_SHARED_MEM_CHUNK_SENSOR_MODES);
            if (shmErr != CAM_SHARED_MEM_ERR_NONE) {
                DBGT_ERROR("SensorModes: Failed to invalidate shared memory chunk, err=%d\n", shmErr);
                OstTraceFiltStatic1(TRACE_ERROR, "SensorModes: Failed to invalidate shared memory chunk, err=%d", (&mENSComponent), shmErr);
                DBC_ASSERT(0);
                return 0; // Fixme: should return to an error state (break)
            }
            
            pSensor->setSensorModesPointer(pSensorModesShmChunk->armLogicalAddress);

            // check that current VPB1 port resolution is supported by sensor
            OMX_PARAM_PORTDEFINITIONTYPE mParamPortDefinition;
            mParamPortDefinition.nSize = sizeof(OMX_PARAM_PORTDEFINITIONTYPE);
            getOmxIlSpecVersion(&mParamPortDefinition.nVersion);
            mENSComponent.getPort(CAMERA_PORT_OUT1)->getParameter(OMX_IndexParamPortDefinition,&mParamPortDefinition);
        //Before making changes in the below check, also check the default format set on port1 in camport.cpp
        if(mParamPortDefinition.format.video.eColorFormat != (OMX_COLOR_FORMATTYPE)OMX_SYMBIAN_COLOR_FormatYUV420MBPackedSemiPlanar)
	    {
            Sensor_Output_Mode_ts* pSensorModes = (Sensor_Output_Mode_ts*)pSensorModesShmChunk->armLogicalAddress;
            t_uint16 SensorModesNber = pSensor->getSensorModesNber();
            t_uint8 i = 0;

            for (i=0 ; i<SensorModesNber ; i++)
            {            
                /* u32_usage_restriction_bitmask should also be checked (not available in FW right now) */
                if ((pSensorModes[i].u32_output_res_width == mParamPortDefinition.format.video.nFrameWidth)
                    && ((pSensorModes[i].u32_output_res_height + pSensor->getNberOfNonActiveLines()) == mParamPortDefinition.format.video.nFrameHeight)
                    && (pSensorModes[i].u32_woi_res_width == mParamPortDefinition.format.video.nFrameWidth)
                    && ((pSensorModes[i].u32_woi_res_height + pSensor->getNberOfNonActiveLines()) == mParamPortDefinition.format.video.nFrameHeight))
                {
                    currentStillSensorMode = pSensorModes[i];
                    break;
                }
            }

            // no sensor mode corresponding to port resolution
            if (SensorModesNber == i)
            {
                MSG0("SensorModes:VPB1 resolution is not supported\n");
                OstTraceFiltStatic0(TRACE_DEBUG, "SensorModes:VPB1 resolution is not supported", (&mENSComponent));
                currentStillSensorMode = pSensorModes[0];
                //mENSComponent.eventHandler(OMX_EventError, (OMX_U32)OMX_ErrorBadParameter, 0);
            }
	    }
		else
		{
			// CR_STILL_YUV//Do nothing here
		}

            SCF_TRANSIT_TO(&CAM_SM::PostBooting_SensorInfo);
            return 0;
        }
        case SCF_STATE_EXIT_SIG:
        {
            EXIT;
            return 0;
        }
        default: break;
    }
    return SCF_STATE_PTR(&CAM_SM::Booting);
}


SCF_STATE CAM_SM::PostBooting_SensorInfo(s_scf_event const *e) {
	switch (e->sig) {
		case SCF_STATE_ENTRY_SIG:
		{
			ENTRY;
			MSG0("PostBooting_SensorInfo-SCF_STATE_ENTRY_SIG\n");
			OstTraceFiltStatic0(TRACE_DEBUG, "PostBooting_SensorInfo-SCF_STATE_ENTRY_SIG", (&mENSComponent));
			// Sensor related data
			pIspctlCom->queuePE(SensorInformation_e_Flag_Available_Byte0,0);			//0
			pIspctlCom->queuePE(SensorInformation_u8_manufacturer_id_Byte0,0);			//1
			pIspctlCom->queuePE(SensorInformation_u16_model_id_Byte0,0);				//2
			pIspctlCom->queuePE(SensorInformation_u8_revision_number_Byte0,0);			//3
			pIspctlCom->queuePE(DeviceParameters_u32_FirmwareVersionMajor_Byte0,0);		//4
			pIspctlCom->queuePE(DeviceParameters_u32_FirmwareVersionMinor_Byte0,0);		//5
			pIspctlCom->queuePE(DeviceParameters_u32_FirmwareVersionMicro_Byte0,0);		//6
			pIspctlCom->queuePE(DeviceParameters_u32_LLA_Sensor_Byte0,0);				//7
			pIspctlCom->queuePE(DeviceParameters_u32_LLA_MajorVersion_Byte0,0);			//8
			pIspctlCom->queuePE(DeviceParameters_u32_LLA_MinorVersion_Byte0,0);			//9
			pIspctlCom->queuePE(DeviceParameters_u32_LLCD_MajorVersion_Byte0,0);		//10
			pIspctlCom->queuePE(DeviceParameters_u32_LLCD_MinorVersion_Byte0,0);		//11
			pIspctlCom->queuePE(FrameParamStatus_Extn_u32_focal_length_x100_Byte0,0);	//12
#ifdef SensorInformation_u8_serial_id_0_Byte0
			// Fuse_Id related Data
			pIspctlCom->queuePE(SensorInformation_u8_serial_id_0_Byte0,0);				//13
			pIspctlCom->queuePE(SensorInformation_u8_serial_id_1_Byte0,0);				//14
			pIspctlCom->queuePE(SensorInformation_u8_serial_id_2_Byte0,0);				//15
			pIspctlCom->queuePE(SensorInformation_u8_serial_id_3_Byte0,0);				//16
			pIspctlCom->queuePE(SensorInformation_u8_serial_id_4_Byte0,0);				//17
			pIspctlCom->queuePE(SensorInformation_u8_serial_id_5_Byte0,0);				//18
#endif
			pIspctlCom->readQueue();
			return 0;
		}
		case EVT_ISPCTL_LIST_INFO_SIG:
		{
			MSG0("PostBooting_SensorInfo-EVT_ISPCTL_LIST_INFO_SIG\n");
			OstTraceFiltStatic0(TRACE_DEBUG, "PostBooting_SensorInfo-EVT_ISPCTL_LIST_INFO_SIG", (&mENSComponent));
			if (e->type.ispctlInfo.info_id != ISP_READLIST_DONE) break;
//#if defined (OSI_DEBUG)
			for(t_uint16 i=0; i < 4; i++) {
				MSG3("   PE[%d] %s = 0x%lX\n", i, pSensor->GetPeName(e->type.ispctlInfo.Listvalue[i].pe_addr), e->type.ispctlInfo.Listvalue[i].pe_data );
				//OstTraceFiltStatic3(TRACE_DEBUG, "   PE[%d] %s = 0x%lX\n", i, CXp70::GetPeName(e->type.ispctlInfo.Listvalue[i].pe_addr), e->type.ispctlInfo.Listvalue[i].pe_data );
			}
//#endif
			// display FW version
			MSG4("FW version : %d.%d.%d %d",(t_uint8)e->type.ispctlInfo.Listvalue[4].pe_data,
			    (t_uint8)e->type.ispctlInfo.Listvalue[5].pe_data,
			    (t_uint8)e->type.ispctlInfo.Listvalue[6].pe_data,
			    (t_uint8)e->type.ispctlInfo.Listvalue[7].pe_data);
			OstTraceFiltStatic4(TRACE_DEBUG, "FW version : %d.%d.%d %d", (&mENSComponent),(t_uint8)e->type.ispctlInfo.Listvalue[4].pe_data, (t_uint8)e->type.ispctlInfo.Listvalue[5].pe_data, (t_uint8)e->type.ispctlInfo.Listvalue[6].pe_data, (t_uint8)e->type.ispctlInfo.Listvalue[7].pe_data);
			MSG4(" %d.%d %d.%d\n",(t_uint8)e->type.ispctlInfo.Listvalue[8].pe_data,
			    (t_uint8)e->type.ispctlInfo.Listvalue[9].pe_data,
			    (t_uint8)e->type.ispctlInfo.Listvalue[10].pe_data,
			    (t_uint8)e->type.ispctlInfo.Listvalue[11].pe_data);
			OstTraceFiltStatic4(TRACE_DEBUG, " %d.%d %d.%d", (&mENSComponent),(t_uint8)e->type.ispctlInfo.Listvalue[8].pe_data, (t_uint8)e->type.ispctlInfo.Listvalue[9].pe_data, (t_uint8)e->type.ispctlInfo.Listvalue[10].pe_data, (t_uint8)e->type.ispctlInfo.Listvalue[11].pe_data);
#ifdef SensorInformation_u8_serial_id_0_Byte0
			MSG0("FuseId Info");
			for(t_uint8 i = 13; i <= 18; i++ )
			{
				MSG3("   PE[%d] %s = 0x%lX\n", i, pSensor->GetPeName(e->type.ispctlInfo.Listvalue[i].pe_addr), e->type.ispctlInfo.Listvalue[i].pe_data );
			}
#endif

			// Store sensor related data
			t_sensorDesciptor pDesc;
			pDesc.model = (t_uint16)e->type.ispctlInfo.Listvalue[2].pe_data;
			pDesc.manufacturer = (t_uint8) e->type.ispctlInfo.Listvalue[1].pe_data;
			pDesc.revisionNumber = (t_uint8) e->type.ispctlInfo.Listvalue[3].pe_data;
#ifdef SensorInformation_u8_serial_id_0_Byte0
			pDesc.Fuse_Id[0] = (t_uint8) e->type.ispctlInfo.Listvalue[13].pe_data;
			pDesc.Fuse_Id[1] = (t_uint8) e->type.ispctlInfo.Listvalue[14].pe_data;
			pDesc.Fuse_Id[2] = (t_uint8) e->type.ispctlInfo.Listvalue[15].pe_data;
			pDesc.Fuse_Id4   = (t_uint32) (((((t_uint8) e->type.ispctlInfo.Listvalue[16].pe_data)&0xFFFFFFFF)<<16) // Last BYte is unused
									|((((t_uint8) e->type.ispctlInfo.Listvalue[17].pe_data)&0xFFFF)<<8)
									|(((t_uint8) e->type.ispctlInfo.Listvalue[18].pe_data)&0xFF));
			MSG4(" FusId1 = 0x%d, FusId2 = 0x%d, FusId3 = 0x%x, FusId4 = 0x%lx\n",
					pDesc.Fuse_Id[0],pDesc.Fuse_Id[1],pDesc.Fuse_Id[2],pDesc.Fuse_Id4);
#else
			// Fuse Ids are not supported in this FW
			MSG0("FuseIds are not supported\n");
			pDesc.Fuse_Id[0] = 0x00;
			pDesc.Fuse_Id[1] = 0x00;
			pDesc.Fuse_Id[2] = 0x00;
			pDesc.Fuse_Id4 = 0x00;
#endif
			if (pSensor->GetSensorModel() != pDesc.model ||
				 pSensor->GetSensorManufacturer() != pDesc.manufacturer ||
				 pSensor->GetSensorRevNumber() != pDesc.revisionNumber) {
			  MSG0("WARNING ! Predefined sensor identification is different from firmware provided.\n");
			  OstTraceFiltStatic0(TRACE_DEBUG, "WARNING ! Predefined sensor identification is different from firmware provided.", (&mENSComponent));
			  MSG1("Manufacturer (firmware) = 0x%x\n", (unsigned int)pDesc.manufacturer);
			  OstTraceFiltStatic1(TRACE_DEBUG, "Manufacturer (firmware) = 0x%x", (&mENSComponent), (unsigned int)pDesc.manufacturer);
			  MSG1("Model        (firmware) = 0x%x\n", (unsigned int)pDesc.model);
			  OstTraceFiltStatic1(TRACE_DEBUG, "Model        (firmware) = 0x%x", (&mENSComponent), (unsigned int)pDesc.model);
			  MSG1("Revision     (firmware) = 0x%x\n", (unsigned int)pDesc.revisionNumber);
			  OstTraceFiltStatic1(TRACE_DEBUG, "Revision     (firmware) = 0x%x", (&mENSComponent), (unsigned int)pDesc.revisionNumber);
			  MSG1("Manufacturer (defined ) = 0x%x\n", (unsigned int)pSensor->GetSensorManufacturer());
			  OstTraceFiltStatic1(TRACE_DEBUG, "Manufacturer (defined ) = 0x%x", (&mENSComponent), (unsigned int)pSensor->GetSensorManufacturer());
			  MSG1("Model        (defined ) = 0x%x\n", (unsigned int)pSensor->GetSensorModel());
			  OstTraceFiltStatic1(TRACE_DEBUG, "Model        (defined ) = 0x%x", (&mENSComponent), (unsigned int)pSensor->GetSensorModel());
			  MSG1("Revision     (defined ) = 0x%x\n", (unsigned int)pSensor->GetSensorRevNumber());
			  OstTraceFiltStatic1(TRACE_DEBUG, "Revision     (defined ) = 0x%x", (&mENSComponent), (unsigned int)pSensor->GetSensorRevNumber());
			}

			pSensor->SetSensorDescriptor(&pDesc);

			float fFocalLengthMm = (float)e->type.ispctlInfo.Listvalue[12].pe_data / 100.0;
			pSensor->SetFocalLengthMm(fFocalLengthMm);

			SCF_TRANSIT_TO(&CAM_SM::SensorNVM);
			return 0;
		}
		case SCF_STATE_EXIT_SIG:
		{
			EXIT;
			return 0;
		}
		default: break;
	}
	return SCF_STATE_PTR(&CAM_SM::Booting);
}

SCF_STATE CAM_SM::SensorNVM(s_scf_event const *e) {
static Flag_te ReadCdccConfigBeforeBoot;
	switch (e->sig) {
		case SCF_STATE_ENTRY_SIG:
		{
			ENTRY;
			MSG0("SensorNVM-SCF_STATE_ENTRY_SIG\n");
			OstTraceFiltStatic0(TRACE_DEBUG, "SensorNVM-SCF_STATE_ENTRY_SIG", (&mENSComponent));

			/* In certain cases SW3A_STARTED_SIG is received prior to  the completion of loading
             * tuningloader databse xml false. This leads to hsmcamera moving to erroneous state.*/
            /* Flag chkSignal has been added to avoid transitions to state Sensor_Patch improperly */
			chkSignal=false;
			// Read the NVM size and the current coin value
			pIspctlCom->queuePE(Sensor_Tuning_Status_u16_NVM_Data_Size_Byte0,0);
			pIspctlCom->queuePE(Sensor_Tuning_Status_e_Coin_NVM_Status_Byte0,0);
			pIspctlCom->queuePE(Sensor_Tuning_Control_e_Flag_ReadConfigBeforeBoot_Byte0,0);
			pIspctlCom->readQueue();
			return 0;
		}
		case EVT_ISPCTL_LIST_INFO_SIG:
		{
			MSG0("SensorNVM-EVT_ISPCTL_LIST_INFO_SIG\n");
			OstTraceFiltStatic0(TRACE_DEBUG, "SensorNVM-EVT_ISPCTL_LIST_INFO_SIG", (&mENSComponent));
			if(e->type.ispctlInfo.info_id == ISP_READLIST_DONE)
			{
				// This is the "PE Read" acknowledge
				iNvmSize    = (t_uint16)e->type.ispctlInfo.Listvalue[0].pe_data;
				iStatusCoin = (Coin_te) e->type.ispctlInfo.Listvalue[1].pe_data;
				ReadCdccConfigBeforeBoot = (Flag_te) e->type.ispctlInfo.Listvalue[2].pe_data;
				MSG1("NVM Size: %d bytes\n", iNvmSize);
				OstTraceFiltStatic1(TRACE_DEBUG, "NVM Size: %d bytes", (&mENSComponent), iNvmSize);

#ifdef CAMERA_ENABLE_OMX_3A_EXTENSION
				Camera* pCam = (Camera*)&mENSComponent.getProcessingComponent();
				if (pCam->iSkipNVM)
				{
					// The NVM has been exported already, continue anyway.
					pResourceSharerManager->mRSPing.Ping(0);
					return 0;
				}
#endif
				// Check whether the firmware has NVM data
				if( iNvmSize == 0 )
				{
					MSG0("The sensor has no embedded NVM data\n");
					OstTraceFiltStatic0(TRACE_DEBUG, "The sensor has no embedded NVM data", (&mENSComponent));

#if (IMG_CONFIG_NVM_FROM_FILE == IMG_CONFIG_TRUE)
					// We expect the NVM data provided in a file
					t_camera_info camInfo;
					camInfo.manufId = pSensor->GetSensorManufacturer();
					camInfo.modelId = pSensor->GetSensorModel();
					camInfo.revNum  = pSensor->GetSensorRevNumber();
					camInfo.Fuse_Id[0]= pSensor->GetFuseId1(); //FuseId1
					camInfo.Fuse_Id[1]= pSensor->GetFuseId2(); //FuseId2
					camInfo.Fuse_Id[2]= pSensor->GetFuseId3(); //FuseId3
					camInfo.Fuse_Id4  = pSensor->GetFuseId4(); //FuseId4
					// Raw Processing type
					camInfo.ePreset =(preset_type)ePreset;

					MSG0("SensorNVM: loading NVM tuning data\n");
					OstTraceFiltStatic0(TRACE_DEBUG, "SensorNVM: loading NVM tuning data", (&mENSComponent));
					TuningLoaderMgrError_t tlmErr = TUNING_LOADER_MGR_OK;
					tlmErr = pTuningLoaderManager->loadNvmTuningData(&camInfo);
					if(tlmErr != TUNING_LOADER_MGR_OK) {
						DBGT_ERROR("SensorNVM: NVM tuning data loading failed err=%d (%s)\n",
							 tlmErr, CTuningLoaderManager::errorCode2String(tlmErr));
						OstTraceFiltStatic1(TRACE_ERROR, "SensorNVM: NVM tuning data loading failed err=%d", (&mENSComponent),tlmErr);
						DBC_ASSERT(0);
						return 0;
					}
					return 0;
#else
					// The sensor has no embedded NVM, continue anyway.
					pResourceSharerManager->mRSPing.Ping(0);
					return 0;
#endif
				}
				else
				{
					OMX_BOOL readFromFirmware;
#if (IMG_CONFIG_SENSORNVM_READ == 0) 
					readFromFirmware = OMX_TRUE;
#else 
						readFromFirmware = OMX_FALSE;
					
						MSG0("The sensor has embedded NVM data :- first try loading from file\n");
						OstTraceFiltStatic0(TRACE_DEBUG, "The sensor has embedded NVM data :- first try loading from file ", (&mENSComponent));
						t_camera_info camInfo;
						camInfo.manufId = pSensor->GetSensorManufacturer();
						camInfo.modelId = pSensor->GetSensorModel();
						camInfo.revNum  = pSensor->GetSensorRevNumber();
						camInfo.Fuse_Id[0]= pSensor->GetFuseId1(); //FuseId1
						camInfo.Fuse_Id[1]= pSensor->GetFuseId2(); //FuseId2
						camInfo.Fuse_Id[2]= pSensor->GetFuseId3(); //FuseId3
						camInfo.Fuse_Id4  = pSensor->GetFuseId4(); //FuseId4
						MSG0("SensorNVM: loading NVM tuning data\n");
						OstTraceFiltStatic0(TRACE_DEBUG, "SensorNVM: loading NVM tuning data", (&mENSComponent));
						TuningLoaderMgrError_t tlmErr = TUNING_LOADER_MGR_OK;
						tlmErr = pTuningLoaderManager->loadNvmTuningData(&camInfo); //*IMP. NOTE*: the Sensor NVM object will be created inside this function
						if(tlmErr != TUNING_LOADER_MGR_OK) {
							DBGT_ERROR("SensorNVM: NVM tuning data loading failed err=%d (%s)\n. Now Caching it at tuning loader if available at firmware",
							 tlmErr, CTuningLoaderManager::errorCode2String(tlmErr));
							OstTraceFiltStatic1(TRACE_ERROR, "SensorNVM: NVM tuning data loading failed err=%d", (&mENSComponent),tlmErr);
							readFromFirmware = OMX_TRUE;
							
						}
#endif
#if	(IMG_CONFIG_SENSORNVM_READ == 1)	
if(readFromFirmware == OMX_TRUE) 
{
							DBC_ASSERT(0);
							return 0;
						}
#elif (IMG_CONFIG_SENSORNVM_READ == 2) 
if(readFromFirmware == OMX_TRUE) 
						{
							return 0;
						}
#endif																

					if (readFromFirmware == OMX_TRUE)
					{
						MSG0("The sensor module has NVM data => download it through firmware\n");
						OstTraceFiltStatic0(TRACE_DEBUG, "The sensor module has NVM data => download it through firmware", (&mENSComponent));
						// Check that NVM data will fit into the shared memory
						if( (csm_uint32_t)iNvmSize > pNvmShmChunk->size) {
							MSG0("SensorNVM: NVM Shared memory too small:\n");
							OstTraceFiltStatic0(TRACE_DEBUG, "SensorNVM: NVM Shared memory too small:", (&mENSComponent));
							MSG1("SensorNVM: NVM actual size    = %d\n", iNvmSize);
							OstTraceFiltStatic1(TRACE_DEBUG, "SensorNVM: NVM actual size    = %d", (&mENSComponent), iNvmSize);
							MSG1("SensorNVM: Shared memory size = %ld\n", pNvmShmChunk->size);
							OstTraceFiltStatic1(TRACE_DEBUG, "SensorNVM: Shared memory size = %ld", (&mENSComponent), pNvmShmChunk->size);
								DBC_ASSERT(0);
								return 0;
							}
						// Send the buffer address to XP70 (where NVM has to be copied) and toggle the coin
						pIspctlCom->queuePE(Sensor_Tuning_Control_u32_NVM_Data_Address_Byte0, (t_uint32)pNvmShmChunk->ispLogicalAddress);
						pIspctlCom->queuePE(Sensor_Tuning_Control_e_Coin_NVM__Control_Byte0,(t_uint32)(iStatusCoin==Coin_e_Heads ? Coin_e_Tails : Coin_e_Heads));
						pIspctlCom->processQueue();
						// Stay in this state in order to catch PE write notification
						return 0;
						}
					else{
					return 0;
					}
				} 
			}
			else if (e->type.ispctlInfo.info_id == ISP_WRITELIST_DONE)
			{
				// This is the "PE write" acknowledge
				// Stay in this state to catch ISP_NVM_EXPORT_DONE notification
				return 0;
			}
			else break;
		}
		case TUNING_LOADER_OPERATION_FAILED_SIG:
		{
			MSG0("SensorNVM-TUNING_LOADER_OPERATION_FAILED_SIG\n");
			OstTraceFiltStatic0(TRACE_DEBUG, "SensorNVM-TUNING_LOADER_OPERATION_FAILED_SIG", (&mENSComponent));
			DBGT_ERROR("SensorNVM: failed to load NVM tuning data err=%ld (%s)\n",
			e->type.tuningLoaderEvent.error, pTuningLoaderManager->loaderErrorCode2String(e->type.tuningLoaderEvent.error));
			OstTraceFiltStatic1(TRACE_ERROR, "SensorNVM: failed to load NVM tuning data err=%ld", (&mENSComponent), e->type.tuningLoaderEvent.error);
			if(chkSignal==true)
			{
			    chkSignal=false;
				pResourceSharerManager->mRSPing.Ping(0);
				return 0;
			}

			// Check whether it is a 'file not found' error
			if(e->type.tuningLoaderEvent.error == (t_sint32)TUNING_LOADER_FILE_NOT_FOUND)
			{
				if (iNvmSize != 0)
				{
					MSG0("The sensor module has NVM data => download it from fw\n");
					OstTraceFiltStatic0(TRACE_DEBUG, "The sensor module has NVM data => download it from fw", (&mENSComponent));

					// Check that NVM data will fit into the shared memory
					if( (csm_uint32_t)iNvmSize > pNvmShmChunk->size) {
						MSG0("SensorNVM: NVM Shared memory too small:\n");
						OstTraceFiltStatic0(TRACE_DEBUG, "SensorNVM: NVM Shared memory too small:", (&mENSComponent));
						MSG1("SensorNVM: NVM actual size    = %d\n", iNvmSize);
						OstTraceFiltStatic1(TRACE_DEBUG, "SensorNVM: NVM actual size    = %d", (&mENSComponent), iNvmSize);
						MSG1("SensorNVM: Shared memory size = %ld\n", pNvmShmChunk->size);
						OstTraceFiltStatic1(TRACE_DEBUG, "SensorNVM: Shared memory size = %ld", (&mENSComponent), pNvmShmChunk->size);
						DBC_ASSERT(0);
						return 0;
					}

					// Send the buffer address to XP70 (where NVM has to be copied) and toggle the coin
					pIspctlCom->queuePE(Sensor_Tuning_Control_u32_NVM_Data_Address_Byte0, (t_uint32)pNvmShmChunk->ispLogicalAddress);
					pIspctlCom->queuePE(Sensor_Tuning_Control_e_Coin_NVM__Control_Byte0,(t_uint32)(iStatusCoin==Coin_e_Heads ? Coin_e_Tails : Coin_e_Heads));
					pIspctlCom->processQueue();
					// Stay in this state in order to catch PE write notif
					return 0;

				}
				else
				{
					if(ePreset == OMX_SYMBIAN_RawImageUnprocessed)
					{
						// "Normal" failure, we are in a sensor calibration phase
						// IL client is in the process of generating NVM calibration data into file
						MSG0("SensorNVM: customer calibration phase detected => continue anyway\n");
						OstTraceFiltStatic0(TRACE_DEBUG, "SensorNVM: customer calibration phase detected => continue anyway", (&mENSComponent));
						pResourceSharerManager->mRSPing.Ping(0);
						return 0;
					}
					else
					{
						// Abnormal failure: seems we are not in a sensor calibration phase
						// but NVM calibration data were not found
						DBGT_ERROR("SensorNVM: Abnormal situation: NVM tuning data should be there\n");
						OstTraceFiltStatic0(TRACE_ERROR, "SensorNVM: Abnormal situation: NVM tuning data should be there", (&mENSComponent));
						// Notify the IL client
						mENSComponent.eventHandler(OMX_EventError, (OMX_U32)OMX_ErrorContentPipeOpenFailed, e->type.capturingStruct.nPortIndex);
						DBC_ASSERT(0);
						return 0;
					}
				}
			}
			else
			{
				DBGT_ERROR("SensorNVM: unexpected TuningLoader error\n");
				OstTraceFiltStatic0(TRACE_ERROR, "SensorNVM: unexpected TuningLoader error", (&mENSComponent));
				DBC_ASSERT(0);
				return 0;
			}
		}
		case TUNING_LOADER_OPERATION_DONE_SIG:
		{
			MSG0("SensorNVM-TUNING_LOADER_OPERATION_DONE_SIG\n");

			if(chkSignal==true)
			{
				chkSignal=false;
				pResourceSharerManager->mRSPing.Ping(0);
				return 0;
			}

			OstTraceFiltStatic0(TRACE_DEBUG, "SensorNVM-TUNING_LOADER_OPERATION_DONE_SIG", (&mENSComponent));
			MSG0("SensorNVM: NVM data sucessfully loaded\n");
			OstTraceFiltStatic0(TRACE_DEBUG, "SensorNVM: NVM data sucessfully loaded", (&mENSComponent));
			#ifdef CAMERA_ENABLE_OMX_3A_EXTENSION

			CTuningBinData* pSensorNvmObj = NULL;
			t_tuning_bin_data_error tbdErr = TUNING_BIN_OK;
			MSG0("SensorNVM [read from MMIO]: Retrieving the Sensor NVM tuning object\n");
			OstTraceFiltStatic0(TRACE_DEBUG, "SensorNVM [read from File]: Retrieving the Sensor NVM tuning object", (&mENSComponent));
			pSensorNvmObj = pTuningDataBase->getObject(TUNING_OBJ_SENSOR_NVM_DATA);
			((Sw3AProcessingCompExt*)p3AComponentManager)->SetNVMData((t_uint8*)pSensorNvmObj->getAddr());
			#endif
			pResourceSharerManager->mRSPing.Ping(0);
			return 0;
		}
		case EVT_ISPCTL_INFO_SIG:
		{
			MSG0("SensorNVM-EVT_ISPCTL_INFO_SIG\n");
			OstTraceFiltStatic0(TRACE_DEBUG, "SensorNVM-EVT_ISPCTL_INFO_SIG", (&mENSComponent));

			// Here, we expect event ISP_NVM_EXPORT_DONE from the firmware
			if (e->type.ispctlInfo.info_id != ISP_NVM_EXPORT_DONE) break;

			// NVM export notification
			MSG0("SensorNVM: NVM export completed\n");
			OstTraceFiltStatic0(TRACE_DEBUG, "SensorNVM: NVM export completed", (&mENSComponent));

			// Discard the cache so that we read consistent data from the shared memory
			// (i.e. FW wrote into memory so force reading memory and not the cache)
			camSharedMemError_t shmErr = CAM_SHARED_MEM_ERR_NONE;
			MSG0("SensorNVM: flushing the shared memory cache\n");
			OstTraceFiltStatic0(TRACE_DEBUG, "SensorNVM: flushing the shared memory cache", (&mENSComponent));
			shmErr = pSharedMemory->cacheInvalidate(CAM_SHARED_MEM_CHUNK_SENSOR_NVM);
			if (shmErr != CAM_SHARED_MEM_ERR_NONE) {
				DBGT_ERROR("SensorNVM: Failed to invalidate shared memory chunk, err=%d\n", shmErr);
				OstTraceFiltStatic1(TRACE_ERROR, "SensorNVM: Failed to invalidate shared memory chunk, err=%d", (&mENSComponent), shmErr);
				DBC_ASSERT(0);
				return 0;
			}

#ifdef CAMERA_ENABLE_OMX_3A_EXTENSION
			((Sw3AProcessingCompExt*)p3AComponentManager)->SetNVMData((t_uint8*)pNvmShmChunk->armLogicalAddress);
#else // CAMERA_ENABLE_OMX_3A_EXTENSION
			// Construct the NVM object of the tuning database
			CTuningBinData* pSensorNvmObj = NULL;
			t_tuning_bin_data_error tbdErr = TUNING_BIN_OK;
			MSG0("SensorNVM: constructing the Sensor NVM tuning object\n");
			OstTraceFiltStatic0(TRACE_DEBUG, "SensorNVM: constructing the Sensor NVM tuning object", (&mENSComponent));
			pSensorNvmObj = pTuningDataBase->getObject(TUNING_OBJ_SENSOR_NVM_DATA);
			tbdErr = pSensorNvmObj->construct( iNvmSize, pNvmShmChunk->armLogicalAddress);
			if( tbdErr != TUNING_BIN_OK) {
				DBGT_ERROR("SensorNVM: failed to construct TUNING_OBJ_SENSOR_NVM_DATA tuning object err=%d (%s)\n",
					     tbdErr, CTuningBinData::errorCode2String(tbdErr));
				OstTraceFiltStatic1(TRACE_ERROR, "SensorNVM: failed to construct TUNING_OBJ_SENSOR_NVM_DATA tuning object err=%d", (&mENSComponent), tbdErr);
				DBC_ASSERT(0);
				return 0;
			}
#endif // CAMERA_ENABLE_OMX_3A_EXTENSION

			// NVM retrieval done
			
#if (IMG_CONFIG_SENSORNVM_READ != 0) 
			//Write to afile
			t_camera_info camInfo;
			TuningLoaderMgrError_t tlmErr = TUNING_LOADER_MGR_OK;
			camInfo.manufId = pSensor->GetSensorManufacturer();
			camInfo.modelId = pSensor->GetSensorModel();
			camInfo.revNum  = pSensor->GetSensorRevNumber();
			camInfo.Fuse_Id[0]= pSensor->GetFuseId1(); //FuseId1
			camInfo.Fuse_Id[1]= pSensor->GetFuseId2(); //FuseId2
			camInfo.Fuse_Id[2]= pSensor->GetFuseId3(); //FuseId3
			camInfo.Fuse_Id4  = pSensor->GetFuseId4(); //FuseId4
			MSG0("SensorNVM: save NVM tuning data to a file\n");
			tlmErr = pTuningLoaderManager->saveNvmTuningData(&camInfo,pNvmShmChunk->armLogicalAddress,iNvmSize);
			if(tlmErr != TUNING_LOADER_MGR_OK) {
				DBGT_ERROR("SensorNVM: NVM tuning data loading failed err=%d (%s)\n",
				 tlmErr, CTuningLoaderManager::errorCode2String(tlmErr));
				OstTraceFiltStatic1(TRACE_ERROR, "SensorNVM: NVM tuning data loading failed err=%d", (&mENSComponent),tlmErr);
				DBC_ASSERT(0);
				return 0;
			}
		chkSignal=true;
#else /*(IMG_CONFIG_SENSORNVM_READ == 0)*/
			pResourceSharerManager->mRSPing.Ping(0);
#endif
			return 0;
		}
		case Q_PONG_SIG:
		{
			MSG0("SensorNVM-Q_PONG_SIG\n");
			OstTraceFiltStatic0(TRACE_DEBUG, "SensorNVM-Q_PONG_SIG", (&mENSComponent));
			if (ReadCdccConfigBeforeBoot == Flag_e_FALSE)
				SCF_TRANSIT_TO(&CAM_SM::FirmwareTuning_CheckIfSupported);
			else 	SCF_TRANSIT_TO(&CAM_SM::FlashTuning_LoadData);
			return 0;
		}
		case SCF_STATE_EXIT_SIG:
		{
			EXIT;
			return 0;
		}
		default: break;
	}
	return SCF_STATE_PTR(&CAM_SM::Booting);
}


SCF_STATE CAM_SM::FirmwareTuning_CheckIfSupported(s_scf_event const *e) {
	switch (e->sig) {
		case SCF_STATE_ENTRY_SIG:
		{
			ENTRY;
			MSG0("FirmwareTuning_CheckIfSupported-SCF_STATE_ENTRY_SIG\n");
			OstTraceFiltStatic0(TRACE_DEBUG, "FirmwareTuning_CheckIfSupported-SCF_STATE_ENTRY_SIG", (&mENSComponent));
			// Ask to firmware how many tuning blocks are supported and get current coin value
			pIspctlCom->queuePE(Sensor_Tuning_Status_u16_TotalSubBlockIdsCount_Byte0,0);
			pIspctlCom->queuePE(Sensor_Tuning_Status_e_Coin_SubBlock_Status_Byte0,0);
			pIspctlCom->readQueue();
			return 0;
		}
		case EVT_ISPCTL_LIST_INFO_SIG:
		{
			MSG0("FirmwareTuning_CheckIfSupported-EVT_ISPCTL_LIST_INFO_SIG\n");
			OstTraceFiltStatic0(TRACE_DEBUG, "FirmwareTuning_CheckIfSupported-EVT_ISPCTL_LIST_INFO_SIG", (&mENSComponent));
			if(e->type.ispctlInfo.info_id != ISP_READLIST_DONE) break;

			// Store the Firmware tuning info related data
			iFwSubBlockIdsCount = (t_uint16)e->type.ispctlInfo.Listvalue[0].pe_data;
			iStatusCoin         = (Coin_te) e->type.ispctlInfo.Listvalue[1].pe_data;

			if(iFwSubBlockIdsCount==0)
			{
				// Firmware does not support tuning
				MSG0("   Tuning not supported by the firmware\n");
				OstTraceFiltStatic0(TRACE_DEBUG, "   Tuning not supported by the firmware", (&mENSComponent));
				pFwSubBlockId = NULL;
				SCF_TRANSIT_TO(&CAM_SM::FlashNVM);
				return 0;
			}
			else
			{
				// Firmware supports tuning: retrieve the supported IDs
				MSG1("   Tuning supported by the firmware: %d FwSubBlocks supported\n", iFwSubBlockIdsCount);
				OstTraceFiltStatic1(TRACE_DEBUG, "   Tuning supported by the firmware: %d FwSubBlocks supported", (&mENSComponent), iFwSubBlockIdsCount);
				iStatusCoinRetriesCount = 0;
				iCurrentFwSubBlockIndex = 0;
				pFwSubBlockId = new t_uint16[iFwSubBlockIdsCount];
				if(pFwSubBlockId==NULL) {
					DBGT_ERROR("   Failed to allocate %d entries for FwSubBlock IDs\n", iFwSubBlockIdsCount);
					OstTraceFiltStatic1(TRACE_ERROR, "   Failed to allocate %d entries for FwSubBlock IDs", (&mENSComponent), iFwSubBlockIdsCount);
					DBC_ASSERT(0);
					return 0;
				}
				MSG2("   Allocated pFwSubBlockId=%p (%d entries)\n", pFwSubBlockId, iFwSubBlockIdsCount);
				OstTraceFiltStatic2(TRACE_DEBUG, "   Allocated pFwSubBlockId=0x%x (%d entries)", (&mENSComponent), (t_uint32)pFwSubBlockId, iFwSubBlockIdsCount);
				SCF_TRANSIT_TO(&CAM_SM::FirmwareTuning_QueryBlockId);
				return 0;
			}
		}
		case SCF_STATE_EXIT_SIG:
		{
			EXIT;
			return 0;
		}
		default: break;
	}
	return SCF_STATE_PTR(&CAM_SM::Booting);
}

SCF_STATE CAM_SM::FirmwareTuning_QueryBlockId(s_scf_event const *e) {
	switch (e->sig) {
		case SCF_STATE_ENTRY_SIG:
		{
			ENTRY;
			// Ask the next block ID and toggle the coin
			MSG2("   Query Block[%d], coin state=%d\n", iCurrentFwSubBlockIndex, (int)iStatusCoin);
			OstTraceFiltStatic2(TRACE_DEBUG, "   Query Block[%d], coin state=%d", (&mENSComponent), iCurrentFwSubBlockIndex, (int)iStatusCoin);
			pIspctlCom->queuePE(Sensor_Tuning_Control_u16_SelectedSubBlockIdIndex_Byte0,(t_uint32)iCurrentFwSubBlockIndex);
			pIspctlCom->queuePE(Sensor_Tuning_Control_e_Coin_SubBlock_Control_Byte0,(t_uint32)(iStatusCoin==Coin_e_Heads ? Coin_e_Tails : Coin_e_Heads));
			pIspctlCom->processQueue();
			return 0;
		}
		case EVT_ISPCTL_LIST_INFO_SIG:
		{
			MSG0("FirmwareTuning_QueryBlockId-EVT_ISPCTL_LIST_INFO_SIG\n");
			OstTraceFiltStatic0(TRACE_DEBUG, "FirmwareTuning_QueryBlockId-EVT_ISPCTL_LIST_INFO_SIG", (&mENSComponent));
			if (e->type.ispctlInfo.info_id != ISP_WRITELIST_DONE) break;
			SCF_TRANSIT_TO(&CAM_SM::FirmwareTuning_CheckCoin);
			return 0;
		}
		case SCF_STATE_EXIT_SIG:
		{
			EXIT;
			return 0;
		}
		default: break;
	}
	return SCF_STATE_PTR(&CAM_SM::Booting);
}

SCF_STATE CAM_SM::FirmwareTuning_CheckCoin(s_scf_event const *e) {
	switch (e->sig) {
		case SCF_STATE_ENTRY_SIG:
		{
			ENTRY;
			MSG0("FirmwareTuning_CheckCoin-SCF_STATE_ENTRY_SIG\n");
			OstTraceFiltStatic0(TRACE_DEBUG, "FirmwareTuning_CheckCoin-SCF_STATE_ENTRY_SIG", (&mENSComponent));
			// Get coin value
			pIspctlCom->queuePE(Sensor_Tuning_Status_e_Coin_SubBlock_Status_Byte0,0);
			pIspctlCom->readQueue();
			return 0;
		}
		case EVT_ISPCTL_LIST_INFO_SIG:
		{
			MSG0("FirmwareTuning_CheckCoin-EVT_ISPCTL_LIST_INFO_SIG\n");
			OstTraceFiltStatic0(TRACE_DEBUG, "FirmwareTuning_CheckCoin-EVT_ISPCTL_LIST_INFO_SIG", (&mENSComponent));
			if (e->type.ispctlInfo.info_id != ISP_READLIST_DONE) break;
//#if defined (OSI_DEBUG)
			for(t_uint16 i=0; i < e->type.ispctlInfo.number_of_pe; i++) {
				MSG4("   PE[%d] 0x%X (%s) = 0x%lX\n", i, e->type.ispctlInfo.Listvalue[i].pe_addr, pSensor->GetPeName(e->type.ispctlInfo.Listvalue[i].pe_addr), e->type.ispctlInfo.Listvalue[i].pe_data );
				//OstTraceFiltStatic4(TRACE_DEBUG, "   PE[%d] 0x%X (%s) = 0x%lX\n", i, e->type.ispctlInfo.Listvalue[i].pe_addr, CXp70::GetPeName(e->type.ispctlInfo.Listvalue[i].pe_addr), e->type.ispctlInfo.Listvalue[i].pe_data );
			}
//#endif
			if( (Coin_te)e->type.ispctlInfo.Listvalue[0].pe_data == iStatusCoin) {
				// Coin state has not changed yet
				if(iStatusCoinRetriesCount>=10) {
					MSG0("*****   Coin state unchanged, too many attempts *****\n");
					OstTraceFiltStatic0(TRACE_DEBUG, "*****   Coin state unchanged, too many attempts *****", (&mENSComponent));
					DBC_ASSERT(0);
					return 0;
				}
				MSG1("   Coin state unchanged (state=%d) => try again\n", (int)iStatusCoin);
				OstTraceFiltStatic1(TRACE_DEBUG, "   Coin state unchanged (state=%d) => try again", (&mENSComponent), (int)iStatusCoin);
				iStatusCoinRetriesCount++;
				pIspctlCom->queuePE(Sensor_Tuning_Status_e_Coin_SubBlock_Status_Byte0,0);
				pIspctlCom->readQueue();
			}
			else {
				// Coin state changed => we can read the block ID value
				iStatusCoin = (Coin_te) e->type.ispctlInfo.Listvalue[0].pe_data;
				MSG1("   Coin state unchanged (state=%d) => read Block ID value\n", (int)iStatusCoin);
				OstTraceFiltStatic1(TRACE_DEBUG, "   Coin state unchanged (state=%d) => read Block ID value", (&mENSComponent), (int)iStatusCoin);
				iStatusCoinRetriesCount = 0;
				SCF_TRANSIT_TO(&CAM_SM::FirmwareTuning_ReadBlockId);
			}
			return 0;
		}
		case SCF_STATE_EXIT_SIG:
		{
			EXIT;
			return 0;
		}
		default: break;
	}
	return SCF_STATE_PTR(&CAM_SM::Booting);
}

SCF_STATE CAM_SM::FirmwareTuning_ReadBlockId(s_scf_event const *e) {
	switch (e->sig) {
		case SCF_STATE_ENTRY_SIG:
		{
			ENTRY;
			MSG0("FirmwareTuning_ReadBlockId-SCF_STATE_ENTRY_SIG\n");
			OstTraceFiltStatic0(TRACE_DEBUG, "FirmwareTuning_ReadBlockId-SCF_STATE_ENTRY_SIG", (&mENSComponent));
			pIspctlCom->queuePE(Sensor_Tuning_Status_u16_CurrentSubBlockIdValue_Byte0,0);
			pIspctlCom->readQueue();
			return 0;
		}
		case EVT_ISPCTL_LIST_INFO_SIG:
		{
			MSG0("FirmwareTuning_ReadBlockId-EVT_ISPCTL_LIST_INFO_SIG\n");
			OstTraceFiltStatic0(TRACE_DEBUG, "FirmwareTuning_ReadBlockId-EVT_ISPCTL_LIST_INFO_SIG", (&mENSComponent));
			if (e->type.ispctlInfo.info_id != ISP_READLIST_DONE) break;
//#if defined (OSI_DEBUG)
			for(t_uint16 i=0; i < e->type.ispctlInfo.number_of_pe; i++) {
				MSG3("   PE[%d] %s = 0x%lX\n", i, pSensor->GetPeName(e->type.ispctlInfo.Listvalue[i].pe_addr), e->type.ispctlInfo.Listvalue[i].pe_data );
				//OstTraceFiltStatic3(TRACE_DEBUG, "   PE[%d] %s = 0x%lX\n", i, CXp70::GetPeName(e->type.ispctlInfo.Listvalue[i].pe_addr), e->type.ispctlInfo.Listvalue[i].pe_data );
			}
//#endif
			// Store tuning BlockId value
			pFwSubBlockId[iCurrentFwSubBlockIndex] = (t_uint16)e->type.ispctlInfo.Listvalue[0].pe_data;
			MSG2("   FwSubBlockId[%d] = ID%d\n", iCurrentFwSubBlockIndex, pFwSubBlockId[iCurrentFwSubBlockIndex]);
			OstTraceFiltStatic2(TRACE_DEBUG, "   FwSubBlockId[%d] = ID%d", (&mENSComponent), iCurrentFwSubBlockIndex, pFwSubBlockId[iCurrentFwSubBlockIndex]);
			// Another block to be fetched ?
			iCurrentFwSubBlockIndex++;
			if(iCurrentFwSubBlockIndex<iFwSubBlockIdsCount) {
				// Still some tuning blocks IDs are to be fetched
				MSG0("   Some blocks ID are to be fetched\n");
				OstTraceFiltStatic0(TRACE_DEBUG, "   Some blocks ID are to be fetched", (&mENSComponent));
				SCF_TRANSIT_TO(&CAM_SM::FirmwareTuning_QueryBlockId);
			}
			else {
				// All tuning blocks IDs were fetched
				MSG1("   All block IDs were fetched: total %d\n", iFwSubBlockIdsCount);
				OstTraceFiltStatic1(TRACE_DEBUG, "   All block IDs were fetched: total %d", (&mENSComponent), iFwSubBlockIdsCount);
//#if defined (OSI_DEBUG)
				for(t_uint16 i=0; i<iFwSubBlockIdsCount; i++) {
					MSG2("   FwSubBlockId[%d] = ID%d\n", i, pFwSubBlockId[i]);
					OstTraceFiltStatic2(TRACE_DEBUG, "   FwSubBlockId[%d] = ID%d", (&mENSComponent), i, pFwSubBlockId[i]);
				}
//#endif
				SCF_TRANSIT_TO(&CAM_SM::FirmwareTuning_LoadData);
			}
			return 0;
		}
		case SCF_STATE_EXIT_SIG:
		{
			EXIT;
			return 0;
		}
		default: break;
	}
	return SCF_STATE_PTR(&CAM_SM::Booting);
}

SCF_STATE CAM_SM::FirmwareTuning_LoadData(s_scf_event const *e) {
	switch (e->sig) {
		case SCF_STATE_ENTRY_SIG:
		{
			MSG0("FirmwareTuning_LoadData-SCF_STATE_ENTRY_SIG\n");
			OstTraceFiltStatic0(TRACE_DEBUG, "FirmwareTuning_LoadData-SCF_STATE_ENTRY_SIG", (&mENSComponent));
			TuningLoaderMgrError_t tlmErr = TUNING_LOADER_MGR_OK;

			t_camera_info camInfo;
			// Initialisation to remove Coverity Warning
			camInfo.manufId = 0;
			camInfo.modelId = 0;
			camInfo.revNum = 0;
			camInfo.Fuse_Id[0] = 0;
			camInfo.Fuse_Id[1] = 0;
			camInfo.Fuse_Id[2] = 0;
			camInfo.Fuse_Id4   = 0;
			
			camInfo.manufId = pSensor->GetSensorManufacturer();
			camInfo.modelId = pSensor->GetSensorModel();
			camInfo.revNum  = pSensor->GetSensorRevNumber();

			t_fw_blocks_info fwBlocks;
			fwBlocks.subBlockCount = iFwSubBlockIdsCount;
			fwBlocks.pSubBlockId   = pFwSubBlockId;

			MSG0("FirmwareTuning_LoadData: loading firmware tuning data\n");
			OstTraceFiltStatic0(TRACE_DEBUG, "FirmwareTuning_LoadData: loading firmware tuning data", (&mENSComponent));
			tlmErr = pTuningLoaderManager->loadFirmwareTuningData(&camInfo, &fwBlocks);
			if(tlmErr != TUNING_LOADER_MGR_OK) {
				DBGT_ERROR("FirmwareTuning_LoadData: firmware tuning data loading failed err=%d (%s)\n",
					     tlmErr, CTuningLoaderManager::errorCode2String(tlmErr));
				OstTraceFiltStatic1(TRACE_ERROR, "FirmwareTuning_LoadData: firmware tuning data loading failed err=%d", (&mENSComponent), tlmErr);
				DBC_ASSERT(0);
				return 0;
			}
			return 0;
		}
		case TUNING_LOADER_OPERATION_DONE_SIG:
		{
			MSG0("FirmwareTuning_LoadData-TUNING_LOADER_OPERATION_DONE_SIG\n");
			OstTraceFiltStatic0(TRACE_DEBUG, "FirmwareTuning_LoadData-TUNING_LOADER_OPERATION_DONE_SIG", (&mENSComponent));
			MSG2("Deleting pFwSubBlockId=%p (%d entries)\n", pFwSubBlockId, iFwSubBlockIdsCount);
			OstTraceFiltStatic2(TRACE_DEBUG, "Deleting pFwSubBlockId=0x%x (%d entries)", (&mENSComponent), (t_uint32) pFwSubBlockId, iFwSubBlockIdsCount);
			delete [] pFwSubBlockId;
			pFwSubBlockId = NULL;
			SCF_TRANSIT_TO(&CAM_SM::FirmwareTuning_SendDataToFirmware);
			return 0;
		}
		case TUNING_LOADER_OPERATION_FAILED_SIG:
		{
			DBGT_ERROR("FirmwareTuning_LoadData-TUNING_LOADER_OPERATION_FAILED_SIG\n");
			OstTraceFiltStatic0(TRACE_ERROR, "FirmwareTuning_LoadData-TUNING_LOADER_OPERATION_FAILED_SIG", (&mENSComponent));
			DBGT_ERROR("FirmwareTuning_LoadData: failed to load firmware tuning data err=%ld (%s)\n",
				 e->type.tuningLoaderEvent.error, pTuningLoaderManager->loaderErrorCode2String(e->type.tuningLoaderEvent.error));
			OstTraceFiltStatic1(TRACE_ERROR, "FirmwareTuning_LoadData: failed to load firmware tuning data err=%ld", (&mENSComponent), e->type.tuningLoaderEvent.error);
			DBC_ASSERT(0);
			return 0;
		}
		case SCF_STATE_EXIT_SIG:
		{
			EXIT;
			return 0;
		}
		default: break;
	}
	return SCF_STATE_PTR(&CAM_SM::Booting);
}

SCF_STATE CAM_SM::FirmwareTuning_SendDataToFirmware(s_scf_event const *e) {
	switch (e->sig) {
		case SCF_STATE_ENTRY_SIG:
		{
			ENTRY;
			MSG0("FirmwareTuning_SendDataToFirmware-SCF_STATE_ENTRY_SIG\n");
			OstTraceFiltStatic0(TRACE_DEBUG, "FirmwareTuning_SendDataToFirmware-SCF_STATE_ENTRY_SIG", (&mENSComponent));
			CTuningBinData* pFwTuningObj = NULL;

			// Get the firmware tuning object
			pFwTuningObj = pTuningDataBase->getObject(TUNING_OBJ_FIRMWARE_DATA);

			// Check that the firmware tuning data fit into the shared memory chunk
			if( (csm_uint32_t)pFwTuningObj->getSize() > pFwShmChunk->size) {
				MSG0("FirmwareTuning_SendDataToFirmware: Tuning data shared chunk too small:\n");
				OstTraceFiltStatic0(TRACE_DEBUG, "FirmwareTuning_SendDataToFirmware: Tuning data shared chunk too small:", (&mENSComponent));
				MSG1("FirmwareTuning_SendDataToFirmware: Shared mem size  = %ld\n", pFwShmChunk->size);
				OstTraceFiltStatic1(TRACE_DEBUG, "FirmwareTuning_SendDataToFirmware: Shared mem size  = %ld", (&mENSComponent), pFwShmChunk->size);
				MSG1("FirmwareTuning_SendDataToFirmware: Actual data size = %d\n", pFwTuningObj->getSize());
				OstTraceFiltStatic1(TRACE_DEBUG, "FirmwareTuning_SendDataToFirmware: Actual data size = %d", (&mENSComponent), pFwTuningObj->getSize());
				DBC_ASSERT(0);
				return 0;
			}

			// Copy the tuning data into the firmware address space
			MSG3("FirmwareTuning_SendDataToFirmware: Copying data %p, size %d, into firwmare address space %p\n", pFwTuningObj->getAddr(), pFwTuningObj->getSize(), pFwShmChunk->ispLogicalAddress);
			OstTraceFiltStatic3(TRACE_DEBUG, "FirmwareTuning_SendDataToFirmware: Copying data 0x%x, size %d, into firwmare address space 0x%x", (&mENSComponent), (t_uint32) pFwTuningObj->getAddr(), pFwTuningObj->getSize(), (t_uint32) pFwShmChunk->ispLogicalAddress);
            if (NULL != pFwTuningObj->getAddr()) {
                memcpy(pFwShmChunk->armLogicalAddress, pFwTuningObj->getAddr(), pFwTuningObj->getSize());
            }
            else {
                DBC_ASSERT(0);
                return 0;
            }

			// Flush the memory cache (so that consistent data are read by firmware)
			camSharedMemError_t shmErr = CAM_SHARED_MEM_ERR_NONE;
			shmErr = pSharedMemory->cacheClean(CAM_SHARED_MEM_CHUNK_FW_CONFIG);
			if (shmErr != CAM_SHARED_MEM_ERR_NONE) {
				DBGT_ERROR("FirmwareTuning_SendDataToFirmware: Failed to flush shared memory chunk, err=%d\n", shmErr);
				OstTraceFiltStatic1(TRACE_ERROR, "FirmwareTuning_SendDataToFirmware: Failed to flush shared memory chunk, err=%d", (&mENSComponent), shmErr);
				DBC_ASSERT(0);
				return 0;
			}

			// Send the address (firmware address space) to XP70
			pIspctlCom->queuePE(Sensor_Tuning_Control_u32_SubBlock_Data_Address_Byte0, (t_uint32)pFwShmChunk->ispLogicalAddress );
			pIspctlCom->processQueue();
			return 0;
		}
		case SCF_STATE_EXIT_SIG:
		{
			EXIT;
			return 0;
		}
		case EVT_ISPCTL_LIST_INFO_SIG:
		{
			MSG0("FirmwareTuning_SendDataToFirmware-EVT_ISPCTL_LIST_INFO_SIG\n");
			OstTraceFiltStatic0(TRACE_DEBUG, "FirmwareTuning_SendDataToFirmware-EVT_ISPCTL_LIST_INFO_SIG", (&mENSComponent));
			if (e->type.ispctlInfo.info_id != ISP_WRITELIST_DONE) break;
			SCF_TRANSIT_TO(&CAM_SM::FlashNVM);
			return 0;
		}
		default: break;
	}
	return SCF_STATE_PTR(&CAM_SM::Booting);
}

SCF_STATE CAM_SM::FlashNVM(s_scf_event const *e) {
	switch (e->sig) {
		case SCF_STATE_ENTRY_SIG:
		{
			ENTRY;
			MSG0("FlashNVM-SCF_STATE_ENTRY_SIG\n");
			OstTraceFiltStatic0(TRACE_DEBUG, "FlashNVM-SCF_STATE_ENTRY_SIG", (&mENSComponent));
			pResourceSharerManager->mRSPing.Ping(0);
			return 0;
		}
		case Q_PONG_SIG:
		{
			MSG0("FlashNVM-Q_PONG_SIG\n");
			OstTraceFiltStatic0(TRACE_DEBUG, "FlashNVM-Q_PONG_SIG", (&mENSComponent));
			MSG1("FlashNVM: Sensor:%s\n", pSensor->GetCameraSlot()==ePrimaryCamera?"Primary":"Secondary");
			if (pSensor->GetCameraSlot()==ePrimaryCamera) {
				OstTraceFiltStatic0(TRACE_DEBUG, "FlashNVM: Sensor:Primary", (&mENSComponent));            
            }
			else{
				OstTraceFiltStatic0(TRACE_DEBUG, "FlashNVM: Sensor:Secondary", (&mENSComponent));
			}
            // Check whether the flash has NVM
			TFlashReturnCode eFlashRetCode = FLASH_RET_NONE;
			TFlashDetails    sFlashDetail;
            sFlashDetail.NVMSize = 0;
            TFlashMode       supportedFlashModes = FLASH_MODE_NONE;
            TCameraId        camId = (pSensor->GetCameraSlot() == ePrimaryCamera) ? EPrimary : ESecondary;

            if (pFlashDriver == NULL) {
                SCF_TRANSIT_TO(&CAM_SM::FlashTuning_LoadData);
                return 0;
            }
            pFlashDriver->GetSupportedFlashModes(supportedFlashModes, camId);
            if (supportedFlashModes == FLASH_MODE_NONE) { // If no flash mode is supported, no NVM is requested.
                SCF_TRANSIT_TO(&CAM_SM::FlashTuning_LoadData);
                return 0;
            }
            else {
                eFlashRetCode = pFlashDriver->GetFlashModeDetails(FLASH_MODE_VIDEO_LED, sFlashDetail, camId);
                if (eFlashRetCode == FLASH_RET_NONE) {
                    MSG2("FlashNVM: Sensor:%s, NVM Size:%u\n", pSensor->GetCameraSlot()==ePrimaryCamera?"Primary":"Secondary", sFlashDetail.NVMSize);
		    //OstTraceFiltStatic2(TRACE_DEBUG, "FlashNVM: Sensor:%s, NVM Size:%u", (&mENSComponent), pSensor->GetCameraSlot()==ePrimaryCamera?"Primary":"Secondary", sFlashDetail.NVMSize);
                    if( sFlashDetail.NVMSize != 0)
                    {   // Construct the flash NVM tuning object
                        CTuningBinData* pFlashNvmObj = NULL;
                        t_tuning_bin_data_error tbdErr = TUNING_BIN_OK;

                        pFlashNvmObj = pTuningDataBase->getObject(TUNING_OBJ_FLASH_NVM_DATA);
                        tbdErr = pFlashNvmObj->construct(sFlashDetail.NVMSize);
                        if( tbdErr != TUNING_BIN_OK)
                        {
                            DBGT_ERROR("FlashNVM: failed to construct TUNING_OBJ_FLASH_NVM_DATA tuning object err=%d (%s)\n",
                                tbdErr, CTuningBinData::errorCode2String(tbdErr));
			    OstTraceFiltStatic1(TRACE_ERROR, "FlashNVM: failed to construct TUNING_OBJ_FLASH_NVM_DATA tuning object err=%d", (&mENSComponent), tbdErr);
                            DBC_ASSERT(0);
                            return 0;
                        }
                        // Ask the Async flash to retrieve NVM data from flash IC
                        GetNVM_t NvmReq;
                        Camera* pCam = (Camera*)&mENSComponent.getProcessingComponent();
                        NvmReq.aFlashModes = FLASH_MODE_NONE;
                        NvmReq.BufferSize  = pFlashNvmObj->getSize();
                        NvmReq.NVMBuffer   = pFlashNvmObj->getAddr();
                        NvmReq.aCameraId   = (TCameraId)(pSensor->GetCameraSlot());
                        pCam->mAsyncflashRequest.getNVM(&NvmReq);
                    } else { // The flash has no NVM
                        SCF_TRANSIT_TO(&CAM_SM::FlashTuning_LoadData);
                        return 0;
                    }
                } else {
                    DBGT_ERROR("FlashNVM: GetFlashModeDetails() failed, err=%d\n", eFlashRetCode);
		    OstTraceFiltStatic1(TRACE_ERROR, "FlashNVM: GetFlashModeDetails() failed, err=%d", (&mENSComponent), eFlashRetCode);
                    DBC_ASSERT(0);
                    return 0;
                }
            }
      		return 0;
		}
		case ASYNC_FLASH_OPERATION_DONE_SIG:
		{
			MSG0("FlashNVM-ASYNC_FLASH_OPERATION_DONE_SIG\n");
			OstTraceFiltStatic0(TRACE_DEBUG, "FlashNVM-ASYNC_FLASH_OPERATION_DONE_SIG", (&mENSComponent));
			SCF_TRANSIT_TO(&CAM_SM::FlashTuning_LoadData);
			return 0;
		}
		case ASYNC_FLASH_OPERATION_FAILED_SIG:
		{
			DBGT_ERROR("FlashNVM-ASYNC_FLASH_OPERATION_FAILED_SIG\n");
			OstTraceFiltStatic0(TRACE_ERROR, "FlashNVM-ASYNC_FLASH_OPERATION_FAILED_SIG", (&mENSComponent));
			DBGT_ERROR("FlashNVM: Failed to retrieve the flash NVM\n");
			OstTraceFiltStatic0(TRACE_ERROR, "FlashNVM: Failed to retrieve the flash NVM", (&mENSComponent));
			DBC_ASSERT(0);
			return 0;
		}
		case SCF_STATE_EXIT_SIG:  EXIT; return 0;
		default: break;
	}
	return SCF_STATE_PTR(&CAM_SM::Booting);
}

SCF_STATE CAM_SM::FlashTuning_LoadData(s_scf_event const *e) {
	switch (e->sig) {
		case SCF_STATE_ENTRY_SIG:
		{
			ENTRY;
			MSG0("FlashTuning_LoadData-SCF_STATE_ENTRY_SIG\n");
			OstTraceFiltStatic0(TRACE_DEBUG, "FlashTuning_LoadData-SCF_STATE_ENTRY_SIG", (&mENSComponent));
			pResourceSharerManager->mRSPing.Ping(0);
			return 0;
		}
		case Q_PONG_SIG:
		{
			MSG0("FlashTuning_LoadData-Q_PONG_SIG\n");
			OstTraceFiltStatic0(TRACE_DEBUG, "FlashTuning_LoadData-Q_PONG_SIG", (&mENSComponent));
			MSG1("FlashTuning_LoadData: Sensor:%s\n", pSensor->GetCameraSlot()==ePrimaryCamera?"Primary":"Secondary");
			if (pSensor->GetCameraSlot()==ePrimaryCamera){
				OstTraceFiltStatic0(TRACE_DEBUG, "FlashNVM: Sensor:Primary", (&mENSComponent));            
			}
			else{
				OstTraceFiltStatic0(TRACE_DEBUG, "FlashNVM: Sensor:Secondary", (&mENSComponent));
			}
			MSG1("FlashTuning_LoadData: Tuning-Data-Available:%s\n", pTuningLoaderManager->hasFlashTuningData()?"no":"yes");
			if (pTuningLoaderManager->hasFlashTuningData()){
				OstTraceFiltStatic0(TRACE_DEBUG, "FlashTuning_LoadData: Tuning-Data-Available:no", (&mENSComponent));            
			}
			else{
				OstTraceFiltStatic0(TRACE_DEBUG, "FlashTuning_LoadData: Tuning-Data-Available:yes", (&mENSComponent));
			}
			if(pTuningLoaderManager->hasFlashTuningData())
			{
				// Camera has flash tuning data
				TFlashDetails iFlashDetails;
				TFlashReturnCode iFlashRet = FLASH_RET_NONE;

				if (NULL == pFlashDriver) {
					DBGT_ERROR("FlashTuning_LoadData: No valid flash driver.\n");
					OstTraceFiltStatic0(TRACE_ERROR, "FlashTuning_LoadData: No valid flash driver.", (&mENSComponent));
					DBC_ASSERT(0);
					return 0;
				}

                /* tell coverity that GetCameraSlot return value does not need to be checked */
                /* coverity[check_return : FALSE] */
				iFlashRet = pFlashDriver->GetFlashModeDetails(FLASH_MODE_VIDEO_LED, iFlashDetails, (TCameraId)(pSensor->GetCameraSlot()));
				if( iFlashRet != FLASH_RET_NONE) {
					MSG1("FlashTuning_LoadData: failed to get flash mode details, err=%d\n", iFlashRet);
					DBC_ASSERT(0);
					return 0;
				}

				t_flash_info flashInfo;
				flashInfo.driverType    = (t_uint8)iFlashDetails.FlashDriverType;
				flashInfo.driverVersion = (t_uint8)iFlashDetails.FlashDriverVersion;
				flashInfo.ledType       = (t_uint8)iFlashDetails.FlashLedType;
				flashInfo.reserved      = 0;

#if 1
				//FIXME remove this when flash driver IDs are fixed
				flashInfo.driverType    = 0x01;
				flashInfo.driverVersion = 0x06;
				flashInfo.ledType       = 0x0A;
				flashInfo.reserved      = 0x00;
#endif

				MSG0("FlashTuning_LoadData: loading flash tuning data\n");
				OstTraceFiltStatic0(TRACE_DEBUG, "FlashTuning_LoadData: loading flash tuning data", (&mENSComponent));
				TuningLoaderMgrError_t tlmErr = TUNING_LOADER_MGR_OK;
				tlmErr = pTuningLoaderManager->loadFlashTuningData(&flashInfo);
				if(tlmErr != TUNING_LOADER_MGR_OK) {
					DBGT_ERROR("FlashTuning_LoadData: flash tuning data loading failed err=%d (%s)\n",
						 tlmErr, CTuningLoaderManager::errorCode2String(tlmErr));
					OstTraceFiltStatic1(TRACE_ERROR, "FlashTuning_LoadData: flash tuning data loading failed err=%d", (&mENSComponent), tlmErr);
					DBC_ASSERT(0);
					return 0;
				}
			}
			else {
				// Secondary camera or flash does not work or no flash tuning data available
				// => skip flash tuning
				SCF_TRANSIT_TO(&CAM_SM::ImageQualityTuning_LoadData);
			}
			return 0;
		}
		case TUNING_LOADER_OPERATION_DONE_SIG:
		{
			MSG0("FlashTuning_LoadData-TUNING_LOADER_OPERATION_DONE_SIG\n");
			OstTraceFiltStatic0(TRACE_DEBUG, "FlashTuning_LoadData-TUNING_LOADER_OPERATION_DONE_SIG", (&mENSComponent));
			SCF_TRANSIT_TO(&CAM_SM::ImageQualityTuning_LoadData);
			return 0;
		}
		case TUNING_LOADER_OPERATION_FAILED_SIG:
		{
			DBGT_ERROR("FlashTuning_LoadData-TUNING_LOADER_OPERATION_FAILED_SIG\n");
			OstTraceFiltStatic0(TRACE_ERROR, "FlashTuning_LoadData-TUNING_LOADER_OPERATION_FAILED_SIG", (&mENSComponent));
			DBGT_ERROR("FlashTuning_LoadData: failed to load flash tuning data err=%ld (%s)\n",
				     e->type.tuningLoaderEvent.error, pTuningLoaderManager->loaderErrorCode2String(e->type.tuningLoaderEvent.error));
			OstTraceFiltStatic1(TRACE_ERROR, "FlashTuning_LoadData: failed to load flash tuning data err=%ld", (&mENSComponent), e->type.tuningLoaderEvent.error);
			DBC_ASSERT(0);
			return 0;
		}
		case SCF_STATE_EXIT_SIG:
		{
			EXIT;
			return 0;
		}
		default: break;
	}
	return SCF_STATE_PTR(&CAM_SM::Booting);
}

SCF_STATE CAM_SM::ImageQualityTuning_LoadData(s_scf_event const *e) {
	switch (e->sig) {
		case SCF_STATE_ENTRY_SIG:
		{
			MSG0("FirmwareTuning_LoadData-SCF_STATE_ENTRY_SIG\n");
			OstTraceFiltStatic0(TRACE_DEBUG, "FirmwareTuning_LoadData-SCF_STATE_ENTRY_SIG", (&mENSComponent));
			TuningLoaderMgrError_t tlmErr = TUNING_LOADER_MGR_OK;

			t_camera_info camInfo;
			// Initialisation to remove Coverity Warning			
			camInfo.manufId = 0;
			camInfo.modelId = 0;
			camInfo.revNum = 0;
			camInfo.Fuse_Id[0] = 0;
			camInfo.Fuse_Id[1] = 0;
			camInfo.Fuse_Id[2] = 0;
			camInfo.Fuse_Id4   = 0;

			camInfo.manufId = pSensor->GetSensorManufacturer();
			camInfo.modelId = pSensor->GetSensorModel();
			camInfo.revNum  = pSensor->GetSensorRevNumber();

			MSG0("ImageQualityTuning_LoadData: loading image quality tuning data\n");
			OstTraceFiltStatic0(TRACE_DEBUG, "ImageQualityTuning_LoadData: loading image quality tuning data", (&mENSComponent));
#ifdef CAMERA_ENABLE_OMX_3A_EXTENSION
			/* NB: As long as ENS does not allow to have a shutdown state machine,
			 * we will have to close() in Camera::stop(). */
                        /* tell coverity that GetCameraSlot return value does not need to be checked */
                        /* coverity[check_return : FALSE] */
                        p3AComponentManager->open(pSensor->GetCameraSlot(), pFlashDriver);
#else // CAMERA_ENABLE_OMX_3A_EXTENSION
			tlmErr = pTuningLoaderManager->loadImageQualityTuningData(&camInfo);
			if(tlmErr != TUNING_LOADER_MGR_OK) {
				DBGT_ERROR("ImageQualityTuning_LoadData: image quality tuning data loading failed err=%d (%s)\n",
					     tlmErr, CTuningLoaderManager::errorCode2String(tlmErr));
				OstTraceFiltStatic1(TRACE_ERROR, "ImageQualityTuning_LoadData: image quality tuning data loading failed err=%d", (&mENSComponent), tlmErr);
				DBC_ASSERT(0);
				return 0;
			}
#endif // CAMERA_ENABLE_OMX_3A_EXTENSION
			return 0;
		}
		case TUNING_LOADER_OPERATION_DONE_SIG:
		{
			MSG0("ImageQualityTuning_LoadData-TUNING_LOADER_OPERATION_DONE_SIG\n");
			OstTraceFiltStatic0(TRACE_DEBUG, "ImageQualityTuning_LoadData-TUNING_LOADER_OPERATION_DONE_SIG", (&mENSComponent));
			// The Tuning Data Base is now loaded
			pOpModeMgr->SetTuningDataBase(pTuningDataBase);
			/* NB: As long as ENS does not allow to have a shutdown state machine,
			 * we will have to close() in Camera::stop(). */
            /* tell coverity that GetCameraSlot return value does not need to be checked */
            /* coverity[check_return : FALSE] */
            p3AComponentManager->open(pSensor->GetCameraSlot(), pFlashDriver);

            }
		case SW3A_STARTED_SIG:
		{
            if(!chkSignal)
            {
			// SW3A successfully started
			SCF_TRANSIT_TO(&CAM_SM::Sensor_Patch);
			     chkSignal =  true;
			}else{
                 SM_PUSH_STATE_STATIC(&CAM_SM::Sensor_Patch);
                 chkSignal =  false;
            }
			return 0;
		}
		case TUNING_LOADER_OPERATION_FAILED_SIG:
		{
			DBGT_ERROR("ImageQualityTuning_LoadData-TUNING_LOADER_OPERATION_FAILED_SIG\n");
			OstTraceFiltStatic0(TRACE_ERROR, "ImageQualityTuning_LoadData-TUNING_LOADER_OPERATION_FAILED_SIG", (&mENSComponent));
			DBGT_ERROR("ImageQualityTuning_LoadData: failed to load image quality tuning data err=%ld (%s)\n",
				     e->type.tuningLoaderEvent.error, pTuningLoaderManager->loaderErrorCode2String(e->type.tuningLoaderEvent.error));
			OstTraceFiltStatic1(TRACE_ERROR, "ImageQualityTuning_LoadData: failed to load image quality tuning data err=%ld", (&mENSComponent), e->type.tuningLoaderEvent.error);
			DBC_ASSERT(0);
			return 0;
		}
		case SCF_STATE_EXIT_SIG:
		{
			EXIT;
			return 0;
		}
		default: break;
	}
	return SCF_STATE_PTR(&CAM_SM::Booting);
}

SCF_STATE CAM_SM::Sensor_Patch(s_scf_event const *e)
{
	switch (e->sig)
	{
		case SCF_STATE_ENTRY_SIG:
		{
			ENTRY;
			pResourceSharerManager->mRSPing.Ping(0);
			return 0;
		}
		case SCF_STATE_EXIT_SIG:
			EXIT;
			return 0;
		//case EVT_ISPCTL_INFO_SIG:
		case Q_PONG_SIG:
		{
			MSG0("Sensor_Patch-Q_PONG_SIG\n");
			OstTraceFiltStatic0(TRACE_DEBUG, "Sensor_Patch-Q_PONG_SIG", (&mENSComponent));
			t_uint32 NbOfElementPatch;
			oComI2C.I2CComStruct.pReg = pSensor->GetSensorPatchTable(&NbOfElementPatch);
			if (NbOfElementPatch!=0)
			{
				oComI2C.I2CComStruct.aNb_Element=NbOfElementPatch;
				oComI2C.I2CComStruct.controlType=I2C_Control_WriteByte;
                SM_PUSH_STATE_STATIC(&CAM_SM::Sensor_SpecificSettings);
				SCF_TRANSIT_TO(&CAM_SM::WriteI2C);
			}
			else
				SCF_TRANSIT_TO(&CAM_SM::Sensor_SpecificSettings);
			return 0;
		}
		default: break;
	}
	return SCF_STATE_PTR(&CAM_SM::Booting);
}


SCF_STATE CAM_SM::Sensor_SpecificSettings(s_scf_event const *e)
{
	switch (e->sig)
	{
		case SCF_STATE_ENTRY_SIG:
		{
			ENTRY;
			t_uint32 NbOfElementSpecific;
			ts_PageElement *aSpecific;
			aSpecific=pSensor->GetSensorSpecific(&NbOfElementSpecific);
			if (NbOfElementSpecific!=0)
			{
				for (unsigned int i=0; i < NbOfElementSpecific; i++)
				{
					pIspctlCom->queuePE(aSpecific[i].pe_addr, aSpecific[i].pe_data);
				}
				pIspctlCom->processQueue();
			}
			else
			{
				pResourceSharerManager->mRSPing.Ping(0);
			}
			return 0;
		}
		case SCF_STATE_EXIT_SIG:
			EXIT;
			return 0;
		case EVT_ISPCTL_LIST_INFO_SIG:
		case Q_PONG_SIG:
		{
			MSG0("Sensor_SpecificSettings-EVT_ISPCTL_LIST_INFO_SIG,EVT_ISPCTL_INFO_SIG-Q_PONG_SIG\n");
			OstTraceFiltStatic0(TRACE_DEBUG, "Sensor_SpecificSettings-EVT_ISPCTL_LIST_INFO_SIG,EVT_ISPCTL_INFO_SIG-Q_PONG_SIG", (&mENSComponent));
#ifdef CAMERA_ENABLE_OMX_3A_EXTENSION
			// Skip Tuning Data load.
			SCF_TRANSIT_TO(&CAM_SM::ApplyDefaultOMXConfig);
#else
			SCF_TRANSIT_TO(&CAM_SM::Linearization);
#endif
			return 0;
		}
		default: break;
	}
	return SCF_STATE_PTR(&CAM_SM::Booting);
}

SCF_STATE CAM_SM::Linearization(s_scf_event const *e)
{
	switch (e->sig)
	{
		case SCF_STATE_ENTRY_SIG:
		{
			ENTRY;
			/* Linearization LUTs can be written only with block disabled */
			pIspctlCom->writePE(SDL_Control_e_SDLMode_Control_Byte0, SDL_Disable);
			return 0;
		}
		case EVT_ISPCTL_INFO_SIG:
		{
			/* Write the Linearization LUTs to ISP memory */
			typedef struct
			{
				t_tuning_bin_object DbObj;
				unsigned long       IspLutAddr;
				t_uint16            LastEltPE;
			} t_LinearisationDesc;

			const t_LinearisationDesc iLinearizationDesc[4] =
			{
				{ TUNING_OBJ_LINEARIZATION_RR, LINEAR_MEM_LUT_RED_ELT_OFFSET,  SDL_ELT_u16_LastElementinLUT_RED_Byte0  },
				{ TUNING_OBJ_LINEARIZATION_GR, LINEAR_MEM_LUT_GINR_ELT_OFFSET, SDL_ELT_u16_LastElementinLUT_GIR_Byte0  },
				{ TUNING_OBJ_LINEARIZATION_GB, LINEAR_MEM_LUT_GINB_ELT_OFFSET, SDL_ELT_u16_LastElementinLUT_GIB_Byte0  },
				{ TUNING_OBJ_LINEARIZATION_BB, LINEAR_MEM_LUT_BLUE_ELT_OFFSET, SDL_ELT_u16_LastElementinLUT_BLUE_Byte0 }
			};

			for(int i=0; i<4; i++)
			{
				OMX_ERRORTYPE     ret = OMX_ErrorNone;
				CTuningBinData*   pLinearizationData = NULL;
				t_uint32          iLastEltValue = 0;

				// Write Linearization LUT to ISP (Last element not written)
				pLinearizationData = pTuningDataBase->getObject(iLinearizationDesc[i].DbObj);
				MSG1("CAM_SM::Linearization: writing %s LUT to ISP\n", pLinearizationData->getName());
				//OstTraceFiltStatic1(TRACE_DEBUG, "CAM_SM::Linearization: writing %s LUT to ISP\n", pLinearizationData->getName());
				ret = MMIO_Camera::ispVectorWrite( iLinearizationDesc[i].IspLutAddr,
				                                   (long int*)pLinearizationData->getAddr(),
				                                   (pLinearizationData->getSize()-sizeof(t_uint32))/sizeof(long int));
				if (ret != OMX_ErrorNone) {
					DBGT_ERROR("CAM_SM::Linearization: failed write LUT to ISP memory err=%d\n", ret);
					OstTraceFiltStatic1(TRACE_ERROR, "CAM_SM::Linearization: failed write LUT to ISP memory err=%d", (&mENSComponent), ret);
					DBC_ASSERT(0);
				}

				// Last element of LUT must be written to a Page Element
				iLastEltValue = *(t_uint32*)((t_uint32)pLinearizationData->getAddr() + (t_uint32)pLinearizationData->getSize() - sizeof(t_uint32));
				pIspctlCom->queuePE(iLinearizationDesc[i].LastEltPE, (t_uint32)iLastEltValue);
			}

			// Commit PEs
			pIspctlCom->processQueue();

			// Done
			return 0;
		}
		case SCF_STATE_EXIT_SIG:
			EXIT;
			return 0;

		case EVT_ISPCTL_LIST_INFO_SIG:
		{
			MSG0("Linearization-EVT_ISPCTL_LIST_INFO_SIG\n");
			OstTraceFiltStatic0(TRACE_DEBUG, "Linearization-EVT_ISPCTL_LIST_INFO_SIG", (&mENSComponent));
			SCF_TRANSIT_TO(&CAM_SM::Gridiron);
			return 0;
		}
		default: break;
	}
	return SCF_STATE_PTR(&CAM_SM::Booting);
}


SCF_STATE CAM_SM::Gridiron(s_scf_event const *e)
{
	switch (e->sig)
	{
		case SCF_STATE_ENTRY_SIG:
		{
			ENTRY;
			/* Gridiron memories and control PEs can be written only with block disabled */
			pIspctlCom->writePE(GridironControl_e_Flag_Enable_Byte0, Flag_e_FALSE);
			return 0;
		}
		case EVT_ISPCTL_INFO_SIG:
		{
			/* Write Gridiron grids to ISP memory */
			CTuningBinData* pGridironObj = pTuningDataBase->getObject(TUNING_OBJ_GRIDIRON);
			long int* pGridironData      = (long int*)pGridironObj->getAddr();
			int iGridironCastSize        = (pGridironObj->getStride())/sizeof(long int);

			typedef struct
			{
				unsigned long  iIspAddr;
				long int*      pHostData;
			} t_GridironCast;

			const t_GridironCast iGridironCast[4] =
			{
				{ GRIDIRON_MEM_ELT_CAST0_OFFSET, &pGridironData[0*iGridironCastSize] },
				{ GRIDIRON_MEM_ELT_CAST1_OFFSET, &pGridironData[1*iGridironCastSize] },
				{ GRIDIRON_MEM_ELT_CAST2_OFFSET, &pGridironData[2*iGridironCastSize] },
				{ GRIDIRON_MEM_ELT_CAST3_OFFSET, &pGridironData[3*iGridironCastSize] }
			};

			for(int i=0; i<4; i++)
			{
				// Write Gridion grids to ISP
				OMX_ERRORTYPE ret = OMX_ErrorNone;
				MSG2("CAM_SM::Gridiron: writing %s Cast %d to ISP\n", pGridironObj->getName(), i);
				//OstTraceFiltStatic2(TRACE_DEBUG, "CAM_SM::Gridiron: writing %s Cast %d to ISP\n", pGridironObj->getName(), i);
				ret = MMIO_Camera::ispVectorWrite( iGridironCast[i].iIspAddr,
				                                   iGridironCast[i].pHostData,
				                                   iGridironCastSize);
				if (ret != OMX_ErrorNone) {
					DBGT_ERROR("CAM_SM::Gridiron: failed to write grid to ISP memory err=%d\n", ret);
					OstTraceFiltStatic1(TRACE_ERROR, "CAM_SM::Gridiron: failed to write grid to ISP memory err=%d", (&mENSComponent), ret);
					DBC_ASSERT(0);
				}
			}

			SCF_TRANSIT_TO(&CAM_SM::ApplyDefaultOMXConfig);
			return 0;
		}
		case SCF_STATE_EXIT_SIG:
			EXIT;
			return 0;
		default: break;
	}
	return SCF_STATE_PTR(&CAM_SM::Booting);
}

/* @brief applies in camera all the OMX configs stored in the omxcamera.
 * */
SCF_STATE CAM_SM::ApplyDefaultOMXConfig(s_scf_event const *e) {
    switch (e->sig) {
        case SCF_STATE_ENTRY_SIG:
        {
            MSG0("ApplyDefaultOMXConfig-SCF_STATE_ENTRY_SIG\n");
            OstTraceFiltStatic0(TRACE_DEBUG, "ApplyDefaultOMXConfig-SCF_STATE_ENTRY_SIG", (&mENSComponent));
            ((COmxCamera*)&mENSComponent)->applyOMXConfig();
            pResourceSharerManager->mRSRelease.Release(CAMERA_VF);
            return 0;
        }
        case Q_RELEASED_SIG:
        {
            pOmxStateMgr->changeState(OMX_StateIdle);
            SCF_TRANSIT_TO(&CAM_SM::ProcessPendingEvents);
            return 0;
        }
        case SCF_STATE_EXIT_SIG:
        {
            MSG0("ApplyDefaultOMXConfig-SCF_STATE_EXIT_SIG\n");
            OstTraceFiltStatic0(TRACE_DEBUG, "ApplyDefaultOMXConfig-SCF_STATE_EXIT_SIG", (&mENSComponent));
            return 0;
        }
        default: break;
    }
    return SCF_STATE_PTR(&CAM_SM::Booting);
}
