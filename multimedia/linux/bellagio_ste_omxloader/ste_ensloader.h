/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef _STE_ENSLOADER_H_
#define _STE_ENSLOADER_H_

/** Get the OMX types  */
#include <OMX_Core.h>
/** The structures definition the ENS components registers to this loader */
#include "ste_omx_ens_component_loader.h"
/** The BSD list header */
#include "BSD_list.h"
// Bellagio BOSA_COMPONENTLOADER types
#include "component_loader.h"

/* List elem to keep track of handles allocated for a givne module */
struct omx_handle_list_elem {
    /* For list management */
    LIST_ENTRY(omx_handle_list_elem) list_entry;
    /* Component handle */
    OMX_HANDLETYPE component_handle;
}; 

typedef LIST_HEAD(omx_handle_list_head, omx_handle_list_elem) omx_handle_list_head_t;

/* Structure of list element used as modules registry's */
struct ens_cmp_list_elem {
    /* For list management */
    LIST_ENTRY(ens_cmp_list_elem) list_entry;
    /* The components definition for this element */
    struct ste_omx_ens_component_def_array * modules_components;
    /* The shared library name for this element */
    char* library_name;
    /* The shared library handle for this element */
    void* library_handle;
    /* The handles of components instances created by this module */
    omx_handle_list_head_t handle_list;
};

typedef LIST_HEAD(ens_cmp_list_head, ens_cmp_list_elem) ens_cmp_list_head_t;

class STE_ENS_Loader {
    
 public:
    
    static OMX_ERRORTYPE STE_InitComponentLoader();
    static OMX_ERRORTYPE STE_DeInitComponentLoader();
    static OMX_ERRORTYPE STE_CreateComponent(OMX_HANDLETYPE * pHandle,
					     OMX_STRING cComponentName,
					     OMX_PTR pAppData, OMX_CALLBACKTYPE * pCallBacks);
    static OMX_ERRORTYPE STE_DestroyComponent(OMX_HANDLETYPE hComponent);
    static OMX_ERRORTYPE STE_ComponentNameEnum(OMX_STRING cComponentName, OMX_U32 nNameLength,
					       OMX_U32 nIndex);
    static OMX_ERRORTYPE STE_GetRolesOfComponent(OMX_STRING compName, OMX_U32 * pNumRoles,
						 OMX_U8 ** roles);
    static OMX_ERRORTYPE STE_GetComponentsOfRole(OMX_STRING role,
						 OMX_U32 * pNumComps, OMX_U8 ** compNames);

    /** The loader mutex to protect ens_cmp_list member from concurrent access */
    static pthread_mutex_t mLoaderMutex;

    static void Lock() { pthread_mutex_lock(&mLoaderMutex); }
    static void UnLock() { pthread_mutex_unlock(&mLoaderMutex); }

 private:
    static bool initDone;

    /** The list of ENS components registered to the loader */
    static ens_cmp_list_head_t ens_cmp_list;

    /** In case we are debugging allocations don't close the handles
     * to keep tack of .so mapping in the process memory */
    static bool closeHandles;

    /** Register the components */
    static OMX_ERRORTYPE registerComponents();

    /** Free the allocated resources */
    static OMX_ERRORTYPE freeComponentList();
    
    /** Return the first registry element providing this component name
     * Also set position to the index */
    static struct ens_cmp_list_elem * getRegistryForName(OMX_STRING cComponentName,
							 int * position);

    /** Return the registry element associated to the provided handle
     * returns also the handle list element matching this handle */
    static struct ens_cmp_list_elem * getRegistryForHandle(OMX_HANDLETYPE hComponent,
							   struct omx_handle_list_elem ** helem);
    
    /** Open the shared library of the given registry element
     * an error is retuned if library already opened */
    static OMX_ERRORTYPE openLibraryForElem(struct ens_cmp_list_elem *);
    
    /** Close the shared library if no more components handles using it
     * if check is true then error is raised if handle list is not free */
    static OMX_ERRORTYPE closeLibraryForElem(struct ens_cmp_list_elem *, bool check = true);
        
    /** Print the a registry entry */
    static void dumpRegistry(struct ens_cmp_list_elem * elem);

    /** Print the registry table */
    static void dumpRegistryTable();
};

#endif /* _STE_ENSLOADER_H_ */


