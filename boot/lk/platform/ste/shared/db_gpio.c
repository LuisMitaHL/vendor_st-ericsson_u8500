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

#include <db_gpio.h>
#include <reg.h>

#if PLATFORM_STE_X500
#include "target_config.h"
static void *const gpio[] = {
	(void *)U8500_GPIO0_BASE,
	(void *)U8500_GPIO1_BASE,
	(void *)U8500_GPIO2_BASE,
	(void *)U8500_GPIO3_BASE,
	(void *)U8500_GPIO4_BASE,
	(void *)U8500_GPIO5_BASE,
	(void *)U8500_GPIO6_BASE,
	(void *)U8500_GPIO7_BASE,
	(void *)U8500_GPIO8_BASE,
};
#endif


int gpio_get_mode(unsigned nr)
{
    /* Get base address and gpio pin in current gpio block */
    struct gpio_regs *regs = (struct gpio_regs *) gpio[nr/32];
    uint32_t gpio_pin = 1<< (nr%32);

    if((readl(&regs->dir) & gpio_pin) == 0) {
        return GPIO_INPUT;
    }
    else {
        return GPIO_OUTPUT;
    }
}

int gpio_get_pull(unsigned nr)
{
    /* Get base address and gpio pin in current gpio block */
    struct gpio_regs *regs = (struct gpio_regs *) gpio[nr/32];
    uint32_t gpio_pin = 1<< (nr%32);
    uint32_t pdis=0;
    uint32_t dat=0;

    pdis = readl(&regs->pdis);
    dat = readl(&regs->dat);

    if(pdis & gpio_pin) {
        return GPIO_PULLNONE;
    }

    if(dat & gpio_pin) {
        return GPIO_PULLUP;
    }
    else {
        return GPIO_PULLDOWN;
    }
}

int gpio_get_alt(unsigned nr)
{
    /* Get base address and gpio pin in current gpio block */
    struct gpio_regs *regs = (struct gpio_regs *) gpio[nr/32];
    uint32_t gpio_pin = 1<< (nr%32);
    uint32_t afunc, bfunc;

    afunc = readl(&regs->afsla);
    bfunc = readl(&regs->afslb);

    if(!(bfunc & gpio_pin) && !(afunc & gpio_pin)) {
        return 0;
    } else if(!(bfunc & gpio_pin) && (afunc & gpio_pin)) {
        return GPIO_ALTA_FUNC;
    } else if((bfunc & gpio_pin) && !(afunc & gpio_pin)) {
        return GPIO_ALTB_FUNC;
    } else if((bfunc & gpio_pin) && (afunc & gpio_pin)) {
        return GPIO_ALTC_FUNC;
    }
    return 0;
}


void gpio_make_input(unsigned nr)
{
    /* Get base address and gpio pin in current gpio block */
    struct gpio_regs *regs = (struct gpio_regs *) gpio[nr/32];
    uint32_t gpio_pin = 1<< (nr%32);

    writel(gpio_pin,&regs->dirc);
}

void gpio_set_pull(unsigned nr,unsigned  pull)
{
    /* Get base address and gpio pin in current gpio block */
    struct gpio_regs *regs = (struct gpio_regs *) gpio[nr/32];
    uint32_t gpio_pin = 1<< (nr%32);
    uint32_t pdis;

    pdis = readl(&regs->pdis);
    if (pull == GPIO_PULLNONE) {
        pdis |= gpio_pin;
    }
    else {
        pdis &= ~gpio_pin;
    }
    writel(pdis,&regs->pdis);

    if (pull == GPIO_PULLUP) {
        writel(gpio_pin,&regs->dats);
    }
    else if (pull == GPIO_PULLDOWN) {
        writel(gpio_pin,&regs->datc);
    }
}

void db_gpio_set(uint32_t pin, uint32_t mode)
{
	/* Get base address and gpio pin in current gpio block */
	struct gpio_regs *regs = (struct gpio_regs *) gpio[pin/32];
	uint32_t gpio_pin = 1<< (pin%32);
	uint32_t alt;

	alt = mode & GPIO_ALT_MASK;

	/* Set selected ALT FUNC */
	if (alt == GPIO_ALTA_FUNC) {
		writel((readl(&regs->afsla) | gpio_pin), &regs->afsla);
		writel((readl(&regs->afslb) & ~gpio_pin), &regs->afslb);
	} else if (alt == GPIO_ALTB_FUNC) {
		writel((readl(&regs->afsla) & ~gpio_pin), &regs->afsla);
		writel((readl(&regs->afslb) | gpio_pin), &regs->afslb);

	} else if (alt == GPIO_ALTC_FUNC) {
		writel((readl(&regs->afsla) | gpio_pin), &regs->afsla);
		writel((readl(&regs->afslb) | gpio_pin), &regs->afslb);
	} else {
		writel((readl(&regs->afsla) & ~gpio_pin), &regs->afsla);
		writel((readl(&regs->afslb) & ~gpio_pin), &regs->afslb);
	}


	/* Set pull-up/down configuration */
	if (mode & GPIO_PULLUP) {
		writel((readl(&regs->pdis) & ~gpio_pin), &regs->pdis);
		writel(gpio_pin, &regs->dats);
	} else if (mode & GPIO_PULLDOWN) {
		writel((readl(&regs->pdis) & ~gpio_pin), &regs->pdis);
		writel(gpio_pin, &regs->datc);
	} else {
		writel((readl(&regs->pdis) | gpio_pin), &regs->pdis);
	}

	/* Set input/output and level */
	if (mode & GPIO_OUTPUT) {
		if (mode & GPIO_HIGH)
			writel(gpio_pin, &regs->dats);
		else if (mode & GPIO_LOW)
			writel(gpio_pin, &regs->datc);

		writel(gpio_pin, &regs->dirs);
	} else {
		/* pin is an input */
		writel(gpio_pin, &regs->dirc);
	}
}

/**
 * gpio_config() - configure a gpio pin
 * @nr:		pin to configure
 * @flags:	pin config flags
 *
 * Call from the framework to configure a gpio pin.
 * The pin mux will be set to GPIO.
 */
int gpio_config(unsigned nr, unsigned flags)
{
	db_gpio_set(nr, flags);
	return 0;
}

/**
 * gpio_set() - set the level on a gpio pin
 * @nr:	pin to set
 * @on:	pin level to set (1=high, 0=low)
 *
 * Call from the framework to set the level on a gpio pin. Only the level
 * is set - the configuration is not changed.
 */
void gpio_set(unsigned nr, unsigned on)
{
	struct gpio_regs *regs = (struct gpio_regs *)gpio[nr / 32];
	uint32_t gpio_pin = 1 << (nr % 32);

	if (on)
		writel(gpio_pin, &regs->dats);
	else
		writel(gpio_pin, &regs->datc);
}

/**
 * gpio_get() - get the current level of a gpio pin
 * @nr:	pin to get
 *
 * Return the level of a gpio pin: either 1 or 0 is returned to indicate
 * high or low on the pin.
 */
int gpio_get(unsigned nr)
{
	struct gpio_regs *regs = (struct gpio_regs *)gpio[nr / 32];
	uint32_t gpio_pin = 1 << (nr % 32);

	return !!(readl(&regs->dat) & gpio_pin);
}
