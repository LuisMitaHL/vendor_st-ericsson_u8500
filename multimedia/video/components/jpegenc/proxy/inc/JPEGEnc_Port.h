/******************************************************************************
 Copyright (c) 2009-2011, ST-Ericsson SA
 All rights reserved.
 
 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions
 are met:
 
   1. Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
   2. Redistributions in binary form must reproduce the above
      copyright notice, this list of conditions and the following
      disclaimer in the documentation and/or other materials
      provided with the distribution.
   3. Neither the name of the ST-Ericsson SA nor the names of its
      contributors may be used to endorse or promote products
      derived from this software without specific prior written
      permission.
 
 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 OF THE POSSIBILITY OF SUCH DAMAGE.
 ******************************************************************************/
#include "JPEGEnc_Proxy.h"
#include "JPEGEnc_ParamAndConfig.h"
#include "VFM_Port.h"

class JPEGEnc_Port:public VFM_Port
{

public:
    JPEGEnc_ParamAndConfig *Param;
//	JPEGEnc_Proxy *pProxyComponent;
	OMX_S32 getStride();
	OMX_U32 getSliceHeight();
	OMX_U32 getBufferSize();
	void setBufferSize(OMX_U32);
	JPEGEnc_Port(const EnsCommonPortData& commonPortData, ENS_Component &enscomp, JPEGEnc_ParamAndConfig *mParam);

    virtual void getSlavedSizeFromGivenMasterSize(OMX_U32 nWidthMaster, OMX_U32 nHeightMaster, OMX_U32 *nWidthSlave, OMX_U32 *nHeightSlave) const;

protected:
	virtual OMX_ERRORTYPE setFormatInPortDefinition(const OMX_PARAM_PORTDEFINITIONTYPE& portDef);


	virtual OMX_U32 getBufferSize() const;

	virtual OMX_ERRORTYPE checkSetFormatInPortDefinition(const OMX_PARAM_PORTDEFINITIONTYPE *portDef);

	void jpegenc_port_assert(OMX_ERRORTYPE omxError, OMX_U32 line, OMX_BOOL isFatal);


};
