/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
#define OMXCOMPONENT "I2C_Patch"
#include "osi_trace.h"
#include "sensor.h"
#include "hsm.h"
#include "VhcElementDefs.h"
/* for traces */
#undef OMX_TRACE_UID
#define OMX_TRACE_UID 0x8


//#define TRACING_I2C

#ifdef TRACING_I2C
	#include <los/api/los_api.h>
	#define TRACE_I2C(a,b,c,d,e,f,g) LOS_Log(a,b,c,d,e,f,g)
#else
	#define TRACE_I2C(a,b,c,d,e,f,g)
#endif



#include "protothread.h"

#ifdef __SYMBIAN32__
NONSHARABLE_CLASS(ProtoThread_I2C);
#endif
class ProtoThread_I2C: public HSMProtoThread
//***************************************************************************************
{
public:
	ProtoThread_I2C(COM_SM &hsm);
	int InitWrites(int nbr, ts_I2CReg *ptr);
	virtual int Reset(bool force=false);

public : // User data
	//For I2C writes
	int coin;
	int Nb_Element;
	ts_I2CReg *pReg;
	int mIndex;
} ;


ProtoThread_I2C::ProtoThread_I2C(COM_SM &hsm)
: HSMProtoThread(hsm)
//***************************************************************************************
{
	ProtoThread_I2C::Reset(true);
}

int ProtoThread_I2C::InitWrites(int nbr, ts_I2CReg *ptr)
//***************************************************************************************
{
	Nb_Element=nbr;
	pReg=ptr;
	mIndex=0;
	return(0);
}

int ProtoThread_I2C::Reset(bool force)
//***************************************************************************************
{
	coin=0;
	Nb_Element=0;
	pReg=NULL;
	mIndex=0;
	return(ProtoThread::Reset(force));
}

SCF_STATE COM_SM::WriteI2C(s_scf_event const *e) {
    switch (e->sig) {
        case SCF_STATE_ENTRY_SIG: ENTRY; return 0;
        case SCF_STATE_EXIT_SIG:  EXIT; return 0;
        case SCF_STATE_INIT_SIG: INIT; SCF_INIT(&COM_SM::WriteI2C_Do); return 0;
        case SCF_PARENT_SIG:break;
        case EVT_ISPCTL_ERROR_SIG:
        {
            MSG0("WriteI2C-EVT_ISPCTL_ERROR_SIG\n");
            if (e->type.ispctlError.error_id == ISP_POLLING_TIMEOUT_ERROR)  {
                MSG0("ControlingISP-ISP_POLLING_TIMEOUT_ERROR;\n");
                DBC_ASSERT(0);
            }
            if (e->type.ispctlError.error_id == ISP_DOES_NOT_RESPOND_ERROR)  {
                MSG0("ControlingISP-ISP_DOES_NOT_RESPOND_ERROR;\n");
                DBC_ASSERT(0);
            }
            return 0;
        }
        case Q_DEFERRED_EVENT_SIG:  MSG0("WriteI2C-DEFERRED_EVENT;\n");  return 0;
        default: break;
    }
    return SCF_STATE_PTR(&COM_SM::OMX_Executing);
}


SCF_STATE COM_SM::WriteI2C_Do(s_scf_event const *e)
{
		MSG0("WriteI2C_Do\n");

	   switch (e->sig) {
	   case SCF_STATE_ENTRY_SIG:
		   TRACE_I2C("\t\t WriteI2C_Do SCF_STATE_ENTRY_SIG\n",NULL,NULL,NULL,NULL,NULL,NULL);
		   oComI2C.I2CComStruct.currentNb_Element=0;
		   oComI2C.I2CComStruct.step=0;
		   /* step1 : get the coin */
		   TRACE_I2C("\t WRITE-I2C STEP 1 \n",NULL,NULL,NULL,NULL,NULL,NULL);
		   TRACE_I2C("\t\t\t ask to read Status_e_Coin_Status + AccessControl_e_Coin_Command\n",NULL,NULL,NULL,NULL,NULL,NULL);
		   //TRACE_I2C("\t\t\t\t step %d, coin %d \n",oComI2C.I2CComStruct.step,oComI2C.I2CComStruct.coin,NULL,NULL,NULL,NULL);
		   //pIspctlCom->requestPE(HostToMasterI2CAccessStatus_e_Coin_Status_Byte0);
		   pIspctlCom->queuePE(HostToMasterI2CAccessStatus_e_Coin_Status_Byte0, 0);
		   pIspctlCom->queuePE(HostToMasterI2CAccessControl_e_Coin_Command_Byte0, 0);
		   pIspctlCom->readQueue();
		   return 0;

		case EVT_ISPCTL_LIST_INFO_SIG:
			TRACE_I2C("\t\t WriteI2C_Do EVT_ISPCTL_LIST_INFO_SIG\n",NULL,NULL,NULL,NULL,NULL,NULL);
			if(e->type.ispctlInfo.info_id == ISP_WRITELIST_DONE)
			{
			   TRACE_I2C("\t WRITE-I2C STEP 3 \n",NULL,NULL,NULL,NULL,NULL,NULL);
			   TRACE_I2C("\t\t\t once value are written, check that the I2C resource is free\n",NULL,NULL,NULL,NULL,NULL,NULL);
			   //TRACE_I2C("\t\t\t\t step %d, coin %d \n",oComI2C.I2CComStruct.step,oComI2C.I2CComStruct.coin,NULL,NULL,NULL,NULL);
				pIspctlCom->checkPEValue(HostToMasterI2CAccessStatus_e_Result_Driver_Byte0, Result_e_Success);
			}
			else if(e->type.ispctlInfo.info_id == ISP_READLIST_DONE)
			{
				//e->type.ispctlInfo.Listvalue[x].pe_data
				TRACE_I2C("\t\t Coin_Status Listvalue[0].pe_data = %d\n",e->type.ispctlInfo.Listvalue[0].pe_data,NULL,NULL,NULL,NULL,NULL);
				TRACE_I2C("\t\t Coin_Command Listvalue[1].pe_data = %d\n",e->type.ispctlInfo.Listvalue[1].pe_data,NULL,NULL,NULL,NULL,NULL);

				if(e->type.ispctlInfo.Listvalue[0].pe_data != e->type.ispctlInfo.Listvalue[1].pe_data)
				{
				   TRACE_I2C("\t WRITE-I2C STEP 1 BIS \n",NULL,NULL,NULL,NULL,NULL,NULL);
				   TRACE_I2C("\t\t\t ask to read Status_e_Coin_Status + AccessControl_e_Coin_Command\n",NULL,NULL,NULL,NULL,NULL,NULL);
				   //TRACE_I2C("\t\t\t\t step %d, coin %d \n",oComI2C.I2CComStruct.step,oComI2C.I2CComStruct.coin,NULL,NULL,NULL,NULL);
				   //pIspctlCom->requestPE(HostToMasterI2CAccessStatus_e_Coin_Status_Byte0);
				   pIspctlCom->queuePE(HostToMasterI2CAccessStatus_e_Coin_Status_Byte0, 0);
				   pIspctlCom->queuePE(HostToMasterI2CAccessControl_e_Coin_Command_Byte0, 0);
				   pIspctlCom->readQueue();
				   return 0;
				}

				if(oComI2C.I2CComStruct.currentNb_Element!=oComI2C.I2CComStruct.aNb_Element)
				{
				   TRACE_I2C("\t WRITE-I2C STEP 2 \n",NULL,NULL,NULL,NULL,NULL,NULL);
				   TRACE_I2C("\t\t\t write the values in I2C\n",NULL,NULL,NULL,NULL,NULL,NULL);
				   //TRACE_I2C("\t\t\t\t step %d, coin %d \n",oComI2C.I2CComStruct.step,oComI2C.I2CComStruct.coin,NULL,NULL,NULL,NULL);
					//oComI2C.I2CComStruct.coin= e->type.ispctlInfo.value;
					oComI2C.I2CComStruct.coin= e->type.ispctlInfo.Listvalue[0].pe_data;

					#define DEVICE_ID 0x20

					pIspctlCom->queuePE(HostToMasterI2CAccessControl_u16_DeviceID_Byte0, DEVICE_ID);
					pIspctlCom->queuePE(HostToMasterI2CAccessControl_u16_Index_Byte0, (t_uint32)oComI2C.I2CComStruct.pReg[oComI2C.I2CComStruct.currentNb_Element].I2CAddr);
					pIspctlCom->queuePE(HostToMasterI2CAccessControl_e_HostToMasterI2CRequest_Request_Byte0, (t_uint32)HostToMasterI2CRequest_e_WriteNBytes);

					switch(oComI2C.I2CComStruct.controlType)
					{
						case I2C_Control_WriteByte:
							TRACE_I2C("\t\t\t type I2C_Control_WriteByte\n",NULL,NULL,NULL,NULL,NULL,NULL);
							pIspctlCom->queuePE(HostToMasterI2CAccessControl_u8_NoBytesReadWrite_Byte0, 1);
							pIspctlCom->queuePE(HostToMasterI2CAccessData_u8_arrData_0_Byte0, (t_uint32)oComI2C.I2CComStruct.pReg[oComI2C.I2CComStruct.currentNb_Element].I2CValue);
							break;
						case I2C_Control_WriteWord:
							TRACE_I2C("\t\t\t type I2C_Control_WriteWord\n",NULL,NULL,NULL,NULL,NULL,NULL);
							TRACE_I2C("\t\t\t currentNb_Element = %d\n",oComI2C.I2CComStruct.currentNb_Element,NULL,NULL,NULL,NULL,NULL);
							TRACE_I2C("\t\t\t I2CValue = 0x%x\n",oComI2C.I2CComStruct.pReg[oComI2C.I2CComStruct.currentNb_Element].I2CValue,NULL,NULL,NULL,NULL,NULL);
							pIspctlCom->queuePE(HostToMasterI2CAccessControl_u8_NoBytesReadWrite_Byte0, 2);
							pIspctlCom->queuePE(HostToMasterI2CAccessData_u8_arrData_0_Byte0, oComI2C.I2CComStruct.pReg[oComI2C.I2CComStruct.currentNb_Element].I2CValue>>8);
							pIspctlCom->queuePE(HostToMasterI2CAccessData_u8_arrData_1_Byte0, (t_uint32)oComI2C.I2CComStruct.pReg[oComI2C.I2CComStruct.currentNb_Element].I2CValue);
							break;
						default:
							TRACE_I2C("\t\t\t type I2C_Control_ unknown\n",NULL,NULL,NULL,NULL,NULL,NULL);
							DBC_ASSERT(0);
					}
					oComI2C.I2CComStruct.coin= (oComI2C.I2CComStruct.coin+1)%0xFF;
					TRACE_I2C("\t\t\t ask to toggle coin : %d\n",oComI2C.I2CComStruct.coin,NULL,NULL,NULL,NULL,NULL);
					pIspctlCom->queuePE(HostToMasterI2CAccessControl_e_Coin_Command_Byte0, oComI2C.I2CComStruct.coin);
					pIspctlCom->processQueue();
				}
				else
				{
				   TRACE_I2C("\t WRITE-I2C STEP LAST \n",NULL,NULL,NULL,NULL,NULL,NULL);
				   TRACE_I2C("\t\t\t end of the whole state machine once oComI2C.I2CComStruct.currentNb_Element is oComI2C.I2CComStruct.aNb_Element\n",NULL,NULL,NULL,NULL,NULL,NULL);
				   //TRACE_I2C("\t\t\t\t step %d, coin %d \n",oComI2C.I2CComStruct.step,oComI2C.I2CComStruct.coin,NULL,NULL,NULL,NULL);
				   SCF_TRANSIT_TO_PSTATE(whatNext);
                   whatNext = NULL; // To avoid any confusion.
				}
			}
			else
			{
				DBC_ASSERT(0);
			}
			return 0;

		case EVT_ISPCTL_INFO_SIG:
			TRACE_I2C("\t\t WriteI2C_Do EVT_ISPCTL_INFO_SIG\n",NULL,NULL,NULL,NULL,NULL,NULL);

			if(e->type.ispctlInfo.info_id == ISP_READ_DONE)
			{
				//e->type.ispctlInfo.value
				DBC_ASSERT(0);
			}
			else if (e->type.ispctlInfo.info_id ==ISP_POLLING_PE_VALUE_DONE)
			{
			   TRACE_I2C("\t WRITE-I2C STEP 4 \n",NULL,NULL,NULL,NULL,NULL,NULL);
			   TRACE_I2C("\t\t\t relaunch til we did not reach the number of iterations\n",NULL,NULL,NULL,NULL,NULL,NULL);
			   //TRACE_I2C("\t\t\t\t step %d, coin %d \n",oComI2C.I2CComStruct.step,oComI2C.I2CComStruct.coin,NULL,NULL,NULL,NULL);
			   oComI2C.I2CComStruct.currentNb_Element++;
			   //pIspctlCom->requestPE(HostToMasterI2CAccessStatus_e_Coin_Status_Byte0);
			   pIspctlCom->queuePE(HostToMasterI2CAccessStatus_e_Coin_Status_Byte0, 0);
			   pIspctlCom->queuePE(HostToMasterI2CAccessControl_e_Coin_Command_Byte0, 0);
			   pIspctlCom->readQueue();
			}
			else if (e->type.ispctlInfo.info_id ==ISP_HOST_TO_SENSOR_ACCESS_COMPLETE)
			{
				TRACE_I2C("\t\t WRITE-I2C ISP_HOST_TO_SENSOR_ACCESS_COMPLETE",NULL,NULL,NULL,NULL,NULL,NULL);
			}
			else
			{
				TRACE_I2C("\t\t WRITE-I2C ispctlInfo.info_id == not supported",NULL,NULL,NULL,NULL,NULL,NULL);
				DBC_ASSERT(0);
			}
			return 0;
        case SCF_STATE_EXIT_SIG: EXIT; return 0;
        default: break;
	   }

	return SCF_STATE_PTR(&COM_SM::WriteI2C);
}

