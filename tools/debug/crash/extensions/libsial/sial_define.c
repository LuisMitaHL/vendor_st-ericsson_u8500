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
/*
	This set of functions handle #define for simple constant or macros.
	We read from the current parser input strem untill end of line.

	The big thing is that we need to do some parsing to get the deinf names
	and parameters. Also at the time of the macro instanciation, we need to parse
	the parameters again. That leads to a more complex package...
*/

#define MAXP 20
typedef struct mac_s {

	char *name;		/* this macro name */
	int np;			/* number of parameters */
	int issub;		/* subs have to be threated differently */
	char **p;		/* parameters names */
	char *buf;		/* text for the macro itself */
	struct mac_s *next;	/* next on the list */
	srcpos_t pos;

} mac_t;

typedef struct {
	mac_t *m;
	char **subs;
} smac_t;

static mac_t* macs=0;

/* we have to add a space at the end of the value
   Again, this is to prevent a EOF on the parsing stream */
def_t*
sial_add_def(def_t*d, char*name, char*val)
{
def_t*nd=sial_alloc(sizeof(def_t));
char *buf=sial_alloc(strlen(val)+2);

	strcpy(buf, val);
	strcat(buf, " ");
	sial_free(val);
	nd->name=name;
	nd->val=buf;
	nd->next=d;
	return nd;
}

/* search for a macro is the current list */
mac_t * 
sial_getmac(char *name, int takeof)
{
mac_t *m;
mac_t *prev=0;
mac_t *sial_getcurmac(void);

	if(takeof || !(m=sial_getcurmac())) m=macs;

	for(; m; m=m->next) {

		if( !strcmp(m->name, name) ) {

			if(takeof) {

				if(!prev) macs=m->next;
				else prev->next=m->next;

			}
			return m;
		}
		prev=m;
	}
	return 0;
}

node_t*
sial_macexists(node_t*var)
{
char *name=NODE_NAME(var);
int val;

	if(sial_getmac(name, 0)) val=1;
	else val=0;
	return sial_makenum(B_UL, val);
}
static void
sial_freemac(mac_t*m)
{
int i;

	for(i=0;i<m->np;i++) sial_free(m->p[i]);
	if(m->np) sial_free(m->p);
	sial_free(m);
}

/*
	These are called at 2 different points.
	One call at the very begining. One call for each file.
*/
void* sial_curmac(void) { return macs; }

void
sial_flushmacs(void *vtag)
{
mac_t *m, *next;
mac_t *tag=(mac_t *)vtag;

	for(m=macs; m!=tag; m=next) {

		next=m->next;
		sial_freemac(m);
	}
	macs=m;
}

/* this function is called to register a new macro.
   The text associated w/ the macro is still on the parser stream.
   Untill eol.
*/
void
sial_newmac(char *mname, char *buf, int np, char **p, int silent)
{
char *p2;
mac_t *m;

	{
		char *p=buf+strlen(buf)-1;

		/* eliminate trailing blanks */
		while(*p && (*p==' ' || *p=='\t')) p--;
		*(p+1)='\0';

		/* eliminate leading blanks */
		p=buf;
		while(*p && (*p==' ' || *p=='\t')) p++;

		/* copy and append a space. This is to prevent unloading of the
	   	macro before the sial_chkvarmac() call as been performed */
		p2=sial_alloc(strlen(p)+2);
		strcpy(p2, p);
		sial_free(buf);
		p2[strlen(p2)+1]='\0';
		p2[strlen(p2)]=' ';
		buf=p2;
	}

	if((m=sial_getmac(mname, 1)) && strcmp(m->buf, buf)) {

		/* when processing the compile options, be silent. */
		if(!silent) {

			sial_warning("Macro redefinition '%s' with different value_t\n"
				"value_t=[%s]\n"
				"Previous value_t at %s:%d=[%s]\n"
				, mname, buf, m->pos.file, m->pos.line, m->buf);
		}

	}
	m=(mac_t*)sial_alloc(sizeof(mac_t));
	m->name=sial_strdup(mname);
	m->np=np;
	m->p=p;
	m->buf=buf;
	m->next=macs;
	m->issub=0;
	sial_setpos(&m->pos);
	macs=m;
}

/* this function is called by the enum declaration function and
   when a enum type is extracted from the image to push a set
   of define's onto the stack, that correspond to each identifier 
   in the enum.
*/
void
sial_pushenums(enum_t *et)
{
	while(et) {

		char *buf=sial_alloc(40);

		sprintf(buf, "%d", et->value);
		sial_newmac(et->name, buf, 0, 0, 0);
		et=et->next;
	}
}

static void
sial_skipcomment(void)
{
int c;

	while((c=sial_input())) {

		if(c=='*') {

			int c2;

			if((c2=sial_input())=='/') return;
			sial_unput(c2);
		}
	}
}

static void
sial_skipstr(void)
{
int c;

	while((c=sial_input())) {

		if(c=='\\') sial_input();
		else if(c=='"') return;
	}
}


/* skip over strings and comment to a specific chracter */
static void
sial_skipto(int x)
{
int c;

	while((c=sial_input())) {

		if(c==x) return;

		switch(c) {

			case '\\':
			sial_input();
			break;

			case '"':
			sial_skipstr();
			break;

			case '/': {

				int c2;

				if((c2=sial_input())=='*') {

					sial_skipcomment();

				} else sial_unput(c2);
			}
			break;

			case '(':

				sial_skipto(')');
			break;

			case ')':
			sial_error("Missing parameters to macro");
			break;
		}

	}

	sial_error("Expected '%c'", x);
}


/*
   This function gets called when the buffer for a macro as been fully
   parsed. We need to take the associated parameter substitution macros
   of of the stack and deallocate associated data.
*/
static void
sial_popmac(void *vsm)
{
smac_t *sm=(smac_t *)vsm;
int i;

	for(i=0;i<sm->m->np;i++) {

		mac_t *m=sial_getmac(sm->m->p[i], 1);

		if(!m) sial_error("Oops macro pop!");
		sial_free(m->buf);
		sial_free(m->name);
		sial_free(m);
	}
	sial_free(sm->subs);
	sial_free(sm);
}

/* 

  need to get the actual parameters from the parser stream.
  This can be simple variable or complex multiple line expressions
  with strings and commants imbedded in them... 

*/
static int 
sial_pushmac(mac_t *m)
{
int i;
char **subs=sial_alloc(sizeof(char*)*m->np);
smac_t *sm;
int siallex(void);

	/* the next token should be a '(' */
	if(siallex() != '(') {

		sial_error("Expected '(' after '%s'", m->name);
		
	}

	/* get the parameters */
	for(i=0;i<m->np;i++) {

		char *p=sial_cursorp();
		int nc;

		if(i<m->np-1) sial_skipto(',');
		else sial_skipto(')');

		nc=sial_cursorp()-p-1;
		subs[i]=sial_alloc(nc+2);
		strncpy(subs[i], p, nc);
		subs[i][nc]=' ';
		subs[i][nc+1]='\0';
	}

	/* take care of the macro() case. ex: IS_R10000()i.e. no parms */
	if(!m->np) 
		sial_skipto(')');

	sm=sial_alloc(sizeof(smac_t));

	sm->m=m;
	sm->subs=subs;

	/* we push the associated buffer on the stream */
	sial_pushbuf(m->buf, 0, sial_popmac, sm, 0);

	/* we push the subs onto the macro stack */
	for(i=0;i<m->np;i++) {

		mac_t *pm=sial_alloc(sizeof(mac_t));

		pm->name=sial_alloc(strlen(m->p[i])+1);
		strcpy(pm->name, m->p[i]);
		pm->np=0;
		pm->p=0;
		pm->buf=subs[i];
		pm->next=macs;
		pm->issub=1;
		macs=pm;
	}
	return 1;
	
}


/*
	This one is called from the lexer to check if a 'var' is to be substituted for
	a macro
*/
int
sial_chkmacvar(char *mname)
{
mac_t *m;

	if((m=sial_getmac(mname, 0))) {


		/* simple constant ? */
		if(!m->p) {

			sial_pushbuf(m->buf, 0, 0, 0, m->issub ? m->next : 0);

		} else {
			return sial_pushmac(m);
		}
		return 1;

	}
	return 0;

}

/*
	Skip an unsupported preprocessor directive.
*/
void
sial_skip_directive(void)
{
	sial_free(sial_getline());
}

void
sial_undefine(void)
{
int c;
int i=0;
char mname[MAX_SYMNAMELEN+1];
mac_t *m;

	/* skip all white spaces */
	while((c=sial_input()) == ' ' || c == '\t') if(c=='\n' || !c) {

		sial_error("Macro name expected");
	}

	mname[i++]=c;

	/* get the constant or macro name */
	while((c=sial_input()) != ' ' && c != '\t') {

		if(c=='\n' || !c) break;
		if(i==MAX_SYMNAMELEN) break;
		mname[i++]=c;
	}
	mname[i]='\0';
	if((m=sial_getmac(mname, 1))) sial_freemac(m);
        else sial_addneg(mname);
}

/*
	This one is called from the lexer after #define as been detected 
*/
void
sial_define(void)
{
int c;
int i=0;
char mname[MAX_SYMNAMELEN+1];

	/* skip all white spaces */
	while((c=sial_input()) == ' ' || c == '\t') if(c=='\n' || !c) goto serror;

	mname[i++]=c;

	/* get the constant or macro name */
	while((c=sial_input()) != ' ' && c != '\t' && c != '(') {

		if(c=='\n' || !c) break;

		if(i==MAX_SYMNAMELEN) break;

		mname[i++]=c;
	}
	mname[i]='\0';

	/* does this macro have paraneters */
	/* If so, '(' will be right after name of macro. No spaces. */
	if(c=='(') {

		int np, nc, done;
		char **pnames;
		char curname[MAX_SYMNAMELEN+1];

		np=nc=done=0;
		pnames=(char **)sial_alloc(sizeof(char*)*MAXP);
		
		while(!done) {

			c=sial_input();

			switch(c) {
				case '\n': case 0:
				goto serror;

				/* continuation */
				case '\\':
				if(sial_input()!='\n') goto serror;
				break;

				case ',':
				if(!nc) goto serror;
last:
				curname[nc]='\0';
				pnames[np]=sial_alloc(strlen(curname)+1);
				strcpy(pnames[np], curname);
				nc=0;
				np++;
				break;

				case ')':
				done=1;
				if(nc) goto last;
				break;

				case ' ':
				case '\t':
				break;

				default:
				curname[nc++]=c;
				break;
			}
		}
		sial_newmac(mname, sial_getline(), np, pnames, 0);
		return;

	} else if(c == '\n') {

		/* if nothing speciied then set to "1" */
		sial_newmac(mname, sial_strdup("1"), 0, 0, 0);

	} else {

		sial_newmac(mname, sial_getline(), 0, 0, 0);
	}
		
	return;

serror:

	sial_error("Syntax error on macro definition");
}
