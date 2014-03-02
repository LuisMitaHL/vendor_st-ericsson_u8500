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
#include <string.h>
#include <stdlib.h>
#include <kernel/thread.h>
#include <reg.h>
#include "target_config.h"
#include "mentor_usb.h"
#include <platform/interrupts.h>
#include <platform/irqs.h>

#include <hw/usb.h>

#define LOCAL_TRACE 0

#define musb_reg8(reg) (*REG8(USB_HS_BASE + (reg)))
#define musb_reg16(reg) (*REG16(USB_HS_BASE + (reg)))
#define musb_reg32(reg) (*REG32(USB_HS_BASE + (reg)))

#define MAX_ENDPOINTS	16

/* registers */
#define MUSB_FADDR	0x0
#define MUSB_POWER	0x1
#define MUSB_INTRTX	0x2
#define MUSB_INTRRX	0x4
#define MUSB_INTRTXE	0x6
#define MUSB_INTRRXE	0x8
#define MUSB_INTRUSB	0xa
#define MUSB_INTRUSBE 0xb
#define MUSB_FRAME	0xc
#define MUSB_INDEX	0xe
#define MUSB_TESTMODE 0xf

/* indexed endpoint regs */
#define MUSB_IDX_TXMAXP	0x10
#define MUSB_IDX_TXCSR	0x12
#define MUSB_IDX_TXCSRL	0x12
#define MUSB_IDX_TXCSRH	0x13
#define MUSB_IDX_RXMAXP	0x14
#define MUSB_IDX_RXCSR	0x16
#define MUSB_IDX_RXCSRL	0x16
#define MUSB_IDX_RXCSRH	0x17
#define MUSB_IDX_RXCOUNT	0x18
#define IDX_FIFOSIZE	0x1f

/* if endpoint 0 is selected */
#define MUSB_IDX_CSR0	0x12
#define MUSB_IDX_CONFIGDATA	0x1f

/* endpoint FIFOs */
#define MUSB_FIFOBASE	0x20

#define MUSB_DEVCTL	0x60
#define MUSB_TXFIFOSZ	0x62
#define MUSB_RXFIFOSZ	0x63
#define MUSB_TXFIFOADD	0x64
#define MUSB_RXFIFOADD	0x66
#define MUSB_HWVERS	0x6c
#define MUSB_EPINFO	0x78
#define MUSB_RAMINFO	0x79
#define MUSB_LINKINFO	0x7a

/*
 * MUSB Register bits
 */

/* POWER */
#define MUSB_POWER_ISOUPDATE	0x80
#define MUSB_POWER_SOFTCONN	0x40
#define MUSB_POWER_HSENAB	0x20
#define MUSB_POWER_HSMODE	0x10
#define MUSB_POWER_RESET	0x08
#define MUSB_POWER_RESUME	0x04
#define MUSB_POWER_SUSPENDM	0x02
#define MUSB_POWER_ENSUSPEND	0x01

/* INTRTXE */
#define MUSB_INTRTXE_EP0	0x0001

/* INTRUSB */
#define MUSB_INTR_SUSPEND	0x01
#define MUSB_INTR_RESUME	0x02
#define MUSB_INTR_RESET		0x04
#define MUSB_INTR_BABBLE	0x04
#define MUSB_INTR_SOF		0x08
#define MUSB_INTR_CONNECT	0x10
#define MUSB_INTR_DISCONNECT	0x20
#define MUSB_INTR_SESSREQ	0x40
#define MUSB_INTR_VBUSERROR	0x80

/* DEVCTL */
#define MUSB_DEVCTL_BDEVICE	0x80
#define MUSB_DEVCTL_FSDEV	0x40
#define MUSB_DEVCTL_LSDEV	0x20
#define MUSB_DEVCTL_VBUS	0x18
#define MUSB_DEVCTL_VBUS_SHIFT	3
#define MUSB_DEVCTL_HM		0x04
#define MUSB_DEVCTL_HR		0x02
#define MUSB_DEVCTL_SESSION	0x01

/* CSR0 */
#define MUSB_CSR0_FLUSHFIFO	0x0100
#define MUSB_CSR0_TXPKTRDY	0x0002
#define MUSB_CSR0_RXPKTRDY	0x0001
#define MUSB_CSR0_SVDSETUPEND	0x0080
#define MUSB_CSR0_SVDRXPKTRDY	0x0040
#define MUSB_CSR0_SENDSTALL	0x0020
#define MUSB_CSR0_SETUPEND	0x0010
#define MUSB_CSR0_DATAEND	0x0008
#define MUSB_CSR0_SENTSTALL	0x0004

/* TxType/RxType */
#define MUSB_TYPE_SPEED		0xc0
#define MUSB_TYPE_SPEED_SHIFT	6
#define MUSB_TYPE_PROTO		0x30	/* Implicitly zero for ep0 */
#define MUSB_TYPE_PROTO_SHIFT	4
#define MUSB_TYPE_REMOTE_END	0xf	/* Implicitly zero for ep0 */

/* CONFIGDATA */
#define MUSB_CONFIGDATA_MPRXE		0x80	/* Auto bulk pkt combining */
#define MUSB_CONFIGDATA_MPTXE		0x40	/* Auto bulk pkt splitting */
#define MUSB_CONFIGDATA_BIGENDIAN	0x20
#define MUSB_CONFIGDATA_HBRXE		0x10	/* HB-ISO for RX */
#define MUSB_CONFIGDATA_HBTXE		0x08	/* HB-ISO for TX */
#define MUSB_CONFIGDATA_DYNFIFO		0x04	/* Dynamic FIFO sizing */
#define MUSB_CONFIGDATA_SOFTCONE	0x02	/* SoftConnect */
#define MUSB_CONFIGDATA_UTMIDW		0x01	/* Data width 0/1 => 8/16bits */

/* TXCSR */
#define MUSB_TXCSR_TXPKTRDY		0x0001
#define MUSB_TXCSR_FIFONOTEMPTY	0x0002
#define MUSB_TXCSR_UNDERRUN		0x0004
#define MUSB_TXCSR_FLUSHFIFO	0x0008
#define MUSB_TXCSR_SENDSTALL	0x0010
#define MUSB_TXCSR_SENTSTALL	0x0020
#define MUSB_TXCSR_CLRDATATOG	0x0040
#define MUSB_TXCSR_INCOMPTX		0x0080
#define MUSB_TXCSR_DMAMODE		0x0400
#define MUSB_TXCSR_FRCDATATOG	0x0800
#define MUSB_TXCSR_DMAENAB		0x1000
#define MUSB_TXCSR_MODE			0x2000
#define MUSB_TXCSR_ISO			0x4000
#define MUSB_TXCSR_AUTOSET		0x8000

/* RXCSR */
#define MUSB_RXCSR_RXPKTRDY		0x0001
#define MUSB_RXCSR_FIFOFULL		0x0002
#define MUSB_RXCSR_OVERRUN		0x0004
#define MUSB_RXCSR_DATAERROR	0x0008
#define MUSB_RXCSR_FLUSHFIFO	0x0010
#define MUSB_RXCSR_SENDSTALL	0x0020
#define MUSB_RXCSR_SENTSTALL	0x0040
#define MUSB_RXCSR_CLRDATATOG	0x0080
#define MUSB_RXCSR_INCOMPRX		0x0100
#define MUSB_RXCSR_DMAMODE		0x0800
#define MUSB_RXCSR_ISO			0x4000
#define MUSB_RXCSR_DISNYET_PE	0x1000
#define MUSB_RXCSR_DMAENAB		0x2000
#define MUSB_RXCSR_AUTOCLEAR	0x8000

#define MUSB_TXFIFOSZ_DPB	0x10
#define MUSB_RXFIFOSZ_DPB	0x10

static void setup_dynamic_fifos(void);

enum usb_state {
	USB_DEFAULT = 0,
	USB_ADDRESS,
	USB_CONFIGURED
};

struct usbc_ep {
	bool active;
	uint width;
	uint blocksize;

	/* current data buffer */
	usbc_transfer *transfer;

	/* callback when tx or rx happens on the endpoint */
	int (*callback)(ep_t endpoint, usbc_callback_op_t op,
		usbc_transfer *transfer);
};

struct usbc_stat {
	bool active;
	enum usb_state state;
	uint8_t active_config;

	/* callback for device events */
	usb_callback callback;

	/* ep0 pending tx */
	const void *ep0_tx_buf;
	size_t ep0_tx_len;
	uint ep0_tx_pos;

	struct usbc_ep inep[16];  /* IN endpoints (device to host) */
	struct usbc_ep outep[16]; /* OUT endpoint (host to device) */
};

static struct usbc_stat *usbc;

struct usbc_callback {
	struct list_node node;
	usb_callback callback;
};

static struct list_node usbc_callback_list;

static void call_all_callbacks(usbc_callback_op_t op,
		const union usb_callback_args *arg)
{
	struct usbc_callback *cb;

	list_for_every_entry(&usbc_callback_list, cb,
			struct usbc_callback, node) {
		cb->callback(op, arg);
	}
}

static void print_usb_setup(const struct usb_setup *setup)
{
	printf("usb_setup:\n");
	printf("\ttype 0x%hhx\n", setup->request_type);
	printf("\trequest 0x%hhx\n", setup->request);
	printf("\tvalue 0x%hx\n", setup->value);
	printf("\tindex 0x%hx\n", setup->index);
	printf("\tlength 0x%hx\n", setup->length);
}

static void select_ep(uint ep)
{
	DEBUG_ASSERT(ep < MAX_ENDPOINTS);
	musb_reg8(MUSB_INDEX) = ep;
}

static void dump_ep_regs(uint ep)
{
#if 1
	select_ep(ep);

	LTRACEF("%d TXMAXP %x\n", ep, musb_reg16(MUSB_IDX_TXMAXP));
	LTRACEF("%d RXMAXP %x\n", ep, musb_reg16(MUSB_IDX_RXMAXP));
	LTRACEF("%d TXFIFOSZ %x\n", ep, musb_reg8(MUSB_TXFIFOSZ));
	LTRACEF("%d RXFIFOSZ %x\n", ep, musb_reg8(MUSB_RXFIFOSZ));
	LTRACEF("%d TXFIFOADD %x\n", ep, musb_reg16(MUSB_TXFIFOADD));
	LTRACEF("%d RXFIFOADD %x\n", ep, musb_reg16(MUSB_RXFIFOADD));
	LTRACEF("%d RXCOUNT %x\n", ep, musb_reg16(MUSB_IDX_RXCOUNT));
	LTRACEF("%d TXCSR %x\n", ep, musb_reg16(MUSB_IDX_TXCSR));
	LTRACEF("%d RXCSR %x\n", ep, musb_reg16(MUSB_IDX_RXCSR));
#endif
}

static void dump_regs()
{
#if 1

	LTRACEF("MUSB_FADDR = 0x%x\n", musb_reg8(MUSB_FADDR));
	LTRACEF("MUSB_POWER = 0x%x\n", musb_reg8(MUSB_POWER));
	LTRACEF("MUSB_INTRTXE = 0x%x\n", musb_reg16(MUSB_INTRTXE));
	LTRACEF("MUSB_INTRRXE = 0x%x\n", musb_reg16(MUSB_INTRRXE));
	LTRACEF("MUSB_INTRUSBE = 0x%x\n", musb_reg8(MUSB_INTRUSBE));
	LTRACEF("MUSB_FRAME = 0x%x\n", musb_reg16(MUSB_FRAME));
	LTRACEF("MUSB_DEVCTL = 0x%x\n", musb_reg8(MUSB_DEVCTL));
	LTRACEF("MUSB_IDX_CSR0 = 0x%x\n", musb_reg16(MUSB_IDX_CSR0));
	LTRACEF("MUSB_HWVERS = 0x%x\n", musb_reg16(MUSB_HWVERS));
	LTRACEF("MUSB_IDX_CONFIGDATA = 0x%x\n", musb_reg8(MUSB_IDX_CONFIGDATA));

#endif
}

void dump_musb_regs(void)
{
	dump_regs();
	dump_ep_regs(1);
}

void musb_reg_set8(int addr, unsigned char val)
{
	musb_reg8(addr) = val;
}

void musb_reg_set16(int addr, unsigned short val)
{
	musb_reg16(addr) = val;
}
int musb_reg_get8(int addr)
{
	return musb_reg8(addr);
}
int musb_reg_get16(int addr)
{
	return musb_reg16(addr);
}

#define MULOF4(val) (((uint32_t)(val) & 0x3) == 0)
#define MULOF2(val) (((uint32_t)(val) & 0x1) == 0)

static int read_ep_fifo(uint ep, void *_buf, size_t maxlen)
{
	char *buf = (void *)_buf;
	size_t i=0, rem=0;
	LTRACEF("ep: %d\n", ep);

	select_ep(ep);

	uint8_t fifo_reg = MUSB_FIFOBASE + ep * 4;
	size_t rxcount = musb_reg16(MUSB_IDX_RXCOUNT);

	if (rxcount > maxlen)
		rxcount = maxlen;

	if (MULOF4(buf)) {
		uint32_t *buf32 = (uint32_t *)_buf;
		for (i = 0; i < rxcount / 4; i++)
			buf32[i] = musb_reg32(fifo_reg);
		rem = rxcount % 4;
	} else if (MULOF2(buf)) {
		uint16_t *buf16 = (uint16_t *)_buf;
		for (i = 0; i < rxcount / 2; i++)
			buf16[i] = musb_reg16(fifo_reg);
		rem = rxcount % 2;
	} else {
		/* slow path */
		for (i = 0; i < rxcount; i++)
			buf[i] = musb_reg8(fifo_reg);
	}

	if (rem) {
		buf = (char *)(_buf + rxcount - rem);
		for (i = 0; i < rem; i++)
			buf[i] = musb_reg8(fifo_reg);
	}

	return rxcount;
}

static int write_ep_fifo(uint ep, const void *_buf, size_t len)
{
	char *buf = (void *)_buf;
	size_t i=0, rem=0;

	LTRACEF("ep: %d\n", ep);

	select_ep(ep);

	uint8_t fifo_reg = MUSB_FIFOBASE + ep * 4;

	if (MULOF4(buf)) {
		uint32_t *buf32 = (uint32_t *)_buf;
		for (i = 0; i < len / 4; i++)
			musb_reg32(fifo_reg) = buf32[i];
		rem = len % 4;
	} else if (MULOF2(buf)) {
		uint16_t *buf16 = (uint16_t *)_buf;
		for (i = 0; i < len / 2; i++)
			musb_reg16(fifo_reg) = buf16[i];
		rem = len % 2;
	} else {
		/* slow path */
		for (i = 0; i < len; i++)
			musb_reg8(fifo_reg) = buf[i];
	}

	if (rem) {
		buf = (char *)(_buf + len - rem);
		for (i = 0; i < rem; i++)
			musb_reg8(fifo_reg) = buf[i];
	}
	return len;
}

#undef MULOF4
#undef MULOF2

void usbc_ep0_send(const void *buf, size_t len, size_t maxlen)
{
	LTRACEF("buf %p, len %zu, maxlen %zu\n", buf, len, maxlen);

	/* trim the transfer */
	len = MIN(len, maxlen);

	size_t transfer_len = MIN(64, len);

	/* write the first 64 bytes */
	write_ep_fifo(0, buf, transfer_len);

	/* set txpktready */
	select_ep(0);
	if (len > 64) {
		/* we have more data to send, don't mark data end */
		/* TxPktRdy */
		musb_reg16(MUSB_IDX_CSR0) |= MUSB_CSR0_TXPKTRDY;

		/* save our position so we can continue */
		usbc->ep0_tx_buf = buf;
		usbc->ep0_tx_pos = 64;
		usbc->ep0_tx_len = len;
	} else {
		/* DataEnd, TxPktRdy */
		musb_reg16(MUSB_IDX_CSR0) |=
			MUSB_CSR0_TXPKTRDY | MUSB_CSR0_DATAEND;
		usbc->ep0_tx_buf = NULL;
	}
}

static void ep0_control_send_resume(void)
{
	DEBUG_ASSERT(usbc->ep0_tx_buf != NULL);
	DEBUG_ASSERT(usbc->ep0_tx_len > usbc->ep0_tx_pos);

	LTRACEF("buf %p pos %d len %d\n", usbc->ep0_tx_buf,
					usbc->ep0_tx_pos, usbc->ep0_tx_len);

	size_t transfer_len = MIN(64, usbc->ep0_tx_len - usbc->ep0_tx_pos);

	write_ep_fifo(0, (const uint8_t *)usbc->ep0_tx_buf + usbc->ep0_tx_pos,
				transfer_len);

	usbc->ep0_tx_pos += transfer_len;

	if (usbc->ep0_tx_pos >= usbc->ep0_tx_len) {
		/* completes the transfer */
		/* DataEnd,TxPktRdy */
		musb_reg16(MUSB_IDX_CSR0) |=
			MUSB_CSR0_TXPKTRDY | MUSB_CSR0_DATAEND;
		usbc->ep0_tx_buf = NULL;
	} else {
		musb_reg16(MUSB_IDX_CSR0) |= MUSB_CSR0_TXPKTRDY;
	}
}

void usbc_ep0_ack(void)
{
	/* servicedrxpktrdy & dataend */
	musb_reg16(MUSB_IDX_CSR0) |=
			MUSB_CSR0_SVDRXPKTRDY | MUSB_CSR0_DATAEND;
}

void usbc_ep0_stall(void)
{
	printf("USB STALL\n");
}

static void usb_shutdown_endpoints(void)
{
	/* iterate through all the endpoints, cancelling
	* any pending io and shut down the endpoint */
	ep_t i;
	for (i = 1; i < MAX_ENDPOINTS; i++) {
		if (usbc->inep[i].active && usbc->inep[i].transfer) {
			/* pool's closed */
			usbc_transfer *t = usbc->inep[i].transfer;
			usbc->inep[i].transfer = NULL;
			t->result = USB_TRANSFER_RESULT_CANCELLED;
			usbc->inep[i].callback(i, CB_EP_TRANSFER_CANCELLED, t);
		}
		if (usbc->outep[i].active && usbc->outep[i].transfer) {
			/* pool's closed */
			usbc_transfer *t = usbc->outep[i].transfer;
			usbc->outep[i].transfer = NULL;
			t->result = USB_TRANSFER_RESULT_CANCELLED;
			usbc->outep[i].callback(i, CB_EP_TRANSFER_CANCELLED, t);
		}
	}

	/* clear pending ep0 data */
	usbc->ep0_tx_buf = 0;
}

static void usb_enable_endpoints(void)
{
	setup_dynamic_fifos();
}

static void usb_disconnect(void)
{
	/* we've been disconnected */
	usbc->state = USB_DEFAULT;
	usbc->active_config = 0;

	usb_shutdown_endpoints();
}

static void usb_reset(void)
{
	/* this wipes out our endpoint interrupt disables */
	musb_reg16(MUSB_INTRTXE) = MUSB_INTRTXE_EP0;
	musb_reg16(MUSB_INTRRXE) = 0;

	usb_shutdown_endpoints();
}

static int handle_ep_rx(int ep)
{
	struct usbc_ep *e = &usbc->outep[ep];
	int len = 0;
	LTRACEF("ep: %d\n", ep);

	DEBUG_ASSERT(e->active);

	usbc_transfer *t = e->transfer;
	if (!t) {
		LTRACEF("buffer is not available to recieve %d bytes.\n",
					musb_reg16(MUSB_IDX_RXCOUNT));
		return 0;
	}

	uint rxcount = musb_reg16(MUSB_IDX_RXCOUNT);
	uint readcount = MIN(rxcount, t->buflen - t->bufpos);
	readcount = MIN(readcount, e->blocksize);
	if (readcount > 0) {
		len = read_ep_fifo(ep, (uint8_t *)t->buf + t->bufpos,
						readcount);
		LTRACEF("Rx %d bytes\n", len);
	}

	/* no more packet ready */
	musb_reg16(MUSB_IDX_RXCSRL) &= ~MUSB_RXCSR_RXPKTRDY;

	t->bufpos += len;

	if (rxcount < e->blocksize ||  t->bufpos >= t->buflen) {
		/* we're done with this transfer */
		e->transfer = NULL;
		/* musb_reg16(MUSB_INTRRXE) &= ~(1<<ep); */

		t->result = USB_TRANSFER_RESULT_OK;

		DEBUG_ASSERT(e->callback);
		e->callback(ep, CB_EP_RXCOMPLETE, t);

		return 1;
	}

	return 0;
}

bool usbc_is_highspeed(void)
{
	return (musb_reg8(MUSB_POWER) & (1<<4)) ? true : false;
}

static enum handler_return musb_interrupt(void *arg)
{
	int i;
	uint16_t intrtx = musb_reg16(MUSB_INTRTX);
	uint16_t intrrx = musb_reg16(MUSB_INTRRX);
	uint8_t intrusb = musb_reg8(MUSB_INTRUSB);
	enum handler_return ret = INT_NO_RESCHEDULE;

	LTRACEF("intrtx %x:%x, intrrx %x:%x, intrusb %x, intrusbe %x\n",
			intrtx, musb_reg16(MUSB_INTRTXE),
			intrrx, musb_reg16(MUSB_INTRRXE), intrusb,
			musb_reg8(MUSB_INTRUSBE));

	/* look for global usb interrupts */
	intrusb &= musb_reg8(MUSB_INTRUSBE);
	if (intrusb) {
		if (intrusb & MUSB_INTR_SUSPEND) {
			/* suspend */
			TRACEF("suspend\n");
			call_all_callbacks(CB_SUSPEND, 0);
			ret = INT_RESCHEDULE;
		}
		if (intrusb & MUSB_INTR_RESUME) {
			/* resume */
			TRACEF("resume\n");
			call_all_callbacks(CB_RESUME, 0);
			ret = INT_RESCHEDULE;
		}
		if (intrusb & MUSB_INTR_RESET) {
			/* reset */
			TRACEF("reset\n");
			TRACEF("high speed %d\n",
				musb_reg8(MUSB_POWER) & MUSB_POWER_RESET ? 1 : 0);
			call_all_callbacks(CB_RESET, 0);
			usb_reset();
			ret = INT_RESCHEDULE;
		}
		if (intrusb & MUSB_INTR_SOF) {
			/* SOF */
			TRACEF("sof\n");
		}
		if (intrusb & MUSB_INTR_CONNECT) {
			/* connect (host only) */
			TRACEF("connect\n");
		}
		if (intrusb & MUSB_INTR_DISCONNECT) {
			/* disconnect */
			TRACEF("disconnect\n");
			call_all_callbacks(CB_DISCONNECT, 0);
			usb_disconnect();
			ret = INT_RESCHEDULE;
		}
		if (intrusb & MUSB_INTR_SESSREQ) {
			/* session request (A device only) */
			TRACEF("session request\n");
		}
		if (intrusb & MUSB_INTR_VBUSERROR) {
			/* vbus error (A device only) */
			TRACEF("vbus error\n");
		}
	}

	/* look for endpoint 0 interrupt */
	if (intrtx & MUSB_INTRTXE_EP0) {
		select_ep(0);
		uint16_t csr = musb_reg16(MUSB_IDX_CSR0);
		LTRACEF("ep0 csr 0x%hhx\n", csr);

		/* clear the stall bit */
		if (csr & MUSB_CSR0_SENTSTALL)
			musb_reg16(MUSB_IDX_CSR0) &= ~MUSB_CSR0_SENTSTALL;

		/* do we have any pending tx data? */
		if (usbc->ep0_tx_buf != NULL) {
			if (csr & MUSB_CSR0_SETUPEND) { /* setup end */
				/* we got an abort on the data transfer */
				usbc->ep0_tx_buf = NULL;
			} else {
				/* send more data */
				ep0_control_send_resume();
			}
		}

		/* clear the setup end bit */
		if (csr & MUSB_CSR0_SETUPEND)
			/* servicedsetupend */
			musb_reg16(MUSB_IDX_CSR0) |= MUSB_CSR0_SVDSETUPEND;

		if (csr & MUSB_CSR0_RXPKTRDY) {
			/* rxpktrdy */
			LTRACEF("ep0: rxpktrdy, cnt %d\n",
						musb_reg16(MUSB_IDX_RXCOUNT));

			struct usb_setup setup;
			read_ep_fifo(0, (void *)&setup, sizeof(setup));
			/*print_usb_setup(&setup); */
			/* servicedrxpktrdy */
			musb_reg16(MUSB_IDX_CSR0) |= MUSB_CSR0_SVDRXPKTRDY;

			union usb_callback_args args;
			args.setup = &setup;
			call_all_callbacks(CB_SETUP_MSG, &args);

			switch (setup.request) {
			case SET_ADDRESS: {
				LTRACEF("SET_ADDRESS:%d\n",
							setup.value);
				dprintf(INFO, "usb: Set address %d\n",
							setup.value);
				usbc_ep0_ack();

				musb_reg8(MUSB_FADDR) = setup.value;
				if (setup.value == 0)
					usbc->state = USB_DEFAULT;
				else
					usbc->state = USB_ADDRESS;

				break;
			}
			case SET_CONFIGURATION:
				LTRACEF("SET_CONFIGURATION:%d\n",
							setup.value);

				if (setup.value == 0) {
					if (usbc->state == USB_CONFIGURED)
						usbc->state = USB_ADDRESS;
					call_all_callbacks(CB_OFFLINE, 0);
				} else {
					usbc->state = USB_CONFIGURED;
					call_all_callbacks(CB_ONLINE, 0);
				}
				usbc->active_config = setup.value;
				ret = INT_RESCHEDULE;

				/* set up all of the endpoints */
				usb_enable_endpoints();
				break;
		}
		}
	}

	/* handle endpoint interrupts */

	for (i = 1; i < MAX_ENDPOINTS; i++) {
		if (intrtx & (1<<i)) {
			select_ep(i);

			/* data was sent, see if we have more to send */
			struct usbc_ep *e = &usbc->inep[i];
			/* interrupts shouldn't be enabled
			* if there isn't a transfer queued */
			DEBUG_ASSERT(e->transfer);
			usbc_transfer *t = e->transfer;

			if (!(musb_reg16(MUSB_INTRTXE) & (1<<i))) {
				LTRACEF("ep=%d spurious!!!\n", i);
				goto continue_rx;
			}
			if (t->bufpos < t->buflen) {
				LTRACEF("txcsr %i: %x\n", i,
						musb_reg16(MUSB_IDX_TXCSR));
				/* cram more stuff in the buffer */
				uint queuelen = MIN(e->blocksize,
						t->buflen - t->bufpos);
				LTRACEF("writing more tx data len %d,rem %d\n",
					queuelen, t->buflen - t->bufpos);
				write_ep_fifo(i, (uint8_t *)t->buf + t->bufpos,
								queuelen);
				t->bufpos += queuelen;

				/* start the transfer */
				/* txpktrdy */
				musb_reg16(MUSB_IDX_TXCSR) |= MUSB_TXCSR_TXPKTRDY;
			} else {
				/* we're done, callback */
				musb_reg16(MUSB_INTRTXE) &= ~(1<<i);
				/* txpktrdy */
				musb_reg16(MUSB_IDX_TXCSR) &= ~MUSB_TXCSR_TXPKTRDY;

				LTRACEF("txcsr %i: %x\n", i,
						musb_reg16(MUSB_IDX_TXCSR));
				LTRACEF("write complete\n");
				e->transfer = NULL;

				t->result = USB_TRANSFER_RESULT_OK;

				DEBUG_ASSERT(e->callback);
				/*dump_ep_regs(i); */
				e->callback(i, CB_EP_TXCOMPLETE, t);
				ret = INT_RESCHEDULE;
			}
		}
continue_rx:
		if (intrrx & (1<<i)) {
			select_ep(i);
			uint16_t rxcount = musb_reg16(MUSB_IDX_RXCOUNT);
			uint16_t csr = musb_reg16(MUSB_IDX_RXCSR);
			LTRACEF("rxcsr %i: 0x%hx, rxcount=%d(%d)\n",
					i, csr, musb_reg16(MUSB_IDX_RXCOUNT),
					rxcount);

			if (csr & MUSB_RXCSR_RXPKTRDY) { /* rxpktrdy */
				/* see if the endpoint is ready */
				struct usbc_ep *e = &usbc->outep[i];
				if (!e->active) {
					/* stall it */
					musb_reg16(MUSB_IDX_RXCSR) |= MUSB_RXCSR_SENDSTALL;
					/* flush fifo */
					musb_reg16(MUSB_IDX_RXCSR) |= MUSB_RXCSR_FLUSHFIFO;
					panic("rx on inactive endpoint\n");
					continue;
				}
				if (handle_ep_rx(i) > 0)
					ret = INT_RESCHEDULE;
			}
		}
	}

	return ret;
}


void usbc_setup_endpoint(ep_t ep, ep_dir_t dir, bool active,
		ep_callback callback, uint width, uint blocksize)
{
	DEBUG_ASSERT(ep != 0);
	DEBUG_ASSERT(ep < MAX_ENDPOINTS);
	DEBUG_ASSERT(dir == IN || dir == OUT);

	struct usbc_ep *e;
	if (dir == IN)
		e = &usbc->inep[ep];
	else
		e = &usbc->outep[ep];

	/* for now we can only make active */
	e->active = active;
	e->callback = callback;
	e->width = width;
	e->blocksize = blocksize;
}

int usbc_queue_rx(ep_t ep, usbc_transfer *transfer)
{
	LTRACEF("ep %u, transfer %p (buf %p, len %zu)\n",
		ep, transfer, transfer->buf, transfer->buflen);
	struct usbc_ep *e = &usbc->outep[ep];

	DEBUG_ASSERT(ep != 0);
	DEBUG_ASSERT(ep < MAX_ENDPOINTS);
	DEBUG_ASSERT(e->active);

	DEBUG_ASSERT(transfer);
	DEBUG_ASSERT(transfer->buf);

	DEBUG_ASSERT(e->transfer == NULL);

	/* can only queue up multiples of the endpoint blocksize */
	DEBUG_ASSERT(transfer->buflen >= e->blocksize &&
				(transfer->buflen % e->blocksize) == 0);

	enter_critical_section();

	if (usbc->state != USB_CONFIGURED) {
		/* can't transfer now */
		exit_critical_section();
		return -1;
	}

	e->transfer = transfer;

	select_ep(ep);
	if (musb_reg16(MUSB_IDX_RXCSR) & MUSB_RXCSR_RXPKTRDY) {
		/* packet is already ready */
		LTRACEF("****packet already ready (%d)\n",
						musb_reg16(MUSB_IDX_RXCOUNT));

		int rc = handle_ep_rx(ep);
		if (rc > 0) {
			/* the transfer was completed */
			goto done;
		}
	}

	/* unmask irqs for this endpoint */
	musb_reg16(MUSB_INTRRXE) |= (1<<ep);

done:
	exit_critical_section();

	return 0;
}

int usbc_queue_tx(ep_t ep, usbc_transfer *transfer)
{
	LTRACEF("ep %u, transfer %p (buf %p, len %zu)\n",
			ep, transfer, transfer->buf, transfer->buflen);
	struct usbc_ep *e = &usbc->inep[ep];

	DEBUG_ASSERT(ep != 0);
	DEBUG_ASSERT(ep < MAX_ENDPOINTS);
	DEBUG_ASSERT(e->active);

	DEBUG_ASSERT(e->transfer == NULL);

	enter_critical_section();

	if (usbc->state != USB_CONFIGURED) {
		/* can't transfer now */
		LTRACEF("ep %d Device not yet Configured\n", ep);
		exit_critical_section();
		return -1;
	}

	e->transfer = transfer;

	select_ep(ep);

	/* unmask irqs for this endpoint */
	musb_reg16(MUSB_INTRTXE) |= (1<<ep);

	/* if the fifo is empty, start the transfer */
	if ((musb_reg16(MUSB_IDX_TXCSR) & MUSB_TXCSR_FIFONOTEMPTY) == 0) {
		uint queuelen = MIN(e->blocksize, transfer->buflen);
		write_ep_fifo(ep, transfer->buf, queuelen);
		transfer->bufpos = queuelen;

		/* start the transfer */
		musb_reg16(MUSB_IDX_TXCSR) |= MUSB_TXCSR_TXPKTRDY; /* txpktrdy */
	} else {
		LTRACEF("ep %d, TX FIFO Empty. TXCSR:%x\n",
					ep, musb_reg16(MUSB_IDX_TXCSR));
	}

	exit_critical_section();

	return 0;
}

int usbc_set_callback(usb_callback callback)
{

	DEBUG_ASSERT(callback != NULL);

	struct usbc_callback *cb = malloc(sizeof(struct usbc_callback));
	if (!cb)
       return -1;

	enter_critical_section();

	cb->callback = callback;
	list_add_head(&usbc_callback_list, &cb->node);

	exit_critical_section();
	return 0;
}

int usbc_set_active(bool active)
{
	LTRACEF("active %d\n", active);
	if (active) {
		DEBUG_ASSERT(!usbc->active);

		musb_reg8(MUSB_POWER) |= MUSB_POWER_SOFTCONN; /* soft conn */
		usbc->active = true;
	} else {
		musb_reg8(MUSB_POWER) &= ~MUSB_POWER_SOFTCONN; /* soft conn */
		usbc->active = false;
	}

	return 0;
}

static void setup_dynamic_fifos(void)
{

#if LOCAL_TRACE
	uint8_t raminfo = musb_reg8(MUSB_RAMINFO);
	size_t ramsize = (1 << ((raminfo & 0xf) + 2));
	LTRACEF("%zd bytes of onboard ram\n", ramsize);
#endif

	uint32_t offset = 128;

	int highspeed = musb_reg8(MUSB_POWER) & MUSB_POWER_HSMODE;

	int i;
	for (i = 1; i < MAX_ENDPOINTS; i++) {
		select_ep(i);
		if (usbc->inep[i].active) {
			/* 512 byte, double buffered */
			musb_reg8(MUSB_TXFIFOSZ) = MUSB_TXFIFOSZ_DPB|(0x6);
			musb_reg16(MUSB_TXFIFOADD) = offset / 8;
			if (highspeed)
				musb_reg16(MUSB_IDX_TXMAXP) =
						usbc->inep[i].width;
			else
				musb_reg16(MUSB_IDX_TXMAXP) =
				(((usbc->inep[i].blocksize / 64) - 1)
				<< 11) | 64;

			musb_reg16(MUSB_IDX_TXCSR) = MUSB_TXCSR_FRCDATATOG|MUSB_TXCSR_MODE;
			musb_reg8(MUSB_IDX_TXCSRL) = MUSB_TXCSR_FLUSHFIFO;
			musb_reg8(MUSB_IDX_TXCSRL) = MUSB_TXCSR_FLUSHFIFO;
			offset += 512*2;
		} else {
			musb_reg8(MUSB_TXFIFOSZ) = 0;
			musb_reg16(MUSB_TXFIFOADD) = 0;
			musb_reg16(MUSB_IDX_TXMAXP) = 0;
		}

		if (usbc->outep[i].active) {
			/* 512 byte, single buffered */
			musb_reg8(MUSB_RXFIFOSZ) = MUSB_RXFIFOSZ_DPB|(0x6);
			musb_reg16(MUSB_RXFIFOADD) = offset / 8;
			if (highspeed)
				musb_reg16(MUSB_IDX_RXMAXP) =
						usbc->inep[i].width;
			else
				musb_reg16(MUSB_IDX_RXMAXP) =
				(((usbc->outep[i].blocksize / 64) - 1)
				<< 11) | 64;

			offset += 512*2;
			musb_reg8(MUSB_IDX_RXCSRL) = MUSB_RXCSR_CLRDATATOG;
		} else {
			musb_reg8(MUSB_RXFIFOSZ) = 0;
			musb_reg16(MUSB_RXFIFOADD) = 0;
			musb_reg16(MUSB_IDX_RXMAXP) = 0;
		}
	}
}

static void musb_init(void)
{
	/* select endpoint 0 */
	dprintf(INFO, "hwvers 0x%hx\n", musb_reg16(MUSB_HWVERS));
	dprintf(INFO, "epinfo 0x%hhx\n", musb_reg8(MUSB_EPINFO));
	dprintf(INFO, "raminfo 0x%hhx\n", musb_reg8(MUSB_RAMINFO));
	musb_reg8(MUSB_INDEX) = 0;
	dprintf(INFO, "config 0x%hhx\n", musb_reg8(MUSB_IDX_CONFIGDATA));

	/* assert that we have dynamic fifo sizing */
	DEBUG_ASSERT(musb_reg8(MUSB_IDX_CONFIGDATA) & (1<<2));

	/* mask all the interrupts for the endpoints (except 0) */
	musb_reg16(MUSB_INTRTXE) = MUSB_INTRTXE_EP0;
	musb_reg16(MUSB_INTRRXE) = 0;
	musb_reg8(MUSB_DEVCTL) = 0; /* peripheral mode */
	musb_reg8(MUSB_POWER) |= MUSB_POWER_HSENAB;
	musb_reg8(MUSB_INTRUSBE) = MUSB_INTR_SUSPEND|
			MUSB_INTR_RESUME|MUSB_INTR_RESET|
			MUSB_INTR_BABBLE|MUSB_INTR_CONNECT|
			MUSB_INTR_DISCONNECT|MUSB_INTR_SESSREQ|
			MUSB_INTR_VBUSERROR;
}

static void musb_uninit(void)
{
	/* select endpoint 0 */
	dprintf(INFO, "hwvers 0x%hx\n", musb_reg16(MUSB_HWVERS));
	dprintf(INFO, "epinfo 0x%hhx\n", musb_reg8(MUSB_EPINFO));
	dprintf(INFO, "raminfo 0x%hhx\n", musb_reg8(MUSB_RAMINFO));
	musb_reg8(MUSB_INDEX) = 0;
	dprintf(INFO, "config 0x%hhx\n", musb_reg8(MUSB_IDX_CONFIGDATA));

	/* mask all the interrupts for the endpoints (except 0) */
	musb_reg16(MUSB_INTRTXE) = 0;
	musb_reg16(MUSB_INTRRXE) = 0;
	musb_reg8(MUSB_DEVCTL) = 0; /* peripheral mode */
	musb_reg8(MUSB_POWER) = 0;
	musb_reg8(MUSB_INTRUSBE) = 0;
}


int usbc_init(void)
{
	/* allocate some ram for the usb struct */
	usbc = malloc(sizeof(struct usbc_stat));
	if(!usbc)
       return -1;

    memset(usbc, 0, sizeof(struct usbc_stat));

	usbc->state = USB_DEFAULT;

	/* initialize the callback list */
	list_initialize(&usbc_callback_list);

	/* register the interrupt handlers */
	register_int_handler(IRQ_USB_OTG, musb_interrupt, NULL);
	musb_init();

	unmask_interrupt(IRQ_USB_OTG);
	dump_regs();
	dump_ep_regs(0);
	return 0;
}

void usbc_exit(void)
{
	mask_interrupt(IRQ_USB_OTG);
	musb_uninit();
	list_remove_head(&usbc_callback_list);
	usbc->state = USB_DEFAULT;

	/* de-allocate usb struct */
	free(usbc);
	usbc = NULL;
}


