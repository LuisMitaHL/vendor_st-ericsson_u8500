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

#include <hats_hdmi.h>

#include <sys/ioctl.h>
#include <fcntl.h>
#include <errno.h>
#include <pthread.h>
#include <poll.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int hdcp_hats_init(void)
{
    return 0;
}

int hdcp_hats_close(void)
{
    return 0;
}

int hdcp_hats_check_aes_key_otp(void)
{
    return 0;
}

int hdcp_hats_fuse_aes_otp_key(struct aes_key *key, uint8_t otp_crc)
{
    return 0;
}

int hdcp_hats_load_and_verify_aes_keys(struct hdcp_aes_encrypted_keyset *keyset)
{
    return 0;
}


