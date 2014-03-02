/*****************************************************************************/
/**
*  © ST-Ericsson, 2009 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \file    ste_pv_omxil_bellagio.cpp
* \brief   Extends PV OMXIL Interface for Bellagio OMX Core
* \author  ST-Ericsson
*/
/*****************************************************************************/

#define LOG_TAG "ste_pv_omxil-bellagio"
#include <utils/Log.h>

#include <osclconfig.h>

#include <pvlogger.h>

#include <pv_omxcore.h>
#include <omx_interface.h>
#include "ste_omxil_interface.h"

class STEPVOMXInterface : public OMXInterface
{
    public:
        // Handle to the OMX core interface library
        OMXCoreInterface* mOMXCoreInterface;

        ~STEPVOMXInterface()
        {
	    mOMXCoreInterface->release();
        };

        OsclAny* SharedLibraryLookup(const OsclUuid& aInterfaceId)
        {
            // Make sure ipHandle is valid. If ipHandle is NULL, the dlopen
            // call failed.
            if (mOMXCoreInterface->GetpOMX_Init() && aInterfaceId == OMX_INTERFACE_ID) {
		// the library lookup was successful
                return this;
	    }
            // the ID doesn't match
            return NULL;
        };

        static STEPVOMXInterface* Instance()
        {
            return OSCL_NEW(STEPVOMXInterface, ());
        };

        bool UnloadWhenNotUsed(void)
        {
            // As of 9/22/08, the PV OMX core library can not be
            // safely unloaded and reloaded when the proxy interface
            // is enabled.
            return false;
        };

    private:

        STEPVOMXInterface()
        {
            mOMXCoreInterface = GetOMXCoreInterface();

	    pOMX_Init = mOMXCoreInterface->GetpOMX_Init();
	    pOMX_Deinit = mOMXCoreInterface->GetpOMX_Deinit();
	    pOMX_ComponentNameEnum = mOMXCoreInterface->GetpOMX_ComponentNameEnum();
	    pOMX_GetHandle = mOMXCoreInterface->GetpOMX_GetHandle();
	    pOMX_FreeHandle = mOMXCoreInterface->GetpOMX_FreeHandle();
	    pOMX_GetComponentsOfRole = mOMXCoreInterface->GetpOMX_GetComponentsOfRole();
	    pOMX_GetRolesOfComponent = mOMXCoreInterface->GetpOMX_GetRolesOfComponent();
	    pOMX_SetupTunnel = mOMXCoreInterface->GetpOMX_SetupTunnel();
	    pOMX_GetContentPipe = mOMXCoreInterface->GetpOMX_GetContentPipe();
	    pOMXConfigParser = NULL;
	}
};

// function to obtain the interface object from the shared library
extern "C"
{
    OSCL_EXPORT_REF OsclAny* PVGetInterface()
    {
        return STEPVOMXInterface::Instance();
    }
}

