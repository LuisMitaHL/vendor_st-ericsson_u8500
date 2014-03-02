/*
 * Copyright 2001 Silicon Graphics, Inc. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */
#include <ctype.h>
#include <string.h>
#include "sial.h"

/*
        Create a new string node from a string.
*/

value_t *
sial_setstrval(value_t *val, char *buf)
{
char *newbuf=sial_strdup(buf);

	val->v.data=(void*)newbuf;
	val->type.type=V_STRING;
	val->type.size=strlen(buf)+1;
	val->set=0;
	return val;
}

value_t *
sial_makestr(char *s)
{
	return sial_setstrval(sial_newval(), s);
}

static value_t*
sial_exestr(char *buf)
{
value_t *v=sial_newval();

	sial_setstrval(v, buf);
	return v;
}

void
sial_freestrnode(char *buf)
{
	sial_free(buf);
}

node_t*
sial_allocstr(char *buf)
{
node_t*n=sial_newnode();

        n->exe=(xfct_t)sial_exestr;
        n->free=(ffct_t)sial_freestrnode;
        n->data=buf;
	sial_setpos(&n->pos);

	return n;
}

node_t*
sial_strconcat(node_t*n1, node_t*n2)
{
char *newbuf=sial_alloc(strlen(n1->data)+strlen(n2->data)+1);

	strcpy(newbuf, n1->data);
	strcat(newbuf, n2->data);
	sial_free(n1->data);
	n1->data=newbuf;
	sial_freenode(n2);
	return n1;
}

static int
is_valid(int c, int base)
{
	switch(base)
	{
		case 16: return (c>='0' && c<='9') || (toupper(c) >= 'A' && toupper(c) <= 'F');
		case 10: return (c>='0' && c<='9');
		case 8:  return (c>='0' && c<='7');
	}
	return 0;
}

/* extract a number value_t from the input stream */
static int sial_getnum(int base)
{
int val=0;
	while(1)
	{
	char c=sial_input(), C;

		C=toupper(c);
		if(is_valid(C, base)) {

			val=val*base;
			val+=(C>='A')?10+('F'-C):'9'-C;
		}
		else
		{
			sial_unput(c);
			break;
		}
	}
	return val;
}

int
sial_getseq(int c)
{
int i;
static struct {
	int code;
	int value;
} seqs[] = {
	{ 'n', '\n' },
	{ 't', '\t' },
	{ 'f', '\f' },
	{ 'r', '\r' },
	{ 'n', '\n' },
	{ 'v', '\v' },
	{ '\\', '\007' },
};
	for(i=0;i<sizeof(seqs)/sizeof(seqs[0]);i++) {

		if(seqs[i].code==c) return seqs[i].value;
	}
	return c;
}

node_t*
sial_newstr()
{
int maxl=S_MAXSTRLEN;
char *buf=sial_alloc(maxl);
int iline=sial_line(0);
int i, c;

	/* let the input function knwo we want averyting from the 
	   input stream. Comments and all... */
	sial_rawinput(1);

	for(i=0;i<maxl;i++) {

		switch(c=sial_input()) {

		case '\\': /* escape sequence */
			switch(c=sial_input()) {
			case 'x': /* hexa value_t */
				buf[i]=sial_getnum(16);
			break;
			case '0': /* octal value_t */
				buf[i]=sial_getnum(8);
			break;
			default : 
				if(isdigit(c))
				{
					sial_unput(c);
					buf[i]=sial_getnum(10);
				}
				else
				{
					buf[i]=sial_getseq(c); 
				}
			break;

			}
		break;
		case '"': /* we're finished */
		{
			buf[i]='\0';
			sial_rawinput(0);
			return sial_allocstr(buf);

		}
		case (-1):
			sial_error("Unterminated string at line %d", iline);
		break;
		default:
			buf[i]=c;
		break;

		}
	}
	sial_error("String too long at %d", iline);
	return NULLNODE;
}
