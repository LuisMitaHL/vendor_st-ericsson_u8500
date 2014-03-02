/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/**
* \file cgps4utils.c
* \date 07/04/2008
* \version 1.0
*
* <B>Compiler:</B> ARM ADS\n
*
* <B>Description:</B>  This file contains useful functions for cgps module
*
* <TABLE>
*     <TR>
*             <TD> Date</TD><TD> Author</TD><TD> Description</TD>
*     </TR>
*     <TR>
*             <TD> 07.04.08 </TD><TD> Y.DESAPHI </TD><TD> Creation </TD>
*     </TR>
*     <TR>
*             <TD> 15.05.08 </TD><TD> Y.DESAPHI </TD><TD> Add functions dedicated to SUPL feature </TD>
*     </TR>
* </TABLE>
*/

#ifndef __CGPS4UTILS_C__
#define __CGPS4UTILS_C__

#include "cgpsutils.h"

#include "cgpsmain.hit"

#undef MODULE_NUMBER
#define MODULE_NUMBER MODULE_CGPS

#undef PROCESS_NUMBER
#define PROCESS_NUMBER PROCESS_CGPS

#undef FILE_NUMBER
#define FILE_NUMBER 4

#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 1
/*****************************************************************************/
/* CGPS4_01IsRegistered : check if the handle is registered                  */
/*****************************************************************************/
uint8_t CGPS4_01IsRegistered(uint32_t vp_Handle)
{

    if(s_CgpsNaf[vp_Handle].v_State != K_CGPS_NAF_UNUSED)
            return TRUE;


    return FALSE;
}

#ifdef AGPS_UP_FTR

#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 2
/*********************************************************************************************************/
/* CGPS4_02ConvertSuplNotificationType : converts e_GN_SUPL_Notify_Type into e_cgps_SuplNotificationType */
/*********************************************************************************************************/
e_cgps_NotificationType CGPS4_02ConvertSuplNotificationType(e_GN_SUPL_Notify_Type vp_GN_SUPL_Notify_Type)
{
    switch (vp_GN_SUPL_Notify_Type)
    {
    case NOTIFY_NONE:
        return K_CGPS_NO_NOTIFICATION_NO_VERIFICATION;
    case NOTIFY_ONLY:
        return K_CGPS_NOTIFICATION_ONLY;
    case NOTIFY_ALLOWED_ON_TIMEOUT:
        return K_CGPS_NOTIFICATION_AND_VERIFICATION_ALLOWED_NA;
    case NOTIFY_DENIED_ON_TIMEOUT:
        return K_CGPS_NOTIFICATION_AND_VERIFICATION_DENIED_NA;
    default:
        return K_CGPS_NOTIFICATION_ONLY;
    }
}

#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 3
/*****************************************************************************************/
/* CGPS4_03ConvertEncodingType : converts e_GN_SUPL_Notify_Type into e_cgps_EncodingType */
/*****************************************************************************************/
e_cgps_EncodingType CGPS4_03ConvertEncodingType(e_GN_SUPL_Encoding_Type vp_GN_SUPL_Encoding_Type)
{
    switch (vp_GN_SUPL_Encoding_Type)
    {
    case ENCODING_NONE:
        return K_CGPS_NONE;
    case ENCODING_ucs2:
        return K_CGPS_UCS2;
    case ENCODING_gsmDefault:
        return K_CGPS_GSM_DEFAULT;
    case ENCODING_utf8:
        return K_CGPS_UTF8;
    default:
        return K_CGPS_NONE;
    }
}


#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 4
/**************************************************************************************************/
/* CGPS4_04ConvertFormatIndicator : converts e_GN_SUPL_Notify_Fmt_Ind into e_cgps_FormatIndicator */
/**************************************************************************************************/
e_cgps_FormatIndicator CGPS4_04ConvertFormatIndicator(e_GN_SUPL_Notify_Fmt_Ind vp_GN_SUPL_Notify_Fmt_Ind)
{
    switch (vp_GN_SUPL_Notify_Fmt_Ind)
    {
    case NOTIFY_noFormat:
        return K_CGPS_NO_FORMAT;
    case NOTIFY_logicalName:
        return K_CGPS_LOGICAL_NAME;
    case NOTIFY_e_mailAddress:
        return K_CGPS_EMAIL_ADDRESS;
    case NOTIFY_msisdn:
        return K_CGPS_MSISDN;
    case NOTIFY_url:
        return K_CGPS_URL;
    case NOTIFY_sipUrl:
        return K_CGPS_SIP_URL;
    case NOTIFY_min:
        return K_CGPS_MIN;
    case NOTIFY_mdn:
        return K_CGPS_MDN;
    case NOTIFY_imsPublicIdentity:
        return K_CGPS_IMS_PUBLIC_IDENTITY;
    default:
        return K_CGPS_NO_FORMAT;
    }
}

#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 6
/*****************************************************************************/
/* CGPS4_06ReadString : read a line from pl_File and store it into p_buffer  */
/*****************************************************************************/
void CGPS4_06ReadString(char ** pp_buffer, void* pp_File)
{
    char vl_char;
    char pl_TempBuffer[1024];
    uint8_t vl_index = 0;
    uint32_t vl_count=0;

    /* read the first character */
    vl_count = MC_GFL_FREAD(&vl_char, 1, sizeof(char),(t_gfl_FILE *)pp_File);

    if (vl_count != 0)
    {
        while ((vl_char != 0x0D) && (vl_char != 0x0A) && (vl_count != 0))
        {
            /* store the character */
            pl_TempBuffer[vl_index] = vl_char;
            vl_index++;

            /* read the next character */
            vl_count = MC_GFL_FREAD(&vl_char, 1, sizeof(char),(t_gfl_FILE *)pp_File);
        }
        if (vl_char == 0x0D)
        {
            /* read the 0x0A character */
            vl_count = MC_GFL_FREAD(&vl_char, 1, sizeof(char),(t_gfl_FILE *)pp_File);
        }

        if (vl_index != 0)
        {
            /* finalize the string */
            pl_TempBuffer[vl_index] = '\0';

            /* copy temporary buffer to the pp_Buffer */
            pp_buffer[0] = (char*)MC_RTK_GET_MEMORY(sizeof(char)*(vl_index+1));
            strncpy((char*)pp_buffer[0], pl_TempBuffer, vl_index+1);
        }
        else
        {
            *pp_buffer = NULL;
        }
    }
    else
    {
        *pp_buffer = NULL;
    }

}
#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 7
/**************************************************************************************************/
/* CGPS4_07ConvertTcpIpAddressType : converts e_TcpIp_AddressType into t_lsimup_AddressType          */
/**************************************************************************************************/
t_lsimup_AddressType CGPS4_07ConvertTcpIpAddressType(e_TcpIp_AddressType vp_GN_SUPL_TcpIp_AddressType)
{
    switch (vp_GN_SUPL_TcpIp_AddressType)
    {
    case IPV4_Address:
        return K_LSIMUP_IPV4_ADDRESS;
    case IPV6_Address:
        return K_LSIMUP_IPV6_ADDRESS;
    case FQDN_Address:
        return K_LSIMUP_URL_ADDRESS;
        case IP_None :
        default :
        break;
        /* No other address type matching with LSIMUP */
    }

    return K_LSIMUP_IPV4_ADDRESS; /* just to avoid warning during compilation */
}

#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 8
/******************************************************************************************************************/
/* CGPS4_08GetNMRList : gets NMR list from a chain list and stores it to an array of s_NMRElement (SUPL lib type) */
/******************************************************************************************************************/
uint8_t CGPS4_08GetNMRList(s_NMRElement ** pp_NMRList, t_lsimup_NMR * pp_NMR, uint32_t vp_nbCells)
{
    uint8_t vl_index = 0;
    t_lsimup_NMR * pl_element=NULL;
    s_NMRElement * p_Temp=NULL;

    if (vp_nbCells == 0)
    {
        *pp_NMRList = NULL;
        return 0;
    }

    (*pp_NMRList) = (s_NMRElement*)MC_RTK_GET_MEMORY(sizeof(* p_Temp) * vp_nbCells );

    p_Temp = (*pp_NMRList);
    for (vl_index = 0, pl_element = pp_NMR; vl_index < vp_nbCells ; vl_index++, pl_element = pl_element->p_NextNeighbour,p_Temp++)
    {
        p_Temp->aRFCN = pl_element->s_ARFCN;
        p_Temp->bSIC = pl_element->s_BSIC;
        p_Temp->rxLev = pl_element->s_RXLev;
    }

    return vl_index;
}

#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 9
/***********************************************************************************************************************************************/
/* CGPS4_09GetMeasuredResultsList : gets measured results list from a chain list and stores it to an array of s_MeasuredResult (SUPL lib type) */
/***********************************************************************************************************************************************/
uint8_t CGPS4_09GetMeasuredResultsList(s_MeasuredResult ** pp_MeasuredResultsList, t_lsimup_MeasuredResultsList * pp_lsimup_MeasuredResult, uint32_t vp_nbCells,uint8_t vp_CellType )
{
    uint8_t vl_index = 0;
    t_lsimup_MeasuredResultsList * pl_element=NULL;
    s_MeasuredResult *p_Temp=NULL;

    if (vp_nbCells == 0)
    {
        *pp_MeasuredResultsList = NULL;
        return 0;
    }

    ( * pp_MeasuredResultsList )  = (s_MeasuredResult*)MC_RTK_GET_MEMORY(sizeof(*p_Temp) * vp_nbCells);
    p_Temp = * pp_MeasuredResultsList;

    for (vl_index = 0, pl_element = pp_lsimup_MeasuredResult; vl_index < vp_nbCells; vl_index++,pl_element = pl_element->p_NextMeasuredResultsList , p_Temp++ )
    {
        p_Temp->FrequencyInfoPresent = TRUE;

        if (vp_CellType == K_LSIMUP_WCDMA_CELL)
        {
            /* ffd is a value specify in the Glonav library */
            p_Temp->FrequencyInfo.modeSpecificInfoType = fdd;
            p_Temp->FrequencyInfo.of_type.fdd.uarfcn_UL = pl_element->v_freqInfo.v_ULArfcnPresent ? (int16_t)pl_element->v_freqInfo.v_ULArfcn : -1 ;
            p_Temp->FrequencyInfo.of_type.fdd.uarfcn_DL = pl_element->v_freqInfo.v_DLArfcn;
        }
        else
        {
            /* tdd is a value specify in the Glonav library */
            p_Temp->FrequencyInfo.modeSpecificInfoType = tdd;
            /*FIXME need to verify if uarfcn_Nt is set by SPVNET */
            p_Temp->FrequencyInfo.of_type.tdd.uarfcn_Nt = pl_element->v_freqInfo.v_DLArfcn;
            MC_CGPS_TRACE(("CGPS4_09GetMeasuredResultsList Cell info Cell N%i : uarfcn_Nt : %i",vl_index,p_Temp->FrequencyInfo.of_type.tdd.uarfcn_Nt));
        }

        if (pl_element->v_UTRACarrierRSS<=127)
        {
            p_Temp->utra_CarrierRSSI = (int8_t)pl_element->v_UTRACarrierRSS;
               MC_CGPS_TRACE(("CGPS4_09GetMeasuredResultsList Cell info Cell N%i : RSSI : %i",vl_index,p_Temp->utra_CarrierRSSI));
        }
        else
        {
            /* out of range */
            p_Temp->utra_CarrierRSSI = -1;
            MC_CGPS_TRACE(("WARNING value OUT OF RANGE CGPS4_09GetMeasuredResultsList Cell info  N%i : RSSI : %i",vl_index,p_Temp->utra_CarrierRSSI));
        }
        p_Temp->CellMeasuredResultCount = CGPS4_14GetWCDMACellMeasure( &( p_Temp->p_CellMeasuredResultsList ), pl_element->p_cellMeasuredResultsList, pl_element->v_NumberOfCellsInUARFCN,vp_CellType);
    }

    return vl_index;
}
#endif /* AGPS_UP_FTR */


#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 10
/*************************************************************************************************/
/* CGPS4_10SetLbsHostVersion : Sets the host version into GPS library contained in global variable. */
/* Note : The version information in vg_Lbs_Host_Version is set with this API. Caller needs to take care of validity of vg_Lbs_Host_Version  */
/*************************************************************************************************/
void CGPS4_10SetLbsHostVersion( )
{
    char pl_LbsHostVer[GN_GPS_SIZE_VER];

    memcpy( pl_LbsHostVer, vg_Lbs_Host_Version, GN_GPS_SIZE_VER );

    GN_GPS_Set_HostVersion( pl_LbsHostVer );
}


#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 11
/*************************************************************************************************/
/* CGPS4_11SendAcknowledge : sends a PGNV acknowledge to all registered applications             */
/*************************************************************************************************/
void CGPS4_11SendAcknowledge(const char* pp_Ack)
{
    uint8_t vl_index;

    for (vl_index = 0; vl_index < K_CGPS_MAX_NUMBER_OF_NAF; vl_index++)
    {
        if (s_CgpsNaf[vl_index].v_State == K_CGPS_NAF_READY && ((s_CgpsNaf[vl_index].v_OutputType == K_CGPS_NMEA) || (s_CgpsNaf[vl_index].v_OutputType == K_CGPS_NMEA_AND_C_STRUCT)))
        {
            t_cgps_NavData vl_NavData;
            vl_NavData.v_Type = K_CGPS_NMEA;
            vl_NavData.v_Length = strlen(pp_Ack);
            vl_NavData.v_NavStatus = K_CGPS_UPDATES_REMAINING;
            vl_NavData.p_NavData = (char*)MC_RTK_GET_MEMORY(vl_NavData.v_Length+1); /* +1 for NULL character */
            strncpy(vl_NavData.p_NavData, pp_Ack, vl_NavData.v_Length+1);

            if(s_CgpsNaf[vl_index].p_Callback != NULL)
            {
                (s_CgpsNaf[vl_index].p_Callback)( vl_NavData );
            }

            MC_RTK_FREE_MEMORY(vl_NavData.p_NavData);
        }
    }
}

#ifdef AGPS_UP_FTR

#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 13
/***************************************************************************************/
/* CGPS4_13DeleteSuplContext : delete the context dedicated to an external application */
/***************************************************************************************/
void CGPS4_13DeleteSuplContext(void* vp_Handle)
{
    MC_CGPS_TRACE(("Deleting Context for supl session handle = 0x%x",((t_CgpsSuplAppli*)vp_Handle)->v_GPSHandle ));

    ((t_CgpsSuplAppli*)vp_Handle)->v_IsRegistered                     = FALSE;
    ((t_CgpsSuplAppli*)vp_Handle)->v_GPSHandle                        = (void*)NULL;
    ((t_CgpsSuplAppli*)vp_Handle)->v_ConnectionHandle                 = 0;
    ((t_CgpsSuplAppli*)vp_Handle)->v_ConnectionParm.Port              = 0;
    ((t_CgpsSuplAppli*)vp_Handle)->v_ConnectionParm.TcpIp_AddressType = IP_None;
    ((t_CgpsSuplAppli*)vp_Handle)->v_bearer                           = NULL;
    ((t_CgpsSuplAppli*)vp_Handle)->v_Suspended                        = CGPS_MAX_PRIORITY_VALUE;
    ((t_CgpsSuplAppli*)vp_Handle)->v_PrevPositionStatus               = K_CGPS_POSITION_UNKNOWN;
    ((t_CgpsSuplAppli*)vp_Handle)->v_PrevCellIdPositionStatus         = K_CGPS_POSITION_UNKNOWN;
    ((t_CgpsSuplAppli*)vp_Handle)->v_TriggeredEventType               = K_CGPS_TRIGGER_NONE;

    if (((t_CgpsSuplAppli*)vp_Handle)->v_ConnectionParm.p_TcpIp_Address != NULL)
    {
        MC_RTK_FREE_MEMORY(((t_CgpsSuplAppli*)vp_Handle)->v_ConnectionParm.p_TcpIp_Address);
        ((t_CgpsSuplAppli*)vp_Handle)->v_ConnectionParm.p_TcpIp_Address = NULL;
    }

    vg_CGPS_Supl_Count--;

    CGPS4_20CloseSessionOperatorRules(CGPS_SUPL_APPLICATION_ID);
}

#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 14
/************************************************************************************************************************************************************/
/* CGPS4_14GetCellMeasuredResultsList : gets WCDMAcell measured results list from a chain list and stores it to an array of s_CellMeasuredResult (SUPL lib type) */
/************************************************************************************************************************************************************/
uint8_t CGPS4_14GetWCDMACellMeasure(s_CellMeasuredResult ** pp_CellMeasuredResultsList, t_lsimup_cellMeasuredResultsList * pp_lsimup_CellMeasuredResult, uint32_t vp_nbCells,uint8_t vp_CellType)
{
    s_CellMeasuredResult* p_Temp=NULL;
    uint8_t vl_index = 0;
    t_lsimup_cellMeasuredResultsList * pl_element=NULL;

    MC_CGPS_TRACE(("CGPS4_14GetWCDMACellMeasure"));

    if (vp_nbCells == 0)
    {
        *pp_CellMeasuredResultsList = NULL;
        return 0;
    }

    (* pp_CellMeasuredResultsList ) = (s_CellMeasuredResult*)MC_RTK_GET_MEMORY(sizeof(* p_Temp) * vp_nbCells);
    p_Temp = (* pp_CellMeasuredResultsList );

    for (vl_index = 0, pl_element = pp_lsimup_CellMeasuredResult; vl_index < vp_nbCells; vl_index++, pl_element = pl_element->p_NextCellMeasuredResultsList , p_Temp++)
    {
        p_Temp->cellIdentity = pl_element->v_CellIdPresent ? (int32_t)pl_element->v_CellId : -1;

        if (vp_CellType == K_LSIMUP_WCDMA_CELL)
        {
            p_Temp->modeSpecificInfoType = fdd;
            /* The following section of code works around the problem with path loss being out of valid range */
            /* FIXME: USE only for the IOT, this code must be removed after IOT */
            /* This value must be check in Stack or in lsimup, it's not at CGPS to do these tests*/
            if ((pl_element->v_CPICH_Bitmap & 0x1) && (pl_element->v_CPICH_PathLoss >= 46) && (pl_element->v_CPICH_PathLoss <= 173))
            {
                    p_Temp->of_type.fdd.pathloss = (uint8_t)pl_element->v_CPICH_PathLoss;
            }
            else
            {
                MC_CGPS_TRACE(("WARNING value OUT OF RANGE CGPS4_14GetWCDMACellMeasure Cell N%i : pathloss : %i",vl_index,p_Temp->of_type.fdd.pathloss));
                p_Temp->of_type.fdd.pathloss = 0;
            }

            /* A hack to get around invalid values recvd from stack in Ericsson IOT */
            if( ( pl_element->v_CPICH_Bitmap & 0x2 ) && ( pl_element->v_CPICH_EcNo <= 63 ) )
            {
                p_Temp->of_type.fdd.cpich_Ec_N0 = (int8_t)pl_element->v_CPICH_EcNo;
            }
            else
            {
                p_Temp->of_type.fdd.cpich_Ec_N0 = -1;
            }
            if( ( pl_element->v_CPICH_Bitmap & 0x4 ) && ( pl_element->v_CPICH_RSCP <= 127 ) )
            {
                p_Temp->of_type.fdd.cpich_RSCP = (int8_t)pl_element->v_CPICH_RSCP ;
            }
            else
            {
                p_Temp->of_type.fdd.cpich_RSCP = -1;
            }
            p_Temp->of_type.fdd.PrimaryScramblingCode = (uint16_t) pl_element->v_ScramblingCode;
        }
        else
        {
            p_Temp->modeSpecificInfoType = tdd;
            p_Temp->of_type.tdd.cellParametersID = pl_element->v_ScramblingCode;

            MC_CGPS_TRACE(("CGPS4_14GetWCDMACellMeasure Cell info Cell N%i : ID : %i",vl_index,p_Temp->of_type.tdd.cellParametersID));

            if((pl_element->v_CPICH_EcNo<= 14))
         {
            p_Temp->of_type.tdd.proposedTGSN= pl_element->v_CPICH_EcNo;
            }
            else
            {
                p_Temp->of_type.tdd.proposedTGSN= -1;
            }
            MC_CGPS_TRACE(("CGPS4_14GetWCDMACellMeasure Cell info Cell N%i : proposedTGSN : %i",vl_index,p_Temp->of_type.tdd.proposedTGSN));

            if((pl_element->v_CPICH_RSCP<= 127) && (pl_element->v_CPICH_Bitmap & 0x2))
            {
                p_Temp->of_type.tdd.primaryCCPCH_RSCP= pl_element->v_CPICH_RSCP;
            }
            else
            {
                p_Temp->of_type.tdd.primaryCCPCH_RSCP= -1;
            }
            MC_CGPS_TRACE(("CGPS4_14GetWCDMACellMeasure Cell info Cell N%i : primaryCCPCH_RSCP : %i",vl_index,p_Temp->of_type.tdd.primaryCCPCH_RSCP));

            if((pl_element->v_CPICH_PathLoss>= 46) && (pl_element->v_CPICH_PathLoss<= 173) && (pl_element->v_CPICH_Bitmap & 0x2))
            {
                p_Temp->of_type.tdd.pathloss= pl_element->v_CPICH_PathLoss;
            }
            else
            {
                p_Temp->of_type.tdd.pathloss= 0;
            }
            MC_CGPS_TRACE(("CGPS4_14GetWCDMACellMeasure Cell info Cell N%i : pathloss : %i",vl_index,p_Temp->of_type.tdd.pathloss));

            if ((pl_element->v_CPICH_Bitmap & 0x4) && (pl_element->v_TimeslotISCPList.v_NumTimeslotISCP!=0))
            {
                p_Temp->of_type.tdd.p_TimeslotISCP_List = (uint8_t*)MC_RTK_GET_MEMORY(sizeof(pl_element->v_TimeslotISCPList.v_NumTimeslotISCP));
                memcpy(p_Temp->of_type.tdd.p_TimeslotISCP_List,pl_element->v_TimeslotISCPList.p_TimeslotISCPList,pl_element->v_TimeslotISCPList.v_NumTimeslotISCP);
                p_Temp->of_type.tdd.timeslotISCP_Count = pl_element->v_TimeslotISCPList.v_NumTimeslotISCP;
            }
            else
            {
                p_Temp->of_type.tdd.timeslotISCP_Count = 0;
            }
            MC_CGPS_TRACE(("CGPS4_14GetWCDMACellMeasure Cell info Cell N%i : timeslotISCP_Count : %i",vl_index,p_Temp->of_type.tdd.timeslotISCP_Count));
        }
    }


    return vl_index;
}

#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 15
/************************************************************************************************************************************************************/
/* CGPS4_14GetCellMeasuredResultsList : gets TDSCDMA cell measured results list from a chain list and stores it to an array of s_CellMeasuredResult (SUPL lib type) */
/************************************************************************************************************************************************************/
uint8_t CGPS4_15GetTDSCDMACellMeasure(s_CellMeasuredResult ** pp_CellMeasuredResultsList, t_lsimup_cellMeasuredResultsList * pp_lsimup_CellMeasuredResult, uint32_t vp_nbCells)
{
    s_CellMeasuredResult* p_Temp=NULL;
    uint8_t vl_index = 0;
    t_lsimup_cellMeasuredResultsList * pl_element=NULL;

    if (vp_nbCells == 0)
    {
        *pp_CellMeasuredResultsList = NULL;
        return 0;
    }

    (* pp_CellMeasuredResultsList ) = (s_CellMeasuredResult*)MC_RTK_GET_MEMORY(sizeof(* p_Temp) * vp_nbCells);
    p_Temp = (* pp_CellMeasuredResultsList );

    for (vl_index = 0, pl_element = pp_lsimup_CellMeasuredResult; vl_index < vp_nbCells; vl_index++, pl_element = pl_element->p_NextCellMeasuredResultsList , p_Temp++)
    {
        p_Temp->cellIdentity = pl_element->v_CellIdPresent ? (int32_t)pl_element->v_CellId : -1;
        p_Temp->modeSpecificInfoType = fdd;
        /* The following section of code works around the problem with path loss being out of valid range */
        /* FIXME: USE only for the IOT, this code must be removed after IOT */
        /* This value must be check in Stack or in lsimup, it's not at CGPS to do these tests*/
        if ( pl_element->v_CPICH_Bitmap && 0x1 )
        {
            if ( pl_element->v_CPICH_PathLoss < 46 )
            {
                p_Temp->of_type.fdd.pathloss = 0;
            }
            else if ( pl_element->v_CPICH_PathLoss > 173 )
            {
                p_Temp->of_type.fdd.pathloss = 0;
            }
            else
            {
                p_Temp->of_type.fdd.pathloss = (uint8_t)pl_element->v_CPICH_PathLoss;
            }
        }
        else
        {
            p_Temp->of_type.fdd.pathloss = 0;
        }
        /* When the pathloss issue is resolved, remove the above code and replace with the following line of code */
        /* p_Temp->of_type.fdd.pathloss = ( pl_element->v_CPICH_Bitmap && 0x1 ) ? (uint8_t)pl_element->v_CPICH_PathLoss : 0 ; */
        p_Temp->of_type.fdd.cpich_Ec_N0 = ( pl_element->v_CPICH_Bitmap && 0x2 ) ? (int8_t)pl_element->v_CPICH_EcNo : -1 ;
        p_Temp->of_type.fdd.cpich_RSCP = ( pl_element->v_CPICH_Bitmap && 0x4 ) ?(int8_t)pl_element->v_CPICH_RSCP : -1 ;
        p_Temp->of_type.fdd.PrimaryScramblingCode = (uint16_t) pl_element->v_ScramblingCode;
    }


    return vl_index;
}

#endif /* AGPS_UP_FTR */



#ifdef  AGPS_FTR

#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 16
/*************************************************************************************************/
/* CGPS4_16GetShapeType : return the shape type according to available fields                    */
/*************************************************************************************************/
uint8_t CGPS4_16GetShapeType(t_lsimcp_GpsPosition* pp_GpsPos)
{
    uint8_t vl_ShapeType = LSIMCP_EP; /* we assume that latitude and longitude are available */

    if (pp_GpsPos->v_UncertSemiMajor != LSIMCP_U8_UNKNOWN)
        vl_ShapeType = LSIMCP_EP_UNC_CIRCLE;

    if (pp_GpsPos->v_UncertSemiMajor != LSIMCP_U8_UNKNOWN && pp_GpsPos->v_UncertSemiMinor != LSIMCP_U8_UNKNOWN
            && pp_GpsPos->v_UncertAltitude != LSIMCP_U8_UNKNOWN && pp_GpsPos->v_AxisAngle != LSIMCP_U8_UNKNOWN
            && pp_GpsPos->v_Confidence != LSIMCP_U8_UNKNOWN)
        vl_ShapeType = LSIMCP_EP_UNC_ELLIP;

    if (pp_GpsPos->v_DirectOfAlt != LSIMCP_U8_UNKNOWN)
        vl_ShapeType = LSIMCP_EP_ALT;

    if (pp_GpsPos->v_DirectOfAlt != LSIMCP_U8_UNKNOWN && pp_GpsPos->v_UncertSemiMajor != LSIMCP_U8_UNKNOWN
            && pp_GpsPos->v_UncertSemiMinor != LSIMCP_U8_UNKNOWN && pp_GpsPos->v_UncertAltitude != LSIMCP_U8_UNKNOWN
            && pp_GpsPos->v_AxisAngle != LSIMCP_U8_UNKNOWN && pp_GpsPos->v_Confidence != LSIMCP_U8_UNKNOWN)
        vl_ShapeType = LSIMCP_EP_ALT_UNC_ELLIP;

    if (pp_GpsPos->v_InnerRadius != LSIMCP_U32_UNKNOWN && pp_GpsPos->v_UncertSemiMajor != LSIMCP_U8_UNKNOWN
            && pp_GpsPos->v_OffsetAngle != LSIMCP_U8_UNKNOWN && pp_GpsPos->v_IncludedAngle != LSIMCP_U8_UNKNOWN
            && pp_GpsPos->v_Confidence != LSIMCP_U8_UNKNOWN)
        vl_ShapeType = LSIMCP_EP_ARC;

    return vl_ShapeType;
}

#endif /* AGPS_FTR */

#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 17
/*************************************************************************************************/
/* CGPS4_17UpdateOperatorID : update the operator ID                    */
/*************************************************************************************************/
void CGPS4_17UpdateOperatorID(uint16_t vp_Mcc,uint16_t vp_Mnc)
{
    uint8_t vl_index;
    uint8_t vl_found=FALSE;

    //search the MCC ad MNC in the operator rule
    for (vl_index =0; vl_index < CGPS_MAX_OPERATOR_IN_RULE_TAB_SIZE;vl_index++ )
    {
        if ((vg_CgpsCpOperatorRules[vl_index].v_CgpsCpMCC==vp_Mcc) &&
                (vg_CgpsCpOperatorRules[vl_index].v_CgpsCpMNC==vp_Mnc))
        {
            /* find the good operator update the Index*/
            vg_CGPS_CurrentOperator = vl_index;
            vl_found = TRUE;
        }
    }
    if (!vl_found)
    {
        /*Set with default parameters*/
        vg_CGPS_CurrentOperator = CGPS_DEFAULT_OPERATOR_INDEX;
    }
}

#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 18
/*************************************************************************************************/
/* CGPS4_18CheckNewSessionRight : Check if a new session can be launch or not                                     */
/*************************************************************************************************/
uint8_t CGPS4_18CheckNewSessionRight(e_cgps_cp_application_id vp_ApllicationType)
{
    uint8_t vl_return=TRUE;

#ifdef AGPS_FTR
    if (vg_IsEmergencyCall)
    {
        /* emergency call running, start new session is forbidden */
        vl_return = FALSE;
    }
    else
    {
#endif
        if (vp_ApllicationType & CGPS_AUTONOMOUS_APPLICATION_ID)
        {
            if (vg_CGPS_Naf_Count>0)
            {
                /*At least one application running*/
                if (!(vg_CgpsCpOperatorRules[vg_CGPS_CurrentOperator].v_newSession & CGPS_NEW_SESSION_MASK_AUTONOMOUS_VS_AUTONOMOUS_RESTRICTION))
                {
                    vl_return = FALSE;
                }
            }
#ifdef AGPS_UP_FTR
            if (vg_CGPS_Supl_Count>0)
            {
                /* At least one SUPL session is opened*/
                if (!(vg_CgpsCpOperatorRules[vg_CGPS_CurrentOperator].v_newSession & CGPS_NEW_SESSION_MASK_SUPL_VS_AUTONOMOUS_RESTRICTION))
                {
                    vl_return = FALSE;
                }
            }
#endif
#ifdef AGPS_FTR
            if (vg_CGPS_Cp_Count> 0)
            {
                /* At least one CP session is opened*/
                if (!(vg_CgpsCpOperatorRules[vg_CGPS_CurrentOperator].v_newSession & CGPS_NEW_SESSION_MASK_CPNI_VS_AUTONOMOUS_RESTRICTION))
                {
                    vl_return = FALSE;
                }
            }
            if (vg_CGPS_Cp_Mo_Count> 0)
            {
                if (!(vg_CgpsCpOperatorRules[vg_CGPS_CurrentOperator].v_newSession & CGPS_NEW_SESSION_MASK_CPMO_VS_AUTONOMOUS_RESTRICTION))
                {
                    vl_return = FALSE;
                }
            }
#endif
        }
#ifdef AGPS_UP_FTR
        if (vp_ApllicationType & CGPS_SUPL_APPLICATION_ID)
        {
            if (vg_CGPS_Naf_Count>0)
            {
                /*At least one application running*/
                if (!(vg_CgpsCpOperatorRules[vg_CGPS_CurrentOperator].v_newSession & CGPS_NEW_SESSION_MASK_AUTONOMOUS_VS_SUPL_RESTRICTION))
                {
                    vl_return =  FALSE;
                }
            }
            if (vg_CGPS_Supl_Count>0)
            {
                /* At least one SUPL session is opened*/
                if (!(vg_CgpsCpOperatorRules[vg_CGPS_CurrentOperator].v_newSession & CGPS_NEW_SESSION_MASK_SUPL_VS_SUPL_RESTRICTION))
                {
                    vl_return =  FALSE;
                }
            }
#ifdef AGPS_FTR
            if (vg_CGPS_Cp_Count> 0)
            {
                /* At least one CP session is opened*/
                if (!(vg_CgpsCpOperatorRules[vg_CGPS_CurrentOperator].v_newSession & CGPS_NEW_SESSION_MASK_CPNI_VS_SUPL_RESTRICTION))
                {
                    vl_return =  FALSE;
                }
            }
            if (vg_CGPS_Cp_Mo_Count> 0)
            {
                if (!(vg_CgpsCpOperatorRules[vg_CGPS_CurrentOperator].v_newSession & CGPS_NEW_SESSION_MASK_CPMO_VS_SUPL_RESTRICTION))
                {
                    vl_return =  FALSE;
                }
            }
#endif
        }
#endif /* AGPS_UP_FTR */
#ifdef AGPS_FTR
        if (vp_ApllicationType & CGPS_CP_MO_APPLICATION_ID)
        {
            if (vg_CGPS_Naf_Count>0)
            {
                /*At least one application running*/
                if (!(vg_CgpsCpOperatorRules[vg_CGPS_CurrentOperator].v_newSession & CGPS_NEW_SESSION_MASK_AUTONOMOUS_VS_CPMO_RESTRICTION))
                {
                    vl_return =  FALSE;
                }
            }
#ifdef AGPS_UP_FTR
            if (vg_CGPS_Supl_Count>0)
            {
                /* At least one SUPL session is opened*/
                if (!(vg_CgpsCpOperatorRules[vg_CGPS_CurrentOperator].v_newSession & CGPS_NEW_SESSION_MASK_SUPL_VS_CPMO_RESTRICTION))
                {
                    vl_return =  FALSE;
                }
            }
#endif
            if (vg_CGPS_Cp_Count> 0)
            {
                /* At least one CP session is opened*/
                if (!(vg_CgpsCpOperatorRules[vg_CGPS_CurrentOperator].v_newSession & CGPS_NEW_SESSION_MASK_CPNI_VS_CPMO_RESTRICTION))
                {
                    vl_return =  FALSE;
                }
            }
            if (vg_CGPS_Cp_Mo_Count> 0)
            {
                if (!(vg_CgpsCpOperatorRules[vg_CGPS_CurrentOperator].v_newSession & CGPS_NEW_SESSION_MASK_CPMO_VS_CPMO_RESTRICTION))
                {
                    vl_return =  FALSE;
                }
            }
        }
        if (vp_ApllicationType & CGPS_CP_NI_APPLICATION_ID)
        {
            if (vg_CGPS_Naf_Count>0)
            {
                /*At least one application running*/
                if (!(vg_CgpsCpOperatorRules[vg_CGPS_CurrentOperator].v_newSession & CGPS_NEW_SESSION_MASK_AUTONOMOUS_VS_CPNI_RESTRICTION))
                {
                    vl_return =  FALSE;
                }
            }
#ifdef AGPS_UP_FTR
            if (vg_CGPS_Supl_Count>0)
            {
                /* At least one SUPL session is opened*/
                if (!(vg_CgpsCpOperatorRules[vg_CGPS_CurrentOperator].v_newSession & CGPS_NEW_SESSION_MASK_SUPL_VS_CPNI_RESTRICTION))
                {
                    vl_return =  FALSE;
                }
            }
#endif
            if (vg_CGPS_Cp_Count> 0)
            {
                /* At least one CP session is opened*/
                if (!(vg_CgpsCpOperatorRules[vg_CGPS_CurrentOperator].v_newSession & CGPS_NEW_SESSION_MASK_CPNI_VS_CPNI_RESTRICTION))
                {
                    vl_return =  FALSE;
                }
            }
            if (vg_CGPS_Cp_Mo_Count> 0)
            {
                if (!(vg_CgpsCpOperatorRules[vg_CGPS_CurrentOperator].v_newSession & CGPS_NEW_SESSION_MASK_CPMO_VS_CPNI_RESTRICTION))
                {
                    vl_return =  FALSE;
                }
            }
        }
#endif /* AGPS_FTR */
#ifdef AGPS_FTR
    }
#endif
    return vl_return;
}

#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 19
/************************************************************************************************************/
/* CGPS4_19UpdateOtherApplicationState : Suspend old application if it's needed, depending which new application must be launch */
/************************************************************************************************************/
void CGPS4_19UpdateOtherApplicationState(e_cgps_cp_application_id vp_newApplicationId)
{
    /* */

    switch (vp_newApplicationId)
    {
    case CGPS_AUTONOMOUS_APPLICATION_ID:
    {
        if (vg_CGPS_Naf_Count>0)
        {
            if (vg_CgpsCpOperatorRules[vg_CGPS_CurrentOperator].v_oldSession & CGPS_OLD_SESSION_MASK_AUTONOMOUS_VS_AUTONOMOUS_RESTRICTION)
            {
                /* Autonomous is not allowed, suspend all autonomous session */
                CGPS4_21SetSuspendedState(CGPS_AUTONOMOUS_APPLICATION_ID ,TRUE);
            }
        }
#ifdef AGPS_UP_FTR
        if (vg_CGPS_Supl_Count>0)
        {
            if (vg_CgpsCpOperatorRules[vg_CGPS_CurrentOperator].v_oldSession & CGPS_OLD_SESSION_MASK_AUTONOMOUS_VS_SUPL_RESTRICTION)
            {
                /* SUPL is not allowed, suspend all SUPL session */
                CGPS4_21SetSuspendedState(CGPS_SUPL_APPLICATION_ID ,TRUE);
            }
        }
#endif
#ifdef AGPS_FTR
        if (vg_CGPS_Cp_Mo_Count> 0)
        {
            if (vg_CgpsCpOperatorRules[vg_CGPS_CurrentOperator].v_oldSession & CGPS_OLD_SESSION_MASK_AUTONOMOUS_VS_CPMO_RESTRICTION)
            {
                /* CP MO is not allowed, suspend all CP MO session */
                CGPS4_21SetSuspendedState(CGPS_CP_MO_APPLICATION_ID ,TRUE);
            }
        }
        if (vg_CGPS_Cp_Count>0)
        {
            if (vg_CgpsCpOperatorRules[vg_CGPS_CurrentOperator].v_oldSession & CGPS_OLD_SESSION_MASK_AUTONOMOUS_VS_CPNI_RESTRICTION)
            {
                /* CP NI is not allowed, suspend all CP NI session */
                CGPS4_21SetSuspendedState(CGPS_CP_NI_APPLICATION_ID ,TRUE);
            }
        }
#endif
    }
    break;
#ifdef AGPS_UP_FTR
    case CGPS_SUPL_APPLICATION_ID:
    {
        if (vg_CGPS_Naf_Count>0)
        {
            if (vg_CgpsCpOperatorRules[vg_CGPS_CurrentOperator].v_oldSession & CGPS_OLD_SESSION_MASK_SUPL_VS_AUTONOMOUS_RESTRICTION)
            {
                /* Autonomous is not allowed, suspend all autonomous session */
                CGPS4_21SetSuspendedState(CGPS_AUTONOMOUS_APPLICATION_ID ,TRUE);
            }
        }
        if (vg_CGPS_Supl_Count>0)
        {
            if (vg_CgpsCpOperatorRules[vg_CGPS_CurrentOperator].v_oldSession & CGPS_OLD_SESSION_MASK_SUPL_VS_SUPL_RESTRICTION)
            {
                /* SUPL is not allowed, suspend all SUPL session */
                CGPS4_21SetSuspendedState(CGPS_SUPL_APPLICATION_ID ,TRUE);
            }
        }
#ifdef AGPS_FTR
        if (vg_CGPS_Cp_Mo_Count> 0)
        {
            if (vg_CgpsCpOperatorRules[vg_CGPS_CurrentOperator].v_oldSession & CGPS_OLD_SESSION_MASK_SUPL_VS_CPMO_RESTRICTION)
            {
                /* CP MO is not allowed, suspend all CP MO session */
                CGPS4_21SetSuspendedState(CGPS_CP_MO_APPLICATION_ID ,TRUE);
            }
        }

        if (vg_CGPS_Cp_Count>0)
        {
            if (vg_CgpsCpOperatorRules[vg_CGPS_CurrentOperator].v_oldSession & CGPS_OLD_SESSION_MASK_SUPL_VS_CPNI_RESTRICTION)
            {
                /* CP NI is not allowed, suspend all CP NI session */
                CGPS4_21SetSuspendedState(CGPS_CP_NI_APPLICATION_ID ,TRUE);
            }
        }
#endif
        break;
    }
#endif /* AGPS_UP_FTR */
#ifdef AGPS_FTR
    case CGPS_CP_MO_APPLICATION_ID:
    {
        if (vg_CGPS_Naf_Count>0)
        {
            if (vg_CgpsCpOperatorRules[vg_CGPS_CurrentOperator].v_oldSession & CGPS_OLD_SESSION_MASK_CPMO_VS_AUTONOMOUS_RESTRICTION)
            {
                /* Autonomous is not allowed, suspend all autonomous session */
                CGPS4_21SetSuspendedState(CGPS_AUTONOMOUS_APPLICATION_ID ,TRUE);
            }
        }
#ifdef AGPS_UP_FTR
        if (vg_CGPS_Supl_Count>0)
        {
            if (vg_CgpsCpOperatorRules[vg_CGPS_CurrentOperator].v_oldSession & CGPS_OLD_SESSION_MASK_CPMO_VS_SUPL_RESTRICTION)
            {
                /* SUPL is not allowed, suspend all SUPL session */
                CGPS4_21SetSuspendedState(CGPS_SUPL_APPLICATION_ID ,TRUE);
            }
        }
#endif
        if (vg_CGPS_Cp_Mo_Count> 0)
        {
            if (vg_CgpsCpOperatorRules[vg_CGPS_CurrentOperator].v_oldSession & CGPS_OLD_SESSION_MASK_CPMO_VS_CPMO_RESTRICTION)
            {
                /* CP MO is not allowed, suspend all CP MO session */
                CGPS4_21SetSuspendedState(CGPS_CP_MO_APPLICATION_ID ,TRUE);
            }
        }
        if (vg_CGPS_Cp_Count>0)
        {
            if (vg_CgpsCpOperatorRules[vg_CGPS_CurrentOperator].v_oldSession & CGPS_OLD_SESSION_MASK_CPMO_VS_CPNI_RESTRICTION)
            {
                /* CP NI is not allowed, suspend all CP NI session */
                CGPS4_21SetSuspendedState(CGPS_CP_NI_APPLICATION_ID ,TRUE);
            }
        }
        break;
    }
    case CGPS_CP_NI_APPLICATION_ID:
    {
        if (vg_CGPS_Naf_Count>0)
        {
            if (vg_CgpsCpOperatorRules[vg_CGPS_CurrentOperator].v_oldSession & CGPS_OLD_SESSION_MASK_CPNI_VS_AUTONOMOUS_RESTRICTION)
            {
                /* Autonomous is not allowed, suspend all autonomous session */
                CGPS4_21SetSuspendedState(CGPS_AUTONOMOUS_APPLICATION_ID ,TRUE);
            }
        }
#ifdef AGPS_UP_FTR
        if (vg_CGPS_Supl_Count>0)
        {
            if (vg_CgpsCpOperatorRules[vg_CGPS_CurrentOperator].v_oldSession & CGPS_OLD_SESSION_MASK_CPNI_VS_SUPL_RESTRICTION)
            {
                /* SUPL is not allowed, suspend all SUPL session */
                CGPS4_21SetSuspendedState(CGPS_SUPL_APPLICATION_ID ,TRUE);
            }
        }
#endif
        if (vg_CGPS_Cp_Mo_Count> 0)
        {
            if (vg_CgpsCpOperatorRules[vg_CGPS_CurrentOperator].v_oldSession & CGPS_OLD_SESSION_MASK_CPNI_VS_CPMO_RESTRICTION)
            {
                /* CP MO is not allowed, suspend all CP MO session */
                CGPS4_21SetSuspendedState(CGPS_CP_MO_APPLICATION_ID ,TRUE);
            }
        }
        if (vg_CGPS_Cp_Count>0)
        {
            if (vg_CgpsCpOperatorRules[vg_CGPS_CurrentOperator].v_oldSession & CGPS_OLD_SESSION_MASK_CPNI_VS_CPNI_RESTRICTION)
            {
                /* CP NI is not allowed, suspend all CP NI session */
                CGPS4_21SetSuspendedState(CGPS_CP_NI_APPLICATION_ID ,TRUE);
            }
        }
        break;
    }
#endif /* AGPS_FTR */
    case CGPS_EMERGENCY_APPLICATION_ID:
    {
        /*in this case just  suspend all the other session*/
        CGPS4_21SetSuspendedState(CGPS_AUTONOMOUS_APPLICATION_ID ,TRUE);
        CGPS4_21SetSuspendedState(CGPS_SUPL_APPLICATION_ID ,TRUE);
        CGPS4_21SetSuspendedState(CGPS_CP_NI_APPLICATION_ID ,TRUE);
        CGPS4_21SetSuspendedState(CGPS_CP_MO_APPLICATION_ID ,TRUE);
        break;
    }
    default:
        MC_CGPS_TRACE((" Case CGPS_CP_MAX_VALUE_APPLICATION_ID not handled "));
    break;
    }
}

#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 20
/****************************************************************************************************************/
/* CGPS4_20CloseSessionStateOperatorRules : One session is close check if another session must be activated ( not suspended state ) */
/****************************************************************************************************************/
uint8_t CGPS4_20CloseSessionOperatorRules(e_cgps_cp_application_id vp_closeApplicationId)
{
    uint8_t vl_AutonomousOldSessionAllowed= TRUE;
    uint8_t vl_AutonomousNewSessionAllowed= FALSE;
#ifdef AGPS_UP_FTR
    uint8_t vl_SUPLOldSessionAllowed = TRUE;
    uint8_t vl_SUPLNewSessionAllowed = FALSE;
#endif
#ifdef AGPS_FTR
    uint8_t vl_CPMOOldSessionAllowed = TRUE;
    uint8_t vl_CPNIOldSessionAllowed = TRUE;
    uint8_t vl_CPMONewSessionAllowed = FALSE;
    uint8_t vl_CPNINewSessionAllowed = FALSE;
#endif

    uint8_t vl_return= TRUE;

    MC_CGPS_TRACE(("CGPS4_20CloseSessionOperatorRules"));

    if (vg_CGPS_Naf_Count>0)
    {
        if (vg_CgpsCpOperatorRules[vg_CGPS_CurrentOperator].v_oldSession & CGPS_OLD_SESSION_MASK_AUTONOMOUS_VS_AUTONOMOUS_RESTRICTION)
        {
            /* OLD session are not allowed, only one session must be launch (the new new session :-) ) */
            vl_AutonomousOldSessionAllowed = FALSE;
        }
#ifdef AGPS_UP_FTR
        if (vg_CgpsCpOperatorRules[vg_CGPS_CurrentOperator].v_oldSession & CGPS_OLD_SESSION_MASK_AUTONOMOUS_VS_SUPL_RESTRICTION)
        {
            /* SUPL is not allowed */
            vl_SUPLOldSessionAllowed = FALSE;
        }
#endif
#ifdef AGPS_FTR
        if (vg_CgpsCpOperatorRules[vg_CGPS_CurrentOperator].v_oldSession & CGPS_OLD_SESSION_MASK_AUTONOMOUS_VS_CPMO_RESTRICTION)
        {
            /* CP MO is not allowed*/
            vl_CPMOOldSessionAllowed = FALSE;
        }
        if (vg_CgpsCpOperatorRules[vg_CGPS_CurrentOperator].v_oldSession & CGPS_OLD_SESSION_MASK_AUTONOMOUS_VS_CPNI_RESTRICTION)
        {
            /* CP NI is not allowed*/
            vl_CPNIOldSessionAllowed = FALSE;
        }
#endif
    }
    else
    {
        /* Autonomous session are not allowed --> there are no session */
        vl_AutonomousOldSessionAllowed = FALSE;
    }

#ifdef AGPS_UP_FTR
    if (vg_CGPS_Supl_Count>0)
    {
        /* one request is ongoing */
        if (vg_CgpsCpOperatorRules[vg_CGPS_CurrentOperator].v_oldSession & CGPS_OLD_SESSION_MASK_SUPL_VS_AUTONOMOUS_RESTRICTION)
        {
            /* Autonomous is not allowed*/
            vl_AutonomousOldSessionAllowed = FALSE;
        }
        if (vg_CgpsCpOperatorRules[vg_CGPS_CurrentOperator].v_oldSession & CGPS_OLD_SESSION_MASK_SUPL_VS_SUPL_RESTRICTION)
        {
            /* SUPL is not allowed */
            vl_SUPLOldSessionAllowed = FALSE;
        }
#ifdef AGPS_FTR
        if (vg_CgpsCpOperatorRules[vg_CGPS_CurrentOperator].v_oldSession & CGPS_OLD_SESSION_MASK_SUPL_VS_CPMO_RESTRICTION)
        {
            /* CP MO is not allowed*/
            vl_CPMOOldSessionAllowed = FALSE;
        }
        if (vg_CgpsCpOperatorRules[vg_CGPS_CurrentOperator].v_oldSession & CGPS_OLD_SESSION_MASK_SUPL_VS_CPNI_RESTRICTION)
        {
            /* CP NI is not allowed*/
            vl_CPNIOldSessionAllowed = FALSE;
        }
#endif
    }
    else
    {
        /* SUPL session are not allowed --> there are no session */
        vl_SUPLOldSessionAllowed = FALSE;
    }
#endif

#ifdef AGPS_FTR
    if (vg_CGPS_Cp_Count >0)
    {
        if (vg_CgpsCpOperatorRules[vg_CGPS_CurrentOperator].v_oldSession & CGPS_OLD_SESSION_MASK_CPNI_VS_AUTONOMOUS_RESTRICTION)
        {
            /* Autonomous is not allowed */
            vl_AutonomousOldSessionAllowed = FALSE;
        }
#ifdef AGPS_UP_FTR
        if (vg_CgpsCpOperatorRules[vg_CGPS_CurrentOperator].v_oldSession & CGPS_OLD_SESSION_MASK_CPNI_VS_SUPL_RESTRICTION)
        {
            /* SUPL is not allowed */
            vl_SUPLOldSessionAllowed = FALSE;
        }
#endif
        if (vg_CgpsCpOperatorRules[vg_CGPS_CurrentOperator].v_oldSession & CGPS_OLD_SESSION_MASK_CPNI_VS_CPMO_RESTRICTION)
        {
            /* CP MO is not allowed*/
            vl_CPMOOldSessionAllowed = FALSE;
        }
        if (vg_CgpsCpOperatorRules[vg_CGPS_CurrentOperator].v_oldSession & CGPS_OLD_SESSION_MASK_CPNI_VS_CPNI_RESTRICTION)
        {
            /* CP NI is not allowed*/
            vl_CPNIOldSessionAllowed = FALSE;
        }
    }
    else
    {
        /* CPNI session are not allowed --> there are no session */
        vl_CPNIOldSessionAllowed = FALSE;
    }

    if (vg_CGPS_Cp_Mo_Count> 0)
    {
        /* one request is ongoing */
        if (vg_CgpsCpOperatorRules[vg_CGPS_CurrentOperator].v_oldSession & CGPS_OLD_SESSION_MASK_CPMO_VS_AUTONOMOUS_RESTRICTION)
        {
            /* Autonomous is not allowed*/
            vl_AutonomousOldSessionAllowed = FALSE;
        }
#ifdef AGPS_UP_FTR
        if (vg_CgpsCpOperatorRules[vg_CGPS_CurrentOperator].v_oldSession & CGPS_OLD_SESSION_MASK_CPMO_VS_SUPL_RESTRICTION)
        {
            /* SUPL is not allowed */
            vl_SUPLOldSessionAllowed = FALSE;
        }
#endif
        if (vg_CgpsCpOperatorRules[vg_CGPS_CurrentOperator].v_oldSession & CGPS_OLD_SESSION_MASK_CPMO_VS_CPMO_RESTRICTION)
        {
            /* CP MO is not allowed*/
            vl_CPMOOldSessionAllowed = FALSE;
        }
        if (vg_CgpsCpOperatorRules[vg_CGPS_CurrentOperator].v_oldSession & CGPS_OLD_SESSION_MASK_CPMO_VS_CPNI_RESTRICTION)
        {
            /* CP NI is not allowed*/
            vl_CPNIOldSessionAllowed = FALSE;
        }
    }
    else
    {
        /* CPMO session are not allowed --> there are no session */
        vl_CPMOOldSessionAllowed = FALSE;
    }
#endif /* AGPS_FTR */

    /*All sesion of autonmous are allowed */
    if (vl_AutonomousOldSessionAllowed)
    {
        CGPS4_21SetSuspendedState(CGPS_AUTONOMOUS_APPLICATION_ID,FALSE);
    }
    else
    {
        if ((vp_closeApplicationId==CGPS_AUTONOMOUS_APPLICATION_ID)&&(vg_CGPS_Naf_Count>0))
        {
            /* test if a new autonomous session can be launch */
            vl_AutonomousNewSessionAllowed= CGPS4_18CheckNewSessionRight(vp_closeApplicationId);
            if (vl_AutonomousNewSessionAllowed)
            {
                /* launch only one autonomous application the new new session */
                CGPS4_22ActivateOneSession(CGPS_AUTONOMOUS_APPLICATION_ID);

            }

        }
    }

#ifdef AGPS_UP_FTR
    if (vl_SUPLOldSessionAllowed)
    {
        CGPS4_21SetSuspendedState(CGPS_SUPL_APPLICATION_ID,FALSE);
    }
    else
    {
        if ((vp_closeApplicationId==CGPS_SUPL_APPLICATION_ID)&&(vg_CGPS_Supl_Count>0))
        {
            /* test if a new SUPL session can be launch */
            vl_SUPLNewSessionAllowed = CGPS4_18CheckNewSessionRight(vp_closeApplicationId);
            if (vl_SUPLNewSessionAllowed)
            {
                /* launch only one SUPL application the new new session */
                CGPS4_22ActivateOneSession(CGPS_SUPL_APPLICATION_ID);
            }

        }
    }
#endif

#ifdef AGPS_FTR
    if (vl_CPMOOldSessionAllowed)
    {
        CGPS4_21SetSuspendedState(CGPS_CP_MO_APPLICATION_ID,FALSE);
    }
    else
    {
        if ((vp_closeApplicationId==CGPS_CP_MO_APPLICATION_ID)&&(vg_CGPS_Cp_Mo_Count> 0))
        {
            /* test if a new CP MO session can be launch */
            vl_CPMONewSessionAllowed = CGPS4_18CheckNewSessionRight(vp_closeApplicationId);
            if (vl_CPMONewSessionAllowed)
            {
                /* launch only one CP MO application the new new session */
                CGPS4_22ActivateOneSession(CGPS_CP_MO_APPLICATION_ID);
            }

        }
    }

    if (vl_CPNIOldSessionAllowed)
    {
        CGPS4_21SetSuspendedState(CGPS_CP_NI_APPLICATION_ID,FALSE);
    }
    else
    {
        if ((vp_closeApplicationId==CGPS_CP_NI_APPLICATION_ID)&&(vg_CGPS_Cp_Count>0))
        {
            /* test if a new CP NI session can be launch */
            vl_CPNINewSessionAllowed = CGPS4_18CheckNewSessionRight(vp_closeApplicationId);
            if (vl_CPNINewSessionAllowed)
            {
                /* launch only one CP NI application the new new session */
                CGPS4_22ActivateOneSession(CGPS_CP_NI_APPLICATION_ID);
            }

        }
    }
#endif

    if ( ( /*there are one or more session*/
                (vg_CGPS_Naf_Count>0)
#ifdef AGPS_UP_FTR
                || (vg_CGPS_Supl_Count>0)
#endif
#ifdef AGPS_FTR
                || (vg_CGPS_Cp_Count >0)
                ||  (vg_CGPS_Cp_Mo_Count>0)
#endif
            ) &&
            ( /*No old session has been activated */
                (!vl_AutonomousOldSessionAllowed)
#ifdef AGPS_UP_FTR
                && (!vl_SUPLOldSessionAllowed)
#endif
#ifdef AGPS_FTR
                && (!vl_CPMOOldSessionAllowed)
                && (!vl_CPNIOldSessionAllowed)
#endif
            )  &&
            ( /*No new session has been activated*/
                (!vl_AutonomousNewSessionAllowed)
#ifdef AGPS_UP_FTR
                && (!vl_SUPLNewSessionAllowed)
#endif
#ifdef AGPS_FTR
                && (!vl_CPMONewSessionAllowed)
                && (!vl_CPNINewSessionAllowed)
#endif
            )
#ifdef AGPS_FTR
            && (!vg_IsEmergencyCall) /*Emergency call isn't running*/
#endif
       )
    {
        /* It's a problem all session are suspended no session RUN*/
        /* test if we are using default operator rules */
        if (vg_CGPS_CurrentOperator != CGPS_DEFAULT_OPERATOR_INDEX)
        {
            /* change the operator ID to default and retry */
            /*vg_CGPS_CurrentOperator = CGPS_DEFAULT_OPERATOR_INDEX;*/
            if (!CGPS4_20CloseSessionOperatorRules(vp_closeApplicationId))
            {
                /*One session or more are blocked due to operator rules */
                /* FIXME perhaps it's appropriate to generate BD.  Must check with Thomas and Philippe (for me we must generate BD because default rule must be perfect ;-) )*/
                MC_CGPS_TRACE(("CGPS4_20CloseSessionOperatorRules : Problem in the operator rules"));
                vl_return = FALSE;
            }
        }
        else
        {
            MC_CGPS_TRACE(("CGPS4_20CloseSessionOperatorRules : Problem in the operator rules"));
            vl_return = FALSE;
        }
    }

    return vl_return;
}


#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 21
/****************************************************************************************************************/
/* CGPS4_21SetSuspendedState : set all session suspended state */
/****************************************************************************************************************/
void CGPS4_21SetSuspendedState(e_cgps_cp_application_id vp_applicationId,uint8_t vp_state)
{
    uint32_t vl_index=0;

    MC_CGPS_TRACE(("CGPS4_21SetSuspendedState"));

    switch (vp_applicationId)
    {
    case CGPS_AUTONOMOUS_APPLICATION_ID:
    {
        /* if it isn't the last session the rules must not be modified*/
        for (vl_index =0;vl_index < K_CGPS_MAX_NUMBER_OF_NAF;vl_index++)
        {
            if (vp_state)
            {
                s_CgpsNaf[vl_index].v_Suspended++;
            }
            else
            {
                s_CgpsNaf[vl_index].v_Suspended=CGPS_SESSION_NOT_SUSPENDED;
            }
        }
        break;
    }
#ifdef AGPS_UP_FTR
    case CGPS_SUPL_APPLICATION_ID:
    {
        for (vl_index=0;vl_index < K_CGPS_MAX_NUMBER_OF_SUPL;vl_index ++)
        {
            /* Only for registered applications , change state to suspended */
            if ( s_CgpsSupl[vl_index].v_IsRegistered )
            {
                if (vp_state)
                {
                    s_CgpsSupl[vl_index].v_Suspended++;
                }
                else
                {
                    if (s_CgpsSupl[vl_index].v_Suspended != CGPS_SESSION_NOT_SUSPENDED)
                    {
                        s_CgpsSupl[vl_index].v_Suspended = CGPS_SESSION_NOT_SUSPENDED;
                        /* activate an SUPL connection*/
                        if (-1 == vg_CGPS_Supl_Pending_Connection)
                        {
                            CGPS5_18ConnectSUPLSession(vl_index);
                        }
                    }
                }
            }
        }

        break;
    }
#endif
#ifdef AGPS_FTR
    case CGPS_CP_MO_APPLICATION_ID:
    {
        /*MO CP not yet implemented*/
        break;
    }
    case CGPS_CP_NI_APPLICATION_ID:
    {
        for (vl_index=0;vl_index < K_CGPS_MAX_NUMBER_OF_CP;vl_index ++)
        {
            if (vp_state)
            {
                s_CgpsCp[vl_index].v_Suspended++;
            }
            else
            {
                s_CgpsCp[vl_index].v_Suspended=CGPS_SESSION_NOT_SUSPENDED;
            }
        }
        break;
    }
#endif
    default:
        MC_CGPS_TRACE((" Case CGPS_EMERGENCY_APPLICATION_ID & CGPS_CP_MAX_VALUE_APPLICATION_ID not handled "));
    break;
    }
}


#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 22
/****************************************************************************************************************/
/* CGPS4_22ActivateOneSession : sActivate one session using the priority                                                       */
/****************************************************************************************************************/
void CGPS4_22ActivateOneSession(e_cgps_cp_application_id vp_applicationId)
{
    uint32_t vl_index=0;
    uint32_t vl_foundIndex;
    uint8_t vl_priority=CGPS_MAX_PRIORITY_VALUE;

    MC_CGPS_TRACE(("CGPS4_22ActivateOneSession"));

    switch (vp_applicationId)
    {
    case CGPS_AUTONOMOUS_APPLICATION_ID:
    {
        vl_foundIndex=K_CGPS_MAX_NUMBER_OF_NAF;
        /* Search the session wich the biggest priority*/
        for (vl_index =0;vl_index < K_CGPS_MAX_NUMBER_OF_NAF;vl_index++)
        {
            if (s_CgpsNaf[vl_index].v_Suspended<vl_priority)
            {
                vl_foundIndex = vl_index;
                vl_priority = s_CgpsNaf[vl_index].v_Suspended;
            }
        }
        if (K_CGPS_MAX_NUMBER_OF_NAF!=vl_foundIndex)
        {
            s_CgpsNaf[vl_foundIndex].v_Suspended=CGPS_SESSION_NOT_SUSPENDED;
        }
        break;
    }
#ifdef AGPS_UP_FTR
    case CGPS_SUPL_APPLICATION_ID:
    {
        vl_foundIndex=K_CGPS_MAX_NUMBER_OF_SUPL;
        /* Search the session wich the biggest priority , which is registered and which is not suspended */
        for (vl_index =0;vl_index < K_CGPS_MAX_NUMBER_OF_SUPL;vl_index++)
        {
            if ( ( s_CgpsSupl[vl_index].v_IsRegistered ) && ( s_CgpsSupl[vl_index].v_Suspended != CGPS_SESSION_NOT_SUSPENDED ) )
            {
                if (s_CgpsSupl[vl_index].v_Suspended<vl_priority)
                {
                    vl_foundIndex = vl_index;
                    vl_priority = s_CgpsSupl[vl_index].v_Suspended;
                }
            }
        }
        if (K_CGPS_MAX_NUMBER_OF_SUPL!=vl_foundIndex)
        {
            s_CgpsSupl[vl_foundIndex].v_Suspended=CGPS_SESSION_NOT_SUSPENDED;
            CGPS5_18ConnectSUPLSession(vl_foundIndex);
        }
        break;
    }
#endif
#ifdef AGPS_FTR
    case CGPS_CP_MO_APPLICATION_ID:
    {
        /*MO CP not yet implemented*/
        break;
    }
    case CGPS_CP_NI_APPLICATION_ID:
    {
        vl_foundIndex=K_CGPS_MAX_NUMBER_OF_CP;
        /* Search the session wich the biggest priority*/
        for (vl_index =0;vl_index < K_CGPS_MAX_NUMBER_OF_CP;vl_index++)
        {
            if (s_CgpsCp[vl_index].v_Suspended<vl_priority)
            {
                vl_foundIndex = vl_index;
                vl_priority = s_CgpsCp[vl_index].v_Suspended;
            }
        }
        if (K_CGPS_MAX_NUMBER_OF_CP!=vl_foundIndex)
        {
            s_CgpsCp[vl_foundIndex].v_Suspended=CGPS_SESSION_NOT_SUSPENDED;
        }
        break;
    }
#endif
    default:
        MC_CGPS_TRACE((" Case CGPS_EMERGENCY_APPLICATION_ID & CGPS_CP_MAX_VALUE_APPLICATION_ID not handled "));
    break;
    }
}


#ifdef AGPS_FTR

#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 23
/****************************************************************************************************************/
/* CGPS4_23CPNotificationEncodingType :                                                    */
/****************************************************************************************************************/
e_cgps_EncodingType CGPS4_23CPNotificationEncodingType(uint8_t vp_CPAlphabet)
{
    e_cgps_EncodingType vl_result= K_CGPS_NONE;

    switch (vp_CPAlphabet)
    {
    case LSIMCP_DCS_7_BIT_DATA_CODED:
    {
        vl_result = K_CGPS_DCS_8_BIT_TEXT;
        break;
    }
    case LSIMCP_DCS_8_BIT_DATA:
    {
        vl_result = K_CGPS_DCS_8_BIT_DATA;
        break;
    }
    case LSIMCP_DCS_UCS2:
    {
        vl_result = K_CGPS_UCS2;
        break;
    }
    case LSIMCP_UCS2_COMPRESSED:
    {
        vl_result = K_CGPS_UCS2_COMPRESSED;
        break;
    }
    case LSIMCP_DCS_8_BIT_TEXT:
    {
        vl_result = K_CGPS_DCS_8_BIT_TEXT;
        break;
    }
/*
Sysol UPV module defines fiollowing encoding types which have been mapped to LSIMCP constants as
UPV_DCS_7_BIT_DATA_CODED   LSIMCP_DCS_7_BIT_DATA_CODED
UPV_DCS_8_BIT_DATA         LSIMCP_DCS_8_BIT_DATA
UPV_DCS_UCS2               LSIMCP_DCS_UCS2
UPV_UCS2_COMPRESSED        LSIMCP_UCS2_COMPRESSED
UPV_DCS_8_BIT_TEXT         LSIMCP_DCS_8_BIT_TEXT
UPV_DCS_UNDEFINED          LSIMCP_DCS_UNDEFINED
*/
#ifndef __RTK_E__
/* + Support for new DCS type from LSIMCP */
    case LSIMCP_DCS_GSM_DEFAULT:
    {
        vl_result = K_CGPS_GSM_DEFAULT;
        break;
    }
/* - Support for new DCS type from LSIMCP */
#endif /* __RTK_E__*/

    case LSIMCP_DCS_UNDEFINED:
    {
        vl_result = K_CGPS_NONE;
        break;
    }
    default:
    {
        vl_result = K_CGPS_NONE;
        break;
    }
    }
    return vl_result;
}

#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 24
/****************************************************************************************************************/
/* CGPS4_24CPNotificationServiceType :                                                 */
/****************************************************************************************************************/
e_cgps_LcsServiceType CGPS4_24CPNotificationServiceType(uint8_t vp_CPService)
{
    e_cgps_LcsServiceType vl_result= K_CGPS_MAX_LCSSERVICETYPE;

    switch (vp_CPService)
    {
    case LSIMCP_EMERGENCY_SERVICES:
    {
        vl_result = K_CGPS_EMERGENCY_SERVICES;
        break;
    }
    case LSIMCP_EMERGENCY_ALERT_SERVICES:
    {
        vl_result = K_CGPS_EMERGENCY_ALERT_SERVICES;
        break;
    }
    case LSIMCP_PERSON_TRACKING:
    {
        vl_result = K_CGPS_PERSON_TRACKING;
        break;
    }
    case LSIMCP_FLEET_MANAGEMENT:
    {
        vl_result = K_CGPS_FLEET_MANAGEMENT;
        break;
    }
    case LSIMCP_ASSET_MANAGEMENT:
    {
        vl_result = K_CGPS_ASSET_MANAGEMENT;
        break;
    }
    case LSIMCP_TRAFFIC_CONGESTION_REPORTING:
    {
        vl_result = K_CGPS_TRAFFIC_CONGESTION_REPORTING;
        break;
    }
    case LSIMCP_ROAD_SIDE_ASSISTANCE:
    {
        vl_result = K_CGPS_ROAD_SIDE_ASSISTANCE;
        break;
    }
    case LSIMCP_ROUTING_TO_NEAREST_COMMERCIAL_ENTERPRISE:
    {
        vl_result = K_CGPS_ROUTING_TO_NEAREST_COMMERCIAL_ENTERPRISE;
        break;
    }
    case LSIMCP_NAVIGATION:
    {
        vl_result = K_CGPS_NAVIGATION;
        break;
    }
    case LSIMCP_CITY_SIGHTSEEING:
    {
        vl_result = K_CGPS_CITY_SIGHTSEEING;
        break;
    }
    case LSIMCP_LOCALIZED_ADVERTISING:
    {
        vl_result = K_CGPS_LOCALIZED_ADVERTISING;
        break;
    }
    case LSIMCP_MOBILE_YELLOW_PAGES:
    {
        vl_result = K_CGPS_MOBILE_YELLOW_PAGES;
        break;
    }
    default:
    {
        vl_result = K_CGPS_MAX_LCSSERVICETYPE;
        break;
    }

    }
    return vl_result;
}
#endif /* AGPS_FTR */

t_CGPSMsgQueue *vg_QueueFirst = NULL;

#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 25
/****************************************************************************************************************/
/* CGPS4_25EnQueueMessage :                                                    */
/****************************************************************************************************************/

void CGPS4_25EnQueueMessage(t_ProcessIdentity v_SrcProcess, t_ProcessIdentity v_DstProcess,t_OperationType v_Event, uint8_t* p_Data)
{
   t_CGPSMsgQueue *vl_Queue = vg_QueueFirst;
   t_CGPSMsgQueue *vl_Temp= NULL;

   MC_CGPS_TRACE(("+CGPS4_25EnQueueMessage"));
   while (vl_Queue != NULL)
   {
     if(vl_Queue->v_QNext == NULL) break;
     vl_Queue = (t_CGPSMsgQueue *)vl_Queue->v_QNext;
   }
   vl_Temp= MC_RTK_GET_MEMORY(sizeof(*vl_Temp));
   if(vl_Temp == NULL)
   {
      MC_CGPS_TRACE(("Error Error- Could not allocate memory"));
      return;
   }
   vl_Temp->v_SrcProcess = v_SrcProcess;
   vl_Temp->v_DstProcess = v_DstProcess;
   vl_Temp->v_Event = v_Event;
   vl_Temp->p_Data = p_Data;
   vl_Temp->v_QNext = NULL;
   MC_CGPS_TRACE(("Adding Dst=%x, Event=%x",v_DstProcess,v_Event));


   if(vl_Queue == NULL)
     vg_QueueFirst = vl_Temp;
   else
     vl_Queue->v_QNext = (t_CGPSMsgQueuePtr)vl_Temp;
   MC_CGPS_TRACE(("vg_QueueFirst %x vl_Queue %x",vg_QueueFirst,vl_Queue));

   MC_CGPS_TRACE(("-CGPS4_25EnQueueMessage"));
   return;
}

#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 26
/****************************************************************************************************************/
/* CGPS4_26DeQueueMessage :                                                    */
/****************************************************************************************************************/

void CGPS4_26DeQueueMessage()
{
   t_CGPSMsgQueue *vl_Queue = vg_QueueFirst;
   t_CGPSMsgQueue *vl_Temp= NULL;

   MC_CGPS_TRACE(("+CGPS4_26DeQueueMessage"));

   MC_CGPS_TRACE(("vg_QueueFirst %x vl_Queue %x",vg_QueueFirst,vl_Queue));
   while (vl_Queue != NULL)
   {

     MC_CGPS_TRACE(("+Sending Dst=%x, Event=%x",vl_Queue->v_DstProcess,vl_Queue->v_Event));

     MC_RTK_SEND_MSG_TO_PROCESS(vl_Queue->v_SrcProcess, 0,
                    vl_Queue->v_DstProcess, 0,
                    vl_Queue->v_Event,
                    (t_MsgHeader *)vl_Queue->p_Data);
     MC_CGPS_TRACE(("-Sending Dst=%x, Event=%x",vl_Queue->v_DstProcess,vl_Queue->v_Event));

     vl_Temp = (t_CGPSMsgQueue *)vl_Queue->v_QNext;
     MC_RTK_FREE_MEMORY(vl_Queue);
     vl_Queue = vl_Temp;
   }

   vg_QueueFirst = NULL;
   MC_CGPS_TRACE(("-CGPS4_26DeQueueMessage"));
   return;
}
#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 27
/****************************************************************************************************************/
/* CGPS4_27EmptyQueueMessage :                                                 */
/****************************************************************************************************************/

void CGPS4_27EmptyQueueMessage()
{
   t_CGPSMsgQueue *vl_Queue = vg_QueueFirst;
   t_CGPSMsgQueue *vl_Temp= NULL;

   while (vl_Queue != NULL)
   {
     MC_CGPS_TRACE(("Freeing Dst=%x, Event=%x",vl_Queue->v_DstProcess,vl_Queue->v_Event));

     vl_Temp = (t_CGPSMsgQueue *)vl_Queue->v_QNext;
     MC_RTK_FREE_MEMORY(vl_Queue);
     vl_Queue = vl_Temp;
   }

   vg_QueueFirst = NULL;

   return;
}

#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 28
/****************************************************************************************************************/
/* CGPS4_28SendMsgToProcess :                                                  */
/****************************************************************************************************************/
void CGPS4_28SendMsgToProcess(t_ProcessIdentity v_SrcProcess, t_ProcessIdentity v_DstProcess,t_OperationType v_Event, t_MsgHeader * p_Data)
{
/*+LMSqb89879 AFE*/
#ifdef AGPS_FTR
/*-LMSqb89879 AFE*/
   if(vg_IsEmergencyCall)
   {
     CGPS4_25EnQueueMessage(v_SrcProcess,
                    v_DstProcess,
                    v_Event,
                    (uint8_t *)p_Data);

   }
   else
/*+LMSqb89879 AFE*/
 #endif /* AGPS_FTR */
/*-LMSqb89879 AFE*/
   {

     MC_RTK_SEND_MSG_TO_PROCESS(v_SrcProcess, 0,
                    v_DstProcess, 0,
                    v_Event,
                    (t_MsgHeader *)p_Data);
   }

  return;

}


/* +LMSqb93714 */
#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 29
/****************************************************************************************************************/
/* CGPS4_29SaveTCXOCalibration :                                                  */
/****************************************************************************************************************/
void CGPS4_29SaveTCXOCalibration()
{
    /* Remember the last TCXO Calibration */
    s_GN_GPS_Time_Data  vl_TimeData;            /* Time Data */
    int32_t                 vl_TCXOCalibppb = 0;    /* TCXO Offset [ppb] */
    int32_t                 vl_TCXOCalibrms = 0;    /* RMS of the TCXO Offset [ppb]*/

    if ( GN_GPS_Get_Time_Data ( &vl_TimeData ) )
    {

        vl_TCXOCalibppb = (int32_t)(vl_TimeData.Clock_Drift * 1.0e9 );  /* [s/s -> ppb] */
        vl_TCXOCalibrms = (int32_t)(vl_TimeData.F_AccEst    * 1.0e9f);  /* [s/s -> ppb] */

        MC_CGPS_TRACE(("CGPS4_29SaveTCXOCalibration : TCXOCalibppb : %i, vl_TCXOCalibrms : %i", vl_TCXOCalibppb, vl_TCXOCalibrms ));

        /* If the recovered TCXO Calibration is non-zero and only has an RMS of either :
               a) Just over 100 ppb (ie 0.5 ppm Max) then it is probably based on a previous
               GN_AGPS_Set_ClkFreq() input that had no opportunity for further improvement
               (eg in MS-Assisted mode) and has degraded slightly.
               a) About 333 ppb (ie 1ppm Max) then it is not based on a recent GPS fix
               (eg in MS-Assisted mode testing) and/or the time has been adjusted
               forwards by more than 52 weeks, or backwards more than 10 minutes.
               If so, then its probably OK
               so set the RMS to 170 ppb, so that its used without adding any
               further random error. */
        if ( vl_TCXOCalibppb != 0 )
        {
/* ++ LMSQC01305 ANP 16/07/2009  */
            if ( ( vl_TCXOCalibrms > 100  &&  vl_TCXOCalibrms < 200 )  ||
             ( vl_TCXOCalibrms > 310  &&  vl_TCXOCalibrms < 350 )   )
            {
                    vl_TCXOCalibrms = 100;                             /* [ppb] */
            }
        }


        /* In order to prevent feeding in the exact correct TCXO Calibration,
                  add some noise but keeping it within +/- 200 ppb. */
        if ( vl_TCXOCalibrms <= 100 )     /* ie within 100 ppb RMS, ie < 500 ppb Max. */
        {
            int32_t vl_TempMargin;

            /* Compute the margin between 3 x RMS and 200 ppb. */
            vl_TempMargin = 200 - 3 *  vl_TCXOCalibrms;

            if ( vl_TempMargin > 0 )      /* There is margin to add some noise */
            {
                /* Randomly scale the margin by -1.0 to + 1.0 and add as noise. */
                vl_TempMargin = (int32_t)( vl_TempMargin * 2.0 * ( ( rand() * 1.0 / RAND_MAX ) - 0.5 ) );    /* range +/- 200   */
                vl_TCXOCalibppb = vl_TCXOCalibppb + vl_TempMargin;
            }
            vl_TCXOCalibrms = 100;  /* Increase to +/-100 ppb RMS, +/-200 ppb Max. */
/* -- LMSQC01305 ANP 16/07/2009  */
        }

        vg_ClockFreqData.Calibration  = (R4)vl_TCXOCalibppb;
        vg_ClockFreqData.RMS_ppb     =  (U4)vl_TCXOCalibrms;

        vg_ClockFreqRestorePending       =  TRUE;       /* Mask to check if Clock frequency data has to be applied on wake-up. */
    }
}


#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 30
/*****************************************************************************************************/
/* CGPS4_30RestoreTCXOCalibration : Apply the clock frequency                */
/*****************************************************************************************************/
void CGPS4_30RestoreTCXOCalibration(void)
{
    MC_CGPS_TRACE(("CGPS4_30RestoreTCXOCalibration : Clock freq set pending : %i, TCXOCalibppb : %f, vl_TCXOCalibrms : %d",
                   vg_ClockFreqRestorePending, vg_ClockFreqData.Calibration, vg_ClockFreqData.RMS_ppb ));

    if ( vg_ClockFreqRestorePending )
    {

        if ( vg_ClockFreqData.Calibration != 0 &&  vg_ClockFreqData.RMS_ppb <= 200 )
        {
            vg_ClockFreqData.OS_Time_ms   =  (uint32_t)GN_GPS_Get_OS_Time_ms();
            if ( FALSE == GN_AGPS_Set_ClkFreq( &vg_ClockFreqData ) )
            {
                MC_CGPS_TRACE(( "CGPS4_30RestoreTCXOCalibration : Clock frequency set failed"));
            }
        }
        vg_ClockFreqRestorePending = FALSE;
    }
}
/* -LMSqb93714 */


#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 31
/*****************************************************************************************************/
/* CGPS4_31ClearNVStore : register or update a control plane application in MS based mode            */
/*****************************************************************************************************/
void CGPS4_31ClearNVStore( int8_t * Items)   /* i - String describing the Data items*/
/*        to clear */
{
    uint16_t NV_Size     = 0;              /* Size of the GN_GPS_Lib NV Store*/
    uint8_t *p_NV_Store = NULL;           /* Pointer to the internal GN_GPS_Lib NV Store*/
    /* Get a pointer to and the size of the libraries internal NV Store RAM copy.*/
    NV_Size = GN_GPS_Get_NV_Store( &p_NV_Store );

    if ( p_NV_Store != NULL   &&  NV_Size > 0 )
    {
        /* Clear the required items in the NV Store.*/
        if ( GN_GPS_Clear_NV_Data( p_NV_Store, (char*)Items ) )
        {
            /* Save the cleared NV store back to the host file system.*/
            GN_GPS_Write_NV_Store( NV_Size, p_NV_Store );
        }
    }
}

/*+SYScs46505*/
#undef  PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 32
/**************************************************************************************************/
/* CGPS4_32CheckAssistanceNeedInTracking :                         */
/**************************************************************************************************/
void CGPS4_32CheckAssistanceNeedInTracking()
{
    uint16_t vl_Mask = 0;
#if defined (AGPS_FTR) || defined (AGPS_UP_FTR) || defined (AGPS_HISTORICAL_DATA_FTR)
    s_GN_AGPS_Assist_Req p_AR;
#endif
    uint32_t vl_CurrentTime = GN_GPS_Get_OS_Time_ms();

    if(vg_CGPS_Naf_Count == 0)
    {
        return;
    }

    /*Track assistance only after certain Period "P" has expired*/
    /*If the flag is not set then only check for current time*/
    if(    (vg_AssistTrack.vl_lastAssistTrackTimestamp != K_CGPS_INVALID_TIMESTAMP) 
        && (vl_CurrentTime < (vg_AssistTrack.vl_lastAssistTrackTimestamp+K_CGPS_ASSIST_TRACK_PERIOD))) 
    {
        return;
    }

    vg_AssistTrack.vl_lastAssistTrackTimestamp = vl_CurrentTime;

#if defined (AGPS_FTR) || defined (AGPS_UP_FTR) || defined (AGPS_HISTORICAL_DATA_FTR)
    if(!CGPS4_61CheckIfAssistanceRequired(&vl_Mask, &p_AR))
    {
        return;
    }
#endif

    /*Assistance Data is required by Library*/
    if(     (vg_AssistTrack.vl_lastSUPLSessionTimestamp != K_CGPS_INVALID_TIMESTAMP) 
        &&  (vl_CurrentTime < (vg_AssistTrack.vl_lastSUPLSessionTimestamp + K_CGPS_MIN_TIME_BETWEEN_SUPL))) 
    {
        return;
    }

#ifdef AGPS_UP_FTR
    /*Check if SUPL/CP session is ongoing, don't initiate one more SUPL session*/
    /* +LMSqc51839 : 24-08-2010 */
    /* Do not trigger a SUPL sessions if (1) SUPL session in progress (2) CP session in progress or (3) no NAF session present.ex during patch upload at init */
    if(  (vg_CGPS_Supl_Count > 0) ||
#ifdef AGPS_FTR
         (vg_CGPS_Cp_Count > 0) || (vg_CGPS_Cp_Assitance_No_Request > 0) || (vg_CGPS_Cp_Mo_Count > 0)||
#endif
          0 ) 
    {
        return;
    }
    /* -LMSqc51839 : 24-08-2010 */
        if(vg_AssistTrack.vl_NumSuplSessionAttemptInCell > K_CGPS_ASSIST_TRACK_MAX_NUM_FAIL_ATTEMPT) return;

        /*We did SUPL Session in last X Min, don't do another SUPL Session. Network will not be happy if it receive too frequent Assistance Data Request*/
    vg_AssistTrack.vl_NumSuplSessionAttemptInCell++;

        /*As we prefer SUPL session over MOLR, we will start SUPL exchange with network*/
        /*This SUPL session must not be linked to Applications*/

    MC_CGPS_TRACE(("CGPS4_32CheckAssistanceNeedInTracking : Triggering a SUPL session" ));

        CGPS5_13StartSUPL(NULL,NULL);

        CGPS4_34UpdateLastSUPLSessionTime(&vg_AssistTrack); /*It is being  done here to avoid case when SUPL session has failed whihc has results in we requesting SUPL very frequently*/
#endif
}


#undef  PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 33
/**************************************************************************************************/
/* CGPS4_33InitializeAssistTracking :                         */
/**************************************************************************************************/

void CGPS4_33InitializeAssistTracking(t_cgps_AssistTracking* vl_Assist)
{
   /*This fields must be initialized when GPS has started OR when Cell has changed.*/
   vl_Assist->vl_lastAssistTrackTimestamp =K_CGPS_INVALID_TIMESTAMP;
   vl_Assist->vl_NumSuplSessionAttemptInCell=0;
   vl_Assist->vl_NumSuplSessionFailureInCell=0;
   vl_Assist->vl_lastSUPLSessionTimestamp=K_CGPS_INVALID_TIMESTAMP;
   MC_CGPS_TRACE(( "CGPS4_33: AT=%ld,ST=%ld,SA=%ld,SF=%ld",
                            vl_Assist->vl_lastAssistTrackTimestamp,
                            vl_Assist->vl_lastSUPLSessionTimestamp,
                            vl_Assist->vl_NumSuplSessionAttemptInCell,
                            vl_Assist->vl_NumSuplSessionFailureInCell));

}


#undef  PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 34
/**************************************************************************************************/
/* CGPS4_34UpdateLastSUPLSessionTime :                         */
/**************************************************************************************************/

void CGPS4_34UpdateLastSUPLSessionTime(t_cgps_AssistTracking* vl_Assist)
{
/*This must be done when connection is successful for SUPL session
Other than that there is no other way to know when SUPL session will be successful*/
    uint32_t vl_CurrentTime = GN_GPS_Get_OS_Time_ms();
    vl_Assist->vl_lastSUPLSessionTimestamp=vl_CurrentTime;
    MC_CGPS_TRACE(( "CGPS4_34: AT=%ld,ST=%ld,SA=%ld,SF=%ld",
                            vl_Assist->vl_lastAssistTrackTimestamp,
                            vl_Assist->vl_lastSUPLSessionTimestamp,
                            vl_Assist->vl_NumSuplSessionAttemptInCell,
                            vl_Assist->vl_NumSuplSessionFailureInCell));
}

#undef  PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 35
/**************************************************************************************************/
/* CGPS4_35UpdateNumSuplFailure :                         */
/**************************************************************************************************/

void CGPS4_35UpdateNumSuplFailure(t_cgps_AssistTracking* vl_Assist)
{
    /*This must be doen when connectino has failed or PDP has failed etc*/
    vl_Assist->vl_NumSuplSessionFailureInCell++;

    MC_CGPS_TRACE(( "CGPS4_35: AT=%ld,ST=%ld,SA=%ld,SF=%ld",
                            vl_Assist->vl_lastAssistTrackTimestamp,
                            vl_Assist->vl_lastSUPLSessionTimestamp,
                            vl_Assist->vl_NumSuplSessionAttemptInCell,
                            vl_Assist->vl_NumSuplSessionFailureInCell));
}
/*-SYScs46505*/
#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 36

/*****************************************************************************************************/
/* CGPS4_36ConvertQoP :  */
/*****************************************************************************************************/

void CGPS4_36ConvertQoP( t_CgpsNaf* pp_Handle , t_cgps_QopIndexFormat * pp_QoP, uint8_t vl_Latency )
{
    double  vl_HorizAccuracy , vl_VertAccuracy;

    uint8_t v_Latency =  vl_Latency ;

    /*For MOLR requests where expectation is to achieve QOP, CGPS sets delay option as Delay Tolerant .*/

    pp_QoP->v_Delay = K_CGPS_DELAY_TOLERANT ;/*It implies that fulfillment of the accuracy requirement
                                                                                  takes precedence over fulfillment of the response time requirement. */

    /* convert horizontal accuracy from m to 3GPP spec. hor_acc_in_m = 10*( 1.1^k -1 )
                therefore k = log( (hor_acc_in_m/10) + 1 ) / log ( 1.1 )
     */
    vl_HorizAccuracy = log( (pp_Handle->v_HorizAccuracy/10.0) + 1.0 ) / log ( 1.1 ) ;

    if ( vl_HorizAccuracy > 127.0 )
    {
        pp_QoP->v_Horacc = 127;
    }
    else
    {
        pp_QoP->v_Horacc = (uint8_t)floor(vl_HorizAccuracy);
    }

    /* vertical accuracy according to 3GPP spec is  ver_acc_in_m = 45*( 1.025^k - 1 )
              therefore k = log( (ver_acc_in_m/45) + 1 ) / log ( 1.025 )
          */
    vl_VertAccuracy = log( (pp_Handle->v_VertAccuracy /45.0) + 1.0 ) / log ( 1.025 ) ;
    if ( vl_VertAccuracy > 127.0 )
    {
        pp_QoP->v_Veracc = 127;
    }
    else
    {
        pp_QoP->v_Veracc = (uint8_t)floor(vl_VertAccuracy);
    }

    /* AgeLimit in NAF is in milliseconds */
    pp_QoP->v_MaxLocAge = pp_Handle->v_AgeLimit / 1000.0 ;

    MC_CGPS_TRACE(("CGPS7_38ConvertQoP: calculated value  Horizontal accuracy : %i vertical accuracy:%i Age limit: %i Delay: %i",pp_QoP->v_Horacc,pp_QoP->v_Veracc,pp_QoP->v_MaxLocAge,pp_QoP->v_Delay));

}
#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 33

/******************************************************************************/
/* CGPS4_33UserConfigurationFileRead : Update Configuration from Configuration file                                                    */
/******************************************************************************/
void CGPS4_33UserConfigurationFileRead()
{
    t_gfl_FILE * UserConfigFILE;

    t_cgps_Config  vl_Config;
    if ((UserConfigFILE = MC_GFL_FOPEN(CGPS_USER_CONFIGURATION_FILE_NAME,(const uint8_t*)"rb")) == NULL)
    {
      MC_CGPS_TRACE_ERR(("ERR : LbsConfig.cfg File Not Found"));

        /* Update with Default values */
        MC_CGPS_TRACE(("INF: Updated User Config with Default values"));


#ifdef LBS_GLONASS
        //GPS Enabled, Glonass Enabled,SBAS Disabled ->0111110100111111111 = 256511

        vg_cgps_user_config.v_ConfigMask =    K_CGPS_CONFIG_MS_POS_TECH_MSASSISTED
                                            | K_CGPS_CONFIG_MS_POS_TECH_MSBASED
                                            | K_CGPS_CONFIG_MS_POS_TECH_AUTONOMOUS
                                            | K_CGPS_CONFIG_MS_POS_TECH_ECID
                                            | K_CGPS_CONFIG_AGPS_TRANSPORT_UPLANE
                                            | K_CGPS_CONFIG_AGPS_TRANSPORT_CPLANE
                                            | K_CGPS_CONFIG_AGPS_TRANSPORT_PREF_UPLANE
                                            | K_CGPS_CONFIG_AGPS_SESSION_MO_ALLOWED
                                            | K_CGPS_CONFIG_AGPS_SESSION_MT_ALLOWED
                                            | K_CGPS_CONFIG_NOTLS_SUPPORTED
                                            | K_CGPS_CONFIG_AGPS_FTA
                                            | K_CGPS_CONFIG_FREQ_AIDING
                                            | K_CGPS_CONFIG_SEMIAUTO
                                            | K_CGPS_CONFIG_GPS_ENABLED
                                            | K_CGPS_CONFIG_GLONASS_ENABLED;

#else

        //59903  -->  1110100111111111
        //vg_cgps_user_config.v_ConfigMask    = 59903; /* Multi Session = 0, NBP_Supported = 0, SUPL2.0 = 0, Others = 1*/
        vg_cgps_user_config.v_ConfigMask =    K_CGPS_CONFIG_MS_POS_TECH_MSASSISTED
                                            | K_CGPS_CONFIG_MS_POS_TECH_MSBASED
                                            | K_CGPS_CONFIG_MS_POS_TECH_AUTONOMOUS
                                            | K_CGPS_CONFIG_MS_POS_TECH_ECID
                                            | K_CGPS_CONFIG_AGPS_TRANSPORT_UPLANE
                                            | K_CGPS_CONFIG_AGPS_TRANSPORT_CPLANE
                                            | K_CGPS_CONFIG_AGPS_TRANSPORT_PREF_UPLANE
                                            | K_CGPS_CONFIG_AGPS_SESSION_MO_ALLOWED
                                            | K_CGPS_CONFIG_AGPS_SESSION_MT_ALLOWED
                                            | K_CGPS_CONFIG_NOTLS_SUPPORTED
                                            | K_CGPS_CONFIG_AGPS_FTA
                                            | K_CGPS_CONFIG_FREQ_AIDING
                                            | K_CGPS_CONFIG_SEMIAUTO;

#endif

#ifdef LBS_QZSS
        vg_cgps_user_config.v_ConfigMask = vg_cgps_user_config.v_ConfigMask | K_CGPS_CONFIG_QZSS_ENABLED;
#endif



        vg_cgps_user_config.v_Config2Mask        = 31; /* Bearer Config = 0, ESLP Config = 0, Others = 1 */

        vg_cgps_user_config.v_ColdStart = K_CGPS_COLD_START_DEFAULT;
        vg_cgps_user_config.v_PowerPref = GN_GPS_POW_PERF_DEFAULT;
        vg_cgps_user_config.v_SensMode = GN_GPS_SENS_MODE_DYNAMIC;
        vg_cgps_user_config.v_SensTtff = GN_GPS_COLD_TTFF_BALANCE;


        vg_cgps_user_config.v_Slp_Address_Type = K_CGPS_SLPADDR_IPV4;
        vg_cgps_user_config.v_Port_Num = 7275;
        memcpy(vg_cgps_user_config.v_Slp_Address, "www.spirent-lcs.com", strlen("www.spirent-lcs.com") + 1 );
        vg_cgps_user_config.v_Slp_AddrLen = strlen( (const char*)vg_cgps_user_config.v_Slp_Address );


    }
    else
    {
        memset(&vl_Config, 0, sizeof(vl_Config));
        MC_CGPS_TRACE(("INF : LbsConfig.cfg File Found"));

        MC_GFL_FREAD(&vg_cgps_user_config, sizeof(char), sizeof( t_CGPSUserConfig ), UserConfigFILE);

        vl_Config.v_ConfigMaskValidity = 0xFFFFFFFF;
        vl_Config.v_ConfigMask         = vg_cgps_user_config.v_ConfigMask;
        vl_Config.v_Config2Mask        = vg_cgps_user_config.v_Config2Mask;



        MC_CGPS_TRACE(("INF : User Configuration Version: %s",vg_cgps_user_config.v_Version));

      if (strcmp ((const char*)vg_cgps_user_config.v_Version, (const char*)vg_CGPS_Config_Ver) == 0)
      {/* Apn */
        {
            uint8_t vl_LengthApn            = strlen( (const char*)vg_cgps_user_config.v_Apn );
            vl_Config.s_BearerConfig.s_APN  = (int8_t*)MC_RTK_GET_MEMORY(vl_LengthApn + 1);

            if( vl_Config.s_BearerConfig.s_APN == NULL )
            {
               MC_CGPS_TRACE_ERR(("ERR : Memory could not be allocated"));
            }
            else
            {
              strncpy((char*)vl_Config.s_BearerConfig.s_APN, (const char*)vg_cgps_user_config.v_Apn, vl_LengthApn +1);
            }
        }

        /* Login */
        {
            uint8_t vl_LengthLogin             = strlen( (const char*)vg_cgps_user_config.v_Login );
            vl_Config.s_BearerConfig.s_Login   = (int8_t*)MC_RTK_GET_MEMORY(vl_LengthLogin + 1);

            if(vl_Config.s_BearerConfig.s_Login == NULL)
            {
               MC_CGPS_TRACE_ERR(("ERR : Memory could not be allocated"));
            }
            else
            {
                strncpy( (char*)vl_Config.s_BearerConfig.s_Login, (const char*)vg_cgps_user_config.v_Login, vl_LengthLogin +1);
            }
        }

        /* Password */
        {
            uint8_t vl_LengthPassword            = strlen( (const char*)vg_cgps_user_config.v_Password );
            vl_Config.s_BearerConfig.s_Password  = (int8_t*)MC_RTK_GET_MEMORY(vl_LengthPassword + 1);

            if(vl_Config.s_BearerConfig.s_Password == NULL)
            {
               MC_CGPS_TRACE_ERR(("ERR : Memory could not be allocated"));
            }
            else
            {
                strncpy( (char*)vl_Config.s_BearerConfig.s_Password, (const char*)vg_cgps_user_config.v_Password, vl_LengthPassword +1);
            }
        }

        /*SLP Address*/
        {
             uint8_t vl_LengthSlpAddr                 = strlen( (const char*)vg_cgps_user_config.v_Slp_Address );

             vl_Config.v_ServerConfig.v_AddrType   = vg_cgps_user_config.v_Slp_Address_Type;
             vl_Config.v_ServerConfig.v_PortNum    = vg_cgps_user_config.v_Port_Num;
             vl_Config.v_ServerConfig.v_Addr       = (int8_t*)MC_RTK_GET_MEMORY(vl_LengthSlpAddr + 1);

             if(vl_Config.v_ServerConfig.v_Addr == NULL)
             {
                MC_CGPS_TRACE_ERR(("ERR : Memory could not be allocated"));
             }
             else
             {
                strncpy((char *)vl_Config.v_ServerConfig.v_Addr, (char *)vg_cgps_user_config.v_Slp_Address, vl_LengthSlpAddr +1);
                vl_Config.v_ServerConfig.v_AddrLen = vl_LengthSlpAddr;
             }

        }

        /* Slp Address */
        {
             uint8_t  vl_Index;
             uint8_t vl_LengthESlpAddr                 = strlen( (const char*)vg_cgps_user_config.v_ESlp_Address);

             MC_CGPS_TRACE_ERR(("vl_LengthESlpAddr = %d ", vl_LengthESlpAddr ));

             //e-slp.spirent-lcs.com
             for(vl_Index=0;vl_Index<CGPS_MAX_ESLP_ADDRESS;vl_Index++)
             {
                if(vl_Config.v_EslpServerConfig.s_ESlpAddr[vl_Index] == NULL)
                    vl_Config.v_EslpServerConfig.s_ESlpAddr[vl_Index] = (int8_t*)MC_RTK_GET_MEMORY(CGPS_MAX_ESLP_ADDR_LENGTH + 1);
             }
             CGPS4_37EslpAddressPopulate(vl_Config,vl_LengthESlpAddr);

        }

        vl_Config.v_ColdStart = vg_cgps_user_config.v_ColdStart;
        if( !(vl_Config.v_Config2Mask | K_CGPS_CONFIG2_COLD_START_MODE))
        {
            vg_cgps_user_config.v_ColdStart = K_CGPS_COLD_START_DEFAULT;
            vl_Config.v_ColdStart = vg_cgps_user_config.v_ColdStart;
        }

        vl_Config.v_PowerPref = vg_cgps_user_config.v_PowerPref;
        if( !(vl_Config.v_Config2Mask | K_CGPS_CONFIG2_POWER_PREFERENCE))
        {
            vg_cgps_user_config.v_PowerPref = GN_GPS_POW_PERF_DEFAULT;
            vl_Config.v_PowerPref = vg_cgps_user_config.v_PowerPref;

        }

        vl_Config.v_SensMode = vg_cgps_user_config.v_SensMode;
        if( !(vl_Config.v_Config2Mask | K_CGPS_CONFIG2_SENSITIVITY_MODE))
        {
            vg_cgps_user_config.v_SensMode = GN_GPS_SENS_MODE_DYNAMIC;
            vl_Config.v_SensMode = vg_cgps_user_config.v_SensMode;

        }

        vl_Config.v_SensTtff = vg_cgps_user_config.v_SensTtff;
        if( !(vl_Config.v_Config2Mask | K_CGPS_CONFIG2_TTFF_SENSITIVITY))
        {
            vg_cgps_user_config.v_SensTtff = GN_GPS_COLD_TTFF_BALANCE;
            vl_Config.v_SensTtff = vg_cgps_user_config.v_SensTtff;

        }

        MC_GFL_FCLOSE(UserConfigFILE);

        vg_CGPS_Config_Ver_flag = TRUE;
        MC_CGPS_UPDATE_USER_CONFIGURATION(&vl_Config);
      }
      else
      {
        vg_CGPS_Config_Ver_flag = FALSE;
        CGPS4_41CheckConfigToolVersionMismatch();
     }
   }

}

#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 34

/*********************************************************************************/
/* CGPS4_34UserConfigurationFileWrite : Update Configuration from database toConfiguration file*/
/*********************************************************************************/
void CGPS4_34UserConfigurationFileWrite()
{
    t_gfl_FILE * UserConfigFILEUpdate;
    uint8_t vl_LengthVersion;

    if ((UserConfigFILEUpdate = MC_GFL_FOPEN(CGPS_USER_CONFIGURATION_FILE_NAME,(const uint8_t*)"wb")) == NULL)
    {
      MC_CGPS_TRACE_ERR(("ERR : LbsConfig.cfg File Not Found for Updating"));
    }
    else
    {
        MC_CGPS_TRACE_INF(("INF : LbsConfig.cfg File Found for Updating"));

        vg_cgps_user_config.v_ConfigMask = vg_CGPS_Configuration;
        vg_cgps_user_config.v_Config2Mask = 0xFFFF;


        vl_LengthVersion               = strlen( (const char*)vg_cgps_user_config.v_Version);
        strncpy((char *)vg_cgps_user_config.v_Version,(char *)vg_cgps_user_config.v_Version , vl_LengthVersion +1);

#ifdef AGPS_UP_FTR

        if ( vg_CGPS_SUPL_Bearer.s_APN )
        {
            uint8_t vl_LengthApn                 = strlen( (const char*)vg_CGPS_SUPL_Bearer.s_APN );
            strncpy((char*)vg_cgps_user_config.v_Apn, (const char*)vg_CGPS_SUPL_Bearer.s_APN, vl_LengthApn +1);
        }

        if(vg_CGPS_SUPL_Bearer.s_Login)
        {
            uint8_t vl_LengthLogin                = strlen( (const char*)vg_CGPS_SUPL_Bearer.s_Login);
            strncpy((char*)vg_cgps_user_config.v_Login, (const char*)vg_CGPS_SUPL_Bearer.s_Login , vl_LengthLogin +1);
        }

        if( vg_CGPS_SUPL_Bearer.s_Password )
        {
            uint8_t vl_LengthPassword                = strlen( (const char*)vg_CGPS_SUPL_Bearer.s_Password );
            strncpy((char*)vg_cgps_user_config.v_Password, (const char*)vg_CGPS_SUPL_Bearer.s_Password , vl_LengthPassword +1);
        }

        if(  vg_CGPS_Supl_Slp_Config.v_Addr != NULL)
        {
            vg_cgps_user_config.v_Slp_Address_Type = vg_CGPS_Supl_Slp_Config.v_AddrType;
            vg_cgps_user_config.v_Port_Num = vg_CGPS_Supl_Slp_Config.v_PortNum;


            strncpy((char *)vg_cgps_user_config.v_Slp_Address, (const char*)vg_CGPS_Supl_Slp_Config.v_Addr, vg_CGPS_Supl_Slp_Config.v_AddrLen +1);
            vg_cgps_user_config.v_Slp_AddrLen = vg_CGPS_Supl_Slp_Config.v_AddrLen;

        }
#endif
        vg_cgps_user_config.v_ColdStart = vg_CGPS_ColdStartMode;

        MC_GFL_FWRITE(&vg_cgps_user_config, 1, sizeof(t_CGPSUserConfig), UserConfigFILEUpdate);

        MC_GFL_FCLOSE(UserConfigFILEUpdate);
    }



}



#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 35

/******************************************************************************/
/* CGPS4_35LoggingConfigurationFileRead : Update Logging Configuration from Configuration file                                                    */
/******************************************************************************/
void CGPS4_35LoggingConfigurationFileRead()
{
    LBSCFG_LogReadConfiguration();
}



#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 37
/******************************************************************************/
/* CGPS4_37EslpAddressPopulate : Populate the ESLP Addresses from the User Configuration File */
/******************************************************************************/
void CGPS4_37EslpAddressPopulate(t_cgps_Config  vl_Config,uint8_t vl_LengthESlpAddr )
{
    uint16_t   v_CompleteStringIndex = 0;
    uint16_t   vl_EslpIndex = 0;
    int8_t*    vp_ESlpAddr  = NULL;

    for(vl_EslpIndex=0;vl_EslpIndex<CGPS_MAX_ESLP_ADDRESS;vl_EslpIndex++)
    {
        if(vl_Config.v_EslpServerConfig.s_ESlpAddr[vl_EslpIndex] == NULL)
        {
            MC_CGPS_TRACE_ERR(("ERR : Memory could not be allocated"));
        }
        else
        {
            vp_ESlpAddr = vl_Config.v_EslpServerConfig.s_ESlpAddr[vl_EslpIndex];

            MC_CGPS_TRACE(("vl_EslpIndex %i", vl_EslpIndex ));

            for( ;v_CompleteStringIndex < vl_LengthESlpAddr && *(vg_cgps_user_config.v_ESlp_Address + v_CompleteStringIndex) != ';'; v_CompleteStringIndex++)
            {
                *vp_ESlpAddr = *(vg_cgps_user_config.v_ESlp_Address + v_CompleteStringIndex);
                vp_ESlpAddr++;
            }
            *vp_ESlpAddr = '\0';
            v_CompleteStringIndex++;
            MC_CGPS_TRACE(("vp_ESlpAddr %s , v_CompleteStringIndex %i ", vl_Config.v_EslpServerConfig.s_ESlpAddr[vl_EslpIndex], v_CompleteStringIndex ));

        }
    }

}


#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 37
/******************************************************************************/
/* CGPS4_38UpdateConfiguration : Update Configuration to Position engine*/
/******************************************************************************/
void CGPS4_38UpdateConfiguration()
{
    t_lbscfg_LogParam vl_Config_GnNav;
    t_lbscfg_LogParam vl_Config_GnEvent;
    t_lbscfg_LogParam vl_Config_GnGnb;
    s_GN_GPS_Config   vl_GPS_Config;

    LBSCFG_LogGetConfiguration(K_LBSCFG_CHIP_LOG1, &vl_Config_GnGnb);
    LBSCFG_LogGetConfiguration(K_LBSCFG_CHIP_LOG3, &vl_Config_GnNav);
    LBSCFG_LogGetConfiguration(K_LBSCFG_CHIP_LOG4, &vl_Config_GnEvent);

    if( GN_GPS_Get_Config( &vl_GPS_Config ) )
    {
        vl_GPS_Config.GPGLL_Rate        = 0;     /* Turn off NMEA $GPGLL*/
        vl_GPS_Config.GPVTG_Rate        = 0;     /* Turn off NMEA $GPVTG*/
        vl_GPS_Config.GPZDA_Rate        = 0;     /* Turn off NMEA $GPZDA*/
        vl_GPS_Config.ForceCold_Timeout = 0;
        vl_GPS_Config.NV_Write_Interval = 10;
        /*vl_GPS_Config.BGA_Chip            = FALSE; *//* CSP part used */
        /*      vl_GPS_Config.PosFiltMode = 0;   */      /* Turn off position filtering */
        vl_GPS_Config.Static_Filt_Enabled =TRUE;

        /* This field becomes valid when using library version 3.01.3-090430 and later */
        vl_GPS_Config.SBAS_Enabled      = FALSE; /* Disable SBAS usage */

        /* ++Below fields updated from the User Configuration */
        vl_GPS_Config.ColdTTFF          = vg_cgps_user_config.v_SensTtff;
        vl_GPS_Config.SensMode          = vg_cgps_user_config.v_SensMode;
        vl_GPS_Config.PowerPerf          = vg_cgps_user_config.v_PowerPref;

        vl_GPS_Config.Enable_Nav_Debug  = vl_Config_GnNav.v_Opt? 1:0;
        vl_GPS_Config.Enable_GNB_Debug  = vl_Config_GnGnb.v_Opt? 1:0;

        if(vl_Config_GnEvent.v_Opt > 7) //Debug enabled
           vl_GPS_Config.Enable_Event_Log  = 4;
        else if(vl_Config_GnEvent.v_Opt > 0)
           vl_GPS_Config.Enable_Event_Log  = 3;
        else
           vl_GPS_Config.Enable_Event_Log  = 0;

        /* --Below fields updated from the User Configuration */

#ifdef LBS_GLONASS
        vl_GPS_Config.GPS__Enabled = (vg_CGPS_Configuration & K_CGPS_CONFIG_GPS_ENABLED)? TRUE : FALSE ;
        vl_GPS_Config.GLON_Enabled = (vg_CGPS_Configuration & K_CGPS_CONFIG_GLONASS_ENABLED)? TRUE : FALSE;
        vl_GPS_Config.SBAS_Enabled = (vg_CGPS_Configuration & K_CGPS_CONFIG_SBAS_ENABLED)? TRUE : FALSE ;
#endif

#ifdef LBS_QZSS
        vl_GPS_Config.QZSS_Enabled = (vg_CGPS_Configuration & K_CGPS_CONFIG_QZSS_ENABLED)? TRUE : FALSE;
#endif


        GN_GPS_Set_Config( &vl_GPS_Config );
    }
    else
    {
        MC_CGPS_TRACE(("GNSS_Driver_OFF_Start: GN_GPS_Get_Config failed"));
    }
}
#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 39

/******************************************************************************/
/* CGPS4_39PGNVCommandsHandler : Handles PGNV commands*/
/******************************************************************************/

void CGPS4_39PGNVCommandsHandler()
{
    uint16_t  vl_pgnvCommand;
    s_GN_GPS_Nav_Data vl_NavDataTmp;

    /* If there is a Native App Framework user check to see if they have input*/
    /*any $PGNV commands or test data.*/
    if( vg_CGPS_Naf_Count <= 0 ) return;


    /* Check for $PGNVS,...  diagnostic / test / control inputs.*/
    vl_pgnvCommand = GN_GPS_Parse_PGNV();

    switch ( vl_pgnvCommand )
    {
/*------------------------------------------------------------------------------------------*/
    case GN_PGNV_WAKE:
    {
#ifdef AGPS_UP_FTR
        CGPS5_13StartSUPL( NULL,NULL );
#endif /* AGPS_UP_FTR */
        /* send an acknowledge to all registered applications */
        CGPS4_11SendAcknowledge("$PGNVR,WAKE,ACK,*20\x0D\x0A");
        MC_HALGPS_SLEEP(FALSE);
    }
    break;
/*------------------------------------------------------------------------------------------*/
    case GN_PGNV_GSLP:
    {
        /* Do not want to send a Sleep command while already sleeping*/
        /*  because this will actually cause a Wake-Up! (UART activity!)*/
        /* send an acknowledge to all registered applications */
        CGPS4_11SendAcknowledge("$PGNVR,GSLP,ACK,*30\x0D\x0A");
        MC_HALGPS_SLEEP(TRUE);
    }
    break;
/*------------------------------------------------------------------------------------------*/
    case GN_PGNV_SDWN:
    {
        /* ++ LMSqb80273 */
        memset( (uint8_t*)&vg_Nav_Data, 0, sizeof(vl_NavDataTmp) );
        /* -- LMSqb80273 */
        memset( (uint8_t*)&vg_Latest_Fix_Nav_Data, 0, sizeof(vl_NavDataTmp) );
    }
    break;
/*------------------------------------------------------------------------------------------*/
    case GN_PGNV_RSET:
    {
#ifdef AGPS_UP_FTR
        CGPS5_13StartSUPL( NULL,NULL );
#endif /* AGPS_UP_FTR */
        /* ++ LMSqb80273 */
        memset( (uint8_t*)&vg_Nav_Data, 0, sizeof(vl_NavDataTmp) );
        /* -- LMSqb80273 */
        memset( (uint8_t*)&vg_Latest_Fix_Nav_Data, 0, sizeof(vl_NavDataTmp) );

        /* send an acknowledge to all registered applications */
        CGPS4_11SendAcknowledge("$PGNVR,RSET,ACK,*28\x0D\x0A");
    }
    break;
/*------------------------------------------------------------------------------------------*/
    default:
        /* No action required.*/
        break;
    }

}
#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 40


/******************************************************************************/
/* CGPS4_40ConfigurationPrint : This function is used to print the configuration values */
/******************************************************************************/
void CGPS4_40ConfigurationPrint()
{
    MC_CGPS_TRACE(("LBSConfig -  MS assisted positioning method - %i",
            (vg_CGPS_Configuration & K_CGPS_CONFIG_MS_POS_TECH_MSASSISTED)?TRUE:FALSE ));

    MC_CGPS_TRACE(("LBSConfig -  MS Based positioning method - %i",
        (vg_CGPS_Configuration & K_CGPS_CONFIG_MS_POS_TECH_MSBASED)?TRUE:FALSE ));

    MC_CGPS_TRACE(("LBSConfig -  Autonomous positioning method - %i",
        (vg_CGPS_Configuration & K_CGPS_CONFIG_MS_POS_TECH_AUTONOMOUS)?TRUE:FALSE ));
#ifdef GPS_FREQ_AID_FTR
    MC_CGPS_TRACE(("LBSConfig -  Frequency aiding allowed - %i",
        (vg_CGPS_Configuration & K_CGPS_CONFIG_FREQ_AIDING)?TRUE:FALSE ));
#endif /*GPS_FREQ_AID_FTR*/

#ifdef AGPS_UP_FTR
    MC_CGPS_TRACE(("LBSConfig -  User Plane allowed - %i",
        (vg_CGPS_Configuration & K_CGPS_CONFIG_AGPS_TRANSPORT_UPLANE)?TRUE:FALSE ));

    MC_CGPS_TRACE(("LBSConfig -  Cell ID positioning method - %i",
        (vg_CGPS_Configuration & K_CGPS_CONFIG_MS_POS_TECH_ECID)?TRUE:FALSE ));

    MC_CGPS_TRACE(("LBSConfig -  multi SI session Allowed - %i",
        (vg_CGPS_Configuration & K_CGPS_CONFIG_SUPL_MULTI_SESSION)?TRUE:FALSE ));

    MC_CGPS_TRACE(("LBSConfig -  NO TLS Supported - %i",
        (vg_CGPS_Configuration & K_CGPS_CONFIG_NOTLS_SUPPORTED)?TRUE:FALSE ));

    MC_CGPS_TRACE(("LBSConfig -  SUPL v2.0 - %i",
        (vg_CGPS_Configuration & K_CGPS_CONFIG_SUPLV2_SUPPORTED)?TRUE:FALSE ));
#endif /*AGPS_UP_FTR*/

#ifdef AGPS_FTR
    MC_CGPS_TRACE(("LBSConfig -  Control Plane Allowed - %i",
        (vg_CGPS_Configuration & K_CGPS_CONFIG_AGPS_TRANSPORT_CPLANE)?TRUE:FALSE ));
    MC_CGPS_TRACE(("LBSConfig -  Fine Time Assistance Allowed - %i",
        (vg_CGPS_Configuration & K_CGPS_CONFIG_AGPS_FTA)?TRUE:FALSE ));
#endif

#if defined(AGPS_UP_FTR) || defined(AGPS_FTR)
    MC_CGPS_TRACE(("LBSConfig -  MO positioning Allowed - %i",
        (vg_CGPS_Configuration & K_CGPS_CONFIG_AGPS_SESSION_MO_ALLOWED)?TRUE:FALSE ));

    MC_CGPS_TRACE(("LBSConfig -  MT positioning Allowed - %i",
        (vg_CGPS_Configuration & K_CGPS_CONFIG_AGPS_SESSION_MT_ALLOWED)?TRUE:FALSE ));


    MC_CGPS_TRACE(("LBSConfig -  Preferred UPLANE Allowed - %i",
        (vg_CGPS_Configuration & K_CGPS_CONFIG_AGPS_TRANSPORT_PREF_UPLANE)?TRUE:FALSE ));

    MC_CGPS_TRACE(("LBSConfig -  Semi Auto Location Mode Allowed - %i",
        (vg_CGPS_Configuration & K_CGPS_CONFIG_SEMIAUTO)?TRUE:FALSE ));
#endif
#ifdef LBS_GLONASS
   MC_CGPS_TRACE(("LBSConfig   GPS Enabled - %i",
       (vg_CGPS_Configuration & K_CGPS_CONFIG_GPS_ENABLED)?TRUE:FALSE ));

   MC_CGPS_TRACE(("LBSConfig   GLONASS Enabled- %i",
       (vg_CGPS_Configuration & K_CGPS_CONFIG_GLONASS_ENABLED)?TRUE:FALSE ));

   MC_CGPS_TRACE(("LBSConfig   SBAS Enabled - %i",
       (vg_CGPS_Configuration & K_CGPS_CONFIG_SBAS_ENABLED)?TRUE:FALSE ));

#endif
    MC_CGPS_TRACE(("LBSConfig    EXTENDED EPHEMERIS GPS Enabled - %i",
        (vg_CGPS_Configuration & K_CGPS_CONFIG_EXTENDED_EPHEMERIS_GPS_ENABLED)?TRUE:FALSE ));

    MC_CGPS_TRACE(("LBSConfig    EXTENDED EPHEMERIS_GLONAS Enabled - %i",
        (vg_CGPS_Configuration & K_CGPS_CONFIG_EXTENDED_EPHEMERIS_GLONASS_ENABLED)?TRUE:FALSE ));


    MC_CGPS_TRACE(("LBSConfig    Wi-Fi Positioning Enabled - %i",
        (vg_CGPS_Configuration & K_CGPS_CONFIG_WIFI_POS_ENABLED)?TRUE:FALSE ));

    MC_CGPS_TRACE(("LBSConfig    QZSS Enabled - %i",
        (vg_CGPS_Configuration & K_CGPS_CONFIG_QZSS_ENABLED)?TRUE:FALSE ));
    
    MC_CGPS_TRACE(("LBSConfig    SNTP Reference Time Enabled - %i",
    (vg_CGPS_Configuration & K_CGPS_CONFIG_SNTP_REF_TIME_ENABLED)?TRUE:FALSE ));

    MC_CGPS_TRACE(("LBSConfig    SELF ASSISTED GPS Enabled - %i",
        (vg_CGPS_Configuration & K_CGPS_CONFIG_SELF_ASSISTED_GPS_ENABLED)?TRUE:FALSE ));

    MC_CGPS_TRACE(("LBSConfig    SELF ASSISTED GLONAS Enabled - %i",
        (vg_CGPS_Configuration & K_CGPS_CONFIG_SELF_ASSISTED_GLONASS_ENABLED)?TRUE:FALSE ));
}
#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 41


/******************************************************************************/
/* CGPS4_41CheckConfigToolVersionMismatch() : This function is used to print the configuration according to the version of config tool */
/******************************************************************************/
void CGPS4_41CheckConfigToolVersionMismatch()
{

    MC_CGPS_TRACE(("--------------------------------------------------------------------------------"));

    if(vg_CGPS_Config_Ver_flag == FALSE)
    {
        MC_CGPS_TRACE(("ERROR : InValid LbsConfig.cfg tool version - Please use the tool version: %s", vg_CGPS_Config_Ver ));
        MC_CGPS_TRACE (("Switching  to default Configuration"));
        CGPS4_40ConfigurationPrint();
    }
    else
        CGPS4_40ConfigurationPrint();

    MC_CGPS_TRACE(("--------------------------------------------------------------------------------"));

}


#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 42
/******************************************************************************/
/* CGPS4_42PlatformConfigurationFileRead : Update Platform Configuration from Configuration file                                                    */
/******************************************************************************/
void CGPS4_42PlatformConfigurationFileRead()
{
    bool vl_FileReadSuccess = FALSE;

    t_gfl_FILE * pl_PltConfigFile;

    t_CGPSPlatformConfig   vl_FileContents;
    t_cgps_PlatformConfig  vl_PlatformConfig;

    if ((pl_PltConfigFile = MC_GFL_FOPEN(CGPS_PLATFORM_CONFIGURATION_FILE_NAME,(const uint8_t*)"rb")) != NULL)
    {
        int32_t vl_BytesRead = 0;

        MC_CGPS_TRACE(("INF : LbsPltConfig.cfg File Found"));

        memset((uint8_t*)&vl_FileContents, 0, sizeof(vl_FileContents));

        vl_BytesRead = MC_GFL_FREAD(&vl_FileContents, sizeof(char),sizeof( t_CGPSPlatformConfig ), pl_PltConfigFile);


        if( vl_BytesRead == sizeof( t_CGPSPlatformConfig ) )
        {
            vl_FileReadSuccess = TRUE;
        }
        else
        {
            MC_CGPS_TRACE_ERR(("ERR : Number of bytes read doesnt match the expected value"));
        }

        MC_GFL_FCLOSE( pl_PltConfigFile );
    }
    else
    {
        MC_CGPS_TRACE_ERR(("ERR : LbsPltConfig.cfg File Not Found"));
    }


    if( vl_FileReadSuccess == TRUE )
    {
        MC_CGPS_TRACE(("INF: Update Platform Config with file settings"));

        vl_PlatformConfig.v_Config1MaskValid   = 0xFFFFFFFF;
        vl_PlatformConfig.v_Config1Mask        = vl_FileContents.v_Config1Mask;
        vl_PlatformConfig.v_Config2Mask        = vl_FileContents.v_Config2Mask;

        vl_PlatformConfig.v_MsaMandatoryMask   = vl_FileContents.v_MsaMandatoryMask;
        vl_PlatformConfig.v_MsbMandatoryMask   = vl_FileContents.v_MsbMandatoryMask;

        vl_PlatformConfig.v_SleepTimerDuration = vl_FileContents.v_SleepTimerDuration;
        vl_PlatformConfig.v_ComaTimerDuration  = vl_FileContents.v_ComaTimerDuration;
        vl_PlatformConfig.v_DeepComaDuration   = vl_FileContents.v_DeepComaDuration;
    }
    else
    {
        /* Update with Default values */
        MC_CGPS_TRACE(("INF: Update Platform Config with Default values"));
        memset((uint8_t*)&vl_PlatformConfig, 0, sizeof(vl_PlatformConfig));

        vl_PlatformConfig.v_Config1MaskValid   = 0xFFFFFFFF;

        /*PureMSA CP = 1, PureMSA UP = 0, External LNA = 1*/
        vl_PlatformConfig.v_Config1Mask        = 5;

        /*MSA Mandatory = 1, MSB Mandatory = 1, Delete Aiding Data = 1, GPS State = 1*/
        vl_PlatformConfig.v_Config2Mask        = 7;

#ifdef AGPS_FTR
        vl_PlatformConfig.v_MsaMandatoryMask   = (LSIMCP_AID_REQ_REF_TIME | LSIMCP_AID_REQ_ACQ_ASSIST);
        vl_PlatformConfig.v_MsbMandatoryMask   = (LSIMCP_AID_REQ_REF_TIME | LSIMCP_AID_REQ_REF_LOC | LSIMCP_AID_REQ_NAV_MODEL | LSIMCP_AID_REQ_IONO_MODEL);
#endif /*AGPS_FTR */

        vl_PlatformConfig.v_SleepTimerDuration = K_CGPS_SLEEP_TIMER_DURATION;
        vl_PlatformConfig.v_ComaTimerDuration  = K_CGPS_COMA_TIMER_DURATION;
        vl_PlatformConfig.v_DeepComaDuration   = K_CGPS_DEEPCOMA_TIMER_DURATION;
    }

     MC_CGPS_UPDATE_PLATFORM_CONFIGURATION(&vl_PlatformConfig);
}


#undef  PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 43
/**************************************************************************************************/
/* CGPS4_43ExtLNAConfig :                                                                          */
/**************************************************************************************************/
void CGPS4_43ExtLNAConfig(bool vl_EnableLNA)
{
    if(MC_CGPS_IS_PLATFORM_CONFIGURATION_ELEMENT_SET(K_CGPS_CONFIG_EXT_LNA))
    {
        MC_CGPS_TRACE(("External LNA Mask SET, Antennae select Mask SET"));
        s_GN_GPS_Config   vl_GPS_Config;

        if( GN_GPS_Get_Config( &vl_GPS_Config ) )
        {
            if(vl_EnableLNA)
            {
                vl_GPS_Config.ExtLNA_Enable = TRUE;
                vl_GPS_Config.AntSel = GN_GPS_ANT_SEL_HIGH;
                MC_CGPS_TRACE(("External LNA is enabled, Antennae select is High"));
            }
            else
            {
                vl_GPS_Config.ExtLNA_Enable = FALSE;
                vl_GPS_Config.AntSel = GN_GPS_ANT_SEL_DIASABLED;
                MC_CGPS_TRACE(("External LNA is disabled, Antennae select is disabled"));
            }
        }

        GN_GPS_Set_Config( &vl_GPS_Config );
    }
}


#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 44
/******************************************************************************/
/* CGPS4_44CheckLocationInGeoArea : Check if position is inside defined area   */
/******************************************************************************/
e_cgps_RelativePosition CGPS4_44CheckLocationInGeoArea(t_cgps_GeographicTargetArea* pp_GeoAreaDefn,
                                                       t_Cgps_Coordinate*           pp_CurrentPosition)
{
    e_cgps_RelativePosition vl_RelativePosition = K_CGPS_POSITION_UNKNOWN;

    switch( pp_GeoAreaDefn->v_ShapeType)
    {
        case K_CGPS_CIRCULAR_AREA:
            vl_RelativePosition = CGPS4_45CheckLocationInCircle( *pp_GeoAreaDefn->v_Latitude,
                                                                 *pp_GeoAreaDefn->v_LatSign,
                                                                 *pp_GeoAreaDefn->v_Longitude,
                                                                 pp_GeoAreaDefn->v_CircularRadius,
                                                                 pp_GeoAreaDefn->v_CircularRadiusMin,
                                                                 pp_GeoAreaDefn->v_CircularRadiusMax,
                                                                 pp_CurrentPosition);
            break;
        case K_CGPS_ELLIPTICAL_AREA:
            vl_RelativePosition = CGPS4_46CheckLocationInEllipse( *pp_GeoAreaDefn->v_Latitude,
                                                                  *pp_GeoAreaDefn->v_LatSign,
                                                                  *pp_GeoAreaDefn->v_Longitude,
                                                                  pp_GeoAreaDefn->v_EllipSemiMinorAngle,
                                                                  pp_GeoAreaDefn->v_EllipSemiMajor,
                                                                  pp_GeoAreaDefn->v_EllipSemiMajorMin,
                                                                  pp_GeoAreaDefn->v_EllipSemiMajorMax,
                                                                  pp_GeoAreaDefn->v_EllipSemiMinor,
                                                                  pp_GeoAreaDefn->v_EllipSemiMinorMin,
                                                                  pp_GeoAreaDefn->v_EllipSemiMinorMax,
                                                                  pp_CurrentPosition
                                                                );
            break;
        case K_CGPS_POLYGON_AREA:

            vl_RelativePosition = CGPS4_47CheckLocationInPolygon( pp_GeoAreaDefn->v_Latitude,
                                                                  pp_GeoAreaDefn->v_LatSign,
                                                                  pp_GeoAreaDefn->v_Longitude,
                                                                  pp_GeoAreaDefn->v_NumberOfPoints,
                                                                  pp_GeoAreaDefn->v_PolygonHysteresis,
                                                                  pp_CurrentPosition
                                                                );
            break;
        default:
            MC_CGPS_TRACE(("CGPS5_44CheckEvent : No Target Area Defined"));
            break;
    }

    return vl_RelativePosition;
}


#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 45
/******************************************************************************/
/* CGPS4_45CheckLocationInCircle : Check if position is inside a circle   */
/******************************************************************************/
e_cgps_RelativePosition CGPS4_45CheckLocationInCircle(
                                                              uint32_t  vp_Latitude,
                                                              uint8_t   vp_LatSign,
                                                              int32_t   vp_Longitude,
                                                              uint32_t  vp_CircularRadius,
                                                              uint32_t  vp_CircularRadiusMin,
                                                              uint32_t  vp_CircularRadiusMax,
                                                              t_Cgps_Coordinate*    pp_CurrentPosition
                                                             )
{
    e_cgps_RelativePosition vl_RelativePosition = K_CGPS_POSITION_UNKNOWN;
    vp_CircularRadiusMin = vp_CircularRadiusMin;
    vp_CircularRadiusMax = vp_CircularRadiusMax;

    MC_CGPS_TRACE(("CGPS4_45CheckLocationInCircle : Circle centre is lat %i , long  %i ",  vp_Latitude, vp_Longitude));

    MC_CGPS_TRACE(("CGPS4_45CheckLocationInCircle : Set Position is lat %i, long  %i ",  pp_CurrentPosition->v_Latitude, pp_CurrentPosition->v_Longitude ));

    float vl_RefLat = (float)MC_CGPS_LAT_CONV_GAD_TO_WGS84_RAD(vp_Latitude, vp_LatSign);
    float vl_RefLon = (float)MC_CGPS_LON_CONV_GAD_TO_WGS84_RAD(vp_Longitude);

    float vl_CurLat = (float)MC_CGPS_LAT_CONV_GAD_TO_WGS84_RAD(pp_CurrentPosition->v_Latitude, pp_CurrentPosition->v_LatitudeSign);
    float vl_CurLon = (float)MC_CGPS_LON_CONV_GAD_TO_WGS84_RAD(pp_CurrentPosition->v_Longitude);

    uint32_t vl_HorDist = 0;

    /*Using Spherical Law of Cosines formula for computing horizontal distance*/
    vl_HorDist = (uint32_t)(K_CGPS_EARTH_RADIUS_METERS* acos(cos(vl_RefLat)*cos(vl_RefLon)*cos(vl_CurLat)*cos(vl_CurLon) +
                                                        cos(vl_RefLat)*sin(vl_RefLon)*cos(vl_CurLat)*sin(vl_CurLon) +
                                                        sin(vl_RefLat)*sin(vl_CurLat)));

   MC_CGPS_TRACE(("CGPS4_45CheckLocationInCircle : Circle radius is %i, Distance of point-center is %i ", vp_CircularRadius, vl_HorDist));

   if( vl_HorDist <= vp_CircularRadius )
        vl_RelativePosition =  K_CGPS_POSITION_INSIDE;
   else
        vl_RelativePosition =  K_CGPS_POSITION_OUTSIDE;

   return vl_RelativePosition;
}


#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 46
/******************************************************************************/
/* CGPS4_45CheckLocationInCircle : Check if position is inside a ellipse   */
/******************************************************************************/
e_cgps_RelativePosition CGPS4_46CheckLocationInEllipse(
                                        uint32_t vp_Latitude,
                                        uint8_t  vp_LatSign,
                                        int32_t  vp_Longitude,
                                        uint8_t  vp_EllipSemiMinorAngle,
                                        uint32_t vp_EllipSemiMajor,           /**< Describes the Semi Major of Ellipse */
                                        uint32_t vp_EllipSemiMajorMin,        /**< Describes the Minimum Semi Major of Ellipse */
                                        uint32_t vp_EllipSemiMajorMax,        /**< Describes the Maximum Semi Major of Ellipse */
                                        uint32_t vp_EllipSemiMinor,           /**< Describes the Semi Minor of Ellipse */
                                        uint32_t vp_EllipSemiMinorMin,        /**< Describes the Minimum Semi Minor of Ellipse */
                                        uint32_t vp_EllipSemiMinorMax,        /**< Describes the Maximum Semi Minor of Ellipse */
                                        t_Cgps_Coordinate*  pp_CurrentPosition)
{
    e_cgps_RelativePosition vl_RelativePosition = K_CGPS_POSITION_UNKNOWN;

    vp_EllipSemiMajorMin = vp_EllipSemiMajorMin;
    vp_EllipSemiMajorMax = vp_EllipSemiMajorMax;
    vp_EllipSemiMinorMin = vp_EllipSemiMinorMin;
    vp_EllipSemiMinorMax = vp_EllipSemiMinorMax;

    MC_CGPS_TRACE(("CGPS4_46CheckLocationInEllipse : Current Position Lat %i, Long  %i ",  pp_CurrentPosition->v_Latitude, pp_CurrentPosition->v_Longitude));
    MC_CGPS_TRACE(("CGPS4_46CheckLocationInEllipse : Ellipse center Lat %i, Long %i ",  vp_Latitude, vp_Longitude));

    float foci1_lat,foci1_lon, foci2_lat,foci2_lon,foci_distance;

    float CurrentPoint_distance_from_foci1 = 0, CurrentPoint_distance_from_foci2 = 0, CurrentPoint_distance_from_foci_sum = 0;
    float angle_in_degrees = 0,angle_in_radians=0;

    angle_in_degrees = 90 - vp_EllipSemiMinorAngle;  /* Angle of SemiMajorAxis with respect to Horizontal Axis*/

    angle_in_radians = angle_in_degrees * M_PI/180;    /*convert degree to radians*/


    foci_distance   =  sqrt(vp_EllipSemiMajor*vp_EllipSemiMajor - vp_EllipSemiMinor*vp_EllipSemiMinor);

    float vl_RefLat = (float)MC_CGPS_LAT_CONV_GAD_TO_WGS84_RAD(vp_Latitude, vp_LatSign);
    float vl_RefLon = (float)MC_CGPS_LON_CONV_GAD_TO_WGS84_RAD(vp_Longitude);

    float vl_CurLat = (float)MC_CGPS_LAT_CONV_GAD_TO_WGS84_RAD(pp_CurrentPosition->v_Latitude, pp_CurrentPosition->v_LatitudeSign);
    float vl_CurLon = (float)MC_CGPS_LON_CONV_GAD_TO_WGS84_RAD(pp_CurrentPosition->v_Longitude);


    foci1_lon = vl_RefLon  + cos( angle_in_radians )* foci_distance*(M_PI)/(180*K_CGPS_LAT_to_METERS*cos(vl_RefLat));
    foci1_lat = vl_RefLat  + sin( angle_in_radians )* foci_distance*(M_PI)/(180*K_CGPS_LAT_to_METERS);
    foci2_lon = vl_RefLon  - cos( angle_in_radians )* foci_distance*(M_PI)/(180*K_CGPS_LAT_to_METERS*cos(vl_RefLat));
    foci2_lat = vl_RefLat  - sin( angle_in_radians )* foci_distance*(M_PI)/(180*K_CGPS_LAT_to_METERS);

    CurrentPoint_distance_from_foci1 = CGPS4_54DistanceCalc(vl_CurLat, vl_CurLon, foci1_lat, foci1_lon);
    CurrentPoint_distance_from_foci2 = CGPS4_54DistanceCalc(vl_CurLat, vl_CurLon, foci2_lat, foci2_lon);

    CurrentPoint_distance_from_foci_sum = CurrentPoint_distance_from_foci1 + CurrentPoint_distance_from_foci2;

   MC_CGPS_TRACE(("CGPS4_46CheckLocationInEllipse : SumDistanceFocii %f, MajorAxis %i",  CurrentPoint_distance_from_foci_sum, 2*vp_EllipSemiMajor));


   if( CurrentPoint_distance_from_foci_sum <= (2*vp_EllipSemiMajor))

         vl_RelativePosition =  K_CGPS_POSITION_INSIDE;
   else
        vl_RelativePosition =  K_CGPS_POSITION_OUTSIDE;

   return vl_RelativePosition;
}


#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 47
/******************************************************************************/
/* CGPS4_47CheckLocationInPolygon : Check if position is inside a polygon   */
/******************************************************************************/
e_cgps_RelativePosition CGPS4_47CheckLocationInPolygon( uint32_t*  pp_Latitude,
                                                        uint8_t*   pp_LatSign,
                                                        int32_t*   pp_Longitude,
                                                        uint8_t    vp_NumPoint,
                                                        uint32_t   vp_Hysteresis,
                                                        t_Cgps_Coordinate* pp_CurrentPosition )

{
    e_cgps_RelativePosition vl_RelativePosition = K_CGPS_POSITION_UNKNOWN;

    MC_CGPS_TRACE(("CGPS4_47CheckLocationInPolygon : Polygon is %i,  %i ",  pp_Latitude[0], pp_Longitude[0]));

    uint8_t  vl_NoOfIntersections= 0;   /* Represents the no. of edge which intersect the ray from cp .*/
    uint8_t  vl_Index;
    float    vl_LongMax=0;             /* Variable to hold coordinate of a point lying on the edge of polygon with same latitude as that of a test point for each edge.*/
    bool     vl_IsLongInTrans = FALSE;       /* Indicates whether the polygon area spreads over a longitude( x ) difference of more then 180.*/

    float vl_RefLatOnY[K_CGPS_MAX_POLYGON_POINTS];
    float vl_RefLonOnX[K_CGPS_MAX_POLYGON_POINTS];

    float vl_CurLatOnY = (float)MC_CGPS_LAT_CONV_GAD_TO_WGS84_DEG( pp_CurrentPosition->v_Latitude, pp_CurrentPosition->v_LatitudeSign );
    float vl_CurLonOnX = (float)MC_CGPS_LON_CONV_GAD_TO_WGS84_DEG( pp_CurrentPosition->v_Longitude );

    vp_Hysteresis = vp_Hysteresis;

    for( vl_Index = 0; vl_Index <= vp_NumPoint ; vl_Index++ )
    {
        /* Co-ordinate 0 is repeated again after all the co-ordinates */
        vl_RefLatOnY[vl_Index] = (float)MC_CGPS_LAT_CONV_GAD_TO_WGS84_DEG( pp_Latitude[vl_Index % vp_NumPoint], pp_LatSign[vl_Index % vp_NumPoint] );
        vl_RefLonOnX[vl_Index] = (float)MC_CGPS_LON_CONV_GAD_TO_WGS84_DEG( pp_Longitude[vl_Index % vp_NumPoint] );

        MC_CGPS_TRACE(("CGPS4_47 Polygon isvl_Index %i, vl_RefLatOnY %f, vl_RefLonOnX %f\n " ,vl_Index,vl_RefLatOnY[vl_Index],vl_RefLonOnX[vl_Index]));


    }



    for( vl_Index = 0; vl_Index < vp_NumPoint ; vl_Index++ )
    {
        /* Indicates the presence of longitudes with transition from +180 to -180.*/
        if((vl_RefLonOnX[vl_Index]-vl_RefLonOnX[vl_Index+1])>=180)
            vl_IsLongInTrans = TRUE;
    }

    /* If transition occurs add 360 to all negative longitudes to make it continuous.*/
    if( vl_IsLongInTrans == TRUE )
    {
        for( vl_Index=0; vl_Index <= vp_NumPoint ; vl_Index++ )
        {
            if(vl_RefLonOnX[vl_Index]<=0)
                vl_RefLonOnX[vl_Index] += 360;
        }

        if(vl_CurLonOnX <= 0 )
            vl_CurLonOnX += 360;
    }

    MC_CGPS_TRACE(("CGPS4_47CheckLocation  InPolygon : Polygon is vl_CurLatOnY %f , vl_CurLonOnX %f \n " ,vl_CurLatOnY , vl_CurLonOnX));

    for ( vl_Index=0; vl_Index < vp_NumPoint ; vl_Index++)
    {
        /*Using MIN function Check whether lat coordinate of test point is in the range(minima) of the segment*/
        if ( vl_CurLatOnY > ( MC_CGPS_MIN(vl_RefLatOnY[vl_Index],vl_RefLatOnY[vl_Index+1])) )
        {
            /* Using MAX function Check whether latitude coordinate of test point is in the range(maxima) of the segment*/
            if (vl_CurLatOnY <= ( MC_CGPS_MAX(vl_RefLatOnY[vl_Index],vl_RefLatOnY[vl_Index+1])) )
            {
                /* Using MAX function Check whether longitude coordinate of test point is in the range(maxima) of the segment*/
                if (vl_CurLonOnX <= ( MC_CGPS_MAX(vl_RefLonOnX[vl_Index],vl_RefLonOnX[vl_Index+1])) )
                {
                    /* Find max lon value of lon using slope formula.*/
                    vl_LongMax = ( vl_CurLatOnY-vl_RefLatOnY[vl_Index])*((vl_RefLonOnX[vl_Index+1]-vl_RefLonOnX[vl_Index])/(vl_RefLatOnY[vl_Index+1]-vl_RefLatOnY[vl_Index]))+vl_RefLonOnX[vl_Index];

                    MC_CGPS_TRACE(("CGPS4_47CheckLocationInPolygon : Polygon is vl_CurLatOnY %f , vl_CurLonOnX %f , vl_LongMax  %f\n " ,vl_CurLatOnY , vl_CurLonOnX,vl_LongMax));
                    MC_CGPS_TRACE(("CGPS4_47 Polygon vl_Index %i, vl_RefLatOnY %f, vl_RefLonOnX %f , vl_RefLonOnX[vl_Index+1] %f \n " ,vl_Index,vl_RefLatOnY[vl_Index],vl_RefLonOnX[vl_Index],vl_RefLonOnX[vl_Index+1]));




                    /* If the longitude value is greater, then ray from that point intersects the segment.*/
                    if ( vl_CurLonOnX < vl_LongMax)
                        vl_NoOfIntersections+=1;

                    if(    ( vl_CurLonOnX >= (vl_LongMax - K_CGPS_MARGIN_LONG_DEG))
                        && ( vl_CurLonOnX <= (vl_LongMax + K_CGPS_MARGIN_LONG_DEG))
                       )    /* if the lon value is equal then that point lies on the segment itself.*/
                    {
                        vl_RelativePosition = K_CGPS_POSITION_INSIDE;
                        break;
                    }
                }
            }
        }

        /* To check if latitude of reference point is within the latitude range of the edge defined by 2 points considered*/
        if(      (     vl_CurLatOnY >= (vl_RefLatOnY[vl_Index]   - K_CGPS_MARGIN_LAT_DEG  )
                  &&   vl_CurLatOnY <= (vl_RefLatOnY[vl_Index+1] + K_CGPS_MARGIN_LAT_DEG)
                 )
             ||  (     vl_CurLatOnY <= (vl_RefLatOnY[vl_Index]   + K_CGPS_MARGIN_LAT_DEG  )
                  &&   vl_CurLatOnY >= (vl_RefLatOnY[vl_Index+1] - K_CGPS_MARGIN_LAT_DEG)
                 )
            )
        {

        /* To check if edge is parallel to equator*/
            if(     (vl_RefLatOnY[vl_Index+1]-vl_RefLatOnY[vl_Index]) <= K_CGPS_MARGIN_LAT_DEG
                &&  (vl_RefLatOnY[vl_Index]-vl_RefLatOnY[vl_Index+1]) <= K_CGPS_MARGIN_LAT_DEG
              )
            {
                /* This check segments which are parallel to x-axis as its lat points are excluded in above algorithm.*/
                if(vl_RefLonOnX[vl_Index+1] <= vl_CurLonOnX && vl_CurLonOnX <= vl_RefLonOnX[vl_Index])
                {
                    vl_RelativePosition = K_CGPS_POSITION_INSIDE;
                    break;
                }
            }
        }
    }

    if( vl_RelativePosition== K_CGPS_POSITION_UNKNOWN )
    {
         /* If it crosses odd time then  point is inside.*/
        if ((vl_NoOfIntersections % 2 )== 0)
            vl_RelativePosition =  K_CGPS_POSITION_OUTSIDE;
        else
            vl_RelativePosition =  K_CGPS_POSITION_INSIDE;
    }

    return vl_RelativePosition;
}



#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 48
/******************************************************************************/
/* CGPS4_48CheckForGeoAreaEvent : Check if event has occured      */
/******************************************************************************/
bool CGPS4_48CheckForGeoAreaEvent(t_cgps_GeographicTargetArea* pp_GeoAreaDefn,
                           e_cgps_AreaEventType         vp_AreaEventType,
                           e_cgps_RelativePosition      vp_PrevPositionStatus,
                           e_cgps_RelativePosition*     pp_CurrentPositionStatus,
                           t_Cgps_Coordinate*           pp_CurrentPosition)
{
    bool               vl_EventOccurred  = FALSE;

    e_cgps_RelativePosition vl_RelativePosition = CGPS4_44CheckLocationInGeoArea( pp_GeoAreaDefn,
                                                                                  pp_CurrentPosition);

    MC_CGPS_TRACE(("INF : CGPS4_48CheckForGeoAreaEvent - Current Position %s, Previous Positon %s", \
            a_CGPS_RelativePositionasEnum[vl_RelativePosition],
            a_CGPS_RelativePositionasEnum[vp_PrevPositionStatus]));

    switch( vp_AreaEventType )
    {
        case K_CGPS_ENTERING_AREA:
            switch ( vp_PrevPositionStatus )
            {
                case K_CGPS_POSITION_INSIDE :
                /* Nothing to do */
                    break;
                case K_CGPS_POSITION_OUTSIDE : /*No break - intentional*/
                case K_CGPS_POSITION_UNKNOWN : /*Position previously is unknown, hence report position if inside area*/
                    if( vl_RelativePosition == K_CGPS_POSITION_INSIDE )
                        vl_EventOccurred = TRUE;
                    break;
                default :
                    MC_CGPS_TRACE(("Err : CGPS4_48CheckForGeoAreaEvent - Unknown Position status"));
                    break;
            }
            break;

        case K_CGPS_INSIDE_AREA :
            if( vl_RelativePosition == K_CGPS_POSITION_INSIDE )
                vl_EventOccurred = TRUE;
            break;

        case K_CGPS_OUTSIDE_AREA :
            if( vl_RelativePosition == K_CGPS_POSITION_OUTSIDE )
                vl_EventOccurred = TRUE;
            break;

        case K_CGPS_LEAVING_AREA :
            switch ( vp_PrevPositionStatus )
            {
                case K_CGPS_POSITION_INSIDE : /*No break - intentional*/
                case K_CGPS_POSITION_UNKNOWN : /*Position previously is unknown, hence report position if outside area*/
                    if( vl_RelativePosition == K_CGPS_POSITION_OUTSIDE )
                        vl_EventOccurred = TRUE;
                    break;
                case K_CGPS_POSITION_OUTSIDE :
                    /* Nothing to do */
                    break;
                default :
                    MC_CGPS_TRACE(("ERR : CGPS4_48CheckForGeoAreaEvent - Unknown Position status"));
                    break;
            }
            break;

        default :
            MC_CGPS_TRACE(("ERR : CGPS4_48CheckForGeoAreaEvent - Unknown Area Event Type"));
            break;
    }

        *pp_CurrentPositionStatus = vl_RelativePosition;

    MC_CGPS_TRACE(("INF : CGPS4_48CheckForGeoAreaEvent - Event Occurred %s", (vl_EventOccurred?"TRUE":"FALSE")));
    return vl_EventOccurred;
}



#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 49
/******************************************************************************/
/* CGPS4_49ValidateLatitudeSign : Check if latitude sign is correct */
/******************************************************************************/
e_cgps_Error  CGPS4_49ValidateLatitudeSign(  uint8_t vp_LatSign )
{
    if( vp_LatSign == 0 || vp_LatSign == 1 )
        return K_CGPS_NO_ERROR;
    else
        MC_CGPS_TRACE(("ERR : Latitude sign not correct"));
        return K_CGPS_ERR_INVALID_PARAMETER;
}


#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 50
/******************************************************************************/
/* CGPS4_50ValidateLatitude : Check if latitude value is in range */
/******************************************************************************/
e_cgps_Error  CGPS4_50ValidateLatitude(  uint32_t vp_Latitude )
{
    if( vp_Latitude <= 8388607 )
        return K_CGPS_NO_ERROR;
    else
    {
        MC_CGPS_TRACE(("ERR : Latitude value not correct"));
        return K_CGPS_ERR_INVALID_PARAMETER;
    }
}


#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 51
/******************************************************************************/
/* CGPS4_51ValidateLongitude : Check if longitude value is in range */
/******************************************************************************/
e_cgps_Error  CGPS4_51ValidateLongitude(  int32_t vp_Longitude )
{
    if( vp_Longitude >= -8388608 && vp_Longitude <= 8388607 )
        return K_CGPS_NO_ERROR;
    else
        MC_CGPS_TRACE(("ERR : Longitude value not correct"));
        return K_CGPS_ERR_INVALID_PARAMETER;

}


#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 52
/******************************************************************************/
/* CGPS4_52ValidateTargetAreaParams : Check for the correctness of target area parameters */
/******************************************************************************/
e_cgps_Error  CGPS4_52ValidateTargetAreaParams(  t_cgps_GeographicTargetArea* pp_TargetArea )
{
    e_cgps_Error vl_Return = K_CGPS_NO_ERROR;

    if ( pp_TargetArea  == NULL  )
    {
        MC_CGPS_TRACE(("ERR : Callback not set"));
        vl_Return = K_CGPS_ERR_INVALID_PARAMETER;
    }
    switch( pp_TargetArea->v_ShapeType )
    {
        case K_CGPS_CIRCULAR_AREA :
        {
            MC_CGPS_TRACE(( "Parameter Shape Type : %s", a_CGPS_ShapeTypeasEnum[pp_TargetArea->v_ShapeType]));

            MC_CGPS_TRACE(( "Parameter Lat %i, Lat Sign %i, Long %i", *pp_TargetArea->v_Latitude,
                                                                      *pp_TargetArea->v_LatSign,
                                                                      *pp_TargetArea->v_Longitude));
            MC_CGPS_TRACE(( "Parameter Circle Radius : %i", pp_TargetArea->v_CircularRadius));
            MC_CGPS_TRACE(( "Parameter Circle Radius Min : %i", pp_TargetArea->v_CircularRadiusMin));
            MC_CGPS_TRACE(( "Parameter Circle Radius Max : %i", pp_TargetArea->v_CircularRadiusMax));

            if(    CGPS4_49ValidateLatitudeSign (*pp_TargetArea->v_LatSign)
                || CGPS4_50ValidateLatitude(*pp_TargetArea->v_Latitude)
                || CGPS4_51ValidateLongitude(*pp_TargetArea->v_Longitude)
              )
            {
                vl_Return = K_CGPS_ERR_INVALID_PARAMETER;
            }
            if( pp_TargetArea->v_CircularRadius < 1 || pp_TargetArea->v_CircularRadius > 1000000 )
            {
                MC_CGPS_TRACE(("ERR : Circle radius not in range [range 1..1000000m]."));
                vl_Return = K_CGPS_ERR_INVALID_PARAMETER;
            }
            if( pp_TargetArea->v_CircularRadiusMin < 1 || pp_TargetArea->v_CircularRadiusMin > 1000000 )
            {
                MC_CGPS_TRACE(("ERR : Circle radius not in range [range 1..1000000m]."));
                vl_Return = K_CGPS_ERR_INVALID_PARAMETER;
            }
            if( pp_TargetArea->v_CircularRadiusMax < 1 || pp_TargetArea->v_CircularRadiusMax > 1500000 )
            {
                MC_CGPS_TRACE(("ERR : Circle radius not in range [range 1..1500000m]."));
                vl_Return = K_CGPS_ERR_INVALID_PARAMETER;
            }
        }
        break;
        case K_CGPS_ELLIPTICAL_AREA :
        {
            MC_CGPS_TRACE(( "Parameter Shape Type : %s", a_CGPS_ShapeTypeasEnum[pp_TargetArea->v_ShapeType]));

            MC_CGPS_TRACE(( "Parameter Lat %i, Lat Sign %i, Long %i", *pp_TargetArea->v_Latitude,
                                                                      *pp_TargetArea->v_LatSign,
                                                                      *pp_TargetArea->v_Longitude));

            MC_CGPS_TRACE(( "Parameter Ellipse Semi Major : %i", pp_TargetArea->v_EllipSemiMajor));
            MC_CGPS_TRACE(( "Parameter Ellipse Semi Major Min : %i", pp_TargetArea->v_EllipSemiMajorMin));
            MC_CGPS_TRACE(( "Parameter Ellipse Semi Major Max : %i", pp_TargetArea->v_EllipSemiMajorMax));
            MC_CGPS_TRACE(( "Parameter Ellipse Semi Minor : %i", pp_TargetArea->v_EllipSemiMinor));
            MC_CGPS_TRACE(( "Parameter Ellipse Semi Minor Min : %i", pp_TargetArea->v_EllipSemiMinorMin));
            MC_CGPS_TRACE(( "Parameter Ellipse Semi Minor Max : %i", pp_TargetArea->v_EllipSemiMinorMax));
            MC_CGPS_TRACE(( "Parameter Ellipse Semi Minor Angle : %i", pp_TargetArea->v_EllipSemiMinorAngle));

            if(    CGPS4_49ValidateLatitudeSign (*pp_TargetArea->v_LatSign)
                || CGPS4_50ValidateLatitude(*pp_TargetArea->v_Latitude)
                || CGPS4_51ValidateLongitude(*pp_TargetArea->v_Longitude)
              )
            {
                vl_Return = K_CGPS_ERR_INVALID_PARAMETER;
            }
            if( pp_TargetArea->v_EllipSemiMajor < 1 || pp_TargetArea->v_EllipSemiMajor > 1000000 )
            {
                MC_CGPS_TRACE(("ERR : Ellipse Semi Major not in range [range 1..1000000m]."));
                vl_Return = K_CGPS_ERR_INVALID_PARAMETER;
            }
            if(  pp_TargetArea->v_EllipSemiMajorMin > 1000000 )
            {
                MC_CGPS_TRACE(("ERR : Ellipse Semi Major Min not in range [range 1..1000000m]."));
                vl_Return = K_CGPS_ERR_INVALID_PARAMETER;
            }
            if(  pp_TargetArea->v_EllipSemiMajorMax > 1500000 )
            {
                MC_CGPS_TRACE(("ERR : Ellipse Semi Major Max not in range [range 1..1500000m]."));
                vl_Return = K_CGPS_ERR_INVALID_PARAMETER;
            }
            if( pp_TargetArea->v_EllipSemiMinor < 1 || pp_TargetArea->v_EllipSemiMinor > 1000000 )
            {
                MC_CGPS_TRACE(("ERR : Ellipse Semi Minor not in range [range 1..1000000m]."));
                vl_Return = K_CGPS_ERR_INVALID_PARAMETER;
            }
            if(  pp_TargetArea->v_EllipSemiMinorMin > 1000000 )
            {
                MC_CGPS_TRACE(("ERR : Ellipse Semi Minor Min not in range [range 1..1000000m]."));
                vl_Return = K_CGPS_ERR_INVALID_PARAMETER;
            }
            if( pp_TargetArea->v_EllipSemiMinorMax > 1500000 )
            {
                MC_CGPS_TRACE(("ERR : Ellipse Semi Minor Max not in range [range 1..1500000m]."));
                vl_Return = K_CGPS_ERR_INVALID_PARAMETER;
            }
            if( pp_TargetArea->v_EllipSemiMinorAngle >= 179 )
            {
                MC_CGPS_TRACE(("ERR : Ellipse Semi Minor Angle not in range [range 0..179]."));
                vl_Return = K_CGPS_ERR_INVALID_PARAMETER;
            }
        }
        break;
        case K_CGPS_POLYGON_AREA :
        {
            MC_CGPS_TRACE(( "Parameter Shape Type : %s", a_CGPS_ShapeTypeasEnum[pp_TargetArea->v_ShapeType]));
            MC_CGPS_TRACE(( "Parameter Polygon Num Points : %i", pp_TargetArea->v_NumberOfPoints));
            MC_CGPS_TRACE(( "Parameter Polygon Hysterisis : %i", pp_TargetArea->v_PolygonHysteresis));

            if( pp_TargetArea->v_NumberOfPoints < 3 || pp_TargetArea->v_NumberOfPoints > 15 )
            {
                MC_CGPS_TRACE(("ERR : Polygon - Number of point not in range [range 3..15]."));
                vl_Return = K_CGPS_ERR_INVALID_PARAMETER;
            }
            if( pp_TargetArea->v_PolygonHysteresis > 1000000 )
            {
                MC_CGPS_TRACE(("ERR : Polygon - hysteresis not in range [range 1..1000000]."));
                vl_Return = K_CGPS_ERR_INVALID_PARAMETER;
            }
            else
            {
                uint8_t vl_Index;

                for( vl_Index = 0; vl_Index < pp_TargetArea->v_NumberOfPoints ; vl_Index++ )
                {
                    MC_CGPS_TRACE(( "Parameter Lat %i, Lat Sign %i, Long %i", pp_TargetArea->v_Latitude[vl_Index],
                                                                              pp_TargetArea->v_LatSign[vl_Index],
                                                                              pp_TargetArea->v_Longitude[vl_Index]));
                    if(    CGPS4_49ValidateLatitudeSign (pp_TargetArea->v_LatSign[vl_Index])
                        || CGPS4_50ValidateLatitude(pp_TargetArea->v_Latitude[vl_Index])
                        || CGPS4_51ValidateLongitude(pp_TargetArea->v_Longitude[vl_Index])
                      )
                    {
                        vl_Return = K_CGPS_ERR_INVALID_PARAMETER;
                        break;
                    }
                }
            }
        }
        break;
        default:
            MC_CGPS_TRACE(("ERR : Target Area not a valid shape type"));
            vl_Return = K_CGPS_ERR_INVALID_PARAMETER;
        break;
    }

    return vl_Return;
}


#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 53
/******************************************************************************/
/* CGPS4_53ValidatePositioningParams : Check if Positioning Parameter values are correct  */
/******************************************************************************/
e_cgps_Error  CGPS4_53ValidatePositioningParams( t_cgps_PositioningParams* pp_PositioningParams )
{
    e_cgps_Error vl_Return = K_CGPS_NO_ERROR;

    MC_CGPS_TRACE(( "Parameter NumOfFixes Mask : %i", pp_PositioningParams->v_NumberOfFixes));

    if( !pp_PositioningParams->v_NumberOfFixes )
    {
        MC_CGPS_TRACE(("ERR : Number of fixes not set"));
        vl_Return = K_CGPS_ERR_INVALID_PARAMETER;
    }
    else
    {
        /* Single shot fix */
        if( pp_PositioningParams->v_NumberOfFixes == 1 )
        {
            MC_CGPS_TRACE(( "Parameter Timeout : %i", pp_PositioningParams->v_Timeout ));
            if(    pp_PositioningParams->v_Timeout == 0
                || pp_PositioningParams->v_Timeout > 3600000)
            {
                MC_CGPS_TRACE(("ERR : Timeout not in valid range.[range 1..3600000ms]"));
                vl_Return = K_CGPS_ERR_INVALID_PARAMETER;
            }
        }
        /* Periodic fix request */
        else
        {
            MC_CGPS_TRACE(( "Parameter Fix Interval : %i", pp_PositioningParams->v_FixInterval));
            MC_CGPS_TRACE(( "Parameter Start Time : %i", pp_PositioningParams->v_StartTime));
            MC_CGPS_TRACE(( "Parameter Stop Time : %i", pp_PositioningParams->v_StopTime));

            if(    pp_PositioningParams->v_FixInterval < 100
                || pp_PositioningParams->v_FixInterval > 604800000 )
            {
                MC_CGPS_TRACE(("ERR : FixInterval not in valid range.[range 1..604800000ms]"));
                vl_Return = K_CGPS_ERR_INVALID_PARAMETER;
            }
            if( pp_PositioningParams->v_StartTime > 2678400 )
            {
                MC_CGPS_TRACE(("ERR : StartTime not in valid range.[range 1 to 2678400s]"));
                vl_Return = K_CGPS_ERR_INVALID_PARAMETER;
            }
            if( pp_PositioningParams->v_StopTime > 11318399 )
            {
                MC_CGPS_TRACE(("ERR : StopTime not in valid range.[range 1 to 11318399s]."));
                vl_Return = K_CGPS_ERR_INVALID_PARAMETER;
            }
            if(    pp_PositioningParams->v_StopTime
                && pp_PositioningParams->v_StopTime - pp_PositioningParams->v_StartTime > 8639999 )
            {
                MC_CGPS_TRACE(("ERR : StopTime - StartTime > 8639999s. StopTime can be greater than StartTime not more than 8639999s"));
                vl_Return = K_CGPS_ERR_INVALID_PARAMETER;
            }
        }
    }

    return vl_Return;
}

#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 54
/******************************************************************************/
/* CGPS4_54DistanceCalc: calculates distance between two points.  */
/******************************************************************************/
float CGPS4_54DistanceCalc(float vp_LatRadA, float vp_LongRadA, float vp_LatRadB, float vp_LongRadB)
{
    /*Calculate distance using havesines formula*/

    float vl_DeltaLat  = vp_LatRadB - vp_LatRadA;
    float vl_DeltaLong = vp_LongRadB - vp_LongRadA;
    float vl_ArcTangentRad = 0, vl_GreatCircleDist = 0, vl_Dist = 0;

    vl_ArcTangentRad = ( sin(vl_DeltaLat/2) )*( sin(vl_DeltaLat/2) ) + cos (vp_LatRadA) * cos (vp_LatRadB) *  ( sin(vl_DeltaLong/2) )*( sin(vl_DeltaLong/2) );

    vl_GreatCircleDist = 2* atan2( sqrt(vl_ArcTangentRad), sqrt(1-vl_ArcTangentRad) );

    vl_Dist = K_CGPS_EARTH_RADIUS_METERS * vl_GreatCircleDist;

    return vl_Dist;
}

#undef  PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 55
/**************************************************************************************************/
/* CGPS4_55SendEventNotify : Send event notification to all registered applications               */
/**************************************************************************************************/
void CGPS4_55SendEventNotify ( e_cgps_GpsStatus vp_Result )
{
    if(vg_CgpsEvtNotify != NULL)
    {
        MC_CGPS_TRACE(("CGPS4_55SendEventNotify vp_Result %d ",vp_Result));
        (vg_CgpsEvtNotify)( vp_Result);
    }
}

#ifdef AGPS_UP_FTR

#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 56
/******************************************************************************/
/* CGPS4_56CheckForAreaIdEvent : Check if area id event has occured      */
/******************************************************************************/
bool CGPS4_56CheckForAreaIdEvent(t_cgps_AreaIdList*   pp_AreaIdDefn,
                           e_cgps_AreaEventType         vp_AreaEventType,
                           e_cgps_RelativePosition      vp_PrevPositionStatus,
                           e_cgps_RelativePosition*     pp_CurrentPositionStatus,
                           s_LocationId*                pp_LocationId )

{
    bool               vl_EventOccurred  = FALSE;

    e_cgps_RelativePosition vl_RelativePosition = CGPS4_57CheckLocationInAreaId( pp_AreaIdDefn,
                                                                                 pp_LocationId  );

    MC_CGPS_TRACE(("INF : CGPS4_56CheckForAreaIdEvent - Current Position %s, Previous Positon %s", \
            a_CGPS_RelativePositionasEnum[vl_RelativePosition],
            a_CGPS_RelativePositionasEnum[vp_PrevPositionStatus]));

    switch( vp_AreaEventType )
    {
        case K_CGPS_ENTERING_AREA:
            switch ( vp_PrevPositionStatus )
            {
                case K_CGPS_POSITION_INSIDE :
                /* Nothing to do */
                    break;
                case K_CGPS_POSITION_OUTSIDE : /*No break - intentional*/
                case K_CGPS_POSITION_UNKNOWN : /*Position previously is unknown, hence report position if inside area*/
                    if( vl_RelativePosition == K_CGPS_POSITION_INSIDE )
                        vl_EventOccurred = TRUE;
                    break;
                default :
                    MC_CGPS_TRACE(("Err : CGPS4_56CheckForAreaIdEvent - Unknown Position status"));
                    break;
            }
            break;

        case K_CGPS_INSIDE_AREA :
            if( vl_RelativePosition == K_CGPS_POSITION_INSIDE )
                vl_EventOccurred = TRUE;
            break;

        case K_CGPS_OUTSIDE_AREA :
            if( vl_RelativePosition == K_CGPS_POSITION_OUTSIDE )
                vl_EventOccurred = TRUE;
            break;

        case K_CGPS_LEAVING_AREA :
            switch ( vp_PrevPositionStatus )
            {
                case K_CGPS_POSITION_INSIDE : /*No break - intentional*/
                case K_CGPS_POSITION_UNKNOWN : /*Position previously is unknown, hence report position if outside area*/
                    if( vl_RelativePosition == K_CGPS_POSITION_OUTSIDE )
                        vl_EventOccurred = TRUE;
                    break;
                case K_CGPS_POSITION_OUTSIDE :
                    /* Nothing to do */
                    break;
                default :
                    MC_CGPS_TRACE(("ERR : CGPS4_56CheckForAreaIdEvent - Unknown Position status"));
                    break;
            }
            break;

        default :
            MC_CGPS_TRACE(("ERR : CGPS4_56CheckForAreaIdEvent - Unknown Area Event Type"));
            break;
    }

    *pp_CurrentPositionStatus = vl_RelativePosition;

    MC_CGPS_TRACE(("INF : CGPS4_56CheckForAreaIdEvent - Event Occurred %s", (vl_EventOccurred?"TRUE":"FALSE")));
    return vl_EventOccurred;
}

#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 57
/******************************************************************************/
/* CGPS4_57CheckLocationInAreaId : Check if position is inside defined AreaId   */
/******************************************************************************/
e_cgps_RelativePosition CGPS4_57CheckLocationInAreaId(t_cgps_AreaIdList*   pp_AreaIdDefn,
                                                              s_LocationId*        pp_LocationId)
{

    uint8_t vl_NumAreaId;

    e_cgps_RelativePosition vl_RelativePosition = K_CGPS_POSITION_UNKNOWN;

    for(vl_NumAreaId=0; vl_NumAreaId < pp_AreaIdDefn->v_NumAreaId ; vl_NumAreaId++)
    {

        t_cgps_AreaIdSet *pp_AreaIdSetParms = &(pp_AreaIdDefn->a_AreaIdSetParms[vl_NumAreaId]);

        if( pp_LocationId->Type == pp_AreaIdSetParms->v_AreaIdSet )
        {
            switch( pp_AreaIdSetParms->v_AreaIdSet)
            {
                case K_CGPS_AREA_IDSET_GSMAREAID:
                    vl_RelativePosition = CGPS4_58CheckLocationInGsmAreaId(  pp_AreaIdSetParms->v_GSMRefMcc,
                                                                             pp_AreaIdSetParms->v_GSMRefMnc,
                                                                             pp_AreaIdSetParms->v_GSMRefLac,
                                                                             pp_AreaIdSetParms->v_GSMRefCi,
                                                                             pp_LocationId);
                    break;
                case K_CGPS_AREA_IDSET_WCDMAAREAID:
                    vl_RelativePosition = CGPS4_59CheckLocationInWcdmaAreaId( pp_AreaIdSetParms->v_WCDMARefMcc,
                                                                              pp_AreaIdSetParms->v_WCDMARefMnc,
                                                                              pp_AreaIdSetParms->v_WCDMARefUc,
                                                                              pp_LocationId);
                    break;
                case K_CGPS_AREA_IDSET_CDMAAREAID:

                    vl_RelativePosition = CGPS4_60CheckLocationInCdmaAreaId( pp_AreaIdSetParms->v_CDMARefSid,
                                                                             pp_AreaIdSetParms->v_CDMARefNid,
                                                                             pp_AreaIdSetParms->v_CDMARefBaseid,
                                                                             pp_LocationId);
                    break;
                default:
                    MC_CGPS_TRACE(("CGPS4_57CheckLocationInAreaId : No Area Id Defined"));
                    break;
            }
        }

        if(vl_RelativePosition == K_CGPS_POSITION_INSIDE)
           break;

    }

    return vl_RelativePosition;
}


#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 58
/******************************************************************************/
/* CGPS4_58CheckLocationInGsmAreaId : Check if position is inside a GsmAreaId   */
/******************************************************************************/
e_cgps_RelativePosition CGPS4_58CheckLocationInGsmAreaId(
                                                                   uint16_t  vp_GSMRefMcc,
                                                                   uint16_t  vp_GSMRefMnc,
                                                                   uint16_t  vp_GSMRefLac,
                                                                   uint16_t  vp_GSMRefCi,
                                                                   s_LocationId* pp_LocationId
                                                                  )
{
    e_cgps_RelativePosition vl_RelativePosition = K_CGPS_POSITION_UNKNOWN;

    s_gsmCellInfo  *vl_gsmCellInfo = &(pp_LocationId->of_type.gsmCellInfo);

    MC_CGPS_TRACE(("CGPS4_58CheckLocationInGsmAreaId : Set Id is refMCC %i, refMNC  %i ",  vl_gsmCellInfo->refMCC , vl_gsmCellInfo->refMNC));
    MC_CGPS_TRACE(("CGPS4_58CheckLocationInGsmAreaId : Set Id is refLAC %i, refCI  %i ",  vl_gsmCellInfo->refLAC , vl_gsmCellInfo->refCI ));

    MC_CGPS_TRACE(("CGPS4_58CheckLocationInGsmAreaId  : Area Id is MCC %i , MNC %i ",  vp_GSMRefMcc , vp_GSMRefMnc));
    MC_CGPS_TRACE(("CGPS4_58CheckLocationInGsmAreaId  : Area Id is LAC %i , CI  %i ",  vp_GSMRefLac , vp_GSMRefCi ));

    if( (vl_gsmCellInfo->refMCC == vp_GSMRefMcc) && (vl_gsmCellInfo->refMNC == vp_GSMRefMnc) &&
        (vl_gsmCellInfo->refLAC == vp_GSMRefLac) && (vl_gsmCellInfo->refCI  == vp_GSMRefCi)   )

        vl_RelativePosition =  K_CGPS_POSITION_INSIDE;
    else
        vl_RelativePosition =  K_CGPS_POSITION_OUTSIDE;

    return vl_RelativePosition;
}


#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 59
/******************************************************************************/
/* CGPS4_59CheckLocationInWcdmaAreaId : Check if position is inside a WcdmaAreaId   */
/******************************************************************************/
e_cgps_RelativePosition CGPS4_59CheckLocationInWcdmaAreaId(
                                                                   uint16_t  vp_WCDMARefMcc,
                                                                   uint16_t  vp_WCDMARefMnc,
                                                                   uint32_t  vp_WCDMARefUc,
                                                                   s_LocationId* pp_LocationId
                                                                  )
{
    e_cgps_RelativePosition vl_RelativePosition = K_CGPS_POSITION_UNKNOWN;

    s_wcdmaCellInfo  *vl_wcdmaCellInfo = &(pp_LocationId->of_type.wcdmaCellInfo);

    MC_CGPS_TRACE(("CGPS4_59CheckLocationInWcdmaAreaId : Set Id is refMCC %i, refMNC  %i ",  vl_wcdmaCellInfo->refMCC , vl_wcdmaCellInfo->refMNC));
    MC_CGPS_TRACE(("CGPS4_59CheckLocationInWcdmaAreaId : Set Id is refUC %i ",  vl_wcdmaCellInfo->refUC));

    MC_CGPS_TRACE(("CGPS4_59CheckLocationInWcdmaAreaId  : Area Id is MCC %i , MNC %i ",  vp_WCDMARefMcc , vp_WCDMARefMnc));
    MC_CGPS_TRACE(("CGPS4_59CheckLocationInWcdmaAreaId  : Area Id is UC  %i ",  vp_WCDMARefUc ));

    if( (vl_wcdmaCellInfo->refMCC == vp_WCDMARefMcc) && (vl_wcdmaCellInfo->refMNC == vp_WCDMARefMnc) &&
        (vl_wcdmaCellInfo->refUC  == vp_WCDMARefUc) )

        vl_RelativePosition =  K_CGPS_POSITION_INSIDE;
    else
        vl_RelativePosition =  K_CGPS_POSITION_OUTSIDE;

    return vl_RelativePosition;
}


#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 60
/******************************************************************************/
/* CGPS4_60CheckLocationInCdmaAreaId : Check if position is inside CdmaAreaId  */
/******************************************************************************/
e_cgps_RelativePosition CGPS4_60CheckLocationInCdmaAreaId(
                                                                   uint16_t  vp_CDMARefSid,
                                                                   uint16_t  vp_CDMARefNid,
                                                                   uint16_t  vp_CDMARefBaseid,
                                                                   s_LocationId* pp_LocationId
                                                                  )
{
    e_cgps_RelativePosition vl_RelativePosition = K_CGPS_POSITION_UNKNOWN;

    s_cdmaCellInfo  *vl_cdmaCellInfo = &(pp_LocationId->of_type.cdmaCellInfo);

    MC_CGPS_TRACE(("CGPS4_60CheckLocationInCdmaAreaId : Set Id is refNID %i, refSID  %i ",  vl_cdmaCellInfo->refNID, vl_cdmaCellInfo->refSID));
    MC_CGPS_TRACE(("CGPS4_60CheckLocationInCdmaAreaId : Set Id is refBASEID %i ",  vl_cdmaCellInfo->refBASEID));

    MC_CGPS_TRACE(("CGPS4_60CheckLocationInCdmaAreaId  : Area Id is NID %i, SID  %i ",  vp_CDMARefNid , vp_CDMARefSid));
    MC_CGPS_TRACE(("CGPS4_60CheckLocationInCdmaAreaId  : Area Id is BASEID %i ",  vp_CDMARefBaseid ));

    if( (vl_cdmaCellInfo->refNID == vp_CDMARefNid) && (vl_cdmaCellInfo->refSID == vp_CDMARefSid) && (vl_cdmaCellInfo->refBASEID == vp_CDMARefBaseid) )

        vl_RelativePosition =  K_CGPS_POSITION_INSIDE;
    else
        vl_RelativePosition =  K_CGPS_POSITION_OUTSIDE;

    return vl_RelativePosition;
}
#endif /*AGPS_UP_FTR*/


#if defined (AGPS_FTR) || defined (AGPS_UP_FTR) || defined (AGPS_HISTORICAL_DATA_FTR)
#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 61
/******************************************************************************/
/* CGPS4_56CheckIfAssistanceRequired : Check for assistance data                               */
/******************************************************************************/
bool CGPS4_61CheckIfAssistanceRequired(uint16_t *vl_Mask, s_GN_AGPS_Assist_Req *p_AR)
{
    bool vl_Return     = FALSE;
    bool vl_needAssist = GN_AGPS_Get_Assist_Req( p_AR );
    t_lsimee_GetRefLocationReq *pl_lsimee_GetRefLocationReq = NULL;
    t_lsimee_GetRefTimeReq *pl_lsimee_GetRefTimeReq = NULL;

    *vl_Mask = 0;

    if( vl_needAssist )
    {
    /*+SYScs46505_2*/
        if(( p_AR->Alm_Req ) && (p_AR->Num_Sat_Alm < 32))
    /*+SYScs46505_2*/
        {
#ifndef DO_NOT_USE_ALMANAC
            *vl_Mask |= CGPS_ASSIST_MASK_ALM_MODEL;
#endif
        }

    /*+SYScs46505_2*/
        if(( p_AR->Eph_Req ) && (p_AR->Num_Sat_Eph < 8))
    /*+SYScs46505_2*/
        {
            *vl_Mask |= CGPS_ASSIST_MASK_EPHEMERIS;
        }

        if(( p_AR->Ref_Time_Req ) && (!p_AR->Approx_Time_Known ))
        {
            *vl_Mask |= CGPS_ASSIST_MASK_REF_TIME;
            
            /*Reference TIME Request 194997*/
            if( MC_CGPS_IS_CONFIGURATION_ELEMENT_SET(K_CGPS_CONFIG_SNTP_REF_TIME_ENABLED ))
            {
                MC_CGPS_TRACE(("CGPS_LSIMEE_GET_REFTIME_REQ posted to LSIM for TIME"));

                pl_lsimee_GetRefTimeReq = (t_lsimee_GetRefTimeReq*)MC_RTK_GET_MEMORY(sizeof(*pl_lsimee_GetRefTimeReq));

                CGPS4_28SendMsgToProcess(PROCESS_CGPS,
                                         PROCESS_LSIMUP,
                                         CGPS_LSIMEE_GET_REFTIME_REQ,
                                         (t_MsgHeader *)pl_lsimee_GetRefTimeReq);
            }
            /*Reference TIME Request 194997*/


        }

        if(( p_AR->Ref_Pos_Req ) && ( !p_AR->Approx_Pos_Known ))
        {
#ifdef AGPS_HISTORICAL_DATA_FTR
            t_CGPSHistoricalCellInfo *p_RefLoc;

            p_RefLoc = CGPS10_05SearchIdInList();

            if( NULL != p_RefLoc )
            {
                GN_AGPS_Set_GAD_Ref_Pos( &p_RefLoc->historicalcell_nav_data.gad );
            }
#endif /*AGPS_HISTORICAL_DATA_FTR*/
            *vl_Mask |= CGPS_ASSIST_MASK_REF_POSITION;

            /*XYBRID Integration :194997*/
            if( MC_CGPS_IS_CONFIGURATION_ELEMENT_SET(K_CGPS_CONFIG_WIFI_POS_ENABLED ))
            {
                MC_CGPS_TRACE(("CGPS_LSIMEE_GET_REFLOCATION_REQ posted to LSIM "));

                pl_lsimee_GetRefLocationReq = (t_lsimee_GetRefLocationReq*)MC_RTK_GET_MEMORY(sizeof(*pl_lsimee_GetRefLocationReq));

                CGPS4_28SendMsgToProcess(PROCESS_CGPS,
                                         PROCESS_LSIMUP,
                                         CGPS_LSIMEE_GET_REFLOCATION_REQ,
                                         (t_MsgHeader *)pl_lsimee_GetRefLocationReq);
            }
            /*XYBRID Integration :194997*/
        }

        if ((p_AR->Ion_Req) && ( !p_AR->Ion_Known))
        {
            *vl_Mask |= CGPS_ASSIST_MASK_IONO_MODEL;
        }


        if ((p_AR->UTC_Req) && ( !p_AR->UTC_Known))
        {
            *vl_Mask |= CGPS_ASSIST_MASK_UTC_MODEL;
        }

        if ( p_AR->Bad_SV_List_Req )
        {
            *vl_Mask |= CGPS_ASSIST_MASK_REQ_RTI;
        }

        if ((*vl_Mask & CGPS_ASSIST_MASK_EPHEMERIS) || (*vl_Mask & CGPS_ASSIST_MASK_REF_TIME) || (*vl_Mask & CGPS_ASSIST_MASK_REF_POSITION))
        {
            *vl_Mask |= CGPS_ASSIST_MASK_ACQ_ASSIST;
        }
    /*+SYScs46505*/
        MC_CGPS_TRACE(("CGPS4_61:AssistReq=%d,ReqMsk=%x",vl_needAssist, *vl_Mask));
    /*+SYScs46505*/
    }
    
    if(*vl_Mask > 0)
    {
        vl_Return = TRUE;
    }

    return vl_Return;
}
#endif /* AGPS_FTR || AGPS_UP_FTR || AGPS_HISTORICAL_DATA_FTR */


#undef  PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 62
/**************************************************************************************************/
/* CGPS4_62HandleStartMode : Handle desired start mode for  GPS  operation                                                                  */
/**************************************************************************************************/
void CGPS4_62HandleStartMode()
{

    switch( vg_CGPS_ColdStartMode )
    {
        case K_CGPS_COLD_START_FIRST_ONLY:
            /* We have to do cold start only this time. Next time, normal start. Clear cold start choice */
            vg_CGPS_ColdStartMode = K_CGPS_COLD_START_DEFAULT;
            /*No break - intentional*/
        case K_CGPS_COLD_START_ALWAYS:
            /* We have to do cold start everytime */
            MC_CGPS_TRACE(("CGPS4_62HandleStartMode : COLD Start requested"));
            CGPS4_31ClearNVStore((int8_t *)"COLD");
            memset( (uint8_t*)&vg_Nav_Data, 0, sizeof(s_GN_GPS_Nav_Data) );
            memset( (uint8_t*)&vg_Latest_Fix_Nav_Data, 0, sizeof(s_GN_GPS_Nav_Data) );
        break;

        case K_CGPS_WARM_START_ALWAYS:
            MC_CGPS_TRACE(("CGPS4_62HandleStartMode : WARM Start requested"));
            CGPS4_31ClearNVStore((int8_t*)"E");
            memset( (uint8_t*)&vg_Nav_Data, 0, sizeof(s_GN_GPS_Nav_Data) );
            memset( (uint8_t*)&vg_Latest_Fix_Nav_Data, 0, sizeof(s_GN_GPS_Nav_Data) );
        break;

        case K_CGPS_HOT_START_ALWAYS:
            MC_CGPS_TRACE(("CGPS4_62HandleStartMode : HOT  Start requested"));
            memset( (uint8_t*)&vg_Nav_Data, 0, sizeof(s_GN_GPS_Nav_Data) );
            memset( (uint8_t*)&vg_Latest_Fix_Nav_Data, 0, sizeof(s_GN_GPS_Nav_Data) );
        break;

        default:
        break;
    }
}


#undef  PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 63
/**************************************************************************************************/
/* CGPS4_63PrintGPSEphmeris : This API pulls out the current GPS  Ephemaris used by PE and prints the same. This is only for Debugging purpose */
/**************************************************************************************************/
void CGPS4_63PrintGPSEphmeris()
{

    int i=0;
    s_GN_AGPS_Eph_El Eph_El;


    for (i=1;i<=32;i++)
    {
        if(TRUE == GN_AGPS_Get_Eph_El(i,&Eph_El))
        {

            MC_CGPS_TRACE(("Ephemeris at End of Session got from PE : %3u, %1u , %2u , %2u , %1u , %2u , %2d , %4d , %4d , %4u , %5u , %5u , %4u , %6d , %6d , %5d , %6d , %6d , %6d , %6d , %6d , %6d , %8d , %11d , %10u , %10u , %11d , %11d , %11d , %8d",
            Eph_El.SatID,Eph_El.CodeOnL2,Eph_El.URA,Eph_El.SVHealth,Eph_El.FitIntFlag,Eph_El.AODA,Eph_El.L2Pflag,Eph_El.TGD,Eph_El.af2,Eph_El.Week,Eph_El.toc,Eph_El.toe,Eph_El.IODC,Eph_El.af1,Eph_El.dn,Eph_El.IDot,Eph_El.Crs,Eph_El.Crc,Eph_El.Cus,Eph_El.Cuc,Eph_El.Cis,Eph_El.Cic,Eph_El.af0,Eph_El.M0,Eph_El.e,Eph_El.APowerHalf,Eph_El.Omega0,Eph_El.i0,Eph_El.w,Eph_El.OmegaDot));

        }
        else
        {
            MC_CGPS_TRACE(( "Ephemeris at End of Session got from PE SV:=%2d : NOT AVAILABLE",i));
        }
    }

}


#undef  PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 64
/**************************************************************************************************/
/* CGPS4_64PrintGlonassEphmeris : This API pulls out the current Glonass  Ephemaris used by PE and prints the same. This is only for Debugging purpose */
/**************************************************************************************************/

void CGPS4_64PrintGlonassEphmeris()
{

#ifdef LBS_GLONASS
    int i=0;
    s_GN_AGLON_Eph_El Eph_El;

    for (i=1;i<=24;i++)
    {
        if(TRUE == GN_AGLON_Get_Eph_El(i,&Eph_El))
        {
            MC_CGPS_TRACE(( "Ephemeris at End of Session got from PE :    %2d,    %2d,    %u,    %u,    %u,    %u,    %u,    %d,    %u,    %d,    %d,    %d,    %d,    %d,    %d,    %d,    %d,    %d,    %d,    %d,    %d,    %d",
                          Eph_El.gloN,Eph_El.gloFrqCh,Eph_El.gloTb,Eph_El.gloNT ,Eph_El.gloLn , Eph_El.gloFT ,
                          Eph_El.gloEn, Eph_El.gloP1,Eph_El.gloP2,Eph_El.gloM,Eph_El.gloX ,Eph_El.gloY ,Eph_El.gloZ,Eph_El.gloXdot ,Eph_El.gloYdot , Eph_El.gloZdot , Eph_El.gloXdotdot, Eph_El.gloYdotdot,Eph_El.gloZdotdot,
                          Eph_El.gloDeltaTau,Eph_El.gloTau ,Eph_El.gloGamma));
        }
        else
        {
            MC_CGPS_TRACE(( "Ephemeris at End of Session got from PE SV:=%2d : NOT AVAILABLE",i));
        }
    }
#endif

}


#endif // __CGPS4UTILS_C__
