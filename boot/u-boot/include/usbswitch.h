/*
 * usbswitch.h
 *
 * Copyright (C) ST-Ericssson Le Mans SA 2012
 * Author: etienne carriere <etienne.carriere@stericsson.com> for ST-Ericssson Le Mans.
 * License terms:  GNU General Public License (GPL), version 2
 */

/* usbswitch_link - enumerated values returned by usbswitch() */
enum usbswitch_link {
	USBSWITCH_ERROR = -1,
	USBSWITCH_NONE = 0,
	USBSWITCH_NOVBUS_UNKWON,
	USBSWITCH_VBUS_STD,
	USBSWITCH_VBUS_UNKWON,
	USBSWITCH_SSG_UART_BOOT_ON,		/* no Vbus, Rid=619kOhm */
	USBSWITCH_SSG_UART_BOOT_OFF,	/* no Vbus, Rid=523kOhm */
	USBSWITCH_SSG_USB_BOOT_ON,		/* Vbus,    Rid=301kOhm */
	USBSWITCH_SSG_USB_BOOT_OFF,		/* Vbus,    Rid=255kOhm */
	USBSWITCH_SSG_AUDIODEV_TYPE1,	/* no Vbus, Rid=1MOhm */
	USBSWITCH_SSG_AUDIODEV_TYPE2,	/* no Vbus, Rid=80kOhm */
	USBSWITCH_SSG_AUDIODEV_TYPE3,	/* no Vbus, Rid=27.8kOhm */
	USBSWITCH_SSG_PPD,				/* no Vbus, Rid=102kOhm */
	USBSWITCH_SSG_TTY_CONV,			/* no Vbus, Rid=121kOhm */
	USBSWITCH_SSG_AUDVID_NOVBUS,	/* no Vbus, Rid=365kOhm */
	USBSWITCH_SSG_AUDVID_VBUS,		/* Vbus,    Rid=365kOhm */
	USBSWITCH_SSG_DIG_AV,			/* no Vbus, Rid=1kOhm */
	USBSWITCH_SSG_UART,				/* no Vbus, Rid=150kOhm */
	USBSWITCH_UNKOWN,
};

/* usbswitch_detect - return enum according to cable identification */
enum usbswitch_link usbswitch_detect(void);

/* usbswitch_not_usb_cable - return non zero if not a USB cable */
int usbswitch_not_usb_cable(void);
