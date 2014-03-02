/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2012. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */
/*****************************************************************************/

#ifndef _ALSACTRL_FM_INTERFACE_H_
#define _ALSACTRL_FM_INTERFACE_H_

// Audio HAL-interface (Audiocodec) (called from AudiocodecBaseNmfMpc through audioChipsetApiInterface)

int setup_fm_analog_out(void);
int setup_fm_rx_i2s(void);
void teardown_fm_i2s(void);
int setup_i2s_fm_tx(void);

#endif // _ALSACTRL_FM_INTERFACE_H_
