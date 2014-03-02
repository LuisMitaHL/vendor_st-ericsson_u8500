/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/**
\file RSO.c
\brief channel gain or digital calculations
\details Remove Slant offset Manager initialises and controls the RSO block
 - RSO Configuration will be different for 2 sensors.
 - At one time only one sensor is active so RSO_Configure will have to be called for the
   that sensor.Also page elements values will have to be set accordingly.
\ingroup WBStats
*/
#include "RSO_OPInterface.h"
#include "RSO_ip_interface.h"



RSO_Control_ts          g_RSO_Control = { RSO_DEFAULT_ENABLE, RSO_DEFAULT_MODE, };

RSO_Data_ts             g_RSO_DataCtrl =
{
    RSO_DEFAULT_GR_X_COEFF,
    RSO_DEFAULT_GR_Y_COEFF,
    RSO_DEFAULT_R_X_COEFF,
    RSO_DEFAULT_R_Y_COEFF,
    RSO_DEFAULT_B_X_COEFF,
    RSO_DEFAULT_B_Y_COEFF,
    RSO_DEFAULT_GB_X_COEFF,
    RSO_DEFAULT_GB_Y_COEFF,
    RSO_DEFAULT_GR_DC_TERM,
    RSO_DEFAULT_R_DC_TERM,
    RSO_DEFAULT_B_DC_TERM,
    RSO_DEFAULT_GB_DC_TERM,
    RSO_DEFAULT_X_SLANT_ORIGIN,
    RSO_DEFAULT_Y_SLANT_ORIGIN
};

RSO_Data_ts             g_RSO_DataStatus =
{
    RSO_DEFAULT_GR_X_COEFF,
    RSO_DEFAULT_GR_Y_COEFF,
    RSO_DEFAULT_R_X_COEFF,
    RSO_DEFAULT_R_Y_COEFF,
    RSO_DEFAULT_B_X_COEFF,
    RSO_DEFAULT_B_Y_COEFF,
    RSO_DEFAULT_GB_X_COEFF,
    RSO_DEFAULT_GB_Y_COEFF,
    RSO_DEFAULT_GR_DC_TERM,
    RSO_DEFAULT_R_DC_TERM,
    RSO_DEFAULT_B_DC_TERM,
    RSO_DEFAULT_GB_DC_TERM,
    RSO_DEFAULT_X_SLANT_ORIGIN,
    RSO_DEFAULT_Y_SLANT_ORIGIN
};



/**
 * \if      INCLUDE_IN_HTML_ONLY
 * \fn      void RSO_Update(void)
 * \brief   Configures RSO. Updates the status page after damping the control parameters.
 * \return  void
 * \callgraph
 * \callergraph
 * \ingroup
 * \endif
*/
void
RSO_Update(void)
{

        /// copy the ctrl page with status page

        /// gir channel
        g_RSO_DataStatus.u32_XCoefGr = g_RSO_DataCtrl.u32_XCoefGr;
        g_RSO_DataStatus.u32_YCoefGr = g_RSO_DataCtrl.u32_YCoefGr;
        g_RSO_DataStatus.u16_DcTermGr = g_RSO_DataCtrl.u16_DcTermGr;

        /// r channel
        g_RSO_DataStatus.u32_XCoefR = g_RSO_DataCtrl.u32_XCoefR;
        g_RSO_DataStatus.u32_YCoefR = g_RSO_DataCtrl.u32_YCoefR;
        g_RSO_DataStatus.u16_DcTermR = g_RSO_DataCtrl.u16_DcTermR;

        /// b channel
        g_RSO_DataStatus.u32_XCoefB = g_RSO_DataCtrl.u32_XCoefB;
        g_RSO_DataStatus.u32_YCoefB = g_RSO_DataCtrl.u32_YCoefB;
        g_RSO_DataStatus.u16_DcTermB = g_RSO_DataCtrl.u16_DcTermB;

        /// gib channel
        g_RSO_DataStatus.u32_XCoefGb = g_RSO_DataCtrl.u32_XCoefGb;
        g_RSO_DataStatus.u32_YCoefGb = g_RSO_DataCtrl.u32_YCoefGb;
        g_RSO_DataStatus.u16_DcTermGb = g_RSO_DataCtrl.u16_DcTermGb;

        g_RSO_DataStatus.u16_XSlantOrigin = g_RSO_DataCtrl.u16_XSlantOrigin;
        g_RSO_DataStatus.u16_YSlantOrigin = g_RSO_DataCtrl.u16_YSlantOrigin;

}


/**
 * \if      INCLUDE_IN_HTML_ONLY
 * \fn      void RSO_Commit(void)
 * \brief   Writes the firmware values(DC terms,x,y coefficients of Gr,Rr,Bb,Gb channels)
            to hardware registers
 * \return  void
 * \callgraph
 * \callergraph
 * \ingroup WBStats
 * \endif
*/
void
RSO_Commit(void)
{
    if (Flag_e_TRUE == g_RSO_Control.e_Flag_EnableRSO)
    {
        /// gir channel
        Set_RSO_DC_TERM_GR(g_RSO_DataStatus.u16_DcTermGr);
        Set_RSO_X_COEF_GR(g_RSO_DataStatus.u32_XCoefGr);
        Set_RSO_Y_COEF_GR(g_RSO_DataStatus.u32_YCoefGr);

        /// r channel
        Set_RSO_DC_TERM_RR(g_RSO_DataStatus.u16_DcTermR);
        Set_RSO_X_COEF_RR(g_RSO_DataStatus.u32_XCoefR);
        Set_RSO_Y_COEF_RR(g_RSO_DataStatus.u32_YCoefR);

        /// b channel
        Set_RSO_DC_TERM_BB(g_RSO_DataStatus.u16_DcTermB);
        Set_RSO_X_COEF_BB(g_RSO_DataStatus.u32_XCoefB);
        Set_RSO_Y_COEF_BB(g_RSO_DataStatus.u32_YCoefB);

        /// gib channel
        Set_RSO_DC_TERM_GB(g_RSO_DataStatus.u16_DcTermGb);
        Set_RSO_X_COEF_GB(g_RSO_DataStatus.u32_XCoefGb);
        Set_RSO_Y_COEF_GB(g_RSO_DataStatus.u32_YCoefGb);

        /// slant origin
        Set_RSO_X_SLANT_ORIGIN(g_RSO_DataStatus.u16_XSlantOrigin);
        Set_RSO_Y_SLANT_ORIGIN(g_RSO_DataStatus.u16_YSlantOrigin);

        Set_RSO_ENABLE();
    }
    else
    {
        Set_RSO_DISABLE();
    }

    return;
}


// this function will damp the various ctrl params that are to be written on to the hw block.
// for every parameters that is to be damped, Damper_make function is called.
// the output of the damper is then multiplied on to the ctrl parameters
// and applied on to the hw block.


