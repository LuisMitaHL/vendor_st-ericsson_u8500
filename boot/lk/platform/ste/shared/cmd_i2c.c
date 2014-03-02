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
#include <stdio.h>
#include <printf.h>
#include <list.h>
#include <string.h>
#include <arch/ops.h>
#include <platform.h>
#include <platform/debug.h>
#include <platform/timer.h>
#include "i2c.h"
#include "target_config.h"
#include "target.h"

#ifdef WITH_LIB_CONSOLE
#include <lib/console.h>

int last_used_dev = 0;
#if PLATFORM_DB8540
bool initiated_dev[] = {
    false,
    true,
    false,
    false,
    false,
    false
};
#else /* PLATFORM_DB8540 */
bool initiated_dev[] = {
    true,
    false,
    false,
    false
};
#endif /* PLATFORM_DB8540 */

static int cmd_i2c(int argc, const cmd_args *argv);

STATIC_COMMAND_START
#if DEBUGLEVEL > 0
    { "i2c", "I2C handling\n"
                  "Options: speed, dev, md and mw\n"
                  "  speed : Show/set speed, syntax bus [new speed]\n"
                  "  dev   : Show/set bus, syntax [bus]\n"
                  "  md    : Read register value, syntax bus address reg [count]\n"
                  "  mw    : Write register value, syntax bus address reg value [count]\n"
                  "  fd    : Read frame bytes, syntax bus address count\n"
                  "  fw    : Write frame bytes, syntax bus address byte1 [byte2] ... [byteN]\n"
                  "  probe : Probe the given I2C chip address, syntax bus [address]",
                  &cmd_i2c },
#endif
STATIC_COMMAND_END(cmd_i2c);

int cmd_i2c_speed(int argc, const cmd_args *argv)
{
    if (argc < 3) {
        printf("To few arguments for i2c speed, should be bus\n");
        return -1;
    }

    if (I2C_LAST_BUS < argv[2].i) {
        printf("I2C bus %d not available, last available is %d\n",
               argv[2].i, I2C_LAST_BUS);
        return -1;
    }

    if (!initiated_dev[argv[2].i]) {
        printf("I2C device %d not initiated\n", argv[2].i);
        return -1;
    }

    if (argc == 3) {
        printf("Speed for I2C device %d is %d\n",
               argv[2].i, i2c_get_bus_speed(argv[2].i));
    } else {
        i2c_set_bus_speed(argv[2].i, argv[3].i);
        printf("Changed speed for I2C device %d to %d\n",
               argv[2].i, i2c_get_bus_speed(argv[2].i));
    }

    return 0;
}

int cmd_i2c_dev(int argc, const cmd_args *argv)
{
    if (argc == 2) {
        printf("Last used I2C device is %d\n", last_used_dev);
        return 0;
    }

    if (argc == 3) {
        if (I2C_LAST_BUS < argv[2].i) {
            printf("I2C bus %d not available, last available is %d\n",
                   argv[2].i, I2C_LAST_BUS);
            return -1;
        }
        if (!initiated_dev[argv[2].i]) {
            i2c_init(argv[2].i);
            initiated_dev[argv[2].i] = true;
            printf("Initiated I2C device %d\n", argv[2].i);
        }
        last_used_dev = argv[2].i;
    }
    return 0;
}

int cmd_i2c_md(int argc, const cmd_args *argv)
{
    uint8_t i2c_reg;
    int     count = 1;
    uint8_t *buffer = NULL;

    if (argc < 5) {
        printf("To few arguments for i2c md, should be bus address reg\n");
        return -1;
    }

    if (I2C_LAST_BUS < argv[2].i) {
        printf("I2C bus %d not available, last available is %d\n",
               argv[2].i, I2C_LAST_BUS);
        return -1;
    }

    if (!initiated_dev[argv[2].i]) {
        printf("I2C device %d not initiated\n", argv[2].i);
        return -1;
    }

    i2c_reg = argv[4].u;
    if (argc == 6) {
        count = argv[5].i;
        if (count == 0) {
            printf("I2C device can not read 0 byte\n", argv[2].i);
            return -1;
        }
    }

    buffer = (uint8_t*) malloc(count);
    if (NULL == buffer) {
        printf("Could not allocate buffer for write\n");
        return -1;
    }

    buffer[0] = i2c_reg;

    if (i2c_write(argv[2].i, argv[3].u, false, buffer, 1)) {
        printf("Failed read to device %d address %02x reg %02x\n",
               argv[2].i, argv[3].u, i2c_reg);
        free(buffer);
        return -1;
    }

    if (!i2c_read(argv[2].i, argv[3].u, true, buffer, count)) {
        for (int i = 0; i < count; i++, i2c_reg++) {
            printf("Device %d address %02x reg %02x has value %02x\n",
                   argv[2].i, argv[3].u, i2c_reg, buffer[i]);
        }
    } else {
        printf("Failed read to device %d address %02x reg %02x\n",
               argv[2].i, argv[3].u, i2c_reg);
        free(buffer);
        return -1;
    }

    free(buffer);

    return 0;
}

int cmd_i2c_mw(int argc, const cmd_args *argv)
{
    uint8_t i2c_reg;
    int     count = 1;
    uint8_t *buffer = NULL;

    if (argc < 6) {
        printf("To few arguments for i2c mw, should be bus address reg value\n");
        return -1;
    }

    if (I2C_LAST_BUS < argv[2].i) {
        printf("I2C bus %d not available, last available is %d\n",
               argv[2].i, I2C_LAST_BUS);
        return -1;
    }

    if (!initiated_dev[argv[2].i]) {
        printf("I2C device %d not initiated\n", argv[2].i);
        return -1;
    }

    i2c_reg = argv[4].u;
    if (argc == 7) {
        count = argv[6].i;
        if (count == 0) {
            printf("I2C device can not write 0 byte\n", argv[2].i);
            return -1;
        }
    }

    buffer = (uint8_t*) malloc(count + 1);
    if (NULL == buffer) {
        printf("Could not allocate buffer for write\n");
        return -1;
    }

    buffer[0] = i2c_reg;
    for (int i = 0; i < count; i++) {
        buffer[i+1] = argv[5].u;
    }

    if (!i2c_write(argv[2].i, argv[3].u, true, buffer, count+1)) {
        for (int i = 0; i < count; i++, i2c_reg++) {
            printf("Written %02x to device %d address %02x reg %02x\n",
                   buffer[i+1], argv[2].i, argv[3].u, i2c_reg);
        }
    } else {
        printf("Failed write to device %d address %02x reg %02x\n",
               argv[2].i, argv[3].u, i2c_reg);
        free(buffer);
        return -1;
    }

    free(buffer);

    return 0;
}

int cmd_i2c_fd(int argc, const cmd_args *argv)
{
    int     count = 1;
    uint8_t *buffer = NULL;

    if (argc < 5) {
        printf("To few arguments for i2c fd, should be bus address length\n");
        return -1;
    }

    if (I2C_LAST_BUS < argv[2].i) {
        printf("I2C bus %d not available, last available is %d\n",
               argv[2].i, I2C_LAST_BUS);
        return -1;
    }

    if (!initiated_dev[argv[2].i]) {
        printf("I2C device %d not initiated\n", argv[2].i);
        return -1;
    }

    count = argv[4].i;
    if (count == 0) {
        printf("I2C device can not read 0 byte\n", argv[2].i);
        return -1;
    }

    buffer = (uint8_t*) malloc(count);
    if (NULL == buffer) {
        printf("Could not allocate buffer for read\n");
        return -1;
    }

    if (!i2c_read(argv[2].i, argv[3].u, true, buffer, count)) {
        printf("Device %d address %02x has read frame:\n",
               argv[2].i, argv[3].u);
        for (int i = 0; i < count; i++) {
            if ((i%10 == 9) || (i == count-1))
                printf("0x%02x\n", buffer[i]);
            else
                printf("0x%02x ", buffer[i]);
        }
    } else {
        printf("Failed to read device %d address %02x\n",
               argv[2].i, argv[3].u);
        free(buffer);
        return -1;
    }

    free(buffer);

    return 0;
}

int cmd_i2c_fw(int argc, const cmd_args *argv)
{
    int     count = 1;
    uint8_t *buffer = NULL;

    if (argc < 5) {
        printf("To few arguments for i2c fw, should be bus address byte1 [byte2] ... [byteN]\n");
        return -1;
    }

    if (I2C_LAST_BUS < argv[2].i) {
        printf("I2C bus %d not available, last available is %d\n",
               argv[2].i, I2C_LAST_BUS);
        return -1;
    }

    if (!initiated_dev[argv[2].i]) {
        printf("I2C device %d not initiated\n", argv[2].i);
        return -1;
    }

    count = argc - 4;

    buffer = (uint8_t*) malloc(count);
    if (NULL == buffer) {
        printf("Could not allocate buffer for write\n");
        return -1;
    }

    for (int i = 0, j=4; i < count; i++, j++) {
        buffer[i] = argv[j].u;
    }

    if (!i2c_write(argv[2].i, argv[3].u, true, buffer, count)) {
        printf("Device %d address %02x has sent frame:\n",
               argv[2].i, argv[3].u);
        for (int i = 0; i < count; i++) {
            if ((i%10 == 9) || (i == count-1))
                printf("0x%02x\n", buffer[i]);
            else
                printf("0x%02x ", buffer[i]);
        }
    } else {
        printf("Failed write to device %d address %02x\n",
                   argv[2].i, argv[3].u);
        free(buffer);
        return -1;
    }

    free(buffer);

    return 0;
}

/*
 * Probe the given I2C chip address or probe all the I2C addresses
 * of selected bus (I2C chip address not given in the command).
 * Returns 0 if a chip responded, not 0 on failure.
 */
int cmd_i2c_probe(int argc, const cmd_args *argv)
{
    int i;

    if (argc < 3) {
        printf("Too few arguments for i2c probe, should be bus [address]\n");
        return -1;
    }

    if (I2C_LAST_BUS < argv[2].i) {
        printf("I2C bus %d not available, last available is %d\n",
               argv[2].i, I2C_LAST_BUS);
        return -1;
    }

    if (!initiated_dev[argv[2].i]) {
        i2c_init(argv[2].i);
        initiated_dev[argv[2].i] = true;
        printf("Initiated I2C device %d\n", argv[2].i);
    }

    if (argc == 3) {
        printf("Valid chip addresses on I2C device %d:", argv[2].i);
        for (i = 0; i < 128; i++) {
            if (i2c_probe(argv[2].i, i) == 0)
                printf(" 0x%02X", i);
        }
        putc ('\n');
        return 0;
    }
    else if (i2c_probe(argv[2].i, argv[3].u) == 0) {
        printf("Response from I2C device %d address 0x%02x\n", argv[2].i,
               argv[3].u);
        return 0;
    } else {
        printf("Failure for I2C device %d address 0x%02x\n", argv[2].i,
               argv[3].u);
        return -1;
    }

}

static int cmd_i2c(int argc, const cmd_args *argv)
{

    if (argc < 2) {
        printf("not enough arguments\n");
        return -1;
    }

    if (strcmp(argv[1].str, "speed") == 0) {
        return cmd_i2c_speed(argc, argv);
    }
    if (strcmp(argv[1].str, "dev") == 0) {
        return cmd_i2c_dev(argc, argv);
    }
    if (strcmp(argv[1].str, "md") == 0) {
        return cmd_i2c_md(argc, argv);
    }
    if (strcmp(argv[1].str, "mw") == 0) {
        return cmd_i2c_mw(argc, argv);
    }
    if (strcmp(argv[1].str, "fd") == 0) {
        return cmd_i2c_fd(argc, argv);
    }
    if (strcmp(argv[1].str, "fw") == 0) {
        return cmd_i2c_fw(argc, argv);
    }
    if (strcmp(argv[1].str, "probe") == 0) {
        return cmd_i2c_probe(argc, argv);
    }

    printf("Unknown option %s to the i2c command\n", argv[1].str);
    return -1;
}
#endif
