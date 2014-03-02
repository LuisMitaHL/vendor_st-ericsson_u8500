/*
 * Copyright (c) 2008 Travis Geiselbrecht
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
#include <debug.h>
#include <err.h>
#include <stdlib.h>
#include <string.h>
#include <dev/usb.h>
#include <kernel/event.h>
#include "mentor_usb.h"

#include <dev/udc.h>
#include <dev/usb_descriptors.h>


#define LOCAL_TRACE 0

#define GET_DESCRIPTOR_DEVICE   0x100
#define GET_DESCRIPTOR_CONFIG   0x200
#define GET_DESCRIPTOR_LNGID   0x300
#define GET_DESCRIPTOR_STRING_S   0x301
#define GET_DESCRIPTOR_STRING_E   0x3FF
#define GET_DESCRIPTOR_DEVQUAL   0x600
#define GET_DESCRIPTOR_DBG   0xA00

static usb_config config;
struct udc_gadget *the_gadget;
struct udc_device *the_device;
static bool usb_active;
#define MAX_STRINGS 8
static usb_string strings[MAX_STRINGS];
static uint8_t active_config;
usbc_transfer usb_transfer;
usb_descriptor str_desc;

struct usb_request {
	struct udc_request req;
	struct ept_queue_item *item;
};

struct udc_endpoint {
	struct udc_endpoint *next;
	unsigned bit;
	struct ept_queue_head *head;
	struct usb_request *req;
	unsigned char num;
	unsigned char in;
	unsigned short maxpkt;
};

static event_t rx_xfer_done;

static int set_string_descriptor(usb_descriptor *desc, const char *string)
{
	int len = strlen(string);
	ushort *data;
	int datalen = len * 2 + 2;

	LTRACEF("\n");

	data = malloc(datalen);
	if (!data)
       return -1;

	/* write length field */
	data[0] = 0x0300 + datalen;

	/* copy the string into the uint16_t based usb string */
	int i;
	for (i = 0; i < len; i++)
		data[i + 1] = string[i];

	desc->desc = (void *)data;
	desc->len = datalen;
    return 0;
}

static int add_string(const char *string, uint8_t id)
{
	uint i;
	LTRACEF("\n");
	if (set_string_descriptor(&str_desc, string) < 0)
       return -1;
	for (i = 0; i < MAX_STRINGS; i++) {
		if (strings[i].id == 0) {
			strings[i].string = str_desc;
			strings[i].id = id;
			break;
		}
	}
    return 0;
}

void usb_remove_string(uint8_t id)
{
	uint i;
	LTRACEF("\n");
	for (i = 0; i < MAX_STRINGS; i++) {
		if (strings[i].id == id) {
			free(strings[i].string.desc);
			strings[i].string.desc = NULL;
			strings[i].id = 0;
			break;
		}
	}
}

static int tx_transfer_complete(ep_t endpoint,
usbc_callback_op_t op, usbc_transfer *transfer)
{
	struct udc_request *req;
	LTRACEF("buflen=%d,buf=%p,bufpos=%d,result=%d,extra=%p\n",
			transfer->buflen, transfer->buf, transfer->bufpos,
			transfer->result, transfer->extra);
	req = (struct udc_request *) transfer->extra;
	if (!req) {
		LTRACEF("context information corrupted!\n");
		return -1;
	}
	LTRACEF("req=%p, req->length=%d, req->buf=%p\n", req, req->length,
req->buf);
	req->complete(req, transfer->bufpos, transfer->result);
	return 0;
}

static int rx_transfer_complete(ep_t endpoint, usbc_callback_op_t op,
usbc_transfer *transfer)
{
	LTRACEF("buflen=%d,buf=%p,bufpos=%d,result=%d,extra=%p\n",
			transfer->buflen, transfer->buf, transfer->bufpos,
			transfer->result, transfer->extra);

	event_signal(&rx_xfer_done, 0);
	return 0;
}

int udc_request_queue(struct udc_endpoint *ept, struct udc_request *req)
{
	unsigned char *buf;
	unsigned int xfer = 0, count = 0;
    int len = 0;

	LTRACEF("req=%p, req->length=%d, req->buf=%p\n",
				req, req->length, req->buf);

	memset(&usb_transfer, 0, sizeof(usb_transfer));

	if (ept->in) {
		usb_transfer.buf = req->buf;
		usb_transfer.buflen = req->length;
		usb_transfer.extra = (void *)req;
		usbc_queue_tx(ept->num, &usb_transfer);
	} else {
		buf = req->buf;
		len = req->length;
		usb_transfer.extra = (void *)req;
		while (len > 0) {
			xfer = (len > 512) ? 512 : len;
			usb_transfer.buf = buf;
			usb_transfer.buflen = xfer;
			usb_transfer.bufpos = 0;
			usbc_queue_rx(ept->num, &usb_transfer);
			event_wait(&rx_xfer_done);

			count += usb_transfer.bufpos;
			buf += usb_transfer.bufpos;
			len -= usb_transfer.bufpos;

			/* short transfer? */
			if (usb_transfer.bufpos != xfer) {
				LTRACEF("short transfer\n");
				break;
			}
		}
		req->complete(req, count, usb_transfer.result);
	}
	return 0;
}

int udc_request_cancel(struct udc_endpoint *ept, struct udc_request *req)
{
	LTRACEF("\n");
	return 0;
}

/* bits used in all the endpoint status registers */
#define EPT_TX(n) (1 << ((n) + 16))
#define EPT_RX(n) (1 << (n))
unsigned int ept_alloc_table;
struct udc_endpoint *udc_endpoint_alloc(unsigned type, unsigned maxpkt)
{
	struct udc_endpoint *ept;
	unsigned n;
	unsigned in;

	LTRACEF("\n");
	if (type == UDC_TYPE_BULK_IN)
		in = 1;
	else if (type == UDC_TYPE_BULK_OUT)
		in = 0;
	else
		return NULL;

	for (n = 1; n < 16; n++) {
		unsigned bit = in ? EPT_TX(n) : EPT_RX(n);
		if (ept_alloc_table & bit)
			continue;
		ept = malloc(sizeof(*ept));
        if (!ept)
           return NULL;
		if (ept) {
			ept_alloc_table |= bit;
			ept->maxpkt = maxpkt;
			ept->num = n;
			ept->in = in;
			ept->req = 0;
			if (ept->in)
				/* setup the IN endpoint for TX */
				usbc_setup_endpoint(n, !in,
						true, tx_transfer_complete,
						maxpkt, 512);
			else
				/* setup the OUT endpoint for RX */
				usbc_setup_endpoint(n, !in,
						true, rx_transfer_complete,
						maxpkt, 512);
			return ept;
		} else {
			return NULL;
		}
	}
	return NULL;
}

void udc_endpoint_free(struct udc_endpoint *ept)
{
	LTRACEF("\n");
    if (ept)
    	free(ept);
}

struct udc_request *udc_request_alloc(void)
{
	struct udc_request *req;

	LTRACEF("\n");
	req = malloc(sizeof(struct udc_request));
    if (!req)
       return NULL;
	req->buf = 0;
	req->length = 0;
	if (req)
		LTRACEF("req=%p, req->length=%d, req->buf=%p\n",
				req, req->length, req->buf);
	return req;
}

void udc_request_free(struct udc_request *req)
{
	LTRACEF("\n");
    if (req)
    	free(req);
}


int udc_init(struct udc_device *devinfo)
{
	LTRACEF("\n");
	the_device = devinfo;
	if (!the_device) {
		dprintf(ALWAYS, "USB device is already initialized\n");
		return -1;
	}
	event_init(&rx_xfer_done, 0, EVENT_FLAG_AUTOUNSIGNAL);
	if (usbc_init() < 0)
       return -1;
    return 0;
}

int udc_exit(struct udc_device *devinfo)
{
	LTRACEF("\n");
	if (the_device != devinfo) {
		dprintf(ALWAYS, "registered device is not matched\n");
		return -1;
	}
	event_destroy(&rx_xfer_done);
	usbc_exit();
	the_device = NULL;
	return 0;
}

int udc_register_gadget(struct udc_gadget *gadget)
{
	LTRACEF("\n");

	if (the_gadget) {
		dprintf(ALWAYS, "Gadget is already registered\n");
		return -1;
	}
	the_gadget = gadget;
	return 0;
}

int udc_unregister_gadget(struct udc_gadget *gadget)
{
	LTRACEF("\n");

	if (the_gadget != gadget) {
		/* registered  is not matched */
		dprintf(ALWAYS, "registered gadget is not matched\n");
		return -1;
	}
	the_gadget = NULL;
	return 0;
}


static int setup_transaction_callback(usbc_callback_op_t op,
			const union usb_callback_args *args)
{
	/* start looking for specific things to handle */
	if (op == CB_SETUP_MSG) {
		const struct usb_setup *setup = args->setup;
		DEBUG_ASSERT(setup);
		if (setup->value != GET_DESCRIPTOR_DEVICE)
			LTRACEF("SETUP: type=%x req=%x val=%x idx=%x len=%x\n",
			setup->request_type, setup->request, setup->value,
			setup->index, setup->length);
		if ((setup->request_type & TYPE_MASK) == TYPE_STANDARD) {
			switch (setup->request) {
			case SET_ADDRESS:
				LTRACEF("SET_ADDRESS 0x%x\n", setup->value);
				usbc_ep0_ack();
				break;
			case SET_FEATURE:
			case CLEAR_FEATURE:
				/* OTAY */
				LTRACEF("SET/CLEAR_FEATURE:%x\n", setup->value);
				usbc_ep0_ack();
				break;
			case SET_DESCRIPTOR:
				LTRACEF("SET_DESCRIPTOR\n");
				usbc_ep0_stall();
				break;
			case GET_DESCRIPTOR: {
				/* Get right descriptors based on speed */
				const struct usb_descriptor_speed *speed;
				if (usbc_is_highspeed())
					speed = &config.highspeed;
				else
					speed = &config.lowspeed;

				if ((setup->request_type & RECIP_MASK) ==
						RECIP_DEVICE) {
					switch (setup->value) {

                    case GET_DESCRIPTOR_DEVICE:
						usbc_ep0_send(
							speed->device.desc,
							speed->device.len,
							setup->length);
						LTRACEF("GET_DESC, dev\n");
						break;
                    case GET_DESCRIPTOR_CONFIG:
						LTRACEF("GET_DESC, cfg\n");
						usbc_ep0_send(
							speed->config.desc,
							speed->config.len,
							setup->length);
							break;
                    case GET_DESCRIPTOR_LNGID:
						LTRACEF("GET_DESC, lngid\n");
						usbc_ep0_send(
							config.langid.desc,
							config.langid.len,
							setup->length);
						break;
					case (GET_DESCRIPTOR_STRING_S)...(GET_DESCRIPTOR_STRING_E): {
					/* string descriptor,
					* search our list for a match */
					uint i;
					bool found = false;
					uint8_t id =
					setup->value & 0xff;
					for (i = 0; i < MAX_STRINGS; i++) {
						if (strings[i].id == id) {
							usbc_ep0_send(
							strings[i].string.desc,
							strings[i].string.len,
							setup->length);
							found = true;
							break;
						}
					}
					if (!found) {
						/* couldn't find one, stall */
						usbc_ep0_stall();
					}
					break;
					}
                    case GET_DESCRIPTOR_DEVQUAL:
						LTRACEF("GET_DESC, dev qual\n");
						usbc_ep0_send(
							speed->device_qual.desc,
							speed->device_qual.len,
							setup->length);
						break;
					case GET_DESCRIPTOR_DBG:
						/* we aint got one of these */
						LTRACEF("GET_DESC, dbg desc\n");
						usbc_ep0_stall();
						break;
					default:
						LTRACEF("unhandled desc %x\n",
								setup->value);
						/* stall */
						break;
					}
				} else {
					/* interface/endpoint descriptors?
					* let someone else handle it */
					/* STALL */
				}
				break;
			}

			case SET_CONFIGURATION:
				LTRACEF("SET_CONFIGURATION %d\n", setup->value);
				active_config = setup->value;
				usbc_ep0_ack();
				break;

			case GET_CONFIGURATION:
				LTRACEF("GET_CONFIGURATION\n");
				usbc_ep0_send(&active_config, 1, setup->length);
				break;

			case SET_INTERFACE:
				LTRACEF("SET_INTERFACE %d\n", setup->value);
				usbc_ep0_ack();
				break;

			case GET_INTERFACE: {
				static uint8_t i = 1;
				LTRACEF("GET_INTERFACE\n");
				usbc_ep0_send(&i, 1, setup->length);
				break;
				}
			case GET_STATUS: {
				static uint16_t i = 1; /* self powered */
				LTRACEF("GET_STATUS\n");
				usbc_ep0_send(&i, 2, setup->length);
				break;
				}

			default:
				LTRACEF("unhandled standard request 0x%x\n",
					setup->request);
			}
		}
	}

	return 0;
}

static int usb_set_config_callback(usbc_callback_op_t op,
const union usb_callback_args *args)
{
	if (op == CB_ONLINE) {
		LTRACEF("\n");
		the_gadget->notify(the_gadget, UDC_EVENT_ONLINE);
	}
	if (op == CB_DISCONNECT) {
		LTRACEF("\n");
		the_gadget->notify(the_gadget, UDC_EVENT_OFFLINE);
	}
	return 0;
}


#define USB_DEVICE_DESC_SIZE	18
#define USB_CONFIG_DESC_SIZE	32
int udc_start(void)
{
	unsigned char *dev_desc, *cfg_desc;
	unsigned int size;
	struct udc_endpoint *ept;

	LTRACEF("\n");
	if (!the_device) {
		LTRACEF("Device is not initialized\n");
		return -1;
	}

	if (!the_gadget) {
		LTRACEF("Gadget is not initialized\n");
		return -1;
	}

	/* device descriptor */
	dev_desc = malloc(USB_DEVICE_DESC_SIZE);
    if (!dev_desc)
       return -1;

	memcpy(dev_desc, dev_descr, sizeof(dev_descr));
	memcpy(dev_desc + 8, &the_device->vendor_id, sizeof(short));
	memcpy(dev_desc + 10, &the_device->product_id, sizeof(short));
	memcpy(dev_desc + 12, &the_device->version_id, sizeof(short));

	config.highspeed.device.desc = dev_desc;
	config.highspeed.device.len = sizeof(dev_descr);

	/* configuration descriptor */
	cfg_desc = malloc(USB_CONFIG_DESC_SIZE);
    if (!cfg_desc)
       return -1;

    memcpy(cfg_desc, cfg_descr_highspeed, sizeof(cfg_descr_highspeed));
	size = sizeof(cfg_descr_highspeed);
	cfg_desc[2] = size;
	cfg_desc[3] = size >> 8;

	/* interface descriptor changes */
	cfg_desc[13] = the_gadget->ifc_endpoints;
	cfg_desc[14] = the_gadget->ifc_class;
	cfg_desc[15] = the_gadget->ifc_subclass;
	cfg_desc[16] = the_gadget->ifc_protocol;

	/* endpoint descriptor 1 */
	ept = the_gadget->ept[0];
	cfg_desc[20] = ept->num | (ept->in ? 0x80 : 0x00);
	cfg_desc[21] = 0x02;
	cfg_desc[22] = ept->maxpkt;
	cfg_desc[23] = ept->maxpkt>>8;

	/* endpoint descriptor 2 */
	ept = the_gadget->ept[1];
	cfg_desc[27] = ept->num | (ept->in ? 0x80 : 0x00);
	cfg_desc[28] = 0x02;
	cfg_desc[29] = ept->maxpkt;
	cfg_desc[30] = ept->maxpkt>>8;
	config.highspeed.config.desc = cfg_desc;
	config.highspeed.config.len = sizeof(cfg_descr_highspeed);

	config.langid.desc = langid;
	config.langid.len = sizeof(langid);

	if (add_string(the_device->manufacturer, 0x1) < 0)
       return -1;
	if (add_string(the_device->product, 0x2) < 0)
       return -1;
	if (add_string(the_device->serialno, 0x3) < 0)
       return -1;
	if (usbc_set_callback(&setup_transaction_callback) < 0)
       return -1;
	if (usbc_set_callback(&usb_set_config_callback) < 0)
       return -1;
	if (usb_active == false)
		usbc_set_active(true);
	return 0;
}

int udc_stop(void)
{
	LTRACEF("\n");
	usb_remove_string(0x1);
	usb_remove_string(0x2);
	usb_remove_string(0x3);

	if (usb_active == true)
		usbc_set_active(false);
	return 0;
}

