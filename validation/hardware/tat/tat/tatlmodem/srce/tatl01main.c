/*
* =====================================================================================
* 
*        Filename:  tatl01main.c
* 
*     Description:  TATLMODEM main
* 
*         Version:  1.0
*         Created:  06-16-2010
*
*        Compiler:  gcc
* 
*        Company:  ST-Ericsson
* 		
*	  Copyright (C) ST-Ericsson 2009
* =====================================================================================
*/

#define TATL01MAIN_C
#include "tatlmodem.h"
#undef TATL01MAIN_C

/* uses TAT MODEM SERVICES */
#include "tatmodemlib.h"

/* uses Powerservices */
#include "hats_power.h"

u32 v_tatlmodem_set_mode = TAT_MODEM_MODE_UNKNOWN;
u32 v_tatlmodem_open_close = MODEM_CLOSE_ISI;
u32 v_tatlmodem_pwr_mode;
u32 v_tatlmodem_pwr_mode_status;

int dth_init_service(void)
{
    int vl_err;

    /* set TAT MODEM SERVICES library to same logging level as myself */
    tat_modem_set_log(tatlmodem_syslog_get_level());

    /* register the DTH structure */
    vl_err = tatlmodem_register_dth_struct();

    return vl_err;
}

int DthModem_exec(struct dth_element *elem)
{
    int vl_err = EBADRQC;
    int vl_mode;

    SYSLOG(LOG_DEBUG, "DthModem_exec(%u)", elem->user_data);

    switch (elem->user_data)
    {
    case ACT_MODEM_CONNECTION:

        vl_err = 0;
        switch ( v_tatlmodem_open_close )
        {
        case MODEM_CLOSE_ISI:

            vl_err = tat_modem_close();
            break;

        case MODEM_OPEN_ISI:

            vl_err = tat_modem_open();
            break;

        default:
            vl_err = EINVAL;
        }
        break;

    case ACT_MODEM_SET_MODE:

        vl_err = 0;
        switch ( v_tatlmodem_set_mode )
        {
        case MODEM_NORMAL_MODE:

            vl_err = tat_modem_set_mode(TAT_MODEM_MODE_NORMAL);
            break;

        case MODEM_LOCAL_MODE:

            vl_err = tat_modem_set_mode(TAT_MODEM_MODE_LOCAL);
            break;

        default:
            vl_err = EINVAL;
        }

        break;

    case ACT_MODEM_CONTEXT:

        vl_err = tat_modem_get_mode(TAT_MODEM_SRCE_AUTO, &vl_mode);
        break;

    case ACT_MODEM_POWER_MODE:

	vl_err = pwm_set_modem_mode(v_tatlmodem_pwr_mode);
	v_tatlmodem_pwr_mode_status = vl_err;
	SYSLOG(LOG_DEBUG, "pwm_set_modem_mode(%u)", v_tatlmodem_pwr_mode);
	break;
    }

    SYSLOG(LOG_DEBUG, "DthModem_exec(%u) exited with code %d", elem->user_data, vl_err);

    return vl_err;
}

int DthModem_get(struct dth_element *elem, void *value)
{
    int vl_err = EBADRQC;
    int vl_mode;

    SYSLOG(LOG_DEBUG, "DthModem_get(%u)", elem->user_data);

    switch (elem->user_data)
    {
    case ARG_MODEM_CONNECTION_STATE:

        vl_err = 0;
        switch ( tat_modem_get_state() )
        {
#if defined(CONFIG_PHONET)
		case TAT_MODEM_OFFLINE:
#else
		case TAT_MODEM_DISCONNECTED:
#endif
            *((uint32_t*)value) = MODEM_ISI_CLOSED;
            break;

#if defined(CONFIG_PHONET)
        case TAT_MODEM_ONLINE:
#else
		case TAT_MODEM_CONNECTED:
#endif			
            *((uint32_t*)value) = MODEM_ISI_OPENED;
            break;

        default:
            vl_err = EINVAL;
        }
        break;

    case ARG_MODEM_MODE:

        vl_mode = TAT_MODEM_MODE_UNKNOWN;
        tat_modem_get_mode(TAT_MODEM_SRCE_LIB, &vl_mode);

        vl_err = 0;
        switch ( vl_mode )
        {
        case TAT_MODEM_MODE_LOCAL:

            *((uint32_t*)value) = MODEM_LOCAL_MODE;
            break;

        case TAT_MODEM_MODE_NORMAL:

            *((uint32_t*)value) = MODEM_NORMAL_MODE;
            break;

        case TAT_MODEM_MODE_UNKNOWN:

            *((uint32_t*)value) = MODEM_UNAVAILABLE;
            break;

        case TAT_MODEM_MODE_ERROR:

            *((uint32_t*)value) = MODEM_IN_ERROR;
            break;

        case TAT_MODEM_MODE_RESET:

            *((uint32_t*)value) = MODEM_IN_RESET;
            break;

        default:
            vl_err = EINVAL;
        }

        break; /* ARG_MODEM_MODE */

    case ARG_MODEM_OWNER:

        vl_err = 0;
        tat_modem_get_owner((char*)value);
        if ( 0 == *((char*)value) )
        {
            strcpy((char*)value,"(none)");
        }
        break;

    case ARG_MODEM_POWER_MODE_STATE:
	vl_err = 0;
	*((uint32_t*)value) = v_tatlmodem_pwr_mode_status;
	break;
    }

    SYSLOG(LOG_DEBUG, "DthModem_get(%u) exited with code %d", elem->user_data, vl_err);

    return vl_err;
}

int DthModem_set(struct dth_element *elem, void *value)
{
    int vl_err = EBADRQC;

    SYSLOG(LOG_DEBUG, "DthModem_set(%u)", elem->user_data);

    switch (elem->user_data)
    {
    case ACT_MODEM_CONNECTION:

        v_tatlmodem_open_close = *((uint32*)value);
        vl_err = 0;
        break;

    case ACT_MODEM_SET_MODE:

        v_tatlmodem_set_mode = *((uint32*)value);
        vl_err = 0;
        break;

    case ACT_MODEM_POWER_MODE:

	switch(*(uint32*)value)
	{
		case MODEM_MODE_LP_DISABLE:
			v_tatlmodem_pwr_mode = MODEM_LP_DISABLED;
			break;
		case MODEM_MODE_LP_ENABLE:
			v_tatlmodem_pwr_mode = MODEM_LP_ENABLED;
			break;
	}
        vl_err = 0;
        break;
    }

    SYSLOG(LOG_DEBUG, "DthModem_set(%u) exited with code %d", elem->user_data, vl_err);

    return vl_err;	
}

int DthModem_setDebug(struct dth_element *elem, void *value)
{
    int vl_err = EBADRQC;

    SYSLOG(LOG_DEBUG, "DthModem_setDebug(%u)", elem->user_data);

    switch (elem->user_data)
    {
    case ARG_MODEM_DEBUG_LEVEL:

        /* call default interface */
        tatlmodem_set_debug(elem, value);

        /* change TAT MODEM SERVICES log level */
        tat_modem_set_log(tatlmodem_syslog_get_level());

        vl_err = 0;
        break;
    }

    SYSLOG(LOG_DEBUG, "DthModem_set(%u) exited with code %d", elem->user_data, vl_err);

    return vl_err;	
}

