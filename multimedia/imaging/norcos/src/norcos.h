#pragma once

#ifdef WIN32_STUB_COMPILATION
#include "win32_stub_compilation_include.h"
#endif /* WIN32_STUB_COMPILATION */

#include "AIQ_CommonTypes.h"
#include "OMX_extensions/norcos_OMX_extensions.h"

/* opaque type to store context */
typedef void* NorcosHandler_t;

/* external API */
AIQ_Error NorcosInit                 (NorcosHandler_t *Hdl,AIQ_U32 u32_width);
AIQ_Error NorcosSetTuningParams      (NorcosHandler_t Hdl,NorcosTuningParams_t *p_TuningParams);
AIQ_Error NorcosSetControlMode       (NorcosHandler_t Hdl,NorcosControlMode_e  NorcosControlMode);
AIQ_Error NorcosSetBufferSharingMode (NorcosHandler_t Hdl,NorcosControlMode_e  NorcosControlMode);
AIQ_Error NorcosFilter               (NorcosHandler_t Hdl,AIQ_Frame *p_Input,AIQ_Frame *p_Output);
AIQ_Error NorcosDeInit               (NorcosHandler_t Hdl);
