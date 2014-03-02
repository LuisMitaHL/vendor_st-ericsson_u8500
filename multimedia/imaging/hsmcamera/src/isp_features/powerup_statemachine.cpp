/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
#include "hsmcam.h" // rename_me sc_cam.h
#include "MMIO_Camera.h" //MMIO_Camera


// OST framework
#include "OstTraceDefinitions_ste.h"
#include "OpenSystemTrace_ste.h"

#ifdef OST_TRACE_COMPILER_IN_USE
#include "hsmcamera_src_isp_features_powerup_statemachineTraces.h"
#endif


SCF_STATE CAM_SM::PowerUp_STC(s_scf_event const *e)
{
	switch (e->sig) {
		case SCF_STATE_ENTRY_SIG:
			MSG0("PowerUp_STC-SCF_STATE_ENTRY_SIG\n");
			OstTraceFiltStatic0(TRACE_DEBUG, "PowerUp_STC-SCF_STATE_ENTRY_SIG", (&mENSComponent));
			pIspctlCom->requestPE(Sensor_Tuning_Control_e_Flag_ReadConfigBeforeBoot_Byte0);
			return 0;
		
		case EVT_ISPCTL_INFO_SIG :
			MSG0("Powerup_STC-EVT_ISPCTL_INFO_SIG;\n");
			OstTraceFiltStatic0(TRACE_DEBUG, "Powerup_STC-EVT_ISPCTL_INFO_SIG;", (&mENSComponent));
			if (ISP_READ_DONE == e->type.ispctlInfo.info_id) /* Acknowledge to read */
			{
				Flag_te ReadCdccConfigBeforeBoot = (Flag_te)e->type.ispctlInfo.value;
				if (ReadCdccConfigBeforeBoot == Flag_e_TRUE) SCF_TRANSIT_TO(&CAM_SM::PowerUp_STC_SensorInfo);
				else SCF_TRANSIT_TO(&COM_SM::SHARED_ControlingISPState);
				return 0;
			}
			break;
		case SCF_STATE_EXIT_SIG:
			MSG0("PowerUp_STC-SCF_STATE_EXIT_SIG\n");
			OstTraceFiltStatic0(TRACE_DEBUG, "PowerUp_STC-SCF_STATE_EXIT_SIG", (&mENSComponent));
			return 0;
		default:
			break;
	}
	return SCF_STATE_PTR(&COM_SM::SHARED_ControlingISPState_Boot);
}
SCF_STATE CAM_SM::PowerUp_STC_SensorInfo(s_scf_event const *e)
{
	switch (e->sig) {
		case SCF_STATE_ENTRY_SIG:
		MSG0("PowerUp_STC_SensorInfo-SCF_STATE_ENTRY_SIG\n");
		OstTraceFiltStatic0(TRACE_DEBUG, "PowerUp_STC_SensorInfo-SCF_STATE_ENTRY_SIG", (&mENSComponent));
		pIspctlCom->queuePE(SensorInformation_e_Flag_Available_Byte0,0);
		pIspctlCom->queuePE(SensorInformation_u8_manufacturer_id_Byte0,0);
		pIspctlCom->queuePE(SensorInformation_u16_model_id_Byte0,0);
		pIspctlCom->queuePE(SensorInformation_u8_revision_number_Byte0,0);
		pIspctlCom->queuePE(SensorFrameConstraints_u16_MaxOPXOutputSize_Byte0,0);
		pIspctlCom->queuePE(SensorFrameConstraints_u16_MaxOPYOutputSize_Byte0,0);
		pIspctlCom->queuePE(CurrentFrameDimension_u16_NumberofNonActiveLinesAtTopEdge_Byte0,0);
		pIspctlCom->readQueue();
		return 0;

		case EVT_ISPCTL_LIST_INFO_SIG:
		{
			MSG0("PowerUp_STC_SensorInfo-EVT_ISPCTL_LIST_INFO_SIG\n");
			OstTraceFiltStatic0(TRACE_DEBUG, "PowerUp_STC_SensorInfo-EVT_ISPCTL_LIST_INFO_SIG", (&mENSComponent));
			if (e->type.ispctlInfo.info_id != ISP_READLIST_DONE) break;
//#if defined (OSI_DEBUG)
			for(t_uint16 i=0; i < e->type.ispctlInfo.number_of_pe; i++) {
				MSG3("   PE[%d] %s = 0x%lX\n", i, pSensor->GetPeName(e->type.ispctlInfo.Listvalue[i].pe_addr), e->type.ispctlInfo.Listvalue[i].pe_data );
				//OstTraceFiltStatic3(TRACE_DEBUG, "   PE[%d] %s = 0x%lX", (&mENSComponent), i, CXp70::GetPeName(e->type.ispctlInfo.Listvalue[i].pe_addr), e->type.ispctlInfo.Listvalue[i].pe_data );
			}
//#endif
			// Store sensor related data
			t_sensorDesciptor pDesc;

			// Initialisation to remove Coverity Warning
			pDesc.manufacturer = 0;
			pDesc.model        = 0;
			pDesc.revisionNumber = 0;
			pDesc.Fuse_Id[0] = 0;
			pDesc.Fuse_Id[1] = 0;
			pDesc.Fuse_Id[2] = 0;
			pDesc.Fuse_Id4   = 0;

			pDesc.model = (t_uint16)e->type.ispctlInfo.Listvalue[2].pe_data;
			pDesc.manufacturer = (t_uint8) e->type.ispctlInfo.Listvalue[1].pe_data;
			pDesc.revisionNumber = (t_uint8) e->type.ispctlInfo.Listvalue[3].pe_data;
			pSensor->SetSensorDescriptor(&pDesc);

			pSensor->setBayerWidth((t_uint32)e->type.ispctlInfo.Listvalue[4].pe_data);
			pSensor->setBayerHeight((t_uint32)(e->type.ispctlInfo.Listvalue[5].pe_data + e->type.ispctlInfo.Listvalue[6].pe_data));
			pSensor->setNberOfNonActiveLines((t_uint8)e->type.ispctlInfo.Listvalue[6].pe_data);

			// update VPB1 port settings according to sensor parameters
			OMX_PARAM_PORTDEFINITIONTYPE mParamPortDefinition;
			mParamPortDefinition.nSize = sizeof(OMX_PARAM_PORTDEFINITIONTYPE);
			getOmxIlSpecVersion(&mParamPortDefinition.nVersion);
			mENSComponent.getPort(CAMERA_PORT_OUT1)->getParameter(OMX_IndexParamPortDefinition,&mParamPortDefinition);

			if ((mParamPortDefinition.format.video.nFrameWidth != pSensor->getBayerWidth())
				|| (mParamPortDefinition.format.video.nFrameHeight != pSensor->getBayerHeight())) {
				mParamPortDefinition.format.video.nFrameWidth = pSensor->getBayerWidth();
				mParamPortDefinition.format.video.nFrameHeight = pSensor->getBayerHeight();
                mParamPortDefinition.format.video.nSliceHeight = pSensor->getBayerHeight();
				mENSComponent.getPort(CAMERA_PORT_OUT1)->setParameter(OMX_IndexParamPortDefinition,&mParamPortDefinition);                
                
				// notify IL client of update
				mENSComponent.eventHandler(OMX_EventPortSettingsChanged, mENSComponent.getPort(CAMERA_PORT_OUT1)->getPortIndex(), 0);            
			}
			SCF_TRANSIT_TO(&CAM_SM::PowerUp_STC_GetNbBlockIds);
			return 0;
		}
	
		case SCF_STATE_EXIT_SIG:
			MSG0("PowerUp_STC_SensorInfo-SCF_STATE_EXIT_SIG\n");
			OstTraceFiltStatic0(TRACE_DEBUG, "PowerUp_STC_SensorInfo-SCF_STATE_EXIT_SIG", (&mENSComponent));
			return 0;
		default:
			break;
	}
	return SCF_STATE_PTR(&COM_SM::SHARED_ControlingISPState_Boot);
}

SCF_STATE CAM_SM::PowerUp_STC_GetNbBlockIds(s_scf_event const *e) {
	switch (e->sig) {
		case SCF_STATE_ENTRY_SIG:
		{
			ENTRY;
			MSG0("PowerUp_STC_GetNbBlockIds-SCF_STATE_ENTRY_SIG\n");
			OstTraceFiltStatic0(TRACE_DEBUG, "PowerUp_STC_GetNbBlockIds-SCF_STATE_ENTRY_SIG", (&mENSComponent));
			// Ask to firmware how many tuning blocks are supported and get current coin value
			pIspctlCom->queuePE(Sensor_Tuning_Status_u16_TotalSubBlockIdsCount_Byte0,0);
			pIspctlCom->queuePE(Sensor_Tuning_Status_e_Coin_SubBlock_Status_Byte0,0);
			pIspctlCom->readQueue();
			return 0;
		}
		case EVT_ISPCTL_LIST_INFO_SIG:
		{
			MSG0("PowerUp_STC_GetNbBlockIds-EVT_ISPCTL_LIST_INFO_SIG\n");
			OstTraceFiltStatic0(TRACE_DEBUG, "PowerUp_STC_GetNbBlockIds-EVT_ISPCTL_LIST_INFO_SIG", (&mENSComponent));
			if(e->type.ispctlInfo.info_id != ISP_READLIST_DONE) break;

			// Store the Firmware tuning info related data
			iFwSubBlockIdsCount = (t_uint16)e->type.ispctlInfo.Listvalue[0].pe_data;
			iStatusCoin         = (Coin_te) e->type.ispctlInfo.Listvalue[1].pe_data;

			if(iFwSubBlockIdsCount==0)
			{
				// Firmware does not support tuning
				MSG0("   Tuning not supported by the firmware\n");
				OstTraceFiltStatic0(TRACE_DEBUG, "Tuning not supported by the firmware", (&mENSComponent));
				pFwSubBlockId = NULL;
				SCF_TRANSIT_TO(&COM_SM::SHARED_ControlingISPState);
				return 0;
			}
			else
			{
				// Firmware supports tuning: retrieve the supported IDs
				MSG1("   Tuning supported by the firmware: %d FwSubBlocks supported\n", iFwSubBlockIdsCount);
				OstTraceFiltStatic1(TRACE_DEBUG, "Tuning supported by the firmware: %d FwSubBlocks supported", (&mENSComponent), iFwSubBlockIdsCount);
				iStatusCoinRetriesCount = 0;
				iCurrentFwSubBlockIndex = 0;
				pFwSubBlockId = new t_uint16[iFwSubBlockIdsCount];
				if(pFwSubBlockId==NULL) {
					DBGT_ERROR("   Failed to allocate %d entries for FwSubBlock IDs\n", iFwSubBlockIdsCount);
					OstTraceFiltStatic1(TRACE_ERROR, "Failed to allocate %d entries for FwSubBlock IDs", (&mENSComponent), iFwSubBlockIdsCount);
					DBC_ASSERT(0);
					return 0;
				}
				MSG2("   Allocated pFwSubBlockId=%p (%d entries)\n", pFwSubBlockId, iFwSubBlockIdsCount);
				OstTraceFiltStatic2(TRACE_DEBUG, "Allocated pFwSubBlockId=0x%x (%d entries)", (&mENSComponent), (t_uint32)pFwSubBlockId, iFwSubBlockIdsCount);
				SCF_TRANSIT_TO(&CAM_SM::PowerUp_STC_QueryId);
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
	return SCF_STATE_PTR(&COM_SM::SHARED_ControlingISPState_Boot);
}

SCF_STATE CAM_SM::PowerUp_STC_QueryId(s_scf_event const *e) {
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
			MSG0("PowerUp_STC_QueryId-EVT_ISPCTL_LIST_INFO_SIG\n");
			OstTraceFiltStatic0(TRACE_DEBUG, "PowerUp_STC_QueryId-EVT_ISPCTL_LIST_INFO_SIG", (&mENSComponent));
			if (e->type.ispctlInfo.info_id != ISP_WRITELIST_DONE) break;
			SCF_TRANSIT_TO(&CAM_SM::PowerUp_STC_CheckCoin);
			return 0;
		}
		case SCF_STATE_EXIT_SIG:
		{
			EXIT;
			return 0;
		}
		default: break;
	}
	return SCF_STATE_PTR(&COM_SM::SHARED_ControlingISPState_Boot);
}

SCF_STATE CAM_SM::PowerUp_STC_CheckCoin(s_scf_event const *e) {
	switch (e->sig) {
		case SCF_STATE_ENTRY_SIG:
		{
			ENTRY;
			MSG0("PowerUp_STC_CheckCoin-SCF_STATE_ENTRY_SIG\n");
			OstTraceFiltStatic0(TRACE_DEBUG, "PowerUp_STC_CheckCoin-SCF_STATE_ENTRY_SIG", (&mENSComponent));
			// Get coin value
			pIspctlCom->queuePE(Sensor_Tuning_Status_e_Coin_SubBlock_Status_Byte0,0);
			pIspctlCom->readQueue();
			return 0;
		}
		case EVT_ISPCTL_LIST_INFO_SIG:
		{
			MSG0("PowerUp_STC_CheckCoin-EVT_ISPCTL_LIST_INFO_SIG\n");
			OstTraceFiltStatic0(TRACE_DEBUG, "PowerUp_STC_CheckCoin-EVT_ISPCTL_LIST_INFO_SIG", (&mENSComponent));
			if (e->type.ispctlInfo.info_id != ISP_READLIST_DONE) break;
//#if defined (OSI_DEBUG)
			for(t_uint16 i=0; i < e->type.ispctlInfo.number_of_pe; i++) {
				MSG4("   PE[%d] 0x%X (%s) = 0x%lX\n", i, e->type.ispctlInfo.Listvalue[i].pe_addr, pSensor->GetPeName(e->type.ispctlInfo.Listvalue[i].pe_addr), e->type.ispctlInfo.Listvalue[i].pe_data );
				//OstTraceFiltStatic4(TRACE_DEBUG, "   PE[%d] 0x%X (%s) = 0x%lX", (&mENSComponent), i, e->type.ispctlInfo.Listvalue[i].pe_addr, CXp70::GetPeName(e->type.ispctlInfo.Listvalue[i].pe_addr), e->type.ispctlInfo.Listvalue[i].pe_data );
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
				OstTraceFiltStatic1(TRACE_DEBUG, "Coin state unchanged (state=%d) => try again", (&mENSComponent), (int)iStatusCoin);
				iStatusCoinRetriesCount++;
				pIspctlCom->queuePE(Sensor_Tuning_Status_e_Coin_SubBlock_Status_Byte0,0);
				pIspctlCom->readQueue();
			}
			else {
				// Coin state changed => we can read the block ID value
				iStatusCoin = (Coin_te) e->type.ispctlInfo.Listvalue[0].pe_data;
				MSG1("   Coin state unchanged (state=%d) => read Block ID value\n", (int)iStatusCoin);
				OstTraceFiltStatic1(TRACE_DEBUG, "Coin state unchanged (state=%d) => read Block ID value", (&mENSComponent), (int)iStatusCoin);
				iStatusCoinRetriesCount = 0;
				SCF_TRANSIT_TO(&CAM_SM::PowerUp_STC_ReadId);
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
	return SCF_STATE_PTR(&COM_SM::SHARED_ControlingISPState_Boot);
}

SCF_STATE CAM_SM::PowerUp_STC_ReadId(s_scf_event const *e) {
	switch (e->sig) {
		case SCF_STATE_ENTRY_SIG:
		{
			ENTRY;
			MSG0("PowerUp_STC_ReadId-SCF_STATE_ENTRY_SIG\n");
			OstTraceFiltStatic0(TRACE_DEBUG, "PowerUp_STC_ReadId-SCF_STATE_ENTRY_SIG", (&mENSComponent));
			pIspctlCom->queuePE(Sensor_Tuning_Status_u16_CurrentSubBlockIdValue_Byte0,0);
			pIspctlCom->readQueue();
			return 0;
		}
		case EVT_ISPCTL_LIST_INFO_SIG:
		{
			MSG0("PowerUp_STC_ReadId-EVT_ISPCTL_LIST_INFO_SIG\n");
			OstTraceFiltStatic0(TRACE_DEBUG, "PowerUp_STC_ReadId-EVT_ISPCTL_LIST_INFO_SIG", (&mENSComponent));
			if (e->type.ispctlInfo.info_id != ISP_READLIST_DONE) break;
//#if defined (OSI_DEBUG)
			for(t_uint16 i=0; i < e->type.ispctlInfo.number_of_pe; i++) {
				MSG3("   PE[%d] %s = 0x%lX\n", i, pSensor->GetPeName(e->type.ispctlInfo.Listvalue[i].pe_addr), e->type.ispctlInfo.Listvalue[i].pe_data );
				//OstTraceFiltStatic3(TRACE_DEBUG, "PE[%d] %s = 0x%lX", (&mENSComponent), i, CXp70::GetPeName(e->type.ispctlInfo.Listvalue[i].pe_addr), e->type.ispctlInfo.Listvalue[i].pe_data );
			}
//#endif
			// Store tuning BlockId value
			pFwSubBlockId[iCurrentFwSubBlockIndex] = (t_uint16)e->type.ispctlInfo.Listvalue[0].pe_data;
			MSG2("   FwSubBlockId[%d] = ID%d\n", iCurrentFwSubBlockIndex, pFwSubBlockId[iCurrentFwSubBlockIndex]);
			OstTraceFiltStatic2(TRACE_DEBUG, "FwSubBlockId[%d] = ID%d", (&mENSComponent), iCurrentFwSubBlockIndex, pFwSubBlockId[iCurrentFwSubBlockIndex]);
			// Another block to be fetched ?
			iCurrentFwSubBlockIndex++;
			if(iCurrentFwSubBlockIndex<iFwSubBlockIdsCount) {
				// Still some tuning blocks IDs are to be fetched
				MSG0("   Some blocks ID are to be fetched\n");
				OstTraceFiltStatic0(TRACE_DEBUG, "Some blocks ID are to be fetched", (&mENSComponent));
				SCF_TRANSIT_TO(&CAM_SM::PowerUp_STC_QueryId);
			}
			else {
				// All tuning blocks IDs were fetched
				MSG1("   All block IDs were fetched: total %d\n", iFwSubBlockIdsCount);
				OstTraceFiltStatic1(TRACE_DEBUG, "All block IDs were fetched: total %d", (&mENSComponent), iFwSubBlockIdsCount);
//#if defined (OSI_DEBUG)
				for(t_uint16 i=0; i<iFwSubBlockIdsCount; i++) {
					MSG2("   FwSubBlockId[%d] = ID%d\n", i, pFwSubBlockId[i]);
					OstTraceFiltStatic2(TRACE_DEBUG, "FwSubBlockId[%d] = ID%d", (&mENSComponent), i, pFwSubBlockId[i]);
				}
//#endif
				SCF_TRANSIT_TO(&CAM_SM::PowerUp_STC_LoadData);
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
	return SCF_STATE_PTR(&COM_SM::SHARED_ControlingISPState_Boot);
}

SCF_STATE CAM_SM::PowerUp_STC_LoadData(s_scf_event const *e) {
	switch (e->sig) {
		case SCF_STATE_ENTRY_SIG:
		{
			MSG0("PowerUp_STC_LoadData-SCF_STATE_ENTRY_SIG\n");
			OstTraceFiltStatic0(TRACE_DEBUG, "PowerUp_STC_LoadData-SCF_STATE_ENTRY_SIG", (&mENSComponent));
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

			MSG0("PowerUp_STC_LoadData: loading firmware tuning data\n");
			OstTraceFiltStatic0(TRACE_DEBUG, "PowerUp_STC_LoadData: loading firmware tuning data", (&mENSComponent));
			tlmErr = pTuningLoaderManager->loadFirmwareTuningData(&camInfo, &fwBlocks);
			if(tlmErr != TUNING_LOADER_MGR_OK) {
				DBGT_ERROR("PowerUp_STC_LoadData: firmware tuning data loading failed err=%d (%s)\n",
					     tlmErr, CTuningLoaderManager::errorCode2String(tlmErr));
				OstTraceFiltStatic1(TRACE_ERROR, "PowerUp_STC_LoadData: firmware tuning data loading failed err=%d", (&mENSComponent), tlmErr);
				DBC_ASSERT(0);
				return 0;
			}
			return 0;
		}
		case TUNING_LOADER_OPERATION_DONE_SIG:
		{
			MSG0("PowerUp_STC_LoadData-TUNING_LOADER_OPERATION_DONE_SIG\n");
			OstTraceFiltStatic0(TRACE_DEBUG, "PowerUp_STC_LoadData-TUNING_LOADER_OPERATION_DONE_SIG", (&mENSComponent));
			MSG2("Deleting pFwSubBlockId=%p (%d entries)\n", pFwSubBlockId, iFwSubBlockIdsCount);
			OstTraceFiltStatic2(TRACE_DEBUG, "Deleting pFwSubBlockId=%0x%x (%d entries)", (&mENSComponent), (t_uint32)pFwSubBlockId, iFwSubBlockIdsCount);
			delete [] pFwSubBlockId;
			pFwSubBlockId = NULL;
			SCF_TRANSIT_TO(&CAM_SM::PowerUp_STC_SendDataToFirmware);
			return 0;
		}
		case TUNING_LOADER_OPERATION_FAILED_SIG:
		{
			MSG0("PowerUp_STC_LoadData-TUNING_LOADER_OPERATION_FAILED_SIG\n");
			OstTraceFiltStatic0(TRACE_DEBUG, "PowerUp_STC_LoadData-TUNING_LOADER_OPERATION_FAILED_SIG", (&mENSComponent));
			DBGT_ERROR("PowerUp_STC_LoadData: failed to load firmware tuning data err=%ld (%s)\n",
				     e->type.tuningLoaderEvent.error, pTuningLoaderManager->loaderErrorCode2String(e->type.tuningLoaderEvent.error));
			OstTraceFiltStatic1(TRACE_ERROR, "PowerUp_STC_LoadData: failed to load firmware tuning data err=%ld", (&mENSComponent), e->type.tuningLoaderEvent.error);
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
	return SCF_STATE_PTR(&COM_SM::SHARED_ControlingISPState_Boot);
}

SCF_STATE CAM_SM::PowerUp_STC_SendDataToFirmware(s_scf_event const *e) {
	switch (e->sig) {
		case SCF_STATE_ENTRY_SIG:
		{
			ENTRY;
			MSG0("PowerUp_STC_SendDataToFirmware-SCF_STATE_ENTRY_SIG\n");
			OstTraceFiltStatic0(TRACE_DEBUG, "PowerUp_STC_SendDataToFirmware-SCF_STATE_ENTRY_SIG", (&mENSComponent));
			CTuningBinData* pFwTuningObj = NULL;

			// Get the firmware tuning object
			pFwTuningObj = pTuningDataBase->getObject(TUNING_OBJ_FIRMWARE_DATA);

			// Check that the firmware tuning data fit into the shared memory chunk
			if( (csm_uint32_t)pFwTuningObj->getSize() > pFwShmChunk->size) {
				MSG0("PowerUp_STC_SendDataToFirmware: Tuning data shared chunk too small:\n");
				OstTraceFiltStatic0(TRACE_DEBUG, "PowerUp_STC_SendDataToFirmware: Tuning data shared chunk too small:", (&mENSComponent));
				MSG1("PowerUp_STC_SendDataToFirmware: Shared mem size  = %ld\n", pFwShmChunk->size);
				OstTraceFiltStatic1(TRACE_DEBUG, "PowerUp_STC_SendDataToFirmware: Shared mem size  = %ld", (&mENSComponent), pFwShmChunk->size);
				MSG1("PowerUp_STC_SendDataToFirmware: Actual data size = %d\n", pFwTuningObj->getSize());
				OstTraceFiltStatic1(TRACE_DEBUG, "PowerUp_STC_SendDataToFirmware: Actual data size = %d", (&mENSComponent), pFwTuningObj->getSize());
				DBC_ASSERT(0);
				return 0;
			}

// Copy the tuning data into the firmware address space
			MSG3("PowerUp_STC_SendDataToFirmware: Copying data %p, size %d, into firwmare address space %p\n", pFwTuningObj->getAddr(), pFwTuningObj->getSize(), pFwShmChunk->ispLogicalAddress);
			OstTraceFiltStatic3(TRACE_DEBUG, "PowerUp_STC_SendDataToFirmware: Copying data 0x%x, size %d, into firwmare address space 0x%x", (&mENSComponent), (t_uint32)pFwTuningObj->getAddr(), pFwTuningObj->getSize(), (t_uint32)pFwShmChunk->ispLogicalAddress);
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
				DBGT_ERROR("PowerUp_STC_SendDataToFirmware: Failed to flush shared memory chunk, err=%d\n", shmErr);
				OstTraceFiltStatic1(TRACE_ERROR, "PowerUp_STC_SendDataToFirmware: Failed to flush shared memory chunk, err=%d", (&mENSComponent), shmErr);
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
			MSG0("PowerUp_STC_SendDataToFirmware-EVT_ISPCTL_LIST_INFO_SIG\n");
			OstTraceFiltStatic0(TRACE_DEBUG, "PowerUp_STC_SendDataToFirmware-EVT_ISPCTL_LIST_INFO_SIG", (&mENSComponent));
			if (e->type.ispctlInfo.info_id != ISP_WRITELIST_DONE) break;
			SCF_TRANSIT_TO(&CAM_SM::PowerUp_STC_Done);
			return 0;
		}
		default: break;
	}
	return SCF_STATE_PTR(&COM_SM::SHARED_ControlingISPState_Boot);
}

SCF_STATE CAM_SM::PowerUp_STC_Done(s_scf_event const *e)
{
	switch (e->sig) {
		case SCF_STATE_ENTRY_SIG:
			MSG0("PowerUp_STC_Done-SCF_STATE_ENTRY_SIG\n");
			OstTraceFiltStatic0(TRACE_DEBUG, "PowerUp_STC_Done-SCF_STATE_ENTRY_SIG", (&mENSComponent));
			pIspctlCom->writePE(Sensor_Tuning_Control_e_Flag_ConfigurationDone_Byte0,Flag_e_TRUE);
			return 0;
		
		case EVT_ISPCTL_INFO_SIG :
			MSG0("Powerup_STC_Done-EVT_ISPCTL_INFO_SIG;\n");
			OstTraceFiltStatic0(TRACE_DEBUG, "Powerup_STC_Done-EVT_ISPCTL_INFO_SIG;", (&mENSComponent));
			if (ISP_WRITE_DONE == e->type.ispctlInfo.info_id){} /*Here waiting BOOT_COMPLETE  */
			else break;
			return 0;
		case SCF_STATE_EXIT_SIG:
			MSG0("PowerUp_STC_Done-SCF_STATE_EXIT_SIG\n");
			OstTraceFiltStatic0(TRACE_DEBUG, "PowerUp_STC_Done-SCF_STATE_EXIT_SIG", (&mENSComponent));
			return 0;
		default:
			break;
	}
	return SCF_STATE_PTR(&COM_SM::SHARED_ControlingISPState_Boot);
}

