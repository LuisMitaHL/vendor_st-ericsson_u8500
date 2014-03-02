/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2012. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   AFM_MpcHeapMgr.cpp
 * \brief
 * \author ST-Ericsson
 */
/*****************************************************************************/

#include "AFM_MpcHeapMgr.h"

AFM_MpcHeapMgr *     AFM_MpcHeapMgr::mpSgAFM_MpcHeapMgr = 0;
int                  AFM_MpcHeapMgr::mRefCount    = 0;
t_cm_memory_handle   AFM_MpcHeapMgr::mMemHdls[NB_MEM_TYPE] = {0, 0, 0, 0, 0, 0};
t_uint32             AFM_MpcHeapMgr::mMemHeapSizes[NB_MEM_TYPE] = {
    DEFAULT_SIA_HEAP_INT_MEM_X_SIZE,
    DEFAULT_SIA_HEAP_INT_MEM_Y_SIZE,
    DEFAULT_SIA_HEAP_ESRAM_MEM_16_SIZE,
    DEFAULT_SIA_HEAP_ESRAM_MEM_24_SIZE,
    DEFAULT_SIA_HEAP_EXT_MEM_16_SIZE,
    DEFAULT_SIA_HEAP_EXT_MEM_24_SIZE
};
t_cm_instance_handle AFM_MpcHeapMgr::mNmfHdl = 0;
t_cm_instance_handle AFM_MpcHeapMgr::mOstTrace = 0;
OMX_U32              AFM_MpcHeapMgr::mNMFDomainHandle = 0;
t_sint32             AFM_MpcHeapMgr::mSteSiaHeapDebug = 0;
const char*          AFM_MpcHeapMgr::mMemHeapName[NB_MEM_TYPE] = {
    "X_MEM",
    "Y_MEM",
    "ESRAM16",
    "ESRAM24",
    "SDRAM16",
    "SDRAM24"
};

const char*          AFM_MpcHeapMgr::mMemHeapPropName[NB_MEM_TYPE] = {
    SIA_HEAP_INT_MEM_X_SIZE,
    SIA_HEAP_INT_MEM_Y_SIZE,
    SIA_HEAP_ESRAM_MEM_16_SIZE,
    SIA_HEAP_ESRAM_MEM_24_SIZE,
    SIA_HEAP_EXT_MEM_16_SIZE,
    SIA_HEAP_EXT_MEM_24_SIZE
};

#define LOG_AND_EXIT_IF_FAIL(reason, error, no_error)  {    \
    if (error != no_error) {                                \
        HEAPMGR_LOG_LEVEL(SIA_HEAP_DEBUG_LEVEL_ERR, "AFM_MpcHeapMgr::AFM_MpcHeapMgr(): Error in %s [0x%x]\n", #reason, error); \
        goto err;                                           \
        }                                                   \
    }

AFM_MpcHeapMgr * AFM_MpcHeapMgr::getHandle(OMX_U32 NMFDomainHandle, t_cm_instance_handle ostTrace) {

    mOstTrace = ostTrace;

    {
        GET_PROPERTY(SIA_HEAP_DEBUG, value, "0");
        mSteSiaHeapDebug = atoi(value);
    }

    HEAPMGR_LOG_LEVEL(SIA_HEAP_DEBUG_LEVEL_DEBUG, "AFM_MpcHeapMgr::getHandle()\n");

    if (mRefCount++) {
        return mpSgAFM_MpcHeapMgr;
    }
    mpSgAFM_MpcHeapMgr = new AFM_MpcHeapMgr(NMFDomainHandle);

    return mpSgAFM_MpcHeapMgr;
}

void AFM_MpcHeapMgr::freeHandle() {

    HEAPMGR_LOG_LEVEL(SIA_HEAP_DEBUG_LEVEL_DEBUG, "AFM_MpcHeapMgr::freeHandle()\n");

    if (--mRefCount) {
        return;
    }

    delete mpSgAFM_MpcHeapMgr;
    mpSgAFM_MpcHeapMgr = static_cast<AFM_MpcHeapMgr*>(NULL);
}

AFM_MpcHeapMgr::AFM_MpcHeapMgr(OMX_U32 NMFDomainHandle) {

    HEAPMGR_LOG_LEVEL(SIA_HEAP_DEBUG_LEVEL_DEBUG, "AFM_MpcHeapMgr::AFM_MpcHeapMgr()\n");

    OMX_ERRORTYPE error;

    mNMFDomainHandle = NMFDomainHandle;
    CM_REGISTER_STUBS(libeffects_cpp);

    // NMF boring plumbing: create, start, ...
    error = ENS::instantiateNMFComponent(mNMFDomainHandle,
                                         "libeffects.mpc.libeffects",
                                         "libeffects",
                                         &mNmfHdl,
                                         NMF_SCHED_NORMAL);
    LOG_AND_EXIT_IF_FAIL(instantiateNMFComponent, error, OMX_ErrorNone);

    error = ENS::bindComponent(mNmfHdl, "osttrace", mOstTrace, "osttrace");
    LOG_AND_EXIT_IF_FAIL(bindComponentWithOstTrace, error, OMX_ErrorNone);

    error = ENS::startNMFComponent(mNmfHdl);
    LOG_AND_EXIT_IF_FAIL(startNMFComponent, error, OMX_ErrorNone);

    error = ENS::bindComponentFromHostEx(mNmfHdl,
                                         "init",
                                         &mIinit,
                                         5);
    LOG_AND_EXIT_IF_FAIL(bindComponentFromHostEx, error, OMX_ErrorNone);

    // get the heap sizes from environment variables,
    // if these env variables are defined,otherwise, stick
    // to the statically defined values!
    // note: these sizes are in mmdsp words!!

    for (unsigned int mem_idx = 0; mem_idx < NB_MEM_TYPE; mem_idx++)
    {
        char * default_val = "0";
        GET_PROPERTY(mMemHeapPropName[mem_idx], value, default_val);
        if (memcmp(value, default_val, strlen(default_val)))
        {
            mMemHeapSizes[mem_idx] = atoi(value);
        }
    }

    {
        t_uint32 pMpcAddresses[NB_MEM_TYPE];
        t_cm_mpc_memory_type pMpcMemType[NB_MEM_TYPE] = {
            CM_MM_MPC_TCM24_X,
            CM_MM_MPC_TCM24_Y,
            CM_MM_MPC_ESRAM16,
            CM_MM_MPC_ESRAM24,
            CM_MM_MPC_SDRAM16,
            CM_MM_MPC_SDRAM24};

        for (unsigned int mem_idx = 0; mem_idx < NB_MEM_TYPE; mem_idx++)
        {
          if (mMemHeapSizes[mem_idx])
          {
              // allocate the memory!
              error = ENS::allocMpcMemory(mNMFDomainHandle,
                                          pMpcMemType[mem_idx],
                                          mMemHeapSizes[mem_idx],
                                          CM_MM_ALIGN_2WORDS,
                                          &mMemHdls[mem_idx]);
              LOG_AND_EXIT_IF_FAIL(allocMpcMemory, error, OMX_ErrorNone);

              t_cm_error cm_error;
              cm_error = CM_GetMpcMemoryMpcAddress(mMemHdls[mem_idx],
                                                   &pMpcAddresses[mem_idx]);
              LOG_AND_EXIT_IF_FAIL(CM_GetMpcMemoryMpcAddress, cm_error, CM_OK);

              HEAPMGR_LOG_LEVEL(SIA_HEAP_DEBUG_LEVEL_DEBUG,
                                "AFM_MpcHeapMgr::AFM_MpcHeapMgr(): allocating %u words of mpc mem: %s, got mpc addr 0x%x\n",
                                mMemHeapSizes[mem_idx],
                                mMemHeapName[mem_idx],
                                (unsigned int)pMpcAddresses[mem_idx]);
          }
          else
          {
              HEAPMGR_LOG_LEVEL(SIA_HEAP_DEBUG_LEVEL_INFO,
                                "AFM_MpcHeapMgr::AFM_MpcHeapMgr(): skipping allocation of mem %mem: %s\n",
                                mMemHeapName[mem_idx]);
              pMpcAddresses[mem_idx] = 0;
          }
        }

        // then configure our heaps
        mIinit.minit_intmem((void*)pMpcAddresses[INT_X_MEM_TYPE],
                            mMemHeapSizes[INT_X_MEM_TYPE],
                            (void*)pMpcAddresses[INT_Y_MEM_TYPE],
                            mMemHeapSizes[INT_Y_MEM_TYPE]);

        mIinit.minit_extmem((void*)pMpcAddresses[INT_SDRAM24_MEM_TYPE],
                            mMemHeapSizes[INT_SDRAM24_MEM_TYPE],
                            (void*)pMpcAddresses[INT_SDRAM16_MEM_TYPE],
                            mMemHeapSizes[INT_SDRAM16_MEM_TYPE]);

        mIinit.minit_esram((void*)pMpcAddresses[INT_ESRAM24_MEM_TYPE],
                           mMemHeapSizes[INT_ESRAM24_MEM_TYPE],
                           (void*)pMpcAddresses[INT_ESRAM16_MEM_TYPE],
                           mMemHeapSizes[INT_ESRAM16_MEM_TYPE]);
    }

    return;
 err:
    delete mpSgAFM_MpcHeapMgr;
    mpSgAFM_MpcHeapMgr = static_cast<AFM_MpcHeapMgr*>(NULL);
}

AFM_MpcHeapMgr::~AFM_MpcHeapMgr() {

    OMX_ERRORTYPE error;

    HEAPMGR_LOG_LEVEL(SIA_HEAP_DEBUG_LEVEL_DEBUG, "AFM_MpcHeapMgr::~AFM_MpcHeapMgr()\n");

    // mark heaps as uninitialized on the dsp
    mIinit.release_heaps();

    for (unsigned int mem_idx = 0; mem_idx < NB_MEM_TYPE; mem_idx++)
    {
        if (mMemHdls[mem_idx])
        {
            HEAPMGR_LOG_LEVEL(SIA_HEAP_DEBUG_LEVEL_DEBUG,
                              "AFM_MpcHeapMgr::~AFM_MpcHeapMgr(): free %u in %s\n",
                              mMemHdls[mem_idx],
                              mMemHeapName[mem_idx]);

            // free the memory!
            error = ENS::freeMpcMemory(mMemHdls[mem_idx]);
            LOG_AND_EXIT_IF_FAIL(freeMpcMemory, error, OMX_ErrorNone);
            mMemHdls[mem_idx] = 0;
        }
    }

    // NMF boring plumbing again: unbind, stop, destroy, ...
    error = ENS::unbindComponent(mNmfHdl, "osttrace");
    LOG_AND_EXIT_IF_FAIL(unbindComponentOstTrace, error, OMX_ErrorNone);

    error = ENS::unbindComponentFromHost(&mIinit);
    LOG_AND_EXIT_IF_FAIL(unbindComponentFromHost, error, OMX_ErrorNone);

    error = ENS::stopNMFComponent(mNmfHdl);
    LOG_AND_EXIT_IF_FAIL(stopNMFComponent, error, OMX_ErrorNone);

    error = ENS::destroyNMFComponent(mNmfHdl);
    LOG_AND_EXIT_IF_FAIL(destroyNMFComponent, error, OMX_ErrorNone);

    mNmfHdl = 0;

    CM_UNREGISTER_STUBS(libeffects_cpp);

    return;

err:
    ALOGE("Error while deleting singleton AFM_MpcHeapMgr\n");
}
