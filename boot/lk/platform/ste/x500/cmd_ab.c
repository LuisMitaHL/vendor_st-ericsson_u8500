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
#include "abx500.h"

#ifdef WITH_LIB_CONSOLE
#include <lib/console.h>

static int cmd_ab(int argc, const cmd_args *argv);

STATIC_COMMAND_START
#if DEBUGLEVEL > 0
    { "ab", "read/write ab register", &cmd_ab },
#endif
STATIC_COMMAND_END(cmd_ab);

static int cmd_ab(int argc, const cmd_args *argv)
{
    bool is_write;

    if (argc < 4) {
        printf("not enough arguments\n");
        printf("%s <read> <bankadress> <offset>\n", argv[0].str);
        printf("%s <write> <bankadress> <offset> <val>\n", argv[0].str);
        return -1;
    }

    if (strcmp(argv[1].str, "read") == 0) {
        is_write = false;
    } else if (strcmp(argv[1].str, "write") == 0) {
        is_write = true;
        if (argc < 5) {
            printf("not enough arguments for write\n");
            return -1;
        }
    } else {
        printf("Options for %s are read or write not %s\n",
               argv[0].str, argv[1].str);
        return -1;
    }

    uint16_t bank_reg = (argv[2].u << 8) | argv[3].u;
    int ret;

    if (!is_write) {
        ret = ab8500_reg_read(bank_reg);
        if (ret < 0) {
            printf("ab read failed at address 0x%lx,0x%lx\n",
                   argv[2].u, argv[3].u);
            return -1;
        } else {
            printf("ab read(0x%lx,0x%lx) = 0x%x\n",
                   argv[2].u, argv[3].u, ret);
        }
    } else {
        ret = ab8500_reg_write(bank_reg, argv[4].u);
        if (ret < 0) {
            printf("ab write failed at address 0x%lx,0x%lx\n",
                   argv[2].u, argv[3].u);
            return -1;
        } else {
            printf("ab write(0x%lx,0x%lx,0x%lx)\n",
                   argv[2].u, argv[3].u, argv[4].u);
        }
    }

    return 0;
}
#endif

