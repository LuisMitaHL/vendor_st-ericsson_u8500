/*****************************************************************************
* Copyright (C) ST-Ericsson SA 2010. All rights reserved.
* This code is ST-Ericsson proprietary and confidential.
* Any use of the code for whatever purpose is subject to
* specific written permission of ST-Ericsson SA.
*****************************************************************************/
#ifndef _WOMDEMOCPT_PROXY_H_
#define _WOMDEMOCPT_PROXY_H_

class womDemoCpt_Proxy : public Wom_Proxy
//*************************************************************************************************************
{
public:
	womDemoCpt_Proxy(ImgEns_Component &enscomp);
	~womDemoCpt_Proxy();
protected :
	I_ToEnsComponent   mI_ToEnsComponent; // Interface for speaking with Ens component
};

#endif // _WOMDEMOCPT_PROXY_H_
