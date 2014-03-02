/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/*
 * Gamma_ip_interface.h
 *
 *  Created on: Feb 2, 2010
 *      Author: mamtac
 */

#ifndef GAMMA_IP_INTERFACE_H_
#define GAMMA_IP_INTERFACE_H_

/**
 \file  Gamma_IPInterface.h

 \brief This file is NOT a part of the gamma module release code.
        All inputs needed by the gamma module that can only
        be resolved at the project level (at integration time)
        are met through this file.
        It is the responsibility of the integrator to generate
        this file at integration time and meet all the input
        dependencies.
        For the current code release, there are no input dependencies
        to be met. However this file must be present (even if it is blank)
        in incldue path of the integrated project during compilation of code.

 \ingroup Gamma
*/

#include "Gamma_PlatformSpecific.h"

/// Pipe 0
/// Interface to fetch the Gamma Curve in CE0
#   define Gamma_GetGammaCurve_Ce0()                            g_CE_GammaControl[0].e_GammaCurve

/// Interface to Set Gamma Sharp Enable for Ce0
#   define Gamma_Set_GammaSharp_Enable_Ce0()                    Set_ISP_CE0_FLEXTF_SHARP_ISP_CE0_FLEXTF_SHARP_ENABLE_enable__ENABLE()

/// Interface to Set Gamma Sharp Disable for Ce0
#   define Gamma_Set_GammaSharp_Disable_Ce0()                   Set_ISP_CE0_FLEXTF_SHARP_ISP_CE0_FLEXTF_SHARP_ENABLE_enable__DISABLE()

/// Interface to Set Gamma Sharp LastGreenGIRValue for Ce0
#   define Gamma_Set_GammaSharp_LstGreenGIRValue_Ce0(Value)     Set_ISP_CE0_FLEXTF_SHARP_ISP_CE0_FLEXTF_SHARP_LAST_ELT_GREEN_GIR_last_elt_green_gir(Value)

/// Interface to Set Gamma Sharp LastRedValue for Ce0
#   define Gamma_Set_GammaSharp_LstRedValue_Ce0(Value)          Set_ISP_CE0_FLEXTF_SHARP_ISP_CE0_FLEXTF_SHARP_LAST_ELT_RED_last_elt_red(Value)

/// Interface to Set Gamma Sharp LastBlueValue for Ce0
#   define Gamma_Set_GammaSharp_LstBlueValue_Ce0(Value)         Set_ISP_CE0_FLEXTF_SHARP_ISP_CE0_FLEXTF_SHARP_LAST_ELT_BLUE_last_elt_blue(Value)

/// Interface to Set Gamma Sharp LastGreenGIBValue for Ce0
#   define Gamma_Set_GammaSharp_LstGreenGIBValue_Ce0(Value)     Set_ISP_CE0_FLEXTF_SHARP_ISP_CE0_FLEXTF_SHARP_LAST_ELT_GIB_last_elt_gib(Value)

/// Interface to Set Gamma Sharp PixelInShift for Ce0
#   define Gamma_Set_GammaSharp_PixelInShift_Ce0(Value)         Set_ISP_CE0_FLEXTF_SHARP_ISP_CE0_FLEXTF_SHARP_PIXELIN_SHIFT_pixel_shift(Value)

/// Interface to Set Gamma UnSharp Enable for Ce0
#   define Gamma_Set_GammaUnSharp_Enable_Ce0()                  Set_ISP_CE0_FLEXTF_UNSHARP_ISP_CE0_FLEXTF_UNSHARP_ENABLE_enable__ENABLE();

/// Interface to Set Gamma UnSharp Disable for Ce0
#   define Gamma_Set_GammaUnSharp_Disable_Ce0()                 Set_ISP_CE0_FLEXTF_UNSHARP_ISP_CE0_FLEXTF_UNSHARP_ENABLE_enable__DISABLE()

/// Interface to Set Gamma UnSharp LastGreenGIRValue for Ce0
#   define Gamma_Set_GammaUnSharp_LstGreenGIRValue_Ce0(Value)   Set_ISP_CE0_FLEXTF_UNSHARP_ISP_CE0_FLEXTF_UNSHARP_LAST_ELT_GREEN_GIR_last_elt_green_gir(Value)

/// Interface to Set Gamma UnSharp LastRedValue for Ce0
#   define Gamma_Set_GammaUnSharp_LstRedValue_Ce0(Value)        Set_ISP_CE0_FLEXTF_UNSHARP_ISP_CE0_FLEXTF_UNSHARP_LAST_ELT_RED_last_elt_red(Value)

/// Interface to Set Gamma UnSharp LastBlueValue for Ce0
#   define Gamma_Set_GammaUnSharp_LstBlueValue_Ce0(Value)       Set_ISP_CE0_FLEXTF_UNSHARP_ISP_CE0_FLEXTF_UNSHARP_LAST_ELT_BLUE_last_elt_blue(Value)

/// Interface to Set Gamma UnSharp LastGreenGIBValue for Ce0
#   define Gamma_Set_GammaUnSharp_LstGreenGIBValue_Ce0(Value)   Set_ISP_CE0_FLEXTF_UNSHARP_ISP_CE0_FLEXTF_UNSHARP_LAST_ELT_GIB_last_elt_gib(Value)

/// Interface to Set Gamma UnSharp PixelInShift for Ce0
#   define Gamma_Set_GammaUnSharp_PixelInShift_Ce0(Value)       Set_ISP_CE0_FLEXTF_UNSHARP_ISP_CE0_FLEXTF_UNSHARP_PIXELIN_SHIFT_pixel_shift(Value)

/// Pipe1
/// Interface to fetch the Gamma Curve in CE1
#   define Gamma_GetGammaCurve_Ce1()                            g_CE_GammaControl[1].e_GammaCurve

/// Interface to Set Gamma Sharp Enable for Ce1
#   define Gamma_Set_GammaSharp_Enable_Ce1()                    Set_ISP_CE1_FLEXTF_SHARP_ISP_CE1_FLEXTF_SHARP_ENABLE_enable__ENABLE()

/// Interface to Set Gamma Sharp Disable for Ce1
#   define Gamma_Set_GammaSharp_Disable_Ce1()                   Set_ISP_CE1_FLEXTF_SHARP_ISP_CE1_FLEXTF_SHARP_ENABLE_enable__DISABLE()

/// Interface to Set Gamma Sharp LastGreenGIRValue for Ce1
#   define Gamma_Set_GammaSharp_LstGreenGIRValue_Ce1(Value)     Set_ISP_CE1_FLEXTF_SHARP_ISP_CE1_FLEXTF_SHARP_LAST_ELT_GREEN_GIR_last_elt_green_gir(Value)

/// Interface to Set Gamma Sharp LastRedValue for Ce1
#   define Gamma_Set_GammaSharp_LstRedValue_Ce1(Value)          Set_ISP_CE1_FLEXTF_SHARP_ISP_CE1_FLEXTF_SHARP_LAST_ELT_RED_last_elt_red(Value)

/// Interface to Set Gamma Sharp LastBlueValue for Ce1
#   define Gamma_Set_GammaSharp_LstBlueValue_Ce1(Value)         Set_ISP_CE1_FLEXTF_SHARP_ISP_CE1_FLEXTF_SHARP_LAST_ELT_BLUE_last_elt_blue(Value)

/// Interface to Set Gamma Sharp LastGreenGIBValue for Ce1
#   define Gamma_Set_GammaSharp_LstGreenGIBValue_Ce1(Value)     Set_ISP_CE1_FLEXTF_SHARP_ISP_CE1_FLEXTF_SHARP_LAST_ELT_GIB_last_elt_gib(Value)

/// Interface to Set Gamma Sharp PixelInShift for Ce1
#   define Gamma_Set_GammaSharp_PixelInShift_Ce1(Value)         Set_ISP_CE1_FLEXTF_SHARP_ISP_CE1_FLEXTF_SHARP_PIXELIN_SHIFT_pixel_shift(Value)


/// Interface to Gamma UnSharp Enable for Ce1
#   define Gamma_Set_GammaUnSharp_Enable_Ce1()                  Set_ISP_CE1_FLEXTF_UNSHARP_ISP_CE1_FLEXTF_UNSHARP_ENABLE_enable__ENABLE();

/// Interface to Set Gamma UnSharp Disable for Ce1
#   define Gamma_Set_GammaUnSharp_Disable_Ce1()                 Set_ISP_CE1_FLEXTF_UNSHARP_ISP_CE1_FLEXTF_UNSHARP_ENABLE_enable__DISABLE()

/// Interface to Set Gamma UnSharp LastGreenGIRValue for Ce1
#   define Gamma_Set_GammaUnSharp_LstGreenGIRValue_Ce1(Value)   Set_ISP_CE1_FLEXTF_UNSHARP_ISP_CE1_FLEXTF_UNSHARP_LAST_ELT_GREEN_GIR_last_elt_green_gir(Value)

/// Interface to Set Gamma UnSharp LastRedValue for Ce1
#   define Gamma_Set_GammaUnSharp_LstRedValue_Ce1(Value)        Set_ISP_CE1_FLEXTF_UNSHARP_ISP_CE1_FLEXTF_UNSHARP_LAST_ELT_RED_last_elt_red(Value)

/// Interface to Set Gamma UnSharp LastBlueValue for Ce1
#   define Gamma_Set_GammaUnSharp_LstBlueValue_Ce1(Value)       Set_ISP_CE1_FLEXTF_UNSHARP_ISP_CE1_FLEXTF_UNSHARP_LAST_ELT_BLUE_last_elt_blue(Value)

/// Interface to Set Gamma UnSharp LastGreenGIBValue for Ce1
#   define Gamma_Set_GammaUnSharp_LstGreenGIBValue_Ce1(Value)   Set_ISP_CE1_FLEXTF_UNSHARP_ISP_CE1_FLEXTF_UNSHARP_LAST_ELT_GIB_last_elt_gib(Value)

/// Interface to Set Gamma UnSharp PixelInShift for Ce1
#   define Gamma_Set_GammaUnSharp_PixelInShift_Ce1(Value)       Set_ISP_CE1_FLEXTF_UNSHARP_ISP_CE1_FLEXTF_UNSHARP_PIXELIN_SHIFT_pixel_shift(Value)
#endif /* GAMMA_IP_INTERFACE_H_ */

