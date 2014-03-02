/*****************************************************************************/
/**
*  © ST-Ericsson, 2011 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \file	   rfparams.c
* \brief   routines to parse Comma Separated Value files
* \author  ST-Ericsson
*/
/*****************************************************************************/
#include "dthrf.h"
#include "rfdef.h"
#include "rfparams.h"
#include "pmmidmap.h"

static struct dth_element *g_rf_params = NULL;
static int g_nb_rf_params = 0;
static u8 *g_rf_params_type = NULL;
static int g_nb_rf_params_type = 0;

/* test line is UNIX shell type comment */
int csv_is_comment(const char *str)
{
	int vl_is_comment = 0;

	const char *ptr = str;
	while (('\0' != *ptr) && (isspace(*ptr) != 0)) {
		/* char is space */
		ptr++;
	}

	if ('#' == *ptr) {
		/* this string is a comment */
		vl_is_comment = 1;
	}

	return vl_is_comment;
}

/* test line is an empty line which contain trailing blanks */
int csv_is_empty_line(const char *str)
{
	int vl_is_empty = 0;

	const char *ptr = str;
	while (('\0' != *ptr) && (isspace(*ptr) != 0)) {
		/* char is space */
		ptr++;
	}

	if (('\n' == *ptr) || ('\0' == *ptr)) {
		/* this is an empty string */
		vl_is_empty = 1;
	}

	return vl_is_empty;
}

/* allocate an array of dth_element and initialize each entry */
struct dth_element *dth_element_alloc(int nb_element)
{
	int i;
	struct dth_element *p_elements = NULL;
	struct dth_element *copie = NULL;

	p_elements = calloc(nb_element, sizeof(struct dth_element));
	if (p_elements == NULL)
		goto alloc_error;

	copie = p_elements;

	for (i = 0; i < nb_element; i++) {
		p_elements->path = calloc(MAX_LOCAL_STRING_SIZE, sizeof(char));
		if (p_elements->path == NULL)
			goto alloc_error;

		p_elements->type = 0;
		p_elements->cols = 0;
		p_elements->rows = 0;

		p_elements->enumString =
		    calloc(MAX_LOCAL_STRING_SIZE, sizeof(char));
		if (p_elements->enumString == NULL)
			goto alloc_error;

		p_elements->info = calloc(MAX_LOCAL_STRING_SIZE, sizeof(char));
		if (p_elements->info == NULL)
			goto alloc_error;

		p_elements->min = 0;
		p_elements->max = 0;
		p_elements->get = NULL;
		p_elements->set = NULL;
		p_elements->exec = NULL;
		p_elements->user_data = 0;
		p_elements->default_value = NULL;
		p_elements++;
	}

	p_elements = copie;
	return p_elements;

alloc_error:
	if (p_elements != NULL) {
		free(p_elements);
	}

	return NULL;
}

int get_param_store_type(int param_id, int *store_type)
{
	int result = TAT_BAD_REQ;

	if ((NULL != g_rf_params_type) && (g_nb_rf_params_type > param_id)) {
		*store_type = g_rf_params_type[param_id];
		result = 0;
	}

	return result;
}

static int set_param_store_type(int param_id, int store_type)
{
	int result = TAT_BAD_REQ;

	if ((NULL != g_rf_params_type) && (g_nb_rf_params_type > param_id)) {
		g_rf_params_type[param_id] = store_type;
		result = 0;
	}

	return result;
}

int load_rf_param_file(const char *filename)
{
	FILE *file = NULL;
	struct dth_element *p_elem = NULL;
	char s_buffer[MAX_CHAR_PER_LINE];
	char *token;
	int nb_alloc_elements, nb_init_elements = 0;
	size_t vl_position = 0;

	/* open the file */
	file = fopen(filename, "r");
	if (file == NULL) {
		SYSLOG(LOG_ERR, "fail to open file %s \n", filename);
		return TAT_ERROR_FILE_OPEN;
	}

	/* count number of records for allocation */

	/* first line is the file header. this is not a record */
	fgets(s_buffer, MAX_CHAR_PER_LINE, file);

	nb_alloc_elements = 0;
	while (!feof(file)) {
		fgets(s_buffer, MAX_CHAR_PER_LINE, file);
		if (!csv_is_comment(s_buffer) && !csv_is_empty_line(s_buffer)) {
			nb_alloc_elements++;
		}
	}

	/* allocate memory */
	free_rf_params();

	g_nb_rf_params = nb_alloc_elements;
	g_rf_params = dth_element_alloc(g_nb_rf_params);

	g_nb_rf_params_type = tatl14_01CountRfParam();

	g_rf_params_type = (u8 *) calloc(g_nb_rf_params_type, sizeof(u8));

	/* set cursor to first element */
	p_elem = g_rf_params;

	/* go back to the beginning of the file */
	fseek(file, 0, SEEK_SET);

	/* read the first out of the loop because it is not useful */
	fgets(s_buffer, MAX_CHAR_PER_LINE, file);

	nb_init_elements = 0;
	while (!feof(file)) {
		memset(s_buffer, 0, MAX_CHAR_PER_LINE);

		/* get and split the line into token separated by "," */
		fgets(s_buffer, MAX_CHAR_PER_LINE, file);

		/* Ignore the current record if the line begins by a comment mark */
		if (csv_is_comment(s_buffer) || csv_is_empty_line(s_buffer)) {
			/* this line is a comment or empty string, ignore it */
			/*printf("This line is IGNORED:\n    %s\n", s_buffer); */
			continue;
		}

		nb_init_elements++;
		if (nb_init_elements > nb_alloc_elements) {
			SYSLOG(LOG_WARNING, "buffer overflow detected!\n");
			break;
		}
		/* read path and paramater name */
		token = strtok(s_buffer, SEP);
		if (token == NULL) {
			break;
		}

		int pathlen = strlen(token);
		if ((pathlen > 0) && (pathlen < (MAX_LOCAL_STRING_SIZE - 2))) {
			strncpy(p_elem->path, token, pathlen);
			p_elem->path[pathlen] = 0;
			if (token[pathlen - 1] != '/') {
				p_elem->path[pathlen] = '/';
				p_elem->path[pathlen + 1] = 0;
			}
		} else {
			SYSLOG(LOG_ERR, "path '%s' is too long!\n", token);
			break;
		}

		token = strtok(NULL, SEP);	/* read parameter name */
		if (token == NULL) {
			break;
		}

		/*check if parameter exists in RF_HAL module. */

		int vl_paramid = tatl14_06GetRfParamId(token, &vl_position);
		if (vl_paramid < 0) {
			SYSLOG(LOG_ERR, "%s : param name not defined\n", token);
			if (file != NULL) {
				fclose(file);
			}
			return -1;
		} else {
			p_elem->user_data = vl_paramid;
		}

		/*add parameter name in the string ==> s=path+parameter. */
		if ((strlen(p_elem->path) + strlen(token)) >=
		    MAX_LOCAL_STRING_SIZE) {
			SYSLOG(LOG_ERR, "path exceeds max capactity: %s/%s\n",
			       p_elem->path, token);
			break;
		}
		strncat(p_elem->path, token, MAX_LOCAL_STRING_SIZE);

		/* read  DTH type_Display constant */
		token = strtok(NULL, SEP);
		if (!strcmp(token, "u8"))
			p_elem->type = DTH_TYPE_U8;
		else if (!strcmp(token, "s8"))
			p_elem->type = DTH_TYPE_S8;
		else if (!strcmp(token, "u16"))
			p_elem->type = DTH_TYPE_U16;
		else if (!strcmp(token, "s16"))
			p_elem->type = DTH_TYPE_S16;
		else if (!strcmp(token, "u32"))
			p_elem->type = DTH_TYPE_U32;
		else if (!strcmp(token, "s32"))
			p_elem->type = DTH_TYPE_S32;
		else if (!strcmp(token, "u64"))
			p_elem->type = DTH_TYPE_U64;
		else if (!strcmp(token, "s64"))
			p_elem->type = DTH_TYPE_S64;
		else if ((strcmp(token, "Float") == 00)
			 || (strcmp(token, "float") == 0))
			p_elem->type = DTH_TYPE_FLOAT;
		else {
			SYSLOG(LOG_ERR, "invalid display type for %s \n",
			       p_elem->path);
			p_elem->type = atoi(token);
		}

		/* read  DTH type_storage constant */
		token = strtok(NULL, SEP);
		if (!strcmp(token, "u8"))
			set_param_store_type(vl_paramid, DTH_TYPE_U8);
		else if (!strcmp(token, "s8"))
			set_param_store_type(vl_paramid, DTH_TYPE_S8);
		else if (!strcmp(token, "u16"))
			set_param_store_type(vl_paramid, DTH_TYPE_U16);
		else if (!strcmp(token, "s16"))
			set_param_store_type(vl_paramid, DTH_TYPE_S16);
		else if (!strcmp(token, "u32"))
			set_param_store_type(vl_paramid, DTH_TYPE_U32);
		else if (!strcmp(token, "s32"))
			set_param_store_type(vl_paramid, DTH_TYPE_S32);
		else if (!strcmp(token, "u64"))
			set_param_store_type(vl_paramid, DTH_TYPE_U64);
		else if (!strcmp(token, "s64"))
			set_param_store_type(vl_paramid, DTH_TYPE_S64);
		else if ((strcmp(token, "Float") == 00)
			 || (strcmp(token, "float") == 0))
			set_param_store_type(vl_paramid, DTH_TYPE_FLOAT);
		else {
			SYSLOG(LOG_ERR, "invalid storage type for %s \n",
			       p_elem->path);
			set_param_store_type(vl_paramid, atoi(token));
		}

		/* read  int cols */
		token = strtok(NULL, SEP);
		p_elem->cols = atoi(token);

		/* read  int rows */
		token = strtok(NULL, SEP);
		p_elem->rows = atoi(token);

		/* read  const char *enumString */
		token = strtok(NULL, SEP);
		if (strcmp(token, "NO_ENUM") == 0)
			p_elem->enumString = NULL;
		else {
			if ((strlen(token)) >= MAX_LOCAL_STRING_SIZE) {
				SYSLOG(LOG_WARNING,
				       "enum string is too long and has been truncated.\n");
			}
			strncpy((char *)p_elem->enumString, token,
				MAX_LOCAL_STRING_SIZE);
		}

		/* read  const char *info */
		token = strtok(NULL, SEP);
		if (strcmp(token, "NO_INFO") == 0)
			p_elem->info = NULL;
		else {
			char *pt_buffer = NULL;

			/*copy the vl_result into p_elem->info field */
			if ((strlen(token)) >= MAX_LOCAL_STRING_SIZE) {	/* warning */
				SYSLOG(LOG_WARNING,
				       "info string is too long and has been truncated.\n");
			}
			strncpy((char *)p_elem->info, token,
				MAX_LOCAL_STRING_SIZE);

			pt_buffer = (char *)p_elem->info;

			/* replace ! per a carriage return */
			pt_buffer = strchr(pt_buffer, '!');
			while (pt_buffer != NULL) {
				*pt_buffer = '\n';
				pt_buffer = strchr(pt_buffer, '!');
			}
		}

		/* read  long long min */
		token = strtok(NULL, SEP);
		p_elem->min = atoll(token);

		/* read  long long max */
		token = strtok(NULL, SEP);
		p_elem->max = atoll(token);

		/* read  int (*get)(struct dth_element *elem, void *value) */
		token = strtok(NULL, SEP);
		if (strcmp(token, "GetParam") == 0) {
			p_elem->get = DthRf_Get_PMM_Parameter;
		} else if (strcmp(token, "GetParam_Q1") == 0) {
			p_elem->get = DthRf_Get_PMM_Parameter_Q1;
		} else if (strcmp(token, "GetParam_Q2") == 0) {
			p_elem->get = DthRf_Get_PMM_Parameter_Q2;
		} else if (strcmp(token, "GetParam_Q3") == 0) {
			p_elem->get = DthRf_Get_PMM_Parameter_Q3;
		} else if (strcmp(token, "GetParam_Q6") == 0) {
			p_elem->get = DthRf_Get_PMM_Parameter_Q6;
		} else if (strcmp(token, "GetParam_Q8") == 0) {
			p_elem->get = DthRf_Get_PMM_Parameter_Q8;
		} else if (strcmp(token, "GetParam_Q10") == 0) {
			p_elem->get = DthRf_Get_PMM_Parameter_Q10;
		} else if (strcmp(token, "GetParam_Q12") == 0) {
			p_elem->get = DthRf_Get_PMM_Parameter_Q12;
		} else if (strcmp(token, "GetParam_Q14") == 0) {
			p_elem->get = DthRf_Get_PMM_Parameter_Q14;
		} else if (strcmp(token, "GetParam_Q16") == 0) {
			p_elem->get = DthRf_Get_PMM_Parameter_Q16;
		} else if (strcmp(token, "GetParam_Q23") == 0) {
			p_elem->get = DthRf_Get_PMM_Parameter_Q23;
		} else if (strcmp(token, "GetParamForAllBand") == 0) {
		} else {
			SYSLOG(LOG_ERR, "unresolved interface %s for %s\n",
			       token, p_elem->path);
			p_elem->get = NULL;
		}

		/* read int (*set)(struct dth_element *elem, void *value) */
		token = strtok(NULL, SEP);
		if (strcmp(token, "SetParam") == 0) {
			p_elem->set = DthRf_Set_PMM_Parameter;
		} else if (strcmp(token, "SetParam_Q1") == 0) {
			p_elem->set = DthRf_Set_PMM_Parameter_Q1;
		} else if (strcmp(token, "SetParam_Q2") == 0) {
			p_elem->set = DthRf_Set_PMM_Parameter_Q2;
		} else if (strcmp(token, "SetParam_Q3") == 0) {
			p_elem->set = DthRf_Set_PMM_Parameter_Q3;
		} else if (strcmp(token, "SetParam_Q6") == 0) {
			p_elem->set = DthRf_Set_PMM_Parameter_Q6;
		} else if (strcmp(token, "SetParam_Q8") == 0) {
			p_elem->set = DthRf_Set_PMM_Parameter_Q8;
		} else if (strcmp(token, "SetParam_Q10") == 0) {
			p_elem->set = DthRf_Set_PMM_Parameter_Q10;
		} else if (strcmp(token, "SetParam_Q12") == 0) {
			p_elem->set = DthRf_Set_PMM_Parameter_Q12;
		} else if (strcmp(token, "SetParam_Q14") == 0) {
			p_elem->set = DthRf_Set_PMM_Parameter_Q14;
		} else if (strcmp(token, "SetParam_Q16") == 0) {
			p_elem->set = DthRf_Set_PMM_Parameter_Q16;
		} else if (strcmp(token, "SetParam_Q23") == 0) {
			p_elem->set = DthRf_Set_PMM_Parameter_Q23;
		} else if (strcmp(token, "SetParamForAllBand") == 0) {
		} else {
			SYSLOG(LOG_ERR, "unresolved interface %s for %s\n",
			       token, p_elem->path);
			p_elem->set = NULL;
		}

		/* read int (*exec)(struct dth_element *elem) */
		token = strtok(NULL, SEP);
		p_elem->exec = NULL;

		/* read default_value */
		token = strtok(NULL, SEP);
		if (strcmp(token, "NULL") == 0)
			p_elem->default_value = NULL;
		else {
			switch (p_elem->type) {
			case DTH_TYPE_U8:

				p_elem->default_value = calloc(1, sizeof(u8));
				p_elem->default_value =
				    (void *)((u8 *) atoi(token));
				break;

			case DTH_TYPE_S8:

				p_elem->default_value = calloc(1, sizeof(s8));
				p_elem->default_value =
				    (void *)((s8 *) atoi(token));
				break;

			case DTH_TYPE_U16:

				p_elem->default_value = calloc(1, sizeof(u16));
				p_elem->default_value =
				    (void *)((u16 *) atoi(token));
				break;

			case DTH_TYPE_S16:

				p_elem->default_value = calloc(1, sizeof(s16));
				p_elem->default_value =
				    (void *)((s16 *) atoi(token));
				break;

			case DTH_TYPE_U32:

				p_elem->default_value = calloc(1, sizeof(u32));
				p_elem->default_value =
				    (void *)((u32 *) atoi(token));
				break;

			case DTH_TYPE_S32:

				p_elem->default_value = calloc(1, sizeof(s32));
				p_elem->default_value =
				    (void *)((s32 *) atoi(token));
				break;

			case DTH_TYPE_U64:

				p_elem->default_value = calloc(1, sizeof(u64));
				p_elem->default_value =
				    (void *)((u64 *) atol(token));
				break;

			case DTH_TYPE_S64:

				p_elem->default_value = calloc(1, sizeof(s64));
				p_elem->default_value =
				    (void *)((s64 *) atol(token));
				break;

			case DTH_TYPE_FLOAT:
				{
					float *ptr = NULL;
					p_elem->default_value =
					    calloc(1, sizeof(float));
					ptr = (float *)p_elem->default_value;
					*ptr = (float)atof(token);
				}
				break;

			default:

				p_elem->default_value = calloc(1, sizeof(u64));
				p_elem->default_value =
				    (void *)((u64 *) atol(token));
			}
		}

		SYSLOG(LOG_DEBUG,
		       "path:%s, type_Display:%i, type_Storage:%i, cols:%i, rows:%i, enum:%s, info:%s, min:%lld, max:%lld, user_data: %d \n",
		       p_elem->path, p_elem->type,
		       g_rf_params_type[vl_paramid], p_elem->cols,
		       p_elem->rows, p_elem->enumString,
		       p_elem->info, p_elem->min, p_elem->max,
		       p_elem->user_data);

		/* register element into DTH server */
		if (!TAT_OK(dth_register_element(p_elem))) {
			SYSLOG(LOG_ERR, "fail to register RF parameter %s",
			       p_elem->path);
			fclose(file);
			return -1;
		}

		p_elem++;
	}			/* end for each record in file */

	/* post condition : all allocated dth_elements should be initialized */
	if (nb_alloc_elements != nb_init_elements) {
		SYSLOG(LOG_WARNING,
		       "%d elements were allocated but %d were initialized.\n",
		       nb_alloc_elements, nb_init_elements);
	}

	fclose(file);

	return nb_init_elements;
}

void free_rf_params(void)
{
	if (g_nb_rf_params > 0) {
		free(g_rf_params);
		g_rf_params = NULL;
		g_nb_rf_params = 0;
	}

	if (g_nb_rf_params_type > 0) {
		free(g_rf_params_type);
		g_rf_params_type = NULL;
		g_nb_rf_params_type = 0;
	}
}
