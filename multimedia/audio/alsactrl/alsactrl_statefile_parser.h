
/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */
/*****************************************************************************/
#ifndef _AUDIO_HWCTRL_STATEFILE_PARSER_H_
#define _AUDIO_HWCTRL_STATEFILE_PARSER_H_

#include <tinyalsa/asoundlib.h>
#include <stdbool.h>
#define ENUM_VAL_MAX_LEN	128
#define CTRL_NAME_MAX_LEN	100

union value_t
{
	int integer;/*Bool, int*/
	long long integer64;/*int64*/
	char enum_val_str[ENUM_VAL_MAX_LEN];/*byte, enum*/
	double real;
	const void *ptr;
};

struct listvalue
{
	union value_t data;
	int pos;
	struct listvalue *next;
};

struct parser_context
{
	char *save_ptr_outer;
	char *Inner_data;
	char *temp_string;
	char *save_ptr_inner;
	char *token;
	int  header;
	struct listvalue *listhead; /* head for the list */
	struct listvalue *list;
	int allocated_nodes_count; /* keep track of number of values : may be same as count*/
	int node_flag; /*needs to be initialized by 0*/
};

struct control_config_t
{
	char name[CTRL_NAME_MAX_LEN];
	enum mixer_ctl_type type;
	int count;
	union value_t *value; /* pointer to store multiple values*/
	struct control_config_t *next;
};
int init_parser(struct parser_context **parser_p,const char *data);
void exit_parser(struct parser_context *parser);
int copy_control_list(struct control_config_t **dest, struct control_config_t *src);
int delete_control_list(struct control_config_t *list);
int populate_control_list(struct parser_context *parser_p,struct control_config_t **list);
int override_control_list(struct control_config_t *main_llist, struct control_config_t *override_llist);

#endif /*_AUDIO_HWCTRL_STATEFILE_PARSER_H_ */
