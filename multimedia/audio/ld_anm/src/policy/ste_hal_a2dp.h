
/*******************************************************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2012. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 * \brief   Header of Interface used by ANM policy manager part to address A2DP external HAL
 */
/********************************************************************************************************************/

#ifndef ANDROID_AUDIO_POLICY_MANAGER_ANM_A2DP_HAL_H
#define ANDROID_AUDIO_POLICY_MANAGER_ANM_A2DP_HAL_H



struct a2dp_internal_data_s{
    ste_audio_output_descriptor_t  output_desc;
    audio_io_handle_t output_handle;       // A2DP output handler
    char device_address[MAX_DEVICE_ADDRESS_LEN]; // A2DP device MAC address
    bool suspended;
    unsigned int state;
};

int a2dp_get_capabilities(hw_ext_module_t * hw_module);



#endif
