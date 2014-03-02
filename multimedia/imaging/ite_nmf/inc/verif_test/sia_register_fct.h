/*****************************************************************************
 * C STMicroelectronics 
 *
 * Reproduction and Communication of this document is strictly prohibited 
 * unless specifically authorized in writing by STMicroelectronics.  
 * ---------------------------------------------------------------------------- 
 * TPA / MPU / R&D 
 * MultiMedia Technology Center
 *****************************************************************************/
/** 
 * \file hv_top_api.h
 * \brief API for common functions
 * \author Chrsitophe CHEVALLAZ 5771
 * \date 2004-07-22
 *****************************************************************************/


#ifndef _SIA_FCT_H_
#define _SIA_FCT_H_

//For Linux
#include <inc/type.h>


#define SIA_MEMORY_SPACE_ADDR_L       0xA0200000  


#define hw_read8(  _addr_)        (*((volatile unsigned char  *)(_addr_)))
#define hw_read16( _addr_)        (*((volatile unsigned short *)(_addr_)))
#define hw_read32( _addr_)        (*((volatile unsigned int   *)(_addr_)))
#define hw_write8( _addr_,_data_) (*((volatile unsigned char  *)(_addr_))) = _data_
#define hw_write16(_addr_,_data_) (*((volatile unsigned short *)(_addr_))) = _data_
#define hw_write32(_addr_,_data_) (*((volatile unsigned int   *)(_addr_))) = _data_

#define hv_SetReg_short(reg_offset, val)         hv_SetReg_short_with_string(#reg_offset, reg_offset, val)
#define hv_SetReg_short_with_string(_string_, _offset_, _val_) hi_SetReg_with_string(_string_, _offset_, 16, (t_uint32)_val_)
#define hv_SetReg_with_string(      _string_, _offset_, _val_) hi_SetReg_with_string(_string_, _offset_, 32, (t_uint32)_val_)

#define hv_GetReg_short(reg_offset)	         hv_GetReg_short_with_string(#reg_offset, reg_offset)
#define hv_GetReg_short_with_string(_string_, _offset_) (t_uint16)hi_GetReg_with_string(_string_, _offset_, 16)
#define hv_GetReg_with_string(      _string_, _offset_) (t_uint32)hi_GetReg_with_string(_string_, _offset_, 32)
#define hv_SetReg(      reg_offset, val)         hv_SetReg_with_string(      #reg_offset, reg_offset, val)
#define hv_GetReg(      reg_offset)         hv_GetReg_with_string(      #reg_offset, reg_offset)

int hi_SetReg_with_string(char *reg_string, int reg_offset, int reg_width, t_uint32 val);
t_uint32 hi_GetReg_with_string(char *reg_string, int reg_offset, int reg_width)	;

#endif /* _SIA_FCT_H_ */

