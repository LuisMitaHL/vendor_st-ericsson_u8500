/*****************************************************************************
* Copyright (C) ST-Ericsson SA 2010. All rights reserved.
* This code is ST-Ericsson proprietary and confidential.
* Any use of the code for whatever purpose is subject to
* specific written permission of ST-Ericsson SA.
*****************************************************************************/
#include "wom_shared.h"
#include <stdio.h>
#include <stdarg.h>

#include <omxil/OMX_Core.h>
#include <omxil/OMX_IVCommon.h>

#include "wom_tools.h"

int WomReportCodeError(const char *module, unsigned int line)
//*************************************************************************************************************
{
	if ((module == NULL) || (*module == '\0') )
		printf("\n******************** code ERROR ********************\n");
	else
		printf("\n******************** code ERROR %s:%d********************\n", module, line);
	return(S_OK);
}

//*************************************************************************************************************
//*************************************************************************************************************
//*************************************************************************************************************
//*************************************************************************************************************

void sWomBufferInfo::InitDefault()
//*************************************************************************************************************
{
	pBufferHdr = NULL;
	Format     = OMX_COLOR_FormatUnused;
	Width      = 0;
	Height     = 0;
	Stride     = 0;
	SliceHeight= 0;
	pPort      = NULL;
	Error      = 0;
	//pImageInfo = NULL;
}


#if 0
const _sWomImageFormatDescription* GetWomImageFormatDescription(int format)
{
	return(NULL);
}
#endif

#if 1
extern "C"
{
	void dbc_assert(bool ) {}
}
#endif

