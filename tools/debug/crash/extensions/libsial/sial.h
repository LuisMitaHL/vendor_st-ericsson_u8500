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
#include "sial_api.h"
typedef unsigned long long caddr;

#define SRCPOS_S	struct srcpos_s
#define DVAR_S		struct dvar_s
#define CASELIST_S	struct caselist_s
#define CASEVAL_S	struct caseval_s
#define STMEMBER_S	struct stmember_s
#define STINFO_S	struct stinfo_s

SRCPOS_S;
DVAR_S;
CASELIST_S;
CASEVAL_S;
STMEMBER_S;
STINFO_S;


/************* source position tracking ************/
typedef SRCPOS_S {
	char *file;
	int line;
	int col;
} srcpos_t;

/* member information */
typedef MEMBER_S {

	char *name;
	int offset;	/* offset from top of structure */
	int size;	/* size in bytes of the member or of the bit array */
	int fbit;	/* fist bit (-1) is not a bit field */
	int nbits;	/* number of bits for this member */
	int value;      /* for a enum member, the corresponding value_t */

} member_t;

/* list to hold enum constant information */
typedef ENUM_S {

	struct enum_s *next;
	char *name;
	int value;

} enum_t;

/* list of macro symbols and there corresponding value_ts */
typedef DEF_S {
	struct def_s * next;
	char *name;
	char *val;

} def_t;

/* type_t information past back and forth */
typedef TYPE_S {
	int type;	/* type_t of type_t */
	ull idx;	/* index to basetype_t or ctype_t */
	int size;	/* size of this item */
			/* ... next fields are use internally */
	int typattr;	/* base type_t qualifiers */
	int ref;	/* level of reference */
	int fct;        /* 1 if function pointer */
	int *idxlst;    /* points to list of indexes if array */
	ull rtype;	/* type_t a reference refers too */
} type_t;

/* scope/storage of variables */
#define S_FILE		1	/* persistant file scope */
#define S_STAT		2	/* persistant statement scope */
#define S_AUTO		3	/* stack (default) */
#define S_GLOB		4	/* add to the global variables */

typedef union vu_s {
	unsigned char uc;
	signed char sc;
	unsigned short us;
	signed short ss;
	unsigned int ul;
	signed int sl;
	unsigned long long ull;
	signed long long sll;
	void *data;
} vu_t;

/************* value_t **************/
typedef VALUE_S  {
	type_t 	 type;
	int	 set;	/* is this is a Lvalue_t then set is 1 */
	VALUE_S	*setval;/* value_t to set back to */
	void	(*setfct)(struct value_s*, struct value_s*);
			/* the function that will set the value */
	ARRAY_S	*arr;	/* array associated with value */
	vu_t	 v;
	ull	 mem;
} value_t;

/************** array linked lists *****************/
typedef ARRAY_S {

	ARRAY_S *next;	/* to support a linked list of array elements */
	ARRAY_S *prev;
	int ref;	/* reference count on this array */
	VALUE_S *idx;	/* arrays can be indexed using any type of variables */
	VALUE_S *val;	/* arrays element values */

} array_t;

/************* node_t *************/
typedef NODE_S {
	VALUE_S* (*exe)(void*);	/* execute it */
	void   (*free)(void*);	/* free it up */
	char* (*name)(void*);	/* get a name */
	void *data;		/* opaque data */
	NODE_S* next;
	SRCPOS_S pos;
} node_t;

typedef IDX_S {

	int nidx;
	NODE_S *idxs[MAXIDX];

} idx_t;

/*************** variable list ****************/
typedef VAR_S {

	char *name;
	VAR_S *next;
	VAR_S *prev;
	VALUE_S *v;
	int ini;
	DVAR_S *dv;

} var_t;

/* V_BASE subtype */
#define B_SC    0       /* signed char */
#define B_UC    1       /* unsignec char */
#define B_SS    2       /* signed short */
#define B_US    3       /* unsigned short */
#define B_SL    4       /* signed long */
#define B_UL    5       /* unsigned long */
#define B_SLL   6       /* signed long long */
#define B_ULL   7       /* unsigned long long */

#define is_ctype(t) ((t)==V_UNION || (t)==V_STRUCT)
#define VAL_TYPE(v)  (v->type.type)
#define TYPE_SIZE(t) ((t)->type==V_REF?sial_defbsize():(t)->size)

/* type_ts of jumps */
#define J_CONTINUE	1
#define J_BREAK		2
#define J_RETURN	3
#define J_EXIT		4

#define sial_setval(v, v2)	if((v)->set) ((v)->setfct)((v)->setval, (v2))

/************* case *************/
typedef CASEVAL_S {

	int isdef;
	ull val;
	CASEVAL_S *next;
	SRCPOS_S pos;

} caseval_t;

typedef CASELIST_S {

	CASEVAL_S *vals;
	NODE_S *stmt;
	CASELIST_S *next;
	SRCPOS_S pos;

} caselist_t;

/*************** struct member info  ****************/
typedef STMEMBER_S {

        TYPE_S type;    /* corresponding type_t */
        MEMBER_S m;     /* member information */

        STMEMBER_S *next;

} stmember_t;

typedef DVAR_S {

	char		*name;
	int		 refcount;
	int		 ref;
	int		 fct;
	int		 bitfield;
	int		 nbits;
	IDX_S		*idx;
	NODE_S		*init;
	VAR_S		*fargs;
	SRCPOS_S	 pos;
	DVAR_S		*next;

} dvar_t;

typedef STINFO_S {
	char		*name;	/* structure name */
	ull		 idx;	/* key for search */
	int		 all;	/* local : partial or complete declaration ? */
	TYPE_S		 ctype;	/* associated type */
	TYPE_S		 rtype;	/* real type_t when typedef */
	STMEMBER_S	*stm;	/* linked list of members */
	ENUM_S		*enums;	/* enums names and values */
	STINFO_S	 *next;  /* next struct on the list */

} stinfo_t;

stinfo_t *sial_getstbyindex(ull idx, int type_t);

typedef value_t* (*xfct_t)(void *);
typedef char*    (*nfct_t)(void *);
typedef void     (*ffct_t)(void *);
typedef void     (*setfct_t)(value_t*, value_t*);

#ifdef DEBUG
#define NODE_EXE(n) (printf("(%s):[%d]\n",__FILE__, __LINE__), (n)->exe((n)->data)) */
#else
#define NODE_EXE(n) ((n)->exe((n)->data))
#endif
#define NODE_NAME(n) ((n)->name?((n)->name((n)->data)):0)
#define NODE_FREE(n) (sial_freenode(n))

#ifdef __GNUC__
#define __return_address (void*)(__builtin_return_address(0))
#else
// must be the SGI Mips compiler.
#endif
#if 1
#define TAG(p) sial_caller(p, __return_address)
#else
#define TAG(p) ;
#endif

node_t  *sial_sibling(node_t*, node_t*);
node_t  *sial_newnode(void);
node_t  *sial_newvnode(char *);
node_t  *sial_newstr(void);
node_t  *sial_newnum(char *);
node_t  *sial_newop(int op, int nagrs, ...);
node_t  *sial_newptrto(int, node_t*);
node_t  *sial_newmult(node_t*, node_t*, int);
node_t  *sial_newstat(int op, int nargs, ...);
node_t  *sial_stat_decl(node_t*, var_t*);
node_t  *sial_addstat(node_t*, node_t*);
node_t  *sial_type_cast(type_t*, node_t*);
node_t  *sial_newmem(int, node_t*, node_t*);
node_t  *sial_newcall(node_t*, node_t*);
node_t  *sial_newindex(node_t*, node_t*);
node_t  *sial_newadrof(node_t*);
node_t  *sial_newcase(node_t*, node_t*);
node_t  *sial_addcase(node_t*, node_t*);
node_t  *sial_caseval(int, node_t*);
node_t  *sial_addcaseval(node_t*, node_t*);
node_t  *sial_sizeof(void *p, int type_t);
node_t  *sial_tdeftovar(type_t *td);
node_t  *sial_getppnode(void);
node_t  *sial_allocstr(char *buf);
node_t  *sial_makenum(int type_t, ull val);
node_t  *sial_macexists(node_t *var_t);
node_t  *sial_newptype(var_t *v);
node_t  *sial_newpval(node_t *vn, int fmt);
node_t  *sial_strconcat(node_t *, node_t *);
node_t	*sial_typecast(type_t*type, node_t*expr);

dvar_t  *sial_newdvar(node_t *v);
dvar_t  *sial_linkdvar(dvar_t *dvl, dvar_t *dv);
dvar_t  *sial_dvarini(dvar_t *dv, node_t *init);
dvar_t  *sial_dvaridx(dvar_t *dv, node_t *n);
dvar_t  *sial_dvarfld(dvar_t *dv, node_t *n);
dvar_t  *sial_dvarptr(int ref, dvar_t *dv);
dvar_t  *sial_dvarfct(dvar_t *dv, var_t *fargs);

void  sial_pushjmp(int type_t, void *env, void *val);
void  sial_popjmp(int type_t);
void *sial_getcurfile(void);
void  sial_walkarray(node_t *varnode_t, node_t *arrnode_t, void(*cb)(void *), void *data);
void  get_bit_value(ull val, int nbits, int boff, int size, value_t *v);
void  sial_enqueue(var_t *vl, var_t *v);
void  sial_freenode(node_t *n);
void  sial_validate_vars(var_t *svs);
void  sial_freesvs(var_t *svs);
void *sial_setexcept(void);
void  sial_tdef_decl(dvar_t *dv, type_t *t);
void  sial_refarray(value_t *v, int inc);
void *sial_curmac(void);
void  sial_setfct(value_t *v1, value_t *v2);
void  sial_exevi(char *fname, int line);
void  sial_unput(char);
void  sial_dupval(value_t *v, value_t *vs);
void  sial_parseback(void);
void  sial_curpos(srcpos_t *p, srcpos_t *s);
void  sial_rmexcept(void *osa);
void  sial_chksign(type_t*t);
void  sial_chksize(type_t*t);
void  sial_setpos(srcpos_t *p);
void  sial_rerror(srcpos_t *p, char *fmt, ...);
void  sial_rwarning(srcpos_t *p, char *fmt, ...);
void  sial_chkandconvert(value_t *vto, value_t *vfrm);
void  sial_warning(char *fmt, ...);
void  sial_format(int tabs, char *str);
void  sial_freevar(var_t*v);
void  sial_rmbuiltin(var_t*v);
void  sial_rm_globals(void *vg);
void  sial_addnewsvs(var_t*avl, var_t*svl, var_t*nvl);
void  sial_dojmp(int type, void *val);
void  sial_pushbuf(char *buf, char *fname, void(*f)(void*), void *d, void *m);
void  sial_rsteofoneol(void);
void  sial_settakeproto(int v);
void  sial_popallin(void);
void  sial_tagst(void);
void  sial_flushtdefs(void);
void  sial_setsvlev(int newlev);
void  sial_flushmacs(void *tag);
void  sial_add_auto(var_t*nv);
void *sial_chkbuiltin(char *name);
void  sial_freedata(value_t *v);
void  sial_dupdata(value_t *v, value_t *vs);
void  sial_setarray(array_t**arpp);
void  sial_rawinput(int on);
void  sial_setini(node_t*n);
void  sial_valindex(value_t *var, value_t *idx, value_t *ret);
void  sial_free_siblings(node_t*ni);
void  sial_mkvsigned(value_t*v);
void  sial_transval(int s1, int s2, value_t *v, int issigned);
void  sial_popref(type_t*t, int ref);
void  sial_getmem(ull kp, void *p, int n);
void  sial_baseop(int op, value_t *v1, value_t *v2, value_t *result);
void  sial_setinsizeof(int v);
void  sial_freeidx(idx_t *idx);
void  sial_freedvar(dvar_t*dv);
void  sial_pushenums(enum_t *et);
void  sial_addfunc_ctype(int idx);
void  sial_setapiglobs(void);
void  sial_setbuiltins(void);
void  sial_setdefbtype(int size, int sign);
void  get_bit_value(ull val, int nbits, int boff, int size, value_t *v);
void *sial_findfile(char *name, int unlink);
void  sial_newmac(char *mname, char *buf, int np, char **p, int silent);
void *sial_getcurfile(void);
void *sial_getcurfile(void);
void  sial_startctype(int type, node_t*namen);
void  sial_addtolist(var_t*vl, var_t*v);
void  sial_arch_swapvals(void* vp, void *sp);
void  sial_fillst(stinfo_t *st);
void  sial_exememlocal(value_t *vp, stmember_t* stm, value_t *v);
void  sial_do_deref(int n, value_t *v, value_t *ref);
void  sial_addneg(char *name);

stmember_t*sial_member(char *mname, type_t*tp);

ull   set_bit_value_t(ull dvalue_t, ull value_t, int nbits, int boff);
ull   unival(value_t *);
ul    sial_bool(value_t *);

value_t *sial_docall(node_t *, node_t *, void *);
value_t *sial_docast(void);
value_t *sial_newval(void);
value_t *sial_exebfunc(char *, value_t **);
value_t *sial_exevar(void *);
value_t *sial_exenode(node_t *);
value_t *sial_setstrval(value_t *, char *);
value_t *sial_defbtype(value_t *, ull);
value_t *sial_defbtypesize(value_t *, ull, int);
value_t *sial_sprintf(value_t *, ...);
value_t *sial_printf(value_t *, ...);
value_t *sial_exists(value_t *vname);
value_t *sial_exit(int v);
value_t *sial_bload(value_t *name);
value_t *sial_bdepend(value_t *name);
value_t *sial_bunload(value_t *vfname);
value_t *sial_showtemp(void);
value_t *sial_showaddr(value_t *vadr);
value_t *sial_findsym(value_t *vadr);
value_t *sial_memdebugon(void);
value_t *sial_memdebugoff(void);
value_t *sial_ismember(value_t*vp, value_t*vm);

value_t *sial_prarr(value_t*name, value_t*root);
value_t *sial_getstr(value_t*vm);

var_t *sial_vardecl(dvar_t *dv, type_t *t);
var_t *sial_inlist(char *name, var_t *vl);
var_t *sial_dupvlist(var_t *vl);
var_t *sial_getcurgvar(void);
var_t *sial_getvarbyname(char *name, int silent, int local);
var_t *sial_getsgrp_avs(node_t *n);
var_t *sial_getsgrp_svs(node_t *n);
var_t *sial_parsexpr(char *);

int   sial_file_decl(var_t *svs);
int   sial_newfunc(var_t *fvar, node_t* body);
int   sial_line(int inc);
int   sial_samectypename(int type_t, ull idx1, ull idx2);
int   sial_issigned(int attr);
int   sial_isstatic(int atr);
int   sial_isjuststatic(int attr);
int   sial_isconst(int atr);
int   sial_issigned(int atr);
int   sial_istdef(int atr);
int   sial_isxtern(int atr);
int   sial_isvoid(int atr);
int   sial_isstor(int atr);
int   sial_ispartial(type_t*t);
int   sial_input(void);
int   sial_addsvs(int type, var_t*sv);
int   sial_pushfile(char *name);
int   sial_chkfname(char *fname, void *fd);
int   sial_lookuparray(node_t*vnode, node_t*arrnode);
int   sial_runcmd(char *fname, var_t*args);
int   sial_getseq(int c);
int   sial_newfile(char *name, int silent);
int   sial_deletefile(char *name);
int   sial_getsvlev(void);
int   sial_idxtoattr(int idx);
int   sial_docase(ull val, caselist_t*cl);
int   siallex(void);
int   sialpplex(void);
int   sial_ismemdebug(void);
int   sial_isenum(int atr);
int   sial_funcexists(char *name);
int   sial_isnew(void* p);
int   sial_isneg(char *name);

char  *sial_vartofunc(node_t *name);
char  *sial_gettdefname(ull idx);
char  *sial_ctypename(int type_t);
char  *sial_filempath(char *fname);
char  *sial_fileipath(char *fname);
char  *sial_getline(void);
char  *sial_cursorp(void);
char  *sial_getbtypename(int typattr);
char  *sial_filename(void);
char  *sial_curp(char *);

type_t  *sial_newcast(var_t *v);
type_t  *sial_newctype(int ctype_t, node_t *n);
type_t  *sial_addbtype(type_t *t, int newtok);
type_t  *sial_ctype_decl(int ctype_t, node_t *n, var_t *list);
type_t  *sial_enum_decl(int ctype_t, node_t *n, dvar_t *dvl);
type_t  *sial_addstorage(type_t *t1, type_t *t2);
type_t  *sial_getvoidstruct(int ctype);

extern int lineno, needvar, instruct, nomacs;
node_t *lastv;

#define NULLNODE ((node_t*)0)

/* configuration variables */
#define S_MAXSTRLEN	1024	/* lengh of a STRING variable value_t */
#define S_MAXDEEP	500	/* maximum stacking of calls */
#define S_MAXFILES	200	/* maximum number of macro files  */

#define S_VARARG	"__VARARG" /* name of the special var for ... */
