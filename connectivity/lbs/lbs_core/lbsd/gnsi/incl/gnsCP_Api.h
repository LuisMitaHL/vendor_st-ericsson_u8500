/*****************************************************************************
 *  ST-Ericsson GPS Technology
 *  Copyright (C) 2009 ST-Ericsson.
 *  All rights reserved
 *  Date: 03-12-2009
 *  Author: Rahul Ranade
 *  Email : rahul.ranade@stericsson.com
 *****************************************************************************/
#ifndef GNS_CP_API_H
#define GNS_CP_API_H

#include "gns.h"
#include "gnsCP_Typedefs.h"

/*! \addtogroup Location_gnsCP_Functions */
/*@{*/

/* Callback which is executed to request for platform functionalities.  */
/*!
* \brief    Callback registered with \ref GNS_Initialize
* \details This callback is executed with messages and data which indicate
   what type of Platform calls need to be executed.
* \param Type of Request \ref  e_gnsCP_MsgType
* \param Length of data uint32_t
* \param Data associated with Request \ref  u_gnsCP_MsgData
*/
typedef void (*t_GnsCpCallback)( e_gnsCP_MsgType , uint32_t , u_gnsCP_MsgData* );

/*!
* \brief  Initialises the CP functionality of GNS Module
* \details This API MUST be called before any of the CP functionality can be exercised by OemLbs

* \param v_Callback of type \ref t_GnsCpCallback
* \return             A flag to indicate whether the Initialisation was successful
* \retval TRUE        Init Successful
* \retval FALSE     Init Failure
*/
bool GNS_CpInitialize(t_GnsCpCallback v_Callback );

/*------------------------------------*/
/*!
* \brief  CP Positioning Request to GNS module
* \details
* \param vp_SessionId      Session ID
* \param pp_PosInstruct    pointer of type \ref s_gnsCP_PosInstruct
* \return None
* \retval None
*/
void GNS_CpPosInstructInd( uint8_t vp_SessionId , s_gnsCP_PosInstruct * pp_PosInstruct );

/*!
* \brief  CP Ephemeris [ NavModel ] available
* \details
* \param vp_NumSv    of type uint8_t  : Specifies number of ephemerides available
* \param pp_NavModelElm     pointer of type \ref s_gnsCP_NavModelElm : Pointer to Array of ephemerides. Array size specifies by vp_NumSv
* \return None
* \retval None
*/
void GNS_CpNavModelElmInd( uint8_t vp_NumSv , s_gnsCP_NavModelElm *pp_NavModelElm );

/*!
* \brief  CP Acquisition Assistance available
* \details
* \param pp_AcquisAssist     pointer of type \ref s_gnsCP_AcquisAssist : Pointer to Acquisition Assistance
* \return None
* \retval None
*/
void GNS_CpAcquisAssistInd( s_gnsCP_AcquisAssist *pp_AcquisAssist );

/*!
* \brief  CP Almanac available
* \details
* \param vp_NumSv    of type uint8_t  : Specifies size of almanac array available
* \param pp_AlmanacElm     pointer of type \ref s_gnsCP_AlmanacElm : Pointer to Array of almanac elements. Array size specifies by vp_NumSv
* \param pp_GlobalHealth     pointer of type \ref s_gnsCP_GlobalHealth : Global Health of SVs. If not available, can be NULL
* \return None
* \retval None
*/
void GNS_CpAlmanacElm( uint8_t vp_NumSv , s_gnsCP_AlmanacElm *pp_AlmanacElm , s_gnsCP_GlobalHealth *pp_GlobalHealth );

/*!
* \brief  CP Reference Time available
* \details
* \param pp_RefTime     pointer of type \ref s_gnsCP_RefTime
* \return None
* \retval None
*/
void GNS_CpRefTimeInd( s_gnsCP_RefTime *pp_RefTime );


/*!
* \brief  CP Reference Time available along with Source of time
* \details added new API to provide backward compatibility of the API without Source.
* \param v_Src              Source of the Reference Time
* \param pp_RefTime     pointer of type \ref s_gnsCP_RefTime
* \return None
* \retval None
*/
void GNS_CpRefTimeWithSrcInd(e_gnsAS_AssistSrc v_Src, s_gnsCP_RefTime *pp_RefTime );

/*!
* \brief  CP Reference Position available
* \details
* \param pp_RefPosition     pointer of type \ref s_gnsCP_GpsPosition
* \return None
* \retval None
*/
void GNS_CpRefPositionInd( s_gnsCP_GpsPosition *pp_RefPosition );

/*!
* \brief  CP Ionospheric Model available
* \details
* \param pp_IonoModel     pointer of type \ref s_gnsCP_IonoModel
* \return None
* \retval None
*/
void GNS_CpIonoModelInd( s_gnsCP_IonoModel *pp_IonoModel );

/*!
* \brief  CP UTC Model available
* \details
* \param pp_UtcModel     pointer of type \ref s_gnsCP_UtcModel
* \return None
* \retval None
*/
void GNS_CpUtcModelInd( s_gnsCP_UtcModel *pp_UtcModel );

/*!
* \brief  CP Real Time Integrity available
* \details
* \param pp_RtIntegrity     pointer of type \ref s_gnsCP_RTIntegrity
* \return None
* \retval None
*/
void GNS_CpRtIntegrityInd( s_gnsCP_RTIntegrity *pp_RtIntegrity );

/*!
* \brief  CP DGPS Corrections available
* \details
* \param pp_DgpsCorr     pointer of type \ref s_gnsCP_DGpsCorr
* \return None
* \retval None
*/
void GNS_CpDgpsCorrectionsInd( s_gnsCP_DGpsCorr *pp_DgpsCorr );

/*!
* \brief  CP Session Abort Requested
* \details
* \param pp_Abort     pointer of type \ref s_gnsCP_Abort
* \return None
* \retval None
*/
void GNS_CpAbortInd( s_gnsCP_Abort *pp_Abort );



/*!
* \brief  CP Locatin Notify
* \details
* \param pp_LocNotif     pointer of type \ref s_gnsCP_LocNotification
* \param vp_SessionId     session Id
* \return None
* \retval None
*/
void GNS_CpLocNotificationReq(uint32_t vp_SessionId,s_gnsCP_LocNotification *pp_LocNotif);
/*!
* \brief  CP Locatin Notify Timeout Ind
* \details
* \param vp_SessionId     session Id
* \return None
* \retval None
*/
void GNS_CpLocNotificationAbortInd(uint32_t vp_SessionId); 


/*!
* \brief  Reset UE position stored information
* \details
* \return None
* \retval None
*/
void GNS_CpResetPositionInd( void );



/* +LMSqcMOLR */
/*!
* \brief  Abort MOLR 
* \details
* \param vp_ErrorReason Error reason for aborting the MOLR request
* \param vp_SessionId     session Id
* \param vp_MolrType      \ref e_gnsCP_MolrType
* \return None
* \retval None
*/


void GNS_CpMolrAbortInd(uint8_t vp_SessionId,e_gnsCP_MolrErrorReason vp_ErrorReason, e_gnsCP_MolrType vp_MolrType);

/*!
* \brief  MOLR Response from network
* \details
* \param pp_GpsPosition   \ref s_gnsCP_GpsPosition
* \param vp_SessionId     session Id
* \return None
* \retval None
*/
void GNS_CpMolrStartRespInd(uint8_t vp_SessionId,s_gnsCP_GpsPosition *pp_GpsPosition);
/* -LMSqcMOLR */



/*@}*/


#endif /* GNS_CP_API_H */
