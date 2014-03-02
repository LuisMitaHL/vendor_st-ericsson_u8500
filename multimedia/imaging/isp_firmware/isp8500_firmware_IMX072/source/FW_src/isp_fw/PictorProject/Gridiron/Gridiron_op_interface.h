/**
 * Gridiron_op_interface.h
 * Ver: 2.0 on 11.08.2010
 * Ver: 3.0 on 12.08.2010
 */
#ifndef GRIDIRON_OP_INTERFACE_H_
#define GRIDIRON_OP_INTERFACE_H_

#include "Platform.h"
#include "PictorhwReg.h"
#include "SystemConfig.h"

/**
 \struct GridironControl_ts
 \brief  Page to accept inputs that are directly applied/used for the Gridiron block.
 \ingroup Gridiron
*/
typedef struct
{
    /// Live Cast, The parameter can be changed any time. It will be effective only when systemconfig coin is toggled.
    /// [NOTE]: Based on the Live cast, FW will enable respective cast in H/W and calculate cast
    /// Live cast can only be a postive no. All cast position below should be in increasing order.
    float_t f_LiveCast;

    /// Cast0
    /// [NOTE]: Must be programmed before RUN
    float_t f_CastPosition0;

    /// Cast1
    /// [NOTE]: Must be programmed before RUN
    float_t f_CastPosition1;

    /// Cast2
    /// [NOTE]: Must be programmed before RUN
    float_t f_CastPosition2;

    /// Cast3
    /// [NOTE]: Must be programmed before RUN
    float_t f_CastPosition3;

   /// (grid samples per row, i.e. 66)
   uint16_t u16_GridWidth;

   /// (grid samples per column, i.e. 50)
   uint16_t u16_GridHeight;

   /// Enable or disable Gridiron
   /// TRUE : Enable
   /// False: Disable
   /// [NOTE]: Must be programmed before RUN
   uint8_t e_Flag_Enable;

   ///  Enable/Disable VerticalFlip
   /// Must be programmed by host in BML. In BMS, it will automatically computed by ISP FW
   uint8_t e_Flag_VerFlip;

   /// Enable/Disable HorizontalFlip
   /// Must be programmed by host in BML. In BMS, it will automatically computed by ISP FW
   uint8_t e_Flag_HorFlip;

   /// Number of active cast positions
   uint8_t u8_Active_ReferenceCast_Count;  

   /// Characterization time bayer order
   /// PixelOrder_e_GrR_BGb 
   /// PixelOrder_e_RGr_GbB
   /// PixelOrder_e_BGb_GrR
   /// PixelOrder_e_GbB_RGr
   /// Default : PixelOrder_e_GrR_BGb   
   uint8_t e_PixelOrder;     
} GridironControl_ts;


/**
 \struct GridironStatus_ts
 \brief  Page to accept inputs that are directly applied/used for the Gridiron block.
 \ingroup Gridiron
*/
typedef struct
{
    /// Live cast 
   float_t f_LiveCast;

   /// h-scale factor to be used to 'fool' gridiron hw block
   float_t f_Sensor_HScale;

   /// v-scale factor set in the gridiron hw block
   float_t f_Sensor_VScale;    

   /// gridiron HW readable form of f_Sensor_HScale
   uint16_t u16_Sensor_HScale_x256;

   /// gridiron HW readable form of f_Sensor_VScale
   uint16_t u16_Sensor_VScale_x256;    

   /// horizontal offset for sensor crop starting point
   uint16_t u16_Crop_HStart;

   /// vertical offset for sensor crop starting point
   uint16_t u16_Crop_VStart;

   /// Image width
   uint16_t u16_Image_HSize;

   /// Image height
   uint16_t u16_Image_VSize;    

   /// maximum value of h_size supported by sensor
   uint16_t u16_Sensor_HSize;

   /// Phase calculated and programmed by ISP FW
   uint8_t u16_Phase;      

   /// Log pitch selected by ISP FW
   uint8_t u8_LogGridPitch;

   /// Enable/disable status 0f cast_0
   /// Flag_e_TRUE : cast_0 enabled
   /// Flag_e_TRUE : cast_0 disabled   
   uint8_t e_Flag_Cast0;

   /// Enable/disable status 0f cast_1
   /// Flag_e_TRUE : cast_1 enabled
   /// Flag_e_TRUE : cast_1 disabled   
   uint8_t e_Flag_Cast1;

   /// Enable/disable status 0f cast_2
   /// Flag_e_TRUE : cast_2 enabled
   /// Flag_e_TRUE : cast_2 disabled   
   uint8_t e_Flag_Cast2;

   /// Enable/disable status 0f cast_3
   /// Flag_e_TRUE : cast_3 enabled
   /// Flag_e_TRUE : cast_3 disabled   
   uint8_t e_Flag_Cast3;

   /// Flag to enable/disable Gridiron
   /// TRUE : Enable
   /// False: Disable
   /// [NOTE]: Must be programmed before RUN
   uint8_t e_Flag_Enable;

   ///  Enable/Disable VerticalFlip
   /// Must be programmed by host in BML. In BMS, it will automatically computed by ISP FW
   uint8_t e_Flag_VerFlip;

   /// Enable/Disable HorizontalFlip
   /// Must be programmed by host in BML. In BMS, it will automatically computed by ISP FW
   uint8_t e_Flag_HorFlip;

   /// Number of active cast positions
   uint8_t u8_Active_ReferenceCast_Count;   
} GridironStatus_ts;

/************************ Exported Page elements *********************/
/// Control Page element for Gridiron
extern GridironControl_ts   g_GridironControl;

/// Status Page element for Gridiron
extern GridironStatus_ts   g_GridironStatus;

void Gridiron_UpdateGridStaticParameters(void) TO_EXT_DDR_PRGM_MEM;
void Gridiron_UpdateLiveParameters(void) TO_EXT_DDR_PRGM_MEM;
void Gridiron_CommitGridStaticParameters(void) TO_EXT_DDR_PRGM_MEM;
void Gridiron_CommitGridLiveParameters(void) TO_EXT_DDR_PRGM_MEM;
void Gridiron_ApplyGridStaticParameters(void) TO_EXT_DDR_PRGM_MEM;
void Gridiron_ApplyGridLiveParameters(void) TO_EXT_DDR_PRGM_MEM;
uint32_t CalculateLogGridPitch() TO_EXT_DDR_PRGM_MEM; 

#endif /* GRIDIRON_OP_INTERFACE_H_ */

