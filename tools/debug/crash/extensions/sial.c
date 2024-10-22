/*
 * $Id: sial.c,v 1.17 2012/03/27 19:11:20 anderson Exp $
 *
 * This file is part of lcrash, an analysis tool for Linux memory dumps.
 *
 * Created by Silicon Graphics, Inc.
 * Contributions by IBM, and others
 *
 * Copyright (C) 1999 - 2005 Silicon Graphics, Inc. All rights reserved.
 * Copyright (C) 2001, 2002 IBM Deutschland Entwicklung GmbH, IBM Corporation
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version. See the file COPYING for more
 * information.
 */

#include "gdb/defs.h"
#include "target.h"
#include "symtab.h"
#include "gdbtypes.h"
#include "gdbcore.h"
#include "frame.h"
#include "value.h"
#include "symfile.h"
#include "objfiles.h"
#include "gdbcmd.h"
#include "call-cmds.h"
#include "gdb_regex.h"
#include "expression.h"
#include "language.h"
#include "demangle.h"
#include "inferior.h"
#include "linespec.h"
#include "source.h"
#include "filenames.h"		/* for FILENAME_CMP */
#include "objc-lang.h"

#include "hashtab.h"

#include "gdb_obstack.h"
#include "block.h"
#include "dictionary.h"

#include <sys/types.h>
#include <fcntl.h>
#include "gdb_string.h"
#include "gdb_stat.h"
#include <ctype.h>
#include "cp-abi.h"

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sial_api.h>

/////////////////////////////////////////////////////////////////////////
// some stuff from crash's defs.h, file which cannot be included here.
// Hate to do this but this is a quick port. 
// If anyone cares to work on the include and defs structure to make
// this work cleanly...
//
/*
 *  Global data (global_data.c) 
 */
extern char *args[];      
extern int argcnt;            
extern int argerrs;
#define SYNOPSIS      (0x1)
#define COMPLETE_HELP (0x2)
#define PIPE_TO_LESS  (0x4)
#define KVADDR (0x1)
#define QUIET (0x4)

typedef void (*cmd_func_t)(void);

struct command_table_entry {               /* one for each command in menu */
	char *name;
	cmd_func_t func;
	char **help_data;
	ulong flags;
};
extern FILE *fp; 
extern char *crash_global_cmd();

typedef unsigned long long int ulonglong;
extern int readmem(ulonglong, int, void *, long, char *, ulong);
extern int symbol_exists(char *);
extern ulong symbol_value(char *);
extern void cmd_usage(char *, int);
extern void register_extension(struct command_table_entry *);


//
/////////////////////////////////////////////////////////////////////////
/*
	This is the glue between the sial interpreter and crash.
*/

static int
apigetmem(ull iaddr, void *p, int nbytes)
{
	return readmem(iaddr, KVADDR, p, nbytes, NULL, QUIET);
}

// Since crash is target dependant (build for the 
static uint8_t apigetuint8(void* ptr)
{
uint8_t val;
    if(!readmem((unsigned long)ptr, KVADDR, (char*)&val, sizeof val, NULL, QUIET)) return (uint8_t)-1;
    return val;
}

static uint16_t apigetuint16(void* ptr)
{
uint16_t val;
    if(!readmem((unsigned long)ptr, KVADDR, (char*)&val, sizeof val, NULL, QUIET)) return (uint16_t)-1;
    return val;
}

static uint32_t apigetuint32(void* ptr)
{
uint32_t val;
    if(!readmem((unsigned long)ptr, KVADDR, (char*)&val, sizeof val, NULL, QUIET)) return (uint32_t)-1;
    return val;
}

static uint64_t apigetuint64(void* ptr)
{
uint64_t val;
    if(!readmem((unsigned long)ptr, KVADDR, (char*)&val, sizeof val, NULL, QUIET)) return (uint64_t)-1;
    return val;
}

static int
apiputmem(ull iaddr, void *p, int nbytes)
{
	return 1;
}

/* extract a complex type (struct, union and enum) */
static int
apigetctype(int ctype, char *name, TYPE_S *tout)
{
    struct symbol *sym;
    struct type *type;
    int v=0;
    
    sial_dbg_named(DBG_TYPE, name, 2, "Looking for type %d name [%s] in struct domain...", ctype, name);
#ifdef GDB_6_1
    sym = lookup_symbol(name, 0, STRUCT_DOMAIN, 0, (struct symtab **) NULL);
#else
    sym = lookup_symbol(name, 0, STRUCT_DOMAIN, 0);
#endif
    if(!sym) {
            sial_dbg_named(DBG_TYPE, name, 2, "Not found.\nLooking for type %d name [%s] in var domain...", ctype, name);
#ifdef GDB_6_1
            sym = lookup_symbol(name, 0, VAR_DOMAIN, 0, (struct symtab **) NULL);
#else
            sym = lookup_symbol(name, 0, VAR_DOMAIN, 0);
#endif
            if(sym) {
                sial_dbg_named(DBG_TYPE, name, 2, "found class=%d\n", sym->aclass);
                if(sym->aclass == LOC_TYPEDEF) v=1;
            }
    }
        
    if (sym) {
        type=sym->type;
        if(sial_is_typedef(ctype) && v) goto match;
        switch(TYPE_CODE(type)) {
	    case TYPE_CODE_INT: 
                sial_dbg_named(DBG_TYPE, name, 2, "Variable is integer\n");
		if (sial_is_typedef(ctype)) {
		    sial_dbg_named(DBG_TYPE, name, 2, 
				    "integer is not a typedef\n");
		    break;
			
		}
	
            case TYPE_CODE_TYPEDEF:
                                    if(sial_is_typedef(ctype))  goto match; break;
            case TYPE_CODE_ENUM:    if(sial_is_enum(ctype))     goto match; break;
            case TYPE_CODE_STRUCT:  if(sial_is_struct(ctype))   goto match; break;
            case TYPE_CODE_UNION:   if(sial_is_union(ctype))    goto match; break;
        }
        sial_dbg_named(DBG_TYPE, name, 2, "Found but no match.\n");
    }
    else sial_dbg_named(DBG_TYPE, name, 2, "Not Found.\n");

    return 0;

match:
    sial_dbg_named(DBG_TYPE, name, 2, "Found.\n");
    /* populate */
    sial_type_settype(tout, ctype);
    sial_type_setsize(tout, TYPE_LENGTH(type));
    sial_type_setidx(tout, (ull)(unsigned long)type);
    sial_pushref(tout, 0);
    return 1;
}

/* set idx value to actual array indexes from specified size */
static void
sial_setupidx(TYPE_S*t, int ref, int nidx, int *idxlst)
{
        /* put the idxlst in index size format */
        if(nidx) {

                int i;

                for(i=0;i<nidx-1;i++) {
			/* kludge for array dimensions of [1] */
			if (idxlst[i+1] == 0) {
				idxlst[i+1] = 1;
			}
			idxlst[i]=idxlst[i]/idxlst[i+1];
		}

                /* divide by element size for last element bound */
                if(ref) idxlst[i] /= sial_defbsize();
                else idxlst[i] /= sial_type_getsize(t);
                sial_type_setidxlst(t, idxlst);
        }
}
/*
	This function needs to drill down a typedef and
	return the corresponding type.
	If the typedef is from a basetype sial_parsetype() will be
	called back to build the type 
*/
static char *
drilldowntype(struct type *type, TYPE_S *t)
{
char *tstr=0;
int fctflg=0, ref=0;
int *idxlst=0;
int nidx=0;

	while(type)
	{
            check_typedef(type);

            // check out for stub types and pull in the definition instead
            if(TYPE_STUB(type) && TYPE_TAG_NAME(type)) {
#ifdef GDB_6_1
                struct symbol *sym=lookup_symbol(TYPE_TAG_NAME(type), 0, STRUCT_DOMAIN, 0, (struct symtab **) NULL);
#else
                struct symbol *sym=lookup_symbol(TYPE_TAG_NAME(type), 0, STRUCT_DOMAIN, 0);
#endif
                if(sym) {
                    type=sym->type;
                } 
            }

            switch(TYPE_CODE(type)) {

		/* typedef inserts a level of reference to the 1'dactual type */
		case TYPE_CODE_PTR:
	
			ref++;
                        type=TYPE_TARGET_TYPE(type);
       			/* this could be a void*, in which case the drill down stops here */
			if(!type) {

				/* make it a char* */
				sial_parsetype("char", t, ref);
				return 0;

			}
		break;

		/* handle pointer to functions */
		case TYPE_CODE_FUNC:

			fctflg=1;
                        type=TYPE_TARGET_TYPE(type);
		break;

		/* Is this an array ? if so then just skip this type info and
		   we only need information on the elements themselves */
		case TYPE_CODE_ARRAY:
                        if(!idxlst) idxlst=sial_calloc(sizeof(int)*(MAXIDX+1));
                        if(nidx >= MAXIDX) sial_error("Too many indexes! max=%d\n", MAXIDX);
                        if (TYPE_LENGTH (type) > 0 && TYPE_LENGTH (TYPE_TARGET_TYPE (type)) > 0)
	                {
                            idxlst[nidx++]=TYPE_LENGTH (type) / TYPE_LENGTH (check_typedef(TYPE_TARGET_TYPE (type)));
                        }
                        type=TYPE_TARGET_TYPE(type);
		break;

		/* typedef points to a typedef itself */
		case TYPE_CODE_TYPEDEF:
                        type=TYPE_TARGET_TYPE(type);
		break;

		case TYPE_CODE_INT:

			sial_parsetype(tstr=TYPE_NAME(type), t, 0);
			type=0;
		break;

		case TYPE_CODE_BOOL:
			sial_parsetype("char", t, ref);
			type=0;
			break;

		case TYPE_CODE_UNION: 
			sial_type_mkunion(t);
			goto label;

		case TYPE_CODE_ENUM:
			sial_type_mkenum(t);
			goto label;

		case TYPE_CODE_STRUCT:
		{
			sial_type_mkstruct(t);

label:
			sial_type_setsize(t, TYPE_LENGTH(type));
			sial_type_setidx(t, (ull)(unsigned long)type);
			tstr=TYPE_TAG_NAME(type);
                        type=0;
		}
		break;

		/* we don;t have all the info about it */
		case TYPE_CODE_VOID:
			sial_parsetype("int", t, 0);
                        type=0;
		break;


		default: 
			sial_error("Oops drilldowntype");
		break;
		}


	}
	sial_setupidx(t, ref, nidx, idxlst);
	if(fctflg) sial_type_setfct(t, 1);
	sial_pushref(t, ref+(nidx?1:0));
	if(tstr) return sial_strdup(tstr);
	return sial_strdup("");
}

static char *
apigetrtype(ull idx, TYPE_S *t)
{
	return drilldowntype((struct type*)(unsigned long)(idx), t);
}

/*
   	Return the name of a symbol at an address (if any)
*/
static char*
apifindsym(char *p)
{
    return NULL;
}


/* 
	Get the type, size and position information for a member of a structure.
*/
static char*
apimember(char *mname,  ull tnum, TYPE_S *tm, MEMBER_S *m, ull *lnum)
{
struct type *type=(struct type*)(unsigned long)tnum;
int midx;
#define LASTNUM (*lnum)

	/* if we're being asked the next member in a getfirst/getnext sequence */
	if(mname && !mname[0] && LASTNUM) {

		midx = LASTNUM;

	} else {

		if (TYPE_CODE(type) == TYPE_CODE_TYPEDEF) {
			return 0;
		}
		if ((TYPE_CODE(type) != TYPE_CODE_STRUCT) && (TYPE_CODE(type) != TYPE_CODE_UNION)) {
			return 0;
		}
                midx=0;
	}
	while(midx < TYPE_NFIELDS(type)) {
        
		if (!mname || !mname[0] || !strcmp(mname, TYPE_FIELD_NAME(type, midx))) {

                        check_typedef(TYPE_FIELD_TYPE(type, midx));
			sial_member_soffset(m, TYPE_FIELD_BITPOS(type, midx)/8);
			sial_member_ssize(m, TYPE_FIELD_TYPE(type, midx)->length);
			sial_member_snbits(m, TYPE_FIELD_BITSIZE(type, midx));
			sial_member_sfbit(m, TYPE_FIELD_BITPOS(type, midx)%8);
			sial_member_sname(m, TYPE_FIELD_NAME(type, midx));
			LASTNUM=midx+1;
			return drilldowntype(TYPE_FIELD_TYPE(type, midx), tm);
		}
		midx++;
	}
	return 0;
}

/*
	This function gets the proper allignment value for a type.
*/
static int
apialignment(ull idx)
{
struct type *type=(struct type *)(unsigned long)idx;

    while(1)
    {
	switch(TYPE_CODE(type)) {

	    case TYPE_CODE_ARRAY: case TYPE_CODE_TYPEDEF:
		    type=TYPE_TARGET_TYPE(type);
	    break;

	    case TYPE_CODE_STRUCT:
	    case TYPE_CODE_UNION:
	    {
		int max=0, cur;
		int midx=0;

		while(midx < TYPE_NFIELDS(type)) {
                    cur=apialignment((ull)(unsigned long)TYPE_FIELD_TYPE(type, midx));
	            if(cur > max) max=cur;
	            midx++;
		}
		return max;
            }
            

	    case TYPE_CODE_PTR:
	    case TYPE_CODE_ENUM:
	    case TYPE_CODE_INT:

		    return TYPE_LENGTH (type);

	    default:

		    sial_error("Oops apialignment");
	}
    }
}

/* get the value of a symbol */
static int
apigetval(char *name, ull *val)
{
    if (symbol_exists(name)) {
        *val=symbol_value(name);
        return 1;
    }
    return 0;
}

/*
	Get the list of enum symbols.
*/
ENUM_S*
apigetenum(char *name)
{
    struct symbol *sym;

#ifdef GDB_6_1
    sym = lookup_symbol(name, 0, STRUCT_DOMAIN, 0, (struct symtab **) NULL);
#else
    sym = lookup_symbol(name, 0, STRUCT_DOMAIN, 0);
#endif
    if (sym && TYPE_CODE(sym->type)==TYPE_CODE_ENUM) {
	ENUM_S *et=0;
        struct type *type=sym->type;
        int n=0;
	while(n < TYPE_NFIELDS (type)) {
      	    et=sial_add_enum(et, sial_strdup(TYPE_FIELD_NAME(type, n)), TYPE_FIELD_BITPOS(type, n));
            n++;
	}
        return et;
    }
    return 0;
}

/*
	Return the list of preprocessor defines.
	For Irix we have to get the die for a startup.c file.
	Of dwarf type DW_TAG_compile_unit.
	the DW_AT_producer will contain the compile line.

	We then need to parse that line to get all the -Dname[=value]
*/
DEF_S *
apigetdefs(void)
{
DEF_S *dt=0;
int i;
static struct linuxdefs_s {

	char *name;
	char *value;

} linuxdefs[] = {

	{"crash",		"1"},
	{"linux",		"1"},
	{"__linux",		"1"},
	{"__linux__",		"1"},
	{"unix",		"1"},
	{"__unix",		"1"},
	{"__unix__",		"1"},
	// helper macros
	{"LINUX_2_2_16",	"(LINUX_RELEASE==0x020210)"},
	{"LINUX_2_2_17",	"(LINUX_RELEASE==0x020211)"},
	{"LINUX_2_4_0",		"(LINUX_RELEASE==0x020400)"},
	{"LINUX_2_2_X",		"(((LINUX_RELEASE) & 0xffff00) == 0x020200)"},
	{"LINUX_2_4_X",		"(((LINUX_RELEASE) & 0xffff00) == 0x020400)"},
	{"LINUX_2_6_X",		"(((LINUX_RELEASE) & 0xffff00) == 0x020600)"},
	{"LINUX_3_X_X",         "(((LINUX_RELEASE) & 0xff0000) == 0x030000)"},
#ifdef i386
	{"i386",         "1"},
	{"__i386",       "1"},
	{"__i386__",     "1"},
#endif
#ifdef s390
	{"s390",         "1"},
	{"__s390",       "1"},
	{"__s390__",     "1"},
#endif
#ifdef s390x
	 {"s390x",       "1"},
	 {"__s390x",     "1"},
	 {"__s390x__",   "1"},
#endif
#ifdef __ia64__
	{"ia64",         "1"},
	{"__ia64",       "1"},
	{"__ia64__",     "1"},
	{"__LP64__",     "1"},
	{"_LONGLONG",    "1"},
	{"__LONG_MAX__", "9223372036854775807L"},
#endif
#ifdef ppc64
	 {"ppc64",       "1"},
	 {"__ppc64",     "1"},
	 {"__ppc64__",   "1"},
#endif
	};
        
static char *untdef[] = { 
    "clock",  
    "mode",  
    "pid",  
    "uid",  
    "xtime",  
    "init_task", 
    "size", 
    "type",
    "level",
    0 
};

#if 0
How to extract basic set of -D flags from the kernel image

	prod=sial_strdup(kl_getproducer());
	for(p=prod; *p; p++) {

		if(*p=='-' && *(p+1)=='D') {

			char *def=p+2;

			while(*p && *p != '=' && *p != ' ') p++;

			if(!*p || *p == ' ') {

				*p='\0';
				dt=sial_add_def(dt, sial_strdup(def), sial_strdup("1"));

			} else {

				char *val=p+1;

				*p++='\0';
				while(*p && *p != ' ') p++;
				*p='\0';

				dt=sial_add_def(dt, sial_strdup(def), sial_strdup(val));
			}
		}
	}
#endif	

        /* remove some tdef with very usual identifier.
           could also be cases where the kernel defined a type and variable with same name e.g. xtime.
           the same can be accomplished in source using #undef <tdefname> or forcing the evaluation of 
           a indentifier as a variable name ex: __var(xtime).
           
           I tried to make the grammar as unambiqguous as I could.
           
           If this becomes to much of a problem I might diable usage of all image typedefs usage in sial!
        */ 
        {
            char **tdefname=untdef;
            while(*tdefname) sial_addneg(*tdefname++);;
            
        }
        
	/* insert constant defines from list above */
	for(i=0;i<sizeof(linuxdefs)/sizeof(linuxdefs[0]);i++) {

		dt=sial_add_def(dt, sial_strdup(linuxdefs[i].name), 
			sial_strdup(linuxdefs[i].value));
	}

#if 1
        {
            ull addr;
            char banner[200];
            if(apigetval("linux_banner", &addr)) {
                if(apigetmem(addr, banner, sizeof banner-1)) {
                
                    // parse the banner string and set up release macros
                    banner[sizeof banner -1]='\0';
                    char *tok=strtok(banner, " \t");
                    if(tok) tok=strtok(NULL, " \t");
                    if(tok) tok=strtok(NULL, " \t");
                    if(tok) {
                        int version, patchlevel, sublevel, ret;
                        ret = sscanf(tok, "%d.%d.%d-", &version, &patchlevel, &sublevel);
			switch (ret) {
			case 2:
			    sublevel = 0;
			case 3:
			    sprintf(banner, "0x%02x%02x%02x", version, patchlevel, sublevel);
		            dt=sial_add_def(dt, sial_strdup("LINUX_RELEASE"), sial_strdup(banner));
                            sial_msg("Core LINUX_RELEASE == '%s'\n", tok);
			default:
			    break;
                        }
                    }
                }
                else sial_msg("Sial init: could not read symbol 'linux_banner' from corefile.\n");
            }
            else sial_msg("Sial init: could not find symbol 'linux_banner' in corefile.\n");
        }
#endif
	return dt;
}

apiops icops= {
	apigetmem, 
	apiputmem, 
	apimember, 
	apigetctype, 
	apigetrtype, 
	apialignment, 
	apigetval, 
	apigetenum, 
	apigetdefs,
	apigetuint8,
	apigetuint16,
	apigetuint32,
	apigetuint64,
	apifindsym
};

void
sial_version(void)
{
	sial_msg("< Sial interpreter version %d.%d >\n"
		, S_MAJOR, S_MINOR);
}

static void
run_callback(void)
{
extern char *crash_global_cmd();
FILE *ofp = NULL;

	if (fp) {
		ofp = sial_getofile();
		sial_setofile(fp);
	}

	sial_cmd(crash_global_cmd(), args, argcnt);

	if (ofp) 
		sial_setofile(ofp);
}


void
edit_cmd(void)
{
int c, file=0;
        while ((c = getopt(argcnt, args, "lf")) != EOF) {
                switch(c)
                {
                case 'l':
                    sial_vilast();
                    return;
                break;
                case 'f':
                    file++;
                break;
                default:
                        argerrs++;
                        break;
                }
        }

        if (argerrs)
                cmd_usage(crash_global_cmd(), SYNOPSIS);

        else if(args[optind]) {
            while(args[optind]) {
	        sial_vi(args[optind++], file);
            }
	}
        else cmd_usage(crash_global_cmd(), SYNOPSIS);
}

char *edit_help[]={
		"edit",
                "Start a $EDITOR session of a sial function or file",
                "<-f fileName>|<function name>",
                "This command can be use during a tight development cycle",
                "where frequent editing->run->editing sequences are executed.",
                "To edit a known sial macro file use the -f option. To edit the file",
                "at the location of a known function's declaration omit the -f option.",
                "Use a single -l option to be brought to the last compile error location.",
                "",
                "EXAMPLES:",
                "  %s> edit -f ps",
                "  %s> edit ps",
                "  %s> edit ps_opt",
                "  %s> edit -l",
                NULL
};


// these control debug mode when parsing (pre-processor and compile)
int sialdebug, sialppdebug;

void
load_cmd(void)
{
	if(argcnt< 2) cmd_usage(crash_global_cmd(), SYNOPSIS);
	else {
            sial_setofile(fp);
            sial_loadunload(1, args[1], 0);
        }
}

char *load_help[]={
		"load",
                "Load a sial file",
                "<fileName>|<Directory>",
                "  Load a file or a directory. In the case of a directory",
		"  all files in that directory will be loaded.",
                NULL
                
};

void
unload_cmd(void)
{
	if(argcnt < 2) cmd_usage(crash_global_cmd(), SYNOPSIS);
	else sial_loadunload(0, args[1], 0);
}

char *unload_help[]={
		"unload",
                "Unload a sial file",
                "<fileName>|<Directory>",
                "  Unload a file or a directory. In the case of a directory",
		"  all files in that directory will be unloaded.",
                NULL
};

void
sdebug_cmd(void)
{
	if(argcnt < 2) sial_msg("Current sial debug level is %d\n", sial_getdbg());
	else sial_setdbg(atoi(args[1]));
}

char *sdebug_help[]={
		"sdebug",
                "Print or set sial debug level",
                "<Debug level 0..9>",
                "  Set the debug of sial. Without any parameter, shows the current debug level.",
                NULL
};

void
sname_cmd(void)
{
	if(argcnt < 2) {
            if(sial_getname()) sial_msg("Current sial name match is '%s'\n", sial_getname());
            else sial_msg("No name match specified yet.\n");
	} else sial_setname(args[1]);
}

char *sname_help[]={
		"sname",
                "Print or set sial name match.",
                "<name>",
                "  Set sial name string for matches. Debug messages that are object oriented",
                "  will only be displayed if the object name (struct, type, ...) matches this",
		"  value.",
                NULL
};

void
sclass_cmd(void)
{
	if(argcnt < 2) {
            char **classes=sial_getclass();
            sial_msg("Current sial classes are :");
            while(*classes) sial_msg("'%s' ", *classes++);
            sial_msg("\n");
	
        }
        else {
            int i;
            for(i=1; i<argcnt; i++) sial_setclass(args[i]);
        }
}

char *sclass_help[]={
		"sclass",
                "Print or set sial debug message class(es).",
                "<className>[, <className>]",
                "  Set sial debug classes. Only debug messages that are in the specified classes",
                "  will be displayed.",
                NULL
};

#define NCMDS 200
static struct command_table_entry command_table[NCMDS] =  {

	{"edit", edit_cmd, edit_help},
	{"load", load_cmd, load_help},
	{"unload", unload_cmd, unload_help},
	{"sdebug", sdebug_cmd, sdebug_help},
	{"sname", sname_cmd, sname_help},
	{"sclass", sclass_cmd, sclass_help},
	{(char *)0 }
};

static void
add_sial_cmd(char *name, void (*cmd)(void), char **help, int flags)
{
struct command_table_entry *cp;
struct command_table_entry *crash_cmd_table();

    // check for a clash with native commands
    for (cp = crash_cmd_table(); cp->name; cp++) {
        if (!strcmp(cp->name, name)) {
            sial_msg("Sial command name '%s' conflicts with native crash command.\n", name);
            return;
        }
    }

    // make sure we have enough space for the new command
    if(!command_table[NCMDS-2].name) {
        for (cp = command_table; cp->name; cp++);
        cp->name=sial_strdup(name);
        cp->func=cmd;
        cp->help_data=help;
        cp->flags=flags;
    }
}

static void
rm_sial_cmd(char *name)
{
struct command_table_entry *cp, *end;

    for (cp = command_table; cp->name; cp++) {
        if (!strcmp(cp->name, name)) {
            sial_free(cp->name);
            sial_free(cp->help_data[0]);
            sial_free(cp->help_data[2]);
            sial_free(cp->help_data[3]);
            free(cp->help_data);
            memmove(cp, cp+1, sizeof *cp *(NCMDS-(cp-command_table)-1));
            break;
        }
    }
}

/*
	This function is called for every new function
	generated by a load command. This enables us to
	register new commands.

	We check here is the functions:

	fname_help()
	fname_opt()
	and
	fname_usage()

	exist, and if so then we have a new command.
	Then we associated (register) a function with
	the standard sial callbacks.
*/
void
reg_callback(char *name, int load)
{
char fname[MAX_SYMNAMELEN+sizeof("_usage")+1];
char *help_str, *opt_str;
char **help=malloc(sizeof *help * 5);

    if(!help) return;
    snprintf(fname, sizeof(fname), "%s_help", name);
    if(sial_chkfname(fname, 0)) {
        snprintf(fname, sizeof(fname), "%s_usage", name);
        if(sial_chkfname(fname, 0)) {
            if(load) {
                opt_str=sial_strdup((char*)(unsigned long)sial_exefunc(fname, 0));
                snprintf(fname, sizeof(fname), "%s_help", name);
                help_str=sial_strdup((char*)(unsigned long)sial_exefunc(fname, 0));
                help[0]=sial_strdup(name);
                help[1]="";
                help[2]=sial_strdup(opt_str);
                help[3]=sial_strdup(help_str);
                help[4]=0;
                add_sial_cmd(name, run_callback, help, 0);
                sial_free(help_str);
                sial_free(opt_str);
                return;
            }
            else rm_sial_cmd(name);
        }
    }
    free(help);
    return;
}

/* 
 *  The _fini() function is called if the shared object is unloaded. 
 *  If desired, perform any cleanups here. 
 */
void _fini() 
{ 
    // need to unload any files we have loaded
    
}

VALUE_S *curtask(VALUE_S *v, ...)
{
unsigned long get_curtask();
    return sial_makebtype((ull)get_curtask());
}

_init() /* Register the command set. */
{ 
#define LCDIR "/usr/share/sial/crash"
#define LCIDIR "include"
#define LCUDIR ".sial"


	if(sial_open() >= 0) {

		char *path, *ipath;
		char *homepath=0;
               	char *home=getenv("HOME");

		/* set api, default size, and default sign for types */
#ifdef i386
#define SIAL_ABI  ABI_INTEL_X86
#else 
#ifdef __ia64__
#define SIAL_ABI  ABI_INTEL_IA
#else
#ifdef __x86_64__
#define SIAL_ABI  ABI_INTEL_IA
#else
#ifdef __s390__
#define SIAL_ABI  ABI_S390
#else
#ifdef __s390x__
#define SIAL_ABI  ABI_S390X
#else
#ifdef PPC64
#define SIAL_ABI  ABI_PPC64
#else
#ifdef PPC
#define SIAL_ABI  ABI_PPC
#else
#error sial: Unkown ABI 
#endif
#endif
#endif
#endif
#endif
#endif
#endif
		sial_apiset(&icops, SIAL_ABI, sizeof(long), 0);

		sial_version();

        	/* set the macro search path */
        	if(!(path=getenv("SIAL_MPATH"))) {

                	if(home) {

                        	path=sial_alloc(strlen(home)+sizeof(LCUDIR)+sizeof(LCDIR)+4);
				homepath=sial_alloc(strlen(home)+sizeof(LCUDIR)+2);

				/* build a path for call to sial_load() */
				strcpy(homepath, home);
				strcat(homepath, "/");
				strcat(homepath, LCUDIR);

				/* built the official path */
                        	strcpy(path, LCDIR);
                        	strcat(path, ":");
                        	strcat(path, home);
                        	strcat(path, "/");
				strcat(path, LCUDIR);
                	}
                	else path=LCDIR;
		}
		sial_setmpath(path);

		fprintf(fp, "\tLoading sial commands from %s .... ",
                                         path);

		/* include path */
		if(!(ipath=getenv("SIAL_IPATH"))) {

                	if(home) {

                        	ipath=sial_alloc(strlen(home)+sizeof(LCDIR)+sizeof(LCUDIR)+(sizeof(LCIDIR)*2)+(sizeof("/usr/include")+2)+6);

				/* built the official path */
                        	strcpy(ipath, LCDIR);
                        	strcat(ipath, "/"LCIDIR":");
                        	strcat(ipath, home);
                        	strcat(ipath, "/");
				strcat(ipath, LCUDIR);
				strcat(ipath, "/"LCIDIR);
				strcat(ipath, ":/usr/include");
                	}
                	else ipath=LCDIR"/"LCIDIR;
		}
		sial_setipath(ipath);

		/* set the new function callback */
		sial_setcallback(reg_callback);

		/* load the default macros */
		sial_loadall();

		/* load some sial specific commands */
                register_extension(command_table);
                
                /* some builtins */
        	sial_builtin("int curtask()", curtask);
                
                fprintf(fp, "Done.\n");
	} 
        return 1;
}
