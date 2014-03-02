/*
 * Copyright (C) ST-Ericsson SA 2011
 * Author: Seshagiri HOLI<seshagiri.holi@stericsson.com> for
 * ST-Ericsson.
 * License terms: GNU General Public License (GPL), version 2.
 */


#ifndef __U5500_KEYPAD_H__
#define __U5500_KEYPAD_H__


void init_keypad(void);
int wait_for_keypress_forever(void);
int wait_for_keypress(ulong msec);

#define KEY_CAMERA		0x001
#define KEY_CAMERA_FOCUS	0x002
#define KEY_MENU		0x004
#define KEY_BACK		0x008
#define KEY_SEND		0x010
#define KEY_HOME		0x020
#define KEY_END		0x040
#define KEY_VOLUMEUP		0x080
#define KEY_VOLUMEDOWN		0x100


#define KEYPAD_MAX_ROWS		9
#define KEYPAD_MAX_COLS		8

#endif /* __U5500_KEYPAD_H__ */
