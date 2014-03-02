/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <bass_app.h>
#include <debug.h>

#define BINNAME "smcl_cmd"

static int cmd_dbg_unlock_mem();
static int cmd_reset_unlock_mem();
static int cmd_restart_modem();
static int cmd_encrypt_pin();

static const struct {
    const char *str;
    int (*func)();
} cmd_funcs[] = {
    {
        "dbg_unlock_mem", cmd_dbg_unlock_mem
    }, {
        "reset_unlock_mem", cmd_reset_unlock_mem
    }, {
        "restart_modem", cmd_restart_modem
    }, {
        "encrypt_pin", cmd_encrypt_pin
    },
};

static const size_t num_cmd_funcs = sizeof(cmd_funcs) / sizeof(cmd_funcs[0]);

int main(int argc, char **argv)
{
    size_t n;
    uint32_t ab8500cutid = 0xFF;

    if (argc < 2) {
        goto usage;
    }

    for (n = 0; n < num_cmd_funcs; n++) {
        if (strcmp(cmd_funcs[n].str, argv[1]) == 0) {
            if (strcmp(cmd_funcs[n].str, "restart_modem") == 0) {
                if (argc < 3) {
                    dprintf(ERROR, "You must specified ab8500cutid\n");
                    goto usage;
                } else {
                    if (strcmp(argv[2], "V11") == 0) {
                        ab8500cutid = 0x11;
                    } else if (strcmp(argv[2], "V2") == 0) {
                        ab8500cutid = 0x20;
                    } else {
                        dprintf(ERROR, "Invalid ab8500cutid!\n");
                        goto usage;
                    }

                    dprintf(INFO, "calling %s(0x%x)\n", cmd_funcs[n].str,
                            ab8500cutid);
                    cmd_funcs[n].func(ab8500cutid);
                }
            } else {
                dprintf(INFO, "calling %s\n", cmd_funcs[n].str);
                cmd_funcs[n].func();
            }

            goto out;
        }
    }

usage:
    dprintf(ALWAYS, "Usage : %s\n ", argv[0]);

    for (n = 0; n < num_cmd_funcs; n++) {
        if (strcmp(cmd_funcs[n].str, "restart_modem") == 0) {
            dprintf(ALWAYS, "%s %s [valid ab8500cutid: V11, V2]\n", argv[0],
                    cmd_funcs[n].str);
        } else {
            dprintf(ALWAYS, "%s %s\n ", argv[0], cmd_funcs[n].str);
        }
    }

    dprintf(ALWAYS, "\n");

out:
    return EXIT_SUCCESS;
}

static int cmd_dbg_unlock_mem()
{
    if (smcl_unlock_dbg_modem_mem() == SMCL_OK) {
        dprintf(ALWAYS, "smcl_unlock_dbg_modem_mem() PASSED!\n");
    } else {
        dprintf(ERROR, "smcl_unlock_dbg_modem_mem() FAILED!\n");
    }

    return 0;
}

static int cmd_reset_unlock_mem()
{
    if (smcl_reset_unlock_modem_mem() == SMCL_OK) {
        dprintf(ALWAYS, "smcl_reset_unlock_modem_mem() PASSED!\n");
    } else {
        dprintf(ERROR, "smcl_reset_unlock_modem_mem() FAILED!\n");
    }

    return 0;
}

static int cmd_restart_modem(uint32_t ab8500cutid)
{
    if (smcl_restart_modem(ab8500cutid) == SMCL_OK) {
        dprintf(ALWAYS, "smcl_restart_modem() PASSED!\n");
    } else {
        dprintf(ERROR, "smcl_restart_modem() FAILED!\n");
    }

    return 0;
}

static void dump_pin(char *info, uint8_t *pin, int len)
{
    int i = 0;

    if (info) {
        dprintf(ALWAYS, "%s\n", info);
    }

    for (i = 0; i < len; ++i) {
        printf(" %02x", pin[i]);
    }
    printf("\n");
    /*
     * Just a short delay to not get mixed prints from secure world and
     * normal world.
     */
    usleep(100000);
}

static int cmd_encrypt_pin()
{
    uint32_t ret = SMCL_OK;
    uint8_t short_pin[4];
    uint8_t long_pin[20];
    uint8_t pin[16] = {1, 2, 3, 4 };
    uint8_t correct_pin[16] = {1, 2, 3, 4, 0, 0, 0, 0,
                               0, 0, 0, 0, 0, 0, 0, 0};

    /* Testing encryption. */
    if (smcl_encrypt_pin(pin, sizeof(pin), SMCL_DIR_ENCRYPT) != SMCL_OK) {
        dprintf(ERROR, "smcl_encrypt_pin encrypting pin failed!\n");
        ret = SMCL_GENERAL_FAILURE;
    }
    dump_pin("Encrypted pin", pin, sizeof(pin));

    /*
     * Testing decryption, should get back the value of the previous
     * encryption.
     */
    if (smcl_encrypt_pin(pin, sizeof(pin), SMCL_DIR_DECRYPT) != SMCL_OK) {
        dprintf(ERROR, "smcl_encrypt_pin decrypting pin failed!\n");
        ret = SMCL_GENERAL_FAILURE;
    }
    dump_pin("decrypted pin", pin, sizeof(pin));

    /* Testing for correct value after encryption and decryption. */
    if (memcmp(correct_pin, pin, sizeof(correct_pin))) {
        dprintf(ERROR, "smcl_encrypt_pin decrypting pin failed!\n");
        ret = SMCL_GENERAL_FAILURE;
    }

    /* Negative test for NULL pointer buffer. */
    if (smcl_encrypt_pin(NULL, sizeof(pin), SMCL_DIR_ENCRYPT) == SMCL_OK) {
        dprintf(ERROR, "smcl_encrypt_pin accepts NULL pointer buffer!\n");
        ret = SMCL_GENERAL_FAILURE;
    }

    /* Negative test for incorrect encryption direction. */
    if (smcl_encrypt_pin(pin, sizeof(pin), 0xFF) == SMCL_OK) {
        dprintf(ERROR, "smcl_encrypt_pin accepts non valid encryption "
                "direction!\n");
        ret = SMCL_GENERAL_FAILURE;
    }

    /* Negative test for too short buffer. */
    if (smcl_encrypt_pin(short_pin, sizeof(short_pin), SMCL_DIR_ENCRYPT)
        == SMCL_OK) {
        dprintf(ERROR, "smcl_encrypt_pin accepts a too short buffer!\n");
        ret = SMCL_GENERAL_FAILURE;
    }

    /* Negative test for too long buffer. */
    if (smcl_encrypt_pin(long_pin, sizeof(long_pin), SMCL_DIR_ENCRYPT)
        == SMCL_OK) {
        dprintf(ERROR, "smcl_encrypt_pin accepts a too long buffer!\n");
        ret = SMCL_GENERAL_FAILURE;
    }

    if (ret == SMCL_OK) {
        dprintf(ALWAYS, "smcl_encrypt_pin PASSED!\n");
    } else {
        dprintf(ERROR, "smcl_encrypt_pin FAILED!\n");
    }

    return 0;
}
