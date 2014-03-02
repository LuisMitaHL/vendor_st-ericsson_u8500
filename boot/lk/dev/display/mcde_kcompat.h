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

#ifndef _KCOMPAT_H
#define _KCOMPAT_H 1

#include <stdint.h>
#include <platform/timer.h>		/* udelay() */
#include <malloc.h>

/* compatability defines */
#define u8  uint8_t
#define u16 uint16_t
#define u32 uint32_t

#define usleep_range(a, b) udelay(a)
#define mdelay(n) ({unsigned long msec = (n); while (msec--) udelay(1000); })
#define msleep(a) mdelay(a)
#define kfree(a) free(a)

#define GFP_KERNEL	0

#define EINVAL		-ERR_INVALID_ARGS
#define ENOMEM		-ERR_NO_MEMORY
#define EBUSY		-ERR_NOT_READY
#define EAGAIN 		-ERR_NOT_READY
#define ENOTSUPP	-ERR_NOT_SUPPORTED
#define EIO		-ERR_IO
#define IORESOURCE_MEM	-ERR_NO_MEMORY
#define ENODEV		-ERR_NOT_FOUND
#define ETIME		-ERR_TIMED_OUT

#define min(a_,b_) ((a_) < (b_) ? (a_) : (b_))
#define max(a_,b_) ((a_) > (b_) ? (a_) : (b_))

#define IS_ERR(a) ((int)a < 0)
#define ERR_PTR(a) ((void*)a)
#define PTR_ERR(a) ((int)a)

#define __init
#define __devinit
#define KERN_INFO

#define pr_info(...)	dprintf(INFO, __VA_ARGS__)

#define __WARN()  dprintf(CRITICAL, "WARN: %s:%d %s\n", \
						__FILE__, __LINE__, __func__)
#define WARN_ON(condition) ({                                   \
         int __ret_warn_on = !!(condition);                     \
         if (unlikely(__ret_warn_on))                           \
                 __WARN();                                      \
         unlikely(__ret_warn_on);                               \
})
#define WARN_ON_ONCE(condition) ({                              \
         static bool __warned;                                  \
         int __ret_warn_once = !!(condition);                   \
                                                                \
         if (unlikely(__ret_warn_once))                         \
                 if (WARN_ON(!__warned))                        \
                         __warned = true;                       \
         unlikely(__ret_warn_once);                             \
})

#ifndef u8
typedef unsigned char u8;
#endif
typedef short s16;
typedef int s32;
#ifndef u32
typedef unsigned int u32;
#endif

extern void *memset (void *, int, size_t);

inline void *kzalloc(int size, int flags)
{
	void *buf;
	buf = malloc(size);
	if (buf)
		memset(buf, 0, size);
	return buf;
}

extern int clk_enable(void *c);

inline int clk_disable(void *c)
{
	return 0;
}

inline int clk_put(void *c)
{
	return 0;
}

inline struct regulator *regulator_get(void *dev, char *reg)
{
	return 0;
}

inline int regulator_put(void *reg)
{
	return 0;
}

inline int regulator_enable(void *reg)
{
	return 0;
}

inline int regulator_disable(void *reg)
{
	return 0;
}

inline int regulator_set_voltage(void *reg, int volt_min, int volt_max)
{
	return 0;
}

/* from irqflags.h */
static inline u32 local_irq_save()
{
	return 0;
}

static inline u32 local_irq_restore(u32 flags)
{
	return 0;
}

/* TODO: fix serverity */
#define dev_warn(a, ...) dprintf(CRITICAL, __VA_ARGS__)
#define dev_err(a, ...) dprintf(CRITICAL, __VA_ARGS__)
#define dev_info(a, ...) dprintf(INFO, __VA_ARGS__)
#define dev_dbg(a, ...) dprintf(SPEW, __VA_ARGS__)
#define dev_vdbg(a, ...) dprintf(SPEW, __VA_ARGS__)
#define pr_error(...) dprintf(CRITICAL, __VA_ARGS__)

/* typedefs for unused types */
typedef int pm_message_t;

#endif
