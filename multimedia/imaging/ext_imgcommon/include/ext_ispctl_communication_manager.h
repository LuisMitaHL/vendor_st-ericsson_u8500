/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef _EXT_ISPCTL_COMMUNICATION_MANAGER_H_
#define _EXT_ISPCTL_COMMUNICATION_MANAGER_H_

#include "ext_nmf_eventhandler.h"
#include "host/ext_ispctl/api/cmd.hpp"
#include "ext_client.h"
#include "OMX_Core.h"

enum enumExtCameraSlot
{
	ePrimaryExtCamera,
	eSecondaryExtCamera,
};

#ifdef __SYMBIAN32__
NONSHARABLE_CLASS(CExtIspctlCom);
#endif
class CExtIspctlCom
{
public :

	/* constructor */
    /* +353212 */
    CExtIspctlCom(t_sia_client_id client_id):clientId(client_id),mPanic(OMX_FALSE){
    }
    /* -353212 */

	void setExtIspctlCmd(Iext_ispctl_api_cmd val);

	void requestPE(t_uint16 addr);
	Iext_ispctl_api_cmd getExtIspctlCmd();

    /* +353212 */
    void setPanic(OMX_BOOL panic){mPanic=panic;}
    /* -353212 */

private :
	Iext_ispctl_api_cmd mExtIspctlCommand;
	t_sia_client_id clientId;

    /* +353212 */
    OMX_BOOL mPanic;
    /* -353212 */
};
#endif /*_EXT_ISPCTL_COM_H_*/
