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
%}

%{
#include <string.h>

#ifdef linux
#define YY_INPUT(buf,result,max_size) \
{ \
	int c = sial_input(); \
	result = (c == EOF) ? YY_NULL : (buf[0] = c, 1); \
}
#endif

#define yylval siallval
#include	"sial.h"
#define YY_NO_UNPUT
#include	"sial.tab.h"

#define retok(t) return(t)
int needvar=0, instruct=0;
node_t *lastv;
static char *lastvar=0;
char *sial_lastvar(void) { return lastvar; }
extern void sial_skip_directive(void);
extern void sial_define(void);
extern void sial_include(void);
extern void sial_undefine(void);
extern char sial_newchar(void);
extern int  sial_chkmacvar(char *);
%}

ABC		[a-zA-Z_]
ABCN		[a-zA-Z0-9_]
N		[0-9]
X		[0-9a-fA-F]
W		[ \t\n]
P		#[ \t]*
OP              [(]
CP              [)]

%s var1
%s var2
%s var3
%s var4
%%

{W}		{ ; }

"..."		{ retok(VARARGS); }
"&&"		{ retok(BAND); }
"||"		{ retok(BOR); }
"<"		{ retok(LT); }
"<="		{ retok(LE); }
"=="		{ retok(EQ); }
">="		{ retok(GE); }
">"		{ retok(GT); }
"!="		{ retok(NE); }

"&="		{ retok(ANDME); }
"|"		{ retok(OR); }
"|="		{ retok(ORME); }
"!"		{ retok(NOT); }
"^"		{ retok(XOR); }
"&"		{ retok(AND); }
"^="		{ retok(XORME); }
">>"		{ retok(SHR); }
"<<="		{ retok(SHLME); }
">>="		{ retok(SHRME); }
"<<"		{ retok(SHL); }
"++"		{ retok(INCR); }
"+"		{ retok(ADD); }
"--"		{ retok(DECR); }
"-"		{ retok(SUB); }
"+="		{ retok(ADDME); }
"-="		{ retok(SUBME); }
"*="		{ retok(MULME); }
"/="		{ retok(DIVME); }
"/"		{ retok(DIV); }
"%="		{ retok(MODME); }
"%"		{ retok(MOD); }
"="		{ retok(ASSIGN); }
"->"		{ retok(INDIRECT); }
"."		{ retok(DIRECT); }
"{"		{ needvar=0; retok('{'); }

\*+		{ 
			yylval.i=strlen(yytext); 
			return PTR; 
		}

(("0x"+){X}+[lL]*|{N}+[lL]*)	{ yylval.n = sial_newnum(yytext); retok(NUMBER); }

{P}ident		{  sial_skip_directive(); }
{P}pragma		{  sial_skip_directive(); }
{P}define		{
			/* preprocessor command */
			/* either a simple constant or a macro */
			sial_define();
		}
{P}include 	{

			/* file inclusion */
			sial_include();
		}
{P}undef		{
			sial_undefine();
		}
while		{ retok(WHILE); }
for		{ retok(FOR); }
do		{ retok(DO); }
if		{ retok(IF); }
else		{ retok(ELSE); }
break		{ retok(BREAK); }
continue	{ retok(CONTINUE); }
in		{ retok(IN); }
return		{ retok(RETURN); }

__char__	{ retok(yylval.i=CHAR); }
__short__	{ retok(yylval.i=SHORT); }
__int__		{ retok(yylval.i=INT); }
__float__	{ retok(yylval.i=FLOAT); }
__double__	{ retok(yylval.i=DOUBLE); }
__register__	{ retok(yylval.i=REGISTER); }
__volatile__	{ retok(yylval.i=VOLATILE); }
__void__	{ retok(yylval.i=VOID); }
__unsigned__	{ retok(yylval.i=UNSIGNED); }
__signed__	{ retok(yylval.i=SIGNED); }
__long__	{ retok(yylval.i=LONG); }
__const__	{ retok(yylval.i=CONST); }
__static__ 	{ retok(yylval.i=STATIC); }
__extern__	{ retok(yylval.i=EXTERN); }

char		{ retok(yylval.i=CHAR); }
short		{ retok(yylval.i=SHORT); }
int		{ retok(yylval.i=INT); }
float		{ retok(yylval.i=FLOAT); }
double		{ retok(yylval.i=DOUBLE); }
register	{ retok(yylval.i=REGISTER); }
volatile	{ retok(yylval.i=VOLATILE); }
void		{ retok(yylval.i=VOID); }
unsigned	{ retok(yylval.i=UNSIGNED); }
signed		{ retok(yylval.i=SIGNED); }
long		{ retok(yylval.i=LONG); }
const		{ retok(yylval.i=CONST); }
static 		{ retok(yylval.i=STATIC); }
extern		{ retok(yylval.i=EXTERN); }

string		{ retok(yylval.i=STRTYPE); }
__inline	{ ; }
switch		{ retok(SWITCH); }
case		{ retok(CASE); }
default		{ retok(DEFAULT); }
enum		{ retok(yylval.i=ENUM); }
union		{ retok(yylval.i=UNION);}
struct 		{ retok(yylval.i=STRUCT); }
typedef		{ retok(yylval.i=TDEF); }
sizeof 		{ retok(SIZEOF); }
print		{ retok(PRINT); }
printo		{ retok(PRINTO); }
printd		{ retok(PRINTD); }
printx		{ retok(PRINTX); }
take_array	{ retok(TAKE_ARR); }

__var__              { BEGIN(var1); }
<var1>{W}*{OP}{W}*   { BEGIN(var2); }
<var2>{ABC}{ABCN}*   { BEGIN(var3); goto forcevar; }
<var3>{W}*{CP}{W}*   { BEGIN(INITIAL); }


{ABC}{ABCN}*	{ 
				if((!needvar) && (yylval.t=sial_getctype(V_TYPEDEF, yytext, 1)))
				{ 
					/* hack to remember last tdef name */
					if(lastvar) sial_free(lastvar);
					lastvar=sial_alloc(strlen(yytext)+1);
					strcpy(lastvar, yytext);
					needvar++;
					retok(TYPEDEF);
				}
forcevar:
				needvar=0;
				if(strlen(yytext) > MAX_SYMNAMELEN) {

					sial_error("Symbol name too long");
				}
				if(!sial_chkmacvar(yytext)) {
					yylval.n = sial_newvnode(yytext); 
					retok(VAR);
				}
		}

\"		{ yylval.n = sial_newstr(); retok(STRING); }
\'.\'		{ yylval.n = sial_makenum(B_SC, yytext[1]); retok(NUMBER); }
\'\\.\'		{ yylval.n = sial_makenum(B_SC, sial_getseq(yytext[2])); retok(NUMBER); }

.		{ retok(yylval.i = yytext[0]); }

%%
#undef input
#undef unput
#define input()       sial_input()
#define unput(c)      sial_unput(c)
