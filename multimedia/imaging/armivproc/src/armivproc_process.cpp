/*****************************************************************************
* Copyright (C) ST-Ericsson SA 2010. All rights reserved.
* This code is ST-Ericsson proprietary and confidential.
* Any use of the code for whatever purpose is subject to
* specific written permission of ST-Ericsson SA.
*****************************************************************************/
#include <stdio.h>
#include <string.h>

#undef  OMX_TRACE_UID
#define OMX_TRACE_UID 0x8

#undef    OMXCOMPONENT
#define   OMXCOMPONENT "ArmIVProc"
#include "osi_trace.h"
#include "extradata.h"
#include "wrapper_openmax_lib.h"
#include "ImageConverter_lib.h"
#include "armivproc_proxy.h"
#include "armivproc_process.h"


#ifdef __SYMBIAN32__
	#define PERF_LOG_FILE "f:/siaalgo/armivproc/perf.txt"
#elif defined(WORKSTATION)
	#define PERF_LOG_FILE "perf.txt"
#else
	#define PERF_LOG_FILE "/sdcard/siaalgo/armivproc/perf.txt"
#endif

typedef struct
{
	int OmxValue;
	int InternalValue;
} _tCorrespondanceTable;

const _tCorrespondanceTable FormatTable[]=
{
	{OMX_SYMBIAN_COLOR_FormatYUV420MBPackedSemiPlanar , eImageFormat_YUV420mb   },
	{OMX_COLOR_FormatYUV422SemiPlanar                 , eImageFormat_YUV422mb   },
	{OMX_COLOR_FormatYUV420SemiPlanar                 , eImageFormat_NV21       }, // YUV in 2 planes: Y and V/U interleaved in 2*2
	{OMX_COLOR_FormatYUV420PackedPlanar               , eImageFormat_YUV420_blue}, // YUV 420 3 planes Y, Cr, Cb 1:4:4 (YUV420 Packed Planar page 309)
	{OMX_COLOR_FormatYUV422PackedPlanar               , eImageFormat_YUV422_blue}, // YUV 422 3 planes Y, Cr, Cb 1:2:2
	{OMX_COLOR_FormatCbYCrY                           , eImageFormat_CbYCrY     }, // YUV 422 interleaved  16 bits/pixels
	{OMX_COLOR_FormatYCbYCr                           , eImageFormat_YCbYCr     }, // YUV 422 interleaved  16 bits/pixels
	{OMX_COLOR_FormatCrYCbY                           , eImageFormat_CrYCbY     }, // YUV 422 interleaved  16 bits/pixels
	{OMX_COLOR_FormatYCrYCb                           , eImageFormat_YCrYCb     }, // YUV 422 interleaved  16 bits/pixels
	{OMX_COLOR_Format16bitRGB565                      , eImageFormat_RGB565     },
	{OMX_COLOR_Format24bitRGB888                      , eImageFormat_RGB888     },
	{OMX_COLOR_FormatRawBayer8bit                     , eImageFormat_Raw8       },
	{OMX_COLOR_Format32bitARGB8888                    , eImageFormat_ARGB8888   },
	{OMX_COLOR_Format32bitBGRA8888                    , eImageFormat_BGRA8888   },
};

const _tCorrespondanceTable RotationTable[]=
{ //make correspondance between OMX value and internal ones
	{ 0 , eRotation_None },
	{ 90, eRotation_90   },
	{180, eRotation_180  },
	{270, eRotation_270  },
};

const _tCorrespondanceTable MirrorTable[]=
{//make correspondance between OMX value and internal ones
	{OMX_MirrorNone      , eMirror_None   },
	{OMX_MirrorVertical  , eMirror_Y      },
	{OMX_MirrorHorizontal, eMirror_X      },
	{OMX_MirrorBoth      , eMirror_XY     },
};


const _tCorrespondanceTable SceneOrientationTable[]=
{ //make correspondance between OMX value and internal ones
	{ OMX_SYMBIAN_OrientationNotSpecified         , eRotation_None }, /**< Orientatio of frame is not specified */
	{ OMX_SYMBIAN_OrientationRowTopColumnLeft     , eRotation_None }, /**< Frame is in normal orientation */
	{ OMX_SYMBIAN_OrientationRowTopColumnRight    , eRotation_None }, /**< Frame is mirrored */
	{ OMX_SYMBIAN_OrientationRowBottomColumnRight , eRotation_180  }, /**< Frame is 180 degrees CW rotated (=mirrored and flipped) */
	{ OMX_SYMBIAN_OrientationRowBottomColumnLeft  , eRotation_None }, /**< Frame is flipped */
	{ OMX_SYMBIAN_OrientationRowLeftColumnTop     , eRotation_90   }, /**< Frame is 90 degrees CW rotated and mirrored */
	{ OMX_SYMBIAN_OrientationRowRightColumnTop    , eRotation_270  }, /**< Frame is 270 degrees CW rotated */
	{ OMX_SYMBIAN_OrientationRowRightColumnBottom , eRotation_270  }, /**< Frame is 270 degrees CW rotated and mirrored */
	{ OMX_SYMBIAN_OrientationRowLeftColumnBottom  , eRotation_90   }, /**< Frame is 90 degrees CW rotated */
};

const _tCorrespondanceTable *GetCorrespondanceTable(const int value, const _tCorrespondanceTable *pTable, const size_t SizeTable)
//*************************************************************************************************************
{
	if (pTable==NULL)
		return(NULL);
	for (size_t i=0; i<SizeTable; ++i)
	{
		if (pTable->OmxValue== value)
		{
			return(pTable);
		}
		++pTable;
	}
	return(NULL);
}


enum_ImageFormat GetFormat(OMX_COLOR_FORMATTYPE format)
//*************************************************************************************************************
{ //return internal format
	const size_t Nbr= SIZEOF_ARRAY(FormatTable);
	const _tCorrespondanceTable *ptr=FormatTable;
	for (size_t i=0; i<Nbr; ++i)
	{
		if (ptr->OmxValue== (int)format)
		{
			return((enum_ImageFormat)ptr->InternalValue);
		}
		++ptr;
	}
	//WARNING(0);  // link error on symbian
	DBG_MSG("Unsupported OMX format %d\n", format);
	return(eImageFormat_UnknowFormat);
}

int GetRotation(int rot, eEnumRotation &aRot)
//*************************************************************************************************************
{ //transform omx rotation to internal ones
	const size_t Nbr= SIZEOF_ARRAY(RotationTable);
	const _tCorrespondanceTable *ptr=RotationTable;
	for (size_t i=0; i<Nbr; ++i)
	{
		if (ptr->OmxValue== rot)
		{
			aRot=(eEnumRotation)ptr->InternalValue;
			return(S_OK);
		}
		++ptr;
	}
	return(-1); //Not found
}

int GetRotationFromSceneOrientation(OMX_SYMBIAN_ORIENTATIONORIGINTYPE rot, eEnumRotation &aRot)
//*************************************************************************************************************
{ //transform omx rotation to internal ones
	IN0("\n");
	const size_t Nbr= SIZEOF_ARRAY(SceneOrientationTable);
	const _tCorrespondanceTable *ptr=GetCorrespondanceTable(rot, SceneOrientationTable, Nbr);
	if (ptr!=NULL)
	{
		aRot=(eEnumRotation)ptr->InternalValue;
		OUTR("", S_OK);
		return(S_OK);
	}
	aRot=eRotation_None;
	OUTR("", -1);
	return(-1); //Not found
}

int armivproc_UserSetConfig(const _tConvertionConfig &Config, int index, void *ptr)
//*************************************************************************************************************
{
	IN0("\n");
	int res=S_OK;
	ArmIVProc_process *pProc= (ArmIVProc_process*) Config.pProcessor;
	do 
	{
		if (pProc==NULL)
		{
			res=eError_ArmIVProcEngineIsNull;
			break;
		}
		ArmIVProc_Engine *m_pEngine= pProc->GetEngine();
		if (m_pEngine ==NULL)
		{
			res=eError_ArmIVProcEngineIsNull;
			break;
		}
		switch(index)
		{
		case OMX_IndexConfigCommonRotate:
			{
				OMX_CONFIG_ROTATIONTYPE *pRot=(OMX_CONFIG_ROTATIONTYPE *)ptr;
				if (pRot ==NULL)
				{
					res=eError_DataStructPtrIsNullForSetConfig;
					break;
				}
				pProc->Set_OmxRotation(pRot->nRotation);
				DBG_MSG("armivproc_UserSetConfig Set_OmxRotation(%ld)\n", pRot->nRotation); 
			}
			break;
		case OMX_IndexConfigCommonMirror:
			{ //Mirroring struct
				OMX_CONFIG_MIRRORTYPE *pMirror=(OMX_CONFIG_MIRRORTYPE *)ptr;
				if (pMirror ==NULL)
				{
					res=eError_DataStructPtrIsNullForSetConfig;
					break;
				}
				pProc->Set_OmxMirroring(pMirror->eMirror);
				DBG_MSG("UserSetConfig Set_Mirrroring(%d)\n", pMirror->eMirror); 
			}
			break;
		default:
			DBG_MSG("armivproc_UserSetConfig Unsupported index (%d-%x)\n", (int)index, (int)index); 
			break;

		}
	} while(0);
	CHECK_ERROR(res);
	OUTR("", res);
	return(res);
}

//*************************************************************************************************************
//*************************************************************************************************************
//*************************************************************************************************************
#undef    OMXCOMPONENT
#ifdef __SYMBIAN32__
	#define   OMXCOMPONENT "ArmIVProc_process"
#else
	#define   OMXCOMPONENT GetComponentName()
#endif


//Static instance 
ReferenceCount ArmIVProc_process::m_Count;

ArmIVProc_process::ArmIVProc_process()
//*************************************************************************************************************
{ 
	IN0("\n");
	m_Count.Ref();
	if (m_Count==1)
	{
		SetComponentName(m_UserName);
	}
	else
	{
#ifdef __SYMBIAN32__
		//Symbian doesn't support snprint use sprintf instead
		sprintf(m_UserName, "ArmIVProc%d", (unsigned int)m_Count);
#else
		snprintf(m_UserName, sizeof(m_UserName), "ArmIVProc%d", (unsigned int)m_Count);
#endif
		SetComponentName(m_UserName);
	}
	MSG1("Construction of ArmIVProc_process (user name %s)\n", m_pName);
	m_fn_UserOpen          = NULL;
	m_fn_UserClose         = NULL;
	m_fn_UserSetConfig     = armivproc_UserSetConfig;
	m_pEngine= new ArmIVProc_Engine;

	m_OmxRotation=0; //Default value
	m_Mirroring=eMirror_None;
	AIQ_InitPerfTrace(PERF_LOG_FILE);

	OUT0("\n");
}

ArmIVProc_process::~ArmIVProc_process()
//*************************************************************************************************************
{ 
	IN0("\n");
	m_Count.UnRef();
	if (m_pEngine)
		delete m_pEngine;
	AIQ_DeInitPerfTrace();

	OUT0("\n");
}

int ArmIVProc_process::UserOpen()
//*************************************************************************************************************
{ // Called by the framwork when component is ready to 'open' transition from idle ->execute
	IN0("\n");
	int res=S_OK;
	_tConvertionConfig Config;
	GetConvertionConfig(Config, false);
	int status= ConfigureAlgo(Config, eRotation_NotDefined); //for making allocation
	MSG2("%s UserOpen::ConfigureAlgo return %d\n", GetComponentName(), status);
	if (status != S_OK)
	{
	}
	OUTR("", res);
	return(res);
}

int ArmIVProc_process::GetConvertionConfig (_tConvertionConfig &Config, bool bDequeue)
//*************************************************************************************************************
{ //This method is called for preparing default ConvertionConfig struct
	IN0("\n");
	int res=InitProcessingInfo(Config, bDequeue, eInputPort_Img, eOutputPort_Img, -1);
	OUTR("", res);
	return(res);
}

void ArmIVProc_process::sendCommand(OMX_COMMANDTYPE cmd, t_uword param)
//*************************************************************************************************************
{ // fsm.component.component.type interface sendcommand method sendCommand
	IN0("\n");
	if ( (cmd == OMX_CommandStateSet) && (param== OMX_StateExecuting) )
	{ // Make real intialisation
	}
	Component::sendCommand(cmd, param);
	OUT0("\n");
}


void ArmIVProc_process::process()
//*************************************************************************************************************
{ // Process IMG to Metadata
	IN0("\n");
	const int InputIndex  = eInputPort_Img;
	const int OutputIndex = eOutputPort_Img;

	//Check that there are at least on input and one ouptut buffer
	int inBufCount  = GetPort(InputIndex) ->queuedBufferCount();
	int outBufCount = GetPort(OutputIndex)->queuedBufferCount();
	MSG3("%s process inBufCount %d, outBufCount %d\n", GetComponentName(), inBufCount, outBufCount);
	if ((inBufCount==0)  || (outBufCount==0))
	{
		MSG0("Nothing to process\n");
		OUT0("\n");
		return;
	}
	_tConvertionConfig Config;
	int res;

	int rotation;
	eEnumRotation rot;
	GetRotation(Get_OmxRotation(), rot);
	rotation=rot;
	// Input and output buffers are available: can do the processing
	MSG1(" %s Fill convertion config\n", GetComponentName());
	res=InitProcessingInfo(Config, true, InputIndex, OutputIndex, -1);

	char *ptr;
	if ( (m_pOpenMax_Component!=NULL) && (((ArmIVProc*)m_pOpenMax_Component)->GetAutoRotateEnable()==true) && (GetExtraData(Config, OMX_SYMBIAN_CameraExtraDataCaptureParameters, ptr )==S_OK))
	{ //Manage metadata -> update rotation with orientation scene
		OMX_SYMBIAN_CAPTUREPARAMETERSTYPE *pData=(OMX_SYMBIAN_CAPTUREPARAMETERSTYPE *)ptr;
		eEnumRotation orientation;
		GetRotationFromSceneOrientation(pData->eSceneOrientation, orientation);
		rotation +=(int)orientation;
		if (rotation >= 360)
			rotation=0; //round modulo 360
	}

	//Now we can call our processing function
	res=-1;
	_tImageBuffer Outbuffer;
	_tImageBuffer Inputbuffer;

	AIQ_StartPerfCounter(0);
	if (m_pEngine)
	{
		AIQ_StartPerfCounter(1);
		res=ConfigureAlgo(Config, (eEnumRotation)rotation);
		if (res !=S_OK)
		{ // Wrong config, disable the output port
			proxy.eventHandler(OMX_EventError, res, 0); //Send an error message
			// MSG0("Wrong configuration, disable port 1 (output)\n");
			ReportError(res, "ConfigureAlgo fails(%d-%s)\n", res, GetStringForArmIVProcError(res));

			MSG0("Wrong configuration, Return buffer and send error\n");
			//Return all pending buffers
			Config.Input .pBufferHdr->nFilledLen = 0; // reset processed buffer
			GetPort(InputIndex) ->returnBuffer(Config.Input .pBufferHdr);
			
			Config.Output .pBufferHdr->nFilledLen = 0; // reset processed buffer
			GetPort(OutputIndex) ->returnBuffer(Config.Output .pBufferHdr);
			
			//portInformation *pPortInfo=GetPortInfo(OutputIndex);
			//ToOMXComponent.setConfig(OMX_CommandPortDisable, (void *)pPortInfo->omxPortIndex); //send a command to disable the port
			OUT0("\n");
			return;
		}
		else
		{
			int status;
			int res=S_OK;
			size_t size_out=Config.Output.pImageInfo->ImageSize;
			MSG4("Set output Image %dx%d, stride=%d, size=%d\n", Config.Output.Width, Config.Output.Height, Config.Output.Stride, size_out);
			MSG1(", format=%d\n", Config.Output.Format);
			Inputbuffer.AttachBuffer((unsigned char *)Config.Input.pBufferHdr->pBuffer+Config.Input.pBufferHdr->nOffset , Config.Input.pImageInfo->ImageSize);

			enum_ImageFormat fmt=GetFormat((OMX_COLOR_FORMATTYPE)Config.Input.Format);
			if (fmt == eImageFormat_UnknowFormat)
			{ //To avoid coverity pb!
			}
			res |= status=Inputbuffer.SetFormat(fmt);
			if (status!=S_OK)
			{
				ReportError(res, "Setting input format(%d) fails (error=%d)\n", fmt, status);
				WOM_ASSERT(status==S_OK);

			}
			res |= status=Inputbuffer.SetImageSize(Config.Input.Width, Config.Input.Height, Config.Input.pImageInfo->StrideInPixel);
			if (status!=S_OK)
			{
				ReportError(res, "Setting input size fails (error=%d) (%dx%d stride= %d )\n", status, Config.Input.Width, Config.Input.Height, Config.Input.pImageInfo->StrideInPixel);
				WOM_ASSERT(status==S_OK);

			}
			Outbuffer  .AttachBuffer((unsigned char *)Config.Output.pBufferHdr->pBuffer+Config.Output.pBufferHdr->nOffset, size_out);

			fmt=GetFormat((OMX_COLOR_FORMATTYPE)Config.Output.Format);
			if (fmt == eImageFormat_UnknowFormat)
			{ //To avoid coverity pb!
			}

			res |= status=Outbuffer.SetFormat(fmt);
			if (status!=S_OK)
			{
				ReportError(res, "Setting Output format(%d) fails (error=%d)\n", Config.Output.pImageInfo->pFormatDescription->ColorFormat, status);
				WOM_ASSERT(status==S_OK);

			}
			res |= status=Outbuffer.SetImageSize(Config.Output.Width, Config.Output.Height, Config.Output.pImageInfo->StrideInPixel);
			if (status!=S_OK)
			{
				ReportError(res, "Setting Output size fails (error=%d) (%dx%d stride= %d)\n", status, Config.Output.Width, Config.Output.Height, Config.Output.pImageInfo->StrideInPixel);
				WOM_ASSERT(status==S_OK);
			}

			AIQ_StopPerfCounter(1, "Armivproc:config & alloc");

			AIQ_StartPerfCounter(2);
			MSG0("Calling  ProcessImage\n");
			res= m_pEngine->ProcessImage(Inputbuffer, Outbuffer);
			MSG1(" ProcessImage, return %d\n", res);
			AIQ_StopPerfCounter(2, "Armivproc:Engine processing");

		}
	}
	AIQ_StopPerfCounter(0, "Armivproc:global processing");

	if (res!=S_OK)
	{ // should implement error handling
		MSG2("******************************ERROR(%s:%d) in ProcessImage \n", GetComponentName(), res);
		ReportError(res, "Process message fails\n", res);
		proxy.eventHandler(OMX_EventError, res, 0);
	}

	if( (Config.Input.pBufferHdr->nFlags & WOM_BUFFERFLAG_CALLBACK_MASK) !=0)
	{ //Input buffer is marked with some flags, recopy of the flag is needed
		MSG2("Armivproc event %x on port %d\n", (int)Config.Input.pBufferHdr->nFlags, OutputIndex);
		portInformation *pPortInfo;
		//pPortInfo=GetPortInfo(InputIndex);
		//if (pPortInfo)
		//	proxy.eventHandler(OMX_EventBufferFlag, pPortInfo->omxPortIndex, (Config.Input.pBufferHdr->nFlags & WOM_BUFFERFLAG_CALLBACK_MASK));
		pPortInfo=GetPortInfo(OutputIndex);
		if (pPortInfo)
			proxy.eventHandler(OMX_EventBufferFlag, pPortInfo->omxPortIndex, (Config.Input.pBufferHdr->nFlags & WOM_BUFFERFLAG_CALLBACK_MASK) );
	}
	else
	{
	}
	
	//release processed buffer
	Config.Output.pBufferHdr->nFilledLen = Outbuffer.GetSize();             //Update output size
	Config.Output.pBufferHdr->nFlags     = Config.Input.pBufferHdr->nFlags; //Recopy buffer's flags
	Config.Output.pBufferHdr->nOffset    = 0;

	MSG0("Copy ExtraData\n");
	res= CopyExtraData(Config);
	if (res !=S_OK)
	{
		WOM_ASSERT(res==S_OK);
		ReportError(-1, "%s copy extradata failed %d", GetComponentName(), res);
	}

	Config.Input .pBufferHdr->nFilledLen = 0; // reset processed buffer

	GetPort(InputIndex) ->returnBuffer(Config.Input .pBufferHdr);
	GetPort(OutputIndex)->returnBuffer(Config.Output.pBufferHdr);
	// Must add metadata release
	OUT0("\n");
}

int ArmIVProc_process::Set_OmxRotation(int rot)
//*************************************************************************************************************
{ // Set the rotation value and reconfigure the algo
	IN0("\n");
	int res=S_OK;
	m_OmxRotation=rot;
	OUTR("", res);
	return(res);
}

int ArmIVProc_process::Set_OmxMirroring(OMX_MIRRORTYPE mirror)
//*************************************************************************************************************
{ // Set the mirroring value
	IN0("\n");
	int res=S_OK;
	m_Mirroring=eMirror_None;
	const _tCorrespondanceTable *ptr=GetCorrespondanceTable(mirror, MirrorTable, SIZEOF_ARRAY(MirrorTable));
	if (ptr!=NULL)
	{
		m_Mirroring=(eEnumMirror)ptr->InternalValue;
	}
	else
	{
		res= eError_WrongMirroringValue;
	}
	OUTR("", res);
	return(res);
}

int ArmIVProc_process::ConfigureAlgo(const _tConvertionConfig &Config, eEnumRotation Rotation)
//*************************************************************************************************************
{ // Set the rotation value and reconfigure the algo
	IN0("\n");
	int res=S_OK;

	_sImageFormat InputConfig;
	_sImageFormat OutputConfig;

	if(    (Config.Input.pPort ==NULL) || (((Port *)Config.Input.pPort )->isEnabled()==false) 
		|| (Config.Output.pPort==NULL) || (((Port *)Config.Output.pPort)->isEnabled()==false))
	{ //Do nothing more
		m_pEngine->Reset();
		OUTR("", res);
		return(res);
	}
	
	//eEnumRotation Rotation;
	//if ( GetRotation(Get_OmxRotation(), Rotation)!=S_OK)
	//{
	//	res=eError_WrongRotation;
	//	CHECK_ERROR(res);
	//	OUTR("", res);
	//	return(res);
	//}
	//DBG_MSG("Rotation =%d", Rotation);


	//InputConfig.
	InputConfig.SizeX        = Config.Input.Width;
	InputConfig.SizeY        = Config.Input.Height;
	InputConfig.HeaderSize   = 0;
	InputConfig.Format       = GetFormat((OMX_COLOR_FORMATTYPE)Config.Input.Format);
	InputConfig.pUserPtr     = NULL;
	InputConfig.pImageStream = NULL;
	CHECK_ERROR(InputConfig.Format== eImageFormat_UnknowFormat);
	if (InputConfig.Format== eImageFormat_UnknowFormat)
	{
		res= eError_WrongInputFormat;
	}

	//OutputConfig.
	OutputConfig.SizeX        = Config.Output.Width;
	OutputConfig.SizeY        = Config.Output.Height;
	OutputConfig.HeaderSize   = 0;
	OutputConfig.Format       = GetFormat((OMX_COLOR_FORMATTYPE)Config.Output.Format);
	OutputConfig.pUserPtr     = NULL;
	OutputConfig.pImageStream = NULL;
	CHECK_ERROR(OutputConfig.Format== eImageFormat_UnknowFormat);
	if (OutputConfig.Format== eImageFormat_UnknowFormat)
	{
		res= eError_WrongOutputFormat;
	}

	if (res==S_OK)
		res=m_pEngine->Configure(InputConfig, OutputConfig, Rotation, m_Mirroring);

	OUTR("", res);
	return(res);
}

