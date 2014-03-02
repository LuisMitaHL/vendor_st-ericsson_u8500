/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef __SW_3A_LIST_HPP__
#define __SW_3A_LIST_HPP__

#include <sw_3A.nmf>

class CpeList
{
    public:
        CpeList();
        t_sint32 clear();
        t_sint32 push(t_uint16 addr, t_uint32 data);
        t_sint32 pushf(t_uint16 addr, float f_data);
        t_sw3A_PageElementList getStorage();
        t_sint32 filled_size();
    private:
        t_sw3A_PageElementList peList;
};

#endif /* __SW_3A_LIST_HPP__ */
