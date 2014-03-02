/*
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef _TUNING_USER_H_
#define _TUNING_USER_H_

/** list of all supported users */
typedef enum
{
    IQSET_USER_UNKNOW = -1,
    IQSET_USER_DRIVER = 0,
    IQSET_USER_ISP,
    IQSET_USER_ISP_DAMPERS,
    IQSET_USER_SW3A_AEC,
    IQSET_USER_SW3A_AWB,
    IQSET_USER_SW3A_AFC,
    IQSET_USER_SW3A_ART,
    IQSET_USER_NORCOS,
    IQSET_USER_NORCOS_DAMPERS,
    IQSET_USER_SCENE_DETECTOR,
    IQSET_USER_COUNT
} e_iquser_id;

#ifdef __SYMBIAN32__
NONSHARABLE_CLASS(CTuningUser);
#endif

class CTuningUser
{
    public:
        static e_iquser_id FindUserIdFromName(const char*);
        static const char* FindUserNameFromId(const e_iquser_id);
        inline static bool IsValidUserId(const e_iquser_id aUserId) {return aUserId>=0 && aUserId<IQSET_USER_COUNT;}
};

#endif /*_TUNING_USER_H_*/
