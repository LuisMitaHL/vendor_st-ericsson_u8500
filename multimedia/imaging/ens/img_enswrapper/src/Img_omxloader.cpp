/*****************************************************************************
* Copyright (C) ST-Ericsson SA 2010. All rights reserved.
* This code is ST-Ericsson proprietary and confidential.
* Any use of the code for whatever purpose is subject to
* specific written permission of ST-Ericsson SA.

* Author:     : laurent.regnier@st.com
* Filename:   : Img_omxloader.cpp
* Description : imaging loader for omx component
*****************************************************************************/
#define _CRT_SECURE_NO_WARNINGS
#include <string.h>
#include <stdlib.h>

// Get the OMX_VERSION from the ENS headers
#include <omxil/OMX_Types.h>

// Wrapper services
#include "Img_EnsWrapper_Shared.h"
#include "Img_EnsWrapper.h"

#define LOG_TAG "Img_omxLoader"
#include "linux_utils.h"

// Header file for this module
#include "osi_toolbox_lib.h"
#include "omxapi_loader.h" //Bellagio osal 
#include "Img_omxloader_interface.h"
#include "Img_omxloader.h"
#include "Img_ComponentManager.h"

using namespace ImgEns;

const char ComponentEntry_Name[]=IMG_OMXLOADER_COMPONENT_ENTRY_NAME; // Functionn name for implementing a ImgEns OMX component

ImgModule::ImgModule()
//******************************************************************************************
{
	Components         = NULL;
	NbrFactory         = 0;
	ModuleHandle       = 0;
	NbrActiveComponent = 0;
}

ImgModule::~ImgModule()
//******************************************************************************************
{
}

int ImgModule::FindComponent(const char *name, const _tImgOmxFactoryDescription **ppFactory) const
//******************************************************************************************
{
	int index_ok=-1; //invalid by default
	const _tImgOmxFactoryDescription *pDesc=Components;
	for (size_t i=0; i< NbrFactory; ++i)
	{
		OTB_ASSERT(ModuleHandle!=0);
		if (strcmp(name, pDesc->name)==0)
		{
			index_ok=(int)i; // Keep current index
			break;
		}
		++pDesc;
	}
	if (index_ok < 0)
	{
		pDesc=NULL;
	}
	if (ppFactory)
		*ppFactory=pDesc;
	return(index_ok);
}

const _tImgOmxFactoryDescription * ImgModule::GetComponent(size_t index) const
//******************************************************************************************
{
	if (index >= NbrFactory)
		return(NULL);
	else
		return(&Components[index]);
}

ImgModule & ImgModule::operator =(const ImgModule &Module)
//******************************************************************************************
{
	Components  = Module.Components;       /// The components definition for this element */
	NbrFactory  = Module.NbrFactory;       /// Number of registred component is this module
	ModulePath  = Module.ModulePath;       /// The shared library name for this element
	ModuleHandle= Module.ModuleHandle;     /// The shared library handle for this element
	return(*this);
}

int ImgModule::CloseModule()
//******************************************************************************************
{
	int res=S_OK;
	if (ModuleHandle !=0)
		res= ::CloseModule(ModuleHandle);
	ModuleHandle       = 0;
	NbrActiveComponent = 0;
	return(res);
}

size_t ImgModule::ActiveComponentsCount() const
//******************************************************************************************
{
	return(NbrActiveComponent);
}

size_t ImgModule::ActiveComponentsDowncount()
//******************************************************************************************
{
	OTB_ASSERT(NbrActiveComponent);
	if (NbrActiveComponent> 0)
		--NbrActiveComponent;
	return(NbrActiveComponent);
}

int CmpImgModule(void *criteria, ImgModule *pModule, int what)
//******************************************************************************************
{
	switch(what)
	{
	case eFindBy_ModuleName:
		return (strcmp((const char *)criteria, pModule->ModulePath.GetBuffer()));
	case eFindBy_ModuleHandle:
#ifdef _MSC_VER
		return( (_tModuleHandle)criteria - pModule->ModuleHandle);
#else
		return( (int)criteria - (int)pModule->ModuleHandle);
#endif
	default:
		OTB_ASSERT(0);
		return(-1);
	}
}

int CmpActiveComponents(void *criteria, _tActiveOmxComponent *pModule, int what)
//******************************************************************************************
{
	switch(what)
	{
	case eFindBy_ModuleHandle:
#ifdef _MSC_VER
		return( (_tModuleHandle)criteria - pModule->ModuleHandle);
#else
		return( (int)criteria - (int)pModule->ModuleHandle);
#endif
	case eFindBy_OmxHandle:
		return( (const char *)criteria - (const char *)pModule->OmxHandle);
	default:
		OTB_ASSERT(0);
		return(-1);
	}
}

//******************************************************************************************
//******************************************************************************************
//  BOSA interface
//******************************************************************************************
//******************************************************************************************

STE_Img_Loader & Get_ImgLoader_Instance()
//******************************************************************************************
{ // Return global instance
	static STE_Img_Loader Global_Img_Loader(ComponentEntry_Name);
	return(Global_Img_Loader); 
}

STE_Img_Loader::STE_Img_Loader(const char *fnEntryName)
//******************************************************************************************
{
	m_fnEntryName=fnEntryName;
	m_ArrayModule.SetFN_FindCompare(LinearFind, (_fnCompare)CmpImgModule);
	m_ArrayActiveComponents.SetFN_FindCompare(LinearFind, (_fnCompare)CmpActiveComponents);
}


STE_Img_Loader::~STE_Img_Loader()
//******************************************************************************************
{
}

const char * STE_Img_Loader::GetDirectory() const
//******************************************************************************************
{
	return(GetDefaultImgOmxComponentPath());
}


int STE_Img_Loader::RegisterHandle(OMX_HANDLETYPE Handle, ImgModule &Module, size_t factory_index)
//******************************************************************************************
{ // Add a component to the list of active one
	OTB_ASSERT(Handle);
	if ((Handle == NULL) || (Module.GetModuleHandle() ==0))
	{
		OTB_ASSERT(0);
		return(STE_Img_Loader::eError_NullOmxHandle);
	}
	Module.ActiveComponentsUpcount(); // increment the module active counter
	_tActiveOmxComponent *pActive;
	//Create a new entry
	m_ArrayActiveComponents.GetNew(1, (void **)&pActive);
	if (pActive ==NULL)
		return(STE_Img_Loader::eError_CannotRegisterOmxHandle);
	pActive->ModuleHandle = Module.GetModuleHandle();
	pActive->OmxHandle    = Handle;
	pActive->IndexFactory = factory_index; // Not yet managed
	return(S_OK);
}

int STE_Img_Loader::UnregisterHandle(OMX_HANDLETYPE Handle)
//******************************************************************************************
{ // Remove a component from the list of active one
	OTB_ASSERT(Handle);
	if (Handle == NULL)
	{
		OTB_ASSERT(0);
		return(STE_Img_Loader::eError_NullOmxHandle);
	}
	_tActiveOmxComponent *pActive;
	int index=m_ArrayActiveComponents.Find(Handle, 0, NULL, (void *)eFindBy_OmxHandle);
	if (index < 0)
	{ // not found
		OTB_ASSERT(0);
		return(STE_Img_Loader::eError_InvalidActiveOmxHandle);
	}
	pActive=m_ArrayActiveComponents[index];
	ImgModule *pModule=GetModuleFromModuleHandle(pActive->ModuleHandle);
	if (pModule == NULL)
	{ // not found
		OTB_ASSERT(0);
		return(STE_Img_Loader::eError_InvalidActiveModuleHandle);
	}

	pModule->ActiveComponentsDowncount();
	int res=m_ArrayActiveComponents.RemoveAt(index, 1);
	OTB_ASSERT(res ==1);
	return(S_OK);
}

size_t STE_Img_Loader::FindCompatibleComponents(const char *Directory)
//*************************************************************************************************************
{ // Scan a directory and search all compatible Img components
	DirScanner Dir;
	size_t NbLibrary=0;
	int status=Dir.Open(Directory);
	if (S_OK != status) 
	{
		LOGE("Failed to open directory %s", Directory);
		return (size_t)STE_Img_Loader::eError_DirectoryNotFound;
	}
	const char *Filename;
	ImgModule Module;
	StaticString<MAX_PATH> lib_absolute_path=Directory;
	NormalizePathEnd(lib_absolute_path);
	const size_t PathLen=lib_absolute_path.GetLength(); //store the length of the
	const char *ModuleExtension= GetModuleExtension(); // return the extension for a module versus OS '.so' or '.dll'
	LOGI("Scanning directory '%s'. Search for IMG.OMX.components with '%s' extension.", lib_absolute_path.GetBuffer(), ModuleExtension);
	while (Dir.GoNext()==S_OK)
	{
		Filename=Dir.GetFilename();
		const char *Extension=GetExtension(Filename);
		if (strcmp(Extension, ModuleExtension)!=0)
			continue; // Go next
#ifdef _MSC_VER
		// Reject some specific dlls
		if ( (strcmp(Filename, "msvcr90d.dll")==0) || (strcmp(Filename, "mfc90d.dll")==0) )
			continue; // Go next

#endif
		lib_absolute_path.SetLen(PathLen); // Keep only path part
		lib_absolute_path+= Filename;
		Module.SetModulePath(lib_absolute_path);
		status= OpenModule(Module);
		if (status==S_OK)
		{
			status=m_ArrayModule.Push(Module); //Store the component
			Module.SetModuleHandle(0); // Detach the handle
			LOGV("Module '%s' succesfully loaded", Filename);
		}
		else
		{
			LOGV("Module '%s' Not compatible!", Filename);
		}
	}
	Dir.Close();
	return NbLibrary;
}

void STE_Img_Loader::DumpModule(const ImgModule &Elem) const
//******************************************************************************************
{
	size_t cmp_iter = 0;
	LOGI("%s has registered:", Elem.GetModulePath());
	const _tImgOmxFactoryDescription *pComponent;
	pComponent= Elem.GetComponent(0);

	while(Elem.GetNbrFactory() > cmp_iter) 
	{
		LOGI("%s - %p - with roles:", pComponent->name, (void *) pComponent->fnImgFactory);
		int role_iter = 0;
		while(pComponent->roles[role_iter] !=NULL)
		{
			LOGI("  %s", pComponent->roles[role_iter]);
			role_iter++;
		}
		cmp_iter++;
		++pComponent;
	}
}

void STE_Img_Loader::DumpAllModules() const
//******************************************************************************************
{
//	ImgModule * iter_elem = NULL;
	size_t Nbr= m_ArrayModule.GetCount();
	for (size_t index=0; index< Nbr; ++index)
	{
		DumpModule(*m_ArrayModule[index]);
	}
}

int STE_Img_Loader::UnloadAll()
//******************************************************************************************
{ //Removr all loaded componenr
	int error = S_OK;
	size_t Nbr= m_ArrayModule.GetCount();
	while (Nbr > 0)
	{ //Close individually all modules
		Nbr--;
		if (CloseModule(*m_ArrayModule[Nbr])!= S_OK)
		{
			LOGE("Cannot unload %s", m_ArrayModule[Nbr]->GetModulePath() );
			error=eError_CannotUnregister;
		}
		else
			m_ArrayModule.Pop();

	}
	return error;
}

ImgModule * STE_Img_Loader::GetModuleFromName(const char *cComponentName, size_t *pIndex)
//******************************************************************************************
{ // Scan in all module the given component
	ImgModule * pModule = m_ArrayModule[0];
	size_t Nbr= m_ArrayModule.GetCount();
	const _tImgOmxFactoryDescription *pFactory;
	int component_index;
	for (size_t index=0; index< Nbr; ++index)
	{ // find on each Module
		component_index=pModule->FindComponent(cComponentName, &pFactory);
		if ( component_index >= 0)
		{ //element found
		if (pIndex)
			*pIndex=component_index;
			return(pModule);
		}
		++pModule;
	}
	if (pIndex)
		*pIndex=0;
	return(NULL); // not found
}

ImgModule * STE_Img_Loader::GetModuleFromModuleHandle(const _tModuleHandle Handle, size_t * pIndex)
//******************************************************************************************
{/// Return the registry element associated to the module handle
	ImgModule * pModule = m_ArrayModule[0];
	size_t Nbr= m_ArrayModule.GetCount();
	for (size_t index=0; index< Nbr; ++index)
	{ // find on each Module
		if ( Handle == pModule->GetModuleHandle())
		{ //element found
			if (pIndex)
				*pIndex=index;
			return(pModule);
		}
		++pModule;
	}
	if (pIndex)
		*pIndex=0;
	return(NULL); // not found
}


ImgModule * STE_Img_Loader::GetModuleFromOmxHandle(OMX_HANDLETYPE hComponent, size_t * pIndex)
//******************************************************************************************
{
	_tActiveOmxComponent * pActive = m_ArrayActiveComponents[0];
	size_t Nbr= m_ArrayActiveComponents.GetCount();
	//int component_index;
	for (size_t index=0; index< Nbr; ++index)
	{ // find on each Module
		if ( hComponent == pActive->OmxHandle)
		{ //element found
			return(GetModuleFromModuleHandle(pActive->ModuleHandle, pIndex));
		}
		++pActive;
	}
	if (pIndex)
		*pIndex=0;
	return(NULL); // not found
}

int STE_Img_Loader::OpenModule(ImgModule &Module)
//******************************************************************************************
{
	t_fnImgComponentEntry fnComponentEntry;
	const char *path=Module.GetModulePath();

	if(Module.GetModuleHandle() != NULL) 
	{ // already open
		return eError_ModuleAlreadyOpen;
	}
	ModuleLoader Loader;

	if (Loader.Open(path)!=S_OK) 
	{
		LOGE("failed to load %s", path);
		return eError_ModuleNotCompatible;
	}
	fnComponentEntry = (t_fnImgComponentEntry) Loader.GetProcAddress(m_fnEntryName);
	if (fnComponentEntry == NULL) 
	{
		LOGW("'%s' isn't a valid ImgEns library. Entry point '%s' not found", path, m_fnEntryName);
		return eError_ModuleNotCompatible;
	}

	const _tImgOmxFactoryDescription *pComponentsDesciption = NULL;
	// Now call the component entry point to get components description
	int Nb_component= fnComponentEntry(pComponentsDesciption);
	if ( (Nb_component<=0) || (pComponentsDesciption == NULL) )
	{
		LOGW("No valid components in library '%s'", path);
		return eError_ModuleNotCompatible;
	}
	// Element is ok, 'register' it
	Module.SetComponents(pComponentsDesciption, Nb_component);
	Module.SetModuleHandle(Loader.DetachModule()); // for avoiding closing the module 
	return S_OK;
}

OMX_ERRORTYPE STE_Img_Loader::CloseModule(ImgModule &Module, bool /*check*/)
//******************************************************************************************
{
	if(Module.GetModuleHandle() == NULL) 
		return OMX_ErrorNone;
	size_t Nbr=Module.ActiveComponentsCount();
	if ( Nbr == 0)
	{ // no more active component
		Module.CloseModule();
		return OMX_ErrorNone;
	}
	LOGE("STE_Img_Loader::CloseModule(%s) some components(nbr=%u) remain active  \n", Module.GetModulePath(), Nbr);
	return OMX_ErrorUndefined;
}

//******************************************************************************************
//***********                   Bellagio OSAL implementation            *******************
//******************************************************************************************

OMX_ERRORTYPE STE_Img_Loader_InitComponentLoader(BOSA_COMPONENTLOADER * /*loader*/)
//******************************************************************************************
{
	LOGI("STE_Img_Loader::InitComponentLoader");
	STE_Img_Loader &Loader=Get_ImgLoader_Instance();
	// Register the components

	const char *Dir;
	Dir=GetDefaultImgOmxComponentPath();

	size_t Nbr = Loader.FindCompatibleComponents(Dir);
	if ( Nbr > (size_t)STE_Img_Loader::eError_LoaderError)
		return OMX_ErrorComponentNotFound;
	//GET_PROPERTY("libc.debug.malloc", value, "0");
	//Get_ImgLoader_Instance().closeHandles = (atoi(value) ? false : true);
	return (OMX_ErrorNone);
}

OMX_ERRORTYPE STE_Img_Loader_DeInitComponentLoader(BOSA_COMPONENTLOADER * /*loader*/)
//******************************************************************************************
{
	STE_Img_Loader &Loader=Get_ImgLoader_Instance();
	LOGI("STE_Img_Loader::DeInitComponentLoader");
	if(Loader.UnloadAll() == S_OK)
		return OMX_ErrorNone;
	return (OMX_ErrorUndefined);
}

OMX_ERRORTYPE STE_Img_Loader_CreateComponent(BOSA_COMPONENTLOADER * /*loader*/, OMX_HANDLETYPE * pHandle, OMX_STRING cComponentName, OMX_PTR pAppData, OMX_CALLBACKTYPE * pCallBacks)
//******************************************************************************************
{
	ImgModule * pModule;
	size_t position;
	OMX_ERRORTYPE error;
	//struct omx_handle_list_elem * hlist_elem;
	STE_Img_Loader &Loader=Get_ImgLoader_Instance();

	// Default case for errors
	*pHandle = NULL;

	if((pModule = Loader.GetModuleFromName(cComponentName, &position)) == NULL) 
	{
		LOGI("Failed to find requested component :%s", cComponentName);
		return OMX_ErrorComponentNotFound;
	}

	// Open the shared library if not already opened
	if(pModule->GetModuleHandle() == NULL) 
	{
		if( Loader.OpenModule(*pModule) != S_OK)
		{
			return OMX_ErrorInsufficientResources;
		}
	}

	OMX_COMPONENTTYPE *pComponentType   = (OMX_COMPONENTTYPE *)new OMX_COMPONENTTYPE;
	pComponentType->pApplicationPrivate = pAppData;                 // Register the application private data to it
	pComponentType->nVersion.nVersion   = OMX_VERSION;              // Use the versions defined in the OMX_Types.h from the ENS
	pComponentType->nSize               = sizeof(OMX_COMPONENTTYPE);// set structure size
	*pHandle = pComponentType;

	// Call the component factory method
	error = (OMX_ERRORTYPE) (pModule->GetComponent(position)->fnImgFactory)(pComponentType);
	if ( error != OMX_ErrorNone) 
	{
		LOGE("Error returned by \"%s\" factory method\n", cComponentName);
		return error;
	}
	if(pHandle == NULL) 
	{
		LOGE("\"%s\" factory method failed to set handle", cComponentName);
		return OMX_ErrorComponentNotFound;
	}
	// Register the client callbacks to the components
	error = pComponentType->SetCallbacks(pComponentType, pCallBacks, pAppData);
	if (error != OMX_ErrorNone) 
	{
		LOGE("Failed to create requested component :%s", cComponentName);
		return error;
	}
	// Add the new handle to the list of active component
	Loader.RegisterHandle(pComponentType, *pModule, position);
	return OMX_ErrorNone;
}

OMX_ERRORTYPE STE_Img_Loader_DestroyComponent(BOSA_COMPONENTLOADER * /*loader*/, OMX_HANDLETYPE hComponent)
//******************************************************************************************
{
	ImgModule * pModule;
	OMX_ERRORTYPE error = OMX_ErrorNone;
	STE_Img_Loader &Loader=Get_ImgLoader_Instance();

	LOGI("DestroyComponent hComponent=0x%08x\n", (unsigned int) hComponent);

	size_t index;
	pModule = Loader.GetModuleFromOmxHandle(hComponent, &index);

	if(pModule == NULL) 
	{
		OTB_ASSERT(pModule);
		LOGE("DestroyComponent hComponent=0x%08x ERROR this component is not managed by Img_Loader", (unsigned int) hComponent);
		return OMX_ErrorComponentNotFound; /* Not a component managed by our loader */
	}
	// Request the component to DeInit
	if ((error = ((OMX_COMPONENTTYPE *) hComponent)->ComponentDeInit(hComponent)) != OMX_ErrorNone) 
	{
		return error;
	}
	
	// Close the library if no more handles opened
	Loader.UnregisterHandle(hComponent);
	if (hComponent)
		delete (OMX_COMPONENTTYPE *)hComponent; // Free the allocated OMX_COMPONENTTYPE

	LOGI("return OMX_ErrorNone - DestroyComponent hComponent=0x%08x", (unsigned int) hComponent);
	return OMX_ErrorNone;
}

OMX_ERRORTYPE STE_Img_Loader_ComponentNameEnum(BOSA_COMPONENTLOADER * /*loader*/, OMX_STRING cComponentName, OMX_U32 nNameLength, OMX_U32 nIndex)
//******************************************************************************************
{
	STE_Img_Loader &Loader=Get_ImgLoader_Instance();
	size_t iModule  = 0;
	size_t position = 0;
	ImgModule * pModule;

	//iterate from all module
	do 
	{
		pModule= Loader.GetModule(iModule);
		if (pModule==NULL)
			break;
		if (nIndex  < (position + pModule->GetNbrFactory()) )
		{ // found
			strncpy(cComponentName, pModule->GetComponent(nIndex-position)->name, nNameLength);
			return OMX_ErrorNone;
		}
		position += pModule->GetNbrFactory();
		++iModule;
	}while (position < nIndex);
	// Means that nIndex is bigger than the number of components registered
	LOGV("return OMX_ErrorNoMore -ComponentNameEnum nIndex=%d", (int) nIndex);
	return OMX_ErrorNoMore;
}

OMX_ERRORTYPE STE_Img_Loader_GetRolesOfComponent(BOSA_COMPONENTLOADER * /*loader*/, OMX_STRING compName, OMX_U32* pNumRoles, OMX_U8** roles)
//******************************************************************************************
{
	size_t position;
	OMX_U32 index = 0;
	OMX_U32 nbRolesCopied = 0;
	STE_Img_Loader &Loader=Get_ImgLoader_Instance();

	//LOGV("GetRolesOfComponent %s", compName);
	ImgModule * pModule = Loader.GetModuleFromName(compName, &position);

	if(pModule == NULL) 
	{ // Not found
		*pNumRoles = 0;
		LOGV("return OMX_ErrorComponentNotFound - GetRolesOfComponent %s pNumRoles=0", compName);
		return OMX_ErrorComponentNotFound;
	}

	const _tImgOmxFactoryDescription *pComponent= pModule->GetComponent(position);
	if (pComponent)
	{
		while(pComponent->roles[index] !=NULL) 
		{
			if((roles != NULL) && (*pNumRoles > index)) 
			{
				strcpy((char *) roles[index], pComponent->roles[index]);
				nbRolesCopied++;
			}
			index++;
		}
	}

	if(nbRolesCopied) 
	{// likely roles != NULL and we have copied out some roles set *pNumRoles to number of roles copied
		*pNumRoles = nbRolesCopied;
	} 
	else 
	{// likely roles == NULL so just set *pNumRoles to number of roles
		*pNumRoles = index;
	}
	//LOGV("return OMX_ErrorNone - GetRolesOfComponent %s pNumRoles=%d", compName, (int) *pNumRoles);
	return OMX_ErrorNone;
}

OMX_ERRORTYPE STE_Img_Loader_GetComponentsOfRole(BOSA_COMPONENTLOADER * /*loader*/, OMX_STRING role, OMX_U32* pNumComps, OMX_U8** compNames)
//******************************************************************************************
{
	OMX_U32 nbCmpCopied = 0;
	OMX_U32 nbCmpMatch = 0;

	//LOGV("GetComponentsOfRole %s", role);
	size_t iModule    = 0;
	size_t iComponent = 0;
	ImgModule * pModule;
	const _tImgOmxFactoryDescription *pComponent;
	STE_Img_Loader &Loader=Get_ImgLoader_Instance();
	const char **pRoles;

	//iterate from all module
	pModule= Loader.GetModule(iModule);
	while (pModule !=NULL)
	{// Scan each module
		if ( iComponent < pModule->GetNbrFactory())
		{
			pComponent= pModule->GetComponent(iComponent);
			pRoles=pComponent->roles;
			while (*pRoles !=NULL)
			{ // Scan each role of the given component
				if(strcmp(role, *pRoles) == 0) 
				{	// found matching role
					nbCmpMatch++;
					if((compNames != NULL) &&(*pNumComps > nbCmpCopied)) 
					{
						strcpy( (char *) compNames[nbCmpCopied], *pRoles);
						nbCmpCopied++;
					}
				}
				++pRoles;
			}
			++iComponent;
			
		}
		else
		{
			iComponent=0;
			++iModule;
			pModule= Loader.GetModule(iModule); // Get the new pointer
		}
	}
	if(nbCmpCopied != 0) 
	{	// Found some components matching the requested role they have been copied to the array
		*pNumComps = nbCmpCopied;
	} 
	else 
	{
		*pNumComps = nbCmpMatch;
	}
	//LOGV("return OMX_ErrorNone - GetComponentsOfRole %s pNumComps=%d", role, (int) *pNumComps);
	return OMX_ErrorNone;
}


static const BOSA_COMPONENTLOADER BellagioOsal_Img_OmxLoader=
{ //Const struct that define the interface with imaging loader
	&STE_Img_Loader_InitComponentLoader,
	&STE_Img_Loader_DeInitComponentLoader,
	&STE_Img_Loader_CreateComponent,
	&STE_Img_Loader_DestroyComponent,
	&STE_Img_Loader_ComponentNameEnum,
	&STE_Img_Loader_GetRolesOfComponent,
	&STE_Img_Loader_GetComponentsOfRole,
	NULL,
};

/**************************************************************************************************************
** The function called by the Bellagio core to register this components loader 
***************************************************************************************************************/
extern "C" IMG_ENSWRAPPER_API
OMX_ERRORTYPE setup_component_loader(BOSA_COMPONENTLOADER * pLoader)
//*************************************************************************************************************
{/** The function called by the Bellagio core to register this components loader */
	OTB_ASSERT(pLoader);
	if (pLoader == NULL) 
	{
		LOGE("setup_component_loader called with NULL loader containers\n");
		return OMX_ErrorInsufficientResources;
	}
	*pLoader= BellagioOsal_Img_OmxLoader; // Make a copy of the struct
	return OMX_ErrorNone;
}


