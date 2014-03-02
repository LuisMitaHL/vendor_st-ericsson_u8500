/*****************************************************************************
 *  ST-Ericsson GPS Technology
 *  Copyright (C) 2009 ST-Ericsson.
 *  All rights reserved
 *
 *****************************************************************************/
/*!
 * \file     gnsFA.h
 * \date     10/02/2010
 * \version  0.1
*/

#ifndef GNS_FA_H
#define GNS_FA_H

#include "gns.h"

/*! \addtogroup Location_gnsFA_Constants */
/*@{*/

/*----------------------------------------------------------------------*/
/*!
 *  \enum   e_gnsFA_MsgType
 *      List of constant used to define type of message.
 */
typedef enum
{
    E_gnsFA_CLOCK_CAL_REQ       /*!< Request to start/stop frequency Calibration \ref s_gnsFA_ClockCalReq. */
} e_gnsFA_MsgType;


/*----------------------------------------------------------------------*/
/*!
 *  \enum   e_gnsFA_ClockCalCommand
 *      List of constant used to indicate a registration state.
 */
typedef enum{
    E_gnsFA_CLOCK_CAL_STOP                       = 0,    /*!< Stop  Clock Calibration. */
    E_gnsFA_CLOCK_CAL_START                      = 1,    /*!< Start Clock Calibration. */

} e_gnsFA_ClockCalCommand;


/*!
 *  \enum   e_gnsFA_ClockStatus
 *      List of constant used to indicate a registration state.
 */
typedef enum{
    E_gnsFA_CLOCK_STAT_ACTIVE                  = 0,    /*!< 0 - Clock Calibration Active, Pulse is being generated. */
    E_gnsFA_CLOCK_STAT_NOT_ACTIVE              = 1,    /*!< 1 - Clock Calibration Not Active, Pulse is stopped or not started. */
    E_gnsFA_CLOCK_STAT_NOT_SUPPORTED           = 2,    /*!< 2 - Clock Calibration Not Supported. */
    E_gnsFA_CLOCK_STAT_ALTERNATE_FREQ          = 3,    /*!< 3 - Clock Calibration Not Started-Frequency requested for pulse not supported.. */
    E_gnsFA_CLOCK_STAT_UNKOWN                  = 255,  /*!< 255 - Unknown Values. */

} e_gnsFA_ClockStatus;


/*!
 *  \enum   e_gnsFA_FreqLockStatus
 *      List of constant used to indicate a registration state.
 */
typedef enum{
    E_gnsFA_FREQ_LOCKED                          = 0,    /*!< Frequency is locked to carrier. */
    E_gnsFA_FREQ_NOT_LOCKED                      = 1,    /*!< Frequency is not locked to carrier. */
} e_gnsFA_FreqLockStatus;


/*@}*/


/*! \addtogroup Location_gnsFA_Structures */
/*@{*/

/*!
 *  \struct s_gnsFA_ClockCalReq
 *  \brief  Request to start/stop Clock Calibration procedure in Modem
 */
typedef struct
{
    uint8_t   v_Command;         /*!< Frequency of pulse generated or supported
                                  This field is mandatory if v_Status indicate option (2) else it may just provide
                                  the frequency of the pulse
                              \verbatim
                                  Stop  Clock Calibration.
                                  Start Clock Calibration.
                              \endverbatim
                                  Refer \ref e_gnsFA_ClockCalCommand
                             */
   uint32_t   v_Frequency;     /*!< Frequency of pulse required to be generated [Hz]
                              \verbatim
                                  [range 100Hz .. 10 MHz]
                                  0 - default Pulse Frequency of Modem
                                  u32max - Unknown Value
                              \endverbatim

                             */
    uint32_t   v_Threshold;      /*!< Threshold
                                  This field is mandatory if v_Status indicate option (2) else it may just provide
                                  the frequency of the pulse
                              \verbatim
                                  0 - default values in modem
                                  1 - (u32max-1) - Valid values
                                  (uint32_t max) - Unknown values
                              \endverbatim
                            */
} s_gnsFA_ClockCalReq;

/*!
 *  \struct s_gnsFA_ClockCalInd
 *  \brief  Indicate the status of clock calibration procedure in modem
 */
typedef struct
{
    uint8_t   v_Status;           /*!< Status of Clock Calibration at Modem
                              \verbatim
                                   Clock Calibration Active.
                                   Clock Calibration Not Active.
                                   Clock Calibration Not Supported.
                                   Clock Calibration Not Started-Frequency requested for pulse not supported
                                   Clock Calibration Unknown
                                   4-254 - Reserved Values
                              \endverbatim
                                   Refer \ref e_gnsFA_ClockStatus
                              */
    uint8_t   v_FreqLockStatus;   /*!< Status of Frequency Lock with carrier
                              \verbatim
                                  Frequency is locked to carrier
                                  Frequency is not locked to carrier
                                  2-254 - Reserved Values
                              \endverbatim
                                  Refer \ref e_gnsFA_FreqLockStatus

                             */
    uint32_t  v_ErrorEstimateRMS; /*!< Uncertainty (RMS value) of the known frequency [ppb]
                              \verbatim
                                  u32max - Unknown Value
                              \endverbatim
                             */
    uint32_t   v_Frequency;     /*!< Frequency of pulse generated or supported [Hz]
                                  This field is mandatory if v_Status indicate #E_gnsFA_CLOCK_STAT_ALTERNATE_FREQ else it may just provide
                                  the frequency of the pulse.
                              \verbatim
                                  [range 100Hz .. 10 MHz]
                                  0 - default Pulse Frequency of Modem
                                  u32max - Unknown Value
                              \endverbatim
                             */

} s_gnsFA_ClockCalInd;




/*@}*/


/*! \addtogroup Location_gnsFA_Structures */
/*@{*/

/*! \union  u_gnsFA_MsgDataOut
 *  \brief  Union which defines the data associated with \ref e_gnsFA_MsgType
 */
typedef union
{
    s_gnsFA_ClockCalReq            v_GnsFAClockCalReq;        /*!< Corresponds to message type #E_gnsFA_CLOCK_CAL_REQ */
} u_gnsFA_MsgDataOut ;
/*@}*/


/*! \addtogroup Location_gnsFA_Functions */
/*@{*/

/* Callback which is executed to request for platform functionalities.  */
/*!
* \brief    Callback registered with \ref GNS_Initialize
* \details This callback is executed with messages and data which indicate
   what type of Platform calls need to be executed.
* \param Type of Request \ref  e_gnsFA_MsgType
* \param Length of data  uint32_t
* \param Data associated with Request \ref  u_gnsFA_MsgDataOut
*/
typedef void (*t_gnsFA_Callback)( e_gnsFA_MsgType , uint32_t , u_gnsFA_MsgDataOut* );


/*------------------------------------*/
/*!
* \brief  Initialises the GNS Frequency Aiding Module
* \details This API MUST be called before any of the Frequency Aiding functionality can be exercised

* \param v_FaCallback of type \ref t_gnsFA_Callback
* \return             A flag to indicate whether the Initialisation was successful
* \retval TRUE        Init Successful
* \retval FALSE     Init Failure
*/
bool GNS_FaInitialize( t_gnsFA_Callback v_FaCallback );




/*!
* \brief  Clock Calibartion Status Indication.
* \details This API is sent as an indication during Clock Calibration procedure. Clock Calibration procedure is initiated by GPS software refer #E_gnsFA_CLOCK_CAL_REQ and #s_gnsFA_ClockCalReq,
*          This API is used to indicate the status of clock calibration to modem when:
*      \verbatim
*          Pulse generation is started
*          Pulse generation is stopped
*          The error is higher than threshold, as a alarm to stop clock calibration.
*          Frequency is not locked to carrier
*          Frequency aiding is not supported
*          Pulse generation is stopped
*      \endverbatim
* \param pp_ClockCal     pointer of type \ref s_gnsFA_ClockCalInd
* \return None
* \retval None
*/
void GNS_FaClockCalInd( s_gnsFA_ClockCalInd *pp_ClockCal );

/*@}*/


#endif /*GNS_FA_H */
