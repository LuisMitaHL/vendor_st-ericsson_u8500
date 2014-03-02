%{
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
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <malloc.h>
// to help resolve type name versus var name ambiguity...
#define VARON  needvar=1;
#define VAROFF needvar=0;
static int sial_toctype(int);
int sialerror(char *);
%}

%union {
	node_t	*n;
	char 	*s;
	int	 i;
	type_t	*t;
	dvar_t	*d;
	var_t	*v;
}

%token	<i>	STATIC DOBLK WHILE RETURN TDEF EXTERN VARARGS
%token  <i>	CHAR SHORT FLOAT DOUBLE VOID INT UNSIGNED LONG SIGNED VOLATILE REGISTER STRTYPE CONST
%token	<i>	BREAK CONTINUE DO FOR FUNC
%token	<i>	IF PATTERN BASETYPE
%token	<i>	STRUCT ENUM UNION
%token	<i>	SWITCH CASE DEFAULT
%token	<n>	ELSE CEXPR
%token	<n>	VAR NUMBER STRING
%token  <t>	TYPEDEF
%token	<i>	'(' ')' ',' ';' '{' '}'

%type	<n>	termlist term opt_term opt_termlist
%type	<n>	stmt stmtlist expstmt stmtgroup
%type	<n>	var opt_var c_string
%type	<n>	for if while switch case caselist caseconstlist caseconst

%type	<d>	dvar dvarlist dvarini

%type	<v>	one_var_decl var_decl_list var_decl farglist decl_list

%type	<t>	type ctype rctype btype_list tdef typecast
%type	<t>	storage_list string type_decl
%type	<t>	ctype_decl
%type   <i>	btype storage ctype_tok print

%right	<i>	ASSIGN ADDME SUBME MULME DIVME MODME ANDME XORME
%right	<i>	ORME SHLME SHRME
%right	<i>	'?'
%left	<i>	IN
%left	<i>	BOR
%left	<i>	BAND
%left	<i>	OR
%left	<i>	XOR
%left	<i>	AND
%left	<i>	EQ NE
%left	<i>	GE GT LE LT
%left	<i>	SHL SHR
%left	<i>	ADD SUB
%left	<i>	MUL DIV MOD
%left	<i>	PRINT PRINTO PRINTD PRINTX TAKE_ARR
%right	<i>	ADROF PTRTO PTR UMINUS SIZEOF TYPECAST POSTINCR PREINCR POSTDECR PREDECR INCR DECR FLIP NOT
%left	<i>	ARRAY CALL INDIRECT DIRECT

%%

file:
	/* empty */
	| fileobj
	| file fileobj
	;

fileobj:
	function
	| var_decl ';'			{ sial_file_decl($1); }
	| ctype_decl ';'		{ ; }
	;

function:
	one_var_decl stmtgroup
					{ sial_newfunc($1, $2); }
	;


for:
	FOR '(' opt_termlist ';' opt_term ';' opt_termlist ')' expstmt
					{ $$ = sial_newstat(FOR, 4, $3, $5, $7, $9); }
	| FOR '(' var IN term ')' expstmt
					{ $$ = sial_newstat(IN, 3, $3, $5, $7); }
	;

if:
	IF '(' {VARON;} term {VAROFF;} ')'		{ $$ = $4; }
	;

switch :
	SWITCH '(' {VARON;} term {VAROFF;} ')' '{' caselist '}'

					{ $$ = sial_newstat(SWITCH, 2, $4, $8); }
	;

caselist:
	case
	| caselist case 		{ $$ = sial_addcase($1, $2); }
	;

case :
	caseconstlist stmtlist		{ $$ = sial_newcase($1, $2); }
	;

caseconst:
	CASE term ':'			{ $$ = sial_caseval(0, $2); }
	| DEFAULT ':'			{ $$ = sial_caseval(1, 0); }
	;

caseconstlist:
	caseconst
	| caseconstlist caseconst	{ $$ = sial_addcaseval($1, $2); }
	;

opt_term:
	/* empty */			{ $$ = 0; }
	| term
	;

termlist:
	  term
	| termlist ',' term		{ $$ = sial_sibling($1, $3); }
	;

opt_termlist:
	  /* empty */			{ $$ = 0; }
	| termlist
	;

stmt:
	  termlist ';'			{ $$ = sial_newstat(PATTERN, 1, $1); }
	| while expstmt			{ $$ = sial_newstat(WHILE, 2, $1, $2); }
	| switch
	| for
	| if expstmt ELSE expstmt	{ $$ = sial_newstat(IF, 3, $1, $2, $4); }
	| if expstmt			{ $$ = sial_newstat(IF, 2, $1, $2); }
	| DO expstmt WHILE '(' term ')' ';'
					{ $$ = sial_newstat(DO, 2, $2, $5); }
	| RETURN term ';'		{ $$ = sial_newstat(RETURN, 1, $2); }
	| RETURN ';'			{ $$ = sial_newstat(RETURN, 1, NULLNODE); }
	| BREAK ';'			{ $$ = sial_newstat(BREAK, 0); }
	| CONTINUE ';'			{ $$ = sial_newstat(CONTINUE, 0); }
	| ';'				{ $$ = 0; }
	;

stmtlist:
	   /* empty */			{ $$ = 0; }
	| stmt
	| stmtgroup
	| stmtlist stmt			{ $$ = sial_addstat($1, $2); }
	| stmtlist stmtgroup		{ $$ = sial_addstat($1, $2); }
	;

stmtgroup:
	'{' decl_list stmtlist '}'	{ $$ = sial_stat_decl($3, $2); }
	| '{' stmtlist '}'  		{ $$ = sial_stat_decl($2, 0); }
	;

expstmt:
	stmt
	| stmtgroup
	;

term:

	  term '?' term ':' term %prec '?'
	 				{ $$ = sial_newop(CEXPR, 3, $1, $3, $5); }
	| term BOR 	term		{ $$ = sial_newop(BOR, 2, $1, $3); }
	| term BAND	term		{ $$ = sial_newop(BAND, 2, $1, $3); }
	| NOT term			{ $$ = sial_newop(NOT, 1, $2); }
	| term ASSIGN	term		{ $$ = sial_newop(ASSIGN, 2, $1, $3); }
	| term EQ	term		{ $$ = sial_newop(EQ, 2, $1, $3); }
	| term GE	term		{ $$ = sial_newop(GE, 2, $1, $3); }
	| term GT	term		{ $$ = sial_newop(GT, 2, $1, $3); }
	| term LE	term		{ $$ = sial_newop(LE, 2, $1, $3); }
	| term LT	term		{ $$ = sial_newop(LT, 2, $1, $3); }
	| term IN	term		{ $$ = sial_newop(IN, 2, $1, $3); }
	| term NE	term		{ $$ = sial_newop(NE, 2, $1, $3); }
	| '(' term ')'			{ $$ = $2; }
	| term ANDME	term		{ $$ = sial_newop(ANDME, 2, $1, $3); }
	| PTR term %prec PTRTO 		{ $$ = sial_newptrto($1, $2); }
	| AND term %prec ADROF		{ $$ = sial_newadrof($2); }
	| term OR	term		{ $$ = sial_newop(OR, 2, $1, $3); }
	| term ORME	term		{ $$ = sial_newop(ORME, 2, $1, $3); }
	| term XOR	term		{ $$ = sial_newop(XOR, 2, $1, $3); }
	| term XORME	term		{ $$ = sial_newop(XORME, 2, $1, $3); }
	| term SHR	term		{ $$ = sial_newop(SHR, 2, $1, $3); }
	| term SHRME	term		{ $$ = sial_newop(SHRME, 2, $1, $3); }
	| term SHL	term		{ $$ = sial_newop(SHL, 2, $1, $3); }
	| term SHLME	term		{ $$ = sial_newop(SHLME, 2, $1, $3); }
	| term ADDME	term		{ $$ = sial_newop(ADDME, 2, $1, $3); }
	| term SUBME	term		{ $$ = sial_newop(SUBME, 2, $1, $3); }
	| term MULME	term		{ $$ = sial_newop(MULME, 2, $1, $3); }
	| term DIV	term		{ $$ = sial_newop(DIV, 2, $1, $3); }
	| term DIVME	term		{ $$ = sial_newop(DIVME, 2, $1, $3); }
	| term MODME	term		{ $$ = sial_newop(MODME, 2, $1, $3); }
	| term MOD	term		{ $$ = sial_newop(MOD, 2, $1, $3); }
	| term SUB	term		{ $$ = sial_newop(SUB, 2, $1, $3); }
	| term ADD	term		{ $$ = sial_newop(ADD, 2, $1, $3); }
	| term PTR term	%prec MUL	{ $$ = sial_newmult($1, $3, $2); }
	| term AND term			{ $$ = sial_newop(AND, 2, $1, $3); }
	| SUB term %prec UMINUS		{ $$ = sial_newop(UMINUS, 1, $2); }
	| '~' term %prec FLIP		{ $$ = sial_newop(FLIP, 1, $2); }
	| '+' term %prec UMINUS		{ $$ = $2; }
	| term '(' ')' %prec CALL	{ $$ = sial_newcall($1, NULLNODE); }
	| term '(' termlist ')' %prec CALL	{ $$ = sial_newcall($1, $3); }
	| DECR term			{ $$ = sial_newop(PREDECR, 1, $2); }
	| INCR term			{ $$ = sial_newop(PREINCR, 1, $2); }
	| term DECR			{ $$ = sial_newop(POSTDECR, 1, $1); }
	| term INCR			{ $$ = sial_newop(POSTINCR, 1, $1); }
	| term INDIRECT var		{ $$ = sial_newmem(INDIRECT, $1, $3); }
	| term INDIRECT tdef		{ $$ = sial_newmem(INDIRECT, $1, sial_tdeftovar($3)); } // resolve ambiguity
	| term DIRECT var		{ $$ = sial_newmem(DIRECT, $1, $3); }
	| term DIRECT tdef		{ $$ = sial_newmem(DIRECT, $1, sial_tdeftovar($3)); } // resolve ambiguity
	| term  '[' term ']' %prec ARRAY	
					{ $$ = sial_newindex($1, $3); }
	| NUMBER
	| c_string
	| typecast term %prec TYPECAST	{ $$ = sial_typecast($1, $2); }
	| SIZEOF '(' var_decl ')'
					{ $$ = sial_sizeof(sial_newcast($3), 1); }
	| SIZEOF term			{ $$ = sial_sizeof($2, 2); }
	| print '(' var_decl ')' %prec SIZEOF	
					{ $$ = sial_newptype($3); }
	| print term %prec SIZEOF	{ $$ = sial_newpval($2, $1); }
	| TAKE_ARR '(' term ',' term ')' { $$ = $3; /* sial_newtakearr($3, $5); */ }
	| var
	;

print:
	PRINT
	| PRINTX
	| PRINTO
	| PRINTD
	;

typecast:
        '(' var_decl ')'	 	{ $$ = sial_newcast($2); }
	;

var_decl_list:
	var_decl ';'
	| var_decl_list var_decl ';'	{ sial_addnewsvs($1, $1, $2); $$=$1; }
	;

decl_list:
	ctype_decl ';'			{ $$ = 0; }
	| var_decl ';'			{ $$ = $1; }
	| decl_list var_decl ';'	{ $$=$1; if($1 && $2) sial_addnewsvs($1, $1, $2); }
	| decl_list ctype_decl ';'	{ $$ = $1; }
	;


var_decl:
	type_decl dvarlist	{ needvar=0; $$ = sial_vardecl($2, $1); }
	;

one_var_decl:
	type_decl dvar	{ needvar=0; $$ = sial_vardecl($2, $1); }
	;

type_decl:
	type				{ $$=$1; needvar++; }
	| storage_list			{ $$=$1; needvar++; }
	| type storage_list		{ $$=sial_addstorage($1, $2); needvar++; }
	| storage_list type		{ $$=sial_addstorage($2, $1); needvar++; }
	| type_decl PTR			{ $$=$1; sial_pushref($1, $2);; needvar++; }
	| type_decl storage_list	{ $$=sial_addstorage($1, $2); needvar++; }
	;

type:
	ctype
	| tdef
	| btype_list
	| string
	| ctype_decl
	;

ctype_decl:
	ctype_tok var '{' {sial_startctype(sial_toctype($1),$2);instruct++;} var_decl_list '}'
		 			{ instruct--; $$ = sial_ctype_decl(sial_toctype($1), $2, $5); }
	| ctype_tok tdef '{' {sial_startctype(sial_toctype($1),lastv=sial_tdeftovar($2));instruct++;} var_decl_list '}'
		 			{ instruct--; $$ = sial_ctype_decl(sial_toctype($1), lastv, $5); }
	| ctype_tok var '{' dvarlist '}'
		 			{ $$ = sial_enum_decl(sial_toctype($1), $2, $4); }
	| ctype_tok tdef '{' dvarlist '}'
		 			{ $$ = sial_enum_decl(sial_toctype($1), sial_tdeftovar($2), $4); }
	;

ctype:
	rctype				{ $$ = $1; }
	| ctype_tok '{' {instruct++;} var_decl_list '}'
					{  instruct--; $$ = sial_ctype_decl(sial_toctype($1), 0, $4); }
	| ctype_tok '{' dvarlist '}'
					{  $$ = sial_enum_decl(sial_toctype($1), 0, $3); }
	;

farglist:
	/* empty */			{ $$ = 0; }
	| one_var_decl			{ $$ = $1; }
	| farglist ',' one_var_decl	{ 
						if(!$1) sial_error("Syntax error"); 
						if($3) sial_addnewsvs($1, $1, $3); $$=$1; 
					}
	| farglist ',' VARARGS		{ 
						if(!$1) sial_error("Syntax error"); 
						sial_addtolist($1, sial_newvar(S_VARARG)); $$=$1; 
					}
	;
	

string:
	STRTYPE                        { 
						type_t *t;
						t=sial_newtype(); 
						t->type=V_STRING;
						t->typattr=0;
						$$ = t;
					}
	;

rctype:
	ctype_tok var 			{ $$ = sial_newctype(sial_toctype($1), $2); }
	| ctype_tok tdef		{ $$ = sial_newctype(sial_toctype($1), sial_tdeftovar($2)); }
	;

ctype_tok:
	STRUCT
	| ENUM
	| UNION
	;

btype_list:
	btype				{ $$ = sial_newbtype($1); }
	| btype_list btype		{ $$ = sial_addbtype($1, $2); }
	;

c_string:
	STRING				{ $$ = $1; }
	| c_string  STRING		{ $$ = sial_strconcat($1, $2); }
	;

btype:
	LONG
	| CHAR
	| INT
	| SHORT
	| UNSIGNED
	| SIGNED
	| DOUBLE
	| FLOAT
	| VOID
	;

storage_list:
	storage				{ $$ = sial_newbtype($1); }
	| storage_list storage		{ sial_error("Only one storage class can be speficied"); }
	;

storage:
	STATIC
	| VOLATILE
	| REGISTER
	| TDEF
	| EXTERN
	| CONST
	;

dvarlist:
	dvarini				{ $$ = $1; }
	| dvarlist ',' dvarini		{ $$ = sial_linkdvar($1, $3); }
	;

dvarini:
	dvar				{ $$ = $1; }
	| dvar ASSIGN  term		{ $$ = sial_dvarini($1, $3); }
	;

dvar:
	opt_var				{ $$ = sial_newdvar($1); needvar=0; }
	| ':' term 			{ $$ = sial_dvarfld(sial_newdvar(0), $2); }
	| dvar ':' term 		{ $$ = sial_dvarfld($1, $3); }
	| dvar '[' opt_term ']'		{ $$ = sial_dvaridx($1, $3); }
	| PTR dvar			{ $$ = sial_dvarptr($1, $2); }
	| dvar '(' ')'			{ $$ = sial_dvarfct($1, 0); }
	| dvar '(' farglist ')'		{ $$ = sial_dvarfct($1, $3); }
	| '(' dvar ')'			{ $$ = $2; }
	;

opt_var:
	/* empty */			{ $$ = 0; }
	| var				{ $$ = $1; }
	;

var:
	VAR				{ $$ = $1; }
	;	

tdef:
	TYPEDEF				{ $$ = $1; }
	;

while:
	WHILE '(' {VARON;} term {VAROFF;} ')'  { $$ = $4; }
	;

%%

static int
sial_toctype(int tok)
{
	switch(tok) {
	case STRUCT: return V_STRUCT;
	case ENUM: return V_ENUM;
	case UNION: return V_UNION;
	default: sial_error("Oops sial_toctype!"); return 0;
	}
}

/*
	This file gets included into the yacc specs.
	So the "sial.h" is already included 
*/

int sialerror(char *p) { sial_error(p); return 0; }

