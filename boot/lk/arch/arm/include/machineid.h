/*
 * Copyright (C) ST-Ericsson SA 2012. All rights reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files
 * (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge,
 * publish, distribute, sublicense, and/or sell copies of the Software,
 * and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#ifndef _MACHINEID_H
#define _MACHINEID_H

#include "target_config.h"

#define MACH_TYPE_INVALID   0
#define MACH_TYPE_U8500     2368
#define MACH_TYPE_HREFV60   3293
#define MACH_TYPE_U9540     3763
#define MACH_TYPE_A9500     3867
#define MACH_TYPE_U8520     3990
#define MACH_TYPE_U8540     4211

#define CPUID_DB8500ED      0x410fc090
#define CPUID_DB8500V1      0x411fc091
#define CPUID_DB8500V2      0x412fc091
#define CPUID_DB9540V1      0x413fc090
#define CPUID_DB8540V1      0x413fc090

#define ASICID_DB8500V11    0x008500A1
#define ASICID_DB8500V20    0x008500B0
#define ASICID_DB8500V21    0x008500B1
#define ASICID_DB8500V22    0x008500B2
#define ASICID_DB8520V22    0x008520B2
#define ASICID_DB9540V10    0x009540A0
#define ASICID_DB8540V10    0x008540A0

#endif  /* _MACHINEID_H */
