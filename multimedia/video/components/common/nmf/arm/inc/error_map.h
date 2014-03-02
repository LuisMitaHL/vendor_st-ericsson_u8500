/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef _ERROR_MAP_H_
#define _ERROR_MAP_H_

#include "omxilosalservices.h"

typedef unsigned int size_t;
#define _SIZE_ERROR_MAP_ (((1920/16)*((1080/16)+1))/8)+1
extern "C" IMPORT_C void *memset(void *s, int c, size_t n);

class FastUpdate_Map {
    public:
        FastUpdate_Map() : mEnableVFU(1), mFirstGOB(0), mFirstMB(0), mNumMBs(0) {};
    
        void reset() { mFirstGOB = mFirstMB = mNumMBs = 0; }
            
    public:
        /// @brief Indicate if fast update is enabled or not
        t_bool mEnableVFU;
    
        /// @brief Contains the number of the first row of macroblocks
        OMX_U32 mFirstGOB;  
        
        /// @brief The location of the first macroblock row relative to the first group of blocks (GOB)
        OMX_U32 mFirstMB; 

        /// @brief The number of macroblocks to be refreshed from the nFirstGOB and nFirstMB
        OMX_U32 mNumMBs;
};

class Common_Error_Map {
    public:
        Common_Error_Map() : mIsAvailable(0), mEventToSent(1), mIsErrorMapAvailable(1) {
            memset(mErrorMap, 0, sizeof(mErrorMap)); 
            omxilosalservices::OmxILOsalMutex::MutexCreate(mMutex);
        }
            
        ~Common_Error_Map() { 
            omxilosalservices::OmxILOsalMutex::MutexFree(mMutex);
        }
        
        /// @brief Lock the mutex
        inline void lockMutex()    { mMutex->MutexLock(); }
        
        /// @brief Release the mutex
        inline void releaseMutex() { mMutex->MutexUnlock(); } ;
        
        /// @brief Try to lock the mutex
        /// @return No error if the mutex has been locked properly, an error if it was already locked
        /// @note FIXME: right now, it does a lock
        inline OMX_ERRORTYPE tryLockMutex()
        { return mMutex->MutexTryLock(); } 
                
        /// @brief Reset the error map
        /// @param [in] width The width of the frame
        /// @param [in] height The height of the frame
        inline void reset(t_uint32 width, t_uint32 height) { 
            reset_with_size(size_error_map(width, height)); 
        } ;
        
        /// @brief Reset the error map
        /// @param [in] size The size of the errormap to reset, in bytes
        inline void reset_with_size(t_uint32 size) { 
            memset(mErrorMap, 0, size); 
            mFastUpdate.reset();
            mIsAvailable = 0;
            mEventToSent = 1;
        } ;
        
        /// @brief Reset the error map
        /// @param [in] size The size of the errormap to reset, in bytes
        inline void reset_with_size_but_notevent(t_uint32 size) { 
            memset(mErrorMap, 0, size); 
            mFastUpdate.reset();
            mIsAvailable = 0;
        } ;

        /// @brief Return the number of macro-blocks given a size
        /// @param [in] size The size, in pixels
        /// @return Number of macro-blocks of size 16, including padding.
        inline static t_uint32 mb_number(t_uint32 size) { 
            return (size+15)/16;
        }
        
        /// @brief Return the sie, in bytes, of the error map
        /// @param [in] width The width of the frame
        /// @param [in] height The height of the frame
        inline static t_uint32 size_error_map(t_uint32 width, t_uint32 height) {
            t_uint32 size = mb_number(width)*mb_number(height);
            return (size+7)/8;
        }
        
    public:
        /// @brief The error map, as defined in OMX specification: 
        /// 
        /// The format of the macroblock map is a bit mapped string of values that 
        /// corresponds to each macroblock within the video frame, when the bit value is set 
        /// it indicates that the corresponding macroblock is to be refreshed as an intra 
        /// macroblock. 
        /// 
        /// As an example, a video frame having a resolution of 176x144 contains 99 
        /// macroblocks thus the macroblock map will contain 99 bit mapped values 
        /// identifying each and every macroblock within the frame (the nErrMapSize 
        /// parameter will contain a size of 13 – rounded up to the nearest byte boundary). Bit 
        /// 0 of the macroblock map refers to macroblock 0 within the video frame, bit 1 
        /// refers to macroblock 1 and so on
        unsigned char mErrorMap[_SIZE_ERROR_MAP_];
        
        /// @brief The fast update structure, as defined in OMX specification
        FastUpdate_Map mFastUpdate;
        
        /// @brief Indicate if an error map is available or not.
        /// For a decoder, the proxy should reset it on a GetConfig, whereas the decoder should set
        /// it when new errors are detected. For a coder, the proxy should set it on a SetConfig, whereas 
        /// the coder should reset it when the error map has been taken into acount
        t_bool mIsAvailable;
        
        /// @brief Indicate if an Event has to be sent or not, when error map information are available.
        /// This is only for decoders. This is set by the proxy when a GetConfig is perform. This is resetted
        /// by the decoder when the event is sent.
        t_bool mEventToSent;
    
        /// @brief When mIsAvailable==true, it indicates if the information is in the ErrorMap (true)
        /// or in the FastUpdate structure (false).
        /// This is only for coders. This is set by the proxy when a SetConfig is received.
        t_bool mIsErrorMapAvailable;
    
    private:
        /// @brief Mutex to avoid concurrent accesses to the ErrorMap at the same time
        omxilosalservices::OmxILOsalMutex* mMutex;
};

#endif
