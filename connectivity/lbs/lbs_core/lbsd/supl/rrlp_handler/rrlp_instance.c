//*************************************************************************
/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */ 
//
// Filename abst_instance.c
//
// $Header: X:/MKS Projects/prototype/prototype/rrlp_handler/rcs/rrlp_instance.c 1.27 2009/01/13 14:09:42Z grahama Rel $
// $Locker: $
//*************************************************************************
///
/// \ingroup os_abstraction
///
/// \file
/// \brief
///      Instance creation, lookup and deletion function implementation.
///
///      The SUPL implementation requires an instance of various items to allow
///      the simultaneous handling of various concurrent states.
///
//*************************************************************************

#include <limits.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include <math.h>
#include "rrlp_instance.h"
#include "RRLP_SatStatus.h"
#include "RRLP_NavigationModel.h"
#include "GN_SUPL_api.h"

#ifdef AGPS_LINUX_FTR
#define _vsnprintf   vsnprintf  // _vsnprintf is private static in linux c runtime
#endif

//*****************************************************************************
/// \brief
///      Check each instance of the RRLP Handler for any required processing.
///
/// <b>Global Parameters:</b>
///      <p>#RRLP_Instances [global] Container for current RRLP instances.
//*****************************************************************************
void RRLP_Instance_Handler( void )
{
   // Each existing instance is checked for actions to be taken such as polling for external events.
   U1 i;
   BL ValidInstanceFound = FALSE;

   for ( i = 0 ; i < RRLP_Instances.MaxInstanceCount ; i++ )
   {
      if ( RRLP_Instances.InstanceList[i] != NULL )
      {
         // valid instance found in list
            RRLP_Instance_Process( RRLP_Instances.InstanceList[i] );
         ValidInstanceFound = TRUE;
      }
   }
   // Special case for RRLP agent. Check for Assistance data request even when no instance.
   if ( ! ValidInstanceFound ) RRLP_Instance_Process( NULL );
}

//*************************************************************************
/// \brief
///      Initialise a container for instances.
///
/// <b>Global Parameters:</b>
///      <p>#RRLP_Instances [global] Container for current RRLP instances.
//*************************************************************************
void RRLP_Instances_Initialise(
   U1 MaxInstanceCount              ///< Maximum number of instances supported.
)
{
    s_RRLP_Instances *p_Instances = &RRLP_Instances;  ///< Instance container to be
                                                      /// initialised.
    memset( p_Instances, 0, sizeof( s_RRLP_Instances ) );
    p_Instances->MaxInstanceCount = MaxInstanceCount;
}

//*************************************************************************
/// \brief
///      De-Initialises Instance container.
///
/// <b>Global Parameters:</b>
///      <p>#RRLP_Instances [global] Container for current RRLP instances.
/// \returns
///      Nothing.
//*************************************************************************
void RRLP_Instances_DeInitialise( void )
{
   U1 i;

   /// Cycle through the available instances.
   for ( i = 0 ; i < INSTANCE_RRLP_MAX ; i++ )
   {
      /// If you find a valid instance.
      if ( RRLP_Instances.InstanceList[i] != 0 )
      {
         /// Delete it and free the contents.
         RRLP_Instance_Delete( RRLP_Instances.InstanceList[i] );
      }
   }
}

//*************************************************************************
/// \brief
///      Retrieves an Instance Pointer based on an Instance Pointer.
///
/// <b>Global Parameters:</b>
///      <p>#RRLP_Instances [global] Container for current RRLP instances.
/// \returns
///      Instance Pointer if the Instance Pointer passed in was valid,
///      otherwise #NULL.
/// \retval
///      Valid Instance Pointer.
/// \retval
///      #NULL if Instance Pointer is not valid.
//*************************************************************************
s_RRLP_Instance *RRLP_Instance_Get_Data(
   s_RRLP_Instance *p_RRLP_Instance       ///< Instance Handle.
)
{
   s_RRLP_Instances *p_Instances = &RRLP_Instances;      ///< Container of instances.
   U1 i;

   /// If we have a non zero Instance Pointer to search for.
   if ( p_RRLP_Instance != NULL )
   {
      /// Search through the available instances.
      for ( i = 0 ; i < p_Instances->MaxInstanceCount ; i++ )
      {
         /// Return the Instance Pointer if a matching one is found.
         if ( p_Instances->InstanceList[i] == p_RRLP_Instance )
         {
             return p_Instances->InstanceList[i];
         }
      }
   }
   /// Otherwise return #NULL.
   return NULL;
}

//*************************************************************************
/// \brief
///      Creates a new instance.
///
/// <b>Global Parameters:</b>
///      <p>#RRLP_Instances [global] Container for current RRLP instances.
/// \returns
///      Instance Pointer.
/// \retval
///      Valid Instance Pointer if one allocated.
/// \retval
///      #NULL if none allocated.
//*************************************************************************
s_RRLP_Instance *RRLP_Instance_Request_New( void )
{
   s_RRLP_Instance *p_RRLP_Instance = RRLP_Instance_Base_Request_New();

   if ( p_RRLP_Instance != NULL )
   {
            p_RRLP_Instance->p_RRLP = GN_Calloc( 1, sizeof( s_RRLP_InstanceData ) );
              memset(p_RRLP_Instance->p_RRLP , 0 ,sizeof( s_RRLP_InstanceData ));
            p_RRLP_Instance->p_RRLP->State = state_RRLP_Idle;
            p_RRLP_Instance->Handle = &p_RRLP_Instance->ThisInstanceId;
            p_RRLP_Instance->p_RRLP->ReferenceNumber = -1; // Reference number is not yet set.
      return p_RRLP_Instance;
   }
   else
   {
      return NULL;
   }
}

//*************************************************************************
/// \brief
///      Request a new RRLP instance from an instance container.
///
/// <b>Global Parameters:</b>
///      <p>#RRLP_Instances [global] Container for current RRLP instances.
/// \returns
///      Pointer to an instance.
//*************************************************************************
s_RRLP_Instance *RRLP_Instance_Base_Request_New( void )
{
   s_RRLP_Instances  *p_Instances = &RRLP_Instances;
   s_RRLP_Instance   *p_Instance_Unused;

   if ( p_Instances == NULL )
   {
      return NULL;
   }
   else
   {
      t_RRLP_InstanceId    NewInstanceId;
      BL InstanceId_Unique, InstanceId_Found;
      U2 i;

      for ( i = 0 ; i < p_Instances->MaxInstanceCount ; i++ )
      {
         if ( p_Instances->InstanceList[i] == NULL )
         {
            break;
         }
      }
      if ( i != p_Instances->MaxInstanceCount ) // We found a gap.
      {
         // We found an unused instance entry. Allocate storage.
         p_Instances->InstanceList[i] = GN_Calloc( 1, sizeof( s_RRLP_Instance ) );

         p_Instance_Unused = p_Instances->InstanceList[i];

         InstanceId_Unique = FALSE;
         while ( ! InstanceId_Unique )
         {
            // Calculate a new instance id.
            if ( p_Instances->InstanceIdHighWaterMark < INSTANCE_RRLP_LIMIT - INSTANCE_RRLP_MAX )
            {
               NewInstanceId = p_Instances->InstanceIdHighWaterMark + 1;
               p_Instances->InstanceIdHighWaterMark = NewInstanceId;
            }
            else
            {
               NewInstanceId = 1;
               p_Instances->InstanceIdHighWaterMark = 1;
            }

            // Make sure new instance id is not in use.
            InstanceId_Found = FALSE;
            for ( i = 0 ; i < p_Instances->MaxInstanceCount ; i++ )
            {
               if( p_Instances->InstanceList[i] != NULL &&
                   p_Instances->InstanceList[i]->ThisInstanceId == NewInstanceId )
               {
                  InstanceId_Found = TRUE;
                  break;
               }
            }
            if ( ! InstanceId_Found ) InstanceId_Unique = TRUE;
         }
         p_Instances->InstanceIdHighWaterMark = NewInstanceId;

         // Store the new instance id.
         p_Instance_Unused->ThisInstanceId = NewInstanceId;
         return p_Instance_Unused;
      }
      else
      {
         return NULL;
      }
   }
}


//*************************************************************************
/// \brief
///      Retrieve an RRLP Instance from an instance container using the
///      associated Handle.
///
/// <b>Global Parameters:</b>
///      <p>#RRLP_Instances [global] Container for current RRLP instances.
/// \returns
///      Instance Pointer if the Handle passed in was valid,
///      otherwise #NULL.
/// \retval
///      Valid Instance Pointer.
/// \retval
///      #NULL if Handle is not valid.
//*************************************************************************
s_RRLP_Instance *RRLP_Instance_From_Handle( void *Handle )
{
   U1 i;
   s_RRLP_Instances *p_RRLP_Instances = &RRLP_Instances;

   for ( i = 0 ; i < INSTANCE_RRLP_MAX ; i++ )
   {
      if ( p_RRLP_Instances->InstanceList[i] != NULL )
      {
         if ( p_RRLP_Instances->InstanceList[i]->Handle == Handle )
         {
            return p_RRLP_Instances->InstanceList[i];
         }
      }
   }
   return NULL;
}

//*****************************************************************************
/// \brief
///      Deletes an existing instance from the #RRLP_Instance list.
///
/// <b>Global Parameters:</b>
///      <p>#RRLP_Instances [global] Container for current RRLP instances.
//*****************************************************************************
void RRLP_Instance_Delete(
   s_RRLP_Instance *p_RRLP_Instance ///< Instance to delete.
)
{
   s_RRLP_Instance *p_TempInstance = RRLP_Instance_Get_Data( p_RRLP_Instance );

   if ( p_TempInstance != NULL )
   {
      if ( p_RRLP_Instance->p_RRLP != NULL )
      {
         if ( p_RRLP_Instance->p_RRLP->Payload.p_PDU_Data != NULL )
         {
            GN_Free( p_RRLP_Instance->p_RRLP->Payload.p_PDU_Data );
         }
         if ( p_RRLP_Instance->p_RRLP->p_GN_GPS_Nav_Data != NULL )
         {
            GN_Free( p_RRLP_Instance->p_RRLP->p_GN_GPS_Nav_Data );
         }
         if ( p_RRLP_Instance->p_RRLP->p_GN_AGPS_GAD_Data != NULL )
         {
            GN_Free( p_RRLP_Instance->p_RRLP->p_GN_AGPS_GAD_Data );
         }
         if ( p_RRLP_Instance->p_RRLP->p_GN_AGPS_Meas != NULL )
         {
            GN_Free( p_RRLP_Instance->p_RRLP->p_GN_AGPS_Meas );
         }
         if ( p_RRLP_Instance->p_RRLP->p_GN_RRLP_QoP != NULL )
         {
            GN_Free( p_RRLP_Instance->p_RRLP->p_GN_RRLP_QoP );
         }

         GN_Free( p_RRLP_Instance->p_RRLP );
         p_RRLP_Instance->p_RRLP = NULL;
      }

      GN_RRLP_Log( "RRLP Instance ID %d : Deleted",
               p_RRLP_Instance->ThisInstanceId );
      RRLP_Instance_Base_Delete( p_RRLP_Instance );
   }
}

//*************************************************************************
/// \brief
///      Removes the Instance entry from the Instance container.
/// \todo
///      Free up data in this instance.
//*************************************************************************
BL RRLP_Instance_Base_Delete(
   s_RRLP_Instance *p_RRLP_Instance    ///< Pointer to Instance entry to remove.
)
{
   s_RRLP_Instances *p_Instances = &RRLP_Instances;
   unsigned i;

    for ( i = 0 ; i < p_Instances->MaxInstanceCount ; i++ )
    {
        if ( p_Instances->InstanceList[i] == p_RRLP_Instance )
        {
            GN_Free( p_RRLP_Instance );
            p_Instances->InstanceList[i] = NULL;
            // No point checking the rest of the entries. Bail out now.
            return TRUE;
        }
    }
    return FALSE;
}


//*****************************************************************************
/// \brief
///      Handles the State Transition for the RRLP Handler.
/// \warning
///      The entries in RRLP_States_Text have to liner up with the enumerated
///      values in #e_RRLP_States for the logging to work correctly.
//*****************************************************************************
void RRLP_Instance_State_Transition(
   s_RRLP_Instance   *p_RRLP_Instance, ///< Instance to transition to the new state.
   e_RRLP_States     New_State,        ///< New State.
   CH                *Cause            ///< What caused the transition (usually an event).
)
{
   CH * RRLP_States_Text[] = {
      "state_RRLP_Idle",
      "state_RRLP_Transaction_In_Progress",
      "state_RRLP_Assistance_Data_Delivery",
      "state_RRLP_Measure_Position_Request",
      "state_RRLP_Position_Requested_From_GPS"
   };

   s_RRLP_Instance *p_tempInstance = RRLP_Instance_Get_Data( p_RRLP_Instance );

   if ( p_tempInstance != NULL )
   {

         GN_RRLP_Log( "State:RRLP:    %s->%s [label=\"RRLP Event:%s\"]",
                RRLP_States_Text[p_tempInstance->p_RRLP->State],
                RRLP_States_Text[New_State],
                Cause );

      p_tempInstance->p_RRLP->State = New_State;

      if ( p_tempInstance->p_RRLP->State == state_RRLP_Idle )
      {
          RRLP_Instance_Delete( p_tempInstance );
      }
   }

}


//*****************************************************************************
/// \brief
///      Logs RRLP Event messages.
///
///      Sends "<stdio>" formatted debug Log messages to the
///      GN_GPS_Event_Log channel and appends a "<CR><LF>".
/// \note
///      The total message length must be less than 250 characters long.
//*****************************************************************************
void GN_RRLP_Log( const char *format, ... )
{
   CH       buff[256];
   CH       *formattedLine = buff;
   U2       prefixSize = 0;
   CH       postFix[] = "\x0d\x0a";
   va_list  arg_list;
   I2       i;
   I2       buffSize = sizeof( buff );
   I2       ret;

   // Add a header of "~ OS_Time_ms " to distinguish these Host Wrapper Event
   // Logs from the GN_GPS_Lib internal Event Logs.
   i = sprintf( buff, "@ %6u ", GN_GPS_Get_OS_Time_ms() );

   prefixSize = i;

   buffSize -= prefixSize ; // Take away the size of the timestamp from the available buffer size.
   buffSize -= sizeof( postFix ) ; // Allow 3 extra to add trailing CR/LF at the end.

   // Format the user string.
   va_start( arg_list, format );
   ret = _vsnprintf( ( formattedLine + i ), buffSize, format, arg_list );
   va_end( arg_list );

   if ( ret >= 0 &&        // if no error and...
        ret <  buffSize )  // if the size of the generated string is less than the available size
   {
      // identify the end of the current string.
      i = i + ret ;
   }
   else if ( ( ret + (I2) sizeof( postFix ) ) > buffSize )
   {
      // The amount required was greater than the available buffer, so we make a buffer big enough.
      buffSize = prefixSize + ret + sizeof( postFix );

      // if the size of the generated string would be greater than the available size
      formattedLine = GN_Calloc( 1,  buffSize );

      if ( formattedLine != NULL )
      {
         // copy over prefix
         strncpy( formattedLine, buff, prefixSize );

         buffSize -= prefixSize ; // Take away the size of the timestamp from the available buffer size.
         buffSize -= sizeof( postFix ) ; // Allow 3 extra to add trailing CR/LF at the end.

         // Format the user string.
         va_start( arg_list, format );
         ret = _vsnprintf( ( formattedLine + i ), buffSize, format, arg_list );
         va_end( arg_list );
         if ( ret >= 0 &&     // if no error and...
              ret < buffSize ) // if the size of the generated string is less than the available size
         {
            // identify the end of the current string.
            i = i + ret ;
         }
      }
      else // GN_Calloc failed
      {
         formattedLine = buff;
         ret = -1;
      }
   }
   if ( ret < 0 ||      // if _vsnprintf or GN_Calloc could not be made to work properly dump an error.
        ret > buffSize )
   {
      // If an error occured
      strcpy( formattedLine + prefixSize, "GN_RRLP_Log:--- Error formatting log output. ---" );
      i = (I2) strlen( formattedLine );
   }

   // Append a <CR><LF> & NULL Terminate
   formattedLine[i]   = postFix[0];
   formattedLine[i+1] = postFix[1];
   formattedLine[i+2] = postFix[2];

   // Send the String to the GloNav Event Log channel.
   GN_SUPL_Write_Event_Log( (U2) ( i + 2 ), (CH*) formattedLine );

   if ( formattedLine != buff )
   {
      // we must have allocated the buffer so free it.
      GN_Free( formattedLine );
   }

   return;
}


#ifdef     SUPL_CMCC_LOGGING_ENABLE

static BL IsLoggingAllowed( void* handle )
{
    s_RRLP_Instance *p_Instance;

    p_Instance = RRLP_Instance_From_Handle( handle );

    if( NULL !=  p_Instance )
    {
        return ! p_Instance->p_RRLP->LoggingDisabled;
    }
    return TRUE;
}

extern void* GN_SUPL_GetHandleFromRRLPHandle( void* handle );
extern void GN_SUPL_Log_CMCC_PosReport( void* handle ,s_GN_AGPS_GAD_Data *p_GN_AGPS_GAD_Data , U4 msTTFF );
// extern void GN_SUPL_Log_CMCC_PosReport( void* handle ,s_GN_AGPS_GAD_Data *p_GN_AGPS_GAD_Data );
#endif


#define GN_SUPL_LOG_CMCC_AGPS_AIDING_RECVD       0x00000004


void GN_RRLP_Log_AssistRecvd( void* handle , RRLP_GPS_AssistData_t*p_GPS_AssistData )
{
   handle           = handle;
   p_GPS_AssistData = p_GPS_AssistData;

#ifdef     SUPL_CMCC_LOGGING_ENABLE
   if( IsLoggingAllowed( handle ) )
   {
       U2  count =0;
       CH  buff[128] = {0};

       /* SYNTAX: local_session_num, num_sat, gps_week, toe, sat_num1, sat_num2, …., sat_numN */

      /*Add local session number to the buffer*/
      count += sprintf( buff ,"%d" , (int)GN_SUPL_GetHandleFromRRLPHandle(handle) );

      /*Check whether Navigation modem is present or not*/
      if ((  NULL != p_GPS_AssistData )  &&  ( NULL  !=  p_GPS_AssistData->controlHeader.navigationModel )  )
      {
         U1 i  = 0;
         U1 NavigationModelCount  = 0 ;
         RRLP_NavigationModel_t *p_NavigationModel = NULL;
         RRLP_NavModelElement_t  *p_NavModelElement = NULL;

        /*Navigation model structure assigned*/
         p_NavigationModel = p_GPS_AssistData->controlHeader.navigationModel;

         /*Navigation model element assigned*/
         p_NavModelElement = (RRLP_NavModelElement_t*)p_NavigationModel->navModelList.list.array;

         /*Number of satellites*/
         NavigationModelCount = (U1)p_NavigationModel->navModelList.list.count;

         /*Add number of satellites to buffer*/
         count += sprintf( buff+count ,",%u" , NavigationModelCount );

         /*Check whether reference time is present or not*/
         if( NULL  !=  p_GPS_AssistData->controlHeader.referenceTime )
         {
            /*Copy gps week and gps time to buffer*/

            /*GPSTOW, range 0-604799.92, resolution 0.08 sec, 23-bit presentation*/
            /*GPSTOW23b ::= INTEGER (0..7559999)*/
            count += sprintf( buff+count ,",%u,%u",
                                        p_GPS_AssistData->controlHeader.referenceTime->gpsTime.gpsWeek,
                                        p_GPS_AssistData->controlHeader.referenceTime->gpsTime.gpsTOW23b*0.08 );
         }

          /*Run for total Navigation model count*/
         for ( i = 0 ; i < NavigationModelCount && (p_NavModelElement != NULL) ; i++ )
         {
            /*Take the satellite element*/
            p_NavModelElement = p_GPS_AssistData->controlHeader.navigationModel->navModelList.list.array[i];

            /*Add satellite ID, if present*/
            switch (p_NavModelElement->satStatus.present)
            {
              case RRLP_SatStatus_PR_newNaviModelUC:

               /*Fall through done delibrately*/

              case RRLP_SatStatus_PR_newSatelliteAndModelUC:
                    count += sprintf( buff+count ,",%u",(U1)p_NavModelElement->satelliteID + 1 );
                    break;

               /*OLD satellite and Model is not supported here*/
              case RRLP_SatStatus_PR_oldSatelliteAndModel:
                    break;
            }
         }
      }
      else
      {
         /* local_session_num, num_sat, gps_week, toe, sat_num1, sat_num2, …., sat_numN */

         /*Check whether reference time is present or not*/
         if ( NULL  !=  p_GPS_AssistData->controlHeader.referenceTime )
         {
            /*GPSTOW, range 0-604799.92, resolution 0.08 sec, 23-bit presentation*/
            /*GPSTOW23b ::= INTEGER (0..7559999)*/
            count += sprintf( buff+count ,",0,%u,%u",
                                        p_GPS_AssistData->controlHeader.referenceTime->gpsTime.gpsWeek,
                                        p_GPS_AssistData->controlHeader.referenceTime->gpsTime.gpsTOW23b*0.08 );
         }
      }

      /*Write the ephemeris content to CMCC logs*/
      GN_SUPL_Write_Event_Log_CMCC( GN_SUPL_LOG_CMCC_AGPS_AIDING_RECVD , buff , "Eph Aiding to GPS" );
   }
#endif
}
void GN_RRLP_Log_PosRspTimeout( void* handle )
{

    handle = handle;

#ifdef SUPL_CMCC_LOGGING_ENABLE
#define GN_SUPL_LOG_CMCC_POS_FAILURE    0x13000000
    if( IsLoggingAllowed( handle ) )
    {
        CH local_session[20];
        sprintf( local_session , "%d" , (CH)GN_SUPL_GetHandleFromRRLPHandle(handle) );
        GN_SUPL_Write_Event_Log_CMCC( GN_SUPL_LOG_CMCC_POS_FAILURE , local_session , "GPS cannot produce position within given response time" );
    }
#endif
}

void GN_RRLP_Log_PosReport( void* handle , s_GN_AGPS_GAD_Data *p_GN_AGPS_GAD_Data , U4 msTTFF )
{
    handle             = handle;
    p_GN_AGPS_GAD_Data = p_GN_AGPS_GAD_Data;
    msTTFF             = msTTFF;

#ifdef SUPL_CMCC_LOGGING_ENABLE
    if( IsLoggingAllowed(handle) )
    {
        GN_SUPL_Log_CMCC_PosReport( GN_SUPL_GetHandleFromRRLPHandle(handle) , p_GN_AGPS_GAD_Data , msTTFF );
    }
#endif
}

void GN_RRLP_Log_MsgDecodeFailed( void* handle )
{
    handle   = handle;

#ifdef SUPL_CMCC_LOGGING_ENABLE
#define GN_SUPL_LOG_CMCC_ERROR_SUPL_DECODE       0x22000002
    if( IsLoggingAllowed( handle ) )
    {
        CH local_session[20];
        sprintf( local_session , "%d" , (CH)GN_SUPL_GetHandleFromRRLPHandle(handle) );
        GN_SUPL_Write_Event_Log_CMCC( GN_SUPL_LOG_CMCC_ERROR_SUPL_DECODE , local_session , "RRLP Decode Failure" );
    }
#endif
}
void GN_RRLP_Log_QoP(void* handle , s_GN_RRLP_QoP* p_GN_QoP )
{
    p_GN_QoP = p_GN_QoP;
    handle   = handle;

#ifdef SUPL_CMCC_LOGGING_ENABLE
#define GN_SUPL_LOG_CMCC_AGPS_SESSION_QOP        0x00000002
    if( IsLoggingAllowed(handle) )
    {
      /* First check if QoP is present and set to the appropriate values */
      if( p_GN_QoP != NULL )
      {
         CH  textLog[128];
         I4  horacc = -1 , veracc = -1 , delay = -1;

         /* If a particular field has no value, then zero is assumed */

         if( p_GN_QoP->horacc != 0 )
         {
            /* hor_acc_in_m = 10*( 1.1^k -1 ) */
            horacc = (I4)(10*( pow( 1.1 , p_GN_QoP->horacc ) - 1 ));
         }
         if( p_GN_QoP->veracc != 0 )
         {
            /* ver_acc_in_m = 45*( 1.025^k - 1 ) */
            veracc = (I4)(45*( pow(1.025,p_GN_QoP->veracc ) - 1 ));
         }
         if( p_GN_QoP->delay != -1 )
         {
            delay = (I4)pow( 2 , p_GN_QoP->delay );
         }
           /* local_session_num, hor_acc(in meters), ver_acc (in meters), resp_time(in seconds), num_fixes, time_btw_fixes */
           sprintf( textLog , "%d,%ld,%ld,%ld,1,0",
                   (CH)GN_SUPL_GetHandleFromRRLPHandle(handle),
                   horacc,
                   veracc,
                   delay);

           GN_SUPL_Write_Event_Log_CMCC( GN_SUPL_LOG_CMCC_AGPS_SESSION_QOP , textLog , "QoP" );
      }
    }
#endif
}
