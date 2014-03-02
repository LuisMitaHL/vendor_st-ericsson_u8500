/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */


#ifndef __INC_ITE_SKE_H
#define __INC_ITE_SKE_H


extern volatile t_uint32 g_keypad_still;
extern volatile t_uint8  g_line;
extern volatile t_uint8  g_column;


#ifdef __cplusplus
extern "C"
{
#endif

void ITE_SKEInit(void);

#ifdef __cplusplus
}
#endif

#endif /* __INC_ITE_SKE_H */
