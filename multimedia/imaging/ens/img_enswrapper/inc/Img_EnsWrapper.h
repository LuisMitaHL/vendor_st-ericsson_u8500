/******************************************************************************
* Copyright (C) ST-Ericsson SA 2010. All rights reserved.
* This code is ST-Ericsson proprietary and confidential.
* Any use of the code for whatever purpose is subject to
* specific written permission of ST-Ericsson SA.
* \file
* \brief   This wrapper will ensure run-to-completion execution model OSI
* OMX components rely on. This wrapper takes care of creating threads requiered in
* Think and Linux cases to manage NMF MPC to HOST calls. For now a very simple
* implementation has been made, possible improvements are:
*   - use statically allocated message queue instead of list avoiding memory allocations
*   - identifiy threads posting callbacks to let them execute those callbacks and not
*   have the client call back be executed by a thread which has not posted it
*****************************************************************************/
#ifndef _IMG_ENSWRAPPER_H_
#define _IMG_ENSWRAPPER_H_

#include "Img_EnsWrapper_Shared.h"
#include "BSD_list.h"
#include "osi_toolbox_lib.h"

//Omx definitions
#include <omxil/OMX_Component.h>
// ENS
#include "ImgEns_Component.h"

class Img_EnsWrapper;
class ste_Semaphore;

//Uncomment for new implementation
#define NEW_CMD_RECEIVED_TO_WAIT

struct OmxFactoryDescriptor
{/** Stucture to register a given component along with its OMX std roles and the constructor setting up an ENS component */
	const char *name;   /** The component name */
	OMX_ERRORTYPE (*fnFactory)(OMX_COMPONENTTYPE &);/** The component constructor */
	const char **roles; /** The component roles */
};

// this function is used to retreive
extern "C" int Omx_RegistredFactories(const OmxFactoryDescriptor *pOmxFactoryArray);


enum Enum_AsicID
{
	eAsicID_Invalid  = -1,
	eAsicID_Db8500ed =  0,
	eAsicID_Db8500v1 =  1,
	eAsicID_Db8500v2 =  2,
} ;

#define GLOBAL_IMGWRAPPER_VERSION 12

extern int IMG_ENSWRAPPER_API Global_ImgWrapperVersion;

extern const OMX_CALLBACKTYPE g_OmxCallbacks_Wrapper; /// Global callbacks dedicated to wrapper

/********************************************************************************
* List element to store active components
********************************************************************************/
struct omx_cmp_list_elem_t
{
	LIST_ENTRY(omx_cmp_list_elem_t) list_entry;
	OMX_COMPONENTTYPE * omx_component;
};


typedef LIST_HEAD(omx_cmp_list_head_t, omx_cmp_list_elem_t) omx_cmp_list_head_t;

/********************************************************************************
* List element to store active components
********************************************************************************/
struct omx_cmp_trace_list_elem_t
{
	LIST_ENTRY(omx_cmp_trace_list_elem_t) list_entry;
	/* The name of component for which OSTtrace is to be activated */
	char name[OMX_MAX_STRINGNAME_SIZE];
	/* The Group bitmask for this component */
	OMX_U16 nTraceEnable;
};

typedef LIST_HEAD(omx_cmp_trace_list_head_t, omx_cmp_trace_list_elem_t) omx_cmp_trace_list_head_t;

enum
{
	eThread_None=0, // When none get the mutex
};

enum enumSpecificUids
{
	eUid_None  = 0,
	eUid_Specific          = 0xffffff00,
	eUid_ImgCallbackThread = eUid_Specific, // For Callback thread
	eUid_ComponentManager, 
	eUid_User01,
	eUid_User02,
	eUid_User03,
	eUid_User04,
	eUid_User05,
	eUid_User06,
	eUid_User07,
	eUid_User08,
	eUid_User09,
	eUid_LastEnum,
};

/********************************************************************************
* Img_EnsWrapper related definitions
********************************************************************************/

class Img_EnsWrapper_Rme;
class Img_EnsWrapper_Construct;
class ImgEns_IOmx;

class IMG_ENSWRAPPER_API ExtendedMutex
//********************************************************************************
{
public:
	ExtendedMutex(const char *name="unamed");
	~ExtendedMutex();
	enum enumLockType
	{
		eLock_Master,     // Means be the first to take the mutex
		eLock_Child,      // Means that the component can be allready lock
		eLock_Singleton,  // Means that only one user for the mutex
	};
	int Lock  (enumLockType type, const int uid, const char *from="???");
	int Unlock(const int uid, const char *from="???");
protected:
	ste_Mutex    m_Mutex;
	ste_Mutex    m_InternalMutex;
	t_ThreadId   m_Lock_ThreadId;
	size_t       m_Lock_Depth;
	int          m_Lock_Owner;
	const char * m_Lock_Comment;

protected:
	const char *m_Name; //Name of mutex (for debug purpose)
};

class ComponentManager;
class ste_Semaphore;

enum enumTraceLevel
{
	eTrace_Debug,
	eTrace_Omx,
	eTrace_Ost,
	eTrace_LastEnum,
};

class IMG_ENSWRAPPER_API ComponentManager
//********************************************************************************
{
public:
	ComponentManager();
	~ComponentManager();
	int GlobalLock  (ExtendedMutex::enumLockType type, const int pHandle, const char *Comment="");
	int GlobalUnlock(const int pHandle, const char *Comment="");

	unsigned int GetTraceLevel(int what) const;
	unsigned int SetTraceLevel(int what, unsigned int newlevel);
protected:
	unsigned int *GetTraceLevelVariable(int what) const;

	ExtendedMutex m_GlobalMutex;
	unsigned int TraceLevel[eTrace_LastEnum];

	enum 
	{
		eError_None,
		eError_RegisterNullHandle =-1,
	};
// A voir
public:
	void PrintComponentList();                                  ///Print the list of active components
	Img_EnsWrapper * GetWrapperForID(unsigned long id) const;   /// Return the wrapper instance corresponding to the requested id, Null otherwise
	bool IsComponentActive(OMX_COMPONENTTYPE *) const;          /// Return true in case OMX_COMPONENTTYPE is still active
	int RemoveHandleFromActiveList(OMX_COMPONENTTYPE *);        /// Remove from list of active components the provided handle
	int RegisterHandle(OMX_COMPONENTTYPE *);                    /// Add to list of active components the provided handle

	unsigned long CreateUniqueID() { ++mUniqueIDCounter; return(mUniqueIDCounter); }

	static ImgEns_Component* getEnsComponent(OMX_COMPONENTTYPE *hComp);

	int Init_OMX_COMPONENTTYPE(OMX_COMPONENTTYPE *m_pOmxComponent, void *pComponentPrivate) const ;

protected:
	omx_cmp_list_head_t       omx_cmp_list;  /** The list of active OMX components */
	volatile unsigned long    mUniqueIDCounter; /** The counter we'll use to assign a unique id to each component */
	omx_cmp_trace_list_head_t omx_cmp_trace_list;/** The list of OMX component trace sepc */
};

IMG_ENSWRAPPER_API ComponentManager & GetComponentManager();
IMG_ENSWRAPPER_API ImgEns_Component*  getImgEns_Component(OMX_HANDLETYPE hComp);


#if 0
class Obsolete
{
public:
	void memStatus(void) const ; /// Dump the process mem status in a file 
	void addOSTtraceFilteringSpec(const char *componentName, OMX_U16 traceEnableMask); /// Add to OST activation list given component with group 

};
#endif

class IMG_ENSWRAPPER_API Img_EnsWrapper
//********************************************************************************
{
public:
	/** Constructor for wrapper using provided semaphore as component lock */
	Img_EnsWrapper(ImgEns_IOmx *m_pEnsComponent, OMX_COMPONENTTYPE * m_pOmxComponent);
	~Img_EnsWrapper();
	
	ImgEns_IOmx            * getImgEns_Component() const { return m_pEnsComponent; };
	OMX_COMPONENTTYPE      * getOMX_Component()    const { return m_pOmxComponent; };

	friend class Global_ImgWrapper;
	friend class ComponentManager;

	// ComponentManager &GetComponentManager() const { return(m_ComponentManager); }
protected:
	ComponentManager &m_ComponentManager;
#if 1
	unsigned int OMX_dump_level;
	unsigned int OMX_trace_level;
	unsigned int OMX_dump_output;
#endif
protected:
	OMX_COMPONENTTYPE * m_pOmxComponent; /** OMX component allocated by ENS core and provided to OMX IL Client */
	ImgEns_IOmx       * m_pEnsComponent; /** OMX IL handle of the wrapped OSI component */

	OMX_CALLBACKTYPE * m_Omx_UserCallbacks; /** OMX IL Client call backs */

	/** Semaphore to hold client thread on SendCommand waiting for ENS response */
#ifndef NEW_CMD_RECEIVED_TO_WAIT
	volatile unsigned int  mNbCmdReceivedToWait;
#endif
	ste_Semaphore          m_Semaphore_WaitingForCmdReceived;
	
	omx_cmp_list_elem_t list_elem; /// The list structure to register this wrapper instance to omx_cmp_list
public:
	/********************************************************************************/
	/**
	* The OMX components interfaces 
	* the OMX IL client will call The wrapper will return the following functions as component entry point
	*/
	static OMX_ERRORTYPE GetComponentVersion(OMX_HANDLETYPE hComponent, OMX_STRING pComponentName, OMX_VERSIONTYPE* pComponentVersion, OMX_VERSIONTYPE* pSpecVersion, OMX_UUIDTYPE* pComponentUUID);
	static OMX_ERRORTYPE SendCommand            ( OMX_HANDLETYPE hComponent, OMX_COMMANDTYPE Cmd, OMX_U32 nParam, OMX_PTR pCmdData);
	static OMX_ERRORTYPE GetParameter           ( OMX_HANDLETYPE hComponent, OMX_INDEXTYPE nIndex, OMX_PTR pComponentParameterStructure);
	static OMX_ERRORTYPE SetParameter           ( OMX_HANDLETYPE hComponent, OMX_INDEXTYPE nIndex, OMX_PTR pComponentParameterStructure);
	static OMX_ERRORTYPE GetConfig              ( OMX_HANDLETYPE hComponent, OMX_INDEXTYPE nIndex, OMX_PTR pComponentConfigStructure);
	static OMX_ERRORTYPE SetConfig              ( OMX_HANDLETYPE hComponent, OMX_INDEXTYPE nIndex, OMX_PTR pComponentConfigStructure);
	static OMX_ERRORTYPE GetExtensionIndex      ( OMX_HANDLETYPE hComponent, OMX_STRING cParameterName, OMX_INDEXTYPE* pIndexType);
	static OMX_ERRORTYPE GetState               ( OMX_HANDLETYPE hComponent, OMX_STATETYPE* pState);
	static OMX_ERRORTYPE ComponentTunnelRequest ( OMX_HANDLETYPE hComponent, OMX_U32 nPort, OMX_HANDLETYPE hTunneledComp, OMX_U32 nTunneledPort, OMX_TUNNELSETUPTYPE* pTunnelSetup);
	static OMX_ERRORTYPE UseBuffer              ( OMX_HANDLETYPE hComponent, OMX_BUFFERHEADERTYPE** ppBufferHdr, OMX_U32 nPortIndex, OMX_PTR pAppPrivate, OMX_U32 nSizeBytes, OMX_U8* pBuffer);
	static OMX_ERRORTYPE AllocateBuffer         ( OMX_HANDLETYPE hComponent, OMX_BUFFERHEADERTYPE** ppBufferHdr, OMX_U32 nPortIndex, OMX_PTR pAppPrivate, OMX_U32 nSizeBytes);
	static OMX_ERRORTYPE FreeBuffer             ( OMX_HANDLETYPE hComponent, OMX_U32 nPortIndex, OMX_BUFFERHEADERTYPE* pBuffer);
	static OMX_ERRORTYPE EmptyThisBuffer        ( OMX_HANDLETYPE hComponent, OMX_BUFFERHEADERTYPE* pBuffer);
	static OMX_ERRORTYPE FillThisBuffer         ( OMX_HANDLETYPE hComponent, OMX_BUFFERHEADERTYPE* pBuffer);
	static OMX_ERRORTYPE SetCallbacks           ( OMX_HANDLETYPE hComponent, OMX_CALLBACKTYPE* pCallbacks, OMX_PTR pAppData);
	static OMX_ERRORTYPE ComponentDeInit        ( OMX_HANDLETYPE hComponent);
	static OMX_ERRORTYPE UseEGLImage            ( OMX_HANDLETYPE hComponent, OMX_BUFFERHEADERTYPE** ppBufferHdr, OMX_U32 nPortIndex, OMX_PTR pAppPrivate, void* eglImage);
	static OMX_ERRORTYPE ComponentRoleEnum      ( OMX_HANDLETYPE hComponent, OMX_U8 *cRole, OMX_U32 nIndex);

	/** The OMX IL client call backs passed to the OSI components */
	static  OMX_ERRORTYPE EventHandler   (OMX_HANDLETYPE hComponent, OMX_PTR pAppData, OMX_EVENTTYPE eEvent, OMX_U32 nData1, OMX_U32 nData2, OMX_PTR pEventData);
	static  OMX_ERRORTYPE EmptyBufferDone(OMX_HANDLETYPE hComponent, OMX_PTR pAppData, OMX_BUFFERHEADERTYPE* pBuffer);
	static  OMX_ERRORTYPE FillBufferDone (OMX_HANDLETYPE hComponent, OMX_PTR pAppData, OMX_BUFFERHEADERTYPE* pBuffer);
};


// Global function that redirect to class ones
enum enum_OmxCmdApi
{
	eOmxCmdApi_Unknow,
	eOmxCmdApi_GetComponentVersion,
	eOmxCmdApi_SendCommand,
	eOmxCmdApi_GetParameter,
	eOmxCmdApi_SetParameter,
	eOmxCmdApi_GetConfig,
	eOmxCmdApi_SetConfig,
	eOmxCmdApi_GetExtensionIndex,
	eOmxCmdApi_GetState,
	eOmxCmdApi_ComponentTunnelRequest,
	eOmxCmdApi_UseBuffer,
	eOmxCmdApi_AllocateBuffer,
	eOmxCmdApi_FreeBuffer,
	eOmxCmdApi_EmptyThisBuffer,
	eOmxCmdApi_FillThisBuffer,
	eOmxCmdApi_SetCallbacks,
	eOmxCmdApi_ComponentDeInit,
	eOmxCmdApi_UseEGLImage,
	eOmxCmdApi_ComponentRoleEnum,
};

#if 0
{
	{eOmxCmdApi_Unknow                , "Unknow"                          }, // 
	{eOmxCmdApi_GetComponentVersion   , "GetComponentVersion"             }, //  GetComponentVersion   ( OMX_HANDLETYPE hComponent, OMX_STRING pComponentName, OMX_VERSIONTYPE* pComponentVersion, OMX_VERSIONTYPE* pSpecVersion, OMX_UUIDTYPE* pComponentUUID);
	{eOmxCmdApi_SendCommand           , "SendCommand"                     }, //  SendCommand           ( OMX_HANDLETYPE hComponent, OMX_COMMANDTYPE Cmd,  OMX_U32 nParam1,  OMX_PTR pCmdData);
	{eOmxCmdApi_GetParameter          , "GetParameter"                    }, //  GetParameter          ( OMX_HANDLETYPE hComponent, OMX_INDEXTYPE nIndex, OMX_PTR pComponentParameterStructure);
	{eOmxCmdApi_SetParameter          , "SetParameter"                    }, //  SetParameter          ( OMX_HANDLETYPE hComponent, OMX_INDEXTYPE nIndex,  OMX_PTR pComponentParameterStructure);
	{eOmxCmdApi_GetConfig             , "GetConfig"                       }, //  GetConfig             ( OMX_HANDLETYPE hComponent, OMX_INDEXTYPE nIndex, OMX_PTR pComponentConfigStructure);
	{eOmxCmdApi_SetConfig             , "SetConfig"                       }, //  SetConfig             ( OMX_HANDLETYPE hComponent, OMX_INDEXTYPE nIndex,  OMX_PTR pComponentConfigStructure);
	{eOmxCmdApi_GetExtensionIndex     , "GetExtensionIndex"               }, //  GetExtensionIndex     ( OMX_HANDLETYPE hComponent, OMX_STRING cParameterName, OMX_INDEXTYPE* pIndexType);
	{eOmxCmdApi_GetState              , "GetState"                        }, //  GetState              ( OMX_HANDLETYPE hComponent, OMX_STATETYPE* pState);
	{eOmxCmdApi_ComponentTunnelRequest, "ComponentTunnelRequest"          }, //  ComponentTunnelRequest( OMX_HANDLETYPE hComponent, OMX_U32 nPort,  OMX_HANDLETYPE hTunneledComp,  OMX_U32 nTunneledPort,  OMX_TUNNELSETUPTYPE* pTunnelSetup);
	{eOmxCmdApi_UseBuffer             , "UseBuffer"                       }, //  UseBuffer             ( OMX_HANDLETYPE hComponent, OMX_BUFFERHEADERTYPE** ppBufferHdr, OMX_U32 nPortIndex, OMX_PTR pAppPrivate, OMX_U32 nSizeBytes, OMX_U8* pBuffer);
	{eOmxCmdApi_AllocateBuffer        , "AllocateBuffer"                  }, //  AllocateBuffer        ( OMX_HANDLETYPE hComponent, OMX_BUFFERHEADERTYPE** ppBuffer, OMX_U32 nPortIndex, OMX_PTR pAppPrivate, OMX_U32 nSizeBytes);
	{eOmxCmdApi_FreeBuffer            , "FreeBuffer"                      }, //  FreeBuffer            ( OMX_HANDLETYPE hComponent, OMX_U32 nPortIndex,  OMX_BUFFERHEADERTYPE* pBuffer);
	{eOmxCmdApi_EmptyThisBuffer       , "EmptyThisBuffer"                 }, //  EmptyThisBuffer       ( OMX_HANDLETYPE hComponent, OMX_BUFFERHEADERTYPE* pBuffer);
	{eOmxCmdApi_FillThisBuffer        , "FillThisBuffer"                  }, //  FillThisBuffer        ( OMX_HANDLETYPE hComponent, OMX_BUFFERHEADERTYPE* pBuffer);
	{eOmxCmdApi_SetCallbacks          , "SetCallbacks"                    }, //  SetCallbacks          ( OMX_HANDLETYPE hComponent, OMX_CALLBACKTYPE* pCallbacks,  OMX_PTR pAppData);
	{eOmxCmdApi_ComponentDeInit       , "ComponentDeInit"                 }, //  ComponentDeInit       ( OMX_HANDLETYPE hComponent);
	{eOmxCmdApi_UseEGLImage           , "UseEGLImage"                     }, //  UseEGLImage           ( OMX_HANDLETYPE hComponent, OMX_BUFFERHEADERTYPE** ppBufferHdr, OMX_U32 nPortIndex, OMX_PTR pAppPrivate, void* UseEGLImage);
	{eOmxCmdApi_ComponentRoleEnum     , "ComponentRoleEnum"               }, //  ComponentRoleEnum     ( OMX_HANDLETYPE hComponent, OMX_U8 *cRole, OMX_U32 nIndex);
};


// Global function that redirect to class ones
GetComponentVersion   ( OMX_HANDLETYPE hComponent, OMX_STRING pComponentName, OMX_VERSIONTYPE* pComponentVersion, OMX_VERSIONTYPE* pSpecVersion, OMX_UUIDTYPE* pComponentUUID);
SendCommand           ( OMX_HANDLETYPE hComponent, OMX_COMMANDTYPE Cmd,  OMX_U32 nParam1,  OMX_PTR pCmdData);
GetParameter          ( OMX_HANDLETYPE hComponent, OMX_INDEXTYPE nIndex, OMX_PTR pComponentParameterStructure);
SetParameter          ( OMX_HANDLETYPE hComponent, OMX_INDEXTYPE nIndex,  OMX_PTR pComponentParameterStructure);
GetConfig             ( OMX_HANDLETYPE hComponent, OMX_INDEXTYPE nIndex, OMX_PTR pComponentConfigStructure);
SetConfig             ( OMX_HANDLETYPE hComponent, OMX_INDEXTYPE nIndex,  OMX_PTR pComponentConfigStructure);
GetExtensionIndex     ( OMX_HANDLETYPE hComponent, OMX_STRING cParameterName, OMX_INDEXTYPE* pIndexType);
GetState              ( OMX_HANDLETYPE hComponent, OMX_STATETYPE* pState);
ComponentTunnelRequest( OMX_HANDLETYPE hComponent, OMX_U32 nPort,  OMX_HANDLETYPE hTunneledComp,  OMX_U32 nTunneledPort,  OMX_TUNNELSETUPTYPE* pTunnelSetup);
UseBuffer             ( OMX_HANDLETYPE hComponent, OMX_BUFFERHEADERTYPE** ppBufferHdr, OMX_U32 nPortIndex, OMX_PTR pAppPrivate, OMX_U32 nSizeBytes, OMX_U8* pBuffer);
AllocateBuffer        ( OMX_HANDLETYPE hComponent, OMX_BUFFERHEADERTYPE** ppBuffer, OMX_U32 nPortIndex, OMX_PTR pAppPrivate, OMX_U32 nSizeBytes);
FreeBuffer            ( OMX_HANDLETYPE hComponent, OMX_U32 nPortIndex,  OMX_BUFFERHEADERTYPE* pBuffer);
EmptyThisBuffer       ( OMX_HANDLETYPE hComponent, OMX_BUFFERHEADERTYPE* pBuffer);
FillThisBuffer        ( OMX_HANDLETYPE hComponent, OMX_BUFFERHEADERTYPE* pBuffer);
SetCallbacks          ( OMX_HANDLETYPE hComponent, OMX_CALLBACKTYPE* pCallbacks,  OMX_PTR pAppData);
ComponentDeInit       ( OMX_HANDLETYPE hComponent);
UseEGLImage           ( OMX_HANDLETYPE hComponent, OMX_BUFFERHEADERTYPE** ppBufferHdr, OMX_U32 nPortIndex, OMX_PTR pAppPrivate, void* UseEGLImage);
ComponentRoleEnum     ( OMX_HANDLETYPE hComponent, OMX_U8 *cRole, OMX_U32 nIndex);
#endif
#endif /* _IMG_ENSWRAPPER_H_ */
