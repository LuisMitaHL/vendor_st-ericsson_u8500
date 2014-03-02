/*====================================================================
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
 *=====================================================================*/
/**
 *addtogroup Linux Driver Common Include File
 *brief
 *
 */
/**
 *file : CW1200 Common Header File
 *- <b>PROJECT</b>			 : CW1200_LINUX_DRIVER
 *- <b>FILE</b>				: cw1200_common.h
 *brief
 *Common Header file included by all modules.
 *ingroup : CW1200 Common Header Files
 *date 25/02/2010
 */

#ifndef __CW1200_COMMON__
#define  __CW1200_COMMON__

/***********************************************************************
*										INCLUDE FILES
************************************************************************/
#include <asm/atomic.h>
#include <linux/netdevice.h>
#include <linux/module.h>
#include <linux/wait.h>
#include <net/cfg80211.h>
#include <linux/spinlock.h>
#include <linux/wakelock.h>
#include "../umac_lib/mod_umi/export/UMI_Api.h"  /*UMAC API declaration file */

/***********************************************************************
*										MACROS
************************************************************************/
#define FALSE		0
#define TRUE		1
#define FIRMWARE_CUT11	"wsm_11.bin"
#define FIRMWARE_CUT10	"wsm_10.bin"
#define FIRMWARE_CUT20	"wsm_20.bin"
#define FIRMWARE_CUT22	"wsm_22.bin"
#define SDD_FILE_11	"sdd_11.bin"
#define SDD_FILE_10	"sdd_10.bin"
#define SDD_FILE_20	"sdd_20.bin"
#define SDD_FILE_22	"sdd_22.bin"

#define UMAC_NOTSTARTED 0
#define UMAC_STARTED  1

#define ALIGN4(x)	((((int)(x)) & 0x3) == 0)

#ifndef USE_SPI
typedef struct sdio_func  CW1200_bus_device_t;
#else
typedef struct spi_device CW1200_bus_device_t;
#endif

extern const struct cw1200_platform_data *wpd;

#define	MACADDR_LEN	 6

/* 5 seconds*/
#define CLEANUP_TIMEOUT		5000


/* Maximum number of stations supported in case of AP/P2P GO mode */
#define MAX_SUPPORTED_STA 9

typedef enum CW1200_STATUS {
	SUCCESS = 0,
	ERROR,
	ERROR_SDIO,
	ERROR_INVALID_PARAMETERS,
	ERROR_BUF_UNALIGNED,
	WAIT_INIT_VAL,
} CW1200_STATUS_E;

#define MCADDR_LIST_MAXSIZE UMI_MAX_GRP_ADDRTABLE_ENTRIES

extern struct device cw1200_device;

extern struct completion shutdown_comp;

extern struct sdio_priv sdio_cw_priv;

struct cw1200_key_map_table {
	u8 mac_addr[ETH_ALEN];
	uint8_t e_index;
};

struct CW1200_priv {
	UMI_HANDLE  umac_handle;
	UMI_HANDLE  lower_handle;
	uint32_t umac_status;
	struct net_device *netdev;
	struct net_device_stats stats;
	struct workqueue_struct *umac_WQ;
	struct work_struct umac_work;
	atomic_t tx_count;

	atomic_t bk_count;
	atomic_t be_count;
	atomic_t vi_count;
	atomic_t vo_count;

	uint32_t operational_power_mode;

	/* Enable/disable dual band module param*/
	bool dualband;

	/*SBUS related declaraions*/
	atomic_t 	Interrupt_From_Device;
	atomic_t	 Umac_Tx;
	struct workqueue_struct *sbus_WQ;
	int32_t sdio_wr_buf_num_qmode;
	int32_t sdio_rd_buf_num_qmode;
	CW1200_bus_device_t *func;
	struct work_struct sbus_work;
	struct work_struct mc_work;  /*for multicast filter only*/
	uint32_t max_size_supp;
	int32_t max_num_buffs_supp;
	atomic_t num_unprocessed_buffs_in_device;
	uint16_t in_seq_num;
	uint16_t out_seq_num;

	uint32_t hw_type;
	uint32_t cw1200_cut_no;
	uint32_t device_sleep_status;
	uint32_t dynamic_ps_timeout;
	struct delayed_work sbus_sleep_work;
	/*CIL related declarations */
	struct semaphore cil_sem;
	UMI_EVENT wait_event;
	wait_queue_head_t  cil_set_wait;
	atomic_t cil_set_cond;
	wait_queue_head_t  cil_get_wait;
	uint8_t suspend;
	/*uint8_t cil_get_resume;*/
	void *get_buff_pointer ;
	uint32_t get_buff_len;
	uint32_t set_status;
	struct cfg80211_scan_request *request;
	uint32_t connection_status;
	uint32_t wait_flag_set ;
	uint32_t wait_flag_get ;
	spinlock_t cil_lock ;
	spinlock_t pm_lock ;
	uint8_t tx_block_ack;
	uint8_t rx_block_ack;
	uint8_t u_rssi;
	uint8_t l_rssi;
	uint32_t beacon_interval;
	uint32_t delayed_link_loss;
	bool cqm_beacon_loss_enabled;
	uint32_t cqm_beacon_loss_count;
	UMI_OID_802_11_MULTICAST_ADDR_FILTER ste_mc_filter;
	uint32_t cqm_link_loss_count;
	struct delayed_work bss_loss_work;
	struct delayed_work connection_loss_work;
	struct delayed_work connect_result;
	struct delayed_work roc_work;
	struct delayed_work scan_timeout;
	struct workqueue_struct * bss_loss_WQ;
	UMI_BSSID_PMKID_BKID pmk_list;
	uint8 connected_ssid[32];
#ifdef WORKAROUND
	struct semaphore sem;
#endif
	struct completion cleanup_comp;
	uint32_t eil_shutdown;
	uint32_t ap_channel;
	uint8_t device_mode;
	UMI_EVT_DATA_CONNECT_INFO * sta_info;
	uint8_t sta_count;
	struct net_device * mon_netdev;
	int8_t setget_linkid;
	int8_t auth_mode_default;
	/* Declarations for P2P support */
	uint64_t rem_on_chan_cookie;
	struct ieee80211_channel *rem_on_chan;
	enum nl80211_channel_type rem_on_channel_type;
	int rem_on_chan_duration;
	uint64_t mgmt_tx_cookie;
	struct completion roc_comp;
	bool soft_ap_started;
	struct mutex cfg_set;
	struct mutex scan_timeout_lock;
	struct wake_lock wake_lock;
	struct wake_lock wake_lock_rx;
	bool    suspend_block;
	uint16_t minAutoTriggerInterval;
	uint16_t maxAutoTriggerInterval;
	uint16_t autoTriggerStep;
	/* P2P PS settings */
	int ctwindow;
        uint32 duration;
        uint32 interval;
        uint32 startTime;
	uint8 count;
	bool p2p_enabled;
	struct proc_dir_entry *proc;
	struct list_head rx_list;
	struct work_struct rx_list_work;
	struct workqueue_struct *rx_WQ;
	struct mutex rx_list_lock;
	struct delayed_work per_work;
	uint32 per_start;
	uint32 per_interval;
	uint32 per_tx_threshold;
	uint32 per_failure_threshold;
	uint32 rssi_filter_threshold;
	uint8 arp_offload;
	uint16 key_map_index;
	struct cw1200_key_map_table key_maps[MAX_SUPPORTED_STA];
	atomic_t cw1200_unresponsive;
	struct delayed_work recover_bh;
	atomic_t scan_timeout_count;
} ;

struct cw1200_rx_buf {
	struct list_head list;
	struct sk_buff *skb;
	uint8_t *data;
};

struct sdio_priv {
	struct CW1200_priv *driver_priv;
	struct delayed_work unload_work;
	struct mutex eil_sdwn_lock;
};


#define PRINT(args...)  printk(args);

/*DEBUG MACROS */
#define DBG_NONE		0x00000000
#define DBG_SBUS		0x00000003
#define DBG_EIL			0x00000003
#define DBG_CIL		  	0x00000003
#define DBG_ERROR		0x00000008
#define DBG_MESSAGE		0x00000010
#define DBG_SUS			0x00000020


#define DEBUG_LEVEL			(DBG_ERROR)

#if 1
#define K_DEBUG(f, m, args...)do { \
	if ((f & m)) \
		PRINT(args); \
} while (0)
#else
#define K_DEBUG(f, m, args...)do { \
	if ((f & m)) \
		printk(args); \
} while (0)
#endif

#if DEBUG_LEVEL > DBG_NONE
#define DEBUG(f, args...)	 K_DEBUG(f, DEBUG_LEVEL , args)
#else
#define DEBUG(f, args...)
#endif

#endif
