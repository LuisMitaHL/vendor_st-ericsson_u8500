/*
 * Copyright (C) ST-Ericsson SA 2011
 * Author: Per Persson per.xb.persson@stericsson.com for
 * ST-Ericsson.
 *
 * License terms:
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#ifdef __cplusplus
extern "C" {
#endif

#ifndef _HDMI_SERVICE_API_H
#define _HDMI_SERVICE_API_H

/*
 * Set to 1 to stay alive when system suspends.
 * Set to 0 to sleep when system suspends.
 */
#define HDMI_SERVICE_STAY_ALIVE_DURING_SUSPEND 0

/* If defined, socket usage is hidden for messages from service,
 * and a callback function is used instead
 */
/*#define HDMI_SERVICE_USE_CALLBACK_FN*/

/* Service initialisation, threads creation
 * Input avoid_return_msg: set to 1 to avoid messages from service.
 * Return value: socket number where events will be notified.
 */
int hdmi_init(int avoid_return_msg);

/* Service exit, threads destruction */
int hdmi_exit(void);

/* Enable HDMI HW */
int hdmi_enable(void);

/* Disable HDMI HW */
int hdmi_disable(void);

#ifdef HDMI_SERVICE_USE_CALLBACK_FN
/* Set callback for reception of messages from service */
void hdmi_callback_set(void (*hdmi_cb)(int cmd, int data_size, __u8 *data));
#endif /*HDMI_SERVICE_USE_CALLBACK_FN*/

/* Change resolution. cea=0: VESA resolution, cea=1: CEA resolution */
int hdmi_resolution_set(int cea, int vesaceanr);

/* Release frame buffer */
int hdmi_fb_release(void);

/* Send CEC message */
int hdmi_cec_send(__u8 initiator, __u8 destination, __u8 data_size, __u8 *data);

/* Manually request EDID */
int hdmi_edid_request(__u8 block);

/* Initialise HDCP. AES data is required */
int hdmi_hdcp_init(__u16 aes_size, __u8 *aes_data);

/*
 * Send 3D Infoframe
 * format = 0x00: 3D off
 * format = 0x01: Side-by-Side(Half)
 * format = 0x02: Top-and-Bottom
 */
int hdmi_3d_infoframe_send(__u8 format);

/* Send Infoframe */
int hdmi_infoframe_send(__u8 type, __u8 version, __u8 crc, __u8 data_size,
								__u8 *data);

/* Set preferred resolution priorities */
int hdmi_vesa_cea_prio_set(__u8 vesa_cea1, __u8 nr1,
			__u8 vesa_cea2, __u8 nr2,
			__u8 vesa_cea3, __u8 nr3);


/* Messages from service */

/*
 * cmd=HDMI_PLUGGED_EV
 * u8 audio_support
 * u8 nr of supported resolutions
 * Two bytes for each resolution:
 * u8 vesa (0) or cea (1) for resolution x
 * u8 vesanr or ceanr for resolution x
 */

/*
 * cmd=HDMI_EDIDRESP data format
 * u8 result (0 = ok, 1 = not ok)
 * u8 edid_data[128] (if result == ok)
 */

/*
 * cmd=HDMI_CECRECVD
 * u8 source
 * u8 destination
 * u8 data length
 * u8 data[data_length]
 */

/*
 * cmd=HDMI_3D_SUPPORT
 * u8 nr of supported 3D resolutions
 * Two bytes for each resolution:
 * u8 ceanr for resolution x
 * u8 3D format for resolution x
 *
 * 3D format specification:
 * 0x01: Side-by-Side(Half)
 * 0x02: Top-and-Bottom
 */

/*
 * cmd=HDMI_HDCPSTATE data format
 * u8 state
 *	state = 0: No Receiver state
 *	state = 1: Receiver connected state
 *	state = 2: No HDCP receiver state
 *	state = 3: No Encryption state
 *	state = 4: Authentication on going state
 *	state = 5: Authentication fail state
 *	state = 6: Authentication succeed state
 *	state = 7: Encryption on going state
 */

/* HDMI message cmd sent from hdmi_service */
#define HDMI_PLUGGED_EV			0x10
#define HDMI_UNPLUGGED_EV		0x11
#define HDMI_EDIDRESP			0x12
#define HDMI_CECRECVD			0x13
#define HDMI_3D_SUPPORT			0x14
#define HDMI_ILLSTATE_POWERED		0x80
#define HDMI_ILLSTATE_UNPOWERED		0x81
#define HDMI_ILLSTATE_UNPLUGGED		0x82
#define HDMI_ILLSTATE_PWRON_PLUGGED	0x83
#define HDMI_CECSENDERR			0x84
#define HDMI_HDCPSTATE			0x85

#endif /* #ifdef _HDMI_SERVICE_API_H */

#ifdef __cplusplus
}
#endif
