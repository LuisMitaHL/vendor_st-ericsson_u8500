//*************************************************************************
/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */ 
//
// Filename supl_instance.c
//
// $Header: X:/MKS Projects/prototype/prototype/supl_module/rcs/supl_instance.c 1.52 2009/01/19 12:32:30Z grahama Rel $
// $Locker: $
//*************************************************************************
///
/// \ingroup supl_module
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
#include <string.h>  //memset
#include <stdio.h>
#include <stdarg.h>

#include "supl_instance.h"
#include "supl_config.h"
#include "GN_GPS_api.h"
#include "GN_SUPL_api.h"
#include "GN_RRLP_api.h"
#include "supl_log.h"

//*************************************************************************
/// \brief
///      Initialise a container for instances.
//*************************************************************************
void SUPL_Instances_Initialise
(
    U1 MaxInstanceCount             ///< Maximum number of instances supported.
)
{
   memset( &SUPL_Instances, 0, sizeof( s_SUPL_Instances ) );
   SUPL_Instances.MaxInstanceCount = MaxInstanceCount;
}

//*************************************************************************
/// \brief
///      Frees up all data for instances.
//*************************************************************************
void SUPL_Instances_DeInitialise( void )
{
   U1 i;

   for ( i = 0 ; i < INSTANCE_SUPL_MAX ; i++ )
   {
      if ( SUPL_Instances.InstanceList[i] != 0 )
      {
         SUPL_Instance_Delete( SUPL_Instances.InstanceList[i] );
      }
   }
}

//*************************************************************************
/// \brief
///      Allocates a new unique Instance.
/// \returns
///      Pointer to an Instance ID.
//*************************************************************************
s_SUPL_Instance *SUPL_Instance_Request_New( void )
{
   s_SUPL_Instance *p_SUPL_Instance = SUPL_Instance_Base_Request_New( &SUPL_Instances );

   if ( p_SUPL_Instance != NULL )
   {
         U1 Config_Major, Config_Minor, Config_Service_Indicator;

         e_SetIdType SetIdType = supl_config_get_SUPL_SetIdType();
         supl_config_get_SUPL_version( &Config_Major, &Config_Minor, &Config_Service_Indicator );
         
         p_SUPL_Instance->p_SUPL               = GN_Calloc( 1, sizeof( s_SUPL_InstanceData ) );
         p_SUPL_Instance->p_SUPL->SetSessionId = p_SUPL_Instance->ThisInstanceId;
         p_SUPL_Instance->Handle =  &p_SUPL_Instance->ThisInstanceId; // Use the Instance ID as a Handle for the time being.
         p_SUPL_Instance->p_SUPL->V2_Data.p_ApplicationID  = NULL;
         p_SUPL_Instance->p_SUPL->V2_Data.p_ThirdPartyData = NULL;
         p_SUPL_Instance->p_SUPL->SetId.type  = SetIdType;
         p_SUPL_Instance->p_SUPL->TransactionSuplVersion   = Config_Major;  // This is to ensure if SUPL_INIT decoding has failed and we would need to send SUPL END with appropriate version field

         switch ( SetIdType )
         {
         case GN_SETId_PR_msisdn:
            memcpy(  p_SUPL_Instance->p_SUPL->SetId.u.msisdn,
                     supl_config_get_SUPL_msisdn(),
                     sizeof( p_SUPL_Instance->p_SUPL->SetId.u.msisdn ) );
            break;
         case GN_SETId_PR_mdn:
            memcpy(  p_SUPL_Instance->p_SUPL->SetId.u.mdn,
                     supl_config_get_SUPL_mdn(),
                     sizeof( p_SUPL_Instance->p_SUPL->SetId.u.mdn ) );
            break;
         case GN_SETId_PR_min:
            //memcpy(  p_SUPL_Instance->p_SUPL->SetId.u.min,
            //         supl_config_get_SUPL_min(),
            //         sizeof( p_SUPL_Instance->p_SUPL->SetId.u.min ) );
            break;
         case GN_SETId_PR_imsi:
            memcpy(  p_SUPL_Instance->p_SUPL->SetId.u.imsi,
                     supl_config_get_SUPL_imsi(),
                     sizeof( p_SUPL_Instance->p_SUPL->SetId.u.imsi ) );
            break;
         case GN_SETId_PR_nai:
            break;
         case GN_SETId_PR_iPAddressV4:
            break;
         case GN_SETId_PR_iPAddressV6:
            break;
      /* Added to remove warning */
      case GN_SETId_PR_NOTHING:
         GN_SUPL_Log( "SUPL_Instance_Request_New:   No SETId Configured" );
         break;
      }
      GN_SUPL_Log_SessionStarted( p_SUPL_Instance->Handle );
   }
   else
   {
      GN_SUPL_Log( "GN_SUPL:    --- [label=\"Handle Creation Failed!\"];" );
   }
   return p_SUPL_Instance;
}

//*************************************************************************
/// \brief
///      Retrieves an instance based on an instance ID.
/// \returns
///      Pointer to an Instance ID.
//*************************************************************************
s_SUPL_Instance *SUPL_Instance_Get_Data
(
    s_SUPL_Instance  *p_ThisInstance    ///< Instance pointer.
)
{
    unsigned i;

    if ( p_ThisInstance != NULL )
    {
        for ( i = 0 ; i < SUPL_Instances.MaxInstanceCount ; i++ )
        {
            if ( SUPL_Instances.InstanceList[i] == p_ThisInstance )
            {
                return SUPL_Instances.InstanceList[i];
            }
        }
    }
    return NULL;
}

//*************************************************************************
/// \brief
///      Request a new non specific instance from an instance container.
/// \returns
///      Pointer to an instance.
//*************************************************************************
s_SUPL_Instance *SUPL_Instance_Base_Request_New
(
   s_SUPL_Instances *p_Instances   ///< Instance container from which to
                                   /// obtain an instance.
)
{
   s_SUPL_Instance *p_Instance_Unused;

   if ( p_Instances == NULL )
   {
      return NULL;
   }
   else
   {
      t_SUPL_InstanceId    NewInstanceId;
      BL InstanceId_Unique;
      BL InstanceId_Found;
      U2 i;

      for ( i = 0 ; i < p_Instances->MaxInstanceCount ; i++ )
      {
         if ( p_Instances->InstanceList[i] == NULL )
         {
            break ;
         }
      }
      if ( i != p_Instances->MaxInstanceCount ) // We found a gap.
      {
         // We found an unused instance entry. Allocate storage.
         p_Instances->InstanceList[i] = GN_Calloc( 1, sizeof( s_SUPL_Instance ) );

         p_Instance_Unused = p_Instances->InstanceList[i] ;

         InstanceId_Unique = FALSE;
         while ( ! InstanceId_Unique )
         {
            // Calculate a new instance id.
            if ( p_Instances->InstanceIdHighWaterMark < INSTANCE_SUPL_LIMIT - INSTANCE_SUPL_MAX )
            {
               NewInstanceId = p_Instances->InstanceIdHighWaterMark + 1 ;
               p_Instances->InstanceIdHighWaterMark = NewInstanceId ;
            }
            else
            {
               NewInstanceId = 1;
               p_Instances->InstanceIdHighWaterMark = NewInstanceId ;
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
            if ( ! InstanceId_Found ) InstanceId_Unique = TRUE ;
         }
         p_Instances->InstanceIdHighWaterMark = NewInstanceId ;

         // Store the new instance id.
         p_Instance_Unused->ThisInstanceId = NewInstanceId ;
         return p_Instance_Unused ;
      }
      else
      {
         return NULL ;
      }
   }
}

//*****************************************************************************
/// \brief
///      Deletes an existing instance from the #SUPL_Instance list.
///
/// <b>Global Parameters:</b>
///      <p>#SUPL_Instances [global] Container for current SUPL instances.
//*****************************************************************************
BL SUPL_Instance_Delete
(
   s_SUPL_Instance *p_SUPL_Instance ///< Instance to delete.
)
{
   BL successful = FALSE;
   s_SUPL_Instance *p_TempInstance = SUPL_Instance_Get_Data( p_SUPL_Instance );

   if ( p_TempInstance != NULL )
   {
      if ( p_SUPL_Instance->p_SUPL != NULL )
      {

         /////\Todo This case is no longer valid.
         //if ( p_SUPL_Instance->Peer_RRLP_Instance != NULL )
         //{
         //   BL Status;
         //   e_GN_Status GN_Status;
         //   Status =  GN_RRLP_Sequence_End_In( p_SUPL_Instance->Handle, &GN_Status );
         //}

         if ( p_SUPL_Instance->POS_Handle != NULL )
         {
            BL Status;
            e_GN_Status GN_Status = GN_SUCCESS;
            Status =  GN_RRLP_Sequence_End_In( p_SUPL_Instance->POS_Handle, &GN_Status, NULL );
         }
         if ( p_SUPL_Instance->p_SUPL->p_SlpSessionId != NULL )
         {
            GN_Free( p_SUPL_Instance->p_SUPL->p_SlpSessionId );
            p_SUPL_Instance->p_SUPL->p_SlpSessionId = NULL;
         }
         if ( p_SUPL_Instance->p_SUPL->p_GN_QoP != NULL )
         {
            GN_Free( p_SUPL_Instance->p_SUPL->p_GN_QoP );
            p_SUPL_Instance->p_SUPL->p_GN_QoP = NULL;
         }
         if ( p_SUPL_Instance->p_SUPL->p_VER != NULL )
         {
            GN_Free( p_SUPL_Instance->p_SUPL->p_VER );
            p_SUPL_Instance->p_SUPL->p_VER = NULL;
         }
         if ( p_SUPL_Instance->p_SUPL->SUPL_INIT.p_PDU_Data != NULL )
         {
            GN_Free( p_SUPL_Instance->p_SUPL->SUPL_INIT.p_PDU_Data );
         }
         supl_Free_LocationId_Data( &p_SUPL_Instance->p_SUPL->LocationId );
         if( p_SUPL_Instance->p_SUPL->V2_Data.p_ThirdPartyData != NULL)
         {
            supl_Free_ThirdParty_Data( p_SUPL_Instance->p_SUPL->V2_Data.p_ThirdPartyData );
            p_SUPL_Instance->p_SUPL->V2_Data.p_ThirdPartyData = NULL;
         }
         if( p_SUPL_Instance->p_SUPL->V2_Data.p_ApplicationID != NULL)
         {
             supl_Free_ApplicationID_Data( p_SUPL_Instance->p_SUPL->V2_Data.p_ApplicationID);
             p_SUPL_Instance->p_SUPL->V2_Data.p_ApplicationID = NULL;
         }
         GN_Free( p_SUPL_Instance->p_SUPL );
         p_SUPL_Instance->p_SUPL = NULL;
      }
      
      successful = SUPL_Instance_Base_Delete( p_SUPL_Instance );
   }
   SUPL_Segmented_PDU_Buffer_Init();
   return successful;
}


//*************************************************************************
/// \brief
///      Retrieves the SUPL Instance identifier from SUPL Handle.
/// \details
///      Retrieves the SUPL Instance identifier which is associated with the
///      positioning sequence associated with the handle SUPL handle.
//*************************************************************************
s_SUPL_Instance *SUPL_Instance_From_Handle
(
   void *Handle      ///< SUPL subsystem handle.
)
{
   U1 i;
   for ( i = 0 ; i < INSTANCE_SUPL_MAX ; i++ )
   {
      if ( SUPL_Instances.InstanceList[i] != NULL )
      {
         if ( SUPL_Instances.InstanceList[i]->Handle == Handle )
         {
            return SUPL_Instances.InstanceList[i];
         }
      }
   }
   return NULL;
}

//*************************************************************************
/// \brief
///      Retrieves the SUPL Instance identifier from POS_Handle.
/// \details
///      Retrieves the SUPL Instance identifier which is associated with the
///      positioning sequence associated with the handle POS_handle.
//*************************************************************************
s_SUPL_Instance *SUPL_Instance_From_POS_Handle
(
   void *POS_Handle     ///< Positioning subsystem Handle.
)
{
   U1 i;
   for ( i = 0 ; i < INSTANCE_SUPL_MAX ; i++ )
   {
      if ( SUPL_Instances.InstanceList[i] != NULL )
      {
         if ( SUPL_Instances.InstanceList[i]->POS_Handle == POS_Handle )
         {
            return SUPL_Instances.InstanceList[i];
         }
      }
   }
   return NULL;
}


//*************************************************************************
/// \brief
///      Removes the Instance entry from the Instance container.
/// \todo
///      Free up data in this instance.
//*************************************************************************
BL SUPL_Instance_Base_Delete
(
    s_SUPL_Instance *p_ThisInstance ///< Pointer to Instance entry to remove.
)
{
   unsigned i;

   for ( i = 0 ; i < SUPL_Instances.MaxInstanceCount ; i++ )
   {
      if ( SUPL_Instances.InstanceList[i] == p_ThisInstance )
      {
         GN_Free( p_ThisInstance );
         SUPL_Instances.InstanceList[i] = NULL;
         // No point checking the rest of the entries. Bail out now.
         return TRUE;
      }
   }
   return FALSE;
}

//*************************************************************************
/// \brief
///      Provides a State Description from a State enum.
/// \returns
///      Pointer to a string with a descriptive name.
//*************************************************************************
CH *SUPL_StateDesc_From_State( e_SUPL_States State )
{
   char * SUPL_States_Text[] = {
      "state_SUPL_Idle",
      "state_SUPL_Push_Received",
      "state_SUPL_Comms_Open_Sent",
      "state_SUPL_SUPL_START_Sent",
      "state_SUPL_SUPL_RESPONSE_Sent",

      "state_SUPL_SUPL_POS_INIT_Sent",
      "state_SUPL_Waiting_For_RRLP",
      "state_SUPL_POS_Payload_Delivered",
      "state_SUPL_SUPL_POS_Sent",
      "state_SUPL_SUPL_END_Sent",

      "state_SUPL_Server_Open_Requested",
      "state_SUPL_Server_Active",
      "state_SUPL_Comms_Close_Sent",
      "state_SUPL_Awaiting_Confirmation",
/* +LMSqb89433 */ 
      "state_SUPL_Awaiting_Confirmation_Or_Comms_Open_Sent",
/* -LMSqb89433 */ 
      "state_SUPL_MO_Position_Requested",

/* RRR : 26-08-2010 : SUPL-2.0 specific states */

      "state_SUPLv2_SUPL_TRIGGER_START_Sent",
      "state_SUPLv2_SUPL_TRIGGER_Active",
      "state_SUPLv2_SUPL_TRIGGER_Suspended",
      "state_SUPLv2_Location_Notification",
      "state_SUPLv2_SUPL_SET_INIT_Sent",

/* RRR : 26-08-2010 : SUPL-2.0 specific states */

      "state_SUPL_Error"
   };
   return SUPL_States_Text[State];
}

//*****************************************************************************
/// \brief
///      Retrieves a description associated with the Status passed in.
/// \returns
///      Pointer to description associated with GN_Status.
//*****************************************************************************
CH* GN_SUPL_StatusDesc_From_Status
(
   e_GN_Status GN_Status   ///< Status identifier.
)
{
   switch ( GN_Status )
   {
   case GN_SUCCESS:                 return "GN Success Status"; break;
   // Error Return State Codes
   case GN_ERR_HANDLE_INVALID:      return "GN Error Status - Handle provided was not recognised."; break;
   case GN_ERR_POINTER_INVALID:     return "GN Error Status - Pointer provided was not set (ie NULL)."; break;
   case GN_ERR_PARAMETER_INVALID:   return "GN Error Status - Parameter provided was not valid (ie invalid value)."; break;
   case GN_ERR_NO_RESOURCE:         return "GN Error Status - No resources available to perform action."; break;
   case GN_ERR_CONN_SHUTDOWN:       return "GN Error Status - Connection was shutdown spontaneously."; break;
   case GN_ERR_CONN_REJECTED:       return "GN Error Status - Connection attempt was rejected."; break;
   case GN_ERR_CONN_TIMEOUT:        return "GN Error Status - Connection attempt timed out."; break;
   // Warning Return State Codes
   case GN_WRN_NOT_READY:           return "GN Warning Status - Destination not ready."; break;
   case GN_WRN_NO_POSITION:         return "GN Warning Status - No position available."; break;
   default:                         return "GN Error status: Unknown "; break;
   }
}

//*****************************************************************************
/// \brief
///      Handles the State Transition for the SUPL Agent.
/// \warning
///      The entries in SUPL_States_Text have to line up with the enumerated
///      values in #e_SUPL_States for the logging to work correctly.
//*****************************************************************************
void SUPL_Instance_State_Transition
(
   s_SUPL_Instance   *p_SUPL_Instance, ///< Instance to transition to the new state.
   e_SUPL_States     New_State,        ///< New State.
   CH                *Cause            ///< What caused the transition (usually an event).
)
{
   s_SUPL_Instance *p_tempInstance = SUPL_Instance_Get_Data( p_SUPL_Instance );

   if ( p_tempInstance != NULL )
   {

     GN_SUPL_Log(
        "State:SUPL:    %s->%s [label=\"SUPL Event:%s\"]",
        SUPL_StateDesc_From_State( p_tempInstance->p_SUPL->State ),
        SUPL_StateDesc_From_State( New_State ),
        Cause );
      p_tempInstance->p_SUPL->State = New_State;
   }
}


