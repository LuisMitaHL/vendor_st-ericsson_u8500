/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/*
\file Damper.h
 */
#ifndef DAMPER_H_
#   define DAMPER_H_

#   include "Platform.h"

/*
/// This macro is used for compile time definition of the varying Damper structure.
/// Depending upon, the Dimension(1-D or 2-D) & the different damper points for Basis0 & Basis1,
/// a Damper structure gets defined & declared.
/// Avoids lot of manual process.
#define Damper(u8_DimensionCount, u8_BasisPoints0, u8_BasisPoints1, damper_name)	\
	typedef struct                                                                  \
	{                                       										\
		float_t f_DamperPoints_Y[u8_BasisCount0 * u8_BasisCount1];					\
		float_t f_DamperPoints_X0[u8_BasisCount0];									\
		float_t f_DamperPoints_X1[(u8_DimensionCount - 1) * u8_BasisCount1];		\
		uint16_t u16_DamperBasis[u8_DimensionCount];								\
		uint8_t  u8_BasisPoints[u8_DimensionCount];									\
	}damper_name##_Damper_ts;														\
name##_Damper_ts name##_Damper;// = {(u8_BasisCount0 << 5 | u8_BasisCount0 << 2 | u8_DimensionCount)};\
name##_Damper.u8_BasisPoints = u8_BasisPoints0; \
name##_Damper.u8_BasisPoints = u8_BasisPoints1;
*/

// this macro defines the structure with the given parameters
// u8_DimensionCount -> 1 for 1D damper, 2 for 2D damper
// u8_BasisCount0    -> number of X points on base0
// u8_BasisCount1    -> number of X points on base1
// u8_Parameters     -> number of parameters that the shared damper will cater to. for eg: 12 for RSO
// damper_name       -> name of IP, eg RSO


/*
typedef struct
{
	float_t f_DamperPoints_Y[8];
	float_t f_DamperPoints_X0[4];
	float_t f_DamperPoints_X1[2];
	uint16_t u16_DamperBasis[2];
	uint8_t  u8_BasisPoints[2];
}Damper_ts;
*/

/*
typedef struct
{
float_t f_DamperPoints_Y[u8_Parameters][u8_BasisCount0][u8_BasisCount1];
float_t f_DamperPoints_X0[u8_BasisCount0];
float_t f_DamperPoints_X1[(u8_DimensionCount - 1) * u8_BasisCount1];
uint16_t u16_DamperBasis[u8_DimensionCount];
uint8_t u8_BasisPoints[u8_DimensionCount];
}RSO_Shared_Damper_ts;
*/
typedef enum
{
    DamperBasis_e_ANALOG_GAIN,
    DamperBasis_e_INTEGRATION_TIME
} DamperBasis_te;

/*
typedef struct
{
	float_t * ptrf_DamperPoints_Y;
	float_t * ptrf_DamperPoints_X0;
	float_t * ptrf_DamperPoints_X1;
	uint16_t * ptru16_DamperBasis;
	uint8_t  * ptru8_BasisPoints;
	uint8_t  * ptru8_DimensionCount;
}Damper_Input_ts;

extern Damper_ts g_Damper;
*/
float_t Damper_Make (
        float_t *ptrf_DamperPoints_Y,
        float_t *ptrf_DamperPoints_X0,
        float_t *ptrf_DamperPoints_X1,
        float_t *ptrf_DamperBase,
        uint8_t *ptru8_BasisPoints,
        uint8_t u8_DimensionCount)TO_EXT_DDR_PRGM_MEM;
float_t get_fx_value (uint8_t u8_Basis)TO_EXT_DDR_PRGM_MEM;
uint16_t get_index (float_t fx_value_basis_0, float_t *ptrf_DamperPoints_X, uint8_t u8_BasisPoints)TO_EXT_DDR_PRGM_MEM;
float_t get_weight (float_t fx_value, uint8_t index_0, uint8_t index_1, float_t *ptrf_DamperPoints_X)TO_EXT_DDR_PRGM_MEM;
float_t get_output (
        uint8_t u8_DimensionCount,
        uint8_t u8_Parameter,
        float_t weight_1,
        float_t weight_2,
        float_t *f_DamperPoints_Y,
        uint8_t index_0_basis_0,
        uint8_t index_1_basis_0,
        uint8_t index_0_basis_1,
        uint8_t index_1_basis_1,
        uint8_t u8_BasisPoints0,
        uint8_t u8_BasisPoints1)TO_EXT_DDR_PRGM_MEM;
uint8_t get_index_in_op_matrix (uint8_t u8_index_i, uint8_t u8_index_j, uint8_t u8_len_i, uint8_t u8_len_j)TO_EXT_DDR_PRGM_MEM;
#endif /* DAMPER_H_ */

