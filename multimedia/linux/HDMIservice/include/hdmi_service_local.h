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

#ifndef _HDMI_SERVICE_LOCAL_H
#define _HDMI_SERVICE_LOCAL_H

enum hdmi_fb_state {
	HDMI_FB_CLOSED,
	HDMI_FB_OPENED
};

enum hdmi_plug_state {
	HDMI_UNPLUGGED,
	HDMI_PLUGGED,
	HDMI_PLUGUNDEF
};

enum hdmi_power_state {
	HDMI_POWEROFF,
	HDMI_POWERON,
	HDMI_POWERUNDEF
};

enum hdmi_format {
	HDMI_FORMAT_HDMI,
	HDMI_FORMAT_SDTV,
	HDMI_FORMAT_DVI
};

struct cmd_data {
	__u32 cmd;
	__u32 cmd_id;
	__u32 data_len;
	__u8 data[512];
	struct cmd_data *next;
};

struct threed_support {
	__u8 frame_packing;
	__u8 top_bottom;
	__u8 side_side;
};

struct video_format {
	__u8 cea;	/* 0=VESA, 1=CEA */
	__u8 vesaceanr;
	__u8 threed_format;
	__u8 sink_support;
	__u8 prio;
};

struct vesacea {
	__u8 cea;
	__u8 nr;
};

struct edid_latency {
	int video_latency;
	int audio_latency;
	int intlcd_video_latency;
	int intlcd_audio_latency;
};


#define FORMAT_3D_SIDE_SIDE	0x01
#define FORMAT_3D_TOP_BOTTOM	0x02
#define FORMAT_3D_FRAME_PACKING	0x04

struct threed_format {
	__u8 ceanr;
	__u8 format;
};

#define MAX_VIC_3D 16

struct threed_info {
	__u8			support_2d_50;
	__u8			support_2d_60;
	struct threed_format	format[MAX_VIC_3D];
};

typedef void(*cb_fn)(int cmd, int data_length, __u8 *data);

int cecrx_subscribe(void);
int cecsend(__u32 cmd_id, __u8 in, __u8 dest, __u8 len, __u8 *data);
int cecrx(void);
int edid_read(__u8 block, __u8 *data);
int edid_parse0(__u8 *data, __u8 *extension, struct video_format *, int size);
int edid_parse_ext_map(__u8 *data);
int edid_parse1(__u8 *data, struct video_format formats[], int nr_formats,
		int *basic_audio_support, struct edid_latency *edid_latency,
		int *hdmi, struct threed_info *info_3d);
int edidreq(__u8 block, __u32 cmd_id);
int hdcp_init(__u8 *aes);
int hdcp_state(void);
int video_formats_clear(void);
int threed_info_clear(void);
int vesacea_supported(int *nr_supported, struct vesacea vesacea[]);
int video_formats_supported_hw(void);
int nr_formats_get(void);
struct video_format *video_formats_get(void);
struct threed_info *threed_info_get(void);
int threed_supported(int *nr, struct threed_format formats[]);
void set_vesacea_prio_all(void);
int hdmi_fb_chres(__u8 cea, __u8 vesaceanr);
int vesaceaprio_set(__u8 len, __u8 *data);
void vesacea_prio_default(void);
int hdmievclr(__u8 mask);
void thread_kevent_fn(void *arg);
int hdmiplug_subscribe(void);
int hdmi_event(int event);
int get_best_videoformat(__u8 *cea, __u8 *vesaceanr);
int listensocket_set(int sock);
int listensocket_get(void);
int clientsocket_get(void);
int cecsenderr(void);
int get_new_cmd_id_ind(void);
void thread_socklisten_fn(void *arg);
int cmd_add(struct cmd_data *cmd);
int serversocket_create(int avoid_return_msg);
int serversocket_write(int len, __u8 *data);
int serversocket_close(void);
int poweronoff(__u8 onoff);
int clientsocket_send(__u8 *buf, int len);
int dispdevice_file_open(char *file, int attr);

int hdmi_service_init(int avoid_return_msg);
int hdmi_service_exit(void);
int hdmi_service_enable(void);
int hdmi_service_disable(void);

#ifdef HDMI_SERVICE_USE_CALLBACK_FN
void hdmi_service_callback_set(cb_fn hdmi_cb);
cb_fn hdmi_service_callback_get(void);
#endif /*HDMI_SERVICE_USE_CALLBACK_FN*/

int hdmi_service_resolution_set(int cea, int vesaceanr);
int hdmi_service_fb_release(void);
int hdmi_service_cec_send(__u8 initiator, __u8 destination, __u8 data_size,
							__u8 *data);
int hdmi_service_edid_request(__u8 block);
int hdmi_service_hdcp_init(__u16 aes_size, __u8 *aes_data);
int hdmi_service_3d_infoframe_send(__u8 format);
int hdmi_service_infoframe_send(__u8 type, __u8 version, __u8 crc,
						__u8 data_size, __u8 *data);
int hdmi_service_vesa_cea_prio_set(__u8 vesa_cea1, __u8 nr1,
				__u8 vesa_cea2, __u8 nr2,
				__u8 vesa_cea3, __u8 nr3);

#define AES_KEYS_SIZE	297
#define FORMATS_MAX	35

#define false 0
#define true 1
#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))

#define COMPPATH		"/dev/"
#define COMPPREFIX		"comp"

#ifdef ANDROID
#define FBPATH			"/dev/graphics/"
#define LOGHDMILIBE ALOGE
#define LOGHDMILIB ALOGD
#define LOGHDMILIB2 ALOGD
#define LOGHDMILIB3(format, ...)
#else
#define FBPATH			"/dev/"
#define LOGHDR "libhdmi:"
#define LOGHDMILIBE(format, ...) printf(LOGHDR format"\r\n", __VA_ARGS__)
#define LOGHDMILIB(format, ...) printf(LOGHDR format"\r\n", __VA_ARGS__)
#define LOGHDMILIB2(format, ...) printf(LOGHDR format"\r\n", __VA_ARGS__)
#define LOGHDMILIB3(format, ...) printf(LOGHDR format"\r\n", __VA_ARGS__)
#endif

#ifdef ANDROID
#define SOCKET_LISTEN_PATH	"/dev/socket/hdmi_listen"
#else
#define SOCKET_LISTEN_PATH	"/dev/hdmi_listen"
#endif

#define STOREASTEXT_FILE	"/sys/class/misc/hdmi/storeastext"
#define PLUGDETEN_FILE		"/sys/class/misc/hdmi/plugdeten"
#define EVENT_FILE		"/sys/class/misc/hdmi/evread"
#define EVENTCLR_FILE		"/sys/class/misc/hdmi/evclr"
#define EVWAKEUP_FILE		"/sys/class/misc/hdmi/evwakeup"
#define EDIDREAD_FILE		"/sys/class/misc/hdmi/edidread"
#define HDCPCHKAESOTP_FILE	"/sys/class/misc/hdmi/hdcpchkaesotp"
#define HDCPLOADAES_FILE	"/sys/class/misc/hdmi/hdcploadaes"
#define HDCPSTATEGET_FILE	"/sys/class/misc/hdmi/hdcpstateget"
#define HDCPAUTH_FILE		"/sys/class/misc/hdmi/hdcpauthencr"
#define HDCPEVEN_FILE		"/sys/class/misc/hdmi/hdcpeven"
#define CECSEND_FILE		"/sys/class/misc/hdmi/cecsend"
#define CECRXEVEN_FILE		"/sys/class/misc/hdmi/ceceven"
#define CECREAD_FILE		"/sys/class/misc/hdmi/cecread"
#define INFOFRSEND_FILE		"/sys/class/misc/hdmi/infofrsend"
#define POWERONOFF_FILE		"/sys/class/misc/hdmi/poweronoff"

#define DISPDEVICE_PATH_1	"/sys/devices/"
#define DISPDEVICE_PATH_2	"av8100_hdmi"
#define DISPONOFF_FILE		"disponoff"
#define HDMIFORMAT_FILE		"hdmisdtvswitch"
#define VESACEAFORMATS_FILE	"vesacea"
#define TIMING_FILE		"timing"
#define STAYALIVE_FILE		"stayalive"

#define STOREASTEXT_STR		"01"	/* Use hextext format in sysfs files */
#define STOREASBIN_STR		"00"	/* Use binary format in sysfs files */

#define HDMI_PLUGGED_IN_EVSTR	"01"
#define HDMI_PLUGGED_OUT_EVSTR	"02"
#define HDMI_CEC_EVSTR		"04"
#define HDMI_HDCP_EVSTR		"08"
#define HDMI_CECTXERR_EVSTR	"10"
#define HDMI_USER_EVSTR		"20"

#define EDIDREAD_SIZE		0x80
#define POLL_READ_SIZE		1
#define CEAPRIO_MAX_SIZE	10
#define VESACEAPRIO_DEFAULT	254
#define VESACEAPRIO_USED	255
#define OTP_UNPROGGED		0
#define OTP_PROGGED		1
#define TIMING_SIZE		32
#define CEC_MSG_SIZE_MAX	15
#define INFOFR_MSG_SIZE_MAX	27

#define HDMIEVENT_POLLSIZEFAIL -1
#define HDMIEVENT_EVENTUNKNOWN -2
#define HDMIEVENT_NOEVENT	0
#define HDMIEVENT_HDMIPLUGGED	0x1
#define HDMIEVENT_HDMIUNPLUGGED	0x2
#define HDMIEVENT_CEC		0x4
#define HDMIEVENT_HDCP		0x8
#define HDMIEVENT_CECTXERR	0x10
#define HDMIEVENT_WAKEUP	0x20

#define EVENTMASK_ALL		0xFF
#define EVENTMASK_PLUG		0x03

/* User commands */
#define HDMIEVENT_CMD		0x010000
#define CMD_OFFSET		0
#define CMDID_OFFSET		4
#define CMDLEN_OFFSET		8
#define CMDBUF_OFFSET		12


#define LOADAES_OK		0
#define LOADAES_NOT_OK		-1
#define LOADAES_NOT_FUSED	-2
#define LOADAES_CRC_MISMATCH	-3

#define RESULT_OK		0
#define STOREAS_FAIL		-1
#define SYSFS_FILE_FAILED	-2
#define EDIDREAD_FAIL		-3
#define EDIDREAD_NOEXT		-4
#define EDIDREAD_NOVIDEO	-5
#define EDIDREAD_BL1_TAG_REV_ERR -6
#define EDIDREAD_EXTENSION_MAP_ERR -7
#define HDCP_OK			0
#define AESKEYS_FAIL		-1
#define HDCPSTATE_FAIL		-2
#define HDCPAUTHENCR_FAIL	-3

#define EDID_BL0_HEADER_OFFSET		0x00
#define EDID_BL0_VERSION_OFFSET		0x12
#define EDID_BL0_REVISION_OFFSET	0x13
#define EDID_BL0_EST_TIMING1_OFFSET	0x23
#define EDID_BL0_EST_TIMING2_OFFSET	0x24
#define EDID_BL0_EXTFLAG_OFFSET		0x7E
#define EDID_BL0_ESTTIM1_OFFSET		0x23
#define EDID_BL0_ESTTIM2_OFFSET		0x24
#define EDID_BL0_STDTIM1_OFFSET		0x26
#define EDID_BL0_PIXCLK_OFFSET		0x36
#define EDID_BL0_HORPIXLOW_OFFSET	0x38
#define EDID_BL0_HORBLANKLOW_OFFSET	0x39
#define EDID_BL0_HORPIXHIGH_OFFSET	0x3A
#define EDID_BL0_HORBLANKHIGH_OFFSET	0x3A
#define EDID_BL0_VERTPIXLOW_OFFSET	0x3B
#define EDID_BL0_VERTBLANKLOW_OFFSET	0x3C
#define EDID_BL0_VERTPIXHIGH_OFFSET	0x3D
#define EDID_BL0_VERTBLANKHIGH_OFFSET	0x3D
#define EDID_BL0_INTLCD_OFFSET		0x47

#define EDID_BL1_TAG_OFFSET		0x00
#define EDID_BL1_REVNR_OFFSET		0x01
#define EDID_BL1_OFFSET_OFFSET		0x02
#define EDID_BL1_AUDIO_SUPPORT_OFFSET	0x03
#define EDID_BL1_ESTTIM3_1_FLAG_OFFSET	0x48
#define EDID_BL1_ESTTIM3_2_FLAG_OFFSET	0x5A
#define EDID_BL1_ESTTIM3_3_FLAG_OFFSET	0x6C
#define EDID_BL1_ESTTIM3_BYTE_START	6
#define EDID_BL1_ESTTIM3_BYTE_END	11
#define EDID_BL1_STDTIM9_1_FLAG_OFFSET	0x48
#define EDID_BL1_STDTIM9_2_FLAG_OFFSET	0x5A
#define EDID_BL1_STDTIM9_3_FLAG_OFFSET	0x6C
#define EDID_BL1_STDTIM9_BYTE_START	5
#define EDID_BL1_TAG_EXPECTED		0x02
#define EDID_TAG_EXT_BLOCK_MAP		0xF0
#define EDID_SVD_ID_MASK		0x7F
#define EDID_EXTVER_3			0x03
#define EDID_NO_DATA			0x04
#define EDID_BLK_START			0x04
#define EDID_BLK_CODE_MSK		0xE0
#define EDID_BLK_CODE_SHIFT		5
#define EDID_BLK_LENGTH_MSK		0x1F
#define EDID_CODE_VIDEO			0x02
#define EDID_CODE_VSDB			0x03
#define EDID_BL0_STDTIM1_SIZE		8
#define EDID_BL1_STDTIM9_SIZE		6
#define EDID_STDTIM_AR_MASK		0xC0
#define EDID_STDTIM_AR_SHIFT		6
#define EDID_STDTIM_FREQ_MASK		0x3F
#define EDID_STDTIM_FREQ_SHIFT		0
#define EDID_BASIC_AUDIO_SUPPORT_MASK	0x40
#define EDID_VSD_PHYS_SRC		4
#define EDID_VSD_FLAG_IND		8
#define EDID_VSD_LAT_FLD_MASK		0x80
#define EDID_VSD_INTLCD_LAT_FLD_MASK	0x40
#define EDID_VSD_VIDEO_FLD_MASK		0x20
#define EDID_VSD_VID_LAT		9
#define EDID_VSD_AUD_LAT		10
#define EDID_VSD_INTLCD_VID_LAT		11
#define EDID_VSD_INTLCD_AUD_LAT		12
#define EDID_VSD_3D_MASK		0x80
#define EDID_VSD_3DM_MASK		0x60
#define EDID_VSD_3DM_SHIFT		5
#define EDID_VSD_VICLEN_MASK		0xE0
#define EDID_VSD_VICLEN_SHIFT		5
#define EDID_VSD_3DLEN_MASK		0x1F
#define THREED_STRUCTURE_ALL_HIGH_SHIFT	8
#define VIC_3D_SIZE			8
#define THREED_MULTIPRES_HIGH_SHIFT	8
#define TWOD_VIC_ORDER_MASK		0xF0
#define TWOD_VIC_ORDER_SHIFT		4
#define THREED_STRUCTURE_MASK		0x0F
#define THREED_DETAIL_MASK		0xF0
#define THREED_DETAIL_SHIFT		4
#define EDID_VSD_3D_STR_FRAME_PACKING	0x00
#define EDID_VSD_3D_STR_TOP_BOTTOM	0x06
#define EDID_VSD_3D_STR_SIDE_SIDE	0x08
#define EDID_VSD_3D_STR_ALL_FRAME_PACKING 0x0001
#define EDID_VSD_3D_STR_ALL_TOP_BOTTOM	0x0040
#define EDID_VSD_3D_STR_ALL_SIDE_SIDE	0x0100

/* HDCP states */
#define HDCP_STATE_NO_RECV		0
#define HDCP_STATE_RECV_CONN		1
#define HDCP_STATE_NO_HDCP		2
#define HDCP_STATE_NO_ENCR		3
#define HDCP_STATE_AUTH_ONGOING		4
#define HDCP_STATE_AUTH_FAIL		5
#define HDCP_STATE_AUTH_SUCCEDED	6
#define HDCP_STATE_ENCR_ONGOING		7

#define VIDEO_FORMAT_DEFAULT	1	/* 640x480@60P */

#define STARTUP_DELAY_US	6000000
#define HDCPAUTH_WAITTIME	1000000
#define LOADAES_WAITTIME	250000
#define EDIDREAD_WAITTIME0	2000000
#define EDIDREAD_WAITTIME1	100000
#define FILEOPEN_WAITTIME	10000
#define FILEOPEN_RETRIES	5

#define AVI_INFOFRAME_DATA_SIZE	13
#define AVI_INFOFRAME_TYPE	0x82
#define AVI_INFOFRAME_VERSION	0x02
#define AVI_INFOFRAME_DB1	0x10	/* Active Information present */
#define AVI_INFOFRAME_DB2	0x08	/* Active Portion Aspect ratio */

/* Infoframe 3D_Structure */
#define INFOFR_3D_PACKET_TYPE			0x81
#define INFOFR_3D_VERSION			0x01
#define INFOFR_3D_REG_ID_1			0x03
#define INFOFR_3D_REG_ID_2			0x0C
#define INFOFR_3D_REG_ID_3			0x00
#define INFOFR_3D_VIDEO_FORMAT			0x40
#define INFOFR_3D_STRUCTURE_TOP_AND_BOTTOM	0x60
#define INFOFR_3D_STRUCTURE_SIDE_BY_SIDE_HALF	0x80
#define INFOFR_3D_HOR_SUBSAMPL			0x00
#define INFOFR_3D_RESERVED			0x00
#define INFOFR_2D				0x00

/* Socket listen thread */
#define SOCKET_DATA_MAX 256
#define SOCKET_MAX_CONN 10

/* Command format */
/* Message data format
 *u32 cmd
 *u32 cmd_id
 *u32 size
 *u8 data[size]   data format is specified below
 */

/* HDMI message cmd sent to hdmi_service */
#define HDMI_ENABLE		0x1

#define HDMI_DISABLE		0x2

/* cmd=HDMI_EDIDREQ data format
 *u8 block (0 or 1)
 */
#define HDMI_EDIDREQ		0x3

/* cmd=HDMI_CECSEND and HDMI_CECRECVD data format
 *u8 initiator
 *u8 destination
 *u8 cec_data_size
 *u8 cec_data[cec_data_size]
 */
#define HDMI_CECSEND		0x4

/* cmd=HDMI_FB_RES_SET data format
 *u8 vesa(0)/cea(1)
 *u8 vesa/cea nr
 */
#define HDMI_FB_RES_SET		0x5

#define HDMI_FB_RELEASE		0x6

/* cmd=HDMI_HDCP_INIT data format
 *u8 aesdata[297]
 */
#define HDMI_HDCP_INIT		0x7

/* cmd=HDMI_VESACEAPRIO_SET data format
 *u8 nrofprios
 *u8 vesa/cea[0]	0=VESA, 1=CEA
 *u8 vesaceanr[0]	vesanr or ceanr		Prio 1
 *u8 vesa/cea[1]	0=VESA, 1=CEA
 *u8 vesaceanr[1]	vesanr or ceanr		Prio 2
 *....
 *u8 vesa/cea[size-1]	0=VESA, 1=CEA
 *u8 vesaceanr[size-1]	vesanr or ceanr		Prio size
 */
#define HDMI_VESACEAPRIO_SET	0x8

/* cmd=HDMI_INFOFR data format
 *u8 type
 *u8 version
 *u8 crc
 *u8 size
 *u8 data[size]
 */
#define HDMI_INFOFR		0x9

/*
 * cmd=HDMI_3D_INFOFR data format
 * u8 format	0= 3D off, 1=Side-by-Side(half), 2=Top-and-Bottom
 */
#define HDMI_3D_INFOFR		0xA

/*
 * cmd=HDMI_UNPLUG_SIMULATE
 * No arguments
 * Used for test purpose
 */
#define HDMI_UNPLUG_SIMULATE	0xB

/*
 * cmd=HDMI_PLUG_SIMULATE
 * No arguments
 * Used for test purpose
 */
#define HDMI_PLUG_SIMULATE	0xC

#define HDMI_EXIT		0xFF



#endif /* #ifdef _HDMI_SERVICE_LOCAL_H */

#ifdef __cplusplus
}
#endif
