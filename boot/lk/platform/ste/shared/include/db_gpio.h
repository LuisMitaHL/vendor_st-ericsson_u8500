#ifndef DB_GPIO_H
#define DB_GPIO_H

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

#include <inttypes.h>
#include <dev/gpio.h>

/*
 * Device framework GPIO defines use the lower 16bits so DB specific
 * GPIO defines will use the upper 16bits to avoid clashes.
 */

/* ALT FUNC */
#define GPIO_ALTA_FUNC	(0x0001 << 16)
#define GPIO_ALTB_FUNC	(0x0002 << 16)
#define GPIO_ALTC_FUNC	(0x0004 << 16)

/* Mode */
#define GPIO_OUT_HIGH	(GPIO_OUTPUT | GPIO_HIGH)
#define GPIO_OUT_LOW	(GPIO_OUTPUT | GPIO_LOW)

#define GPIO_ALT_MASK	(0x0007 << 16)


/* GPIO registers */
struct gpio_regs {
        uint32_t dat;           /* 0x00 Data register */
        uint32_t dats;          /* 0x04 Data set register */
        uint32_t datc;         	/* 0x08 Data clear regiser */
        uint32_t pdis;       	/* 0x0C Pull disable register */
        uint32_t dir;       	/* 0x10 Direction register */
        uint32_t dirs;       	/* 0x14 Direction set register */
        uint32_t dirc;       	/* 0x18 Direction clear register */
        uint32_t slpc;       	/* 0x1C Sleep mode register */
        uint32_t afsla;       	/* 0x20 Alternate function select A register */
        uint32_t afslb;       	/* 0x24 Alternate function select B register */
        uint32_t lowemi;     	/* 0x28 Low EMI mode register */
        uint32_t rsvd_0x2C;     /* 0x2C reserved */
        uint32_t rsvd_0x30;     /* 0x30 reserved */
        uint32_t rsvd_0x34;     /* 0x34 reserved */
        uint32_t rsvd_0x38;     /* 0x38 reserved */
        uint32_t rsvd_0x3C;     /* 0x3C reserved */
        uint32_t rimsc;         /* 0x40 Rising edge interrupt mask set clear register */
        uint32_t fimsc;       	/* 0x44 Falling edge interrupt mask set clear register */
        uint32_t is;          	/* 0x48 Interrupt status register */
        uint32_t ic;          	/* 0x4C Interrupt clear register */
        uint32_t rwmsc;         /* 0x50 Rising edge wakeup mask set clear register */
        uint32_t fwmsc;         /* 0x54 Falling edge wakeup mask set clear register */
        uint32_t wks;           /* 0x58 Wakeup status register */
};

void db_gpio_set(uint32_t pin, uint32_t mode);

#endif /* DB_GPIO_H */
