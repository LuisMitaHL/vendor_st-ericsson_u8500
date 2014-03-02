/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 *
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef STE_CODECS_PLUGIN_H_

#define STE_CODECS_PLUGIN_H_

#include <OMXPluginBase.h>
#include <ste_omxil_interface.h>


namespace android {

struct STEOMXCodecsPlugin : public OMXPluginBase {
    STEOMXCodecsPlugin();

    virtual ~STEOMXCodecsPlugin();

    virtual OMX_ERRORTYPE makeComponentInstance(
            const char *name,
            const OMX_CALLBACKTYPE *callbacks,
            OMX_PTR appData,
            OMX_COMPONENTTYPE **component);

    virtual OMX_ERRORTYPE destroyComponentInstance(
            OMX_COMPONENTTYPE *component);

    virtual OMX_ERRORTYPE enumerateComponents(
            OMX_STRING name,
            size_t size,
            OMX_U32 index);

    virtual OMX_ERRORTYPE getRolesOfComponent(
            const char *name,
            Vector<String8> *roles);

private:
    STEOMXCodecsPlugin(const STEOMXCodecsPlugin &);
    STEOMXCodecsPlugin &operator=(const STEOMXCodecsPlugin &);
    void                *mHandle; /**< DL handle */
    OMXCoreInterface    *mInterface; /**< OMXcore interface */

};

}  // namespace android

#endif  // STE_CODECS_PLUGIN_H_
