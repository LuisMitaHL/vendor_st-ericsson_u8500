/*****************************************************************************
 *  ST-Ericsson GPS Technology
 *  Copyright (C) 2009 ST-Ericsson.
 *  All rights reserved
 *
 *****************************************************************************/

#ifndef GNS_FTA_TYPEDEFS_H
#define GNS_FTA_TYPEDEFS_H

#include "gns.h"
#include "gnsCP_Typedefs.h"

/*! \addtogroup Location_gnsFTA_Constants */
/*@{*/

/*----------------------------------------------------------------------*/
/*!
 *  \enum   e_gnsFTA_MsgType
 *      List of constant used to define type of message.
 */
typedef enum
{
    E_gnsFTA_PULSE_REQ       /*!< Request to generate a pulse for Fine time \ref s_gnsFTA_PulseReq. */
} e_gnsFTA_MsgType;

/*@}*/


/*! \addtogroup Location_gnsFTA_Structures */
/*@{*/


/*! \struct  s_gnsFTA_PulseReq
 *  \brief  Structure defines data associated with \ref e_gnsFTA_MsgType
 */

typedef struct
{
  e_gns_RATType v_CellType;      /*!< Type of RAT for which pulse is expected to be generated.*/

  uint16_t      v_CellIdentifier; /*!< Provide the identification of the cell for which pulse needs to be generated (OPTIONAL).
                                              \verbatim
                                            - If v_CellType indicate WCDMA-FDD this field shall contain Primary Scrambling code.
                                            - If v_CellType indicate WCDMA-TDD this field shall contain Cell Parameter Id.
                                            - If v_CellType indicate GSM              this field shall contain Arfcn.
                                            - If v_CellType indicate No cell           this field shall be ignored and pulse need to be generated on current cell.
                                            - If v_CellType indicate a RAT type     this is optional as many time LBS stack don't have the exact information on cell information,
                                                                                                    eg when it has received reference time, it knows cell information,but for uplink
                                                                                                    FTA LBS stack may have gone through handovers and it may not be up to date for cell 
                                                                                                    information, in such case it will set to 0xffff.
                                             \endverbatim
                                            */
} s_gnsFTA_PulseReq;


/*! \union  s_gnsFTA_PulseCnf
 *  \brief  Union which defines the data associated with \ref GNS_FtaPulseCnf
 */

typedef struct
{
  e_gns_RATType v_CellTimeType;  /*!< Type of RAT for which cell time is present.*/

  u_gnsCP_CellTime  v_CellTime;  /*!< Cell Time information.*/
    
} s_gnsFTA_PulseCnf;



/*! \union  u_gnsFTA_MsgDataOut
 *  \brief  Union which defines the data associated with \ref e_gnsFTA_MsgType
 */
typedef union
{
    s_gnsFTA_PulseReq         v_gnsFtaPulseReq;
} u_gnsFTA_MsgDataOut ;


/*@}*/



#endif /* GNS_FTA_TYPEDEFS_H*/

