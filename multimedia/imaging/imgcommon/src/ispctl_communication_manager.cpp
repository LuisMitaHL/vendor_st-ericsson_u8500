/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
/* \brief   This file contains the methods to communicate to the XP70 through ISPTCL (read/write page elements)
*               It uses the "mIspctlCommand" interface coming from the CIspctlComponentManager to send message to the ISPTCL
*               The answers to these messsages  are received in the callbacks provided  in CIspctlComponentManager.
*/

#define OMXCOMPONENT "ISPCOM"
#include "ENS_DBC.h"

#include "ispctl_communication_manager.h"
#include "osi_trace.h"
#include <stdio.h>

#include <cm/inc/cm.hpp>


// OST framework
#include "OstTraceDefinitions_ste.h"
#include "OpenSystemTrace_ste.h"

#ifdef OST_TRACE_COMPILER_IN_USE
#include "imgcommon_src_ispctl_communication_managerTraces.h"
#endif

/* for traces */
#undef OMX_TRACE_UID
#define OMX_TRACE_UID 0x8

#define CLIENT_NAME_FROM_ID(id) ((id)==SIA_CLIENT_CAMERA?"CAMERA":"ISPPROC")
CSensor* CIspctlCom::pIspctlSensor = NULL;

/* for convertion */
float CIspctlCom::u32tofloat(t_uint32 value){
	union 	{
		float f_value;
		t_uint32 u32_value;
		}v;
	v.u32_value = value;
	return v.f_value;
}

t_uint32 CIspctlCom::floattou32(float value){
	union 	{
		float f_value;
		t_uint32 u32_value;
		}v;
	v.f_value = value;
	return v.u32_value;
}	

void CIspctlCom::setIspctlCmd(Iispctl_api_cmd val){
	mIspctlCommand = val;
}

Iispctl_api_cmd CIspctlCom::getIspctlCmd(){
	return mIspctlCommand;
}

/*
 * fill an internal table with a list of PE to be sent
 * a call to processQueue function must follow
 */
void CIspctlCom::queuePE(t_uint16 addr, t_uint32 data) {

	queue[queueIndex].pe_addr =addr;
	queue[queueIndex].pe_data=data;

	if(queueIndex < NB_MAX) queueIndex++;
	else DBC_ASSERT(0);
}

void CIspctlCom::queuePE(t_uint16 addr, float data) {
typedef union {
	float f_value;
	t_uint32 u32_value;
	}t_floatPE;

	t_floatPE dataConvertion;
	dataConvertion.f_value = data;
	queuePE(addr, dataConvertion.u32_value);

}

void CIspctlCom::queuePE(t_uint16 addr, int data) {
	
	queuePE(addr, (t_uint32)data);

}
void CIspctlCom::queuePE(t_uint16 addr, OMX_S32 data) {
	
	queuePE(addr, (t_uint32)data);

}
void CIspctlCom::queuePE(t_uint16 addr, double data) {

	queuePE(addr, (float)data);

}

/*
 * will send the internal table as a list of PE to the Xp70 FW
 */
void CIspctlCom:: processQueue(void) {
    writeListPE();
    queueIndex=0;
}

/*
 * will read a list of PE
 */
void CIspctlCom:: readQueue(void) {
	readListPE();
	queueIndex=0;
}


/* will ask DSP to wait for pe @ addr taking the value pe_value
 * the wait duration is hardcoded in this function (4000)
 * the function will return 0 if after this duration the pe has taken the expected value else returns 1
 */
t_uint16 CIspctlCom::checkPEValue(t_uint16 addr, t_uint32 pe_value){

	t_uint32 data=0xFFFF;
	waitExpectedPeValue(addr, pe_value, &data, 4000);
	if(data == pe_value) return 0;
	else  {
		return 1;
	}

}

/*
 * will read one PE
 */
void CIspctlCom::requestPE(t_uint16 addr)
{
	if(mPanic==OMX_TRUE)return;
	//MSG2("[%s] Read PE: %s\n",CLIENT_NAME_FROM_ID(clientId),CXp70::GetPeName(addr));
	DBGT_PDEBUG("[%s] Read PE: %s\n",CLIENT_NAME_FROM_ID(clientId),pIspctlSensor->GetPeName(addr));
	//OstTraceFiltStatic2(TRACE_DEBUG, "[%s] Read PE: %s", (mTraceObject),CLIENT_NAME_FROM_ID(clientId),CXp70::GetPeName(addr));
	mIspctlCommand.readPageElement(addr, clientId);
}

/*
 * will write one PE
 */
void CIspctlCom::writePE(t_uint16 addr, t_uint32 data)
{
	if(mPanic==OMX_TRUE)return;
	//MSG3("[%s] Write PE: %s val:0x%lx\n",CLIENT_NAME_FROM_ID(clientId),CXp70::GetPeName(addr),data);
	DBGT_PDEBUG("[%s] Write PE: %s val:0x%lx\n",CLIENT_NAME_FROM_ID(clientId),pIspctlSensor->GetPeName(addr),data);
	
	//OstTraceFiltStatic3(TRACE_DEBUG, "[%s] Write PE: %s val:0x%lx", (mTraceObject),CLIENT_NAME_FROM_ID(clientId),CXp70::GetPeName(addr),data);
	mIspctlCommand.writePageElement(addr, data, clientId);
}


/*
 * this one is used in the processQueue function
 */
void CIspctlCom::writeListPE(void)
{
	if(mPanic==OMX_TRUE)return;
	for(t_uint16 i=0; i<queueIndex;i++)
	{
#if (!defined(__SYMBIAN32__)&&defined(CONFIG_DEBUG))||(defined(__SYMBIAN32__)&&defined(_DEBUG))
		// Trace msg manually built because usual MSGx macros have 4 arguments max
		char msg[256]="";
		snprintf(msg,sizeof(msg),"[%s] Write PE: %s val:0x%lx (dec:%ld) (float:%f)",
		            CLIENT_NAME_FROM_ID(clientId),
		            pIspctlSensor->GetPeName(queue[i].pe_addr),
		            queue[i].pe_data,
		            (t_sint32)queue[i].pe_data,
		            u32tofloat(queue[i].pe_data));
		//MSG1("%s\n",msg);
		DBGT_PDEBUG("%s \n",msg);
		//OstTraceFiltStatic1(TRACE_DEBUG, "%s", (mTraceObject),msg);
#endif
	}
	mIspctlCommand.writeListPageElement(queue, queueIndex, clientId);
}

void CIspctlCom::readListPE(void)
{
	if(mPanic==OMX_TRUE)return;
	for(t_uint16 i=0; i<queueIndex;i++)
	{
		//MSG2("[%s] Read PE: %s\n",CLIENT_NAME_FROM_ID(clientId),CXp70::GetPeName(queue[i].pe_addr));
		DBGT_PDEBUG("[%s] Read PE: %s\n",CLIENT_NAME_FROM_ID(clientId),pIspctlSensor->GetPeName(queue[i].pe_addr));
		//OstTraceFiltStatic2(TRACE_DEBUG, "[%s] Read PE: %s", (mTraceObject),CLIENT_NAME_FROM_ID(clientId),CXp70::GetPeName(queue[i].pe_addr));
	}
	mIspctlCommand.readListPageElement(queue, queueIndex, clientId);
}

/*
 * this one is used in the checkPEValue function
 */
void CIspctlCom::waitExpectedPeValue(t_uint16 addr, t_uint32 value_expected, t_uint32 * value, t_uint32 timeout)
{
	if(mPanic==OMX_TRUE)return;
	MSG2("waitExpectedPeValue PE:%s val:0x%lx\n",pIspctlSensor->GetPeName(addr),value_expected);
	//OstTraceFiltStatic2(TRACE_DEBUG, "waitExpectedPeValue PE:%s val:0x%lx", (mTraceObject),CXp70::GetPeName(addr),value_expected);
	mIspctlCommand.WaitExpectedPeValue(addr, value_expected, 40, timeout, clientId);
}


void CIspctlCom::allowSleep(void)
{
	if(mPanic==OMX_TRUE)return;
	mIspctlCommand.allowSleep(); 
}

void CIspctlCom::preventSleep(void)
{
	if(mPanic==OMX_TRUE)return;
	mIspctlCommand.preventSleep(); 
}
void CIspctlCom::setIspctlSensor(CSensor * iIspctlSensor)
{
	pIspctlSensor = iIspctlSensor; 
}