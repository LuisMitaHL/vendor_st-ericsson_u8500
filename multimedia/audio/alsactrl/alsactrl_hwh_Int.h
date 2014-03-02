/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2012. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */
/*****************************************************************************/

#ifndef ALSACTRL_HWH_INT_H
#define ALSACTRL_HWH_INT_H

alsactrl_dev_info_t* Alsactrl_Hwh_GetDevInfo(enum alsactrl_alsa_device alsa_dev, unsigned int stream_dir);
alsactrl_dev_info_t* Alsactrl_Hwh_GetDevInfo_Peer(alsactrl_dev_info_t* dev);

int Alsactrl_Hwh_OpenAlsaDev(alsactrl_dev_info_t* dev_info_p);
void Alsactrl_Hwh_CloseAlsaDev(alsactrl_dev_info_t* dev_info_p);

#endif /* ALSACTRL_HWH_INT_H */

