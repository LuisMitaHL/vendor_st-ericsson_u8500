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

#if WITH_LIB_CONSOLE

/*******************************************************************************
 * Includes
 ******************************************************************************/

#include <debug.h>

#include <string.h>
#include <stdio.h>
#include <malloc.h>
#include <lib/console.h>

#include "app.h"
#include "tiny_env.h"
#include "bootimg.h"
#include "boot.h"
#include "machineid.h"
#include "target.h"

/*******************************************************************************
 * Types, constants
 ******************************************************************************/

typedef enum sys_boot_e {
    SYS_ANDROID,
    SYS_HATS
} sys_boot_t;


typedef struct conditions_st{
    sys_boot_t  sys;
    int         boot;
    int         save;
    int         uart_res;
} conditions_t;


#define STR_SAVE            "save"
#define STR_NOBOOT          "noboot"
#define STR_UART            "uart"

#define ENV_CONSOLE         "console"
#define ENV_OLD_CONSOLE     "oldconsole"
#define ENV_INIT            "rdinit"
#define ENV_TOOL            "hwtoolonuart"
#define ENV_ROOT            "root"

#define VAL_INIT_ANDROID    "init"
#define VAL_INIT_HATS       "init.hats.rc"
#define VAL_INIT_HATS_IMG   "/linuxrc"
#define VAL_DEV_NULL        "/dev/null"
#define VAL_ROOT_ANDROID    "/dev/ram0"
#define VAL_ROOT_HATS       "/dev/mmcblk0p8"

#define DEFAULT_CONSOLE     "ttyAMA2,115200n8"


/*******************************************************************************
 * Declaration of internal functions
 ******************************************************************************/
static int cmd_set_android (int argc, const cmd_args *argv);
static int cmd_set_hats (int argc, const cmd_args *argv);

static void parse_options (int argc, const cmd_args *argv, conditions_t *cond);
static int set_env_and_boot (conditions_t *cond);


/*******************************************************************************
 * Definition of external functions
 ******************************************************************************/


/*******************************************************************************
 * Definition of internal functions
 ******************************************************************************/


STATIC_COMMAND_START
        { "droid", "Setup to start Android\n"
                   "Syntax: droid [save] [noboot]\n"
                   "    save   : save envirnonment for next boot (optional)\n"
                   "    noboot : No auto boot after command (optional)",
                   &cmd_set_android },
        { "hats", "Set to start HATS\n"
                   "Syntax: hats [save] [noboot] [uart]\n"
                   "    save   : save envirnonment for next boot (optional)\n"
                   "    noboot : No auto boot after command (optional)\n"
                   "    uart   : Reserve UART for HATS (optional)",
                  &cmd_set_hats },
STATIC_COMMAND_END(hats);


static int cmd_set_android (int argc, const cmd_args *argv)
{
    conditions_t cond;

    if (is_env_signed()) {
        dprintf(CRITICAL, "env is signed so it cannot be updated\n");
        return 0;
    }

    dprintf (INFO, "\nSetup to boot Android\n");

    cond.sys = SYS_ANDROID;
    parse_options (argc, argv, &cond);

    return set_env_and_boot (&cond);
}


static int cmd_set_hats (int argc, const cmd_args *argv)
{
    conditions_t cond;

    if (is_env_signed()) {
        dprintf(CRITICAL, "env is signed so it cannot be updated\n");
        return 0;
    }

    dprintf (INFO, "\nSetup to boot HATS\n");

    cond.sys = SYS_HATS;
    parse_options (argc, argv, &cond);

    return set_env_and_boot (&cond);
}


static void parse_options (int argc, const cmd_args *argv, conditions_t *cond)
{
    int ii;

    cond->boot = 1;
    cond->save = 0;
    cond->uart_res = 0;

    for (ii = 1; ii < argc; ii++) {
        if (0 == strcmp(STR_SAVE, argv[ii].str)) {
            cond->save = 1;
            continue;
        }
        if (0 == strcmp(STR_NOBOOT, argv[ii].str)) {
            cond->boot = 0;
            continue;
        }
        if (0 == strcmp(STR_UART, argv[ii].str)) {
            cond->uart_res = 1;
            continue;
        }
    }
}


static int set_env_and_boot (conditions_t *cond)
{
    char *val;
    char *val_root = VAL_ROOT_ANDROID;
    char *oldval;
    char *tool;
    int rv;
    bool NewHatsPart = false;

    if ((get_machine_id() == MACH_TYPE_U9540)
        || (get_machine_id() == MACH_TYPE_U8540))
        NewHatsPart = true;
    /* Setup init */
    if (cond->sys == SYS_ANDROID) {
        val = VAL_INIT_ANDROID;
        if (NewHatsPart == true) {
            val_root = VAL_ROOT_ANDROID;
        }
    } else {
        if (NewHatsPart == true) {
            val = VAL_INIT_HATS_IMG;
            val_root = VAL_ROOT_HATS;
        }
        else
            val = VAL_INIT_HATS;
    }
    if (create_env (ENV_INIT, val, true)) {
        dprintf (CRITICAL, "Failed to create %s\n", ENV_INIT);
        return 1;
    }
    if (NewHatsPart == true) {
        if (create_env (ENV_ROOT, val_root, true)) {
            dprintf (CRITICAL, "Failed to create %s\n", ENV_ROOT);
            return 1;
        }
    }
    /* Setup UART */
    /* Get backup and current console settings */
    oldval = tenv_getval (ENV_OLD_CONSOLE);
    val = tenv_getval (ENV_CONSOLE);

    if (!val) {
        dprintf (CRITICAL, "Failed to get console settings\n");
        return 1;
    }

    if (cond->sys == SYS_ANDROID) {
        /* Check for console disble */
        if (0 == strcmp(val, VAL_DEV_NULL)) {
            if (NULL == oldval) {
                dprintf (CRITICAL, "No console backup, setting default \"%s\"\n",
                                DEFAULT_CONSOLE);
                /* Default console */
                val = DEFAULT_CONSOLE;
            } else {
                /* Restore to old value */
                val = oldval;
            }
        }

        /* Restore active console */
        if (create_env (ENV_CONSOLE, val, true)) {
            dprintf (CRITICAL, "Failed to create %s\n", ENV_CONSOLE);
            return 1;
        }

        /* Invalidate HATS console, if exists */
        if (create_env (ENV_TOOL, "0", true)) {
            dprintf (CRITICAL, "Failed to create %s\n", ENV_TOOL);
            return 1;
        }
    } else {
        if (cond->uart_res) {
            /* Backup current console */
            if (create_env (ENV_OLD_CONSOLE, val, true)) {
                dprintf (CRITICAL, "Failed to create %s\n", ENV_OLD_CONSOLE);
                return 1;
            }

            /* Redirect console */
            if (create_env (ENV_CONSOLE, VAL_DEV_NULL, true)) {
                dprintf (CRITICAL, "Failed to create %s\n", ENV_CONSOLE);
                return 1;
            }

            /* Set HATS console */
            tool = malloc (3 + strlen(val));
            if (!tool) {
                dprintf (CRITICAL, "Failed to alloc HATS tool parameter\n");
                return 1;
            }
            tool[0] = '1';
            tool[1] = ',';
            tool[2] = 0;
            strcat (tool, val);

            rv = create_env (ENV_TOOL, tool, true);
            free (tool);
            if (rv) {
                dprintf (CRITICAL, "Failed to create %s\n", ENV_TOOL);
                return 1;
            }
        }
    }

    /* Save environment */
    if (cond->save) {
        dprintf (INFO, "Saving environment\n");
        if (tenv_save ()) {
            dprintf (CRITICAL, "Saving environment failed\n");
            return 1;
        }
    }

    /* Boot */
    if (cond->boot) {
#if BOOTLOADER_START_MODEM
        if (0 != start_modem()) {
            dprintf (CRITICAL, "Failed to start modem");
            return 1;
        }
#endif
        boot_from_flash(BOOT_NAME);
        /* start_linux should not return! If it does, somethings wrong...*/
        return 1;
    }

    return 0;
}

#endif /* WITH_LIB_CONSOLE */
