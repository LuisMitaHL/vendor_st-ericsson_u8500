
/*******************************************************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2012. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 * \brief   Header of Interface used by ANM policy manager part to address USB external HAL.
 */
/********************************************************************************************************************/

#ifndef ANDROID_AUDIO_POLICY_MANAGER_ANM_USB_HAL_H
#define ANDROID_AUDIO_POLICY_MANAGER_ANM_USB_HAL_H



struct usb_internal_data_s{
    ste_audio_output_descriptor_t  output_desc;
    char usb_card_and_device[MAX_DEVICE_ADDRESS_LEN];
    audio_io_handle_t output_handle;       // output handler
    unsigned int state;
};

int usb_get_capabilities(hw_ext_module_t * hw_module);



#endif
