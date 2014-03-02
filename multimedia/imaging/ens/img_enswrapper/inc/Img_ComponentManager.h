#ifndef _IMG_COMPONENTMANAGER_H_
#define _IMG_COMPONENTMANAGER_H_

IMG_ENSWRAPPER_API const char *GetDefaultImgOmxComponentPath();


class IMG_ENSWRAPPER_API Img_ComponentManager
//********************************************************************************
{ // Global instance of wrapper for imaging
public:
	Img_ComponentManager();
	~Img_ComponentManager();
};


class IMG_ENSWRAPPER_API OmxComponentLoader
//*************************************************************************************************************
{ /// Manage load/unload and factories for OMX ste component
public:
	OmxComponentLoader();
	~OmxComponentLoader();
	//int SetOmxPath(const char *path) { m_Path=path; return(S_OK);}
	const char *GetDefaultPath() const;

	int SetFunctionName(const char *func) { m_FunctionName=func; return(S_OK);}
	const char *GetFunctionName() const { return(m_FunctionName); }

	int ResetList();
	//int ScanPath();

	enum
	{
		eError_None,
		eError_Module_notFound,
		eError_FunctionNotFoundInModule,
		eError_NoOmxFactoryInModule,
		eError_CannotFillStructur,
		eError_ComponentNotFound,
		eError_DirectoryNotFound,
	};

	typedef struct
	{
		const char           * name;
		size_t                 in_use; // number of component currently in use
		ModuleLoader::_tModule Module;
		void                 * pFunction;
		void Default() {name =""; in_use=0; Module=0; pFunction=NULL;}
		enum 
		{
			eCmp_ModuleByName=0,
			eCmp_NameWithModuleName,
			eCmp_ComponentName,      // Search a component name for getHandle
		};
	} OmxModule;

	int LoadOmxModule(const char *module_name, const char *path=NULL);
	int ScanDirectory(const char *path=NULL /* NULL for default name*/ );
	int CreateComponent(const char *name, OMX_COMPONENTTYPE &ComponentType);

protected:
	// const char * m_Path;         //path for searching omx  components
	const char * m_FunctionName;   //function to search for
	DynamicObjectArray<OmxModule> m_ArrayModule;
};



#endif //_IMG_COMPONENTMANAGER_H_
