/*########################################################################*/
/**************************************************************************/
/*                                                                        */
/*                    ALCATEL BUSINESS SYSTEMS FRANCE                     */
/*                                                                        */
/*                         CONFIDENTIAL CODE                              */
/*                                                                        */
/*                          ADVANCED STUDIES                              */
/*                                                                        */
/**************************************************************************/
/*########################################################################*/
   
/**************************************************************************/
/*C=C=C=C=C=C=C=C=C=C=C=C=C=C=C=C=C=C=C=C=C=C=C=C=C=C=C=C=C=C=C=C=C=C=C=C=*/
/*                                                                        */
/*            get_param.c                          Version: 1.0           */
/*                                                                        */
/*C=C=C=C=C=C=C=C=C=C=C=C=C=C=C=C=C=C=C=C=C=C=C=C=C=C=C=C=C=C=C=C=C=C=C=C=*/
/**************************************************************************/
/* Author : Laurent SAÏD                                                  */
/*          ECG/BDMP/MSD                                                  */
/* Started : february 16th, 2000                                           */
/**************************************************************************/
      
/*+++++++++++++++++ DESCRIPTION ++++++++++++++++++++++++++++++++++++++++++
All the parameter functions to be used as generic functions
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

/********************************************************/
/*	 integer		 HEADER FILES 			*/
/********************************************************/
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include "get_param.h"
#include "stdlib.h"


#define ON 1
#define OFF 0
#define END_OF_FIELD 2
#define END_OF_FILE	1


char get_name(char *line_string,char *line_index,char *name_string)
	{
	char cpt_name,end_status,space;
	
	space=OFF;
	end_status=0;
	cpt_name=0;
	while(!end_status)
		{
		switch(line_string[(short)*line_index])
			{
			case ':':
				name_string[(short)cpt_name]='\0';
				end_status = END_OF_FIELD;
				break;
			case '\n':
				name_string[(short)cpt_name]='\0';
				end_status = END_OF_FIELD;
				break;
			case '\r':
				name_string[(short)cpt_name]='\0';
				end_status = END_OF_FIELD;
				break;
			case '\0':
				// printf("Warning : empty item\n");
				end_status = END_OF_FILE;
				break;
			case ' ':
				space=ON;
				break;
			case '\t':
				space=ON;
				break;
			default:
				if(!space)
					{
					name_string[(short)cpt_name]=line_string[(short)*line_index];
					cpt_name++;
					}
				else
					{
					printf("Invalid name : containing space character\n");
					end_status = END_OF_FILE;
					}
				break;
			}
		if((*line_index)++>120) end_status = END_OF_FIELD;
		}
	if(end_status == END_OF_FIELD) end_status = 0;
	return(end_status);
	}

char get_value(char *line_string,char *line_index,char *value_string)
	{
	char cpt_val,end_status,space;
	
	space=OFF;
	end_status=0;
	cpt_val=0;
	while(!end_status)
		{
		switch(line_string[(short)*line_index])
			{
			case ';':
				value_string[(short)cpt_val]='\0';
				end_status = END_OF_FIELD;
				break;
			case '\n':
				value_string[(short)cpt_val]='\0';
				end_status = END_OF_FIELD;
				break;
			case '\r':
				value_string[(short)cpt_val]='\0';
				end_status = END_OF_FIELD;
				break;
			case '\0':
				end_status = END_OF_FIELD;
				break;
			case '\t':
			case ' ':
				if(cpt_val!=0) space = ON;
				break;
			case '.':
			case '-':
			case 'x':
			case 'X':
				value_string[(short)cpt_val]=line_string[(short)*line_index];
				cpt_val++;
				break;
			default:
				if(!space)
					{
					if((line_string[(short)*line_index]>='0' && line_string[(short)*line_index]<='9')||(line_string[(short)*line_index]>='a' && line_string[(short)*line_index]<='f')||(line_string[(short)*line_index]>='A' && line_string[(short)*line_index]<='F'))
						{
						value_string[(short)cpt_val]=line_string[(short)*line_index];
						cpt_val++;
						}
					else
						{
						printf("Invalid value : containing character which are not figures\n");
						end_status = END_OF_FILE;
						}
					break;
					}
				else
					{
					end_status = END_OF_FILE;
					printf("ERROR : Value containing space character!\n");
					}
			}
		if((*line_index)++>120) end_status = END_OF_FIELD;
		}
	if(end_status == END_OF_FIELD) end_status = 0;
	return(end_status);
	}

char get_string(char *line_string,char *line_index,char *strval_string)
	{
	char cpt_strval,end_status,space;
	
	end_status=0;
	cpt_strval=0;
	space=OFF;
	while(!end_status)
		{
		switch(line_string[(short)*line_index])
			{
			case ';':
				strval_string[(short)cpt_strval]='\0';
				end_status = END_OF_FIELD;
				break;
			case '\n':
				strval_string[(short)cpt_strval]='\0';
				end_status = END_OF_FIELD;
				break;
			case '\r':
				strval_string[(short)cpt_strval]='\0';
				end_status = END_OF_FIELD;
				break;
			case '\0':
				end_status = END_OF_FIELD;
				break;
			case ' ':
				if(cpt_strval!=0) space=ON;
				break;
			case '\t':
				if(cpt_strval!=0) space=ON;
				break;
			default:
				if(!space)
					{
					strval_string[(short)cpt_strval]=line_string[(short)*line_index];
					cpt_strval++;
					}
				else
					{
					printf("ERROR : Invalid string : containing space character\n");
					end_status = END_OF_FILE;
					}
				break;
			}
		if((*line_index)++>120) end_status = END_OF_FIELD;
		}
	if(end_status == END_OF_FIELD) end_status = 0;
	return(end_status);
	}

long get_param(FILE *file,char *label,char type,void *output, int mode)
	{
	/*
	if last arg is 't': enter step mode; next call to get_param starts when last one stopped.
	Useful when you have several mnemonic of the same type.
	Assumes you know the order of the different types of mnemonics.
	
	'l' option returns the whole line, in step mode.
	*/
//	long  pos,found=0;
	//char line[1024],cpt_char,end_of_file;
	char line[1024],cpt_char,end_of_file;
	long cpt_line;
	char name[60],value[60];
	char *error[2]={0};
	char *string;
	long *decimal;
	double *double_float;
	short   get_line=0;

	end_of_file = OFF;
	cpt_line=0;
	
	if (mode!=1)	
		{
		fseek(file,0,0);
		mode=0;
		}
	if (mode==2) get_line=1;
	
//	pos=ftell(file);
	while(!end_of_file && !(feof(file)))
		{
		cpt_line++;
		fgets(line,1023,file);
      if (feof(file))
         return -1;
		cpt_char=0;
		end_of_file=get_name(line,&cpt_char,name);
		// if(end_of_file) printf("Error line %ld : cannot get name!\n",cpt_line);
		if(end_of_file) return -1;
		if(!strcmp(name,label))
			{
			switch(type) 
				{
				case 'd':
					decimal =(long *)output;
					end_of_file=get_value(line,&cpt_char,value);
					*decimal=strtol(value,error,0);
					if(end_of_file) // printf("Error line %ld : cannot get value!\n",cpt_line);
                  cpt_line = -1;
					break;
				case 'f':
					double_float = (double *)output;
					end_of_file=get_value(line,&cpt_char,value);
					*double_float=strtod(value,error);
					if(end_of_file) // printf("Error line %ld : cannot get value!\n",cpt_line);
                  cpt_line = -1;
					break;
				case 's':
					string = (char *)output;
//					found=1;
					if (get_line==1)	strcpy(string, line);
					else
						{
						end_of_file=get_string(line,&cpt_char,string);
						}
					if(end_of_file) // printf("Error line %ld : cannot get string!\n",cpt_line);
                  cpt_line = -1;
					break;
				default:
					printf("SORRY but this format is not supported !\n use \'s\' for string, \'d\' for long, \'f\' for double .\n");					
				}
			end_of_file=ON;
			}
		}
	/*if (mode=='t' && found==0)
		{
		printf("fseek pos %i\n",pos);
		fseek(file,pos,0);
		}*/
    if (feof(file))
        cpt_line = -1;
    return(cpt_line);
}
