/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef _ISPPROC_PORT_H_
#define _ISPPROC_PORT_H_

#include <cm/inc/cm.hpp>
#include "ifmport.h"
#include "grab_types.idt.h"


#define ISPPROC_NB_OUT_PORTS 2
#define ISPPROC_NB_IN_PORTS 1
#define ISPPROC_NB_PORTS (ISPPROC_NB_OUT_PORTS+ISPPROC_NB_IN_PORTS)



#define ISPPROC_PORT_OUT1 2
#define ISPPROC_PORT_OUT0 1
#define ISPPROC_PORT_IN0 0



#ifdef __SYMBIAN32__
NONSHARABLE_CLASS(ispprocport);
#endif
class ispprocport: public ifmport {
   public:
      static const int DEFAULT_WIDTH = 640;
      static const int DEFAULT_HEIGHT = 480;
      static const OMX_COLOR_FORMATTYPE DEFAULT_COLOR_FORMAT = OMX_COLOR_FormatCbYCrY;
      static const  OMX_COLOR_FORMATTYPE DEFAULT_BAYER_COLOR_FORMAT = OMX_COLOR_FormatRawBayer8bit;
      static const int DEFAULT_FRAMERATE = 20;


      ispprocport(const EnsCommonPortData& commonPortData, ENS_Component& ensComp)
         : ifmport(commonPortData, ensComp){
            mBytesPerPixel = 4;
            extradataSize = 0;
            portSettingsChanged=OMX_FALSE;
            mInternalFrameWidth = DEFAULT_WIDTH;
            mInternalFrameHeight = DEFAULT_HEIGHT;
          }
      virtual ~ispprocport() {};

      virtual OMX_ERRORTYPE setFormatInPortDefinition(const OMX_PARAM_PORTDEFINITIONTYPE &portdef) ;
      void setDefaultFormatInPortDefinition(OMX_PARAM_PORTDEFINITIONTYPE *);

      virtual OMX_ERRORTYPE checkFormatInPortDefinition(const OMX_PARAM_PORTDEFINITIONTYPE &portdef);


      void setExtraDataSize(OMX_U32 size) {
		  extradataSize = size;
      }




      OMX_BOOL portSettingsChanged;
	
	OMX_BOOL isSupportedFmt(t_uint16 portID, OMX_COLOR_FORMATTYPE omxformat);
	void getHwConstraints(t_uint16 portId,OMX_COLOR_FORMATTYPE omxformat, t_uint16 * p_multiple_width, t_uint16 * p_multiple_height, t_uint16 * p_multiple_stride); 
	  
	  
	  
	  
   private:
      OMX_U32 	extradataSize;

};



#endif // _ISPPROC_PORT_H_
