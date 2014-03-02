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

/*******************************************************************************
 * Includes
 ******************************************************************************/

#include <debug.h>

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include <reg.h>
#include <kernel/thread.h>
#include <arch/ops.h>

#include "app.h"
#include "usb_wrapper.h"

#include <kernel/thread.h>
#include <kernel/event.h>
#include <dev/udc.h>

static struct udc_device usbtest_device = {
	.vendor_id	= 0x18d1,
	.product_id	= 0xD00D,
	.version_id	= 0x0100,
	.manufacturer	= "ST-Ericsson",
	.product	= "Fastboot downloder",
	.serialno	= "08500-00-00",
};

static event_t usb_online;
unsigned char is_usb_ready;

static void usbtest_notify(struct udc_gadget *gadget, unsigned event);
static struct udc_gadget usbtest_gadget = {
	.notify		= usbtest_notify,
	.ifc_string	= "Fastboot",
};

unsigned char rbuf[8192+100];

static void usbtest_notify(struct udc_gadget *gadget, unsigned event)
{
	if (event == UDC_EVENT_ONLINE) {
		dprintf(ALWAYS, "USB is connected and ready for transfer\n");
		is_usb_ready = 1;
		event_signal(&usb_online, 0);
	}
	if (event == UDC_EVENT_OFFLINE) {
		dprintf(ALWAYS, "USB is disconnected\n");
		is_usb_ready = 0;
	}
}

void start_usbtest(const struct app_descriptor *app)
{
	int rx_count = 0;
	event_init(&usb_online, 0, EVENT_FLAG_AUTOUNSIGNAL);
    usbdev_init(&usbtest_device, &usbtest_gadget);
	event_wait(&usb_online);
	while (1) {
		rx_count = usbdev_read(rbuf, 8192);
		if (rx_count >= 0)
			usbdev_write(rbuf, rx_count);
	}
}

APP_START(usb_test)
    .init = start_usbtest,
APP_END
