/*****************************************************************************/
/**
*  © ST-Ericsson, 2009 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \file    ste_omxil_interface.cpp
* \brief   Define OMXIL Interface
* \author  ST-Ericsson
*/
/*****************************************************************************/

#include "ste_omxil_interface.h"
#include <dlfcn.h>

#define LOG_TAG "ste_omxil_interface"
#ifdef ANDROID
#include <utils/Log.h>
#define LOGE ALOGE
#define LOGI ALOGI
#else
#include <stdio.h>
#define LOGE(...) printf("E/" LOG_TAG ": " __VA_ARGS__)
#define LOGI(...) printf("I/" LOG_TAG ": " __VA_ARGS__)
#endif

#define CHECK_SYMBOL(var, symbol)					\
    do { if (var == NULL) { LOGE( "dlsym failed on " symbol "\n" ) ; } } while(0)

#ifdef ANDROID
#define OMX_CORE_LIBRARY "/system/lib/libomxil-bellagio.so"
#else
#define OMX_CORE_LIBRARY "/usr/lib/libomxil-bellagio.so"
#endif

class STEOMXInterface : public OMXCoreInterface
{
    public:

        ~STEOMXInterface()
        {
            if ((NULL != ipHandle) && (0 != dlclose(ipHandle)))
            {
                // dlclose() returns non-zero value if close failed, check for errors
                const char* pErr = dlerror();
                if (NULL != pErr)
                {
                    LOGE("Error closing library: %s\n", pErr);
                }
                else
                {
                    LOGE("Error closing library, no error reported\n");
                }
            } else {
		LOGI(OMX_CORE_LIBRARY " successfully closed\n");
	    }

            ipHandle = NULL;
        };

        static STEOMXInterface* Instance()
        {
            return (new STEOMXInterface());
        };

    private:
        // Handle to the OMX core library
        void* ipHandle;

        STEOMXInterface()
        {
            ipHandle = dlopen(OMX_CORE_LIBRARY, RTLD_NOW);

            if (NULL == ipHandle)
            {
                pOMX_Init = NULL;
                pOMX_Deinit = NULL;
                pOMX_ComponentNameEnum = NULL;
                pOMX_GetHandle = NULL;
                pOMX_FreeHandle = NULL;
                pOMX_GetComponentsOfRole = NULL;
                pOMX_GetRolesOfComponent = NULL;
                pOMX_SetupTunnel = NULL;
                pOMX_GetContentPipe = NULL;
                // check for errors
                const char* pErr = dlerror();
                if (NULL == pErr)
                {
                    // No error reported, but no handle to the library
                    LOGE("LoadLibrary: Error opening "
                         "library (%s) but no error reported\n", OMX_CORE_LIBRARY);
                }
                else
                {
                    // Error reported
                    LOGE("LoadLibrary: Error opening "
                         "library (%s): %s\n", OMX_CORE_LIBRARY, pErr);
                }
            }
            else
            {
                // Lookup all the symbols in the OMX core
                pOMX_Init = (tpOMX_Init)dlsym(ipHandle, "OMX_Init");
		CHECK_SYMBOL(pOMX_Init, "OMX_Init");
                pOMX_Deinit = (tpOMX_Deinit)dlsym(ipHandle, "OMX_Deinit");
		CHECK_SYMBOL(pOMX_Deinit, "OMX_Deinit");
                pOMX_ComponentNameEnum = (tpOMX_ComponentNameEnum)dlsym(ipHandle, "OMX_ComponentNameEnum");
		CHECK_SYMBOL(pOMX_ComponentNameEnum, "OMX_ComponentNameEnum");
                pOMX_GetHandle = (tpOMX_GetHandle)dlsym(ipHandle, "OMX_GetHandle");
		CHECK_SYMBOL(pOMX_GetHandle, "OMX_GetHandle");
                pOMX_FreeHandle = (tpOMX_FreeHandle)dlsym(ipHandle, "OMX_FreeHandle");
		CHECK_SYMBOL(pOMX_FreeHandle, "OMX_FreeHandle");
                pOMX_GetComponentsOfRole = (tpOMX_GetComponentsOfRole)dlsym(ipHandle, "OMX_GetComponentsOfRole");
		CHECK_SYMBOL(pOMX_GetComponentsOfRole, "OMX_GetComponentsOfRole");
                pOMX_GetRolesOfComponent = (tpOMX_GetRolesOfComponent)dlsym(ipHandle, "OMX_GetRolesOfComponent");
		CHECK_SYMBOL(pOMX_GetRolesOfComponent, "OMX_GetRolesOfComponent");
                pOMX_SetupTunnel = (tpOMX_SetupTunnel)dlsym(ipHandle, "OMX_SetupTunnel");
		CHECK_SYMBOL(pOMX_SetupTunnel, "OMX_SetupTunnel");
                pOMX_GetContentPipe = (tpOMX_GetContentPipe)dlsym(ipHandle, "OMX_GetContentPipe");
		CHECK_SYMBOL(pOMX_GetContentPipe, "OMX_GetContentPipe");
            }
        };

};


static OMXCoreInterface* pOMXCoreInterface = NULL;
static int nbClients = 0;

void OMXCoreInterface::release()
{
    nbClients--;
    if(nbClients == 0) {
	delete pOMXCoreInterface;
	pOMXCoreInterface = NULL;
    }
}

// function to obtain the interface object from the shared library
extern "C"
{
    OMXCoreInterface* GetOMXCoreInterface()
    {
	nbClients++;
        if (NULL == pOMXCoreInterface )
	    {
		pOMXCoreInterface = (OMXCoreInterface*)STEOMXInterface::Instance();
		return pOMXCoreInterface;
	}
	else
	{
		return pOMXCoreInterface;
	}
    }
}

