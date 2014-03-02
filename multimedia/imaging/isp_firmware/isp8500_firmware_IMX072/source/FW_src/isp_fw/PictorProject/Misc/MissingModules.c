/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/**
 \if INCLUDE_IN_HTML_ONLY
 \file  MissingModules.c
 \brief Contains the placeholder implementations for modules that do
        not exist at the moment.

 \ingroup Miscellaneous
 \endif
*/
#include "MissingModules.h"
#include "PictorhwReg.h"

void
GPIO_Init(void)
{
    uint32_t    u32_GPD;

    // enable the GPIO IP
    Set_GPIO_GPIO_ENABLE(gpio_enable_ENABLE, soft_reset_DISABLE);   // gpio_enable,soft_reset

    // set the output of all the GPIOs to low
    Set_GPIO_GPIO_GPO(0);

    // set the direction of the XSHUTDOWN GPIOs as output, rest as input
    u32_GPD = (uint32_t) ((uint32_t) gpd_DIR_OUTPUT << 5) | (uint32_t) ((uint32_t) gpd_DIR_OUTPUT << 6);    // GPIO pins 5 and 6 are being configured as output
    Set_GPIO_GPIO_GPD(u32_GPD);

    // set the GPIO 5 and 6 output to be driven by the GPO register
    Set_GPIO_GPIO_OUTPUT_CONFIG(
    gpo0_config_GPO_CHANNEL,
    gpo1_config_GPO_CHANNEL,
    gpo2_config_GPO_CHANNEL,
    gpo3_config_GPO_CHANNEL,
    gpo4_config_GPO_CHANNEL,
    gpo5_config_GPO_REGISTER,
    gpo6_config_GPO_REGISTER,
    gpo7_config_GPO_CHANNEL);   // gpo0_config,gpo1_config,gpo2_config,gpo3_config,gpo4_config,gpo5_config,gpo6_config,gpo7_config
    return;
}


void
GPIO_SetGPIO(
uint16_t    u16_EnableMask)
{
    Set_GPIO_GPIO_GPO(u16_EnableMask);

    return;
}

