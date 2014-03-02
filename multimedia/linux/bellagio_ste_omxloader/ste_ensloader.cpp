/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include <assert.h>
#include <sys/types.h>
#include <dirent.h>
#include <string.h>
#include <dlfcn.h>
#include <stdlib.h>
#include <pthread.h>

// Get the OMX_VERSION from the ENS headers
#include "OMX_Types.h"
// Wrapper services
#include "ENS_Wrapper.h"
// ENS prototypes
#include "ENS_DBC.h"

//#define LOG_NDEBUG 0
#define LOG_TAG "ENSLoader"
#include "linux_utils.h"
#include "OMX_debug.h"

// Header file for this module
#include "ste_ensloader.h"

// The list of ENS components
ens_cmp_list_head_t STE_ENS_Loader::ens_cmp_list;
// The loader mutex
pthread_mutex_t STE_ENS_Loader::mLoaderMutex = PTHREAD_MUTEX_INITIALIZER;

bool STE_ENS_Loader::closeHandles = true;
bool STE_ENS_Loader::initDone = false;

extern "C" {

    /** The STE component loader contructor.
     *
     * This function creates the STE dynamic component loader, and creates
     * the list of available components.
     */
    static OMX_ERRORTYPE BOSA_STE_InitComponentLoader(BOSA_COMPONENTLOADER * loader)
    {
	STE_ENS_Loader::Lock();
        OMX_ERRORTYPE retval = STE_ENS_Loader::STE_InitComponentLoader();
        STE_ENS_Loader::UnLock();
	return retval;
    }

    /** The destructor of the STE component loader.
     *
     * This function deallocates the list of available components.
     */
    static OMX_ERRORTYPE BOSA_STE_DeInitComponentLoader(BOSA_COMPONENTLOADER * loader)
    {
	STE_ENS_Loader::Lock();
	OMX_ERRORTYPE retval = STE_ENS_Loader::STE_DeInitComponentLoader();
	STE_ENS_Loader::UnLock();
	return retval;
    }

    /** Creator of the requested OpenMAX component
     *
     * This function searches for the requested component in the internal list.
     * If the component is found, its constructor is called,
     * and the standard callbacks are assigned.
     * A pointer to a standard OpenMAX component is returned.
     */
    OMX_ERRORTYPE BOSA_STE_CreateComponent(BOSA_COMPONENTLOADER * loader,
					   OMX_HANDLETYPE * pHandle,
					   OMX_STRING cComponentName,
					   OMX_PTR pAppData,
					   OMX_CALLBACKTYPE * pCallBacks)
    {
	ALOGV("CreateComponent %s\n", cComponentName);

	STE_ENS_Loader::Lock();
	OMX_ERRORTYPE retval = STE_ENS_Loader::STE_CreateComponent(pHandle, cComponentName,
								   pAppData, pCallBacks);
	STE_ENS_Loader::UnLock();

	char result_str[128];
	ALOGV("return %s - CreateComponent %s hComponent=0x%08x\n",
	     strOMX_ERRORTYPE(retval, result_str, 128), cComponentName, (int) *pHandle);

	return retval;
    }

    /** The component destructor of the STE component loader
     *
     * This function implements the OMX_FreeHandle function for the
     * STE component loader. Its interface is the same as the
     * standard FreeHandle function, except that the first parameter
     * that contains the private data of the specific component loader.
     */
    OMX_ERRORTYPE BOSA_STE_DestroyComponent(BOSA_COMPONENTLOADER * loader,
					    OMX_HANDLETYPE hComponent)
    {
	STE_ENS_Loader::Lock();
	OMX_ERRORTYPE retval = STE_ENS_Loader::STE_DestroyComponent(hComponent);
	STE_ENS_Loader::UnLock();
	return retval;
    }

    /** This function search for the index from 0 to end of the list
     *
     * This function searches in the list of STE dynamic components and enumerates
     * both the class names and the role specific components.
     */
    OMX_ERRORTYPE BOSA_STE_ComponentNameEnum(BOSA_COMPONENTLOADER * loader,
					     OMX_STRING cComponentName,
					     OMX_U32 nNameLength,
					     OMX_U32 nIndex)
    {
	STE_ENS_Loader::Lock();
	OMX_ERRORTYPE retval = STE_ENS_Loader::STE_ComponentNameEnum(cComponentName,
								     nNameLength, nIndex);
	STE_ENS_Loader::UnLock();
	return retval;
    }

    /** The specific version of OMX_GetRolesOfComponent
     *
     * This function replicates exactly the behavior of the
     * standard OMX_GetRolesOfComponent function for the STE component loader
     */
    OMX_ERRORTYPE BOSA_STE_GetRolesOfComponent(BOSA_COMPONENTLOADER * loader,
					       OMX_STRING compName,
					       OMX_U32 * pNumRoles,
					       OMX_U8 ** roles)
    {
	STE_ENS_Loader::Lock();
	OMX_ERRORTYPE retval = STE_ENS_Loader::STE_GetRolesOfComponent(compName, pNumRoles, roles);
	STE_ENS_Loader::UnLock();
	return retval;
    }

    /** The specific version of OMX_GetComponentsOfRole
     *
     * This function replicates exactly the behavior of the standard
     * OMX_GetComponentsOfRole function for the STE  component loader
     */
    OMX_ERRORTYPE BOSA_STE_GetComponentsOfRole(BOSA_COMPONENTLOADER * loader,
					       OMX_STRING role,
					       OMX_U32 * pNumComps,
					       OMX_U8 ** compNames)
    {
	STE_ENS_Loader::Lock();
	OMX_ERRORTYPE retval = STE_ENS_Loader::STE_GetComponentsOfRole(role, pNumComps, compNames);
	STE_ENS_Loader::UnLock();
	return retval;
    }

    /** The function called by the Bellagio core to register this components loader */
    OMX_ERRORTYPE setup_component_loader(BOSA_COMPONENTLOADER * loader)
    {

	ALOGV("setup_component_loader\n");
	if (loader == NULL) {
	    ALOGE("setup_component_loader called with NULL loader containers\n");
	    return OMX_ErrorInsufficientResources;
	}

	loader->BOSA_InitComponentLoader = &BOSA_STE_InitComponentLoader;
	loader->BOSA_DeInitComponentLoader = &BOSA_STE_DeInitComponentLoader;
	loader->BOSA_CreateComponent = &BOSA_STE_CreateComponent;
	loader->BOSA_DestroyComponent = &BOSA_STE_DestroyComponent;
	loader->BOSA_ComponentNameEnum = &BOSA_STE_ComponentNameEnum;
	loader->BOSA_GetRolesOfComponent = &BOSA_STE_GetRolesOfComponent;
	loader->BOSA_GetComponentsOfRole = &BOSA_STE_GetComponentsOfRole;

	return OMX_ErrorNone;
    }
}

/** dbc_assert implementation called by the ENS components */
void dbc_assert(int a, DBCError_e errorId, const char * file, int line)
{
    if(a == 0) {
	ALOGE("dbc_assert errorId=%d file=%s line=%d\n", errorId, file, line);
    }
    assert(a);
}

OMX_ERRORTYPE STE_ENS_Loader::STE_InitComponentLoader()
{
    OMX_ERRORTYPE error;

    ALOGV("InitComponentLoader\n");

    if (!initDone) {

        // Initialize the list of components
        LIST_INIT(&ens_cmp_list);

        // Register the components
        if((error = registerComponents()) != OMX_ErrorNone)
            return error;

        GET_PROPERTY("libc.debug.malloc", value, "0");
        closeHandles = (atoi(value) ? false : true);

        initDone = true;
    }

    return OMX_ErrorNone;
}

OMX_ERRORTYPE STE_ENS_Loader::STE_DeInitComponentLoader()
{
    OMX_ERRORTYPE error;
    ALOGV("DeInitComponentLoader\n");
    if (initDone) {
        // Free used resources
        if((error = freeComponentList()) != OMX_ErrorNone)
            return error;
        initDone = false;
    }
    return OMX_ErrorNone;
}

OMX_ERRORTYPE STE_ENS_Loader::STE_CreateComponent(OMX_HANDLETYPE * pHandle,
						  OMX_STRING cComponentName,
						  OMX_PTR pAppData,
						  OMX_CALLBACKTYPE * pCallBacks)
{
    struct ens_cmp_list_elem * elem;
    int position;
    OMX_ERRORTYPE error;
    ENS_Wrapper *wrapper;
    struct omx_handle_list_elem * hlist_elem;

    // Default case for errors
    *pHandle = NULL;

    if((elem = getRegistryForName(cComponentName, &position)) == NULL) {
	ALOGV("Failed to find requested component :%s\n", cComponentName);
	return OMX_ErrorComponentNotFound;
    }

    // Open the shared library if not already opened
    if(elem->library_handle == NULL) {
	if((error = openLibraryForElem(elem)) != OMX_ErrorNone) {
	    return error;
	}
    }

    // Call the component factory method
    ENS_IOMX* pENSComponent = NULL;
    if ((error = elem->modules_components->array[position].constructor(((ENS_Component **)(&pENSComponent))))
	!= OMX_ErrorNone) {
	ALOGE("Error returned by \"%s\" factory method\n", cComponentName);
	closeLibraryForElem(elem, false);
	return error;
    }
    if(pENSComponent == NULL) {
	ALOGE("\"%s\" factory method failed to set handle\n", cComponentName);
	closeLibraryForElem(elem, false);
	return OMX_ErrorComponentNotFound;
    }

    // Allocate the OMX component handle
    OMX_COMPONENTTYPE *openmaxStandComp = (OMX_COMPONENTTYPE *) calloc(1, sizeof(OMX_COMPONENTTYPE));
    if (!openmaxStandComp) {
	ALOGE("Failed to allocate OMX_COMPONENTTYPE for \"%s\"\n", cComponentName);
	closeLibraryForElem(elem, false);
	return OMX_ErrorInsufficientResources;
    }

    // Register the application private data to it
    openmaxStandComp->pApplicationPrivate = pAppData;

    // Use the versions defined in the OMX_Types.h from the ENS
    openmaxStandComp->nVersion.s.nVersionMajor	= OMX_VERSION_MAJOR;
    openmaxStandComp->nVersion.s.nVersionMinor	= OMX_VERSION_MINOR;
    openmaxStandComp->nVersion.s.nRevision	= OMX_VERSION_REVISION;
    openmaxStandComp->nVersion.s.nStep		= OMX_VERSION_STEP;
    // set structure size
    openmaxStandComp->nSize			= sizeof(OMX_COMPONENTTYPE);

    // Set name before creating wrapper as it is used for debug
    pENSComponent->setName(cComponentName);

    // As long as not moved to factory method of each component
    // we call it before wrapper instance creation
    error = pENSComponent->createResourcesDB();
    if(error != OMX_ErrorNone) {
	goto exit_error;
    }

    // Allocate the wrapper and wrap the ENS component
    // The wrapper overwrite the pComponentPrivate to point to wrapper instead of pENSComponent
    wrapper = new ENS_Wrapper(pENSComponent, openmaxStandComp, cComponentName);
    if (wrapper == 0) {
	error = OMX_ErrorInsufficientResources;
	goto exit_error;
    }

    // Activate OST trace
    error = wrapper->ActivateOSTtrace(wrapper->OST_trace_mpc_mode);
    if (error != OMX_ErrorNone) {
	delete wrapper;
	goto exit_error;
    }

    // Register the component to the RME
    error = pENSComponent->setOMXHandleAndRegisterRm(openmaxStandComp);
    if (error != OMX_ErrorNone) {
	delete wrapper;
	goto exit_error;
    }

    // Add this new handle to the list of handle of this elem
    hlist_elem = (struct omx_handle_list_elem *) malloc(sizeof(struct omx_handle_list_elem));
    if(hlist_elem == NULL) {
	delete wrapper;
	error = OMX_ErrorInsufficientResources;
	goto exit_error;
    }
    hlist_elem->component_handle = openmaxStandComp;
    LIST_INSERT_HEAD(&elem->handle_list, hlist_elem, list_entry);

    // Register the client callbacks to the components
    error = openmaxStandComp->SetCallbacks(openmaxStandComp, pCallBacks, pAppData);
    if (error != OMX_ErrorNone) {
	delete wrapper;
	goto exit_error;
    }

    // finaly provide the handle to the client
    *pHandle = openmaxStandComp;

    return OMX_ErrorNone;

 exit_error:
    ALOGE("Failed to create requested component :%s\n", cComponentName);
    pENSComponent->ComponentDeInit();
    closeLibraryForElem(elem, false);
    return error;
}

OMX_ERRORTYPE STE_ENS_Loader::STE_DestroyComponent(OMX_HANDLETYPE hComponent)
{
    struct ens_cmp_list_elem * elem;
    struct omx_handle_list_elem * helem;
    OMX_ERRORTYPE error = OMX_ErrorNone;

    ALOGV("DestroyComponent hComponent=0x%08x\n", (unsigned int) hComponent);

    elem = getRegistryForHandle(hComponent, &helem);

    if(elem == NULL) {
	ALOGV("return OMX_ErrorComponentNotFound - DestroyComponent hComponent=0x%08x\n",
	     (unsigned int) hComponent);
	/* Not a component managed by our loader */
	return OMX_ErrorComponentNotFound;
    }
    // Request the component to DeInit
    if ((error = ((OMX_COMPONENTTYPE *) hComponent)->ComponentDeInit(hComponent)) != OMX_ErrorNone) {
	return error;
    }
    // Free the wrapper instance
    delete (ENS_Wrapper_p) ((OMX_COMPONENTTYPE *) hComponent)->pComponentPrivate;

    // Free the allocated OMX_COMPONENTTYPE
    free(hComponent);

    // Remove and free handle from list of handles for this module
    LIST_REMOVE(helem, list_entry);
    free(helem);

    // Close the library if no more handles opened
    closeLibraryForElem(elem, false);

    ALOGV("return OMX_ErrorNone - DestroyComponent hComponent=0x%08x\n", (unsigned int) hComponent);

    return OMX_ErrorNone;
}

OMX_ERRORTYPE STE_ENS_Loader::STE_ComponentNameEnum(OMX_STRING cComponentName, OMX_U32 nNameLength,
						    OMX_U32 nIndex)
{
    struct ens_cmp_list_elem * iter_elem = NULL;
    OMX_U32 index = 0;
    int found = 0;

    ALOGV("ComponentNameEnum nIndex=%d\n", (int) nIndex);

    LIST_FOREACH(iter_elem, &ens_cmp_list, list_entry) {
	int iter = 0;
	while(iter_elem->modules_components->array[iter].name[0] !='\0') {
	    if (index == nIndex) {
		// here we assume that component name is not bigger than nNameLength
		// and that memory after name is readable ...
		strncpy(cComponentName, iter_elem->modules_components->array[iter].name, nNameLength);
		found = 1;
		break;
	    }
	    index++;
	    iter++;
	}
	if (found)
	    break;
    }
    if (!found) {
	// Means that nIndex is bigger than the number of components registered
	ALOGV("return OMX_ErrorNoMore -ComponentNameEnum nIndex=%d\n", (int) nIndex);
	return OMX_ErrorNoMore;
    }
    ALOGV("return OMX_ErrorNone - ComponentNameEnum cComponentName=%s nIndex=%d\n",
	 cComponentName, (int) nIndex);
    return OMX_ErrorNone;
}

OMX_ERRORTYPE STE_ENS_Loader::STE_GetRolesOfComponent(OMX_STRING compName, OMX_U32* pNumRoles,
						      OMX_U8** roles)
{
    struct ens_cmp_list_elem * elem;
    int position;
    OMX_U32 index = 0;
    OMX_U32 nbRolesCopied = 0;

    ALOGV("GetRolesOfComponent %s\n", compName);

    if((elem = getRegistryForName(compName, &position)) == NULL) {
	*pNumRoles = 0;
	ALOGV("return OMX_ErrorComponentNotFound - GetRolesOfComponent %s pNumRoles=0\n", compName);
	return OMX_ErrorComponentNotFound;
    }

    while(elem->modules_components->array[position].roles[index][0] !='\0') {
	if((roles != NULL) && (*pNumRoles > index)) {
	    strcpy((char *) roles[index], elem->modules_components->array[position].roles[index]);
	    nbRolesCopied++;
	}
	index++;
    }

    if(nbRolesCopied) {
	// likely roles != NULL and we have copied out some roles
	// set *pNumRoles to number of roles copied
	*pNumRoles = nbRolesCopied;
   } else {
	// likely roles == NULL so just set *pNumRoles to number of roles
	*pNumRoles = index;
    }
    ALOGV("return OMX_ErrorNone - GetRolesOfComponent %s pNumRoles=%d\n", compName, (int) *pNumRoles);
    return OMX_ErrorNone;
}

OMX_ERRORTYPE STE_ENS_Loader::STE_GetComponentsOfRole(OMX_STRING role,
						      OMX_U32* pNumComps,
						      OMX_U8** compNames)
{
    struct ens_cmp_list_elem * iter_elem;
    OMX_U32 nbCmpCopied = 0;
    OMX_U32 nbCmpMatch = 0;

    ALOGV("GetComponentsOfRole %s\n", role);

    LIST_FOREACH(iter_elem, &ens_cmp_list, list_entry) {
	int iter_cmp = 0;
	while(iter_elem->modules_components->array[iter_cmp].name[0] !='\0') {
	    int iter_role = 0;
	    while(iter_elem->modules_components->array[iter_cmp].roles[iter_role][0] !='\0') {
		if(strcmp(role, iter_elem->modules_components->array[iter_cmp].roles[iter_role]) == 0) {
		    // found matching role
		    nbCmpMatch++;
		    if((compNames != NULL) &&(*pNumComps > nbCmpCopied)) {
			strcpy((char *) compNames[nbCmpCopied],
			       iter_elem->modules_components->array[iter_cmp].name);
			nbCmpCopied++;
		    }
		}
		iter_role++;
	    }
	    iter_cmp++;
	}
    }

    if(nbCmpCopied != 0) {
	// Found some components matching the requested role
	// they have been copied to the array
	*pNumComps = nbCmpCopied;
    } else {
	*pNumComps = nbCmpMatch;
    }
    ALOGV("return OMX_ErrorNone - GetComponentsOfRole %s pNumComps=%d\n", role, (int) *pNumComps);
    return OMX_ErrorNone;
}

OMX_ERRORTYPE STE_ENS_Loader::registerComponents()
{
    DIR *dirp;
    struct dirent *dp;
    void *handle;
    void (*fptr)(struct ste_omx_ens_component_def_array** refs);
    OMX_ERRORTYPE error;
    char *buffer;
    char *omx_components_dir;

    buffer = getenv("OMX_STE_ENS_COMPONENTS_DIR");
    if(buffer!=NULL&&*buffer!='\0') {
	omx_components_dir = buffer;
    } else {
	omx_components_dir = (char *) OMX_COMPONENT_PATH;
    }
    ALOGV("About to opendir %s...\n", omx_components_dir);
    dirp = opendir(omx_components_dir);
    ALOGV("opendir %s done\n", omx_components_dir);
    if (NULL == dirp) {
	ALOGE("Failed to open directory %s\n", omx_components_dir);
	return OMX_ErrorUndefined;
    } else {
	ALOGI("Scanning directory %s for STE ENS based components\n", omx_components_dir);
    }

    while ((dp = readdir(dirp)) != NULL) {
	ALOGV("About to check %s\n", dp->d_name);

	int len = strlen(dp->d_name);

	if (len <= 3)
	    continue;

	if (strncmp(dp->d_name + len - 3, ".so", 3) == 0) {
	    char lib_absolute_path[strlen(omx_components_dir) + len + 1];

	    strcpy(lib_absolute_path, omx_components_dir);
	    strcat(lib_absolute_path, dp->d_name);

	    ALOGV("About to dlopen %s\n", lib_absolute_path);
	    if ((handle = dlopen(lib_absolute_path, RTLD_NOW)) == NULL) {
		ALOGE("failed to load %s: %s\n", lib_absolute_path, dlerror());
	    } else {
		ALOGV("Opened %s\n", lib_absolute_path);
		if ((fptr = (ste_omx_ens_component_register_t)
		     dlsym(handle, "ste_omx_ens_component_register")) == NULL) {
		    ALOGW("%s is not loadable by STE ENS component loader\n", lib_absolute_path);
		    dlclose(handle);
		    continue;
		}
		ALOGV("dlsym of ste_omx_ens_component_register done.\n");
		struct ens_cmp_list_elem * registry_elem =
		    (struct ens_cmp_list_elem *) calloc(1, sizeof(struct ens_cmp_list_elem));
		if(registry_elem == NULL) {
		    ALOGE("Failed to allocate memory for registry_elem\n");
		    dlclose(handle);
		    freeComponentList();
		    closedir(dirp);
		    return OMX_ErrorInsufficientResources;
		}

		// Initialize the handle list
		LIST_INIT(&(registry_elem->handle_list));

		// Add this element to the registry list
		LIST_INSERT_HEAD(&ens_cmp_list, registry_elem, list_entry);

		// Register the library handle
		registry_elem->library_handle = handle;

		// Get the registration structure from the component
		struct ste_omx_ens_component_def_array * defs = NULL;
		fptr(&defs);

		if(defs == NULL) {
		    ALOGE("the library %s did not registered correctly to STE ENS component loader",
			 lib_absolute_path);
		    freeComponentList(); // this will also take care to dlclose
		    closedir(dirp);
		    return OMX_ErrorUndefined;
		}

		// Allocate and copy the components definition as we'll close the library right after
		struct ste_omx_ens_component_def_array * defs_copy =
		    (struct ste_omx_ens_component_def_array *)
		    malloc(sizeof(struct ste_omx_ens_component_def_array));
		if(defs_copy == NULL) {
		    ALOGE("Failed to allocate memory for defs_copy\n");
		    freeComponentList(); // this will also take care to dlclose
		    closedir(dirp);
		    return OMX_ErrorInsufficientResources;
		}
		memcpy(defs_copy, defs, sizeof(struct ste_omx_ens_component_def_array));
		registry_elem->modules_components = defs_copy;

		// Record the library name providing the registered components
		registry_elem->library_name = (char*) calloc(1, strlen(lib_absolute_path) + 1);
		strcpy(registry_elem->library_name, lib_absolute_path);

		// Provide information related to the registered component
		dumpRegistry(registry_elem);

		if ((error = closeLibraryForElem(registry_elem)) != OMX_ErrorNone) {
		    closedir(dirp);
		    return error;
		}
	    }
	}
    }

    closedir(dirp);

    return OMX_ErrorNone;
}

void STE_ENS_Loader::dumpRegistry(struct ens_cmp_list_elem * elem)
{
    int cmp_iter = 0;
    ALOGV("%s has registered:\n", elem->library_name);
    while(elem->modules_components->array[cmp_iter].name[0] !='\0') {
	ALOGV("%s - %p - with roles:\n", elem->modules_components->array[cmp_iter].name,
	     (void *) elem->modules_components->array[cmp_iter].constructor);
	int role_iter = 0;
	while(elem->modules_components->array[cmp_iter].roles[role_iter][0] !='\0') {
	    ALOGV("  %s\n", elem->modules_components->array[cmp_iter].roles[role_iter]);
	    role_iter++;
	}
	cmp_iter++;
    }
}

void STE_ENS_Loader::dumpRegistryTable()
{
    struct ens_cmp_list_elem * iter_elem = NULL;
    LIST_FOREACH(iter_elem, &ens_cmp_list, list_entry) {
	dumpRegistry(iter_elem);
    }
}

OMX_ERRORTYPE STE_ENS_Loader::freeComponentList()
{
    struct ens_cmp_list_elem * iter_elem = NULL;
    struct ens_cmp_list_elem * tmp_next_elem = NULL;
    OMX_ERRORTYPE error = OMX_ErrorNone;

    LIST_FOREACH_SAFE(iter_elem, &ens_cmp_list, list_entry, tmp_next_elem) {
	LIST_REMOVE(iter_elem, list_entry);
	if((error = closeLibraryForElem(iter_elem)) != OMX_ErrorNone)
	    return error;
	free(iter_elem->library_name);
	free(iter_elem->modules_components);
	free(iter_elem);
    }
    return OMX_ErrorNone;
}

struct ens_cmp_list_elem * STE_ENS_Loader::getRegistryForName(OMX_STRING cComponentName, int * position)
{
    struct ens_cmp_list_elem * iter_elem = NULL;
    LIST_FOREACH(iter_elem, &ens_cmp_list, list_entry) {
	int iter = 0;
	while(iter_elem->modules_components->array[iter].name[0] !='\0') {
	    if(strcmp(iter_elem->modules_components->array[iter].name, cComponentName) == 0) {
		// Found matching component name
		if(position)
		    *position = iter;
		else
		    return NULL;
		return iter_elem;
	    }
	    iter++;
	}
    }
    return NULL;
}

struct ens_cmp_list_elem * STE_ENS_Loader::getRegistryForHandle(OMX_HANDLETYPE hComponent,
								struct omx_handle_list_elem ** helem)
{
    struct ens_cmp_list_elem * iter_elem = NULL;
    struct omx_handle_list_elem * iter_helem = NULL;
    LIST_FOREACH(iter_elem, &ens_cmp_list, list_entry) {
	LIST_FOREACH(iter_helem, &iter_elem->handle_list, list_entry) {
	    if(iter_helem->component_handle == hComponent) {
		*helem = iter_helem;
		return iter_elem;
	    }
	}
    }
    return NULL;
}

OMX_ERRORTYPE STE_ENS_Loader::openLibraryForElem(struct ens_cmp_list_elem * elem)
{
    void (*fptr)(struct ste_omx_ens_component_def_array **refs);

    if(elem->library_handle != NULL) {
	return OMX_ErrorUndefined;
    }

    ALOGV("About to dlopen %s\n", elem->library_name);
    elem->library_handle = dlopen(elem->library_name, RTLD_NOW);

    if (elem->library_handle == NULL) {
	ALOGE("failed to load %s: %s\n", elem->library_name, dlerror());
	return OMX_ErrorInsufficientResources;
    }
    ALOGV("Opened %s\n", elem->library_name);

    if ((fptr = (ste_omx_ens_component_register_t)
	 dlsym(elem->library_handle, "ste_omx_ens_component_register")) == NULL) {
	ALOGE("Failed to open library %s already register to STE ENS component loader: %s\n",
	     elem->library_name, dlerror());
	closeLibraryForElem(elem);
	return OMX_ErrorInsufficientResources;
    }

    struct ste_omx_ens_component_def_array * defs = NULL;

    // Get the definition once again to get the constructors addresses
    fptr(&defs);

    if(defs == NULL) {
	ALOGE("The registered library %s failed to provide its registration\n", elem->library_name);
	closeLibraryForElem(elem);
	return OMX_ErrorUndefined;
    }
    // Copy the defs to the allocated structure to update the functions pointers
    memcpy(elem->modules_components, defs, sizeof(struct ste_omx_ens_component_def_array));

    return OMX_ErrorNone;
}

OMX_ERRORTYPE STE_ENS_Loader::closeLibraryForElem(struct ens_cmp_list_elem * elem, bool check)
{
    if(elem->library_handle != NULL) {
	if (LIST_EMPTY(&elem->handle_list)) {
            if (closeHandles) {
                ALOGV("About to dlclose %s\n", elem->library_name);
                dlclose(elem->library_handle);
                elem->library_handle = NULL;
                ALOGV("Closed %s\n", elem->library_name);
            }
	} else {
	    if(check) {
		// Handle list is not NULL
		ALOGE("closeLibraryForElem called with handle list is not NULL for %s\n",
		     elem->library_name);
		return OMX_ErrorUndefined;
	    }
	}
    }
    return OMX_ErrorNone;
}
