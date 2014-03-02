/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
#ifndef DEBUG_H
#define DEBUG_H

#undef A_
#ifdef PRINT_A_
#define A_(p) p
#else
#define A_(p)
#endif

#undef B_
#ifdef PRINT_B_
#define B_(p) p
#else
#define B_(p)
#endif

#undef C_
#ifdef PRINT_C_
#define C_(p) p
#else
#define C_(p)
#endif

#endif /*DEBUG_H*/
