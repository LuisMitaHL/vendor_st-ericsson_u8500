/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 *
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include "STECodecsPlugin.h"
#include <dlfcn.h>

#define DBGT_DECLARE_AUTOVAR
#define DBGT_PREFIX "STECodecsPlugin"

#include "debug_trace.h"
#include "OMX_debug.h"


#ifndef STEOMXINT_SO_FILE
#define STEOMXINT_SO_FILE "/system/lib/libste_omxil-interface.so"
#endif


namespace android {
    OMXPluginBase * createOMXPlugin() {

    DBGT_TRACE_INIT(stecodecsplugin);
    DBGT_PROLOG();

    OMXPluginBase *pPluginBase = new STEOMXCodecsPlugin();

    DBGT_EPILOG();
    return pPluginBase;
}

STEOMXCodecsPlugin::STEOMXCodecsPlugin() {
    DBGT_PROLOG();

    OMX_ERRORTYPE err = OMX_ErrorNone;

    mHandle = dlopen(STEOMXINT_SO_FILE, RTLD_NOW);
    if ( NULL == mHandle ) {
        DBGT_CRITICAL("dlopen() failed %s", dlerror());
        goto dl_close;
    }

    DBGT_PINFO("dlopen passed");

    typedef OMXCoreInterface* (*pGetCoreInterface)();

    pGetCoreInterface temp;
    temp = (pGetCoreInterface)dlsym(mHandle, "GetOMXCoreInterface");
    mInterface  = (*temp)();
    if (mInterface == NULL) {
        DBGT_CRITICAL("dlsym(GetOMXCoreInterface) failed, err=%s", dlerror());
        goto dl_close;
    }

    err = (mInterface->GetpOMX_Init())();

    if (err != OMX_ErrorNone) {
        DBGT_CRITICAL("GetpOMX_Init failed, err=%s",
                      OMX_TYPE_TO_STR(OMX_ERRORTYPE, err));
        goto clean_interface;
    }

    DBGT_PINFO("GetpOMX_Init passed for interface=%p", mInterface);

    DBGT_EPILOG();
    return;

clean_interface:
    mInterface = NULL;
dl_close:
    dlclose(mHandle);
    mHandle = NULL ;

    DBGT_EPILOG("error, %s %s", mHandle?"":"mHandle=NULL",
                mInterface?"":"mInterface=NULL");
    return;

}

STEOMXCodecsPlugin::~STEOMXCodecsPlugin() {
    DBGT_PROLOG();
    OMX_ERRORTYPE err = OMX_ErrorNone;

    err = (mInterface->GetpOMX_Deinit())();
    if (err != OMX_ErrorNone) {
        DBGT_ERROR("GetpOMX_Deinit failed, err=%s",
                   OMX_TYPE_TO_STR(OMX_ERRORTYPE, err));
        goto error;
    }
    DBGT_PINFO("GetpOMX_Deinit passed for interface=%p", mInterface);

    if (NULL != mHandle) {
        dlclose(mHandle);
        mHandle = NULL;
    }
    DBGT_EPILOG();
    return;

error:
    DBGT_EPILOG("error");
    return;

}

OMX_ERRORTYPE STEOMXCodecsPlugin::makeComponentInstance(
    const char *name,
    const OMX_CALLBACKTYPE *callbacks,
    OMX_PTR appData,
    OMX_COMPONENTTYPE **component) {

    DBGT_PROLOG();

    OMX_ERRORTYPE err = (mInterface->GetpOMX_GetHandle())(
            reinterpret_cast<OMX_HANDLETYPE *>(component),
            const_cast<char *>(name),
            appData,
            const_cast<OMX_CALLBACKTYPE *>(callbacks));

    if (err) {
        DBGT_ERROR("failed for %s, err=%s" ,
                   name,
                   OMX_TYPE_TO_STR(OMX_ERRORTYPE, err));
        goto error;
    }

    DBGT_PINFO("%s: handle=%p", name, *component);

    DBGT_EPILOG("%s: handle=%p", name, *component);
    return OMX_ErrorNone;

error:
    DBGT_EPILOG("err=%s", OMX_TYPE_TO_STR(OMX_ERRORTYPE, err));
    return err;

}

OMX_ERRORTYPE STEOMXCodecsPlugin::destroyComponentInstance(
    OMX_COMPONENTTYPE *component) {

    DBGT_PROLOG("handle=%p", component);

    OMX_ERRORTYPE err = (mInterface->GetpOMX_FreeHandle())(component);
    if (err) {
        DBGT_ERROR("failed for handle=%p, err=%s" , component,
                   OMX_TYPE_TO_STR(OMX_ERRORTYPE, err));
        goto error;
    }

    DBGT_EPILOG();
    return OMX_ErrorNone;

error:
    DBGT_EPILOG("err=%s", OMX_TYPE_TO_STR(OMX_ERRORTYPE, err));
    return err;

}

OMX_ERRORTYPE STEOMXCodecsPlugin::enumerateComponents(
    OMX_STRING name,
    size_t size,
    OMX_U32 index) {

    DBGT_PROLOG("index=%lu", index);
    OMX_ERRORTYPE err = (mInterface->GetpOMX_ComponentNameEnum())(name, size, index);

    if (err == OMX_ErrorNoMore) {
        goto nomore;
    }

    if (err != OMX_ErrorNone) {
        DBGT_ERROR("GetpOMX_ComponentNameEnum failed, err=%s",
                   OMX_TYPE_TO_STR(OMX_ERRORTYPE, err));
        goto error;
    }

    DBGT_EPILOG("index=%lu: name=%s", index, name);
    return OMX_ErrorNone;

nomore:
    DBGT_EPILOG("no more");
    return OMX_ErrorNoMore;

error:
    DBGT_EPILOG("err=%s", OMX_TYPE_TO_STR(OMX_ERRORTYPE, err));
    return err;
}

OMX_ERRORTYPE STEOMXCodecsPlugin::getRolesOfComponent(
    const char *name,
    Vector<String8> *roles) {

    DBGT_PROLOG("%s", name);

    roles->clear();

    OMX_U32 numRoles;
    OMX_ERRORTYPE err = (mInterface->GetpOMX_GetRolesOfComponent())(
            const_cast<char *>(name),
            &numRoles,
            NULL);

    if (err != OMX_ErrorNone) {
        DBGT_CRITICAL("GetpOMX_GetRolesOfComponent failed, err=%s",
                      OMX_TYPE_TO_STR(OMX_ERRORTYPE, err));
        goto error;
    }

    if (numRoles > 0) {
        OMX_U8 **array = new OMX_U8 *[numRoles];
        for (OMX_U32 i = 0; i < numRoles; ++i) {
            array[i] = new OMX_U8[OMX_MAX_STRINGNAME_SIZE];
        }

        OMX_U32 numRoles2=numRoles;

        err = (mInterface->GetpOMX_GetRolesOfComponent())(
                const_cast<char *>(name), &numRoles2, array);

        DBGT_ASSERT(err == OMX_ErrorNone,
                    "GetpOMX_GetRolesOfComponent failed for %s, err=%s" , name,
                    OMX_TYPE_TO_STR(OMX_ERRORTYPE, err));
        DBGT_ASSERT(numRoles == numRoles2,
                    "GetpOMX_GetRolesOfComponent failed for %s, err=%s" , name,
                    OMX_TYPE_TO_STR(OMX_ERRORTYPE, err));

        for (OMX_U32 i = 0; i < numRoles; ++i) {
            String8 s((const char *)array[i]);
            roles->push(s);
            DBGT_PTRACE("%s: role=%s",
                        name, (const char *)array[i]);

            delete[] array[i];
            array[i] = NULL;
        }

        delete[] array;
        array = NULL;
    }

    DBGT_EPILOG();
    return OMX_ErrorNone;

error:
    DBGT_EPILOG("err=%s", OMX_TYPE_TO_STR(OMX_ERRORTYPE, err));
    return err;

}

}  // namespace android
