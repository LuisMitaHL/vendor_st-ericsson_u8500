/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include "protothread.h"



ProtoThread::ProtoThread()
: mStatus(eStatus_Idle),mOnCall(false)
//***************************************************************************************
{
	Reset(true);
	Start();
}

int ProtoThread::Start()
//***************************************************************************************
{
	if (mOnCall==false)
	{ // reset info
		Reset(true);
		mProtoThreadState=0;
		//PT_INIT(this);
	}
	else
		mOnCall=false; //Clear the flag
	return(0);
}


int ProtoThread::Reset(bool force)
//***************************************************************************************
{
	if ((mOnCall==true) && (force==false))
		return 0;
	mThreated=false;
	mProtoThreadState=0;
	mOnCall=false;
	return(0);
}

int ProtoThread::ptBegin()
//***************************************************************************************
{
	mThreated=false;
	return(0);
}


//***************************************************************************************
//***************************************************************************************
//***************************************************************************************
//***************************************************************************************

HSMProtoThread::HSMProtoThread(COM_SM &hsm)
: ProtoThread()
, m_HSM(hsm)
//***************************************************************************************
{
}

