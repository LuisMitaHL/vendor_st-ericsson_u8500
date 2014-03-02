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

#include <toc.h>
#include <crc32.h>
#include <malloc.h>
#include <stdio.h>
#include <string.h>
#include "bass_app.h"
#include "tiny_env.h"
#include "partition_parser.h"

/* headpointer for all env variables */
envNode* head;
envNode* tail;

/* head pointer for the current sub string */
subStringNode* headString;

/* Security data */
bool was_env_signed;
bool was_env_loaded;


void tenv_init(void)
{
	head = NULL;
	tail = NULL;

	headString = NULL;

	init_crc32();

#if 0
	create_env("tmp1", "1", true);
	create_env("tmp2", "2", true);
	create_env("tmp3", "3", true);
	create_env("tmp4", "4", true);
#endif
}

envNode* find_env(char* name)
{
	envNode* tmp;
	tmp = head;

	while (tmp != NULL)
	{
		if (strcmp(tmp->name,name) == 0)
			return tmp;
		else
			tmp = tmp->next;
	}
	return NULL;
}

char* tenv_getval(char* name)
{
	envNode *node;

	node = find_env(name);
	if (node == NULL)
		return NULL;

	return node->value;
}

void free_sub_strings(subStringNode* startString)
{
	subStringNode *nextSubString, *currSubString;
	currSubString = startString;
	while (currSubString != NULL)
	{
		nextSubString = currSubString->next;
		free(currSubString);
		currSubString = nextSubString;
	}
	headString = NULL;
}

subStringNode* get_sub_strings(char *start)
{
	char *tokenPtr, *startTokenPtr, *endTokenPtr;
	int envFound = false;
	subStringNode *tmpString_node, *prevSubString_node=NULL;

	//printf("getSubString entered\n");

	if (headString != NULL)	/* Make sure that list is deleted before creating a new one */
		return NULL;
	if (*start == '\0')     /* return if empty string */
		return NULL;

	startTokenPtr = start;
	endTokenPtr = start-1;
	tokenPtr = strpbrk(startTokenPtr,"$'\\");

	while (tokenPtr != NULL)
	{
		if ((endTokenPtr + 1) != tokenPtr)
		{
			/* Add string between values */
			tmpString_node = (subStringNode*)malloc(sizeof(subStringNode));
			tmpString_node->start = endTokenPtr+1;
			tmpString_node->end = tokenPtr-1;
			tmpString_node-> isEnv = false;
			tmpString_node->next = NULL;
			if (headString == NULL)
				headString = tmpString_node;
		 	else
				prevSubString_node->next = tmpString_node;
			prevSubString_node = tmpString_node;
			//printf("Subsub string found\n");
		}
		//printf("Sub string found\n");
		if (*tokenPtr == '$')
		{
			startTokenPtr = tokenPtr + 1;
			envFound = true;
			if (*(startTokenPtr) == '{')
			{
				startTokenPtr++;
				endTokenPtr = strchr(startTokenPtr,'}');
				if (endTokenPtr == NULL)
				{
					free_sub_strings(headString);
					return NULL;
				}
			}
			else
			{
				endTokenPtr = strpbrk(startTokenPtr,"$ ");
				if (endTokenPtr == NULL)
					endTokenPtr = startTokenPtr+strlen(startTokenPtr);
			}
			endTokenPtr--;
			tokenPtr = strpbrk(endTokenPtr+1,"$'\\");
		}
		else if (*tokenPtr == '\'')
		{
			startTokenPtr = tokenPtr+1;
			endTokenPtr = strchr(startTokenPtr,'\'');
			if (endTokenPtr == NULL)
			{
				free_sub_strings(headString);
				return NULL;
			}
			else
			{
				endTokenPtr--;
				tokenPtr = strpbrk(endTokenPtr+2,"$'\\");
			}
		}
		else  /* if backslash */
		{

		}
		tmpString_node = (subStringNode*)malloc(sizeof(subStringNode));
		tmpString_node->start = startTokenPtr;
		tmpString_node->end = endTokenPtr;
		tmpString_node-> isEnv = envFound;
		tmpString_node->next = NULL;
		if (headString == NULL)
		{
			headString = tmpString_node;
			prevSubString_node = headString;
		}
		else
			prevSubString_node->next = tmpString_node;
		prevSubString_node = tmpString_node;

		if (*(endTokenPtr + 1) == '\'' || *(endTokenPtr + 1 ) == '}')
			endTokenPtr++;
		envFound = false;

	}
	if (endTokenPtr != (startTokenPtr+strlen(startTokenPtr) - 1 ))
	{
		//printf("Last sub string found\n");
		tmpString_node = (subStringNode*)malloc(sizeof(subStringNode));
		tmpString_node->start = endTokenPtr+1;
		tmpString_node->end = startTokenPtr+strlen(startTokenPtr)-1;
		tmpString_node-> isEnv = false;
		tmpString_node->next = NULL;
		if (headString == NULL)
		{
			headString = tmpString_node;
			prevSubString_node = headString;
		}
		else
			prevSubString_node->next = tmpString_node;
		prevSubString_node = tmpString_node;

	}

	return headString;
}

void extractData(subStringNode* start, int dataType, void* data)
{
	subStringNode *tmpString_node;
	int offset = 0;
	*(int*)data = 0;
	tmpString_node = headString;
	while (tmpString_node != NULL)
	{
		if (tmpString_node->isEnv)
		{
			char tmpChar = *(tmpString_node->end+1);
			char *tmpName;
			*(tmpString_node->end+1) = '\0';
			tmpName = tenv_getval(tmpString_node->start);
			if (tmpName != NULL)
			{
				if (dataType == GET_LENGTH)
				{
					//printf("calc strlen:%i\n",strlen(tmpName));
					*(int*)data += strlen(tmpName);
				}
				else if (dataType == GET_STRING)
				{
					strncpy((char*)data + offset,tmpName,strlen(tmpName));
					offset += strlen(tmpName);
				}
			}
			*(tmpString_node->end+1) = tmpChar;
		}
		else
		{
			if (dataType == GET_LENGTH)
			{
				*(int*)data += tmpString_node->end-tmpString_node->start+1;
				//printf("calc strlen:%i\n",tmpString_node->end-tmpString_node->start+1);
			}
			else if (dataType == GET_STRING)
			{
				int strlength = tmpString_node->end-tmpString_node->start+1;
				strncpy((char*)data + offset,tmpString_node->start,strlength);
				offset += strlength;
			}
		}
		tmpString_node = tmpString_node->next;
	}
}

char* tenv_eval_env(char* env_string)
{
	int stringLength;
	char *evalVal;
	subStringNode *tmpStrings;

	tmpStrings = get_sub_strings(env_string);
	extractData(tmpStrings,GET_LENGTH,(void*)&stringLength);
	evalVal = (char*)calloc(stringLength + 1,1);
	extractData(tmpStrings,GET_STRING,(void*)evalVal);
	free_sub_strings(tmpStrings);
	return evalVal;
}

int create_env(char *name, char *val, int evalIt)
{
	envNode* tmp;
	char *evalVal;

	evalVal = tenv_eval_env(val);
	if (evalIt)	/* If manually input is done */
	{
		/* Check if it already exists */
		tmp = find_env(name);
		if (tmp != NULL)  /* It exists */
		{
			if (strlen(val) == 0)  /* should it be deleted? */
			{
				//printf("Value deleted\n");
				free(tmp->value);
				free(tmp->name);
				if (tmp->prev == NULL)
					head = tmp->next;
				else
					tmp->prev->next = tmp->next;
				if (tmp->next == NULL)
					tail = tmp->prev;
				else
					tmp->next->prev = tmp->prev;
				free(tmp);
			}
			else  /* New value entered */
			{
				//printf("New value enterd\n");
				free(tmp->value);
				tmp->value = evalVal;
			}
		}
		else if (strlen(val) != 0)   /* New variable */
		{
			tmp = (envNode*)malloc(sizeof(envNode));
			//printf("ENV created\n");

			tmp->name = (char*)malloc(strlen(name)+1);
			strcpy(tmp->name,name);
			tmp->value = evalVal;

			if (head == NULL)
			{
				head = tmp;
				tail = tmp;
				tmp->next = NULL;
				tmp->prev = NULL;
			}
			else
			{
				tmp->prev = tail;
				tmp->next = NULL;
				tail->next = tmp;
				tail = tmp;
			}
		}
	}
	else  /* tenv_eval alloocates memory for the string */
	{	/* When read from EMMC */
		evalVal = (char*)calloc(strlen(val) + 1, 1);
		strcpy(evalVal,val);
		tmp = find_env(name);
		if (tmp != NULL)
		{
			free(tmp->value);
			tmp->value = evalVal;

		}
		else
		{
			tmp = (envNode*)malloc(sizeof(envNode));
			//printf("ENV created\n");

			tmp->name = (char*)malloc(strlen(name)+1);
			strcpy(tmp->name,name);
			tmp->value = evalVal;

			if (head == NULL)
			{
				head = tmp;
				tail = tmp;
				tmp->next = NULL;
				tmp->prev = NULL;
			}
			else
			{
				tmp->prev = tail;
				tmp->next = NULL;
				tail->next = tmp;
				tail = tmp;
			}

		}
	}
	return 0;
}

int tenv_save(void)
{
	char* savedata=NULL;
	envNode *currEnv;
	currEnv = head;
	int offset = 4; /* Save 4 byte for crc32 */
	int env_size = 4; /* Save 4 byte for crc32 */
	int sizeOfEnvName;
	int sizeOfEnvValue;

	/* Find out the size of env data */
	currEnv = head;
	while (currEnv != NULL)
	{
		sizeOfEnvName = strlen(currEnv->name);
		sizeOfEnvValue = strlen(currEnv->value);

		env_size += sizeOfEnvName + 1; // 1 is for '='
		env_size += sizeOfEnvValue + 1; // 1 is for '\0'
		currEnv = currEnv->next;
	}
	env_size++; // for the final '\0'
	dprintf(INFO, "env_size is 0x%04X\n", env_size);
	if (env_size > (int) get_partition_size(ENV_NAME)) {
		dprintf(CRITICAL, "tenv_save new env size %04X > partition %04X\n",
				env_size, get_partition_size(ENV_NAME));
		return -1;
	}

	/* Setup the binary env data */
	savedata = (char*)calloc(env_size,1);
	if (NULL == savedata) {
		dprintf(CRITICAL, "tenv_save could not allocate data\n");
		return -1;
	}
	currEnv = head;
	while (currEnv != NULL)
	{
		sizeOfEnvName = strlen(currEnv->name);
		sizeOfEnvValue = strlen(currEnv->value);

		strcpy(savedata+offset,currEnv->name);
		offset += sizeOfEnvName;
		savedata[offset++] = '=';
		strcpy(savedata+offset,currEnv->value);
		offset += sizeOfEnvValue;
		savedata[offset++] = '\0';
		currEnv = currEnv->next;
	}
	/* Do the saving to MMC and then free up savedata */
	printf("crc32 of savedata:0x%X\n",calc_crc32((uint8_t*)(savedata+4),env_size-4));
	*(uint32_t*)savedata = calc_crc32((uint8_t*)(savedata+4),env_size-4);
	toc_save_id(ENV_NAME, (uint32_t)savedata, -1);	/* copy image into RAM */
	toc_update(0, ENV_NAME, env_size);
	free(savedata);
	return 0;
}

int tenv_load(void)
{
	char *env_data;
	struct toc_entry *toc_entry;
	char *startOfStr, *separator;
	bass_return_code sec_result;
	bass_signed_header_info_t sec_hdr_info;
	struct toc_entry *toc_list;
	bass_payload_type_t pltype = BASS_PL_TYPE_APE_NW_CODE;

	toc_list = toc_addr_get();
	toc_entry = toc_find_id(toc_list, ENV_NAME);
	if (toc_entry == (struct toc_entry *)~0) {
		printf("tenv_load: %s image not found\n", ENV_NAME);
		was_env_loaded = false;
		return -1;
	}

	env_data = (char*)calloc(((toc_entry->size / 512) + 1) * 512,1);
	toc_entry = toc_load_id(ENV_NAME, (uint32_t)env_data);	/* copy image into RAM */

	if (toc_entry == (struct toc_entry *)~0) {
		printf("%s not found\n", ENV_NAME);
		free(env_data);
		was_env_loaded = false;
		return -1;
	}

#if 0
	/* Test to manipulate security signed header */
	env_data[48] ^= 0x01;
#endif

	/* Make the security verifications */
	sec_result = verify_signedheader
				 ((uint8_t*) env_data,
				 toc_entry->size,
				 pltype,
				 &sec_hdr_info);
	if (BASS_RC_SUCCESS != sec_result) {
		dprintf(CRITICAL, "tenv_load: verify_signedheader failed, returned %d\n",
				(int) sec_result);
		free(env_data);
		was_env_loaded = false;
		was_env_signed = true;
		return -1;
	}
	was_env_signed = sec_hdr_info.size_of_signed_header > 0x00;
	dprintf(INFO, "size_of_signed_header %08X and signed = %d\n",
			sec_hdr_info.size_of_signed_header, was_env_signed);


#if 0
	/* Test to manipulate security signed payload */
	env_data[sec_hdr_info.size_of_signed_header + 5] ^= 0x01;
#endif

	sec_result = bass_check_payload_hash
				 (&sec_hdr_info.ht, &pltype,
				 (void*) ((uint32_t)env_data + sec_hdr_info.size_of_signed_header),
				 sec_hdr_info.pl_size,
				 (void*) sec_hdr_info.ehash.value,
				 SHA256_HASH_SIZE);
	if (BASS_RC_SUCCESS != sec_result) {
		dprintf(CRITICAL, "tenv_load: bass_check_payload_hash failed, returned %d\n",
				(int) sec_result);
		free(env_data);
		was_env_loaded = false;
		was_env_signed = true;
		return -1;
	}

	/* Check if CRC32 match? */
	if (*(uint32_t*)(env_data + sec_hdr_info.size_of_signed_header) ==
		calc_crc32((uint8_t*)(env_data + sec_hdr_info.size_of_signed_header + 4),
		(toc_entry->size - sec_hdr_info.size_of_signed_header - 4)))
	{
		startOfStr = env_data + sec_hdr_info.size_of_signed_header + 4;
		if (startOfStr != NULL && strlen(startOfStr) != 0)
		{
			separator = strchr(startOfStr,'=');
			*separator = '\0';
			separator++;
			while ( 1 )
			{
				create_env(startOfStr, separator, false);
				startOfStr = strchr(separator,'\0') + 1;
				if (*startOfStr == '\0' )
					break;
				separator = strchr(startOfStr,'=');
				*separator = '\0';
				separator++;
			}
		}
	}
	else {
		printf("CRC doesn't match!\n");
		was_env_loaded = false;
		free(env_data);
		return -1;
	}
	free(env_data);
	was_env_loaded = true;
	return 0;
}

void print_env(void)
{
	envNode* tmp;
	tmp = head;
	while (tmp != NULL)
	{
		printf("%s=%s\n",tmp->name,tmp->value);
		tmp = tmp->next;
	}
}


int tenv_remove_env (char *name)
{
	envNode* iter;
	envNode* tmp;

	iter = head;
	tmp = NULL;
	while (iter != NULL)
	{
		if (0 == strcmp(name, iter->name)) {
			if (iter == head) {
				head = iter->next;
				head->prev = NULL;
			} else {
				tmp = iter;
				tmp->next = iter->next;
				tmp = iter->next;
				if (tmp)
					tmp->prev = iter->prev;
				else
					tail = iter->prev;
			}

			free(iter->name);
			free(iter->value);
			free (iter);
		}
		iter = iter->next;
	}

	return 0;
}


char *tenv_env_to_cmdline ()
{
	envNode		*tmp;
	size_t		len;
	char		*cmdline = NULL;

	if (NULL == head)		/* No valid command line */
		return cmdline;

	tmp = head;
	len = 0;
	while (tmp != NULL)
	{
		/* length of name and value plus '=' and trailing space */
		len += strlen (tmp->name) + strlen(tmp->value) + 2;
		tmp = tmp->next;
	}
	len += 1; /* Room for terminating NULL */

	cmdline = (char*)malloc (len);

	if (cmdline) {
		cmdline[0] = 0;
		tmp = head;
		while (tmp != NULL)
		{
			/* Exclude "memargs" and "ddrdie", that lines contain sub pairs, name not needed */
			if (strcmp("memargs", tmp->name) && strcmp("ddrdie", tmp->name)) {
				strcat (cmdline, tmp->name);
				strcat (cmdline, "=");
			}
			strcat (cmdline, tmp->value);
			tmp = tmp->next;
			if (tmp)
				strcat (cmdline, " ");
		}
	}

	return cmdline;
}


bool is_env_signed()
{
	return was_env_signed;
}


bool is_env_loaded()
{
	return was_env_loaded;
}


#if !defined(ENABLE_FEATURE_BUILD_HBTS)
#if WITH_LIB_CONSOLE
#include <lib/console.h>

static int cmd_printenv (int argc, const cmd_args *argv);
static int cmd_setenv (int argc, const cmd_args *argv);
static int cmd_saveenv (int argc, const cmd_args *argv);
static int cmd_loadenv (int argc, const cmd_args *argv);


STATIC_COMMAND_START
        { "printenv", "printenv", &cmd_printenv},
        { "setenv", "setenv <name> <val>", &cmd_setenv},
        { "saveenv", "saveenv", &cmd_saveenv},
        { "loadenv", "loadenv", &cmd_loadenv},
STATIC_COMMAND_END(env);

static int cmd_printenv(int argc, const cmd_args *argv)
{
	char *p;

	printf("ENV:\n");
	print_env();

	p = tenv_env_to_cmdline();
	printf ("\ncmdline %s\n", p);

	if (p)
		free (p);

	return 0;
}

int cmd_setenv(int argc, const cmd_args *argv)
{
	if (was_env_signed) {
		printf("\nNot allowed when env is signed\n");
		return 0;
	}
	if (!was_env_loaded) {
		printf("\nNot allowed when env are not loaded\n");
		return 0;
	}
	if (argc == 1)
		return 0;
	else if (argc == 2)
		create_env((char*)(argv[1].str),"", true);
	else if (argc == 3)
		create_env((char*)(argv[1].str),(char*)(argv[2].str), true);
	else
	{
		int total_length = 0;
		int i;
		char* tmp_val;
		for (i=3;i<=argc;i++)
			total_length += strlen(argv[i-1].str)+1;

		tmp_val = (char*)calloc(total_length,1);
		for (i=3;i<=argc;i++)
		{
			strcat(tmp_val,argv[i-1].str);
			if (i < argc)
				strcat(tmp_val," ");
		}
		create_env((char*)(argv[1].str),tmp_val, true);
		free(tmp_val);
	}
        return 0;
}

static int cmd_saveenv(int argc, const cmd_args *argv)
{
	if (was_env_signed) {
		printf("\nNot allowed when env is signed\n");
		return 0;
	}
	if (!was_env_loaded) {
		printf("\nNot allowed when env are not loaded\n");
		return 0;
	}
    tenv_save();
    return 0;
}

static int cmd_loadenv(int argc, const cmd_args *argv)
{
        tenv_load();
        return 0;
}


#endif /* WITH_LIB_CONSOLE */
#endif /* ENABLE_FEATURE_BUILD_HBTS */
