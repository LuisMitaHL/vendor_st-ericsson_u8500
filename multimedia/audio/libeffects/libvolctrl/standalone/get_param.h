/*****************************************************************************/
/**
*  (c) ST-Ericsson, 2009 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \file   get_param.h
* \brief  
* \author ST-Ericsson
*/
/*****************************************************************************/
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
