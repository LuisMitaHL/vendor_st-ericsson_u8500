/*
* Copyright (C) ST-Ericsson SA 2011

* This has implementation for U8500 keypad drive derived from kernel.
*
* Author: Pengzhou ZHAN<pengzhou.zhan@stericsson.com> for ST-Ericsson
* 	  Yitian BU<yitian.bu@stericsson.com> for ST-Ericsson
* Thanks to:
*	Seshagiri HOLI<seshagiri.holi@stericsson.com> for ST-Ericsson
* 	Naveen Kumar G <naveen.gaddipati@stericsson.com> for ST-Ericsson
*	Sundar Iyer <sundar.iyer@stericsson.com> for ST-Ericsson
*
* License terms: GNU General Public License (GPL), version 2.*
*/

#include <common.h>
#include <malloc.h>
#include <asm/io.h>
#include <asm/types.h>
#include <linux/bitops.h>
#include <u8500_keypad.h>
#include "db8500_pins.h"

/* Key pad */
#define U8500_PER3_BASE		0x80000000
#define U8500_SKE_BASE 		(U8500_PER3_BASE + 0x6000) 	/*SKE register base*/
#define SKE_CR 			(U8500_SKE_BASE + 0x0000) 	/*SKE control register*/
#define SKE_VAL0 		(U8500_SKE_BASE + 0x0004) 	/*SKE value0 register*/
#define SKE_VAL1 		(U8500_SKE_BASE + 0x0008) 	/*SKE value1 register*/
#define SKE_DBCR 	 	(U8500_SKE_BASE + 0x000C) 	/*SKE debounce register*/
#define SKE_IMSC 		(U8500_SKE_BASE + 0x0010) 	/*SKE interrupt mask register*/
#define SKE_RIS 	 	(U8500_SKE_BASE + 0x0014) 	/*SKE raw interrupt status register*/
#define SKE_MIS 	 	(U8500_SKE_BASE + 0x0018) 	/*SKE masked interrupt status register*/
#define SKE_ICR 		(U8500_SKE_BASE + 0x001C) 	/*SKE interrupt clear register*/
#define SKE_ASR0 		(U8500_SKE_BASE + 0x0020) 	/*SKE autoscan result register 0*/
#define SKE_ASR1 		(U8500_SKE_BASE + 0x0024) 	/*SKE autoscan result register 1*/
#define SKE_ASR2 		(U8500_SKE_BASE + 0x0028) 	/*SKE autoscan result register 2*/
#define SKE_ASR3 		(U8500_SKE_BASE + 0x002C) 	/*SKE autoscan result register 3*/

/* SKE_CR bits */
#define SKE_KPMLT	(0x1 << 6)
#define SKE_KPCN	(0x7 << 3)
#define SKE_KPASEN	(0x1 << 2)
#define SKE_KPASON	(0x1 << 7)

/* SKE_IMSC bits */
#define SKE_KPIMA	(0x1 << 2)

/* SKE_ICR bits */
#define SKE_KPICS	(0x1 << 3)
#define SKE_KPICA	(0x1 << 2)

/* SKE_RIS bits */
#define SKE_KPRISA	(0x1 << 2)

/*SKE_MIS bits */
#define SKE_KPMISA 	(0x1<<2)


#define SKE_NUM_ASRX_REGISTERS	(4)
#define	KEY_PRESSED_DELAY	10

#define KEY_UNPRESSED 	0
#define KEY_PRESSED	1

#define DEBOUNCE_VALUE 	5
#define PERIOD 1000

#define GET_INDEX_ROW_COL(row, col)	(row  + (col * KEYPAD_MAX_ROWS))

static void keypad_scan(struct ske_key *key);

static struct ske_key keypad;

static void ske_keypad_set_bits(u32 addr,u16 mask, u16 data)
{
	u32 value;

	value = readl(addr);
	value &= ~mask;
	value |= data;
	writel(value,addr);
	return;
}

static void keypad_report( u8 status, int col, struct ske_key *key )
{
	int row = 0, code, pos;
	int num_of_rows;

	/* find out the row */
	num_of_rows = hweight8(status);
	do {

		pos = generic_ffs(status) - 1;
		row = pos;
		status &= ~(1 << pos);

		code = MATRIX_SCAN_CODE(row, col, SKE_KEYPAD_ROW_SHIFT);
		code = code;

		if (key->key_code[row][col] == KEY_UNPRESSED)
		{
			key->key_code[row][col] = KEY_PRESSED;
			key->key_num++;
		}

		num_of_rows--;

	} while (num_of_rows);

}

static void keypad_read_data(struct ske_key *key)
{
	u8 status;
	int col = 0;
	int ske_asr, i;

	/*
	 * Read the auto scan registers
	 *
	 * Each SKE_ASRx (x=0 to x=3) contains two row values.
	 * lower byte contains row value for column 2*x,
	 * upper byte contains row value for column 2*x + 1
	 */
	for (i = 0; i < SKE_NUM_ASRX_REGISTERS; i++) {
		ske_asr = readl(SKE_ASR0 + (4 * i));
		if (!ske_asr)
			continue;

		/* now that ASRx is zero, find out the coloumn x and row y */
		status = ske_asr & 0xff;
		if (status) {
			col = i * 2;
			keypad_report(status, col, key);
		}
		status = (ske_asr & 0xff00) >> 8;
		if (status) {
			col = (i * 2) + 1;
			keypad_report(status, col, key);
		}
	}
}

void keypad_scan(struct ske_key *key)
{
	int timeout = 10;

	ske_keypad_set_bits(SKE_IMSC, ~SKE_KPIMA, 0x0);
	ske_keypad_set_bits(SKE_ICR, 0x0, SKE_KPICA);
	/* Wait for autoscan to complete */
	while ((readl(SKE_CR) & SKE_KPASON) && --timeout)
		udelay(1000);

	/* SKEx registers are stable and can be read */
	keypad_read_data(key);

	timeout = 10;
	/* wait until raw interrupt is clear */
        while ((readl(SKE_RIS)) && --timeout)
                udelay(1000);

        /* enable auto scan interrupts */
        ske_keypad_set_bits(SKE_IMSC, 0x0, SKE_KPIMA);

}

/*init keypad: Enable Multi key pressed detection, auto scan mode*/

void keypad_init(void)
{
	u32 value = 0;
	int timeout = DEBOUNCE_VALUE;

	memset (&keypad, 0x0, sizeof(struct ske_key));

	/*check SKE_RIS to be 0*/
	while ((readl(SKE_RIS) != 0x00000000) && timeout--);
	if(!timeout){
		printf("error:%s,%d\n",__FUNCTION__,__LINE__);
		return;
	}

	/*
	 * set debounce value
	 * keypad dbounce is configured in DBCR[15:8]
	 * dbounce value in steps of 32/32.768 ms
	 */
	value = readl(SKE_DBCR);
	value = value & 0xff;
	value |= (( DEBOUNCE_VALUE * 32000)/32768) << 8;
	writel(value,SKE_DBCR);

	/*enable multikey detection*/
	ske_keypad_set_bits(SKE_CR,0,SKE_KPMLT);

	/**
	 * set up the number of columns
	 * KPCN[5:3] defines no. of keypad columns to be auto scanned
	 */
	value = (KEYPAD_MAX_COLS - 1) << 3;
	ske_keypad_set_bits(SKE_CR, SKE_KPCN, value);

	/* clear keypad interrupt for auto(and pending SW) scans */
	ske_keypad_set_bits(SKE_ICR, 0x0, SKE_KPICA | SKE_KPICS);

	/* un-mask keypad interrupts */
	ske_keypad_set_bits(SKE_IMSC, 0x0, SKE_KPIMA);

	/* enable automatic scan */
	ske_keypad_set_bits(SKE_CR, 0x0, SKE_KPASEN);

	return;
}

/*
 *this function will lunch key detect action,
 *and record keys which are pressed.
 */
int lunch_key_detect(void)
{
	if((readl(SKE_MIS) & SKE_KPMISA)){

		keypad_scan(&keypad);

	}

	return 0;
}

#ifdef CONFIG_VIA_KEYPAD
#define KEY_VOLUMEDOWN		114
#define KEY_VOLUMEUP		115

struct gpio_key {
	int key_code;
	int row_gpio;
	int col_gpio;
	int pressed;
};

int detect_lunch_key(void)
{
	u32 i = 0;

	struct gpio_key lunchkey[] = {
#if 1
		    {
			.key_code = KEY_VOLUMEUP,
			.row_gpio = 163,	/*KP_I1*/
			.col_gpio = 165,	/*KP_O3*/
			.pressed = 0,
			},
#endif
			{
			.key_code = KEY_VOLUMEDOWN,
			.row_gpio = 162, 	/*KP_I2*/
			.col_gpio = 165,	/*KP_O3*/
			.pressed = 0,	
			}
	};

	for(i = 0; i < ARRAY_SIZE(lunchkey); i++){
		db8500_gpio_set_output(lunchkey[i].col_gpio, 0); 		
		udelay(3000);
		if(db8500_gpio_get_input(lunchkey[i].row_gpio) == 0){
			lunchkey[i].pressed = 1;
		}
		db8500_gpio_set_output(lunchkey[i].col_gpio, 1);

#ifndef CONFIG_BOARD_SNDA
		printf("U8500-keypad.c enter detect_lunch_key lunchkey[i].pressed = %d\n",lunchkey[i].pressed);
		if(lunchkey[i].pressed == 0)
			return 1;
#endif
	}
#ifdef CONFIG_BOARD_SNDA
	if(lunchkey[0].pressed == 1)
		return VOLUME_UP_PRESS;
	else if(lunchkey[1].pressed == 1)
		return VOLUME_DOWN_PRESS;
	else
		return VOLUME_KEY_RELEASE;
#else
	return 0;
#endif
}
#endif

int do_lunch_key_detect(void)
{
	do {
		udelay(DEBOUNCE_VALUE * 1000);
		if((readl(SKE_MIS) & SKE_KPMISA)){

			keypad_scan(&keypad);
			printf("Key_number:%d\n",keypad.key_num);
			printf("CR=%x,IMSC=%x,MIS:%x, RIS:%x\n", readl(SKE_CR),
				readl(SKE_IMSC),readl(SKE_MIS), readl(SKE_RIS));

	}
	}while(1);

	return 0;
}
/*
 *check whether specified keys have been pressed already
 *key_num:number of keys to be checked
 *key_code:array of key code that to be checked
 *return: 0 success
 * 	  1 fail
 */
int ske_keypad_getkey_pressed(int key_num, int * key_code)
{
	int i, row, col, ret = -1;

	if (key_num != keypad.key_num){
		return -1;
	}

	for (i = 0; i < key_num; i++){
		row = MATRIX_GET_ROW(key_code[i]);
		col = MATRIX_GET_COL(key_code[i]);

		if (keypad.key_code[row][col] == KEY_PRESSED){
			ret = 0;
		}else{
			ret = -2;
		}
	}

	return ret;
}

void reset_key_pressed(void)
{

	memset (&keypad, 0x0, sizeof(struct ske_key));

	return;
}

int do_keypress_wait(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	int ret = 0;

	switch (argc) {
	case 1: /* No Arguments  passed */
		ret = do_lunch_key_detect();
		break;
	default:
		cmd_usage(cmdtp);
		break;
	}
	return ret;
}

U_BOOT_CMD(
	keypress, 2, 0, do_keypress_wait,
	"waits for keypress",
	"-Waits for keypress\n"
	"-If no arguments passed it will wait forever.\n"
);
