/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file     cli_io.c
 * \brief    this file defines CLI debug functions (for example : error or warning 
 *           messages display functions)
 * \author   ST-Ericsson
 */
/*****************************************************************************/
/*-----------------------------------------*/
/* INCLUDES                                */
/*-----------------------------------------*/

#include "cli_p.h"

/*-----------------------------------------*/
/* Defines                                 */
/*-----------------------------------------*/
#ifdef __ARM_LINUX
#define RED          "\033[31m"
#define BLACK_ON_RED "\033[41;30m"
#define INV_COLOR    "\033[7m"
#define RESET_COLOR  "\033[0m"
#endif

/*-----------------------------------------*/
/* Global Variables                        */
/*-----------------------------------------*/
int ena_display_error   = 0;
int ena_display_warning = 0;
int ena_display_msg     = 0;
int ena_display_debug   = 0;
 
 
/** ------------------------------------------------------------------
 * \ingroup : AteDebugfunc 
 * function : CLI_set_display_filter
 * \param   : int (for example: MASK_ERROR_DISPLAY | MASK_MSG_DISPLAY)
 * \return  : none
 * Description:
 *   
 * -----------------------------------------------------------------*/
EXPORT_C void CLI_set_display_filter (int ena)
{
    ena_display_error   =  ena & MASK_ERROR_DISPLAY;
    ena_display_msg     =  ena & MASK_MSG_DISPLAY;
    ena_display_warning =  ena & MASK_WARNING_DISPLAY;
    ena_display_debug   =  ena & MASK_DEBUG_DISPLAY;
}


/** ------------------------------------------------------------------
 * \ingroup : AteDebugfunc 
 * function : CLI_get_display_filter
 * \param   : none
 * \return  : int (for example: MASK_ERROR_DISPLAY | MASK_MSG_DISPLAY)
 * Description:
 *   
 * -----------------------------------------------------------------*/
EXPORT_C int CLI_get_display_filter (void)
{
    int ena = ena_display_debug | ena_display_warning  | ena_display_msg  | ena_display_error;
    return ena;
}


/** ------------------------------------------------------------------
 * \ingroup : AteDebugfunc 
 * function : CLI_disp_error
 * \param   : error message to display
 * \return  : none
 * Description:
 *   
 * -----------------------------------------------------------------*/

EXPORT_C void CLI_disp_error( char *format, ...)
{
    va_list ap;
	char outBuffer[MAX_PARAM_LENGTH];

    if (!ena_display_error)
		return;

#ifdef __ARM_LINUX
	CLI_io_write_channel(ERROR_DISPLAY, "%s",BLACK_ON_RED);
#endif
	CLI_io_write_channel(ERROR_DISPLAY, "ERROR : " );
    va_start(ap, format);
    vsprintf( outBuffer, format, ap );
    CLI_io_write_channel(ERROR_DISPLAY, outBuffer);
    va_end(ap);
#ifdef __ARM_LINUX
	CLI_io_write_channel(ERROR_DISPLAY, "%s",RESET_COLOR);
#endif
}

/** ------------------------------------------------------------------
 * \ingroup : AteDebugfunc 
 * function : CLI_disp
 * \param   : message to display
 * \return  : none
 * Description:
 *   display a message whatever is the filter
 * -----------------------------------------------------------------*/
EXPORT_C void CLI_disp( char *format, ...)
{
    va_list ap;
    char outBuffer[MAX_PARAM_LENGTH];

    va_start(ap, format);
    vsprintf( outBuffer, format, ap );
    CLI_io_write_channel(MSG_DISPLAY, outBuffer);
    va_end(ap);
}


/** ------------------------------------------------------------------
 * \ingroup : AteDebugfunc 
 * function : CLI_disp_msg
 * \param   : message to display
 * \return  : none
 * Description:
 *   
 * -----------------------------------------------------------------*/
EXPORT_C void CLI_disp_msg( char *format, ...)
{
    va_list ap;
    char outBuffer[MAX_PARAM_LENGTH];

    if (!ena_display_msg)	
		return ;

    va_start(ap, format);
    vsprintf( outBuffer, format, ap );
    CLI_io_write_channel(MSG_DISPLAY, outBuffer);
    va_end(ap);
}


/** ------------------------------------------------------------------
 * \ingroup : AteDebugfunc 
 * function : CLI_disp_warning
 * \param   : warning message to display
 * \return  : none
 * Description:
 *   
 * -----------------------------------------------------------------*/
EXPORT_C void CLI_disp_warning( char *format, ...)
{
    va_list ap;
    char outBuffer[MAX_PARAM_LENGTH];

    if (!ena_display_warning)
		return;

#ifdef __ARM_LINUX
	CLI_io_write_channel(WARNING_DISPLAY, "%s",INV_COLOR);
#endif
	CLI_io_write_channel(WARNING_DISPLAY, "WARNING : " );        
    va_start(ap, format);
    vsprintf( outBuffer, format, ap );
    CLI_io_write_channel(WARNING_DISPLAY, outBuffer);
    va_end(ap);
#ifdef __ARM_LINUX
	CLI_io_write_channel(WARNING_DISPLAY, "%s",RESET_COLOR);
#endif
}


/** ------------------------------------------------------------------
 * \ingroup : AteDebugfunc 
 * function : CLI_disp_debug
 * \param   : debug message to display
 * \return  : none
 * Description:
 *   
 * -----------------------------------------------------------------*/
EXPORT_C void CLI_disp_debug(char *format, ...)
{
    va_list ap;
    char outBuffer[MAX_PARAM_LENGTH];

    if (!ena_display_debug)    
        return;
    
    va_start(ap, format);
    vsprintf( outBuffer, format, ap );
    CLI_io_write_channel(DEBUG_DISPLAY, outBuffer);
    va_end(ap);
}


/** ------------------------------------------------------------------
 * \ingroup : AteDebugfunc 
 * function : CLI_disp_it_error
 * \param   : error message to display 
 * \return  : none
 * Description:
 *  can be called under IT (Replace CLI_disp_error)
 * -----------------------------------------------------------------*/
EXPORT_C void CLI_disp_it_error( char *format, ...)
{
    va_list ap;
    static char outBuffer[MAX_PARAM_LENGTH];

    if (!ena_display_error)
		return;
#ifdef __ARM_LINUX
	CLI_io_write_channel(ERROR_DISPLAY, "%s",BLACK_ON_RED);
#endif
    CLI_io_write_channel(ERROR_DISPLAY, "ERROR : " );    
    va_start(ap, format);
    vsprintf( outBuffer, format, ap );
    CLI_io_write_channel(ERROR_IT_DISPLAY, outBuffer);
    va_end(ap);
#ifdef __ARM_LINUX
	CLI_io_write_channel(ERROR_DISPLAY, "%s",RESET_COLOR);
#endif

}


/** ------------------------------------------------------------------
 * \ingroup : AteDebugfunc 
 * function : CLI_disp_it_msg
 * \param   : message to display
 * \return  : none
 * Description:
 *   can be called under IT (Replace CLI_disp_msg)
 * -----------------------------------------------------------------*/
EXPORT_C void CLI_disp_it_msg( char *format, ...)
{
    va_list ap;
    static char outBuffer[MAX_PARAM_LENGTH];

    if (!ena_display_msg)	
		return;
	
    va_start(ap, format);
    vsprintf( outBuffer, format, ap );
    CLI_io_write_channel(MSG_IT_DISPLAY, outBuffer);
    va_end(ap);
}
