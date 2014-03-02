/*
* Copyright (C) ST-Ericsson 2009
*
* gpsinterface.c
* Date: 17-07-2009
* Author: Anil Kumar
* Email : anil.nambiar@stericsson.com
*/

#include <errno.h>
#include <termios.h>
#ifndef ARM_LINUX_AGPS_FTR
#include "android_log.h"
//#include <cutils/properties.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "gpsclient.h"


#ifdef AGPS_ANDR_GINGER_FTR
#include "hardware/gps.h"
#include <semaphore.h>
#include <pthread.h>
#else
#include "hardware_legacy/gps.h"
#ifdef AGPS_ANDR_ECLAIR_FTR
#include "hardware_legacy/gps_ni.h"
#endif /*  AGPS_ANDR_ECLAIR_FTR */
#endif /*  AGPS_ANDR_GINGER_FTR*/


#ifdef LOG_TAG
#undef LOG_TAG
#endif
#define  LOG_TAG  "gpsinterface"
#ifdef AGPS_ANDROID_HEX_NOTIFICATION_FTR
#define  MAX_STRING_LEN                2048    /*Value obtained from gpsni.h */
#endif /*AGPS_ANDROID_HEX_NOTIFICATION_FTR*/

#define  MILLISECONDS_IN_A_SECOND      1000
#define  DEFAULT_TIMEOUT_VALUE          200
#define  DEFAULT_H_ACCURACY             100
#define  DEFAULT_V_ACCURACY             100
#define  DEFAULT_AGE_LIMIT              100
/* +LMSqc49445_3 RRR August 5th 2010 */
#define LEAP(x)                        (((x) % 4) == 0 && (((x) % 100) != 0 || ((x) % 400) == 0))

#define SEC_PER_MINUTE                  (60)
#define SEC_PER_HOUR                    (60*SEC_PER_MINUTE)
#define SEC_PER_DAY                     (24*SEC_PER_HOUR)

#define DAYS_IN_LEAP_YEAR              (366)
#define DAYS_IN_NORMAL_YEAR            (365)
/* +LMSqc49445_3 RRR August 5th 2010 */

/*
    CONSTELLATION VALUES
    LBS_GN_NO_CONSTELL    = 0,           ///< Invalid Constellation
    LBS_GN_GPS_CONSTELL   = 1,             ///< Navstar GPS
    LBS_GN_GLON_CONSTELL = 2,             ///< Glonass
*/
#define LBS_GN_GLON_CONSTELL              2

/*
    GPS SV ID  / PRN         1-32
    Glonass Slot number      1-24

    GPS satellites are identified by their PRN, which range from 1 up to 32.
    The WAAS system has numbers 33-64 to identify its satellites.
    The numbers 65-88 are used for GLONASS satellites (64 + satellite slot number).
    Glonass ID      65-88
*/
#define LBS_FIRST_GLONASS_SVID                64



#ifdef AGPS_ANDR_GINGER_FTR
static pthread_mutex_t sEventMutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t sEventCond   = PTHREAD_COND_INITIALIZER;

// data written to by GPS callbacks
static GpsLocation  sGpsLocation;
static GpsStatus    sGpsStatus;
static GpsSvStatus  sGpsSvStatus;
static AGpsStatus   sAGpsStatus;
static GpsNiNotification  sGpsNiNotification;

// buffer for NMEA data
#define NMEA_SENTENCE_LENGTH             100
#define NMEA_SENTENCE_COUNT               40
#define NMEA_GPZDA                  "$GPZDA"
#define NMEA_GPZDA_LENGTH                  6

typedef struct
{
    GpsUtcTime  timestamp;
    char        nmea[NMEA_SENTENCE_LENGTH];
    int         length;
}NmeaSentence;

NmeaSentence sNmeaBuffer;
static int mNmeaSentenceCount = 0;

// a copy of the data shared by android_location_GpsLocationProvider_wait_for_event
// and android_location_GpsLocationProvider_read_status
static GpsLocation  sGpsLocationCopy;
static GpsStatus    sGpsStatusCopy;
static GpsSvStatus  sGpsSvStatusCopy;
static AGpsStatus   sAGpsStatusCopy;
NmeaSentence sNmeaBufferCopy;
static GpsNiNotification  sGpsNiNotificationCopy;

enum CallbackType
{
    kLocation            = 1,
    kStatus              = 2,
    kSvStatus            = 4,
    kAGpsStatus          = 8,
    kXtraDownloadRequest = 16,
    kDisableRequest      = 32,
    kNmeaAvailable       = 64,
    kNiNotification      = 128
#ifdef AGPS_ANDR_GINGER_FTR
    ,
    krilCellId           = 256,
    krilMac              = 512,
    krilsetid            = 1024
#endif
};

static int sPendingCallbacks;
uint8_t vg_gpsCallbackThreadExit;

#endif /* #ifdef AGPS_ANDR_GINGER_FTR*/



static GpsCallbacks* gpsCallbacks;
//GpsNotificationCallbacks* gpsNotificationCallbacks;
static AGpsCallbacks    *agpsCallbacks;
#ifdef AGPS_ANDR_ECLAIR_FTR
static GpsNiCallbacks*  gpsNiCallbacks;
#endif /* #ifdef AGPS_ANDR_ECLAIR_FTR */
#ifdef AGPS_ANDR_GINGER_FTR
static AGpsRilCallbacks *agpsRilCallbacks;
#endif

unsigned int FixRate;




void gps_ProcessNmeaCallback( t_gpsClient_NmeaData *NmeaData );
void gps_ProcessNavdataCallback(t_gpsClient_NavData *NavData);
void gps_ProcessAgpsStatusCallback( e_gpsClient_AgpsStatData *p_AgpsStat );
void gps_ProcessNotificationCallback( t_gpsClient_NotifyData *p_Notif );
void gps_ProcessNetworkInitiatedCallback(uint8_t ni_notification);
#ifdef AGPS_ANDR_GINGER_FTR
void gps_ProcessRilCallback( e_gpsClient_NetworkInfoType infoType );
void gps_ProcessRilSetIdCallback(void);
#endif

t_gpsClient_Callbacks  gpsClientCallbacks = {
    .nav_cb = gps_ProcessNavdataCallback,
    .nmea_cb = gps_ProcessNmeaCallback,
    .ni_cb = gps_ProcessNetworkInitiatedCallback
};

t_gpsClient_NiCallbacks gpsClientNiCallbacks= {
   .notify_cb = NULL
};

t_gpsClient_agpsCallbacks gpsClientAgpsCallbacks= {
   .agpsstat_cb = NULL
};

#ifdef AGPS_ANDR_GINGER_FTR
t_gpsClient_RilCallbacks gpsClientRilCallbacks= {
    .ril_cb = NULL,
    .ril_setid_cb = NULL
};
#endif


/*****************************************************************************
* agpsinterface_status_callback :
*****************************************************************************/
static void agpsinterface_status_callback(AGpsStatus* status)
{
#ifdef AGPS_ANDR_GINGER_FTR
    pthread_mutex_lock(&sEventMutex);

    sPendingCallbacks |= kAGpsStatus;
    memcpy(&sAGpsStatus, status, sizeof(sAGpsStatus));

    pthread_cond_signal(&sEventCond);
    pthread_mutex_unlock(&sEventMutex);
#else
    agpsCallbacks->status_cb( status);
#endif
}


/*****************************************************************************
* gpsinterface_location_callback :
*****************************************************************************/
static void gpsinterface_location_callback(GpsLocation* location)
{
#ifdef AGPS_ANDR_GINGER_FTR
    pthread_mutex_lock(&sEventMutex);

    sPendingCallbacks |= kLocation;
    memcpy(&sGpsLocation, location, sizeof(sGpsLocation));

    pthread_cond_signal(&sEventCond);
    pthread_mutex_unlock(&sEventMutex);
#else
    gpsCallbacks->location_cb( location);
#endif
}


/*****************************************************************************
* gpsinterface_status_callback :
*****************************************************************************/
static void gpsinterface_status_callback(GpsStatus* status)
{
#ifdef AGPS_ANDR_GINGER_FTR
    pthread_mutex_lock(&sEventMutex);

    sPendingCallbacks |= kStatus;
    memcpy(&sGpsStatus, status, sizeof(sGpsStatus));

    pthread_cond_signal(&sEventCond);
    pthread_mutex_unlock(&sEventMutex);
#else
    gpsCallbacks->status_cb( status);
#endif
}


/*****************************************************************************
* gpsinterface_sv_status_callback :
*****************************************************************************/
static void gpsinterface_sv_status_callback(GpsSvStatus* sv_status)
{
#ifdef AGPS_ANDR_GINGER_FTR
    pthread_mutex_lock(&sEventMutex);

    sPendingCallbacks |= kSvStatus;
    memcpy(&sGpsSvStatus, sv_status, sizeof(GpsSvStatus));

    pthread_cond_signal(&sEventCond);
    pthread_mutex_unlock(&sEventMutex);
#else
    gpsCallbacks->sv_status_cb( sv_status);
#endif
}


/*****************************************************************************
* gpsinterface_nmea_callback :
*****************************************************************************/
static void gpsinterface_nmea_callback(GpsUtcTime utcTime, const char* nmea, int length)
{
#ifdef AGPS_ANDR_GINGER_FTR
    pthread_mutex_lock(&sEventMutex);

    if (length >= NMEA_SENTENCE_LENGTH) {
        ALOGE("NMEA data too long in nmea_callback (length = %d)\n", length);
        length = NMEA_SENTENCE_LENGTH - 1;
    }
    /* To be checked
    if (mNmeaSentenceCount >= NMEA_SENTENCE_COUNT)
    {
        ALOGE("NMEA data overflowed buffer\n");
        pthread_mutex_unlock(&sEventMutex);
        return;
    }
    */
    sPendingCallbacks |= kNmeaAvailable;
    sNmeaBuffer.timestamp = utcTime;
    memcpy(sNmeaBuffer.nmea, nmea, length);
    sNmeaBuffer.length = length;

    pthread_cond_signal(&sEventCond);
    pthread_mutex_unlock(&sEventMutex);
#else
    gpsCallbacks->nmea_cb(utcTime, (char *)nmea, length );
#endif
}


/*****************************************************************************
* gpsinterface_notification_callback :
*****************************************************************************/
static void gpsinterface_notification_callback( GpsNiNotification *gpsNotif)
{
#ifdef AGPS_ANDR_GINGER_FTR
    pthread_mutex_lock(&sEventMutex);

    sPendingCallbacks |= kNiNotification;
    memcpy(&sGpsNiNotification, gpsNotif, sizeof(GpsNiNotification));

    pthread_cond_signal(&sEventCond);
    pthread_mutex_unlock(&sEventMutex);
#else
    gpsNiCallbacks->notify_cb(gpsNotif);
#endif
}


#ifdef AGPS_ANDR_GINGER_FTR
/*****************************************************************************
* gpsThread_CallbackEventHandler :
*****************************************************************************/
void gpsThread_CallbackEventHandler()
{
    while (vg_gpsCallbackThreadExit == 0)
    {
         pthread_mutex_lock(&sEventMutex);

        while (sPendingCallbacks == 0) {
            pthread_cond_wait(&sEventCond, &sEventMutex);
        }

        // copy and clear the callback flags
        int pendingCallbacks = sPendingCallbacks;
        sPendingCallbacks = 0;
        int nmeaSentenceCount = mNmeaSentenceCount;
        mNmeaSentenceCount = 0;
        /* copy everything and unlock the mutex before calling into Java code to avoid
           the possibility of timeouts in the GPS engine.*/

        DEBUG_LOG_PRINT_LEV2(("gpsThread_CallbackEventHandler Pending callback value %d",  pendingCallbacks));

        if (pendingCallbacks & kLocation)
            memcpy(&sGpsLocationCopy, &sGpsLocation, sizeof(sGpsLocationCopy));
        if (pendingCallbacks & kStatus)
            memcpy(&sGpsStatusCopy, &sGpsStatus, sizeof(sGpsStatusCopy));
        if (pendingCallbacks & kSvStatus)
            memcpy(&sGpsSvStatusCopy, &sGpsSvStatus, sizeof(sGpsSvStatusCopy));
        if (pendingCallbacks & kAGpsStatus)
            memcpy(&sAGpsStatusCopy, &sAGpsStatus, sizeof(sAGpsStatusCopy));
        if (pendingCallbacks & kNmeaAvailable)
            memcpy(&sNmeaBufferCopy, &sNmeaBuffer, sizeof(sNmeaBuffer));

        if (pendingCallbacks & kNiNotification)
            memcpy(&sGpsNiNotificationCopy, &sGpsNiNotification, sizeof(sGpsNiNotificationCopy));


        pthread_mutex_unlock(&sEventMutex);

        if (pendingCallbacks & kLocation) {
            gpsCallbacks->location_cb( &sGpsLocationCopy);
            }
        if (pendingCallbacks & kStatus) {
            gpsCallbacks->status_cb( &sGpsStatusCopy);
            }
        if (pendingCallbacks & kSvStatus) {
            gpsCallbacks->sv_status_cb( &sGpsSvStatus);
            }
        if (pendingCallbacks & kAGpsStatus) {
            agpsCallbacks->status_cb (&sAGpsStatusCopy);
            }
        if (pendingCallbacks & kNmeaAvailable) {
            gpsCallbacks->nmea_cb( sNmeaBufferCopy.timestamp,sNmeaBufferCopy.nmea, sNmeaBufferCopy.length);
            }
        if (pendingCallbacks & kNiNotification) {
            gpsNiCallbacks->notify_cb(&sGpsNiNotificationCopy);
            }
        if (pendingCallbacks & krilMac) {
           agpsRilCallbacks->request_refloc(AGPS_RIL_REQUEST_REFLOC_MAC);
            }
        if (pendingCallbacks & krilCellId) {
           agpsRilCallbacks->request_refloc(AGPS_RIL_REQUEST_REFLOC_CELLID);
            }
        if(pendingCallbacks & krilsetid) {
            agpsRilCallbacks->request_setid(AGPS_RIL_REQUEST_SETID_IMSI);
            }
    }
}
#endif /*AGPS_ANDR_GINGER_FTR*/


/*****************************************************************************
* gps_init :
*****************************************************************************/
static int gps_init(GpsCallbacks* callbacks)
{
    gpsCallbacks = callbacks;

    gpsClient_Init(&gpsClientCallbacks);

#ifdef AGPS_ANDR_GINGER_FTR
    vg_gpsCallbackThreadExit = 0;
    /* Call for creating a thread with android registered callback */
    callbacks->create_thread_cb("report_android",gpsThread_CallbackEventHandler, (void *)NULL);
#endif    /*#ifdef AGPS_ANDR_GINGER_FTR*/
    return 0;
}


/*****************************************************************************
* gps_cleanup :
*****************************************************************************/
static void gps_cleanup(void)
{
    /* when stopping the service itself all the clean up activity would have been finished */
    gpsClient_Disable();
    vg_gpsCallbackThreadExit =1;


    return;
}


/*****************************************************************************
* gps_start :
*****************************************************************************/
static int gps_start()
{
    int retVal;
    int outputType = 1;
    int nmeaMask = 0;

    DEBUG_LOG_PRINT_LEV2(("SM: gps_start() in gpsinterface fixRate is  %d\n", FixRate ));


    gpsClient_ServiceStart( );

    if( FixRate >= 0xFFFF * 1000 )
    {
        unsigned int timeOut = DEFAULT_TIMEOUT_VALUE * MILLISECONDS_IN_A_SECOND;
        unsigned int horizAccuracy = DEFAULT_H_ACCURACY;
        unsigned int vertAccuracy = DEFAULT_V_ACCURACY;
        unsigned int ageLimit = DEFAULT_AGE_LIMIT;

        retVal = gpsClient_SingleShotFix( outputType, nmeaMask, timeOut, horizAccuracy, vertAccuracy, ageLimit);
    }
    else
    {
        retVal = gpsClient_PeriodicFix(outputType, nmeaMask, FixRate);
    }


    if( retVal == GPSCLIENT_NO_ERROR )
    {
        GpsStatus status;
        status.status = GPS_STATUS_ENGINE_ON;
        gpsinterface_status_callback( &status);
        status.status = GPS_STATUS_SESSION_BEGIN;
        gpsinterface_status_callback( &status);
    }

    return retVal;
}


/*****************************************************************************
* gps_stop :
*****************************************************************************/
static int gps_stop()
{

        int retVal;

        retVal = gpsClient_Stop();

        if( retVal == GPSCLIENT_NO_ERROR )
        {
            GpsStatus status;
            status.status = GPS_STATUS_SESSION_END;
            gpsinterface_status_callback( &status);
            status.status = GPS_STATUS_ENGINE_OFF;
            gpsinterface_status_callback( &status);
        }

        return retVal;
}


/*****************************************************************************
* gps_set_fix_frequency :
*****************************************************************************/
static int gps_set_fix_frequency( int frequency )
{
    FixRate = frequency * MILLISECONDS_IN_A_SECOND;
    return 0;
}


/*****************************************************************************
* gps_inject_time :
*****************************************************************************/
static int gps_inject_time(GpsUtcTime time, int64_t timeReference, int uncertainty)
{
    /*Currently STEricsson GPS does not support option to inject UTC time though application*/
    return 0;
}


/*****************************************************************************
* gps_delete_aiding_data :
*****************************************************************************/
static void gps_delete_aiding_data(GpsAidingData aidingDataType)
{
    uint16_t DeleteaidingDataType = (uint16_t)aidingDataType;
    /*Since enum is same we can copy as is*/
    gpsClient_DeleteAidingData( DeleteaidingDataType );

}


#ifdef AGPS_ANDR_GINGER_FTR
/*****************************************************************************
* gps_set_position_mode :
*****************************************************************************/
static int gps_set_position_mode(GpsPositionMode mode, uint32_t Recurrence, uint32_t fix_frequency, uint32_t Pref_Acc, uint32_t Pref_Time)
{
    FixRate = fix_frequency;
    return gpsClient_SetPositionMode( mode );
}
#else


/*****************************************************************************
* gps_set_position_mode :
*****************************************************************************/
static int gps_set_position_mode(GpsPositionMode mode, int fix_frequency)
{
    FixRate = fix_frequency * MILLISECONDS_IN_A_SECOND;
    return gpsClient_SetPositionMode( mode );
}
#endif /*#ifdef AGPS_ANDR_GINGER_FTR*/


#ifdef AGPS_ANDR_CUPCAKE_FTR
/*****************************************************************************
* gps_set_apn :
*****************************************************************************/
int  gps_set_apn( const char* apn )
{
    //This is not implemented in this release
    return 0;
}


/*****************************************************************************
* gps_set_slpAddress :
*****************************************************************************/
int gps_set_slpAddress( uint32_t addr, int port )
{
     return agpsClient_SlpAddress( addr, port );
}


const GpsSuplInterface gpsSuplInterface = {
    gps_set_apn,
    gps_set_slpAddress
};

const GpsSuplInterface* gps_get_supl_interface()
{
    return &gpsSuplInterface;
}
#endif /* #ifdef AGPS_ANDR_CUPCAKE_FTR */


#if defined(AGPS_ANDR_DONUT_FTR) || defined( AGPS_ANDR_ECLAIR_FTR )
/*****************************************************************************
* gps_inject_location :
*****************************************************************************/
int gps_inject_location(double latitude, double longitude, float accuracy)
{
    // We just ignore this and dont use it
    return 0;
}


/*****************************************************************************
* agps_init :
*****************************************************************************/
static void agps_init( AGpsCallbacks* callbacks )
{
    agpsCallbacks = callbacks;
    gpsClientAgpsCallbacks.agpsstat_cb = gps_ProcessAgpsStatusCallback;

    gpsClient_AgpsInit(&gpsClientAgpsCallbacks);
}


/*****************************************************************************
* agps_data_conn_open :
*****************************************************************************/
static int  agps_data_conn_open( const char* apn )
{
    gpsclient_DataConnOpen( (char *)apn );
    return 0;
}


/*****************************************************************************
* agps_data_conn_closed :
*****************************************************************************/
static int agps_data_conn_closed( )
{
    gpsclient_DataConnClosed( );
    return 0;
}


/*****************************************************************************
* agps_data_conn_failed :
*****************************************************************************/
static int agps_data_conn_failed()
{
    gpsclient_DataConnFailed( );
    return 0;
}


/*****************************************************************************
* agps_set_server :
*****************************************************************************/
static int agps_set_server( AGpsType type, const char* hostname, int port )
{
    gpsClient_SlpAddress((char *)hostname,port);
    return 0;
}

const AGpsInterface agpsInterface = {
#ifdef AGPS_ANDR_GINGER_FTR
    sizeof(AGpsInterface),
#endif /*AGPS_ANDR_GINGER_FTR*/
    agps_init,
    agps_data_conn_open,
    agps_data_conn_closed,
    agps_data_conn_failed,
    agps_set_server,
};

const AGpsInterface* agps_get_interface()
{
    return &agpsInterface;
}
#endif /* defined(AGPS_ANDR_DONUT_FTR) || defined( AGPS_ANDR_ECLAIR_FTR ) */


#ifdef AGPS_ANDR_ECLAIR_FTR
/*****************************************************************************
* gps_ni_init :
*****************************************************************************/
static void gps_ni_init( GpsNiCallbacks *callbacks )
{
    if((callbacks != NULL ) && (callbacks->notify_cb != NULL))
        gpsClientNiCallbacks.notify_cb =  gps_ProcessNotificationCallback;
    else
        gpsClientNiCallbacks.notify_cb =  NULL;

    gpsclient_NotificationInit(&gpsClientNiCallbacks);

    gpsNiCallbacks = callbacks;
}


/*****************************************************************************
* gps_ni_response :
*****************************************************************************/
static void gps_ni_response(int notif_id, GpsUserResponseType user_response)
{
    e_gpsClient_UserResponseType usr_rsp = (e_gpsClient_UserResponseType)user_response;
    gpsclient_NotificationResponse(notif_id,usr_rsp);
}

const GpsNiInterface gpsNiInterface = {
#ifdef AGPS_ANDR_GINGER_FTR
    sizeof(GpsNiInterface),
#endif /*AGPS_ANDR_GINGER_FTR*/
    gps_ni_init,
    gps_ni_response
};


/*****************************************************************************
* gps_ni_get_interface :
*****************************************************************************/
const GpsNiInterface* gps_ni_get_interface()
{
    return &gpsNiInterface;
}
#endif  /*AGPS_ANDR_ECLAIR_FTR*/



#ifdef AGPS_ANDR_GINGER_FTR
/*****************************************************************************
* agps_ril_init : Opens the AGPS interface and provides the callback routines to the implemenation of this interface.
*****************************************************************************/
void  agps_ril_init( AGpsRilCallbacks* callbacks )
{
    if( callbacks != NULL )
    {
        gpsClientRilCallbacks.ril_cb       = gps_ProcessRilCallback;
        gpsClientRilCallbacks.ril_setid_cb = gps_ProcessRilSetIdCallback;

    }
    else
    {
        gpsClientRilCallbacks.ril_cb       = NULL;
        gpsClientRilCallbacks.ril_setid_cb = NULL;
    }

    gpsclient_AgpsRilInit(&gpsClientRilCallbacks);
    agpsRilCallbacks = callbacks;
}


/*****************************************************************************
* agps_set_mobile_info :
*****************************************************************************/
void agps_set_mobile_info( t_gpsClient_RefLocation *agps_reflocation, size_t sz_struct,const char* setid)
{
    static t_gpsClient_RefLocation agps_reflocation_tmp={0};
    static size_t sz_struct_tmp = 0;
    static char setid_tmp[64];
    static int imsi_info_available = 0;

    /*IMSI value given*/
    if(NULL != setid)
    {
       /*Check whether mobile info is provided or not*/
        if (0 == agps_reflocation_tmp.type)
        {
            DEBUG_LOG_PRINT_LEV2(("Setting IMSI value only \n"));

            /*Just set the IMSI value*/
            gpsclient_SetMobileInfo(agps_reflocation, 0 , setid);

            /*Copy IMSI value to temporary value*/
            memcpy(setid_tmp , setid, strlen(setid) + 1);

            /*IMSI flag enabled*/
            imsi_info_available = 1;
        }
        else
        {
            DEBUG_LOG_PRINT_LEV2(("Setting Mobile info and IMSI both\n"));

            /*Set mobile info and IMSI value*/
            gpsclient_SetMobileInfo(&agps_reflocation_tmp, sz_struct_tmp, setid);

            /*Initialize agps_reflocation_tmp for next iteration*/
            memset(&agps_reflocation_tmp, 0, sizeof(t_gpsClient_RefLocation));

            /*Initialize sz_struct_tmp for next iteration*/
            memset(&sz_struct_tmp , 0, sizeof(size_t));

            /*Reinitialize IMSI flag*/
            imsi_info_available = 0;
        }
    }
    else
    {
         DEBUG_LOG_PRINT_LEV2(("agps_set_mobile_info: agps_reflocation->type is  %d\n", agps_reflocation->type ));
        if(agps_reflocation->type == AGPS_REG_LOCATION_TYPE_MAC)
        {
            DEBUG_LOG_PRINT_LEV2(("Setting the WLAN MAC ID info\n"));
            ALOGD("Setting the WLAN MAC ID info\n");
            gpsclient_SetMobileInfo(agps_reflocation,sz_struct,NULL);
        }
        else
        {

            if(0 == imsi_info_available)
            {
                DEBUG_LOG_PRINT_LEV2(("Mobile Info available\n"));

                /*Copy the agps_reflocation to temporary location*/
                agps_reflocation_tmp = *agps_reflocation;

                /*Copy the sz_struct to temporary location*/
                sz_struct_tmp = sz_struct;
            }
            else
            {
                DEBUG_LOG_PRINT_LEV2(("Setting IMSI and Mobile Info both\n"));

                /*Set mobile info and IMSI value*/
                gpsclient_SetMobileInfo(agps_reflocation, sz_struct, setid_tmp);

                /*Reinitialize IMSI value*/
                memset(setid_tmp, 0 , 64);

                /*Reset IMSI flag*/
                imsi_info_available = 0;
            }
        }
    }
}


/*****************************************************************************
* agps_ril_set_ref_location : Sets the reference location.
*****************************************************************************/
void agps_ril_set_ref_location (const AGpsRefLocation *agps_reflocation, size_t sz_struct)
{
    t_gpsClient_RefLocation *gpsClient_reflocation = (t_gpsClient_RefLocation *)agps_reflocation;
    ALOGD(" agps_reflocation %p gpsClient_reflocation %p",agps_reflocation,gpsClient_reflocation);
    agps_set_mobile_info(gpsClient_reflocation, sz_struct, NULL);
}


/*****************************************************************************
* agps_ril_set_set_id :
*****************************************************************************/
void agps_ril_set_set_id (AGpsSetIDType type, const char* setid)
{
    t_gpsClient_RefLocation agps_reflocation_tmp= {0};

    /*Support for IMSI ONLY*/
    if(AGPS_SETID_TYPE_IMSI == type)
    {
        /*Setting invalid value for type*/
        agps_reflocation_tmp.type = (uint8_t)(-1);

        /*Call set mobile info*/
        agps_set_mobile_info(&agps_reflocation_tmp ,0 , setid);

        DEBUG_LOG_PRINT_LEV2(("Set Id from ril: %s\n",setid));
    }
    else
    {
        /*Ideally here it should not come here*/
        DEBUG_LOG_PRINT_LEV2(("Error: Implementation missing for set id type: %d  , Set Id: %s\n", type, setid));
    }
}


/*****************************************************************************
* agps_ril_ni_push : Provides the intial SUPL message in case of NI session.
*****************************************************************************/
void agps_ril_ni_push (uint8_t*sms_push, size_t sms_push_len)
{
    gpsclient_SendSuplPush(sms_push,sms_push_len,NULL,0);
}


/*****************************************************************************
* agps_ril_get_interface
*****************************************************************************/
void agps_ril_update_network_state(int connected, int type, int roaming, const char* extra_info)
{
}


const AGpsRilInterface agpsRilInterface = {
    sizeof(AGpsRilInterface),
    agps_ril_init,
    agps_ril_set_ref_location,
    agps_ril_set_set_id,
    agps_ril_ni_push,
    agps_ril_update_network_state
};


/*****************************************************************************
* agps_ril_get_interface
*****************************************************************************/
const AGpsRilInterface* agps_ril_get_interface()
{
    return &agpsRilInterface;
}
#endif /*AGPS_ANDR_GINGER_FTR */


/*****************************************************************************
* gps_get_extra_interface
*****************************************************************************/
const GpsXtraInterface* gps_get_extra_interface()
{
    return NULL;
}


/*****************************************************************************
* gps_get_extension
*****************************************************************************/
const void* gps_get_extension(const char* name)
{
    void *extension = NULL;

#ifdef AGPS_ANDR_CUPCAKE_FTR
    if( strcmp(name, GPS_SUPL_INTERFACE) == 0 )
        extension = (void *)gps_get_supl_interface();
#else
    if( strcmp(name, AGPS_INTERFACE) == 0 )
        extension = (void *)agps_get_interface();
#endif /* #ifdef AGPS_ANDR_CUPCAKE_FTR */

#ifdef AGPS_ANDR_ECLAIR_FTR
    else if( strcmp(name,GPS_NI_INTERFACE ) == 0 )
        extension = (void *)gps_ni_get_interface();
#endif /* AGPS_ANDR_ECLAIR_FTR */

#ifdef AGPS_ANDR_GINGER_FTR
    else if( strcmp(name,AGPS_RIL_INTERFACE)  == 0 )
    extension = (void *)agps_ril_get_interface();
#endif /*AGPS_ANDR_GINGER_FTR */

    else if( strcmp(name, GPS_XTRA_INTERFACE) == 0 )
        extension = (void *)gps_get_extra_interface();

   DEBUG_LOG_PRINT_LEV2(("gps_get_extension for <%s> - interface <%x>" , name , extension ));

    return extension;

}

const GpsInterface  gpsInterface = {
#ifdef AGPS_ANDR_GINGER_FTR
    sizeof(GpsInterface),
#endif /*AGPS_ANDR_GINGER_FTR*/
    gps_init,
    gps_start,
    gps_stop,
#ifdef     AGPS_ANDR_CUPCAKE_FTR
    gps_set_fix_frequency,
#endif
    gps_cleanup,
    gps_inject_time,
#if defined(AGPS_ANDR_DONUT_FTR) || defined( AGPS_ANDR_ECLAIR_FTR )
    gps_inject_location,
#endif
    gps_delete_aiding_data,
    gps_set_position_mode,
    gps_get_extension
};


#ifdef AGPS_ANDR_GINGER_FTR
/*Froyo onward have the extra device as parameter*/
/*****************************************************************************
* gps_get_hardware_interface
*****************************************************************************/
const GpsInterface* gps_get_hardware_interface(struct gps_device_t* test)
{
    DEBUG_LOG_PRINT_LEV2(("gps_get_hardware_interface for GingerBread Called")); // Roy

    return &gpsInterface;
}
#else
/*****************************************************************************
* gps_get_hardware_interface
*****************************************************************************/
const GpsInterface* gps_get_hardware_interface()
{
    DEBUG_LOG_PRINT_LEV2(("gps_get_hardware_interface for Froyo Called")); // Roy

    return &gpsInterface;
}
#endif



/* +LMSqc49445_3 RRR August 5th 2010 */
/*****************************************************************************
* GetUtcTimestamp
*****************************************************************************/
static int64_t GetUtcTimestamp(int year,    /* Absolute years in Gregorian calender ex 2007 */
                               int month,   /* 1..12  */
                               int day,     /* 1..31  Depending on the month */
                               int hours,   /* 0..23  */
                               int minutes, /* 0..59  */
                               int seconds, /* 0..59  */
                               int millis   /* 0..999 */
                               )
{
    int64_t utctimeSeconds = 0;

    static const DaysBeforeMonth[12] ={0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334}; // only for non leap year. For leap year, compensated later

    if( year > 1970 )
    {
        int localYear = 1970;

        do
        {
            /* For each year add the seconds! */
            if( LEAP(localYear) )
            {
                utctimeSeconds += DAYS_IN_LEAP_YEAR*SEC_PER_DAY;
            }
            else
            {
                utctimeSeconds += DAYS_IN_NORMAL_YEAR*SEC_PER_DAY;
            }
            localYear++;
        } while (localYear < year );
    }

    /* Now calculate the seconds in current year */

    /* The number of days till date = all days in previous month + days till today in current month */
    utctimeSeconds +=  (DaysBeforeMonth[month-1] + (day-1) )*SEC_PER_DAY;

    /* Seconds in the current day */
    utctimeSeconds +=  hours*SEC_PER_HOUR + minutes*SEC_PER_MINUTE + seconds;

    if( LEAP(year) && month > 2 )
    {
        /* Leap Year *//*If the day is after febraury , add another day worth of seconds */
        utctimeSeconds += SEC_PER_DAY;
    }

    return ((utctimeSeconds*1000)+millis);
}
/* -LMSqc49445_3 RRR August 5th 2010 */



/*****************************************************************************
* gps_ProcessNavdataCallback
*****************************************************************************/
void gps_ProcessNavdataCallback(t_gpsClient_NavData *NavData)
{
    GpsLocation  location;
    GpsSvStatus  svStatus;
    double hAccuracy;
    double vAccuracy;
    int index;
    uint32_t mask;

    if(NavData == NULL)
        return;

    hAccuracy = sqrt(NavData->v_NAccEst * NavData->v_NAccEst + NavData->v_EAccEst * NavData->v_EAccEst);
    vAccuracy = NavData->v_VAccEst;


    location.timestamp = GetUtcTimestamp(
                              NavData->v_Year,
                              NavData->v_Month,
                              NavData->v_Day,
                              NavData->v_Hours,
                              NavData->v_Minutes,
                              NavData->v_Seconds,
                              NavData->v_Milliseconds);
    DEBUG_LOG_PRINT_LEV2(("year:%d, month:%d, day:%d, hours:%d, minutes:%d, seconds:%d, millis:%d, Timestamp:%ld\n",
                              NavData->v_Year,
                              NavData->v_Month,
                              NavData->v_Day,
                              NavData->v_Hours,
                              NavData->v_Minutes,
                              NavData->v_Seconds,
                              NavData->v_Milliseconds,
                              location.timestamp));
    location.latitude = NavData->v_Latitude;
    location.longitude = NavData->v_Longitude;
    location.altitude = NavData->v_AltitudeEll;

    /* +LMSqc49349  */
    location.speed = (int) (NavData->v_SpeedOverGround);
    /* -LMSqc49349  */

    /* ++ LMSqc30345  */
    /*    location.bearing = NavData->v_HAccMajBrg; */
    location.bearing = NavData->v_CourseOverGround;
    /* -- LMSqc30345  */

    /* +LMSqc49444 */
    location.accuracy = ceilf(hAccuracy * 100)/ 100;
    /* -LMSqc49444 */

    if( NavData->v_Valid3DFix == 1 )
    {
        location.flags = GPS_LOCATION_HAS_LAT_LONG | GPS_LOCATION_HAS_ALTITUDE | GPS_LOCATION_HAS_SPEED |
                            GPS_LOCATION_HAS_ACCURACY | GPS_LOCATION_HAS_BEARING;
/* ++LMSqc29457 */
        DEBUG_LOG_PRINT_LEV2(("Calling gpsCallbacks->location_cb : (CallBackFromCgps)\n"));
        gpsinterface_location_callback( &location);
/* --LMSqc29457 */
    }
    else if( NavData->v_Valid2DFix == 1 )
    {
        location.flags = GPS_LOCATION_HAS_LAT_LONG | GPS_LOCATION_HAS_SPEED |
                            GPS_LOCATION_HAS_ACCURACY | GPS_LOCATION_HAS_BEARING;
/* ++LMSqc29457 */
        DEBUG_LOG_PRINT_LEV2(("Calling gpsCallbacks->location_cb : (CallBackFromCgps)\n"));
        gpsinterface_location_callback( &location);
/* --LMSqc29457 */
    }



    svStatus.num_svs = NavData->v_SatsInView;
    svStatus.ephemeris_mask = 0;
    svStatus.almanac_mask = 0;
    svStatus.used_in_fix_mask = 0;

    DEBUG_LOG_PRINT_LEV2(("Number of SVs is %d\n", NavData->v_SatsInView));
    //Setting Status of all SVs in view
    for(index = 0; index < svStatus.num_svs; ++index){
        svStatus.sv_list[index].prn = NavData->v_SatsInViewSVId[index];

    /*Begin Changes for Glonass Support in Android UI 30-Nov-2011 Mohan-194997*/
    /*
        GPS SV ID  / PRN         1-32
        Glonass Slot number      1-24

        GPS satellites are identified by their PRN, which range from 1 up to 32.
        The WAAS system has numbers 33-64 to identify its satellites.
        The numbers 65-88 are used for GLONASS satellites (64 + satellite slot number).

        Glonass ID      65-88

        CONSTELLATION VALUES
        LBS_GN_NO_CONSTELL   = 0,           ///< Invalid Constellation
        LBS_GN_GPS_CONSTELL  = 1,             ///< Navstar GPS
        LBS_GN_GLON_CONSTELL = 2,             ///< Glonass
    */

    if(NavData->v_SatsInViewConstell[index]  == LBS_GN_GLON_CONSTELL)
    {
        svStatus.sv_list[index].prn = svStatus.sv_list[index].prn + LBS_FIRST_GLONASS_SVID;
    }
    /*End Changes for Glonass Support in Android UI 30-Nov-2011 Mohan-194997*/

        svStatus.sv_list[index].snr       = (float)NavData->v_SatsInViewSNR[index];
        svStatus.sv_list[index].azimuth   = (float)NavData->v_SatsInViewAzim[index];
        svStatus.sv_list[index].elevation = (float)NavData->v_SatsInViewElev[index];

        if( NavData->v_Valid3DFix || NavData->v_Valid2DFix )
        {
            if( NavData->v_SatsInViewUsed[index] > 0 )
            {
                // The bitmask for that particular SVID has to be set.
                /* + LMSqc48660 : Incorrect mapping of SV Used in fix */
                /* mask = 1 << ( GPS_MAX_SVS - NavData->v_SatsInViewSVId[index] ); */
                /* The expected masking is as follows : Extracted from GpsStatus.java */
                /* satellite.mUsedInFix = ((usedInFixMask & prnShift) != 0); where prnshift = (1 << prn); */
                mask = 1 << ( NavData->v_SatsInViewSVId[index] - 1 );
                /* - LMSqc48660 : Incorrect mapping of SV Used in fix */
                svStatus.used_in_fix_mask |= mask;
            }
        }
    }

    DEBUG_LOG_PRINT_LEV2(("Calling gpsCallbacks->sv_status_cb : (CallBackFromCgps)\n"));
    gpsinterface_sv_status_callback( &svStatus );

}


/* + LMSqc26087 -Anil */
/*****************************************************************************
* gps_GetNextField
*****************************************************************************/
int gps_GetNextField(uint8_t *data, uint8_t *result, char sep )
{
    int i;
    for(i = 0; data[i] != sep && data[i] != '\0'; ++i)
    {
        result[i] = data[i];
    }

    result[i] = '\0';
    return i;
}


/*****************************************************************************
* gps_GetTimeFromNmea
*****************************************************************************/
uint64_t gps_GetTimeFromNmea(uint8_t *data)
{
    uint64_t time;
    int year = 0;
    int month = 0;
    int day = 0;
    int hours = 0;
    int minutes = 0;
    int seconds = 0;
    int millis = 0;
    char field[15];
    int offset = 0;
    int flag = 0;
    char tmp[3] = {0, 0, 0};
    int i;

    for(i = 0; data[i] != '\0'; ++i)
    {
        if(data[i] == '$')
        {
            if( strncmp((char *)(data +  i), NMEA_GPZDA, NMEA_GPZDA_LENGTH) == 0 )
            {
                offset += gps_GetNextField(data + i + NMEA_GPZDA_LENGTH + 1, (uint8_t *)field, ',');
                if(strlen(field) == 0)
                    return 0;
                strncpy(tmp, field, 2);
                hours = atoi(tmp);
                strncpy(tmp, field + 2, 2);
                minutes = atoi(tmp);
                strncpy(tmp, field + 4, 2);
                seconds = atoi(tmp);
                offset += gps_GetNextField(data + i + NMEA_GPZDA_LENGTH + 1 + offset + 1, (uint8_t *)field, ',') + 1;
                if(strlen(field) == 0)
                    return 0;
                day = atoi(field);

                offset += gps_GetNextField(data + i + NMEA_GPZDA_LENGTH + 1 + offset + 1, (uint8_t *)field, ',') + 1;
                if(strlen(field) == 0)
                    return 0;
                month = atoi(field);

                offset += gps_GetNextField(data + i + NMEA_GPZDA_LENGTH + 1 + offset + 1, (uint8_t *)field, ',') + 1;
                if(strlen(field) == 0)
                    return 0;
                year = atoi(field);
                flag = 1;
            }
        }
    }
    if(flag == 1)
        return GetUtcTimestamp(year, month, day, hours, minutes, seconds, millis);
    else
        return 0;
}


/*****************************************************************************
* gps_ProcessNmeaCallback
*****************************************************************************/
void gps_ProcessNmeaCallback( t_gpsClient_NmeaData *NmeaData )
{
    uint64_t utcTime = 0;
    int i, length = 0;
    uint8_t nmea[100];
    utcTime = gps_GetTimeFromNmea(NmeaData->pData);

#if defined( AGPS_ANDR_ECLAIR_FTR ) || defined( AGPS_ANDR_FROYO_FTR )
    for(i = -1; i < NmeaData->length-1; )
    {
        length = gps_GetNextField((uint8_t*)NmeaData->pData + i + 1, nmea, '\n');
        i += length + 1;
        DEBUG_LOG_PRINT_LEV2(("NMEA data \n%s", (char *)nmea));
        if(strlen((char *)nmea) > 0)
          gpsinterface_nmea_callback(utcTime, (char *)nmea, length );
    }
#endif
}


#ifdef AGPS_ANDR_ECLAIR_FTR
/*****************************************************************************
* gps_ProcessNetworkInitiatedCallback
*****************************************************************************/
void gps_ProcessNetworkInitiatedCallback(uint8_t ni_notification)
{
    GpsStatus status;
    DEBUG_LOG_PRINT_LEV2(("gps_ProcessNetworkInitiatedCallback %d",ni_notification));

    switch (ni_notification)
    {
        case GPSCLIENT_GPS_ENGINE_ON:
           DEBUG_LOG_PRINT_LEV2(("gps_ProcessNetworkInitiatedCallback GPSCLIENT_GPS_ENGINE_ON"));
           status.status = GPS_STATUS_ENGINE_ON;
           gpsinterface_status_callback( &status);
           status.status = GPS_STATUS_SESSION_BEGIN;
           gpsinterface_status_callback( &status);
           break;
        case GPSCLIENT_GPS_ENGINE_OFF:
           DEBUG_LOG_PRINT_LEV2(("gps_ProcessNetworkInitiatedCallback GPSCLIENT_GPS_ENGINE_OFF"));
           status.status = GPS_STATUS_SESSION_END;
           gpsinterface_status_callback( &status);
           status.status = GPS_STATUS_ENGINE_OFF;
           gpsinterface_status_callback( &status);
           break;
    }


}


/* according to GSM 03.38. Note that this table is incomplete to a great extent */

static uint8_t  gsmAlphabet[] =
{
/* 0 - F */      ' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',
/* 10 -1F */   ' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',
/* 20 -2F */   ' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',
/* 30 -3F */   '0' , '1' , '2','3','4','5','6','7','8','9',' ',' ',' ',' ',' ',' ',
/* 40 -4F */   ' ', 'A','B','C','D','E','F','G','H','I','J','K','L','M','N','O',
/* 50 -5F */   'P','Q','R','S','T','U','V','W','X','Y','Z',' ',' ',' ',' ',
/* 60 -6F */   ' ', 'a','b','c','d','e','f','g','h','i','j','k','l','m','n','o',
/* 70 -7F */   'p','q','r','s','t','u','v','w','x','y','z',' ',' ',' ',' ',
};

/*Referred http://www.developershome.com/sms/gsmAlphabet.asp for look-up table.*/

static uint8_t  gsmHexAlphabet[] =
{
/*   0 - F */   0x40,0xA3,0x24,0xA5,0xE8,0xE9,0xF9,0xEC,0xF2,0xC7,0x0A,0xD8,0xF8,0x0D,0xC5,0xE5,
/* 10 -1F */   0x00,0x5F,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xC6,0xE6,0xDF,0xC9,
/* 20 -2F */   0x20,0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,0x2A,0x2B,0x2C,0x2D,0x2E,0x2F,
/* 30 -3F */   0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x3A,0x3B,0x3C,0x3D,0x3E,0x3F,
/* 40 -4F */   0xA1,0x41,0x42,0x43,0x44,0x45,0x46,0x47,0x48,0x49,0x4A,0x4B,0x4C,0x4D,0x4E,0x4F,
/* 50 -5F */   0x50,0x51,0x52,0x53,0x54,0x55,0x56,0x57,0x58,0x59,0x5A,0xC4,0xD6,0xD1,0xDC,0xA7,
/* 60 -6F */   0xBF,0x61,0x62,0x63,0x64,0x65,0x66,0x67,0x68,0x69,0x6A,0x6B,0x6C,0x6D,0x6E,0x6F,
/* 70 -7F */   0x70,0x71,0x72,0x73,0x74,0x75,0x76,0x77,0x78,0x79,0x7A,0xE4,0xF6,0xF1,0xFC,0xE0,
};


/*****************************************************************************
* GsmAlphabetToString
*****************************************************************************/
void GsmAlphabetToString( uint8_t *gsm , uint8_t *string )
{
    int s_index , o_index=0, index=0;
    int length = strlen( (char *)gsm ) , offset;
    unsigned int   workSpace;

    for( s_index=0 , o_index = 0 ; s_index < length ; o_index++ ,s_index++)
    {
       offset = s_index % 7;

       if( offset == 0 )
       {
          string[o_index] = 0x7f & gsm[s_index];
       }
       else
       {
          workSpace = gsm[s_index];
          workSpace = (workSpace << 8) & 0xff00;
          workSpace += gsm[s_index-1];
          workSpace = workSpace >> ( 8 - offset  );
          string[o_index] = 0x7f & workSpace;

          if( 7 == (offset+1) )
          {
             /* We have another whole byte to decode */
             o_index++;
             string[o_index] = 0x7f & (workSpace >> 7 );
          }
       }
    }

    /*Ex: String send by network encoded in gsm-default format-"d208d319098bc7e49269d303" .
          After converting using above logic we get “52114C4E116162636425261B3D”. Now this string
          has GSM 7-bit default alphabet(Hexadecimal). To convert it to normal UTF8 HEX values
          look-up table is used*/

    for( s_index=0 ; s_index < o_index ; s_index++)
    {
       workSpace = string[s_index];

       /* Special Characters are handled by skipping the character if its 0x1B and considering next
                 character as a special character.*/

       if  (  workSpace == 0x1B )
       {
           workSpace = string[++s_index] ;

           switch( workSpace )
           {
               case 0x0A:   string[index] = 0x0C;
               break;
               case 0x14:   string[index] = 0x5E;
               break;
               case 0x28:   string[index] = 0x7B;
               break;
               case 0x29:   string[index] = 0x7D;
               break;
               case 0x2F:   string[index] = 0x5C;
               break;
               case 0x3C:   string[index] = 0x5B;
               break;
               case 0x3D:   string[index] = 0x7E;
               break;
               case 0x3E:   string[index] = 0x5D;
               break;
               case 0x40:   string[index] = 0x7C;
               break;
               default:     string[index] = 0x00;
               break;
           }
       }
       else
       {
           string[index]= gsmHexAlphabet[workSpace]; // Converts gsmHex Alphabets to Hexadecimal values.
       }
       index++;
    }

    string[index] = '\0';

}

/*****************************************************************************
* UCS2ToStringEng
*****************************************************************************/
void UCS2ToStringEng( uint8_t *ucs2 ,uint8_t ucs2length, uint8_t *string )
{
      int i = 0;
    int v_Length = ucs2length / 2;
    if(string!=NULL)
    {
        for( i=0 ; i < (v_Length - 1) ; i++ )
        {
            string[i] = ucs2[ 1 + 2*i ];
        }
        string[i] = '\0';
    }
    DEBUG_LOG_PRINT_LEV2(("ucs2 decoded  %s\n" , string));


}


/*****************************************************************************
* getEncodingType
*****************************************************************************/
static GpsNiEncodingType getEncodingType( uint8_t encodingType )
{
    DEBUG_LOG_PRINT_LEV2(("getEncodingType : type <%d>" , encodingType));
    switch( encodingType )
    {
        case GPSCLIENT_UCS2:
        case GPSCLIENT_UCS2_COMPRESSED:
            return GPS_ENC_SUPL_UCS2;
        case GPSCLIENT_GSM_DEFAULT:
            return GPS_ENC_SUPL_GSM_DEFAULT;
        case GPSCLIENT_UTF8:
        case GPSCLIENT_DCS_8_BIT_DATA:
        case GPSCLIENT_DCS_8_BIT_TEXT:
            return GPS_ENC_SUPL_UTF8;
        default:
            DEBUG_LOG_PRINT_LEV2(("getEncodingType : Unknown type <%d>" , encodingType));
            return GPS_ENC_UNKNOWN;
    }
}


/*****************************************************************************
* gps_LogByteStream
*****************************************************************************/
void gps_LogByteStream(char *header , uint8_t *stream , int len )
{
    char *byteStream;
    int i=0;

    byteStream = malloc( 2*len + 1);

    for( i=0 ; i < len ; i++ )
    {
        sprintf( byteStream + 2*i , "%02x" , stream[i] );
    }

    byteStream[2*i] = 0;

    ALOGD("%s : <%s>" ,header , byteStream );

    free( byteStream );
    byteStream = NULL;
}


/*****************************************************************************
* gps_GetEncodedData
*****************************************************************************/
int gps_GetEncodedData( uint8_t *encodedData , uint8_t encodedType , uint8_t encodedLength , uint8_t *decodedData )
{
    GpsNiEncodingType   type;

    type = getEncodingType( encodedType );

    if( type == GPS_ENC_SUPL_GSM_DEFAULT)
    {
        GsmAlphabetToString(encodedData, decodedData);
        type= GPS_ENC_SUPL_UTF8;
    }
    else if(type == GPS_ENC_SUPL_UCS2 )
    {
        UCS2ToStringEng( encodedData , encodedLength, decodedData );
        type = GPS_ENC_SUPL_UTF8;
    }
    else
    {
        memcpy(decodedData , encodedData , encodedLength );
    }

    return type;

}


#ifdef AGPS_ANDROID_HEX_NOTIFICATION_FTR
/*****************************************************************************
* gps_StringToHex
*****************************************************************************/
void gps_StringToHex( uint8_t* string, uint8_t* hexData )
{
    uint16_t index     = 0;
    uint16_t count     = 0;
    uint16_t inpStrLen = 0;

    inpStrLen = strlen(string);

    for( index=0 ; index < inpStrLen ; index++ )
    {
        count += sprintf( hexData + count , "%02X" , *(string+index) );
    }

    hexData[ 2*inpStrLen ] = '\0';
}
#endif /*AGPS_ANDROID_HEX_NOTIFICATION_FTR*/


/*****************************************************************************
* gps_ProcessNotificationCallback
*****************************************************************************/
void gps_ProcessNotificationCallback( t_gpsClient_NotifyData *p_Notif )
{
    GpsNiNotification *gpsNotif;
#ifdef AGPS_ANDROID_HEX_NOTIFICATION_FTR
    uint8_t *reqId;
    uint8_t *text;

    reqId = malloc( MAX_STRING_LEN );
    memset( reqId , 0 , MAX_STRING_LEN );

    text = malloc( MAX_STRING_LEN );
    memset( text , 0 , MAX_STRING_LEN );
#endif /*AGPS_ANDROID_HEX_NOTIFICATION_FTR*/

    gpsNotif = malloc( sizeof(*gpsNotif) );

    memset( gpsNotif , 0 , sizeof(*gpsNotif ) );


#ifdef AGPS_ANDR_GINGER_FTR
    gpsNotif->size = sizeof ( GpsNiNotification );
#endif //AGPS_ANDR_GINGER_FTR


    gpsNotif->notification_id = p_Notif->v_Handle;
#ifndef AGPS_ANDROID_HEX_NOTIFICATION_FTR

    /*The string starts from the 2nd index. Prior indexes stores the length of the string.  */
    gpsNotif->requestor_id_encoding =
                    gps_GetEncodedData(p_Notif->a_RequestorId + 1,
                                                p_Notif->v_RequestorIdEncodingType,
                                                GPSCLIENT_MAX_LENGTH_REQUESTER_ID - 1,
                                                (uint8_t *)&gpsNotif->requestor_id);

    gpsNotif->text_encoding =
                    gps_GetEncodedData(p_Notif->a_ClientName + 1,
                                                p_Notif->v_ClientNameEncodingType,
                                                GPSCLIENT_MAX_LENGTH_CLIENT_NAME - 1,
                                                (uint8_t *)&gpsNotif->text);

    DEBUG_LOG_PRINT(("Notification : Requestor ID %s", gpsNotif->requestor_id));
    DEBUG_LOG_PRINT(("Notification : Client Name %s", gpsNotif->text));
#else
     /*The string starts from the 2nd index. Prior indexes stores the length of the string.  */
    gpsNotif->requestor_id_encoding =
                    gps_GetEncodedData(p_Notif->a_RequestorId + 1,
                                                p_Notif->v_RequestorIdEncodingType,
                                                GPSCLIENT_MAX_LENGTH_REQUESTER_ID - 1,
                                                reqId);

    gpsNotif->text_encoding =
                    gps_GetEncodedData(p_Notif->a_ClientName + 1,
                                                p_Notif->v_ClientNameEncodingType,
                                                GPSCLIENT_MAX_LENGTH_CLIENT_NAME - 1,
                                                text);

    DEBUG_LOG_PRINT(("Notification : Requestor ID %s", reqId));
    DEBUG_LOG_PRINT(("Notification : Client Name %s", text));

    gps_StringToHex(reqId, (uint8_t *)&gpsNotif->requestor_id);
    gps_StringToHex(text, (uint8_t *)&gpsNotif->text);

    DEBUG_LOG_PRINT(("Notification : Requestor ID in HEX%s", gpsNotif->requestor_id));
    DEBUG_LOG_PRINT(("Notification : Client Name in HEX %s", gpsNotif->text));

    free(reqId);
    reqId = NULL;
    free(text);
    text = NULL;

#endif /*AGPS_ANDROID_HEX_NOTIFICATION_FTR*/


    gpsNotif->timeout = 20;

    gpsNotif->ni_type = GPS_NI_TYPE_UMTS_SUPL; // @todo : This value does not make any sense but we have to stick with what Android provides

    switch( p_Notif->v_Type )
    {
        case GPSCLIENT_NOTIFICATION_AND_VERIFICATION_ALLOWED_NA:
        DEBUG_LOG_PRINT_LEV2(("Notification : K_AGPS_NOTIFICATION_AND_VERIFICATION_ALLOWED_NA"));
            gpsNotif->notify_flags = GPS_NI_NEED_NOTIFY | GPS_NI_NEED_VERIFY;
            gpsNotif->default_response = GPS_NI_RESPONSE_ACCEPT;
            break;
        case GPSCLIENT_NOTIFICATION_AND_VERIFICATION_DENIED_NA:
        DEBUG_LOG_PRINT_LEV2(("Notification : K_AGPS_NOTIFICATION_AND_VERIFICATION_DENIED_NA"));
            gpsNotif->notify_flags = GPS_NI_NEED_NOTIFY | GPS_NI_NEED_VERIFY;
            gpsNotif->default_response = GPS_NI_RESPONSE_DENY;
            break;
        case GPSCLIENT_NOTIFICATION_ONLY:
        DEBUG_LOG_PRINT_LEV2(("Notification Only"));
            gpsNotif->notify_flags = GPS_NI_NEED_NOTIFY;
            gpsNotif->default_response = GPS_NI_RESPONSE_NORESP;
            break;
        default:
            free( gpsNotif );
            gpsNotif = NULL;
            return; /* All other cases, do nothing */
    }

    if( NULL != gpsNiCallbacks )
    {
        DEBUG_LOG_PRINT_LEV2(("Notification Callback Called"));

        //gpsNiCallbacks->notify_cb(gpsNotif);
        gpsinterface_notification_callback( gpsNotif );
    }

#ifdef AGPS_AUTO_ACCECPT_NOTIFICATION
    if( (p_Notif->v_Type ==  K_AGPS_NOTIFICATION_AND_VERIFICATION_ALLOWED_NA) ||
          (p_Notif->v_Type ==  K_AGPS_NOTIFICATION_AND_VERIFICATION_DENIED_NA)
          )
        gpsclient_NotificationResponse(p_Notif->v_Handle,GPSCLIENT_NI_RESPONSE_ACCEPT);
#endif

    free( gpsNotif );
    gpsNotif = NULL;

}
#endif /* #ifdef AGPS_ANDR_ECLAIR_FTR */


/*****************************************************************************
* gps_ProcessAgpsStatusCallback
*****************************************************************************/
void gps_ProcessAgpsStatusCallback( e_gpsClient_AgpsStatData *p_AgpsStat )
{
    AGpsStatus status;
    DEBUG_LOG_PRINT_LEV2(("Received data is Bearer request\n"));

    status.size = sizeof (AGpsStatus);
    status.type = AGPS_TYPE_SUPL;
    switch(*p_AgpsStat)
    {
       case GPSCLIENT_REQUEST_AGPS_DATA_CONN:  status.status = GPS_REQUEST_AGPS_DATA_CONN; break;
       case GPSCLIENT_RELEASE_AGPS_DATA_CONN:  status.status = GPS_RELEASE_AGPS_DATA_CONN; break;
       default: return;
    }
    agpsinterface_status_callback( &status );
    DEBUG_LOG_PRINT_LEV2(("Bearer request processed\n"));
}


#ifdef AGPS_ANDR_GINGER_FTR
/*****************************************************************************
* gps_ProcessRilSetIdCallback
*****************************************************************************/
void  gps_ProcessRilSetIdCallback()
{
    /*Obtain Mutex*/
    pthread_mutex_lock(&sEventMutex);

    /*Enum set for requesting Setid*/
    sPendingCallbacks |= krilsetid;

    /*Signal generated for event*/
    pthread_cond_signal(&sEventCond);

    /*Mutex released*/
    pthread_mutex_unlock(&sEventMutex);
}


/*****************************************************************************
* gps_ProcessRilCallback
*****************************************************************************/
void gps_ProcessRilCallback( e_gpsClient_NetworkInfoType infoType )
{
    pthread_mutex_lock( &sEventMutex );

    if( infoType == GPSCLIENT_NETOWRK_INFO_TYPE_CELLULAR )
        sPendingCallbacks |= krilCellId;
    else if ( infoType == GPSCLIENT_NETOWRK_INFO_TYPE_WLAN )
        sPendingCallbacks |= krilMac;

    pthread_cond_signal( &sEventCond );

    pthread_mutex_unlock( &sEventMutex );
}
#endif
