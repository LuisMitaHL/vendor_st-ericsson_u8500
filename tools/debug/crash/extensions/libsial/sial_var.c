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
#include <setjmp.h>
#include <string.h>
#include "sial.h"

/*
	Get an existing variable from the current set.
*/

/* variable lists for the different scopes */
typedef struct {
	int type;
	var_t*svs;
} svlist;

typedef struct glo {
	struct glo *next;
	var_t*vv;
} glo;

/*
	Free indexes specifications.
*/
void
sial_freeidx(idx_t *idx)
{
int i;

	for(i=0;i<idx->nidx;i++) {

		if(idx->idxs[i]) NODE_FREE(idx->idxs[i]);
	}
	sial_free(idx);
}

/*
	Free a variable declaration structure.
*/
void
sial_freedvar(dvar_t*dv)
{
	if(!dv) return;
	if(--dv->refcount) return;
	if(dv->name) sial_free(dv->name);
	if(dv->idx) sial_freeidx(dv->idx);
	if(dv->init) NODE_FREE(dv->init);
	if(dv->fargs) sial_freesvs(dv->fargs);
	sial_free(dv);
}

void
sial_setarray(array_t**arpp)
{
array_t*arp=*arpp;

	if(!arp) {

		arp=sial_calloc(sizeof(array_t));
		TAG(arp);
		arp->next=arp->prev=arp;
		arp->ref=1;
		*arpp=arp;
	}
}

/*
	this is the main variable declaration function.
	We support the global scope attribute that make the declared
	variable accessible to all function from all scripts.

	By default the scope of a variable either the statement block
	where it was declared (or first used):
	{
	int var;
	 ...
	}
	Then it's scope is the block itself.

	Or the file, if it was declared outside of a function.

	Storage is by default 'automatic' and can be made permanent
	by using the 'static' keywork in the declaration.
	'Volatile' and 'register' storage classes are supported but 
	have no effect.
*/
var_t*
sial_vardecl(dvar_t*dv, type_t*t)
{
var_t*vlist=sial_newvlist();
var_t*var;

	/* type *and* dv can have ref counts. First comes from typedef parsing
	   second comes from the declaration itself */
	dv->ref += t->ref;

	/* add one level of ref for arrays */
	if(dv->idx) dv->ref++;

	/* reset ref level for tests below */
	sial_popref(t, t->ref);

	TAG(vlist);

	if(!t->type) {

		int sto=sial_isstor(t->typattr);

		sial_freetype(t);
		t=sial_newbtype(0);
		t->typattr |= sto;
	}
	else if(t->type==V_BASE && !dv->ref) {

		sial_chksign(t);
		sial_chksize(t);
	}

	/* is this a new typedef declaration ? */
	/* typedef is considered just like any other storage class */
	if(sial_istdef(t->typattr)) {

		sial_tdef_decl(dv, t);
		return 0;
	}

	while(dv) {
        
                /* disalow var names that match against already defined vars */
                if(dv->name[0]) {
                    type_t *t=sial_getctype(V_TYPEDEF, dv->name, 1);
                    if(t) {
                    
                        sial_freetype(t);
                        sial_warning("Variable '%s' already defined as typedef.\n");
                    }
                }

		/* 
		   some sanity checks here that apply to both var and struct 
		   declarations 
		*/
		if(is_ctype(t->type) && !dv->ref) {

			if(dv->name[0]) {

				if(!instruct) {

					if(!sial_isxtern(t->typattr)) {

						sial_freesvs(vlist);
						sial_error("struct/union instances not supported, please use pointers");
					}

				} else if(sial_ispartial(t)) {

					sial_freesvs(vlist);
					sial_error("Reference to incomplete type");
				}
			}
		}
		if(dv->nbits) { 

			if(t->type != V_BASE) {

				sial_freesvs(vlist);
				sial_error("Bit fields can only be of integer type");

			}
			if(dv->idx) {

				sial_freesvs(vlist);
				sial_error("An array of bits ? Come on...");
			}
		}

		var=sial_newvar(dv->name);

		t->fct=dv->fct;
		sial_duptype(&var->v->type, t);
		sial_pushref(&var->v->type, dv->ref);

		var->dv=dv;

		TAG(var);

		if(t->type == V_STRING) {

			sial_setstrval(var->v, "");

		} 

		sial_setpos(&dv->pos);

		sial_enqueue(vlist, var);

		dv=dv->next;
	}
	sial_free(t);
	TAG(vlist);
	return vlist;
}

dvar_t*
sial_newdvar(node_t*v)
{
dvar_t*dv;

	dv=sial_alloc(sizeof(dvar_t));
	memset(dv, 0, sizeof(dvar_t));
	if(v) {
		dv->name=NODE_NAME(v);
		NODE_FREE(v);

	} else {

		dv->name=sial_alloc(1);
		dv->name[0]='\0';
	}
	dv->refcount=1;
	sial_setpos(&dv->pos);
	return dv;
}

dvar_t*
sial_dvarini(dvar_t*dv, node_t*init)
{
	dv->init=init;
	return dv;
}

dvar_t*
sial_dvarptr(int ref, dvar_t*dv)
{
	dv->ref+=ref;
	return dv;
}

dvar_t*
sial_dvaridx(dvar_t*dv, node_t*n)
{
	if(!dv->idx) {

		dv->idx=sial_alloc(sizeof(idx_t));
		dv->idx->nidx=0;
	}
	dv->idx->idxs[dv->idx->nidx++]=n;
	return dv;
}

dvar_t*
sial_dvarfld(dvar_t*dv, node_t*n)
{

	if(n) {

		value_t *va=sial_exenode(n);

		/* get the value_t for the bits */
		if(!va) dv->nbits=0;
		else {
			dv->nbits=unival(va);
			sial_freeval(va);
		}
		NODE_FREE(n);

	} else dv->nbits=0;

	dv->bitfield=1;
	return dv;
}

dvar_t*
sial_dvarfct(dvar_t*dv, var_t*fargs)
{
	dv->fct=1;
	dv->fargs=fargs;
	return dv;
}

dvar_t*
sial_linkdvar(dvar_t*dvl, dvar_t*dv)
{
dvar_t*v;

	/* need to keep declaration order for variable initialization */
	if(dv) {

		for(v=dvl; v->next; v=v->next);
		dv->next=0;
		v->next=dv;
	}
	return dvl;
}

idx_t *
sial_newidx(node_t*n)
{
idx_t *idx;

	if(!instruct) {

		sial_error("Array supported only in struct/union declarations");
	}
	idx=sial_alloc(sizeof(idx_t));
	idx->nidx=1;
	idx->idxs[0]=n;
	return idx;
}

idx_t *
sial_addidx(idx_t *idx, node_t*n)
{
	if(idx->nidx==MAXIDX) {

		sial_error("Maximum number of dimension is %d", MAXIDX);
	}
	idx->idxs[idx->nidx++]=n;
	return idx;
}

static svlist svs[S_MAXDEEP];
static glo *globs=0;
int svlev=0;

void
sial_refarray(value_t *v, int inc)
{
array_t*ap, *na;

	if(!v->arr) return;
	v->arr->ref+=inc;
	if(v->arr->ref == 0) {

		/* free all array element. */
		for(ap=v->arr->next; ap!=v->arr; ap=na) {

			na=ap->next;
			sial_freeval(ap->idx);
			sial_freeval(ap->val);
			sial_free(ap);
		}
		sial_free(v->arr);
		v->arr=0;

	} else {

		/* do the same to all sub arrays */
		for(ap=v->arr->next; ap!=v->arr; ap=na) {

			na=ap->next;
			sial_refarray(ap->val, inc);
		}
	}
		
}

void
sial_freedata(value_t *v)
{
	
	if(is_ctype(v->type.type) || v->type.type == V_STRING) {

		if(v->v.data) sial_free(v->v.data);
		v->v.data=0;

	}
	sial_refarray(v, -1);
}

void
sial_dupdata(value_t *v, value_t *vs)
{

	if(is_ctype(vs->type.type) || vs->type.type == V_STRING) {

		v->v.data=sial_alloc(vs->type.size);
		memmove(v->v.data, vs->v.data, vs->type.size);
	}
}

void
sial_freeval(value_t *v)
{
	if(!v) return;
	sial_freedata(v);
	sial_free(v);
}


void
sial_freevar(var_t*v)
{

	if(v->name) sial_free(v->name);
	sial_freeval(v->v);
	sial_freedvar(v->dv);
	sial_free(v);
}

void 
sial_enqueue(var_t*vl, var_t*v)
{
	v->prev=vl->prev;
	v->next=vl;
	vl->prev->next=v;
	vl->prev=v;
}

void
sial_dequeue(var_t*v)
{
	v->prev->next=v->next;
	v->next->prev=v->prev;
	v->next=v->prev=v;
}

/*
	This function is called to validate variable declaration.
	No array decalration for variables (this can only be checked in 
	sial_stat_decl() and sial_file_decl() usingthe idx field ofthe var struct.
	Same comment for nbits. Only in struct declarations.
*/
void
sial_validate_vars(var_t*svs)
{
var_t*v, *next;

	if(!svs) return;

	for(v=svs->next; v!=svs; v=next) {

		next=v->next;

		/* just remove extern variables */
		if(sial_isxtern(v->v->type.typattr)) {

			sial_dequeue(v);
			sial_freevar(v);

		} else {

			if(v->dv->idx) {

				sial_freesvs(svs);
				sial_error("Array instanciations not supported.");

			} 
			if(v->dv->nbits) {

				sial_freesvs(svs);
				sial_error("Syntax error. Bit field unexpected.");
			}
		}
	}
}

var_t*
sial_inlist(char *name, var_t*vl)
{
var_t*vp;

	if(vl) {

		for(vp=vl->next; vp!=vl; vp=vp->next) {

			if(!strcmp(name, vp->name)) {

				return vp;

			}

		}
	}
	return 0;
}

static var_t*apiglobs;

void
sial_setapiglobs(void)
{
	apiglobs=sial_newvlist();
	sial_add_globals(apiglobs);
}

static var_t*
sial_inglobs(char *name)
{
var_t*vp;
glo *g;

	for(g=globs; g; g=g->next) {

		if((vp=sial_inlist(name, g->vv))) return vp;
	}
	return 0;
}


void
sial_chkglobsforvardups(var_t*vl)
{
var_t*v;

	if(!vl) return;

	for(v=vl->next; v != vl; v=v->next) {

		var_t*vg;

		if(v->name[0] && (vg=sial_inglobs(v->name))) {

			/* if this is a prototype declaration then skip it */
			if(v->dv && v->dv->fct) continue;

			sial_rerror(&v->dv->pos, "Duplicate declaration of variable '%s', defined at %s:%d"
				, v->name, vg->dv->pos.file, vg->dv->pos.line);
		}
	}
}

/*
   This function scans a list of variable and looks for those that have not been initialized yet.
   Globals, statics and autos all get initialized through here.
*/
static void
sial_inivars(var_t*sv)
{
var_t*v;

	if(!sv) return;

	for(v=sv->next; v!=sv; v=v->next) {

		/* check if we need to initialize it */
		if(!v->ini && v->dv && v->dv->init) {

			value_t *val;
			srcpos_t pos;

			sial_curpos(&v->dv->pos, &pos);

			if((val=sial_exenode(v->dv->init))) {

				sial_chkandconvert(v->v, val);
				sial_freeval(val);
				v->ini=1;

			} else {

				sial_rwarning(&v->dv->pos, "Error initializing '%s'", v->name);
			}
			sial_curpos(&pos, 0);
		}
	}
}

/* return the last set of globals */
var_t*
sial_getcurgvar()
{
	if(!globs) return 0;
	return globs->vv;
}

void *
sial_add_globals(var_t*vv)
{
glo *ng=sial_alloc(sizeof(glo));

	sial_inivars(vv);
	ng->vv=vv;
	ng->next=globs;
	sial_chkglobsforvardups(vv);
	globs=ng;
	return ng;
}

void
sial_rm_globals(void *vg)
{
glo *g=(glo*)vg;

	if(globs) {

		if(globs==g) globs=g->next;
		else {

			glo *gp;

			for(gp=globs; gp; gp=gp->next) {

				if(gp->next==g) {

					gp->next=g->next;

				}

			}
		}
		sial_free(g);
	}
}



/*
	This is where we implement the variable scoping.
*/
var_t*
sial_getvarbyname(char *name, int silent, int local)
{
var_t*vp;
int i, aidx=0;
ull apiv;

	for(i=svlev-1; i>=0; i--) {

		if((vp=sial_inlist(name, svs[i].svs))) {

			return vp;
		}
		if(svs[i].type==S_AUTO && !aidx) aidx=i;

		/* when we get to the function we're finished */
		if(svs[i].type==S_FILE) break;
	}

	/* have'nt found any variable named like this one */
	/* first check the globals */
	if(!(vp=sial_inglobs(name))) {

		int off=0;

		/* check the API for a corresponding symbol */
		/* Jump over possible leading "IMG_" prefix */
		if(!strncmp(name, "IMG_", 4)) off=4;
		if(!local && API_GETVAL(name+off, &apiv)) {

			vp=sial_newvar(name);
			vp->ini=1;

			sial_defbtype(vp->v, apiv);
			vp->v->mem=apiv;

			/* put this on the global list */
			sial_enqueue(apiglobs, vp);
		}
		else {

			if(silent) return 0;
			sial_error("Unknown variable [%s]", name);
		}
	}
	return vp;
}

value_t *
sial_exists(value_t *vname)
{
char *name=sial_getptr(vname, char);

	return sial_defbtype(sial_newval(), (sial_getvarbyname(name, 1, 0) || sial_funcexists(name)));
}

/* get a new empty vlist */
var_t*
sial_newvlist()
{
var_t*p=sial_newvar("");
	TAG(p);
	TAG(p->name);
	return p;
}

/* this is called when we duplicate a list of automatic variables */
var_t*
sial_dupvlist(var_t*vl)
{
var_t*nv=(var_t*)sial_newvlist(); /* new root */
var_t*vp;

	for(vp=vl->next; vp !=vl; vp=vp->next) {

		var_t*v=sial_newvar(vp->name); /* new var_t*/

		v->dv=vp->dv;
		v->dv->refcount++;
		v->ini=vp->ini;
		sial_dupval(v->v, vp->v);

		/* we start with a new array for automatic variable */
		sial_refarray(v->v, -1);
		v->v->arr=0;
		sial_setarray(&v->v->arr);
		
		/* can't check ctypes for initialisation */
		if(is_ctype(v->v->type.type)) v->ini=1;
		sial_enqueue(nv, v);

	}
	return nv;
}

void
sial_addtolist(var_t*vl, var_t*v)
{
	if(!v->name[0] || !sial_inlist(v->name, vl)) {

		sial_enqueue(vl, v);

	} else {

		/* if this is a prototype declaration then skip it */
		if(v->dv && v->dv->fct) return;

		sial_error("Duplicate declaration of variable %s", v->name);
	}
}

static void
sial_chkforvardups(var_t*vl)
{
var_t*v;

	if(!vl) return;

	for(v=vl->next; v!=vl; v=v->next) {

		var_t*v2=v->next;

		for(v2=v->next; v2!=vl; v2=v2->next) {

			if(v2->name[0] && !strcmp(v->name, v2->name)) {

				sial_rerror(&v2->dv->pos, "Duplicate declaration of variable '%s'", v->name);

			}
		}
	}
}

static int takeproto=0;
void sial_settakeproto(int v) { takeproto=v; }


/* 
	This function scans a new list of declared variables
	searching for static variables.
*/
void
sial_addnewsvs(var_t*avl, var_t*svl, var_t*nvl)
{
var_t*v;

	if(nvl) {

		for(v=nvl->next; v!=nvl; ) {

			var_t*next;

			/* save next before sial_enqueue() trashes it ... */
			next=v->next;

			/* if this is a external variable or prototype function declaration 
			   skip it */
			if((!takeproto && v->dv->fct && !v->dv->ref) || sial_isxtern(v->v->type.typattr)) {

				v=next;
				continue;
			}

			if(sial_isstatic(v->v->type.typattr)) {

				sial_addtolist(svl, v);

			} else {

				sial_addtolist(avl, v);
			}
			/* with each new variables check for duplicate declarations */
			sial_chkforvardups(avl);
			sial_chkforvardups(svl);

			v=next;
		}
		/* discard nvl's root */
		sial_freevar(nvl);
	}
}

int
sial_addsvs(int type, var_t*sv)
{
int curlev=svlev;

	if(svlev==S_MAXDEEP) {

		sial_error("Svars stack overflow");

	} else {

		svs[svlev].type=type;
		svs[svlev].svs=sv;
		svlev++;

		/* perform automatic initializations */
		sial_inivars(sv);
		
		/* if S_FILE then we are entering a function so start a newset of
		   stack variables */
		if(type == S_FILE ) {

			(void)sial_addsvs(S_AUTO, (var_t*)sial_newvlist());

		}
	}
	return curlev;
}

void
sial_add_statics(var_t*var)
{
int i;

	for(i=svlev-1;i>=0;i--) {

		if(svs[i].type==S_FILE ) {

			if(svs[i].svs)
				sial_enqueue(svs[i].svs, var);
			else
				svs[i].svs=var;
			return;

		}
	}
	sial_rwarning(&var->dv->pos, "No static context for var %s.", var->name);
}

void sial_freesvs(var_t*v)
{
var_t*vp;

	for(vp=v->next; vp != v; ) {

		var_t*vn=vp->next;

		sial_freevar(vp);

		vp=vn;
	}
	sial_freevar(v);
}

int
sial_getsvlev() { return svlev; }

/* reset the current level of execution and free up any automatic
   variables. */
void
sial_setsvlev(int newlev)
{
int lev;

	for(lev=svlev-1; lev>=newlev; lev--) {

			if(svs[lev].type==S_AUTO) {

				sial_freesvs(svs[lev].svs);

			}

	}
	svlev=newlev;
}

/*
	called by the 'var in array' bool expression.
*/
int
sial_lookuparray(node_t*vnode, node_t*arrnode)
{
value_t *varr=NODE_EXE(arrnode);
array_t*ap, *apr=varr->arr;
value_t *val;
int b=0;

	val=NODE_EXE(vnode);

	if(apr) {

		for(ap=apr->next; ap != apr; ap=ap->next) {

			if(VAL_TYPE(ap->idx) == VAL_TYPE(val)) {

				switch(VAL_TYPE(val)) {
				case V_STRING:	b=(!strcmp(ap->idx->v.data, val->v.data)); break;
				case V_BASE:	b=(unival(ap->idx)==unival(val)); break;
				case V_REF:	
					if(sial_defbsize()==4) 
						b=(ap->idx->v.ul==val->v.ul);
					else
						b=(ap->idx->v.ull==val->v.ull);
				break;
				default:
					sial_rerror(&vnode->pos, "Invalid indexing type %d", VAL_TYPE(val));
				}
				if(b) break;
			}

		}
	}
	sial_freeval(val);
	sial_freeval(varr);
	return b;
}

/*
	The actual for(i in array) core...
*/
void
sial_walkarray(node_t*varnode, node_t*arrnode, void (*cb)(void *), void *data)
{
value_t *v;
value_t *av;
array_t*ap, *apr;

	sial_setini(varnode);
	v=NODE_EXE(varnode);

	av=NODE_EXE(arrnode);

	if(av->arr) {

		for(apr=av->arr, ap=apr->next; ap != apr; ap=ap->next) {

			/* we set the value_t of the variable */
			sial_setval(v,ap->idx);

			(cb)(data);

		}
	}
	sial_freeval(v);
	sial_freeval(av);
}

/* scan the current array for a specific index and return value_t 
   XXX should use some hashing tables here for speed and scalability */
array_t*
sial_getarrval(array_t**app, value_t *idx)
{
array_t*ap, *apr;

	/* sial_setarray(app); AAA comment out */
	apr=*app;

	for(ap=apr->next; ap != apr; ap=ap->next) {

		if(ap->idx->type.type == idx->type.type) {

		int b=0;

			switch(idx->type.type) {
			case V_STRING: b=(!strcmp(ap->idx->v.data, idx->v.data));
			break;
			case V_BASE: b=(unival(ap->idx)==unival(idx));
			break;
			case V_REF:	
				if(sial_defbsize()==4) 
					b=(ap->idx->v.ul==idx->v.ul);
				else
					b=(ap->idx->v.ull==idx->v.ull);
			break;
			default:
				sial_error("Invalid index type %d", idx->type.type);
			}

			if(b) {

				return ap;

			}
		}
	}

	/* we have not found this index, create one */
	ap=(array_t*)sial_calloc(sizeof(array_t));
	ap->idx=sial_makebtype(0);
	sial_dupval(ap->idx, idx);

	/* just give it a int value_t of 0 for now */
	ap->val=sial_makebtype(0);

	/* we must set the same refenrence number as the
	   upper level array_t*/
	ap->val->arr->ref=apr->ref;

	/* link it in */
	ap->prev=apr->prev;
	ap->next=apr;
	apr->prev->next=ap;
	apr->prev=ap;
	ap->ref=0;
	return ap;
}

value_t *
sial_intindex(value_t *a, int idx)
{
value_t *v=sial_makebtype(idx);
array_t*ap=sial_getarrval(&a->arr, v);

	sial_dupval(v, ap->val);
	return v;
}

value_t *
sial_strindex(value_t *a, char *idx)
{
value_t *v=sial_makestr(idx);
array_t*ap=sial_getarrval(&a->arr, v);

	sial_dupval(v, ap->val);
	return v;
}


void
sial_setarrbval(array_t*a, int val)
{
	sial_defbtype(a->val, (ull)val);
}

array_t*
sial_addarrelem(array_t**arr, value_t *idx, value_t *val)
{
array_t*na;

	na=sial_getarrval(arr, idx);

	/* copy new val over */
	sial_freeval(na->val);
	na->val=val;

	return na;
}

/* insert a variable at the end of the list */
static void
sial_varinsert(var_t*v)
{
int i;

	for(i=svlev-1;i>=0;i--) {

		if(svs[i].type==S_AUTO) {

			sial_enqueue(svs[i].svs, v);
			break;
		}
	}
}

/* Dupicate and add a set of variables. Used to setup a function execution.
   The new veriables are the actual parameters of the function so we mark them
   As being initialized.
*/
void
sial_add_auto(var_t*nv)
{
	nv->ini=1;
	sial_varinsert(nv);
}

void
sial_valindex(value_t *var, value_t *idx, value_t *ret)
{
	if(is_ctype(idx->type.type)) {

		sial_error("Invalid indexing type");

	} else {

		array_t*a;

		a=sial_getarrval(&var->arr, idx);

		/* this is the first level of indexing through a variable */
		sial_dupval(ret, a->val);
		ret->set=1;
		ret->setval=a->val;
	}
}

void
sial_addvalarray(value_t*v, value_t*idx, value_t*val)
{
	sial_addarrelem(&v->arr, idx, val);
	sial_freeval(idx);
}

static void
prtval(value_t*v)
{
value_t*fmt=sial_makestr("%?");

	sial_printf(fmt, v, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0);
	sial_freeval(fmt);
}

static void
prlevel(char *name, value_t*root, int level)
{
ARRAY_S *arr;

	for(arr=root->arr->next; arr != root->arr; arr=arr->next) {

		printf("%*s%s[", level*3, "", name);
		prtval(arr->idx);
		printf("]=");
		prtval(arr->val);
		printf("\n");
		prlevel(name, arr->val, level+1);
	}
}

/* sial_prarr builtin */
value_t*
sial_prarr(value_t*vname, value_t*root)
{
char *name=sial_getptr(vname, char);
	printf("%s=", name);
	prtval(root);
	printf("\n");
	prlevel(name, root, 1);
	return sial_makebtype(0);
}

var_t*
sial_newvar(char *name)
{
var_t*v=sial_calloc(sizeof(var_t));
char *myname=sial_alloc(strlen(name)+1);

	TAG(myname);
	strcpy(myname,name);
	v->name=myname;
	v->v=sial_makebtype(0);
	v->v->setval=v->v;
	v->next=v->prev=v;
	return v;
}


typedef struct {
	node_t*n;
	char name[1];
} vnode_t ;

static int insizeof=0;
void sial_setinsizeof(int v) { insizeof=v;}

value_t *
sial_exevar(void *arg)
{
vnode_t *vn = arg;
value_t *nv;
var_t*curv;
srcpos_t pos;

	sial_curpos(&vn->n->pos, &pos);

	if(!(curv=sial_getvarbyname(vn->name, 0, 0))) {

		sial_error("Oops! Var ref1.[%s]", vn->name);

	}
	if(!curv->ini && !insizeof) {

		sial_error("Variable [%s] used before being initialized", curv->name);

	}

	nv=sial_newval();
	sial_dupval(nv,curv->v);
	nv->set=1;
	nv->setval=curv->v;
	nv->setfct=sial_setfct;

	sial_curpos(&pos, 0);

	return nv;
}

/* make sure a variable is flaged as being inited */
void
sial_setini(node_t*n)
{
	if((void*)n->exe == (void*)sial_exevar) {

		var_t*v=sial_getvarbyname(((vnode_t*)(n->data))->name, 0, 0);
		v->ini=1;
	}
}


/* get the name of a function through a variable */
char *
sial_vartofunc(node_t*name)
{
char *vname=NODE_NAME(name);
value_t *val;

	/* if the nore is a general expression, then vname is 0 */
	if(!vname) {

		val=sial_exenode(name);

	} else  {

		var_t*v;
		
		v=sial_getvarbyname(vname, 1, 1);
		if(!v) return vname;
		val=v->v;
	}

	switch(val->type.type)
	{
		case V_STRING:
		{
		char *p=sial_alloc(val->type.size+1);
			/* return the value_t of that string variable */
			strcpy(p, val->v.data);
			sial_free(vname);
			return p;
		}
		default:
			/* return the name of the variable itself */
			sial_error("Invalid type for function pointer, expected 'string'.");
			return vname;
	}
}

char *
sial_namevar(vnode_t*vn)
{
char *p;

	p=sial_strdup(vn->name);
	TAG(p);
	return p;
}

static void
sial_freevnode(vnode_t*vn)
{
	sial_free(vn);
}

/*
        create or return existing variable node.
*/      
node_t*
sial_newvnode(char *name)
{
node_t*n=sial_newnode();
vnode_t*vn=sial_alloc(sizeof(vnode_t)+strlen(name)+1);

	TAG(vn);

	strcpy(vn->name, name);
	n->exe=(xfct_t)sial_exevar;
	n->free=(ffct_t)sial_freevnode;
	n->name=(nfct_t)sial_namevar;
	n->data=vn;
	vn->n=n;

	sial_setpos(&n->pos);

        return n;
}

#define TO (*to)
#define FRM (*frm)

void
sial_cparrelems(array_t**to, array_t**frm)
{
array_t*ap;

	if(FRM) {

		sial_setarray(to);
		for(ap=FRM->next; ap!=FRM; ap=ap->next) {

			array_t*na=sial_calloc(sizeof(array_t));

			/* copy value_ts */
			sial_dupval(na->idx, ap->idx);
			sial_dupval(na->val, ap->val);

			/* link it in */
			na->prev=TO->prev;
			na->next=TO;
			TO->prev->next=na;
			TO->prev=na;
			na->ref=1;

			/* copy that branch */
			sial_cparrelems(&na->val->arr, &ap->val->arr);
		}
	}
}

