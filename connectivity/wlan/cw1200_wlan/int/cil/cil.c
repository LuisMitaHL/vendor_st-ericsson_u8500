

/*===========================================================================
*
*Linux Driver for CW1200 series
*
*
*Copyright (c) ST-Ericsson SA, 2010
*
*This program is free software; you can redistribute it and/or modify it
*under the terms of the GNU General Public License version 2 as published
*by the Free Software Foundation.
*
*===========================================================================*/

/**
* DOC: CIL.C
*
* PROJECT	:	CW1200_LINUX_DRIVER
*
* FILE		:	cil.c
*
* This module interfaces with the Linux Kernel CFG80211 layer.
*
* date 25/02/2010
*/


/*****************************************************************************
*						INCLUDE FILES
******************************************************************************/


#include "cil.h"
#include "eil.h"  /*For EIL_Init_Complete */
#include <linux/proc_fs.h>
#include <net/cfg80211.h>
#include <linux/ieee80211.h>
#include <net/netlink.h>
#include <linux/netdevice.h>
#include <linux/etherdevice.h>
#ifdef KERNEL_2_6_32
#include <linux/sched.h>
#endif
#include "UMI_Api.h"
#include "ste_nl80211_testmode_msg_copy.h"
#include "driver_nl80211_testmode_ste_copy.h"

struct CW1200_priv *proc_priv;
uint32_t wlan_psm_flag;
/*****************************************************************
*				LOCAL funcs
******************************************************************/
static int cw1200_change_virtual_intf(struct wiphy *wiphy,
		struct net_device *dev,
		enum nl80211_iftype type, u32 *flags,
		struct vif_params *params);

static int cw1200_join_ibss(struct wiphy *wiphy, struct net_device *dev,
		struct cfg80211_ibss_params *params);

static int cw1200_scan(struct wiphy *wiphy, struct net_device *ndev,
		struct cfg80211_scan_request *request);

/*static int cw1200_auth(struct wiphy *wiphy, struct net_device *dev,
		struct cfg80211_auth_request *req);*/

static int cw1200_add_key(struct wiphy *wiphy, struct net_device *dev,
		u8 key_idx, bool pairwise, const u8 *mac_addr,
		struct key_params *params);

static int cw1200_del_key(struct wiphy *wiphy, struct net_device *ndev,
		u8 key_idx, bool pairwise, const u8 *mac_addr);

static int cw1200_leave_ibss(struct wiphy *wiphy,
		struct net_device *dev);

static int  cw1200_connect(struct wiphy *wiphy,
		struct net_device *dev,
		struct cfg80211_connect_params *sme);

/*static int cw1200_assoc(struct wiphy *wiphy,
		struct net_device *dev,
		struct cfg80211_assoc_request *req);*/

static int cw1200_disconnect(struct wiphy *wiphy,
		struct net_device *dev,
		u16 reason_code);

static int cw1200_set_default_key(struct wiphy *wiphy,
		struct net_device *ndev,
		u8 key_index, bool unicast, bool multicast);

static int cw1200_set_power_mgmt(struct wiphy *wiphy,
		struct net_device *dev,
		bool enabled, int timeout);

static int cw1200_testmode_cmd(struct wiphy *wiphy,
		void *data, int len);

static int tunnel_set_11n_conf(struct wiphy *wiphy,
		struct ste_msg_11n_conf *tdata , int len);

static int cw1200_rssi_config(struct wiphy *wiphy,
			struct net_device *dev,
			s32 rssi_thold, u32 rssi_hyst);

static int cw1200_beacon_miss_config(struct wiphy *wiphy,
				struct net_device *dev,
				u32 beacon_thold);

static int cw1200_tx_fail_config(struct wiphy *wiphy,
				struct net_device *dev,
				u32 tx_thold);

static int cw1200_set_pmksa(struct wiphy *wiphy, struct net_device *netdev,
				struct cfg80211_pmksa *pmksa);

static int cw1200_del_pmksa(struct wiphy *wiphy, struct net_device *netdev,
				struct cfg80211_pmksa *pmksa);

void cw1200_bss_loss_work(struct work_struct *work);

static void cw1200_unresponsive_driver(struct work_struct *work);

void cw1200_connection_loss_work(struct work_struct *work);

void cw1200_handle_delayed_link_loss(struct CW1200_priv *priv);

void cw1200_check_connect(struct work_struct *work);

struct net_device * cw1200_add_virtual_intf(struct wiphy *wiphy, char *name,
				enum nl80211_iftype type, u32 *flags,
				struct vif_params *params);

int cw1200_del_virtual_intf(struct wiphy *wiphy, struct net_device *dev);

int cw1200_set_channel(struct wiphy *wiphy, struct net_device *dev,
			struct ieee80211_channel *chan,
			enum nl80211_channel_type channel_type);

int cw1200_add_beacon(struct wiphy *wiphy, struct net_device *dev,
			      struct beacon_parameters *info);

int cw1200_set_beacon(struct wiphy *wiphy, struct net_device *dev,
			      struct beacon_parameters *info);

int cw1200_deauth(struct wiphy *wiphy, struct net_device *dev,
			  struct cfg80211_deauth_request *req,
			  void *cookie);

int cw1200_del_beacon(struct wiphy *wiphy, struct net_device *dev);

int cw1200_get_station(struct wiphy *wiphy, struct net_device *dev,
			 u8 *mac, struct station_info *sinfo);

int cw1200_remain_on_channel(struct wiphy *wiphy,
				     struct net_device *dev,
				     struct ieee80211_channel *chan,
				     enum nl80211_channel_type channel_type,
				     unsigned int duration,
				     uint64_t *cookie);

int cw1200_cancel_remain_on_channel(struct wiphy *wiphy,
					struct net_device *dev,
					uint64_t cookie);

int cw1200_mgmt_tx(struct wiphy *wiphy, struct net_device *dev,
			struct ieee80211_channel *chan, bool offchan,
			enum nl80211_channel_type channel_type,
			bool channel_type_valid, unsigned int wait,
			const u8 *buf, size_t len, u64 *cookie);

int cw1200_mgmt_tx_cancel_wait(struct wiphy *wiphy,
			       struct net_device *dev,
			       u64 cookie);

void cw1200_mgmt_frame_register(struct wiphy *wiphy,
				struct net_device *dev,
				uint16_t frame_type, bool reg);

#if 0 //From Prameela
void cw1200_roc_timeout(struct work_struct *work);
#endif

void cw1200_per_start(struct work_struct *work);

static int tunnel_set_p2p_state(struct wiphy *wiphy,
		struct ste_msg_set_p2p *tdata , int len);

static int tunnel_set_uapsd(struct wiphy *wiphy,
		struct ste_msg_set_uapsd *tdata , int len);

static int tunnel_set_p2p_power_save(struct wiphy *wiphy,
		void *tdata, int len, int p2p_ps_type);

static int hif_debug(struct file *file, const char __user *buffer,
			unsigned long count, void *data);

static int __cw1200_add_key(struct wiphy *wiphy, struct net_device *ndev,
				u8 key_idx, bool pairwise, const u8 *mac_addr,
				struct key_params *params);

static int tunnel_set_p2p_probe_res_ie(struct wiphy *wiphy,
		void *tdata , int len);

static int tunnel_start_packet_error_rate(struct wiphy *wiphy,
		void *tdata , int len);

static int tunnel_set_rssi_filter( struct wiphy *wiphy,
		void *tdata , int len);
static int cw1200_suspend(struct wiphy *wiphy);

static int cw1200_resume(struct wiphy *wiphy);

static void default_key(struct net_device *ndev,
		u8 key_index);

uint8_t *CIL_get_noa_attr(struct CW1200_priv *, int32_t *);

static int tunnel_p2p_disconnect_client(struct wiphy *wiphy,
		void *tdata , int len);

static inline void cw1200_init_key_map_table(struct CW1200_priv *priv);

static inline CW1200_STATUS_E cw1200_add_key_map_index(struct CW1200_priv *priv,
		const u8 *mac_addr, uint8 e_index);

static inline int8_t cw1200_remove_key_map_index(struct CW1200_priv *priv,
		const u8 *mac_addr);


static int tunnel_p2p_disconnect_client(struct wiphy *wiphy,
		void *tdata , int len);

void cw1200_scan_timeout(struct work_struct *work);


static const u32 cw1200_cipher_suites[] = {
	WLAN_CIPHER_SUITE_WEP40,
	WLAN_CIPHER_SUITE_WEP104,
	WLAN_CIPHER_SUITE_TKIP,
	WLAN_CIPHER_SUITE_CCMP,
#ifdef WAPI
	WLAN_CIPHER_SUITE_SMS4
#endif
};

static const struct ieee80211_txrx_stypes
ieee80211_default_mgmt_stypes[NUM_NL80211_IFTYPES] = {
	[NL80211_IFTYPE_ADHOC] = {
		.tx = 0xffff,
		.rx = BIT(IEEE80211_STYPE_ACTION >> 4),
	},
	[NL80211_IFTYPE_STATION] = {
		.tx = 0xffff,
		.rx = BIT(IEEE80211_STYPE_ACTION >> 4) |
			BIT(IEEE80211_STYPE_PROBE_REQ >> 4),
	},
	[NL80211_IFTYPE_AP] = {
		.tx = 0xffff,
		.rx = BIT(IEEE80211_STYPE_ASSOC_REQ >> 4) |
			BIT(IEEE80211_STYPE_REASSOC_REQ >> 4) |
			BIT(IEEE80211_STYPE_PROBE_REQ >> 4) |
			BIT(IEEE80211_STYPE_DISASSOC >> 4) |
			BIT(IEEE80211_STYPE_AUTH >> 4) |
			BIT(IEEE80211_STYPE_DEAUTH >> 4) |
			BIT(IEEE80211_STYPE_ACTION >> 4),
	},
	[NL80211_IFTYPE_AP_VLAN] = {
		/* copy AP */
		.tx = 0xffff,
		.rx = 0xffff,
		},
	[NL80211_IFTYPE_P2P_CLIENT] = {
		.tx = 0xffff,
		.rx = BIT(IEEE80211_STYPE_ACTION >> 4) |
			BIT(IEEE80211_STYPE_PROBE_REQ >> 4),
	},
	[NL80211_IFTYPE_P2P_GO] = {
		.tx = 0xffff,
		.rx = BIT(IEEE80211_STYPE_ASSOC_REQ >> 4) |
			BIT(IEEE80211_STYPE_REASSOC_REQ >> 4) |
			BIT(IEEE80211_STYPE_PROBE_REQ >> 4) |
			BIT(IEEE80211_STYPE_DISASSOC >> 4) |
			BIT(IEEE80211_STYPE_AUTH >> 4) |
			BIT(IEEE80211_STYPE_DEAUTH >> 4) |
			BIT(IEEE80211_STYPE_ACTION >> 4),
	},
	[NL80211_IFTYPE_MESH_POINT] = {
		.tx = 0x00,
		.rx = 0x00,
	},
};

/* indicated the link speed rate (Mbit/s) type used in Upper MAC */
static int link_speed[22] = {1, 2, 5, 11, 22, 33, 6, 9, 12, 18,
			     24, 36, 48, 54, 6, 13, 19, 26, 39,
			     52, 58, 65};


static int CIL_AP_Open(struct net_device *dev)
{
	DEBUG(DBG_CIL, "%s, Called \n", __func__);
	return SUCCESS;
}

static int CIL_AP_Stop(struct net_device *dev)
{
	DEBUG(DBG_CIL, "%s, Called \n", __func__);
	return SUCCESS;
}

static int CIL_AP_Transmit(struct sk_buff *skb, struct net_device *dev)
{
	u8 *pos= NULL;
	struct ieee80211_hdr *hdr= NULL;
	struct cfg_priv *c_priv = NULL;
	int32_t retval = SUCCESS;
	u8 sa[ETH_ALEN];
	u8 da[ETH_ALEN];
	UMI_STATUS_CODE umi_status = UMI_STATUS_SUCCESS;
	struct CW1200_priv *priv = NULL;
	int32_t sta_link_id;

	c_priv = netdev_priv(dev);
	priv = c_priv->driver_priv;

	pos = (u8 *) (skb->data + 24 + 14);
	/* Skip rfc1042_header */
	pos += 6;

	if (ETH_P_PAE == GET_BE16(pos) ) {
		DEBUG(DBG_CIL, "EAPOL frame \n");
		/* 802.11 -> 802.3 conversion */
		memcpy(sa, &skb->data[18+6], ETH_ALEN);
		memcpy(da, &skb->data[18], ETH_ALEN);

		memcpy(&skb->data[18+6], da, ETH_ALEN);
		memcpy(&skb->data[18+6+6], sa, ETH_ALEN);

		/* Update SKB pointer */
		skb_pull(skb, 14 + 4 + 6);
		retval = EIL_Transmit(skb, priv->netdev);
	} else {
		print_hex_dump_bytes ( "PACKET_CIL:", DUMP_PREFIX_NONE ,
				skb->data ,skb->len );
		/* Update SKB pointer- Skip radio tap header */
		skb_pull(skb, 14);
		hdr = (struct ieee80211_hdr *)skb->data;
		/* Check if the frame received is ACTION frame */
		if ( IEEE80211_FC(0,13) == hdr->frame_control ) {
			DEBUG(DBG_CIL, "%s, Send Action Frame\n", __func__);
			sta_link_id = map_mac_addr(priv, hdr->addr1);

			if (-1 == sta_link_id)
				sta_link_id = 0;
			umi_status = UMI_MgmtTxFrame(priv->umac_handle,
					sta_link_id, skb->len, skb->data);
			if (UMI_STATUS_SUCCESS != umi_status) {
				DEBUG(DBG_CIL, "%s,UMI_MgmtTxFrame:Error:%d\n",
					__func__, umi_status);
				retval = -EIO;
			}
			dev_kfree_skb(skb);
		}
	}
	DEBUG(DBG_CIL, "%s, Called:Retval:[%d] \n", __func__, retval);

	return retval;
}

static const struct net_device_ops ap_netdev_ops = {
	.ndo_open                = CIL_AP_Open,
	.ndo_stop                = CIL_AP_Stop,
	.ndo_start_xmit          = CIL_AP_Transmit,
};

static struct cfg80211_ops cw1200_config_ops = {
	.change_virtual_intf = cw1200_change_virtual_intf,
	.scan = cw1200_scan,
	.join_ibss = cw1200_join_ibss,
	.leave_ibss = cw1200_leave_ibss,
	.connect = cw1200_connect,
	.add_key = cw1200_add_key,
	.del_key = cw1200_del_key,
	.disconnect = cw1200_disconnect,
	.set_default_key = cw1200_set_default_key,
	.testmode_cmd = cw1200_testmode_cmd,
	.set_cqm_rssi_config = cw1200_rssi_config,
	.set_cqm_beacon_miss_config = cw1200_beacon_miss_config,
	.set_cqm_tx_fail_config = cw1200_tx_fail_config,
	.set_pmksa = cw1200_set_pmksa,
	.del_pmksa = cw1200_del_pmksa,
	.add_virtual_intf = cw1200_add_virtual_intf,
	.del_virtual_intf  = cw1200_del_virtual_intf,
	.set_channel = cw1200_set_channel,
	.add_beacon = cw1200_add_beacon,
	.set_beacon = cw1200_set_beacon,
	.del_beacon = cw1200_del_beacon,
	.deauth = cw1200_deauth,
	.get_station = cw1200_get_station,
	.remain_on_channel = cw1200_remain_on_channel,
	.cancel_remain_on_channel = cw1200_cancel_remain_on_channel,
	.mgmt_tx = cw1200_mgmt_tx,
	.mgmt_tx_cancel_wait = cw1200_mgmt_tx_cancel_wait,
	.mgmt_frame_register = cw1200_mgmt_frame_register,
	.suspend = cw1200_suspend,
	.resume = cw1200_resume,
};

#define RATETAB_ENT(_rate, _rateid, _flags)		\
	{						\
	.bitrate		= (_rate),		\
	.hw_value		= (_rateid),		\
	.flags			= (_flags),		\
	}


static struct ieee80211_rate cw1200_rates[] = {
	RATETAB_ENT(10,  0x1,   0),
	RATETAB_ENT(20,  0x2,   0),
	RATETAB_ENT(55,  0x4,   0),
	RATETAB_ENT(110, 0x8,   0),
	RATETAB_ENT(60,  0x10,  0),
	RATETAB_ENT(90,  0x20,  0),
	RATETAB_ENT(120, 0x40,  0),
	RATETAB_ENT(180, 0x80,  0),
	RATETAB_ENT(240, 0x100, 0),
	RATETAB_ENT(360, 0x200, 0),
	RATETAB_ENT(480, 0x400, 0),
	RATETAB_ENT(540, 0x800, 0),
};

#define cw1200_a_rates		(cw1200_rates + 4)
#define cw1200_a_rates_size		8
#define cw1200_g_rates		(cw1200_rates + 0)
#define cw1200_g_rates_size		12


#define CHAN2G(_channel, _freq, _flags) {			\
	.band			= IEEE80211_BAND_2GHZ,		\
	.center_freq	= 	(_freq),			\
	.hw_value		= (_channel),			\
	.flags			= (_flags),			\
	.max_antenna_gain	= 0,				\
	.max_power		= 30,				\
}

#define CHAN5G(_channel, _flags) {				\
	.band			= IEEE80211_BAND_5GHZ,		\
	.center_freq	= 5000 + (5 * (_channel)),		\
	.hw_value		= (_channel),			\
	.flags			= (_flags),			\
	.max_antenna_gain	= 0,				\
	.max_power		= 30,				\
}

static struct ieee80211_channel cw1200_2ghz_chantable[] = {
	CHAN2G(1, 2412, 0),
	CHAN2G(2, 2417, 0),
	CHAN2G(3, 2422, 0),
	CHAN2G(4, 2427, 0),
	CHAN2G(5, 2432, 0),
	CHAN2G(6, 2437, 0),
	CHAN2G(7, 2442, 0),
	CHAN2G(8, 2447, 0),
	CHAN2G(9, 2452, 0),
	CHAN2G(10, 2457, 0),
	CHAN2G(11, 2462, 0),
	CHAN2G(12, 2467, 0),
	CHAN2G(13, 2472, 0),
	CHAN2G(14, 2484, 0),
};

static struct ieee80211_channel cw1200_5ghz_chantable[] = {
	CHAN5G(34, 0), 		CHAN5G(36, 0),
	CHAN5G(38, 0), 		CHAN5G(40, 0),
	CHAN5G(42, 0), 		CHAN5G(44, 0),
	CHAN5G(46, 0), 		CHAN5G(48, 0),
	CHAN5G(52, 0), 		CHAN5G(56, 0),
	CHAN5G(60, 0), 		CHAN5G(64, 0),
	CHAN5G(100, 0),		CHAN5G(104, 0),
	CHAN5G(108, 0),		CHAN5G(112, 0),
	CHAN5G(116, 0),		CHAN5G(120, 0),
	CHAN5G(124, 0),		CHAN5G(128, 0),
	CHAN5G(132, 0),		CHAN5G(136, 0),
	CHAN5G(140, 0),		CHAN5G(149, 0),
	CHAN5G(153, 0),		CHAN5G(157, 0),
	CHAN5G(161, 0),		CHAN5G(165, 0),
	CHAN5G(184, 0),		CHAN5G(188, 0),
	CHAN5G(192, 0),		CHAN5G(196, 0),
	CHAN5G(200, 0),		CHAN5G(204, 0),
	CHAN5G(208, 0),		CHAN5G(212, 0),
	CHAN5G(216, 0),
};


static struct ieee80211_supported_band cw1200_band_2ghz = {
	.channels = cw1200_2ghz_chantable,
	.n_channels = ARRAY_SIZE(cw1200_2ghz_chantable),
	.bitrates = cw1200_g_rates,
	.n_bitrates = cw1200_g_rates_size,
};

static struct ieee80211_supported_band cw1200_band_5ghz = {
	.channels = cw1200_5ghz_chantable,
	.n_channels = ARRAY_SIZE(cw1200_5ghz_chantable),
	.bitrates = cw1200_a_rates,
	.n_bitrates = cw1200_a_rates_size,
};


#define DBG_SEM DBG_CIL

#define DOWN(__sem__, __buf__) do { \
	DEBUG(DBG_SEM, "--> down begin %s\n", __func__); \
	if (down_interruptible(__sem__) < 0) { \
		DEBUG(DBG_SEM, "--> interrupted %s", __func__); \
		WARN_ON(1); \
		return -EAGAIN; \
	} \
	DEBUG(DBG_SEM, "--> down ok %s %s\n", __func__, (__buf__)); \
} while(0)

#define DOWN_TRYLOCK(__sem__) do { \
	DEBUG(DBG_SEM, "--> down_trylock begin %s\n", __func__); \
	if (down_trylock(__sem__) != 0) { \
		DEBUG(DBG_SEM, "--> contended %s\n", __func__); \
		return -EAGAIN; \
	} \
	DEBUG(DBG_SEM, "--> down_trylock ok %s\n", __func__); \
} while(0)

#define UP(__sem__, __buf__) do { \
	DEBUG(DBG_SEM, "--> up %s %s\n", __func__, (__buf__)); \
	up(__sem__); \
} while(0)

extern uint32_t hif_dbg_flag;

static int hif_debug(struct file *file, const char __user *buffer,
			unsigned long count, void *data)
{
	char *page;
	unsigned long val = 0;
	UMI_POWER_MODE power_mode;
	uint32_t oprPowerMode = 0;
	int retval;

	page = kmalloc(count, GFP_KERNEL);
	if (!page)
		return -ENOMEM;

	if (copy_from_user(page, buffer, count)) {
		kfree(page);
		return -EFAULT;
	}
	if (count > 1)
		val = simple_strtoul(page, NULL, 10);

	if (1 == val) {
		DEBUG(DBG_CIL,"WLAN HIF Debugging ENABLED \n");
		hif_dbg_flag = 1;
	} else if (2 == val) {
		wlan_psm_flag = 2;
		DEBUG(DBG_CIL,"WLAN PSM ENABLED \n");
		DOWN(&proc_priv->cil_sem, "PSM Enable runtime");
		oprPowerMode = proc_priv->operational_power_mode;
		retval = CIL_Set(proc_priv,
				UMI_DEVICE_OID_802_11_OPERATIONAL_POWER_MODE,
				&oprPowerMode, 4);
		if (retval) {
			wlan_psm_flag = 4;
			DEBUG(DBG_CIL, "CIL_Set() for UMI_DEVICE"
				"_OID_802_11_OPERATIONAL_POWER_MODE returned error\n");
			UP(&proc_priv->cil_sem, "PSM Enable runtime");
			goto end;
		}

		memset(&power_mode, 0, sizeof(UMI_POWER_MODE));
		/*  Only use fastps if p2p is disabled */
		if (proc_priv->p2p_enabled == false) {
			/* Enable POWER mode with FAST PSM */
			power_mode.PmMode = 0x81;
			/* Multiply by 2 as UMAC unit is 500us */
			power_mode.FastPsmIdlePeriod = proc_priv->dynamic_ps_timeout * 2;
		} else
			power_mode.PmMode = 1;
		retval = CIL_Set(proc_priv,
				UMI_DEVICE_OID_802_11_POWER_MODE,
				&power_mode, sizeof(UMI_POWER_MODE));
		if (retval) {
			wlan_psm_flag = 4;
			DEBUG(DBG_CIL, "proc PSM :CIL_Set"
				"for POWER MODE returned error \n");
		}
		UP(&proc_priv->cil_sem, "PSM Enable runtime");
	} else if (3 == val){
		wlan_psm_flag = 3;
		DEBUG(DBG_CIL,"WLAN PSM DISABLED \n");
		DOWN(&proc_priv->cil_sem, "PSM Disable runtime");
		oprPowerMode = WLAN_ACTIVE_MODE;
		retval = CIL_Set(proc_priv,
				UMI_DEVICE_OID_802_11_OPERATIONAL_POWER_MODE,
				&oprPowerMode, 4);
		if (retval) {
			wlan_psm_flag = 4;
			DEBUG(DBG_CIL, "CIL_Set() for UMI_DEVICE"
				"_OID_802_11_OPERATIONAL_POWER_MODE returned error\n");
			UP(&proc_priv->cil_sem, "PSM Disable runtime");
			goto end;
		}

		memset(&power_mode, 0, sizeof(UMI_POWER_MODE));
		retval = CIL_Set(proc_priv,
				UMI_DEVICE_OID_802_11_POWER_MODE,
				&power_mode, sizeof(UMI_POWER_MODE));
		if (retval) {
			wlan_psm_flag = 4;
			DEBUG(DBG_CIL, "proc PSM CIL_Set"
				"for POWER MODE returned error \n");
		}
		UP(&proc_priv->cil_sem, "PSM Disable runtime");
	} else if (0 == val){
		DEBUG(DBG_CIL,"WLAN HIF Debugging DISABLED \n");
		hif_dbg_flag = 0;
	} else if (4 == val){
		DEBUG(DBG_CIL,"WLAN PSM enable/disable will be controlled by supplicant\n");
		wlan_psm_flag = 4;
	}

end:
	kfree(page);
	return count;
}


/**
* CIL_Init
*
* This func initialises the CIL layer.It registers with the CFG80211.
* return the Status code - Pass or failure.
*
* @priv: pointer to the driver private structure.
*/
CW1200_STATUS_E CIL_Init(struct CW1200_priv *priv)
{

	struct wireless_dev *wdev;
	struct device *dev = &(priv->func->dev);
	int32_t  ret = 0;
	struct cfg_priv *c_priv = NULL;
	CW1200_STATUS_E result = SUCCESS;
	proc_priv = priv;
	DEBUG(DBG_MESSAGE , "CIL_Init Called\n");
	printk(KERN_ERR"################# CIL_Init");
	sema_init(&(priv->cil_sem), 1);
	spin_lock_init(&(priv->cil_lock));
	init_waitqueue_head(&priv->cil_set_wait);
	init_waitqueue_head(&priv->cil_get_wait);
	atomic_set(&priv->cil_set_cond, 0);
	priv->get_buff_pointer = NULL;
	priv->get_buff_len = 0;
	priv->connection_status = CW1200_DISCONNECTED;
	priv->wait_flag_set = FALSE;
	priv->wait_flag_get = FALSE;
	priv->suspend = FALSE;
	/* Add runtime control for enabling various WLAN driver params */
	priv->proc =  create_proc_entry("wlan_hif_debug", S_IFREG | 0644 ,init_net.proc_net);
	if (!priv->proc) {
		PRINT("CIL: Unable to create proc entry\n");
		return -ERROR;
	}
	priv->proc->write_proc = hif_debug;
	priv->proc->read_proc = NULL;
	priv->proc->uid = 0;
	priv->proc->gid = 0;
	priv->ctwindow = 0;

	priv->arp_offload = FALSE;

	wdev = kzalloc(sizeof(struct wireless_dev), GFP_KERNEL);
	if (!wdev) {
		result = -ERROR;
		goto fail_1;
	}
	wdev->wiphy = wiphy_new(&cw1200_config_ops,
			sizeof(struct cfg_priv));
	if (!wdev->wiphy) {
		result = -ERROR;
		goto fail_2;
	}
	c_priv = wiphy_priv(wdev->wiphy);
	c_priv->driver_priv = priv;
	set_wiphy_dev(wdev->wiphy, dev);
	wdev->wiphy->interface_modes = BIT(NL80211_IFTYPE_STATION) |
		BIT(NL80211_IFTYPE_ADHOC) | BIT(NL80211_IFTYPE_AP) |
		BIT(NL80211_IFTYPE_MONITOR) |
		BIT(NL80211_IFTYPE_P2P_GO) | BIT(NL80211_IFTYPE_P2P_CLIENT);

	wdev->wiphy->bands[IEEE80211_BAND_2GHZ] = &cw1200_band_2ghz;
	if (c_priv->driver_priv->dualband)
		wdev->wiphy->bands[IEEE80211_BAND_5GHZ] = &cw1200_band_5ghz;

	wdev->wiphy->signal_type = CFG80211_SIGNAL_TYPE_MBM;
	wdev->wiphy->cipher_suites = cw1200_cipher_suites;
	wdev->wiphy->n_cipher_suites = ARRAY_SIZE(cw1200_cipher_suites);
	wdev->wiphy->max_scan_ie_len = IE_LEN_MAX ;
	wdev->wiphy->max_scan_ssids = 2;
	wdev->wiphy->mgmt_stypes = ieee80211_default_mgmt_stypes;
	wdev->wiphy->max_remain_on_channel_duration = 5000;
	ret = wiphy_register(wdev->wiphy);
	if (ret < 0) {
		DEBUG(DBG_CIL, "CIL_Init():Couldn't register wiphy device\n");
		result = -ERROR;
		goto fail_3;
	}

	/*Update Wireless Device Pointer */
	priv->netdev->ieee80211_ptr = wdev;
	SET_NETDEV_DEV(priv->netdev, wiphy_dev(wdev->wiphy));
	wdev->netdev = priv->netdev;
	wdev->iftype = NL80211_IFTYPE_ADHOC;

	/* Initialise default values for BLOCK ACK */
	priv->tx_block_ack = TX_BLOCK_ACK_TIDS;
	priv->rx_block_ack = RX_BLOCK_ACK_TIDS;

	priv->bss_loss_WQ = create_singlethread_workqueue("bss_loss_work");
	if (priv->bss_loss_WQ == NULL) {
		DEBUG(DBG_MESSAGE, "Error in creating bss_loss_WQ!!!!\n");
		result = -ENOMEM;
		goto fail_3;
	}

	INIT_DELAYED_WORK(&priv->bss_loss_work, cw1200_bss_loss_work);
	INIT_DELAYED_WORK(&sdio_cw_priv.unload_work, cw1200_unresponsive_driver);
	INIT_DELAYED_WORK(&priv->connection_loss_work,
				cw1200_connection_loss_work);
	INIT_DELAYED_WORK(&priv->connect_result, cw1200_check_connect);

#if 0 // From Prameela

	/* Init delayed work for handling Remain on Chan timeout */
	INIT_DELAYED_WORK(&priv->roc_work, cw1200_roc_timeout);
#endif

        /* Init SCAN Watchdog */
	INIT_DELAYED_WORK(&priv->scan_timeout, cw1200_scan_timeout);

	INIT_DELAYED_WORK(&priv->per_work, cw1200_per_start);

	init_completion(&priv->roc_comp);


	priv->cqm_beacon_loss_count = CQM_BEACON_LOSS;
	priv->cqm_link_loss_count = CQM_LINK_LOSS;

	memset(&priv->pmk_list, 0, sizeof(UMI_BSSID_PMKID_BKID));
	memset(&priv->connected_ssid, 0, 32);
	priv->sta_count = 0;

	mutex_init(&priv->cfg_set);
	mutex_init(&priv->scan_timeout_lock);

	priv->minAutoTriggerInterval = 0;
	priv->maxAutoTriggerInterval = 0;
	priv->autoTriggerStep = 0;
	/* Allocate internal structures needed to store channel list */
	priv->sta_info = kzalloc(
			sizeof(UMI_EVT_DATA_CONNECT_INFO) * MAX_SUPPORTED_STA,
			GFP_KERNEL);
	if (!priv->sta_info) {
		DEBUG(DBG_MESSAGE , "CIL_Init():Out of Memory\n");
		result = -ENOMEM;
		goto fail_4;
	}

	/* Initialize key map index table */
	cw1200_init_key_map_table(priv);

	return SUCCESS;
fail_4:
	DEBUG(DBG_MESSAGE , "%s: Destroy mutex\n", __func__);
	mutex_destroy(&priv->cfg_set);
	mutex_destroy(&priv->scan_timeout_lock);
fail_3:
	DEBUG(DBG_MESSAGE , "%s: Destroy workqueue\n", __func__);
	if (priv->bss_loss_WQ)
		destroy_workqueue(priv->bss_loss_WQ);

	wiphy_free(wdev->wiphy);
fail_2:
	kfree(wdev);
fail_1:
	remove_proc_entry("wlan_hif_debug", init_net.proc_net);
	return result;
}


/**
*  CIL_Shutdown
*
* This func de initialises the CIL layer.
* Return the Status code - Pass or failure code.
*
* @priv: pointer to the driver private structure.
*/
CW1200_STATUS_E CIL_Shutdown(struct CW1200_priv *priv)
{
	DEBUG(DBG_MESSAGE, "%s: called\n", __func__);

	if (NULL == priv) {
		DEBUG(DBG_MESSAGE, "%s: priv NULL\n", __func__);
		return ERROR;
	}

	if (priv->netdev)
		if (priv->netdev->ieee80211_ptr) {
			wiphy_unregister(priv->netdev->ieee80211_ptr->wiphy);
			wiphy_free(priv->netdev->ieee80211_ptr->wiphy);
			kfree(priv->netdev->ieee80211_ptr);
		} else
			DEBUG(DBG_MESSAGE, "%s: ieee80211_ptr NULL\n",
						__func__);
	else
		DEBUG(DBG_MESSAGE, "%s: ieee80211_ptr NULL\n",
					__func__);

	cancel_delayed_work_sync(&priv->bss_loss_work);
	cancel_delayed_work_sync(&priv->connection_loss_work);
	cancel_delayed_work_sync(&priv->per_work);
	cancel_delayed_work_sync(&priv->connect_result);
	cancel_delayed_work_sync(&priv->roc_work);
	cancel_delayed_work_sync(&priv->scan_timeout);

	if (priv->bss_loss_WQ) {
		flush_workqueue(priv->bss_loss_WQ);
		destroy_workqueue(priv->bss_loss_WQ);
	}
	remove_proc_entry("wlan_hif_debug", init_net.proc_net);
	mutex_destroy(&priv->cfg_set);
	mutex_destroy(&priv->scan_timeout_lock);
	kfree(priv->sta_info);

	return SUCCESS;
}


/**
* CIL_Set
*
* This func is called to pass a new OID value to the UMAC.
* returns the Length of the OID data to be set.
*
* @priv: pointer to the driver private structure.
* @UMI_DEVICE_OID_E: The OID value to be set.
* @oid_data: pointer to the OID data to be set.
*/
CW1200_STATUS_E CIL_Set(struct CW1200_priv *priv,
			enum UMI_DEVICE_OID_E oid_to_set,
			void *oid_data, int32_t length)
{
	UMI_STATUS_CODE umi_status = UMI_STATUS_SUCCESS;
	CW1200_STATUS_E status = ERROR;
	long val = 0;
	int retval = 0;

	if (unlikely(atomic_read(&(priv->cw1200_unresponsive)) == TRUE))
		return ERROR;

	mutex_lock(&priv->cfg_set);
	val = atomic_read(&priv->cil_set_cond);
	priv->set_status = WAIT_INIT_VAL;
	WARN_ON(val);

	if (val)
		DEBUG(DBG_CIL, "cil_set_cond: %ld\n", val);

	atomic_set(&priv->cil_set_cond, 0);
	/* Acquire spinlock as this context
	can be broken by the SBUS bottom half */
	spin_lock_bh(&(priv->cil_lock));
	umi_status = UMI_SetParameter(priv->umac_handle, oid_to_set,
					length, priv->setget_linkid, oid_data);
	spin_unlock_bh(&(priv->cil_lock));
	if (UMI_STATUS_SUCCESS == umi_status) {
		status = SUCCESS;
	} else if (UMI_STATUS_PENDING == umi_status) {
		DEBUG(DBG_CIL, "CIL_Set():Going to sleep \n");
		if (atomic_add_return(1, &priv->cil_set_cond) == 1) {
			retval = wait_event_interruptible_timeout(priv->cil_set_wait,
						priv->wait_flag_set, 4*HZ);
			if (retval < 0) {
				DEBUG(DBG_CIL, "CIL_Set(): interrupted for  OID = [%d]\n", oid_to_set);
				WARN_ON(1); 
			} else if (retval == 0) {
				DEBUG(DBG_CIL, "CIL_Set(): Timed out for  OID = [%d]\n", oid_to_set);
				WARN_ON(1);
			}
			priv->wait_flag_set = FALSE ;
		}
		DEBUG(DBG_CIL, "CIL_Set(): Woken up \n");
		/*Control comes here after wakeup or wait completion */
		if (priv->set_status != SUCCESS) {
			DEBUG(DBG_CIL, "UMI_SetParameter():"
					"Complete return err[%x]\n",
					umi_status);
		} else
			status = SUCCESS;
	} else 	{
		status = umi_status;
		DEBUG(DBG_CIL, "UMI_SetParameter() return error [%x]\n",
				umi_status);
	}
	mutex_unlock(&priv->cfg_set);
	return status;
}


/**
* CIL_Get
*
* This func is called to read an OID value from the UMAC.
* returns the Length of the OID data to be read.
*
* @priv: pointer to the driver private structure.
* @UMI_DEVICE_OID_E: The OID value to be read.
* @oid_data: pointer to the memory buffer where the OID data will be stored.
*/
void *CIL_Get(struct CW1200_priv *priv,
		enum UMI_DEVICE_OID_E oid_to_get,
		int32_t length)
{
	UMI_GET_PARAM_STATUS *pUMIGetParamStatus = NULL;
	void *oid_data = NULL;

	if (unlikely(atomic_read(&(priv->cw1200_unresponsive)) == TRUE))
			return NULL;

	pUMIGetParamStatus = UMI_GetParameter(priv->umac_handle,
						oid_to_get,priv->setget_linkid);

	if (NULL == pUMIGetParamStatus) {
		return NULL;
	}

	if (UMI_STATUS_SUCCESS == pUMIGetParamStatus->status) {
		DEBUG(DBG_CIL, "UMI_GetParameter() returned success \n");
		oid_data = pUMIGetParamStatus->pValue;
		if (pUMIGetParamStatus->length != length)
			DEBUG(DBG_CIL, "Returned Len is more than"
					"the buffer len \n");
		/*Free container structure The oid_data will be
		* freed by the caller */
		kfree(pUMIGetParamStatus);
		pUMIGetParamStatus = NULL;
	} else if (UMI_STATUS_PENDING == pUMIGetParamStatus->status) {
			DEBUG(DBG_CIL, "CIL_Get():Going to sleep \n");
			priv->get_buff_pointer = NULL;
			priv->get_buff_len = 0;
			/*Wait for the response */
			wait_event_interruptible(priv->cil_get_wait ,
						priv->wait_flag_get);
			priv->wait_flag_get = FALSE;
			if (NULL != priv->get_buff_pointer) {
				oid_data = priv->get_buff_pointer;
				if (length != priv->get_buff_len)
					DEBUG(DBG_CIL, "Return Len is"
							"more than buf len \n");
				kfree(pUMIGetParamStatus);
			} else
				goto err_out;
	} else
		goto err_out;

	return oid_data;
err_out:
	kfree(pUMIGetParamStatus);
	return NULL;
}


/**
* CIL_get_sta_linkid
*
* This func gets the linkID for the given MAC address.
* returns the MAC address to search.
*
* @priv: pointer to the driver private structure.
*/
int CIL_get_sta_linkid(struct CW1200_priv *priv,const uint8_t * mac_addr)
{
	uint32_t i=0;
	int sta_link_id = -1;

	UMI_EVT_DATA_CONNECT_INFO *sta_list = NULL;
	if (NL80211_IFTYPE_AP < priv->device_mode) {
		return 0;
	}
	if ((!mac_addr) || (is_multicast_ether_addr(mac_addr))) {
		sta_link_id = 0;
	} else {
		for (i=0; i < MAX_SUPPORTED_STA; i++) {
			sta_list = (UMI_EVT_DATA_CONNECT_INFO *)
						(priv->sta_info + i);
			if (compare_ether_addr(mac_addr,
				&(sta_list->stationMacAddr[0]) ) == 0) {
				sta_link_id = sta_list->linkId;
				break;
			}
		}

		if ( i >= MAX_SUPPORTED_STA) {
			DEBUG(DBG_ERROR, "CIL_get_sta_linkid:STA not found"
				" in internal list \n");
		}
	}
	return sta_link_id;
}

static int _UMI_mc_list(struct CW1200_priv *priv)
{
	UMI_OID_802_11_MULTICAST_ADDR_FILTER *mcfilter;
	int retval;

	mcfilter = &(priv->ste_mc_filter);
	DEBUG(DBG_CIL, "Multicast filter: %p enable: %lu\n"
		, mcfilter,mcfilter->enable);
	retval = CIL_Set(priv,
		UMI_DEVICE_OID_802_11_MULTICAST_ADDR_FILTER,
		mcfilter, sizeof(UMI_OID_802_11_MULTICAST_ADDR_FILTER));
	if (retval) {
		DEBUG(DBG_ERROR, "CIL_Set for"
			"Multicast address list returned error\n");
	}
	return retval;
}


/**
* UMI_mc_list
*
* This func is called to set Multicast filter OID in the UMAC.
* returns the SUCCESS/FAILURE from UMAC
*
* @priv: pointer to the driver private structure.
*/
int UMI_mc_list(struct CW1200_priv *priv)
{
	int result;

	DOWN(&priv->cil_sem, "from eil");
	result = _UMI_mc_list(priv);
	UP(&priv->cil_sem, "from eil");

	return result;
}


/****************************************************************************
*				UMAC Callback funcs
*****************************************************************************/


/**
* UMI_CB_Indicate_Event
*
* This func is called by UMAC to indicate to the driver that UMAC has received
* a packet from the device.
*
* @UpperHandle: upper layer driver instance for UMIcallback.
* @Status: Further error code associated with the event.
* @Event: The event to be indicated.
* @EventDataLength: Length of the data associated with this event.
* @EventData: Data Associated with this event.
*/
void UMI_CB_Indicate_Event(UL_HANDLE UpperHandle,
			UMI_STATUS_CODE Status,
			UMI_EVENT Event,
			uint16_t EventDataLength,
			void *EventData)
{
	struct CW1200_priv *priv = (struct CW1200_priv *)UpperHandle;
	UMI_OID_802_11_ASSOCIATION_INFORMATION *assoc_info = NULL;
	uint8_t *ie_ptr = NULL;
	uint32_t rcode = 0;
	UMI_EVT_DATA_MIC_FAILURE *mic_failure_data = NULL;
	enum nl80211_key_type key_type;
	struct cfg80211_bss *bss = NULL;
	uint32_t lock_status = FALSE;
	UMI_EVT_DATA_CONNECT_INFO  *sta_info;
	UMI_EVT_DATA_CONNECT_INFO *sta_list = NULL;
	uint8_t *mac_addr = NULL;

	/* if not checked here causes crash in cfg80211_disconnected*/
	if (unlikely(atomic_read(&(priv->cw1200_unresponsive)) == TRUE)){
		DEBUG(DBG_ERROR, "%s CW1200 Unresponsive dumping "
							"indication received\n",__func__);
		return;
	}
	switch (Event) {
	case UMI_EVT_START_COMPLETED:
	{
		DEBUG(DBG_CIL, "CIL:START_COMPLETED received from UMAC \n");
		/*Completed the Init Process */
		EIL_Init_Complete(priv);
		break;
	}
	case UMI_EVT_SCAN_COMPLETED:
	{
		DEBUG(DBG_CIL, "CIL: EVENT RECEIVED SCAN COMPLETED\n");
		/* Stop SCAN watchdog */
		cancel_delayed_work(&priv->scan_timeout);

		mutex_lock(&priv->scan_timeout_lock);
		atomic_set(&priv->scan_timeout_count, 0);
		mutex_unlock(&priv->scan_timeout_lock);

		cw1200_handle_delayed_link_loss(priv);
		if (priv->request != NULL) {
			cfg80211_scan_done(priv->request , 0);
			priv->request = NULL;
		}
		if (priv->wait_event == UMI_EVT_SCAN_COMPLETED) {
			priv->wait_event = 0;
			UP(&priv->cil_sem, "UMI_EVT_SCAN_COMPLETED");
		}
		break;
	}
	case UMI_EVT_STOP_COMPLETED:
	{
		DEBUG(DBG_CIL, "CIL:STOP Completed Event Received \n");

		if (priv->wait_event) {
			priv->wait_event = 0;
			UP(&priv->cil_sem, "UMI_EVT_STOP_COMPLETED");
		}
		complete(&priv->cleanup_comp);
		break;
	}
	case UMI_EVT_CONNECTING:
	{
		DEBUG(DBG_CIL, "CIL :EVENT RECEIVE->Device Connecting\n");
		break;
	}
	case UMI_EVT_RECONNECTED:
		DEBUG(DBG_CIL , "CIL :EVENT RECEIVED RECONNECTED.... \n");
		/* Fall through is intentional */
	case UMI_EVT_CONNECTED:
	{
		DEBUG(DBG_CIL, "CIL :EVENT Connected/RECONNECTED....\n");
		/* Stop the CONNECTION Watchdog */
		sta_info = (UMI_EVT_DATA_CONNECT_INFO *)EventData;
		cancel_delayed_work(&priv->connect_result);
		cancel_delayed_work(&priv->bss_loss_work);
		cancel_delayed_work(&priv->connection_loss_work);

		/* Release spinlock as this context might sleep.
		Safe to release spinklock here	*/
		if (spin_is_locked(&priv->cil_lock))
			lock_status = TRUE;
		if ((NL80211_IFTYPE_AP == priv->device_mode) &&
			(( sta_info->linkId < 1 ) ||
			(sta_info->linkId >= MAX_SUPPORTED_STA))) {
			 DEBUG(DBG_CIL, "CIL: Invalid LinkId reported from UMAC"
					"linkId = <%d>\n",sta_info->linkId);
			WARN_ON(1);
			break;
		}
		if (lock_status)
			spin_unlock_bh(&(priv->cil_lock));
		/* Update linkID for the associated STA */
		if (NL80211_IFTYPE_AP == priv->device_mode) {
			priv->setget_linkid = sta_info->linkId;
		} else {
			priv->setget_linkid = 0;
		}
		assoc_info = (UMI_OID_802_11_ASSOCIATION_INFORMATION *)
		CIL_Get(priv, UMI_DEVICE_OID_802_11_ASSOCIATION_INFORMATION ,
			sizeof(UMI_OID_802_11_ASSOCIATION_INFORMATION));
		/* Reset Set/Get Link ID */
		priv->setget_linkid = 0;

		if (lock_status)
			spin_lock_bh(&(priv->cil_lock));
		if (NL80211_IFTYPE_AP == priv->device_mode) {
			/* Store the list of STAs connected in internal list */
			sta_list = (UMI_EVT_DATA_CONNECT_INFO *)priv->sta_info;
			memcpy(sta_list + sta_info->linkId, sta_info,
				sizeof(UMI_EVT_DATA_CONNECT_INFO));
			priv->sta_count++;
			DEBUG(DBG_CIL, "CIL :linkid = %d\n",sta_info->linkId);
			mac_addr = &sta_info->stationMacAddr[0];
		} else {
			mac_addr = &assoc_info->currentApAddress[0];
		}

		if (NULL != assoc_info) {
			DEBUG(DBG_CIL, "CIL:ASSOCIATED %pM \n", mac_addr);
			ie_ptr = (uint8_t *)((uint8_t *)
				(&assoc_info->variableIELenRsp) + 2);
			cfg80211_connect_result(priv->netdev,
					mac_addr,
					ie_ptr, assoc_info->variableIELenReq,
					(ie_ptr + assoc_info->variableIELenReq),
					assoc_info->variableIELenRsp,
					WLAN_STATUS_SUCCESS, GFP_ATOMIC);
			if (NL80211_IFTYPE_AP != priv->device_mode) {
				/* Get information on BEACON interval */
				bss = cfg80211_get_bss(
					priv->netdev->ieee80211_ptr->wiphy,
					NULL, &assoc_info->currentApAddress[0],
					NULL, 0, 0, 0);
				if (!bss) {
					DEBUG(DBG_ERROR, "CIL:Unable to get Beacon_Interval\n");
					priv->beacon_interval =
							DEFAULT_BEACON_INT;
				} else {
					DEBUG(DBG_CIL, "CIL:EVENT:BEACON INTERVAL [%d]\n",
						bss->beacon_interval);
					priv->beacon_interval = bss->beacon_interval;
					cfg80211_put_bss(bss);
				}
				if (priv->wait_event == UMI_EVT_CONNECTED) {
					priv->wait_event = 0;
					UP(&priv->cil_sem, "UMI_EVT_CONNECTED STA");
				}
			}
			priv->connection_status = CW1200_CONNECTED;
			kfree(assoc_info);
		} else {
			DEBUG(DBG_ERROR, "CIL:UMI_CB_Indicate_Event:error\n");

			if (priv->connection_status == CW1200_CONNECTING) {
			cfg80211_connect_result(priv->netdev,
					NULL, NULL, 0, NULL, 0,
					WLAN_STATUS_UNSPECIFIED_FAILURE,
					GFP_ATOMIC);
			}
			priv->connection_status = CW1200_DISCONNECTED;
			cfg80211_disconnected(priv->netdev,
						rcode, NULL, NULL, 0,
						GFP_ATOMIC);
			if (priv->wait_event == UMI_EVT_CONNECTED) {
				priv->wait_event = 0;
				UP(&priv->cil_sem, "UMI_CB_Indicate_Event:error");
			}
		}
		break;
	}
	case UMI_EVT_CONNECT_FAILED:
	{
		DEBUG(DBG_CIL, "CIL : EVENT RECEIVED -> Connect Failed...\n");
		/* Stop the CONNECTION Watchdog */
		cancel_delayed_work(&priv->connect_result);

		priv->connection_status = CW1200_DISCONNECTED;
		/* Abort Scanning if active */
		if (priv->request != NULL) {
			/* DO not cancel this work to keep recovery possible */
			//cancel_delayed_work(&priv->scan_timeout);
			mutex_lock(&priv->scan_timeout_lock);
			atomic_set(&priv->scan_timeout_count, 0);
			mutex_unlock(&priv->scan_timeout_lock);
			cfg80211_scan_done(priv->request , 0);
			priv->request = NULL;
		}
		cfg80211_connect_result(priv->netdev,
				NULL, NULL, 0, NULL, 0,
				WLAN_STATUS_UNSPECIFIED_FAILURE, GFP_ATOMIC);
		if (priv->wait_event == UMI_EVT_CONNECTED) {
			priv->wait_event = 0;
			UP(&priv->cil_sem, "UMI_EVT_CONNECT_FAILED");
		}
		break;
	}
	case UMI_EVT_BSS_LOST:
	{
		DEBUG(DBG_CIL, "CIL :EVENT RECEIVED BSS LOST...\n");
		cancel_delayed_work(&priv->bss_loss_work);
		cancel_delayed_work(&priv->connection_loss_work);

		if (priv->wait_event == UMI_EVT_CONNECTED) {
			DEBUG(DBG_CIL, "%s Ignore " \
				"BSS LOST Connection on-going\n", __func__);
		} else if (priv->wait_event == UMI_EVT_DISCONNECTED) {
			DEBUG(DBG_CIL, "%s Ignore " \
				"BSS LOST DisConnection on-going\n", __func__);
		} else if (NULL == priv->request) {
			priv->delayed_link_loss = FALSE;
			queue_delayed_work(priv->bss_loss_WQ,
					&priv->bss_loss_work, 0);
		} else {
			DEBUG(DBG_CIL, "%s Delay BSS LOST\n", __func__);
			priv->delayed_link_loss = TRUE;
			queue_delayed_work(priv->bss_loss_WQ,
					&priv->bss_loss_work, 10 * HZ);
		}
		break;
	}
	case UMI_EVT_BSS_REGAINED:
	{
		DEBUG(DBG_CIL, "CIL :EVENT RECEIVED BSS REGAINED...\n");
		priv->delayed_link_loss = FALSE;
		cancel_delayed_work(&priv->bss_loss_work);
		cancel_delayed_work(&priv->connection_loss_work);
		break;
	}
	/* Fall through intended */
	case UMI_EVT_DISCONNECTED:
	{
		UMI_EVT_DATA_DISCONNECT_INFO *disc =
			(UMI_EVT_DATA_DISCONNECT_INFO *) EventData;
		DEBUG(DBG_CIL, "CIL :EVENT RECEIVED DISCONNECTED...\n");

		/* If connection was in progress we should send
		 * the status first */
		if (priv->connection_status == CW1200_CONNECTING) {
			/* Stop the CONNECTION Watchdog */
			cancel_delayed_work(&priv->connect_result);
			cfg80211_connect_result(priv->netdev,
					NULL, NULL, 0, NULL, 0,
					WLAN_STATUS_UNSPECIFIED_FAILURE,
					GFP_ATOMIC);
		} else if(priv->connection_status == CW1200_DISCONNECTED){
			DEBUG(DBG_CIL, "CIL :IGNORE DISCONNECT...\n");
			break;
		}

		if (NL80211_IFTYPE_AP == priv->device_mode) {
			uint8 macAddr[UMI_MAC_ADDRESS_SIZE];

			sta_list = (UMI_EVT_DATA_CONNECT_INFO *)priv->sta_info
							+ disc->linkId;
			memcpy(macAddr, sta_list->stationMacAddr,
							UMI_MAC_ADDRESS_SIZE);
			DEBUG(DBG_CIL, "CIL: STA: %pM\n", macAddr);
			cfg80211_disconnected(priv->netdev, rcode,
				(u8 *)macAddr, NULL, 0, GFP_ATOMIC);
			memset(sta_list, 0, sizeof(UMI_EVT_DATA_CONNECT_INFO));
			priv->sta_count--;
			break;
		} else {
			priv->connection_status = CW1200_DISCONNECTED;
		        cancel_delayed_work(&priv->bss_loss_work);
		        cancel_delayed_work(&priv->connection_loss_work);
			priv->delayed_link_loss = FALSE;

			cfg80211_disconnected(priv->netdev,
					rcode, NULL, NULL, 0, GFP_ATOMIC);

			/* Abort Scanning if active */
			if (priv->request != NULL) {
				cancel_delayed_work_sync(&priv->scan_timeout);
				mutex_lock(&priv->scan_timeout_lock);
				atomic_set(&priv->scan_timeout_count, 0);
				mutex_unlock(&priv->scan_timeout_lock);
				cfg80211_scan_done(priv->request , 0);
				priv->request = NULL;
			}

			if (priv->wait_event == UMI_EVT_DISCONNECTED ||
			    priv->wait_event == UMI_EVT_SCAN_COMPLETED ||
			    priv->wait_event == UMI_EVT_CONNECTED) {
				priv->wait_event = 0;
				UP(&priv->cil_sem, "UMI_EVT_DISCONNECTED");
			}
		}
		break;
	}
	case UMI_EVT_MICFAILURE:
	{
		DEBUG(DBG_CIL, "CIL:MICFAILURE Event Received... \n");
		mic_failure_data =
			(UMI_EVT_DATA_MIC_FAILURE *)EventData;
		if (mic_failure_data->IsPairwiseKey) {
			DEBUG(DBG_CIL, "CIL: MICFAILURE for Pairwise Key \n");
			key_type = NL80211_KEYTYPE_PAIRWISE;
		} else {
			DEBUG(DBG_CIL, "CIL:MICFAILURE for Group Key \n");
			key_type = NL80211_KEYTYPE_GROUP ;
		}
		cfg80211_michael_mic_failure(priv->netdev ,
				&(mic_failure_data->PeerAddress[0]) ,
				key_type, mic_failure_data->KeyIndex ,
				&(mic_failure_data->Rsc[0]) , GFP_ATOMIC);
		break;
	}
	case UMI_EVT_RCPI_RSSI:
	{
		DEBUG(DBG_CIL, "CIL :EVENT RECEIVED RCPI_RSSI %d\n",
				*((char *)EventData));
		/* Device RSSI reporting is in unsigned char format
		 * so no need for converting before comparing with u_rssi/l_rssi
		 * as we stored converted u_rssi/l_rssi */
		if (*((unsigned char *)EventData) > priv->u_rssi) {
			cfg80211_cqm_rssi_notify(priv->netdev,
					NL80211_CQM_RSSI_THRESHOLD_EVENT_HIGH,
					GFP_ATOMIC);
		} else if (*((unsigned char *)EventData) < priv->l_rssi) {
			cfg80211_cqm_rssi_notify(priv->netdev,
					NL80211_CQM_RSSI_THRESHOLD_EVENT_LOW,
					GFP_ATOMIC);
		} else {
			DEBUG(DBG_CIL, "CIL:EVENT:Invalid RSSI Event received \n");
		}
		break;
	}
	case UMI_EVT_TX_FAILURE_THRESHOLD_EXCEEDED:
	{
		DEBUG(DBG_CIL, "CIL:UMI_EVT_TX_FAILURE_THRESHOLD_EXCEEDED"
				"received ... \n");
		cfg80211_cqm_tx_fail_notify(priv->netdev, GFP_ATOMIC);
		break;
	}
	case UMI_EVT_BSS_STARTED:
	{
		DEBUG(DBG_CIL, "CIL:UMI_EVT_BSS_STARTED Received \n");
		break;
	}
	case UMI_EVT_STOP_AP_COMPLETED:
	{
		DEBUG(DBG_CIL, "CIL:UMI_EVT_STOP_AP_COMPLETED Received \n");
		priv->soft_ap_started = false;
		UP(&priv->cil_sem, "UMI_EVT_STOP_AP_COMPLETED");
		break;
	}
	case UMI_EVT_RX_MGMT_FRAME:
	{
		bool cfg_retval;

		DEBUG(DBG_CIL, "CIL:UMI_EVT_RX_MGMT_FRAME Received \n");
		/* Check if REMAIN on CHANNEL is Active */
		if (priv->rem_on_chan) {
			cfg_retval = cfg80211_rx_mgmt(priv->netdev,
				ieee80211_channel_to_frequency(priv->rem_on_chan->hw_value, IEEE80211_BAND_2GHZ),
				(const uint8_t *)EventData,
				EventDataLength, GFP_ATOMIC);

			if (false == cfg_retval) {
				DEBUG(DBG_CIL, "CIL:P2P:cfg80211_rx_mgmt()"
					"returned error \n");
			}
		}
		break;
	}
	case UMI_EVT_RX_ACTION_FRAME:
	{
		bool cfg_retval;
		UMI_EVT_DATA_ACTION_FRAME *pActionFrame =
			(UMI_EVT_DATA_ACTION_FRAME *)EventData;

		DEBUG(DBG_CIL, "CIL:UMI_EVT_RX_ACTION_FRAME Received\n");

		cfg_retval = cfg80211_rx_mgmt(priv->netdev,
				ieee80211_channel_to_frequency(pActionFrame->Channel, IEEE80211_BAND_2GHZ),
				(const uint8_t *)pActionFrame->pFrame,
				pActionFrame->FrameLength, GFP_ATOMIC);

		if (false == cfg_retval) {
			DEBUG(DBG_CIL, "CIL:P2P:cfg80211_rx_mgmt()"
					"returned error \n");
		}
		break;
	}
	case UMI_EVT_CC_STATE_ABORTED:
	{
		DEBUG(DBG_CIL, "CIL:UMI_EVT_CC_STATE_ABORTED received\n");
		/*Unblock remain_on_channel API thread*/
		complete(&priv->roc_comp);
		cfg80211_remain_on_channel_expired(priv->netdev,
				priv->rem_on_chan_cookie, priv->rem_on_chan,
		                priv->rem_on_channel_type, GFP_ATOMIC);
		/* Restore */
		priv->rem_on_chan = NULL;
		priv->rem_on_chan_duration = 0;
		priv->rem_on_chan_cookie = 0;
		break;
	}
	case UMI_EVT_CC_STATE_SWITCHING:
	{
		DEBUG(DBG_CIL, "CIL:UMI_EVT_CC_STATE_SWITCHING received\n");
		/*Unblock remain_on_channel API thread*/
		complete(&priv->roc_comp);
		/* Inform WPA Supplicant that REMAIN on CHANNEL has started */
		cfg80211_ready_on_channel(priv->netdev,
		        priv->rem_on_chan_cookie,
		        priv->rem_on_chan,
		        priv->rem_on_channel_type,
		        priv->rem_on_chan_duration, GFP_ATOMIC);
		break;
	}
	case UMI_EVT_CC_STATE_COMPLETED:
	{
		DEBUG(DBG_CIL, "CIL:UMI_EVT_CC_STATE_COMPLETED received\n");
		cfg80211_remain_on_channel_expired(priv->netdev,
			priv->rem_on_chan_cookie,
			priv->rem_on_chan,
			priv->rem_on_channel_type, GFP_ATOMIC);
		/* Restore */
		priv->rem_on_chan = NULL;
		priv->rem_on_chan_duration = 0;
		priv->rem_on_chan_cookie = 0;
		break;
	}
	case UMI_EVT_DEVICE_UNRESPONSIVE:
	{
		atomic_xchg(&(priv->cw1200_unresponsive), TRUE);
		DEBUG(DBG_CIL, "CIL:EVENT RECEIVED UNRESPONSIVE\n");
		schedule_delayed_work(&sdio_cw_priv.unload_work, 0);
		break;
	}
	default:
		DEBUG(DBG_CIL, "CIL:Event not recognized [%d] \n", Event);
	}
}


/**
* UMI_CB_ScanInfo
*
* This func is called by UMAC to indicate BSS List Entry
*
* @UpperHandle: upper layer driver instance for UMIcallback.
* @cacheInfoLen:
* @pCacheInfo:
*/
void UMI_CB_ScanInfo(UL_HANDLE UpperHandle,
			uint32  cacheInfoLen,
			UMI_BSS_CACHE_INFO_IND *pCacheInfo)
{
	struct CW1200_priv *priv = (struct CW1200_priv *)UpperHandle;
	struct cfg80211_bss *res = NULL;
	struct ieee80211_channel *channel = NULL;

        /* ER459510: while driver was unloading, a beacon was received
	 * and while delivering this to cfg, it crashed.
	 */
        if (unlikely(atomic_read(&(priv->cw1200_unresponsive)) == TRUE)){
                printk(KERN_ERR, "%s Discard scan info received while driver is unloading\n",__func__);
                return;
        }
	channel = ieee80211_get_channel(priv->netdev->ieee80211_ptr->wiphy,
			ieee80211_channel_to_frequency(pCacheInfo->channelNumber, IEEE80211_BAND_2GHZ));

	if(priv->dualband && !channel) {
		channel =ieee80211_get_channel(priv->netdev->ieee80211_ptr->wiphy,
			ieee80211_channel_to_frequency(pCacheInfo->channelNumber, IEEE80211_BAND_5GHZ));
	}

	if (!channel) {
		DEBUG(DBG_ERROR, "UMI_CB_ScanInfo():Unable to get channel \n");
		return;
	}

	DEBUG(DBG_CIL, "%s %pM\n", __func__, pCacheInfo->bssId);
	res = cfg80211_inform_bss(priv->netdev->ieee80211_ptr->wiphy,
				channel, pCacheInfo->bssId,
				pCacheInfo->timeStamp,
				pCacheInfo->capability,
				pCacheInfo->beaconInterval,
				&pCacheInfo->ieElements[0],
				pCacheInfo->ieLength,
				pCacheInfo->rssi * 100, GFP_KERNEL);

	if (NULL == res)
		DEBUG(DBG_ERROR, "UMI_CB_ScanInfo():Error in creating BSS\n");

	cfg80211_put_bss(res);
}


/**
* UMI_CB_GetParameterComplete
*
* This func is called by UMAC to indicate that a pending GET operation
* initiated by the driver is complete.
*
* @UpperHandle: upper layer driver instance for UMIcallback.
* @Oid: OID number that associated with the parameter.
* @Status: Status code for the "get" operation.
* @Length: Number of bytes in the data.
* @pValue: The parameter data.
*/
void UMI_CB_GetParameterComplete(UL_HANDLE UpperHandle, uint16_t Oid,
				UMI_STATUS_CODE Status,
				uint16_t Length, void *pValue)
{
	struct CW1200_priv *priv = (struct CW1200_priv *)UpperHandle;

	DEBUG(DBG_CIL, "UMI_CB_GetParameterComplete() Called \n");

	if (UMI_STATUS_SUCCESS == Status) {
		/*Allocate a buffer to store the results */
		priv->get_buff_pointer = kmalloc(Length , GFP_KERNEL);
		if (NULL == priv->get_buff_pointer) {
			DEBUG(DBG_ERROR,
				"CIL - GetParameterComplete:Out of memory \n");
			atomic_xchg(&(priv->cw1200_unresponsive), TRUE);
			schedule_delayed_work(&sdio_cw_priv.unload_work, 0);
			return;
		}
		memcpy(priv->get_buff_pointer , pValue, Length);
		priv->get_buff_len = Length;
	} else {
		priv->get_buff_pointer = NULL;
		priv->get_buff_len = 0;
	}

	/*Wake up the CIL_Get() thread */
	priv->wait_flag_get = TRUE;
	wake_up_interruptible(&priv->cil_get_wait);
}


/**
* UMI_CB_SetParameterComplete
*
* This func is called by UMAC to indicate that a pending SET operation
* initiated by the driver is complete.
*
* @UpperHandle: upper layer driver instance for UMIcallback.
* @Oid: OID number that associated with the parameter
* @Status: Status code for the "set" operation.
*/
void UMI_CB_SetParameterComplete(UL_HANDLE UpperHandle,
			uint16_t Oid, UMI_STATUS_CODE  Status)
{
	struct CW1200_priv *priv = (struct CW1200_priv *)UpperHandle;

	DEBUG(DBG_CIL, "UMI_CB_SetParameterComplete() Called [%x] \n", Oid);

	if (UMI_STATUS_SUCCESS == Status) {
		/*Allocate a buffer to store the results */
		priv->set_status = SUCCESS;

#if 0 //From Prameela
		if (UMI_DEVICE_OID_802_11_CHANGE_CHANNEL == Oid) {
			/* Inform WPA Supplicant that REMAIN on CHANNEL has started */
			cfg80211_ready_on_channel(priv->netdev,
				priv->rem_on_chan_cookie,
				priv->rem_on_chan,
				priv->rem_on_channel_type,
				priv->rem_on_chan_duration, GFP_ATOMIC);
			/* Scheduled Delayed WQ for RESTORE CHAN */
			/* Due to FW issue starting Timer for a longer value */
			queue_delayed_work(priv->bss_loss_WQ,
					&priv->roc_work,
					msecs_to_jiffies(priv->rem_on_chan_duration));
		}
#endif
	} else {
		priv->set_status = ERROR;
		DEBUG(DBG_CIL, "UMI_CB_SetParameterComplete()"
				"return error [%x] \n", Status);
	}

	/*Wake up the CIL_Set() thread */
	if (atomic_sub_return(1, &priv->cil_set_cond) == 0) {
		priv->wait_flag_set = TRUE;
		wake_up_interruptible(&priv->cil_set_wait);
	}
}



/******************************************************************************
*				CFG80211 Ops
*******************************************************************************/

/**
* cw1200_change_virtual_intf
*
* change type/configuration of virtual interface, keep the struct
* wireless_dev's iftype updated.
*
* @wiphy: Pointer to wiphy device describing the WLAN interface.
* @dev: Network Device describing this driver.
* @type: the virtual interface type
* @params: virtual interface parameters.
*/
static int cw1200_change_virtual_intf(struct wiphy *wiphy,
			struct  net_device *dev,
			enum nl80211_iftype type, u32 *flags,
			struct vif_params *params)
{
	struct cfg_priv *c_priv = NULL;
	uint32_t mode = 0;
	int32_t retval = SUCCESS;
	uint32_t oprPowerMode = 0;
	int result = SUCCESS;

	DEBUG(DBG_CIL, "cw1200_change_virtual_intf called [%d] \n" , type);

	c_priv = wdev_priv(dev->ieee80211_ptr);

	DOWN(&c_priv->driver_priv->cil_sem, "");

	switch (type) {
	case NL80211_IFTYPE_ADHOC:
		dev->ieee80211_ptr->iftype = NL80211_IFTYPE_ADHOC;
		c_priv->driver_priv->device_mode = NL80211_IFTYPE_ADHOC;
		mode = UMI_802_IBSS;
		break;
	case NL80211_IFTYPE_STATION:
		dev->ieee80211_ptr->iftype = NL80211_IFTYPE_STATION;
		c_priv->driver_priv->device_mode = NL80211_IFTYPE_STATION;
		mode = UMI_802_INFRASTRUCTURE;
		break;
	case NL80211_IFTYPE_AP:
		dev->ieee80211_ptr->iftype = NL80211_IFTYPE_AP;
		c_priv->driver_priv->device_mode = NL80211_IFTYPE_AP;
		/* Set mode to AP*/
		mode = 2;
		break;
	case NL80211_IFTYPE_MONITOR:
		dev->ieee80211_ptr->iftype = NL80211_IFTYPE_MONITOR;
		c_priv->driver_priv->device_mode = NL80211_IFTYPE_MONITOR;
		/* Set mode to AP*/
		mode = 6;
		break;
	case NL80211_IFTYPE_P2P_CLIENT:
		dev->ieee80211_ptr->iftype = NL80211_IFTYPE_P2P_CLIENT;
		/* We will keep internal mode as STATION */
		c_priv->driver_priv->device_mode = NL80211_IFTYPE_STATION;
		break;
	case NL80211_IFTYPE_P2P_GO:
		dev->ieee80211_ptr->iftype = NL80211_IFTYPE_P2P_GO;
		/* We will keep internal mode as AP */
		c_priv->driver_priv->device_mode = NL80211_IFTYPE_AP;
		break;
	default:
		result = -EINVAL;
		goto fail;
	}

	if (mode < 2) {
		retval = CIL_Set(c_priv->driver_priv,
			UMI_DEVICE_OID_802_11_INFRASTRUCTURE_MODE,
			&mode, 4);
		if (retval) {
			result = -EIO;
			goto fail;
		}
	}
	/* Set operational power mode */
	oprPowerMode = c_priv->driver_priv->operational_power_mode;
	retval = CIL_Set(c_priv->driver_priv,
			UMI_DEVICE_OID_802_11_OPERATIONAL_POWER_MODE,
			&oprPowerMode, 4);
	if (retval) {
		DEBUG(DBG_CIL, "CIL_Set() for UMI_DEVICE"
			"_OID_802_11_OPERATIONAL_POWER_MODE returned error  \n");
	}

fail:
	UP(&c_priv->driver_priv->cil_sem, "");
	return result;
}


/**
* static int cw1200_scan
*
* Request to do a scan. If returning zero, the scan request is given the
* driver, and will be valid until passed to cfg80211_scan_done(). For scan
* results, call cfg80211_inform_bss(); you can call this outside the
* scan/scan_done bracket too.
*
* @wiphy: Pointer to wiphy device describing the WLAN interface.
* @ndev: Network Device describing this driver.
* @request: the corresponding scan request.
*/
static int cw1200_scan(struct wiphy *wiphy,
		struct net_device *ndev,
		struct cfg80211_scan_request *request)
{
	struct cfg_priv *c_priv = NULL;
	UMI_BSS_LIST_SCAN *scanreq = NULL;
	CW1200_STATUS_E retval = SUCCESS;
	struct CW1200_priv *priv = NULL;
	uint32_t i;
	UMI_CHANNELS *u_channel;
	struct cfg80211_ssid *cssids;
	UMI_UPDATE_VENDOR_IE vendor_ie;
	int result = SUCCESS;
	uint32_t num_2ghz_chan=0;
	uint32_t num_5ghz_chan=0;

	cssids = request->ssids;
	DEBUG(DBG_CIL, "cw1200_scan Called, request = [%p] \n", request);

	c_priv = wdev_priv(ndev->ieee80211_ptr);
	priv = netdev_priv(ndev);

	DOWN(&priv->cil_sem, "expected: SCAN_EVENT_COMPLETE");
	/* Check if change channel is in progress */
	if (priv->rem_on_chan) {
		DEBUG(DBG_CIL,
			"cw1200_rem_on_chan while change channel. Discard.\n");
		result = -EBUSY;
		goto fail;
	}
	if (priv->connection_status == CW1200_CONNECTING) {
		DEBUG(DBG_CIL, "cw1200_scan when connecting. Discard.\n");
		result = -EBUSY;
		goto fail;
	}

	priv->wait_event = UMI_EVT_SCAN_COMPLETED;
	scanreq = (UMI_BSS_LIST_SCAN *)kzalloc
			((request->n_channels)*sizeof(UMI_CHANNELS) +
			sizeof(UMI_BSS_LIST_SCAN), GFP_KERNEL);
	if (NULL == scanreq) {
		DEBUG(DBG_CIL, "CIL:cw1200_scan():Out of memory \n");
		result = -ENOMEM;
		goto fail;
	}
	/*Currently the UMAC has support for scanning only one SSID
	So issue a passive scan for all channels */
	if (request->n_ssids > 2)
		request->n_ssids = 2;
	if (priv->request == NULL) {
		/*Store the SCAN request param to be used in SCAN done*/
		priv->request = request;
	} else {
		DEBUG(DBG_CIL, "CIL:cw1200_scan():already in progress\n");
		result = -EAGAIN;
		goto fail;
	}

	u_channel = scanreq->Channels;

	for (i = 0; i < request->n_channels; i++) {
		if (!(request->channels[i]->flags & IEEE80211_CHAN_PASSIVE_SCAN)) {
			scanreq->NumOfProbeRequests = 4; /*ER437085*/
			u_channel[i].ChannelNum =
					request->channels[i]->hw_value;
			u_channel[i].MinChannelTime = 20; /*ER437085*/
			u_channel[i].TxPowerLevel =
					 request->channels[i]->max_power * 10;
			u_channel[i].MaxChannelTime = 40;
		} else {
			u_channel[i].ChannelNum =
					request->channels[i]->hw_value;
			u_channel[i].MinChannelTime = 40;
			u_channel[i].TxPowerLevel =
					request->channels[i]->max_power * 10;
			u_channel[i].MaxChannelTime = 120;
		}
		if (NL80211_BAND_2GHZ == request->channels[i]->band) {
			num_2ghz_chan++;
		}
		else if (c_priv->driver_priv->dualband) {
			num_5ghz_chan++;
		}
	}
	scanreq->NumberOfChannels_2_4Ghz = num_2ghz_chan;
	if (c_priv->driver_priv->dualband)
		scanreq->NumberOfChannels_5Ghz = num_5ghz_chan;

	if (cssids->ssid_len) {
		memcpy(scanreq->Ssids[0].ssid, cssids->ssid, cssids->ssid_len);
		scanreq->Ssids[0].ssidLength = cssids->ssid_len;
		scanreq->NumOfSSIDs = request->n_ssids;
	}
#ifdef ENABLE_P2P_WPS
	if (request->ie) {
		memset(&vendor_ie, 0, sizeof(UMI_UPDATE_VENDOR_IE));
		vendor_ie.pProbeReqIE = (uint8 *)request->ie;
		vendor_ie.probeReqIElength = request->ie_len;

		retval = CIL_Set(c_priv->driver_priv,
			UMI_DEVICE_OID_802_11_UPDATE_VENDOR_IE ,
			&vendor_ie, sizeof(UMI_UPDATE_VENDOR_IE));
		if (retval) {
			DEBUG(DBG_CIL, "cw1200_scan():CIL_Set"
				"for VENDOR IE returned error \n");
			priv->request = NULL;
			result = -EIO;
			goto fail;
		}
	}
#endif

	scanreq->RssiThreshold = (uint8_t)(c_priv->driver_priv->rssi_filter_threshold);

	/* Enable AUTO scan */
	if (c_priv->driver_priv->dualband)
		scanreq->flags = 1;
	else
		scanreq->flags = 2;

	/* Start a SCAN Watchdog */
	queue_delayed_work(priv->bss_loss_WQ,
			&priv->scan_timeout,
			msecs_to_jiffies(4 * 1000));
	retval = CIL_Set(c_priv->driver_priv,
			UMI_DEVICE_OID_802_11_BSSID_LIST_SCAN,
			scanreq, sizeof(UMI_BSS_LIST_SCAN) +
			(request->n_channels)*sizeof(UMI_CHANNELS));
	kfree(scanreq);
	scanreq = NULL;
	if (retval) {
		DEBUG(DBG_CIL, "cw1200_scan():returned error\n");
		priv->request = NULL;
		cw1200_handle_delayed_link_loss(priv);
		cancel_delayed_work(&priv->scan_timeout);
		result = -EAGAIN;
		goto fail;
	}
	return SUCCESS;

fail:
	if (scanreq)
		kfree(scanreq);
	priv->wait_event = 0;
	UP(&priv->cil_sem, NULL);
	DEBUG(DBG_CIL, "cw1200_scan Failed, request = [%p] \n", request);

	return result;
}


/**
* static int cw1200_join_ibss
*
* Join the specified IBSS (or create if necessary). Once done, call
* cfg80211_ibss_joined(), also call that function when changing
* BSSID due to a merge.
*
* @wiphy:Pointer to wiphy device describing the WLAN interface.
* @ndev: Network Device describing this driver.
*/
static int cw1200_join_ibss(struct wiphy *wiphy,
			struct net_device *ndev,
			struct cfg80211_ibss_params *params)
{
	UMI_802_11_SSID ssid_info;
	CW1200_STATUS_E retval = SUCCESS;
	struct cfg_priv *c_priv = NULL;
	int result = SUCCESS;

	DEBUG(DBG_CIL, "cw1200_join_ibss Called \n");

	ssid_info.ssidLength = params->ssid_len;
	memcpy(&ssid_info.ssid, params->ssid, params->ssid_len);
	c_priv = wdev_priv(ndev->ieee80211_ptr);

	DOWN(&c_priv->driver_priv->cil_sem, "");

	retval = CIL_Set(c_priv->driver_priv, UMI_DEVICE_OID_802_11_SSID,
			&ssid_info, sizeof(UMI_802_11_SSID));

	if (retval) {
		DEBUG(DBG_CIL, "cw1200_join_ibss():Error[%d]\n", retval);
		result = -EIO;
	}
	UP(&c_priv->driver_priv->cil_sem, "");

	return result;
}

/**
 * static int cw1200_add_key
 *
 * add a key with the given parameters.
 *
 * @wiphy: Pointer to wiphy device describing the WLAN interface.
 * @ndev: Network Device describing this driver.
 * @key_idx:
 * @mac_addr:NULL when adding a group key
 * @params: key information
 */
static int cw1200_add_key(struct wiphy *wiphy, struct net_device *ndev,
				u8 key_idx, bool pairwise, const u8 *mac_addr,
				struct key_params *params)
{
	struct cfg_priv *c_priv = NULL;
	CW1200_STATUS_E retval = SUCCESS;

	DEBUG(DBG_CIL, "cw1200_add_key Called for MAC addr %pM key_idx=%d\n",
			mac_addr, key_idx);

	c_priv = wdev_priv(ndev->ieee80211_ptr);
	DOWN(&c_priv->driver_priv->cil_sem, "");
	retval = __cw1200_add_key(wiphy, ndev, key_idx, pairwise,
						mac_addr, params);
	UP(&c_priv->driver_priv->cil_sem, "");

	return retval;
}

static int __cw1200_add_key(struct wiphy *wiphy, struct net_device *ndev,
				u8 key_idx, bool pairwise, const u8 *mac_addr,
				struct key_params *params)
{
	struct cfg_priv *c_priv = NULL;
	struct CW1200_priv *priv = NULL;
	UMI_OID_802_11_KEY s_key;
	uint32_t encr = 0;
	CW1200_STATUS_E retval = SUCCESS;
	int result = SUCCESS;

	DEBUG(DBG_CIL, "%s called\n", __func__);
	c_priv = wdev_priv(ndev->ieee80211_ptr);
	priv = c_priv->driver_priv;

	/* Moved this up as this will be used for key_index map */
	if (NL80211_IFTYPE_AP <= priv->device_mode) {
		priv->setget_linkid =
			CIL_get_sta_linkid(priv, mac_addr);
		if (priv->setget_linkid < 0) {
			result = -EIO;
			goto fail;
		}
	} else {
		/* Hardcoding right now */
		priv->setget_linkid = 1;
	}

	switch (params->cipher) {
	case WLAN_CIPHER_SUITE_WEP40:
	case WLAN_CIPHER_SUITE_WEP104:
		encr = 0x1;/*WEP Mode:Ref API doc */
		DEBUG(DBG_CIL, "WLAN_CIPHER_SUITE_WEP40/104:[%d]\n", key_idx);
		/* keys to be placed at 0,3,4,5, */
		if (0 == key_idx) {
			s_key.entryIndex = 0;
		} else {
			s_key.entryIndex = key_idx + 2;
		}
		s_key.keyType = WEP_GROUP;
		s_key.Key.WepGroupKey.keyId = key_idx ;
		memcpy(&(s_key.Key.WepGroupKey.keyData[0]),
				params->key, params->key_len);
		s_key.Key.WepGroupKey.keyLength = params->key_len;
		break;
	case WLAN_CIPHER_SUITE_TKIP:
		encr = 0x2;
		DEBUG(DBG_CIL, "WLAN_CIPHER_SUITE_TKIP\n");
		if (mac_addr) {
			DEBUG(DBG_CIL, "CIL:TKIP PAIRWISE\n");
			s_key.keyType = TKIP_PAIR;
			s_key.entryIndex = priv->setget_linkid;
			memcpy(&(s_key.Key.TkipPairwiseKey.peerAddress[0]),
					mac_addr, ETH_ALEN);
			memcpy(&(s_key.Key.TkipPairwiseKey.tkipKeyData[0]),
					params->key, params->key_len);
			memcpy(&(s_key.Key.TkipPairwiseKey.rxMicKey[0]),
					params->key + 24, 8);
			memcpy(&(s_key.Key.TkipPairwiseKey.txMicKey[0]),
					params->key + 16, 8);
		} else {
			DEBUG(DBG_CIL, "CIL:TKIP GROUPWISE \n");
			s_key.keyType = TKIP_GROUP;
			s_key.entryIndex = 0;
			s_key.Key.TkipGroupKey.keyId = key_idx;
			memcpy(&(s_key.Key.TkipGroupKey.tkipKeyData[0]),
					params->key, 16);
			memcpy(&(s_key.Key.TkipGroupKey.rxMicKey),
					params->key + 24, 8);
			memcpy(&(s_key.Key.TkipGroupKey.rxSequenceCounter),
					params->seq, params->seq_len);
		}
		break;

	case WLAN_CIPHER_SUITE_CCMP:
		encr = 0x4;
		DEBUG(DBG_CIL, "WLAN_CIPHER_SUITE_CCMP \n");
		if (mac_addr) {
			s_key.keyType = AES_PAIR;
			s_key.entryIndex = priv->setget_linkid;
			memcpy(&(s_key.Key.AesPairwiseKey.aesKeyData[0]),
				params->key, params->key_len);
			memcpy(&(s_key.Key.AesPairwiseKey.peerAddress[0]),
					mac_addr, ETH_ALEN);
		} else {
			s_key.keyType = AES_GROUP;
			s_key.entryIndex = 0;
			s_key.Key.AesGroupKey.keyId = key_idx ;
			memcpy(&(s_key.Key.AesGroupKey.aesKeyData[0]),
				params->key, params->key_len);
			memcpy(&(s_key.Key.AesGroupKey.rxSequenceCounter),
					params->seq , params->seq_len);
		}
		break;
#ifdef WAPI
	case WLAN_CIPHER_SUITE_SMS4:
		encr = 0x8;
		DEBUG(DBG_CIL, "WLAN_CIPHER_SUITE_SMS4/WAPI \n");
		/*WAPI is not supported for SoftAP mode */
		if (mac_addr) {
			s_key.keyType = WAPI_PAIR;
			/*hardcoding pairwise entry index to 1 as during rekeying, key is overwritten on the same index*/
			s_key.entryIndex = 1;
			WARN_ON(params->key_len != 16 + 16); // key + mic
			s_key.Key.WapiPairwiseKey.keyId = key_idx;
			memcpy(&(s_key.Key.WapiPairwiseKey.wapiKeyData[0]),
					params->key, 16);
			memcpy(&(s_key.Key.WapiPairwiseKey.peerAddress[0]),
					mac_addr, ETH_ALEN);
			/* Copy MIC key of length 16 bytes from 32th offset */
			memcpy(&(s_key.Key.WapiPairwiseKey.micKeyData[0]),
					params->key + 16, 16);

		} else {
			s_key.keyType = WAPI_GROUP;
			/*hardcoding group entry index to 0 as during rekeying, key is overwritten on the same index*/
			s_key.entryIndex = 0;
			s_key.Key.WapiGroupKey.keyId = key_idx;
			WARN_ON(params->key_len != 16 + 16); // key + mic
			memcpy(&(s_key.Key.WapiGroupKey.wapiKeyData[0]),
					params->key, 16);
			/* Copy MIC key of length 16 bytes from 32th offset */
			memcpy(&(s_key.Key.WapiGroupKey.micKeyData[0]),
					params->key + 16, 16);
		}
		break;
#endif
	default:
		result = -EINVAL;
		goto fail;
	}

	/* Add the key map index for this peer */
	if (NL80211_IFTYPE_AP <= priv->device_mode) {
		if (cw1200_add_key_map_index(priv, mac_addr,
							s_key.entryIndex)) {
			DEBUG(DBG_ERROR, "%s: Key map index not added\n",
								 __func__);
			result = -EIO;
			goto fail;
		}
	}

	retval = CIL_Set(priv,
			UMI_DEVICE_OID_802_11_ENCRYPTION_STATUS,
			&encr, sizeof(uint32_t));
	if (retval) {
		DEBUG(DBG_CIL, "cw1200_add_key():CIL_Set"
				"for ENCRYPTION status \n");
		result = -EIO;
		goto fail;
	}
	DEBUG(DBG_CIL, "cw1200_add_key : linkId = %d\n", priv->setget_linkid);
	/* STA mode reset linkId to 0*/
	if (NL80211_IFTYPE_AP > priv->device_mode) {
		priv->setget_linkid = 0;
	}
	retval = CIL_Set(priv,
			UMI_DEVICE_OID_802_11_ADD_KEY,
			&s_key, sizeof(UMI_OID_802_11_KEY));
	/* Reset */
	priv->setget_linkid = 0;
	if (retval) {
		DEBUG(DBG_CIL, "cw1200_add_key():returned error \n");
		result = -EIO;
		goto fail;
	}
	if (priv->auth_mode_default == UMI_SHARED_KEY) {
		default_key(ndev, key_idx);
		priv->auth_mode_default = 0;
	}

fail:
	return result;
}

/**
 * static int cw1200_del_key
 *
 * remove a key given
 *
 * @wiphy: Pointer to wiphy device describing the WLAN interface.
 * @ndev: Network Device describing this driver.
 * @key_idx: return -ENOENT if the key doesn't exist.
 * @mac_addr: NULL for a group key
 */
static int cw1200_del_key(struct wiphy *wiphy,
			struct net_device *ndev,
			u8 key_idx, bool pairwise, const u8 *mac_addr)
{
	struct cfg_priv *c_priv = NULL;
	struct CW1200_priv *priv = NULL;
	CW1200_STATUS_E retval = SUCCESS;
	UMI_OID_802_11_REMOVE_KEY removekey = {0};
	int result = SUCCESS;
	int8_t e_index;

	DEBUG(DBG_CIL, "cw1200_del_key Called for MAC addr %pM key_idx=%d\n",
			mac_addr, key_idx);
	c_priv = wdev_priv(ndev->ieee80211_ptr);
	priv = c_priv->driver_priv;

	/* if del_key is returned error, recovery fails to reboot, and just shutdown WLAN*/
	if (unlikely(atomic_read(&(priv->cw1200_unresponsive)) == TRUE))
		return SUCCESS;

	DOWN(&priv->cil_sem, "");

	/* Entry index will be 0 for group key and will be mapped
	 * to linkId in case of SoftAP
	 * In STATION mode peer key Entry Index will be 1.
	 */
	if (NL80211_IFTYPE_AP <= priv->device_mode) {
		/* Peer Key */
		e_index = cw1200_remove_key_map_index(priv, mac_addr);
		if (e_index < 0) {
			result = -EIO;
			goto fail;
		}
		removekey.EntryIndex = e_index;
	} else {
		/* Hardcoding right now */
		removekey.EntryIndex = key_idx;
	}
	if (CW1200_DISCONNECTED != priv->connection_status) {
		DEBUG(DBG_CIL, "cw1200_del_key Called for linkId = %d "
				"and Entry Index is = %d\n",
				priv->setget_linkid,removekey.EntryIndex);
		/* STA mode reset linkId to 0*/
		retval = CIL_Set(priv,
				UMI_DEVICE_OID_802_11_REMOVE_KEY,
				&removekey, sizeof(UMI_OID_802_11_REMOVE_KEY));
		priv->setget_linkid = 0;
		if (retval) {
			DEBUG(DBG_CIL, "cw1200_del_key():returned error \n");
			result = -EIO;
			goto fail;
		}
	}
fail:
	UP(&priv->cil_sem, "");
	return result;
}

/**
* static int cw1200_leave_ibss
*
* Leave the IBSS
*
* @wiphy: Pointer to wiphy device describing the WLAN interface.
* @ndev: Network Device describing this driver.
*/
static int cw1200_leave_ibss(struct wiphy *wiphy, struct net_device *ndev)
{
	struct cfg_priv *c_priv = NULL;
	CW1200_STATUS_E retval = SUCCESS;
	int result = SUCCESS;

	DEBUG(DBG_CIL, "cw1200_leave_ibss Called \n");

	c_priv = wdev_priv(ndev->ieee80211_ptr);

	DOWN(&c_priv->driver_priv->cil_sem, "");
	retval = CIL_Set(c_priv->driver_priv,
				UMI_DEVICE_OID_802_11_DISASSOCIATE, NULL, 0);
	if (retval) {
		DEBUG(DBG_CIL, "cw1200_leave_ibss():"
				"CIL_Set for DISASSOC returned error\n");
		result = -EIO;
	}
	UP(&c_priv->driver_priv->cil_sem, "");

	return result;
}


/**
* static int cw1200_connect
*
* Connect to the ESS with the specified parameters. When connected, call
* cfg80211_connect_result() with status code WLAN_STATUS_SUCCESS. If the
* connection fails for some reason, call cfg80211_connect_result() with
* the status from the AP.
*
* @wiphy: Pointer to wiphy device describing the WLAN interface.
* @dev: Network Device describing this driver.
* @sme: 802.11 Connection parameters
*/
static int  cw1200_connect(struct wiphy *wiphy, struct net_device *dev,
			struct cfg80211_connect_params *sme)
{
	uint8_t auth_mode = 0;
	struct cfg_priv *c_priv = NULL;
	CW1200_STATUS_E retval = SUCCESS;
	uint32_t PowerLevel = 0;
	UMI_802_11_SSID ssid_info;
	struct key_params key_param;
	struct cfg80211_crypto_settings *csettings = NULL;
	UMI_BSSID_OID bssid_oid;
	struct ieee80211_channel *channel = sme->channel;
	UMI_BLOCK_ACK_POLICY block_ack;
	uint32_t encr = 0;
	UMI_BEACON_LOST_COUNT beacon_threshold;
	UMI_UPDATE_VENDOR_IE vendor_ie;
	UMI_OID_802_11_SET_UAPSD uapsd_oid;
	int result = SUCCESS;
	int connect_result_queued = FALSE;
	int cancel_result = 0;

	DEBUG(DBG_CIL, "cw1200_connect Called\n");

	memset(&bssid_oid, 0, sizeof(UMI_BSSID_OID));
	csettings = &sme->crypto;
	if (channel)
		PowerLevel = channel->max_power * 10;
	/*
	* WPA_Vesrions
	* WPA_1   for wpa
	* WPA_2  for wpa2
	*/
	if (sme->auth_type == NL80211_AUTHTYPE_OPEN_SYSTEM &&
		 csettings->wpa_versions == NL80211_WPA_VERSION_1) {
		/*WPA */
		/*
		* #define WLAN_AKM_SUITE_8021X            0x000FAC01
		* #define WLAN_AKM_SUITE_PSK              0x000FAC02
		* #define WPA_AKM_SUITE_802_1X            0x0050F201
		* #define WPA_AKM_SUITE_PSK               0x0050F202
		*/

		if (csettings->akm_suites[0] == WLAN_AKM_SUITE_PSK) {
			DEBUG(DBG_CIL, "CIL: %s UMI_RSN_PSK \n", __func__);
			auth_mode = UMI_WPA_PSK;
		} else if (csettings->akm_suites[0] == WPA_AKM_SUITE_PSK) {
			DEBUG(DBG_CIL, "CIL: %s UMI_WPA_PSK \n", __func__);
			auth_mode = UMI_WPA_PSK;
		} else if (csettings->akm_suites[0] == WLAN_AKM_SUITE_8021X
				||
			csettings->akm_suites[0] == WPA_AKM_SUITE_802_1X) {
			DEBUG(DBG_CIL, "CIL: %s UMI_WPA_UNSPEC_TKIP \n", __func__);
			auth_mode = UMI_WPA_TKIP;
		} else {
			DEBUG(DBG_CIL, "CIL: %s unsupported akm suite %0X \n", __func__, csettings->akm_suites[0]);
			auth_mode = UMI_OPEN_SYSTEM;
		}

	} else if (sme->auth_type == NL80211_AUTHTYPE_OPEN_SYSTEM &&
			csettings->wpa_versions == NL80211_WPA_VERSION_2) {
		/*WPA2*/
		if (csettings->akm_suites[0] == WLAN_AKM_SUITE_PSK) {
			DEBUG(DBG_CIL, "CIL: %s UMI_RSN_PSK\n", __func__);
			auth_mode = UMI_WPA2_PSK;
		} else {
			auth_mode = UMI_WPA2_AES;
			DEBUG(DBG_CIL, "CIL: %s UMI_RSN_UNSPEC_AES \n", __func__);
		}
	}
#ifdef WAPI
	else if (csettings->wpa_versions == NL80211_WAPI_VERSION_1) {
			/* WAPI */
		auth_mode = UMI_WAPI_1 ;
		}
#endif
	else {
		/*If WPA/WPA2 is not set then it must be OPEN */
		switch (sme->auth_type) {
		case NL80211_AUTHTYPE_AUTOMATIC:
		case NL80211_AUTHTYPE_OPEN_SYSTEM:
			auth_mode = UMI_OPEN_SYSTEM;
			break;
		case NL80211_AUTHTYPE_SHARED_KEY:
			auth_mode = UMI_SHARED_KEY;
			break;
		default:
			return -EOPNOTSUPP;
		}
	}
	c_priv = wdev_priv(dev->ieee80211_ptr);

	cancel_delayed_work_sync(&c_priv->driver_priv->bss_loss_work);
	cancel_delayed_work_sync(&c_priv->driver_priv->connection_loss_work);
	DOWN(&c_priv->driver_priv->cil_sem, "expected: UMI_EVT_CONNECTED");
	c_priv->driver_priv->wait_event = UMI_EVT_CONNECTED;

	retval = CIL_Set(c_priv->driver_priv,
			UMI_DEVICE_OID_802_11_AUTHENTICATION_MODE,
			&auth_mode, sizeof(uint8_t));

	if (retval) {
		DEBUG(DBG_CIL, "cw1200_connect():CIL_Set"
				"for AUTH MODE returned error \n");
		result = -EIO;
		goto fail;
	}

	/* Enable all ENCR modes */
	encr = 0xF;
	retval = CIL_Set(c_priv->driver_priv,
			UMI_DEVICE_OID_802_11_ENCRYPTION_STATUS,
			&encr, sizeof(uint32_t));

	if (retval) {
		DEBUG(DBG_CIL, "cw1200_connect():CIL_Set"
				"for ENCRYPTION status \n");
		result = -EIO;
		goto fail;
	}

	retval = CIL_Set(c_priv->driver_priv,
			UMI_DEVICE_OID_802_11_TX_POWER_LEVEL,
			&PowerLevel, 4);
	if (retval) {
		DEBUG(DBG_CIL, "cw1200_connect():CIL_Set"
				"Power Level returned error \n");
		result = -EIO;
		goto fail;
	}

	/* Set UAPSD flags if enabled */
	if (sme->uapsd != -1) {
		memset(&uapsd_oid, 0, sizeof(UMI_OID_802_11_SET_UAPSD));
		DEBUG(DBG_CIL, "%s,UAPSD flags [%x] \n", __func__, sme->uapsd);

		uapsd_oid.uapsdFlags = sme->uapsd;

		if (c_priv->driver_priv->minAutoTriggerInterval) {
			/* Enable Pseudo UAPSD */
			uapsd_oid.uapsdFlags |= 0x10;
			uapsd_oid.minAutoTriggerInterval =
				c_priv->driver_priv->minAutoTriggerInterval ;
			uapsd_oid.maxAutoTriggerInterval =
				c_priv->driver_priv->maxAutoTriggerInterval;
			uapsd_oid.autoTriggerStep =
				c_priv->driver_priv->autoTriggerStep;
		}

		retval = CIL_Set(c_priv->driver_priv,
			UMI_DEVICE_OID_802_11_SET_UAPSD,
			&uapsd_oid, sizeof(UMI_OID_802_11_SET_UAPSD));
		if (retval) {
			DEBUG(DBG_CIL, "cw1200_connect():CIL_Set"
				"for UMI_OID_802_11_SET_UAPSD status \n");
			result = -EIO;
			goto fail;
		}
	}

	/*By default power save is enabled in the device.Disabling Power Save */
	/*Set the KEY Info */
	if (sme->key) {
		key_param.key = kmemdup(sme->key, sme->key_len, GFP_KERNEL);
		if (key_param.key == NULL) {
			result = -ENOMEM;
			goto fail;
		}

		c_priv->driver_priv->auth_mode_default = UMI_SHARED_KEY;
		key_param.key_len = sme->key_len;
		key_param.seq_len = 0;
		key_param.cipher = sme->crypto.ciphers_pairwise[0];
		retval = __cw1200_add_key(wiphy, dev, sme->key_idx, 1, sme->bssid,
					&key_param);

		kfree(key_param.key);
		if (retval < 0) {
			DEBUG(DBG_CIL, "CIL:Invalid key_params\n");
			result = retval;
			goto fail;
		}
	}
#ifdef ENABLE_P2P_WPS
	/* Update WPS IE to be sent in ASSOC request */
	if (sme->ie_len != 0) {
		DEBUG(DBG_CIL, "cw1200_connect():IE received \n");
		/* Check if WPS IE is received */
		if (0xdd == sme->ie[0] && (GET_BE32(&sme->ie[2]) == WPS_IE_VENDOR_TYPE ) ) {
			DEBUG(DBG_CIL, "cw1200_connect():WPS IE found\n");

			memset(&vendor_ie, 0, sizeof(UMI_UPDATE_VENDOR_IE));
			vendor_ie.assocReqIElength = sme->ie_len;
			vendor_ie.pAssocReqIE = sme->ie;
			retval = CIL_Set(c_priv->driver_priv,
				UMI_DEVICE_OID_802_11_UPDATE_VENDOR_IE ,
				&vendor_ie, sizeof(UMI_UPDATE_VENDOR_IE));

			if (retval) {
				DEBUG(DBG_CIL, "cw1200_connect():CIL_Set"
					"for VENDOR IE returned error \n");
				result = -EIO;
				goto fail;
			}
		} else {
			/* Check if P2P IE is received, we have to find it among
			 * other IEs. */
			u8 *pos = sme->ie;
			while (pos < &sme->ie[sme->ie_len-7]) {
				if (0xdd == *pos && (GET_BE32(&pos[2]) == P2P_IE_VENDOR_TYPE ) ) {
					DEBUG(DBG_CIL, "cw1200_connect(): P2P IE found\n");

					memset(&vendor_ie, 0, sizeof(UMI_UPDATE_VENDOR_IE));
					vendor_ie.probeReqIElength =
						vendor_ie.assocReqIElength = pos[1] + 2;
					vendor_ie.pProbeReqIE = vendor_ie.pAssocReqIE = pos;
					retval = CIL_Set(c_priv->driver_priv,
							UMI_DEVICE_OID_802_11_UPDATE_VENDOR_IE ,
							&vendor_ie, sizeof(UMI_UPDATE_VENDOR_IE));

					if (retval) {
						DEBUG(DBG_CIL, "cw1200_connect():CIL_Set"
								"for VENDOR IE returned error \n");
						result = -EIO;
						goto fail;
					}
				}
				pos += pos[1] + 2;
			}
		}
	}
#endif
	/* SET Block ACK policy for 11n AMPDU */
	block_ack.blockAckTxTidPolicy = c_priv->driver_priv->tx_block_ack;
	block_ack.blockAckRxTidPolicy = c_priv->driver_priv->rx_block_ack;
	retval = CIL_Set(c_priv->driver_priv,
			UMI_DEVICE_OID_802_11_BLOCK_ACK_POLICY ,
			&block_ack, sizeof(UMI_BLOCK_ACK_POLICY));
	if (retval) {
		DEBUG(DBG_CIL, "cw1200_connect():CIL_Set"
				"for BLOCK ACK returned error \n");
		result = -EIO;
		goto fail;
	}

	beacon_threshold.BeaconLostCount =
			c_priv->driver_priv->cqm_beacon_loss_count;

	retval = CIL_Set(c_priv->driver_priv,
			UMI_DEVICE_OID_802_11_SET_BEACON_LOST_COUNT,
			&beacon_threshold, sizeof(UMI_BEACON_LOST_COUNT));

	if (retval) {
		DEBUG(DBG_CIL, "cw1200_connect():"
				"SET_BEACON_LOSS value falied \n");
		retval = -EIO;
	}
	if (sme->bssid) {
		memcpy(&bssid_oid.Bssid[0], sme->bssid, ETH_ALEN);
		DEBUG(DBG_CIL, "%s to %pM\n", __func__, sme->bssid);
		/* If ROAM flag set and Same SSID send reassoc */
		if (sme->prev_bssid &&
		    memcmp(&c_priv->driver_priv->connected_ssid, sme->ssid, sme->ssid_len) == 0) {
			DEBUG(DBG_CIL, "cw1200_connect(): reassoc \n");
			bssid_oid.RoamingEnable = 1;
		} else {
			bssid_oid.RoamingEnable = 0;
		}

		c_priv->driver_priv->connection_status = CW1200_CONNECTING;
		queue_delayed_work(c_priv->driver_priv->bss_loss_WQ,
				   &c_priv->driver_priv->connect_result, CONNECT_TIMEOUT);
		connect_result_queued = TRUE;
		retval = CIL_Set(c_priv->driver_priv,
				UMI_DEVICE_OID_802_11_BSSID,
				&bssid_oid, sizeof(UMI_BSSID_OID));
		if (retval) {
			/* It was observed that sometimes UMAC is
			internally connected. Force disconnect.*/
			c_priv->driver_priv->connection_status = CW1200_DISCONNECTED;
			printk("bssid = %02x %02x %02x %02x %02x %02x\n",
				bssid_oid.Bssid[0],
				bssid_oid.Bssid[1],
				bssid_oid.Bssid[2],
				bssid_oid.Bssid[3],
				bssid_oid.Bssid[4],
				bssid_oid.Bssid[5]);
			retval = CIL_Set(c_priv->driver_priv,
				UMI_DEVICE_OID_802_11_DISASSOCIATE, NULL, 0);
			if (retval) {
				DEBUG(DBG_CIL, "cw1200_connect():CIL_Set"
						"for BSSID returned error\n");
			}
			result = -EIO;
			goto fail;
		}

	} else if (sme->ssid) {
		ssid_info.ssidLength = sme->ssid_len;
		memcpy(&ssid_info.ssid, sme->ssid, sme->ssid_len);

		c_priv->driver_priv->connection_status = CW1200_CONNECTING;
		queue_delayed_work(c_priv->driver_priv->bss_loss_WQ,
				   &c_priv->driver_priv->connect_result, CONNECT_TIMEOUT);
		connect_result_queued = TRUE;
		retval = CIL_Set(c_priv->driver_priv,
				UMI_DEVICE_OID_802_11_SSID,
				&ssid_info, sizeof(UMI_802_11_SSID));
		if (retval) {
			/* It was observed that sometimes UMAC is
			internally connected. Force disconnect.*/
			c_priv->driver_priv->connection_status = CW1200_DISCONNECTED;
			retval = CIL_Set(c_priv->driver_priv,
					UMI_DEVICE_OID_802_11_DISASSOCIATE,
					NULL, 0);
			if (retval) {
				DEBUG(DBG_CIL, "cw1200_connect():CIL_Set"
					"for SSID returned error\n");
			}
			result = -EIO;
			goto fail;
		}
	} else {
		result = -EINVAL;
		goto fail;
	}
	/* Store the SSID to check for Reassoc req */
	memcpy(&c_priv->driver_priv->connected_ssid, sme->ssid, sme->ssid_len);

	return SUCCESS;

fail:
	cancel_result = cancel_delayed_work_sync(&c_priv->driver_priv->connect_result);
	c_priv->driver_priv->wait_event = 0;
	if (connect_result_queued == TRUE && !cancel_result) {
		return result;
	}
	UP(&c_priv->driver_priv->cil_sem, "FAIL to connect");
	return result;
}

#if 0
static int cw1200_assoc(struct wiphy *wiphy, struct net_device *dev,
			struct cfg80211_assoc_request *req)
{
	DEBUG(DBG_CIL, "cw1200_assoc, Called \n");
	return SUCCESS;
}
#endif


/**
* static int cw1200_disconnect
*
* Disconnect from the BSS/ESS.
*
* @wiphy: Pointer to wiphy device describing the WLAN interface.
* @dev: Network Device describing this driver.
*/
static int cw1200_disconnect(struct wiphy *wiphy, struct net_device *dev,
				u16 reason_code)
{
	struct cfg_priv *c_priv = NULL;
	CW1200_STATUS_E retval = SUCCESS;

	DEBUG(DBG_CIL, "cw1200_disconnect, Called:Reason Code: [%d]\n",
		reason_code);

	netdev_priv(dev);
	c_priv = wdev_priv(dev->ieee80211_ptr);
	DOWN(&c_priv->driver_priv->cil_sem, "expected: UMI_EVT_DISCONNECTED");
	c_priv->driver_priv->wait_event = UMI_EVT_DISCONNECTED;

	if (CW1200_DISCONNECTED != c_priv->driver_priv->connection_status) {
		retval = CIL_Set(c_priv->driver_priv,
				UMI_DEVICE_OID_802_11_DISASSOCIATE,
				NULL, 0);
		if (retval) {
			DEBUG(DBG_CIL, "cw1200_disconnect():CIL_Set"
					"for DISCONN returned error\n");
		/* TODO: Check for return code and report error only if fatal.*/
		/* return -EIO;*/
			c_priv->driver_priv->wait_event = 0;
			UP(&c_priv->driver_priv->cil_sem, "UMI_EVT_DISCONNECTED");
		}
	} else {
		c_priv->driver_priv->wait_event = 0;
		UP(&c_priv->driver_priv->cil_sem, "UMI_EVT_DISCONNECTED");
	}
	return SUCCESS;
}


void default_key(struct net_device *ndev,
			u8 key_index)
{
	UMI_OID_802_11_WEP_DEFAULT_KEY_ID wep_default;
	CW1200_STATUS_E retval = SUCCESS;
	struct cfg_priv *c_priv = NULL;
	int result = SUCCESS;

	c_priv = wdev_priv(ndev->ieee80211_ptr);

	DEBUG(DBG_CIL, "cw1200_set_default_key() called:Key_index:[%d]\n",
			key_index);

	wep_default.wepDefaultKeyId = key_index;

	retval = CIL_Set(c_priv->driver_priv,
			UMI_DEVICE_OID_802_11_WEP_DEFAULT_KEY_ID, &wep_default,
			sizeof(UMI_OID_802_11_WEP_DEFAULT_KEY_ID));

	if (retval) {
		DEBUG(DBG_CIL, "cw1200_set_default_key():CIL_Set"
				"for set default key returned error\n");
		result = -EIO;
	}
}


/**
 * static int cw1200_set_default_key
 *
 * set the default key on an interface.
 *
 * @wiphy: Pointer to wiphy device describing the WLAN interface.
 * @ndev: Network Device describing this driver.
 */
static int cw1200_set_default_key(struct wiphy *wiphy,
				struct net_device *ndev,
				u8 key_index, bool unicast, bool multicast)
{
	UMI_OID_802_11_WEP_DEFAULT_KEY_ID wep_default;
	CW1200_STATUS_E retval = SUCCESS;
	struct cfg_priv *c_priv = NULL;
	int result = SUCCESS;

	c_priv = wdev_priv(ndev->ieee80211_ptr);

	DOWN(&c_priv->driver_priv->cil_sem, "");
	DEBUG(DBG_CIL, "cw1200_set_default_key() called:Key_index:[%d]\n",
			key_index);

	wep_default.wepDefaultKeyId = key_index;

	retval = CIL_Set(c_priv->driver_priv, UMI_DEVICE_OID_802_11_WEP_DEFAULT_KEY_ID,
		&wep_default, sizeof(UMI_OID_802_11_WEP_DEFAULT_KEY_ID));

	if (retval) {
		DEBUG(DBG_CIL, "cw1200_set_default_key():CIL_Set"
				"for set default key returned error\n");
		result = -EIO;
	}
	UP(&c_priv->driver_priv->cil_sem, "");

	return result;
}

static int cw1200_set_power_mgmt(struct wiphy *wiphy,
		struct net_device *dev,
		bool enabled, int timeout)
{
	struct cfg_priv *c_priv = NULL;
	CW1200_STATUS_E retval = SUCCESS;
	UMI_POWER_MODE power_mode;
	uint32_t oprPowerMode = 0;
	struct CW1200_priv *priv = NULL;

	DEBUG(DBG_CIL, "cw1200_set_power_mgmt()"
			"called:Power Mode :[%d]\n", enabled);

	c_priv = wdev_priv(dev->ieee80211_ptr);
	priv = c_priv->driver_priv;
	oprPowerMode = priv->operational_power_mode;

	if (NL80211_IFTYPE_AP == priv->device_mode) {
		/* We are in AP mode.So reject the call */
		DEBUG(DBG_CIL, "cw1200_set_power_mgmt()"
			"called in AP mode\n");
		return SUCCESS;
	}

	/* Set operational power mode */
	if (2 == wlan_psm_flag) {
		oprPowerMode = priv->operational_power_mode;
		DEBUG(DBG_CIL,"operational_power_mode Enabled at run time\n");
	} else if (3 == wlan_psm_flag) {
		oprPowerMode = WLAN_ACTIVE_MODE;
		DEBUG(DBG_CIL,"operational_power_mode Disabled at run time\n");
	}

	retval = CIL_Set(priv,
			UMI_DEVICE_OID_802_11_OPERATIONAL_POWER_MODE,
			&oprPowerMode, 4);
	if (retval) {
		DEBUG(DBG_CIL, "CIL_Set() for UMI_DEVICE"
			"_OID_802_11_OPERATIONAL_POWER_MODE returned error\n");
	}

	memset(&power_mode, 0, sizeof(UMI_POWER_MODE));

	if (2 == wlan_psm_flag) {
		enabled = 1;
		 DEBUG(DBG_CIL,"POWER_MODE enabled at run time\n");
	} else if (3 == wlan_psm_flag) {
		enabled = 0;
		 DEBUG(DBG_CIL,"POWER_MODE Disbled at run time\n");
	}

	if (enabled) {
		/*  Only use fastps if p2p is disabled */
		if (priv->p2p_enabled == false) {
			/* Enable POWER mode with FAST PSM */
			power_mode.PmMode = 0x81;
			/* Multiply by 2 as UMAC unit is 500us */
			power_mode.FastPsmIdlePeriod = timeout * 2;
		} else
			power_mode.PmMode = 1;
	}

	retval = CIL_Set(priv,
			UMI_DEVICE_OID_802_11_POWER_MODE,
			&power_mode, sizeof(UMI_POWER_MODE));
	if (retval) {
		DEBUG(DBG_CIL, "cw1200_set_power_mgmt():CIL_Set"
				"for POWER MODE returned error \n");
		return -EIO;
	}
	return SUCCESS;
}


static int tunnel_set_ps(struct wiphy *wiphy,
		struct ste_msg_ps_conf *tdata , int len)
{
	int retval = 0;
	struct CW1200_priv *priv = NULL;
	struct cfg_priv *c_priv = NULL;
	UMI_SET_ARP_IP_ADDR_FILTER set_arp_filter;

	c_priv = wiphy_priv(wiphy);
	priv = c_priv->driver_priv;
	priv->dynamic_ps_timeout = tdata->dynamic_ps_timeout;

	DEBUG(DBG_CIL, "tunnel_set_ps called, "
			"tdata len= %d\n", len);

	if (tdata->flags & STE_PS_CONF_IEEE_PS) {

		DEBUG(DBG_CIL, "Enabling powersave \n");
		retval = cw1200_set_power_mgmt(wiphy, priv->netdev, 1,
						tdata->dynamic_ps_timeout);
		if (retval != SUCCESS)
			DEBUG(DBG_CIL, "error in enabling powersave");

	} else if (tdata->flags & STE_PS_CONF_IDLE) {

		retval = cw1200_set_power_mgmt(wiphy, priv->netdev, 1,
						tdata->dynamic_ps_timeout);
		DEBUG(DBG_CIL, "Enabling powersave \n");
		if (retval != SUCCESS)
			DEBUG(DBG_CIL, "error in enabling powersave");

	} else {

		retval = cw1200_set_power_mgmt(wiphy, priv->netdev, 0, 0);
		DEBUG(DBG_CIL, "Disabling powersave \n");
		if (retval != SUCCESS)
			DEBUG(DBG_CIL, "error in disabling powersave");
	}

	/*ARP FILTER Setting*/
	DEBUG(DBG_CIL, "%s: Enabling-Disabling ARP filtering\n", __func__);
	set_arp_filter.filterMode =
		(uint32)tdata->arp_filtering_list.enable;

	/*Enable ARP Offload*/
	if(priv->arp_offload)
		set_arp_filter.filterMode = set_arp_filter.filterMode | 0x02;

	if(set_arp_filter.filterMode) {
		memcpy(set_arp_filter.ipV4Addr
				, tdata->arp_filtering_list.ip_addr
				, IP_ALEN);
		DEBUG(DBG_CIL, "%s: Setting IPADDR %d.%d.%d.%d \n"
				, __func__,
				set_arp_filter.ipV4Addr[0],
				set_arp_filter.ipV4Addr[1],
				set_arp_filter.ipV4Addr[2],
				set_arp_filter.ipV4Addr[3]);
	}
	retval = CIL_Set(priv
			, UMI_DEVICE_OID_802_11_SET_ARP_IP_ADDR_FILTER
			, &set_arp_filter, sizeof(UMI_SET_ARP_IP_ADDR_FILTER));
	if (retval != SUCCESS)
		DEBUG(DBG_ERROR, "%s: error in Setting ARP filter",__func__);

	if(tdata->flags & STE_PS_CONF_MC_FILTERING) {
		if(priv->ste_mc_filter.enable == 0) {
			DEBUG(DBG_CIL, "Enabling MC filtering \n");
			priv->ste_mc_filter.enable = 1;
			if(priv->ste_mc_filter.numOfAddresses)
				retval = _UMI_mc_list(priv);
		}
	} else {
		if(priv->ste_mc_filter.enable == 1){
			DEBUG(DBG_CIL, "Disabling MC filtering \n");
			priv->ste_mc_filter.enable = 0;
			retval = _UMI_mc_list(priv);
		}
	}
	return retval;
}


/**
* static int cw1200_testmode_cmd
*
* run a test mode command
*
* @wiphy: Pointer to wiphy device describing the WLAN interface.
*/
static int cw1200_testmode_cmd(struct wiphy *wiphy, void *data, int len)
{
	struct nlattr *type_p = nla_find(data, len, STE_TM_MSG_ID);
	struct nlattr *data_p = nla_find(data, len, STE_TM_MSG_DATA);
	CW1200_STATUS_E retval = SUCCESS;
	struct cfg_priv *c_priv = NULL;
	struct sk_buff *skb = NULL;
	uint8_t *noa_buf = NULL;
	int32_t noa_len = 0;

	if (!type_p || !data_p)
		return -EINVAL;

	DEBUG(DBG_CIL, "\t%s-type:%i\n", __func__, nla_get_u32(type_p));
	c_priv = wiphy_priv(wiphy);
	DOWN(&c_priv->driver_priv->cil_sem, "");

	switch (nla_get_u32(type_p)) {
	case  STE_MSG_PS_CONF_SET: {
		tunnel_set_ps(wiphy, nla_data(data_p), nla_len(data_p));
		break;
	}
	case  STE_MSG_11N_CONF_SET: {
		retval = tunnel_set_11n_conf(wiphy, nla_data(data_p),
						nla_len(data_p));
		if (retval) {
			retval = -EIO;
		}
		break;
	}
	case STE_MSG_SET_P2P:
		retval = tunnel_set_p2p_state(wiphy, nla_data(data_p),
						nla_len(data_p));
		if (retval) {
			retval = -EIO;
		}
		break;
	case STE_MSG_SET_UAPSD:
		retval = tunnel_set_uapsd(wiphy, nla_data(data_p),
						nla_len(data_p));
		if (retval) {
			retval = -EIO;
		}
		break;
	case STE_MSG_SET_P2P_POWER_SAVE:
		retval = tunnel_set_p2p_power_save(wiphy, nla_data(data_p),
							 nla_len(data_p),CIL_P2P_OPS);
		if (retval) {
			retval = -EIO;
		}
		break;
	case STE_MSG_SET_NOA:
		retval = tunnel_set_p2p_power_save(wiphy, nla_data(data_p),
							 nla_len(data_p),CIL_P2P_NOA);

		if (retval) {
			retval = -EIO;
		}
		break;
	case STE_MSG_GET_NOA:
		noa_buf = CIL_get_noa_attr(c_priv->driver_priv, &noa_len);
		if (noa_buf) {
			skb = cfg80211_testmode_alloc_reply_skb(wiphy,
				nla_total_size(noa_len));
			if (!skb) {
				kfree(noa_buf);
				retval = -ENOMEM;
				break;
			}
			NLA_PUT(skb, STE_TM_MSG_DATA, noa_len, noa_buf);
			retval = cfg80211_testmode_reply(skb);
			kfree(noa_buf);
		} else
			retval = -EIO;
		break;
	case STE_MSG_PROBE_RES:
		retval = tunnel_set_p2p_probe_res_ie(wiphy, nla_data(data_p),
						nla_len(data_p));
		if (retval) {
			retval = -EIO;
		}
		break;
	case STE_MSG_TX_STAT_CHK:
		retval = tunnel_start_packet_error_rate(wiphy, nla_data(data_p),
						nla_len(data_p));
		if(retval) {
			retval = -EIO;
		}
		break;
	case STE_MSG_P2P_DISCONNECT_CLIENT:
		DEBUG(DBG_CIL, "P2P Disconnect Client\n");
		retval = tunnel_p2p_disconnect_client(wiphy, nla_data(data_p),
				nla_len(data_p));
		if (retval) {
			DEBUG(DBG_CIL, "Failed disconnecting client\n");
			retval = -EIO;
		}
		break;
	case STE_MSG_SET_RSSI_FILTER:
		retval = tunnel_set_rssi_filter(wiphy, nla_data(data_p),
						nla_len(data_p));
		if(retval) {
			retval = -EIO;
		}
		break;
	default:
		break;
	}
	UP(&c_priv->driver_priv->cil_sem, "");
	return retval;

nla_put_failure:
	UP(&c_priv->driver_priv->cil_sem, "");
	kfree_skb(skb);
	kfree(noa_buf);
	return -ENOBUFS;
}


/**
* static int tunnel_set_11n_conf
*
* 11n configuration of the device
*
* @wiphy: Pointer to wiphy device describing the WLAN interface.
* @tdata: Pointer to buffer of type - struct ste_msg_11n_conf *
* @len: Length of buffer pointed by tdata
*/
static int tunnel_set_11n_conf(struct wiphy *wiphy,
		struct ste_msg_11n_conf *tdata , int len)
{
	struct cfg_priv *c_priv = NULL;
	CW1200_STATUS_E retval = SUCCESS;
	uint32_t amsdu_size;

	DEBUG(DBG_CIL, "tunnel_set_11n_conf()"
			"called:11n_conf FLAGS :[%d] \n", tdata->flags);

	c_priv = wiphy_priv(wiphy);

	/* Check if TID policy flag is set and pass the settings to UMAC */
	if (tdata->flags & STE_11N_CONF_USE_TID_POLICY) {
		/* Update BLOCK ACK TID policy which will be
		* set in cw1200_connect() function */
		DEBUG(DBG_CIL, "tunnel_set_11n_conf():CONF_TID_POLICY \n");
		c_priv->driver_priv->tx_block_ack = tdata->block_ack_tx_tid_policy;
		c_priv->driver_priv->rx_block_ack = tdata->block_ack_rx_tid_policy;
	}

	if (tdata->flags & STE_11N_CONF_USE_AMSDU_DEFAULT_SIZE) {

		if (tdata->amsdu_default_size == 4096)
			amsdu_size = 0;
		else
			amsdu_size = 1;

		DEBUG(DBG_CIL, "tunnel_set_11n_conf():AMSDU_DEFAULT_SIZE \n");
		retval = CIL_Set(c_priv->driver_priv,
				UMI_DEVICE_OID_802_11_MAX_RX_AMSDU_SIZE ,
				&amsdu_size, sizeof(uint32_t));
		if (retval) {
			DEBUG(DBG_CIL, "tunnel_set_11n_conf():CIL_Set"
					"for AMSDU size returned error \n");
			return ERROR;
		}
	}
	return SUCCESS;
}

static int tunnel_set_p2p_state(struct wiphy *wiphy,
		struct ste_msg_set_p2p *tdata , int len)
{
	struct cfg_priv *c_priv = NULL;
	CW1200_STATUS_E retval = SUCCESS;
	UMI_OID_USE_P2P p2p_oid;
	struct CW1200_priv *priv = NULL;

	DEBUG(DBG_CIL, "tunnel_set_p2p_state()"
		"called: :[%d]\n", tdata->enable);

	c_priv = wiphy_priv(wiphy);
	priv = c_priv->driver_priv;

	if (tdata->enable) {
		p2p_oid.useP2P = 1;
		priv->p2p_enabled = true;
	} else {
		p2p_oid.useP2P = 0;
		priv->p2p_enabled = false;
	}

	retval = CIL_Set(priv,
		UMI_DEVICE_OID_802_11_USE_P2P,
		&p2p_oid, sizeof(UMI_OID_USE_P2P));

	if (retval)
		retval = -EIO;

	return retval;
}


static int tunnel_set_uapsd(struct wiphy *wiphy,
		struct ste_msg_set_uapsd *tdata , int len)
{
	struct cfg_priv *c_priv = NULL;
	CW1200_STATUS_E retval = SUCCESS;
	UMI_OID_802_11_SET_UAPSD uapsd_oid;

	c_priv = wiphy_priv(wiphy);
	DEBUG(DBG_CIL, "%s,Tunnel UAPSD flags [%x] \n", __func__, tdata->flags);

	if (tdata->flags != -1) {
		memset(&uapsd_oid, 0, sizeof(UMI_OID_802_11_SET_UAPSD));

		uapsd_oid.uapsdFlags = tdata->flags;
		/* Enable Pseudo UAPSD */
		if (tdata->minAutoTriggerInterval) {
			uapsd_oid.uapsdFlags |= 0x10;
			c_priv->driver_priv->minAutoTriggerInterval =
				uapsd_oid.minAutoTriggerInterval = tdata->minAutoTriggerInterval ;
			c_priv->driver_priv->maxAutoTriggerInterval =
				uapsd_oid.maxAutoTriggerInterval = tdata->maxAutoTriggerInterval;
			c_priv->driver_priv->autoTriggerStep =
				uapsd_oid.autoTriggerStep = tdata->autoTriggerStep;
		} else {
			c_priv->driver_priv->minAutoTriggerInterval = 0;
		}
		retval = CIL_Set(c_priv->driver_priv,
			UMI_DEVICE_OID_802_11_SET_UAPSD,
			&uapsd_oid, sizeof(UMI_OID_802_11_SET_UAPSD));

		if (retval) {
			DEBUG(DBG_CIL, "tunnel_set_uapsd():CIL_Set"
				"for UMI_OID_802_11_SET_UAPSD status \n");
			retval = -EIO;
		}
	}

	return retval;
}

static int tunnel_set_p2p_power_save(struct wiphy *wiphy,
		void *tdata , int len, int p2p_ps_type)
{
	struct cfg_priv *c_priv = NULL;
	CW1200_STATUS_E retval = SUCCESS;
	UMI_OID_802_11_P2P_PS_MODE p2p_ps;
	struct ste_msg_set_p2p_power_save * p2p_ops;
	struct ste_msg_set_noa * p2p_noa;

	DEBUG(DBG_CIL, "tunnel_set_p2p_power_save() called:\n");

	c_priv = wiphy_priv(wiphy);

	memset(&p2p_ps, 0, sizeof(UMI_OID_802_11_P2P_PS_MODE));

	if (CIL_P2P_OPS == p2p_ps_type) {
		/* Parameters updated by supplicant for P2P Ops*/
		p2p_ops = ( struct ste_msg_set_p2p_power_save *)tdata;
		DEBUG(DBG_CIL, "OPS:%d, CTWindow:%d\n", p2p_ops->opp_ps,
				p2p_ops->ctwindow);

		p2p_ps.oppPsCTWindow = c_priv->driver_priv->ctwindow;

		/* Set OppPs and CTWindow attributes.
		 * According spec the CTWindow shall
		 * be non-zero when the OppPS bit is set to 1.
		 */
		if (p2p_ops->ctwindow > 0) {
			p2p_ps.oppPsCTWindow &= P2P_OPS_ENABLE;
			p2p_ps.oppPsCTWindow |= p2p_ops->ctwindow & 0x7F;
		} else if (p2p_ops->ctwindow == 0)
			p2p_ps.oppPsCTWindow = 0;

		if (STE_P2P_OPP_PS_ENABLE == p2p_ops->opp_ps &&
			(p2p_ps.oppPsCTWindow & 0x7F)) {
			p2p_ps.oppPsCTWindow |= P2P_OPS_ENABLE;
		} else if (STE_P2P_OPP_PS_DISABLE == p2p_ops->opp_ps)
			p2p_ps.oppPsCTWindow = p2p_ps.oppPsCTWindow & 0x7F;

		c_priv->driver_priv->ctwindow = p2p_ps.oppPsCTWindow;

		/* Set NOA attributes */
		p2p_ps.count = c_priv->driver_priv->count;
		p2p_ps.startTime = c_priv->driver_priv->startTime;
		p2p_ps.duration = c_priv->driver_priv->duration;

	} else {
		/* Params updated for NOA */
		p2p_noa = (struct ste_msg_set_noa *)tdata;
		p2p_ps.count = c_priv->driver_priv->count = p2p_noa->count;
		p2p_ps.startTime = c_priv->driver_priv->startTime =
							p2p_noa->start;
		/* Supplicant send duration in ms and UMAC expects in us */
		p2p_ps.duration = c_priv->driver_priv->duration =
						p2p_noa->duration * 1000;
		p2p_ps.oppPsCTWindow = c_priv->driver_priv->ctwindow;
	}

	/* Currently keeping it one */
	p2p_ps.DtimCount = 1;
	p2p_ps.interval = p2p_ps.duration * 2;

	retval = CIL_Set(c_priv->driver_priv,
			UMI_DEVICE_OID_802_11_P2P_PS_MODE,
			&p2p_ps, sizeof(UMI_OID_802_11_P2P_PS_MODE));

	if (retval)
		retval = -EIO;

	return retval;
}

static int tunnel_set_p2p_probe_res_ie(struct wiphy *wiphy,
		void *tdata , int len)
{
	struct cfg_priv *c_priv = NULL;
	CW1200_STATUS_E retval = SUCCESS;
	UMI_UPDATE_VENDOR_IE vendor_ie;

	DEBUG(DBG_CIL, "tunnel_set_p2p_probe_res_ie() called:\n");

	c_priv = wiphy_priv(wiphy);

	if ((NULL == tdata) || (len == 0)) {
		DEBUG(DBG_CIL, "tunnel_set_p2p_probe_res_ie():Wrong Params \n");
		return -EIO;
	}
#ifdef ENABLE_P2P_WPS
	memset(&vendor_ie, 0, sizeof(UMI_UPDATE_VENDOR_IE));
	vendor_ie.pProbeRespIE = tdata;
	vendor_ie.probeRespIElength = len;

	retval = CIL_Set(c_priv->driver_priv,
			UMI_DEVICE_OID_802_11_UPDATE_VENDOR_IE ,
			&vendor_ie, sizeof(UMI_UPDATE_VENDOR_IE));
	if (retval) {
		DEBUG(DBG_CIL, "tunnel_set_p2p_probe_res_ie:CIL_Set"
				"for VENDOR IE returned error \n");
		return -EIO;
	}
#endif
	return SUCCESS;
}

static int tunnel_start_packet_error_rate(struct wiphy *wiphy,
		void *tdata , int len)
{
	struct cfg_priv *c_priv = NULL;
	struct CW1200_priv *priv = NULL;
	struct ste_msg_tx_stat_chk * stat_chk;
	uint32_t retval = SUCCESS;
	UMI_OID_802_11_TX_STATS *stat = NULL;

	DEBUG(DBG_CIL, "CIL: %s called: \n",__func__);

	c_priv = wiphy_priv(wiphy);
	priv = c_priv->driver_priv;

	stat_chk = (struct ste_msg_tx_stat_chk *)tdata;

	priv->per_start = stat_chk->enable;
	priv->per_interval = stat_chk->tx_stat_chk_prd;
	priv->per_tx_threshold = stat_chk->tx_stat_chk_num;
	priv->per_failure_threshold = stat_chk->tx_fail_rate;

	if(priv->per_start) {
		stat = (UMI_OID_802_11_TX_STATS *)CIL_Get(priv,
			UMI_DEVICE_OID_802_11_GET_TX_STATS,
			sizeof(UMI_OID_802_11_TX_STATS));
		queue_delayed_work(priv->bss_loss_WQ,
				   &priv->per_work, (priv->per_interval) * HZ);
	}
	else {
		cancel_delayed_work_sync(&priv->per_work);
	}
	kfree(stat);
	return retval;
}

/**
 * static int tunnel_p2p_disconnect_client
 *
 * CFG80211 callback function for disconnecting p2p client
 *
 * @wiphy: Pointer to wiphy device describing the WLAN interface.
 * @tdata: Pointer to buffer of type - struct ste_msg_11n_conf *
 * @len: Length of buffer pointed by tdata
 */
static int tunnel_p2p_disconnect_client(struct wiphy *wiphy,
		void *tdata , int len)
{
	struct CW1200_priv *priv =
		((struct cfg_priv *)wiphy_priv(wiphy))->driver_priv;
	CW1200_STATUS_E retval = SUCCESS;
	u8 cl_mac_addr[ETH_ALEN];

	DEBUG(DBG_CIL, "%s: called\n", __func__);

	if ((NULL == tdata) || (len != ETH_ALEN)) {
		DEBUG(DBG_CIL, "%s: Wrong Params\n", __func__);
		return -EIO;
	}

	memcpy(cl_mac_addr, tdata, ETH_ALEN);
	DEBUG(DBG_CIL, "Client MAC address : %02x %02x %02x %02x %02x %02x\n",
			cl_mac_addr[0], cl_mac_addr[1], cl_mac_addr[2],
			cl_mac_addr[3], cl_mac_addr[4], cl_mac_addr[5]);

	priv->setget_linkid = CIL_get_sta_linkid(priv, cl_mac_addr);
	if (priv->setget_linkid < 0) {
		DEBUG(DBG_CIL, "Invalid MAC address\n");
		return -EIO;
	}
	DEBUG(DBG_CIL, "link id: %d\n", priv->setget_linkid);

	retval = CIL_Set(priv, UMI_DEVICE_OID_802_11_DISASSOCIATE, NULL, 0);
	if (retval) {
		DEBUG(DBG_CIL, "P2P discoonect returned error\n");
		return -EIO;
	}
	return SUCCESS;
}


static int tunnel_set_rssi_filter( struct wiphy *wiphy,
		void *tdata, int len)
{
	struct cfg_priv *c_priv = NULL;
	CW1200_STATUS_E retval = SUCCESS;
	struct CW1200_priv *priv = NULL;
	struct ste_msg_set_rssi_filter *rssi_filter;

	if (NULL == tdata) {
		DEBUG(DBG_CIL, "%s:Wrong Params\n", __func__);
		return -EIO;
	}

	rssi_filter = (struct ste_msg_set_rssi_filter *)tdata;

	DEBUG(DBG_CIL, "%s threshold:[%d]\n", __func__, rssi_filter->threshold);

	c_priv = wiphy_priv(wiphy);
	priv = c_priv->driver_priv;

	priv->rssi_filter_threshold = rssi_filter->threshold;

	return retval;
}

/**
 * static int cw1200_rssi_config
 *
 * CFG80211 callback function for RSSIThreshold
 *
 * @wiphy: Pointer to wiphy device describing the WLAN interface.
 * @ndev: Network Device describing this driver.
 * @rssi_thold: RSSI Threshold value.
 * @rssi_hyst: RSSI hysteris value
 */
static int cw1200_rssi_config(struct wiphy *wiphy,
			struct net_device *ndev,
			s32 rssi_thold,
			u32 rssi_hyst)
{

	UMI_RCPI_RSSI_THRESHOLD rssi_oid;
	uint32_t retval = SUCCESS;
	struct cfg_priv *c_priv = NULL;

	DEBUG(DBG_CIL, "%s Called:RSSI[%d],RSSI_HYST[%u]\n", __func__ ,
						rssi_thold , rssi_hyst);
	if (rssi_thold == 0) {
		rssi_oid.RssiRcpiMode = RSSI_DISABLED ;
		/*set rest params to avoid any garbage/previous value set*/
		rssi_oid.LowerThreshold = 0;
		rssi_oid.UpperThreshold = 0;
		rssi_oid.RollingAverageCount = 0;
		DEBUG(DBG_CIL, "Called:RSSI DISABLE");
	} else {
		rssi_oid.RssiRcpiMode = RSSI_RCPI_MODE ;
		/* set the other params if rssi_thold !=0
		Enabling RSSI Threshold for both upper
		and lower threshold detection */
		rssi_oid.LowerThreshold = (uint8_t)(rssi_thold);
		rssi_oid.UpperThreshold = (uint8_t)(rssi_thold + rssi_hyst*2);
		/* Number of samples to use for average */
		rssi_oid.RollingAverageCount = 5;
	}
	c_priv = wdev_priv(ndev->ieee80211_ptr);

	DOWN(&c_priv->driver_priv->cil_sem, "");
	retval = CIL_Set(c_priv->driver_priv,
			UMI_DEVICE_OID_802_11_SET_RCPI_RSSI_THRESHOLD,
			&rssi_oid, sizeof(UMI_RCPI_RSSI_THRESHOLD));

	if (retval) {
		DEBUG(DBG_CIL, "cw1200_rssi_config():SET_RSSI value falied \n");
		retval = -EIO;
	} else {
		c_priv->driver_priv->u_rssi = (uint8_t)rssi_oid.UpperThreshold;
		c_priv->driver_priv->l_rssi = (uint8_t)rssi_oid.LowerThreshold;
	}

	UP(&c_priv->driver_priv->cil_sem, "");

	return retval;
}


/**
* static int cw1200_beacon_miss_config
*
* CFG80211 callback function for Beacon miss configuration
*
* @wiphy: Pointer to wiphy device describing the WLAN interface.
* @beacon_thold: Beacon miss Threshold value.
*
*/
static int cw1200_beacon_miss_config(struct wiphy *wiphy,
				struct net_device *dev,
				u32 beacon_thold)
{
	uint32_t retval = SUCCESS;
	struct cfg_priv *c_priv = NULL;

	DEBUG(DBG_CIL, "%s Called:Beacon_Thold[%d] \n", __func__ ,
							beacon_thold);

	c_priv = wdev_priv(dev->ieee80211_ptr);

	DOWN(&c_priv->driver_priv->cil_sem, "");
	/* Store in priv.Will be set in cw1200_connect() function */
	if (beacon_thold)
	{
		c_priv->driver_priv->cqm_beacon_loss_count = beacon_thold;
		c_priv->driver_priv->cqm_beacon_loss_enabled = TRUE;
	}
	else
	{
		/* Don't notify beacon_loss but connection_loss should
		 * work fine */
		c_priv->driver_priv->cqm_beacon_loss_count = CQM_BEACON_LOSS;
		c_priv->driver_priv->cqm_beacon_loss_enabled = FALSE;
	}
	UP(&c_priv->driver_priv->cil_sem, "");

	return retval;
}


/**
* cw1200_tx_fail_config
*
* CFG80211 callback function for TX fails configuration.
*
* @wiphy: Pointer to wiphy device describing the WLAN interface.
* @beacon_thold: TX Fails Threshold value.
*
*/
static int cw1200_tx_fail_config(struct wiphy *wiphy,
				struct net_device *dev,
				u32 tx_thold)
{
	uint32_t retval = SUCCESS;
	struct cfg_priv *c_priv = NULL;
	UMI_TX_FAILURE_THRESHOLD_COUNT tx_threshold;

	DEBUG(DBG_CIL, "%s Called:TX_Thold[%d] \n", __func__ ,
							tx_thold);

	c_priv = wdev_priv(dev->ieee80211_ptr);
	tx_threshold.TransmitFailureThresholdCount = tx_thold;

	DOWN(&c_priv->driver_priv->cil_sem, "");
	retval = CIL_Set(c_priv->driver_priv,
			UMI_DEVICE_OID_802_11_TX_FAILURE_THRESHOLD_COUNT,
			&tx_threshold, sizeof(UMI_TX_FAILURE_THRESHOLD_COUNT));

	if (retval) {
		DEBUG(DBG_CIL, "cw1200_tx_fail_config():"
				"SET_TX_HOLD value falied\n");
		retval = -EIO;
	}
	UP(&c_priv->driver_priv->cil_sem, "");

	return retval;
}

void cw1200_handle_delayed_link_loss(struct CW1200_priv *priv)
{
	if (priv->delayed_link_loss) {
		priv->delayed_link_loss = FALSE;
		/* Restart beacon loss timer and requeue BSS loss work. */
		DEBUG(DBG_CIL, "[CQM] Requeue BSS loss in %d beacons.\n",
				priv->cqm_beacon_loss_count);
		cancel_delayed_work_sync(&priv->bss_loss_work);
		queue_delayed_work(priv->bss_loss_WQ, &priv->bss_loss_work,
			msecs_to_jiffies(priv->cqm_beacon_loss_count * priv->beacon_interval));
	}
}


void cw1200_bss_loss_work(struct work_struct *work)
{
	struct CW1200_priv *priv =
		container_of(work, struct CW1200_priv, bss_loss_work.work);
	int timeout; /* in beacons */

	DEBUG(DBG_CIL, "%s called\n", __func__);

	timeout = priv->cqm_link_loss_count - priv->cqm_beacon_loss_count;

	if (priv->cqm_beacon_loss_enabled) {
		DEBUG(DBG_CIL, "[CQM] Beacon loss.\n");
		if (timeout <= 0)
			timeout = 0;
		cfg80211_cqm_beacon_miss_notify(priv->netdev, GFP_KERNEL);
	} else {
		timeout = 0;
	}
	cancel_delayed_work_sync(&priv->connection_loss_work);
	queue_delayed_work(priv->bss_loss_WQ,
		&priv->connection_loss_work,
		msecs_to_jiffies(timeout * priv->beacon_interval)) ;
}


static void cw1200_unresponsive_driver(struct work_struct *work)
{
	static uint8_t unload_schedule;

	DEBUG(DBG_CIL,"%s called\n", __func__);

	if (unload_schedule) {
		DEBUG(DBG_CIL, "Already scheduled\n");
		return;
	}

	unload_schedule = TRUE;
	mutex_lock(&sdio_cw_priv.eil_sdwn_lock);
	if (sdio_cw_priv.driver_priv) {
		(sdio_cw_priv.driver_priv)->set_status = ERROR;
		(sdio_cw_priv.driver_priv)->wait_flag_set = TRUE;
		wake_up_interruptible(&(sdio_cw_priv.driver_priv)->cil_set_wait);
		DEBUG(DBG_CIL,"%s called wakeup for any blocked wait\n",
				__func__);

		if(down_trylock(&(sdio_cw_priv.driver_priv)->cil_sem)){
			DEBUG(DBG_CIL,"%s:pending semaphore for waitevent:%d\n",
				__func__, (sdio_cw_priv.driver_priv)->wait_event);
			(sdio_cw_priv.driver_priv)->wait_event = 0;
		}
		UP(&(sdio_cw_priv.driver_priv)->cil_sem,
				"cleaning any pending semaphore\n");

		/* Inform user space that we cannot continue */
		cfg80211_driver_hang_notify(sdio_cw_priv.driver_priv->netdev,
					GFP_KERNEL);
		/* Calling EIL_Shutdown from here saves 40 msec
		EIL_Shutdown(sdio_cw_priv.driver_priv);*/
	}
	mutex_unlock(&sdio_cw_priv.eil_sdwn_lock);
}

void cw1200_connection_loss_work(struct work_struct *work)
{
	uint32_t retval = SUCCESS;
	struct CW1200_priv *priv = container_of(work, struct CW1200_priv,
			connection_loss_work.work);

	DEBUG(DBG_CIL, "[CQM] Reporting connection loss.\n");

	cfg80211_disconnected(priv->netdev,
		WLAN_REASON_DISASSOC_STA_HAS_LEFT, NULL, NULL, 0, GFP_KERNEL);
	if (priv->wait_event == UMI_EVT_CONNECTED) {
		DEBUG(DBG_CIL, "%s wait_event = UMI_EVT_CONNECTED\n", __func__);
	}
	DOWN(&priv->cil_sem, "");/*warning admissible*/
	retval = CIL_Set(priv, UMI_DEVICE_OID_802_11_DISASSOCIATE, NULL, 0);
	if (retval) {
			DEBUG(DBG_CIL, "cw1200_leave_ibss():"
				"CIL_Set for DISASSOC returned error\n");
	}
	UP(&priv->cil_sem, "");
}

void cw1200_check_connect(struct work_struct *work)
{
	struct CW1200_priv *priv =
		container_of(work, struct CW1200_priv, connect_result.work);

	DEBUG(DBG_CIL, "CIL:Timeout waiting for connect result\n");

	cancel_delayed_work_sync(&priv->bss_loss_work);
	cancel_delayed_work_sync(&priv->connection_loss_work);
	priv->delayed_link_loss = FALSE;

	if (priv->connection_status == CW1200_CONNECTING) {
		priv->connection_status = CW1200_DISCONNECTED;
		cfg80211_connect_result(priv->netdev,
			NULL, NULL, 0, NULL, 0,
			WLAN_STATUS_UNSPECIFIED_FAILURE, GFP_KERNEL);
	} else if (priv->connection_status == CW1200_CONNECTED) {
		/* TODO Check if required */
		cfg80211_disconnected(priv->netdev,
			0, NULL, NULL, 0, GFP_KERNEL);
	}

	if (priv->wait_event == UMI_EVT_CONNECTED) {
		priv->wait_event = 0;
		UP(&priv->cil_sem, "connection timeout");
	}
}

#if 0 //From Prameela
/**
* cw1200_roc_timeout
*
*This function is called on REMAIN on CHANNEL timeout.
*
* @work: Pointer to work_struct
*/
void cw1200_roc_timeout(struct work_struct *work)
{
	struct CW1200_priv *priv =
		container_of(work, struct CW1200_priv, roc_work.work);

	uint32_t retval = SUCCESS;

	DEBUG(DBG_CIL, "CIL:P2P:REMAIN on CHANNEL Timeout\n");
	DOWN(&priv->cil_sem, "");/*warning admissible*/

	/* TBD : To check if Action Frame exchange on ON */
	retval = CIL_Set(priv,
			UMI_DEVICE_OID_802_11_RESTORE_CHANNEL,
			0,0);
	if (retval) {
		DEBUG(DBG_CIL, "%s:P2P:RESTORE_CHANNEL falied \n", __func__);
	}

	cfg80211_remain_on_channel_expired(priv->netdev, priv->rem_on_chan_cookie,
				priv->rem_on_chan,
				priv->rem_on_channel_type,
				GFP_KERNEL);
	/* Restore */
	priv->rem_on_chan = NULL;
	priv->rem_on_chan_duration = 0;
	priv->rem_on_chan_cookie = 0;

	UP(&priv->cil_sem, "");
}
#endif

void cw1200_per_start(struct work_struct *work)
{
	UMI_OID_802_11_TX_STATS *stat = NULL;
	uint32 packet_err_rate;
	struct CW1200_priv *priv =
		container_of(work, struct CW1200_priv, per_work.work);

	DEBUG(DBG_CIL, "CIL: %s called\n", __func__);
	DOWN(&priv->cil_sem, "");/*warning admissible*/
	stat = (UMI_OID_802_11_TX_STATS *)CIL_Get(priv,
					UMI_DEVICE_OID_802_11_GET_TX_STATS,
					sizeof(UMI_OID_802_11_TX_STATS));
	UP(&priv->cil_sem, "");

	if(stat != NULL) {
		if(stat->TxCount > priv->per_tx_threshold) {
			packet_err_rate = (stat->TxRetriedCount * 100) / stat->TxCount;

			if(packet_err_rate > (priv->per_failure_threshold * 10)) {
				DEBUG(DBG_CIL, "CIL: %s Send Event to Supplicant\n", __func__);
				cfg80211_tx_stat_error(priv->netdev, GFP_ATOMIC);
			}
		}
	}
	queue_delayed_work(priv->bss_loss_WQ, &priv->per_work,
				(priv->per_interval) * HZ);
	kfree(stat);
}

/**
* cw1200_set_pmksa
*
* Cache a PMKID for a BSSID. This is mostly useful for fullmac devices running
* firmwares capable of generating the (re)association RSN IE. It allows for
* faster roaming between WPA2 BSSIDs.
*
* @wiphy: Pointer to wiphy device describing the WLAN interface.
* @netdev: Network Device describing this driver.
* @pmksa: Pointer to PMK Security Association
*/
static int cw1200_set_pmksa(struct wiphy *wiphy, struct net_device *netdev,
				struct cfg80211_pmksa *pmksa)
{
	uint32_t retval = SUCCESS;
	struct cfg_priv *c_priv = NULL;
	struct CW1200_priv *priv = NULL;
	uint32_t bss_count=0, i = 0;
	int result = SUCCESS;

	DEBUG(DBG_CIL, "%s ,called for BSSID: %pM \n", __func__, pmksa->bssid);

	c_priv = wdev_priv(netdev->ieee80211_ptr);
	priv = c_priv->driver_priv;

	DOWN(&priv->cil_sem, "");
	/* Check if UMAC can accept any more */
	bss_count = priv->pmk_list.bssidInfoCount;
	if (bss_count >= UMI_MAX_BSSID_INFO_ENTRIES) {
		DEBUG(DBG_CIL, "CIL: Exceeded max entries allowed by UMAC \n");
		result = -EIO;
		goto fail;
	}
	for (i=0; i < bss_count; i++) {
		if (memcmp(pmksa->bssid, &priv->pmk_list.bssidInfo[i].bssid , ETH_ALEN) == 0)
			break;
	}

	memcpy(&priv->pmk_list.bssidInfo[i].bssid,
		pmksa->bssid, ETH_ALEN);

	memcpy(&priv->pmk_list.bssidInfo[i].pmkidBkid,
		pmksa->pmkid, UMI_PMKID_BKID_SIZE);

	if ( i >= bss_count)
		priv->pmk_list.bssidInfoCount++;

	retval = CIL_Set(priv,
			UMI_DEVICE_OID_802_11_SET_PMKID_BKID,
			&priv->pmk_list, sizeof(UMI_BSSID_PMKID_BKID));

	if (retval) {
		DEBUG(DBG_CIL, "%s:SET_PMKID value falied \n", __func__);
		priv->pmk_list.bssidInfoCount--;
		result = -EIO;
		goto fail;
	}

fail:
	UP(&priv->cil_sem, "");
	return retval;

}


/**
* static int cw1200_del_pmksa
*
* Delete a cached PMKID.
*
* @wiphy: Pointer to wiphy device describing the WLAN interface.
* @netdev: Network Device describing this driver.
* @pmksa: Pointer to PMK Security Association
*/
static int cw1200_del_pmksa(struct wiphy *wiphy, struct net_device *netdev,
				struct cfg80211_pmksa *pmksa)
{
	struct cfg_priv *c_priv = NULL;
	struct CW1200_priv *priv = NULL;
	uint32_t i=0,j=0;
	uint32_t bss_count=0;
	int result = SUCCESS;

	DEBUG(DBG_CIL, "%s ,called for BSSID: %pM \n", __func__, pmksa->bssid);

	c_priv = wdev_priv(netdev->ieee80211_ptr);
	priv = c_priv->driver_priv;

	DOWN(&priv->cil_sem, "");
	bss_count = priv->pmk_list.bssidInfoCount;
	if (bss_count == 0) {
		DEBUG(DBG_CIL, "%s ,Invalid value of BSSCount\n", __func__);
		result = -EIO;
		goto fail;
	}

	for (i=0; i < bss_count; i++) {
		if (memcmp(pmksa->bssid, &priv->pmk_list.bssidInfo[i].bssid , ETH_ALEN) == 0)
		break;
	}

	for (j=i; j < (bss_count -1); j++) {
		memcpy(&priv->pmk_list.bssidInfo[j], &priv->pmk_list.bssidInfo[j+1],
			sizeof(UMI_BSSID_INFO));
	}

	priv->pmk_list.bssidInfoCount--;
	memset(&priv->pmk_list.bssidInfo[bss_count], 0,  sizeof(UMI_BSSID_INFO));

fail:
	UP(&priv->cil_sem, "");
	return result;
}


/**
* int cw1200_add_virtual_intf
*
* create a new virtual interface with the given name, must set the struct
* wireless_dev's iftype. Beware: You must create the new netdev in the wiphy's
* network namespace!
*
* @wiphy: Pointer to wiphy device describing the WLAN interface.
* @name: Name of the interface
* @type: the virtual interface type
* @params: virtual interface parameters.
*/
struct net_device * cw1200_add_virtual_intf(struct wiphy *wiphy, char *name,
				enum nl80211_iftype type, u32 *flags,
				struct vif_params *params)
{
	struct net_device *netdev = NULL;
	struct cfg_priv *c_priv = NULL;
	struct cfg_priv *c_new_priv = NULL;
	struct wireless_dev *wdev = NULL;
        printk(KERN_ERR"######### enter cw1200_add_virtual_intf");
	DEBUG(DBG_CIL, "%s,IF-NAME [%s]\n", __func__, name);
	c_priv = wiphy_priv(wiphy);

	DOWN(&c_priv->driver_priv->cil_sem, "");/*warning admissible*/
	netdev = alloc_etherdev(sizeof(struct cfg_priv));
	if (!netdev)
		goto fail;

	c_new_priv = netdev_priv(netdev);
	c_new_priv->driver_priv = c_priv->driver_priv;

	strcpy(netdev->name, name);
	memcpy(netdev->dev_addr, c_priv->driver_priv->netdev->dev_addr, ETH_ALEN);
	SET_NETDEV_DEV(netdev, wiphy_dev(c_priv->driver_priv->netdev->ieee80211_ptr->wiphy));

	wdev = kzalloc(sizeof(struct wireless_dev), GFP_KERNEL);
	if (!wdev)
	{
		free_netdev(netdev);
		netdev = NULL;
		goto fail;
	}
	wdev->iftype = NL80211_IFTYPE_AP;
	wdev->netdev = netdev;
	wdev->wiphy = c_priv->driver_priv->netdev->ieee80211_ptr->wiphy;
	netdev->ieee80211_ptr = wdev;
	netdev->netdev_ops = &ap_netdev_ops;
	c_priv->driver_priv->mon_netdev = netdev;
	register_netdevice(netdev);

fail:
	UP(&c_priv->driver_priv->cil_sem, "");
	return netdev;
}


/**
 * int cw1200_del_virtual_intf
 *
 * remove the virtual interface determined by ifindex.
 *
 * @wiphy: Pointer to wiphy device describing the WLAN interface.
 * @dev: Network Device describing this driver.
 */
int cw1200_del_virtual_intf(struct wiphy *wiphy, struct net_device *dev)
{
	struct CW1200_priv *priv = NULL;
	struct cfg_priv *c_priv = NULL;

	DEBUG(DBG_CIL, "%s, Called \n", __func__);

	c_priv = wiphy_priv(wiphy);
	priv = c_priv->driver_priv;

	DOWN(&priv->cil_sem, "");
	unregister_netdevice(dev);
	priv->mon_netdev = NULL;
	UP(&priv->cil_sem, "");

	return SUCCESS;
}


/**
 * int cw1200_set_channel
 *
 * Set channel
 *
 * @wiphy: Pointer to wiphy device describing the WLAN interface.
 * @dev: Network Device describing this driver.
 * @chan: channel definition.
 * @channel_type: Channel type.
 */
int cw1200_set_channel(struct wiphy *wiphy, struct net_device *dev,
			struct ieee80211_channel *chan,
			enum nl80211_channel_type channel_type)
{
	struct cfg_priv *c_priv = NULL;
	struct CW1200_priv *priv = NULL;

	DEBUG(DBG_CIL, "%s, Called.Channel No:%d \n", __func__, chan->hw_value);
	c_priv = wiphy_priv(wiphy);
	priv = c_priv->driver_priv;

	DOWN(&priv->cil_sem, "");
	priv->ap_channel = chan->hw_value;
	UP(&priv->cil_sem, "");

	return SUCCESS;
}


/**
* int cw1200_add_beacon
*
* Add a beacon with given parameters.
*
* @wiphy: Pointer to wiphy device describing the WLAN interface.
* @dev: Network Device describing this driver.
* @info: Information about beacon parameters. head, interval and dtim_period
* will be valid, tail is optional.
*/
int cw1200_add_beacon(struct wiphy *wiphy, struct net_device *dev,
			      struct beacon_parameters *info)
{
	DEBUG(DBG_CIL, "%s, Called \n", __func__);
	/* TBD : Store params in priv */
	return SUCCESS;
}


/**
* cw1200_set_beacon
*
* Change the beacon parameters for an access point mode interface.
* This should reject the call when no beacon has been configured.
*
* @wiphy: Pointer to wiphy device describing the WLAN interface.
* @dev: Network Device describing this driver.
* @info: Information about beacon parameters.
*/
int cw1200_set_beacon(struct wiphy *wiphy, struct net_device *dev,
			      struct beacon_parameters *info)
{
	struct cfg_priv *c_priv = NULL;
	struct CW1200_priv *priv = NULL;
	UMI_OID_802_11_START_AP ap;
	int32_t retval = SUCCESS;
	uint8_t *pos = NULL;
	uint32_t ssid_len = 0;
	struct ieee80211_mgmt *head = (struct ieee80211_mgmt *)info->head;
	u8 *tail = info->tail;
	int tail_len = info->tail_len;
	int i;
	u8 *p2p_ie = NULL;
	u8 p2p_ie_len = 0;
	u8 *wps_ie = NULL;
	u8 wps_ie_len = 0;
	uint32_t encr = 0x0;
	uint32_t auth_mode = UMI_OPEN_SYSTEM;
	UMI_UPDATE_VENDOR_IE vendor_ie;
	bool p2p_active = false;
	struct wmm_parameter_element *wmm_param = NULL;
	UMI_OID_802_11_CONFIG_WMM_PARAMS wmm_oid;
	int result = SUCCESS;

	DEBUG(DBG_CIL, "%s, Called \n", __func__);
	c_priv = wiphy_priv(wiphy);
	priv = c_priv->driver_priv;

	if ( NULL == head) {
		DEBUG(DBG_CIL, "%s,Invalid Param \n", __func__);
		return -EINVAL;
	}

	pos = &head->u.beacon.variable[0];

	if (*pos++ == WLAN_EID_SSID) {
		ssid_len = *pos++;
		DEBUG(DBG_CIL , "SSID IE found,SSID length [%d] \n", ssid_len);
	} else {
		DEBUG(DBG_CIL, "SSID IE not found \n");
		return -EINVAL;
	}

	if(tail)
	{
		for(i=0; i<tail_len; i++)
		{
			/* Search for P2P IE - OUI_WFA 0x506f9a
			P2P_OUI_TYPE 9 */
			if ( (tail[i] == 0xdd) && (tail[i+2] == 0x50) && (tail[i+3] == 0x6f)
				&& (tail[i+4] == 0x9a) && (tail[i+5] == 9) ) {
				DEBUG(DBG_CIL, "%s,P2P IE found \n",__func__);
				p2p_ie = &tail[i];
				p2p_ie_len  = tail[i+1] + 2;
				p2p_active = true;
			} else
			/* Search for WPS IE */
			if ((tail[i] == 0xdd) &&
				(GET_BE32(&tail[i+2]) == WPS_IE_VENDOR_TYPE) ) {
				DEBUG(DBG_CIL, "%s,WPS IE found \n",__func__);
				wps_ie = &tail[i];
				wps_ie_len = tail[i+1] + 2;
			} else
			if ((tail[i] == 0xdd) &&
				(GET_BE32(&tail[i+2]) == WMM_IE_VENDOR_TYPE) ) {
				DEBUG(DBG_CIL, "%s,WMM IE found\n", __func__);
				wmm_param = (struct wmm_parameter_element *)
					&tail[i+2];
				wmm_oid.qosInfo = wmm_param->qos_info;
				wmm_oid.reserved1 = 0;
				memcpy(&wmm_oid.ac_BestEffort, &wmm_param->ac[0], sizeof(WMM_AC_PARAM));
				memcpy(&wmm_oid.ac_BackGround, &wmm_param->ac[1], sizeof(WMM_AC_PARAM));
				memcpy(&wmm_oid.ac_Video, &wmm_param->ac[2], sizeof(WMM_AC_PARAM));
				memcpy(&wmm_oid.ac_Voice, &wmm_param->ac[3], sizeof(WMM_AC_PARAM));
			} else
			// WPA - skip WPS
			if(tail[i] == 0xdd && tail[i+5] == 0x01)
			{
				DEBUG(DBG_CIL, "WPA\n");
				encr |= 0x02;
				auth_mode = UMI_WPA_PSK;
			} else
			// WPA/RSN
			if(tail[i] == 0x30)
			{
				DEBUG(DBG_CIL, "WPA2/RSN\n");
				encr |= 0x04;
				auth_mode = UMI_WPA2_PSK;
			} else
			{
				DEBUG(DBG_CIL, "skiping EID: 0x%02X, len: %d\n",
				      tail[i], tail[i+1]);
			}
			i = i + tail[i+1] + 1;
		}
	}

	if (p2p_active)
		ap.mode = 1;
	else
		ap.mode = 0;

	ap.band = 0;
	ap.networkType = 2;
	ap.channelNum = priv->ap_channel;
	ap.CTWindow = 0;
	ap.beaconInterval = info->interval;
	ap.DTIMperiod = info->dtim_period;
	ap.preambleType = 1;
	ap.probeDelay = 0;
	ap.ssidLength = ssid_len;
	/* Hard coded SSID */
	memcpy(&ap.ssid[0],pos,ssid_len);

	if (p2p_active)
		ap.basicRates = 0x40;
	else
		ap.basicRates = 0xF;

	memcpy(&ap.bssid ,priv->netdev->dev_addr, ETH_ALEN);

	DOWN(&priv->cil_sem, "");
	retval = CIL_Set(priv,
			UMI_DEVICE_OID_802_11_ENCRYPTION_STATUS,
			&encr, sizeof(uint32_t));
	if (retval) {
		DEBUG(DBG_CIL, "cw1200_set_beacon:CIL_Set"
				"for ENCRYPTION status \n");
		result = -EIO;
		goto fail;
	}

	retval = CIL_Set(priv,
			UMI_DEVICE_OID_802_11_AUTHENTICATION_MODE,
			&auth_mode, sizeof(uint8_t));

	if (retval) {
		DEBUG(DBG_CIL, "cw1200_set_beacon():CIL_Set"
				"for AUTH MODE returned error \n");
		result = -EIO;
		goto fail;
	}
#ifdef ENABLE_P2P_WPS
	if (p2p_ie_len + wps_ie_len) {
		/* Set the VENDOR P2P_IE and WPS_IE to UMAC */
		memset(&vendor_ie, 0, sizeof(UMI_UPDATE_VENDOR_IE));
		vendor_ie.pBeaconIE = kmalloc(p2p_ie_len + wps_ie_len, GFP_KERNEL);
		if (!vendor_ie.pBeaconIE) {
			DEBUG(DBG_CIL, "cw1200_set_beacon():Memory"
				"allocation failure\n");
			result = -ENOMEM;
			goto fail;
		}

		if (p2p_ie_len) {
			memcpy(vendor_ie.pBeaconIE, p2p_ie, p2p_ie_len);
			if (wps_ie_len) {
				memcpy(&vendor_ie.pBeaconIE[0] + p2p_ie_len , wps_ie, wps_ie_len);
				vendor_ie.beaconIElength = p2p_ie_len + wps_ie_len;
			} else
				vendor_ie.beaconIElength = p2p_ie_len;
		} else {
			memcpy(&vendor_ie.pBeaconIE[0], wps_ie, wps_ie_len);
			vendor_ie.beaconIElength = wps_ie_len;
		}

		/* Update Vendor IE for AssocResp and ProbeResp */
		vendor_ie.pAssocRespIE = vendor_ie.pBeaconIE;
		vendor_ie.assocRespIElength = vendor_ie.beaconIElength;

		retval = CIL_Set(priv,
				UMI_DEVICE_OID_802_11_UPDATE_VENDOR_IE,
				&vendor_ie, sizeof(UMI_UPDATE_VENDOR_IE));

		kfree(vendor_ie.pBeaconIE);

		if (retval) {
		DEBUG(DBG_CIL, "%s:UMI_DEVICE_OID_802_11_UPDATE_VENDOR_IE"
				"set falied \n", __func__);
		result = -EIO;
		goto fail;
		}
	}
#endif
	/* Set the WMM information */
	if (wmm_param) {
		retval = CIL_Set(priv,
				UMI_DEVICE_OID_802_11_CONFIGURE_WMM_PARAMS,
				&wmm_oid,
				sizeof(UMI_OID_802_11_CONFIG_WMM_PARAMS));

		if (retval) {
			DEBUG(DBG_CIL, "%s:OID CONFIGURE_WMM_PARAMS"
					"set failed\n", __func__);
			return -EIO;
		}
	}
	/* Check if the call was for updating IEs or
	starting AP */
	if (priv->soft_ap_started == false) {
		uint32_t intra_bss = 1;
		retval = CIL_Set(priv,
				UMI_DEVICE_OID_802_11_START_AP,
				&ap, sizeof(UMI_OID_802_11_START_AP));

		if (retval) {
			DEBUG(DBG_CIL, "%s:UMI_DEVICE_OID_802_11_START_AP"
				"set falied\n", __func__);
			result = -EIO;
			goto fail;
		} else
			priv->soft_ap_started = true;

		/* Enable IntraBSS forwarding */
		retval = CIL_Set(priv,
				UMI_DEVICE_OID_802_11_INTRA_BSS_BRIDGING,
				&intra_bss, sizeof(intra_bss));
		if (retval) {
			WARN_ON(1);
			DEBUG(DBG_CIL, "%s:OID INTRA_BSS_BRIDGING"
				"set falied\n", __func__);
		}
	}

fail:
	UP(&priv->cil_sem, "");
	return result;
}


/**
* cw1200_deauth
*
* Request to deauthenticate from the specified peer
*
* @wiphy: Pointer to wiphy device describing the WLAN interface.
* @dev: Network Device describing this driver.
* @req: 802.11 Deauthentication request data
* @cookie:
*/
int cw1200_deauth(struct wiphy *wiphy, struct net_device *dev,
			  struct cfg80211_deauth_request *req,
			  void *cookie)
{

	DEBUG(DBG_CIL, "%s, Called \n", __func__);

	return SUCCESS;
}


/**
 * int cw1200_del_beacon
 *
 * Remove beacon configuration and stop sending the beacon.
 * @wiphy: Pointer to wiphy device describing the WLAN interface.
 * @dev: Network Device describing this driver.
 */
int cw1200_del_beacon(struct wiphy *wiphy, struct net_device *dev)
{
	struct cfg_priv *c_priv = NULL;
	struct CW1200_priv *priv = NULL;
	int32_t retval = SUCCESS;
	int result = SUCCESS;

	DEBUG(DBG_CIL, "%s, Called \n", __func__);

	c_priv = wiphy_priv(wiphy);
	priv = c_priv->driver_priv;

	DOWN(&priv->cil_sem, "expected: STOP_AP_COMPLETE");
	retval = CIL_Set(priv,
			UMI_DEVICE_OID_802_11_STOP_AP,
			0,0);

	if (retval) {
		DEBUG(DBG_CIL, "%s:UMI_DEVICE_OID_802_11_STOP_AP set falied \n",
				 __func__);
		UP(&priv->cil_sem, "");
		result = -EIO;
	}

	return result;
}


/**
* cw1200_get_station
*
* This function fills the current unicast bitrate (in Mbits/s) to the station.
*
* @wiphy: Pointer to wiphy device describing the WLAN interface.
* @dev: Network Device describing this driver.
* @sinfo: Station information filled by driver
*/
int cw1200_get_station(struct wiphy *wiphy,
				struct net_device *dev,
				u8 *mac,
				struct station_info *sinfo)
{
	int *rate_index = NULL;
	int *pSignal = NULL;

	struct cfg_priv *c_priv = NULL;
	struct CW1200_priv *priv = NULL;
	c_priv = wiphy_priv(wiphy);
	priv = c_priv->driver_priv;

	DEBUG(DBG_CIL, "%s Called \n", __func__);

	DOWN_TRYLOCK(&priv->cil_sem);
	rate_index = (int *) CIL_Get(priv, UMI_DEVICE_OID_802_11_GET_LINK_SPEED,
				sizeof(UMI_DEVICE_OID_802_11_GET_LINK_SPEED));

	if(rate_index != NULL)
	{
		if((*rate_index) >= 0 && (*rate_index) < 22)
		{
			sinfo->filled |= STATION_INFO_TX_BITRATE;
			sinfo->txrate.legacy = link_speed[*rate_index]*10;
		}
	}
	kfree(rate_index);

	if (priv->connection_status == CW1200_CONNECTED) {
		/* Get current signal strength (RSSI) */
		pSignal = (int *) CIL_Get(priv, UMI_DEVICE_OID_802_11_RSSI_RCPI,
				sizeof(UMI_DEVICE_OID_802_11_RSSI_RCPI));

		if(pSignal != NULL)
		{
			/* Convert RCPI got from UMAC to RSSI (dB) expected by upper layers */
			sinfo->filled |= STATION_INFO_SIGNAL;
			sinfo->signal = (*pSignal / 2 - 110);
		}
		kfree(pSignal);
	}

	UP(&priv->cil_sem, "");
	return 0;
}


/**
 * cw1200_remain_on_channel -
 *
 * @wiphy: Pointer to wiphy device describing the WLAN interface.
 * @dev: Network Device describing this driver.
 * @chan: channel definition.
 * @channel_type:channel Type.
 */
int cw1200_remain_on_channel(struct wiphy *wiphy,
				     struct net_device *dev,
				     struct ieee80211_channel *chan,
				     enum nl80211_channel_type channel_type,
				     unsigned int duration,
				     uint64_t *cookie)
{
	struct cfg_priv *c_priv = NULL;
	struct CW1200_priv *priv = NULL;
	int32_t retval = SUCCESS;
	uint32_t channel = chan->hw_value;
	UMI_OID_802_11_CHANGE_CHANNEL_REQ new_chan;

	DEBUG(DBG_CIL, "%s, Called:CHANNEL:%d,DURATION:%d \n",
				__func__, channel, duration);

	c_priv = wiphy_priv(wiphy);
	priv = c_priv->driver_priv;

	DOWN(&priv->cil_sem, "");

	/* Check if REMAIN on CHANNEL is active */
	if (NULL == priv->rem_on_chan) {

		*cookie = random32() | 1;
		priv->rem_on_chan = chan;
		priv->rem_on_channel_type = channel_type;
		priv->rem_on_chan_duration = duration;
		priv->rem_on_chan_cookie = *cookie;
#if 0 //From Prameela
		if (NL80211_IFTYPE_AP == priv->device_mode)
			cfg80211_ready_on_channel(priv->netdev,
				priv->rem_on_chan_cookie,
				priv->rem_on_chan,
				priv->rem_on_channel_type,
				priv->rem_on_chan_duration, GFP_ATOMIC);
		else
#endif
		/* Prepare channel OID */
		new_chan.Channel = chan->hw_value;
		new_chan.Band = chan->band;
		//new_chan.DurationInKus = duration;
		new_chan.DurationInKus = duration - 10;
		{
			retval = CIL_Set(priv,
				UMI_DEVICE_OID_802_11_CHANGE_CHANNEL,
				&new_chan,
				sizeof(UMI_OID_802_11_CHANGE_CHANNEL_REQ));

			if (retval != UMI_STATUS_SUCCESS &&
				retval != UMI_STATUS_REQ_REJECTED &&
				retval != UMI_STATUS_PENDING) {
				DEBUG(DBG_CIL, "%s:UMI_DEVICE_OID_"
					"802_11_CHANGE_CHANNEL set"
					"failed\n", __func__);
				priv->rem_on_chan = NULL;
				priv->rem_on_channel_type = NULL;
				priv->rem_on_chan_duration = 0;
				retval = -EIO;
			} else
				wait_for_completion(&priv->roc_comp);
		}
	} else
		retval = -EBUSY;

	UP(&priv->cil_sem, "");
	return retval;
}


/**
* cw1200_cancel_remain_on_channel
*
* @wiphy: Pointer to wiphy device describing the WLAN interface.
* @dev: Network Device describing this driver.
*/
int cw1200_cancel_remain_on_channel(struct wiphy *wiphy,
					struct net_device *dev,
					uint64_t cookie)
{
	uint32_t retval=SUCCESS;
	struct cfg_priv *c_priv = NULL;
	struct CW1200_priv *priv = NULL;

	DEBUG(DBG_CIL, "%s, Called \n", __func__);

	c_priv = wiphy_priv(wiphy);
	priv = c_priv->driver_priv;

#if 0 //From Prameela
	cancel_delayed_work_sync(&priv->roc_work);
	if (NL80211_IFTYPE_AP == priv->device_mode) {
		if (priv->rem_on_chan) {
			 cfg80211_remain_on_channel_expired(priv->netdev,
						priv->rem_on_chan_cookie,
						priv->rem_on_chan,
						priv->rem_on_channel_type,
						GFP_KERNEL);
		}
	} else
#endif
	{
		retval = CIL_Set(priv,
				UMI_DEVICE_OID_802_11_RESTORE_CHANNEL, 0,0);
		if (retval)
			DEBUG(DBG_CIL,
				"%s:P2P:RESTORE_CHANNEL falied\n", __func__);
	}
#if 0
	/* Restore */
	priv->rem_on_chan = NULL;
	priv->rem_on_chan_duration = 0;
#endif

	return SUCCESS;
}


/**
* cw1200_mgmt_tx
*
* @wiphy: Pointer to wiphy device describing the WLAN interface.
* @dev:	Network Device describing this driver.
* @chan: channel definition.
* @offchan:
* @channel_type: channel Type.
* @channel_type_valid:
* @wait:
* @buf:
* @cookie:
*/
int cw1200_mgmt_tx(struct wiphy *wiphy, struct net_device *dev,
		struct ieee80211_channel *chan, bool offchan,
		enum nl80211_channel_type channel_type,
		bool channel_type_valid, unsigned int wait,
		const u8 *buf, size_t len, u64 *cookie)
{
	struct cfg_priv *c_priv = NULL;
	struct CW1200_priv *priv = NULL;
	UMI_STATUS_CODE umi_status = UMI_STATUS_SUCCESS;
	uint32_t retval = SUCCESS;
	uint32_t channel = chan->hw_value;
	struct ieee80211_hdr *hdr = NULL;
	int32_t sta_link_id;
	UMI_OID_802_11_CHANGE_CHANNEL_REQ new_chan;

	DEBUG(DBG_CIL, "%s, called\n", __func__);

	c_priv = wiphy_priv(wiphy);
	priv = c_priv->driver_priv;

	DOWN(&priv->cil_sem, "");

#if 0 // From Prameela
	if (offchan && NL80211_IFTYPE_AP != priv->device_mode) {
#else
	if (offchan) {
#endif
		if (NULL == priv->rem_on_chan) {
			DEBUG(DBG_CIL, "%s, Called:OffChannel TX\n", __func__);
			*cookie = random32() | 1;
			priv->rem_on_chan_cookie = *cookie;
			priv->rem_on_chan = chan;
			priv->rem_on_channel_type = channel_type;
			priv->rem_on_chan_duration = wait;

			/* Prepare channel OID */
			new_chan.Channel = chan->hw_value;
			new_chan.Band = chan->band;
			/*TBD: To check why supplicant passes 0 wait*/
			if (!wait) {
				WARN_ON(1);
				wait = 50;
			}
			new_chan.DurationInKus = wait;

			retval = CIL_Set(priv,
				UMI_DEVICE_OID_802_11_CHANGE_CHANNEL,
				&new_chan,
				sizeof(UMI_OID_802_11_CHANGE_CHANNEL_REQ));
			if (retval != UMI_STATUS_SUCCESS &&
				retval != UMI_STATUS_REQ_REJECTED &&
				retval != UMI_STATUS_PENDING) {
				DEBUG(DBG_CIL, "%s: "
					"UMI_DEVICE_OID_802_11_CHANGE_CHANNEL"
					"set failed\n", __func__);
				priv->rem_on_chan = NULL;
				priv->rem_on_chan_duration = 0;
				priv->rem_on_chan_cookie = 0;
				UP(&priv->cil_sem, "");
				return -EIO;
			} else
				wait_for_completion(&priv->roc_comp);
		}
	}

	hdr = (struct ieee80211_hdr *)buf;
	sta_link_id = map_mac_addr(priv, hdr->addr1);

	if (-1 == sta_link_id)
		sta_link_id = 0;

	/* Pass frame to UMAC */
	umi_status = UMI_MgmtTxFrame(priv->umac_handle,
					sta_link_id, len, (uint8_t *)buf);

	if (UMI_STATUS_SUCCESS != umi_status) {
		DEBUG(DBG_CIL, "%s,UMI_MgmtTxFrame() returned error[%d]\n",
				__func__, umi_status);
		retval = -EIO;
	} else {
		retval = SUCCESS;
		*cookie = COOKIE_MGMT_TX;
		priv->mgmt_tx_cookie = *cookie;
	}
	UP(&priv->cil_sem, "");

	return retval;
}


/**
 * cw1200_mgmt_tx_cancel_wait -
 *
 * @wiphy: Pointer to wiphy device describing the WLAN interface.
 * @dev: Network Device describing this driver.
 */
int cw1200_mgmt_tx_cancel_wait(struct wiphy *wiphy,
			       struct net_device *dev,
			       u64 cookie)
{
	int32_t retval = SUCCESS;

	DEBUG(DBG_CIL, "%s, Called \n", __func__);

	retval = cw1200_cancel_remain_on_channel(wiphy,
						dev,
						cookie);

	return retval;
}


/**
 * cw1200_mgmt_frame_register -
 *
 * @wiphy: Pointer to wiphy device describing the WLAN interface.
 * @dev: Network Device describing this driver.
 */
void cw1200_mgmt_frame_register(struct wiphy *wiphy,
				struct net_device *dev,
				uint16_t frame_type, bool reg)
{
	DEBUG(DBG_CIL, "%s, Called:frame_type:%x \n", __func__, frame_type);
}

static int cw1200_suspend(struct wiphy *wiphy)
{
	struct CW1200_priv *priv = NULL;
	struct cfg_priv *c_priv = NULL;

	DEBUG(DBG_CIL, "%s\n", __func__);
	c_priv = wiphy_priv(wiphy);
	priv = c_priv->driver_priv;
	if (atomic_read(&(priv->num_unprocessed_buffs_in_device))){
		DEBUG(DBG_SUS, "unprocessed_buffs cw1200_suspend reject\n");
		return -EBUSY;
	}

	if (TRUE == atomic_read(&(priv->Interrupt_From_Device))) {
		DEBUG(DBG_SUS, "Interrupt_From_Device cw1200_suspend reject\n");
		return -EBUSY;
	}

	if ((priv->wait_event == UMI_EVT_SCAN_COMPLETED) ||
		(priv->wait_event == UMI_EVT_CONNECTED )){
		DEBUG(DBG_SUS, "scan or connect cw1200_suspend reject\n");
			return -EBUSY;
	}
	if (work_pending(&priv->rx_list_work)){
		DEBUG(DBG_SUS, "rx_list_work cw1200_suspend reject\n");
		return -EBUSY;
	}
	if (atomic_read(&priv->bk_count) ||atomic_read(&priv->vo_count)
	   || atomic_read(&priv->vi_count) || atomic_read(&priv->be_count)){
		DEBUG(DBG_SUS, "QoS Data tx cw1200_suspend reject\n");
		return -EBUSY;
	}
	if (delayed_work_pending(&priv->sbus_sleep_work)){
		DEBUG(DBG_SUS, "sbus_sleep_work cw1200_suspend reject\n");
		return -EBUSY;
	}
	if (delayed_work_pending(&priv->bss_loss_work)){
		DEBUG(DBG_SUS, "bss_loss_work cw1200_suspend reject\n");
		return -EBUSY;
	}
	if (delayed_work_pending(&priv->connection_loss_work)){
		DEBUG(DBG_SUS, "connection_loss_work cw1200_suspend reject\n");
		return -EBUSY;
	}
	priv->suspend = TRUE;

	return 0;
}

static int cw1200_resume(struct wiphy *wiphy)
{
	struct CW1200_priv *priv = NULL;
	struct cfg_priv *c_priv = NULL;

	DEBUG(DBG_CIL, "%s\n", __func__);
	c_priv = wiphy_priv(wiphy);
	priv = c_priv->driver_priv;
	/*priv->cil_get_resume = TRUE;
	wake_up_interruptible(&priv->cil_get_wait);*/
	priv->suspend = FALSE;
	return 0;
}

void UMI_CB_TxMgmtFrmComplete(UL_HANDLE ulHandle,
			UMI_TX_MGMT_FRAME_COMPLETED *pTxMgmtData)
{
	struct CW1200_priv *priv = (struct CW1200_priv *)ulHandle;

	DEBUG(DBG_CIL, "%s, Called\n", __func__);

	if (UMI_STATUS_SUCCESS == pTxMgmtData->Status) {
		cfg80211_mgmt_tx_status(priv->netdev, priv->mgmt_tx_cookie,
				(const uint8_t *)pTxMgmtData->pFrame,
				pTxMgmtData->FrameLength,
				true,GFP_ATOMIC);
	} else {
		DEBUG(DBG_CIL,"%s,P2P,Unable to transmit MGMT Frame:ERROR:"
				"%lu\n", __func__, pTxMgmtData->Status);
		cfg80211_mgmt_tx_status(priv->netdev, priv->mgmt_tx_cookie,
				(const uint8_t *)pTxMgmtData->pFrame,
				pTxMgmtData->FrameLength,
				false,GFP_ATOMIC);

	}
	priv->mgmt_tx_cookie = 0;
}

uint8_t *CIL_get_noa_attr(struct CW1200_priv *priv, int32_t *len)
{
	UMI_OID_802_11_P2P_PS_MODE *p2p_ps = NULL;
	u8 *buf = NULL;
	u8 *pos = NULL;

	p2p_ps = CIL_Get(priv,
			UMI_DEVICE_OID_802_11_P2P_PS_MODE,
			sizeof(UMI_OID_802_11_P2P_PS_MODE));
	if (!p2p_ps) {
		DEBUG(DBG_CIL, "%s:UMI_OID_802_11_P2P_PS_MODE"
				"get failed\n", __func__);
		return NULL;
	} else
		DEBUG(DBG_CIL, "%s:count:%d,duration:%lu,interval:%lu,"
			"startTime:%lu\n", __func__, p2p_ps->count,
			p2p_ps->duration, p2p_ps->interval, p2p_ps->startTime);

	buf = kzalloc(NOA_ATTR_LEN, GFP_KERNEL);
	if (!buf)
		return NULL;

	pos = buf;

	PUT_U8(pos, 0); /*Index initialised to 0*/
	PUT_U8(pos, 0); /*CTWindow */
	if (p2p_ps->count) {
		PUT_U8(pos, p2p_ps->count);
		PUT_LE32(pos, p2p_ps->duration); /* Duration field */
		PUT_LE32(pos, p2p_ps->interval); /* Noa interval */
		PUT_LE32(pos, p2p_ps->startTime); /* Schedule start time */
		*len = NOA_ATTR_LEN;
	} else
		*len = (NOA_ATTR_LEN - NOA_DESC_LEN); /*NoA without NoA desc*/

	kfree(p2p_ps);
	return buf;
}

/**
 * cw1200_init_key_map_table
 *
 * Initialize the key map table
 *
 * @priv: Pointer to driver private structure.
 */
static inline void cw1200_init_key_map_table(struct CW1200_priv *priv)
{
	memset(priv->key_maps, 0,
			sizeof(struct cw1200_key_map_table) * MAX_SUPPORTED_STA);
	priv->key_map_index = 0;
}

/**
 * cw1200_add_key_map_index
 *
 * Add the entry index for the corresponding MAC address
 *
 * @priv: Pointer to driver private structure.
 * @mac_addr: MAC address of STA to add key map, NULL for group key
 * @e_index: Entry index of the key
 * @return SUCCESS incase of null mac address or successful addition of key map
 * ERROR incase maximum supported stations reached.
 */
static inline CW1200_STATUS_E cw1200_add_key_map_index(struct CW1200_priv *priv,
		const u8 *mac_addr, uint8 e_index)
{
	/* Check for valid MAC address */
	if (!mac_addr) {
		DEBUG(DBG_CIL, "%s: NULL MAC (group key) Discard\n", __func__);
		return SUCCESS;
	}

	if (priv->key_map_index >= MAX_SUPPORTED_STA) {
		DEBUG(DBG_ERROR, "%s: Reached MAX STA supported\n", __func__);
		return ERROR;
	}

	/* Add the MAC address and entry index at last entry */
	memcpy(priv->key_maps[priv->key_map_index].mac_addr,
			mac_addr, ETH_ALEN);
	priv->key_maps[priv->key_map_index].e_index = e_index;
	priv->key_map_index++;
	return SUCCESS;
}

/**
 * static int cw1200_remove_key_map_index
 *
 * Find and remove the key entry index for the given MAC address
 * return the entry index corresponding to given MAC address
 *
 * @priv: Pointer to driver private structure.
 * @mac_addr: MAC address of STA for which key map index is to be returned
 * and removed from the key maps table. NULL for group key
 * @return entry index incase of connected station,
 * -1 if mac address is not present in the key map table
 * 0 incase of NULL MAC.
 */
static inline int8_t cw1200_remove_key_map_index(struct CW1200_priv *priv,
		const u8 *mac_addr)
{
	uint8_t cnt;
	int8_t e_index = -1;

	if (!mac_addr) {
		DEBUG(DBG_ERROR, "%s: NULL MAC address, hard coding"
				" entry index 0 for group key\n", __func__);
		return 0;
	}

	for (cnt = 0; cnt < priv->key_map_index; cnt++) {
		if ((compare_ether_addr(priv->key_maps[cnt].mac_addr,
					mac_addr)) == 0) {
			e_index = priv->key_maps[cnt].e_index;
			/*Ifthe key index being deleted is not the last one, then copy the last one to the index which is being deleted*/
			if (cnt != (priv->key_map_index-1)) {
				memcpy(priv->key_maps[cnt].mac_addr,
					priv->key_maps[priv->key_map_index-1].mac_addr,
					ETH_ALEN);
				priv->key_maps[cnt].e_index = priv->key_maps[priv->key_map_index-1].e_index;
			}
			memset(priv->key_maps[priv->key_map_index-1].mac_addr, 0,
				ETH_ALEN);
			priv->key_maps[priv->key_map_index-1].e_index = 0;
			priv->key_map_index--;
			break;
		}
	}
	DEBUG(DBG_CIL, "%s: entry index for remove key = %d\n",
			__func__, e_index);

	return e_index;
}

CW1200_STATUS_E cw1200_stop_scan(struct CW1200_priv *priv)
{
	UMI_BSS_LIST_SCAN scan_req;
	CW1200_STATUS_E	retval = SUCCESS;

	DEBUG(DBG_CIL, "%s, Called \n", __func__);
	if (atomic_read(&(priv->cw1200_unresponsive)) == FALSE){
		memset(&scan_req, 0, sizeof(UMI_BSS_LIST_SCAN));
		scan_req.flags = 0;

		schedule_delayed_work(&priv->scan_timeout, HZ);

		mutex_lock(&priv->scan_timeout_lock);
		atomic_set(&priv->scan_timeout_count, 1);
		mutex_unlock(&priv->scan_timeout_lock);

		retval = CIL_Set(priv,
			UMI_DEVICE_OID_802_11_BSSID_LIST_SCAN,
			&scan_req, sizeof(UMI_BSS_LIST_SCAN));
	} else {
		if (priv->request) {
			cfg80211_scan_done(priv->request , true);
		}
		priv->request = NULL;
		priv->wait_event = 0;
		UP(&priv->cil_sem, "UMI_EVT_SCAN_COMPLETED");
	}
	return retval;
}

void cw1200_scan_timeout(struct work_struct *work)
{
	struct CW1200_priv *priv =
		container_of(work, struct CW1200_priv, scan_timeout.work);

	DEBUG(DBG_CIL, "%s,Scan timeout\n", __func__);
	if(atomic_read(&priv->scan_timeout_count) == 1) {
		atomic_xchg(&(priv->cw1200_unresponsive), TRUE);

		mutex_lock(&priv->scan_timeout_lock);
		atomic_set(&priv->scan_timeout_count, 0);
		mutex_unlock(&priv->scan_timeout_lock);

		DEBUG(DBG_ERROR, "Firmware or UMAC hanged, Calling Unload driver\n");
		schedule_delayed_work(&sdio_cw_priv.unload_work, 0);
	} else {
		WARN_ON(cw1200_stop_scan(priv));
	}
}
