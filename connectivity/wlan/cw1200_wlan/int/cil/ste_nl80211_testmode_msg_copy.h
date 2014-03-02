/*
 * ST-Ericsson hostap/wpa_supplicant changes and additions.
 *
 * Copyright (C) ST-Ericsson AB 2010
 *
 */

#ifndef _STE_NL80211_TESTMODE_MSG_H_
#define _STE_NL80211_TESTMODE_MSG_H_

#include <linux/types.h>

/**
 * enum ste_ps_conf_flags - configuration flags
 *
 * Flags to define powersave configuration options.
 *
 * @STE_PS_CONF_IEEE_PS: Enable 802.11 power save mode (managed mode only)
 *
 * @STE_PS_CONF_IDLE: The device is running, but idle;
 *	if the flag is set the driver should be prepared
 *	to handle configuration requests but may turn the
 *	device off as much as possible. Typically, this flag
 *	will be set when an interface is set UP but not
 *	associated or scanning. It can also be unset in that
 *	case when monitor interfaces are active.
 *
 * @STE_PS_CONF_ARP_FILTERING: Enable ARP filtering.
 *	Should be only activated when PS is on
 *
 * @STE_PS_CONF_MC_FILTERING: Enable Multicast MAC address filtering.
 *	Should be only activated when PS is on
 *
 */
enum ste_ps_conf_flags {
	STE_PS_CONF_IEEE_PS       = (1<<0),
	STE_PS_CONF_IDLE          = (1<<1),
	STE_PS_CONF_ARP_FILTERING = (1<<2),
	STE_PS_CONF_MC_FILTERING  = (1<<3),
};

enum ste_p2p_legacy_ps {
	STE_P2P_LEGACY_PS_NO_CHANGE  = (1<<0),
	STE_P2P_LEGACY_PS_ENABLE     = (1<<1),
	STE_P2P_LEGACY_PS_DISABLE    = (1<<2),
};

enum ste_p2p_opp_ps {
	STE_P2P_OPP_PS_NO_CHANGE  = (1<<0),
	STE_P2P_OPP_PS_ENABLE     = (1<<1),
	STE_P2P_OPP_PS_DISABLE    = (1<<2),
};

/**
 * struct ste_arp_filtering_list - list of IP addresses
 *
 * This struct defines the IP addresses for ARP filtering
 *
 * @enable: indicates whether arp filtering is enabled or disabled
 *
 * @ip_addr: ip address of wlan interface
 *
 */
struct ste_arp_filtering_list {
	__u8 enable;
	__u8 ip_addr[4];
};

/**
 * struct ste_ps_conf - powersave configuration of the device
 *
 * This struct indicates how the driver shall configure
 * powersave settings in the hardware.
 *
 * @flags: configuration flags defined above
 *
 * @listen_interval_unit: listen interval unit
 *	0    = unchanged
 *	1    = DTIM (DTIM skipping)
 *	2    = TIM  (TIM skipping, but chip needs to wakeup
 *	             on DTIMs as well)
 *	0xFF = driver default
 *
 * @listen_interval: listen interval in units of DTIM or TIM interval.
 *	This parameter is only applicable if listen_interval_unit
 *	is set to 1 or 2.
 *
 * @dynamic_ps_timeout: dynamic powersave timout (in ms)
 *	This variable is valid only when the STE_PS_CONF_IEEE_PS flag
 *	is set.
 *	0    = unchanged
 *	0xFF = driver default
 *
 * @multicast_filtering_enable: indicates whether arp filtering is
 * enabled or disabled
 *
 * @ipaddr_list: list of the wlan interface IP addresses used in
 * arp filtering
 *
 */
struct ste_msg_ps_conf {
	__u32 flags;
	__u8 listen_interval_unit;
	__u8 listen_interval;
	__u8 dynamic_ps_timeout;
	__u8 multicast_filtering_enable;
	struct ste_arp_filtering_list arp_filtering_list;
};


/**
 * enum ste_modtest_packet_monitor_flags - bitwise configuration flags
 *
 * Flags to define packet monitor configuration options.
 *
 * @STE_MODTEST_PACKET_MONITOR_IP_UDP: Monitor IP UDP packets (unencrypted)
 *
 * @STE_MODTEST_PACKET_MONITOR_IP_TCP: Monitor IP TCP packets (unencrypted)
 *
 * @STE_MODTEST_PACKET_MONITOR_IP_ICMP: Monitor IP ICMP packets (unencrypted)
 *
 * @STE_MODTEST_PACKET_MONITOR_PAE: Monitor PAE packets
 *
 * @STE_MODTEST_PACKET_MONITOR_WMM: Monitor WMM in packets (priority)
 *
 * @STE_MODTEST_PACKET_MONITOR_DEST_MAC_CHECK: Monitor packets MAC destination
 */
enum ste_modtest_packet_monitor_flags {
	STE_MODTEST_PACKET_MONITOR_IP_UDP          = (1<<0),
	STE_MODTEST_PACKET_MONITOR_IP_TCP          = (1<<1),
	STE_MODTEST_PACKET_MONITOR_IP_ICMP         = (1<<2),
	STE_MODTEST_PACKET_MONITOR_PAE             = (1<<3),
	STE_MODTEST_PACKET_MONITOR_WMM             = (1<<4),
	STE_MODTEST_PACKET_MONITOR_DEST_MAC_CHECK  = (1<<5),
};

struct ste_msg_modtest_packet_monitor {
	__u32 flags; 		/* Use ste_modtest_packet_monitor_flags bit flags */
	__u16 payload_size; /* Check only packets with specific payload size. 0 = all packets. Only useful for unencrypted data. */
	__u8 dest_mac[6]; 	/* Destination MAC addr to check (if set in flags) */
};

struct ste_msg_modtest_packet_monitor_report {
	__u32 udp_tx;          /* Unencrypted IP/UDP packets sent */
	__u32 tcp_tx;          /* Unencrypted IP/TCP packets sent */
	__u32 icmp_tx;         /* Unencrypted IP/ICMP packets sent */
	__u32 pae_tx;          /* Encrypted (PAE) packets sent */
	__u32 wmm_ac_tx[4];    /* WMM packets sent for each AC */
};


/**
 * enum ste_msg_11n_flags - bitwise .11n configuration flags
 *
 * @STE_11N_CONF_USE_TID_POLICY: TID policy fields are valid.
 *
 * @STE_11N_CONF_USE_AMSDU_DEFAULT_SIZE: @amsdu_default_size field is valid.
 */

enum ste_msg_11n_flags {
	STE_11N_CONF_USE_TID_POLICY		= (1 << 0),
	STE_11N_CONF_USE_AMSDU_DEFAULT_SIZE	= (1 << 1),
};

/**
 * struct ste_msg_11n_conf - .11n configuration of the device
 *
 * This struct indicates how the driver shall configure
 * block ACK and/or AMSDU default size settings in the hardware.
 *
 * @flags: configuration flags defined above
 *
 * @block_ack_tx_tid_policy: When enabled, the WLAN device firmware can
 *	attempt to establish a block Ack agreement for that TID in the
 *	transmit direction.
 *	Bits 7:0 correspond to TIDs 7:0 respectively.
 *	Bit value	= 0 - Block Ack disabled
 *			= 1 - Block Ack enabled (driver default).
 *
 * @block_ack_rx_tid_policy: When enabled, the WLAN device firmware shall,
 *	where possible, accept requests to establish a block
 *	Ack agreement for that TID in the receive direction.
 *	Bits 7:0 correspond to TIDs 7:0 respectively.
 *	Bit value	= 0 - Block Ack requests disabled
 *			= 1 - Block Ack requests accepted
 *			      subject to availability (driver default).
 *
 * @amsdu_default_size: AMSDU default size: 8K, 4K.
 */

struct ste_msg_11n_conf {
	__u32 flags;
	__u8  block_ack_tx_tid_policy;
	__u8  block_ack_rx_tid_policy;
	__u16 amsdu_default_size;
};

/**
 * enum ste_msg_cqm_flags - bitwise STE CQM configuration flags
 *
 * @STE_CQM_CONF_USE_BEACON_THRESHOLD: @beacon_threshold field is valid.
 *
 * @STE_CQM_CONF_USE_TX_THRESHOLD: @tx_treshold field is valid.
 */

enum ste_msg_cqm_flags {
	STE_CQM_CONF_USE_BEACON_THRESHOLD		= (1 << 0),
	STE_CQM_CONF_USE_TX_THRESHOLD			= (1 << 1),
};

/**
 * struct ste_msg_cqm_conf - STE connection quality monitor configuration
 *
 * This struct indicates how the driver shall configure
 * STE CQM settings in the hardware.
 *
 * @flags:		Configuration flags defined above
 *
 * @beacon_threshold:	Driver shall trigger STE_MSG_CQM_EVENT event with
 *	appropriate flags when it detects that @beacon_threshold consecutive
 *	beacons (not DTIMs!) have been lost. Driver shall auto-disable
 *	beacon loss subscription when event is triggered.
 *	Value "0" means "disabled" (default).
 *
 * @tx_treshold:	Driver shall trigger STE_MSG_CQM_EVENT event with
 *	appropriate flags when it detects that @tx_treshold consecutive
 *	TX MPDU have not been delivered. Driver shall auto-disable
 *	TX loss subscription when event is triggered.
 *	Note that tx_treshold is a number of TX MPDU, not a number
 *	of transmission attempts.
 *	Value "0" means "disabled" (default).
 *
 * @bt_window:		Driver shall use this value as an interval
 *	(in milliseconds) for @bt_percent calculation.
 *
 */

struct ste_msg_cqm_conf {
	__u32 flags;
	__u8  beacon_threshold;
	__u8  tx_treshold;
	__u16 bt_window;
};

/**
 * enum ste_msg_cqm_event_flags - bitwise STE CQM event flags.
 *
 * @STE_CQM_EVENT_BEACON_THRESHOLD_TRIGGERED: Beacon threshold event is triggred.
 *
 * @STE_CQM_EVENT_TX_THRESHOLD_TRIGGERED: TX threshold event is triggred.
 *
 * @STE_CQM_EVENT_BT_HINTS: Driver estimated bluetooth activity and filled
 *	appropriate fields.
 */

enum ste_msg_cqm_event_flags {
	STE_CQM_EVENT_BEACON_THRESHOLD_TRIGGERED	= (1 << 0),
	STE_CQM_EVENT_TX_THRESHOLD_TRIGGERED		= (1 << 1),
	STE_CQM_EVENT_BT_HINTS				= (1 << 2),
};

/**
 * struct ste_msg_cqm_event - STE connection quality monitor event
 * which is sent by the driver to userspace when STE CQM event is
 * detected.
 *
 * @flags:		Event flags defined above
 *
 * @bt_active:		Driver shall fill this field with estimated
 *	time interval (in milliseconds) of the last BT activity if
 *	this activity could affect WLAN performance. Driver shall
 *	fill this value with 0 if bluetooth is not active or if the
 *	activity is not disturbing WLAN.
 *
 * @bt_percent:		Driver shall fill this value with estimated
 *	percent of time in rolling window (@bt_window) when antenna
 *	is granted for bluetooth.
 */

struct ste_msg_cqm_event {
	__u32 flags;
	__u16 bt_active;
	__u8  bt_percent;
};

#ifndef ETH_ALEN
#define ETH_ALEN 6
#endif

/**
 * ste_msg_modtest_set_power - set power for given bssid.
 *
 * @bssid: Network id
 *
 * @ power_level: Signal strength level (dBm)
 *
 */
struct ste_msg_modtest_set_power {
	char bssid[ETH_ALEN];
	int power_level;
};

/**
 * ste_msg_modtest_set_beacon - Turn on/off beacons.
 *
 * @bssid: Network id
 *
 * @ disabled: Turn on/off beacon broadcasting
 *
 */
struct ste_msg_modtest_set_beacon {
	char bssid[ETH_ALEN];
	int disabled;
};

/**
 * ste_msg_modtest_set_tx fail - Turn on/off tx fails.
 *
 * @bssid: Network id
 *
 * @ enabled: Turn on/off tx fails
 *
 */
struct ste_msg_modtest_set_tx_fail {
        int enabled;
};



/**
 * ste_msg_set_p2p - Turn on/off P2P
 *
 * @enable: Turn on/off P2P
 *
 */
struct ste_msg_set_p2p {
	int enable;
};

struct ste_msg_set_p2p_power_save {
	__u8 legacy_ps;
	__u8 opp_ps;
	int ctwindow;
};

struct ste_msg_set_noa {
	__u8 count;
	int start;
	int duration;
};

struct ste_msg_set_uapsd {
	int flags;
	__u16 minAutoTriggerInterval;
	__u16 maxAutoTriggerInterval;
	__u16 autoTriggerStep;
};

struct ste_msg_tx_stat_chk {
	int enable;
	int tx_stat_chk_prd;
	int tx_fail_rate;
	int tx_stat_chk_num;
};


struct ste_msg_p2p_disconnect_client {
	__u8 peer_addr[ETH_ALEN];
};

struct ste_msg_set_rssi_filter {
	int threshold;
};

enum ste_msg_id
{
	STE_MSG_PS_CONF_SET = 0x0000, 			// SEND: struct ste_msg_ps_conf, REPLY: no data
	STE_MSG_PS_CONF_GET,          			// SEND: no data, REPLY: struct ste_msg_ps_conf

	// Commands controlling module test driver
	STE_MSG_MODTEST_PACKET_MONITOR_START,   	// SEND: ste_msg_packet_monitor, REPLY: no data
	STE_MSG_MODTEST_PACKET_MONITOR_STOP,    	// SEND: no data, REPLY: ste_msg_packet_monitor_report

	// Block ACK configuratuion
	STE_MSG_11N_CONF_SET,				// SEND: struct ste_msg_11n_conf, REPLY: no data

	STE_MSG_CQM_CONF_SET,				// SEND: struct ste_msg_cqm_conf, REPLY: no data
	STE_MSG_CQM_EVENT,				// EVENT: struct ste_msg_cqm_event

	STE_MSG_SET_P2P,				// SEND: ste_msg_set_p2p, REPLY: no data
	STE_MSG_SET_P2P_POWER_SAVE,			// SEND: ste_msg_set_p2p_power_save, REPLY: no data
	STE_MSG_SET_NOA,				// SEND: ste_msg_set_noa, REPLY: no data
	STE_MSG_GET_NOA,				// SEND: no data, REPLY: struct ste_msg_ps_conf

	STE_MSG_SET_UAPSD,				// SEND: ste_msg_set_uapsd, REPLY: no data
	STE_MSG_PROBE_RES,				// SEND: probe_response IE, REPLY: no data

	STE_MSG_TX_STAT_CHK,				// TODO: update comment
	STE_MSG_P2P_DISCONNECT_CLIENT,  		// SEND: ste_msg_p2p_disconnect_client, REPLY: no data
	STE_MSG_SET_RSSI_FILTER,			       /* SEND: ste_msg_set_rssi_filter, REPLY: no data */

	// Add new message ids here - modtest specific below!
	STE_MSG_SET_POWER,				// SEND: ste_msg_modtest_set_power, REPLY: no data
	STE_MSG_SET_BEACON,				// SEND: ste_msg_modtest_set_beacon, REPLY: no data
	STE_MSG_SET_TX_FAIL,				// SEND: ste_msg_modtest_set_tx_fail, REPLY: no data
	STE_MSG_ID_MAX,					// Max indicator so module test may add its own message ids
};

union ste_msg
{
	struct ste_msg_ps_conf                          ps_conf;
	struct ste_msg_modtest_packet_monitor           packet_monitor;
	struct ste_msg_modtest_packet_monitor_report    packet_monitor_report;
	struct ste_msg_11n_conf				ste_11n_conf;
	struct ste_msg_cqm_conf				cqm_conf;
	struct ste_msg_cqm_event			cqm_event;
	struct ste_msg_modtest_set_power		set_power;
	struct ste_msg_modtest_set_beacon		set_beacon;
	struct ste_msg_modtest_set_tx_fail              set_tx_fail;
	struct ste_msg_set_p2p				set_p2p;
	struct ste_msg_set_p2p_power_save		set_p2p_power_save;
	struct ste_msg_set_noa				set_noa;
	struct ste_msg_set_uapsd			set_uapsd;
	struct ste_msg_set_rssi_filter			set_rssi_filter;
	struct ste_msg_tx_stat_chk			stat_chk;
	struct ste_msg_p2p_disconnect_client	p2p_disconnect_client;
};

#endif /* _STE_NL80211_TESTMODE_MSG_H_ */

