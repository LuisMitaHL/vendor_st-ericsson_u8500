/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include "tuning_user.h"
#include "tuning_osal.h"
#include "tuning_types.h"
#include "tuning_macros.h"
#include <string.h>

typedef struct
{
    e_iquser_id id;
    char name[16];
} t_user_entry_hash;

/** List of all possible entries in the xml file and associated enumerate on sw side*/
static const t_user_entry_hash KUserList[] =
{
    { IQSET_USER_DRIVER,                       "DRIVER"                       },
    { IQSET_USER_ISP,                          "ISP"                          },
    { IQSET_USER_ISP_DAMPERS,                  "ISP_DAMPERS"                  },
    { IQSET_USER_SW3A_AEC,                     "SW3A_AEC"                     },
    { IQSET_USER_SW3A_AWB,                     "SW3A_AWB"                     },
    { IQSET_USER_SW3A_AFC,                     "SW3A_AFC"                     },
    { IQSET_USER_SW3A_ART,                     "SW3A_ART"                     },
    { IQSET_USER_NORCOS,                       "NORCOS"                       },
    { IQSET_USER_NORCOS_DAMPERS,               "NORCOS_DAMPERS"               },
    { IQSET_USER_SCENE_DETECTOR,               "SCENE_DETECTOR"               }
};

static const t_sint32 KUserCount = sizeof(KUserList)/(sizeof(KUserList[0]));

// Prevent export of symbols
#ifdef __SYMBIAN32__
NONSHARABLE_CLASS(CTuningUser);
#endif

/****************************************************************************/
/**
 * \brief
 * \author
 * \in
 * \return
 *
 **/
/****************************************************************************/
e_iquser_id CTuningUser::FindUserIdFromName(const char* aName)
{
    t_sint32 cnt = 0;
    e_iquser_id id = IQSET_USER_UNKNOW;

    while (cnt < KUserCount)
    {
        if (!strncmp( aName, KUserList[cnt].name, sizeof(KUserList[cnt].name)))
        {
            id = KUserList[cnt].id;
            break;
        }
        cnt++;
    }
    return id;
}

/****************************************************************************/
/**
 * \brief
 * \author
 * \in
 * \return
 *
 **/
/****************************************************************************/
const char* CTuningUser::FindUserNameFromId(const e_iquser_id aUserId)
{
    t_sint32 cnt = 0;
    const char* userName = NULL;

    while (cnt < KUserCount)
    {
        if (KUserList[cnt].id == aUserId)
        {
            userName = KUserList[cnt].name;
            break;
        }
        cnt++;
    }
    return userName;
}
