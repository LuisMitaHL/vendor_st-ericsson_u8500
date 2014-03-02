/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include "osi_trace.h"
#undef _CNAME_
#define _CNAME_ VFM

#include "VFM_DDepUtility.h"
#include "ENS_DBC.h"
#include "VFM_Memory.h"
#include "VFM_ParamAndConfig.h"
#include "mmhwbuffer.h"
#include "error_map.h"
#ifdef __SYMBIAN32__
#include <armnmf_dbc.h>
#endif

#ifdef __ARM_NEON_SWAP__
extern "C" IMPORT_C void swap_vector_64(unsigned long long *, unsigned long long *, int);
extern "C" IMPORT_C void swap_vector_128(unsigned long long *, unsigned long long *, int);
extern "C" IMPORT_C void swap_vector_256(unsigned long long *, unsigned long long *, int);
extern "C" IMPORT_C void swap_vector_256_lu(unsigned long long *, unsigned long long *, int);
#endif
// OST framework
#include "OstTraceDefinitions_ste.h"
#include "OpenSystemTrace_ste.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "video_vfm_src_VFM_DDepUtilityTraces.h"
#endif
// Traces
#ifdef PACKET_VIDEO_SUPPORT
#ifndef LOG_TAG
#define LOG_TAG "VFM"
#endif
#include<cutils/log.h>
#endif

// Define FORCE_NOREALLOC to force reallocation to fail
// #define FORCE_NOREALLOC

// Flagging a buffer to check it is an internal one
#define VFM_FLAG_INTERNALBUFFER  0x40000000

#define INTERNAL_TMP     -4
#define INTERNAL_SENT    -3
#define INTERNAL_FREE    -2
#define INTERNAL_CURRENT -1

#ifdef ANDROID
#define PRINTPG
#else
#define PRINTPG
#endif

// VFM_DDEP_SECURITY = nb of bytes of security, if anybody wants to add extra bytes after the copy
#define VFM_DDEP_SECURITY 16

inline void VFM_nmf_ddep::vfm_assert_static(int condition, int line, OMX_BOOL isFatal, int param1, int param2)
{
    if (!condition) {
        OstTraceInt3(TRACE_ERROR, "VFM: VFM_nmf_ddep: vfm_assert_static: VIDEOTRACE Error line %d,  param1=%d  param2=%d\n", line, param1, param2);
        if (isFatal) {
            DBC_ASSERT(0==1);
        }
    }
}

EXPORT_C VFM_SetDDep::VFM_SetDDep(int size, int isInputPort) :
        mSizeItem(size), mIsInputPort(isInputPort), mInCodec(0), mNotInCodec(0), mCtxtMemory(0),
        mCheckSignature(OMX_FALSE), mHeaderDataChanging(OMX_FALSE),ptrParamConfig(0)
{
}

EXPORT_C VFM_SetDDep::~VFM_SetDDep()
{
    free();
}

void VFM_SetDDep::free()
{
    // Nothing should be sent to the codec anymore, otherwise
    // that would mean the buffers have not been sent back to the application
    VFM_nmf_ddep::vfm_assert_static((mInCodec==0), __LINE__, OMX_TRUE);

    VFM_SetDDepItem *pItem, *pNext;
    for (pItem=mNotInCodec; pItem; pItem=pNext) {
        pNext = pItem->mNext;

        if (mSizeItem!=0) {
            VFM_Free(mCtxtMemory, (t_uint8 *)pItem->mSentToCodec);
        }

        if (mHeaderDataChanging) {
            // free is already done in function getBufferHeader() in this case
        } else {
            VFM_Free(mCtxtMemory, (t_uint8 *)pItem->mBufferHeader->pBuffer);
        }

        VFM_Free(mCtxtMemory, (t_uint8 *)pItem);
    }
    mNotInCodec = 0;
}

void VFM_SetDDep::TransferNotInToIn(VFM_SetDDepItem *pItem, VFM_SetDDepItem *pPrev)
{
    if (pPrev == 0) {
        mNotInCodec = pItem->mNext;
    } else {
        pPrev->mNext = pItem->mNext;
    }
    pItem->mNext = mInCodec;
    mInCodec = pItem;
}

void VFM_SetDDep::TransferInToNotIn(VFM_SetDDepItem *pItem, VFM_SetDDepItem *pPrev)
{
    if (pPrev == 0) {
        mInCodec = pItem->mNext;
    } else {
        pPrev->mNext = pItem->mNext;
    }
    pItem->mNext = mNotInCodec;
    mNotInCodec = pItem;
}

OMX_U32 VFM_SetDDep::ComputeSignature(OMX_PTR pt, OMX_U32 size)
{
    OMX_U32 res=0;
    for (OMX_U32 i=0; i<size; i++) {
        res += ((char *)pt)[i];
    }
    return res;
}

EXPORT_C OMX_PTR VFM_SetDDep::add(OMX_BUFFERHEADERTYPE *pBufferHeader, OMX_PTR pSentToCodec)
{
    VFM_SetDDepItem *current=mNotInCodec, *prev=0;
    VFM_nmf_ddep::vfm_assert_static(((pSentToCodec==0 && mSizeItem!=0) || (pSentToCodec!=0 && mSizeItem==0)), __LINE__, OMX_TRUE);
    while (current) {
        if (pBufferHeader == current->mBufferHeader) {
            if (pSentToCodec) {
                current->mSentToCodec = pSentToCodec;   // it may have changed after a flush
            } else {
                pSentToCodec = current->mSentToCodec;
            }

            TransferNotInToIn(current, prev);
            break;
        }
        prev = current;
        current = current->mNext;
    }
    if (current==0) {
        current = (VFM_SetDDepItem *)VFM_Alloc(mCtxtMemory, sizeof(VFM_SetDDepItem), VFM_MEM_NEW, 0, 0, __LINE__, (t_uint8 *)__FILE__);
        if (pSentToCodec == 0) {
            pSentToCodec = VFM_Alloc(mCtxtMemory, mSizeItem, VFM_MEM_NEW, 0, 0, __LINE__, (t_uint8 *)__FILE__);
        }

        current->mSentToCodec = pSentToCodec;
        current->mBufferHeader = pBufferHeader;
        current->mNext = mInCodec;
        mInCodec = current;

        if (!mHeaderDataChanging) {
            AddMemoryExternalSource(pBufferHeader);
        }
    }

    if (mHeaderDataChanging) {
        AddMemoryExternalSource(pBufferHeader);
    }

    if (mCheckSignature==OMX_TRUE) {
        current->mSignature = ComputeSignature(pBufferHeader->pBuffer+pBufferHeader->nOffset, pBufferHeader->nFilledLen);
    }

    return pSentToCodec;
}

EXPORT_C OMX_BUFFERHEADERTYPE *VFM_SetDDep::getBufferHeader(OMX_PTR pSentToCodec)
{
    for (VFM_SetDDepItem *current=mInCodec, *prev=0; current!=0; prev=current, current=current->mNext) {
        if (current->mSentToCodec == pSentToCodec) {
            TransferInToNotIn(current, prev);
            if (mCheckSignature==OMX_TRUE) {
                if (current->mSignature != ComputeSignature(current->mBufferHeader->pBuffer+current->mBufferHeader->nOffset, current->mBufferHeader->nFilledLen)) {
                    NMF_PANIC("Signature have changed line %d of file %s\n", __LINE__,  __FILE__);
                } else {
                    // NMF_LOG("Signature %d is correct\n", current->mSignature);
                }
            }

            if (mHeaderDataChanging) {
                VFM_Free(mCtxtMemory, (t_uint8 *)current->mBufferHeader->pBuffer);
            }

            return current->mBufferHeader;
        }
    }
    VFM_nmf_ddep::vfm_assert_static((0==1), __LINE__, OMX_TRUE);
    return 0;
}

void VFM_SetDDep::remove(OMX_BUFFERHEADERTYPE *pHeader)
{
    VFM_SetDDepItem *pItem, *pPrevious=0;
    for (pItem=mNotInCodec; pItem; pItem=pItem->mNext) {
        if (pItem->mBufferHeader == pHeader) {
            if (pPrevious) {
                pPrevious->mNext = pItem->mNext;
            } else {
                mNotInCodec = pItem->mNext;
            }

            if (mSizeItem!=0) {
                VFM_Free(mCtxtMemory, (t_uint8 *)pItem->mSentToCodec);
            }

            if (mHeaderDataChanging) {
                // free is already done in function getBufferHeader() in this case
            } else {
				OMX_BOOL isMetadata = OMX_FALSE;
				if(ptrParamConfig)
				{
					isMetadata = ((VFM_ParamAndConfig *)ptrParamConfig)->getStoreMetadataInBuffers();
				}

				if (!(isMetadata && mIsInputPort))
				{
                VFM_Free(mCtxtMemory, (t_uint8 *)pItem->mBufferHeader->pBuffer);
            }
            }

            VFM_Free(mCtxtMemory, (t_uint8 *)pItem);
            return;
        }
        pPrevious = pItem;
    }
}

EXPORT_C OMX_PTR VFM_SetDDep::FlushOneBuffer()
{
    if (mInCodec) {
        return mInCodec->mSentToCodec;
    } else {
        return 0;
    }
}

EXPORT_C void VFM_SetDDep::AddMemoryExternalSource(OMX_BUFFERHEADERTYPE *pBuffer)
{
    PRINTPG("**************** %s:%d *************",__func__,__LINE__);
    OMX_ERRORTYPE error;
    OMX_BOOL isMetadata = OMX_FALSE; //change for ER 423657
    MMHwBuffer *sharedChunk;
    if (mIsInputPort) {
        sharedChunk = (MMHwBuffer *)pBuffer->pInputPortPrivate;
        PRINTPG("**************** %s:%d InIndex : %0x *************",__func__,__LINE__,sharedChunk);
    } else {
        sharedChunk = (MMHwBuffer *)pBuffer->pOutputPortPrivate;
        PRINTPG("**************** %s:%d OutIndex : %0x *************",__func__,__LINE__,sharedChunk);
    }

    PRINTPG("**************** In VFM_SetDDep::AddMemoryExternalSourcc, Value of ptrParamConfig 0x%x mIsInputPort 0x%x ************* \n ",(OMX_U32)ptrParamConfig,(OMX_U32)mIsInputPort);

	/* Change for ER 4523657 */
	/* As we pass omxbufferheadertype while executing mmte_bellagio testcases so
	   input side buffers also need to be added to the list
	   Additional check introduced to know if metaData is enabled or not */
    if(ptrParamConfig)
        isMetadata = ((VFM_ParamAndConfig *)ptrParamConfig)->getStoreMetadataInBuffers();

    PRINTPG("VFM_SetDDep::AddMemoryExternalSource : isMetadata : %d",isMetadata);

    if ((!isMetadata)||(!(ptrParamConfig && mIsInputPort))) //Change for CR 399938
    {
    	VFM_nmf_ddep::vfm_assert_static((sharedChunk!=0), __LINE__, OMX_TRUE);

    	OMX_U32 PhysicalAddress;
    	error = sharedChunk->PhysAddress((OMX_U32)pBuffer->pBuffer, pBuffer->nAllocLen, PhysicalAddress);
    	VFM_nmf_ddep::vfm_assert_static((error==OMX_ErrorNone), __LINE__, OMX_TRUE, error);
    	VFM_AddMemoryExternalSource(mCtxtMemory, pBuffer->pBuffer, (t_uint8*)PhysicalAddress, pBuffer->nAllocLen, sharedChunk);
    	// NMF_LOG("VFM_SetDDep::AddMemoryExternalSource(): Log=%x-%x  Phys=%x-%x\n",
    	//        (OMX_U32)pBuffer->pBuffer,(OMX_U32)pBuffer->pBuffer+pBuffer->nAllocLen, PhysicalAddress, PhysicalAddress+pBuffer->nAllocLen);
	}
}


EXPORT_C int VFM_SetDDep::getCount(VFM_SetDDepItem *list) const
{
    if (!list) {
        return 0;
    } else {
        int nb;
        VFM_SetDDepItem *current;
        for (current=list, nb=0; current; current = current->mNext) {
            nb ++;
        }
        return nb;
    }
}

extern "C" EXPORT_C OMX_BOOL VFM_frameInfoDifferent(const t_common_frameinfo *pFrameInfo1, const t_common_frameinfo *pFrameInfo2)
{
    _VFM_CHECKDIFF_(pFrameInfo1->pic_width != pFrameInfo2->pic_width);
    _VFM_CHECKDIFF_(pFrameInfo1->pic_height != pFrameInfo2->pic_height);

    _VFM_CHECKDIFF_(pFrameInfo1->eProfile != pFrameInfo2->eProfile);
    _VFM_CHECKDIFF_(pFrameInfo1->eLevel != pFrameInfo2->eLevel);

    _VFM_CHECKDIFF_(pFrameInfo1->nPixelAspectRatioWidth != pFrameInfo2->nPixelAspectRatioWidth);
    _VFM_CHECKDIFF_(pFrameInfo1->nPixelAspectRatioHeight != pFrameInfo2->nPixelAspectRatioHeight);

    _VFM_CHECKDIFF_(pFrameInfo1->frame_cropping_flag != pFrameInfo2->frame_cropping_flag);
    if (pFrameInfo1->frame_cropping_flag) {
        _VFM_CHECKDIFF_(pFrameInfo1->frame_crop_right != pFrameInfo2->frame_crop_right);
        _VFM_CHECKDIFF_(pFrameInfo1->frame_crop_left != pFrameInfo2->frame_crop_left);
        _VFM_CHECKDIFF_(pFrameInfo1->frame_crop_top != pFrameInfo2->frame_crop_top);
        _VFM_CHECKDIFF_(pFrameInfo1->frame_crop_bottom != pFrameInfo2->frame_crop_bottom);
    }

    _VFM_CHECKDIFF_(pFrameInfo1->nSupportedExtension != pFrameInfo2->nSupportedExtension);
    _VFM_CHECKDIFF_(pFrameInfo1->nColorPrimary != pFrameInfo2->nColorPrimary);

    return OMX_FALSE;
}

EXPORT_C VFM_nmf_ddep::VFM_nmf_ddep(
        VFM_BufferMode inputBufferMode, VFM_BufferMode outputBufferMode, int nSizeInput, int nSizeOutput,
        Port *pPorts, OMX_BOOL manageDVFS) :
    mInputSet(nSizeInput, 1), mInputBufferMode(inputBufferMode),
    mOutputSet(nSizeOutput, 0), mOutputBufferMode(outputBufferMode),
    mCtxtMemory(0),
    mPerformance(100),
    mIsCodecFlushing(0), mMaskAckFlushing(0xFFFFFFFF),
    mPorts(pPorts),
    mNbDelayedCommand(0),
    mDomainDdrId(0xdeadbeef),
    mDomainEsramId(0xdeadbeef),
    mRequireFullDVFS(OMX_FALSE),
//+ ER335583
    mIsDVFSSet(OMX_FALSE),
    mRequireFullDDR(OMX_FALSE),
    mIsDDRSet(OMX_FALSE),
//- ER335583
    mPower(manageDVFS),
    mOutputBufferSwapEndianess(OMX_FALSE),
    //+Change for CR 399938
    ptrParamConfig(0)
    //-Change for CR 399938
{
	ptrOMXHandle = 0x0;
}

EXPORT_C VFM_nmf_ddep::~VFM_nmf_ddep()
{
    mInputSet.free();
    mOutputSet.free();
    if (mCtxtMemory) {
        VFM_CloseMemoryList(mCtxtMemory);
    }
}

EXPORT_C bool VFM_nmf_ddep::VFM_CheckStart()
{
    // IMPORTANT !!!!
    // For any new condition, do not forget to add "scheduleProcessEvent()" when the condition is disabled!
    // (check implementation of codec_end_flushing() for an example)

    // we can process if no current command is being run
    return (!mCurrentCommand.isValid());
}

EXPORT_C void VFM_nmf_ddep::sendCommand(OMX_COMMANDTYPE cmd, t_uword param)
{
    // Check a current command is already executing
    if (mCurrentCommand.isValid()) {
        VFM_nmf_ddep::vfm_assert_static((mNbDelayedCommand<mMaxDelayedCommands), __LINE__, OMX_TRUE, mNbDelayedCommand, mMaxDelayedCommands);
        mDelayedCommand[mNbDelayedCommand].set(cmd, param);
        mNbDelayedCommand++;
        return;
    }

    // New command to execute
    VFM_nmf_ddep::vfm_assert_static((mNbDelayedCommand==0), __LINE__, OMX_TRUE, mNbDelayedCommand);
    mCurrentCommand.set(cmd, param);
    executeCurrentCommand();
}


void VFM_nmf_ddep::executeCurrentCommand()
{
    int mask;
    OMX_COMMANDTYPE cmd = mCurrentCommand.getCommand();
    t_uword param = mCurrentCommand.getParam();

    // execute the command itself
    switch (cmd) {
    case OMX_CommandStateSet:
        switch (param) {
        case OMX_StateIdle:
            codec_start_flushing((1<<getPortCount())-1);  // we flush all the ports
            return;
        case OMX_StatePause:
            pause_command_to_codec();
            return;
        case OMX_StateExecuting:
            // Power management. But nothing else specific to do
            mPower.force();
            break;
        default:
            VFM_nmf_ddep::vfm_assert_static((0==1), __LINE__, OMX_TRUE, param);
            break;
        }
        break;
    case OMX_CommandPortDisable:
    case OMX_CommandFlush:
        if (param == OMX_ALL) {
            mask = (1<<getPortCount())-1;
        } else {
            mask = 1<<param;
        }
        codec_start_flushing(mask); // we flush the specified port
        return;
    case OMX_CommandPortEnable:
        // Power management. But nothing else specific to do
        mPower.force();
        break;
    default:
        break;
    }

    // command completion by calling the FSM part of the command, and by calling the
    // next delayed command if any.
    endCurrentCommand();
}

void VFM_nmf_ddep::endCurrentCommand()
{
    OMX_COMMANDTYPE cmd = mCurrentCommand.getCommand();
    t_uword param = mCurrentCommand.getParam();

    // if we are flushing, we must reset mInputSet and mOutputSet
    // as buffers may change
    if (    ((cmd==OMX_CommandStateSet) && (param==OMX_StateIdle)) ||
            (cmd==OMX_CommandPortDisable) ||
            (cmd==OMX_CommandFlush)) {
        mInputSet.free();
        mOutputSet.free();
    }

    // in case of disable, copy all input buffers into internal buffers
    // into internal buffers to treat them
    if (cmd==OMX_CommandPortDisable && ((param==0) || (param==OMX_ALL))) {
        if (mInputBufferMode & VFMDDEP_BUFFERMODE_COPY) {
            // please have a look at copyOneInput() for this part of the code
            VFM_DDepInternalBuffer *pInternal;

            while (mPorts[0].queuedBufferCount() != 0) {
                // Complete the copy, if possible
                pInternal = mInputInternal.getCurrent(OMX_FALSE);
                if (!pInternal) {
                    pInternal = mInputInternal.getFree(OMX_TRUE);
                }
                if (!pInternal) {
                    VFM_nmf_ddep::vfm_assert_static((0==1), __LINE__, OMX_FALSE);
                    break;
                } else {
                    pInternal->setStatus(INTERNAL_CURRENT);
                    copyInInternalBuffer(pInternal, 0, OMX_TRUE);
                }
            }
        }
    }

    // Power Management
    if ((cmd==OMX_CommandStateSet) && ((param==OMX_StateIdle) || (param==OMX_StatePause))) {
        mPower.cut();
    }

    // execute the fsm part of the command
    Component::sendCommand(cmd, param);

    // execute a delayed command if any, or reschedule a process
    if (mNbDelayedCommand) {
        mCurrentCommand = mDelayedCommand[0];
        mNbDelayedCommand--;
        for (int i=0; i<mNbDelayedCommand; i++) {
            mDelayedCommand[i] = mDelayedCommand[i+1];
        }
        executeCurrentCommand();
    } else {
        mCurrentCommand.reset();
        scheduleProcessEvent();     // run in order to treat the buffers that were stopped
    }
}

void VFM_nmf_ddep::codec_start_flushing(int nPortMask)
{
    VFM_nmf_ddep::vfm_assert_static((mIsCodecFlushing==0), __LINE__, OMX_TRUE, mIsCodecFlushing);
    mIsCodecFlushing = nPortMask;
    mMaskAckFlushing = 0;
    flush_command_to_codec();
}

EXPORT_C void VFM_nmf_ddep::codec_ack_flushing(int nPortIndex)
{
    mMaskAckFlushing |= (1 << nPortIndex);
    if (mMaskAckFlushing == ((1<<getPortCount())-1)) {
        // all ports send the acknowledgment
        codec_end_flushing();
    }
}

EXPORT_C void VFM_nmf_ddep::pause_command_to_codec()
{
    codec_ack_pause();
}

EXPORT_C void VFM_nmf_ddep::codec_ack_pause()
{
    VFM_nmf_ddep::vfm_assert_static((mCurrentCommand.isValid()), __LINE__, OMX_TRUE);
    OMX_COMMANDTYPE cmd = mCurrentCommand.getCommand();
    t_uword param = mCurrentCommand.getParam();

    VFM_nmf_ddep::vfm_assert_static(((cmd==OMX_CommandStateSet) && (param==OMX_StatePause)), __LINE__, OMX_TRUE, cmd, param);
    endCurrentCommand();
}

void VFM_nmf_ddep::codec_end_flushing()
{
    VFM_nmf_ddep::vfm_assert_static((mIsCodecFlushing && mMaskAckFlushing==((1<<getPortCount())-1)), __LINE__, OMX_TRUE);
    VFM_nmf_ddep::vfm_assert_static((mCurrentCommand.isValid()), __LINE__, OMX_TRUE);
    OMX_COMMANDTYPE cmd = mCurrentCommand.getCommand();
    t_uword param = mCurrentCommand.getParam();

    // Flush the internal buffers, if any
    for (int nPortIndex=0; nPortIndex<getPortCount(); nPortIndex++) {
        if (!(mIsCodecFlushing & (1<<nPortIndex))) {
            continue;
        }

        VFM_DDepAllInternalBuffers *pAllInternalBuffers;
        if (nPortIndex==0) {
            if (!(mInputBufferMode & VFMDDEP_BUFFERMODE_COPY)) {
                continue;
            }
            pAllInternalBuffers = &mInputInternal;
        } else {
            if (!(mOutputBufferMode & VFMDDEP_BUFFERMODE_COPY)) {
                continue;
            }
            pAllInternalBuffers = &mOutputInternal;
        }

        if (cmd==OMX_CommandStateSet && param==OMX_StateIdle) {
            pAllInternalBuffers->setAllFree();
        } else if (cmd==OMX_CommandPortDisable) {
            // nothing to be done
        } else if (cmd==OMX_CommandFlush) {
            pAllInternalBuffers->setAllFree();
        } else {
            VFM_nmf_ddep::vfm_assert_static((0==1), __LINE__, OMX_TRUE, cmd, param);
        }
    }

    // Flush is complete... just missing the FSM function to be called
    mIsCodecFlushing = 0;
    mMaskAckFlushing = 0xFFFFFFFF;
    endCurrentCommand();        // run the fsm part of the command and other scheduled commands
}

void VFM_nmf_ddep::initOMXHandles(OMX_PTR omxHandle)
{
	ptrOMXHandle = omxHandle;
	mInputInternal.ptrOpenMaxHandle = omxHandle;
	mOutputInternal.ptrOpenMaxHandle = omxHandle;

	for (int i=0;i<VFM_DDepAllInternalBuffers::mNbInternalBuffer;i++)
	{
		mInputInternal.mInternalBuffer[i].ptropenmaxHandle = omxHandle;
	}
}

EXPORT_C void VFM_nmf_ddep::initParamConfig(void *ptrParam)
{
	if (mCtxtMemory)
	{
#ifndef ANDROID
	ptrParam = 0;
#endif
		ptrParamConfig = ptrParam;
		mInputSet.setVFMParam(&(ptrParam));
		mOutputSet.setVFMParam(&(ptrParam));
		VFM_InitParamConfig(mCtxtMemory,ptrParam);
	}
	else
	{
		DBC_ASSERT(0==1);
	}
}

EXPORT_C bool VFM_nmf_ddep::initVFMMemory(int full_sw,OMX_PTR omxHandle)
{
    initOMXHandles(omxHandle);

    if (!mCtxtMemory) {
        VFM_nmf_ddep::vfm_assert_static(((mDomainDdrId != 0xdeadbeef) && (mDomainEsramId != 0xdeadbeef)), __LINE__, OMX_TRUE, mDomainDdrId, mDomainEsramId);
        mCtxtMemory = VFM_CreateMemoryList(mDomainDdrId, mDomainEsramId, full_sw);
    }
    if (!mCtxtMemory) {
        return false;
    } else {
		VFM_MemoryInit(mCtxtMemory,omxHandle);
        send_vfm_memory(mCtxtMemory);
    }

    mInputSet.setMemoryContext(mCtxtMemory);
    mOutputSet.setMemoryContext(mCtxtMemory);

    return true;
}

EXPORT_C void VFM_nmf_ddep::returnBufferDDep(int nPortIndex, OMX_BUFFERHEADERTYPE *pBuffer)
{
    // Assumption: buffers are coming back in reverse order!
    if (mIsCodecFlushing) {
        if (mIsCodecFlushing & (1<<nPortIndex)) {
            // we are flushing this port
            VFM_nmf_ddep::vfm_assert_static((mCurrentCommand.isValid()), __LINE__, OMX_TRUE);
            OMX_COMMANDTYPE cmd = mCurrentCommand.getCommand();
            t_uword param = mCurrentCommand.getParam();

            if (cmd==OMX_CommandStateSet && param==OMX_StateIdle) {
                // when going to idle, all the buffers must go back
                // of course, internal buffers should stay
                returnBufferToProxy(nPortIndex, pBuffer, OMX_FALSE);
            } else if (cmd==OMX_CommandPortDisable) {
                // when port disabling, one must save the buffers so that they can be treated again
                returnBufferForDisable(nPortIndex, pBuffer);
            } else if (cmd==OMX_CommandFlush) {
                // Output tunneled buffers should not be flushed. The other ones should go back
                returnBufferForFlush(nPortIndex, pBuffer);
            } else {
                VFM_nmf_ddep::vfm_assert_static((0==1), __LINE__, OMX_TRUE, cmd, param);
            }
        } else {
            // this port is not currently flushing ==> we have to reinject it!
            reinjectBuffer(nPortIndex, pBuffer);
        }
    } else {
        // we are not flushing this port
#ifdef __ndk5500_a0__
        //returnBufferToProxy(nPortIndex, pBuffer, OMX_TRUE);
        if (pBuffer->nFlags & OMX_BUFFERFLAG_EOS && pBuffer->nFilledLen==0)
            returnBufferToProxy(nPortIndex, pBuffer, OMX_FALSE);
        else
#endif
        returnBufferToProxy(nPortIndex, pBuffer, OMX_TRUE);
    }
}

void VFM_nmf_ddep::returnBufferToProxy(int nPortIndex, OMX_BUFFERHEADERTYPE *pBuffer, OMX_BOOL feedOutput)
{
    if (pBuffer->nFlags & VFM_FLAG_INTERNALBUFFER) {
        VFM_DDepInternalBuffer *pInternal = (VFM_DDepInternalBuffer *)pBuffer->pAppPrivate;
        if (nPortIndex==0) {
            // if internal buffer, we let it know it comes back
            pInternal->setIsBack();
        } else {
            pInternal->setStatus(mOutputInternal.getNextStatus());

            // Copy Case ==> copy buffers that came back from the codec into a port ...
            returnOutputBuffers(feedOutput);
        }
    } else {
	// otherwise it is not an internal buffer, that is sent back to the proxy
	if (isDispatching()) {
	    mPorts[nPortIndex].returnBuffer(pBuffer);
	} else {
	    returnBufferAsync(nPortIndex, pBuffer);
	}
    }

    // Power management: set standard power when the 1st frame is output
    if (nPortIndex==1) {
        mPower.regular();
    }
}

void VFM_nmf_ddep::returnBufferForFlush(int nPortIndex, OMX_BUFFERHEADERTYPE *pBuffer)
{
    if (pBuffer->nFlags & VFM_FLAG_INTERNALBUFFER) {
        // if internal buffer, we let it know it comes back
        VFM_DDepInternalBuffer *pInternal = (VFM_DDepInternalBuffer *)pBuffer->pAppPrivate;
        pInternal->setIsBack();
    } else {
        // we requeue the buffers
        // once requeued as we'll call endCurrentCommand() that calls Component::sendCommand(),
        // buffers that are queued may be sent back to the proxy if not an
        // output buffer that is tunneled
        mPorts[nPortIndex].requeueBuffer(pBuffer);
    }
}

void VFM_nmf_ddep::returnBufferForDisable(int nPortIndex, OMX_BUFFERHEADERTYPE *pBuffer)
{
    VFM_DDepInternalBuffer *pInternal=0;
    if (pBuffer->nFlags & VFM_FLAG_INTERNALBUFFER) {
        // if internal buffer, we let it know it comes back
        pInternal = (VFM_DDepInternalBuffer *)pBuffer->pAppPrivate;
    }

    // Saves input buffers if possible (that is copy is enabled!)
    // On output, nothing to be done
    if (nPortIndex==0) {
        if (pInternal) {
            // we have to reinject this buffer when needed
            mInputInternal.insertFront(pInternal);
        } else if (mInputBufferMode & VFMDDEP_BUFFERMODE_COPY) {
            VFM_DDepInternalBuffer *pFree = mInputInternal.getFree(OMX_TRUE);
            if (!pFree) {
                // Cannot force to get a free buffer
                VFM_nmf_ddep::vfm_assert_static((0==1), __LINE__, OMX_FALSE);
            } else {
                pFree->catBufferHeader(pBuffer, &mInputSet);
                if (pFree->isCorrupted()) {
                    pFree->setStatus(INTERNAL_FREE);
                } else {
                    mInputInternal.insertFront(pFree);
                }
            }
        } else {
            // copy in internal buffers is not possible as the option is not set
            // on the input buffer
            // no matter... we do not block the process!
        }
    } else if (pInternal) {
        pInternal->setIsBack(); // the output buffers are set back
    }

    // All non-internal buffers are sent back to the proxy, even in Pause
    if (!pInternal) {
        returnBufferAsync(nPortIndex, pBuffer);
    }
}

/**
 * Set variable that indicates if endianess should be swapped (>0) or not (0)
 *
 */
EXPORT_C void VFM_nmf_ddep::setOutputBufferSwapEndianess(t_uint32 endianess_swap)
{
#ifdef __ndk5500_a0__
    if (endianess_swap>0) mOutputBufferSwapEndianess = OMX_TRUE;
    else mOutputBufferSwapEndianess = OMX_FALSE;
#else
/* just to make sure that this is not enabled in anyway for 8500 */
	mOutputBufferSwapEndianess = OMX_FALSE;
#endif

}/**/

/**
  * Swap endianess of buffer (HVA output buffers are in BE <-> spec
revision)
  * Swap bytes in 64 bits words (B0B1...B7 -> B7B6...B0)
  */
void VFM_nmf_ddep::swap_endianess(t_uint8 *pDst, t_uint8 *pSrc, t_uint32
size)
{
#ifndef __ARM_NEON_SWAP__
     int i;
     t_uint32 *p_src, *p_dst, input_data1, input_data2;

     p_src = (t_uint32*)pSrc;
     p_dst = (t_uint32*)pDst;

     if (p_src == (t_uint32 *)0) {
        // NMF_LOG("%s: WARNING src==NULL\n", __func__);
         return;
     }

     for (i = (size>>3)-1; i >=0 ; i--) {
         input_data1 = *p_src;
         p_src++;
         input_data2 = *p_src;
         p_src++;

         *(p_dst) = ((((input_data2) >> 24) & 0x000000FF) | (((input_data2) >> 8) & 0x0000FF00) | (((input_data2) << 8) & 0x00FF0000) | (((input_data2) << 24) &  0xFF000000));
         p_dst++;
         *(p_dst) = ((((input_data1) >> 24) & 0x000000FF) | (((input_data1) >> 8) & 0x0000FF00) | (((input_data1) << 8) & 0x00FF0000) | (((input_data1) << 24) &  0xFF000000));
         p_dst++;
     }
#else
	 int num_double_words;
     unsigned long long *p_src, *p_dst;
     p_src = (unsigned long long*)pSrc;
     p_dst = (unsigned long long*)pDst;
     if (p_src == (unsigned long long *)0) {
        // NMF_LOG("%s: WARNING src == NULL \n", __func__);
         return;
     }//NMF_LOG("TO BE REMOVED: NIKHIL: NEON OPT CODE USED");
	 num_double_words = size >> 3;
	 // In one iteration, 16 double words are swapped
	 if (num_double_words >> 4)
		swap_vector_256_lu(p_dst, p_src, (num_double_words >> 4));
	 // We are left with atmost 15 more double words (8Bytes) to swap.
	 // In one iteration, 4 double words are swapped
	 if ((num_double_words & 0xF) >> 2)
		swap_vector_256_lu(p_dst, p_src, ((num_double_words & 0xF) >> 2));
	 // In one iteration, 2 double words are swapped
	 if ((num_double_words & 0x3) >> 1)
		swap_vector_256_lu(p_dst, p_src, ((num_double_words & 0x3) >> 1));
	 // In one iteration, 1 double word is swapped
	 if (num_double_words & 0x1)
		swap_vector_256_lu(p_dst, p_src, (num_double_words & 0x1));
#endif
}/*End of swap_endianess*/

#ifdef __ndk5500_a0__
void VFM_nmf_ddep::returnOutputBuffers(OMX_BOOL feedOutput)
{
    OMX_BUFFERHEADERTYPE *pBufferDst, *pBufferSrc;
    VFM_DDepInternalBuffer *pInternal;
    OMX_U32 size;

    pInternal = mOutputInternal.getCurrent(OMX_FALSE);
    if (!pInternal) {
        pInternal = mOutputInternal.getNextToSend(OMX_TRUE, INTERNAL_CURRENT);
    }
    if (pInternal) {
        pBufferSrc = pInternal->getBufferHeader();
    }

    while (pInternal && mPorts[1].queuedBufferCount()) {
	pBufferDst = mPorts[1].dequeueBuffer();
        size = pBufferSrc->nFilledLen;

	if (pBufferSrc->nFilledLen != 0) {
	    // Copy the data part
            if (size>pBufferSrc->nAllocLen) {
                size = pBufferSrc->nAllocLen;
            }

            if (mOutputBufferSwapEndianess == OMX_TRUE)
                swap_endianess(pBufferDst->pBuffer, pBufferSrc->pBuffer + pBufferSrc->nOffset, size);
            else
                memcpy(pBufferDst->pBuffer, pBufferSrc->pBuffer + pBufferSrc->nOffset, size);\
	}

        // Copy the header part
        // nSize
        // nVersion
        // pBuffer
        // nAllocLen
        pBufferDst->nFilledLen = size;
        pBufferDst->nOffset=0;
        // pAppPrivate
        // pPlatformPrivate
        // pInputPortPrivate
        // pOutputPortPrivate
        pBufferDst->hMarkTargetComponent = pBufferSrc->hMarkTargetComponent;
        pBufferDst->pMarkData = pBufferSrc->pMarkData;
        pBufferDst->nTickCount = pBufferSrc->nTickCount;
        pBufferDst->nTimeStamp = pBufferSrc->nTimeStamp;
        pBufferDst->nFlags = pBufferSrc->nFlags & (~VFM_FLAG_INTERNALBUFFER);
        if (size == pBufferSrc->nFilledLen) {
            pBufferDst->nFlags |= OMX_BUFFERFLAG_ENDOFFRAME;
        } else {
            pBufferDst->nFlags &= (~OMX_BUFFERFLAG_ENDOFFRAME);
        }
        // nOutputPortIndex
        // nInputPortIndex

	// return the buffer
        returnBufferToProxy(1, pBufferDst, feedOutput);

        if (size == pBufferSrc->nFilledLen) {
            // this internal buffer becomes free ==> we sent it back!
            pInternal->setStatus(INTERNAL_FREE);
            if (feedOutput) {
                pInternal->setStatus(INTERNAL_SENT);
                process_outputbuffer(pInternal->getBufferHeader(), OMX_TRUE);
            }

            pInternal = mOutputInternal.getNextToSend(OMX_TRUE, INTERNAL_CURRENT);
            if (pInternal) {
                pBufferSrc = pInternal->getBufferHeader();
            } else {
                pBufferSrc = 0;
            }
        } else {
            pBufferSrc->nOffset += size;
            pBufferSrc->nFilledLen -= size;
        }
    }
}

#else
void VFM_nmf_ddep::returnOutputBuffers(OMX_BOOL feedOutput)
{
    OMX_BUFFERHEADERTYPE *pBufferDst, *pBufferSrc;
    VFM_DDepInternalBuffer *pInternal;
    OMX_U32 size;

    pInternal = mOutputInternal.getCurrent(OMX_FALSE);
    if (!pInternal) {
        pInternal = mOutputInternal.getNextToSend(OMX_TRUE, INTERNAL_CURRENT);
    }
    if (pInternal) {
        pBufferSrc = pInternal->getBufferHeader();
    }

    while (pInternal && mPorts[1].queuedBufferCount()) {
        if (pBufferSrc->nFilledLen == 0) {
            size = pBufferSrc->nFilledLen;
        } else {
            // Copy the data part
            pBufferDst = mPorts[1].dequeueBuffer();
            size = pBufferSrc->nFilledLen;
            if (size>pBufferSrc->nAllocLen) {
                size = pBufferSrc->nAllocLen;
            }

            if (mOutputBufferSwapEndianess == OMX_TRUE)
            swap_endianess(pBufferDst->pBuffer, pBufferSrc->pBuffer + pBufferSrc->nOffset, size);
            else
            memcpy(pBufferDst->pBuffer, pBufferSrc->pBuffer + pBufferSrc->nOffset, size);

            // Copy the header part
            // nSize
            // nVersion
            // pBuffer
            // nAllocLen
            pBufferDst->nFilledLen = size;
            pBufferDst->nOffset=0;
            // pAppPrivate
            // pPlatformPrivate
            // pInputPortPrivate
            // pOutputPortPrivate
            pBufferDst->hMarkTargetComponent = pBufferSrc->hMarkTargetComponent;
            pBufferDst->pMarkData = pBufferSrc->pMarkData;
            pBufferDst->nTickCount = pBufferSrc->nTickCount;
            pBufferDst->nTimeStamp = pBufferSrc->nTimeStamp;
            pBufferDst->nFlags = pBufferSrc->nFlags & (~VFM_FLAG_INTERNALBUFFER);
            if (size == pBufferSrc->nFilledLen) {
                pBufferDst->nFlags |= OMX_BUFFERFLAG_ENDOFFRAME;
            } else {
                pBufferDst->nFlags &= (~OMX_BUFFERFLAG_ENDOFFRAME);
            }
            // nOutputPortIndex
            // nInputPortIndex

            // return the buffer
            returnBufferToProxy(1, pBufferDst, feedOutput);
        }

        if (size == pBufferSrc->nFilledLen) {
            // this internal buffer becomes free ==> we sent it back!
            pInternal->setStatus(INTERNAL_FREE);
            if (feedOutput) {
                pInternal->setStatus(INTERNAL_SENT);
                process_outputbuffer(pInternal->getBufferHeader(), OMX_TRUE);
            }

            pInternal = mOutputInternal.getNextToSend(OMX_TRUE, INTERNAL_CURRENT);
            if (pInternal) {
                pBufferSrc = pInternal->getBufferHeader();
            } else {
                pBufferSrc = NULL;
            }
        } else {
            pBufferSrc->nOffset += size;
            pBufferSrc->nFilledLen -= size;
        }
    }
}
#endif

void VFM_nmf_ddep::sendInternalOutputBuffers()
{
    VFM_DDepInternalBuffer *pInternal;
    while (0 != (pInternal = mOutputInternal.getFree(OMX_FALSE))) {
        pInternal->setStatus(INTERNAL_SENT);
        process_outputbuffer(pInternal->getBufferHeader(), OMX_TRUE);
    }
}

void VFM_nmf_ddep::reinjectBuffer(int nPortIndex, OMX_BUFFERHEADERTYPE *pBuffer)
{
    VFM_DDepInternalBuffer *pInternal=0;
    if (pBuffer->nFlags & VFM_FLAG_INTERNALBUFFER) {
        // if internal buffer, we let it know it comes back
        pInternal = (VFM_DDepInternalBuffer *)pBuffer->pAppPrivate;
    }

    // Saves input buffers if possible (that is copy is enabled!)
    switch (nPortIndex) {
    case 0:
        if (pInternal) {
            // we have to reinject this buffer when needed
            mInputInternal.insertFront(pInternal);
        } else if (mInputBufferMode & VFMDDEP_BUFFERMODE_COPY) {
            VFM_DDepInternalBuffer *pFree = mInputInternal.getFree(OMX_TRUE);
            if (!pFree) {
                // Cannot force to get a free buffer
                VFM_nmf_ddep::vfm_assert_static((0==1), __LINE__, OMX_FALSE);
            } else {
                pFree->catBufferHeader(pBuffer, &mInputSet);
                if (pFree->isCorrupted()) {
                    pFree->setStatus(INTERNAL_FREE);
                } else {
                    mInputInternal.insertFront(pFree);
                }
            }
            returnBufferToProxy(nPortIndex, pBuffer, OMX_TRUE);
        } else {
            mPorts[nPortIndex].requeueBuffer(pBuffer);
        }
        break;
    case 1:
        if (pInternal) {
            pInternal->setIsBack(); // the output buffers are set back
        } else {
            mPorts[nPortIndex].requeueBuffer(pBuffer);
        }
        break;
    default:
        VFM_nmf_ddep::vfm_assert_static((0==1), __LINE__, OMX_TRUE);
        break;
    }
}

EXPORT_C void VFM_nmf_ddep::setTunnelStatus(t_sint16 portIdx, t_uint16 isTunneled)
{
    if (portIdx == -1) {
        for (int i=0; i<2; i++) {
            if (isTunneled & (1<<i)) {
                mPorts[i].setTunnelStatus(true);
            } else {
                mPorts[i].setTunnelStatus(false);
            }
        }
    } else {
        if (isTunneled & (1<<portIdx)) {
            mPorts[portIdx].setTunnelStatus(true);
        } else {
            mPorts[portIdx].setTunnelStatus(false);
        }
    }
}


EXPORT_C void VFM_nmf_ddep::ost_port_status_flow()
{
    OstTraceFiltInst2(TRACE_FLOW, "VFM: VFM_nmf_ddep: ost_port_status_flow: Input   Port / %02d (Queued in DDep) / %02d (SentToCodec)\n", mPorts[0].queuedBufferCount(), mInputSet.getCountInCodec());
    OstTraceFiltInst2(TRACE_FLOW, "VFM: VFM_nmf_ddep: ost_port_status_flow: Output  Port / %02d (Queued in DDep) / %02d (SentToCodec)\n", mPorts[1].queuedBufferCount(), mOutputSet.getCountInCodec());
}

EXPORT_C void VFM_nmf_ddep::ost_port_status_debug()
{
    OstTraceFiltInst2(TRACE_DEBUG, "VFM: VFM_nmf_ddep: ost_port_status_flow: Input   Port / %02d (Queued in DDep) / %02d (SentToCodec)\n", mPorts[0].queuedBufferCount(), mInputSet.getCountInCodec());
    OstTraceFiltInst2(TRACE_DEBUG, "VFM: VFM_nmf_ddep: ost_port_status_flow: Output  Port / %02d (Queued in DDep) / %02d (SentToCodec)\n", mPorts[1].queuedBufferCount(), mOutputSet.getCountInCodec());
}

EXPORT_C void VFM_nmf_ddep::nmflog_port_status()
{
   OstTraceFiltInst2(TRACE_DEBUG, "VFM: VFM_nmf_ddep: ost_port_status_flow: Input   Port / %02d (Queued in DDep) / %02d (SentToCodec)\n", mPorts[0].queuedBufferCount(), mInputSet.getCountInCodec());
   OstTraceFiltInst2(TRACE_DEBUG, "VFM: VFM_nmf_ddep: ost_port_status_flow: Output  Port / %02d (Queued in DDep) / %02d (SentToCodec)\n", mPorts[1].queuedBufferCount(), mOutputSet.getCountInCodec());
}

EXPORT_C void VFM_nmf_ddep::dump_port_status(const char *string, int nPortIndex)
{
    NMF_LOG("%s: %s Port / %02d (Queued in DDep) / %02d (SentToCodec)\n",
        string,
        (nPortIndex==0 ? "Input" : "Output"),
        mPorts[nPortIndex].queuedBufferCount(),
        (nPortIndex==0 ? mInputSet : mOutputSet) . getCountInCodec()
    );
}

EXPORT_C void VFM_nmf_ddep::raiseEventErrorMap(Common_Error_Map *pErrorMap)
{
    if (pErrorMap && pErrorMap->tryLockMutex() == OMX_ErrorNone) {
        if (pErrorMap->mIsAvailable && pErrorMap->mEventToSent) {
            pErrorMap->mEventToSent = 0;
            sendProxyEvent(OMX_EventError, 1, (OMX_U32)OMX_ErrorMbErrorsInFrame);
        }
        pErrorMap->releaseMutex();
    }
}

EXPORT_C void VFM_nmf_ddep::process_inputbuffer(OMX_BUFFERHEADERTYPE *pBuffer, OMX_BOOL isInternal)
{
    // if used, a specific implementation must be defined in the ddep of the codec
    VFM_nmf_ddep::vfm_assert_static((0==1), __LINE__, OMX_TRUE);
}

EXPORT_C void VFM_nmf_ddep::process_special_inputbuffer(OMX_BUFFERHEADERTYPE *pBuffer)
{
    // if used, a specific implementation must be defined in the ddep of the codec
    VFM_nmf_ddep::vfm_assert_static((0==1), __LINE__, OMX_TRUE);
}

EXPORT_C void VFM_nmf_ddep::sendAllOutput()
{
    if ((mOutputBufferMode & VFMDDEP_BUFFERMODE_FASTANDCOPY) == VFMDDEP_BUFFERMODE_FASTANDCOPY) {
        // we need to know if we'll always make the fast path or if we'll always make the copy
        // this is a current limitation of the internal output buffer
        OMX_BUFFERHEADERTYPE *pBuffer;
        if (!mPorts[1].queuedBufferCount()) {
            return;
        } else {
            pBuffer = mPorts[1].getBuffer(0);
        }
        if (activateOutputFastPath(pBuffer)) {
            mOutputBufferMode = VFMDDEP_BUFFERMODE_FASTPATH;
        } else {
            mOutputBufferMode = VFMDDEP_BUFFERMODE_COPY;
        }
    }

    if (mOutputBufferMode & VFMDDEP_BUFFERMODE_FASTPATH) {
        // FASTPATH Case ==> send each buffer to the process
        while (mPorts[1].queuedBufferCount()) {
            OMX_BUFFERHEADERTYPE *pBuffer = mPorts[1].dequeueBuffer();
            process_outputbuffer(pBuffer, OMX_FALSE);
        }
    } else {
        // Copy Case ==> copy buffers that came back from the codec into a port ...
        returnOutputBuffers(OMX_TRUE);
        // ... and then send each internal buffer to the process
        sendInternalOutputBuffers();
    }
}

EXPORT_C void VFM_nmf_ddep::process_outputbuffer(OMX_BUFFERHEADERTYPE *pBuffer, OMX_BOOL isInternal)
{
    // if used, a specific implementation must be defined in the ddep of the codec
    VFM_nmf_ddep::vfm_assert_static((0==1), __LINE__, OMX_TRUE);
}

EXPORT_C void VFM_nmf_ddep::process_special_outputbuffer(OMX_BUFFERHEADERTYPE *pBuffer)
{
    // if used, a specific implementation must be defined in the ddep of the codec
    VFM_nmf_ddep::vfm_assert_static((0==1), __LINE__, OMX_TRUE);
}

EXPORT_C void VFM_nmf_ddep::sendOneInput()
{
    VFM_DDepInternalBuffer *pInternal;
    OMX_BUFFERHEADERTYPE *pBuffer;

    // do we have an internal buffer to send?
    if (mInputBufferMode & VFMDDEP_BUFFERMODE_COPY) {
        pInternal = mInputInternal.getNextToSend(OMX_TRUE, INTERNAL_SENT);
        if (pInternal) {
            process_inputbuffer(pInternal->getBufferHeader(), OMX_TRUE);
            return;
        }
    }

    // new buffer to proceed to send to the codec?
    if (mPorts[0].queuedBufferCount() == 0) {
        return;
    }
    pBuffer = mPorts[0].getBuffer(0);

    // check we started a copy in internal buffer ==> copy is mandatory
    if (mInputBufferMode & VFMDDEP_BUFFERMODE_COPY) {
        pInternal = mInputInternal.getCurrent(OMX_FALSE);
        if (pInternal) {
            copyInputAndSend(pInternal);
            return;
        }
    }

    // check for special case
    if (isSpecialCase(pBuffer)) {
        process_special_inputbuffer(pBuffer);
        return;
    }

    // buffer on the port that could be sent using fast path ?
    if (activateInputFastPath(pBuffer)) {
        process_inputbuffer(mPorts[0].dequeueBuffer(), OMX_FALSE);
        return;
    }

    // we could copy the buffers, and may be send the last one
    if (mInputBufferMode & VFMDDEP_BUFFERMODE_COPY) {
        pInternal = mInputInternal.getCurrent(OMX_TRUE);
        if (pInternal) {
            copyInputAndSend(pInternal);
            return;
        }
    }
}

EXPORT_C OMX_BOOL VFM_nmf_ddep::copyOneInput()
{
    VFM_DDepInternalBuffer *pInternal;
    OMX_BUFFERHEADERTYPE *pBuffer;

    if (!(mInputBufferMode & VFMDDEP_BUFFERMODE_COPY)) {
        return OMX_FALSE;
    }

    pInternal = mInputInternal.getCurrent(OMX_FALSE);
    pBuffer = mPorts[0].getBuffer(0);
    if (pInternal) {
        // if we have a current internal buffers, we should continue with this one
        copyInInternalBuffer(pInternal, 0, OMX_FALSE);
    } else if (isSpecialCase(pBuffer) || activateInputFastPath(pBuffer)) {
        // is the lastest-in is will be "fastpath"
        // if so, we cannot do anything else
        return OMX_FALSE;
    } else {
        pInternal = mInputInternal.getCurrent(OMX_TRUE);
        if (pInternal) {
            copyInInternalBuffer(pInternal, 0, OMX_FALSE);
        } else {
            return OMX_FALSE;
        }
    }
    return OMX_TRUE;
}

EXPORT_C void VFM_nmf_ddep::copyInputAndSend(VFM_DDepInternalBuffer *pInternal)
{
    VFM_DDepInternalBuffer *pToSend;
    copyInInternalBuffer(pInternal, 0, OMX_FALSE);
    t_sint8 status = pInternal->getStatus();
    if (status==0) {
        // it is ready to send
        pToSend = mInputInternal.getNextToSend(OMX_TRUE, INTERNAL_SENT);
        process_inputbuffer(pToSend->getBufferHeader(), OMX_TRUE);
    } else if (status==INTERNAL_FREE) {
        copyInputAndSend(mInputInternal.getCurrent(OMX_TRUE));    // send again. It could failed because of realloc issue
    }
}

EXPORT_C OMX_BOOL VFM_nmf_ddep::activateInputFastPath(OMX_BUFFERHEADERTYPE *pBuffer) const
{
    // Do we forbid internal copy?
    if (!(mInputBufferMode&VFMDDEP_BUFFERMODE_COPY)) {
        VFM_nmf_ddep::vfm_assert_static((pBuffer->pInputPortPrivate!=0), __LINE__, OMX_TRUE);
        return OMX_TRUE;
    }

    // Do we have the FastPath option?
    if (!(mInputBufferMode & VFMDDEP_BUFFERMODE_FASTPATH)) {
        return OMX_FALSE;
    }

    // Do we have MMHwBuffer informations?
    if (!pBuffer->pInputPortPrivate) {
        return OMX_FALSE;
    }

    // Is this buffer complete
    if (!VFM_DDepInternalBuffer::isLastBuffer(pBuffer->nFlags)) {
        return OMX_FALSE;
    }

    return specificActivateInputFastPath(pBuffer);
}

EXPORT_C OMX_BOOL VFM_nmf_ddep::activateOutputFastPath(OMX_BUFFERHEADERTYPE *pBuffer)
{
    // Do we forbid internal copy?
    if (!(mOutputBufferMode&VFMDDEP_BUFFERMODE_COPY)) {
        VFM_nmf_ddep::vfm_assert_static((pBuffer->pOutputPortPrivate!=0), __LINE__, OMX_TRUE);
        return OMX_TRUE;
    }

    // Do we have the FastPath option?
    if (!(mOutputBufferMode & VFMDDEP_BUFFERMODE_FASTPATH)) {
        return OMX_FALSE;
    }

    // Do we have MMHwBuffer informations?
    if (!pBuffer->pOutputPortPrivate) {
        return OMX_FALSE;
    }

    // Is this buffer sufficiently large
    VFM_DDepInternalBuffer *pInternal = mOutputInternal.getFree(OMX_FALSE);
    VFM_nmf_ddep::vfm_assert_static((pInternal!=0), __LINE__, OMX_TRUE);
    if (pBuffer->nAllocLen < pInternal->getBufferHeader()->nAllocLen) {
        return OMX_FALSE;
    }

    return specificActivateOutputFastPath(pBuffer);
}

EXPORT_C OMX_BOOL VFM_nmf_ddep::isSpecialCase(OMX_BUFFERHEADERTYPE *pBuffer) const
{
    return OMX_FALSE;
}

EXPORT_C OMX_BOOL VFM_nmf_ddep::specificActivateInputFastPath(OMX_BUFFERHEADERTYPE *pBuffer) const
{
    return OMX_TRUE;
}

EXPORT_C OMX_BOOL VFM_nmf_ddep::specificActivateOutputFastPath(OMX_BUFFERHEADERTYPE *pBuffer) const
{
    return OMX_TRUE;
}

EXPORT_C void VFM_nmf_ddep::setDVFSValue(OMX_BOOL req)
{
    if (req == mRequireFullDVFS) {
        return;
    }

    if (mIsDVFSSet) {
        unsetDVFS();
        mRequireFullDVFS = req;
        setDVFS();
    } else {
        mRequireFullDVFS = req;
    }
}

EXPORT_C void VFM_nmf_ddep::setDVFS()
{
    if (mRequireFullDVFS && !mIsDVFSSet) {
        // NMF_LOG("Before setFullDVFS()\n");
        VFM_PowerManagement::setFullDVFS(this);
        // NMF_LOG("After setFullDVFS()\n");
    }
    mIsDVFSSet = OMX_TRUE;
}

EXPORT_C void VFM_nmf_ddep::unsetDVFS()
{
    if (mRequireFullDVFS && mIsDVFSSet) {
        // NMF_LOG("Before releaseFullDVFS()\n");
        VFM_PowerManagement::releaseFullDVFS(this);
        // NMF_LOG("After releaseFullDVFS()\n");
    }
    mIsDVFSSet = OMX_FALSE;
}

//+ ER335583
EXPORT_C void VFM_nmf_ddep::setDDRValue(OMX_BOOL req)
{
    if (req == mRequireFullDDR) {
        return;
    }

    if (mIsDDRSet) {
        unsetDDR();
        mRequireFullDDR = req;
        setDDR();
    } else {
        mRequireFullDDR = req;
    }
}

EXPORT_C void VFM_nmf_ddep::setDDR()
{
    if (mRequireFullDDR && !mIsDDRSet) {
        VFM_PowerManagement::setFullDDR(this);
    }
    mIsDDRSet = OMX_TRUE;
}

EXPORT_C void VFM_nmf_ddep::unsetDDR()
{
    if (mRequireFullDDR && mIsDDRSet) {
        VFM_PowerManagement::releaseFullDDR(this);
    }
    mIsDDRSet = OMX_FALSE;
}
//- ER335583

EXPORT_C VFM_HostPerformances::VFM_HostPerformances(OMX_U32 nDisplayWindows)
{
    reset(nDisplayWindows);
}


EXPORT_C void VFM_HostPerformances::reset(OMX_U32 nDisplayWindows)
{
    mNbInDDep[0] = mNbInDDep[1] = 0;
    mNbTotal[0] = mNbTotal[1] = 0;
    mNbAdd = mNbFrames = 0;
    mInitTime = mLastTime = 0;
    mDisplayWindows = nDisplayWindows;
}

EXPORT_C void VFM_HostPerformances::add(OMX_U32 in0, OMX_U32 total0, OMX_U32 in1, OMX_U32 total1, OMX_BOOL new_frame)
{
    if (mDisplayWindows==0) {
        return;     // no statistics to be computed
    }

    mNbInDDep[0]  += in0;
    mNbInDDep[1]  += in1;
    if (mNbTotal[0] < total0) {
        mNbTotal[0] = total0;
    }
    if (mNbTotal[1] < total1) {
        mNbTotal[1] = total1;
    }
    mNbAdd ++;

    if (new_frame) {
        mNbFrames ++;
    }
}

#define TIMER_PERIOD 1E-6
EXPORT_C void VFM_HostPerformances::display(void)
{
    if (mDisplayWindows==0) {
        return;     // no statistics to be computed
    }
    if (mNbFrames == 1) {
        mInitTime = mLastTime = getTime();
        return;
    }
    if (mNbFrames%mDisplayWindows == 0) {
        OMX_U32 time = getTime();
        NMF_LOG("VFM_HostPerformances: %u output frames  --  Last %d frames: %.2ffps  --  Total: %.2ffps\n",
                (unsigned int) mNbFrames, (int) mDisplayWindows, getFPS(mLastTime, time, mDisplayWindows), getFPS(mInitTime, time, mNbFrames));
        mLastTime = time;
        NMF_LOG("| : Input  port (%02d buffers): %.2f buffers in codec (average)\n",
               (int) mNbTotal[0], ((float)mNbInDDep[0])/mNbAdd);
        NMF_LOG("| : Output port (%02d buffers): %.2f buffers in codec (average)\n",
                (int) mNbTotal[1], ((float)mNbInDDep[1])/mNbAdd);
    }
}

double VFM_HostPerformances::getFPS(OMX_U32 time1, OMX_U32 time2, OMX_U32 nFrames)
{
    return nFrames / ((time2-time1)*TIMER_PERIOD);
}

#ifdef __SYMBIAN32__
#include <e32base.h>
OMX_U32 VFM_HostPerformances::getTime(void)
{
    return User::NTickCount()*1000;
}
#else
#include "los/api/los_api.h"
OMX_U32 VFM_HostPerformances::getTime(void)
{
    return LOS_getSystemTime();
}
#endif

EXPORT_C VFM_DDepInternalBuffer::VFM_DDepInternalBuffer() :
        mChunk(0),
        mStatus(INTERNAL_FREE),
        mIsCorrupted(OMX_FALSE),
        ptropenmaxHandle(0)
{
    memset(&mBufferHeader, 0, sizeof(mBufferHeader));
}

EXPORT_C VFM_DDepInternalBuffer::~VFM_DDepInternalBuffer()
{
    free();
}

EXPORT_C VFM_DDepAllInternalBuffers::VFM_DDepAllInternalBuffers(): mNb(0)
{
    ptrOpenMaxHandle = 0x0;
}

EXPORT_C OMX_ERRORTYPE VFM_DDepAllInternalBuffers::alloc(VFM_SetDDep *pSet, t_uint32 nb, t_uint32 size, OMX_BOOL isCached)
{
    VFM_nmf_ddep::vfm_assert_static((mNb==0 || mNb==nb), __LINE__, OMX_TRUE);
    VFM_nmf_ddep::vfm_assert_static((nb<=mNbInternalBuffer), __LINE__, OMX_TRUE, nb, mNbInternalBuffer);

    mNb = nb;

    // Realloc all the buffers
    for (int i=0; i<mNb; i++) {
        OMX_ERRORTYPE error = mInternalBuffer[i].alloc(size, isCached);
        if (error != OMX_ErrorNone) {
            // Allocation failed
            VFM_nmf_ddep::vfm_assert_static((0==1), __LINE__, OMX_FALSE);
            return error;
        }
    }

    return OMX_ErrorNone;
}

VFM_DDepInternalBuffer *VFM_DDepAllInternalBuffers::addOneBuffer()
{
    OMX_ERRORTYPE error;

    if (!(mNb<mNbInternalBuffer)) {
        // Too many internal buffers required
        VFM_nmf_ddep::vfm_assert_static((0==1), __LINE__, OMX_FALSE);
        return NULL;
    }

    mNb++;

    MMHwBuffer::TBufferInfo aInfo;
    error = mInternalBuffer[0].getChunk()->BufferInfo(0, aInfo);
    if (error != OMX_ErrorNone) {
        // Cannot get cached info
        VFM_nmf_ddep::vfm_assert_static((0==1), __LINE__, OMX_FALSE);
        return NULL;
    }

    OMX_BOOL isCached = ((aInfo.iCacheAttr==MMHwBuffer::ENormalCached) ? OMX_TRUE : OMX_FALSE);
    error = mInternalBuffer[mNb-1].alloc(aInfo.iAllocatedSize, isCached);
    if (error != OMX_ErrorNone) {
        // Cannot allocate 1 buffer
        VFM_nmf_ddep::vfm_assert_static((0==1), __LINE__, OMX_FALSE);
        return NULL;
    }

    return &mInternalBuffer[mNb-1];
}


OMX_ERRORTYPE VFM_DDepInternalBuffer::alloc(t_uint32 size, OMX_BOOL cached)
{
    OMX_ERRORTYPE error=OMX_ErrorNone;
    VFM_nmf_ddep::vfm_assert_static((mStatus != INTERNAL_SENT), __LINE__, OMX_TRUE);
    VFM_nmf_ddep::vfm_assert_static(((mBufferHeader.nAllocLen==0 && mChunk==0) || (mBufferHeader.nAllocLen!=0 && mChunk!=0)), __LINE__, OMX_TRUE);

    if (mBufferHeader.nAllocLen >= size) {
        return OMX_ErrorNone;
    }

    VFM_DDepInternalBuffer saved = *this;
    int should_copy = (mStatus != INTERNAL_FREE);

    MMHwBuffer::TBufferPoolCreationAttributes poolAttrs;
    poolAttrs.iBuffers = 1;                // Number of buffers in the pool
    poolAttrs.iDeviceType = MMHwBuffer::ESystemMemory;      // Memory type
    poolAttrs.iDomainID = 0;                                // MPC Domain ID (only requested for MPC memory type)
    poolAttrs.iSize = size;                                 // Size (in byte) of a buffer
    poolAttrs.iAlignment = 0x100;                           // Alignment applied to the base address of each buffer in the pool
                                                            //   1,2,4,8,16,32,64 byte or -1 for MMU pageword size (default)
    poolAttrs.iCacheAttr = (cached ? (MMHwBuffer::ENormalCached) : (MMHwBuffer::ENormalUnCached));
    mChunk = 0;
#ifdef FORCE_NOREALLOC
    if (saved.mChunk==0) {
        error = MMHwBuffer::Create(poolAttrs, ptropenmaxHandle, mChunk);       // 2nd parameter as "0" instead of the handle of the component
    } else {
     //   NMF_LOG("Creation failed because FORCE_NOREALLOC\n");
    }
#else
    error = MMHwBuffer::Create(poolAttrs, ptropenmaxHandle, mChunk);       // 2nd parameter as "0" instead of the handle of the component
#endif

    if (error!=OMX_ErrorNone || mChunk==0) {
        // keep the previous value of the mChunk
        *this = saved;
        saved.mChunk = 0;   // because the destructor frees the chunk, and we want to keep the old one
        // NMF_LOG("Leaving with errors\n");
        return OMX_ErrorInsufficientResources;
    }

    MMHwBuffer::TBufferInfo info;
    error = mChunk->BufferInfo(0, info);
    if (error != OMX_ErrorNone) {
        // Cannot be allocated
        VFM_nmf_ddep::vfm_assert_static((0==1), __LINE__, OMX_FALSE, size);
        return OMX_ErrorInsufficientResources;
    }
    mBufferHeader.pBuffer = (t_uint8 *)info.iLogAddr;
    mBufferHeader.pInputPortPrivate = mBufferHeader.pOutputPortPrivate = mChunk;
    mBufferHeader.pAppPrivate = (OMX_PTR)this;
    mBufferHeader.nAllocLen = size;
    mBufferHeader.nFilledLen = 0;
    mBufferHeader.nFlags = 0;
    mBufferHeader.nOffset = 0;
    mStatus = INTERNAL_FREE;

    // make the copy if needed
    if (should_copy) {
        copy(&saved);
    }

    if (saved.mChunk) {
        free(&saved.mChunk);
        saved.mChunk=0;
    }

    return OMX_ErrorNone;
}

EXPORT_C void VFM_DDepInternalBuffer::free()
{
    if (mChunk) {
        OMX_ERRORTYPE error = MMHwBuffer::Destroy(mChunk);
        VFM_nmf_ddep::vfm_assert_static((error==OMX_ErrorNone), __LINE__, OMX_TRUE, error);
    }

    mChunk = 0;
    mStatus = INTERNAL_FREE;
    mBufferHeader.nAllocLen = mBufferHeader.nFilledLen = mBufferHeader.nOffset = 0;
}

void VFM_DDepInternalBuffer::free(MMHwBuffer **pChunk)
{
    OMX_ERRORTYPE error = MMHwBuffer::Destroy(*pChunk);
    VFM_nmf_ddep::vfm_assert_static((error==OMX_ErrorNone), __LINE__, OMX_TRUE, error);
}

void VFM_DDepAllInternalBuffers::insertFront(VFM_DDepInternalBuffer *pInternal)
{
    VFM_DDepInternalBuffer *pCurrent;
    OMX_BUFFERHEADERTYPE *pBufferHeader = pInternal->getBufferHeader();
    pBufferHeader->nFlags &= ~VFM_FLAG_INTERNALBUFFER;      // removes the internal flag as this buffer will be sent again
    pInternal->setStatus(0);
    for (int i=0; i<mNb; i++) {
        pCurrent = &(mInternalBuffer[i]);
        if (pCurrent == pInternal) {
            continue;
        }
        t_sint8 status = pCurrent->getStatus();
        if (status>=0) {
            pCurrent->setStatus(status+1);
        }
    }
}

void VFM_DDepInternalBuffer::copy(VFM_DDepInternalBuffer *pSaved)
{
    // Cannot copy in smaller buffer
    VFM_nmf_ddep::vfm_assert_static((mBufferHeader.nAllocLen>pSaved->mBufferHeader.nAllocLen), __LINE__, OMX_TRUE);

    // Status of buffers is not coherent
    VFM_nmf_ddep::vfm_assert_static((mStatus==INTERNAL_FREE && (pSaved->mStatus!=INTERNAL_FREE && pSaved->mStatus!=INTERNAL_SENT)), __LINE__, OMX_TRUE);

    mBufferHeader.nFilledLen = pSaved->mBufferHeader.nFilledLen;
    mBufferHeader.nFlags = pSaved->mBufferHeader.nFlags;
    memcpy(mBufferHeader.pBuffer, pSaved->mBufferHeader.pBuffer, mBufferHeader.nFilledLen);

    mStatus = pSaved->mStatus;
    pSaved->mStatus = INTERNAL_FREE;
    pSaved->mBufferHeader.nFilledLen = 0;
    pSaved->mBufferHeader.nOffset = 0;
}

VFM_DDepInternalBuffer *VFM_DDepAllInternalBuffers::getFree(OMX_BOOL force)
{
    VFM_DDepInternalBuffer *pCurrent=0, *pResult=0;
    for (int i=0; i<mNb; i++) {
        pCurrent = &(mInternalBuffer[i]);
        if (pCurrent->getStatus() == INTERNAL_FREE) {
            // Get the biggest free buffer
            if ((!pResult) || (pResult->getBufferHeader()->nAllocLen<pCurrent->getBufferHeader()->nAllocLen)) {
                pResult = pCurrent;
            }
        }
    }

    if (pResult) {
        return pResult;
    } else if (force) {
        pCurrent = addOneBuffer();
        return pCurrent;
    } else {
        return 0;
    }
}

EXPORT_C VFM_DDepInternalBuffer *VFM_DDepAllInternalBuffers::getCurrent(OMX_BOOL needANewOne)
{
    VFM_DDepInternalBuffer *pResult=0, *pCurrent;
    for (int i=0; i<mNb; i++) {
        pCurrent = &(mInternalBuffer[i]);
        switch (pCurrent->getStatus()) {
        case INTERNAL_CURRENT:
            return pCurrent;
        case INTERNAL_FREE:
            if (needANewOne==OMX_TRUE) {
                pResult = pCurrent;
            }
            break;
        }
    }
    if (pResult) {
        pResult->setStatus(INTERNAL_CURRENT);
    }

    return pResult;
}

EXPORT_C VFM_DDepInternalBuffer *VFM_DDepAllInternalBuffers::getNextToSend(OMX_BOOL willBeSent, OMX_S32 newStatus)
{
    VFM_DDepInternalBuffer *pNext=0;
    t_sint32 status;

    for (int i=0; i<mNb; i++) {
        status = mInternalBuffer[i].getStatus();
        if (status==0) {
            pNext = &(mInternalBuffer[i]);
            if (willBeSent==OMX_TRUE) {
                pNext->setStatus(newStatus);
            } else {
                return pNext;
            }
        } else if (status>0) {
            if (willBeSent==OMX_TRUE) {
                mInternalBuffer[i].setStatus(status-1);
            }
        }
    }
    return pNext;
}

OMX_ERRORTYPE VFM_DDepInternalBuffer::realloc(OMX_U32 new_size, VFM_SetDDep *pSet)
{
    OMX_ERRORTYPE error;
    VFM_nmf_ddep::vfm_assert_static((mChunk!=0), __LINE__, OMX_TRUE);

    // remove the header from information from the one that have been sent
    pSet->remove(getBufferHeader());

    MMHwBuffer::TBufferInfo aInfo;
    error = mChunk->BufferInfo(0, aInfo);
    VFM_nmf_ddep::vfm_assert_static((error==OMX_ErrorNone), __LINE__, OMX_TRUE, error);

    OMX_BOOL isCached = ((aInfo.iCacheAttr==MMHwBuffer::ENormalCached) ? OMX_TRUE : OMX_FALSE);
    return alloc(new_size, isCached);
}

OMX_ERRORTYPE VFM_DDepInternalBuffer::catBufferHeader(OMX_BUFFERHEADERTYPE *pHeaderToCopy, VFM_SetDDep *pSet)
{
    OMX_ERRORTYPE error;
    OMX_BUFFERHEADERTYPE *pInternalHeader = getBufferHeader();

    if (isCorrupted()) {
        return OMX_ErrorUndefined;
    }

    if (pInternalHeader->nFilledLen+pHeaderToCopy->nFilledLen+VFM_DDEP_SECURITY > pInternalHeader->nAllocLen) {
        error = realloc(pInternalHeader->nFilledLen+pHeaderToCopy->nFilledLen+getReallocMore(), pSet);
        if (error!=OMX_ErrorNone) {
            // reallocation failed
            setCorrupted();
            return error;
        }
    }
    pInternalHeader = getBufferHeader();
    VFM_nmf_ddep::vfm_assert_static((pInternalHeader->nFilledLen+pHeaderToCopy->nFilledLen <= pInternalHeader->nAllocLen), __LINE__, OMX_TRUE);

    // copy the buffer
    memcpy(pInternalHeader->pBuffer+pInternalHeader->nOffset+pInternalHeader->nFilledLen, pHeaderToCopy->pBuffer+pHeaderToCopy->nOffset, pHeaderToCopy->nFilledLen);
    pInternalHeader->nFilledLen += pHeaderToCopy->nFilledLen;

    // update the flag
    pInternalHeader->nFlags |= pHeaderToCopy->nFlags;
    return OMX_ErrorNone;
}

EXPORT_C OMX_U32 VFM_DDepInternalBuffer::getReallocMore()
{
    // default size: we add 128KBytes more when reallocating
    return 128*1024;
}


void VFM_nmf_ddep::copyInInternalBuffer(VFM_DDepInternalBuffer *pInternal, OMX_U32 nPortIndex, OMX_BOOL isAsyncReturn)
{
    VFM_nmf_ddep::vfm_assert_static((pInternal!=0), __LINE__, OMX_TRUE);
    VFM_nmf_ddep::vfm_assert_static((pInternal->getStatus()==INTERNAL_CURRENT), __LINE__, OMX_TRUE);
    VFM_nmf_ddep::vfm_assert_static((pInternal->getChunk()!=0), __LINE__, OMX_TRUE);

    Port *pPort;
    VFM_DDepAllInternalBuffers *pAllInternals;
    VFM_SetDDep *pSet;
    switch (nPortIndex) {
    case 0:
        pPort = &mPorts[0];
        pAllInternals = &mInputInternal;
        pSet = &mInputSet;
        VFM_nmf_ddep::vfm_assert_static((mInputBufferMode&VFMDDEP_BUFFERMODE_COPY), __LINE__, OMX_TRUE);
        break;
    case 1:
        pPort = &mPorts[1];
        pAllInternals = &mOutputInternal;
        pSet = &mOutputSet;
        VFM_nmf_ddep::vfm_assert_static((mOutputBufferMode&VFMDDEP_BUFFERMODE_COPY), __LINE__, OMX_TRUE);
        break;
    default:
        NMF_PANIC("nPortIndex==%d, but should be 0 or 1 at line %d file %s\n", nPortIndex, __LINE__, __FILE__);
        return;
    }

    while (pPort->queuedBufferCount() != 0) {
        OMX_BUFFERHEADERTYPE *pBuffer = pPort->getBuffer(0);
        OMX_U32 flags = pBuffer->nFlags;
        if (!pInternal->isCorrupted()) {
            pInternal->catBufferHeader(pBuffer, pSet);
        }
        if (isAsyncReturn) {
            pBuffer = pPort->dequeueBuffer();
            returnBufferAsync(0, pBuffer);
        } else {
            pPort->dequeueAndReturnBuffer();
        }

        // last buffers of the frame?
        if (VFM_DDepInternalBuffer::isLastBuffer(flags)) {
            pInternal->setStatus(pAllInternals->getNextStatus());
            return;
        }
    }
}


EXPORT_C int VFM_DDepAllInternalBuffers::getNextStatus(void)
{
    t_sint8 status, next=0;

    for (int i=0; i<mNb; i++) {
        status = mInternalBuffer[i].getStatus();
        if (next<=status) {
            next = status+1;
        }
    }
    return next;
}

void VFM_DDepAllInternalBuffers::setAllFree(void)
{
    OMX_BUFFERHEADERTYPE *pBuffer;
    for (int i=0; i<mNb; i++) {
        mInternalBuffer[i].setStatus(INTERNAL_FREE);
        pBuffer = mInternalBuffer[i].getBufferHeader();
        pBuffer->nFilledLen = 0;
        pBuffer->nOffset = 0;
        pBuffer->nFlags = 0;
    }
}

EXPORT_C void VFM_DDepInternalBuffer::setIsBack(void)
{
    VFM_nmf_ddep::vfm_assert_static((mStatus==INTERNAL_SENT), __LINE__, OMX_TRUE);

    mStatus = INTERNAL_FREE;
    mBufferHeader.nFilledLen = 0;
    mBufferHeader.nOffset = 0;
    mBufferHeader.nFlags = 0;
}

void VFM_DDepInternalBuffer::setStatus(t_sint8 status)
{
    if (isCorrupted()) {
        resetCorrupted();
        mStatus = INTERNAL_FREE;
    } else {
        mStatus = status;
    }

    if (mStatus == INTERNAL_FREE) {
        mBufferHeader.nFilledLen = 0;
        mBufferHeader.nOffset = 0;
        mBufferHeader.nFlags = 0;
    }
}

EXPORT_C VFM_DDepCommand::VFM_DDepCommand()
{
    reset();
}

#ifdef __SYMBIAN32__
#include "omxilosalservices_dev.h"
#else
#include "omxilosalservices.h"
using namespace omxilosalservices;
#endif

EXPORT_C void VFM_PowerManagement::setFullDVFS(OMX_PTR omxHandle)
{
    OmxILOsalPrm::ChangeResourceState(ESocDvfsOpp, 100, omxHandle);
}

//+ ER335583
EXPORT_C void VFM_PowerManagement::setFullDDR(OMX_PTR omxHandle)
{
#ifdef __SYMBIAN32__
#else
    OmxILOsalPrm::ChangeResourceState(ESocDDROpp, 100, omxHandle);
#endif
}

EXPORT_C void VFM_PowerManagement::releaseFullDVFS(OMX_PTR omxHandle)
{
    OmxILOsalPrm::ReleaseResourceState(ESocDvfsOpp, omxHandle);
}

EXPORT_C void VFM_PowerManagement::releaseFullDDR(OMX_PTR omxHandle)
{
#ifdef __SYMBIAN32__
#else
    OmxILOsalPrm::ReleaseResourceState(ESocDDROpp, omxHandle);
#endif
}
//- ER335583

EXPORT_C void VFM_PowerManagement::setDVFS(OMX_PTR omxHandle, VFM_ResDVFSType value)
{
    OMX_U32 dvfs=50;
    switch (value) {
    case VFM_ResDVFS_Full: dvfs = 100; break;
    case VFM_ResDVFS_Half: dvfs = 50; break;
    default: VFM_nmf_ddep::vfm_assert_static((0==1), __LINE__, OMX_TRUE); break;
    }

    OmxILOsalPrm::ChangeResourceState(ESocDvfsOpp , dvfs, omxHandle);
}

//+ ER335583
EXPORT_C void VFM_PowerManagement::setDDR(OMX_PTR omxHandle, VFM_ResDDRType value)
{
#ifdef __SYMBIAN32__
#else
    OMX_U32 ddr = 25;
    switch (value) {
    case VFM_ResDDR_Full: ddr = 100; break;
    case VFM_ResDDR_Half: ddr = 50; break;
    default: VFM_nmf_ddep::vfm_assert_static((0==1), __LINE__, OMX_TRUE); break;
    }

    OmxILOsalPrm::ChangeResourceState(ESocDDROpp , ddr, omxHandle);
#endif
}
//- ER335583

EXPORT_C void VFM_PowerManagement::resetDVFS(OMX_PTR omxHandle)
{
    OmxILOsalPrm::ReleaseResourceState(ESocDvfsOpp , omxHandle);
}

//+ ER335583
EXPORT_C void VFM_PowerManagement::resetDDR(OMX_PTR omxHandle)
{
#ifdef __SYMBIAN32__
#else
    OmxILOsalPrm::ReleaseResourceState(ESocDDROpp , omxHandle);
#endif
}
//- ER335583

EXPORT_C void VFM_PowerManagement::setLatency(OMX_PTR omxHandle, VFM_ResLatencyType value)
{
    OMX_U32 latency=20;
    switch (value) {
    case VFM_ResLatency_None: latency=20; break;
    case VFM_ResLatency_Standard: latency = 600; break;
    case VFM_ResLatency_Max: latency = 10000; break;
    default: VFM_nmf_ddep::vfm_assert_static((0==1), __LINE__, OMX_TRUE); break;
    }

    OmxILOsalPrm::ChangeResourceState(EPwrResLatency , latency, omxHandle);
}

EXPORT_C void VFM_PowerManagement::resetLatency(OMX_PTR omxHandle)
{
    OmxILOsalPrm::ReleaseResourceState(EPwrResLatency , omxHandle);
}

//modified for ER335583
EXPORT_C void VFM_PowerManagement::set(VFM_ResDVFSType dvfs, VFM_ResDDRType ddr, VFM_ResLatencyType latency)
{
    VFM_nmf_ddep::vfm_assert_static((mManage), __LINE__, OMX_TRUE);

    mRequestDVFS = dvfs;
	mRequestDDR = ddr;
    mRequestLatency = latency;
    if (mStatus==VFM_PowerStatus_Regular) {
        setDVFS(this, mRequestDVFS);
//+ ER335583
		setDDR(this, mRequestDDR);
//- ER335583
        setLatency(this, mRequestLatency);
    }
}

EXPORT_C void VFM_PowerManagement::force()
{
    if (mManage) {
        if (mStatus!=VFM_PowerStatus_Force) {
            mStatus = VFM_PowerStatus_Force;
            setDVFS(this, VFM_ResDVFS_Full);
//+ ER335583
			setDDR(this, VFM_ResDDR_Full);
//- ER335583
            setLatency(this, VFM_ResLatency_None);
        }
    }
}

EXPORT_C void VFM_PowerManagement::cut()
{
    if (mManage) {
        if (mStatus!=VFM_PowerStatus_Cut) {
            mStatus = VFM_PowerStatus_Cut;
            setDVFS(this, VFM_ResDVFS_Half);
//+ ER335583
			setDDR(this, VFM_ResDDR_Half);
//- ER335583
            setLatency(this, VFM_ResLatency_Max);
        }
    }
}

EXPORT_C void VFM_PowerManagement::regular()
{
    if (mManage) {
        if (mStatus!=VFM_PowerStatus_Regular) {
            mStatus = VFM_PowerStatus_Regular;
            setDVFS(this, mRequestDVFS);
//+ ER335583
			setDDR(this, mRequestDDR);
//- ER335583
            setLatency(this, mRequestLatency);
        }
    }
}

VFM_PowerManagement::VFM_PowerManagement(OMX_BOOL manage):
        mManage(manage),
        mStatus(VFM_PowerStatus_Cut),
        mRequestDVFS(VFM_ResDVFS_Half),
//+ ER335583
		mRequestDDR(VFM_ResDDR_Half),
//- ER335583
        mRequestLatency(VFM_ResLatency_Max)
{
    if (mManage) {
        setDVFS(this, mRequestDVFS);
//+ ER335583
        setDDR(this, mRequestDDR);
//- ER335583
        setLatency(this, mRequestLatency);
    }
}

VFM_PowerManagement::~VFM_PowerManagement()
{
    if (mManage) {
        resetDVFS(this);
//+ ER335583
		resetDDR(this);
//- ER335583
        resetLatency(this);
    }
}

EXPORT_C OMX_BOOL VFM_DDepInternalBuffer::isLastBuffer(OMX_U32 flags)
{
    return ((flags&OMX_BUFFERFLAG_ENDOFFRAME) ? OMX_TRUE : OMX_FALSE);
}

EXPORT_C OMX_BOOL VFM_DDepCommand::isValid() const
{
    return ((mCommand==(OMX_COMMANDTYPE)0xdeadbeef) ? OMX_FALSE : OMX_TRUE);
}

EXPORT_C void VFM_DDepCommand::reset()
{
    mCommand=(OMX_COMMANDTYPE)0xdeadbeef;
    mParam=0xdeadbeef;
}

EXPORT_C bool VFM_nmf_ddep::is_codec_flushing(int nPortIndex) const
{
    return ((unsigned)mMaskAckFlushing != 0xFFFFFFFF);
}

static int vfm_strcmp (const char * s1, const char * s2)
{
    for(; *s1 == *s2; ++s1, ++s2){
        if(*s1 == 0)
            return 0;
	}
    return (*(unsigned char *)s1 < *(unsigned char *)s2) ? -1 : 1;
}

/* +Change for CR 367195, 369543 */
static int vfm_atoi(const char *str1)
{
   int i=0, value=0;
   for ( i = 0, value = 0; str1 [ i ] != '\0'; ++i )
   {
      int digit = str1[i] - '0';
      value = 10 * value + digit;
   }
   return value;
}
/* -Change for CR 367195, 369543 */

/* + Changes for CR 334359 */
OMX_BOOL VFM_SocCapabilityMgt::isSoc1080pCapable()
{
#ifdef __SYMBIAN32__
     return OMX_TRUE;
#else
    switch(OmxILOsalProductId::GetProductId())
    {
        case EProductId8520F: /* new product Id added for CR 430551 */
        case EProductId8520H: /* new product Id added for CR 430551 */
        case EProductId8500A: /* new product Id added for CR 430551 */
        case EProductId8500C:
            //Add non 1080p chip IDs here
            return OMX_FALSE;
        default:
#ifdef FORCE_DB8500_720P /* +Change for CR 445735 */
       return OMX_FALSE;
#else
            char str[128];
            OmxILOsalProperties::GetProperty(EPropertyKeyVideoDecoderMaxResolution, str);
            if(!vfm_strcmp(str,"720p")) {
                return OMX_FALSE;
            }
            else { // 1080p or unspecified
                return OMX_TRUE;
            }
#endif /* -Change for CR 445735 */
    }
#endif
}
/* - Changes for CR 334359 */
OMX_U32 VFM_SocCapabilityMgt::getLevelCapabilities(){
    char str[128];
    OmxILOsalProperties::GetProperty(EPropertyKeyVideoDecoderH264MaxLevel, str);
    if(!vfm_strcmp(str,"3.2")) {
        return 32;
    }
    else if(!vfm_strcmp(str,"4.2")) {
        return 42;
    }
    else{
        return 51;
    }
}

/* +Change for CR 367195, 369543 */
OMX_U32 VFM_SocCapabilityMgt::getOutputBufferCount(){
    char str[128]= {0};
    OmxILOsalProperties::GetProperty(EPropertyKeyVideoEncoderOutputBufferCount, str);
    return vfm_atoi(str);
}

OMX_U32 VFM_SocCapabilityMgt::getMPEG4DecInputBuffSize(){
    char str[128]= {0};
    OmxILOsalProperties::GetProperty(EPropertyKeyVideoDecoderMPEG4InputSize, str);
    return vfm_atoi(str);
}


OMX_U32 VFM_SocCapabilityMgt::getDecoderRecyclingDelay(){
    char str[128] = {0};
    OmxILOsalProperties::GetProperty(EPropertyKeyVideoDecoderRecyclingDelay, str);
    return vfm_atoi(str);
}

OMX_BOOL VFM_SocCapabilityMgt::getDecoderDisplayOrder(){
    char str[128]= {0};
    OmxILOsalProperties::GetProperty(EPropertyKeyVideoDecoderDisplayOrder, str);
    if (vfm_atoi(str))
        return OMX_TRUE;
    else
        return OMX_FALSE;
}

/* +Change for CR 374389 */
OMX_U32 VFM_SocCapabilityMgt::getH264DecoderDefaultLevel(){
    char str[128]= {0};
    OmxILOsalProperties::GetProperty(EPropertyKeyVideoDecoderH264DefaultLevel, str);
    if(!vfm_strcmp(str,"1b")) {
        return 9;
    }
    else if(!vfm_strcmp(str,"1")) {
        return 10;
    }
    else if(!vfm_strcmp(str,"1.1")) {
        return 11;
    }
    else if(!vfm_strcmp(str,"1.2")) {
        return 12;
    }
    else if(!vfm_strcmp(str,"1.3")) {
        return 13;
    }
    else if(!vfm_strcmp(str,"2")) {
        return 20;
    }
    else if(!vfm_strcmp(str,"2.1")) {
        return 21;
    }
    else if(!vfm_strcmp(str,"2.2")) {
        return 22;
    }
    else if(!vfm_strcmp(str,"3")) {
        return 30;
    }
    else if(!vfm_strcmp(str,"3.1")) {
        return 31;
    }
    else if(!vfm_strcmp(str,"3.2")) {
        return 32;
    }
    else if(!vfm_strcmp(str,"4")) {
        return 40;
    }
    else if(!vfm_strcmp(str,"4.1")) {
        return 41;
    }
    else if(!vfm_strcmp(str,"4.2")) {
        return 42;
    }
    else if(!vfm_strcmp(str,"5")) {
        return 50;
    }
    else if(!vfm_strcmp(str,"5.1")) {
        return 51;
    } else {
      //  NMF_LOG("H264DecoderDefaultLevel got unkown '%s', defaulting to 5.1", str);
        return 51;
    }
}
/* -Change for CR 374389 */

/* -Change for CR 367195, 369543 */
