/*****************************************************************************/
/**
*  © ST-Ericsson, 2011 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \file	   {@filename}.c
* \brief   DTH registration and utilities for {@module}
* \author  ST-Ericsson
*/
/*****************************************************************************/
#define {@FILENAME}_C
#include "{@filename}.h"
{@headers}
#undef {@FILENAME}_C

#include <dthsrvhelper/dthsrvhelper.h>

#include <syslog.h>
#include <stdarg.h>

/* module version identification */
#define	{@dthroot}_MODULE_VERSION \
    {@version}
#define {@dthroot}_MODULE_VERSION_LEN   {@version_len}


/* {@dthroot} tree structure */ 
struct dth_element {@prefix}dth_struct [] = {

/* debug interface /DTH/Debug/{@dthroot}/TraceLevel is defined at end of
 * structure
 */

/* default versionning interface */
/* NB: discover_version() in 9pclienthelper requires that the path begin by
 * /DTH/<feature>/VERSION
 */
{
    "/DTH/{@dthroot}/VERSION/Version",
    DTH_TYPE_STRING,
    0, 0,
    NO_ENUM,
    NO_INFO,
    0, 0,
    {@prefix}get_version,
    NO_SET,
    NO_EXEC,
    0,
    NULL
},

/* Other module DTH interface. input file used: {@input} */
{@dth_struct}

    /* END OF TABLE */
    {NULL, 0, 0, 0, NULL, NULL, 0, 0, NULL, NULL, NULL, 0, NULL}
};


int {@prefix}get_signed(int64_t vp_value, struct dth_element* pp_elem,
    void *pp_value)
{
    int vl_result = 0;

    switch (pp_elem->type)
    {
    case DTH_TYPE_S8:

        {@dthroot}_DEREF_PTR_SET(pp_value, int8_t, vp_value);
        break;

    case DTH_TYPE_S16:

        {@dthroot}_DEREF_PTR_SET(pp_value, int16_t, vp_value);
        break;

    case DTH_TYPE_S32:

        {@dthroot}_DEREF_PTR_SET(pp_value, int32_t, vp_value);
        break;

    case DTH_TYPE_S64:

        {@dthroot}_DEREF_PTR_SET(pp_value, int64_t, vp_value);
        break;

    default:
        {@dthroot}_SYSLOG(LOG_ERR, "%s: no a signed DTH type", pp_elem->path);
        vl_result = TAT_ERROR_TYPE;
    }

    if ( 0 == vl_result ) {
        {@dthroot}_SYSLOGSTR(LOG_INFO, "%s: %lld\n", pp_elem->path, vp_value);
    }

    return vl_result;
}

int64_t {@prefix}set_signed(struct dth_element *pp_elem, void *pp_value,
    int *pp_result)
{
    int64_t vl_signed = 0LL;

    switch (pp_elem->type)
    {
    case DTH_TYPE_S8:

        vl_signed = {@dthroot}_DEREF_PTR(pp_value, int8_t);
        *pp_result = 0;
        break;

    case DTH_TYPE_S16:

        vl_signed = {@dthroot}_DEREF_PTR(pp_value, int16_t);
        *pp_result = 0;
        break;

    case DTH_TYPE_S32:

        vl_signed = {@dthroot}_DEREF_PTR(pp_value, int32_t);
        *pp_result = 0;
        break;

    case DTH_TYPE_S64:

        vl_signed = {@dthroot}_DEREF_PTR(pp_value, int64_t);
        *pp_result = 0;
        break;

    default:
        {@dthroot}_SYSLOG(LOG_ERR, "%s: no a signed DTH type", pp_elem->path);
        *pp_result = TAT_ERROR_TYPE;
    }

    if ( 0 == *pp_result ) {
        {@dthroot}_SYSLOGSTR(LOG_INFO, "set %s to %llu\n", pp_elem->path, vl_signed);
    }

    return vl_signed;
}

int {@prefix}get_unsigned(uint64_t vp_value, struct dth_element* pp_elem,
    void *pp_value)
{
    int vl_result = 0;

    switch (pp_elem->type)
    {
    case DTH_TYPE_U8:

        {@dthroot}_DEREF_PTR_SET(pp_value, uint8_t, vp_value);
        break;

    case DTH_TYPE_U16:

        {@dthroot}_DEREF_PTR_SET(pp_value, uint16_t, vp_value);
        break;

    case DTH_TYPE_U32:

        {@dthroot}_DEREF_PTR_SET(pp_value, uint32_t, vp_value);
        break;

    case DTH_TYPE_U64:

        {@dthroot}_DEREF_PTR_SET(pp_value, uint64_t, vp_value);
        break;

    default:
        {@dthroot}_SYSLOG(LOG_ERR, "%s: no an unsigned DTH type",
            pp_elem->path);
        vl_result = TAT_ERROR_TYPE;
    }

    if ( 0 == vl_result ) {
        {@dthroot}_SYSLOGSTR(LOG_INFO, "%s: %llu\n", pp_elem->path, vp_value);
    }

    return vl_result;
}

uint64_t {@prefix}set_unsigned(struct dth_element *pp_elem, void *pp_value,
    int *pp_result)
{
    uint64_t vl_unsigned = 0ULL;

    switch (pp_elem->type)
    {
    case DTH_TYPE_U8:

        vl_unsigned = {@dthroot}_DEREF_PTR(pp_value, uint8_t);
        *pp_result = 0;
        break;

    case DTH_TYPE_U16:

        vl_unsigned = {@dthroot}_DEREF_PTR(pp_value, uint16_t);
        *pp_result = 0;
        break;

    case DTH_TYPE_U32:

        vl_unsigned = {@dthroot}_DEREF_PTR(pp_value, uint32_t);
        *pp_result = 0;
        break;

    case DTH_TYPE_U64:

        vl_unsigned = {@dthroot}_DEREF_PTR(pp_value, uint64_t);
        *pp_result = 0;
        break;

    default:
        {@dthroot}_SYSLOG(LOG_ERR, "%s: no an unsigned DTH type",
            pp_elem->path);
        *pp_result = TAT_ERROR_TYPE;
    }

    if ( 0 == *pp_result ) {
        {@dthroot}_SYSLOGSTR(LOG_INFO, "set %s to %lld\n", pp_elem->path,
            vl_unsigned);
    }

    return vl_unsigned;
}

int {@prefix}get_float(float vp_value, struct dth_element* pp_elem,
    void *pp_value)
{
    int vl_result = TAT_ERROR_TYPE;

    if ( DTH_TYPE_FLOAT == pp_elem->type )
    {
        {@dthroot}_DEREF_PTR_SET(pp_value, float, vp_value);
        vl_result = 0;
    }
    else
    {
        {@dthroot}_SYSLOG(LOG_ERR, "%s: no a float DTH type", pp_elem->path);
    }

    if ( 0 == vl_result ) {
        {@dthroot}_SYSLOGSTR(LOG_INFO, "%s: %f\n", pp_elem->path, vp_value);
    }

    return vl_result;
}

float {@prefix}set_float(struct dth_element* pp_elem, void *pp_value,
    int *pp_result)
{
    float vl_float = 0.0;

    if ( DTH_TYPE_FLOAT == pp_elem->type )
    {
        vl_float = {@dthroot}_DEREF_PTR(pp_value, float);
        *pp_result = 0;

        {@dthroot}_SYSLOGSTR(LOG_INFO, "set %s to %f\n", pp_elem->path,
            vl_float);
    }
    else
    {
        {@dthroot}_SYSLOG(LOG_ERR, "%s: no a float DTH type", pp_elem->path);
        *pp_result = TAT_ERROR_TYPE;
    }

    if ( 0 == *pp_result ) {
        {@dthroot}_SYSLOGSTR(LOG_INFO, "set %s to %f\n", pp_elem->path,
            vl_float);
    }

    return vl_float;
}

/* \begin default debug implementation */

/* any non expected error, not error cases */
int g_{@prefix}syslog_level = ENUM_{@dthroot}_LOG_ERR;

void {@prefix}syslog(
    int vp_prio,
    int vp_output,
    const char *pp_func,
    const char *pp_file,
    int vp_line,
    const char *pp_format,
    ...)
{
    va_list vl_args;
    va_start(vl_args, pp_format);
    static char vl_msg[2048]="";

    vsnprintf(vl_msg, 2048, pp_format, vl_args);
    vl_msg[2047] = '\0';

    /* print line and code information on console if requested */
    if ( ( (vp_output & {@dthroot}_LOG_CONSOLE) != 0) && 
         ( {@dthroot}_SYSLOG_COND >= vp_prio ) )
    {
         if ( (vp_output & {@dthroot}_LOG_TEXTONLY) != 0 )
         {
             fprintf(stdout, vl_msg);
         }
         else
         {
             fprintf(stdout, "{@MODULE},%s; %s,%d(%d)\n\t%s\n\n", pp_func,
                 pp_file, vp_line, vp_prio, vl_msg);
         }
    }

    /* queue any warning or error, if requested */
    if ( ( (vp_output & {@dthroot}_LOG_STACK) != 0) &&
         ( ENUM_{@dthroot}_LOG_WARNING >= vp_prio ) )
    {
        dth_push_message(vp_prio, vl_msg);
    }

    /* add lines to syslog, if requested */
    if ( ( (vp_output & {@dthroot}_LOG_SYSLOG) != 0) &&
         ( {@dthroot}_SYSLOG_COND >= vp_prio ) )
    {
        syslog(vp_prio|LOG_TAT_FACILITY, vl_msg);
    }

    va_end(vl_args);
}

int g_{@prefix}syslog_loaded = 0;

#define LOG_CONF_FILE     TAT_PATH_OPT"/tat/tatcfg.conf"

#define SYSCMD_UPDATE_DBG_LEVEL(file) \
    "sed -e \"s#%s=%d#%s=%d#\" "file" > logconf.temp && mv -f logconf.temp "file

#define SYSCMD_RM_TMP_CONFIG_FILE \
    "rm -f logconf.temp"

#define MK_SYSCMD_UPDATE_DBG_LEVEL(buf, size, key, oldval, newval) \
    snprintf(buf, size, SYSCMD_UPDATE_DBG_LEVEL(LOG_CONF_FILE), key, oldval, \
        key, newval)

#define strcspace(str) (str + strspn(str, " \t"))

/**
 * Write or read the debug level of the module to/from the configuration file.
 * @param level [in/out] address of an integer specifying the value to write or
 * to receive the value read from file. Cannot be NULL.
 * @param saving [in] 0: extract the value, other: write it.
 * @return 0 if successful or -1 on error usually caused by a file access problem.
 */
int {@prefix}log_serialize(int *level, int saving)
{
    int ret = -1; /* 0 if success */
    FILE* fp = 0;
    int vl_level = -1;
    char vl_buf[256];
    static const char * vl_pat = "{@dthroot}_LOG_LEVEL";
    const size_t vl_patlen = strlen(vl_pat);
    const char *vl_gets, *vl_ptr;

    /* read the saved level from config file if any */
    fp = fopen(LOG_CONF_FILE, "r");
    if ( fp ) {
        /* read the file and search for the entry, if existing */
        do {
            vl_gets = fgets(vl_buf, 256, fp);

            vl_ptr = strcspace(vl_buf);
            if ( (vl_patlen < strlen(vl_ptr)) &&
                 strncmp(vl_ptr, vl_pat, vl_patlen) == 0 )
            {
                vl_ptr = strcspace(vl_ptr + vl_patlen);
                if (*vl_ptr == '=') {
                    vl_ptr++;
                    /* this is the key we are searching for */
                    vl_level = atoi(vl_ptr);
                }
            }
        } while ( (0 > vl_level) && (NULL != vl_gets) );
        fclose(fp);
    }

	if (saving) {
		if (0 > vl_level) {
			/* append missing value to file */
            /* reopen file in appending mode */
            fp = fopen(LOG_CONF_FILE, "a+");
            if (fp) {
                fprintf(fp, "\n%s=%d", vl_pat, *level);
                fflush(fp);
                fclose(fp);

                ret = 0;
            } else {
                fprintf(stderr, "fail to open log conf file for writting %s!\n",LOG_CONF_FILE);
            }
		} else {
            /* change the value in file. a temporary file is created during the
             * process */
            char vl_cmd[1024];
            MK_SYSCMD_UPDATE_DBG_LEVEL(vl_cmd, 1024, vl_pat, vl_level, *level);
            system(vl_cmd);

            /* remove temp file */
            system(SYSCMD_RM_TMP_CONFIG_FILE);

            ret = 0;
		}
	} else if (0 <= vl_level) {
        *level = vl_level;
        ret = 0;
    }

    return ret;
}

void {@prefix}syslog_set_level(int level)
{
    g_{@prefix}syslog_level = level;

    /* save to file */
    {@prefix}log_serialize(&level, 1);
}

int {@prefix}syslog_get_level(void)
{
    /* read debug level the first and not further more or the system will be
     * slowed
     */
    if ( 0 == g_{@prefix}syslog_loaded )
    {
        {@prefix}log_serialize(&g_{@prefix}syslog_level, 0);

        /* initialize syslog */
        openlog("{@module}", LOG_TAT_OPTION, LOG_TAT_FACILITY);

        g_{@prefix}syslog_loaded = 1;
    }

    return g_{@prefix}syslog_level;
}

/* module DTH structure registration */
int {@prefix}register_dth_struct(void)
{
    int vl_error = TAT_ERROR_OFF;
    struct dth_element *vl_elt = NULL;

    /* register application DTH structure */
    vl_elt = {@prefix}dth_struct;
    while ((vl_elt->path != NULL) && (vl_error == 0)) {
        vl_error = dth_register_element(vl_elt);
        if (TAT_OK(vl_error))
        {
             {@dthroot}_SYSLOG(LOG_DEBUG, "registered: %s", vl_elt->path);
        }

        vl_elt++;
    }
    
    return vl_error;
}

int {@prefix}unregister_dth_struct(void)
{
	int vl_error = TAT_ERROR_OFF;
#if 0
	struct dth_element *vl_elt = NULL;
#endif

	/* unregister application DTH structure */
    vl_error = dth_unregister_branch("/DTH/{@dthroot}");

#if 0
	vl_elt = {@prefix}dth_struct;
	while ( (vl_elt->path != NULL) && (vl_error == 0) ) {
		vl_error = dth_unregister_element(vl_elt);
		if (0 == vl_error)
			{@dthroot}_SYSLOG(LOG_DEBUG, "unregistered: %s", vl_elt->path);

		vl_elt++;
	}
#endif

    return vl_error;
}

/* default module debug implementation */
int {@prefix}get_debug(struct dth_element *elem, void *value)
{
	if (elem == NULL)
		return 0;

    u32* vl_switch = (u32 *)value;
    *vl_switch = {@prefix}syslog_get_level();

    return TAT_ERROR_OFF;
}

int {@prefix}set_debug(struct dth_element *elem, void *value)
{
    int vl_err = 0;

	if (elem == NULL)
		return 0;

    u32 vl_switch = *((u32 *)value);
    switch (vl_switch) {
    case  ENUM_{@dthroot}_LOG_EMERG: /* system is unusable */

        {@prefix}syslog_set_level(LOG_EMERG);
        break;

    case  ENUM_{@dthroot}_LOG_ALERT: /* action must be taken immediately */

        {@prefix}syslog_set_level(LOG_ALERT);
        break;

    case  ENUM_{@dthroot}_LOG_CRIT: /* critical conditions */

        {@prefix}syslog_set_level(LOG_CRIT);
		break;

    case  ENUM_{@dthroot}_LOG_ERR: /* error conditions */

        {@prefix}syslog_set_level(LOG_ERR);
        break;

    case  ENUM_{@dthroot}_LOG_WARNING: /* warning conditions */

        {@prefix}syslog_set_level(LOG_WARNING);
        break;

    case  ENUM_{@dthroot}_LOG_NOTICE: /* normal, but significant, condition */

        {@prefix}syslog_set_level(LOG_NOTICE);
        break;

    case  ENUM_{@dthroot}_LOG_INFO: /* informational message */

        {@prefix}syslog_set_level(LOG_INFO);
        break;

    case  ENUM_{@dthroot}_LOG_DEBUG: /* debug message */

        {@prefix}syslog_set_level(LOG_DEBUG);
        break;

    default:
		vl_err = TAT_BAD_REQ;
    }

    return vl_err;
}

int {@prefix}get_set_null(struct dth_element *elem, void *value)
{
    (void) elem, (void) value;

    return TAT_ERROR_OFF;
}

int {@prefix}exec_null(struct dth_element *elem)
{
	if (elem == NULL)
		return 0;
    /* does nothing and return 0 */
    return 0;
}

/* default module versioning implementation */
int {@prefix}get_version(struct dth_element *elem, void *value)
{
	if (elem == NULL)
		return 0;

    int vl_len = {@dthroot}_MODULE_VERSION_LEN;
    if (vl_len > 1023)
		vl_len = 1023;

    char *str = (char *)value;
    strncpy(str, {@dthroot}_MODULE_VERSION, vl_len);
    str[vl_len] = 0;

    {@dthroot}_SYSLOG(LOG_DEBUG, "Module version : %*s\n", vl_len, str);

    return TAT_ERROR_OFF;
}

/* END OF FILE */
