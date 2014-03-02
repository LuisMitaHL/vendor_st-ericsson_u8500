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

#ifndef _TINY_ENV_H
#define _TINY_ENV_H

#include <sys/types.h>

#define GET_LENGTH	1
#define GET_STRING	2

#ifndef ENV_NAME
#define ENV_NAME	"BOOT_ENV"
#endif

typedef struct envVar{
	char* name;
	char* value;
	struct envVar* next ;
	struct envVar* prev ;
} envNode;

typedef struct subStringVar{
	char* start;
	char* end;
	int isEnv;
	struct subStringVar* next ;
} subStringNode;

void tenv_init(void);
char* tenv_eval_env(char* env_string);
char* tenv_getval(char* name);
int tenv_save(void);
int tenv_load(void);
int tenv_remove_env (char *name);
char *tenv_env_to_cmdline (void);

int create_env(char *name, char *val, int evalIt);

bool is_env_signed(void);
bool is_env_loaded(void);


#endif /* _TINY_ENV_H */

