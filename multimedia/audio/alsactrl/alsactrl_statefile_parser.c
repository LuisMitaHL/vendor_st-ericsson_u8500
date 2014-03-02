/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */
/*****************************************************************************/

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "alsactrl_debug.h"
#include "alsactrl_statefile_parser.h"
#define SPACE ' '

/**
 * \brief Substitutes one configuration node to another.
 * \param dst Handle to the destination node.
 * \param src Handle to the source node. Must not be the same as \p dst.
 * \return Zero if successful, otherwise a negative error code.
 *
 * If both nodes are compounds, the source compound node members are
 * appended to the destination compound node.
 *
 * If the destination node is a compound and the source node is
 * an ordinary type, the compound members are deleted (including
 * their contents).
 *
 * A successful call to this function invalidates the source node.
 */


static int add_node_to_list(struct parser_context *parser_p, enum mixer_ctl_type type,
								char *value_p, int position)
{
	struct listvalue *temp;
	LOG_I("Enter");
	temp = (struct listvalue *)parser_p->list;
	if (!parser_p->list) {
		parser_p->list = (struct listvalue *)calloc(1, sizeof(struct listvalue));
		temp = parser_p->list;
		parser_p->listhead = temp;
	} else {
		while ((temp->next)!=NULL) {
			temp=temp->next;
		}
		temp->next = (struct listvalue *)calloc(1, sizeof(struct listvalue));
		if (!(temp->next)) {
			LOG_E("Error: Calloc failed Unable to allocate memory to control node");
			return -1;
		}
		temp=temp->next;
	}
	switch (type) {
	case MIXER_CTL_TYPE_BOOL:
	case MIXER_CTL_TYPE_INT:
		temp->data.integer = atoi(value_p);
		break;
	case MIXER_CTL_TYPE_ENUM:
		strncpy(temp->data.enum_val_str, value_p, ENUM_VAL_MAX_LEN);
		break;
	default:
		LOG_E("ERROR: Invalid value!");
		free(temp);
		return -1;
	}
	temp->pos = position;
	temp->next = NULL;
	parser_p->allocated_nodes_count++;
	return 0;
}
static int RemoveMultiBlanks(char *string)
{
	char *blank, *start;
	int length, c = 0, d = 0;
	length = strlen(string);
	blank = string;
	start = (char*)malloc(length + 1);
	if (start == NULL) {
		LOG_E("Error: Malloc failed unable to allocate memory to start");
		return -1;
	}
	while (*(blank + c)) {
		if (*(blank + c) == SPACE && *(blank + c + 1) == SPACE) {
		} else {
			*(start + d) = *(blank + c);
			d++;
		}
		c++;
	}
	*(start + d) = '\0';
	strcpy(string, start);
	free(start);
	return 0;
}

static int insert_count(struct control_config_t *node, int count)
{
	node->count = count;
	LOG_I("Count inserted");
	return 0;
}

static int insert_name(struct control_config_t *node, char *name)
{
	strncpy(node->name, name, sizeof(node->name));
	LOG_I("Name inserted");
	return 0;
}

static int insert_type(struct control_config_t *node, enum mixer_ctl_type Type)
{
	node->type = Type;
	LOG_I("Type inserted");
	return 0;
}

int init_parser(struct parser_context **parser_p, const char *data)
{
	*parser_p = (struct parser_context *)calloc(1, sizeof(struct parser_context));
	if (!(*parser_p)) {
		LOG_E("Error: Calloc failed unable to allocate memory to parser context");
		return -1;
	}
	(*parser_p)->token = strtok_r(data, "{}", &((*parser_p)->save_ptr_outer));
	/*validate the string before further operations*/
	if (!(*parser_p)->token)
		return -1;
	LOG_I("Parser initialized successfully !");
	return 0;
}

static void get_count(struct parser_context *parser, struct control_config_t *nextnode)
{
	parser->Inner_data = parser->Inner_data + strlen(parser->temp_string) + 1;
	parser->temp_string = strtok_r(parser->Inner_data, " ", &(parser->save_ptr_inner));
	insert_count(nextnode, atoi(parser->temp_string));
	parser->Inner_data = parser->Inner_data + strlen(parser->temp_string) + 1;
	LOG_I("The count is : %s", parser->temp_string);
}

static void get_type(struct parser_context *parser, struct control_config_t *nextnode)
{
	parser->Inner_data = parser->Inner_data + strlen(parser->temp_string) + 1;
	parser->temp_string = strtok_r(parser->Inner_data, " ", &(parser->save_ptr_inner));
	if (strncmp(parser->temp_string, "ENUMERATED", 11)==0) {
		insert_type(nextnode, MIXER_CTL_TYPE_ENUM);
	}
	if (strncmp(parser->temp_string, "BOOLEAN", 8)==0) {
		insert_type(nextnode, MIXER_CTL_TYPE_BOOL);
	}
	if (strncmp(parser->temp_string, "INTEGER", 8)==0) {
		insert_type(nextnode, MIXER_CTL_TYPE_INT);
	}
	LOG_I("The Type is : %s", parser->temp_string);
	parser->Inner_data = parser->Inner_data + strlen(parser->temp_string) + 1;
}

static int get_value(struct parser_context *parser, int valueFlag,
						struct control_config_t *nextnode)
{
	int position = 0, ret;
	parser->Inner_data = parser->Inner_data + strlen(parser->temp_string) + 1;
	if (parser->Inner_data[0] == 39) {
		parser->temp_string = strtok_r(parser->Inner_data, "'", &(parser->save_ptr_inner));
		parser->Inner_data = parser->Inner_data + strlen(parser->temp_string) + 3;
	} else {
		parser->temp_string = strtok_r(parser->Inner_data, " ", &(parser->save_ptr_inner));
		if ((parser->temp_string)[0] >= 48 && (parser->temp_string)[0] <= 57) {
			if (valueFlag == 0)
				position = atoi(parser->temp_string);
			else
				position = 0;
			LOG_I("position = %d", position);
		}
		parser->Inner_data = parser->Inner_data + strlen(parser->temp_string) + 1;
		if (parser->Inner_data[0]) {
			if (parser->Inner_data[0] == 39) {
				parser->temp_string = strtok_r(parser->Inner_data, "'", &(parser->save_ptr_inner));
				parser->Inner_data = parser->Inner_data + strlen(parser->temp_string) + 3;
			} else {
				parser->temp_string = strtok_r(parser->Inner_data, " ", &(parser->save_ptr_inner));
				parser->Inner_data = parser->Inner_data + strlen(parser->temp_string) + 1;
			}
		}
	}
	LOG_I("The value is : %s", parser->temp_string);
	ret = add_node_to_list(parser, nextnode->type, parser->temp_string, position);
	return ret;
}

static bool get_name(struct parser_context *parser, struct control_config_t *nextnode)
{
	bool valid_name = false;
	parser->Inner_data = parser->Inner_data + strlen(parser->temp_string) + 1;
	if (parser->Inner_data[0] == 39) {
		parser->temp_string = strtok_r(parser->Inner_data, "'", &(parser->save_ptr_inner));
		insert_name(nextnode, parser->temp_string);
		LOG_I("The name is : %s", parser->temp_string);
		valid_name = true;
		parser->Inner_data = parser->Inner_data + strlen(parser->temp_string) + 3;
	} else {
		parser->temp_string = strtok_r(parser->Inner_data, " ", &(parser->save_ptr_inner));
		insert_name(nextnode, parser->temp_string);
		LOG_I("The name is : %s", parser->temp_string);
		valid_name = true;
		parser->Inner_data = parser->Inner_data + strlen(parser->temp_string) + 1;
	}
	return valid_name;
}

static void get_iface(struct parser_context *parser)
{
	parser->Inner_data = parser->Inner_data + strlen(parser->temp_string) + 1;
	parser->temp_string = strtok_r(parser->Inner_data, " ", &(parser->save_ptr_inner));
	LOG_I("The iface is : %s", parser->temp_string);
	parser->Inner_data = parser->Inner_data + strlen(parser->temp_string) + 1;
}

static void get_item(struct parser_context *parser)
{
	parser->Inner_data = parser->Inner_data + strlen(parser->temp_string) + 1;
	if (parser->Inner_data[0] == 39) {
		parser->temp_string = strtok_r(parser->Inner_data, "'", &(parser->save_ptr_inner));
		parser->Inner_data = parser->Inner_data + strlen(parser->temp_string) + 3;
	} else {
		parser->temp_string = strtok_r(parser->Inner_data, " ", &(parser->save_ptr_inner));
		parser->Inner_data = parser->Inner_data + strlen(parser->temp_string) + 1;
	}
	LOG_I("The item number is : %s", parser->temp_string);
}

static void get_access(struct parser_context *parser)
{
	parser->Inner_data = parser->Inner_data + strlen(parser->temp_string) + 1;
	parser->temp_string = strtok_r(parser->Inner_data, "'", &(parser->save_ptr_inner));
	LOG_I("The access is : %s", parser->temp_string);
	parser->Inner_data = parser->Inner_data + strlen(parser->temp_string) + 2;
}

static int get_next_control(struct parser_context *parser, struct control_config_t *nextnode)
{
	char *ptr_inner_data = NULL;
	int valueFlag = 0, ret;
	bool valid_name = false;
	if (!(nextnode)) {
		LOG_E("Error: Insufficient memory to allocate to node!");
		return -1;
	}
start:
	LOG_I("Moving to next node");
	(parser->node_flag)++;
	parser->token = strtok_r(NULL, "{}", &(parser->save_ptr_outer));
	if (!(parser)->token) {
		LOG_I("NULL token found");
		return -1;
	}
	if (strstr(parser->token, "control"))
		goto start;
	if ((parser->node_flag) % 2 == 0) {
		int block_size;
		block_size = strlen(parser->token)*sizeof(char) + 1;
		ptr_inner_data = parser->Inner_data = (char*)calloc(1, block_size);
		if (!(parser->Inner_data)) {
			LOG_E("Error: Calloc failed unable to allocate memory to Inner_data");
			return -1;
		}
		strcpy(parser->Inner_data, parser->token);
		int accessFlag = 0, typeFlag = 0, countFlag = 0, removeflag;
		removeflag = RemoveMultiBlanks(parser->Inner_data);
		if (removeflag) {
			LOG_E("unable to remove multiple blanks");
			free(parser->Inner_data);
			return -1;
		}
		RemoveMultiBlanks(parser->Inner_data);
		while (parser->Inner_data) {
			if (parser->Inner_data[0] == 'i' || parser->Inner_data[0] == 'n') {
					parser->temp_string = strtok_r(parser->Inner_data, " ", &(parser->save_ptr_inner));
			} else if (parser->Inner_data[0] == 'v') {
				if (parser->Inner_data[5] == '.') {
					parser->temp_string = strtok_r(parser->Inner_data, ".", &(parser->save_ptr_inner));
				} else {
					parser->temp_string = strtok_r(parser->Inner_data, " ", &(parser->save_ptr_inner));
					valueFlag = 1;
				}
			} else {
				parser->temp_string = strtok_r(parser->Inner_data, ".", &(parser->save_ptr_inner));
				if (!(parser->temp_string))
					break;
			}
			if (strncmp(parser->temp_string, " comment", 8) == 0 || strncmp(parser->temp_string, "comment", 7) == 0) {
				parser->Inner_data = parser->Inner_data + strlen(parser->temp_string) + 1;
				parser->temp_string = strtok_r(parser->Inner_data, "  ", &(parser->save_ptr_inner));
				if (strncmp(parser->temp_string, "access", 6) == 0) {
					get_access(parser);
					accessFlag = 1;
				} else if (strncmp(parser->temp_string, "type", 4) == 0) {
					get_type(parser, nextnode);
					typeFlag = 1;
				} else if (strncmp(parser->temp_string, "count", 5) == 0) {
					get_count(parser, nextnode);
					countFlag = 1;
				} else if (accessFlag ==1 && countFlag == 1 && typeFlag == 1) {
					get_item(parser);
				}
			} else if (strncmp(parser->temp_string, "iface", 5) == 0) {
				get_iface(parser);
			} else if (strncmp(parser->temp_string, "name", 4) == 0) {
				valid_name = get_name(parser, nextnode);
                                if (valid_name != true) {
					LOG_E("Invalid name !");
					free(parser->Inner_data);
					return -1;
                                }
			} else if (strncmp(parser->temp_string, "value", 5) == 0) {
				ret = get_value(parser, valueFlag, nextnode);
				if (ret) {
					LOG_E("Unable to write the value to node");
					free(parser->Inner_data);
					return -1;
				}
			}
			while (parser->Inner_data[0] == SPACE) {
				parser->Inner_data++;
			}
		}
	}
	nextnode->value = (union value_t*)calloc(nextnode->count, sizeof(union value_t));
	if (!nextnode->value) {
		LOG_E("ERROR: Insufficient memory\n");
		free(parser->Inner_data);
		return -1;
	}
	struct listvalue *temp, *previous_node;
	temp = parser->listhead;
	int i;
	for (i = 0 ; i < (int)(parser->allocated_nodes_count); i++) {
		memcpy((char *)(nextnode->value + (int)temp->pos),
				(char *)&(temp->data),
				sizeof(union value_t));
		previous_node = temp;
		temp = temp->next;
		if (previous_node)
			free(previous_node);
	}
	parser->listhead = NULL;
	parser->list = NULL;
	parser->allocated_nodes_count = 0;
	if (ptr_inner_data)
		free(ptr_inner_data);
	LOG_I("Exit !");
	return 0 ;
}

void exit_parser(struct parser_context *parser)
{
	LOG_I("\tFree parser\n");
	free(parser);
}

static char *type2str(enum mixer_ctl_type type)
{
	switch (type) {
	case MIXER_CTL_TYPE_BOOL:
		return "BOOL";
	case MIXER_CTL_TYPE_INT:
		return "INTEGER";
	case MIXER_CTL_TYPE_ENUM:
		return "ENUMERATED";
	case MIXER_CTL_TYPE_BYTE:
		return "BYTE";
	case MIXER_CTL_TYPE_IEC958:
		return "IEC958";
	case MIXER_CTL_TYPE_INT64:
		return "INT64";
	default:
		return "UNKNOWN";
	}
	return "UNKNOWN";
}

static void display_control_list(struct control_config_t *node)
{
	int i;
	while (node) {
		for (i=0; i<node->count; i++) {
			switch (node->type) {
			case MIXER_CTL_TYPE_BOOL:
			case MIXER_CTL_TYPE_INT:
				LOG_I("\tName = %s, Type = %s, Count= %d Value= %d \n",
											node->name,
											type2str(node->type),
											node->count,
											node ->value[i].integer);
				break;
			case MIXER_CTL_TYPE_ENUM:
				LOG_I ("\tName = %s, Type = %s, Count= %d Value= %s \n",
											node->name,
											type2str(node->type),
											node->count,
											node ->value[i].enum_val_str);
			}
		}
		node = node->next;
	}
}

int populate_control_list(struct parser_context *parser_p, struct control_config_t **list)
{
	int count = 0;
	struct control_config_t *head = NULL, *node;
	while (1) {
		if (!head) {
			node = (struct control_config_t *)calloc(1, sizeof(struct control_config_t));
			if (!node) {
				LOG_E("Error: Calloc failed unable to allocate memory to node");
				return -1;
			}
			LOG_I("\tNODE Number = %d address = %x \n", count, node);
			if (get_next_control(parser_p, node)== -1) {
				LOG_I("none of the valid controls found");
				free(node);
				return -1;
			}
			head = node;
		} else {
			node->next = (struct control_config_t *)calloc(1, sizeof(struct control_config_t));
			if (!(node->next)) {
				delete_control_list(head);
				LOG_E("Error: Insufficient memory for Node->next\n");
				return -1;
			}
			LOG_I("\tNODE Number = %d address = %x \n", count, node->next);
			if (get_next_control(parser_p ,node->next) == -1) {
				LOG_I("reached at the end of the control list");
				free(node->next);
				node->next = NULL;
				break;
			} else
				node = node->next;
		}
		LOG_I("Count = %d\n", count++);
	}
	*list = head;
	LOG_I("Exit !");
	display_control_list(*list);/*Display the whole list*/
	return 0;
}

int delete_control_list(struct control_config_t *list)
{
	struct control_config_t *temp;
	int i = 0;
	LOG_I("Enter");
	while (list!= NULL) {
		i++;
		temp = list->next;
		if (temp) {
			free(temp->value);
			free(list);
		}
		list = temp;
	}
	return 0;
}

static int node_copy(struct control_config_t *dest, struct control_config_t *src)
{
	LOG_I("Enter");
	if (!src || !dest)
		return -1;
	strncpy((dest->name), (src->name), CTRL_NAME_MAX_LEN);
	dest->type  = src->type;
	dest->count = src->count;
	dest->value = (union value_t *)malloc(src->count * sizeof(union value_t));
	if (!(dest->value)) {
		LOG_E("Error: Malloc failed unable to allocate memory to dest->value");
		return -1;
	}
	memcpy(dest->value, src->value, src->count * sizeof(union value_t));
	return 0;
}

int copy_control_list(struct control_config_t **dest, struct control_config_t *src)
{
	LOG_I("Enter");
	int ret;
	if (!src)
		return -1;
	*dest = (struct control_config_t*)calloc(1, sizeof(struct control_config_t));
	if (!(*dest)) {
		LOG_E("Error: Calloc failed unable to allocate memory to dest node");
		return -1;
	}
	ret = node_copy(*dest, src);
	if (ret) {
		LOG_E("Unable to copy the node");
		free(*dest);
		return -1;
	}
	(*dest)->next = NULL;
	copy_control_list(&((*dest)->next), src->next);
	return 0;
}

static int append_node(struct control_config_t *main_llist, struct control_config_t *new_node)
{
	struct control_config_t *temp_p = main_llist;
	LOG_I("Enter");
	int ret;
	if (!temp_p) {
		LOG_I("append failed");
		return -1;
	}
	while ((temp_p->next)!=NULL) {
		temp_p=temp_p->next;
	}
	temp_p->next = (struct control_config_t *)malloc(sizeof(struct control_config_t));
	if (!(temp_p->next)) {
		LOG_E("Error: Malloc failed unable to allocate memory to temp_p->next");
		return -1;
	}
	temp_p=temp_p->next;
	temp_p->next = NULL;
	ret = node_copy(temp_p, new_node);
	if (ret) {
		LOG_E("unable to copy the node");
		free(temp_p->next);
		return -1;
	}
	return 0;
}

int override_control_node(struct control_config_t *main_llist,
							struct control_config_t *override_node)
{
	int i = 0;
	struct control_config_t *temp_p = main_llist;
	LOG_I("Enter");
	while (temp_p) {
		i++;
		if (strcmp(temp_p->name, override_node->name)==0) {
			memcpy(temp_p->value, override_node->value, temp_p->count*sizeof(union value_t));
			LOG_I("successfully Override control name = %s", temp_p->name);
			return 1;
		}
		temp_p = temp_p->next;
	}
	return 0;
}

int override_control_list(struct control_config_t *main_llist,
							struct control_config_t *override_llist)
{
	struct control_config_t *node_p = NULL;
	node_p = override_llist;
	int over_ride, ret_val = 0;
	LOG_I("Enter");
	while (node_p) {
		over_ride = override_control_node(main_llist, node_p);
		if (!over_ride) {
			ret_val = append_node(main_llist, node_p);
			if (ret_val) {
				LOG_I("Unable to append node");
				break;
			}
		}
		node_p = node_p->next;
	}
	return ret_val;
}

#ifdef ALSACTRL_DEBUG_CTRLLIST
static int DumpCfgFile(struct control_config_t *node, char *pretext)
{
	int i;
	FILE *fd = NULL;
	fd = fopen(CONFIG_FILE_NAME, "a");
	fprintf(fd,"\t%s\n", pretext);
	if (fd != NULL) {
		while (node) {
			fprintf(fd,"\t%s\n", node->name);
			fprintf(fd,"\t%d\n", node->count);
			fprintf(fd,"\t%d\n", (int)node->type);
			for (i=0; i<node->count; i++) {
				switch (node->type) {
				case MIXER_CTL_TYPE_BOOL:
				case MIXER_CTL_TYPE_INT:
					fprintf(fd,"\t%d\n\n", node->value[i].integer);
					break;
				case MIXER_CTL_TYPE_ENUM:
					fprintf(fd,"\t%s\n\n", node->value[i].enum_val_str);
				}
			}
			node = node->next;
		}
	}
	fclose(fd);
	return 0;
}
#endif
