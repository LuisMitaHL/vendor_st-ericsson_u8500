/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/**
* \file cgps1lib.c
* \date 02/04/2008
* \version 1.0
*
* <B>Compiler:</B> ARM ADS\n
*
* <B>Description:</B>  This file contains all the callback functions needed
* to run the GPS library\n
*
* <TABLE>
*     <TR>
*             <TD> Date</TD><TD> Author</TD><TD> Description</TD>
*     </TR>
*     <TR>
*             <TD> ?????? </TD><TD> ???EX-GLONAV GUY?? </TD><TD> Creation </TD>
*     </TR>
*     <TR>
*             <TD> 25.01.08 </TD><TD> M.BELOU </TD><TD> Sysol adaptation </TD>
*     </TR>
*     <TR>
*             <TD> 02.04.08 </TD><TD> Y.DESAPHI </TD><TD> CGPS adaptation </TD>
*     </TR>
* </TABLE>
*/
#define __CGPS1LIB_C__

#undef  MODULE_NUMBER
#define MODULE_NUMBER   MODULE_CGPS

#undef  PROCESS_NUMBER
#define PROCESS_NUMBER  PROCESS_CGPS

#undef  FILE_NUMBER
#define FILE_NUMBER     1

#include "cgpslib.h"
#include "GN_EE_api.h"
#include "cgpsee.h"


#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 0
/**
* \fn U2 GN_GPS_Write_GNB_Patch( U2 ROM_version,U2 Patch_CkSum )
* \brief library Callback to upload patch
*
* GPS library Callback Function to request that the host start uploading a Patch
* File to the GN GPS Baseband for the given ROM code version. The host must
* ensure that if the data in this file is split amongst several writes then
* this is done at a \<CR\>\<LF\> boundary so as to avoid message corruption by the
* Ctrl Data also being sent to the Baseband.
*
* \param ROM_version Current GN Baseband ROM version
* \param Patch_CkSum Current GN Baseband Reported Patch
*
* \return Returns the Patch Checksum, or zero if there is no Patch.
*/

U2 GN_GPS_Write_GNB_Patch( U2 ROM_version, U2 Patch_CkSum )
{
    U2 ret_val=0;                      /*return value*/
    MC_CGPS_TRACE(("GN_GPS_Write_GNB_Patch : ROM_version <%u> Patch_CkSum = =0x%04X" , ROM_version , Patch_CkSum ));
    ret_val = CGPS2_01SetupMEPatch(ROM_version,Patch_CkSum );


    return( ret_val );
}


#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 2
/**
* \fn U2 GN_GPS_Read_PGNV(U2  max_bytes,CH *p_PGNV )
* \brief CPS library Callback to read $PGNV NMEA
*
* GPS library Callback Function to Read $PGNV NMEA Input
* Messages from the Host's chosen $PGNV communications interface.
* Internally the GPS Core library a circular buffer to store this data.
* Therefore, 'max_bytes' is dynamically set to prevent a single Read operation
* from straddling the internal circular buffer's end wrap point, or from
* over-writing data that has not yet been processed.
*
* \param max_bytes Maximum number of bytes to read
* \param p_PGNV  Pointer to the $PGNV data.
*
* \return Returns the number of bytes actually read.  If this is equal to 'max_bytes'
* then this callback function may be called again if 'max_bytes' was limited
* due to the circular buffer's end wrap point..
*/
U2 GN_GPS_Read_PGNV(U2  max_bytes,CH *p_PGNV )
{
    U2 bytes_read = 0;

    bytes_read = CGPS3_01demoGetReceivePacket((uint8_t *)p_PGNV,max_bytes);

    return( (U2)bytes_read );
}

#ifndef AGPS_UP_FTR
BL GN_SUPL_Connect_Req_Out(void* Handle, void* p_Status, void* p_SUPL_Connect_Req_Parms)
{
    return FALSE;
}
BL GN_SUPL_Disconnect_Req_Out(void* Handle, void* p_Status)
{
    return FALSE;
}
BL GN_SUPL_PDU_Delivery_Out(void* Handle, void* p_Status, U2 PDU_Size, U1* p_PDU)
{
    return FALSE;
}
BL GN_SUPL_Notification_Req_Out(void* Handle, void* p_Status, void* p_SUPL_Notification_Parms)
{
    return FALSE;
}
BL GN_SUPL_Position_Resp_Out(void* Handle, void* p_Status, void* p_GN_SUPL_Pos_Data)
{
    return FALSE;
}
#endif

#ifdef AGPS_UP_FTR

#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 3
/**
* \brief
*      SUPL outgoing connection request.
* \details
*      Requests a connection for SUPL to an external network entity.
*      <p> The Handle is set by the SUPL Handler or is set to Handle
*      provided by the #GN_SUPL_Push_Delivery_In() call.
*      <p> Following the #GN_SUPL_Connect_Req_Out() the SUPL subsystem waits
*      for a #GN_SUPL_Connect_Ind_In() when the connection to the SLP is
*      established.
*      <H3>p_Status values returned:</H3>
*      <ul>
*         <li><var>#GN_SUCCESS \copydoc GN_SUCCESS</var>\n
*            p_Status should be set to this value when the return value
*            indicated success.</li>
*         <li><var>#GN_ERR_NO_RESOURCE \copydoc GN_ERR_NO_RESOURCE</var>\n
*            p_Status should be set to this value when there are not
*            enough resources on the host to perform the requested function.</li>
*      </ul>
* \note
*      This function must be implemented by the Host software and will be
*      called by functions from within the GloNav Library.
* \attention
*      The caller of #GN_SUPL_Connect_Req_Out() must manage (e.g. alloc and free)
*      the memory referenced by p_Status and p_SUPL_Connect_Req_Parms.
* \param Handle [in] Opaque Handle used to coordinate requests.
* \param p_Status [out] Status of connection request and to be checked when return flag indicates failure.
* \param p_SUPL_Connect_Req_Parms [in] Details of connection
* \returns
*      Flag to indicate success or failure of the posting of the request.
* \retval #TRUE Flag indicating success, *p_Status will be set to GN_SUCCESS.
* \retval #FALSE Flag indicating failure and that the status should be checked.
*/
BL GN_SUPL_Connect_Req_Out(void* Handle, e_GN_Status* p_Status, s_GN_SUPL_Connect_Req_Parms* p_SUPL_Connect_Req_Parms)
{
    int8_t vl_index = 0;
    BL vl_return = FALSE;

    MC_CGPS_TRACE(("GN_SUPL_Connect_Req_Out : GPSHandle=0x%x", Handle));

    *p_Status = GN_SUCCESS;

    /* search for the CgpsSupl index dedicated to the GPSHandle */
    for (vl_index = 0; vl_index < K_CGPS_MAX_NUMBER_OF_SUPL && s_CgpsSupl[vl_index].v_GPSHandle != Handle; vl_index++);

    /* if the handler hasn't been found */
    if (vl_index ==  K_CGPS_MAX_NUMBER_OF_SUPL)
    {
        MC_CGPS_TRACE(("GN_SUPL_Connect_Req_Out : Handle not found!!!"));
        *p_Status = GN_ERR_HANDLE_INVALID;
    }
    else
    {

        /* store connection parameters into SUPL application context */
        s_CgpsSupl[vl_index].v_ConnectionParm.TcpIp_AddressType = p_SUPL_Connect_Req_Parms->TcpIp_AddressType;
        s_CgpsSupl[vl_index].v_ConnectionParm.p_TcpIp_Address = (char*)MC_RTK_GET_MEMORY(sizeof(char) * (strlen(p_SUPL_Connect_Req_Parms->p_TcpIp_Address)+1));
        strncpy(s_CgpsSupl[vl_index].v_ConnectionParm.p_TcpIp_Address, p_SUPL_Connect_Req_Parms->p_TcpIp_Address, strlen(p_SUPL_Connect_Req_Parms->p_TcpIp_Address)+1);
        s_CgpsSupl[vl_index].v_ConnectionParm.Port = p_SUPL_Connect_Req_Parms->Port;

        if (s_CgpsSupl[vl_index].v_Suspended==CGPS_SESSION_NOT_SUSPENDED)
        {
            /* FIXME */
            /* if ( */
            CGPS5_18ConnectSUPLSession(vl_index);/* )
                   { */
            vl_return = TRUE;
            /* }
            else
            {
                MC_CGPS_TRACE(("GN_SUPL_Connect_Req_Out : connection fail!!!"));
                *p_Status = GN_ERR_NO_RESOURCE;
            } */
            /* FIXME */
        }
        else
        {
            MC_CGPS_TRACE(("GN_SUPL_Connect_Req_Out : SUPL suspended!!!"));
            *p_Status = GN_ERR_CONN_REJECTED;
        }
    }

    return vl_return;
}

#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 4
/**
* \brief
*      SUPL disconnection request.
* \details
*      Requests a disconnection for an existing SUPL connection.
*      <p> The Handle is set to the Handle from the
*      #GN_SUPL_Connect_Req_Out() call.
*      <H3>p_Status values passed in:</H3>
*      <ul>
*         <li><var>#GN_SUCCESS \copydoc GN_SUCCESS</var>\n
*            p_Status should be set to this value when the #GN_SUPL_Disconnect_Req_Out()
*            was successful.</li>
*         <li><var>#GN_ERR_CONN_SHUTDOWN \copydoc GN_ERR_CONN_SHUTDOWN</var>\n
*            p_Status should be set to this value when the connection
*            associated with the Handle was spontaneously shut down.</li>
*      </ul>
*      <H3>p_Status values returned:</H3>
*      <ul>
*         <li><var>#GN_SUCCESS \copydoc GN_SUCCESS</var>\n
*            p_Status should be set to this value when the return value
*            indicated success.</li>
*         <li><var>#GN_ERR_HANDLE_INVALID \copydoc GN_ERR_HANDLE_INVALID</var>\n
*            p_Status should be set to this value when the Handle
*            passed was not recognised by the host software.</li>
*         <li><var>#GN_ERR_NO_RESOURCE \copydoc GN_ERR_NO_RESOURCE</var>\n
*            p_Status should be set to this value when there are not
*            enough resources on the host to perform the requested function.</li>
*         <li><var>#GN_ERR_CONN_SHUTDOWN \copydoc GN_ERR_CONN_SHUTDOWN</var>\n
*            p_Status should be set to this value when the connection
*            associated with the handle is no longer available.</li>
*      </ul>
* \note
*      This function must be implemented by the Host software and will be
*      called by functions from within the GloNav Library.
* \attention
*      The caller of #GN_SUPL_Disconnect_Req_Out() must manage (e.g. alloc and free)
*      the memory referenced by p_Status.
* \param Handle [in] Opaque Handle used to coordinate requests.
* \param p_Status [in/out] Status of disconnect and to be checked when return flag indicates failure.
* \returns
*      Flag to indicate success or failure of the posting of the request.
* \retval #TRUE Flag indicating success, *p_Status will be set to GN_SUCCESS.
* \retval #FALSE Flag indicating failure and that the status should be checked.
*/
BL GN_SUPL_Disconnect_Req_Out(void* Handle, e_GN_Status* p_Status)
{
    int8_t vl_index = 0;
    t_lsimup_TcpIpDisconnectReqM * pl_lsimup_TcpIpDisconnectReq=NULL;
    BL vl_return = FALSE;

    MC_CGPS_TRACE(("GN_SUPL_Disconnect_Req_Out : GPSHandle=0x%x", Handle));

    /* search for the dedicated context */
    for (vl_index = 0; vl_index < K_CGPS_MAX_NUMBER_OF_SUPL && s_CgpsSupl[vl_index].v_GPSHandle != Handle; vl_index++);

    if (vl_index == K_CGPS_MAX_NUMBER_OF_SUPL)
    {
        MC_CGPS_TRACE(("GN_SUPL_Disconnect_Req_Out : Handle not found!!!"));
        *p_Status = GN_ERR_HANDLE_INVALID;
    }
    else
    {
        if (s_CgpsSupl[vl_index].v_Suspended == 0)
        {
/*+LMSqc38060*/
           if(s_CgpsSupl[vl_index].v_ConnectionHandle == 0)
           {
               e_GN_Status vl_Status = GN_SUCCESS;
              /*NO connection for the supl, delete context*/
/*+LMSqc38060_2*/
               GN_SUPL_Disconnect_Ind_In(s_CgpsSupl[vl_index].v_GPSHandle, &vl_Status);
/*-LMSqc38060_2*/
                CGPS4_13DeleteSuplContext(&s_CgpsSupl[vl_index]);
           }
           else
            {
/*-LMSqc38060*/
            pl_lsimup_TcpIpDisconnectReq = (t_lsimup_TcpIpDisconnectReqM*)MC_RTK_GET_MEMORY(sizeof(*pl_lsimup_TcpIpDisconnectReq));
            pl_lsimup_TcpIpDisconnectReq->v_SessionHandle = vg_CGPS_LSIMUP_Session_Handle;
            pl_lsimup_TcpIpDisconnectReq->v_ConnectionHandle = s_CgpsSupl[vl_index].v_ConnectionHandle;

             CGPS4_28SendMsgToProcess(PROCESS_CGPS,
                         PROCESS_LSIMUP,
                                       CGPS_LSIMUP_TCPIP_DISCONNECT_REQ,
                                       (t_MsgHeader *)pl_lsimup_TcpIpDisconnectReq);
            MC_CGPS_TRACE(("GN_SUPL_Disconnect_Req_Out : TCPIP disconnection request sent to LSIMUP for connectionHandle=0x%x", s_CgpsSupl[vl_index].v_ConnectionHandle));
            *p_Status = GN_SUCCESS;
            vl_return= TRUE;
/*+LMSqc38060*/
/*+LMSqc38060_2*/
                s_CgpsSupl[vl_index].v_Suspended = CGPS_SESSION_SUSPENDED;
/*-LMSqc38060_2*/
                         }
/*-LMSqc38060*/

        }
        else
        {
            MC_CGPS_TRACE(("GN_SUPL_Disconnect_Req_Out : SUPL suspended!!!"));
            *p_Status = GN_ERR_CONN_REJECTED;
        }
    }

    return vl_return;
}

#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 5
/**
* \brief
*      SUPL PDU delivery out.
* \details
*      Delivers a SUPL PDU to an existing network connection.
*      <p> The Handle is set to the Handle from the
*      #GN_SUPL_Connect_Req_Out() call.
*      <H3>p_Status values returned:</H3>
*      <ul>
*         <li><var>#GN_SUCCESS \copydoc GN_SUCCESS</var>\n
*            p_Status should be set to this value when the return value
*            indicated success.</li>
*         <li><var>#GN_ERR_HANDLE_INVALID \copydoc GN_ERR_HANDLE_INVALID</var>\n
*            p_Status should be set to this value when the Handle
*            passed was not recognised by the host software.</li>
*         <li><var>#GN_ERR_POINTER_INVALID \copydoc GN_ERR_POINTER_INVALID</var>\n
*            p_Status should be set to this value when the p_PDU is NULL.</li>
*         <li><var>#GN_ERR_PARAMETER_INVALID \copydoc GN_ERR_PARAMETER_INVALID</var>\n
*            p_Status should be set to this value when the PDU_Size is 0.</li>
*         <li><var>#GN_ERR_NO_RESOURCE \copydoc GN_ERR_NO_RESOURCE</var>\n
*            p_Status should be set to this value when there are not
*            enough resources on the host to perform the requested function.</li>
*         <li><var>#GN_ERR_CONN_SHUTDOWN \copydoc GN_ERR_CONN_SHUTDOWN</var>\n
*            p_Status should be set to this value when the connection
*            associated with the handle is no longer available.</li>
*      </ul>
* \note
*      This function must be implemented by the Host software and will be
*      called by functions from within the GloNav Library.
* \attention
*      The caller of #GN_SUPL_PDU_Delivery_Out() must manage (e.g. alloc and free)
*      the memory referenced by p_Status and p_PDU.
* \param Handle [in] Opaque Handle used to coordinate requests.
* \param p_Status [out] Status of PDU delivery and to be checked when return flag indicates failure.
* \param PDU_Size [in] Size of data at p_PDU in bytes.
* \param [in] Pointer to data.
* \returns
*      Flag to indicate success or failure of the posting of the request.
* \retval #TRUE Flag indicating success, *p_Status will be set to GN_SUCCESS.
* \retval #FALSE Flag indicating failure and that the status should be checked.
*/
BL GN_SUPL_PDU_Delivery_Out(void* Handle, e_GN_Status* p_Status, U2 PDU_Size, U1* p_PDU)
{
    t_lsimup_SendDataReqM * pl_lsimup_SendDataReq=NULL;
    int8_t vl_index = 0;
    uint8_t vl_return=FALSE;

    MC_CGPS_TRACE(("GN_SUPL_PDU_Delivery_Out : GPSHandle=0x%x", Handle));

    /* search for the context */
    for (vl_index = 0; vl_index < K_CGPS_MAX_NUMBER_OF_SUPL && s_CgpsSupl[vl_index].v_GPSHandle != Handle; vl_index++);

    /* if the handle hasn't been found */
    if (vl_index == K_CGPS_MAX_NUMBER_OF_SUPL)
    {
        MC_CGPS_TRACE(("GN_SUPL_PDU_Delivery_Out : Handle not found!!!"));
        *p_Status = GN_ERR_HANDLE_INVALID;
    }
    else
    {
        if (s_CgpsSupl[vl_index].v_Suspended == 0)
        {
            pl_lsimup_SendDataReq = (t_lsimup_SendDataReqM*)MC_RTK_GET_MEMORY(sizeof(*pl_lsimup_SendDataReq));
            pl_lsimup_SendDataReq->v_SessionHandle = vg_CGPS_LSIMUP_Session_Handle;
            pl_lsimup_SendDataReq->v_ConnectionHandle = s_CgpsSupl[vl_index].v_ConnectionHandle;

            pl_lsimup_SendDataReq->v_DataLength = (uint32_t)PDU_Size;
            pl_lsimup_SendDataReq->v_DataPointer = (uint8_t*)MC_RTK_GET_MEMORY(PDU_Size);
            memcpy(pl_lsimup_SendDataReq->v_DataPointer, p_PDU, PDU_Size);

             CGPS4_28SendMsgToProcess(PROCESS_CGPS,
                                       PROCESS_LSIMUP,
                                       CGPS_LSIMUP_SEND_DATA_REQ,
                                       (t_MsgHeader *)pl_lsimup_SendDataReq);
            MC_CGPS_TRACE(("GN_SUPL_PDU_Delivery_Out : Send Data Request successfully Sent"));
            *p_Status = GN_SUCCESS;
            vl_return = TRUE;
        }
        else
        {
            MC_CGPS_TRACE(("GN_SUPL_PDU_Delivery_Out : SUPL suspended!!!"));
            *p_Status = GN_ERR_CONN_REJECTED;
        }
    }

    return vl_return;
}

#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 6
/**
* \brief
*      SUPL position response out.
* \details
*      Provides the Position calculated by the SUPL subsystem to the host.
*      <p> The Handle is set to the Handle from the
*      #GN_SUPL_Push_Delivery_In() or a #GN_SUPL_Position_Req_In().
*      <H3>p_Status values returned:</H3>
*      <ul>
*         <li><var>#GN_SUCCESS \copydoc GN_SUCCESS</var>\n
*            p_Status should be set to this value when the return value
*            indicated success.</li>
*         <li><var>#GN_ERR_HANDLE_INVALID \copydoc GN_ERR_HANDLE_INVALID</var>\n
*            p_Status should be set to this value when the Handle
*            passed was not recognised by the host software.</li>
*         <li><var>#GN_ERR_POINTER_INVALID \copydoc GN_ERR_POINTER_INVALID</var>\n
*            p_Status should be set to this value when the p_GN_AGPS_GAD_Data
*            is NULL.</li>
*         <li><var>#GN_ERR_NO_RESOURCE \copydoc GN_ERR_NO_RESOURCE</var>\n
*            p_Status should be set to this value when there are not
*            enough resources on the host to perform the requested function.</li>
*      </ul>
* \note
*      This function must be implemented by the Host software and will be
*      called by functions from within the GloNav Library.
* \attention
*      The caller of #GN_SUPL_Position_Resp_Out() must manage (e.g. alloc and free)
*      the memory referenced by p_Status and p_GN_AGPS_GAD_Data.
* \param Handle [in] Opaque Handle used to coordinate requests.
* \param p_Status [out] Status of disconnect and to be checked when return flag indicates failure.
* \param p_GN_AGPS_GAD_Data [in] Data structure populated containing the position solution.
* \returns
*      Flag to indicate success or failure of the posting of the request.
* \retval #TRUE Flag indicating success, *p_Status will be set to GN_SUCCESS.
* \retval #FALSE Flag indicating failure and that the status should be checked.
*/
BL GN_SUPL_Position_Resp_Out(void* Handle, e_GN_Status* p_Status, s_GN_SUPL_Pos_Data* p_GN_SUPL_Pos_Data)
{
    int8_t vl_index = 0;

    MC_CGPS_TRACE(("GN_SUPL_Position_Resp_Out : GPSHandle=0x%x", Handle));

    *p_Status = GN_SUCCESS;

    /* search for the CgpsSupl index dedicated to the GPSHandle */
    for (vl_index = 0; vl_index < K_CGPS_MAX_NUMBER_OF_SUPL && s_CgpsSupl[vl_index].v_GPSHandle != Handle; vl_index++);

    /* if the handler hasn't been found */
    if (vl_index ==  K_CGPS_MAX_NUMBER_OF_SUPL)
    {
        MC_CGPS_TRACE(("GN_SUPL_Position_Resp_Out : Handle not found!!!"));
        *p_Status = GN_ERR_HANDLE_INVALID;
        return FALSE;
    }
    else
    {
        if (p_GN_SUPL_Pos_Data == NULL)
        {
            MC_CGPS_TRACE(("GN_SUPL_Position_Resp_Out : Position data structure NULL!!!"));
            *p_Status = GN_ERR_POINTER_INVALID;
            return FALSE;
        }
        else
        {
            vg_Nav_Data.Valid_SingleFix = TRUE;
            vg_Nav_Data.Valid_SingleFix = FALSE; //changed to TRUE later

            vg_Nav_Data.OS_Time_ms = GN_GPS_Get_OS_Time_ms();

            vg_Nav_Data.Year = p_GN_SUPL_Pos_Data->Year;
            vg_Nav_Data.Month = p_GN_SUPL_Pos_Data->Month;
            vg_Nav_Data.Day = p_GN_SUPL_Pos_Data->Day;
            vg_Nav_Data.Hours = p_GN_SUPL_Pos_Data->Hours;
            vg_Nav_Data.Minutes = p_GN_SUPL_Pos_Data->Minutes;
            vg_Nav_Data.Seconds = p_GN_SUPL_Pos_Data->Seconds;
            vg_Nav_Data.Milliseconds = p_GN_SUPL_Pos_Data->Milliseconds;

            vg_Nav_Data.Latitude = p_GN_SUPL_Pos_Data->Latitude;
            vg_Nav_Data.Longitude = p_GN_SUPL_Pos_Data->Longitude;
            if (p_GN_SUPL_Pos_Data->AltitudeInfo_Valid)
            {
                vg_Nav_Data.Altitude_Ell = p_GN_SUPL_Pos_Data->Altitude;
                vg_Nav_Data.V_AccEst = p_GN_SUPL_Pos_Data->V_AccEst;
                vg_Nav_Data.Valid_SingleFix = TRUE;
            }

            if (p_GN_SUPL_Pos_Data->HorizontalVel_Valid)
            {
                vg_Nav_Data.SpeedOverGround = p_GN_SUPL_Pos_Data->HorizontalVel;
                vg_Nav_Data.CourseOverGround = p_GN_SUPL_Pos_Data->Bearing;
            }

            if (p_GN_SUPL_Pos_Data->VerticalVel_Valid)
                vg_Nav_Data.VerticalVelocity = p_GN_SUPL_Pos_Data->VerticalVel;

            if (p_GN_SUPL_Pos_Data->H_Acc_Valid)
            {
                vg_Nav_Data.H_AccMaj = p_GN_SUPL_Pos_Data->H_AccMaj;
                vg_Nav_Data.H_AccMin = p_GN_SUPL_Pos_Data->H_AccMin;
                vg_Nav_Data.H_AccMajBrg = p_GN_SUPL_Pos_Data->H_AccMajBrg;
            }
/* ++ LMSQB95465 ANP 15/07/2009 */
        else
        {
                vg_Nav_Data.H_AccMaj = 0;
                vg_Nav_Data.H_AccMin = 0;
                vg_Nav_Data.H_AccMajBrg = 0;
        }
/* -- LMSQB95465 ANP 15/07/2009 */

            if (p_GN_SUPL_Pos_Data->HVel_AccEst_Valid)
                vg_Nav_Data.HVel_AccEst = p_GN_SUPL_Pos_Data->HVel_AccEst;

            if (p_GN_SUPL_Pos_Data->VVel_AccEst_Valid)
                vg_Nav_Data.VVel_AccEst = p_GN_SUPL_Pos_Data->VVel_AccEst;

            vg_Latest_Fix_Nav_Data = vg_Nav_Data;

            /*we have a fix but don't reach the quality*/
//Commented for TTFF issue (TTFF prints two times for SI)
            //CGPS9_04LogPosition(vl_index );

            CGPS0_16SendDataToNaf();
        }
    }

    return TRUE;
}

#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 15
/**
* \brief
*      SUPL position response out.
* \details
*      Provides the Position calculated by the SUPL subsystem to the host.
*      <p> The Handle is set to the Handle from the
*      #GN_SUPL_Push_Delivery_In() or a #GN_SUPL_Position_Req_In().
*      <H3>p_Status values returned:</H3>
*      <ul>
*         <li><var>#GN_SUCCESS \copydoc GN_SUCCESS</var>\n
*            p_Status should be set to this value when the return value
*            indicated success.</li>
*         <li><var>#GN_ERR_HANDLE_INVALID \copydoc GN_ERR_HANDLE_INVALID</var>\n
*            p_Status should be set to this value when the Handle
*            passed was not recognised by the host software.</li>
*         <li><var>#GN_ERR_POINTER_INVALID \copydoc GN_ERR_POINTER_INVALID</var>\n
*            p_Status should be set to this value when the p_GN_AGPS_GAD_Data
*            is NULL.</li>
*         <li><var>#GN_ERR_NO_RESOURCE \copydoc GN_ERR_NO_RESOURCE</var>\n
*            p_Status should be set to this value when there are not
*            enough resources on the host to perform the requested function.</li>
*      </ul>
* \note
*      This function must be implemented by the Host software and will be
*      called by functions from within the GloNav Library.
* \attention
*      The caller of #GN_SUPL_Position_Resp_Out() must manage (e.g. alloc and free)
*      the memory referenced by p_Status and p_GN_AGPS_GAD_Data.
* \param Handle [in] Opaque Handle used to coordinate requests.
* \param p_Status [out] Status of disconnect and to be checked when return flag indicates failure.
* \param p_GN_AGPS_GAD_Data [in] Data structure populated containing the position solution.
* \returns
*      Flag to indicate success or failure of the posting of the request.
* \retval #TRUE Flag indicating success, *p_Status will be set to GN_SUCCESS.
* \retval #FALSE Flag indicating failure and that the status should be checked.
*/
BL GN_SUPL_ThirdPartyPosition_Resp_Out(void* Handle, e_GN_Status* p_Status, s_GN_SUPL_Pos_Data* p_GN_SUPL_Pos_Data)
{
    int8_t vl_index = 0;
    s_GN_GPS_Nav_Data * pl_nav_data_to_use;
    int8_t vl_CgpsIndex = 0;


    MC_CGPS_TRACE(("GN_SUPL_ThirdPartyPosition_Resp_Out : GPSHandle=0x%x", Handle));

    /* search for the CgpsSupl index dedicated to the GPSHandle */
    for (vl_index = 0; vl_index < K_CGPS_MAX_NUMBER_OF_SUPL && s_CgpsSupl[vl_index].v_GPSHandle != Handle; vl_index++);

    /* if the handler hasn't been found */
    if (vl_index ==  K_CGPS_MAX_NUMBER_OF_SUPL)
    {
        MC_CGPS_TRACE(("GN_SUPL_ThirdPartyPosition_Resp_Out : Handle not found!!!"));
        *p_Status = GN_ERR_HANDLE_INVALID;
        return FALSE;
    }

    for(vl_CgpsIndex = 0 ; vl_CgpsIndex < K_CGPS_MAX_NUMBER_OF_NAF; vl_CgpsIndex++)
    {
        /*check if the application is ready to receive data and if it has already received it*/
        if(   ( s_CgpsNaf[vl_CgpsIndex].v_State                         == K_CGPS_NAF_READY )
           && ( s_CgpsNaf[vl_CgpsIndex].p_ThirdPartyPositionCallback    != NULL  )
           && ( s_CgpsNaf[vl_CgpsIndex].v_Suspended                     == 0                )
           && ( s_CgpsNaf[vl_CgpsIndex].v_Config                        == K_CGPS_NAF_SESSION_CONFIG_NETWORK_BASED )
          )
        break;
    }
    /* if the handler hasn't been found */
    if (vl_CgpsIndex ==  K_CGPS_MAX_NUMBER_OF_NAF)
    {
        MC_CGPS_TRACE(("GN_SUPL_ThirdPartyPosition_Resp_Out : CGPS Handle not found!!!"));
        *p_Status = GN_ERR_HANDLE_INVALID;
        return FALSE;
    }

    else
    {
        if ( *p_Status == GN_SUCCESS)
        {
            if (p_GN_SUPL_Pos_Data == NULL)
            {
                MC_CGPS_TRACE(("GN_SUPL_ThirdPartyPosition_Resp_Out : Position data structure NULL!!!"));
                *p_Status = GN_ERR_POINTER_INVALID;
                return FALSE;
            }
            else
            {
                pl_nav_data_to_use = (s_GN_GPS_Nav_Data*)MC_RTK_GET_MEMORY(sizeof(s_GN_GPS_Nav_Data));

                pl_nav_data_to_use->Valid_SingleFix = TRUE;

                pl_nav_data_to_use->OS_Time_ms = GN_GPS_Get_OS_Time_ms();

                pl_nav_data_to_use->Year = p_GN_SUPL_Pos_Data->Year;
                pl_nav_data_to_use->Month = p_GN_SUPL_Pos_Data->Month;
                pl_nav_data_to_use->Day = p_GN_SUPL_Pos_Data->Day;
                pl_nav_data_to_use->Hours = p_GN_SUPL_Pos_Data->Hours;
                pl_nav_data_to_use->Minutes = p_GN_SUPL_Pos_Data->Minutes;
                pl_nav_data_to_use->Seconds = p_GN_SUPL_Pos_Data->Seconds;
                pl_nav_data_to_use->Milliseconds = p_GN_SUPL_Pos_Data->Milliseconds;

                pl_nav_data_to_use->Latitude = p_GN_SUPL_Pos_Data->Latitude;
                pl_nav_data_to_use->Longitude = p_GN_SUPL_Pos_Data->Longitude;

                if (p_GN_SUPL_Pos_Data->AltitudeInfo_Valid)
                {
                    pl_nav_data_to_use->Altitude_Ell = p_GN_SUPL_Pos_Data->Altitude;
                    pl_nav_data_to_use->V_AccEst = p_GN_SUPL_Pos_Data->V_AccEst;
                    pl_nav_data_to_use->Valid_SingleFix = TRUE;

                }

                if (p_GN_SUPL_Pos_Data->HorizontalVel_Valid)
                {

                    pl_nav_data_to_use->SpeedOverGround = p_GN_SUPL_Pos_Data->HorizontalVel;
                    pl_nav_data_to_use->CourseOverGround = p_GN_SUPL_Pos_Data->Bearing;
                }

                if (p_GN_SUPL_Pos_Data->VerticalVel_Valid)
                    pl_nav_data_to_use->VerticalVelocity = p_GN_SUPL_Pos_Data->VerticalVel;

                if (p_GN_SUPL_Pos_Data->H_Acc_Valid)
                {

                    pl_nav_data_to_use->H_AccMaj = p_GN_SUPL_Pos_Data->H_AccMaj;
                    pl_nav_data_to_use->H_AccMin = p_GN_SUPL_Pos_Data->H_AccMin;
                    pl_nav_data_to_use->H_AccMajBrg = p_GN_SUPL_Pos_Data->H_AccMajBrg;
                }

                if (p_GN_SUPL_Pos_Data->HVel_AccEst_Valid)
                    pl_nav_data_to_use->HVel_AccEst = p_GN_SUPL_Pos_Data->HVel_AccEst;

                if (p_GN_SUPL_Pos_Data->VVel_AccEst_Valid)
                    pl_nav_data_to_use->VVel_AccEst = p_GN_SUPL_Pos_Data->VVel_AccEst;


                if(    (s_CgpsNaf[vl_CgpsIndex].v_FirstFixDone == FALSE )
                  && (    pl_nav_data_to_use->Valid_SingleFix )
                 )
                {
                      t_cgps_NavData vl_NavDataToSend;

                      CGPS0_36GetNafData(pl_nav_data_to_use,
                                         &vl_NavDataToSend,
                                         (e_cgps_NavDataType)s_CgpsNaf[vl_CgpsIndex].v_OutputType,
                                         K_CGPS_C_STRUCT );


                      (s_CgpsNaf[vl_CgpsIndex].p_ThirdPartyPositionCallback)( vl_NavDataToSend );
                      MC_RTK_FREE_MEMORY( vl_NavDataToSend.p_NavData );
                      vl_NavDataToSend.p_NavData = NULL;
                }

                MC_RTK_FREE_MEMORY( pl_nav_data_to_use );
                pl_nav_data_to_use = NULL;


            }
        }

        s_CgpsNaf[vl_CgpsIndex].v_State                      = K_CGPS_NAF_REGISTERED;
        s_CgpsNaf[vl_CgpsIndex].p_ThirdPartyPositionCallback = NULL;
#ifdef CMCC_LOGGING_ENABLE
        s_CgpsNaf[vl_CgpsIndex].v_AlreadyUpdated             = FALSE;
#endif /*CMCC_LOGGING_ENABLE*/
    }

    return TRUE;
}
#endif /* AGPS_UP_FTR */






#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 7
/**
* \brief
*      Calloc definition for GloNav library use.
* \details
*      Platform implementation of the calloc function returning a pointer
*      to a block of memory of Number * Size number of bytes.
*      <p> If either of the parameters is 0 #GN_Calloc() will return #NULL.
* \param Number_Of_Elements [in] Number of elements to allocate.
* \param Size_Of_Elements [in] Size of elements to allocate.
* \returns
*      Pointer to memory or NULL.
* \retval
*      #NULL indicates an error occured while allocating memory.
* \retval
*      Non_Null values are pointers to a block of successfully allocated memory.
*      <p> The memory can subsequently be deallocated using GN_Free().
*/
void * GN_Calloc(U2 Number_Of_Elements, U2 Size_Of_Elements)
{
    void * pl_allocatedPointer= NULL;


    if (  (Number_Of_Elements * Size_Of_Elements) !=0)
    {
        pl_allocatedPointer = MC_RTK_GET_MEMORY(Number_Of_Elements * Size_Of_Elements);
        memset(pl_allocatedPointer,0,Number_Of_Elements * Size_Of_Elements);
    }

    /* MC_CGPS_TRACE(("GN_Calloc : address[0x%x], nb=%d, size=%d, total=%d",
        pl_allocatedPointer, Number_Of_Elements, Size_Of_Elements, Number_Of_Elements*Size_Of_Elements));*/

    return pl_allocatedPointer;
}

#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 8
/**
* \brief
*      Free definition for GloNav library use.
* \details
*      Platform implementation of the free function taking a pointer
*      to a block of memory to deallocate.
*      <p> The memory to deallocate will have been allocated by the GN_Calloc()
*      routine.
* \param p_mem [in] Pointer to the memory to deallocate.
*/
void GN_Free(void * p_mem)
{
    /*MC_CGPS_TRACE(("GN_Free : address[0x%x]", p_mem));*/

    if (p_mem != NULL)
        MC_RTK_FREE_MEMORY(p_mem);
}

#ifdef AGPS_UP_FTR

#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 9
/**
* \brief
*      SUPL notification request out.
* \details
*      Indicates the handset user should be notified of a location request.
*      Depending on the parameters in the SUPL exchange the user may be
*      given the option to reject the request.
*      <p> The Handle is set to the Handle from the
*      #GN_SUPL_Push_Delivery_In() call as a notification will only originate from
*      a SUPL-INIT.
*      <p> The host will respond with #GN_SUPL_Notification_Rsp_In() indicating
*      whether the positioning sequence may proceed. If no response is available
*      from the user the Host will respond after a suitable time has elapsed.
*      <H3>p_Status values returned:</H3>
*      <ul>
*         <li><var>#GN_SUCCESS \copydoc GN_SUCCESS</var>\n
*            p_Status should be set to this value when the return value
*            indicated success.</li>
*         <li><var>#GN_ERR_HANDLE_INVALID \copydoc GN_ERR_HANDLE_INVALID</var>\n
*            p_Status should be set to this value when the Handle
*            passed was not recognised by the host software.</li>
*         <li><var>#GN_ERR_POINTER_INVALID \copydoc GN_ERR_POINTER_INVALID</var>\n
*            p_Status should be set to this value when the p_SUPL_Notification_Parms
*            is NULL.</li>
*         <li><var>#GN_ERR_PARAMETER_INVALID \copydoc GN_ERR_PARAMETER_INVALID</var>\n
*            p_Status should be set to this value when the PDU_Size is 0.</li>
*         <li><var>#GN_ERR_NO_RESOURCE \copydoc GN_ERR_NO_RESOURCE</var>\n
*            p_Status should be set to this value when there are not
*            enough resources on the host to perform the requested function.</li>
*      </ul>
* \note
*      This function must be implemented by the Host software and will be
*      called by functions from within the GloNav Library.
* \attention
*      The caller of #GN_SUPL_Notification_Req_Out() must manage (e.g. alloc and free)
*      the memory referenced by p_Status and p_SUPL_Notification_Parms.
* \param Handle [in] Opaque Handle used to coordinate requests.
* \param p_Status [out] Status of disconnect and to be checked when return flag indicates failure.
* \param p_SUPL_Notification_Parms [in] Pointer to a set of notification parameters.
* \returns
*      Flag to indicate success or failure of the posting of the request.
* \retval #TRUE Flag indicating success, *p_Status will be set to GN_SUCCESS.
* \retval #FALSE Flag indicating failure and that the status should be checked.
*/
BL GN_SUPL_Notification_Req_Out(void* Handle, e_GN_Status* p_Status, s_GN_SUPL_Notification_Parms* p_SUPL_Notification_Parms)
{
    int8_t vl_index;

    uint8_t vl_RequestorIdLen;
    uint8_t vl_ClientNameLen;
    BL vl_return= FALSE;

    MC_CGPS_TRACE(("GN_SUPL_Notification_Req_Out : start, Handle=0x%X", Handle));

    /* check if the GPS handle exists */
    for (vl_index = 0; vl_index < K_CGPS_MAX_NUMBER_OF_SUPL && s_CgpsSupl[vl_index].v_GPSHandle != Handle; vl_index++);

    if (vl_index == K_CGPS_MAX_NUMBER_OF_SUPL)
    {
        MC_CGPS_TRACE(("GN_SUPL_Notification_Req_Out : Handle not found!!!"));
        *p_Status = GN_ERR_HANDLE_INVALID;
    }
    else
    {
        if (s_CgpsSupl[vl_index].v_Suspended == 0)
        {
            /* check parameters */
            if (p_SUPL_Notification_Parms == NULL)
            {
                MC_CGPS_TRACE(("GN_SUPL_Notification_Req_Out : No notification parameter!!!"));
                *p_Status = GN_ERR_POINTER_INVALID;
            }
            else
            {
                MC_CGPS_TRACE(("GN_SUPL_Notification_Req_Out : Notification type = %d", p_SUPL_Notification_Parms->GN_SUPL_Notify_Type));

                switch (p_SUPL_Notification_Parms->GN_SUPL_Encoding_Type)
                {
                    case ENCODING_ucs2:
                    {
                        if( p_SUPL_Notification_Parms->RequestorIdLen >= 2 )
                        {
                            vl_RequestorIdLen = p_SUPL_Notification_Parms->RequestorIdLen <= K_CGPS_MAX_REQ_LENGTH ? (p_SUPL_Notification_Parms->RequestorIdLen - 2) : K_CGPS_MAX_REQ_LENGTH;
                        }
                        else
                        {
                            vl_RequestorIdLen = 0;
                        }

                        if( p_SUPL_Notification_Parms->ClientNameLen >= 2 )
                        {
                            vl_ClientNameLen = p_SUPL_Notification_Parms->ClientNameLen <= K_CGPS_MAX_CLIENT_LENGTH ? (p_SUPL_Notification_Parms->ClientNameLen - 2) : K_CGPS_MAX_CLIENT_LENGTH;
                        }
                        else
                        {
                            vl_ClientNameLen = 0;
                        }

                    }
                    break;
                    case ENCODING_gsmDefault:
                    case ENCODING_utf8:
                    case ENCODING_NONE:
                    default:
                    {
                        vl_RequestorIdLen = p_SUPL_Notification_Parms->RequestorIdLen <= K_CGPS_MAX_REQ_LENGTH ? p_SUPL_Notification_Parms->RequestorIdLen : K_CGPS_MAX_REQ_LENGTH;
                        vl_ClientNameLen = p_SUPL_Notification_Parms->ClientNameLen <= K_CGPS_MAX_CLIENT_LENGTH ? p_SUPL_Notification_Parms->ClientNameLen : K_CGPS_MAX_CLIENT_LENGTH;
                    }
                    break;
                }


                /* check if the notification callback has been registered, if not, the notification response is automatically sent according to GN_SUPL_Notify_Type value */
                if (vg_NotificationCallback == NULL)
                {
                    e_GN_Status vl_Status = GN_SUCCESS;

                    MC_CGPS_TRACE(("GN_SUPL_Notification_Req_Out : No notification callback registered"));

                    switch (p_SUPL_Notification_Parms->GN_SUPL_Notify_Type)
                    {
                    case NOTIFY_ONLY:
                        GN_SUPL_Notification_Rsp_In(Handle, &vl_Status, TRUE);
                        break;
                        /* Privacy override and Notify None should not result in any notification to user */
                    case NOTIFY_PRIVACY_OVERRIDE:
                    case NOTIFY_NONE:
                    case NOTIFY_ALLOWED_ON_TIMEOUT:
                        GN_SUPL_Notification_Rsp_In(Handle, &vl_Status, TRUE);
                        break;
                    case NOTIFY_DENIED_ON_TIMEOUT:
                    default:
                        GN_SUPL_Notification_Rsp_In(Handle, &vl_Status, FALSE);
                        break;
                    }

                    s_CgpsNotification[vl_index].v_IsPending = FALSE;
                    GN_SUPL_Handler();

                    /* operation successful */
                    *p_Status = GN_SUCCESS;
                    vl_return = TRUE;
                }
                else
                {
                    /* search for the first available Notification context */
                    for (vl_index = 0; vl_index < K_CGPS_MAX_NUMBER_OF_SUPL && s_CgpsNotification[vl_index].v_IsPending; vl_index++);

                    /* if no more handle is available, the notification is cancelled */
                    if (vl_index == K_CGPS_MAX_NUMBER_OF_SUPL)
                    {
                        MC_CGPS_TRACE(("GN_SUPL_Notification_Req_Out : Notification Handle not found!!!"));
                        *p_Status = GN_ERR_NO_RESOURCE;
                        return FALSE;
                    }
                    else
                    {
                        e_GN_Status vl_Status = GN_SUCCESS;


                        switch (p_SUPL_Notification_Parms->GN_SUPL_Notify_Type)
                        {

                        case NOTIFY_ONLY:
                            GN_SUPL_Notification_Rsp_In(Handle, &vl_Status, TRUE);
                            s_CgpsNotification[vl_index].v_IsPending = FALSE;
                            GN_SUPL_Handler();
                            break;
                            /* Privacy override and Notify None should not result in any notification to user */
                        case NOTIFY_PRIVACY_OVERRIDE:
                        case NOTIFY_NONE:
                            GN_SUPL_Notification_Rsp_In(Handle, &vl_Status, TRUE);
                            s_CgpsNotification[vl_index].v_IsPending = FALSE;
                            /* operation successful */
                            GN_SUPL_Handler();
                            *p_Status = GN_SUCCESS;
                            vl_return = TRUE;
                            return vl_return;
                        default:
                            /* SUPL notification context setting */
                            vg_CGPS_Notification_Count++;
                            s_CgpsNotification[vl_index].v_IsPending = TRUE;

                            break;
                        }
                        {
                            t_cgps_Notification vl_cgps_SuplNotification;
                            memset(&vl_cgps_SuplNotification, 0, sizeof(vl_cgps_SuplNotification));

                            /* SUPL notification context setting */
                            s_CgpsNotification[vl_index].v_Handle = (uint32_t)Handle;
                            s_CgpsNotification[vl_index].v_HandleType = K_CGPS_SUPL_HANDLE;
                            s_CgpsNotification[vl_index].v_DefaultNotificationType = CGPS4_02ConvertSuplNotificationType(p_SUPL_Notification_Parms->GN_SUPL_Notify_Type);
                            s_CgpsNotification[vl_index].v_Timeout = GN_GPS_Get_OS_Time_ms() + K_CGPS_SUPL_NOTIFICATION_TIMEOUT;

                            /* callback parameters settings */
                            vl_cgps_SuplNotification.v_Handle = (void*)(uint32_t)vl_index;

                            vl_cgps_SuplNotification.v_NotificationType = CGPS4_02ConvertSuplNotificationType(p_SUPL_Notification_Parms->GN_SUPL_Notify_Type);

                            vl_cgps_SuplNotification.v_RequestorIdEncodingType = CGPS4_03ConvertEncodingType(p_SUPL_Notification_Parms->GN_SUPL_Encoding_Type);
                            vl_cgps_SuplNotification.v_ClientNameEncodingType = vl_cgps_SuplNotification.v_RequestorIdEncodingType ;
                            vl_cgps_SuplNotification.v_CodeWordAlphabet = vl_cgps_SuplNotification.v_RequestorIdEncodingType ;
                            vl_cgps_SuplNotification.v_RequestorIdLen = vl_RequestorIdLen;
                            vl_cgps_SuplNotification.v_ClientNameLen = vl_ClientNameLen;

                            switch (p_SUPL_Notification_Parms->GN_SUPL_Encoding_Type)
                            {
                                case ENCODING_ucs2:
                                {
                                    if( vl_RequestorIdLen > 0 )
                                    {
                                        memcpy((char*)vl_cgps_SuplNotification.a_RequestorId, ((char*)p_SUPL_Notification_Parms->RequestorId + 2), vl_RequestorIdLen);
                                        vl_cgps_SuplNotification.v_RequestorIdType = CGPS4_04ConvertFormatIndicator(p_SUPL_Notification_Parms->RequestorIdType);
                                    }
                                    else
                                    {
                                        vl_cgps_SuplNotification.v_RequestorIdLen = 0;
                                        memset((uint8_t*)vl_cgps_SuplNotification.a_RequestorId,'\0',K_CGPS_MAX_REQ_LENGTH);
                                        vl_cgps_SuplNotification.v_RequestorIdType = K_CGPS_NO_FORMAT;
                                    }

                                    if( vl_ClientNameLen > 0 )
                                    {
                                        memcpy((char*)vl_cgps_SuplNotification.a_ClientName, ((char*)p_SUPL_Notification_Parms->ClientName + 2), vl_ClientNameLen);
                                        vl_cgps_SuplNotification.v_ClientNameType = CGPS4_04ConvertFormatIndicator(p_SUPL_Notification_Parms->ClientNameType);
                                    }
                                    else
                                    {
                                        vl_cgps_SuplNotification.v_ClientNameLen = 0;
                                        memset((uint8_t*)vl_cgps_SuplNotification.a_ClientName,'\0',K_CGPS_MAX_CLIENT_LENGTH);
                                        vl_cgps_SuplNotification.v_ClientNameType = K_CGPS_NO_FORMAT;

                                    }

                                }
                                break;
                                case ENCODING_gsmDefault:
                                case ENCODING_utf8:
                                case ENCODING_NONE:
                                default:
                                {
                                    memcpy((char*)vl_cgps_SuplNotification.a_RequestorId, (char*)p_SUPL_Notification_Parms->RequestorId, vl_RequestorIdLen);
                                    vl_cgps_SuplNotification.v_RequestorIdType = CGPS4_04ConvertFormatIndicator(p_SUPL_Notification_Parms->RequestorIdType);

                                    memcpy((char*)vl_cgps_SuplNotification.a_ClientName, (char*)p_SUPL_Notification_Parms->ClientName, vl_ClientNameLen);
                                    vl_cgps_SuplNotification.v_ClientNameType = CGPS4_04ConvertFormatIndicator(p_SUPL_Notification_Parms->ClientNameType);

                                }
                                break;
                            }


                            vl_cgps_SuplNotification.v_CodeWordLen = 0;
                            memset((uint8_t*)vl_cgps_SuplNotification.a_CodeWord,'\0',K_CGPS_MAX_CODE_WORD_LENGTH); /* FIXME: necessary ?! */

                            vl_cgps_SuplNotification.v_TonNpi = 0;
                            vl_cgps_SuplNotification.v_TonNpiConfig= 0; /* not populated */
                            memset((uint8_t*)vl_cgps_SuplNotification.a_PhoneNumber,'\0',K_CGPS_MAX_PHONE_NUM_LENGTH); /* FIXME: necessary ?! */

                            vl_cgps_SuplNotification.v_LcsServiceTypeId=K_CGPS_NAVIGATION;

                            vg_NotificationCallback(&vl_cgps_SuplNotification);

                            MC_CGPS_TRACE(("GN_SUPL_Notification_Req_Out : Notification callback called"));

                            /* operation successful */
                            *p_Status = GN_SUCCESS;
                            vl_return = TRUE;
                        }
                    }
                }
            }
        }
        else
        {
            MC_CGPS_TRACE(("GN_SUPL_Notification_Req_Out : SUPL suspended!!!"));
            *p_Status = GN_ERR_CONN_REJECTED;
        }
    }
    return vl_return;;
}


#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 10

void GN_SUPL_Debug_Event(
   e_GN_SUPL_DebugEvents events,
   u_GN_SUPL_DebugEventPayload  *payload
)
{
    switch( events )
    {
        case SUPL_PosRequest:
            MC_CGPS_SEND_PSTE_NMEA_FIX_REQUEST();
            MC_CGPS_TRACE(("SUPL_PosRequest"));
            break;
        case SUPL_PosResponse:
            MC_CGPS_SEND_PSTE_NMEA_FIX_COMPLETE();
            MC_CGPS_TRACE(("SUPL_PosResponse"));
            break;
        case SUPL_PosAbort:     /* Not implemented yet */
            MC_CGPS_TRACE(("SUPL_PosAbort"));
            break;
        case SUPL_PosReport:
            MC_CGPS_SEND_PSTE_NMEA_FIX_WGS84_REPORT(payload->posReport.Latitude,payload->posReport.Longitude,payload->posReport.Altitude);
            MC_CGPS_TRACE(("SUPL_PosReport : %4.11lf , %4.11lf , %6.9lf" , payload->posReport.Latitude,payload->posReport.Longitude,payload->posReport.Altitude));
            break;
    }

}
/* +SUPLv2.0 Modifications */

#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 11

void GN_SUPL_TriggerPeriodic_Req_Out(
   void*          Handle,                 ///< [in] Opaque Handle used to coordinate requests.
   U4             NumberOfFixesRequested, ///< [in] range 1..8639999. Number of fixes*PeriodicInterval < 100 Days
   U4             PeriodicInterval,       ///< [in] Unit of Seconds . range 1..8639999. Number of fixes*PeriodicInterval < 100 Days
   U4             StartTime               ///< [in] Unit of Seconds . range 0..2678400. Relative to time of function call
)
{
    int8_t    vl_index          = 0;
    uint32_t  vl_CurrentOSTime  = GN_GPS_Get_OS_Time_ms();

    for (vl_index = 0; vl_index < K_CGPS_MAX_NUMBER_OF_SUPL && s_CgpsSupl[vl_index].v_GPSHandle != Handle; vl_index++);

    if (vl_index == K_CGPS_MAX_NUMBER_OF_SUPL)
    {
        MC_CGPS_TRACE(("GN_SUPL_TriggerPeriodic_Req_Out : Handle not found!!!"));
        return;
    }

    s_CgpsSupl[vl_index].v_PeriodicTriggeredParams.v_NumFixes    = NumberOfFixesRequested;
    s_CgpsSupl[vl_index].v_PeriodicTriggeredParams.v_IntervalFix = PeriodicInterval*1000;             /* Seconds to Milliseconds */
    s_CgpsSupl[vl_index].v_PeriodicTriggeredParams.v_StartTime   = vl_CurrentOSTime + StartTime*1000; /* Seconds to Milliseconds */

    s_CgpsSupl[vl_index].v_TriggeredEventType = K_CGPS_TRIGGER_EVENTTYPE_PERIODIC;

    if( s_CgpsSupl[vl_index].v_PeriodicTriggeredParams.v_StartTime > vl_CurrentOSTime )
    {
        /* Trigger the timer for Start Time period*/
        CGPS5_36TriggeredStartTime(vl_index);

        //CGPS4_21SetSuspendedState(CGPS_SUPL_APPLICATION_ID ,TRUE); This was defined in the prototype, why it shouldn't be use.
    }
    else
    {
        if(s_CgpsSupl[vl_index].v_PeriodicTriggeredParams.v_NumFixes > 0)
        {
            e_GN_Status v_Status = GN_SUCCESS;

            GN_SUPL_Trigger_Ind_In(s_CgpsSupl[vl_index].v_GPSHandle, &v_Status);

            CGPS5_35TriggeredIntervalTime(vl_index);

            s_CgpsSupl[vl_index].v_PeriodicTriggeredParams.v_NumFixes--;
        }
    }

    MC_CGPS_TRACE(("GN_SUPL_TriggerPeriodic_Req_Out Exit CGPS_State=%s, Gps_State=%s", a_CGPS_StateAsEnum[vg_CGPS_State], a_GPS_StateAsEnum[vg_CGPS_GpsState]));

}


#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 12
void GN_SUPL_TriggerAreaEvent_Req_Out(
   void*          Handle,                            ///< [in] Opaque Handle used to coordinate requests.
   s_GN_SUPL_V2_AreaEventTrigger* AreaEventParams     /// < [in] Pointer to a set of notification parameters.
)
{
    int8_t    vl_SuplSessionIndex  = 0;
    int8_t    vl_Index  = 0;
    int8_t    NumOfAreaId  = 0;
    uint32_t  vl_CurrentOsTime = GN_GPS_Get_OS_Time_ms();
    t_cgps_GeographicTargetArea* pl_GeoTargetArea;
    t_cgps_AreaIdList*  pl_AreaIdLists;

    for (vl_SuplSessionIndex = 0; vl_SuplSessionIndex < K_CGPS_MAX_NUMBER_OF_SUPL && s_CgpsSupl[vl_SuplSessionIndex].v_GPSHandle != Handle; vl_SuplSessionIndex++);

    if (vl_SuplSessionIndex == K_CGPS_MAX_NUMBER_OF_SUPL)
    {
        MC_CGPS_TRACE(("GN_SUPL_TriggerAreaEvent_Req_Out : Handle not found!!!"));
        return;
    }

    s_CgpsSupl[vl_SuplSessionIndex].v_TriggeredEventType = K_CGPS_TRIGGER_EVENTTYPE_AREA;

    s_CgpsSupl[vl_SuplSessionIndex].v_AreaEventTriggerParams.v_AreaEventType    = AreaEventParams->areaEventType;
    s_CgpsSupl[vl_SuplSessionIndex].v_AreaEventTriggerParams.v_MaximumReports   = AreaEventParams->repeatedReportingParams.maximumNumberOfReports;
    s_CgpsSupl[vl_SuplSessionIndex].v_AreaEventTriggerParams.v_MinIntervalTime  = AreaEventParams->repeatedReportingParams.minimumIntervalTime*1000;
    s_CgpsSupl[vl_SuplSessionIndex].v_AreaEventTriggerParams.v_StartTime        = vl_CurrentOsTime + AreaEventParams->startTime *1000;
    s_CgpsSupl[vl_SuplSessionIndex].v_AreaEventTriggerParams.v_StopTime         = vl_CurrentOsTime + AreaEventParams->stopTime  *1000;

    MC_CGPS_TRACE(("GN_SUPL_TriggerAreaEvent_Req_Out : AreaEventType %i,  MaxReports %i, MinIntervalTime %i, CurrentTime %i, StartTime %i, StopTime %i",  \
        s_CgpsSupl[vl_SuplSessionIndex].v_AreaEventTriggerParams.v_AreaEventType,   \
        s_CgpsSupl[vl_SuplSessionIndex].v_AreaEventTriggerParams.v_MaximumReports,  \
        s_CgpsSupl[vl_SuplSessionIndex].v_AreaEventTriggerParams.v_MinIntervalTime, \
        vl_CurrentOsTime,                                                           \
        s_CgpsSupl[vl_SuplSessionIndex].v_AreaEventTriggerParams.v_StartTime,       \
        s_CgpsSupl[vl_SuplSessionIndex].v_AreaEventTriggerParams.v_StopTime        \
    ));

    s_CgpsSupl[vl_SuplSessionIndex].v_AreaEventTriggerParams.v_AreaDefinition.v_NumGeoArea = AreaEventParams->geographicTargetAreaCount;
    
    s_CgpsSupl[vl_SuplSessionIndex].v_AreaEventTriggerParams.v_AreaDefinition.v_NumAreaIdLists = AreaEventParams->AreaIdListsCount;

    MC_CGPS_TRACE(("GN_SUPL_TriggerAreaEvent_Req_Out : TargetAreaCount %i",s_CgpsSupl[vl_SuplSessionIndex].v_AreaEventTriggerParams.v_AreaDefinition.v_NumAreaIdLists ));

    for( vl_Index = 0; vl_Index < AreaEventParams->AreaIdListsCount; vl_Index++ )
    {
        pl_AreaIdLists = &(s_CgpsSupl[vl_SuplSessionIndex].v_AreaEventTriggerParams.v_AreaDefinition.a_AreaIdLists[vl_Index]);
        pl_AreaIdLists->v_NumAreaId = AreaEventParams->AreaIdListsParams[vl_Index].NumAreaId;

        for( NumOfAreaId = 0; NumOfAreaId < AreaEventParams->AreaIdListsParams[vl_Index].NumAreaId; NumOfAreaId++ )
        {

            pl_AreaIdLists->a_AreaIdSetParms[NumOfAreaId].v_AreaIdSet  = AreaEventParams->AreaIdListsParams[vl_Index].s_AreaIdSetParms[NumOfAreaId].AreaIdType;

            switch( pl_AreaIdLists->a_AreaIdSetParms[NumOfAreaId].v_AreaIdSet )
            {
                case K_CGPS_AREA_IDSET_GSMAREAID:
                    pl_AreaIdLists->a_AreaIdSetParms[NumOfAreaId].v_GSMRefMcc = AreaEventParams->AreaIdListsParams[vl_Index].s_AreaIdSetParms[NumOfAreaId].choice.gsmAreaIdInfo.refMCC;
                    pl_AreaIdLists->a_AreaIdSetParms[NumOfAreaId].v_GSMRefMnc = AreaEventParams->AreaIdListsParams[vl_Index].s_AreaIdSetParms[NumOfAreaId].choice.gsmAreaIdInfo.refMNC;
                    pl_AreaIdLists->a_AreaIdSetParms[NumOfAreaId].v_GSMRefLac = AreaEventParams->AreaIdListsParams[vl_Index].s_AreaIdSetParms[NumOfAreaId].choice.gsmAreaIdInfo.refLAC;
                    pl_AreaIdLists->a_AreaIdSetParms[NumOfAreaId].v_GSMRefCi  = AreaEventParams->AreaIdListsParams[vl_Index].s_AreaIdSetParms[NumOfAreaId].choice.gsmAreaIdInfo.refCI;
                break;

                case K_CGPS_AREA_IDSET_WCDMAAREAID :
                    pl_AreaIdLists->a_AreaIdSetParms[NumOfAreaId].v_WCDMARefMcc = AreaEventParams->AreaIdListsParams[vl_Index].s_AreaIdSetParms[NumOfAreaId].choice.wcdmaAreaIdInfo.refMCC;
                    pl_AreaIdLists->a_AreaIdSetParms[NumOfAreaId].v_WCDMARefMnc = AreaEventParams->AreaIdListsParams[vl_Index].s_AreaIdSetParms[NumOfAreaId].choice.wcdmaAreaIdInfo.refMNC;
                    pl_AreaIdLists->a_AreaIdSetParms[NumOfAreaId].v_WCDMARefUc  = AreaEventParams->AreaIdListsParams[vl_Index].s_AreaIdSetParms[NumOfAreaId].choice.wcdmaAreaIdInfo.refUC;
                break;

                case K_CGPS_AREA_IDSET_CDMAAREAID:
                    pl_AreaIdLists->a_AreaIdSetParms[NumOfAreaId].v_CDMARefSid    = AreaEventParams->AreaIdListsParams[vl_Index].s_AreaIdSetParms[NumOfAreaId].choice.cdmaAreaIdInfo.refSID;
                    pl_AreaIdLists->a_AreaIdSetParms[NumOfAreaId].v_CDMARefNid    = AreaEventParams->AreaIdListsParams[vl_Index].s_AreaIdSetParms[NumOfAreaId].choice.cdmaAreaIdInfo.refNID;
                    pl_AreaIdLists->a_AreaIdSetParms[NumOfAreaId].v_CDMARefBaseid = AreaEventParams->AreaIdListsParams[vl_Index].s_AreaIdSetParms[NumOfAreaId].choice.cdmaAreaIdInfo.refBASEID;
                break;

                default:
                    MC_CGPS_TRACE(("GN_SUPL_TriggerAreaEvent_Req_Out : Invalid area id!!!"));
                    return;
                break;
            }
        }
    }

    for( vl_Index = 0; vl_Index < AreaEventParams->geographicTargetAreaCount ; vl_Index++ )
    {
        pl_GeoTargetArea = &(s_CgpsSupl[vl_SuplSessionIndex].v_AreaEventTriggerParams.v_AreaDefinition.a_GeographicTargetArea[vl_Index]);
        pl_GeoTargetArea->v_ShapeType      = AreaEventParams->geographicTargetAreaParams[vl_Index].shapeType;

        switch( pl_GeoTargetArea->v_ShapeType )
        {
            case K_CGPS_CIRCULAR_AREA:
                *pl_GeoTargetArea->v_LatSign   = AreaEventParams->geographicTargetAreaParams[vl_Index].choice.circularArea.center.latitudeSign;
                *pl_GeoTargetArea->v_Latitude  = AreaEventParams->geographicTargetAreaParams[vl_Index].choice.circularArea.center.latitude;
                *pl_GeoTargetArea->v_Longitude = AreaEventParams->geographicTargetAreaParams[vl_Index].choice.circularArea.center.longitude;
                pl_GeoTargetArea->v_CircularRadius    = AreaEventParams->geographicTargetAreaParams[vl_Index].choice.circularArea.radius;
                pl_GeoTargetArea->v_CircularRadiusMax = AreaEventParams->geographicTargetAreaParams[vl_Index].choice.circularArea.radiusMax;
                pl_GeoTargetArea->v_CircularRadiusMin = AreaEventParams->geographicTargetAreaParams[vl_Index].choice.circularArea.radiusMin;
            break;

            case K_CGPS_ELLIPTICAL_AREA :
                *pl_GeoTargetArea->v_LatSign   = AreaEventParams->geographicTargetAreaParams[vl_Index].choice.ellipticalArea.center.latitudeSign;
                *pl_GeoTargetArea->v_Latitude  = AreaEventParams->geographicTargetAreaParams[vl_Index].choice.ellipticalArea.center.latitude;
                *pl_GeoTargetArea->v_Longitude = AreaEventParams->geographicTargetAreaParams[vl_Index].choice.ellipticalArea.center.longitude;
                pl_GeoTargetArea->v_EllipSemiMajor      = AreaEventParams->geographicTargetAreaParams[vl_Index].choice.ellipticalArea.semiMajor;
                pl_GeoTargetArea->v_EllipSemiMajorMax   = AreaEventParams->geographicTargetAreaParams[vl_Index].choice.ellipticalArea.semiMajorMax;
                pl_GeoTargetArea->v_EllipSemiMajorMin   = AreaEventParams->geographicTargetAreaParams[vl_Index].choice.ellipticalArea.semiMajorMin;
                pl_GeoTargetArea->v_EllipSemiMinor      = AreaEventParams->geographicTargetAreaParams[vl_Index].choice.ellipticalArea.semiMinor;
                pl_GeoTargetArea->v_EllipSemiMinorMax   = AreaEventParams->geographicTargetAreaParams[vl_Index].choice.ellipticalArea.semiMinorMax;
                pl_GeoTargetArea->v_EllipSemiMinorMin   = AreaEventParams->geographicTargetAreaParams[vl_Index].choice.ellipticalArea.semiMinorMin;
                pl_GeoTargetArea->v_EllipSemiMinorAngle = AreaEventParams->geographicTargetAreaParams[vl_Index].choice.ellipticalArea.semiMinorAngle;
            break;

            case K_CGPS_POLYGON_AREA:
            {
                uint8_t vl_PointsIndex = 0;
                
                pl_GeoTargetArea->v_NumberOfPoints    = AreaEventParams->geographicTargetAreaParams[vl_Index].choice.polygonArea.numOfPoints;
                pl_GeoTargetArea->v_PolygonHysteresis = AreaEventParams->geographicTargetAreaParams[vl_Index].choice.polygonArea.polygonHysteresis;

                for( vl_PointsIndex = 0; vl_PointsIndex < pl_GeoTargetArea->v_NumberOfPoints ; vl_PointsIndex++ )
                {
                    pl_GeoTargetArea->v_LatSign[vl_PointsIndex]   = AreaEventParams->geographicTargetAreaParams[vl_Index].choice.polygonArea.coordinates[vl_PointsIndex].latitudeSign;
                    pl_GeoTargetArea->v_Latitude[vl_PointsIndex]  = AreaEventParams->geographicTargetAreaParams[vl_Index].choice.polygonArea.coordinates[vl_PointsIndex].latitude;
                    pl_GeoTargetArea->v_Longitude[vl_PointsIndex] = AreaEventParams->geographicTargetAreaParams[vl_Index].choice.polygonArea.coordinates[vl_PointsIndex].longitude;
                }
            }
            break;

            default:
                MC_CGPS_TRACE(("GN_SUPL_TriggerAreaEvent_Req_Out : Invalid shape type!!!"));
                return;
            break;
        }
    }

    if(s_CgpsSupl[vl_SuplSessionIndex].v_AreaEventTriggerParams.v_StartTime > vl_CurrentOsTime)
    {   
        MC_CGPS_TRACE(("GN_SUPL_TriggerAreaEvent_Req_Out : Start Time trigger started"));

        CGPS5_36TriggeredStartTime(vl_SuplSessionIndex);
    }
    else
    {
        if(s_CgpsSupl[vl_SuplSessionIndex].v_AreaEventTriggerParams.v_MaximumReports > 0)
        {   
            e_cgps_RelativePosition vl_CurrentPositionStatus;
            e_GN_Status v_Status = GN_SUCCESS;

            if (s_CgpsSupl[vl_SuplSessionIndex].v_AreaEventTriggerParams.v_AreaDefinition.v_NumAreaIdLists)
            {
                if( CGPS4_56CheckForAreaIdEvent( s_CgpsSupl[vl_SuplSessionIndex].v_AreaEventTriggerParams.v_AreaDefinition.a_AreaIdLists,
                                           s_CgpsSupl[vl_SuplSessionIndex].v_AreaEventTriggerParams.v_AreaEventType,
                                           s_CgpsSupl[vl_SuplSessionIndex].v_PrevCellIdPositionStatus,
                                           &vl_CurrentPositionStatus,
                                           &vg_CGPS_Supl_Location_Id ) )
                {
                    MC_CGPS_TRACE(("CGPS4_56CheckForAreaIdEvent : AreaId event occurred, %s", a_CGPS_RelativePositionasEnum[vl_CurrentPositionStatus]));
            
                    GN_SUPL_Trigger_Ind_In(s_CgpsSupl[vl_SuplSessionIndex].v_GPSHandle, &v_Status);
                }

                s_CgpsSupl[vl_SuplSessionIndex].v_PrevCellIdPositionStatus = vl_CurrentPositionStatus;
            }
            else
            {
                GN_SUPL_Trigger_Ind_In(s_CgpsSupl[vl_SuplSessionIndex].v_GPSHandle, &v_Status);
            }


            if( s_CgpsSupl[vl_SuplSessionIndex].v_AreaEventTriggerParams.v_StopTime
                        < vl_CurrentOsTime+s_CgpsSupl[vl_SuplSessionIndex].v_AreaEventTriggerParams.v_MinIntervalTime )
            {   
                MC_CGPS_TRACE(("GN_SUPL_TriggerAreaEvent_Req_Out : Stop Time trigger started, next iteration not processed"));
                    
                /* If Stop time is lesser than the next fix interval, start a timer with stop time & abort SUPL session on expiry*/
                CGPS5_46AreaEventStopTime(vl_SuplSessionIndex);
            }
            else
            {
                MC_CGPS_TRACE(("GN_SUPL_TriggerAreaEvent_Req_Out : Starting timer for next iteration"));
                CGPS5_35TriggeredIntervalTime(vl_SuplSessionIndex);
            }
        }
    }

    MC_CGPS_TRACE(("GN_SUPL_TriggerAreaEvent_Req_Out Exit CGPS_State=%s, Gps_State=%s", a_CGPS_StateAsEnum[vg_CGPS_State], a_GPS_StateAsEnum[vg_CGPS_GpsState]));
}


#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 13
BL GN_SUPL_Check_AreaEvent
( 
    void*          Handle,                            ///< [in] Opaque Handle used to coordinate requests.
    s_GN_SUPL_V2_Coordinate* pl_CurrentPosition
)
{
    bool      vl_Status            = FALSE;
    int8_t    vl_SuplSessionIndex  = 0;
    e_cgps_RelativePosition vl_CurrentPositionStatus;

    for (vl_SuplSessionIndex = 0; vl_SuplSessionIndex < K_CGPS_MAX_NUMBER_OF_SUPL && s_CgpsSupl[vl_SuplSessionIndex].v_GPSHandle != Handle; vl_SuplSessionIndex++);

    if (vl_SuplSessionIndex == K_CGPS_MAX_NUMBER_OF_SUPL)
    {   
        MC_CGPS_TRACE(("GN_SUPL_Check_AreaEvent : Handle not found!!!"));
    }
    else
    {
        t_Cgps_Coordinate vl_CurrentPosition;

        vl_CurrentPosition.v_LatitudeSign = pl_CurrentPosition->latitudeSign;
        vl_CurrentPosition.v_Latitude     = pl_CurrentPosition->latitude; 
        vl_CurrentPosition.v_Longitude    = pl_CurrentPosition->longitude;

        /* area_id_zahid, check for event only if area present  NI */
        if (s_CgpsSupl[vl_SuplSessionIndex].v_AreaEventTriggerParams.v_AreaDefinition.v_NumGeoArea)
        {
            if( CGPS4_48CheckForGeoAreaEvent( s_CgpsSupl[vl_SuplSessionIndex].v_AreaEventTriggerParams.v_AreaDefinition.a_GeographicTargetArea,
                                       s_CgpsSupl[vl_SuplSessionIndex].v_AreaEventTriggerParams.v_AreaEventType,
                                       s_CgpsSupl[vl_SuplSessionIndex].v_PrevPositionStatus,
                                       &vl_CurrentPositionStatus,
                                       &vl_CurrentPosition) )
            {
                MC_CGPS_TRACE(("GN_SUPL_Check_AreaEvent : Area event occurred, %s", a_CGPS_RelativePositionasEnum[vl_CurrentPositionStatus]));
            
                s_CgpsSupl[vl_SuplSessionIndex].v_AreaEventTriggerParams.v_MaximumReports--;

                vl_Status = TRUE;
            }
            /* Updating the position status irrespective of event occurence */
            s_CgpsSupl[vl_SuplSessionIndex].v_PrevPositionStatus = vl_CurrentPositionStatus;
        }
        else 
        {
            if(s_CgpsSupl[vl_SuplSessionIndex].v_AreaEventTriggerParams.v_AreaDefinition.v_NumAreaIdLists)
            {
                s_CgpsSupl[vl_SuplSessionIndex].v_AreaEventTriggerParams.v_MaximumReports--;

                MC_CGPS_TRACE(("GN_SUPL_Check_AreaEvent : AreaId event occurred"));

                vl_Status = TRUE;
            }
        }

    }

    return vl_Status;
}

/* -SUPLv2.0 Modifications */


#endif /* AGPS_UP_FTR */
#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 14
/**
* \fn BL GN_GPS_Hard_Reset_GNB( void )
* \brief CPS library Callback to reset GPS module.
*
* GN GPS Callback Function to request that if possible the host should
* perform a Hard Power-Down Reset of the GN Baseband Chips.
*
* \return Returns TRUE if this is possible
*/
BL GN_GPS_Hard_Reset_GNB( void )
{

    gn_Patch_Force_Upload = 1;
    gn_Patch_Status       = 1;

    MC_HALGPS_RESET();

    return( TRUE );
}


#ifdef AGPS_EE_FTR

void GN_EE_Shutdown( void )
{
   // There is nothing to do.
   return;
}

void GN_EE_Initialise( void )
{
   // There is nothing to do.
   return;
}


BL GN_EE_Set_GPS_Eph_El( s_GN_EE_GPS_Eph_El *p_Eph_El )
{
    t_lsimee_SAGPS_BCEData   *pl_lsimee_SAGPSBCEData = NULL;


    MC_CGPS_TRACE(("GN_EE_Set_GPS_Eph_El : %3u, %1u , %2u , %2u , %1u , %2u , %2d , %4d , %4d , %4u , %5u , %5u , %4u , %6d , %6d , %5d , %6d , %6d , %6d , %6d , %6d , %6d , %8d , %11d , %10u , %10u , %11d , %11d , %11d , %8d",
    p_Eph_El->SatID,p_Eph_El->CodeOnL2,p_Eph_El->URA,p_Eph_El->SVHealth,p_Eph_El->FitIntFlag,p_Eph_El->AODA,p_Eph_El->L2Pflag,p_Eph_El->TGD,p_Eph_El->af2,p_Eph_El->Week,p_Eph_El->toc,p_Eph_El->toe,p_Eph_El->IODC,p_Eph_El->af1,p_Eph_El->dn,p_Eph_El->IDot,p_Eph_El->Crs,p_Eph_El->Crc,p_Eph_El->Cus,p_Eph_El->Cuc,p_Eph_El->Cis,p_Eph_El->Cic,p_Eph_El->af0,p_Eph_El->M0,p_Eph_El->e,p_Eph_El->APowerHalf,p_Eph_El->Omega0,p_Eph_El->i0,p_Eph_El->w,p_Eph_El->OmegaDot));

    if( (FALSE == MC_CGPS_IS_CONFIGURATION_ELEMENT_SET(K_CGPS_CONFIG_SELF_ASSISTED_GPS_ENABLED))|| (p_Eph_El->SatID > 32) )
    {
       // Remove compiler Warnings!
        p_Eph_El = p_Eph_El;
        return( FALSE );
    }

    MC_CGPS_TRACE(("GN_EE_Set_GPS_Eph_El PRN = %d",p_Eph_El->SatID));

    pl_lsimee_SAGPSBCEData = (t_lsimee_SAGPS_BCEData*)MC_RTK_GET_MEMORY(sizeof(*pl_lsimee_SAGPSBCEData));

    MC_CGPS_TRACE(("GN_EE_Set_GPS_Eph_El SatID=%3u,CodeOnL2=%1u ,URA=%2u ,SVHealth=%2u ,FitIntFlag=%1u ,AODA=%2u ,L2Pflag=%2d ,TGD=%4d ,af2=%4d ,Week=%4u ,toc=%5u ,toe=%5u ,IODC=%4u ,af1=%6d ,dn=%6d ,IDot=%5d ,Crs=%6d ,Crc=%6d ,Cus=%6d ,Cuc=%6d ,Cis=%6d ,Cic=%6d ,af0=%8d ,M0=%11d ,e= %10u ,APowerHalf=%10u ,Omega0=%11d ,i0=%11d ,w=%11d ,OmegaDot=%8d",
      p_Eph_El->SatID,p_Eph_El->CodeOnL2,p_Eph_El->URA,p_Eph_El->SVHealth,p_Eph_El->FitIntFlag,p_Eph_El->AODA,p_Eph_El->L2Pflag,p_Eph_El->TGD,p_Eph_El->af2,p_Eph_El->Week,p_Eph_El->toc,p_Eph_El->toe,p_Eph_El->IODC,p_Eph_El->af1,p_Eph_El->dn,p_Eph_El->IDot,p_Eph_El->Crs,p_Eph_El->Crc,p_Eph_El->Cus,p_Eph_El->Cuc,p_Eph_El->Cis,p_Eph_El->Cic,p_Eph_El->af0,p_Eph_El->M0,p_Eph_El->e,p_Eph_El->APowerHalf,p_Eph_El->Omega0,p_Eph_El->i0,p_Eph_El->w,p_Eph_El->OmegaDot));

    /*Convert the GPS Ephemeris structure from PE library format to MSL Format.*/

    pl_lsimee_SAGPSBCEData->v_NavDataBCE.v_ConsteType = K_CGPS_GPS_CONSTEL;


    pl_lsimee_SAGPSBCEData->v_NavDataBCE.v_GpsBCE.v_Prn         = p_Eph_El->SatID;
    pl_lsimee_SAGPSBCEData->v_NavDataBCE.v_GpsBCE.v_CAOrPOnL2   = p_Eph_El->CodeOnL2;
    pl_lsimee_SAGPSBCEData->v_NavDataBCE.v_GpsBCE.v_Ura         = p_Eph_El->URA;
    pl_lsimee_SAGPSBCEData->v_NavDataBCE.v_GpsBCE.v_Health      = p_Eph_El->SVHealth;
    pl_lsimee_SAGPSBCEData->v_NavDataBCE.v_GpsBCE.v_L2PData     = p_Eph_El->L2Pflag;
    pl_lsimee_SAGPSBCEData->v_NavDataBCE.v_GpsBCE.v_Ure         = 0;
    pl_lsimee_SAGPSBCEData->v_NavDataBCE.v_GpsBCE.v_AODO        = p_Eph_El->AODA;
    pl_lsimee_SAGPSBCEData->v_NavDataBCE.v_GpsBCE.v_TGD         = p_Eph_El->TGD;
    pl_lsimee_SAGPSBCEData->v_NavDataBCE.v_GpsBCE.v_Af2         = p_Eph_El->af2;
    pl_lsimee_SAGPSBCEData->v_NavDataBCE.v_GpsBCE.v_EphemFit    = p_Eph_El->FitIntFlag;
    pl_lsimee_SAGPSBCEData->v_NavDataBCE.v_GpsBCE.v_GpsWeek     = p_Eph_El->Week;
    pl_lsimee_SAGPSBCEData->v_NavDataBCE.v_GpsBCE.v_IoDc        = p_Eph_El->IODC;
    pl_lsimee_SAGPSBCEData->v_NavDataBCE.v_GpsBCE.v_Toc         = p_Eph_El->toc;
    pl_lsimee_SAGPSBCEData->v_NavDataBCE.v_GpsBCE.v_Toe         = p_Eph_El->toe;
    pl_lsimee_SAGPSBCEData->v_NavDataBCE.v_GpsBCE.v_Iode        = p_Eph_El->IODC;
    pl_lsimee_SAGPSBCEData->v_NavDataBCE.v_GpsBCE.v_Af1         = p_Eph_El->af1;
    pl_lsimee_SAGPSBCEData->v_NavDataBCE.v_GpsBCE.v_Crs         = p_Eph_El->Crs;
    pl_lsimee_SAGPSBCEData->v_NavDataBCE.v_GpsBCE.v_DeltaN      = p_Eph_El->dn;
    pl_lsimee_SAGPSBCEData->v_NavDataBCE.v_GpsBCE.v_Cuc         = p_Eph_El->Cuc;
    pl_lsimee_SAGPSBCEData->v_NavDataBCE.v_GpsBCE.v_Cus         = p_Eph_El->Cus;
    pl_lsimee_SAGPSBCEData->v_NavDataBCE.v_GpsBCE.v_Cic         = p_Eph_El->Cic;
    pl_lsimee_SAGPSBCEData->v_NavDataBCE.v_GpsBCE.v_Cis         = p_Eph_El->Cis;
    pl_lsimee_SAGPSBCEData->v_NavDataBCE.v_GpsBCE.v_Crc         = p_Eph_El->Crc;
    pl_lsimee_SAGPSBCEData->v_NavDataBCE.v_GpsBCE.v_IDot        = p_Eph_El->IDot;
    pl_lsimee_SAGPSBCEData->v_NavDataBCE.v_GpsBCE.v_E           = p_Eph_El->e;
    pl_lsimee_SAGPSBCEData->v_NavDataBCE.v_GpsBCE.v_SqrtA       = p_Eph_El->APowerHalf;
    pl_lsimee_SAGPSBCEData->v_NavDataBCE.v_GpsBCE.v_Af0         = p_Eph_El->af0;
    pl_lsimee_SAGPSBCEData->v_NavDataBCE.v_GpsBCE.v_M0          = p_Eph_El->M0;
    pl_lsimee_SAGPSBCEData->v_NavDataBCE.v_GpsBCE.v_Omega0      = p_Eph_El->Omega0;
    pl_lsimee_SAGPSBCEData->v_NavDataBCE.v_GpsBCE.v_I0          = p_Eph_El->i0;
    pl_lsimee_SAGPSBCEData->v_NavDataBCE.v_GpsBCE.v_W           = p_Eph_El->w;
    pl_lsimee_SAGPSBCEData->v_NavDataBCE.v_GpsBCE.v_OmegaDot    = p_Eph_El->OmegaDot;


   CGPS4_28SendMsgToProcess(PROCESS_CGPS ,
                            PROCESS_LSIMEE,
                            CGPS_LSIMEE_FEED_BCE_IND,
                            (t_MsgHeader *)pl_lsimee_SAGPSBCEData);

   return( TRUE );

}

BL GN_EE_Set_GLON_Eph_El( s_GN_EE_GLON_Eph_El *p_Eph_El )
{
    t_lsimee_SAGPS_BCEData   *pl_lsimee_SAGPSBCEData = NULL;
    uint32_t              EE_GLONASS_secs = 0;
    U1 N4=5;    /*TODO: STUB TILL PE GIVES N4 VALUE*/


    MC_CGPS_TRACE(( "GN_EE_Set_GLON_Eph_El :    %2d,    %2d,    %u,    %u,    %u,    %u,    %u,    %d,    %u,    %d,    %d,    %d,    %d,    %d,    %d,    %d,    %d,    %d,    %d,    %d,    %d,    %d",
                  p_Eph_El->gloN,p_Eph_El->gloFrqCh,p_Eph_El->gloTb,p_Eph_El->gloNT ,p_Eph_El->gloLn , p_Eph_El->gloFT ,
                  p_Eph_El->gloEn, p_Eph_El->gloP1,p_Eph_El->gloP2,p_Eph_El->gloM,p_Eph_El->gloX ,p_Eph_El->gloY ,p_Eph_El->gloZ,p_Eph_El->gloXdot ,p_Eph_El->gloYdot , p_Eph_El->gloZdot , p_Eph_El->gloXdotdot, p_Eph_El->gloYdotdot,p_Eph_El->gloZdotdot,
                  p_Eph_El->gloDeltaTau,p_Eph_El->gloTau ,p_Eph_El->gloGamma));


    if(FALSE == MC_CGPS_IS_CONFIGURATION_ELEMENT_SET(K_CGPS_CONFIG_SELF_ASSISTED_GLONASS_ENABLED))
    {
       // Remove compiler Warnings!
        p_Eph_El = p_Eph_El;
        return( FALSE );
    }

    MC_CGPS_TRACE(("GN_EE_Set_GLON_Eph_El gloN = %d",p_Eph_El->gloN));

    pl_lsimee_SAGPSBCEData = (t_lsimee_SAGPS_BCEData*)MC_RTK_GET_MEMORY(sizeof(*pl_lsimee_SAGPSBCEData)); 


    /*Convert the Glonass Ephemeris structure from PE library format to MSL Format.*/

    pl_lsimee_SAGPSBCEData->v_NavDataBCE.v_ConsteType = K_CGPS_GLO_CONSTEL;


    /*
       N4-1  = Will give the previous block
       *1461  i.e  ((365 * 4) +  1 ) 1 for leap year
       total days * by 86400 i.e (24*60*60)
       tb * 900 i.e (15*60)
        */

    EE_GLONASS_secs = ((((N4-1)*1461 + (p_Eph_El->gloNT-1))*86400) + (p_Eph_El->gloTb * 900));

    pl_lsimee_SAGPSBCEData->v_NavDataBCE.v_GloBCE.v_Slot        = p_Eph_El->gloN;
    pl_lsimee_SAGPSBCEData->v_NavDataBCE.v_GloBCE.v_FT          = p_Eph_El->gloFT;
    pl_lsimee_SAGPSBCEData->v_NavDataBCE.v_GloBCE.v_M           = p_Eph_El->gloM;
    pl_lsimee_SAGPSBCEData->v_NavDataBCE.v_GloBCE.v_Bn          = p_Eph_El->gloLn; 
    pl_lsimee_SAGPSBCEData->v_NavDataBCE.v_GloBCE.v_UtcOffset   = 0;
    pl_lsimee_SAGPSBCEData->v_NavDataBCE.v_GloBCE.v_FreqChannel = p_Eph_El->gloFrqCh;
    pl_lsimee_SAGPSBCEData->v_NavDataBCE.v_GloBCE.v_Gamma       = p_Eph_El->gloGamma;
    pl_lsimee_SAGPSBCEData->v_NavDataBCE.v_GloBCE.v_Lsx         = p_Eph_El->gloXdotdot;
    pl_lsimee_SAGPSBCEData->v_NavDataBCE.v_GloBCE.v_Lsy         = p_Eph_El->gloYdotdot;
    pl_lsimee_SAGPSBCEData->v_NavDataBCE.v_GloBCE.v_Lsz         = p_Eph_El->gloZdotdot;
    pl_lsimee_SAGPSBCEData->v_NavDataBCE.v_GloBCE.v_GloSec      = EE_GLONASS_secs;
    pl_lsimee_SAGPSBCEData->v_NavDataBCE.v_GloBCE.v_TauN        = p_Eph_El->gloTau;
    pl_lsimee_SAGPSBCEData->v_NavDataBCE.v_GloBCE.v_X           = p_Eph_El->gloX;
    pl_lsimee_SAGPSBCEData->v_NavDataBCE.v_GloBCE.v_Y           = p_Eph_El->gloY;
    pl_lsimee_SAGPSBCEData->v_NavDataBCE.v_GloBCE.v_Z           = p_Eph_El->gloZ;
    pl_lsimee_SAGPSBCEData->v_NavDataBCE.v_GloBCE.v_Vx          = p_Eph_El->gloXdot;
    pl_lsimee_SAGPSBCEData->v_NavDataBCE.v_GloBCE.v_Vy          = p_Eph_El->gloYdot;
    pl_lsimee_SAGPSBCEData->v_NavDataBCE.v_GloBCE.v_Vz          = p_Eph_El->gloZdot;


   CGPS4_28SendMsgToProcess(PROCESS_CGPS ,
                            PROCESS_LSIMEE,
                            CGPS_LSIMEE_FEED_BCE_IND,
                            (t_MsgHeader *)pl_lsimee_SAGPSBCEData);

   return( TRUE );

}

const uint8_t  URA_Table[11] = { // GPS URAccuracy -> URError [m] Look-up table
               (uint8_t)(    2 ),      //  0 :  RMS = 2^(1 + URA/2)  = 2^1 m
               (uint8_t)(    3 ),      //  1 :  RMS = 2^(1 + URA/2)  = 2^1.5 m
               (uint8_t)(    4 ),      //  2 :  RMS = 2^(1 + URA/2)  = 2^2 m
               (uint8_t)(    6 ),      //  3 :  RMS = 2^(1 + URA/2)  = 2^2.5 m
               (uint8_t)(    8 ),      //  4 :  RMS = 2^(1 + URA/2)  = 2^3 m
               (uint8_t)(   11 ),      //  5 :  RMS = 2^(1 + URA/2)  = 2^3.5 m
               (uint8_t)(   16 ),      //  6 :  RMS = 2^(1 + URA/2) or 2^(URA-2)  = 2^4 m
               (uint8_t)(   32 ),      //  7 :  RMS = 2^(URA-2)  = 2^5 m
               (uint8_t)(   64 ),      //  8 :  RMS = 2^(URA-2)  = 2^6 m
               (uint8_t)(  128 ),      //  9 :  RMS = 2^(URA-2)  = 2^7 m
               (uint8_t)(  256 )  };   // 10 :  RMS = 2^(URA-2)  = 2^8 m
               
/*****************************************************************************************************/
/* GN_EE_Get_GPS_Eph_El :  Get a GN Extended Ephemeris GPS Ephemeris Element */
/* record from the RxNetworks API interface. */
/*****************************************************************************************************/

BL GN_EE_Get_GPS_Eph_El(
   U1 SV,                           // i  - Satellite identification (PRN) number
                                    //         [range 1..32]
   U2 WeekNo,                       // i  - Current full GPS Week No (including roll overs)
                                    //         [range 0..4095]
   U4 TOW_s,                        // i  - Current GPS Time of Week [range 0..604799]
   s_GN_EE_GPS_Eph_El *p_Eph_El )   //  o - Pointer to where the Extended Ephemeris
                                    //         module can write the Ephemeris Elements to.
{
   uint32_t              EE_Ref_GPS_secs;    // EE Ref GPS Seconds since start of GPS Time
   t_lsimee_GetEphemerisReq   *pl_lsimee_ExtendedEph;
   uint16_t             EEClient_status = K_CGPS_EE_SUCCESS;         // API return status code
   uint32_t             v_TimeDiff = 0;


    /*PE is requesting for EE for QZSS but the same is not supported by RXN.Filtering the same here*/
    if( ((FALSE == MC_CGPS_IS_CONFIGURATION_ELEMENT_SET(K_CGPS_CONFIG_EXTENDED_EPHEMERIS_GPS_ENABLED)) &&
        (FALSE == MC_CGPS_IS_CONFIGURATION_ELEMENT_SET(K_CGPS_CONFIG_SELF_ASSISTED_GPS_ENABLED)))
        || (SV > 32) )
    {
        // GPS Extended Ephemeris is not supported so ignore the ephemeris request.
        // Remove compiler Warnings!
        SV       = SV;
        WeekNo   = WeekNo;
        TOW_s    = TOW_s;
        p_Eph_El = p_Eph_El;

        return( FALSE );
    }
       
     MC_CGPS_TRACE(( "GN_EE_Get_GPS_Eph_El:SV requested from library %d",SV));
     MC_CGPS_TRACE(( "GN_EE_Get_GPS_Eph_El:WeekNo =%u,TOW_s = %u",WeekNo,TOW_s));


    // Request for Broadcast Ephemeris or Extended Ephemeris from the EE Client
    pl_lsimee_ExtendedEph = (t_lsimee_GetEphemerisReq*)MC_RTK_GET_MEMORY(sizeof(*pl_lsimee_ExtendedEph)); 

    // We request RxN Extended Ephemerides records to have a centre point
    // half of RXN_EE_UD_INT into the future, so as to minimize errors.

    EE_Ref_GPS_secs  =  (uint32_t)( WeekNo*604800  +  TOW_s )  +  (K_CGPS_EE_UD_INT/2);

    /* callback parameters settings */
    pl_lsimee_ExtendedEph->v_ConstelType = K_LSIMEE_GPS_CONSTEL;

    pl_lsimee_ExtendedEph->v_EERefGPSTime = EE_Ref_GPS_secs;

    pl_lsimee_ExtendedEph->v_PrnBitMask = 0xFFFFFFFF;

    MC_CGPS_TRACE(("GN_EE_Get_GPS_Eph_El v_PrnBitMask = %u,v_ConstelType=%d,v_EERefGPSTime=%u",pl_lsimee_ExtendedEph->v_PrnBitMask,pl_lsimee_ExtendedEph->v_ConstelType,pl_lsimee_ExtendedEph->v_EERefGPSTime));

    if ( ! CGPS11_01EEClientGPSEphemerisCheck( &vg_CGPS_NavDataList.a_GpsList[SV - 1],SV))
    {
        MC_CGPS_TRACE(( "CGPS11_01EEClientGPSEphemerisCheck:  %d  Invalid Ephemeris",SV));

        EEClient_status = K_CGPS_EE_FAIL;
    }
    else
    {
        v_TimeDiff = EE_Ref_GPS_secs - vg_CGPS_NavDataList.EE_Ref_GPS_secs;
        
         MC_CGPS_TRACE(( "CGPS11_01EEClientGPSEphemerisCheck:  vg_CGPS_NavDataList.EE_Ref_GPS_secs=%u,EE_Ref_GPS_secs=%u",vg_CGPS_NavDataList.EE_Ref_GPS_secs,EE_Ref_GPS_secs));
         MC_CGPS_TRACE(( "CGPS11_01EEClientGPSEphemerisCheck:  v_TimeDiff=%u",v_TimeDiff));
         
        /*Checks if cached ephemeris are older than 4 minutes. PGPS expires every 4 minutes.*/
        if(v_TimeDiff > 240)
        {
            MC_CGPS_TRACE(( "CGPS11_01EEClientGPSEphemerisCheck Outdated GPS EE data.Request Again"));
            EEClient_status = K_CGPS_EE_FAIL;
        }
    }
   

   MC_CGPS_TRACE(( "GN_EE_Get_GPS_Eph_El:K_CGPS_EE_FAIL(255)-K_CGPS_EE_SUCCESS(0) %d",EEClient_status));

   
   if ( EEClient_status == K_CGPS_EE_SUCCESS )
   {

      // Validate the RxN URA vs URE.  RxN set the URA as a mid point between two URE's
      // rather than on the pessamistic side.  So, if the URE is larger than the URA
      // implies then increment the URA by 1 step.
      if ( vg_CGPS_NavDataList.a_GpsList[SV - 1].v_Ura <= 9  &&  vg_CGPS_NavDataList.a_GpsList[SV - 1].v_Ure > URA_Table[vg_CGPS_NavDataList.a_GpsList[SV - 1].v_Ura] )
      {
         vg_CGPS_NavDataList.a_GpsList[SV - 1].v_Ura = vg_CGPS_NavDataList.a_GpsList[SV - 1].v_Ura + 1;
      }

      p_Eph_El->SatID      = (U1)vg_CGPS_NavDataList.a_GpsList[SV - 1].v_Prn;          // Satellite ID (PRN)                 :  6 bits [1..63]
      p_Eph_El->CodeOnL2   = (U1)0;                                                      // C/A or P on L2                     :  2 bits [0..3]
      p_Eph_El->URA        = (U1)vg_CGPS_NavDataList.a_GpsList[SV - 1].v_Ura;          // User Range Accuracy Index            :  4 bits [0..15]
      p_Eph_El->SVHealth   = (U1)vg_CGPS_NavDataList.a_GpsList[SV - 1].v_Health;      // Satellite Health Bits                :  6 bits [0..63]
      p_Eph_El->FitIntFlag = (U1)vg_CGPS_NavDataList.a_GpsList[SV - 1].v_EphemFit;   // Fit Interval Flag                    :  1 bit  [0=4hrs, 1=6hrs]
      p_Eph_El->AODA       = (U1)0;                                                     // Age Of Data Offset                 :  5 bits [x 900 sec]
      p_Eph_El->L2Pflag    = (I1)0;                                                     // L2 P Data Flag                     :  1 bit  [0..1]
      p_Eph_El->TGD        = (I1)0;                                                     // Total Group Delay                    :  8 bits [x 2^-31 sec]
      p_Eph_El->af2        = (I1)vg_CGPS_NavDataList.a_GpsList[SV - 1].v_Af2;          // SV Clock Drift Rate                :  8 bits [x 2^-55 sec/sec2]
      p_Eph_El->Week       = (U2)vg_CGPS_NavDataList.a_GpsList[SV - 1].v_GpsWeek;      // GPS Reference Week Number            : 10 bits [0..1023]
      p_Eph_El->toc        = (U2)vg_CGPS_NavDataList.a_GpsList[SV - 1].v_Toc;          // Clock Reference Time of Week        : 16 bits [x 2^4 sec]
      p_Eph_El->toe        = (U2)vg_CGPS_NavDataList.a_GpsList[SV - 1].v_Toe;          // Ephemeris Reference Time of Week    : 16 bits [x 2^4 sec]
      p_Eph_El->IODC       = (U2)vg_CGPS_NavDataList.a_GpsList[SV - 1].v_Iode;          // Issue Of Data Clock                : 10 bits [0..1023]
      p_Eph_El->af1        = (I2)vg_CGPS_NavDataList.a_GpsList[SV - 1].v_Af1;          // SV Clock Drift                     : 16 bits [x 2^-43 sec/sec]
      p_Eph_El->dn         = (I2)vg_CGPS_NavDataList.a_GpsList[SV - 1].v_DeltaN;      // Delta n                            : 16 bits [x 2^-43 semi-circles/sec]
      p_Eph_El->IDot       = (I2)vg_CGPS_NavDataList.a_GpsList[SV - 1].v_IDot;       // Rate of Inclination Angle            : 14 bits [x 2^-43 semi-circles/sec]
      p_Eph_El->Crs        = (I2)vg_CGPS_NavDataList.a_GpsList[SV - 1].v_Crs;          // Coeff-Radius-sine                    : 16 bits [x 2^-5 meters]
      p_Eph_El->Crc        = (I2)vg_CGPS_NavDataList.a_GpsList[SV - 1].v_Crc;          // Coeff-Radius-cosine                : 16 bits [x 2^-5 meters]
      p_Eph_El->Cuc        = (I2)vg_CGPS_NavDataList.a_GpsList[SV - 1].v_Cuc;          // Coeff-Argument_of_Latitude-cosine    : 16 bits [x 2^-29 radians]
      p_Eph_El->Cus        = (I2)vg_CGPS_NavDataList.a_GpsList[SV - 1].v_Cus;          // Coeff-Argument_of_Latitude-sine    : 16 bits [x 2^-29 radians]
      p_Eph_El->Cis        = (I2)vg_CGPS_NavDataList.a_GpsList[SV - 1].v_Cis;          // Coeff-Inclination-sine             : 16 bits [x 2^-29 radians]
      p_Eph_El->Cic        = (I2)vg_CGPS_NavDataList.a_GpsList[SV - 1].v_Cic;          // Coeff-Inclination-cosine            : 16 bits [x 2^-29 radians]
      p_Eph_El->af0        = (I4)vg_CGPS_NavDataList.a_GpsList[SV - 1].v_Af0;          // SV Clock Bias                        : 22 bits [x 2^-31 sec]
      p_Eph_El->M0         = (I4)vg_CGPS_NavDataList.a_GpsList[SV - 1].v_M0;          // Mean Anomaly                        : 32 bits [x 2^-31 semi-circles]
      p_Eph_El->e          = (U4)vg_CGPS_NavDataList.a_GpsList[SV - 1].v_E;           // Eccentricity                        : 32 bits [x 2^-33]
      p_Eph_El->APowerHalf = (U4)vg_CGPS_NavDataList.a_GpsList[SV - 1].v_SqrtA;      // (Semi-Major Axis)^1/2                : 32 bits [x 2^-19 metres^1/2]
      p_Eph_El->Omega0     = (I4)vg_CGPS_NavDataList.a_GpsList[SV - 1].v_Omega0;      // Longitude of the Ascending Node    : 32 bits [x 2^-31 semi-circles]
      p_Eph_El->i0         = (I4)vg_CGPS_NavDataList.a_GpsList[SV - 1].v_I0;          // Inclination angle                    : 32 bits [x 2^-31 semi-circles]
      p_Eph_El->w          = (I4)vg_CGPS_NavDataList.a_GpsList[SV - 1].v_W;           // Argument of Perigee                : 32 bits [x 2^-31 meters]
      p_Eph_El->OmegaDot   = (I4)vg_CGPS_NavDataList.a_GpsList[SV - 1].v_OmegaDot;   // Rate of Right Ascention            : 24 bits [x 2^-43 semi-circles/sec]


      MC_CGPS_TRACE((" GPS EE from MSL to PE : %3u, %1u , %2u , %2u , %1u , %2u , %2d , %4d , %4d , %4u , %5u , %5u , %4u , %6d , %6d , %5d , %6d , %6d , %6d , %6d , %6d , %6d , %8d , %11d , %10u , %10u , %11d , %11d , %11d , %8d",
      p_Eph_El->SatID,p_Eph_El->CodeOnL2,p_Eph_El->URA,p_Eph_El->SVHealth,p_Eph_El->FitIntFlag,p_Eph_El->AODA,p_Eph_El->L2Pflag,p_Eph_El->TGD,p_Eph_El->af2,p_Eph_El->Week,p_Eph_El->toc,p_Eph_El->toe,p_Eph_El->IODC,p_Eph_El->af1,p_Eph_El->dn,p_Eph_El->IDot,p_Eph_El->Crs,p_Eph_El->Crc,p_Eph_El->Cus,p_Eph_El->Cuc,p_Eph_El->Cis,p_Eph_El->Cic,p_Eph_El->af0,p_Eph_El->M0,p_Eph_El->e,p_Eph_El->APowerHalf,p_Eph_El->Omega0,p_Eph_El->i0,p_Eph_El->w,p_Eph_El->OmegaDot));

         return( TRUE );
   }
   else
    {

    memset( p_Eph_El, 0, sizeof(p_Eph_El) );
    p_Eph_El->SatID        = (U1)SV;                    // Satellite ID (PRN)
    p_Eph_El->URA        = (U1)0xFF;                        // 0xFF = Not usable
    p_Eph_El->SVHealth    = (U1)0;                    //     0 = Unhealthy 

    if(CGPS11_05EEClientTimeDiffCheck(vg_CGPS_EEReqTimestamp) > K_CGPS_CUTOFF_TIME)
    {

        MC_CGPS_TRACE(( "GN_EE_Get_GPS_Eph_El: Request made"));
        
        vg_CGPS_NavDataList.EE_Ref_GPS_secs = EE_Ref_GPS_secs;
        
        CGPS4_28SendMsgToProcess(PROCESS_CGPS ,PROCESS_LSIMEE,
            CGPS_LSIMEE_GET_EPHEMERIS_REQ,(t_MsgHeader *)pl_lsimee_ExtendedEph);

        vg_CGPS_EEReqTimestamp = OSA_GetMsCount();
    }

    return( FALSE );
   }


}



BL GN_EE_Get_GLON_Eph_El(
   U1 SV,                           ///< [in] Glonass Satellite Slot Number [range 1..24].
   U1 N4,                           ///< [in] Current Glonass 4yr Block Number [range 1..15].
   U2 NT,                           ///< [in] Current Glonass Day Number [range 1..1461 days within 4 year period].
   U4 TOD_s,                        ///< [in] Current Glonass Time of Day (UTC SU) [range 0..86399 s].
   s_GN_EE_GLON_Eph_El *p_Eph_El    ///< [in] Pointer to destination Glonass Extended Ephemeris data Elements.
)
{

    t_lsimee_GetEphemerisReq   *pl_lsimee_ExtendedEph;
    uint32_t            EE_Ref_GLONASS_secs;
    uint32_t            EE_GLO_secs;
    uint16_t            EE_GLO_Days;    
    uint16_t            EEClient_status = K_CGPS_EE_SUCCESS;         // API return status code
    uint32_t            v_Minutes = 0;
    uint32_t            v_Hours = 0;
    uint32_t            v_CalculatedTODs = 0;
    
    

    if( (FALSE == MC_CGPS_IS_CONFIGURATION_ELEMENT_SET(K_CGPS_CONFIG_EXTENDED_EPHEMERIS_GLONASS_ENABLED)) &&
        (FALSE == MC_CGPS_IS_CONFIGURATION_ELEMENT_SET(K_CGPS_CONFIG_SELF_ASSISTED_GLONASS_ENABLED)))
    {
        // GPS Extended Ephemeris is not supported so ignore the ephemeris request.
        // Remove compiler Warnings!

        SV          = SV;
        N4          = N4;
        NT          = NT;
        TOD_s       = TOD_s;
        p_Eph_El    = p_Eph_El;

        return( FALSE );
    }


    pl_lsimee_ExtendedEph = (t_lsimee_GetEphemerisReq*)MC_RTK_GET_MEMORY(sizeof(*pl_lsimee_ExtendedEph)); 

    MC_CGPS_TRACE(( "GN_EE_Get_GLON_Eph_El:SV requested from library %d",SV));

    MC_CGPS_TRACE(( "N4=%u,NT=%u,TOD_s=%u",N4,NT,TOD_s));

    /*
    GLONASS ephemerides change every 30 minutes,  on the UTC hour and half past the UTC hour.   They have a reference time half way through,  ie quarter past, and quarter to, the UTC hour.
    The Ephemeris reference time "tb" is in units of 15 minutes into the current day UTC Moscow,  range 1..96.      For example,  tb=1 is 12:15 am, tb=3 is 12:45am,  tb=95 is 11:45 pm,   tb=96 is 12;00 pm (midnight UTC Moscow).
     Legal GLONASS ephemerides can only exist on an odd number "tb",  ie quarter past, and quarter to, the UTC hour.*/
    /*
    Ex:-

    Time - 5.25.16
    Total Seconds :-  ((5*60 + 25 )*60)+16 = 19516
    v_Minutes = 19516/60 = 325
    v_Hours = 325/60 = 5
    v_Minutes = 325 - (5*60) = 325-300 = 25

    if(v_Minutes =>0 && v_Minutes =<30) yes, then v_Minutes =15

    Final:-
    Hours = 5
    Minutes = 15
    Calculated TOD_S = ( Hours * 60 + Minutes)*60 

    */

    if(TOD_s !=0 )
    {
        v_Minutes = TOD_s/60;
    }
    
    if(v_Minutes != 0)
    {
        v_Hours = v_Minutes/60;
    }
    
    v_Minutes = v_Minutes - (v_Hours * 60);

    MC_CGPS_TRACE(( "Before Quater Adjustment v_Hours=%u,v_Minutes=%u",v_Hours,v_Minutes));
    
    if(v_Minutes < 30)
        v_Minutes = 15;
    if(v_Minutes >=30 && v_Minutes <=59)
        v_Minutes = 45;

    MC_CGPS_TRACE(( "After Quater Adjustment v_Hours=%u,v_Minutes=%u",v_Hours,v_Minutes));
        
    v_CalculatedTODs = ((v_Hours * 60)+v_Minutes)*60;

    MC_CGPS_TRACE(( "Calculated TODS v_CalculatedTODs=%u, TOD_s=%u",v_CalculatedTODs,TOD_s));

    //EE_Ref_GLONASS_secs                 = ((((N4-1)*1461 + (NT-1))*86400) + (TOD_s)) ; /*No need to multiplyt by 900 i.e 15*60  as TOD_s is in Seconds-not in 15 mins units */
    EE_Ref_GLONASS_secs  = ((((N4-1)*1461 + (NT-1))*86400) + (v_CalculatedTODs)) ; /*No need to multiplyt by 900 i.e 15*60  as TOD_s is in Seconds-not in 15 mins units */

    MC_CGPS_TRACE(( "Calculated GLONASS_secs=%u",EE_Ref_GLONASS_secs));

    /* callback parameters settings */
    pl_lsimee_ExtendedEph->v_ConstelType   = K_LSIMEE_GLO_CONSTEL;    /*Glonass Satellites*/
    pl_lsimee_ExtendedEph->v_EERefGPSTime  = EE_Ref_GLONASS_secs;      /*TODO: suggest to change the naming convetion of v_EERefGPSTime to reflect both gps & GLonass*/
    pl_lsimee_ExtendedEph->v_PrnBitMask    = 0xFFFFFF;                 /*Request for all 24 Glonass Satellites: FFFFFF = 111111111111111111111111*/

    MC_CGPS_TRACE(("GN_EE_Get_GLON_Eph_El v_PrnBitMask = %u,v_ConstelType=%d,v_EERefGPSTime=%u",pl_lsimee_ExtendedEph->v_PrnBitMask,pl_lsimee_ExtendedEph->v_ConstelType,pl_lsimee_ExtendedEph->v_EERefGPSTime));

    if ( ! CGPS11_02EEClientGlonEphemerisCheck( &vg_CGPS_NavDataList.a_GloList[SV - 1],EE_Ref_GLONASS_secs))
    {
       MC_CGPS_TRACE(( "CGPS11_02EEClientGlonEphemerisCheck:  %d  Inavlid Ephemeris",SV));
       
       EEClient_status = K_CGPS_EE_FAIL;
    } 


    MC_CGPS_TRACE(( "GN_EE_Get_GLON_Eph_El :K_CGPS_EE_FAIL(255)-K_CGPS_EE_SUCCESS(0) %d",EEClient_status));


   if ( EEClient_status == K_CGPS_EE_SUCCESS )
   {

      p_Eph_El->gloN = vg_CGPS_NavDataList.a_GloList[SV - 1].v_Slot;
      p_Eph_El->gloFrqCh = vg_CGPS_NavDataList.a_GloList[SV - 1].v_FreqChannel;

      EE_GLO_secs = vg_CGPS_NavDataList.a_GloList[SV - 1].v_GloSec;
      
      MC_CGPS_TRACE(( "Recevied GLONASS_secs from MSL =%u",EE_GLO_secs));
      
      p_Eph_El->gloTb =  (uint8_t)((EE_GLO_secs % 86400)/900);
      EE_GLO_Days = (uint16_t) (EE_GLO_secs / 86400);   
      p_Eph_El->gloNT =  (EE_GLO_Days % 1461) + 1 ;

      p_Eph_El->gloLn = vg_CGPS_NavDataList.a_GloList[SV - 1].v_Bn;
      p_Eph_El->gloFT = vg_CGPS_NavDataList.a_GloList[SV - 1].v_FT;
      p_Eph_El->gloEn = 0;
      p_Eph_El->gloP1 = 0;
      p_Eph_El->gloP2 = 0;
      p_Eph_El->gloM = vg_CGPS_NavDataList.a_GloList[SV - 1].v_M;
      p_Eph_El->gloX = vg_CGPS_NavDataList.a_GloList[SV - 1].v_X;
      p_Eph_El->gloY = vg_CGPS_NavDataList.a_GloList[SV - 1].v_Y;
      p_Eph_El->gloZ = vg_CGPS_NavDataList.a_GloList[SV - 1].v_Z;
      p_Eph_El->gloXdot = vg_CGPS_NavDataList.a_GloList[SV - 1].v_Vx;
      p_Eph_El->gloYdot = vg_CGPS_NavDataList.a_GloList[SV - 1].v_Vy;
      p_Eph_El->gloZdot = vg_CGPS_NavDataList.a_GloList[SV - 1].v_Vz;
      p_Eph_El->gloXdotdot = vg_CGPS_NavDataList.a_GloList[SV - 1].v_Lsx;
      p_Eph_El->gloYdotdot = vg_CGPS_NavDataList.a_GloList[SV - 1].v_Lsy;
      p_Eph_El->gloZdotdot = vg_CGPS_NavDataList.a_GloList[SV - 1].v_Lsz;
      p_Eph_El->gloDeltaTau = 0;
      p_Eph_El->gloTau = vg_CGPS_NavDataList.a_GloList[SV - 1].v_TauN;
      p_Eph_El->gloGamma = vg_CGPS_NavDataList.a_GloList[SV - 1].v_Gamma;

      MC_CGPS_TRACE(( "GLONASS Extended Ephemeris from MSL to PE :    %2d,    %2d,    %u,    %u,    %u,    %u,    %u,    %d,    %u,    %d,    %d,    %d,    %d,    %d,    %d,    %d,    %d,    %d,    %d,    %d,    %d,    %d",
                    p_Eph_El->gloN,p_Eph_El->gloFrqCh,p_Eph_El->gloTb,p_Eph_El->gloNT ,p_Eph_El->gloLn , p_Eph_El->gloFT ,
                    p_Eph_El->gloEn, p_Eph_El->gloP1,p_Eph_El->gloP2,p_Eph_El->gloM,p_Eph_El->gloX ,p_Eph_El->gloY ,p_Eph_El->gloZ,p_Eph_El->gloXdot ,p_Eph_El->gloYdot , p_Eph_El->gloZdot , p_Eph_El->gloXdotdot, p_Eph_El->gloYdotdot,p_Eph_El->gloZdotdot,
                    p_Eph_El->gloDeltaTau,p_Eph_El->gloTau ,p_Eph_El->gloGamma));


         return( TRUE );
   }
    else
    {

        memset( p_Eph_El, 0, sizeof(p_Eph_El) );

        p_Eph_El->gloN        = (U1)SV;                    // Satellite ID (PRN)
        p_Eph_El->gloLn    = (U1)1;                    //     1 = Unhealthy 


        if(CGPS11_05EEClientTimeDiffCheck(vg_CGPS_GLONEEReqTimestamp) > K_CGPS_CUTOFF_TIME)
        {

            MC_CGPS_TRACE(("GN_EE_Get_GLON_Eph_El: Request made"));

            CGPS4_28SendMsgToProcess(PROCESS_CGPS ,
                                     PROCESS_LSIMEE,
                                     CGPS_LSIMEE_GET_EPHEMERIS_REQ,
                                     (t_MsgHeader *)pl_lsimee_ExtendedEph);

            vg_CGPS_GLONEEReqTimestamp = OSA_GetMsCount();
        }



        return( FALSE );
    }

}


#endif


#undef CGPS1LIB_C
