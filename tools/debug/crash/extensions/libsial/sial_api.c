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
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#include <stdio.h>
#include <setjmp.h>
#include <pwd.h>
#include <string.h>

/* here we do some caching of the information. This can have a speedup effect
   since it limits the number of accesses we do the dwarf (or whatever type) db that
   drives the type and symbols information 
 */

static stinfo_t slist={"root"};

/* running key to new structures */
static ull nextidx=0, abitype=ABI_MIPS;
#define LOCALTYPESBASE 0x8000000000000000ll
static ull sial_nextidx(void) { return LOCALTYPESBASE+nextidx++; }

/* this set of function is used to cleanup tdefs after their use.
   Trailing tdefs can be combersome. Trailing struct/union/enum get new idx
   each time and are not a trouble */
static stinfo_t*tag=0;
void
sial_tagst(void)
{
	tag=slist.next;
}

void
sial_flushtdefs(void)
{
stinfo_t*st=slist.next;
stinfo_t*last=&slist;

	while(st != tag) {

		stinfo_t*next=st->next;

		if(st->ctype.type==V_TYPEDEF && st->idx & LOCALTYPESBASE) {

			sial_free(st->name);
			sial_free(st);
			last->next=next;

		} else last=st;

		st=next;

	}
	tag=0;
}

static stinfo_t*
sial_getst(char *name, int type)
{
stinfo_t*tst;

	for(tst=slist.next; tst; tst=tst->next) {

		if(tst->ctype.type == type && tst->name && ! strcmp(tst->name, name)) {

			return tst;	
		}
	}
	return 0;
}

#if 0
Not used yet.
static void
sial_rmst(stinfo_t*rst)
{
stinfo_t*st=slist.next;
stinfo_t*last=&slist;

	while(st) {

		if(st==rst) {

			last->next=st->next;
			sial_free(st->name);
			sial_free(st);

			return;

		} 

		last=st;
		st=st->next;
	}
}
#endif

stinfo_t*
sial_getstbyindex(ull idx, int type)
{
stinfo_t*tst;

	for(tst=slist.next; tst; tst=tst->next) {

		if(tst->ctype.type == type && tst->idx == idx) {

			return tst;	
		}
	}
	return 0;
}

static void
sial_addst(stinfo_t*st)
{
stinfo_t*tst;

	tst=slist.next;
	slist.next=st;
	st->next=tst;
}

typedef struct neg_s {
    struct neg_s *next;
    char *name;
} neg_t;

static neg_t *nlist=0;

void
sial_addneg(char *name)
{
neg_t *neg;

    neg=sial_alloc(sizeof *neg);
    neg->name=sial_strdup(name);
    neg->next=nlist;
    nlist=neg;
}

int 
sial_isneg(char *name)
{
neg_t *neg;

    for(neg=nlist; neg; neg=neg->next) 
        if(!strcmp(neg->name, name)) return 1;
    return 0;
}

/*
	This function is called by sial_vardecl() when the typedef storage class
	as been specified. In which case we need to create new typedefs not variables.
*/
void
sial_tdef_decl(dvar_t*dv, type_t*t)
{
	while(dv) {

		dvar_t*next;

		stinfo_t*st=sial_calloc(sizeof(stinfo_t));

		if(dv->nbits) sial_error("No bits fields for typedefs");
		if(dv->idx) {

			/* we change a 'typedef type var[n];' into a 'typedef type_t*var;' */
			sial_freeidx(dv->idx);
			dv->idx=0;
			dv->ref++;
		}
#if 0
At this time we do not give any error messages or warnings.
If a type is redefined within a single file that will means
problem for the user put this is not a full blown C compiler.

		{
		type_t*t=sial_newtype();

			if(API_GETCTYPE(V_TYPEDEF, dv->name, t)) {

				sial_warning("Typedef %s already defined in image, redefinition ignored",
					dv->name);
			}
			sial_freetype(t);
		}
#endif
		t->typattr &= ~sial_istdef(t->typattr);
		sial_duptype(&st->rtype, t);
		sial_pushref(&st->rtype, dv->ref);
		st->name=dv->name;
		dv->name=0;
		st->idx=sial_nextidx();
		st->ctype.type=V_TYPEDEF;

		sial_addst(st);
		
		next=dv->next;
		dv->next=0;
		sial_freedvar(dv);
		dv=next;
	}
}

int
sial_ispartial(type_t*t)
{
stinfo_t*st=sial_getstbyindex(t->idx, t->type);

	if(!st) {

		sial_error("Oops sial_ispartial");
	}
	return !st->all;
}

char *
sial_gettdefname(ull idx)
{
stinfo_t*tst=sial_getstbyindex(idx, V_TYPEDEF);

	if(tst) return tst->name;
	else return 0;
}

static int init=0;
static void
sial_chkinit(void)
{
	if(!init) {

		sial_error("Sial Package not initialized");

	}
}

void
sial_getmem(ull kp, void *p, int n)
{
	sial_chkinit();
	if(!API_GETMEM(kp, p, n)) {

		sial_error("Error on read from 0x%llx for %d", kp, n);

	}
}

void
sial_putmem(ull kp, char *p, int n)
{
	sial_chkinit();
	if(!API_PUTMEM(kp, p,n)) {

		sial_error("Error on write at 0x%llx for %d", kp, n);

	}
}

void
sial_partialctype(int type, char *name)
{
stinfo_t*st;

	/* check first if we have a partial of that type
	   already in progress (after a forward declaration) */
	if((st=sial_getst(name, type))) {

		/* if it's complete we need to start a new one */
		if(!st->all) return;

	}
	st=sial_calloc(sizeof(stinfo_t));
	st->name=sial_strdup(name);
	st->ctype.type=type;
	st->all=0;
	st->ctype.idx=st->idx=sial_nextidx();
	sial_addst(st);
}

void
sial_startctype_named(int type, char *name)
{
stinfo_t*st;

	/* if no partial yet start one */
	if(!(st=sial_getst(name, type)) || st->all)
		sial_partialctype(type, name);
}

void
sial_startctype(int type, node_t*namen)
{
	sial_startctype_named(type, NODE_NAME(namen));
}

int
sial_samectypename(int type, ull idx1, ull idx2)
{
stinfo_t*st1, *st2;

	if((st1=sial_getstbyindex(idx1, type)) &&
	   (st2=sial_getstbyindex(idx2, type))) {

		// check names
                if(!strcmp(st1->name, st2->name)) return 1;
                
                // check all members and sizes in order
                // unamed ctypes can end up here too...
                if(st1->stm) {
                    stmember_t *m1=st1->stm, *m2=st2->stm;
                    while(m1 && m2) {
                        if(strcmp(m1->m.name, m2->m.name)) break;
                        if(m1->m.offset != m2->m.offset ) break;
                        if(m1->m.size != m2->m.size ) break;
                        m1=m1->next;
                        m2=m2->next;
                    }
                    if(!m1 && !m2) return 1;
                }
                else if(st1->enums) {
                
                    enum_t *e1=st1->enums, *e2=st2->enums;
                    while(e1 && e2) {
                        if(strcmp(e1->name, e2->name)) break;
                        if(e1->value != e2->value ) break;
                        e1=e1->next;
                        e2=e2->next;
                    }
                    if(!e1 && !e2) return 1;
                }

	}
	return 0;
}

#define VOIDIDX 0xbabebabell
type_t*
sial_getvoidstruct(int ctype)
{
type_t*bt=sial_newtype();

	bt->type=ctype;
	bt->idx=VOIDIDX;
	bt->size=0;
	bt->ref=0;
	return bt;
}

void sial_fillst(stinfo_t *st);

/* Just in case this is an unnamed structure member then we need
   to add it to the slist ourselves using the index. sial_getctype() would 
   not found it.
*/
static void
sial_memstinfo(stmember_t *stm, char *pname)
{
int type=stm->type.ref?stm->type.rtype:stm->type.type;

	if(is_ctype(type)) {

		if(!sial_getstbyindex(stm->type.idx, type)) {

			stinfo_t*st=sial_calloc(sizeof(stinfo_t));

			sial_duptype(&st->ctype, &stm->type);
			st->ctype.type=type;
                        // dereference level is attached (wrongly) to type...
                        // zap it
                        st->ctype.ref=0;
			st->idx=st->ctype.idx;
			st->name=sial_strdup(pname);
			sial_addst(st);
		}
	}
}

void
sial_fillst(stinfo_t *st)
{
char *mname=0;
ull idx=st->ctype.idx, lidx=0;
stmember_t *stm=sial_calloc(sizeof(stmember_t)), **last=&st->stm;
char *pname;

        sial_dbg_named(DBG_STRUCT, st->name, 2, "Fill St started [local=%d].\n", (idx & LOCALTYPESBASE) ? 1 : 0);
	/* bail out if this is local type */
	if(idx & LOCALTYPESBASE) return;

	if(st->stm) sial_error("Oops sial_fillst!");

	while((pname=API_MEMBER(mname, idx,  &stm->type, &stm->m, &lidx))) {

                sial_dbg_named(DBG_STRUCT, st->name, 2, "member '%s'\n", pname);
		sial_memstinfo(stm, pname);
		stm->next=0;
		*last=stm;
		last=&stm->next;
		mname="";
		stm=sial_calloc(sizeof(stmember_t));
		if(pname[0]) sial_free(pname);
	}
	st->all=1;
	sial_free(stm);
}

type_t*
sial_getctype(int ctype, char *name, int silent)
{
stinfo_t *st;
type_t *t=sial_newtype();

	sial_chkinit();
        sial_dbg_named(DBG_TYPE, name, 2, "getctype [%d] [%s] [s=%d]\n", ctype, name, silent);
	if(!(st=sial_getst(name, ctype))) {

                sial_dbg_named(DBG_TYPE, name, 2, "getctype [%s] not found in cache\n", name);
                if(silent && sial_isneg(name)) return 0;

		st=sial_calloc(sizeof(stinfo_t));
		if(!API_GETCTYPE(ctype, name,  &st->ctype)) {

                        sial_dbg_named(DBG_TYPE, name, 2, "[%s] not found in image\n", name);
			sial_free(st);
			sial_freetype(t);
                        // add any tdef to the neg list
                        if(ctype == V_TYPEDEF) sial_addneg(name);
			if(silent) return 0;
			/* we fill a partial structure for this one
			   assuming it will be defined later. This is to permit cross
			   referencing of structures, self referencing of structure, and 
			   undefined structure (opaque structures) irix: see types.c : 
			   __pasid_opaque  
			*/
                        sial_dbg_named(DBG_TYPE, name, 2, "[%s] creating partial type\n", name);
			sial_partialctype(ctype, name);
			return sial_getctype(ctype, name, silent);
		}
                sial_dbg_named(DBG_TYPE, name, 2, "getctype [%s] found in image\n", name);
		st->name=sial_alloc(strlen(name)+1);
		strcpy(st->name, name);
		st->stm=0;
		st->idx=st->ctype.idx;
		st->all=1;
		sial_addst(st);
		/*
		  if this is a typedef then drill down to the real type
		  and make sure it is in the cache. That's what we return

		  Bug cure: this would fail:

			struct sv {
        			int i;
			};
			struct foo {
        			sv_t    ms_sv;
			};

			Because the rtype index returned by API_GETRTYPE() is the die offset
			in the image. If We already have redefine the real type locally the
			call to sial_getctype() will not have a matching index later when we
			don't find the index in the type cache.

			So we track the real index with ridx. This also ensures that 
			redefining a struct locally and using a typetef from the image will actualy
			end up pointing to the local struct and not the image struct.
		*/
		if(ctype == V_TYPEDEF) {

			char *tname;
			int itype;

			tname=API_GETRTYPE(st->idx, t);

			if(t->type==V_REF) itype=t->rtype;
			else itype=t->type;

			/* if it's a named struct, enum or union then make sure we have it in the cache */
			if(is_ctype(itype) && tname && tname[0] && 
				(strcmp(tname,"struct ") != 0 
				    && strcmp(tname,"union ") != 0
				    && strcmp(tname,"enum ") != 0)) {

				sial_freetype(t);
				t=sial_getctype(itype, tname, silent);

				/* in IRIX we have a typedef struct __pasid_opaque* aspasid_t;
				   w/ no struct __pasid_opaque defined. The aspasid_t ends
				   up being used as a "named" void *. So we force a void * here */
				/* XXX: This should at least generate a warning */
				if(!t) {
				    sial_warning("voidstruct created (%s)\n", tname);
				    t=sial_getvoidstruct(itype);
				}
			} else if (is_ctype(itype) || itype == V_ENUM) {
				
				/* for unnamed structs, unions and enums create an entry */
	                        stinfo_t*st=sial_calloc(sizeof(stinfo_t));

                       		sial_duptype(&st->ctype, t);
                        	st->idx=t->idx;
                        	st->name=sial_strdup("");
				sial_fillst(st);
                        	sial_addst(st);
			}
			sial_duptype(&st->rtype, t);
                        
		} else if(is_ctype(ctype)) {

			/* get all member info now ! */
			sial_fillst(st);
		}
	}
        else sial_dbg_named(DBG_TYPE, name, 2, "getctype [%s] found in cache\n", name);

	if(ctype == V_ENUM || (ctype == V_TYPEDEF && st->rtype.type == V_ENUM)) {
	    st->enums=API_GETENUM(name);
	    sial_pushenums(st->enums);
	}
	if(ctype==V_TYPEDEF) sial_duptype(t, &st->rtype);
	 else sial_duptype(t, &st->ctype);

	return t;
}

type_t*
sial_newctype(int ctype, node_t*n)
{
type_t*t;
char *name;

	t=sial_getctype(ctype, name=NODE_NAME(n), 0);
	NODE_FREE(n);
	sial_free(name);
	return t;
}

/*
	We don't use the type to point back to get the typedef name.
	The type is now the real type not the type for the typedef.
	So we keep a running sting of the last name variable name
	the parser found and use that.
	5/23/00
*/
node_t*
sial_tdeftovar(type_t*td)
{
char *sial_lastvar(void);
char *name=sial_lastvar();

	sial_free(td);
	return sial_newvnode(name);
}

/*
	Check to see if a cached member info is available
*/
static stmember_t*
sial_getm(char *name, type_t*tp, stinfo_t**sti)
{
ull idx=tp->idx;
stinfo_t*st;
stmember_t*stm;

	for(st=slist.next; st; st=st->next) {

		if(st->idx == idx) {

			*sti=st;

			if(!st->stm) sial_fillst(st);

			for(stm=st->stm; stm; stm=stm->next) {


				if(!strcmp(stm->m.name, name)) {

					return stm;

				}
			}
		}
	}
	return 0;
}

value_t *
sial_ismember(value_t*vp, value_t*vm)
{
char *name=sial_getptr(vm, char);
int ret=0;
stinfo_t*st;

	if(sial_getm(name, &vp->type, &st)) ret=1;

	return sial_defbtype(sial_newval(), ret);
}

/* XXX this entire stuff could very well be machine specific ... */
static int
sial_getalign(type_t*t)
{
	/* this is a custome type deal w/ it */
	if(t->type == V_BASE) {

		int n;

		/* Intel 386 ABI says that double values align on 4 bytes */
		if(abitype==ABI_INTEL_X86) n=((t->size>4)?4:t->size);
		else  n=t->size;
		return n*8;
	}
	if(t->type == V_REF) {
		/*
		 * This is an array but if there are additional references
		 * (>1) it is an array of pointers. In that case the pointer
		 * alignment has to be used.
		 */
		if(t->idxlst && t->ref == 1) {
			int ret;

			sial_popref(t, 1);
			ret=sial_getalign(t);
			sial_pushref(t, 1);
			return ret;
		}
		return sial_defbsize()*8;
	}
	/* alignment of a struct/union is on the largest of it's member or
	   largest allignment of sub structures */
	if(is_ctype(t->type)) {

		stinfo_t*st;
		stmember_t*sm;
		int maxallign=0;

		/* if this is a image type then let the api tell us */
		if(!(t->idx & LOCALTYPESBASE)) {

			return API_ALIGNMENT(t->idx)*8;

		}
	
		if(!(st=sial_getstbyindex(t->idx, t->type))) {

			sial_error("Oops sial_getalign");
		}

		for(sm=st->stm; sm; sm=sm->next) {

			int a=sial_getalign(&sm->type);

			if(a > maxallign) maxallign=a;

		}

		return maxallign;

	}
	/* other types shoudl not be part of a ctype declaration ... */
	sial_error("Oops sial_getalign2!");
	return 0;
}

static stinfo_t*
sial_chkctype(int ctype, char *name)
{
stinfo_t*sti;

	if(name) {

		/* we should already have a partial structure on the stack */
		sti=sial_getst(name, ctype);

#if 0
At this time I choose not to give any warning.
Structure redefinition is a normal part of include files... 

		/* We give a warning message for redefined types */
		{
		type_t*t=sial_newtype();

			if(API_GETCTYPE(ctype, name, t)) {

				sial_warning("%s %s redefinition", sial_ctypename(ctype), name);
			}
			sial_freetype(t);
		}
#endif

		if(sti->all) {

			sial_error("Oops sial_ctype_decl");
		}

		sial_free(name);

	} else {

		sti=sial_alloc(sizeof(stinfo_t));
		sti->name=0;
		sti->idx=sial_nextidx();
		sial_addst(sti);
	}
	return sti;
}

/*
	This function is used to create new enum types.
	The syntax for enum is:
	enum ident {
		ident [= int],
		[ident [= int] ] ...
	};
	So we check for an assign value and is it exists then 
	we reset the counter to it.
	This is the way the mips compiler does it. Which migt be
	the right way or not, although I fail to see why it's done
	that way.

	So enum foo {
		a,
		b,
		c=0,
		d
	};

	Wil yield values :

	a=0
	b=1
	c=0
	c=1
*/
enum_t*
sial_add_enum(enum_t*ep, char *name, int val)
{
enum_t *epi, *nep=sial_alloc(sizeof(enum_t));

	nep->name=name;
	nep->value=val;
	nep->next=0;
	if(!ep) return nep;
	epi=ep;
	while(ep->next) ep=ep->next;
	ep->next=nep;
	return epi;
}
	
type_t*
sial_enum_decl(int ctype, node_t*n, dvar_t*dvl)
{
dvar_t*dv=dvl, *next;
int counter=0;
stinfo_t*sti;
enum_t *ep=0;
char *name=n?NODE_NAME(n):0;
type_t *t;

	if(n) sial_startctype(ctype, n);
	sti=sial_chkctype(ctype, name);

	while(dv) {

		int val;

		/* evaluate an assignment ? */
		if(dv->init) {

			value_t *v=sial_exenode(dv->init);

			if(!v) {

				sial_rerror(&dv->pos, "Syntax error in enum expression");

			} else if(v->type.type != V_BASE) {

				sial_rerror(&dv->pos, "Integer expression needed");
			}

			val=sial_getval(v);
			counter=val+1;
			sial_freeval(v);

		} else {

			val=counter++;
		}

		ep=sial_add_enum(ep, dv->name, val);

		next=dv->next;
		dv->next=0;
		dv->name=0;
		sial_freedvar(dv);
		dv=next;
	}
	sti->enums=ep;

	/* now we push the values in the defines */
	sial_pushenums(sti->enums);

	/* we return a simple basetype_t*/
	/* after stahing the idx in rtype */
	t=sial_newbtype(INT);
	t->rtype=sti->idx;
	t->typattr |= sial_isenum(-1);
		
	return t;
	
}

/*
	The next functions are used to produce a new type
	and make it available throught the local cache.
	This enables custom type definitions on top of the
 	ctypes defined in the object symbol tables.

	There is one function per suported architechture.

*/
/* macro for alignment to a log2 boundary */
#define Alignto(v, a) (((v) + (a) -1) & ~((a)-1))
/*
	The algorith complies with the SysV mips ABI
*/
type_t*
sial_ctype_decl(int ctype, node_t*n, var_t*list)
{
type_t*t;
stinfo_t*sti;
stmember_t **mpp;
var_t*v;
int bits_left, bit_alignment;
int maxbytes, alignment, nextbit;
char *name=n?NODE_NAME(n):0;

	if(list->next==list) {

		sial_error("Empty struct/union/enum declaration");
	}

	t=sial_newbtype(0);
	sti=sial_chkctype(ctype, name);
	t->type=sti->ctype.type=ctype;
	t->idx=sti->ctype.idx=sti->idx;
	sti->stm=0;
	mpp=&sti->stm;

#if LDEBUG
printf("\n%s %s\n", ctype==V_STRUCT?"Structure":"Union", name ? name : "");
#endif

	/* these are the running position in the structure/union */
	nextbit=0;	/* next bit open for business */
	alignment=0;	/* keeps track of the structure alignment
			   Mips ABI says align to bigest alignment of
			   all members of the struct/union. Also
			   unamed bit fields do not participate here. */
	maxbytes=0;	/* tracking of the maximum member size for union */

	for(v=list->next; v!=list; v=v->next) {

		stmember_t*stm=sial_calloc(sizeof(stmember_t));
		dvar_t*dv=v->dv;
		int nbits;

		stm->m.name=sial_strdup(v->name);
		sial_duptype(&stm->type, &v->v->type);

		/* if this member is a bit filed simply use that */
		if(dv->bitfield) {

			nbits=dv->nbits;

			/* aligment is the size of the declared base type size */
			bit_alignment=v->v->type.size*8;

			if(nbits > bit_alignment) {

				sial_error("Too many bits for specified type");
			}

			/* For unamed bit field align to smallest entity */
			/* except for 0 bit bit fields */
			if(!dv->name[0] && nbits) {

				bit_alignment=((nbits+7)/8)*8;

			} 

			/* We compute the number of bits left in this entity */
			bits_left = bit_alignment - (nextbit%bit_alignment);

			/* 0 bits means, jump to next alignement unit anyway 
			   if not already on such a boundary */
			if(!nbits && (bits_left != bit_alignment)) nbits=bits_left;

			/* Not enough space ? */
			if(nbits > bits_left) {

				/* jump to next start of entity */
				nextbit += bits_left;

			}

			/* update member information */
			stm->m.offset=(nextbit/bit_alignment)*v->v->type.size;
			stm->m.fbit=nextbit % bit_alignment;
			stm->m.nbits=nbits;
			stm->m.size=v->v->type.size;
#if LDEBUG
			printf("    [%s] Bit member offset=%d, fbit=%d, nbits=%d\n", stm->m.name, stm->m.offset,  stm->m.fbit, stm->m.nbits);
#endif
			/* an unamed bit field does not participate in the alignment value */
			if(!dv->name[0]) {
	
				bit_alignment=0;

				/* reset size so that it does not have affect in sial_getalign() */
				stm->type.size=1;
			}

		} else {

			int nidx=1;

			if(dv->idx) {

				int i;

				/* flag it */
				stm->type.idxlst=sial_calloc(sizeof(int)*(dv->idx->nidx+1));

				/* multiply all the [n][m][o]'s */
				for(i=0;i<dv->idx->nidx;i++) {

					value_t *vidx;
					ull idxv;

					vidx=sial_exenode(dv->idx->idxs[i]);
					if(!vidx) {

						sial_error("Error while evaluating array size");
					}
					if(vidx->type.type != V_BASE) {

						sial_freeval(vidx);
						sial_error("Invalid index type");

					}

					idxv=sial_getval(vidx);
					sial_freeval(vidx);

					stm->type.idxlst[i]=idxv;

					nidx *= idxv;
				}
			

			}

			/* the number of bits on which this item aligns itself */
			bit_alignment=sial_getalign(&stm->type);

			/* jump to this boundary */
			nextbit = Alignto(nextbit,bit_alignment);


			if(stm->type.ref - (dv->idx?1:0)) {

				nbits=nidx*sial_defbsize()*8;

			} else {

				nbits=nidx*stm->type.size*8;
			}

			if(abitype==ABI_INTEL_X86) {

				int pos=nextbit/8;

				pos = (pos & 0xfffffffc) + 3 - (pos & 0x2);
				stm->m.offset=pos;

			} else {

				stm->m.offset=nextbit/8;
			}
			stm->m.nbits=0;
			stm->m.size=nbits/8;
#if LDEBUG
printf("    [%s] Mmember offset=%d, size=%d size1=%d nidx=%d\n", stm->m.name, stm->m.offset, stm->m.size, stm->type.size, nidx);
#endif

		}

		if(ctype==V_STRUCT) nextbit+=nbits;
		     /* Union members overlap */
		else nextbit=0;

		/* keep track of the maximum alignment */
		if(bit_alignment>alignment) alignment=bit_alignment;

		/* keep track of maximum size for unions */
		if(stm->m.size > maxbytes) maxbytes=stm->m.size;

		stm->next=0;
		*mpp=stm;
		mpp=&stm->next;
	}

	/* pad the final structure according to it's most stricly aligned member */
	if(nextbit) nextbit = Alignto(nextbit, alignment);
	else nextbit=Alignto(maxbytes*8, alignment); /* --> it's the case for a union */

	t->size=sti->ctype.size=nextbit/8;

#if LDEBUG
printf("Final size = %d\n", t->size);
#endif

	sti->all=1;
	sial_addfunc_ctype(sti->idx);
	return t;
}

/*
   member access and caching.
   If the member name is empty then the caller wants us
   to populate the entire engregate. The apimember() should
   support a getfirst() (member name == "") and getnext()
   (member name != "") for this perpose.
 */
stmember_t*
sial_member(char *mname, type_t*tp)
{
stinfo_t *sti;
stmember_t *stm;

	if(!is_ctype(tp->type) && ! (tp->type==V_REF && is_ctype(tp->rtype))) {

		sial_error("Expression for member '%s' is not a struct/union", mname);

	
	}

	if(tp->idx == VOIDIDX) {

		sial_error("Reference to member (%s) from unknown structure type", mname);
	}

	if(!(stm=sial_getm(mname, tp, &sti))) {

			sial_error("Unknown member name [%s]", mname);
	}
	return stm;
}

int
sial_open()
{
	sial_setofile(stdout);
	/* push an empty level for parsing allocation */
	sial_pushjmp(0, 0, 0);
	sial_setapiglobs();
	init=1;
	sial_setbuiltins();
	return 1;
}

/* here is a set of api function that do nothing */
static int apigetmem(ull iaddr, void *p, int nbytes) { return 1; }
static int apiputmem(ull iaddr, void *p, int nbytes) { return 1; }
static char* apimember(char *mname,  ull pidx, type_t*tm, member_t *m, ull *lidx) { return 0; }
static int apigetctype(int ctype, char *name, type_t*tout) { return 0; }
static char * apigetrtype(ull idx, type_t*t) { return ""; }
static int apialignment(ull idx) { return 0; }
static int apigetval(char *name, ull *val) { return 0; }
static enum_t* apigetenum(char *name) { return 0; }
static def_t *apigetdefs(void) { return 0; }
static char* apifindsym(char *p) { return 0; }

static apiops nullops= {
	apigetmem, apiputmem, apimember, apigetctype, apigetrtype, apialignment,
	apigetval, apigetenum, apigetdefs, 0, 0, 0, 0, apifindsym
};

apiops *sial_ops=&nullops;;

void
sial_apiset(apiops *o, int abi, int nbpw, int sign)
{
def_t *dt;

	sial_ops=o?o:&nullops;
	sial_setdefbtype(nbpw, sign);
	/* get the pre defines and push them. */
	dt=API_GETDEFS();
	while(dt) {

		sial_newmac(dt->name, dt->val, 0, 0, 1);
		dt=dt->next;
	}
	/* add the sial define */
	sial_newmac(sial_strdup("sial"), sial_strdup("1"), 0, 0, 1);
}

/*
	Get and set path function.
	ipath is include file search path.
	mpath is macro search path
*/
static char *mpath="";
static char *ipath="";
void sial_setmpath(char *p) { mpath=p; }
void sial_setipath(char *p) { ipath=p; }
char *sial_getmpath(void) { return mpath; }
char *sial_getipath(void) { return ipath; }

static char *curp=0;
char *sial_curp(char *p) { char *op=curp; p?(curp=p):(op=curp); return op; }

static char*
sial_cattry(char *first, char *second)
{
struct stat stats;
char *buf=sial_alloc(strlen(first)+strlen(second)+2);

	strcpy(buf, first);
	strcat(buf, "/");
	strcat(buf, second);
	if(!stat(buf, &stats)) return buf;
	sial_free(buf);
	return 0;
}

char *
sial_filepath(char *fname, char *path)
{
	struct stat buf;
	/* valid file path, return immediatly */
	if(stat(fname,&buf) == 0) {
		/* must return a free'able name */
		char *name=sial_strdup(fname);
		TAG(name);
		return name;

	} else if(fname[0]=='~') {

		if(strlen(fname)>1) {

			char *rname, *start;
			struct passwd *pwd;

			if(fname[1]=='/') {

				/* current user name */
				pwd=getpwuid(getuid());

				if(!pwd) {
					sial_msg("Who are you : uid=%d \n?", getuid());
					return 0;
				}

				start=fname+1;

			} else {

				char *p, s;

				for(p=fname+1;*p;p++) if(*p=='/') break;
				s=*p;
				*p='\0';

				/* other user */
				pwd=getpwnam(fname+1);
				if(!pwd) {

					sial_msg("Who is this : %s ?\n", fname+1);
					return 0;
				}
				if(s) *p=s;
				start=p;
			}
			rname=sial_alloc(strlen(start+1)+strlen(pwd->pw_dir)+2);
			strcpy(rname, pwd->pw_dir);
			strcat(rname, start);
			return rname;
		}

	} else {

		char *p=sial_strdup(path);
		char *tok, *curp;

		/* we check if the file is found relatively to the current
		   position. I.e. the position of the running script */
		if((curp=sial_curp(0)) && (curp=sial_cattry(curp, fname))) {

			sial_free(p);
			return curp;
		}

		tok=strtok(p, ":");
		while(tok) {

			if((curp=sial_cattry(tok, fname))) {

				sial_free(p);
				return curp;
			}
			tok=strtok(NULL, ":");

		}
		sial_free(p);
	}
	return 0;
}

char*
sial_filempath(char *fname) 
{
	return sial_filepath(fname, mpath);
}

char *
sial_fileipath(char *fname) 
{
	return sial_filepath(fname, ipath);
}

/* load a file or a set of file */
int
sial_loadunload(int load, char *name, int silent)
{
DIR *dirp;
int ret=1;
char *fname=sial_filempath(name);

	if(!fname) {

		if(!silent) sial_msg("File not found : %s\n", name);
		return 0;
	}

	if((dirp=opendir(fname))) {

		struct dirent *dp;
		char *buf;

		while ((dp = readdir(dirp)) != NULL) {

			if (!strcmp(dp->d_name, ".") || !strcmp(dp->d_name, ".."))
				continue;

			buf=sial_alloc(strlen(fname)+dp->d_reclen+2);
			sprintf(buf, "%s/%s", fname, dp->d_name);
			if(load) {
				ret &= sial_newfile(buf, silent);
			}else{
				sial_deletefile(buf);
			}
			sial_free(buf);
		}
		closedir(dirp);
	}
	else {

		if(load) {
			ret=sial_newfile(fname, silent);
		}else{
			sial_deletefile(fname);
		}
	}
 	sial_free(fname); 
	return ret;
}

/*
	Load conditionaly.
	If it's already load, return.
*/
ull
sial_depend(char *name)
{
char *fname=sial_filempath(name);
int ret=1 ;
void *fp;

	if(!fname) ret=0;
	else if(!(fp=sial_findfile(fname,0)) || sial_isnew(fp)) {

		ret=sial_loadunload(1, name, 1);
		sial_free(fname);
	}
	return ret;
}

value_t *
sial_bdepend(value_t *vname)
{
	return sial_makebtype(sial_depend(sial_getptr(vname, char)));
}

ull 
sial_load(char *fname)
{
	return sial_loadunload(1, fname, 0);
}

value_t*
sial_bload(value_t *vfname)
{
char *fname=sial_getptr(vfname, char);
value_t *v;

	v=sial_makebtype(sial_load(fname));
	return v;
}

ull
sial_unload(char *fname)
{
	return sial_loadunload(0, fname, 0);
}

value_t*
sial_bunload(value_t *vfname)
{
char *fname=sial_getptr(vfname, char);

	return sial_defbtype(sial_newval(), sial_unload(fname));
}

void
sial_loadall()
{
char *path=sial_strdup(sial_getmpath());
char *p, *pn;

	p=pn=path;
	while(*pn) {

		if(*pn == ':') {

			*pn++='\0';
			sial_loadunload(1, p, 1);
			p=pn;

		} else pn++;
	}
	if(p!=pn) sial_loadunload(1, p, 1);
	/* sial_free(path); */
}

static void
add_flag(var_t*flags, int c)
{
char s[20];
var_t *v;

	sprintf(s, "%cflag", c);
	v=sial_newvar(s);
	sial_defbtype(v->v, (ull)0);
	v->ini=1;
	sial_enqueue(flags, v);
}

int
sial_cmd(char *fname, char **argv, int argc)
{
value_t *idx, *val;

	sial_chkinit();

	if(sial_chkfname(fname, 0)) {

		var_t*flags, *args, *narg;
		char *opts, *newn=sial_alloc(strlen(fname)+sizeof("_usage")+1);
		int c, i;
		extern char *optarg;
		extern int optind;
		int dou;
		char *f=sial_strdup("Xflag");

		flags=(var_t*)sial_newvlist();

		/* build a complete list of option variables */
		for(c='a';c<='z';c++) add_flag(flags, c);
		for(c='A';c<='Z';c++) add_flag(flags, c);

		/* check if there is a getopt string associated with this command */
		/* there needs to be a fname_opt() and a fname_usage() function */
		sprintf(newn, "%s_opt", fname);

		if(sial_chkfname(newn, 0)) opts=(char*)(unsigned long)sial_exefunc(newn, 0);
		else opts="";

		sprintf(newn, "%s_usage", fname);
		dou=sial_chkfname(newn, 0);

		/* build a set of variable from the given list of arguments */
		/* each options generate a conrresponding flag ex: -X sets Xflag to one
		   end the corresponding argument of a ":" option is in ex. Xarg
	   	   each additional arguments is keaped in the array args[] */

		if(opts[0]) {

#ifdef linux
			optind=0;
#else
			getoptreset();
#endif
			while ((c = getopt(argc, argv, opts)) != -1) {

				var_t*flag, *opt;
				char *a=sial_strdup("Xarg");;

				if(c==':') {

					sial_warning("Missing argument(s)");
					if(dou) sial_exefunc(newn, 0);
					sial_free(a);
					goto out;

				} else if(c=='?') {

					if(dou) {

						char *u=(char*)(unsigned long)sial_exefunc(newn, 0);

						if(u) sial_msg("usage: %s %s\n", fname, u);
					}
					sial_free(a);
					goto out;
				}

	
				/* set the Xflag variable  to 1 */
				f[0]=c;
				flag=sial_inlist(f, flags);
				sial_defbtype(flag->v, (ull)1);
				flag->ini=1;

				/* create the Xarg variable */
				if(optarg && optarg[0]) {

					char *p=sial_alloc(strlen(optarg)+1);

					a[0]=c;
					strcpy(p, optarg);
					opt=(var_t*)sial_newvar(a);
					sial_setstrval(opt->v, p);
					opt->ini=1;
					sial_enqueue(flags, opt);
				}
				sial_free(a);
			}
			sial_free(f);
		}
		else optind=1;

		/* put every other args into the argv[] array_t*/
		args=(var_t*)sial_newvar("argv");
		args->ini=1;

		/* create a argv[0] with the name of the command */
		{

			val=sial_makestr(fname);
			idx=sial_makebtype(0);

			/* create the value's value */
			sial_addarrelem(&args->v->arr, idx, val);
			sial_freeval(idx);
		}

		for ( i=1; optind < argc; optind++, i++) {

			val=sial_makestr(argv[optind]);
			idx=sial_makebtype(i);

			/* create the value's value */
			sial_addarrelem(&args->v->arr, idx, val);
			sial_freeval(idx);
		}

		narg=(var_t*)sial_newvar("argc");
		sial_defbtype(narg->v, i);
		narg->ini=1;

		sial_enqueue(flags, narg);

		/* add the args variable to the flags queue */
		sial_enqueue(flags, args);

		/* now execute */
		sial_runcmd(fname, flags);

out:
		/* free all arguments variables Xflag Xarg and argv[] */
		sial_freesvs(flags);

		sial_free(newn);
		return 0;
	}
	return 1;
}

