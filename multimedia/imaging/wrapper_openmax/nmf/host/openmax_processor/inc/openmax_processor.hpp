/*****************************************************************************
* Copyright (C) ST-Ericsson SA 2010. All rights reserved.
* This code is ST-Ericsson proprietary and confidential.
* Any use of the code for whatever purpose is subject to
* specific written permission of ST-Ericsson SA.
*****************************************************************************/
#ifndef __OPENMAX_PROCESSOR_HPP__
#define __OPENMAX_PROCESSOR_HPP__
#include <stdio.h>
#include <OMX_Types.h>
#include <OMX_Component.h>
#include <Component.h> // because we NMF-extend fsm.component.component
#include "wrapper_openmax_tools.h"
#include "openmax_processor.nmf"


typedef struct portInformation
//*******************************************************************************
{ // Defined for workaround limitation of classes Component that doesn't support derivation of Port
	OMX_BUFFERHEADERTYPE **BufferHeaderList;
	int fifoSize;
	int width;
	int height;
	OMX_COLOR_FORMATTYPE colorFormat;
	int         stride;
	int         omxPortIndex;
	size_t      nbAllocatedHeader;
	OMX_DIRTYPE direction; //Port direction Input or Output
	int         bufferSupplier;

	//Methods
	void Default(); //Make default initialiation
	int  AllocateBufferHeader();
	void DeleteBufferHeader();
} portInformation;

/**
@brief Nmf part of the component
Receive commands from proxy side 
*/
class OpenMax_Component;
class openmax_processor: public Component, public openmax_processorTemplate
//*******************************************************************************
{ // Host part of the implementation
public:
	IMPORT_C  openmax_processor(const char *name="openmax_processor", OpenMax_Component* OMXowner=NULL);
	virtual WRAPPER_OPENMAX_API  ~openmax_processor();

	int WRAPPER_OPENMAX_API InitProcessingInfo(_tConvertionConfig &Config, bool bDequeue, const int InputIndex, const int OutputIndex, const int MetadataIndex=-1); /// Create the processing structur for given port

	virtual WRAPPER_OPENMAX_API void start();
	virtual WRAPPER_OPENMAX_API void stop();
	//Constructor interface from component.type
	virtual WRAPPER_OPENMAX_API void fsmInit(fsmInit_t initFsm); 

	virtual WRAPPER_OPENMAX_API void setTunnelStatus(t_sint16 portIdx, t_uint16 isTunneled);
	// virtual void functions from Component.h
	virtual WRAPPER_OPENMAX_API void reset() ;
	virtual void disablePortIndication(t_uint32 /*portIdx*/) {};
	virtual void enablePortIndication (t_uint32 /*portIdx*/) {};
	virtual void flushPortIndication  (t_uint32 /*portIdx*/) {};
	virtual WRAPPER_OPENMAX_API void process();

	// fsm.component.component.type interface sendcommand method sendCommand
	virtual WRAPPER_OPENMAX_API void sendCommand(OMX_COMMANDTYPE cmd, t_uword param);
	// fsm.component.component.type interface posteven method processEvent
	virtual WRAPPER_OPENMAX_API void processEvent(void);

	virtual WRAPPER_OPENMAX_API void emptyThisBuffer(OMX_BUFFERHEADERTYPE_p buffer, t_uint8);
	virtual WRAPPER_OPENMAX_API void fillThisBuffer(OMX_BUFFERHEADERTYPE_p buffer, t_uint8);

	// Extended interface
	virtual WRAPPER_OPENMAX_API void setConfig(t_sint32 index, void* opaque_ptr);
	virtual WRAPPER_OPENMAX_API void setParam(t_sint32 portIndex, t_sint32 fifoSize, t_sint32 direction, t_sint32 buffSupplierType, t_sint32 correspondingPortIndex, t_sint32 width, t_sint32 height, t_sint32 colorFormat, t_sint32 stride);
	virtual WRAPPER_OPENMAX_API void setParameter(t_sint32 nParamIndex, void* pParameterStructure); /// Is it really used
	virtual WRAPPER_OPENMAX_API void setTargetComponent(t_uint32 NbPort, void * component_addr);  /// Is it really used

	WRAPPER_OPENMAX_API const char *GetComponentName() const; /// Return the 'user name' of the derived class
	WRAPPER_OPENMAX_API void        SetComponentName(const char *name);

	WRAPPER_OPENMAX_API size_t GetNbPorts() const; /// Return the total number of ports
	WRAPPER_OPENMAX_API size_t GetNbPorts(int type, int *pStartIndex=NULL) const; /// Return the number of ports for a given family and the start index 
	WRAPPER_OPENMAX_API Port * GetPort(size_t index) const;

	WRAPPER_OPENMAX_API portInformation* GetPortInfo(size_t index) const/**/;

	WRAPPER_OPENMAX_API bool GetUseBufferSharing(void) const {return m_bUseBufferSharing;};


	//Extradata function
	virtual WRAPPER_OPENMAX_API int GetExtraData(const _tConvertionConfig &Config, const int aStructType, char *&pStruct);
	virtual WRAPPER_OPENMAX_API int CopyExtraData(_tConvertionConfig &Config);

	enum
	{
		eError_NoError=0,
	};
	WRAPPER_OPENMAX_API OpenMax_Component * GetOpenMax_Component() const { return(m_pOpenMax_Component);}

protected:
	virtual WRAPPER_OPENMAX_API int Init(size_t nb_ports);
	virtual WRAPPER_OPENMAX_API int Deinit(int  unusedflag=0);
	WRAPPER_OPENMAX_API int InitBufferInfo(struct sBufferInfo &Info, const int Index, bool bDequeue);
	virtual WRAPPER_OPENMAX_API int GetConvertionConfig (_tConvertionConfig &Config, bool bDequeue);

	virtual WRAPPER_OPENMAX_API int UserOpen(); /// Called by the framwork when component is ready to 'open' tréansition from idle ->execute
	virtual WRAPPER_OPENMAX_API int UserClose(); /// Called by the framwork when close

	virtual WRAPPER_OPENMAX_API int ReportError(int error, const char *format, ...);

	WRAPPER_OPENMAX_API void SetOpenMax_Component(OpenMax_Component *pCpt) {m_pOpenMax_Component=pCpt;}


protected:
	size_t           m_NbPorts;
	Port            *m_pPort; // Array of Port
	portInformation *m_pPortInfo;
	void            *m_UserPtr;        /// Pointer dedicated to user (usage: everything)
	const char      *m_pName;         /// For storing the name of the component
	int              m_LastError;
	bool             m_bUseBufferSharing; /// Set statically to true is using buffer sharing
	openmax_processorTemplate  &mProcessorTemplate;

	OpenMax_Component *m_pOpenMax_Component; /// Pointer to OpenMax_Component

	int (*m_fn_UserProcessBuffer) (const _tConvertionConfig &Config);
	int (*m_fn_UserClose)         (void **pHdl);
	int (*m_fn_UserOpen )         (const _tConvertionConfig &Config, void **pHdl);
	int (*m_fn_UserSetConfig)     (const _tConvertionConfig &Config, int index, void *ptr);
	int (*m_fn_UserSetTuning)     (const _tConvertionConfig &Config, int index, void *ptr);
};


#endif //__OPENMAX_PROCESSOR_HPP__
