/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include "osi_trace.h"
#undef _CNAME_
#define _CNAME_ VFM
#include "mmhwbuffer.h"
#include "VFM_Utility.h"

/// @defgroup VFMMemory VFM Memory Support
/// @ingroup VFMMemoryAPI
/// @brief This section is intended to the documentation of the internal implementation of the
/// VFMMemoryAPI. User of this API may only refer to VFMMemoryAPI, and may skip this section
///
/// Implementation of the Memory Support for address translation (physical <-> logical)
/// is performed as a list of item VFM_Memory. These items contains generic information
/// (type, source line, size, ...) and specific members as a union. This union contains
/// the specific allocation information.


// #define VFM_DEBUG
// #define VFM_CHECK_LEAKS

#include "VFM_Memory.h"
#include "omxilosalservices.h"
#include "VFM_ParamAndConfig.h"
#ifdef NOPROXY
#include <stdio.h>
#include <assert.h>
#define ASSERT(a)    if ((a)==0) {printf("Assert Failed at %s %d\n",__FILE__,__LINE__);assert(0);}
#define DBC_ASSERT(a) ASSERT(a)
#define DBC_PRECONDITION(a) ASSERT(a)
#define DBC_POSTCONDITION(a) ASSERT(a)
#else
#include "ENS_DBC.h"
#endif

//#define VFM_DEBUG
//#define VFM_CHECK_LEAKS
//#define VFM_STATUS_REPORT

#ifdef ANDROID
#define PRINTPG
#else
#define PRINTPG
#endif


#if (defined VFM_DEBUG || defined VFM_CHECK_LEAKS || defined VFM_STATUS_REPORT )
#include <stdio.h>
#endif
#include <cm/inc/cm.h>

// OST framework
#include "OstTraceDefinitions_ste.h"
#include "OpenSystemTrace_ste.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "video_vfm_src_VFM_MemoryTraces.h"
#endif

// Traces
#ifdef PACKET_VIDEO_SUPPORT
#ifndef LOG_TAG
#define LOG_TAG "VFM"
#endif
#include<cutils/log.h>
#endif

static void VFM_TraceError(int line, OMX_ERRORTYPE error, int param1, int param2)
{
    OstTraceInt4(TRACE_ERROR, "VFM: VFM_Memory: VFM_TraceError: VIDEOTRACE Error 0x%x, line %d,  param1=%d  param2=%d\n", (int)error, line, param1, param2);
}

/// @ingroup VFMMemory
/// @brief Describe a couple logical address and corresponding physical address,
/// and is used for memory allocated outside VFM Memory. Typically the case for buffers
/// coming from EmptyThisBuffer() / FillThisBuffer()
class VFM_MemoryLogicalPhysical {
    public:
        /// @brief the logical address
        t_uint8 *mLogical;

        /// @brief the corresponding physical address
        t_uint8 *mPhysical;

        ///@brief the corresponding mpc address
        t_uint8 *mMpc;

        inline static void vfm_assert_static(int condition, int line, OMX_BOOL isFatal, int param1=0, int param2=0)
        {
            if (!condition) {
                OstTraceInt3(TRACE_ERROR, "VFM: VFM_Memory: vfm_assert_static: VIDEOTRACE Error line %d,  param1=%d  param2=%d\n", line, param1, param2);
                if (isFatal) {
                    DBC_ASSERT(0==1);
                }
            }
        }
};

#if !(defined WORKSTATION || defined __ndk5500_a0__)
/// @ingroup VFMMemory
/// @brief Describe a couple logical address and corresponding physical address,
/// and is used for memory allocated outside VFM Memory. Typically the case for buffers
/// coming from EmptyThisBuffer() / FillThisBuffer()
class VFM_MemoryCM: public VFM_MemoryLogicalPhysical {
    public:
        /// @brief meta-data of the CM_AllocMPC
        t_cm_memory_handle handle;

        /// @brief set mLogical and mPhysical members of the structure according to
        /// the new MPC memory allocated
        inline void set_logical_physical_cm_mpc() {
            t_cm_system_address sysAddr;

            CM_GetMpcMemorySystemAddress(handle, &sysAddr);
            mLogical = (t_uint8 *)sysAddr.logical;
            mPhysical = (t_uint8 *)sysAddr.physical;

            CM_GetMpcMemoryMpcAddress(handle,(t_uint32 *)(&mMpc));
        }
};
#endif


/// @ingroup VFMMemory
/// @brief Describe an external memory, made of a couple logical address and corresponding physical address,
/// and of metaData related to HWBuffers. The metadata are null if the external memory is not related
/// to HW Buffers
class VFM_MemoryExternal: public VFM_MemoryLogicalPhysical {
    public:
        /// @brief HW Buffers. Is null if the external memory is not related to HW Buffers
        MMHwBuffer *mSharedChunk;

        /// @brief Make a cache operation (clean / invalidate) on a memory
        /// @param[in] aOp The operation to perform (clean / invalidate)
        /// @param[in] LogicalAddress The logical address to flush. If 0, use the starting logical address of this memory
        /// @param[in] size The size of the operation
        /// @return OMX_ErrorNone if no error, an error otherwise
        OMX_ERRORTYPE cacheOperation(MMHwBuffer::TSyncCacheOperation aOp, t_uint8 *LogicalAddress, t_uint32 size) const;
};

/// @ingroup VFMMemory
/// @brief Manages HW Buffers chunk. A chunk is made of several buffers (up to 32)
/// that have the same characteristics. Chunks are allocated as in Symbian the allocation
/// is always align on a page, that is 0x1000, that is the minimal allocation is quite big
class VFM_MemoryHwBufferChunk
{
    friend class VFM_MemoryHwBuffer;
    public:
        /// @brief Allocate a new chunk of hw buffers, that is a set of buffers having the
        /// same characteristics
        /// @param [in] size the size of the buffer we want to allocate
        /// @param [in] align its alignment properties
        /// @param [in] cached TRUE if we want cached memory, FALSE otherwise
        /// @return A new HW Buffer Chunk
        static VFM_MemoryHwBufferChunk *newChunk(t_uint32 size, t_uint32 align, OMX_BOOL cached, OMX_PTR omxhandle);

        /// @brief Get a chunk of hw buffer that could contain buffer. Note that a new
        /// chunk could be allocated in case no suitable chunk already exist
        /// @param [in] size the size of the buffer we want to allocate
        /// @param [in] align its alignment properties
        /// @param [out] index the index (from 0 to 31) of the buffer in the chunk
        /// @param [out] chunk_list is the chunk list that we have in this context. It can be updated
        /// @param [in] cached TRUE if we want cache memory, FALSE if non-cached is searched for
        /// if a new chunk need to be created
        /// @return The chunk that contains this buffer
        static VFM_MemoryHwBufferChunk *getChunk(t_uint32 size, t_uint32 align, t_uint32 *index, VFM_MemoryHwBufferChunk **chunk_list,OMX_BOOL cached, OMX_PTR omxhandle);

        /// @brief The alignement property of HW buffers in Symbian. It corresponds to a page,
        /// and defines the minimal size that is allocated
        static const t_uint32 mMinSize = 0x1000;

        /// @brief Get the shared chunk structure associated with this HW Buffer
        /// @return The shared chunck structure
        MMHwBuffer *getSharedChunk() { return mSharedChunk; }

    private:
        /// @brief Hw buffers pointer
        MMHwBuffer *mSharedChunk;

        /// @brief The size of 1 buffer in the chunk
        t_uint32 mSize;

        /// @brief The alignement property of 1 buffer in the chunk
        t_uint32 mAlign;

        /// @brief The mask of the used buffers in the chunk. As an example,
        /// 0x101 indicates buffers 0 and 2 are used
        t_uint32 mUsedMask;

        /// @brief The number of buffers in the chunk
        t_uint32 mNbBuffers;

        /// @brief TRUE if the memory is cached, FALSE in non-cached
        OMX_BOOL cached;

        /// @brief The next chunk in the list
        VFM_MemoryHwBufferChunk *mNext;

};


VFM_MemoryHwBufferChunk *VFM_MemoryHwBufferChunk::newChunk(t_uint32 size, t_uint32 align, OMX_BOOL cached, OMX_PTR omxhandle)
{
    OMX_ERRORTYPE error;
    VFM_MemoryHwBufferChunk *pChunk = new VFM_MemoryHwBufferChunk;
    if (!pChunk) {
        return 0;
    }

    pChunk->cached = cached;

    pChunk->mSize = size;
    pChunk->mAlign = align;
    pChunk->mUsedMask = 0x0;
    if (size >= (mMinSize / 2)) {
        pChunk->mNbBuffers = 1;
    } else if (size % align == 0) {
        pChunk->mNbBuffers = mMinSize / size;
    } else {
        t_uint32 padded_size = align * ((size+align) / align);
        pChunk->mNbBuffers = mMinSize / padded_size;
    }
    VFM_MemoryLogicalPhysical::vfm_assert_static((pChunk->mNbBuffers>=1), __LINE__, OMX_TRUE);
    if (pChunk->mNbBuffers > 32)     pChunk->mNbBuffers = 32;

    MMHwBuffer::TBufferPoolCreationAttributes poolAttrs;
    poolAttrs.iBuffers = pChunk->mNbBuffers;                // Number of buffers in the pool
    poolAttrs.iDeviceType = MMHwBuffer::ESystemMemory;      // Memory type
    poolAttrs.iDomainID = 0;                                // MPC Domain ID (only requested for MPC memory type)
    poolAttrs.iSize = size;                                 // Size (in byte) of a buffer
    poolAttrs.iAlignment = align;                           // Alignment applied to the base address of each buffer in the pool
                                                            //   1,2,4,8,16,32,64 byte or -1 for MMU pageword size (default)

    poolAttrs.iCacheAttr = (cached ? (MMHwBuffer::ENormalCached) : (MMHwBuffer::ENormalUnCached));
    pChunk->mSharedChunk = 0;
    error = MMHwBuffer::Create(poolAttrs, omxhandle, pChunk->mSharedChunk);         // 2nd parameter as "0" instead of the handle of the component
    if (error != OMX_ErrorNone) {
        delete pChunk;
        return 0;
    }
    VFM_MemoryLogicalPhysical::vfm_assert_static((pChunk->mSharedChunk!=0), __LINE__, OMX_TRUE);

    return pChunk;
}


VFM_MemoryHwBufferChunk *VFM_MemoryHwBufferChunk::getChunk(t_uint32 size, t_uint32 align, t_uint32 *index, VFM_MemoryHwBufferChunk **chunk_list,OMX_BOOL cached, OMX_PTR omxhandle)
{
    VFM_MemoryHwBufferChunk *chunk_found=0;
    for (VFM_MemoryHwBufferChunk *chunk=*chunk_list; chunk && !chunk_found; chunk=chunk->mNext) {
        if (chunk->mSize==size && chunk->mAlign==align && chunk->cached==cached) {
            // check if there is an available index
            for (unsigned int i=0; i<chunk->mNbBuffers && !chunk_found; i++) {
                if (!(chunk->mUsedMask & (1<<i))) {
                    *index = i;
                    chunk_found = chunk;
                }
            }
        }
    }
    if (!chunk_found) {
        chunk_found = VFM_MemoryHwBufferChunk::newChunk(size, align,cached,omxhandle);
        if (!chunk_found) {
            return 0;
        }
        chunk_found->mNext = *chunk_list;
        *chunk_list = chunk_found;
        *index = 0;
    }
    chunk_found->mUsedMask |= (1 << (*index));
    return chunk_found;
}


/// @ingroup VFMMemory
/// @brief Keep internal data used to manage hw buffers
class VFM_MemoryHwBuffer: public VFM_MemoryLogicalPhysical {
    public:
        /// @brief Set the chunk for this buffer
        /// @param [in] size of the buffer to allocate
        /// @param [in] align properties of the buffer to allocate
        /// @param [out] chunk_list if the list of current chunks. It may be updated
        /// @param [in] cached TRUE if cached memory has to be allocated, FALSE otherwise
        /// if  new chunk is required
        OMX_ERRORTYPE setChunk(t_uint32 size, t_uint32 align, VFM_MemoryHwBufferChunk **chunk_list, OMX_BOOL cached);

        /// @brief Free this buffer. It can result in freeing the entire chunk
        /// if it is no more used
        /// @param [out] chunk_list is the list of chunk. It may be updated if the chunk
        /// is entirely freed
        void Free(VFM_MemoryHwBufferChunk **chunk_list);


#ifdef _CACHE_OPT_
        /// @brief Make a cache operation (clean / invalidate) on a memory
        /// @param[in] aOp The operation to perform (clean / invalidate)
        /// @param[in] size The size of the hwbuffer
        /// @return OMX_ErrorNone if no error, an error otherwise
        OMX_ERRORTYPE cacheOperation(MMHwBuffer::TSyncCacheOperation aOp, t_uint8 *LogicalAddress, t_uint32 size) const;
#endif

        /// @brief The chunk this buffer belongs to. It contains the properties of the buffer
        VFM_MemoryHwBufferChunk *mChunk;

        /// @brief The index of this buffer in the chunk mChunk
        t_uint32 mIndex;

        OMX_PTR VFM_MemoryHwBuffer_omxhandle;
};

OMX_ERRORTYPE VFM_MemoryHwBuffer::setChunk(t_uint32 size, t_uint32 align, VFM_MemoryHwBufferChunk **chunk_list,OMX_BOOL cached)
{
    // Get (or create the chunk)
    mChunk = VFM_MemoryHwBufferChunk::getChunk(size, align, &mIndex, chunk_list,cached,VFM_MemoryHwBuffer_omxhandle);
    if (!mChunk) {
        return OMX_ErrorUndefined;
    }

    MMHwBuffer::TBufferInfo info;
    OMX_ERRORTYPE error = mChunk->getSharedChunk()->BufferInfo(mIndex, info);
    VFM_MemoryLogicalPhysical::vfm_assert_static((error==OMX_ErrorNone), __LINE__, OMX_TRUE);
    mLogical = (t_uint8 *)info.iLogAddr;
    mPhysical = (t_uint8 *)info.iPhyAddr;
    return OMX_ErrorNone;
}

void VFM_MemoryHwBuffer::Free(VFM_MemoryHwBufferChunk **chunk_list)
{
    mChunk->mUsedMask &= (~(1 << mIndex));
    // Check it is still used
    if (mChunk->mUsedMask == 0x0) {
        VFM_MemoryHwBufferChunk *prev_chunk=0;
        VFM_MemoryHwBufferChunk *chunk;
        for (chunk=*chunk_list; chunk && chunk!=mChunk; chunk=chunk->mNext) {
            prev_chunk = chunk;
        }
        VFM_MemoryLogicalPhysical::vfm_assert_static((chunk!=0), __LINE__, OMX_TRUE);
        if (!prev_chunk) {
            *chunk_list = mChunk->mNext;
        } else {
            prev_chunk->mNext = mChunk->mNext;
        }

        MMHwBuffer *mSharedChunk = mChunk->getSharedChunk();
        OMX_ERRORTYPE error = MMHwBuffer::Destroy(mSharedChunk);
        VFM_MemoryLogicalPhysical::vfm_assert_static((error == OMX_ErrorNone), __LINE__, OMX_TRUE);
        delete mChunk; //fix for ER 355908
        mChunk = 0;
    }
}

#ifdef _CACHE_OPT_
OMX_ERRORTYPE VFM_MemoryHwBuffer::cacheOperation(MMHwBuffer::TSyncCacheOperation aOp, t_uint8 *LogicalAddress, t_uint32 size) const
{
    MMHwBuffer *sharedChunk = mChunk->getSharedChunk();
    VFM_MemoryLogicalPhysical::vfm_assert_static((sharedChunk!=0), __LINE__, OMX_TRUE);
    OMX_U32 aPhysAddr;
    if (LogicalAddress==0) {
        return mChunk->getSharedChunk()->CacheSync(aOp, (OMX_U32)mLogical, size, aPhysAddr);
    } else {
        return mChunk->getSharedChunk()->CacheSync(aOp, (OMX_U32)LogicalAddress, size, aPhysAddr);
    }
}

OMX_ERRORTYPE VFM_MemoryExternal::cacheOperation(MMHwBuffer::TSyncCacheOperation aOp, t_uint8 *LogicalAddress, t_uint32 size) const
{
    OMX_U32 aPhysAddr;
    if (mSharedChunk) {
        if (LogicalAddress==0) {
            return mSharedChunk->CacheSync(aOp, (OMX_U32)mLogical, size, aPhysAddr);
        } else {
            return mSharedChunk->CacheSync(aOp, (OMX_U32)LogicalAddress, size, aPhysAddr);
        }
    } else {
        return OMX_ErrorUndefined;
    }
}
#endif  // _CACHE_OPT_

/// @ingroup VFMMemory
/// @brief Generic class for the correspondance between logical and physical
/// addresses. This is the member of a pool that contains all the memory
/// references (cf. class  VFM_MemoryContext)
class VFM_Memory
{
    friend class VFM_MemoryContext;

    public:
        /// @brief Constructor
        VFM_Memory() : mType(VFM_MEM_NONE), mSize(0), mLine(0), mFilename(0), mNext(0), vfm_memory_omxhandle(0) { };

        /// @brief Destructor
        ~VFM_Memory() {};

        void setomxhandle(OMX_PTR *handle) { vfm_memory_omxhandle = *handle;}

    private:
        /// @brief Allocate a memory given its type, size and properties
        /// @param [in] size of the allocation
        /// @param [in] type deprecated
        /// @param [in] align alignement properties.
        ///    @note This should be O with VFM_MEM_NEW
        ///    @note Check the real allocation method (CM_AllocMPC(),...) for set of property
        /// @param [in] prop properties of the allocation (non-cachable,...).
        ///    @note N/A with VFM_MEM_NEW
        /// @param [in] line of the source file where the allocation occurs (used in debug mode)
        /// @param [in] file is the file name where the allocation occurs (used in debug mode)
        /// @param [in] chunk_list is the list of the HW buffer chunks
        /// @param[in]  domainDdrId the memory domain to allocate to
        /// @param[in]  domainEsramId the memory domain to allocate to
        /// @return the logical address. This function updates the specific members of the class object with
        ///    logical and physical address correspondances
        /// @note type=VFM_MEM_EXTERNALSOURCE is forbidden. AddMemoryExternalSource() should be used instead of Alloc()
        t_uint8 *Alloc(t_uint32 size, VFM_Mem_Type type, t_uint32 align, t_uint32 prop, t_uint32 line, t_uint8 *file,
            VFM_MemoryHwBufferChunk **chunk_list, t_cm_domain_id domainDdrId, t_cm_domain_id domainEsramId);

        /// @brief Store, in the list, a memory couple when the allocation is already performed outside of
        /// the VFM Memory cope
        /// @param [in] LogicalAddress is the logical address
        /// @param [in] PhysicalAddress is its corresponding physical address
        /// @param [in] size of the allocation
        /// @param [in] pMetaData The metadata associated with a HWBuffer, null if not from an HWBuffer
        /// @note Memory type will be VFM_MEM_EXTERNALSOURCE
        void AddMemoryExternalSource(t_uint8 *LogicalAddress, t_uint8 *PhysicalAddress, t_uint32 size, void *pMetaData);

        /// @brief Free the memory
        /// @param [in] chunk_list the chunk list used to store HW buffers. This may be used when freeing
        /// a memory allocated using HW Buffers
        void Free(VFM_MemoryHwBufferChunk **chunk_list);

        /// @brief Get the logical address of this memory
        /// @return The logical address
        t_uint8 *GetLogicalAddress() const;

        /// @brief Get the physical address of this memory
        /// @return The physical address
        t_uint8 *GetPhysicalAddress() const;

        /// @brief Get the physical address of this memory
        /// @return The physical address
        t_uint8 *GetMpcAddress() const;

        /// @brief Translate a memory type in another one in case a type is no more available.
        /// @param [in] type of memory allocation
        /// @return the corresponding type to use instead
        static VFM_Mem_Type TypeConversion(VFM_Mem_Type type);

        /// @brief Convert an alignement property to a power of 2 one. For example, 0x100 would
        /// be converted into 0xFF and 0xFF would remained the same
        /// @param [in] align the alignment to convert
        /// @return the converted alignment
        static t_uint32 convert_align_to_mask(t_uint32 align);

        /// @brief Convert an alignement property to a power of 2 one. For example, 0xFF would
        /// be converted into 0x100 and 0x100 would remained the same
        /// @param [in] align the alignment to convert
        /// @return the converted alignment
        static t_uint32 convert_align_to_power2(t_uint32 align);

#ifdef _CACHE_OPT_
        /// @brief Make a cache operation (clean / invalidate) on a memory
        /// @param[in] aOp The operation to perform (clean / invalidate)
        /// @return OMX_ErrorNone if no error, an error otherwise
        OMX_ERRORTYPE cacheOperation(MMHwBuffer::TSyncCacheOperation aOp, t_uint8 *LogicalAddress, t_uint32 size) const;
#endif

        /// @brief Return whether or not the memory has a physical address
        /// @return true if this memory has a physical memory
        int hasPhysicalAddress() const   { return mType!=VFM_MEM_NEW; }

    public:
        /// @brief Specific information for the memory. This depends
        /// on the type of the allocation that has been used (new, CM_AllocMPC,...)
        union {
            /// @brief handle used for VFM_MEM_NEW memory type
            VFM_MemoryLogicalPhysical handle_new;

//#ifndef WORKSTATION
#if !(defined WORKSTATION || defined __ndk5500_a0__)
            /// @brief handle used for VFM_MEM_CM_MPC_ESRAM16_ALLOC and VFM_MEM_CM_MPC_SDRAM16_ALLOC memory type
            VFM_MemoryCM handle_cmalloc;
#endif

            /// @brief handle used for VFM_MEM_EXTERNALSOURCE memory type
            VFM_MemoryExternal external_source;

            /// @brief handle used for VFM_MEM_HWBUFFER_NONDSP memory type
            VFM_MemoryHwBuffer handle_hwbuffers;
        } mMem;

        /// @brief The memory type. It defines the allocation / free method to use
        VFM_Mem_Type mType;

        /// @brief Size of the allocated memory
        t_uint32 mSize;

        /// @brief Source line the allocation occured
        t_uint32 mLine;

        /// @brief Source filename the allocation occured
        t_uint8 *mFilename;

        /// @brief Next memory in the list
        VFM_Memory *mNext;

        OMX_PTR vfm_memory_omxhandle;
    };



/// @ingroup VFMMemory
/// @brief The list of memory location. This is the context
class VFM_MemoryContext
{
    public:
        /// @brief Constructor
        VFM_MemoryContext(t_uint32 domainDdrId, t_uint32 domainEsramId, t_uint8 fullsw) :
                mFirstSW(0), mFirstHW(0), mFreeFirst(0), mChunkList(0), mMutex(0), mFullSW(fullsw),
                mDomainDdrId(domainDdrId), mDomainEsramId(domainEsramId) {
            memoryContext_OMXHandle = (OMX_PTR)0;
            ptrVFMParam = (VFM_ParamAndConfig *)0;
            memset(&mMemoryStatus, 0, sizeof(mMemoryStatus));
            omxilosalservices::OmxILOsalMutex::MutexCreate(mMutex);
            VFM_MemoryLogicalPhysical::vfm_assert_static((mMutex!=0), __LINE__, OMX_TRUE);
        }

        /// @brief Destructor
        ~VFM_MemoryContext();

        /// @brief Allocate a memory given its type, size and properties. It adds the memory in the list
        /// @param [in] size of the allocation
        /// @param [in] type deprecated
        /// @param [in] align alignement properties.
        ///    @note This should be O with VFM_MEM_NEW
        ///    @note Check the real allocation method (CM_AllocMPC(),...) for set of property
        /// @param [in] prop properties of the allocation (non-cachable,...).
        ///    @note N/A with VFM_MEM_NEW
        /// @param [in] line of the source file where the allocation occurs (used in debug mode)
        /// @param [in] file is the file name where the allocation occurs (used in debug mode)
        /// @return the logical address. This function updates the specific members of the class object with
        ///    logical and physical address correspondances
        /// @note type=VFM_MEM_EXTERNALSOURCE is forbidden. AddMemoryExternalSource() should be used instead of Alloc()
        t_uint8 *Alloc(t_uint32 size, VFM_Mem_Type type, t_uint32 align, t_uint32 prop, t_uint32 line, t_uint8 *file);

        /// @brief Free the memory pointed by pt (logical address). It will search for
        /// this address in the list
        /// @param [in] pt the logical address to free
        void Free(t_uint8 *pt);

        /// @brief Get the physical address given a logical address. It will search for this address
        /// in a range of [address;address+size[ in the list
        /// @param [in] pt the logical address.
        /// @return The corresponding physical address
        /// @note In full software version, the original address pt is returned (no conversion)
        t_uint8 *GetPhysical(t_uint8 *pt) const;

        /// @brief Get the logical address given a physical address. It will search for this address
        /// in a range of [address;address+size[ in the list
        /// @param [in] pt the physical address.
        /// @return The corresponding logical address
        /// @note In full software version, the original address pt is returned  (no conversion)
        t_uint8 *GetLogical(t_uint8 *pt) const;

        /// @brief Get the mpc address given a logical address. It will search for this address
        /// in a range of [address;address+size[ in the list
        /// @param [in] pt the logical address.
        /// @return The corresponding mpec address
        /// @note In full software version, the original address pt is returned  (no conversion)
        t_uint8 *GetMpc(t_uint8 *pt) const;

        /// @brief In debug mode, printf() memory of the list. This is of interest
        /// before closing the list as it checks for memory leaks. It has no
        /// effect in non-debug mode
        t_uint32 CheckMemory() const;

        /// @brief Find, in the list, the item that has the given address
        /// @param [in] pt the pointer address of the item we are searching for
        /// @param [in] list1 the first list of item to search into
        /// @param [in] list2 the second list of item to search into (can be null)
        /// @param [out] prev the previous item in the list
        /// @param [in] FromLogical equals 1 if pt is a logical address, 0 if physical
        /// @return the memory item corresponding to the address pt
        VFM_Memory *FindMemoryItem(t_uint8 *pt, VFM_Memory *list1, VFM_Memory *list2, VFM_Memory **prev, t_uint8 FromLogical=1) const;

        /// @brief Store, in the list, a memory couple when the allocation is already performed outside of
        /// the VFM Memory cope
        /// @param [in] LogicalAddress is the logical address
        /// @param [in] PhysicalAddress is its corresponding physical address
        /// @param [in] size of the allocation
        /// @param [in] pSharedChunk The HW Buffer
        /// @note Memory type will be VFM_MEM_EXTERNALSOURCE
        void AddMemoryExternalSource(t_uint8 *LogicalAddress, t_uint8 *PhysicalAddress, t_uint32 size, MMHwBuffer *pSharedChunk);

        /// @brief Lock the mutex
        void lockMutex();

        /// @brief Release the mutex
        void releaseMutex();

        /// @brief Get the memory status
        /// @return the memory status
        VFM_MemoryStatus getMemoryStatus() const    { return mMemoryStatus; }


        /// @brief Get a freem VFM_Memory location, obtained from the list mFreeList or from a new allocation
        /// if mFreeList==NULL
        /// @return Free VFM_Memory location
        VFM_Memory *GetFreeMemory();

        /// @brief Free, that is set the VFM_Memory, into the list mFreeFirst
        /// @param [in] item the VFM_Memory to free
        void FreeMemory(VFM_Memory *item);

        /// @brief Include this memory item in the correct list, either the SW or the HW one
        /// @param[in] item the item to push
        void pushItem(VFM_Memory *item);

#ifdef _CACHE_OPT_
        /// @brief Invalidate the cache on a memory
        /// @param [in] LogicalAddress The logical address to clean / invalidate.
        ///     The operation is made on the whole HW Buffer
        /// @param [in] size The size of the invalidate (0 to invalidate the whole buffer that contains
        ///     the address
        /// @return OMX_ErrorNone if no error, an error otherwise
        OMX_ERRORTYPE cacheInvalidate(t_uint8 *LogicalAddress, t_uint32 size) const;

        /// @brief Clean the cache on a memory
        /// @param [in] LogicalAddress The logical address to clean / invalidate.
        ///     The operation is made on the whole HW Buffer
        /// @param [in] size The size of the clean (0 to invalidate the whole buffer that contains
        ///     the address
        /// @return OMX_ErrorNone if no error, an error otherwise
        OMX_ERRORTYPE cacheClean(t_uint8 *LogicalAddress, t_uint32 size) const;
#endif

#ifdef __ndk5500_a0__
        /// @brief Update the esram domain
        /// @param[in] domainEsramId the new esram domain
        inline void updateESramDomain(t_uint32 domainEsramId)    { mDomainEsramId = domainEsramId; }
#endif
    private:
        /// @brief First item SW (no HW address) of the list
        VFM_Memory *mFirstSW;

        /// @brief First item HW (HW address exists) of the list
        VFM_Memory *mFirstHW;

        /// @brief First free item of the list
        VFM_Memory *mFreeFirst;

        /// @brief to be documented
        VFM_MemoryHwBufferChunk *mChunkList;

        /// @brief Current allocated size, per type
        VFM_MemoryStatus mMemoryStatus;

        /// @brief Mutex to allow a single access to the VFM at the same time
        omxilosalservices::OmxILOsalMutex* mMutex;

    public:
        /// @brief Indicate if full-software version or not
        t_uint8 mFullSW;

         /// @brief Default domain ID for allocation of ddr
        t_cm_domain_id mDomainDdrId;

         /// @brief Default domain ID for allocation of esram
        t_cm_domain_id mDomainEsramId;

        OMX_PTR memoryContext_OMXHandle;

        VFM_ParamAndConfig *ptrVFMParam;
};



VFM_Mem_Type VFM_Memory::TypeConversion(VFM_Mem_Type type)
{

    switch (type) {
    case VFM_MEM_NEW:                       return VFM_MEM_NEW;
    case VFM_MEM_EXTERNALSOURCE:            return VFM_MEM_EXTERNALSOURCE;
#if defined WORKSTATION || defined __ndk5500_a0__
    case VFM_MEM_CM_MPC_ESRAM16_ALLOC:      return VFM_MEM_HWBUFFER_NONDSP;
    case VFM_MEM_CM_MPC_SDRAM16_ALLOC:      return VFM_MEM_HWBUFFER_NONDSP;
#else
    case VFM_MEM_CM_MPC_ESRAM16_ALLOC:      return VFM_MEM_CM_MPC_ESRAM16_ALLOC;
    case VFM_MEM_CM_MPC_SDRAM16_ALLOC:      return VFM_MEM_CM_MPC_SDRAM16_ALLOC;
#endif
    case VFM_MEM_HWBUFFER_NONDSP:           return VFM_MEM_HWBUFFER_NONDSP;
    case VFM_MEM_CACHED_HWBUFFER_NONDSP:    return VFM_MEM_CACHED_HWBUFFER_NONDSP;
    default:
#ifdef VFM_DEBUG
        printf("VFM_Memory::TypeConversion(): wrong type %d\n", type);
#endif
        VFM_MemoryLogicalPhysical::vfm_assert_static((0==1), __LINE__, OMX_TRUE);
        break;
    }
    return VFM_MEM_NEW;
}

t_uint32 VFM_Memory::convert_align_to_mask(t_uint32 align)
{
    if (align==0 || align==1)   return align;
    if (!(align & 0x1)) {
        return align - 1;
    } else {
        return align;
    }
}

t_uint32 VFM_Memory::convert_align_to_power2(t_uint32 align)
{
    if (align==0 || align==1)   return 1;
    if (align & 0x1) {
        return align + 1;
    } else {
        return align;
    }
}

t_uint8 *VFM_Memory::Alloc(t_uint32 size, VFM_Mem_Type type, t_uint32 align, t_uint32 prop, t_uint32 line, t_uint8 *file, VFM_MemoryHwBufferChunk **chunk_list,
        t_cm_domain_id domainDdrId, t_cm_domain_id domainEsramId)
{
    DBC_PRECONDITION(mType == VFM_MEM_NONE);
    type = TypeConversion(type);
    t_uint32 size4;
    t_uint8 *res;
    switch (type) {
    case VFM_MEM_NEW:
        VFM_MemoryLogicalPhysical::vfm_assert_static((align==0 && prop==0), __LINE__, OMX_TRUE);
        size4 = size/4;
        if (size4*4 < size) {
            size4++;
        }

        res = (t_uint8 *)new t_uint32[size4];
        mMem.handle_new.mLogical = res;
        mMem.handle_new.mPhysical = 0;
        if (res == 0) {
            return 0;
        }
        break;
//#ifndef WORKSTATION
#if !(defined WORKSTATION || defined __ndk5500_a0__)
    case VFM_MEM_CM_MPC_ESRAM16_ALLOC:
    case VFM_MEM_CM_MPC_SDRAM16_ALLOC:
        {
            // update the alignment
            t_cm_mpc_memory_type memory_type=0;
            t_cm_domain_id thisDomainId=0;
            switch (type) {
            case VFM_MEM_CM_MPC_ESRAM16_ALLOC: memory_type = CM_MM_MPC_ESRAM16; thisDomainId=domainEsramId; break;
            case VFM_MEM_CM_MPC_SDRAM16_ALLOC: memory_type = CM_MM_MPC_SDRAM16; thisDomainId=domainDdrId; break;
            //default: VFM_MemoryLogicalPhysical::vfm_assert_static((0==1), __LINE__, OMX_TRUE); break;
            default:
                break;
            }
            unsigned int size_in_16bits = size/2;
            if (size_in_16bits*2<size) {
                size_in_16bits ++;
            }
            align = convert_align_to_mask(align);
            align = align*2+1;
            if (CM_OK != CM_AllocMpcMemory(thisDomainId, memory_type, size_in_16bits, align, &(mMem.handle_cmalloc.handle))) {
                return 0;
            }
            mMem.handle_cmalloc.set_logical_physical_cm_mpc();
        }
        break;
#endif

    case VFM_MEM_HWBUFFER_NONDSP:
       // update the alignment
        align = convert_align_to_power2(align);
        mMem.handle_hwbuffers.VFM_MemoryHwBuffer_omxhandle = vfm_memory_omxhandle;
        if (OMX_ErrorNone != mMem.handle_hwbuffers.setChunk(size, align, chunk_list,OMX_FALSE)) {
            return 0;
        }
        break;

    case VFM_MEM_CACHED_HWBUFFER_NONDSP:
       // update the alignment
        align = convert_align_to_power2(align);
        mMem.handle_hwbuffers.VFM_MemoryHwBuffer_omxhandle = vfm_memory_omxhandle;
        if (OMX_ErrorNone != mMem.handle_hwbuffers.setChunk(size, align, chunk_list,OMX_TRUE)) {
            return 0;
        }
     break;
    default:
#ifdef VFM_DEBUG
        printf("VFM_Memory::Alloc(): wrong type %d\n", type);
#endif
        VFM_MemoryLogicalPhysical::vfm_assert_static((0==1), __LINE__, OMX_TRUE);
        break;
    }

    mType = type;
    mSize = size;
    mLine = line;
    mFilename = file;

#ifdef VFM_DEBUG
    printf("VFM_Memory addr=%p  size=%x (%d) type=%d align=%d %d %s\n",GetLogicalAddress(),size,size,mType,align,mLine,mFilename);
#endif
    return GetLogicalAddress();
}

void VFM_Memory::AddMemoryExternalSource(t_uint8 *LogicalAddress, t_uint8 *PhysicalAddress, t_uint32 size, void *pMetaData)
{
    DBC_PRECONDITION(mType == VFM_MEM_NONE);
    mMem.external_source.mLogical = LogicalAddress;
    mMem.external_source.mPhysical = PhysicalAddress;
    mMem.external_source.mSharedChunk = (MMHwBuffer *)pMetaData;
    mType = VFM_MEM_EXTERNALSOURCE;
    mSize = size;
    mLine = 0;
    mFilename = (t_uint8 *)"Allocation from unknown source";
}

void VFM_Memory::Free(VFM_MemoryHwBufferChunk **chunk_list)
{
    switch (mType) {
    case VFM_MEM_NEW:
        VFM_MemoryLogicalPhysical::vfm_assert_static((mMem.handle_new.mLogical!=0), __LINE__, OMX_TRUE);
        delete [] mMem.handle_new.mLogical;
        break;
//#ifndef WORKSTATION
#if !(defined WORKSTATION || defined __ndk5500_a0__)
    case VFM_MEM_CM_MPC_ESRAM16_ALLOC:
    case VFM_MEM_CM_MPC_SDRAM16_ALLOC:
        CM_FreeMpcMemory(mMem.handle_cmalloc.handle);
        break;
#endif
    case VFM_MEM_EXTERNALSOURCE:
        // nothing to do. The free will be done by the external source
        break;
    case VFM_MEM_HWBUFFER_NONDSP:
    case VFM_MEM_CACHED_HWBUFFER_NONDSP:
        mMem.handle_hwbuffers.Free(chunk_list);
        break;
    default:
#ifdef VFM_DEBUG
        printf("VFM_Memory::Free(): wrong type %d\n", mType);
#endif
        VFM_MemoryLogicalPhysical::vfm_assert_static((0==1), __LINE__, OMX_TRUE);
        break;
    }
}

#ifdef _CACHE_OPT_
OMX_ERRORTYPE VFM_Memory::cacheOperation(MMHwBuffer::TSyncCacheOperation aOp, t_uint8 *LogicalAddress, t_uint32 size) const
{
    t_uint8 *address_toflush;
    t_uint32 size_toflush;
    if (size==0) {
        address_toflush = 0;
        size_toflush = mSize;
    } else {
        address_toflush = LogicalAddress;
        size_toflush = size;
    }

    switch (mType) {
        case VFM_MEM_EXTERNALSOURCE:
            return mMem.external_source.cacheOperation(aOp, address_toflush, size_toflush);

        case VFM_MEM_CACHED_HWBUFFER_NONDSP:
            return mMem.handle_hwbuffers.cacheOperation(aOp, address_toflush, size_toflush);

        case VFM_MEM_HWBUFFER_NONDSP:
            return OMX_ErrorNone;

        default:
            return OMX_ErrorUndefined;
    }
}
#endif

t_uint8 *VFM_Memory::GetLogicalAddress() const
{
    VFM_MemoryLogicalPhysical *pMem = (VFM_MemoryLogicalPhysical *)(&mMem);
    return pMem->mLogical;
}

t_uint8 *VFM_Memory::GetPhysicalAddress() const
{
    VFM_MemoryLogicalPhysical *pMem = (VFM_MemoryLogicalPhysical *)(&mMem);
    return pMem->mPhysical;
}

t_uint8 *VFM_Memory::GetMpcAddress() const
{
    VFM_MemoryLogicalPhysical *pMem = (VFM_MemoryLogicalPhysical *)(&mMem);
    return pMem->mMpc;
}

VFM_Memory *VFM_MemoryContext::GetFreeMemory()
{
    if (mFreeFirst) {
        VFM_Memory *item = mFreeFirst;
        mFreeFirst = mFreeFirst->mNext;
        return item;
    } else {
        return new VFM_Memory;
    }
}

void VFM_MemoryContext::FreeMemory(VFM_Memory *item)
{
    item->mType = VFM_MEM_NONE;
    item->mNext = mFreeFirst;
    mFreeFirst = item;
}


void VFM_MemoryContext::pushItem(VFM_Memory *item)
{
    if (item->hasPhysicalAddress()) {
        item->mNext = mFirstHW;
        mFirstHW = item;
    } else {
        item->mNext = mFirstSW;
        mFirstSW = item;
    }
}

t_uint8 *VFM_MemoryContext::Alloc(t_uint32 size, VFM_Mem_Type type, t_uint32 align, t_uint32 prop, t_uint32 line, t_uint8 *file)
{
    VFM_Memory *item = GetFreeMemory();
    if (item==0) return 0;
    VFM_MemoryHwBufferChunk *saved_chunk_list=mChunkList;
    item->setomxhandle(&memoryContext_OMXHandle); //<<PG>>
    t_uint8 *pt = item->Alloc(size, type, align, prop, line, file, &mChunkList, mDomainDdrId, mDomainEsramId);
    if (!pt) {
        FreeMemory(item);
        return 0;
    }
    pushItem(item);
    if (item->mType!=VFM_MEM_HWBUFFER_NONDSP && item->mType!=VFM_MEM_CACHED_HWBUFFER_NONDSP) {
        mMemoryStatus.mCurrentSize[item->mType] += size;
    } else {
        if (saved_chunk_list != mChunkList) {
            t_uint32 allocated_size;
            // a new chunk has been allocated. We have to use the size of the complete allocated memory
            if ((size % VFM_MemoryHwBufferChunk::mMinSize) == 0) {
                allocated_size = (size / VFM_MemoryHwBufferChunk::mMinSize) * VFM_MemoryHwBufferChunk::mMinSize;
            } else {
                allocated_size = (size / VFM_MemoryHwBufferChunk::mMinSize + 1) * VFM_MemoryHwBufferChunk::mMinSize;
            }
            mMemoryStatus.mCurrentSize[item->mType] += allocated_size;
        }
        mMemoryStatus.mCurrentNbItem[item->mType] ++;
    }
    mMemoryStatus.mCurrentNbItem[item->mType] ++;
    if (mMemoryStatus.mMaxSize[item->mType]<mMemoryStatus.mCurrentSize[item->mType]) {
        mMemoryStatus.mMaxSize[item->mType] = mMemoryStatus.mCurrentSize[item->mType];
    }
    if (mMemoryStatus.mMaxNbItem[item->mType]<mMemoryStatus.mCurrentNbItem[item->mType]) {
        mMemoryStatus.mMaxNbItem[item->mType] = mMemoryStatus.mCurrentNbItem[item->mType];
    }
    return pt;
}

void VFM_MemoryContext::AddMemoryExternalSource(t_uint8 *LogicalAddress, t_uint8 *PhysicalAddress, t_uint32 size, MMHwBuffer *pSharedChunk)
{
    VFM_Memory *item = GetFreeMemory();
    DBC_POSTCONDITION (item);
    item->AddMemoryExternalSource(LogicalAddress, PhysicalAddress, size, pSharedChunk);
    pushItem(item);
    mMemoryStatus.mCurrentNbItem[item->mType] ++;
    if (mMemoryStatus.mMaxNbItem[item->mType]<mMemoryStatus.mCurrentNbItem[item->mType]) {
        mMemoryStatus.mMaxNbItem[item->mType] = mMemoryStatus.mCurrentNbItem[item->mType];
    }
}

void VFM_MemoryContext::Free(t_uint8 *pt)
{
	PRINTPG("Value of buffer to be freed 0x%x \n",(OMX_U32)pt);
    VFM_Memory *prev, *item = FindMemoryItem(pt, mFirstSW, mFirstHW, &prev);
    /* some pointers which are not allocated so to avoid this assert, adding print so
    as to keep track of those - AS CHECK */
    if (!item)
    {
		PRINTPG(" Returning without deletion for pointer 0x%x \n",(OMX_U32)pt);
		return;
	}
    VFM_MemoryLogicalPhysical::vfm_assert_static((item!=0 && pt==item->GetLogicalAddress()), __LINE__, OMX_TRUE);
#ifdef VFM_DEBUG
    /*
    printf("VFM_MemoryContext::Free(): 0x%x, of size %d, allocated at line %d, file %s\n",
        pt, item->mMemory.mSize, item->mMemory.mLine, item->mMemory.mFilename);
    */
#endif
    if (prev) {
        prev->mNext = item->mNext;
    } else {
        if (item->hasPhysicalAddress()) {
            mFirstHW = item->mNext;
        } else {
            mFirstSW = item->mNext;
        }
    }
    if (item->mType != VFM_MEM_EXTERNALSOURCE) {
        mMemoryStatus.mCurrentSize[item->mType] -= item->mSize;
    }
    mMemoryStatus.mCurrentNbItem[item->mType] --;
    item->Free(&mChunkList);
    FreeMemory(item);
}

t_uint8 *VFM_MemoryContext::GetPhysical(t_uint8 *pt) const
{
    if (!pt) {
        return 0;
    }
    VFM_Memory *item = FindMemoryItem(pt, mFirstHW, 0, 0);
	PRINTPG("VFM_MemoryContext::GetPhysical Value of item : 0x%x",(OMX_U32)item);
	if (item)
	{
    return pt - item->GetLogicalAddress() + item->GetPhysicalAddress();
}
	else
	{
		return 0;
	}
}

t_uint8 *VFM_MemoryContext::GetLogical(t_uint8 *pt) const
{
    if (!pt) {
        return 0;
    }
    VFM_Memory *item = FindMemoryItem(pt, mFirstHW, 0, 0, 0);
	PRINTPG("VFM_MemoryContext::GetPhysical Value of item : 0x%x",(OMX_U32)item);
	if (item)
	{
    return pt - item->GetPhysicalAddress() + item->GetLogicalAddress();
}
	else
	{
		return 0;
	}
}

t_uint8 *VFM_MemoryContext::GetMpc(t_uint8 *pt) const
{
    if (!pt) {
        return 0;
    }
    VFM_Memory *item = FindMemoryItem(pt, mFirstHW, 0, 0, 1);
    VFM_MemoryLogicalPhysical::vfm_assert_static((item!=0), __LINE__, OMX_TRUE);
    return pt - item->GetLogicalAddress() + item->GetMpcAddress();
}


#ifdef _CACHE_OPT_
OMX_ERRORTYPE VFM_MemoryContext::cacheClean(t_uint8 *LogicalAddress, t_uint32 size) const
{
    VFM_Memory *item = FindMemoryItem(LogicalAddress, mFirstHW, mFirstSW, 0 );
    VFM_MemoryLogicalPhysical::vfm_assert_static((item!=0), __LINE__, OMX_TRUE);
    return item->cacheOperation(MMHwBuffer::ESyncBeforeReadHwOperation, LogicalAddress, size);
}

OMX_ERRORTYPE VFM_MemoryContext::cacheInvalidate(t_uint8 *LogicalAddress, t_uint32 size) const
{
    VFM_Memory *item = FindMemoryItem(LogicalAddress, mFirstHW, mFirstSW, 0 );
    VFM_MemoryLogicalPhysical::vfm_assert_static((item!=0), __LINE__, OMX_TRUE);
    return item->cacheOperation(MMHwBuffer::ESyncAfterWriteHwOperation, LogicalAddress, size);
}
#endif

VFM_Memory *VFM_MemoryContext::FindMemoryItem(t_uint8 *pt, VFM_Memory *list1, VFM_Memory *list2, VFM_Memory **prev, t_uint8 FromLogical) const
{
    t_uint8 *address;
    VFM_Memory *localprev;
    VFM_Memory *item;
    if (prev) {
        if (FromLogical) {
            for (int i=0; i<2; i++) {
                if (i==0) {
                    item = list1;
                } else {
                    item = list2;
                }
                localprev=0;
                for (/* item already  initialized */; item; item=item->mNext) {
                    VFM_MemoryLogicalPhysical *pMem = (VFM_MemoryLogicalPhysical *)(&item->mMem);
                    address = pMem->mLogical;
                    // address = item->GetLogicalAddress();
                    if (address<=pt && pt<address+item->mSize) {
                        *prev = localprev;
                        return item;
                    }
                    localprev = item;
                }
            }
        } else {
            // we only scan the HW list
            VFM_MemoryLogicalPhysical::vfm_assert_static((list2==0), __LINE__, OMX_TRUE);
            localprev=0;
            for (VFM_Memory *item = mFirstHW; item; item=item->mNext) {
                VFM_MemoryLogicalPhysical *pMem = (VFM_MemoryLogicalPhysical *)(&item->mMem);
                address = pMem->mPhysical;
                // address = item->GetPhysicalAddress();
                if (address && address<=pt && pt<address+item->mSize) {
                    *prev = localprev;
                    return item;
                }
                localprev = item;
            }
        }
    } else {
        if (FromLogical) {
            for (int i=0; i<2; i++) {
                if (i==0) {
                    item = list1;
                } else {
                    item = list2;
                }
                for (/* item already  initialized */; item; item=item->mNext) {
                    VFM_MemoryLogicalPhysical *pMem = (VFM_MemoryLogicalPhysical *)(&item->mMem);
                    address = pMem->mLogical;
                    // address = item->GetLogicalAddress();
                    if (address<=pt && pt<address+item->mSize) {
                        return item;
                    }
                }
            }
        } else {
            // we only scan the HW list
            VFM_MemoryLogicalPhysical::vfm_assert_static((list2==0), __LINE__, OMX_TRUE);
            for (VFM_Memory *item = mFirstHW; item; item=item->mNext) {
                VFM_MemoryLogicalPhysical *pMem = (VFM_MemoryLogicalPhysical *)(&item->mMem);
                address = pMem->mPhysical;
                // address = item->GetPhysicalAddress();
                if (address && address<=pt && pt<address+item->mSize) {
                    return item;
                }
            }
        }
    }
#ifdef VFM_DEBUG
    printf("VFM_MemoryContext::FindMemoryItem(): Cannot find pointer 0x%x in list\n", pt);
#endif

	PRINTPG("Item Not found in Memory List");
    return 0;
}

VFM_MemoryContext::~VFM_MemoryContext()
{
    VFM_Memory *item=0, *next;
    // Free the allocated item. Ideally, this should be empty
    for (int i=0; i<3; i++) {
        switch (i) {
        case 0: item = mFirstSW; break;
        case 1: item = mFirstHW; break;
        case 2: item = mFreeFirst; break;
        default: VFM_MemoryLogicalPhysical::vfm_assert_static((0==1), __LINE__, OMX_TRUE); break;
        }
        while (item) {
            next = item->mNext;
            if (i!=2) {
                item->Free(&mChunkList);
            }
            delete item;
            item = next;
        }
    }

    // Free the mutex
    VFM_MemoryLogicalPhysical::vfm_assert_static((mMutex!=0), __LINE__, OMX_TRUE);
    omxilosalservices::OmxILOsalMutex::MutexFree(mMutex);
}

#if (defined VFM_DEBUG || defined VFM_CHECK_LEAKS || defined VFM_STATUS_REPORT)
t_uint32 VFM_MemoryContext::CheckMemory() const
{
    t_uint32 size=0;
    VFM_Memory *item;
    for (int i=0; i<2; i++) {
        switch (i) {
        case 0: item = mFirstSW; break;
        case 1: item = mFirstHW; break;
        default: VFM_MemoryLogicalPhysical::vfm_assert_static((0==1), __LINE__, OMX_TRUE); break;
        }

        t_uint32 nb=0;
        for (/* item is already initialized */ ; item; item=item->mNext) {
            if (item->mType == VFM_MEM_EXTERNALSOURCE) continue;
            size += item->mSize;
            if (nb < 50) {
                printf("CheckMemory(): %d bytes allocated at line %d, file %s\n", item->mSize, item->mLine, item->mFilename);
            }
            nb ++;
        }
        if (nb>=50) {
            printf("CheckMemory(): ... and many others\n");
        }
    }

    for (int i=0; i<VFM_MEM_LAST; i++) {
        printf("CheckMemory(): Max size [%d] = %d bytes\n", i, mMemoryStatus.mMaxSize[i]);
    }
    return size;
}
#else
t_uint32 VFM_MemoryContext::CheckMemory() const
{
    t_uint32 size=0;
    VFM_Memory *item=0;

    for (int i=0; i<2; i++) {
        switch (i) {
        case 0: item = mFirstSW; break;
        case 1: item = mFirstHW; break;
        default: VFM_MemoryLogicalPhysical::vfm_assert_static((0==1), __LINE__, OMX_TRUE); break;
        }

        for (/* item is already initialized */ ; item; item=item->mNext) {
            if (item->mType == VFM_MEM_EXTERNALSOURCE) continue;
            size += item->mSize;
        }
    }
    return size;
}
#endif

void VFM_MemoryContext::lockMutex()
{
    mMutex->MutexLock();
}

void VFM_MemoryContext::releaseMutex()
{
    mMutex->MutexUnlock();
}

extern "C" EXPORT_C void *VFM_CreateMemoryList(t_uint32 domainDdrId, t_uint32 domainEsramId, t_uint8 fullsw)
{
    VFM_MemoryContext *ctxt;
    VFM_MemoryLogicalPhysical::vfm_assert_static((VFM_MEM_LAST_IDT == VFM_MEM_LAST), __LINE__, OMX_TRUE);
    ctxt = new VFM_MemoryContext(domainDdrId, domainEsramId, fullsw);
    PRINTPG("In VFM_CreateMemoryList ctxt : 0x%x \n",(OMX_U32)ctxt);
    return  ctxt;
}

extern "C" EXPORT_C void VFM_InitParamConfig(void *ctxt,void *ptrParam)
{
	PRINTPG("In VFM_InitParamConfig ctxt : 0x%x and ptrParam 0x%x \n",(OMX_U32)ctxt,(OMX_U32)ptrParam);
	VFM_MemoryContext *list = (VFM_MemoryContext *)ctxt;
	list->ptrVFMParam = (VFM_ParamAndConfig *)ptrParam;
}

#ifdef __ndk5500_a0__
extern "C" EXPORT_C void VFM_UpdateESramDomain(void *ctxt, t_uint32 domainEsramId)
{
    VFM_MemoryContext *list = (VFM_MemoryContext *)ctxt;
    list->lockMutex();
    list->updateESramDomain(domainEsramId);
    list->releaseMutex();
}
#endif

extern "C" EXPORT_C void VFM_MemoryInit(void *ctxt,OMX_PTR pOMXHandle)
{
#ifdef VFM_DEBUG
        printf("VFM_MemoryInit omx handle : 0x%x",pOMXHandle);
#endif
	VFM_MemoryContext *list = (VFM_MemoryContext *)ctxt;
	if (list == 0)
	{
		VFM_TraceError(__LINE__, OMX_ErrorUndefined, __LINE__, 0);
	}
	else
	{
		list->memoryContext_OMXHandle = pOMXHandle;
	}
}

extern "C" EXPORT_C t_uint8 *VFM_Alloc(void *ctxt, t_uint32 size, VFM_Mem_Type type, t_uint32 align, t_uint32 prop, t_uint32 line, t_uint8 *file)
{
    VFM_MemoryContext *list = (VFM_MemoryContext *)ctxt;
    list->lockMutex();
    t_uint8 *res = list->Alloc(size, type, align, prop, line, file);
    list->releaseMutex();
    PRINTPG("Inside VFM_Alloc ctxt : 0x%x pointer 0x%x \n",(OMX_U32)ctxt,(OMX_U32)res);
    return res;
}

extern "C" EXPORT_C void VFM_Free(void *ctxt, t_uint8 *pt)
{
    VFM_MemoryContext *list = (VFM_MemoryContext *)ctxt;
    PRINTPG("Inside VFM_Free ctxt : 0x%x pointer 0x%x \n",(OMX_U32)ctxt,(OMX_U32)pt);
    if ((list->ptrVFMParam) &&
        (list->ptrVFMParam)->getStoreMetadataInBuffers())
    {
		PRINTPG("Inside VFM_Free ctxt : Checking for Valid Address \n");
		t_uint8 *res=0;
		res = (t_uint8 *)(list->ptrVFMParam)->getBufferPhysicalAddress((OMX_U32)pt);
		if (res)
		{
			PRINTPG("Inside VFM_Free ctxt : 0x%x pointer 0x%x NOT TO FREE further \n",(OMX_U32)ctxt,(OMX_U32)pt);
			return;
		}
	}
    list->lockMutex();
    list->Free(pt);
    list->releaseMutex();
}

extern "C" EXPORT_C t_uint32 VFM_CloseMemoryList(void *ctxt)
{
    VFM_MemoryContext *list = (VFM_MemoryContext *)ctxt;
    list->lockMutex();
    int status=list->CheckMemory();
    list->releaseMutex();
    delete list;        // to be done once the mutex is released
    return status;
}

extern "C" EXPORT_C t_uint8 *VFM_GetPhysical(void *ctxt, t_uint8 *LogicalAddress)
{
    PRINTPG("In VFM_GetPhysical , ctxt =0x%x logicalAddr=0x%x\n", ctxt,LogicalAddress);
    VFM_MemoryContext *list = (VFM_MemoryContext *)ctxt;
    list->lockMutex();
    t_uint8 *res = 0;
    PRINTPG("line num =%d list =%p, vfmparam = %p",__LINE__, list, list->ptrVFMParam);

    if (list->mFullSW) {
        res = LogicalAddress;
    } else {
        res = list->GetPhysical(LogicalAddress);
    }

    if (!(res) && (list->ptrVFMParam) &&
        (list->ptrVFMParam)->getStoreMetadataInBuffers())
	{
	     PRINTPG("SEARCHING IN METADATA ::: line num =%d Address: 0x%x \n",__LINE__,res);
		res = (t_uint8 *)(list->ptrVFMParam)->getBufferPhysicalAddress((OMX_U32)LogicalAddress);
	}
    list->releaseMutex();
    PRINTPG(" Returning from vfm_getphycal res: 0x%x",__LINE__,res);
    DBC_ASSERT(res != 0);
    return res;
}


extern "C" EXPORT_C t_uint8 *VFM_GetLogical(void *ctxt, t_uint8 *PhysicalAddress)
{
	PRINTPG("In VFM_GetLogical , ctxt =0x%x PhysicalAddress=0x%x\n", ctxt,PhysicalAddress);
    VFM_MemoryContext *list = (VFM_MemoryContext *)ctxt;
    list->lockMutex();
    t_uint8 *res=0;
    if (list->mFullSW) {
        res = PhysicalAddress;
    } else {
        res = list->GetLogical(PhysicalAddress);
    }

    if (!(res) && (list->ptrVFMParam) &&
        (list->ptrVFMParam)->getStoreMetadataInBuffers())
	{
	    PRINTPG(" SEARCHING IN METADATA =%d res: 0x%x",__LINE__,res);
		res = (t_uint8 *)(list->ptrVFMParam)->getBufferLogicalAddress((OMX_U32)PhysicalAddress);
	}
    list->releaseMutex();
    PRINTPG(" Returning from vfm_getlogical OLD line num =%d res: 0x%x",__LINE__,res);
    DBC_ASSERT(res != 0);
    return res;
}

extern "C" EXPORT_C t_uint8 *VFM_GetMpc(void *ctxt, t_uint8 *LogicalAddress)
{
    VFM_MemoryContext *list = (VFM_MemoryContext *)ctxt;
    list->lockMutex();
    t_uint8 *res;
    if (list->mFullSW) {
        res = LogicalAddress;
    } else {
        res = list->GetMpc(LogicalAddress);
    }
    list->releaseMutex();
    return res;
}

extern "C" EXPORT_C void VFM_AddMemoryExternalSource(void *ctxt, t_uint8 *LogicalAddress, t_uint8 *PhysicalAddress, t_uint32 size, void *pMetaData)
{
    VFM_MemoryContext *list = (VFM_MemoryContext *)ctxt;
    list->lockMutex();
    if (list->mFullSW) {
        list->AddMemoryExternalSource(LogicalAddress, LogicalAddress, size, (MMHwBuffer *)pMetaData);
    } else {
        list->AddMemoryExternalSource(LogicalAddress, PhysicalAddress, size, (MMHwBuffer *)pMetaData);
    }
    list->releaseMutex();
}

extern "C" EXPORT_C void VFM_GetMemoryListStatus(void *ctxt, VFM_MemoryStatus *pMemoryStatus)
{
    VFM_MemoryContext *list = (VFM_MemoryContext *)ctxt;
    list->lockMutex();
    *pMemoryStatus = list->getMemoryStatus();
    list->releaseMutex();
}

#ifndef STRINGIFY_ENTRY
#define STRINGIFY_ENTRY(enum) case enum: return #enum
#endif
extern "C" EXPORT_C const char* VFM_MemTypeStringify(VFM_Mem_Type type)
{
    switch(type)
    {
    STRINGIFY_ENTRY(VFM_MEM_NONE);
    STRINGIFY_ENTRY(VFM_MEM_NEW);
    STRINGIFY_ENTRY(VFM_MEM_CM_MPC_ESRAM16_ALLOC);
    STRINGIFY_ENTRY(VFM_MEM_CM_MPC_SDRAM16_ALLOC);
    STRINGIFY_ENTRY(VFM_MEM_EXTERNALSOURCE);
    STRINGIFY_ENTRY(VFM_MEM_HWBUFFER_NONDSP);
    STRINGIFY_ENTRY(VFM_MEM_CACHED_HWBUFFER_NONDSP);
    default: return "<unknown VFM_Mem_type>";
    }
};

#ifdef _CACHE_OPT_
extern "C" EXPORT_C void VFM_CacheInvalidate(void *ctxt, t_uint8 *LogicalAddress, t_uint32 size)
{
    OMX_ERRORTYPE err;
    VFM_MemoryContext *list = (VFM_MemoryContext *)ctxt;
    list->lockMutex();
    err = list->cacheInvalidate(LogicalAddress, size);
    VFM_MemoryLogicalPhysical::vfm_assert_static((err==OMX_ErrorNone), __LINE__, OMX_TRUE);
    list->releaseMutex();
    return ;
}

extern "C" EXPORT_C void VFM_CacheClean(void *ctxt, t_uint8 *LogicalAddress, t_uint32 size)
{
    OMX_ERRORTYPE err;
    VFM_MemoryContext *list = (VFM_MemoryContext *)ctxt;
    list->lockMutex();
    err = list->cacheClean(LogicalAddress, size);
    VFM_MemoryLogicalPhysical::vfm_assert_static((err==OMX_ErrorNone), __LINE__, OMX_TRUE);
    list->releaseMutex();
    return ;
}

#endif
