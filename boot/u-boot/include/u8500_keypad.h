/*
 * Copyright (C) ST-Ericsson SA 2011
 * Author: Pengzhou ZHAN<pengzhou.zhan@stericsson.com> for ST-Ericsson
 * 	   Yitian BU<yitian.bu@stericsson.com> for ST-Ericsson
 * License terms: GNU General Public License (GPL), version 2.
 */

#ifndef __U8500_KEYPAD_H__
#define __U8500_KEYPAD_H__

#define KEYPAD_MAX_ROWS		8
#define KEYPAD_MAX_COLS		8

#define SKE_KEYPAD_ROW_SHIFT	4
#define SKE_KPD_KEYMAP_SIZE	(8 * 8)
#define MATRIX_SCAN_CODE(row, col, row_shift) 	(((row) << (row_shift)) + (col))

#define MATRIX_GET_ROW(code) (((code)>>(SKE_KEYPAD_ROW_SHIFT))&0x0F)
#define MATRIX_GET_COL(code) ((code)&0x0F)
#define CONFIG_VIA_KEYPAD

#define VOLUME_UP_PRESS		0x11
#define VOLUME_DOWN_PRESS		0x22
#define VOLUME_KEY_RELEASE		0x33

/*
 * struct ske_key - data stucture used by keypad driver
 * key_num:number of keys been pressed
 * key_code:code value of the pressed keys
 */
struct ske_key {
	int key_num;
	int key_code[KEYPAD_MAX_ROWS][KEYPAD_MAX_COLS];
};

void keypad_init(void);
void reset_key_pressed(void);
int lunch_key_detect(void);
int ske_keypad_getkey_pressed(int key_num, int * key_code);
#ifdef CONFIG_VIA_KEYPAD
int detect_lunch_key(void);
#endif

#endif /* __U8500_KEYPAD_H__ */
