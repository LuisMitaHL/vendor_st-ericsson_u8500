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
#include <string.h>
#include <stdio.h>
#include "sial.h"

typedef struct {
	int type;
	ull val;
} num;

/*
        Numeric constants.
*/

static value_t*
sial_exenum(num *n)
{
value_t *v=sial_newval();

	v->type.type=V_BASE;
	v->type.idx=n->type;
	if(n->type==B_SLL) {

ll:
		v->v.sll=n->val;
		v->type.size=8;

	}else if(n->type==B_SC) {

		v->v.sc=n->val;
		v->type.size=1;

	} else {

		if(sial_defbsize()==4) {

			v->v.sl=n->val;
			v->type.size=4;

		} else {

			v->type.idx=B_SLL;
			goto ll;
		}
	}
	v->type.typattr=sial_idxtoattr(v->type.idx);
	v->set=0;
	return v;
}

void
sial_freenumnode(num *n)
{
	sial_free(n);
}

node_t*
sial_makenum(int type, ull val)
{
node_t*n=sial_newnode();
num *nu=sial_alloc(sizeof(num));

	TAG(nu);

	nu->type=type;
	nu->val=val;
        n->exe=(xfct_t)sial_exenum;
        n->free=(ffct_t)sial_freenumnode;
        n->data=nu;

	sial_setpos(&n->pos);
	return n;
}

/*
	Execution of the sizeof() operator.
	This sould be done at compile time, but I have not setup
	a 'type only' execution path for the nodes.
	Runtime is good enough to cover mos cases.
*/
#define SN_TYPE 1
#define SN_EXPR 2

typedef struct {
	int type;
	void *p;
	srcpos_t pos;
} snode_t;

static value_t *
sial_exesnode(snode_t*sn)
{
srcpos_t pos;
type_t*t;
value_t *v=sial_newval();
value_t *v2=0;
int size;

	sial_curpos(&sn->pos, &pos);
	if(sn->type == SN_TYPE) {

		t=(type_t*)(sn->p);

	} else {

		sial_setinsizeof(1);
		v2=NODE_EXE((node_t*)(sn->p));
		t=&v2->type;
		sial_setinsizeof(0);
	}

	switch(t->type) {

		case V_REF:

			if(t->idxlst) {

				int i; 
				for(size=t->size,i=0;t->idxlst[i];i++) size *= t->idxlst[i];

			} else size=sial_defbsize();

		break;
		case V_STRUCT: case V_UNION:

			if(sial_ispartial(t)) {

				sial_error("Invalid type specified");
			}
			size=t->size;

		break;
		case V_BASE: case V_STRING:
			size=t->size;
		break;
		
		default: size=0;
	}

	sial_defbtype(v, (ull)size);

	sial_curpos(&pos, 0);

	if(v2) sial_freeval(v2);

	return v;
	
}

static void
sial_freesnode(snode_t*sn)
{
	if(sn->type == SN_TYPE) sial_free(sn->p);
	else NODE_FREE(sn->p);
	sial_free(sn);
}

node_t*
sial_sizeof(void *p, int type)
{
node_t*n=sial_newnode();
snode_t*sn=sial_alloc(sizeof(snode_t));

	n->exe=(xfct_t)sial_exesnode;
	n->free=(ffct_t)sial_freesnode;
	n->data=sn;
	sn->type=type;
	sn->p=p;
	sial_setpos(&sn->pos);
	return n;
}

node_t*
sial_newnum(char *buf)
{
int type;
unsigned long long val;

	type=B_SL;

	/* get the value_t of this constant. Could be hex, octal or dec. */
	if(buf[0]=='0') {

		if(buf[1]=='x') {

			if(!sscanf(buf, "%llx", &val)) goto error;

		} else {

			if(!sscanf(buf,"%llo", &val)) goto error;
		}

	} else {

		if(!sscanf(buf,"%lld", &val)) goto error;

	}

	if(val & 0xffffffff00000000ll) type=B_SLL;
	
	/* threat the long and long long atributes */
	{ 
		int l=strlen(buf);

		if(l>1) {

			if(buf[l-1]=='l' || buf[l-1]=='L') {

				if(l>2) {

					if(sial_defbsize()==8 || buf[l-2]=='l' || buf[l-2]=='L') {

						type=B_SLL;

					}
					else type=B_SL;

				}

			}
		}
	}
	{
	node_t*n=sial_makenum(type, val);
		TAG(n->data);
		return n;
	}
error:
	sial_error("Oops! NUMBER");
	return 0;
}
