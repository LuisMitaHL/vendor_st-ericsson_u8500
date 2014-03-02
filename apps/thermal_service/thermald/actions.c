/*
 * Thermal Service Manager
 *
 * Maintains the potential actions that can occur when
 * a thermal event happens. The actions are either from developed
 * by STE or implemented in the Android service.
 *
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include <sys/types.h>
#include <sys/stat.h>

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <ctype.h>

#include "actions.h"
#include "mitigation_actions.h"
#include "log.h"

#ifdef STE_MODEM_THERMAL_MITIGATIONS
#include "modem_mitigations.h"
#endif

typedef void (*action_function_t)(void);

typedef struct action_table_s {
	char *action_name;
	action_function_t action;
	struct action_table_s *next;
} action_table_t;

static void add_builtin(const char *action, action_function_t func);
static void add_action_to_list(action_table_t *action);
static int actions_exists(const char *action);

static action_table_t *actions_table = NULL;

/*
 * Init actions table
 */
void actions_init(void) {
	actions_cpufreq_init();
	add_builtin("NONE", NULL);
	add_builtin("SHUTDOWN", action_shutdown);
	add_builtin("CPUPERFORMANCELEVEL1", action_cpuperflevel1);
	add_builtin("CPUPERFORMANCELEVEL2", action_cpuperflevel2);
	add_builtin("CPUPERFORMANCELEVEL3", action_cpuperflevel3);
	add_builtin("CPUPERFORMANCELEVEL4", action_cpuperflevel4);
	add_builtin("DISABLEBATTERYCHARGING", action_stopbatterycharge);
	add_builtin("ENABLEBATTERYCHARGING", action_startbatterycharge);

#ifdef STE_MODEM_THERMAL_MITIGATIONS
	add_builtin("MODEM_RF_THERMAL_MITIGATION_ACTIVE", modem_enablethermal);
	add_builtin("MODEM_RF_THERMAL_MITIGATION_INACTIVE", modem_disablethermal);
#endif
}

/*
 * Add an action to the system if it does not exist
 */
void actions_add(const char *action) {
	action_table_t *a;

	if (actions_exists(action)) {
		return;
	}

	if (action == NULL || isspace(*action)) {
		/* invalid string */
		return;
	}

	DBG("adding action %s to system...\n", action);

	a = (action_table_t *) malloc(sizeof(action_table_t));
	if (a == NULL) {
		ERR("unable to allocate memory\n");
		return;
	}

	a->action_name = strdup(action);
	if (a->action_name == NULL) {
		ERR("unable to allocate memory\n");
		free(a);
		return;
	}

	a->action = NULL;
	a->next = NULL;

	add_action_to_list(a);
}

/*
 * Add a built in action to the system
 */
static void add_builtin(const char *action, action_function_t func) {
	action_table_t *a;

	if (actions_exists(action)) {
		DBG("duplicate action. ignoring...\n");
		return;
	}

	a = (action_table_t *) malloc(sizeof(action_table_t));
	if (a == NULL) {
		ERR("unable to allocate memory\n");
		return;
	}

	a->action_name = strdup(action);
	if (a->action_name == NULL) {
		ERR("unable to allocate memory\n");
		free(a);
		return;
	}

	a->action = func;
	a->next = NULL;

	add_action_to_list(a);
}

/*
 * Actually manipulates the linked list
 */
static void add_action_to_list(action_table_t *action) {

	if (action == NULL) {
		return;
	}

	action->next = NULL;

	if (actions_table == NULL) {
		actions_table = action;
	} else {
		action_table_t *nxt = actions_table;
		while (nxt->next != NULL) {
			nxt = nxt->next;
		}
		nxt->next = action;
	}
}

/*
 * Determines if the action is known to the system
 */
static int actions_exists(const char *action) {
	action_table_t *a = actions_table;

	while (a != NULL) {
		if (strcmp(a->action_name, action) == 0) {
			return 1;
		}
		a = a->next;
	}

	return 0;
}

/*
 * Creates a list of action strings
 */
int actions_getlist(char **list, int *count) {
	action_table_t *a;
	int i, len, l;
	char *str;

	len = 0; /* total length of returned string */
	i = 0;
	str = NULL;

	a = actions_table;
	while (a != NULL) {
		char *tmp;
		l = strlen(a->action_name) + 1;
		tmp = realloc(str, len + l);
		if (tmp == NULL) {
			ERR("unable to allocate memory\n");
			i = len = 0;
			if (str != NULL) {
				free(str);
			}
			free(a);
			goto end;
		}
		str = tmp;
		strncpy((str + len), a->action_name, l);
		len += l;
		i++;

		a = a->next;
	}

end:
	*list = str;
	*count = i;

	return len;
}

/*
 * Executes a given action
 */
int actions_execute(const char *action) {
	action_table_t *a;

	if (action == NULL) {
		return -1;
	}

	a = actions_table;
	while (a != NULL) {
		if (a->action_name != NULL) {
			if (strcmp(action, a->action_name) == 0) {
				if (a->action != NULL) {
					INF("executing mitigation action %s...\n", action);
					a->action();
				}
				return 0;
			}
		} else {
			ERR("action's list table corrupted!\n");
			return -1;
		}

		a = a->next;
	}
	return -1;
}
