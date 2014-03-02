/*
 * Copyright (C) ST-Ericsson SA 2009-2010
 * Jonas Aaberg <jonas.aberg@stericsson.com>
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
#include <stdio.h>
#include <stdlib.h>

#include "bootimg.h"
#include "boot.h"
#include "boottime.h"
#include "timer.h"

#ifdef CONFIG_BOOTTIME

static struct atag_boottime boottime = {
	.idle = 0,
	.total = 0,
};

void boottime_tag(char *name)
{
	if (boottime.num == BOOTTIME_MAX) {
		printf("boottime: out of entries!\n");
		return;
	}

	strncpy((char *)boottime.entry[boottime.num].name,
		name,
		BOOTTIME_MAX_NAME_LEN);
	boottime.entry[boottime.num].name[BOOTTIME_MAX_NAME_LEN - 1] = '\0';
	boottime.entry[boottime.num].time = get_timer_us();

	boottime.num++;
}

struct boottime_entry *boottime_get_entry(unsigned int i)
{
	if (i >= boottime.num)
		return NULL;
	else
		return &boottime.entry[i];
}

void boottime_idle_add(unsigned long time)
{
	boottime.idle += time;
}

unsigned long boottime_idle_done(void)
{
	return get_timer_us();
}

unsigned long boottime_idle_get(void)
{
	return boottime.idle;
}

#if WITH_LIB_CONSOLE

#include <lib/console.h>

static int cmd_boottime(int argc, const cmd_args *argv);
static int cmd_boottime_tag (int argc, const cmd_args *argv);

STATIC_COMMAND_START
        { "boottime_info", "print boottime tags", &cmd_boottime },
        { "boottime_tag", "Add a boottime tag at the current time. Syntax: boottime_tag <name>", &cmd_boottime_tag },
STATIC_COMMAND_END(boottime);

static int cmd_boottime(int argc, const cmd_args *argv)
{
	unsigned int i;
	struct boottime_entry *entry;

	for (i = 0; i < BOOTTIME_MAX; i++) {
		entry = boottime_get_entry(i);
		if (entry == NULL)
			break;
		printf("%s: started at %lu ms\n", entry->name,
		       (unsigned long)entry->time / 1000);
	}
	printf("idle: %d%% (%d ms)\n",
	       100 * (int)boottime_idle_get() / (int)get_timer_us(),
	       (int)boottime_idle_get() / 1000);
	return 0;
}

static int cmd_boottime_tag (int argc, const cmd_args *argv)
{

	if (argc < 2) {
		printf ("Usage: boottime_tag <tagname>\n");
		return 1;
	}
	boottime_tag((char*)argv[1].str);

	return 0;
}


#endif  /* #if WITH_LIB_CONSOLE */

#else
/*
 * Dummy functions when boottime is not enabled.
 */
void boottime_tag(char *name)
{

}

void boottime_idle_add(unsigned long time)
{

}

#endif



