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
#include <stdlib.h>
#include <kernel/thread.h>
#include <kernel/event.h>
#include <dev/udc.h>
#include "target.h"

static event_t txn_done;
static struct udc_endpoint *in, *out;
static struct udc_request *req;
int txn_status;

static void read_req_complete(struct udc_request *req, unsigned actual,
int status)
{
	dprintf(INFO, "USB Read done for %d bytes\n", actual);

	txn_status = status;
	req->length = actual;
	event_signal(&txn_done, 0);
}

static void write_req_complete(struct udc_request *req, unsigned actual,
int status)
{
	dprintf(INFO, "USB write done for %d bytes\n", actual);
	txn_status = status;
	req->length = actual;
	event_signal(&txn_done, 0);
}

int usbdev_read(void *_buf, unsigned len)
{
	int r;
	unsigned char *buf = _buf;
	int count = 0;
	if (!buf) {
       return -1;
    }
	memset(req, 0, sizeof(*req));
	req->buf = buf;
	req->length = len;
	req->complete = read_req_complete;
	r = udc_request_queue(out, req);
	if (r < 0) {
		dprintf(INFO, "usb_read() queue failed\n");
		goto oops;
	}
	event_wait(&txn_done);
	count = req->length;

	if (txn_status < 0) {
		dprintf(INFO, "usb_read() transaction failed\n");
		goto oops;
	}
	return count;

oops:
	return -1;
}

int usbdev_write(void *buf, unsigned len)
{
	int r;
	if (!buf) {
       return -1;
    }
	memset(req, 0, sizeof(*req));
	req->buf = buf;
	req->length = len;
	req->complete = write_req_complete;
	r = udc_request_queue(in, req);
	if (r < 0) {
		dprintf(INFO, "usb_write() queue failed\n");
		goto oops;
	}
	event_wait(&txn_done);
	if (txn_status < 0) {
		dprintf(INFO, "usb_write() transaction failed\n");
		goto oops;
	}
	return req->length;

oops:
	return -1;
}

int usbdev_init(struct udc_device *udev, struct udc_gadget *gadget)
{
	if (!udev || !gadget) {
       return -1;
    }

	udev->serialno = get_asic_id_str();

    if (!gadget->ifc_class)
        gadget->ifc_class = 0xff;
	if (!gadget->ifc_subclass)
	    gadget->ifc_subclass = 0x42;
	if (!gadget->ifc_protocol)
	    gadget->ifc_protocol = 0x03;
	if (!gadget->ifc_endpoints)
	    gadget->ifc_endpoints = 2;

	event_init(&txn_done, 0, EVENT_FLAG_AUTOUNSIGNAL);

	if (udc_init(udev) < 0) {
		dprintf(INFO, "USB device registration failed\n");
		return -1;
	}

	in = udc_endpoint_alloc(UDC_TYPE_BULK_IN, 512);
	if (!in) {
		dprintf(INFO, "USB IN enpoint allocation failed\n");
		return -1;
	}
	out = udc_endpoint_alloc(UDC_TYPE_BULK_OUT, 512);
	if (!out) {
		dprintf(INFO, "USB OUT enpoint allocation failed\n");
		return -1;
	}

    gadget->ept[0] = in;
    gadget->ept[1] = out;

	if (udc_register_gadget(gadget) < 0) {
		dprintf(INFO, "USB gadget registration failed\n");
		return -1;
	}
	if (udc_start() < 0) {
		dprintf(INFO, "USB device/gadget is not registered\n");
		return -1;
	}
	req = udc_request_alloc();
	if (!req) {
		dprintf(INFO, "USB request allocation is failed\n");
		return -1;
	}
	return 0;
}

