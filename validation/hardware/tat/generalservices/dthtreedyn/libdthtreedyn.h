/*****************************************************************************/
/**
*  ST-Ericsson, 2011 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \file
* \brief   Dynamic library
* \author  ST-Ericsson
*/
/*****************************************************************************/

#define MAX_CHAR_PER_LINE 300
#define MAX_LOCAL_STRING_SIZE 256

typedef int (*ptf_get)(struct dth_element *, void *);
typedef int (*ptf_set)(struct dth_element *, void *);
typedef int (*ptf_exec)(struct dth_element *);

int dth_xml_init_service(struct dth_element *elem, char * filename, ptf_get get, ptf_set set, ptf_exec exec);

int get_type(char * token);
int get_user_data(char * token);
void get_info(char * token, char * info);
void get_min_max(char * buffer, long long int * min, long long int * max);
void get_enum_item(char * buffer, int enumcnt, char * enumStr);
int scanf_hexa (char * string_arg);

