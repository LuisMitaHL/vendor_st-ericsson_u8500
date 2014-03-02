/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
#ifndef __FAKE_SOURCE_HPP__
#define __FAKE_SOURCE_HPP__

#include "Component.h" // because we NMF-extend fsm.component.component 
#include "OMX_Types.h"
#include "OMX_IVCommon.h"

#include <los/api/los_api.h>

#ifdef NO_DEP_WITH_IFM
#else
#include "IFM_Types.h"
#include "extradata.h"
#endif

#define FAKE_FIFO_SIZE 20

#define FAKE_SOURCE_STRING_SIZE 512

#define FAKE_SOURCE_PATH_PREFIXE_EXTRADATA "extradat_path_detection:"

#define FAKE_SOURCE_STRING_SIZE_XML 64
typedef struct TFakeSource_ExtraData_Detection
{
    int isThere_IFM_BMS_CAPTURE_CONTEXT_TYPE;
    int isThere_IFM_DAMPERS_DATA_TYPE;
    int isThere_OMX_ExtraDataQuantization;
    int isThere_OMX_SYMBIAN_CameraExtraDataCaptureParameters;
    int isThere_OMX_SYMBIAN_CameraExtraDataEXIFAppMarker;
    int isThere_OMX_SYMBIAN_CameraExtraDataFrameDescription;
    int isThere_OMX_SYMBIAN_ExtraDataVideoStabilization;
    int isThere_OMX_SYMBIAN_CameraExtraDataCameraUserSettings;
    int isThere_OMX_ExtraDataCameraProductionFlashTest;
    int isThere_OMX_SYMBIAN_CameraExtraData3AVendorSpecific;
    int isThere_OMX_SYMBIAN_CameraExtraDataFeedback;
    int isThere_OMX_SYMBIAN_CameraExtraDataHistogram;
    int isThere_OMX_SYMBIAN_CameraExtraDataROI;
    int isThere_OMX_STE_ExtraData_CaptureParameters;
    int isThere_OMX_Symbian_CameraExtraDataAFStatus;
}TFakeSource_ExtraData_Detection;

typedef enum eFakeSource_Type{
	eFakeSource_Type_Unknown=0,
	eFakeSource_Type_U8,
	eFakeSource_Type_S8,
	eFakeSource_Type_U16,
	eFakeSource_Type_S16,
	eFakeSource_Type_U32,
	eFakeSource_Type_S32,
}eFakeSource_Type;
#include "McbXML.h"

class fake_source : public Component, public fake_sourceTemplate
{
	public:
		
		virtual void start() ;
		//Constructor interface from component.type
		virtual void fsmInit(fsmInit_t inits);
		virtual void setTunnelStatus(t_sint16 portIdx, t_uint16 isTunneled);
		// virtual void functions from Component.h 
		virtual void reset() ;
		virtual void disablePortIndication(t_uint32 portIdx){}
	    virtual void enablePortIndication(t_uint32 portIdx){}
	    virtual void flushPortIndication(t_uint32 portIdx){}
		virtual void process();		
		// fsm.component.component.type interface sendcommand method sendCommand
		virtual void sendCommand(OMX_COMMANDTYPE cmd, t_uword param);
		// fsm.component.component.type interface posteven method processEvent 
		virtual void processEvent(void){ Component::processEvent() ; }		
		virtual void fillThisBuffer(OMX_BUFFERHEADERTYPE_p buffer);

		virtual void setMyConfig(t_uint32 index, void* opaque_ptr);
		virtual void setParam(t_uint32 portIndex, t_uint32 fifoSize, t_uint32 direction, t_uint32 buffSupplierType, t_uint32 width, t_uint32 height, t_uint32 stride, t_uint32 sliceheight, t_uint32 colorFormat, t_uint32 buffersize, t_uint32 domaine);
		virtual void finish(void);	
		virtual void print_color_format(t_uint32 format);
		virtual char* my_strstr (char * str1, const char * str2 );	

		void add_ExtradatStructure(OMX_BUFFERHEADERTYPE* pBufferH, OMX_EXTRADATATYPE typeED, unsigned int sizeofED, void* bufferED);
		void add_IFM_BMS_CAPTURE_CONTEXT_TYPE(OMX_BUFFERHEADERTYPE* pBufferH);
		void add_IFM_DAMPERS_DATA_TYPE(OMX_BUFFERHEADERTYPE* pBufferH);
		void add_OMX_ExtraDataQuantization(OMX_BUFFERHEADERTYPE* pBufferH);
		void add_OMX_SYMBIAN_ExtraDataVideoStabilization(OMX_BUFFERHEADERTYPE* pBufferH);
		void add_OMX_SYMBIAN_CameraExtraDataCaptureParameters(OMX_BUFFERHEADERTYPE* pBufferH);
		void add_OMX_SYMBIAN_CameraExtraDataEXIFAppMarker(OMX_BUFFERHEADERTYPE* pBufferH);
		void add_OMX_SYMBIAN_CameraExtraDataFrameDescription(OMX_BUFFERHEADERTYPE* pBufferH);
		void add_OMX_SYMBIAN_CameraExtraDataCameraUserSettings(OMX_BUFFERHEADERTYPE* pBufferH);
		void add_OMX_ExtraDataCameraProductionFlashTest(OMX_BUFFERHEADERTYPE* pBufferH);
		void add_OMX_SYMBIAN_CameraExtraData3AVendorSpecific(OMX_BUFFERHEADERTYPE* pBufferH);
		void add_OMX_SYMBIAN_CameraExtraDataFeedback(OMX_BUFFERHEADERTYPE* pBufferH);
		void add_OMX_SYMBIAN_CameraExtraDataHistogram(OMX_BUFFERHEADERTYPE* pBufferH);
		void add_OMX_SYMBIAN_CameraExtraDataROI(OMX_BUFFERHEADERTYPE* pBufferH);
		void add_OMX_STE_ExtraData_CaptureParameters(OMX_BUFFERHEADERTYPE* pBufferH);
		void add_OMX_Symbian_CameraExtraDataAFStatus(OMX_BUFFERHEADERTYPE* pBufferH);
		void add_EndExtraData(OMX_BUFFERHEADERTYPE* pBufferH);

		void Print_ED_Detection();
		void XML_Fill_Array(eFakeSource_Type type, void* pArray, OMX_U32 arrayNbElement, McbXMLElement *ptr_pElement, char* stringForSearchPrefixe);
		void XML_Fill_ROIOBJECTINFOTYPE(OMX_SYMBIAN_ROIOBJECTINFOTYPE* pArray, OMX_U32 arrayNbElement, McbXMLElement *ptr_pElement);
		void XML_Fill_AFROITYPE(OMX_SYMBIAN_AFROITYPE* pArray, OMX_U32 arrayNbElement, McbXMLElement *ptr_pElement);
		void Process_xml(int frameNumber);

		McbXMLElement *pElement_IFM_BMS_CAPTURE_CONTEXT_TYPE; 
		McbXMLElement *pElement_IFM_DAMPERS_DATA_TYPE; 
		McbXMLElement *pElement_OMX_SYMBIAN_CameraExtraDataCaptureParameters; 
		McbXMLElement *pElement_OMX_SYMBIAN_CameraExtraDataFrameDescription; 
		McbXMLElement *pElement_OMX_SYMBIAN_ExtraDataVideoStabilization;
		McbXMLElement *pElement_OMX_SYMBIAN_CameraExtraDataCameraUserSettings;
		McbXMLElement *pElement_OMX_ExtraDataCameraProductionFlashTest;
		McbXMLElement *pElement_OMX_SYMBIAN_CameraExtraData3AVendorSpecific;
		McbXMLElement *pElement_OMX_SYMBIAN_CameraExtraDataFeedback;
		McbXMLElement *pElement_OMX_SYMBIAN_CameraExtraDataHistogram;
		McbXMLElement *pElement_OMX_SYMBIAN_CameraExtraDataROI;
		McbXMLElement *pElement_OMX_STE_ExtraData_CaptureParameters;
		McbXMLElement *pElement_OMX_Symbian_CameraExtraDataAFStatus;
		t_los_memory_handle memory_handle_OMX_Symbian_CameraExtraDataAFStatus;
		unsigned int size_memory_handle_OMX_Symbian_CameraExtraDataAFStatus;
		McbXMLResults pResults;
		McbXMLElement *pElement_root; 
		McbXMLElement *pElement_current;
		McbXMLAttribute *pAttribute_current;
		McbXMLElement *pElement_xml; 
		McbXMLElement *pElement_streaming_param; 
		TFakeSource_ExtraData_Detection ED_Detection;
		int frameNumberExtraData;

private:

		Port mPort[1];
		int fifoSize;
		int Width;
		int Height;
		int SliceHeight;
		int Stride;
		int Format;
		unsigned int BufferSize;
		unsigned int FrameSize;
		unsigned int FrameSizeUpdateDueToStatusLine;
		int Domaine;
		OMX_BUFFERHEADERTYPE mBuff[FAKE_FIFO_SIZE];
		int bufferCount;
		void myProcessBuffer(OMX_BUFFERHEADERTYPE* pBufferH);

		OMX_U8 fileName_data[FAKE_SOURCE_STRING_SIZE];
		t_los_file * fileHandle_data;
		t_los_memory_handle whole_file_data_handle;
		OMX_U32 fileSize_data;
		OMX_U32 fileSizeRead_data;
		t_los_memory_handle memory_handle_extradata;
		OMX_U32 fileSize_extradata;
		char *extraDataPointer;

		char fileName_extradata[FAKE_SOURCE_STRING_SIZE];
		t_los_file * fileHandle_extradata;

		OMX_BOOL bFunction_process_already_called;
		OMX_BOOL bEOS;
		OMX_BOOL bContinuous;
		OMX_BOOL bFrameLimited;
		OMX_U32 nFrameLimit;
		OMX_BOOL bCapturing;
		OMX_BOOL bAutoPause;
		OMX_U32 xEncodeFramerate;
		OMX_S64 beginTimeMicroS;
		OMX_S64 currentTimeMicroS;
		OMX_S64 previsousTimeStampMS;
		t_uint32 currentTimeDiffMS;
		t_uint32 expectedInterFrameInMs;
		t_sint32 sleepInMs;

		OMX_U32 cumulatedSizeWrittenExtraData;
		OMX_U32 currentSizeWrittenExtraData;
		OMX_OTHER_EXTRADATATYPE extradataHeader;

		OMX_U32 s_ExtraDataQuantization[4];
		OMX_SYMBIAN_CAPTUREPARAMETERSTYPE s_OMX_SYMBIAN_CAPTUREPARAMETERSTYPE;
		OMX_SYMBIAN_FRAMEDESCRIPTIONTYPE s_OMX_SYMBIAN_FRAMEDESCRIPTIONTYPE;
		OMX_SYMBIAN_DIGITALVIDEOSTABTYPE s_OMX_SYMBIAN_DIGITALVIDEOSTABTYPE;
		OMX_SYMBIAN_CAMERAUSERSETTINGSTYPE s_OMX_SYMBIAN_CAMERAUSERSETTINGSTYPE;
#ifdef NO_DEP_WITH_IFM
#else		
		IFM_BMS_CAPTURE_CONTEXT s_IFM_BMS_CAPTURE_CONTEXT;
		IFM_DAMPERS_DATA s_IFM_DAMPERS_DATA;
		OMX_STE_PRODUCTIONTESTTYPE s_OMX_STE_PRODUCTIONTESTTYPE;
		OMX_STE_CAPTUREPARAMETERSTYPE s_OMX_STE_CAPTUREPARAMETERSTYPE;
#endif
		//OMX_SYMBIAN_??? s_OMX_SYMBIAN_???; //CameraExtraData3AVendorSpecific
		OMX_SYMBIAN_CONFIG_FEEDBACKTYPE s_OMX_SYMBIAN_CONFIG_FEEDBACKTYPE;
		OMX_SYMBIAN_CONFIG_RGBHISTOGRAM s_OMX_SYMBIAN_CONFIG_RGBHISTOGRAM;
		OMX_SYMBIAN_ROITYPE s_OMX_SYMBIAN_ROITYPE;
		OMX_SYMBIAN_EXTRADATA_EXTFOCUSSTATUSTYPE s_OMX_SYMBIAN_EXTRADATA_EXTFOCUSSTATUSTYPE;
		OMX_SYMBIAN_EXTRADATA_EXTFOCUSSTATUSTYPE *p_OMX_SYMBIAN_EXTRADATA_EXTFOCUSSTATUSTYPE;
		char* pWrittingExtraData;
};

#endif //__FAKE_SOURCE_HPP__
