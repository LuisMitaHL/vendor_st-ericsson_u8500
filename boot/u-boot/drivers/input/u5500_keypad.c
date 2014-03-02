/*
 * Copyright (C) ST-Ericsson SA 2011
 * Author: Seshagiri HOLI<seshagiri.holi@stericsson.com> for
 * ST-Ericsson.
 * License terms: GNU General Public License (GPL), version 2.
 */

/* This has implementation for u5500 keypad drive .*/
#include <common.h>
#include <asm/io.h>
#include <asm/hardware.h>
#include <linux/bitops.h>
#include <u5500_keypad.h>

extern int do_reset (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[]);

/* Key pad */
#define KEYPAD_CTRL_REG		(U5500_KEYPAD_BASE + 0x0000)
#define KEYPAD_IRQ_CLEAR	(U5500_KEYPAD_BASE + 0x0004)
#define KEYPAD_IRQ_EN		(U5500_KEYPAD_BASE + 0x0008)
#define KEYPAD_IRQ_STATUS	(U5500_KEYPAD_BASE + 0x000C)

#define KEY_COLUMN_REG		(U5500_KEYPAD_BASE + 0x0010)
#define KEY_ROW_REG		(U5500_KEYPAD_BASE + 0x0014)
#define KEYPAD_ARRAY_01		(U5500_KEYPAD_BASE + 0x0018)


/* ctrl values */
#define KEYPAD_SCAN_ENBLE	0x080
#define KEYPAD_DEBOUNCE_HITS	0x070
#define KEYPAD_SCAN_INTV	0x000
#define KEYPAD_ARRAY_CHANGEBIT	(1 << 15)


/* kernel code */


#define KEYPAD_NUM_ARRAY_REGS	5
#define KEYPAD_GND_ROW		8
#define KEYPAD_MAX_ROWS		9
#define KEYPAD_MAX_COLS		8
#define NO_KEYSCAN		0

#define GET_INDEX_ROW_COL(row, col)	(row  + (col * KEYPAD_MAX_ROWS))

#define PERIOD	(10 * 1000 * 1000)	/* 10 seconds default*/

static int fun_handler_cam2_back(void);
static void keypad_scan(void);

struct key_func_map_s{
	u16 key_combination;
	int (*cmd)(void);
};

typedef struct key_func_map_s key_func_map_t;
/* key combination lookup table avoid duplicate key combinations */

static key_func_map_t key_func_map[] = {
	{KEY_CAMERA_FOCUS | KEY_BACK,		fun_handler_cam2_back}
};


static u16 key_code[KEYPAD_MAX_ROWS * KEYPAD_MAX_COLS] = {
		/* ROW  -->*/
/*C*/		0, 0, 0, 0, KEY_CAMERA,	 0	 , 0, 0, KEY_END,
/*O*/		0, 0, 0, 0, KEY_CAMERA_FOCUS, 0	 , 0, 0, KEY_VOLUMEUP,
/*L*/		0, 0, 0, 0, KEY_MENU,	 KEY_SEND, 0, 0, KEY_VOLUMEDOWN,
/*L*/		0, 0, 0, 0, KEY_BACK,	 KEY_HOME, 0, 0, 0,
/*U*/		0, 0, 0, 0, 0, 0, 0, 0, 0,
/*M*/		0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0,
};

static u8 previous_set[KEYPAD_MAX_ROWS];
static u32 pressed_key;

static void keypad_report(int row, u8 curr, u8 previous)
{
	u8 changed  = curr ^ previous;

	while (changed) {
		u8 col = generic_ffs(changed) - 1;
		u8 press = curr & BIT_MASK(col);
		printf("ROW X COL = %d X %d pressed = %d - key_code= %d\n",
			row, col, press, key_code[GET_INDEX_ROW_COL(row, col)]);
		if (press)
			pressed_key |= key_code[GET_INDEX_ROW_COL(row, col)];
		else
			pressed_key &= ~key_code[GET_INDEX_ROW_COL(row, col)];

		changed &= ~BIT_MASK(col);
	}
}

void keypad_scan(void)
{
	u8 current_set[KEYPAD_MAX_ROWS];
	int tries = 100;
	u32 changebit;
	u32 data_reg;
	u8 allrows;
	u8 common;
	int i;

	writel(0x01, KEYPAD_IRQ_CLEAR);
	/* clear bit 1 in IRQ register of keypad */
again:
	if (!tries--) {
		printf("values failed to stabilize\n");
		return;
	}

	changebit = readl(KEYPAD_ARRAY_01) & KEYPAD_ARRAY_CHANGEBIT;
	for (i = 0; i < KEYPAD_NUM_ARRAY_REGS; i++) {
		data_reg = readl(KEYPAD_ARRAY_01 + 4 * i);
		/* If the change bit changed, we need to reread the data */
		if (changebit != (data_reg & KEYPAD_ARRAY_CHANGEBIT))
			goto again;

		current_set[2 * i] = ~(data_reg & 0xff);

		/* Last array reg has only one valid set of columns */
		if (i != KEYPAD_NUM_ARRAY_REGS - 1)
			current_set[2 * i + 1] = ~((data_reg & 0xff0000) >> 16);
	}

	allrows = current_set[KEYPAD_GND_ROW];

	/*
	 * Sometimes during a GND row release, an incorrect report is received
	 * where the ARRAY8 all rows setting does not match the other ARRAY*
	 * rows.  Ignore this report; the correct one has been observed to
	 * follow it.
	 */
	common = 0xff;
	for (i = 0; i < KEYPAD_GND_ROW; i++)
		common &= current_set[i];

	if ((allrows & common) != common)
		return;

	for (i = 0; i < ARRAY_SIZE(current_set); i++) {
		/*
		 * If there is an allrows press (GND row), we need to ignore
		 * the allrows values from the reset of the ARRAYs.
		 */
		if (i < KEYPAD_GND_ROW && allrows)
			current_set[i] &= ~allrows;

		if (previous_set[i] == current_set[i])
			continue;

		keypad_report(i, current_set[i], previous_set[i]);
	}

	/* update the reference set of array registers */
	memcpy(previous_set, current_set, sizeof(previous_set));

	return;
}

void init_keypad(void)
{
	/* Set control reg */
	writel(KEYPAD_SCAN_ENBLE
		| KEYPAD_DEBOUNCE_HITS | KEYPAD_SCAN_INTV, KEYPAD_CTRL_REG);
}

int fun_handler_cam2_back(void)
{
	printf("Soft reset !!!!! \n");
	set_to_peripheral_boot_mode();
	do_reset(NULL, 0, 0, NULL);
	return 1;
}

int look_up_key_comb(u16 key_comb, int *handled)
{
	int i;
	int ret = 0;
	for (i = 0; i < ARRAY_SIZE(key_func_map); i++) {
		if (key_comb == key_func_map[i].key_combination) {
			ret = key_func_map[i].cmd();
			*handled = 1;
			break;
		}
	}
	return ret;
}

int wait_for_keypress_forever(void)
{
	int handled = 0;
	int ret = 0;
	ulong ini, end;
	/* Reset KEY press */
	pressed_key = 0;
	/* set bit 1 in KEYPAD_IRQ_EN register to enable IRQ */
	writel(0x01, KEYPAD_IRQ_EN);
	do {
		ini = get_curr_timer();
		end = ini + (ulong)usec2ticks(PERIOD);
		while ((signed)(end - get_curr_timer()) > 0) {
			if (readl(KEYPAD_IRQ_STATUS))
				keypad_scan();
		}
		ret = look_up_key_comb(pressed_key, &handled);
		if (handled == 1)
			break;
	} while (1);
	/* set bit 1 in KEYPAD_IRQ_CLEAR register to clear IRQ */
	writel(0x01, KEYPAD_IRQ_CLEAR);
	/* set bit 0 in KEYPAD_IRQ_EN register to disable IRQ */
	writel(0x00, KEYPAD_IRQ_EN);
	return ret;
}
int wait_for_keypress(ulong msec)
{
	ulong kv;
	ulong ini, end;
	int ret = 0;
	int handled = 0;
	pressed_key = 0;/* Reset KEY press */
	ulong usec = msec * 1000;/* milli sec to micro sec */

	/* scan for key at one first */
	keypad_scan();
	/* set bit 1 in KEYPAD_IRQ_EN register to enable IRQ */
	writel(0x01, KEYPAD_IRQ_EN);
	/* For delays mor than 10 sec we need loop for every chunks of 10 sec */
	do {
		kv = usec > PERIOD ? PERIOD : usec;
		ini = get_curr_timer();
		end = ini + (ulong)usec2ticks(kv);
		while ((signed)(end - get_curr_timer()) > 0) {
			if (readl(KEYPAD_IRQ_STATUS))
				keypad_scan();
		}
		usec -= kv;
	} while (usec);
	/* set bit 1 in KEYPAD_IRQ_CLEAR register to clear IRQ */
	writel(0x01, KEYPAD_IRQ_CLEAR);
	/* set bit 0 in KEYPAD_IRQ_EN register to disable IRQ */
	writel(0x00, KEYPAD_IRQ_EN);
	look_up_key_comb(pressed_key, &handled);
	return ret;
}

