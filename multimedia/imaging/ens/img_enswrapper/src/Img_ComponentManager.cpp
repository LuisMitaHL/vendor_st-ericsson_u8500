/*****************************************************************************
* Copyright (C) ST-Ericsson SA 2010. All rights reserved.
* This code is ST-Ericsson proprietary and confidential.
* Any use of the code for whatever purpose is subject to
* specific written permission of ST-Ericsson SA.
* \file   Img_ComponentManager.cpp
* \author laurent.regnier@st.com
* \brief  
*****************************************************************************/
#include "Img_EnsWrapper_Shared.h"
#include "osi_toolbox_lib.h"
#include "Img_EnsWrapper.h"
#include "Img_ComponentManager.h"
#include "Img_EnsWrapper_Log.h"
#include "Img_omxloader_interface.h"

#ifdef _MSC_VER
	#ifdef _DEBUG
		#define IMG_OMX_COMPONENT_DEFAULT_DIR "Debug/OmxCpt" // Win32 Default dir for scanning Img components 
	#else
		#define IMG_OMX_COMPONENT_DEFAULT_DIR "Release/OmxCpt" // Win32 Default dir for scanning Img components 
	#endif
#elif defined(_WORKSTATION)
	#define IMG_OMX_COMPONENT_DEFAULT_DIR "$ST_ROOT/bin" // Linux workstation Default dir for scanning Img components 
#else
	#define IMG_OMX_COMPONENT_DEFAULT_DIR "/usr/lib/ste_img_omxcomponents" // Linux embedded Default dir for scanning Img components 
#endif

const char OmxDefaultComponentPath   [] = IMG_OMX_COMPONENT_DEFAULT_DIR;
const char Img_DefaultEnvComponentDir[] = IMG_ENV_OMX_COMPONENTS_DIR;


const char *GetDefaultImgOmxComponentPath()
//*************************************************************************************************************
{
	const char *dir = getenv( Img_DefaultEnvComponentDir );
	if ( (dir ==NULL) || (*dir=='\0') )
	{
		dir = OmxDefaultComponentPath;
	}
	return(dir);
}



int CompareModules(const void *pCriteria, const OmxComponentLoader::OmxModule *pModule, int param)
//*************************************************************************************************************
{ //Compare function for localizing module
	switch(param)
	{
	case OmxComponentLoader::OmxModule::eCmp_ModuleByName:
		return(strcmp( ((OmxComponentLoader::OmxModule*)pCriteria)->name, pModule->name));
	case OmxComponentLoader::OmxModule::eCmp_NameWithModuleName:
		return(strcmp((const char *)pCriteria, pModule->name));
	case OmxComponentLoader::OmxModule::eCmp_ComponentName:
		while(pModule->pFunction!=NULL)
		{
			if (strcmp((const char *)pCriteria, pModule->name)==0)
				return(0);
		}
		return(-1);
	default:
		OTB_ASSERT(0);
		return(0);
	}
}

OmxComponentLoader::OmxComponentLoader()
//: m_Path("omx_components")
: m_FunctionName("Omx_RegistredFactories")
, m_ArrayModule(LRAllocator_Mem<sizeof(OmxModule)>::GetAllocator(), NULL )
//*************************************************************************************************************
{
	m_ArrayModule.Reserve(128);
	m_ArrayModule.SetFN_FindCompare((_fnFind)LinearFind , (_fnCompare )CompareModules);

}

OmxComponentLoader::~OmxComponentLoader()
//*************************************************************************************************************
{
	ResetList();
}

const char *OmxComponentLoader::GetDefaultPath() const
//*************************************************************************************************************
{
	return(GetDefaultImgOmxComponentPath());
}

int OmxComponentLoader::ResetList()
//*************************************************************************************************************
{ // remove all components factory and unload libraries
	int status=S_OK;
	ModuleLoader loader;
	for (size_t index=0; index < m_ArrayModule.GetCount(); ++index)
	{
		if (m_ArrayModule[index]->Module!=0)
		{
			loader.SetModule(m_ArrayModule[index]->Module);
			loader.Close(); //Unload it
		}
	}
	m_ArrayModule.Clean();
	return(status);
}


typedef OMX_ERRORTYPE (*fnFactory)(OMX_COMPONENTTYPE &); /** The component constructor */
typedef int (*fn_Omx_RegistredFactories)(const OmxFactoryDescriptor *&pOmxFactoryArray);

#define MAX_STRING_NAME 128

int OmxComponentLoader::CreateComponent(const char *name, OMX_COMPONENTTYPE & /*ComponentType*/)
//*************************************************************************************************************
{
	int status=S_OK;
	//Function found, registers in database if not allready exist
	int index=0;
	int pos=m_ArrayModule.Find(name, 0, &index, (void *)OmxComponentLoader::OmxModule::eCmp_ComponentName);

	if (pos < 0)
		return(eError_ComponentNotFound);

	OTB_ASSERT(0);

#if 0
	// fnFactory factory;
	OMX_ERRORTYPE OmxResult= pOmxFactoryArray[0].fnFactory(ComponentType);
	if (OmxResult !=OMX_ErrorNone)
		status=eError_CannotFillStructur;
#endif


	return(status);
}

int OmxComponentLoader::LoadOmxModule(const char *module_name, const char *path)
//*************************************************************************************************************
{
	int status=S_OK;
	ModuleLoader loader;

	//Construct full name
	StaticString<MAX_STRING_NAME> Name;
	if ( (path ==NULL) || (*path=='\0') )
		path=GetDefaultPath();
	Name=path;
	NormalizePathEnd(Name);
	Name.Add(module_name);
	status= loader.Open(Name);
	if (status!=S_OK)
	{
		LOGI("\nCannot open %s", Name.GetBuffer());
		return(eError_Module_notFound); 
	}
	//Module is loaded, try to find the right function
	fn_Omx_RegistredFactories GetRegistredFactories;
	GetRegistredFactories= (fn_Omx_RegistredFactories)loader.GetProcAddress(m_FunctionName);
	if (GetRegistredFactories==NULL)
	{ 
		loader.Close();
		return(eError_FunctionNotFoundInModule);
	}

	const OmxFactoryDescriptor *pOmxFactoryArray=NULL;
	int nbr =GetRegistredFactories(pOmxFactoryArray);
	if (nbr <=0)
	{ 
		loader.Close();
		return(eError_NoOmxFactoryInModule); // No component 
	}

	//Function found, registers in database if not allready exist
	int index=0;
	int pos=m_ArrayModule.Find(module_name, 0, &index, (void *)OmxComponentLoader::OmxModule::eCmp_NameWithModuleName);
	if (pos < 0)
	{ // Add the module in the database
		OmxModule *pModule;
		m_ArrayModule.GetNew(1, (void **)&pModule); // find a new entry in database
		pModule->Default();
		pModule->name     = module_name;
		pModule->Module   = loader.DetachModule();
	}
	// pModule->pFunction
	else
	{ //allready exist don't close
		loader.DetachModule();
	}
	return(status);
}


#ifdef __linux
#include <sys/types.h>
#include <dirent.h>
#include <stdlib.h>

int OmxComponentLoader::ScanDirectory(const char * dir)
//*************************************************************************************************************
{
	if ( (dir ==NULL) || (*dir=='\0') )
	{
		dir = GetDefaultPath();
	}
	DIR           *pHandleDir;
	struct dirent *pDirEntry;
	pHandleDir = opendir(dir);

	if (NULL == pHandleDir)
	{
		LOGI("Failed to open directory %s\n", dir);
		return eError_DirectoryNotFound;
	} 
	size_t Nbr=0;
	while ( (pDirEntry = readdir(pHandleDir)) != NULL) 
	{
		// size_t len = strlen(pDirEntry->d_name);
		if ( LoadOmxModule(pDirEntry->d_name, dir)==S_OK)
		{
			LOGI("\n%2u: '%s' Has been sucessfuly registred", Nbr, pDirEntry->d_name);
			++Nbr;
		}
		else
			LOGI("\n--: '%s' is not a valid component", pDirEntry->d_name);
	};
	closedir(pHandleDir);
	return((int)Nbr);
}

#else
//Win32 version
#include <io.h>
int OmxComponentLoader::ScanDirectory(const char * dir)
//*************************************************************************************************************
{
	if ( (dir ==NULL) || (*dir=='\0') )
	{
		dir=GetDefaultPath();
	}
	size_t Nbr=0;
	struct _finddata_t fileinfo ;
	int handle=(int)_findfirst(dir, &fileinfo);
	if (handle ==-1)
	{
		LOGI("Failed to open directory %s\n", dir);
		return eError_DirectoryNotFound;
	} 
	while (handle >0)
	{
		if ( LoadOmxModule(fileinfo.name, dir)==S_OK)
		{
			LOGI("\n%2u: '%s' Has been sucessfuly registred", Nbr, fileinfo.name);
			++Nbr;
		}
		else
			LOGI("\n--: '%s' is not a valid component", fileinfo.name);

		if (_findnext(handle, &fileinfo)==-1)
			break; //Exit from loop
	}
	_findclose (handle);
	return(S_OK);
}
#endif

