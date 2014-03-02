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

#define MEMDEBUG 1
/*
*/
#include "sial.h"
#include <string.h>
#include <ctype.h>
#include <stdarg.h>
#include <malloc.h>
#include <limits.h>
#include <sys/mman.h>
#include <setjmp.h>

#ifdef __GNUC__
#       if __LP64__
#               define NKPC	16
#       else
#               define NKPC	4
#       endif
#else
// must be the SGI Mips compiler.
#       if (_MIPS_SZLONG == 64)
#               define NKPC	16
#       else
#               define NKPC	4
#       endif
#endif
#define PAGESIZE (NKPC*1024)


/*
	Jump defines
*/
#define MAXJMPS (S_MAXDEEP*3)
int njmps=0;

typedef struct blklist {

	struct blklist *next;	/* root based doubly chained */
	struct blklist *prev;
	int size;		/* size of the allocation */
	int istmp;		/* was flaged as temp ? */
	int level;		/* coresponding level */
	void *caller;		/* __return_address of caller */
	void *freer;		/* __return_address of freer */

} blist;

#define SIZEBL   (((sizeof(blist)+8)/8)*8)

void pbl(void *p)
{
blist *bl=(blist*)(((char*)p)-SIZEBL);
    sial_msg("struct blklist *%p {", bl);
    sial_msg("      next=%p", bl->next);
    sial_msg("      prev=%p", bl->prev);
    sial_msg("      size=%d", bl->size);
    sial_msg("      istmp=%d", bl->istmp);
    sial_msg("      level=%d", bl->level);
    sial_msg("      caller=%p", bl->caller);
    sial_msg("      freer=%p", bl->freer);
}

static blist temp={ &temp, &temp, 0, 0, 0, 0, 0 };

value_t*
sial_findsym(value_t *vadr)
{
	char *addr=sial_getptr(vadr, char);
	char *p = API_FINDSYM(addr);

	if(p) {
	    return sial_setstrval(sial_newval(), p);
	} else {
	    return sial_setstrval(sial_newval(),"");
	}
}

value_t*
sial_showaddr(value_t *vadr)
{
void *addr=sial_getptr(vadr, void);
blist *bl;
int n=0;

	for(bl=temp.next; bl != &temp; bl=bl->next) {

		if(bl->caller==addr) {

			if(!(n%8)) sial_msg("\n");
			sial_msg("0x%08x ", ((char *)bl) + SIZEBL);
			n++;
		}
	}
	return sial_makebtype(0);
}

static int memdebug=0;

/* these two functions must *not* receive any values */
value_t* sial_memdebugon() { memdebug=1; return sial_makebtype(0); }
value_t* sial_memdebugoff() { memdebug=0; return sial_makebtype(0); }
int sial_ismemdebug() { return memdebug; }

value_t*
sial_showtemp()
{
blist *bl;
int i, totsiz, totbl;
static int ncallers=0;
static void *callers[1000];
static int count[1000];
static int sizes[1000];
static int dir=0;

	if(!dir) {

		memset(callers, 0, sizeof(void*)*1000);
		memset(count, 0, sizeof(int)*1000);
		memset(sizes, 0, sizeof(int)*1000);
		ncallers=0;
	}

	if(dir==1) dir=0;
	else dir=1;

	for(bl=temp.next; bl != &temp; bl=bl->next) {

		int i;

		for(i=0;i<ncallers;i++) 
			if(callers[i]==bl->caller) { 
				if(dir) { count[i]++; sizes[i]+=bl->size; }
				else { count[i]--; sizes[i]-=bl->size; }
				break; 
			}

		if(i==ncallers) {
			callers[ncallers]=bl->caller;
			count[ncallers]=1;
			sizes[ncallers]=bl->size;
			ncallers++;
		}

	}
	totbl=totsiz=0;
	for(i=0;i<ncallers;i++) {

		int c=count[i]<0?-count[i]:count[i];
		int s=sizes[i]<0?-sizes[i]:sizes[i];

		sial_msg("0x%08x [%5d] [%8d]\n", callers[i], c, s);

		totsiz+=s;
		totbl+=c;
	}
	sial_msg("    --------------\nTotal of %d bytes in %d blocks.\n", totsiz, totbl);
	return sial_newval();
}

void
sial_caller(char *p, void *retaddr)
{
blist *bl=(blist*)(((char*)p)-SIZEBL);

    bl->caller=retaddr;
}

#define PAGEMASK 0xfffffffffffff000ll
#define MAGIC    0xdeadbabe
void *
sial_alloc(int size)
{
char *m;
blist *bl;

#ifdef MEMDEBUG
unsigned long p, pp;
int npages;
#endif

	size=size+SIZEBL;

#if MEMDEBUG

	if(memdebug) {

		npages=((size+PAGESIZE+4)/PAGESIZE)+2;
		p=(unsigned long)malloc(npages*PAGESIZE);
		p=(p+PAGESIZE)&PAGEMASK;
		pp=p+((npages-2)*PAGESIZE);
		p=pp-size;
		p = p ^ (p & 0x0fll);
		*((int*)(p-4))=MAGIC;
		mprotect((void*)pp, PAGESIZE, PROT_READ);
		m=(char*)p;

	} else {

		m=malloc(size);
	}

#else

	m=malloc(size);
	
#endif


	bl=(blist*)m;
	bl->size=size;
	bl->level=njmps;
	bl->prev=bl->next=bl;
	bl->istmp=0;
	TAG(m+SIZEBL);
	return m+SIZEBL;
}

void
sial_maketemp(void *p)
{
blist *bl;

	if(!p) return;

	bl=(blist*)(((char*)p)-SIZEBL);
	bl->prev=&temp;
	bl->next=temp.next;
	bl->istmp=1;
	temp.next->prev=bl;
	temp.next=bl;
}

void *
sial_calloc(int size)
{
char *p=sial_alloc(size);

	TAG(p);
	memset(p, 0, size);
	return p;
}

static void
sial_free_bl(blist *bl, void *ra)
{
	bl->freer=ra;
	bl->prev->next=bl->next;
	bl->next->prev=bl->prev;

#ifdef MEMDEBUG

	if(memdebug) {

		/* help out dbx/gdb when they're watching the allocated area
	   	   by writing over it */
		{
		int i, ni=bl->size/sizeof(void*);
		char *p=(char*)bl;
		unsigned long up;

			for(i=0;i<ni;i++) ((char **)p)[i]=ra;
			up=(unsigned long)p;
			if(*((int*)(up-4)) != MAGIC) sial_error("Oops sial_free");
			up=up ^ (up & (PAGESIZE-1));
			mprotect((void*)up, PAGESIZE, PROT_READ);
		}

	} else {

		free(bl);
	}

#else
	free(bl);
#endif

}

void
sial_free(void *p)
{
	if(!p) return;
	sial_free_bl((blist*)(((char*)p)-SIZEBL), __return_address);
}

void
sial_freetemp()
{
blist *bl=temp.next;

	while(bl != &temp) {

		blist *next=bl->next;
		sial_free_bl(bl, __return_address);
		bl=next;
	}
}

int
sial_istemp(void *p)
{
	return ((blist*)(((char*)p)-SIZEBL))->istmp;
}

char *
sial_strdup(char *s)
{
char *ns=sial_alloc(strlen(s)+1);

	strcpy(ns, s);
	TAG(ns);
	return ns;
}

void *
sial_dupblock(void *p)
{
void *p2;
int size=((blist*)(((char*)p)-SIZEBL))->size-SIZEBL;

	if(!p) return 0;

	p2=sial_alloc(size);
	memcpy(p2, p, size);
	return p2;
}

/* cheap realloc. we drop the original
   This function is only used ones in configmon(1) code */
void *
sial_realloc(void *p, int size)
{
int cursize=((blist*)(((char*)p)-SIZEBL))->size-SIZEBL;
void *p2;

	p2=sial_calloc(size);
	memcpy(p2, p, cursize<size?cursize:size);
	sial_free(p);
	return p2;
}

/*
	Warp jumps clearing house
	This is intimetly linked to the jumping stuff.
	We allocate a new list of buffer for each discontinuity (break, continue or
	return).
*/
struct {

	int type;
	int svlev;
	value_t **val;
	jmp_buf *env;

} jmps[MAXJMPS];

/* this is used after a jump since sial_freetemp() should already have cleaned up 
   anything in between the previous and new level */
void
sial_setlev(int level)
{
	njmps=level;
}

void
sial_pushjmp(int type, void *venv, void *val)
{
jmp_buf *env=(jmp_buf *)venv;

	if(njmps<MAXJMPS) {

		jmps[njmps].type=type;
		jmps[njmps].val=(value_t**)val;
		jmps[njmps].env=env;
		jmps[njmps++].svlev=sial_getsvlev();

	} else {

		sial_error("Jump Stack overflow");

	}
}

/*
	Switch context to a break, continue or return end point.
	If we are popoing a break we trash the continues.
	If we are poping a return then we trash the current break and continue.
*/
void
sial_dojmp(int type, void *val)
{
	if(njmps > 1) {

		jmp_buf *env;

		while(njmps && jmps[--njmps].type!=type);
		if(jmps[njmps].val) *(jmps[njmps].val)=val;
		env=jmps[njmps].env;

		/* reset the variable level too... */
		sial_setsvlev(jmps[njmps].svlev);

		longjmp(*env, 1);
		/* NOT REACHED */

	} else sial_parseback(); /* we use the same code for initializing
		static and automatic variables. In the case of statiuc variables
		is the initizer expression throws an error then there's no J_EXIT
		jump context and njmps is null. It's treated as a parsing error */
}

void
sial_popjmp(int type)
{
	if(!njmps) {

		sial_error("Pop underflow!");
	}
	njmps--;
	if(jmps[njmps].type != type) {

		sial_error("Wrong pop! %d vs %d", jmps[njmps].type, type);
	}
	sial_setsvlev(jmps[njmps].svlev);
}

