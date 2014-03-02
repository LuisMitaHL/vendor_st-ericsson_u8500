/*****************************************************************************/
/**
*  ST-Ericsson, 2011 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \brief   This module provides services for power services module
* \author  ST-Ericsson
*/
/*****************************************************************************/

#ifndef TATL01HDMIFUNC_H_
#define TATL01HDMIFUNC_H_

#define TATHDMI_FILE_HDCP_KEY    "/hdcp_key.bin"
#define TATHDMI_HDMI_DEVICE_NAME "/dev/hdmi"
#define AES_ENCRYPTED_KEY_CMD_IDENTIFIER 10

#define BUFFERMAX 128

int tatlx1_00ActExec(struct dth_element *elem);
int tatlx1_01ActSet(struct dth_element *elem, void *Value);
int tatlx1_02ActGet(struct dth_element *elem, void *Value);
int tatlx1_03hdmiReg(unsigned int mode, struct hdmi_register *pp_hdmi_reg);

#endif /* TATL01HDMIFUNC_H_ */
