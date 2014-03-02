/***********************************************************************
 *  © ST-Ericsson, 2010 - All rights reserved
 *
 *  Reproduction and Communication of this document is strictly prohibited
 *  unless specifically authorized in writing by ST-Ericsson.
 ***********************************************************************
 * DESCRIPTION
 *
 * $Id : 
 * Filename:   
 * Author:     
 * Created:    
 *
 ***********************************************************************
 * REVISION HISTORY
 *
 * $Log: spl_flt_to_fix.h,v $
 * Revision 1.1  2009/10/02 12:08:27  erauae
 * Added SPL library for float to fix conversion.
 *
 * Revision 1.1  2009/10/02 11:21:31  erauae
 * Added new structure of spl.
 *
 * Revision 1.2  2009/09/25 15:33:48  erauae
 * Added possibility to compile with float as floating point precision. Bitexact for
 *  SPL_Float_t as double.
 *
 * Revision 1.1  2009/07/09 09:49:43  erauae
 * Removed mean from data for coherence estimate.
 *
 * Revision 1.1  2009/05/25 13:04:27  erauae
 * Moved SPL to separete module.
 *
 * Revision 1.1  2009/04/06 16:56:49  erauae
 * Added routine for round in conversion from SPL_Float_t to short.
 *
 * Revision 1.1  2009/03/23 16:44:05  erauae
 * Added FFT routines to float.
 *
 * Revision 1.1  2009/03/23 13:59:56  erauae
 * Added first structure of source code.
 *
 *
 ***********************************************************************/

#ifndef INCLUSION_GUARD_SPL_FLT_TO_FIX_H
#define INCLUSION_GUARD_SPL_FLT_TO_FIX_H

#include "spl_fix_function_usage.h"

/*
 * Saturate and round double to short
 */ 
/* Single precision varaibles (48 dB dynamic range) */ 
#define SPL_FIX_vint16QData_t sint16  
/* Double precision variables (96 dB dynamic range) */
#define SPL_FIX_vint32QData_t sint32  

/* Pseudo float scalar types */
typedef struct {
  short                  Q; 
  SPL_FIX_vint16QData_t  v;
} SPL_FIX_sint16Q_t;

typedef struct {
  short                  Q; 
  SPL_FIX_vint32QData_t  v;
} SPL_FIX_sint32Q_t;

/* Pseudo float vector types */
typedef struct {
  short                   Q; 
  short                   L;    
  SPL_FIX_vint16QData_t*  v_p;
} SPL_FIX_vint16Q_t;

typedef struct {
  short                 Q; 
  short                 L;    
  SPL_FIX_vint32QData_t *v_p;
} SPL_FIX_vint32Q_t;


 
void SPL_SatRoundFloatToSint16(const SPL_Float_t        tmpFloat,
                                     sint16*      const tmpSint16);

void SPL_SatRoundFloatToSint32(const SPL_Float_t        tmpFloat,
                                     sint32*      const tmpSint32);

void SPL_SaturateFloatToSint16(const SPL_Float_t        tmpFloat,
                                     sint16*      const tmpSint16);

void SPL_SaturateFloatToSint32(const SPL_Float_t        tmpFloat,
                                     sint32*      const tmpSint32);

void SPL_Normalize_vint16Q(const SPL_vint16Q_t*  const Flt_p,
                                 sint16*         const norm_p);

void SPL_Normalize_vint32Q(const SPL_vint32Q_t*  const Flt_p,
                                 sint16*         const norm_p);



/*
 * Write 16 bit fixed point vector to log file
 */ 
void DumpFixVint16Q(      SPL_FIX_vint16Q_t*  const Data_p,
                    const short                     logFile);


/*
 * Write 32 bit fixed point vector to log file
 */ 
void DumpFixVint32Q(      SPL_FIX_vint32Q_t*  const Data_p,
                    const short                     logFile);


/*
 * Convert 16 bit fixed point vector to floating point
 */
void SPL_FixToFlt_vint16Q(const SPL_FIX_vint16Q_t*  const Fix_p,
                                SPL_vint16Q_t*      const Flt_p);


/*
 * Convert 32 bit fixed point vector to floating point
 */
void SPL_FixToFlt_vint32Q(const SPL_FIX_vint32Q_t*  const Fix_p,
                                SPL_vint32Q_t*      const Flt_p);


    
/*
 * Convert 16 bit floating point vector to fixed point
 */
void SPL_FltToFix_vint16Q(const SPL_vint16Q_t*      const Flt_p,
                                SPL_FIX_vint16Q_t*  const Fix_p);


/*
 * Convert 32 bit floating point vector to fixed point
 */
void SPL_FltToFix_vint32Q(const SPL_vint32Q_t*      const Flt_p,
                                SPL_FIX_vint32Q_t*  const Fix_p);
                                
                                
/*
 * Convert 16 bit fixed point scalar to floating point scalar
 */
void SPL_FixToFlt_sint16Q(const SPL_FIX_sint16Q_t*  const Fix_p,
                                SPL_sint16Q_t*      const Flt_p);
                                


/*
 * Convert 32 bit fixed point scalar to floating point
 */
void SPL_FixToFlt_sint32Q(const SPL_FIX_sint32Q_t*  const Fix_p,
                                SPL_sint32Q_t*      const Flt_p);


/*
 * Convert 16 bit floating point scalar to fixed point
 */
void SPL_FltToFix_sint16Q(const SPL_sint16Q_t*      const Flt_p,
                                SPL_FIX_sint16Q_t*  const Fix_p);


/*
 * Convert 32 bit floating point scalar to fixed point
 */
void SPL_FltToFix_sint32Q(const SPL_sint32Q_t*      const Flt_p,
                                SPL_FIX_sint32Q_t*  const Fix_p);

#endif
