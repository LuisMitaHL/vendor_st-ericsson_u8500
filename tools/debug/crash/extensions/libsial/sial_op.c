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
#include <string.h>

#define MAXPARMS 10

typedef struct {

	int op;			/* operator */
	int np;			/* number of operands */
	node_t*parms[MAXPARMS];	/* operands */

	srcpos_t pos;

} oper;

#define P1 (o->parms[0])
#define P2 (o->parms[1])
#define P3 (o->parms[2])
#define P4 (o->parms[3])

#define V1 (v1?v1:(v1=NODE_EXE(P1)))
#define V2 (v2?v2:(v2=NODE_EXE(P2)))
#define V3 (v3?v3:(v3=NODE_EXE(P3)))
#define V4 (v4?v4:(v4=NODE_EXE(P4)))

#define L1 (unival(V1))
#define L2 (unival(V2))
#define L3 (unival(V3))
#define L4 (unival(V4))

#define S1 ((V1)->v.data)
#define S2 ((V2)->v.data)
#define S3 ((V3)->v.data)
#define S4 ((V4)->v.data)

void sial_do_deref(int n, value_t *v, value_t *ref);
ul
sial_bool(value_t *v)
{
	switch(v->type.type) {

	case V_BASE:
		switch(v->type.size) {
			case 1: return !(!(v->v.uc));
			case 2: return !(!(v->v.us));
			case 4: return !(!(v->v.ul));
			case 8: return !(!(v->v.ull));
			default: sial_error("Oops sial_bool()[%d]", v->type.size); break;
		}
	case V_STRING : 	return !(!(*((char*)(v->v.data))));
	case V_REF:		return sial_defbsize()==8?(!(!(v->v.ull))):(!(!(v->v.ul)));
	default :

		sial_error("Invalid operand for boolean expression");
		return 0;
	}
}

static int cops[]={BAND,BOR,NOT,LT,LE,EQ,GE,GT,NE,CEXPR};
#define NCOPS (sizeof(cops)/sizeof(cops[0]))

static int
is_cond(int op)
{
int i;

	for(i=0;i<NCOPS;i++) {

		if(cops[i]==op) return 1;

	}
	return 0;
}

struct {
	int cur, equiv;
} cpls [] ={
	{ ADDME, ADD },
	{ SUBME, SUB },
	{ DIVME, DIV },
	{ MULME, MUL },
	{ SHLME, SHL },
	{ SHRME, SHR },
	{ XORME, XOR },
	{ ANDME, AND },
	{ ORME,  OR  },
	{ MODME, MOD },
};
#define NOPS (sizeof(cpls)/sizeof(cpls[0]))
/* get the equivalent operation ME type operators */
static int getop(int op)
{
int i;
	for(i=0;i<NOPS;i++) {

		if(cpls[i].cur==op) return cpls[i].equiv;

	}
	return op;
}

static void
sial_transfer(value_t *v1, value_t *v2, ull rl)
{
	sial_dupval(v1, v2);
	switch(TYPE_SIZE(&v1->type)) {

		case 1: v1->v.uc=rl; break;
		case 2: v1->v.us=rl; break;
		case 4: v1->v.ul=rl; break;
		case 8: v1->v.ull=rl; break;

	}
	/* the result of an assignment cannot be a lvalue_t */
	v1->set=0;
}

#define anyop(t) (V1->type.type==t || (o->np>1 && V2->type.type==t))

typedef struct {
	node_t*index;
	node_t*var;
	srcpos_t pos;
} index_t ;

static value_t *
sial_exeindex(index_t  *i)
{
value_t *var;
value_t *vi=NODE_EXE(i->index);
value_t *v;
srcpos_t p;

	sial_curpos(&i->pos, &p);

	/* we need to make believe it's been initiazed */
	sial_setini(i->var);
	var=NODE_EXE(i->var);

	/* check the type of the variable */
	/* if it's a pointer then index through the image */
	if(var->type.type==V_REF) {

		int size;
		int n=sial_getval(vi);
		value_t *ref;

		/* if this is an array and we're not at the rightmost index */
		if(var->type.idxlst && var->type.idxlst[1]) {

			int i, size=var->type.size;

			v=sial_cloneval(var);

			v->type.idxlst[0]=0;
			for(i=1; var->type.idxlst[i]; i++) {

				size *= var->type.idxlst[i];
				v->type.idxlst[i]=var->type.idxlst[i+1];
			}

			if(sial_defbsize()==4) {

				v->v.ul+=size*n;
				v->mem=v->v.ul;

			} else {

				v->v.ull+=size*n;
				v->mem=v->v.ull;
			}
			

		} else {

			v=sial_newval();
			ref=sial_cloneval(var);

			if(var->type.ref==1) size=var->type.size;
			else size=sial_defbsize();

			if(sial_defbsize()==4) {

				ref->v.ul+=size*n;
				ref->mem=ref->v.ul;

			} else {

				ref->v.ull+=size*n;
				ref->mem=ref->v.ull;
			}
			sial_do_deref(1, v, ref);
			sial_freeval(ref);
		}

	} else {

		v=sial_newval();

		/* use dynamic indexing aka awk indexing */
		sial_valindex(var, vi, v);
	}

	/* discard expression results */
	sial_freeval(var);
	sial_freeval(vi);
	sial_curpos(&p, 0);

	return v;
}

void
sial_freeindex(index_t  *i)
{
	NODE_FREE(i->index);
	NODE_FREE(i->var);
	sial_free(i);
}

node_t*
sial_newindex(node_t*var, node_t*idx)
{
index_t  *i=sial_alloc(sizeof(index_t ));
node_t*n=sial_newnode();

	i->index=idx;
	i->var=var;
	n->exe=(xfct_t)sial_exeindex;
	n->free=(ffct_t)sial_freeindex;
	n->data=i;
	sial_setpos(&i->pos);
	return n;
}

typedef struct {
	node_t*fname;
	node_t*parms;
	srcpos_t pos;
	void *file;
} call;

static value_t *
sial_execall(call *c)
{
value_t *rv;
srcpos_t p;

	sial_curpos(&c->pos, &p);
	rv=sial_docall(c->fname, c->parms, c->file);
	sial_curpos(&p, 0);
	return rv;
}

void
sial_freecall(call *c)
{
	NODE_FREE(c->fname);
	sial_free_siblings(c->parms);
	sial_free(c);
}

node_t*
sial_newcall(node_t* fname, node_t* parms)
{
node_t*n=sial_newnode();
call *c=sial_alloc(sizeof(call));

	c->fname=fname;
	c->file=sial_getcurfile();
	c->parms=parms;
	n->exe=(xfct_t)sial_execall;
	n->free=(ffct_t)sial_freecall;
	n->data=c;
	sial_setpos(&c->pos);
	return n;
}

typedef struct {
	node_t*expr;
	srcpos_t pos;
} adrof;

static value_t *
sial_exeadrof(adrof *a)
{
value_t *rv, *v=NODE_EXE(a->expr);

#if 0
	/* we can only do this op on something that came from system image
	   Must not allow creation of references to local variable */
	if(!v->mem) {

		sial_freeval(v);
		sial_rerror(&a->pos, "Invalid operand to '&' operator");

	}
#endif
	/* create the reference */
	rv=sial_newval();
	sial_duptype(&rv->type, &v->type);
	sial_pushref(&rv->type, 1);

	/* remmember position in image */
	if(sial_defbsize()==8) rv->v.ull=v->mem;
	else rv->v.ul=v->mem;
	rv->mem=0;

	sial_freeval(v);

	return rv;
}

void
sial_freeadrof(adrof *a)
{
	NODE_FREE(a->expr);
	sial_free(a);
}

node_t*
sial_newadrof(node_t* expr)
{
node_t*n=sial_newnode();
adrof *a=sial_alloc(sizeof(adrof));

	a->expr=expr;
	n->exe=(xfct_t)sial_exeadrof;
	n->free=(ffct_t)sial_freeadrof;
	n->data=a;
	sial_setpos(&a->pos);
	return n;
}

static int
sial_reftobase(value_t *v)
{
int idx= v->type.idx;

	if(v->type.type==V_REF) {

		if(sial_defbsize()==4) 
			v->type.idx=B_UL;
		else 
			v->type.idx=B_ULL;
	}
	return idx;
}

static value_t*
sial_docomp(int op, value_t *v1, value_t *v2)
{

	/* if one parameter is string then both must be */
	if(v1->type.type == V_STRING || v2->type.type == V_STRING) {

		if(v1->type.type != V_STRING || v2->type.type != V_STRING) {

			sial_error("Invalid condition arguments");
		}
		else {

			switch(op) {

				case EQ: {	/* expr == expr */

					return sial_makebtype(!strcmp(v1->v.data, v2->v.data));

				}
				case GT: case GE: {	/* expr > expr */

					return sial_makebtype(strcmp(v1->v.data, v2->v.data) > 0);

				}
				case LE: case LT: {	/* expr <= expr */

					return sial_makebtype(strcmp(v1->v.data, v2->v.data) < 0);

				}
				case NE: {	/* expr != expr */

					return sial_makebtype(strcmp(v1->v.data, v2->v.data));

				}
				default: {

					sial_error("Oops conditional unknown 1");

				}
			}
		}

	}
	else {

		int idx1, idx2;
		value_t *v=sial_newval();

		/* make sure pointers are forced to proper basetype
		   before calling sial_baseop()*/
		idx1=sial_reftobase(v1);
		idx2=sial_reftobase(v2);
		

		switch(op) {

			case EQ:
			case GT:
			case GE: 
			case LE:
			case LT:
			case NE:
				sial_baseop(op, v1, v2, v);
			break;
			default: {

				sial_error("Oops conditional unknown 2");

			}
		}
		v1->type.idx=idx1;
		v2->type.idx=idx2;
		return v;
	}
	return 0;
}

static value_t *
sial_exeop(oper *o)
{
value_t *v=0, *v1=0, *v2=0, *v3=0, *v4=0;
int top;
srcpos_t p;

	sial_curpos(&o->pos, &p);

	/* if ME (op on myself) operator, translate to normal operator
	   we will re-assign onto self when done */

	top=getop(o->op);

	if(top == ASSIGN) {

		goto doop;

	} else if(top == IN) {

		/* the val in array[] test is valid for anything but struct/union */
		v=sial_makebtype((ull)sial_lookuparray(P1,P2));

	}
	else if(is_cond(top)) {

		/* the operands are eithr BASE (integer) or REF (pointer) */ 
		/* all conditional operators accept a mixture of pointers and integer */
		/* set the return as a basetype even if bool */

		switch(top) {

			case CEXPR: {	/* conditional expression expr ? : stmt : stmt */

				if(sial_bool(V1)) {

					v=sial_cloneval(V2);

				} else {

					v=sial_cloneval(V3);

				}

			}
			break;
			case BOR: {	/* a || b */

				v=sial_makebtype((ull)(sial_bool(V1) || sial_bool(V2)));

			}
			break;
			case BAND: {	/* a && b */

				v=sial_makebtype((ull)(sial_bool(V1) && sial_bool(V2)));

			}
			break;
			case NOT: {	/* ! expr */

				v=sial_makebtype((ull)(! sial_bool(V1)));

			}
			break;
			default: {

				v=sial_docomp(top, V1, V2);

			}
		}

	} else if(anyop(V_STRING)) {

		if(top == ADD) 
		{
		char *buf;

			if(V1->type.type != V_STRING || V2->type.type != V_STRING) {

				sial_rerror(&P1->pos, "String concatenation needs two strings!");

			}
			buf=sial_alloc(strlen(S1)+strlen(S2)+1);
			strcpy(buf, S1);
			strcat(buf, S2);
			v=sial_makestr(buf);
			sial_free(buf);
		}
		else {

			sial_rerror(&P1->pos, "Invalid string operator");

		}
	}
	/* arithmetic operator */
	else if(anyop(V_REF)) { 

		int size;
		value_t *vt;

		/* make sure we have the base type second */
		if(V1->type.type != V_REF) { vt=V1; v1=V2; v2=vt; }


		if(V1->type.type == V_BASE) {
inval:
			sial_error("Invalid operand on pointer operation");
		}

		/* get the size of whas we reference */
		size=V1->type.size;
	
		switch(top) {
			case ADD: {	/* expr + expr */
				/* adding two pointers ? */
				if(V2->type.type == V_REF) goto inval;

				V1;
				sial_transfer(v=sial_newval(), v1,
					      unival(v1) + L2 * size);
			}
			break;
			case SUB: {	/* expr - expr */
				/* different results if mixed types.
				   if both are pointers then result is a V_BASE */
				if(V2->type.type == V_REF)
					v=sial_makebtype(L1 - L2);

				else {
					V1;
					sial_transfer(v=sial_newval(), v1,
						      unival(v1) - L2 * size);
				}
			}
			break;
			case PREDECR: { /* pre is easy */
				V1;
				sial_transfer(v=sial_newval(), v1,
					      unival(v1) - size);
				sial_setval(v1, v);
			}
			break;
			case PREINCR: {
				V1;
				sial_transfer(v=sial_newval(), v1,
					      unival(v1) + size);
				sial_setval(v1, v);
			}
			break;
			case POSTINCR: {
				V1;
				sial_transfer(v=sial_newval(), v1,
					      unival(v1) + size);
				sial_setval(v1, v);
				sial_transfer(v, v1, unival(v1));
			}
			break;
			case POSTDECR: {
				V1;
				sial_transfer(v=sial_newval(), v1,
					      unival(v1) - size);
				sial_setval(v1, v);
				sial_transfer(v, v1, unival(v1));
			}
			break;
			default:
				sial_error("Invalid operation on pointer [%d]",top);
		}
	}
	else {

		/* both operands are V_BASE */
		switch(top) {

			/* for mod and div, we check for divide by zero */
			case MOD: case DIV:
				if(!L2) {
					sial_rerror(&P1->pos, "Mod by zero");
				}
			case ADD: case SUB: case MUL: case XOR: 
			case OR: case AND: case SHL: case SHR:
			{
				sial_baseop(top, V1, V2, v=sial_newval());
			}
			break;
			case UMINUS: {

				value_t *v0=sial_newval();
				sial_defbtype(v0, (ull)0);
				/* keep original type of v1 */
				v=sial_newval();
				sial_duptype(&v0->type, &V1->type);
				sial_duptype(&v->type, &V1->type);
				sial_baseop(SUB, v0, V1, v);
				sial_freeval(v0);
				/* must make result signed */
				sial_mkvsigned(v);
			}
			break;
			case FLIP: {

				value_t *v0=sial_newval();
				sial_defbtype(v0, (ull)0xffffffffffffffffll);
				/* keep original type of v1 */
				sial_duptype(&v0->type, &V1->type);
				sial_baseop(XOR, v0, V1, v=sial_newval());
				sial_freeval(v0);
			}
			break;
			case PREDECR: { /* pre is easy */
				V1;
				sial_transfer(v=sial_newval(), v1,
					      unival(v1) - 1);
				sial_setval(v1, v);
			}
			break;
			case PREINCR: {
				V1;
				sial_transfer(v=sial_newval(), v1,
					      unival(v1) + 1);
				sial_setval(v1, v);
			}
			break;
			case POSTINCR: {
				V1;
				sial_transfer(v=sial_newval(), v1,
					      unival(v1) + 1);
				sial_setval(v1, v);
				sial_transfer(v, v1, unival(v1));
			}
			break;
			case POSTDECR: {
				V1;
				sial_transfer(v=sial_newval(), v1,
					      unival(v1) - 1);
				sial_setval(v1, v);
				sial_transfer(v, v1, unival(v1));
			}
			break;
			default: sial_rerror(&P1->pos, "Oops ops ! [%d]", top);
		}
	}
doop:
	/* need to assign the value_t back to P1 */
	if(top != o->op || top==ASSIGN) {

		/* in the case the Lvalue_t is a variable , bypass execution and set ini */
		if(P1->exe == sial_exevar) {

			char *name=NODE_NAME(P1);
			var_t*va=sial_getvarbyname(name, 0, 0);
			value_t *vp;

			sial_free(name);

			if(top != o->op) vp=v;
			else vp=V2;

			sial_chkandconvert(va->v, vp);

			sial_freeval(v);
			v=sial_cloneval(va->v);
			va->ini=1;

		} else {

			if(!(V1->set)) {

				sial_rerror(&P1->pos, "Not Lvalue_t on assignment");

			}
			else {

				/* if it's a Me-op then v is already set */
				V1;
				if(top != o->op) {
					sial_setval(v1, v);
				} else {
					sial_setval(v1, V2);
					v=sial_cloneval(V2);
				}

			}
		}
		/* the result of a assignment if not an Lvalue_t */
		v->set=0;
	}
	sial_freeval(v1);
	sial_freeval(v2);
	sial_freeval(v3);
	sial_freeval(v4);
	sial_setpos(&p);
	return v;
}

void
sial_freeop(oper *o)
{
int i;

	for(i=0;i<o->np;i++) NODE_FREE(o->parms[i]);
	sial_free(o);
}

node_t*
sial_newop(int op, int nargs, ...)
{
va_list ap;
node_t*n=sial_newnode();
oper *o=sial_alloc(sizeof(oper));
int i;

	o->op=op;
	o->np=nargs;

	sial_setpos(&o->pos);

	va_start(ap, nargs);

	for(i=0 ; i<MAXPARMS; i++) {

		if(!(o->parms[i]=va_arg(ap, node_t*))) break;;
	}

	n->exe=(xfct_t)sial_exeop;
	n->free=(ffct_t)sial_freeop;
	n->data=o;
	
	va_end(ap);
	return n;
}

/* mult is a special case since the parse always return a PTR token 
   for the '*' signed. The PTR token value_t is the number of '* found.
*/
node_t*
sial_newmult(node_t*n1, node_t*n2, int n)
{
	if(n>1) {

		sial_error("Syntax error");
	}
	return sial_newop(MUL, 2, n1, n2);
}
/*
	This function is called when we want to set a value_t in live memory
	using a pointer to it.
*/
static void
sial_setderef(value_t *v1, value_t *v2)
{
	void *sial_adrval(value_t *);
	sial_transval(v2->type.size, v1->type.size, v2, sial_issigned(v2->type.typattr));
	API_PUTMEM(v1->mem, sial_adrval(v2), v2->type.size);
}

/*
	Do a de-referencing from a pointer (ref) and put the result in v.
*/
typedef struct {
	int lev;
	node_t*n;
} ptrto;

void
sial_do_deref(int n, value_t *v, value_t *ref)
{
ull madr, new_madr;

	if(n > ref->type.ref) {

		sial_error("Too many levels of dereference");

	}else {
	

		if(sial_defbsize()==4) madr=(ull)ref->v.ul;
		else madr=ref->v.ull;

		/* copy the target type to the returned value_t's type_t*/
		sial_duptype(&v->type, &ref->type);

		/* do a number of deferences according to PTR value_t */
		while(n--) {

			sial_popref(&v->type, 1);

			if(!v->type.ref) {

				/* make sure the pointer is pointing into the vmcore */
				if(is_ctype(v->type.type)) {

					v->v.data=sial_alloc(v->type.size);
					sial_getmem(madr, v->v.data, v->type.size);

				} else {

					/* get the data from the system image */
					switch(TYPE_SIZE(&v->type)) {

						case 1: sial_getmem(madr, &v->v.uc, 1); 
							break;
						case 2: sial_getmem(madr, &v->v.us, 2); 
							break;
						case 4: sial_getmem(madr, &v->v.ul, 4);
							break;
						case 8: sial_getmem(madr, &v->v.ull, 8); 
							break;

					}
				}
			}
			else {
		
				/* get the pointer at this address */
				if(sial_defbsize()==4) {

					sial_getmem(madr, &v->v.ul, 4);
					new_madr=v->v.ul;

				} else {

					sial_getmem(madr, &v->v.ull, 8);
					new_madr=v->v.ull;
				}
			}

			/* remember this address. For the '&' operator */
			v->mem=madr;
			madr=new_madr;
		}
	}

	/* we can always assign to a reference */
	v->set=1;
	v->setval=v;
	v->setfct=sial_setderef;
}

static value_t *
sial_exepto(ptrto *pto)
{
value_t *v=sial_newval();
int n=pto->lev;
value_t *ref=NODE_EXE(pto->n);

	sial_do_deref(n, v, ref);
	sial_freeval(ref);
	return v;
}

static void
sial_freepto(ptrto *pto)
{
	NODE_FREE(pto->n);
	sial_free(pto);
}
	

/* same thing for the ptrto operator */
node_t*
sial_newptrto(int lev, node_t*n)
{
ptrto *pto=sial_alloc(sizeof(ptrto));
node_t*nn=sial_newnode();

	pto->lev=lev;
	pto->n=n;
	nn->exe=(xfct_t)sial_exepto;
	nn->free=(ffct_t)sial_freepto;
	nn->data=pto;
	return nn;
}
