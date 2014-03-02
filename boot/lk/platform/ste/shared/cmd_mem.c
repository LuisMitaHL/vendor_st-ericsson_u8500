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


#include <ctype.h>
#include <debug.h>
#include <stdlib.h>
#include <printf.h>
#include <list.h>
#include <string.h>
#include <arch/ops.h>
#include <platform.h>
#include <platform/debug.h>
#include <platform/timer.h>
#include <arch/ops.h>

#ifdef WITH_LIB_CONSOLE
#include <lib/console.h>

#define CTRL_C 0x03

static int cmd_display_mem_cyclic(int argc, const cmd_args *argv);
static int cmd_modify_mem_cyclic(int argc, const cmd_args *argv);
static int cmd_memtest_lk(int argc, const cmd_args *argv);

STATIC_COMMAND_START
#if DEBUGLEVEL > 0
    { "mdcw", "cyclic display memory in words", &cmd_display_mem_cyclic },
    { "mdch", "cyclic display memory in halfwords", &cmd_display_mem_cyclic },
    { "mdcb", "cyclic display memory in bytes", &cmd_display_mem_cyclic },
    { "mwcw", "cyclic modify word of memory", &cmd_modify_mem_cyclic },
    { "mwch", "cyclic modify halfword of memory", &cmd_modify_mem_cyclic },
    { "mwcb", "cyclic modify byte of memory", &cmd_modify_mem_cyclic },
    { "lk_mtest", "LK memory test", &cmd_memtest_lk },
#endif
STATIC_COMMAND_END(cmd_mem);

bool ctrl_c(void)
{
    char c;
    if (dgetc(&c, 0) < 0) {
        return false;
    }
    if (c == CTRL_C) {
        return true;
    }
    return false;
}

static int cmd_display_mem_cyclic(int argc, const cmd_args *argv)
{
    int size;

    if (argc < 3) {
        printf("not enough arguments\n");
        printf("%s <address> <delay ms>\n", argv[0].str);
        return -1;
    }

    if (strcmp(argv[0].str, "mdcw") == 0) {
        size = 4;
    } else if (strcmp(argv[0].str, "mdch") == 0) {
        size = 2;
    } else {
        size = 1;
    }

    unsigned long address = argv[1].u;

    if ((address & (size - 1)) != 0) {
        printf("unaligned address, cannot display\n");
        return -1;
    }

    /* Disable L1 caches */
    arch_disable_cache(UCACHE);

    while (!ctrl_c()) {
        printf("0x%08lx: ", address);
        switch (size) {
            case 4:
                printf("%08x\n", *(uint32_t *)address);
                break;
            case 2:
                printf("%04hx\n", *(uint16_t *)address);
                break;
            case 1:
                printf("%02hhx\n", *(uint8_t *)address);
                break;
        }
        udelay(1000 * ((unsigned long) argv[2].i));
    }

    return 0;
}

static int cmd_modify_mem_cyclic(int argc, const cmd_args *argv)
{
    int size;

    if (argc < 4) {
        printf("not enough arguments\n");
        printf("%s <address> <val> <delay ms>\n", argv[0].str);
        return -1;
    }

    if (strcmp(argv[0].str, "mwcw") == 0) {
        size = 4;
    } else if (strcmp(argv[0].str, "mwch") == 0) {
        size = 2;
    } else {
        size = 1;
    }

    unsigned long address = argv[1].u;
    unsigned int val = argv[2].u;

    if ((address & (size - 1)) != 0) {
        printf("unaligned address, cannot modify\n");
        return -1;
    }

    /* Disable L1 caches */
    arch_disable_cache(UCACHE);

    while (!ctrl_c()) {
        switch (size) {
            case 4:
                *(uint32_t *)address = (uint32_t)val;
                break;
            case 2:
                *(uint16_t *)address = (uint16_t)val;
                break;
            case 1:
                *(uint8_t *)address = (uint8_t)val;
                break;
        }
        udelay(1000 * ((unsigned long) argv[3].i));
    }

    return 0;
}

static int cmd_memtest_lk(int argc, const cmd_args *argv)
{
    if (argc < 3) {
        printf("not enough arguments\n");
        printf("%s <base> <len>\n", argv[0].str);
        return -1;
    }

    uint32_t *ptr;
    size_t len;

    ptr = (uint32_t *)argv[1].u;
    len = (size_t)argv[2].u;

    size_t i;
    // write out
    printf("writing first pass...");
    for (i = 0; i < len / 4; i++) {
        ptr[i] = i;
    }
    printf("done\n");

    // verify
    printf("verifying...");
    for (i = 0; i < len / 4; i++) {
        if (ptr[i] != i)
            printf("error at %p\n", &ptr[i]);
    }
    printf("done\n");

    return 0;
}

#endif

