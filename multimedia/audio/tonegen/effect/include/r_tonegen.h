/*****************************************************************************/
/**
*  (c) ST-Ericsson, 2010 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \brief
* \author ST-Ericsson
*/
/*****************************************************************************/
#ifndef INCLUSION_GUARD_R_TONEGEN_H
#define INCLUSION_GUARD_R_TONEGEN_H

#include "OMX_Core.h"
#include "t_tonegen.h"


/**
 * Create TG
 */
TG_Handle_t* TG_Create(void);


/**
 * Init TG
 */
TG_Result_t TG_Init(TG_Handle_t* TG_p,
                    const TG_PCM_Config_t* const PCM_Config_p,
                    const TG_JobDescriptor_t* const Job_p);


/**
 * Set tone TG
 */
TG_Result_t TG_SetTone(TG_Handle_t* TG_p,
                       const TG_PCM_Config_t* const PCM_Config_p,
                       const TG_JobDescriptor_t* const Job_p);


/**
 * Set volume TG
 */
void TG_SetVolume(TG_Handle_t* TG_Handle_p,
                  const TG_PCM_Config_t* const PCM_Config_p,
                  sint16 Volume,
                  uint32 Channel,
                  uint32 Time);


/**
 * Ramp Volume TG
 */
void TG_RampVolume(TG_Handle_t* TG_Handle_p,
                   const TG_PCM_Config_t* const PCM_Config_p,
                   sint16 StartVolume,
                   sint16 EndVolume,
                   uint32 Channel,
                   uint32 Time);


/**
 * Fill next Frame
 */
void TG_NextFrame(TG_Handle_t* TG_p,
                  TG_PCM_Config_t* PCM_Config_p,
                  OMX_BUFFERHEADERTYPE* BufHdr_p);


/**
 * Reset TG
 */
void TG_Reset(TG_Handle_t* TG_p, const TG_PCM_Config_t* const PCM_Config_p);


/**
 * Destroy TG
 */
void TG_Destroy(TG_Handle_t** TG_pp);


#endif //INCLUSION_GUARD_R_TONEGEN_H
