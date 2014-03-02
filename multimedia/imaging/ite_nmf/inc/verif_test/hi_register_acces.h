#include "sia_register_fct.h"

/**************************/
/* Register Access Macros */
/**************************/
#define XBUS_TO_AXB( _base_, _addr_) (SIA_MMDSP_32 +2*(_base_+_addr_))
#define XBUS_TO_AXB_BYTE( _base_, _addr_) (SIA_MMDSP_32 +_base_+_addr_)

/* acces registers */
#define SIA_(_Reg_)       (SIA_##_Reg_##_OFFSET)
#define IPP_(_Reg_)       (IPP_IPP_##_Reg_ ## _OFFSET)
#define IPP_R_(_Reg_)     (IPP_##_Reg_ ## _OFFSET)
#define DMA_(_Reg_)       (DMA_DMA_##_Reg_ ## _OFFSET)
#define STBP_(_Reg_)      (STBP_STBP_##_Reg_ ## _OFFSET)
#define HOST_(_Reg_)      XBUS_TO_AXB(HOST_BASE      , (_Reg_))
#define ITC_(_Reg_)       (ITC_##_Reg_)
#define INTEG_(_Reg_)     (_Reg_)
#define PROJ_(_Reg_)      (PROJ_##_Reg_##_OFFSET)

#define ISP_(_Reg_)       XBUS_TO_AXB(ISP_BASE, (_Reg_ ## _OFFSET))



/* write registers */
#define SIA_R_W(_Reg_, _Value_)     hv_SetReg_short(SIA_(_Reg_)     , (_Value_))
#define IPP_R_W(_Reg_, _Value_)     hv_SetReg_short(IPP_(_Reg_)     , (_Value_))
#define IPP_W(  _Reg_, _Value_)     hv_SetReg_short(IPP_R_(  _Reg_) , (_Value_))
#define DMA_R_W(_Reg_, _Value_)     hv_SetReg_short(DMA_(_Reg_)     , (_Value_))
#define STBP_R_W(_Reg_, _Value_)    hv_SetReg_short(STBP_(_Reg_)    , (_Value_))
#define VCF_R_W(_Reg_, _Value_)     hv_SetReg_short(SIA_R_(_Reg_)   , (_Value_))
#define INTEG_R_W(_Reg_, _Value_)   hv_SetReg_short(INTEG_(_Reg_)   , (_Value_))
#define HOST_R_W(_Reg_, _Value_)    hv_SetReg_short(HOST_(_Reg_)    , (_Value_))
#define HOST_R_W_ST(_Reg_, _Value_) hv_SetReg_with_string(#_Reg_    , HOST_(_Reg_),  (_Value_))
#define PROJ_R_W(_Reg_, _Value_)    hv_SetReg_short(PROJ_(_Reg_)    , (_Value_))

#define ISP_R_W(_Reg_,_Value_) hv_SetReg(0x52000 + _Reg_ ## _OFFSET,(_Value_))

/* read registers */
#define SIA_R_R(_Reg_)             (hv_GetReg_short(SIA_(_Reg_)))
#define IPP_R_R(_Reg_)             (hv_GetReg_short(IPP_(_Reg_)))
#define IPP_R(  _Reg_)             (hv_GetReg_short(IPP_R_(  _Reg_)))
#define DMA_R_R(_Reg_)             (hv_GetReg_short(DMA_(_Reg_)))
#define STBP_R_R(_Reg_)            (hv_GetReg_short(STBP_(_Reg_)))
#define VCF_R_R(_Reg_)             (hv_GetReg_short(SIA_R_(_Reg_)))
#define INTEG_R_R(_Reg_)           (hv_GetReg_short(INTEG_(_Reg_)))
#define HOST_R_R(_Reg_)            (hv_GetReg_short(HOST_(_Reg_)))
#define HOST_R_R_ST(_Reg_)         (hv_GetReg_with_string(#_Reg_, HOST_(_Reg_)))
#define PROJ_R_R(_Reg_)            (hv_GetReg_short(PROJ_(_Reg_)))

#define ISP_R_R(_Reg_) (hv_GetReg( 0x52000 + _Reg_ ## _OFFSET))

#define CRM_R_R(_Reg_) hv_GetReg_with_string(#_Reg_, CRM_CRM_ ## _Reg_ ## _OFFSET)
#define CRM_R_W(_Reg_,_Value_) hv_SetReg_with_string(#_Reg_, CRM_CRM_ ## _Reg_ ## _OFFSET , _Value_)

