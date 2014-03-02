/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef _ASYNCFLASH_HPP_
#define _ASYNCFLASH_HPP_

#include <asyncflash_types.idt.h>
#include <flash_api.h>

class asyncflash: public asyncflashTemplate
{
    public:
        asyncflash();
        ~asyncflash();
        /* interface collection asyncflash.api.request */
        virtual void getSupportedFlashModes(void* pReqStruct);
        virtual void getFlashModeDetails(void* pReqStruct);
        virtual void enableFlashMode(void* pReqStruct);
        virtual void configureFlashMode(void* pReqStruct);
        virtual void strobe(void* pReqStruct);
        virtual void getStatus(void* pReqStruct);
        virtual void getXenonLifeCounter(void* pReqStruct);
        virtual void getSupportedSelfTests(void* pReqStruct);
        virtual void getFaultRegisters(void* pReqStruct);
        virtual void doSelfTest(void* pReqStruct);
        virtual void getNVM(void* pReqStruct);
        /* interface collection asyncflash.api.error */
        virtual const char* returnCode2String(t_uint32 aErr);

    private:
        CFlashDriver* pFlashDriver;
        static void FlashTest_cb(TFlashSelftest aResult, void* pContext);
        static void FlashNVM_cb(TFlashReturnCode aReturnCode, void* pContext);
};

#endif /* _ASYNCFLASH_HPP_ */
