/*
* Copyright (C) ST-Ericsson SA 2010. All rights reserved.
* This code is ST-Ericsson proprietary and confidential.
* Any use of the code for whatever purpose is subject to
* specific written permission of ST-Ericsson SA.
*/
#ifndef _IMG_OMXLOADER_H_
#define _IMG_OMXLOADER_H_

#include <omxil/OMX_Core.h>

enum
{
	eFindBy_ModuleName   = 0,
	eFindBy_ModuleHandle,
	eFindBy_OmxHandle,
};

class ImgModule;
int CmpImgModule(void *criteria, ImgModule *pModule, int what);

class IMG_ENSWRAPPER_API ImgModule
//******************************************************************************************
{/* Structure of list element used as modules registry's */
public:
	ImgModule();
	~ImgModule();
	int FindComponent(const char *name, const _tImgOmxFactoryDescription **ppFactory=NULL) const;

	const _tImgOmxFactoryDescription * GetComponent(size_t index) const ;
	void  SetComponents(const _tImgOmxFactoryDescription *pDesc, size_t nbfactory) { Components=pDesc; NbrFactory=nbfactory;}

	ImgModule & operator =(const ImgModule &Module);

	size_t GetNbrFactory() const {return(NbrFactory); }

	_tModuleHandle GetModuleHandle() const {return(ModuleHandle); }
	void SetModuleHandle(_tModuleHandle newhandle) { ModuleHandle=newhandle; }
	int CloseModule();
	
	const char *GetModulePath() const {return(ModulePath.GetBuffer()); }
	void  SetModulePath(const char *path)  {ModulePath=path; }

	size_t ActiveComponentsCount() const;
	size_t ActiveComponentsUpcount()  { return(++NbrActiveComponent); }
	size_t ActiveComponentsDowncount();

	friend int CmpImgModule(void *criteria, ImgModule *pModule, int what);;
	//protected:
protected:
	const _tImgOmxFactoryDescription * Components;        /// The components definition for this element */
	size_t                             NbrFactory;        /// Number of registred factory  is this module
	size_t                             NbrActiveComponent;/// Number of active component is this module
	_tModuleHandle                     ModuleHandle;      /// The shared library handle for this element
	StaticString<MAX_PATH>             ModulePath;        /// The shared library name for this element
};

typedef struct 
{ // Store info about active component
	OMX_HANDLETYPE  OmxHandle;
	_tModuleHandle  ModuleHandle; //The library module hanlde
	size_t          IndexFactory; // Factory index for given library module
} _tActiveOmxComponent;


class IMG_ENSWRAPPER_API STE_Img_Loader
//******************************************************************************************
{
public:
	STE_Img_Loader(const char *fnEntryName); /// fnEntryName is the function name that will be search in the modules (entry points)
	~STE_Img_Loader();

	size_t FindCompatibleComponents(const char *dir); /// Register components from default dir, return the number of loaded module
	int    UnloadAll();/// Free the allocated resources

	size_t     GetModuleCount() const { return(m_ArrayModule.GetCount()); }
	ImgModule *GetModule(const size_t index) const { return(m_ArrayModule.Get(index)); }
	enum
	{
		eError_LoaderError           = -100,
		eError_CannotUnregister,
		eError_InvalidActiveOmxHandle,
		eError_InvalidActiveModuleHandle,
		eError_NullOmxHandle,
		eError_CannotRegisterOmxHandle,
		eError_ModuleAlreadyOpen,
		eError_ModuleNotCompatible,
		eError_InsufficientMemory,
		eError_DirectoryNotFound,
	};
	const char * GetFnEntryName() const { return(m_fnEntryName); }
	const char * GetDirectory()   const;
// protected:
	ImgModule   * GetModuleFromName        (const char *cComponentName , size_t * position= NULL); /// Return the first registry element providing this component name. Also set position to the index
	ImgModule   * GetModuleFromModuleHandle(const _tModuleHandle Handle, size_t * pIndex  = NULL); /// Return the registry element associated to the module handle
	ImgModule   * GetModuleFromOmxHandle   (OMX_HANDLETYPE hComponent  , size_t * pIndex  = NULL); /// Return the registry element associated to the provided handle returns also the handle list element matching this handle

	int           OpenModule  (ImgModule &Module); /// Open the shared library of the given registry element an error is retuned if library already opened 
	OMX_ERRORTYPE CloseModule (ImgModule &, bool check = true);/// Close the shared library if no more components handles using it. if check is true then error is raised if handle list is not free

	int RegisterHandle  (OMX_HANDLETYPE Handle, ImgModule &Module, size_t factory_index);
	int UnregisterHandle(OMX_HANDLETYPE Handle);

	//Helpers
	void DumpModule(const ImgModule &Elem) const;
	void DumpAllModules() const ;
	// Members
	DynamicObjectArray<ImgModule >            m_ArrayModule;             // Store active modules
	DynamicObjectArray<_tActiveOmxComponent > m_ArrayActiveComponents;   // Store active components
	const char                              * m_fnEntryName;
//	const char                              * m_Directory;
};

IMG_ENSWRAPPER_API STE_Img_Loader & Get_ImgLoader_Instance(); // return the global instance object

#endif /* _IMG_OMXLOADER_H_ */
