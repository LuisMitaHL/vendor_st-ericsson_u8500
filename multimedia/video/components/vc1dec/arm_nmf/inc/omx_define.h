/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */


#ifndef __osi_omx_define_h__
#define __osi_omx_define_h__
#ifndef PROXY

#define	OSI_OMX_BUFFERFLAG_EOS		      0x1
#define	OSI_OMX_BUFFERFLAG_DECODEONLY		0x4

#else
// include ENS include file definition here
#define	OSI_OMX_BUFFERFLAG_EOS		OMX_BUFFERFLAG_EOS
#define	OSI_OMX_BUFFERFLAG_DECODEONLY		OMX_BUFFERFLAG_DECODEONLY

#endif

#endif
