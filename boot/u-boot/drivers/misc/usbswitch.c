/*
 * usbswitch.c
 *
 * Copyright (C) ST-Ericssson Le Mans SA 2012
 * Author: etienne carriere <etienne.carriere@stericsson.com> for ST-Ericssson Le Mans.
 * License terms:  GNU General Public License (GPL), version 2
 */
#include <common.h>
#include <asm/errno.h>
#include <asm/io.h>
#include <asm/arch/ab8500.h>
#include <usbswitch.h>

static int uart_on_ulpi(int verbose);
static int usb_on_ulpi(int verbose);

/* dev purpose only. can wdg be disabled ? */
static void dev_wd_kick_disable(void)
{
	ab8500_write(0x02, 0x01, 1); // ena wdog
	udelay(10000);
	ab8500_write(0x02, 0x01, 3); // kick wdog
	udelay(10000);
	ab8500_write(0x02, 0x01, 0); // dis wdog
}

static inline int ab8500_write_mask(u8 bank, u8 off, u8 data, u8 mask)
{
	int val;
	val = ab8500_read(bank, off);
	if (val < 0)
		return -1;
	val = ((val & ~mask) | (data & mask)) & 0xFF;
	if (ab8500_write(bank, off, val) < 0)
		return -1;
	return 0;
}

static void usbswitch_print_detection(void)
{
	int val;
	printf("USB related detections in ITLatchN registers\n");
	val = ab8500_read(0x0E,0x01);
	printf(" 0x0E01 = %02X  ITSource2   (Vbus ch: %d)\n",
			val, !!(val & 0x80));
	val = ab8500_read(0x0E,0x21);
	printf(" 0x0E21 = %02X  ITLatch2    (VbusDetR: %d, VbusDetF %d)\n",
			val, !!(val & 0x80), !!(val & 0x40));
	val = ab8500_read(0x0E,0x22);
	printf(" 0x0E22 = %02X  ITLatch3    (OVV: %d)\n",
			val, !!(val & 0x40));
	val = ab8500_read(0x0E,0x2B);
	printf(" 0x0E2B = %02X  ITLatch12   (usblinkstatus changed: %d, phypowoff: %d)\n",
			val, !!(val & 0x80), !!(val & 0x40));
}
static void usbswitch_print_usbregs(void)
{
	printf("USB related registers from AB chip\n");
	printf(" - UsbLinkStatus 0x0594: 0x%02X\n", ab8500_read(0x05, 0x94));
	printf(" - UsbLineStatus 0x0580: 0x%02X\n", ab8500_read(0x05, 0x80));
	printf(" - UsbOTGCtrl    0x0587: 0x%02X\n", ab8500_read(0x05, 0x87));
	printf(" - UsbOTGStatus  0x0588: 0x%02X\n", ab8500_read(0x05, 0x88));
	printf(" - UsbPhyCtrl    0x058A: 0x%02X\n", ab8500_read(0x05, 0x8A));
	printf(" - UsbPHYStatus  0x0589: 0x%02X\n", ab8500_read(0x05, 0x89));
	printf(" - DetId4        0x0BA3: 0x%02X\n", ab8500_read(0x0B, 0xA3));
}

/* usbswitch_prelude - Prepare AB9540 for USB-ID detection */
static int usbswitch_prelude(int *usbphyctrl, int *usbotgctrl, int *vbus_status)
{
	int val;

	*usbotgctrl = ab8500_read(0x05, 0x87);
	*usbphyctrl = ab8500_read(0x05, 0x8A);
	*vbus_status = ab8500_read(0x0E, 0x01);
	if ((*usbotgctrl == 0) || (usbphyctrl == 0) || (vbus_status == 0))
		return -1;

	*vbus_status = (*vbus_status & 0x80) ? 1 : 0;

	if (*vbus_status) {
		ab8500_read(0x0E,0x21); /* clear IT */
		ab8500_read(0x0E,0x2B); /* clear IT */

		ab8500_write_mask(0x0B, 0xA2, 0x04, 0x04);	/* disconnect VBUS pin */
		udelay(400000);

		val = ab8500_read(0x0E,0x21); /* get/clear VbusDet IT */
		if ((val & 0x40) == 0)
			printf("warning: VbusDetF not detected at VBUS disconnection\n");
		val = ab8500_read(0x0E,0x2B); /* get/clear UsbLinkStatus IT */
		if ((val & 0x80) == 0)
			printf("warning: UsbLinkSt. not detected at VBUS disconnection\n");
	}

	ab8500_write_mask(0x05, 0x8A, 0x00, 0x03);	/* disable PHY */
	udelay(10000);

	ab8500_write_mask(0x05, 0x87, 0x00, 0x23);	/* disable VBUS/ID detection/comparators */
	ab8500_write_mask(0x0B, 0xA2, 0x80, 0x80);	/* IDDET controlled from SW (not FSM) */

	return 0;
}

/* usbswitch_epilogue - Restore AB9540 after manual USB-ID detection */
static int usbswitch_epilogue(int usbphyctrl, int usbotgctrl, int vbus_status)
{
	ab8500_write_mask(0x05, 0x87, usbotgctrl, 0x23);	/* enable back VBUS/ID det/comp */
	ab8500_write_mask(0x0B, 0xA2, 0x00, 0x80);	/* IDDET controlled from FSM (not SW) */

	if (vbus_status) {
		ab8500_read(0x0E,0x21); /* clear IT */
		ab8500_read(0x0E,0x2B); /* clear IT */
	}
	ab8500_write_mask(0x0B, 0xA2, 0x00, 0x04);	/* reconnect VBUS pin */
	udelay(100000);
	if (vbus_status) {
		if ((ab8500_read(0x0E,0x21) & 0x80) == 0)
			printf("warning: VbusDetR not detected at VBUS re-connection\n");
		if ((ab8500_read(0x0E,0x2B) & 0x80) == 0)
			printf("warning: UsbLinkSt. not detected at VBUS re-connection\n");
	}

	if (usbphyctrl & 3) {
		udelay(100000);
		ab8500_write_mask(0x05, 0x8A, usbphyctrl, 0x03);
	}

	return 0;
}

/* usbswith_idplugdet - Detection resistance < 1.05MOhm on USB-ID pin */
static int usbswith_idplugdet(void)
{
	int val;

	ab8500_write_mask(0x0B, 0xA1, 0x20, 0x20);	/* ena pullup 1uA on ID */
	ab8500_write_mask(0x0B, 0xA0, 0x08, 0x08);	/* ena ID plug detect comparator */
	udelay(10000);
	val = ab8500_read(0x0B, 0xA7); /* bit[3]: 0=ID floating, 1: ID below 1.05MOhm */
	if (val < 0)
		return val;

	ab8500_write_mask(0x0B, 0xA1, 0x00, 0x20);	/* disable pullup 1uA on ID */
	ab8500_write_mask(0x0B, 0xA0, 0x00, 0x08);	/* disable ID plug detect comparator */

	printf("id_plug_det returns %s\n", (val & 0x08) ? "true" : "false");
	return !!(val & 0x08);
}

/* usbswith_iddet - test some usefull USB-ID resistances */
static int usbswith_iddet(void)
{
	int val = 0;

	ab8500_write(0x0B, 0xA1, 0x87); /* enable detection supplies */

	ab8500_write(0x0B, 0xA6, 0x40); /* 619kOhm thresh. */
	udelay(30000);
	val = ab8500_read(0x0B, 0xA7);
	if (val < 0)
		goto done;
	if ((val & 0x60) == 0x20) {
		val = 619000;
		goto done;
	}

	ab8500_write(0x0B, 0xA6, 0x20); /*  523kOhm thresh. */
	udelay(30000);
	val = ab8500_read(0x0B, 0xA7);
	if (val < 0)
		goto done;
	if ((val & 0x60) == 0x20) {
		val = 523000;
		goto done;
	}

	ab8500_write(0x0B, 0xA6, 0x10); /*  440kOhm thresh. */
	udelay(30000);
	val = ab8500_read(0x0B, 0xA7);
	if (val < 0)
		goto done;
	if ((val & 0x60) == 0x20) {
		val = 440000;
		goto done;
	}

	ab8500_write(0x0B, 0xA6, 0x08); /*  301kOhm thresh. */
	udelay(30000);
	val = ab8500_read(0x0B, 0xA7);
	if (val < 0)
		goto done;
	if ((val & 0x60) == 0x20) {
		val = 301000;
		goto done;
	}

	ab8500_write(0x0B, 0xA6, 0x04); /*  255kOhm thresh. */
	udelay(30000);
	val = ab8500_read(0x0B, 0xA7);
	if (val < 0)
		goto done;
	if ((val & 0x60) == 0x20) {
		val = 255000;
		goto done;
	}

	ab8500_write(0x0B, 0xA6, 0x02); /*  200kOhm thresh. */
	udelay(30000);
	val = ab8500_read(0x0B, 0xA7);
	if (val < 0)
		goto done;
	if ((val & 0x60) == 0x20) {
		val = 200000;
		goto done;
	}
	val = 0;

done:
	ab8500_write(0x0B, 0xA1, 0x00);				/*  disable all ID pullups */
	if (val == 0)
		printf("iddet returns nothing\n");
	else
		printf("iddet returns %dkOhm resistance\n", val/1000);
	return val;
}

#define	GPADC_DATA_AVAIL(val) \
	do { \
		val = ab8500_read(0x0E, 0x24); \
		if (val < 0) \
			goto end; \
	} while ((val & 0x80) == 0)

#define	GPADC_NOT_BUSY(val) \
	do { \
		val = ab8500_read(0x0A, 0x04); \
		if (val < 0) \
			goto end; \
	} while (val & 0x01);

struct cust_rid_adcid {
	u32 min;	/* min threshold for ADCid for resistance measure */
	u32 max;	/* max threshold for ADCid for resistance measure */
	u32 mean;	/* mean ADC id measured */
	int res;	/* USB-ID resistance in Ohms */
};

#define CUST_RID_ADCID(mn, mx, m, r) {	.min = mn, .max = mx, .mean = m, .res = r }
#define CUST_RID_ADCID_END { .min = 0, .max = 0, .mean = 0, .res = 0 }

static const struct cust_rid_adcid cust_rid_adcid_1_8V_200k[] = {
	/*             min,   max,   mean,  kOhm   Vref=1.8V/200K => up to 400kOhm */
	CUST_RID_ADCID(0x097, 0x0BF, 0x0AB, 28700),
	CUST_RID_ADCID(0x172, 0x188, 0x186, 80000),
	CUST_RID_ADCID(0x1BB, 0x1DF, 0x1CD, 102000),
	CUST_RID_ADCID(0x1F0, 0x214, 0x202, 121000),
	CUST_RID_ADCID(0x237, 0x25B, 0x249, 150000),
	CUST_RID_ADCID(0x2E6, 0x310, 0x2FC, 255000),
	CUST_RID_ADCID(0x31D, 0x348, 0x333, 301000),
	CUST_RID_ADCID(0x35E, 0x383, 0x373, 365000),
	CUST_RID_ADCID_END
};

static const struct cust_rid_adcid cust_rid_adcid_1uA[] = {
	/*             min,   max,   mean,  kOhm   Iref=1uA => 500k to 1MOhm */
	CUST_RID_ADCID(0x177, 0x1A1, 0x18B, 523000),
	CUST_RID_ADCID(0x1C0, 0x1E8, 0x1D3, 619000),
	CUST_RID_ADCID(0x2E4, 0x308, 0x2F6, 1000000),
	CUST_RID_ADCID_END
};

/* usbswith_adc_id - measure USB-ID resistance against target table
 */
static int usbswith_adc_id(int verbose)
{
	int val = 0, adc, res = -1;

	ab8500_write(0x0B, 0xA1, 0x00);				/* disable pullups on ID */
	ab8500_write_mask(0x03, 0x80, 0x02, 0x02);	/* enable GPADC regulator */
	ab8500_write_mask(0x0E, 0x44, 0x80, 0x80);	/* mask IT from GPADC (we're polling) */
	udelay(10000);
	ab8500_write_mask(0x0A, 0x00, 0x01, 0x01);	/* enable ADC */
	ab8500_write_mask(0x05, 0x81, 0x01, 0x01);	/* connect ID to GPADC */
	GPADC_NOT_BUSY(val);
	ab8500_write(0x0A, 0x01, 0x6E);			/* ID measure, over 16 samples (1 is sufficient) */
	ab8500_write(0x0A, 0x00, 0xC1);			/* enable ADC charg. current sense + chan buff */

	/* ID resistance read using pullup 1.8V/200kOhm */
	ab8500_write(0x0B, 0xA1, 0x40);			/* enable pullup 1.8V/200kOhm on ID */
	udelay(10000);
	ab8500_write(0x0A, 0x00, 0xC5);			/* enable ADC sw convertion */
	GPADC_DATA_AVAIL(val);
	adc = ab8500_read(0x0A, 0x05);
	adc |= ab8500_read(0x0A, 0x06) << 8;

	if (verbose) {
		if (adc < 0)
			printf("- failure on ADC read with pullup 1.8V/200kOhm\n");
		else if (adc < 0x3FF)
			printf("ADCid=0x%04X (%dmV / %dkOhm)\n",
					adc, (adc * 1350) / 1023,
					(200 * adc) / (((1023 * 1800) / 1350) - adc));
		else
			printf("gpadc satuartion\n");
	}

	if (adc < 0x3FF) {
		struct cust_rid_adcid *p;
		p = (struct cust_rid_adcid *) cust_rid_adcid_1_8V_200k;
		while (p->max != 0) {
			if ((adc >= p->min) && (adc <= p->max)) {
				res = p->res;
				goto end;
			}
			p++;
		}
	}

	/* ID resistance read using 1uA current source */
	ab8500_write(0x0B, 0xA1, 0x20);			/* enable pullup 11.8V/10kOhm on ID */
	udelay(10000);
	ab8500_write(0x0A, 0x00, 0xC5);			/* enable ADC sw convertion */
	GPADC_DATA_AVAIL(val);
	adc = ab8500_read(0x0A, 0x05);
	adc |= ab8500_read(0x0A, 0x06) << 8;

	if (verbose) {
		if (adc < 0)
			printf("- failure on ADC read with pullup 1.8V/200kOhm\n");
		else if (adc < 0x3FF)
			printf("ADCid=0x%04X (%dmV / %dkOhm)\n",
					adc, (adc * 1350) / 1023,
					(200 * adc) / (((1023 * 1800) / 1350) - adc));
		else
			printf("gpadc satuartion\n");
	}

	if (adc < 0x3FF) {
		struct cust_rid_adcid *p;
		p = (struct cust_rid_adcid *) cust_rid_adcid_1uA;
		while (p->max != 0) {
			if ((adc >= p->min) && (adc <= p->max)) {
				res = p->res;
				goto end;
			}
			p++;
		}
	}

end:
	ab8500_write_mask(0x05, 0x81, 0x00, 0x01);	/* disconnect GPADC from ID */
	ab8500_write(0x0B, 0xA1, 0x00);				/* disable pullups on ID */
	ab8500_write(0x0A, 0x00, 0x00);		/* disable ADC */
	ab8500_write(0x0A, 0x01, 0x00);		/* reset ADC */
	ab8500_write_mask(0x03, 0x80, 0x00, 0x02);	/* disable GPADC regulator */
	if (verbose) {
		if (res > 0)
			printf("- ADC high tolerance => %d Ohm\n", res);
		else
			printf("- ADC high tolerance => error\n");
	}
	return res;
}

/* usbswitch_detect - detect if USB cable plugged, using iddet if required
 * this routine allows to detect some uUSB cables even if AB9540 cut 1.0 is
 * has some bugs: AB9540 register UsbLinkStatus does not detect all cables.
 */
enum usbswitch_link usbswitch_detect(void)
{
	int val, ret,  vbus_status, usblinkstatus, usbotgctrl, usbphyctrl;

	dev_wd_kick_disable();		/* insure detection is operational: kick wd */

	val = ab8500_read(0x05, 0x94);
	if (val < 0)
		goto err;
	usblinkstatus = (val >> 3) & 0x1F;

	switch(usblinkstatus) {
	case 0x17:
		return USBSWITCH_SSG_USB_BOOT_OFF;
	case 0x18:
		return USBSWITCH_SSG_USB_BOOT_ON;
	case 0x19:
		return USBSWITCH_SSG_UART_BOOT_OFF;
	case 0x1A:
		return USBSWITCH_SSG_UART_BOOT_ON;
	}

	ret = 0;
	val = usbswitch_prelude(&usbphyctrl, &usbotgctrl, &vbus_status);
	if (val < 0)
		goto err;

	val = usbswith_idplugdet();
	if (val < 0)
		goto err;
	if (val == 0) {
		ret = (vbus_status) ? USBSWITCH_VBUS_STD : USBSWITCH_NONE;
		goto end;
	}

	val = usbswith_iddet();
	if (val < 0)
		goto err;
	ret = (val == 619000) ? USBSWITCH_SSG_UART_BOOT_ON :
			(val == 523000) ? USBSWITCH_SSG_UART_BOOT_OFF :
			(val == 301000) ? USBSWITCH_SSG_USB_BOOT_ON :
			(val == 255000) ? USBSWITCH_SSG_USB_BOOT_OFF :
			0;
	if (ret)
		goto end;

	val = usbswith_adc_id(1);
	if (val < 0)
		goto err;
	ret = (val == 619000) ? USBSWITCH_SSG_UART_BOOT_ON :
			(val == 523000) ? USBSWITCH_SSG_UART_BOOT_OFF :
			(val == 301000) ? USBSWITCH_SSG_USB_BOOT_ON :
			(val == 255000) ? USBSWITCH_SSG_USB_BOOT_OFF :
			(val == 1000000) ? USBSWITCH_SSG_AUDIODEV_TYPE1 :
			(val == 80000) ? USBSWITCH_SSG_AUDIODEV_TYPE2 :
			(val == 27800) ? USBSWITCH_SSG_AUDIODEV_TYPE3 :
			(val == 102000) ? USBSWITCH_SSG_PPD :
			(val == 121000) ? USBSWITCH_SSG_TTY_CONV :
			(val == 365000) ? USBSWITCH_SSG_AUDVID_NOVBUS :
			(val == 1000) ? USBSWITCH_SSG_DIG_AV :
			(val == 150000) ? USBSWITCH_SSG_UART :
			0;

end:
	if ((ret == USBSWITCH_SSG_AUDVID_NOVBUS) && vbus_status)
		ret = USBSWITCH_SSG_AUDVID_VBUS;
	if ((ret == USBSWITCH_SSG_USB_BOOT_ON) && (vbus_status == 0))
		printf("WARNING: USB-BOOT-On cable without VBUS !\n");
	if ((ret == USBSWITCH_SSG_USB_BOOT_OFF) && (vbus_status == 0))
		printf("WARNING: USB-BOOT-Off cable without VBUS !\n");

	/*  restore AB9540 USB automated detection support */
	val = usbswitch_epilogue(usbphyctrl, usbotgctrl, vbus_status);
	if (val < 0)
		goto err;
	return ret;

err:
	printf("Error detection in %s: retval=%d\n", __func__, val);
	return USBSWITCH_ERROR;
}

int usbswitch_not_usb_cable(void)
{
	int ret;

	ret = usbswitch_detect();
	if ((ret < 0) || (ret == USBSWITCH_SSG_UART_BOOT_ON) ||
		(ret == USBSWITCH_SSG_UART_BOOT_OFF) || (ret == USBSWITCH_SSG_UART))
		return ret;
	return 0;
}

static const char *usbswitch_cable[] = {
	[USBSWITCH_NONE] = "no cable plugged",
	[USBSWITCH_NOVBUS_UNKWON] = "unkwon cable detected, no VBUS",
	[USBSWITCH_VBUS_STD] = "USB cable plugged, VBUS supplied",
	[USBSWITCH_VBUS_UNKWON] ="unkwon cable detected, VBUS supplied",
	[USBSWITCH_SSG_UART_BOOT_ON] = "UART-Boot-On cable plugged",
	[USBSWITCH_SSG_UART_BOOT_OFF] = "UART-Boot-Off cable plugged",
	[USBSWITCH_SSG_USB_BOOT_ON] = "USB-Boot-On cable plugged",
	[USBSWITCH_SSG_USB_BOOT_OFF] = "USB-Boot-Off cable plugged",
	[USBSWITCH_SSG_AUDIODEV_TYPE1] = "Audio device type 1 cable plugged",
	[USBSWITCH_SSG_AUDIODEV_TYPE2] = "Audio device type 1 cable plugged",
	[USBSWITCH_SSG_AUDIODEV_TYPE3] = "Audio device type 1 cable plugged",
	[USBSWITCH_SSG_PPD] = "Phone Power Device cable plugged",
	[USBSWITCH_SSG_TTY_CONV] = "TTY conerter cable plugged",
	[USBSWITCH_SSG_AUDVID_NOVBUS] = "Audio/Video cable plugged, no VBUS",
	[USBSWITCH_SSG_AUDVID_VBUS] = "Audio/Video cable plugged, with VBUS",
	[USBSWITCH_SSG_DIG_AV] = "Digital A/V cable plugged",
	[USBSWITCH_SSG_UART] = "UART calbe plugged",
	[USBSWITCH_UNKOWN] = "unkwon cable detected",
};

static int do_usbswitch(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	int ret;
	if (argc == 1) {
		ret = usbswitch_detect();
		if (ret == USBSWITCH_ERROR) {
			printf("error during cable detection process\n");
			return 0;
		}
		printf("uUSB switch detection status: %s\n", usbswitch_cable[ret]);
	} else if (strncmp(argv[1], "itlatch", 7) == 0) {
		usbswitch_print_detection();
	} else if (strncmp(argv[1], "usbregs", 7) == 0) {
		usbswitch_print_usbregs();
	} else if (strncmp(argv[1], "wd", 3) == 0) {
		dev_wd_kick_disable();
	} else {
		printf("unknwon usbswitch command\n");
		return 0;
	}
	return 1;
}

U_BOOT_CMD(
	usbswitch,	3,	0,	do_usbswitch,
	"usb switch accessory detection",
	"noarg               run id_plug & iddet, disc. vbus.\n"
	"itlatch             print some ABx5xx itlatch registers\n"
	"usbregs             print some USB detection related regs.\n"
);

extern void db8500_apmux_uart_ulpi(void);
extern void db8500_apmux_usb_ulpi(void);
extern void ux500_mux_usb_ulpi(void);

static int uart_on_ulpi(int verbose)
{
	int val;
	val = ab8500_read(0x0B, 0xA3);
	if (val < 0) {
		if (verbose)
			printf("failed to mux UART to ULPI\n");
		return 1;
	}
	writel(0x300, 0x80007030); // UART off
	ab8500_write(0x05, 0x8A, 0); // USB PHY off
	udelay(50000);
	ab8500_write(0x0B, 0xA3, val | 0x04); // UART on ULPI whatever ulpi cmd.
	db8500_apmux_uart_ulpi();
	dev_wd_kick_disable();		/* insure detection is operational: kick wd */
	ab8500_write_mask(0x10, 0x50, 0x0C, 0x1C);      // UART from GPIO13/50 pads
	ab8500_write_mask(0x0B, 0xA3, 0x04, 0x04);      // ULPI in UART mode
	ab8500_write_mask(0x0B, 0xA3, 0x01, 0x01);      // UART from GPIO13/50 pads
	ab8500_write(0x05, 0x8A, 2); // USB PHY on (device mode)
	udelay(10000);
	ab8500_write_mask(0x10, 0x01, 0x00, 0x10); 	// GPIO13 as alt
	ab8500_write_mask(0x10, 0x06, 0x00, 0x02); 	// GPIO50 as alt
	writel(0x301, 0x80007030); // UART on
	if (verbose)
		printf("UART successfully muxed to ULPI\n");
	return 0;
}

static int usb_on_ulpi(int verbose)
{
	int val;
	val = ab8500_read(0x0B, 0xA3);
	if (val < 0) {
		if (verbose)
			printf("failed to mux USB to ULPI\n");
		return 1;
	}
	writel(0x300, 0x80007030); // UART off
	ab8500_write(0x05, 0x8A, 0); // USB PHY off
	udelay(50000);
	ab8500_write(0x0B, 0xA3, val & ~0x04);		// UART on ULPI whatever ulpi cmd.
	db8500_apmux_usb_ulpi();
	writel(0x301, 0x80007030); // UART on
	if (verbose)
		printf("USB successfully muxed to ULPI\n");
	return 0;
}

static int do_uart_on_ulpi(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	int val;

	if (argc == 1) {
		printf("Current UART/ULPI config:\n");
		printf(" - gpio256..267 afsla:   0x%08X\n", readl(0xA03FE420));
		printf("                afslb:   0x%08X\n", readl(0xA03FE424));
		printf("                pdis:    0x%08X\n", readl(0xA03FE40c));
		printf("                pdir:    0x%08X\n", readl(0xA03FE410));
		printf("                dat:     0x%08X\n", readl(0xA03FE400));
		printf("\n");
		printf(" AB9540 ULPI is currently muxed on %s\n",
				(ab8500_read(0x0B, 0xA3) & 4) ? "UART" : "USB-ULPI");
	} else if (strncmp(argv[1], "auto", 4) == 0) {
		val = usbswitch_detect();
		if (val < 0)
			printf("Failure while detecting USB cable\n");
		else if (val == USBSWITCH_NONE)
			printf("no cable detected: no modification of ULPI muxing\n");
		else if (val == USBSWITCH_SSG_UART_BOOT_OFF)
			return uart_on_ulpi(1);
		else if (val == USBSWITCH_SSG_UART_BOOT_OFF)
			return uart_on_ulpi(1);
		else if (val == USBSWITCH_SSG_UART)
			return uart_on_ulpi(1);
		else
			return usb_on_ulpi(1);
	} else if (strncmp(argv[1], "on", 2) == 0) {
		return uart_on_ulpi(1);
	} else if (strncmp(argv[1], "off", 3) == 0) {
		return usb_on_ulpi(1);
	} else if (strncmp(argv[1], "wd", 3) == 0) {
		dev_wd_kick_disable();
	} else {
		printf("syntax error, check help\n");
	}
	return 0;
}

U_BOOT_CMD(
	uartulpi,	3,	0,	do_uart_on_ulpi,
	"UART over ULPI",
	"[on|off]\n"
	"\n"
	"<noarg>          display current state\n"
	"auto             test UART-Boot cable and mux UART if detected.\n"
	"on               enable UART over ULPI (phy, pad mux)\n"
	"off              disable UART over ULPI (phy, pad mux)\n"
);


