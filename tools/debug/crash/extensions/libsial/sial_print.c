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
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "sial.h"
/*
	This set of function are used to print value_ts.
*/

/* utility that returns a string of '*' for a reference */
static
char *sial_getref(int lev)
{
static char *ptrs="*******";

	return ptrs+strlen(ptrs)-lev;
}

static char *
sial_getidx(type_t *t, char*buf, int len)
{
int pos=0;

	buf[0]='\0';
	if(t->idxlst) {

		int i;

		for(i=0; t->idxlst[i] && pos < len; i++) {

			pos += snprintf(buf+pos, len-pos, "[%d]", t->idxlst[i]);
		}
	}
	return buf;
}

#define INDENT		4	/* print indent at beginning of new line */
#define SPACER		16	/* space for type string */
#define NAMESPACE	16	/* space used for member/var names */
#define NBUNDLE		4	/* when printing arrays print this much before \n */

static void
sial_indent(int level, int indent)
{
	if(!indent) return;
	sial_msg("%*s", level*INDENT, "");
}

static void sial_ptype2(type_t*t, value_t*v, int level, int indent, char *name, int ref, int justv);

/*
	Print a struct/union type or value
*/
static void
sial_print_ctype(type_t *t, value_t *v, int level, int indent, char *name, int ref, int justv)
{
stinfo_t *st=sial_getstbyindex(t->idx, t->type);
stmember_t *m;
char buf[100];

	if(!st) sial_error("Oops sial_print_ctype!");

	if(!st->all) {

		sial_fillst(st);
		if(!st->all) sial_error("Reference to a incomplete type");
	}

	sial_indent(level, indent);

	if(!justv) {
		snprintf(buf, sizeof(buf)-1, "%s %s", sial_ctypename(t->type), st->name?st->name:"");
		sial_msg("%-*s ", SPACER, buf);

		/* is this is a pointer, bail out */
	}
	if(ref) return;

	if(v && !justv) sial_msg(" = ");

	sial_msg("{\n");

	for(m=st->stm; m; m=m->next) {

		value_t *vm=0;

		sial_indent(level+1, 1);
		if(v) {
			vm=sial_newval();
			sial_duptype(&vm->type, &m->type);
			sial_exememlocal(v, m, vm);
			sial_ptype2(&vm->type, vm, level+1, 0, m->m.name, 0, 0);

		} else sial_ptype2(&m->type, vm, level+1, 0, m->m.name, 0, 0);
		sial_msg(";\n");
		if(vm) sial_freeval(vm);
	}

	sial_indent(level, 1);
	sial_msg("}");
	if(name) sial_msg(" %s", name);
	
}

static void
sial_prbval(value_t *v)
{
	if(sial_issigned(v->type.typattr)) sial_msg("%8lld", sial_getval(v));
	else sial_msg("%8llu", sial_getval(v));
}

static int 
sial_prtstr(value_t *v, int justv)
{
value_t *vs;
char *s, *p;

	if(sial_defbsize()==8) v->v.ull=v->mem;
	else v->v.ul=v->mem;
	vs=sial_getstr(v);
	s=sial_getptr(vs, char);
	for(p=s; *p; p++) if(!isprint(*p)) return 0;
	if(p==s) { sial_freeval(vs); return 0; }
	if(!justv) sial_msg("= ");
	sial_msg("\"%s\"", s);
	sial_freeval(vs);
	return 1;
}

static void
sial_prtarray(type_t*t, ull mem, int level, int idx)
{
int i;
int j, size=1;

	for(j=idx+1; t->idxlst[j]; j++) size *= t->idxlst[j];
	size *= t->type==V_REF ? sial_defbsize() : t->size;

	/* start printing */
	sial_msg("{");
	sial_msg("\n");
	sial_indent(level+1, 1);

	for(i=0; i<t->idxlst[idx]; i++, mem += size) {

		if(t->idxlst[idx+1]) {

			sial_msg("[%d] = ", i);
			sial_prtarray(t, mem, level+1, idx+1);

		} else {

			/* time to deref and print final type */
			value_t *v=sial_newval(), *vr=sial_newval();
			int *pi=t->idxlst;

			t->idxlst=0;

			sial_duptype(&vr->type, t);
			sial_pushref(&vr->type, 1);
			if(sial_defbsize()==8) vr->v.ull=mem;
			else vr->v.ul=(ul)mem;
			sial_do_deref(1, v, vr);
			if(is_ctype(v->type.type) || !(i%NBUNDLE)) sial_msg("[%2d] ", i);
			sial_ptype2(&v->type, v, level+1, 0, 0, 0, 1);
			sial_msg(", ");
			/* anything else then struct/unions, print in buddles */
			if(!is_ctype(v->type.type) && !((i+1)%NBUNDLE)) {

				sial_msg("\n"); 
				sial_indent(level+1, 1);
			}
			sial_freeval(v);
			sial_freeval(vr);
			t->idxlst=pi;
		}
	}
	sial_msg("\n");
	sial_indent(level, 1);
	sial_msg("}");
}

/*
	Print a type.
	Typical output of the 'whatis' command.
*/
static
void sial_ptype2(type_t*t, value_t*v, int level, int indent, char *name, int ref, int justv)
{
int type=t->type;

	sial_indent(level, indent);
	switch(type) {

		case V_STRUCT: case V_UNION:

			/* make sure we have all the member info */
			sial_print_ctype(t, v, level, 0, name, ref, justv);
		break;


		case V_TYPEDEF:
			/* no typedef should get here */
			sial_warning("Typedef in print!");
		break;

		case V_ENUM:
			/* no enum should get here */
			sial_warning("ENUM in print!");
		break;

		case V_REF:
		{
		int refi=t->ref, ref=refi;

			/* decrement ref if this was declared as a array */
			if(t->idxlst) ref--;

			/* print the referenced type */
			sial_popref(t, t->ref);
			sial_ptype2(t, 0, level, 0, 0, 1, justv);
			sial_pushref(t, refi);

			if(!justv) {

				char buf[100], buf2[100];
				int pos=0, len=sizeof(buf);

				buf[0]='\0';
				if(t->fct) buf[pos++]='(';
				if(pos < len)
					pos += snprintf(buf+pos, len-pos, "%s%s", sial_getref(ref), name?name:"");
				if(pos < len)
					pos += snprintf(buf+pos, len-pos, "%s", sial_getidx(t, buf2, sizeof(buf2)));
				if(pos < len && t->fct)
					pos += snprintf(buf+pos, len-pos, "%s", ")()");

				sial_msg("%*s ", NAMESPACE, buf);
			}

			/* arrays are ref with boundaries... */
			if(t->idxlst && v) {

				if(t->idxlst[1] || t->rtype!=V_BASE || t->size!=1 || !sial_prtstr(v, justv)) 
				{
					if(!justv) sial_msg("= ");
					sial_popref(t, 1);
					sial_prtarray(t, v->mem, level, 0);
					sial_pushref(t, 1);
				}

			} else if(v) {

				if(!justv) sial_msg("= ");
				if(!sial_getval(v)) sial_msg("(nil)");
				else {
					if(sial_defbsize()==8) sial_msg("0x%016llx", sial_getval(v));
					else sial_msg("0x%08x", sial_getval(v));
				}
				if(t->ref==1 && t->rtype==V_BASE && t->size==1) {

					(void)sial_prtstr(v, justv);
				}
			}
		}
		break;

		case V_BASE:
		{
			if(sial_isenum(t->typattr)) {

				stinfo_t *st=sial_getstbyindex(t->rtype, V_ENUM);
				if(!justv) {
					char buf[200];
					snprintf(buf, sizeof(buf), "enum %s", st->name?st->name:"");
					sial_msg("%-*s ", SPACER, buf);
					sial_msg("%*s ", NAMESPACE, (name&&v)?name:"");
				}
				if(v) {

					enum_t *e=st->enums;

					sial_msg("= ");
					sial_prbval(v);
					while(e) {

						if(e->value==sial_getval(v)) {
							sial_msg(" [%s]", e->name);
							break;
						}
						e=e->next;
					}
					if(!e) sial_msg(" [???]");

				}else{

					enum_t *e=st->enums;
					int count=0;

					sial_msg(" {");
					while(e) {

						if(!(count%4)) {
							sial_msg("\n");
							sial_indent(level+1, 1);
						}
						count ++;
						sial_msg("%s = %d, ", e->name, e->value);
						e=e->next;

					}
					sial_msg("\n");
					sial_indent(level, 1);
					sial_msg("%-*s ", SPACER, "}");
					if(ref) return;
					sial_msg("%*s ", NAMESPACE, name?name:"");
				}

			} else {

				if(!justv) {
					sial_msg("%-*s " , SPACER , sial_getbtypename(t->typattr));
					if(ref) return;
					sial_msg("%s%*s ", sial_getref(t->ref), NAMESPACE, name?name:"");
				}
				if(v) { 

					if(!justv) sial_msg("= ");
					sial_prbval(v);
				}
			}
		}
		break;
		case V_STRING:
			if(!justv) {
				sial_msg("%-*s " , SPACER , "string");
				sial_msg("%*s ", NAMESPACE, name?name:"");
			}
			if(v) {

				if(!justv) sial_msg("= ");
				sial_msg("\"%s\"", v->v.data);
			}
		break;
	}
	if(indent) sial_msg("\n");
}

static value_t*
sial_ptype(value_t*v)
{
	sial_ptype2(&v->type, 0, 0, 1, 0, 0, 0);
	sial_msg("\n");
	return 0;
}

node_t*
sial_newptype(var_t*v)
{
node_t*n=sial_newnode();

	n->data=v->next->v;
	v->next->v=0; /* save value against freeing */
	sial_freevar(v->next);
	sial_freevar(v);
	n->exe=(xfct_t)sial_ptype;
	n->free=(ffct_t)sial_freeval;
	n->name=0;
	sial_setpos(&n->pos);
	return n;
}

static value_t *
sial_pval(node_t*n)
{
value_t *v=NODE_EXE(n);
char *name=NODE_NAME(n);

	sial_ptype2(&v->type, v, 0, 1, name, 0, 0);
	sial_free(name);
	sial_freeval(v);
	return 0;
}

node_t*
sial_newpval(node_t*vn, int fmt)
{
node_t*n=sial_newnode();

	n->data=vn;
	n->exe=(xfct_t)sial_pval;
	n->free=(ffct_t)sial_freenode;
	n->name=0;
	sial_setpos(&n->pos);
	return n;
}
