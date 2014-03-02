/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/* Force using int instead of possible native compiler internal type to avoid link issues */

#ifndef __STDBOOL_H__
#define __STDBOOL_H__

#ifndef __cplusplus
#define bool	int
#define true	1
#define false	0
#define __bool_true_false_are_defined	1
#endif

#endif
