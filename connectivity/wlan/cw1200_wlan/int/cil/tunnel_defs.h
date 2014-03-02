/******************************************************************************
| Tunnel function prototypes
\******************************************************************************/

enum ste_msg_type {
		STE_MSGTYPE_REQUEST = 0, /* Request to driver*/
		STE_MSGTYPE_RESPONSE,	/* Response from driver*/
		STE_MSGTYPE_EVENT		/* Event from driver*/
};

struct tunnel_data {
		enum ste_msg_type type;	/* request, response or event) */
		uint16	   msg_id;	/* specific id (ste_msg_id or vendor)*/
		uint16	   data_len;	/* data length*/
		uint8		data_buf;	/* data buffer*/
};


enum ste_msg_id {
		STE_MSG_PS_CONF_SET = 0x0000,
		STE_MSG_RSSI_THRESHOLD_GET,
		STE_MSG_SCAN_REQUEST,
		STE_MSG_SCAN_RESPONSE,
		STE_MSG_SCAN_ABORT,
		STE_MSG_SCAN_EVENT,
		STE_MSG_SCAN_GET,
		STE_MSG_SCAN_REPORT,
		STE_MSG_TRIGGER_SET,
		STE_MSG_TRIGGER_UNSET,
		STE_MSG_TRIGGER_EVENT
};



/**
 * enum ste_ps_conf_flags - configuration flags
 *
 * Flags to define powersave configuration options
 *
 * @STE_PS_CONF_IEEE_PS: Enable 802.11 power save mode (managed mode only)
 * @STE_PS_CONF_IDLE: The device is running, but idle; if the flag is set
 *	  the driver should be prepared to handle configuration requests but
 *	  may turn the device off as much as possible. Typically, this flag will
 *	  be set when an interface is set UP but not associated or scanning, but
 *	  it can also be unset in that case when monitor interfaces are active.
 */
enum ste_ps_conf_flags {
		STE_PS_CONF_IEEE_PS = (1<<0),
		STE_PS_CONF_IDLE	= (1<<1),
		STE_PS_CONF_ARP_FILTERING = (1<<2),
		STE_PS_CONF_MC_FILTERING  = (1<<3),
};


/**
 * struct ste_ps_conf - configuration of the device
 *
 * This struct indicates how the driver shall configure powersave settings in the hardware.
 *
 * @flags: configuration flags defined above
 *
 * @listen_interval_unit: listen interval unit
 *	  0	= unchanged
 *	  1	= DTIM (DTIM skipping)
 *	  2	= TIM  (TIM skipping, but chip needs to wakeup on DTIMs as well)
 *	  0xFF = driver default (original setting)
 * @listen_interval: listen interval in units of DTIM or TIM interval
 *	  This parameter is only applicable if listen_interval_unit is set to 1 or 2
 * @dynamic_ps_timeout: The dynamic powersave timeout (in ms), TBD: add description
 *	  This variable is valid only when the STE_PS_CONF_IEEE_PS flag is set.
 *	  0	= unchanged
 *	  0xFF = driver default (original setting)
 */
struct ste_ps_conf {
		u32 flags;
		u8  listen_interval_unit;
		u8  listen_interval;
		u8  dynamic_ps_timeout;
};



/**
 * struct ste_ssid_list - list of SSIDs
 *
 * @next: pointer to next SSID or NULL
 * @ssid: SSID
 * @ssid_len: SSID length
 */
struct ste_ssid_list {
		struct ste_ssid_list *next;
		u8				   ssid[IEEE80211_MAX_SSID_LEN];
		u8				   ssid_len;
};

/**
 * struct ste_scan_channel_conf - channel configuration
 *
 * @band: defines the band
 * @channel_num: channel number
 */
struct ste_scan_channel_conf {
		enum ieee80211_band band;
	u8				  channel_num;
};

/**
 * enum ste_scan_flags - scan configuration flags
 *
 * Flags to define powersave configuration options
 *
 * @STE_SCAN_CONF_HIGH_PRIORITY: scan with high priority (interrupt traffic)
 * @STE_SCAN_CONF_PASSIVE_SCAN: disables the use of probe requests
 *	  Observe that this parameter shall not overrule the regulatory domain settings
 * @STE_SCAN_CONF_REPORT_ALWAYS: send @ste_scan_event even if no AP is found
 * @STE_SCAN_CONF_DISABLE_CHANNEL_FILTER: turns off the filtering of scan responses
 *	  from adjacent channels
 */
enum ste_scan_flags {
		STE_SCAN_CONF_HIGH_PRIORITY		  = (1<<0),
		STE_SCAN_CONF_PASSIVE_SCAN		   = (1<<2),
		STE_SCAN_CONF_REPORT_ALWAYS		  = (1<<3),
		STE_SCAN_CONF_DISABLE_CHANNEL_FILTER = (1<<4)
};
/**
 * struct ste_scan_conf - scan configuration
 *
 * @flags: scan configuration flags
 * @num_probes: number of probes to be sent
 * @min_channel_time: minimum scan time in ms
 * @max_channel_time: maximum scan time in ms
 */
struct ste_scan_conf {
		u32 flags;
		u8  num_probes;
		u16 min_channel_time;
		u16 max_channel_time;
};

/**
 * struct ste_scan_request - scan request description
 *
 * @interval: scan interval in seconds
 *	  0 = single scan
 * @conf: scan configuration parameters
 * @ssids: SSIDs to scan for or %NULL
 * @n_ssids: number of SSIDs
 *	  0 = wildcard SSID
 * @channels: channels to scan on or %NULL
 * @n_channels: number of channels for each band
 *	  0 = all channels
 * @ie: optional information element(s) to add into Probe Request or %NULL
 * @ie_len: length of ie in octets
 */
struct ste_scan_request {
		u32						  interval;
		struct ste_scan_conf		 conf;
		struct ste_ssid_list		 *ssids;
		int						  n_ssids;
		struct ste_scan_channel_conf **channels;
		u32						  n_channels;
		const u8					 *ie;
		size_t					   ie_len;
};



/**
 * enum ste_threshold_type - threshold subscription type
*/
enum ste_threshold_type {
		STE_THRESHOLD_LOW,
		STE_THRESHOLD_HIGH
};

/**
 * enum ste_trigger_type - threshold subscription type
 *
 * @STE_TRIGGER_TYPE_RSSI: low and high RSSI threshold values are in dBm
 * @STE_TRIGGER_TYPE_SNR: low and high RSSI threshold values are in dB
 *
 * A trigger event shall only be reported once. Hence, the corresponding
 * low/high threshold shall be unsubscribed automatically to prevent
 * frequent reporting. First when the subscription is renewed with the
 * ste_trigger_set command, a new event can be generated.
 *
 * When a subscription is made and the current value is above/below the
 * high/low threshold value, an event shall be generated immediately.
*/
enum ste_trigger_type {
		STE_TRIGGER_TYPE_RSSI,
		STE_TRIGGER_TYPE_SNR,
		STE_TRIGGER_TYPE_BEACON_LOSS
};

/**
 * struct ste_trigger - trigger description
 *
 * @trigger_type: defines the trigger type
 * @low_threshold: low threshold value
 *	  0 = unsubscribe low threshold
 * @high_threshold: high threshold value
 *	  0 = unsubscribe high threshold
 */
struct ste_trigger {
		enum ste_trigger_type trigger_type;
		u16			  low_threshold;
		u16			  high_threshold;
};

/**
 * struct ste_trigger_set - request for threshold subscription
 *
 * @trigger: defines trigger type and threshold values
 */
struct ste_trigger_set {
		struct ste_trigger *trigger;
};

/**
 * struct ste_trigger_unset - request to remove threshold subscription
 *
 * @trigger_type: defines the trigger type
 */
struct ste_trigger_unset {
		enum ste_trigger_type trigger_type;
};



/**
 * struct ste_ipaddr_list - list of IP addresses
 *
 * @next: pointer to next IP address or NULL
 * @ip_addr: pointer to IP address
 */
struct ste_ip_addr_list {
		struct ste_ip_addr_list *next;
		struct in_addr		  ip_addr;
};

/**
 * struct ste_ipfilter_set - set IP address filter
 *
 * @ip_count: number of IP addresses
 * @ip_list: null terminated list of IP addreses
 */
struct ste_ipfilter_set {
		u8					  ip_count;
		struct ste_ip_addr_list ip_list;
};



/******************************************************************************
| Tunnel event function prototypes
\******************************************************************************/

/**
 * struct ste_scan_event - scan results available
 *
 * @num_aps: number of APs found
 */
struct ste_scan_event {
		u8 num_aps;
};



/*
 * struct ste_trigger_event - trigger event occured
 *
 * @trigger_type: defines which event was triggered
 * @threshold_type: defines if a low or high threshold event occured
 */
struct ste_trigger_event {
		enum ste_trigger_type   trigger_type;
		enum ste_threshold_type threshold_type;
};
