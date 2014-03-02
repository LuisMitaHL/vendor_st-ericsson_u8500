/*****************************************************************************
* Copyright (C) ST-Ericsson SA 2010. All rights reserved.
* This code is ST-Ericsson proprietary and confidential.
* Any use of the code for whatever purpose is subject to
* specific written permission of ST-Ericsson SA.
*****************************************************************************/
#ifndef _NORCOS_PROCESS_H_
#define _NORCOS_PROCESS_H_
#include <stdio.h>
#include <OMX_Types.h>
#include <OMX_Component.h>
#include <Component.h> // because we NMF-extend fsm.component.component
#include <IFM_Types.h>
#if defined(NORCOS_DAMPERS_SUPPORT)
#include "norcos_dampers.h"
#include "tuning_data_base.h"
#endif //NORCOS_DAMPERS_SUPPORT


enum
{
	eError_None =0,
	eError_UnknowFlipMode,
	eError_WrongAlgoID,
	eError_InputAndOutputFormatDifferent,
	eError_UnsupportedFormat,
	eError_SizesAreDifferent,
	eError_WrongOuptutSize,
	eError_MetadataPtrIsNull,
	eError_MissingParam1,
	eError_MissingParam2,
	eError_MissingParam3,
	eError_MissingParam4,
};

#if defined(NORCOS_DAMPERS_SUPPORT)
typedef enum
{
   NORCOS_DAMPERS_OK = 0,
   NORCOS_DAMPERS_BINDING_TO_DATABASE_FAILED,
   NORCOS_DAMPERS_CONSTRUCT_FAILED,
   NORCOS_DAMPERS_TUNING_ERROR,
   NORCOS_DAMPERS_CONFIGURE_FAILED,
   NORCOS_DAMPERS_GET_EXTRA_DATA_FAILED,
   NORCOS_DAMPERS_COPY_EXTRA_DATA_FAILED,
   NORCOS_DAMPERS_EVALUATION_FAILED,
   NORCOS_DAMPERS_INTERNAL_ERROR,
   NORCOS_DAMPERS_FILE_NOT_FOUND,
   NORCOS_DAMPERS_TUNNING_NOT_OK,
} t_norcos_dampers_error_code;
#endif //NORCOS_DAMPERS_SUPPORT

class Norcos_proc: public openmax_processor
//*************************************************************************************************************
{
public:
	Norcos_proc();
	virtual ~Norcos_proc();
	virtual void process();
	enum
	{
		eInputPort_Img       =0,
		eOutputPort_Img      =1,
	};

	bool UseBufferSharing() const { return m_bUseBufferSharing; }
protected:
	virtual int GetConvertionConfig (_tConvertionConfig &Config, bool bDequeue);
	virtual int UserOpen();

#if defined(NORCOS_DAMPERS_SUPPORT)
        CTuningDataBase* m_pCTuningDataBase;
        CNorcosDamperFlatDetectGrade m_CNorcosDamperFlatDetectGrade;
        CNorcosDamperFlatParam m_CNorcosDamperFlatParam;
        t_norcos_dampers_error_code m_NorcosDampersErrorCode;
        bool m_bNorcosDampersConfigured;

        t_norcos_dampers_error_code Norcos_ConfigureDampers();
        const char* ErrorCode2String(const t_norcos_dampers_error_code);
#endif //NORCOS_DAMPERS_SUPPORT
};


#endif //_NORCOS_PROCESS_H_
