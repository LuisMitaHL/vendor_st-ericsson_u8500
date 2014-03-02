/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */


//file elaborated from val_vpip.c label GAILLARD_FWVALID_8820A-BEFORE_NMF

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "hi_register_acces.h"
#include "ipp.h"

#include "ite_testenv_utils.h"
#include "VhcElementDefs.h"

#include "primary_sensor.h"
#include "secondary_sensor.h"

#include "ite_vpip.h"
#include "ite_event.h"
#include "ite_pipeinfo.h"
#include "ite_host2sensor.h"
#include "ite_event.h"
#include "ite_recordsystem_time.h"
#include "ite_sia_bootcmd.h"


//For NMF
#include "ite_sia_interface_data.h"
#include <cm/inc/cm_macros.h>
#include <inc/type.h>
#include <los/api/los_api.h>

extern CSensor* iSensor;

extern t_uint8 ispctl_clientId;
extern ts_sensInfo SensorsInfo[2];
extern t_uint8 ITE_Event_Log_flag;
t_uint8 LOS_Log_flag=TRUE;
extern ts_sia_usecase usecase;


/************************************************/
/* void ITE_Log_RW_pageelements_enable(void)    */
/*                      */
/* set flag to tree             */
/************************************************/
void ITE_Log_RW_pageelements_enable(void)
{
   LOS_Log_flag=TRUE;
}
/************************************************/
/* void ITE_Log_RW_pageelements_disable(void)   */
/*                      */
/* set flag to tree             */
/************************************************/
void ITE_Log_RW_pageelements_disable(void)
{
   LOS_Log_flag=FALSE;
}
/************************************************/
/* void ITE_Log(void)       */
/* Initialise and start MMDSP timer     */
/* Should be rework with PMU LOS API        */
/************************************************/
void ITE_Log( char*pstring,void*pdata1,void*pdata2,void*pdata3,void*pdata4,void* pdata5,void* pdata6)
{
   if ( LOS_Log_flag==TRUE)
     LOS_Log((char*)pstring,
     (void*)pdata1,(void*)pdata2,(void*)pdata3,
     (void*)pdata4,(void*)pdata5,(void*)pdata6);

}
/************************************************/
/* void ITE_InitMMDSPTimer(void)        */
/* Initialise and start MMDSP timer     */
/* Should be rework with PMU LOS API        */
/************************************************/
/*
void ITE_InitMMDSPTimer(void) {
    int i;
    t_uint32 siaBaseAdresse = 0;
    siaBaseAdresse = LOS_Remap((t_los_physical_address)SIA_MEM_BASE_ADDR, (SIA_MEM_END_ADDR - SIA_MEM_BASE_ADDR)+sizeof(t_uint32), LOS_MAPPING_CREATE_UNCACHED_UNBUFFERED);


//Programme the timer of ITC
#ifdef __THINK_FLAT_MAPPING
#define SIA_BASE_ADDRESS 0X90250000
#define CONFIG_REG_BASE_ADDRESS 0xA03D8000 /// value to change ??????
#else
#define SIA_BASE_ADDRESS 0XE02E0000
#define CONFIG_REG_BASE_ADDRESS 0xA03D8000 // value to change ?????
#endif

#define CR_REG_0 0x000

   #ifdef __STN_8500
     // disable mmdsp timer
     *(t_uint32 *)(CONFIG_REG_BASE_ADDRESS + CR_REG_0) &=  ~MASK_BIT26; // HTIMEN bit clear
   #else
    //MM WRITE on ITC timer to use 32-bit ITC counter
    //MM set TIMER_ENABLE to 0 (bit 27 HITIMEN in PMU IP
    PMU_SetSaaSvaSiaTimerControl(PMU_SAASVASIA_TIMER_DISABLED);
   #endif


   //MM WRITE timer initialization
    //use logical address
    // *((unsigned*)( SIA_BASE_ADDRESS + 0x5BC00)) = 0x0;
    *((unsigned*)( siaBaseAdresse + 0x50000 + 0x5BC00)) = 0x0;

    //wait
    for(i=0;i<0x10;i++);    //wait for at least 8 SVA clock cycles (to ensure the following write access copies the value into the timer)


   #ifdef __STN_8500
     // enable mmdsp timer
     *(t_uint32 *)(CONFIG_REG_BASE_ADDRESS + CR_REG_0) |=  MASK_BIT26; // HTIMEN bit set
   #else
    //MM set TIMER_ENABLE to 1
    PMU_SetSaaSvaSiaTimerControl(PMU_SAASVASIA_TIMER_ENABLED);
    #endif

}
*/

/************************************************/
/* void ITE_InitMMDSPTimer(void)        */
/* Initialise and start MMDSP timer     */
/* Should be rework with PMU LOS API        */
/************************************************/
void ITE_InitMMDSPTimer(void) {

#if !(defined(__ARM_LINUX) || defined(__ARM_SYMBIAN))
ILOS_sxaTimer90KhzStart();
#endif

/*
int i;
t_uint32 siaBaseAdresse = 0;
t_uint32 *pCR;

siaBaseAdresse = LOS_Remap((t_los_physical_address)SIA_MEM_BASE_ADDR, (SIA_MEM_END_ADDR - SIA_MEM_BASE_ADDR)+sizeof(t_uint32), LOS_MAPPING_CREATE_UNCACHED_UNBUFFERED);

// disable mmdsp timer
pCR = (t_uint32 *)(LOS_Remap(CR_REG_BASE_ADDR,0xFFF,LOS_MAPPING_CREATE_UNCACHED_UNBUFFERED) + 0x00);


*pCR &=  ~MASK_BIT26; // HTIMEN bit clear

//MM WRITE timer initialization
//use logical address
//// *((unsigned*)( SIA_BASE_ADDRESS + 0x5BC00)) = 0x0;
*((unsigned*)( siaBaseAdresse  + 0x5BC00)) = 0x0;


//wait
for(i=0;i<0x10;i++);    //wait for at least 8 SVA clock cycles (to ensure the following write access copies the value into the timer)


// enable mmdsp timer
*pCR |=  MASK_BIT26; // HTIMEN bit set
*/
}

/* ------------------------------------------------------------------------
   FUNCTION : ITE_writePE
   PURPOSE  : write a Page Element and wait for acknowledge.
   ------------------------------------------------------------------------ */

void ITE_writePE(t_uint16 addr, t_uint32 data)
{
   union u_ITE_Event event;


   NMFCALL (ispctlCommand, writePageElement)(addr, data,ispctl_clientId);


   do {
        event = ITE_WaitEvent(ITE_EVT_ISPCTL_RW_EVENTS);

      switch (event.type) {
         case ITE_EVT_ISPCTL_RW_EVENTS:
           //dg add to catch real readPE event and not another ispctl info: to be improved so as to guaranty no event loose
           //avoid ispctl error 4 (forbidden state tranistion in ispctl) that occured randomly
            if (event.ispctlInfo.info_id == ISP_WRITE_DONE)
           {
			   if ( LOS_Log_flag==TRUE) LOS_Log("|- HV : Write Page element at @ 0x%x D 0x%x : %s\n",   addr,
	                                                                     data, iSensor->GetPeName(addr),
                                                                         NULL,NULL,NULL);

                return;
            }
            break;

         case ITE_EVT_ISPCTL_ERROR:

            if(event.ispctlError.error_id == ISP_DOES_NOT_RESPOND_ERROR)
            {
                LOS_Log("ISPCTL error %i ==> PANIC!! IRP DOES NOT RESPOND ANYMORE to the last Write command\n", event.ispctlError.error_id, NULL, NULL, NULL,NULL,NULL);
                exit(1);
            }
            else
            {
                LOS_Log("ISPCTL error %i\n",    event.ispctlError.error_id,NULL,NULL,NULL, NULL, NULL);
            }
            return;

         case ITE_EVT_ISPCTL_DEBUG:
            LOS_Log("ISPCTL debug event %i, data1=%i, data2=%i\n",    event.ispctlDebug.debug_id,
                                                                      event.ispctlDebug.data1,
                                                                      event.ispctlDebug.data2,
                                                                     NULL,NULL,NULL);
            break;

         default:
            LOS_Log("Unexpected event type %i\n",  event.type,
                                                   NULL,NULL,NULL,NULL,NULL);
            break;
        }
    } while (1);

}

/* ---------------------------------------------------------------------------------------------------------
   FUNCTION : ITE_WaitExpectedPeValue
   PURPOSE  : waiting for a specific PE value after a write and wait for acknowledge.
   ---------------------------------------------------------------------------------------------------------- */

t_uint32 ITE_WaitExpectedPeValue(t_uint16 addr, t_uint32 value_expected, t_uint32 polling_frequency, t_uint32 timeout)
{
   union u_ITE_Event event;

   NMFCALL (ispctlCommand, WaitExpectedPeValue)(addr, value_expected, polling_frequency, timeout,ispctl_clientId);

   do
   {

        event = ITE_WaitEvent(ITE_EVT_ISPCTL_EVENTS);

        switch (event.type)
        {
            case ITE_EVT_ISPCTL_INFO:
            if ( LOS_Log_flag==TRUE)    LOS_Log("|- HV : Read Page element at @ 0x%x D 0x%x\n", addr,
                                                                        event.ispctlInfo.value,
                                                                        NULL,NULL,NULL,NULL);
            return event.ispctlInfo.value;

            case ITE_EVT_ISPCTL_ERROR:

            if(event.ispctlError.error_id == ISP_DOES_NOT_RESPOND_ERROR)
                {
                    LOS_Log("ISPCTL error %i ==> PANIC!! IRP DOES NOT RESPOND ANYMORE  to the last polling command\n", event.ispctlError.error_id, NULL, NULL, NULL,NULL,NULL);
                    exit(1);
                }
                else if(event.ispctlError.error_id == ISP_POLLING_TIMEOUT_ERROR)
                {
                    LOS_Log("ISPCTL error %i ==> timeout, last Page element value polled 0x%x\n",   event.ispctlError.error_id,
                                                                                        event.ispctlError.last_PE_data_polled,
                                                                                        NULL,NULL,NULL,NULL);
                }
                else if(event.ispctlError.error_id == ISP_CHECK_VALUE_ERROR)
                {
                    LOS_Log("ISPCTL error %i ==> check error, value expected:0x%x, value read:0x%x\n",  event.ispctlError.error_id,
                                                                                                        value_expected,
                                                                                                        event.ispctlError.last_PE_data_polled,
                                                                                                        NULL,NULL,NULL,NULL);
                }
                else
                {
                    LOS_Log("ISPCTL error %i, last Page element value polled 0x%x\n",   event.ispctlError.error_id,
                                                                                        event.ispctlError.last_PE_data_polled,
                                                                                        NULL,NULL,NULL,NULL);
                }
            return event.ispctlError.last_PE_data_polled;

            case ITE_EVT_ISPCTL_DEBUG:
                LOS_Log("ISPCTL debug event %i, data1=%i, data2=%i\n",  event.ispctlDebug.debug_id,
                                                                        event.ispctlDebug.data1,
                                                                        event.ispctlDebug.data2,
                                                                        NULL,NULL,NULL);
            break;

            default:
                LOS_Log("Unexpected event type %i\n",   event.type,
                                                        NULL,NULL,NULL,NULL,NULL);
            break;
        }
    }while (1);

}

/* -------------------------------------------------------------------------------------
   FUNCTION : ITE_writeListPE
   PURPOSE  : write a List of Page Element and wait for acknowledge.
   --------------------------------------------------------------------------------------*/
void ITE_writeListPE(ts_PageElement tab_pe[], t_uint16 number_of_pe)
{
    union u_ITE_Event event;
    int i=0;

    NMFCALL (ispctlCommand, writeListPageElement)(tab_pe, number_of_pe,ispctl_clientId);

    do
    {
        event = ITE_WaitEvent(ITE_EVT_ISPCTL_EVENTS);

        switch (event.type)
        {
            case ITE_EVT_ISPCTL_INFO:
              if ( LOS_Log_flag==TRUE) {
                for(i=0;i<number_of_pe;i++)
                {LOS_Log("|- HV : Write Page element at @ 0x%x D 0x%x : %s\n",    tab_pe[i].pe_addr,
                                                                             tab_pe[i].pe_data, iSensor->GetPeName(tab_pe[i].pe_addr),
                                                                             NULL,NULL,NULL);}}
            return;

            case ITE_EVT_ISPCTL_ERROR:
                if(event.ispctlError.error_id == ISP_READ_ONLY_IN_LIST_ERROR)
                {
                    i=event.ispctlError.number_of_pe;
                    LOS_Log("ISPCTL error %i ==> Addr 0x%x is READ ONLY!! --> array index: %i\n",  event.ispctlError.error_id,
                                                                                                tab_pe[i].pe_addr,
                                                                                                event.ispctlError.number_of_pe,
                                                                                                NULL,NULL,NULL);
                }
                else if(event.ispctlError.error_id == ISP_WRITELIST_NOT_DONE_OUT_OF_BOUNDARY_ERROR)
                {
                    i=event.ispctlError.number_of_pe;
                    LOS_Log("ISPCTL error %i ==> Write array is out of boundary!! \n", event.ispctlError.error_id, NULL, NULL, NULL,NULL,NULL);
                }
                else if(event.ispctlError.error_id == ISP_DOES_NOT_RESPOND_ERROR)
                {
                    LOS_Log("ISPCTL error %i ==> PANIC!! IRP DOES NOT RESPOND ANYMORE  to the last WriteList command\n", event.ispctlError.error_id,
                                                                                                                         NULL, NULL, NULL,NULL,NULL);
                    exit(1);
                }
                else
                {
                    LOS_Log("ISPCTL error %i\n",    event.ispctlError.error_id,NULL,NULL,NULL, NULL, NULL);
                }
            return;

            case ITE_EVT_ISPCTL_DEBUG:
            LOS_Log("ISPCTL debug event %i, data1=%i, data2=%i\n",   event.ispctlDebug.debug_id,
                                                                     event.ispctlDebug.data1,
                                                                     event.ispctlDebug.data2,
                                                                     NULL,NULL,NULL);
            break;

            default:
            LOS_Log("Unexpected event type %i\n",  event.type,
                                                   NULL,NULL,NULL,NULL,NULL);
            break;
        }
    } while (1);

}

/* ------------------------------------------------------------------------
   FUNCTION : ITE_readListPE
   PURPOSE  : read a Page Element, wait for acknowledge, and return value.
   ------------------------------------------------------------------------ */
t_uint16 ITE_readListPE(ts_PageElement readback_tab_pe[], t_uint16 number_of_pe)
{
    union u_ITE_Event event;
    int i=0;

   NMFCALL (ispctlCommand, readListPageElement)(readback_tab_pe, number_of_pe,ispctl_clientId);

    do
    {
        event = ITE_WaitEvent(ITE_EVT_ISPCTL_EVENTS);

        switch (event.type) {
            case ITE_EVT_ISPCTL_INFO:
                if ( LOS_Log_flag==TRUE)
                LOS_Log("|- HV : Number of Pe read: %d\n", event.ispctlInfo.number_of_pe,NULL,NULL,NULL, NULL, NULL);

                for(i=0;i<number_of_pe;i++)
                {
                    if ( LOS_Log_flag==TRUE)
                    LOS_Log("|- HV : Read Page element at @ 0x%x D 0x%x : %s\n", event.ispctlInfo.Listvalue[i].pe_addr,
                                                                            event.ispctlInfo.Listvalue[i].pe_data, iSensor->GetPeName(event.ispctlInfo.Listvalue[i].pe_addr),
                                                                            NULL,NULL,NULL);
                    readback_tab_pe[i] = event.ispctlInfo.Listvalue[i];
                }
            return 1;

            case ITE_EVT_ISPCTL_ERROR:
                if(event.ispctlError.error_id == ISP_READLIST_NOT_DONE_OUT_OF_BOUNDARY_ERROR)
                {
                    i=event.ispctlError.number_of_pe;
                    LOS_Log("ISPCTL error %i ==> read array is out of boundary!! \n", event.ispctlError.error_id, NULL, NULL, NULL,NULL,NULL);
                }
                else if(event.ispctlError.error_id == ISP_DOES_NOT_RESPOND_ERROR)
                {
                    LOS_Log("ISPCTL error %i ==> PANIC!! IRP DOES NOT RESPOND ANYMORE  to the last ReadList command\n", event.ispctlError.error_id,
                                                                                                                        NULL, NULL, NULL,NULL,NULL);
                    exit(1);
                }
                else
                {
                    LOS_Log("ISPCTL error %i\n",    event.ispctlError.error_id,NULL,NULL,NULL, NULL, NULL);
                }
            return 0;

            case ITE_EVT_ISPCTL_DEBUG:
                LOS_Log("ISPCTL debug event %i, data1=%i, data2=%i\n",  event.ispctlDebug.debug_id,
                                                                        event.ispctlDebug.data1,
                                                                        event.ispctlDebug.data2,
                                                                        NULL,NULL,NULL);
            break;

            default:
                LOS_Log("Unexpected event type %i\n",   event.type,NULL,NULL,NULL,NULL,NULL);
            break;
        }
    }while (1);

}
/* ------------------------------------------------------------------------
   FUNCTION : ITE_readPE
   PURPOSE  : read a Page Element, wait for acknowledge, and return value.
   ------------------------------------------------------------------------ */
t_uint32 ITE_readPE(t_uint16 addr)
{
   union u_ITE_Event event;
   NMFCALL (ispctlCommand, readPageElement)(addr,ispctl_clientId);
   do {
       event = ITE_WaitEvent(ITE_EVT_ISPCTL_RW_EVENTS);
      switch (event.type) {
         case ITE_EVT_ISPCTL_RW_EVENTS:
            //dg add to catch real readPE event and not another ispctl info: to be improved so as to guaranty no event loose
            //avoid ispctl error 4 (forbidden state tranistion in ispctl) that occured randomly
            if (event.ispctlInfo.info_id == ISP_READ_DONE)
           {
                if ( LOS_Log_flag==TRUE) LOS_Log("|- HV : Read Page element at @ 0x%x D 0x%x : %s\n", addr,
                                                                      event.ispctlInfo.value, iSensor->GetPeName(addr),
                                      NULL,NULL,NULL);
                return event.ispctlInfo.value;
            }
            break;

         case ITE_EVT_ISPCTL_ERROR:
            if(event.ispctlError.error_id == ISP_DOES_NOT_RESPOND_ERROR)
            {
             LOS_Log("ISPCTL error %i ==> PANIC!! IRP DOES NOT RESPOND ANYMORE  to the last Read command\n", event.ispctlError.error_id, NULL, NULL, NULL,NULL,NULL);
                exit(1);
            }
            else
            {
             LOS_Log("ISPCTL error %i\n",    event.ispctlError.error_id,NULL,NULL,NULL, NULL, NULL);
            }
            return 0;

         case ITE_EVT_ISPCTL_DEBUG:
             LOS_Log("ISPCTL debug event %i, data1=%i, data2=%i\n",  event.ispctlDebug.debug_id,
                                                                     event.ispctlDebug.data1,
                                                                     event.ispctlDebug.data2);
            break;

         default:
            LOS_Log("Unexpected event type %i\n",event.type);
            break;
        }
    }while (1);
}



float ITE_float_readPE(t_uint16    addr)
{
    float       f_value_read;
    t_uint32    u32_value_read;

    u32_value_read = ITE_readPE(addr);
    f_value_read = *(( float * ) &u32_value_read);

    LOS_Log("PE Read is :%f\n", f_value_read);

    return (f_value_read);
}


//Ported on 8500 V1
//First implementation: synchronous: send command and wait for interrupt
int ITE_SendCommands(t_uint32 data, enum e_ispctlInfo trigEvent){
   union u_ITE_Event event;


   ITE_writePE(HostInterface_Control_e_HostInterfaceCommand_User_Byte0, data);
   do
   {
       event = ITE_WaitEvent(ITE_EVT_ISPCTL_EVENTS);
       if(ITE_Event_Log_flag == TRUE) ITE_traceISPCTLevent(event);
       if(event.type == ITE_EVT_ISPCTL_ERROR) break;
   }
   while( !((event.type == ITE_EVT_ISPCTL_INFO) && (event.ispctlInfo.info_id == trigEvent)) );
   if (event.type == ITE_EVT_ISPCTL_ERROR)
        return(event.ispctlError.error_id);
   else
        return(0);
}

// Do boot sequence as indicated by SensorPowerManagement_Control_e_Flag_EnableSMIAPP_PowerUpSequence_Byte0
// and Sensor_Tuning_Control_e_Flag_ReadConfigBeforeBoot_Byte0 page elements
int ITE_DoBootSequence(int smiapp_power_sequence, int sensor_tunning_before_boot)
{
   union u_ITE_Event event;
     int Error = 0;
   UNUSED(Error);

    if (smiapp_power_sequence)
    {
         ITE_writePE(SensorPowerManagement_Control_e_Flag_EnableSMIAPP_PowerUpSequence_Byte0, Flag_e_TRUE);
    }

    if (sensor_tunning_before_boot)
    {
        ITE_writePE(Sensor_Tuning_Control_e_Flag_ReadConfigBeforeBoot_Byte0, Flag_e_TRUE);
    }

     ITE_writePE(HostInterface_Control_e_HostInterfaceCommand_User_Byte0, HostInterfaceCommand_e_BOOT);

     if (smiapp_power_sequence)
         {
         Error =  ITE_Do_SmiaPP_PowerUp_Sequence();
         }

     if (sensor_tunning_before_boot)
         {
         Error =  ITE_Do_SENSOR_TUNNING_Sequence_Before_Boot(HostInterfaceCommand_e_BOOT, ISP_BOOT_COMPLETE);
         }

   do
   {
       event = ITE_WaitEvent(ITE_EVT_ISPCTL_EVENTS);
       if(ITE_Event_Log_flag == TRUE) ITE_traceISPCTLevent(event);
       if(event.type == ITE_EVT_ISPCTL_ERROR) break;
   }
    while( !((event.type == ITE_EVT_ISPCTL_INFO) && (event.ispctlInfo.info_id == ISP_BOOT_COMPLETE)) );

   if (event.type == ITE_EVT_ISPCTL_ERROR)
        return(event.ispctlError.error_id);
   else
        return(0);

}


int ITE_Do_SmiaPP_PowerUp_Sequence()
{
    t_uint32 u32_power_action_requested = 0;
    const unsigned int delay = 200;

    union u_ITE_Event	event;

    LOS_Sleep(delay);

    ITE_RefreshEventCount(Event0_Count_u16_EVENT0_28_Power_Notification_Byte0);

    u32_power_action_requested = ITE_readPE(SensorPowerManagement_Status_e_PowerCommand_Byte0);
    DoPowerAction(u32_power_action_requested);

    ITE_writePE(SensorPowerManagement_Control_e_Flag_Result_Byte0, Flag_e_TRUE);
    ITE_writePE(SensorPowerManagement_Control_e_PowerAction_Byte0, PowerAction_e_complete);

    LOS_Sleep(delay);
    do
    {
        event = ITE_WaitEvent(ISP_POWER_NOTIFICATION_EVENTS );
        ITE_traceISPCTLevent(event);
        if (event.type == ITE_EVT_ISPCTL_ERROR)
        {
            break;
        }
    } while (!(event.ispctlInfo.info_id ==ISP_POWER_NOTIFICATION));


    ITE_GetEventCount(Event0_Count_u16_EVENT0_28_Power_Notification_Byte0);

    u32_power_action_requested = ITE_readPE(SensorPowerManagement_Status_e_PowerCommand_Byte0);
    DoPowerAction(u32_power_action_requested);

    ITE_writePE(SensorPowerManagement_Control_e_Flag_Result_Byte0, Flag_e_TRUE);
    ITE_writePE(SensorPowerManagement_Control_e_PowerAction_Byte0, PowerAction_e_complete);

    LOS_Sleep(delay);

    return 0;
}


int ITE_Do_SmiaPP_PowerDown_Sequence()
{
    t_uint32 u32_power_action_requested = 0;
    const unsigned int delay = 2000;
    union u_ITE_Event event;
    LOS_Log(">> ITE_Do_SmiaPP_PowerDown_Sequence");

    LOS_Sleep(delay);

    ITE_RefreshEventCount(Event0_Count_u16_EVENT0_28_Power_Notification_Byte0);

    u32_power_action_requested = ITE_readPE(SensorPowerManagement_Status_e_PowerCommand_Byte0);
    DoPowerAction(u32_power_action_requested);

    ITE_writePE(SensorPowerManagement_Control_e_Flag_Result_Byte0, Flag_e_TRUE);
    ITE_writePE(SensorPowerManagement_Control_e_PowerAction_Byte0, PowerAction_e_complete);

    LOS_Sleep(delay);
   do
    {
        event = ITE_WaitEvent(ISP_POWER_NOTIFICATION_EVENTS );
        ITE_traceISPCTLevent(event);
        if (event.type == ITE_EVT_ISPCTL_ERROR)
        {
            break;
        }
    } while (!(event.ispctlInfo.info_id ==ISP_POWER_NOTIFICATION));

    ITE_GetEventCount(Event0_Count_u16_EVENT0_28_Power_Notification_Byte0);

    u32_power_action_requested = ITE_readPE(SensorPowerManagement_Status_e_PowerCommand_Byte0);
    DoPowerAction(u32_power_action_requested);

    ITE_writePE(SensorPowerManagement_Control_e_Flag_Result_Byte0, Flag_e_TRUE);
    ITE_writePE(SensorPowerManagement_Control_e_PowerAction_Byte0, PowerAction_e_complete);

    LOS_Sleep(delay);
    LOS_Log("<< ITE_Do_SmiaPP_PowerDown_Sequence");
    return 0;
}


int ITE_Do_SENSOR_TUNNING_Sequence_Before_Boot(t_uint32 data, enum e_ispctlInfo trigEvent)
{
//    t_uint32 sensor_tunning_event_count_2 = 0;
  union u_ITE_Event event;
  UNUSED(data);
  UNUSED(trigEvent);
   // check if sensor tunning events are recieved
   /*
   do
   {
     ITE_RefreshEventCount(Event0_Count_u16_EVENT0_27_SensorTuning_Available_Byte0);
     ITE_GetEventCount(Event0_Count_u16_EVENT0_27_SensorTuning_Available_Byte0, (t_uint16 *)&sensor_tunning_event_count_2);
   } while (0 == sensor_tunning_event_count_2);
   */
   do
   {
	   event = ITE_WaitEvent(ISP_CDCC_AVAILABLE_EVENTS );
	   if(ITE_Event_Log_flag == TRUE) ITE_traceISPCTLevent(event);
   }
   while( !(event.ispctlInfo.info_id == ISP_CDCC_AVAILABLE));

   LOS_Log("********* SENSOR_TUNNING event recieved **********");

#ifdef __TUNING_FILES_AVAILABLE__
   usecase.FwTuning_handler = ITE_FirmwareTuning();
#endif //__TUNING_FILES_AVAILABLE__
   // tell FW that SENSOR_TUNNING querrying is done
   ITE_writePE(Sensor_Tuning_Control_e_Flag_ConfigurationDone_Byte0, Flag_e_TRUE);

        return(0);
}


/***********************************************************************************/
int SWITCHSENSOR(void) {
/***********************************************************************************/
//New for 8500
int RunError;

   RunError = ITE_SendCommands(HostInterfaceCommand_e_SWITCH_SENSOR, ISP_HOST_COMMS_READY);

   /*
   return(ITE_SendCommands( HostInterfaceCommand_e_SWITCH_SENSOR,
          HostInterface_Status_e_HostInterfaceHighLevelState_Byte0,
          HostInterfaceHighLevelState_e_INIT,
      1000));    // timeout = 1s
    */

    return(RunError);
}

int SLEEPVPIP(void) 
{

    int RunError;
   union u_ITE_Event event;
   
   LOS_Log(">>  SLEEPVPIP");

     ITE_writePE(HostInterface_Control_e_HostInterfaceCommand_User_Byte0, HostInterfaceCommand_e_SLEEP);   

    if (0 != usecase.smiapp_powerUp_sequence)
    {
         ITE_Do_SmiaPP_PowerDown_Sequence();
    }

   do
   {
       event = ITE_WaitEvent(ITE_EVT_ISPCTL_EVENTS);
       if(ITE_Event_Log_flag == TRUE) ITE_traceISPCTLevent(event);
       if(event.type == ITE_EVT_ISPCTL_ERROR) break;
   }
   while( !((event.type == ITE_EVT_ISPCTL_INFO) && (event.ispctlInfo.info_id == ISP_SLEEPING)) );
   if (event.type == ITE_EVT_ISPCTL_ERROR)
        return(event.ispctlError.error_id);
   else
        return(0);

   LOS_Log("<<  SLEEPVPIP");
    return(RunError);
}

/***********************************************************************************/
int WAKEUPVPIP(void) {
/***********************************************************************************/
//Ported on 8500
int RunError;

   RunError = ITE_SendCommands(HostInterfaceCommand_e_WAKEUP, ISP_WOKEN_UP);

   /*
   return(ITE_SendCommands( HostInterfaceCommand_e_WAKEUP,
          HostInterface_Status_e_HostInterfaceHighLevelState_Byte0,
          HostInterfaceHighLevelState_e_STOPPED,
      1000));    // timeout = 1s
    */

    return(RunError);
}

extern t_uint32 ite_sensorselect;

/***********************************************************************************/
int STOPVPIP(void) {
/***********************************************************************************/
//Ported on 8500
//FIXME: do we stop ISP only or both sensor and ISP with command HostInterfaceCommand_e_STOP ???
int RunError;
    int dmaGrabAbortEventCount = 0;
    int abortRxOnStop = 0;

   //WARNING: should use HostInterfaceCommand_e_STOP and not HostInterfaceCommand_e_STOP_ISP as sensor is not stopped in this case !!!!!!!!
   //mle RunError = ITE_SendCommands(HostInterfaceCommand_e_STOP, ISP_STOP_STREAMING);
   #ifdef TEST_PERFORMANCE
   RECORD_SYSTEM_TIME("START", "Event0_Count_u16_EVENT0_6_ISP_STOP","ISP_STOP_STREAM",MAX_ISP_STOP_STREAM);
   RECORD_SYSTEM_TIME("START", "Event0_Count_u16_EVENT0_7_SENSOR_STOP","SENSOR_STOP_STREAM",MAX_SENSOR_STOP_STREAM);
   #endif
   ITE_RefreshEventCount(Event0_Count_u16_EVENT0_6_ISP_STOP_Byte0);
   ITE_RefreshEventCount(Event0_Count_u16_EVENT0_7_SENSOR_STOP_Byte0);
   
   abortRxOnStop = ITE_readPE(SystemSetup_e_Flag_abortRx_OnStop_Byte0);

   LOS_Log("STOPVPIP : abortRxOnStop = %u\n", abortRxOnStop);

   RunError = ITE_SendCommands(HostInterfaceCommand_e_STOP, ISP_SENSOR_STOP_STREAMING);

  ITE_GetEventCount(Event0_Count_u16_EVENT0_6_ISP_STOP_Byte0);
  ITE_GetEventCount(Event0_Count_u16_EVENT0_7_SENSOR_STOP_Byte0);
  
   // work around for 130.26.0 only for CCP
   //if(ite_sensorselect==1) IPP_R_W(SD_RESET,1);
   UNUSED(dmaGrabAbortEventCount);

   return(RunError);

   /*
   return(ITE_SendCommands( HostInterfaceCommand_e_STOP_ISP,
          HostInterface_Status_e_HostInterfaceHighLevelState_Byte0,
          HostInterfaceHighLevelState_e_STOPPED,
      1000));    // timeout = 1s
      */
}


/***********************************************************************************/
int STOPVPIP_BML(void) {
/***********************************************************************************/
//Ported on 8500
//FIXME: do we stop ISP only or both sensor and ISP with command HostInterfaceCommand_e_STOP ???
int RunError;

   RunError = ITE_SendCommands(HostInterfaceCommand_e_STOP, ISP_STOP_STREAMING);

   return(RunError);
}

/***********************************************************************************/
/* return 0 in case of successful restart of VPIP, otherwise return error code     */
int STARTVPIP(void) {
/***********************************************************************************/
//Ported on 8500
int RunError;
#ifdef TEST_PERFORMANCE
 RECORD_SYSTEM_TIME("START", "Event0_Count_u16_EVENT0_8_SENSOR_START","SENSOR_START_STREAM",MAX_ISP_STOP_STREAM);
 RECORD_SYSTEM_TIME("START", "Event0_Count_u16_EVENT0_5_ISP_STREAMING","ISP_START_STREAM",MAX_SENSOR_STOP_STREAM);
#endif
   ITE_CleanEvent(ITE_EVT_ISPCTL_ZOOM_EVENTS);
   ITE_CleanEvent(ITE_EVT_ISPCTL_STATS_EVENTS);
   ITE_CleanEvent(ITE_EVT_ISPCTL_FOCUS_EVENTS);
   
   ITE_RefreshEventCount(Event0_Count_u16_EVENT0_8_SENSOR_START_Byte0);
   ITE_RefreshEventCount(Event0_Count_u16_EVENT0_5_ISP_STREAMING_Byte0);   
   RunError = ITE_SendCommands(HostInterfaceCommand_e_RUN, ISP_STREAMING);

   ITE_GetEventCount(Event0_Count_u16_EVENT0_8_SENSOR_START_Byte0);
   ITE_GetEventCount(Event0_Count_u16_EVENT0_5_ISP_STREAMING_Byte0);
   /*
   RunError = ITE_SendCommands( HostInterfaceCommand_e_RUN,
                     HostInterface_Status_e_HostInterfaceHighLevelState_Byte0,
                     HostInterfaceHighLevelState_e_RUNNING,
             1000);  // timeout 1s
    */
   return(RunError);
}


/***********************************************************************************/
/* return 0 in case of successful restart of VPIP, otherwise return error code     */
int STARTVPIP_BML(void) {
/***********************************************************************************/

int RunError;

   RunError = ITE_SendCommands(HostInterfaceCommand_e_RUN, ISP_LOAD_READY);
   ITE_CleanEvent(ITE_EVT_ISPCTL_ZOOM_EVENTS);
   ITE_CleanEvent(ITE_EVT_ISPCTL_STATS_EVENTS);
   return(RunError);
}


/***********************************************************************************/
int STARTVPIPMULTIFRAME(enum e_grabPipeID pipe,int nb_frame){
/***********************************************************************************/
int RunError = 0;
UNUSED(pipe);
UNUSED(nb_frame);

/*
t_uint16 init_nb_frames;
t_uint16 pe_value;

#if(defined(__PEPS8500_SIA) || defined(_SVP_))
    RunError = ITE_SendCommands(HostInterfaceCommand_e_RUN, HostInterface_Status_e_HostInterfaceLowLevelState_Current_Byte0, HostInterfaceLowLevelState_e_RUNNING, 1000);   // timeout = 1s
    if(pipe == GRBPID_PIPE_HR)
    {
        init_nb_frames = ITE_readPE(PipeStatus_0_u8_FramesStreamedOutOfPipeLastRun_Byte0);
        do
        {
            pe_value = ITE_readPE(PipeStatus_0_u8_FramesStreamedOutOfPipeLastRun_Byte0);
        }while (pe_value < (init_nb_frames+nb_frame));
    }
    else
    {
        init_nb_frames = ITE_readPE(PipeStatus_1_u8_FramesStreamedOutOfPipeLastRun_Byte0);
        do
        {
            pe_value = ITE_readPE(PipeStatus_1_u8_FramesStreamedOutOfPipeLastRun_Byte0);
        }while (pe_value < (init_nb_frames+nb_frame));
    }
#else
   RunError = ITE_SendCommands(CMD_RUN,
                               HostInterfaceManagerStatus_bThisLoLevelState,
                               LOW_LEVEL_RUNNING,
                   1000);   // timeout = 1s

    if(pipe == GRBPID_PIPE_HR)
       {
       init_nb_frames = ITE_readPE(Pipe0Status_bNumberOfFramesStreamed);
         do
          {
          pe_value = ITE_readPE(Pipe0Status_bNumberOfFramesStreamed);
          } while (pe_value < (init_nb_frames+nb_frame));
       }
     else
      {
       init_nb_frames = ITE_readPE(Pipe1Status_bNumberOfFramesStreamed);
         do
          {
          pe_value = ITE_readPE(Pipe1Status_bNumberOfFramesStreamed);
          } while (pe_value < (init_nb_frames+nb_frame));
      }
#endif
*/
    return(RunError);
}


/***********************************************************************************/
int PREPAREVPIP(void){
/***********************************************************************************/
// "ported" on 8500 V1
//Deprecated as no more Prepare command at xP70 FW level
int Error = 0;
/*

   ITE_writePE(ModeSetupBank0_bActiveSensor, ITE_GetActiveSensor());
   Error = ITE_SendCommands(CMD_PREPARE,
                            ModeManagerStatus_bThisLoLevelState,    // Pool on
                            PREPARED,                   // until ==
                    300);                   // timeout = 300ms

   // Send Sensor Patch if Handling initSequence present in sensor.ini file
   ITE_writePE(HostToSensorAccessControl_bActiveSensor, ITE_GetActiveSensor());
   ITE_SendByteToSensorStr(SensorsInfo[(ITE_GetActiveSensor()-1)].handling.initSequence);
   // Patch for 558 rev5
   if ((SensorsInfo[(ITE_GetActiveSensor()-1)].revision.identity == 558) &
       (SensorsInfo[(ITE_GetActiveSensor()-1)].revision.number == 5)) {
       ITE_writePE(Sensor1Capabilities_uwSensorCoarseIntegrationTimeMaxMargin_MSByte, 0x8);
       ITE_writePE(Sensor1Capabilities_fpSensorAnalogGainConstM1_MSByte, 0xBE00);
       ITE_writePE(Sensor1Capabilities_uwSensorDataPedestal_MSByte, 0x4);
      }
*/
   return(Error);
}





//Ported on 8500 V1
int ITE_IsVPIPRunning(void) {
return (HostInterfaceHighLevelState_e_RUNNING == ITE_readPE(HostInterface_Status_e_HostInterfaceHighLevelState_Byte0));
}

//Ported on 8500 V1
int ITE_IsVPIPStopped(void) {
return (HostInterfaceHighLevelState_e_STOPPED == ITE_readPE(HostInterface_Status_e_HostInterfaceHighLevelState_Byte0));
}

