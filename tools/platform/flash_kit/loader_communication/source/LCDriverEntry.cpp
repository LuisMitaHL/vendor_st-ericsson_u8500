/*******************************************************************************
 * Copyright (C) ST-Ericsson SA 2011
 * License terms: 3-clause BSD license
 ******************************************************************************/

#include "LCDriverMethods.h"
#include "Buffers.h"

void destroy(void);

#ifdef _WIN32
BOOL APIENTRY DllMain(HMODULE hModule,
                      DWORD  ul_reason_for_call,
                      LPVOID lpReserved)
{
    switch (ul_reason_for_call) {
    case DLL_PROCESS_ATTACH:
        break;

    case DLL_THREAD_ATTACH:
        break;

    case DLL_THREAD_DETACH:
        break;

    case DLL_PROCESS_DETACH:
        destroy();
        break;
    }

    return TRUE;
}
#else
void __attribute__((destructor)) DllMain(void)
{
    destroy();
}
#endif

//***********************************************************************************************
// Name:    destroy()
// Desc:    Destroy any remaining buffer data.
//**********************************************************************************************/
void destroy(void)
{
    Buffers::ReleaseAllBulkFiles();
    LcmInterface::CloseLCMLibrary();
}
