/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/*
\file Damper.c
 */
#include "Damper.h"
#include "Damper_ip_interface.h"

/*
#define Damper(u8_DimensionCount, u8_BasisPoints0, u8_BasisPoints1, damper_name)	\
	typedef struct                                                                  \
	{                                       										\
		float_t f_DamperPoints_Y[u8_BasisCount0 * u8_BasisCount1];					\
		float_t f_DamperPoints_X0[u8_BasisCount0];									\
		float_t f_DamperPoints_X1[(u8_DimensionCount - 1) * u8_BasisCount1];		\
		uint16_t u16_DamperBasis[u8_DimensionCount];								\
		uint8_t  u8_BasisPoints[u8_DimensionCount];									\
	}damper_name##_Damper_ts;														\
name##_Damper_ts name##_Damper;\
\
Basis0 = name##_Damper.u16_DamperBasis[0]\
Basis1 = name##_Damper.u16_DamperBasis[1]

fx_value_basis_0 =
	switch(Basis0)
	{
	case DamperBasis_e_ANALOG_GAIN:
	case DamperBasis_e_INTEGRATION_TIME:
	};
fx_value_basis_1 =
	switch(Basis1)
	{
	case DamperBasis_e_ANALOG_GAIN:
	case DamperBasis_e_INTEGRATION_TIME:
	};
index_basis_0 = fn(fx_value_basis_0, f_DamperPoints_X0[u8_BasisCount0])
index_basis_1 = fn(fx_value_basis_1, f_DamperPoints_X0[u8_BasisCount1])

basis_0_X, basis_0_Y = fn(index_basis_0)
basis_1_X, basis_1_Y = fn(index_basis_1)

weight_basis_0 = fn(fx_value_basis_0, basis_0_X1, basis_0_Y1, basis_0_X2, basis_0_Y2)
weight_basis_1 = fn(fx_value_basis_1, basis_1_X1, basis_1_Y1, basis_1_X2, basis_1_Y2)

Output = fn(weight_basis_0, weight_basis_1, basis_0_Y1, basis_0_Y2, basis_1_Y1, basis_1_Y2)
*/

/// params are
/// output matrix
/// basis_0_x_points
/// basis_1_x_points
/// before calling this function, it is assumed that the Damper Input parameters have been updated
float_t
Damper_Make(
float_t *ptrf_DamperPoints_Y,
float_t *ptrf_DamperPoints_X0,
float_t *ptrf_DamperPoints_X1,
float_t *ptrf_DamperBase,
uint8_t *ptru8_BasisPoints,
uint8_t u8_DimensionCount)
{
    uint8_t     u8_Basis0,
                u8_BasisPoints0,
                index_0_basis_0,
                index_1_basis_0;
    uint8_t     u8_Basis1,
                u8_BasisPoints1,
                index_0_basis_1,
                index_1_basis_1;
    uint16_t    clubbed_index_basis_0,
                clubbed_index_basis_1;
    float_t     fx_value_basis_0,
                fx_value_basis_1,
                weight_basis_0,
                weight_basis_1,
                damper_output;

    u8_Basis0 = u8_BasisPoints0 = index_0_basis_0 = index_1_basis_0 = 0;
    u8_Basis1 = u8_BasisPoints1 = index_0_basis_1 = index_1_basis_1 = 0;
    clubbed_index_basis_0 = clubbed_index_basis_1 = 0;
    fx_value_basis_0 = fx_value_basis_1 = weight_basis_0 = weight_basis_1 = damper_output = 0;

    u8_Basis0 = *ptrf_DamperBase;
    u8_BasisPoints0 = *ptru8_BasisPoints;
    fx_value_basis_0 = get_fx_value(u8_Basis0);
    clubbed_index_basis_0 = get_index(fx_value_basis_0, ptrf_DamperPoints_X0, u8_BasisPoints0);
    index_0_basis_0 = clubbed_index_basis_0 & 0xff;
    index_1_basis_0 = (clubbed_index_basis_0 >> 8) & 0xff;
    weight_basis_0 = get_weight(fx_value_basis_0, index_0_basis_0, index_1_basis_0, ptrf_DamperPoints_X0);

    if (u8_DimensionCount == 2)
    {
        u8_Basis1 = *(++ptrf_DamperBase);
        u8_BasisPoints1 = *(++ptru8_BasisPoints);
        fx_value_basis_1 = get_fx_value(u8_Basis1);
        clubbed_index_basis_1 = get_index(fx_value_basis_1, ptrf_DamperPoints_X1, u8_BasisPoints1);
        index_0_basis_1 = clubbed_index_basis_1 & 0xff;
        index_1_basis_1 = (clubbed_index_basis_1 >> 8) & 0xff;
        weight_basis_1 = get_weight(fx_value_basis_1, index_0_basis_1, index_1_basis_1, ptrf_DamperPoints_X1);
    }


    damper_output = get_output(
        u8_DimensionCount,
        0,
        weight_basis_0,
        weight_basis_1,
        ptrf_DamperPoints_Y,
        index_0_basis_0,
        index_1_basis_0,
        index_0_basis_1,
        index_1_basis_1,
        u8_BasisPoints0,
        u8_BasisPoints1);
    return (damper_output);
}


float_t
get_fx_value(
uint8_t u8_Basis)
{
    float_t f_fx_value;
    switch (u8_Basis)
    {
        case DamperBasis_e_ANALOG_GAIN:         f_fx_value = Damper_GetAnalogGain(); break;
        case DamperBasis_e_INTEGRATION_TIME:    f_fx_value = Damper_GetIntegrationTime(); break;
        default:                                f_fx_value = Damper_GetDefault();
    };
    return (f_fx_value);
}


// assuming that the X points are in increasing order
uint16_t
get_index(
float_t fx_value,
float_t *ptrf_DamperPoints_X,
uint8_t u8_BasisPoints)
{
    uint8_t u8_index_0,
            u8_index_1,
            u8_index;
    float_t f_basis_point;

    if (u8_BasisPoints == 1)
    {
        // no damping on this basis
        u8_index_0 = 0;
        u8_index_1 = 0;
    }
    else
    {
        // scan through the entire array to find the indices
        for (u8_index = 0; u8_index < u8_BasisPoints; u8_index++)
        {
            f_basis_point = *(ptrf_DamperPoints_X + u8_index);

            // assuming that the X points are in increasing order
            if (fx_value <= f_basis_point)
            {
                //u8_index_0 = u8_index;
                u8_index_1 = u8_index;
                break;
            }
        }


        if (fx_value < *ptrf_DamperPoints_X)
        {
            u8_index_0 = u8_index_1 = 0;
        }
        else if (fx_value > *(ptrf_DamperPoints_X + u8_BasisPoints - 1))
        {
            u8_index_0 = u8_index_1 = u8_BasisPoints - 1;
        }
        else
        {
            // fx_value lies between the f_DamperPoints_X[0] and f_DamperPoints_X[u8_BasisPoints - 1]
            u8_index_0 = u8_index_1 - 1;
        }
    }


    return (u8_index_0 | (u8_index_1 << 8));
}


float_t
get_weight(
float_t fx_value,
uint8_t index_0,
uint8_t index_1,
float_t *ptrf_DamperPoints_X)
{
    float_t f_weight;
    float_t basis_0_X,
            basis_1_X;

    if (index_0 == index_1)
    {
        // either fx_value is left of Xmin or right of Xmax
        if (index_0 == 0)
        {
            // no weight in this basis
            f_weight = 0.0;
        }
        else
        {
            f_weight = 1.0;
        }
    }
    else
    {
        // basis_x_0, basis_x_1
        basis_0_X = *(ptrf_DamperPoints_X + index_0);
        basis_1_X = *(ptrf_DamperPoints_X + index_1);

        // <TODO> check if the parameters are integers
        f_weight = (fx_value - basis_0_X) / (basis_1_X - basis_0_X);
    }


    return (f_weight);
}


float_t
get_output(
uint8_t u8_DimensionCount,
uint8_t u8_Parameter,
float_t weight_1,
float_t weight_2,
float_t *ptrf_DamperPoints_Y,
uint8_t index_0_basis_0,
uint8_t index_1_basis_0,
uint8_t index_0_basis_1,
uint8_t index_1_basis_1,
uint8_t u8_BasisPoints0,
uint8_t u8_BasisPoints1)
{
    float_t y_output_00,
            y_output_01,
            y_output_10,
            y_output_11;
    float_t f_damper_output;

    if (u8_DimensionCount == 2)
    {
        y_output_00 = ptrf_DamperPoints_Y[get_index_in_op_matrix(
            index_0_basis_0,
            index_0_basis_1,
            u8_BasisPoints0,
            u8_BasisPoints1)];
        y_output_01 = ptrf_DamperPoints_Y[get_index_in_op_matrix(
            index_0_basis_0,
            index_1_basis_1,
            u8_BasisPoints0,
            u8_BasisPoints1)];
        y_output_10 = ptrf_DamperPoints_Y[get_index_in_op_matrix(
            index_1_basis_0,
            index_0_basis_1,
            u8_BasisPoints0,
            u8_BasisPoints1)];
        y_output_11 = ptrf_DamperPoints_Y[get_index_in_op_matrix(
            index_1_basis_0,
            index_1_basis_1,
            u8_BasisPoints0,
            u8_BasisPoints1)];

        f_damper_output = (1 - weight_1) * (1 - weight_2) * y_output_00 + (weight_1) * (1 - weight_2) * y_output_10 + (1 - weight_1) * (weight_2) * y_output_01 + (weight_1) * (weight_2) * y_output_11;
    }
    else
    {
        y_output_00 = ptrf_DamperPoints_Y[index_0_basis_0];
        y_output_01 = ptrf_DamperPoints_Y[index_1_basis_0];

        f_damper_output = (1 - weight_1) * y_output_00 + (weight_1) * y_output_01;
    }


    return (f_damper_output);
}


uint8_t
get_index_in_op_matrix(
uint8_t u8_index_i,
uint8_t u8_index_j,
uint8_t u8_len_i,
uint8_t u8_len_j)
{
    // (j)*len(i) + i
    return (u8_index_j * u8_len_i + u8_index_i);
}

