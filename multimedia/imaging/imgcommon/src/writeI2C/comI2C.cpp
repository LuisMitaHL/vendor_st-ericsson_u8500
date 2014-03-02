/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
#define OMXCOMPONENT "COM_I2C"
#include "osi_trace.h"
#include "comI2C.h"


#undef OMX_TRACE_UID
#define OMX_TRACE_UID 0x8


CComI2C::CComI2C()
{
	//IN0("\n");
	I2CComStruct.pReg = NULL;
	I2CComStruct.aNb_Element = 0;
	I2CComStruct.currentNb_Element = 0;
	I2CComStruct.coin = 0;
	I2CComStruct.step = 0;
	I2CComStruct.controlType = I2C_Control_None;
	//OUT0("\n");
}

CComI2C::~CComI2C()
{
	//IN0("\n");
	//OUT0("\n");
}

void CComI2C::configure(ts_I2CReg *pRegister, t_uint32 nbElement, e_I2C_Control_Type ctlType)
{
	//IN0("\n");
	I2CComStruct.pReg = pRegister;
	I2CComStruct.aNb_Element = nbElement;
	I2CComStruct.controlType = ctlType;
	//OUT0("\n");
}
