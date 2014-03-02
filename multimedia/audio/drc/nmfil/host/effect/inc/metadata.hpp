/*****************************************************************************/
/**
*  © ST-Ericsson, 2010 - All rights reserved

*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \brief   OMX metadata class
* \author  ST-Ericsson
*/
/*****************************************************************************/
#ifndef _DRC_NMF_HOST_EFFECT_INC_OMX_METADATA_HPP_
#define _DRC_NMF_HOST_EFFECT_INC_OMX_METADATA_HPP_

#include <OMX_Types.h>

class omx_metadata
{
    public:
        virtual ~omx_metadata() {};

        void setFlags(OMX_U32 flags) {mFlags = flags;}
        void setMarkData(OMX_PTR mark_data) {pMarkData = mark_data;}
        void setFilledLen(OMX_U32 filledlen) {mFilledLen = filledlen;}
        void setTimeStamp(OMX_TICKS timestamp) {mTimeStamp = timestamp;}
        void setMarkTargetComponent(OMX_HANDLETYPE mark_target_component) {pMarkTargetComponent = mark_target_component;}

        OMX_U32 getFlags(void) const {return mFlags;}
        OMX_PTR getMarkData(void) const {return pMarkData;}
        OMX_U32 getFilledLen(void) const {return mFilledLen;}
        OMX_TICKS getTimeStamp(void) const {return mTimeStamp;}
        OMX_HANDLETYPE getMarkTargetComponent(void) const {return pMarkTargetComponent;}

    private:
       OMX_U32           mFilledLen;               // OMX number of bytes in the frame
       OMX_TICKS         mTimeStamp;               // OMX timestamp frame
       OMX_HANDLETYPE    pMarkTargetComponent;     // OMX mark event upon processing this buffer
       OMX_PTR           pMarkData;                // OMX application specific data
       OMX_U32           mFlags;                   // OMX specific flags
};

#endif /* _DRC_NMF_HOST_EFFECT_INC_OMX_METADATA_HPP_ */
