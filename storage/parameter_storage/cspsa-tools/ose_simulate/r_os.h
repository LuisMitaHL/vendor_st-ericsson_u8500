/* **************************************************************************
 *
 * r_os.h
 *
 * Copyright (C) 2010 ST-Ericsson SA
 *
 * This software is released either under the terms of the BSD-style
 * license accompanying CSPSA or a proprietary license obtained from
 * ST-Ericsson SA.
 *
 * Author: 2010, Mikael GULLBERG <mikael.xx.gullberg@stericsson.com>
 *
 *
 **************************************************************************** */

#ifndef R_OS_H
#define R_OS_H

#include <stdlib.h>

#define HEAP_UNTYPED_ALLOC(Size)                                 (malloc(Size))
#define HEAP_FREE(Pointer_pp)                                    do { free(*(Pointer_pp)); (*(Pointer_pp)) = NULL; } while (0)
#define HEAP_UNSAFE_UNTYPED_ALLOC(Size)                          (malloc(Size))
#define HEAP_UNSAFE_UNTYPED_REALLOC(Pointer_pp, NewSize)         (realloc(*(Pointer_pp), NewSize))
#define HEAP_UNSAFE_FREE(Pointer_pp)                             do { free(*(Pointer_pp)); (*(Pointer_pp)) = NULL; } while (0)

#endif
