/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 *
 * Description: none.
 *
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef _get_param_h
#define _get_param_h

char get_name(char *line_string,char *line_index,char *name_string);
char get_value(char *line_string,char *line_index,char *value_string);
char get_string(char *line_string,char *line_index,char *strval_string);
long get_param(FILE *file,
			   char *label,
			   char type,
			   void *output,
			   ...);

#endif //_get_param_h
