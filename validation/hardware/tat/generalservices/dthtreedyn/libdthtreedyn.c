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

#include <dthsrvhelper/dthsrvhelper.h>
#include "libdthtreedyn.h"
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#define TOKEN_OFFSET 11
int debugcnt = 0;
int firsttime = 0;

int dth_xml_init_service(struct dth_element *elem, char * filename, ptf_get get, ptf_set set, ptf_exec exec)
{
	int nb = 0, inAction = 0, inBFC = 0, inBF = 0, inMenu = 0, checkEnum = 0, enumcnt = 0, getArgs = 0;
	FILE *file = NULL;
	char *token = NULL;
	char * myenumString;
	char * myinfoString;
	struct dth_element *p_elements;
	char s_buffer[MAX_CHAR_PER_LINE];
#ifdef SMF
	FILE * fp=NULL;
	char version[100];
#endif
	memset (s_buffer,0,MAX_CHAR_PER_LINE);
	p_elements = elem;
	myenumString = (char *) malloc (MAX_CHAR_PER_LINE * sizeof(char));
	myinfoString = (char *) malloc (MAX_CHAR_PER_LINE * sizeof(char));

	memset(myenumString,0,MAX_CHAR_PER_LINE);
	memset(myinfoString,0,MAX_CHAR_PER_LINE);
	printf("Get elements from %s\n", filename);

	/* Jump Version element */
	p_elements++;
	nb++;

	if ((file = fopen(filename, "r")) == NULL)
        {
            printf("fail to open file %s \n", filename);
            return -1;
        }
	else
	{
		debugcnt = 0;
		printf("Opened %s\n", filename);
#ifdef SMF
		fp = popen("cat /proc/meminfo | grep MemFree | awk '{print $2}'", "r");
		fgets(version, sizeof(version), fp);
		printf ("MemFree : %s\n", version);
		pclose(fp);
#endif
		while (!feof(file))
		{
			//if (debugcnt < 243) printf("XXXXX%02d", debugcnt);
			debugcnt++;
			memset(s_buffer, 0, MAX_CHAR_PER_LINE) ;
			//if (debugcnt < 243) printf(": ");
			fgets (s_buffer, MAX_CHAR_PER_LINE, file);
			//if (debugcnt < 243) printf("%s\n", s_buffer);
			if (( strncmp(s_buffer, "<argument", 9) == 0) && (inMenu == 0) && (inAction == 0))
			{
				inMenu = 1;
				getArgs = 1;
				//printf ("Start menu %d\n", nb);
			}
			if (( strncmp(s_buffer, "<action", 7) == 0) && (inAction == 0) && (inMenu == 0))
			{
				inAction = 1;
				getArgs = 1;
				//printf ("Start action %d\n", nb);
			}
			if (( strncmp(s_buffer, "<argument", 9) == 0) && (inAction == 1) && (inBFC == 0)) 
			{
				inBFC = 1;
				getArgs = 1;
				//printf ("Start BFC\n");
			}
			else if (( strncmp(s_buffer, "<argument", 9) == 0) && (inAction == 1) && (inBFC == 1) && (inBF == 0)) 
			{
				inBF = 1;
				getArgs = 1;
				//printf ("Start BF\n");
			}
			if (getArgs == 1)
			{
				//printf ("Read arguments\n");
				getArgs = 0;
				token = strtok(s_buffer, " ");
				while (token != NULL)
				{
					if ( strncmp(token, "path=\"", 6) == 0)
					{
						token+=6;
						strncpy(p_elements->path, token, strlen(token)-1);
						//printf("PPPPP path = %s\n", p_elements->path);
					}
					if ( strncmp(token, "type=\"", 6) == 0)
					{
						p_elements->type = get_type(token);
						//printf("PPPPP type = %d\n", p_elements->type);
					}
					if ( strncmp(token, "user_data=\"", 11) == 0)
					{
						if ((inBF == 1) || (inBFC == 1) || (inMenu == 1)) p_elements->user_data = 1;
						else p_elements->user_data = get_user_data(token); /* Action element */
						//printf("PPPPP user_data = %d\n", p_elements->user_data);
					}
					if ( strncmp(token, "get=\"", 5) == 0)
					{
						p_elements->get = get;
					}
					if ( strncmp(token, "set=\"", 5) == 0)
					{
						p_elements->set = set;
					}
					if ( strncmp(token, "exec=\"", 6) == 0)
					{
						p_elements->exec = exec;
					}
					token = strtok(NULL," ");
				}
			}
			if (( strncmp(s_buffer, "<range", 6) == 0) && ((inAction == 1) || (inBFC == 1) || (inBF == 1))) 
			{
				get_min_max(s_buffer, &(p_elements->min), &(p_elements->max));
				p_elements->enumString = NULL;
						}
			if (( strncmp(s_buffer, "<info>", 6) == 0) && ((inAction == 1) || (inBFC == 1) || (inBF == 1) || (inMenu == 1))) 
			{
				token = strtok(s_buffer, "<>");
				token = strtok(NULL,"<>");
				myinfoString[0]='\0';
				get_info(token, myinfoString);
				strcat(p_elements->info, myinfoString);
				p_elements++;
				nb++;
				memset(myinfoString, 0, sizeof(myinfoString));
			}
			if (( strncmp(s_buffer, "<enum>", 6) == 0) && ((inAction == 1) || (inBF == 1))) 
			{
				checkEnum = 1;
				enumcnt = 0;
			}
			if (( strncmp(s_buffer, "</enum>", 7) == 0) && ((inAction == 1) || (inBF == 1))) 
			{
				checkEnum = 0;
			}
			if (( strncmp(s_buffer, "<enum_item", 10) == 0) && (checkEnum == 1)) 
			{
				myenumString[0]='\0';
				get_enum_item(s_buffer, enumcnt, myenumString);
				strcat(p_elements->enumString, myenumString);
				//memset(myenumString, 0, sizeof(myenumString));
				enumcnt++;
			}
			if (( strncmp(s_buffer, "</action>", 9) == 0))
			{
				inAction = 0;
				//printf ("End action\n");
			}
			if (( strncmp(s_buffer, "</argument>", 11) == 0) && (inMenu == 1) && (inAction == 0)) 
			{
				inMenu = 0;
				//printf ("End Menu\n");
			}
			if (( strncmp(s_buffer, "</argument>", 11) == 0) && (inBFC == 1) && (inBF == 0)) 
			{
				inBFC = 0;
				//printf ("End BFC\n");
			}
			if (( strncmp(s_buffer, "</argument>", 11) == 0) && (inBFC == 1) && (inBF == 1)) 
			{
				inBF = 0;
				//printf ("End BF\n");
			}
		}
	}
	fclose(file);
	printf("Closed %s\n", filename);
	/* Create NULL element */
	p_elements->path = NULL;
	p_elements->enumString = NULL;
	p_elements->info = NULL;
	//p_elements++;
	nb++;

	if (myenumString != NULL) free(myenumString);
	if (myinfoString != NULL) free(myinfoString);

	return nb;
}

int get_user_data(char * token)
{
	int ud, realval=-1, lenvalue, i;
	token+=TOKEN_OFFSET; /* skip user_data=" */
	char val[10] = "";
	lenvalue = strlen(token);
	for (i = 0; i < lenvalue; i++)
		if (token[i] == ':') realval = i+1;
	if (realval > -1)
	{
		strncpy(val, token+realval,  lenvalue-realval);
		/*lenvalue = lenvalue - realval;
		for (i = 0; i < lenvalue; i++)
			val[i] = val[i+realval];*/
	}
	else strcpy(val, "2");
	ud = scanf_hexa(val);
	return ud;
}

void get_info(char * token, char * info)
{
	char * pch, * old_pch;

	pch = strstr(token, "\\n");
	old_pch = token;
	if (pch == NULL) return;

	while (pch != NULL)
	{
		strncat(info, old_pch, pch-old_pch);
		strcat(info, "\n");
		old_pch = pch+2;
		pch = strstr(pch+2, "\\n");
	}
	strncat(info, old_pch, pch-old_pch);
}

int get_type(char * token)
{
	token+=6; /* skip type=" */
	if (strncmp(token, "FILE", 4) == 0) return DTH_TYPE_FILE;

	if (strncmp(token, "U8B", 3) == 0) return DTH_TYPE_U8B;
	if (strncmp(token, "U16B", 4) == 0) return DTH_TYPE_U16B;
	if (strncmp(token, "U32B", 4) == 0) return DTH_TYPE_U32B;
	if (strncmp(token, "U64B", 4) == 0) return DTH_TYPE_U64B;

	if (strncmp(token, "U8", 2) == 0) return DTH_TYPE_U8;
	if (strncmp(token, "U16", 3) == 0) return DTH_TYPE_U16;
	if (strncmp(token, "U32", 3) == 0) return DTH_TYPE_U32;
	if (strncmp(token, "U64", 3) == 0) return DTH_TYPE_U64;

	if (strncmp(token, "BITFIELD_1_bits", 15) == 0) return DTH_BITFIELD_1_bits;
	if (strncmp(token, "BITFIELD_2_bits", 15) == 0) return DTH_BITFIELD_2_bits;
	if (strncmp(token, "BITFIELD_3_bits", 15) == 0) return DTH_BITFIELD_3_bits;
	if (strncmp(token, "BITFIELD_4_bits", 15) == 0) return DTH_BITFIELD_4_bits;
	if (strncmp(token, "BITFIELD_5_bits", 15) == 0) return DTH_BITFIELD_5_bits;
	if (strncmp(token, "BITFIELD_6_bits", 15) == 0) return DTH_BITFIELD_6_bits;
	if (strncmp(token, "BITFIELD_7_bits", 15) == 0) return DTH_BITFIELD_7_bits;
	if (strncmp(token, "BITFIELD_8_bits", 15) == 0) return DTH_BITFIELD_8_bits;
	if (strncmp(token, "BITFIELD_9_bits", 15) == 0) return DTH_BITFIELD_9_bits;
	if (strncmp(token, "BITFIELD_10_bits", 16) == 0) return DTH_BITFIELD_10_bits;
	if (strncmp(token, "BITFIELD_11_bits", 16) == 0) return DTH_BITFIELD_11_bits;
	if (strncmp(token, "BITFIELD_12_bits", 16) == 0) return DTH_BITFIELD_12_bits;
	if (strncmp(token, "BITFIELD_13_bits", 16) == 0) return DTH_BITFIELD_13_bits;
	if (strncmp(token, "BITFIELD_14_bits", 16) == 0) return DTH_BITFIELD_14_bits;
	if (strncmp(token, "BITFIELD_15_bits", 16) == 0) return DTH_BITFIELD_15_bits;
	if (strncmp(token, "BITFIELD_16_bits", 16) == 0) return DTH_BITFIELD_16_bits;
	if (strncmp(token, "BITFIELD_17_bits", 16) == 0) return DTH_BITFIELD_17_bits;
	if (strncmp(token, "BITFIELD_18_bits", 16) == 0) return DTH_BITFIELD_18_bits;
	if (strncmp(token, "BITFIELD_19_bits", 16) == 0) return DTH_BITFIELD_19_bits;
	if (strncmp(token, "BITFIELD_20_bits", 16) == 0) return DTH_BITFIELD_20_bits;
	if (strncmp(token, "BITFIELD_21_bits", 16) == 0) return DTH_BITFIELD_21_bits;
	if (strncmp(token, "BITFIELD_22_bits", 16) == 0) return DTH_BITFIELD_22_bits;
	if (strncmp(token, "BITFIELD_23_bits", 16) == 0) return DTH_BITFIELD_23_bits;
	if (strncmp(token, "BITFIELD_24_bits", 16) == 0) return DTH_BITFIELD_24_bits;
	if (strncmp(token, "BITFIELD_25_bits", 16) == 0) return DTH_BITFIELD_25_bits;
	if (strncmp(token, "BITFIELD_26_bits", 16) == 0) return DTH_BITFIELD_26_bits;
	if (strncmp(token, "BITFIELD_27_bits", 16) == 0) return DTH_BITFIELD_27_bits;
	if (strncmp(token, "BITFIELD_28_bits", 16) == 0) return DTH_BITFIELD_28_bits;
	if (strncmp(token, "BITFIELD_29_bits", 16) == 0) return DTH_BITFIELD_29_bits;
	if (strncmp(token, "BITFIELD_30_bits", 16) == 0) return DTH_BITFIELD_30_bits;
	if (strncmp(token, "BITFIELD_31_bits", 16) == 0) return DTH_BITFIELD_31_bits;
	if (strncmp(token, "BITFIELD_32_bits", 16) == 0) return DTH_BITFIELD_32_bits;
	if (strncmp(token, "BITFIELD_33_bits", 16) == 0) return DTH_BITFIELD_33_bits;
	if (strncmp(token, "BITFIELD_34_bits", 16) == 0) return DTH_BITFIELD_34_bits;
	if (strncmp(token, "BITFIELD_35_bits", 16) == 0) return DTH_BITFIELD_35_bits;
	if (strncmp(token, "BITFIELD_36_bits", 16) == 0) return DTH_BITFIELD_36_bits;
	if (strncmp(token, "BITFIELD_37_bits", 16) == 0) return DTH_BITFIELD_37_bits;
	if (strncmp(token, "BITFIELD_38_bits", 16) == 0) return DTH_BITFIELD_38_bits;
	if (strncmp(token, "BITFIELD_39_bits", 16) == 0) return DTH_BITFIELD_39_bits;
	if (strncmp(token, "BITFIELD_40_bits", 16) == 0) return DTH_BITFIELD_40_bits;
	if (strncmp(token, "BITFIELD_41_bits", 16) == 0) return DTH_BITFIELD_41_bits;
	if (strncmp(token, "BITFIELD_42_bits", 16) == 0) return DTH_BITFIELD_42_bits;
	if (strncmp(token, "BITFIELD_43_bits", 16) == 0) return DTH_BITFIELD_43_bits;
	if (strncmp(token, "BITFIELD_44_bits", 16) == 0) return DTH_BITFIELD_44_bits;
	if (strncmp(token, "BITFIELD_45_bits", 16) == 0) return DTH_BITFIELD_45_bits;
	if (strncmp(token, "BITFIELD_46_bits", 16) == 0) return DTH_BITFIELD_46_bits;
	if (strncmp(token, "BITFIELD_47_bits", 16) == 0) return DTH_BITFIELD_47_bits;
	if (strncmp(token, "BITFIELD_48_bits", 16) == 0) return DTH_BITFIELD_48_bits;
	if (strncmp(token, "BITFIELD_49_bits", 16) == 0) return DTH_BITFIELD_49_bits;
	if (strncmp(token, "BITFIELD_50_bits", 16) == 0) return DTH_BITFIELD_50_bits;
	if (strncmp(token, "BITFIELD_51_bits", 16) == 0) return DTH_BITFIELD_51_bits;
	if (strncmp(token, "BITFIELD_52_bits", 16) == 0) return DTH_BITFIELD_52_bits;
	if (strncmp(token, "BITFIELD_53_bits", 16) == 0) return DTH_BITFIELD_53_bits;
	if (strncmp(token, "BITFIELD_54_bits", 16) == 0) return DTH_BITFIELD_54_bits;
	if (strncmp(token, "BITFIELD_55_bits", 16) == 0) return DTH_BITFIELD_55_bits;
	if (strncmp(token, "BITFIELD_56_bits", 16) == 0) return DTH_BITFIELD_56_bits;
	if (strncmp(token, "BITFIELD_57_bits", 16) == 0) return DTH_BITFIELD_57_bits;
	if (strncmp(token, "BITFIELD_58_bits", 16) == 0) return DTH_BITFIELD_58_bits;
	if (strncmp(token, "BITFIELD_59_bits", 16) == 0) return DTH_BITFIELD_59_bits;
	if (strncmp(token, "BITFIELD_60_bits", 16) == 0) return DTH_BITFIELD_60_bits;
	if (strncmp(token, "BITFIELD_61_bits", 16) == 0) return DTH_BITFIELD_61_bits;
	if (strncmp(token, "BITFIELD_62_bits", 16) == 0) return DTH_BITFIELD_62_bits;
	if (strncmp(token, "BITFIELD_63_bits", 16) == 0) return DTH_BITFIELD_63_bits;
	if (strncmp(token, "BITFIELD_64_bits", 16) == 0) return DTH_BITFIELD_64_bits;

	return -1;
}

void get_min_max(char * buffer, long long int * min, long long int * max)
{
	char * token;
	token = strtok(buffer, "\""); /*<range min=*/
	token = strtok(NULL, "\""); /*?*/
	*min = atoll(token);
	token = strtok(NULL, "\""); /* max=*/
	token = strtok(NULL, "\""); /*?*/
	*max = atoll(token);
}

void get_enum_item(char * buffer, int enumcnt, char * enumStr)
{
	char * token;
	char val[MAX_CHAR_PER_LINE] = "";
	int lenvalue = 1, realval = -1, i = 0;

	memset(val,0,MAX_CHAR_PER_LINE);
	if ( strncmp(buffer, "<enum_item value", 16) == 0)
	{
		token = strtok(buffer, "<>\""); /*enum_item value=*/
		token = strtok(NULL, "<>\""); /* ?:? or just ? */
		lenvalue = strlen(token);
		strncpy(val, token, lenvalue);
		token = strtok(NULL, "<>\""); /*"*/
	}
	else
	{
		token = strtok(buffer, "<>"); /*<enum_item*/
		token = strtok(NULL, "<>"); /*???????*/
		if (enumcnt > 9) lenvalue = 2;
		sprintf(val, "%d", enumcnt);
	}
	strcat(enumStr, token);
	strcat(enumStr, "\t");

	for (i = 0; i < lenvalue; i++)
		if (val[i] == ':') realval = i+1;
	if (realval > -1)
	{
		lenvalue = lenvalue - realval;
		for (i = 0; i < lenvalue; i++)
			val[i] = val[i+realval];
	}
	strncat(enumStr, val, lenvalue);
	strcat(enumStr, "\n");

}

int HexaToDecimal(char* sHexa)
{
	int ret = 0, t = 0, n = 0;
	char *c = sHexa;
	while (*c && (n < 8)) /* 32bit and less */
	{
		if ((*c >= '0') && (*c <= '9')) t = (*c - '0');
		else if((*c >= 'A') && (*c <= 'F')) t = (*c - 'A' + 10);
		else if((*c >= 'a') && (*c <= 'f')) t = (*c - 'a' + 10);
		else break;
		n++;
		ret *= 16;
		ret += t;
		c++;
	}
	return ret;
}


int scanf_hexa (char * string_arg)
{
	int result = 0;
	if (string_arg[0] == '0' && (string_arg[1] == 'x' || string_arg[1] == 'X'))
		result = HexaToDecimal(string_arg+2);
	else
		result = HexaToDecimal(string_arg);
	return result;
}

