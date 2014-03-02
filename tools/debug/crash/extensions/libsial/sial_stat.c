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
#include "sial.h"
#include "sial.tab.h"
#include <stdarg.h>
#include <setjmp.h>

#define MAXPARMS 10

typedef struct stat {

	int stype;
	int np;
	struct stat *next;
	srcpos_t pos;
	node_t*n;
	node_t*parms[MAXPARMS];
	var_t*svs;	/* if statement block then these are the auto and static
			   wars for it */
	var_t*avs;

} stat;

#define SETVS	value_t *v1=0,*v2=0,*v3=0,*v4=0
#define FV1	sial_freeval(v1),v1=0
#define FV2	sial_freeval(v2),v2=0
#define FV3	sial_freeval(v3),v3=0
#define FV4	sial_freeval(v4),v4=0
#define UNSETVS	FV1,FV2,FV3,FV4

#define P1 (s->parms[0])
#define P2 (s->parms[1])
#define P3 (s->parms[2])
#define P4 (s->parms[3])

#define V1 (v1?v1:(v1=NODE_EXE(P1)))
#define V2 (v2?v2:(v2=NODE_EXE(P2)))
#define V3 (v3?v3:(v3=NODE_EXE(P3)))
#define V4 (v4?v4:(v4=NODE_EXE(P4)))

#define L1 (unival(V1))
#define L2 (unival(V2))
#define L3 (unival(V3))
#define L4 (unival(V4))

#define S1 (V1->v.data)
#define S2 (V2->v.data)
#define S3 (V3->v.data)
#define S4 (V4->v.data)

/* this is used to execute staement lists e.g. i=1,j=3; */
static value_t*
sial_exeplist(node_t*n)
{
value_t *val=0;

	if(n) {

		do {

			if(val) sial_freeval(val), val=0;
			val=NODE_EXE(n);
			n=n->next;

		} while(n);
	}
	return val;
}

static int
sial_dofor(stat *s)
{
jmp_buf brkenv;
jmp_buf cntenv;
SETVS;

	if(!setjmp(brkenv)) {

		sial_pushjmp(J_BREAK, &brkenv, 0);

		v1=sial_exeplist(P1);
		FV1;

		while(!P2 || sial_bool(V2)) {

			FV2;

			if(!setjmp(cntenv)) {

				sial_pushjmp(J_CONTINUE, &cntenv, 0);
				V4;
				FV4;
				sial_popjmp(J_CONTINUE);

			}
			
			UNSETVS; /* make sure we re-execute everything each time */
			v3=sial_exeplist(P3);
			FV3;
		}
		sial_popjmp(J_BREAK);
		
	}
	UNSETVS;
	return 1;
}

static int
sial_dowhile(stat *s)
{
jmp_buf brkenv;
jmp_buf cntenv;
SETVS;

	if(!setjmp(brkenv)) {

		sial_pushjmp(J_BREAK, &brkenv, 0);

		while(sial_bool(V1)) {

			FV1;

			if(!setjmp(cntenv)) {

				sial_pushjmp(J_CONTINUE, &cntenv, 0);
				V2;
				FV2;
				sial_popjmp(J_CONTINUE);

			}
			
			UNSETVS; /* make sure we re-execute everything each time */
		}
		FV1;
		sial_popjmp(J_BREAK);
		
	}

	return 1;
}

static int
sial_dodo(stat *s)
{
jmp_buf brkenv;
jmp_buf cntenv;
SETVS;

	if(!setjmp(brkenv)) {

		sial_pushjmp(J_BREAK, &brkenv, 0);

		do {

			FV2;
			if(!setjmp(cntenv)) {

				sial_pushjmp(J_CONTINUE, &cntenv, 0);
				V1;
				FV1;
				sial_popjmp(J_CONTINUE);

			}
			
			UNSETVS; /* make sure we re-execute everything each time */

		} while (sial_bool(V2));
		FV2;

		sial_popjmp(J_BREAK);

	}

	UNSETVS;
	return 1;
}

static int
sial_doif(stat *s)
{
SETVS;
ul b;

	b=sial_bool(V1);
	FV1;

	if(s->np==3) {

		if (b) 
			V2;
		else 
			V3;

	} else {

		if (b) 
			V2;

	}

	UNSETVS;
	return 1;
}

static int
sial_doswitch(stat *s)
{
jmp_buf brkenv;
ull cval;
SETVS;

	if(!setjmp(brkenv)) {

		sial_pushjmp(J_BREAK, &brkenv, 0);
		cval=unival(V1);
		FV1;
		sial_docase(cval, P2->data);
		sial_popjmp(J_BREAK);

	}

	UNSETVS;
	return 1;
}

static void
sial_exein(stat *s)
{
jmp_buf cntenv;
SETVS;

	if(!setjmp(cntenv)) {

		sial_pushjmp(J_CONTINUE, &cntenv, 0);
		V3;
		sial_popjmp(J_CONTINUE);

	}
	UNSETVS;
}

static int
sial_doin(stat *s)
{
jmp_buf brkenv;
	if(!setjmp(brkenv)) {

		sial_pushjmp(J_BREAK, &brkenv, 0);
		sial_walkarray(P1, P2, (void (*)(void *))sial_exein, s);
		sial_popjmp(J_BREAK);
	}
	return 1;
}

/* this is where all of the flow control takes place */

static value_t*
sial_exestat(stat *s)
{
srcpos_t p;
value_t *val=0;

	do {

		/* dump the val while looping */
		if(val) sial_freeval(val);
		val=0;

		sial_curpos(&s->pos, &p);


		switch(s->stype) {

		case FOR : 	sial_dofor(s); break;
		case WHILE: 	sial_dowhile(s); break;
		case IN:	sial_doin(s); break;
		case IF:	sial_doif(s); break;
		case DO:	sial_dodo(s); break;
		case SWITCH:	sial_doswitch(s); break;
		case DOBLK:
		{
		int lev;

			/* add any static variables to the current context */
			lev=sial_addsvs(S_STAT, s->svs);
			sial_addsvs(S_AUTO, sial_dupvlist(s->avs));

			/* with the block statics inserted exeute the inside stmts */
			if(s->next) val=sial_exestat(s->next);

			/* remove any static variables to the current context */
			if(s->svs) sial_setsvlev(lev);

			sial_curpos(&p, 0);

			return val;
		}

		case BREAK:	sial_dojmp(J_BREAK, 0); break;
		case CONTINUE:	sial_dojmp(J_CONTINUE, 0); break;
		case RETURN: {


			if(s->parms[0]) {

				val=(s->parms[0]->exe)(s->parms[0]->data);
			}
			else val=sial_newval();

			sial_curpos(&p, 0);
			sial_dojmp(J_RETURN, val);
		}
		break;
		case PATTERN:

			val=sial_exeplist(s->parms[0]);

		}

		sial_curpos(&p, 0);

	} while((s=s->next));

	/* we most return a type val no mather what it is */
	/* that's just the way it is...Somethings will never change...*/
	if(!val) val=sial_newval();

	return val;
}

void
sial_freestat(stat *s)
{
int i;

	if(s->next) sial_freenode(s->next->n);

	for(i=0;i<s->np && s->parms[i];i++) {

		NODE_FREE(s->parms[i]);

	}
	sial_free(s);
}

void
sial_freestat_static(stat *s)
{

	if(s->next) sial_freenode(s->next->n);

	/* free associated static var list */
	sial_freesvs(s->svs);
	sial_freesvs(s->avs);
	sial_free(s);
}

var_t*sial_getsgrp_avs(node_t*n) { return ((stat *)n->data)->avs; }
var_t*sial_getsgrp_svs(node_t*n) { return ((stat *)n->data)->svs; }

/* add a set of static variable to a statement */
node_t*
sial_stat_decl(node_t*n, var_t*svs)
{
node_t*nn;
stat *s;

	sial_validate_vars(svs);

	nn=sial_newnode();
	s=sial_alloc(sizeof(stat));

	/* add statics and autos to this statement */
	s->svs=sial_newvlist();
	s->avs=sial_newvlist();
	sial_addnewsvs(s->avs, s->svs, svs);

	if(n) s->next=(stat*)(n->data);
	else s->next=0;
	s->stype=DOBLK;
	s->n=nn;
	nn->exe=(xfct_t)sial_exestat;
	nn->free=(ffct_t)sial_freestat_static;
	nn->data=s;
	sial_setpos(&s->pos);

	return nn;
}

node_t*
sial_newstat(int type, int nargs, ...)
{
va_list ap;
node_t*n=sial_newnode();
stat *s=sial_alloc(sizeof(stat));
int i;

	s->stype=type;

	va_start(ap, nargs);

	for(i=0;i<nargs && i<MAXPARMS; i++) {

		s->parms[i]=va_arg(ap, node_t*);
	}

	s->np=i;
	s->n=n;
        s->next=0;
	n->exe=(xfct_t)sial_exestat;
	n->free=(ffct_t)sial_freestat;
	n->data=s;
	
	sial_setpos(&s->pos);
	
	va_end(ap);
	return n;
}

node_t*
sial_addstat(node_t*list, node_t*s)
{
	if(!s && list) return list;
	if(s && !list) return s;
	else {
		stat *sp=(stat*)(list->data);

		while(sp->next) sp=sp->next;
		sp->next=(stat*)(s->data);
		return list;

	}
}

