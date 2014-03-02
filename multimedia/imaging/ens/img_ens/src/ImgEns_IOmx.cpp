/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
#include <stdlib.h>
#include <stdio.h>
#include "ImgEns_IOmx.h"

ImgEns_IOmx::ImgEns_IOmx(void)
//***************************************************************************************
{
	m_OmxHandle= NULL;
	m_pWrapper = NULL;
	m_Name     = "Base ImgEns_IOmx class";
	mPortCount = 0;
}

ImgEns_IOmx::~ImgEns_IOmx(void)
//***************************************************************************************
{
}

OMX_ERRORTYPE ImgEns_IOmx::setOMXHandle(OMX_HANDLETYPE hComp)
//***************************************************************************************
{
	IMGENS_ASSERT(m_OmxHandle==NULL); // should be null
	m_OmxHandle = hComp;
	return (OMX_ErrorNone);
}

const char *ImgEns_IOmx::getName() const
//***************************************************************************************
{
	return(m_Name);
}

void ImgEns_IOmx::setName(const char *cName)
//***************************************************************************************
{
	m_Name=cName;
}

OMX_VERSIONTYPE ImgEns_IOmx::getVersion(void) const
//***************************************************************************************
{
	return(getImgOmxIlSpecVersion());
}

