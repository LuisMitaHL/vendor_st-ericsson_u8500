/* ===========================================================================
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
 * ==========================================================================*/

/**
* DOC: EIL.C
*
* PROJECT	:	CW1200_LINUX_DRIVER
*
* FILE		:	eil.c
*
* This module interfaces with the Linux Kernel 802.3/Ethernet Layer.
*
* date 25/02/2010
*/


/****************************************************************************
*					INCLUDE FILES
*****************************************************************************/

#include <linux/netdevice.h>
#include <linux/etherdevice.h>
#include <linux/vmalloc.h>
#include <linux/random.h>
#include <linux/module.h>
#include <linux/firmware.h>

#include <net/ip.h>
#include "eil.h"
#include "cil.h"
#include "sbus.h"

MODULE_LICENSE("GPL");

/*****************************************************************************
*					LOCAL FUNCTION PROTOTYPES
******************************************************************************/

static int EIL_Stop(struct net_device *dev);

static int EIL_Open(struct net_device *dev);

static void EIL_Tx_Timeout(struct net_device *dev);

static void EIL_set_mc_list(struct net_device *dev);

static struct net_device_stats *EIL_Statistics(struct net_device *dev);

static void UMAC_bh(struct work_struct *work);
void  UMI_CB_Tx_Complete(UL_HANDLE UpperHandle,
		UMI_STATUS_CODE Status,
		UMI_TX_DATA *pTxData);

UMI_STATUS_CODE UMI_CB_Data_Received(UL_HANDLE	UpperHandle,
		UMI_STATUS_CODE	Status,
		uint16 Length,
		void *pFrame,
		void *pDriverInfo,
		void *pFrmStart,
		uint32 flags);
void UMI_CB_Schedule(UL_HANDLE	UpperHandle);

void UMI_CB_ConfigReqComplete(UL_HANDLE UpperHandle,
			UMI_CONFIG_CNF	*pConfigCnf);


/*
 *  * iwm AC to queue mapping
 *   *
 *    * AC_VO -> queue 3
 *     * AC_VI -> queue 2
 *      * AC_BE -> queue 1
 *       * AC_BK -> queue 0
 *       */
static const u16 cw1200_1d_to_queue[8] = { 1, 0, 0, 1, 2, 2, 3, 3 };

static uint8_t CW1200_MACADDR[6] = {0x00, 0x80, 0xe1, 0x11, 0x22, 0x53} ;

struct completion shutdown_comp;

static u16 EIL_select_queue(struct net_device *dev, struct sk_buff *skb)
{
	skb->priority = cfg80211_classify8021d(skb);

	return cw1200_1d_to_queue[skb->priority];
}

/*******************************************************************************
*				GLOBAL VARS
******************************************************************************/

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 30)
static const struct net_device_ops cw1200_netdev_ops = {
	.ndo_open		 = EIL_Open,
	.ndo_stop		 = EIL_Stop,
	.ndo_get_stats		 = EIL_Statistics,
	.ndo_start_xmit		 = EIL_Transmit,
	.ndo_tx_timeout		 = EIL_Tx_Timeout,
	//.ndo_set_multicast_list =  EIL_set_mc_list,
	.ndo_set_rx_mode =  EIL_set_mc_list,
	.ndo_select_queue	= EIL_select_queue,
};
#endif

static unsigned int arr_count;
static unsigned char mac_addr[MACADDR_LEN];
module_param_array(mac_addr, byte , &arr_count, S_IRUGO);
MODULE_PARM_DESC(mac_addr, "MAC address for CW1200 ");

#ifndef NO_QUIESCENT_MODE
static unsigned int mode = WLAN_QUIESCENT_MODE;
#else
static unsigned int mode = WLAN_ACTIVE_MODE;
#endif
module_param(mode, uint, S_IRUGO);
MODULE_PARM_DESC(mode, "CW1200 operational power mode: "
		 "0 - ACTIVE, 1 - DOZE, 2 - QUIESCENT");

#ifdef ENABLE_5GHZ
static bool dualband = true;
#else
static bool dualband = false;
#endif
module_param(dualband, bool, S_IRUGO);
MODULE_PARM_DESC(dualband, "CW1200 dual band 5GHz support: "
                 "0 - DISABLED, 1 - ENABLED");

/*******************************************************************************
*				FUNCTION DEFINITIONS
*******************************************************************************/



/**
* UMAC_bh
*
* This function implements the UMAC  bottom half handler
*
* @work: pointer to the Work Queue work_struct
*/
static void UMAC_bh(struct work_struct *work)
{
	struct CW1200_priv *priv ;
	priv  = container_of(work, struct CW1200_priv, umac_work);

	/*Service UMAC */
	UMI_Worker(priv->umac_handle);
}


/**
* set_mc_list
*
* This function sets the multicast filter if event is received
*
* @work: pointer to the Work Queue work_struct
*/
static void set_mc_list(struct work_struct *work)
{
	struct CW1200_priv *priv ;
	struct net_device *dev ;
	UMI_OID_802_11_MULTICAST_ADDR_FILTER *lmcfilter;
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,35)
	struct dev_mc_list *macaddr;
#else
	struct netdev_hw_addr *macaddr;
#endif
	int naddr = 0, i = 0;
	priv  = container_of(work, struct CW1200_priv, mc_work);
	lmcfilter = &(priv->ste_mc_filter);
	dev = priv->netdev;
	netif_addr_lock_bh(dev);
	naddr = netdev_mc_count(dev);
	DEBUG(DBG_EIL, "%s: no. of mc addrs %d\n", __func__, naddr);

	if(naddr > MCADDR_LIST_MAXSIZE)
		lmcfilter->numOfAddresses= MCADDR_LIST_MAXSIZE;
	else
		lmcfilter->numOfAddresses= naddr;
	netdev_for_each_mc_addr(macaddr, dev) {
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,35)
		memcpy(&(lmcfilter->AddressList[i]), macaddr->dmi_addr, ETH_ALEN);
#else
		memcpy(&(lmcfilter->AddressList[i]), macaddr->addr, ETH_ALEN);
#endif
		DEBUG(DBG_EIL, "adding mc address %pM \n"
			, lmcfilter->AddressList[i]);
		i = (i+1)%MCADDR_LIST_MAXSIZE;
		naddr--;
	}
	netif_addr_unlock_bh(dev);
	if (unlikely(naddr)) {
		/*wrong value of list size provided*/
		DEBUG(DBG_ERROR, "%s: wrong MC addr list size\n", __func__);
		lmcfilter->numOfAddresses = 0; /*The current list is unusable*/
		return ;
	}

	/* If Multicast address filtering is enabled and addrupdate is true
	 * Then set MC filter OID
	 */
	if(lmcfilter->enable) {
		i = UMI_mc_list(priv);
	}
	return ;
}


/**
* EIL_Init
*
* This function initialises the EIL layer. It registers the driver with the
* Linux 802.3 stack. Returns the Status code - Pass or failure code.
*
* @func: pointer to sdio_func passed by the Linux SDIO stack in probe function
*/
CW1200_STATUS_E EIL_Init(CW1200_bus_device_t *func)
{
	struct net_device *netdev = NULL;
	struct CW1200_priv *priv = NULL;
	CW1200_STATUS_E retval = SUCCESS;
	UMI_START_T  umi_start = {0};
	UMI_CREATE_IN umi_in   ; /*Input paramter to UMI_Create */
	UMI_CREATE_OUT umi_out ; /*Output paramter from UMI_Create */
	UMI_STATUS_CODE umac_retval = UMI_STATUS_SUCCESS;
	char *firmware_path = NULL;
	struct firmware *firmware;

	DEBUG(DBG_MESSAGE, "%s Called\n", __func__);
        printk(KERN_ERR"############# enter EIL_Init");
	netdev = alloc_etherdev_mq(sizeof(struct CW1200_priv), 4);
	priv = netdev_priv(netdev);
	strcpy(netdev->name, DEV_NAME);

	/*Some housekeeping info */
	priv->umac_status = UMAC_NOTSTARTED;

	atomic_set(&(priv->bk_count), 0);
	atomic_set(&(priv->be_count), 0);
	atomic_set(&(priv->vi_count), 0);
	atomic_set(&(priv->vo_count), 0);

	/*Create WORK Queue for UMAC servicing */
	priv->umac_WQ = create_singlethread_workqueue("umac_work");

	if (NULL == priv->umac_WQ) {
		DEBUG(DBG_ERROR, "%s Failed to allocate memory for umac_WQ\n", __func__);
		return ERROR;
	}
	/*1. Register UMAC BH */
	INIT_WORK(&priv->mc_work, set_mc_list);
	INIT_WORK(&priv->umac_work, UMAC_bh);

	/*For CW1200 the WLAN function number is 1. */
#ifndef USE_SPI
	func->num = 1;
#endif
	priv->func = func;

	priv->netdev = netdev;

#if LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 30)
	netdev->open	 = EIL_Open ;
	netdev->stop	 = EIL_Stop;
	netdev->hard_start_xmit	 = EIL_Transmit;
	netdev->get_stats	 = EIL_Statistics;
#else
	netdev->netdev_ops = &cw1200_netdev_ops;
#endif

#if LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 30)
	netdev->tx_timeout	 = EIL_Tx_Timeout;
#endif
	netdev->watchdog_timeo = EIL_TX_TIMEOUT;
	netdev->hard_header_len = EXTRA_MEMORY_SIZE + 8;

	/*Populate Input/Output Params */
	umi_in.apiVersion 	 = 0;
	umi_in.flags		 = 0;
	umi_in.ulHandle		 = (UL_HANDLE)priv;

	/*UPPER LAYER CALL_BACKS */
	umi_in.ulCallbacks.txComplete_Cb = UMI_CB_Tx_Complete;
	umi_in.ulCallbacks.dataReceived_Cb = UMI_CB_Data_Received;
	umi_in.ulCallbacks.schedule_Cb = UMI_CB_Schedule;
	umi_in.ulCallbacks.configReqComplete_Cb = UMI_CB_ConfigReqComplete;
	/*Populate callbacks implemented by CIL */
	umi_in.ulCallbacks.getParameterComplete_Cb
						= UMI_CB_GetParameterComplete;
	umi_in.ulCallbacks.setParameterComplete_Cb
						= UMI_CB_SetParameterComplete;
	umi_in.ulCallbacks.indicateEvent_Cb = UMI_CB_Indicate_Event;
	umi_in.ulCallbacks.scanInfo_Cb = UMI_CB_ScanInfo;
	umi_in.ulCallbacks.txMgmtFrmComplete_Cb = UMI_CB_TxMgmtFrmComplete;
	/*LOWER LAYER CALL-BACKS implemented by SBUS */
	umi_in.llCallbacks.create_Cb = UMI_CB_Create;
	umi_in.llCallbacks.start_Cb = UMI_CB_Start;
	umi_in.llCallbacks.destroy_Cb = UMI_CB_Destroy;
	umi_in.llCallbacks.stop_Cb = UMI_CB_Stop;
	umi_in.llCallbacks.scheduleTx_Cb = UMI_CB_ScheduleTx;
	umi_in.llCallbacks.rxComplete_Cb = UMI_CB_RxComplete;

	umac_retval = UMI_Create(&umi_in, &umi_out);
	if (UMI_STATUS_SUCCESS != umac_retval) {
		DEBUG(DBG_ERROR, "EIL_Init()"
			":UMI_Create returned error:%d \n", umac_retval);
		free_netdev(netdev);
		return ERROR;
	}
	priv->umac_handle = umi_out.umiHandle;

	DEBUG(DBG_ERROR, "Dual band 5GHz support: %s\n",
				dualband?"ENABLED":"DISABLED");
	priv->dualband = dualband;

	/*Initialise CIL Layer */
        printk(KERN_ERR"#############  call CIL_Init");
	retval = CIL_Init(priv);
	if (SUCCESS != retval) {
		UMI_Destroy(priv->umac_handle);
		free_netdev(netdev);
		/*Should we call UMI_Destroy herer */
		return ERROR;
	}
	/*UMI_RegisterEvents ()
	not implemented by UMAC yet.To be included when it is implemented.*/

	if (CUT11 == priv->cw1200_cut_no) {
		firmware_path = FIRMWARE_CUT11;
	} else if (CUT10 == priv->cw1200_cut_no) {
		firmware_path = FIRMWARE_CUT10;
	} else if (CUT2 == priv->cw1200_cut_no) {
		firmware_path = FIRMWARE_CUT20;
	} else {
		firmware_path = FIRMWARE_CUT22;
	}

	retval = request_firmware(&firmware, firmware_path, &(priv->func->dev));
	if (SUCCESS != retval) {
		CIL_Shutdown(priv);
		UMI_Destroy(priv->umac_handle);
		free_netdev(netdev);
		/*Should we call UMI_Destroy herer */
		return ERROR;
	}

	/*Populate param to be sent to UMI_Start () */
	umi_start.pFirmware = firmware->data;
	umi_start.fmLength = firmware->size;

	umac_retval = UMI_Start(priv->umac_handle, &umi_start);
	if (UMI_STATUS_SUCCESS != umac_retval) {
		DEBUG(DBG_EIL,
			"UMI_Start () returned error : [%d] \n", umac_retval);
		CIL_Shutdown(priv);
		free_netdev(netdev);
		UMI_Destroy(priv->umac_handle);
		free_netdev(netdev);
		/*Should we call UMI_Destroy herer */
		return ERROR;
	}
	/*Free the firware buffer */
	init_completion(&priv->cleanup_comp);
	init_completion(&shutdown_comp);
	release_firmware(firmware);
	mutex_init(&sdio_cw_priv.eil_sdwn_lock);
	priv->eil_shutdown = SHUTDOWN_NOT_STARTED;

	return SUCCESS;
}


/**
* EIL_Init_Complete
*
* This function receives the START_COMPLETED event from UMAC
* and completes the registration process with the UMAC.
*
* @priv: pointer to driver private structue.
*/
void EIL_Init_Complete(struct CW1200_priv *priv)
{
	struct firmware *firmware;
	CW1200_STATUS_E retval = SUCCESS;
	UMI_CONFIG_REQ  *pConfigReq = NULL;
	UMI_STATUS_CODE umi_status = UMI_STATUS_SUCCESS;
	uint32_t rand = 0;
	char *sdd_path = NULL;

	if (CUT11 == priv->cw1200_cut_no) {
		sdd_path = SDD_FILE_11;
	} else if (CUT10 == priv->cw1200_cut_no) {
		sdd_path = SDD_FILE_10;
	} else if (CUT2 == priv->cw1200_cut_no) {
		sdd_path = SDD_FILE_20;
	} else {
		sdd_path = SDD_FILE_22;
	}

	retval = request_firmware(&firmware, sdd_path, &(priv->func->dev));
	if (SUCCESS != retval) {
		DEBUG(DBG_ERROR, "SDD File I/O error\n");
		CIL_Shutdown(priv);
		UMI_Destroy(priv->umac_handle);
		free_netdev(priv->netdev);

		return;
	}

	pConfigReq = (UMI_CONFIG_REQ *)
	kmalloc(sizeof(UMI_CONFIG_REQ) + firmware->size - 4, GFP_KERNEL);

	/*Set to ZERO so that default values can be used */
	pConfigReq->dot11MaxReceiveLifeTime = 0;
	pConfigReq->dot11MaxTransmitMsduLifeTime = 0;
	pConfigReq->dot11RtsThreshold = 0;
	if (arr_count != MACADDR_LEN) {
		DEBUG(DBG_ERROR, "CORRECT MAC address not passed as module"
			"param\nUsing random MAC address\n");
		rand = random32();
		memcpy(&CW1200_MACADDR[3], &rand, 3);
		DEBUG(DBG_ERROR, "MAC address = [%pM]\n", CW1200_MACADDR);
	} else {
		memcpy(CW1200_MACADDR, mac_addr, MACADDR_LEN);
		DEBUG(DBG_ERROR, "MAC address = [%pM]\n", CW1200_MACADDR);
	}

	if (mode < 3)
	{
		DEBUG(DBG_ERROR, "Setting operational power mode: %d\n",
				 mode);
		priv->operational_power_mode = mode;
	}
	else
	{
		DEBUG(DBG_ERROR, "Using default operational power mode.\n");
#ifndef NO_QUIESCENT_MODE
		priv->operational_power_mode = WLAN_QUIESCENT_MODE;
#else
		priv->operational_power_mode = WLAN_ACTIVE_MODE;
#endif
	}

	memcpy(&pConfigReq->dpdData.macAddress[0],
		&CW1200_MACADDR[0], MACADDR_LEN);

	memcpy(&pConfigReq->dpdData.sddData,
		firmware->data, firmware->size);
	pConfigReq->dpdData.length = firmware->size + sizeof(UMI_HI_DPD) - 4;
	pConfigReq->dpdData.flags = 0x0005;

	umi_status = UMI_ConfigurationRequest(priv->umac_handle,
					sizeof(UMI_CONFIG_REQ)
					+ firmware->size - 4,
					pConfigReq);
	if ((UMI_STATUS_SUCCESS != umi_status) &&
					(UMI_STATUS_PENDING != umi_status)) {
		DEBUG(DBG_ERROR, "Config Request"
			" not sent successfully:ERROR: %d\n", umi_status);
		atomic_xchg(&(priv->cw1200_unresponsive), TRUE);
		schedule_delayed_work(&sdio_cw_priv.unload_work, 0);
	}
	release_firmware(firmware);
	kfree(pConfigReq);
}


/**
* EIL_Transmit
*
* This function is called by the Linux 802.3 stack to transmit a packet onto
* the device. returns  ERROR -> If the packet was not accepted by UMAC.
*		SUCCESS -> If the packet was accepted by the UMAC.
*
* @skb: pointer to the Linux Network Buffer.
* @dev: Network Device describing this driver.
*/
int EIL_Transmit(struct sk_buff *skb, struct net_device *dev)
{
	struct CW1200_priv *priv = netdev_priv(dev);
	UMI_TX_DATA *pTxData = NULL;
	UMI_STATUS_CODE umi_status = UMI_STATUS_SUCCESS;
	u16 queue=0;
	u8 prio = 0;
	struct ethhdr *eth = NULL;
	int32_t sta_link_id = 0;

	/*Check if the SKB has requisite space required by UMAC */

	if (skb_headroom(skb) < EXTRA_MEMORY_SIZE) {
		/*Expand header and create requisite memory space */
		if (pskb_expand_head(skb,
			EXTRA_MEMORY_SIZE, 0, GFP_ATOMIC)) {
			DEBUG(DBG_ERROR, "EIL_Transmit():"
			" Header expand failed \n");
			return -1 ;
		}
	}

	if (skb_cloned(skb)) {
		struct sk_buff *tmp_skb;

		DEBUG(DBG_EIL, "%s,Cloned SKB\n", __func__);

		tmp_skb = skb;
		skb = skb_copy(tmp_skb, GFP_ATOMIC);
		dev_kfree_skb_any(tmp_skb);
		if (!skb) {
			DEBUG(DBG_ERROR, "EIL_Transmit(): System out of memory \n");
			priv->stats.tx_errors++;
			return 0;
		}
	}

	if (!ALIGN4(skb->data)) {
		DEBUG(DBG_EIL , "EIL_Transmit()"
		":SKB-Data Unaligned [%p] \n", skb->data);
	}

	pTxData = kmalloc(sizeof(UMI_TX_DATA), GFP_ATOMIC);

	if (unlikely(NULL == pTxData)) {
		DEBUG(DBG_ERROR, "EIL_Transmit(): System out of memory \n");
		priv->stats.tx_errors++;
		dev_kfree_skb_any(skb);
		return 0;
	}
	pTxData->pExtraBuffer =
		(uint8_t *)((uint8_t *)(skb->data) - EXTRA_MEMORY_SIZE);
	pTxData->pEthHeader  		 = (uint8_t *) (skb->data);
	pTxData->pEthPayloadStart 	 = (uint8_t *) (skb->data + ETH_HLEN);
	pTxData->ethPayloadLen		 = skb->len   - ETH_HLEN;
	pTxData->pDriverInfo		 = (void *)skb;
	dev->trans_start = jiffies;
	/*calculate priority to be passed*/
	prio = cfg80211_classify8021d(skb);

	DEBUG(DBG_EIL, "priority prio=%02x\n", prio);
	queue = skb_get_queue_mapping(skb);

	DEBUG(DBG_EIL, "Queue=%02x\n", queue);

	/* Find the link for the destination addr */
	if (unlikely(NL80211_IFTYPE_AP == priv->device_mode)) {
		eth = (struct ethhdr *)skb->data;
		sta_link_id = map_mac_addr(priv, eth->h_dest);
		if (sta_link_id == -1) {
			priv->stats.tx_errors++;
			kfree(pTxData);
			dev_kfree_skb_any(skb);
			return 0;
		}
	}

	umi_status = UMI_Transmit(priv->umac_handle, prio, sta_link_id, pTxData);
	if (unlikely(UMI_STATUS_SUCCESS != umi_status)) {
		priv->stats.tx_errors++;
		kfree(pTxData);
		dev_kfree_skb_any(skb);
	} else {
		if (queue == AC_BK) {
			atomic_inc(&(priv->bk_count));
			if (atomic_read(&(priv->bk_count)) >= AC_BK_QUEUE  ) {
			DEBUG(DBG_EIL, "EIL_Transmit ():Going to stop BK Queue\n");
			netif_stop_subqueue(dev, queue);
			}
		} else if (queue == AC_BE) {
			atomic_inc(&(priv->be_count));
			if (atomic_read(&(priv->be_count)) >= AC_BE_QUEUE) {
				DEBUG(DBG_EIL, "EIL_Transmit ():Going to stop"
						"BE Queue\n");
				netif_stop_subqueue(dev, queue);
			}
		} else if (queue == AC_VI) {
			atomic_inc(&(priv->vi_count));
			if (atomic_read(&(priv->vi_count)) >= AC_VI_QUEUE) {
				DEBUG(DBG_EIL, "EIL_Transmit ():Going to stop"
						"VI Queue\n");
				netif_stop_subqueue(dev, queue);
			}
		} else if (queue == AC_VO) {
			atomic_inc(&(priv->vo_count));
			if (atomic_read(&(priv->vo_count)) >= AC_VO_QUEUE) {
				DEBUG(DBG_EIL, "EIL_Transmit ():Going to stop"
						"VO Queue\n");
				netif_stop_subqueue(dev, queue);
			}
		}
		priv->stats.tx_packets++;
	}

	return SUCCESS;
}


/**
* EIL_Stop
*
* This function is called by the Linux 802.3 stack to stop data transfer over
* the device. Always returns success.
*
* @dev: Network Device describing this driver.
*/
static int EIL_Stop(struct net_device *dev)
{
	struct CW1200_priv *priv = netdev_priv(dev);

	DEBUG(DBG_EIL, "EIL_Stop called  \n");
	netif_tx_stop_all_queues(dev);
        /* If any SCAN is in progress wait for it using a semaphore */
	if (priv->wait_event == UMI_EVT_SCAN_COMPLETED) {
		if (down_interruptible(&priv->cil_sem) < 0)
			return 0;

		DEBUG(DBG_EIL, "%s, Scan not in progress \n", __func__);
		up(&priv->cil_sem);
	}
	return 0;
}


/**
* EIL_Open
*
* This function is called by the Linux 802.3 stack to indicate to the
* driver that stack is ready to TX data. Always returns success.
*
* @dev:  Network Device describing this driver.
*/
static int EIL_Open(struct net_device *dev)
{

	DEBUG(DBG_EIL, "EIL_Open called \n");
	/*Start network queues so that driver can start receiving data */
	netif_tx_start_all_queues(dev);
	return 0;
}


/**
* EIL_Shutdown
*
* This function stops and deletes the UMAC instance and calls the CIL shutdown
* handler.
*
* @priv:   pointer to driver private structue.
*/

int EIL_Shutdown(struct CW1200_priv *priv)
{
	int retval = 0;

	printk(KERN_ERR, "EIL_Shutdown called\n");
	if (priv->eil_shutdown == SHUTDOWN_NOT_STARTED) {
		mutex_lock(&sdio_cw_priv.eil_sdwn_lock);
		priv->eil_shutdown = SHUTDOWN_STARTED;

		printk(KERN_ERR, "Calling unregister_netdev and CIL_Shutdown\n");

		if (priv->netdev->reg_state == NETREG_REGISTERED)
			unregister_netdev(priv->netdev);

		CIL_Shutdown(priv);

		UMI_Stop(priv->umac_handle);
		printk(KERN_ERR, "Waiting for UMAC STOP Confirmation\n");
		retval = wait_for_completion_timeout(&priv->cleanup_comp,
				msecs_to_jiffies(CLEANUP_TIMEOUT));

		if (retval)
			printk(KERN_ERR, "UMAC STOP Confirmation received\n");
		else
			printk(KERN_ERR, "Timeout:UMAC STOP Not received\n");

		if (work_pending(&priv->umac_work)) {
			printk(KERN_ERR,"%s Cancelling pending umac_work\n",__func__);
			cancel_work_sync(&priv->umac_work);
		}

		printk(KERN_ERR, "UMI_Destroy from cleanup\n");
		UMI_Destroy(priv->umac_handle);

		flush_workqueue(priv->umac_WQ);
		destroy_workqueue(priv->umac_WQ);

		sdio_cw_priv.driver_priv = NULL;
		free_netdev(priv->netdev);
		mutex_unlock(&sdio_cw_priv.eil_sdwn_lock);
		/*
		 *      Since unresponsive_driver may get scheduled before or after
		 *      EIL_Shutdown(), so we can not destroy this mutex.
		 */

		complete(&shutdown_comp);
		return SHUTDOWN_SUCCESS;
	} else {
		return SHUTDOWN_PENDING;
	}
}


/**
* EIL_Statistics
*
* This function is called by the Linux 802.3 stack to get driver statistic
* (RX/TX count etc). Returns the pointer to net_device_stats
*
* @dev: Network Device describing this driver.
*/
static struct net_device_stats *EIL_Statistics(struct net_device *dev)
{
	struct CW1200_priv *priv = netdev_priv(dev);

	dev->stats.tx_errors = priv->stats.tx_errors;
	dev->stats.tx_packets = priv->stats.tx_packets;
	dev->stats.rx_packets = priv->stats.rx_packets;

	return &dev->stats;
}


/**
* EIL_Tx_Timeout
*
* This function is called by the Linux 802.3 stack whenever Tx timeout is
* detected.
*
* @dev: Network Device describing this driver.
*/
static void EIL_Tx_Timeout(struct net_device *dev)
{
	struct CW1200_priv *priv = netdev_priv(dev);

	DEBUG(DBG_EIL, "EIL:Tx Timeout detected by Linux Kernel\n");
	priv->stats.tx_errors++;
        atomic_xchg(&(priv->Interrupt_From_Device), TRUE);
        queue_work(priv->sbus_WQ, &priv->sbus_work);
}


/**
* EIL_set_mc_list
*
* This function is called by the Linux 802.3 stack to notify change in
* multicast address filter.
*
* @dev: Network Device describing this driver.
*/
static void EIL_set_mc_list(struct net_device *dev)
{
	struct CW1200_priv *priv = netdev_priv(dev);

	DEBUG(DBG_EIL, "MCaddress list update detected by Linux Kernel \n");
	queue_work(priv->bss_loss_WQ, &priv->mc_work);
}


/*****************************************************************************
*			UMAC Callback Functions
******************************************************************************/

/**
* UMI_CB_Tx_Complete
*
* This function is called by UMAC to indicate to the driver that TX
* to the device is complete and the driver can free the buffer.
*
* @UpperHandle: The upper layer driver instance for UMI callback.
* @Status: Completion status code.
* @pTxData:  structure that contains frame which transmitted on air.
*/
void  UMI_CB_Tx_Complete(UL_HANDLE UpperHandle,
			UMI_STATUS_CODE Status,
			UMI_TX_DATA *pTxData)
{
	struct CW1200_priv *priv = (struct CW1200_priv *)UpperHandle;
	uint32_t queue=0;

	DEBUG(DBG_EIL, "UMI_CB_Tx_Complete Status : [%d] \n", Status);

	if (NULL != pTxData) {
		queue = skb_get_queue_mapping((struct sk_buff *)pTxData->pDriverInfo);

		DEBUG(DBG_EIL,"UMI_CB_Tx_Complete : Queue [%d] \n",queue);
		netif_tx_lock_bh(priv->netdev);
		if ( netif_tx_queue_stopped(netdev_get_tx_queue(priv->netdev, queue)) ) {
			netif_tx_wake_queue(netdev_get_tx_queue(priv->netdev, queue));
		}
		netif_tx_unlock_bh(priv->netdev);

		if (queue == AC_BK) {
			atomic_dec(&priv->bk_count);
		} else if (queue == AC_BE) {
			atomic_dec(&priv->be_count);
		} else if (queue == AC_VI) {
			atomic_dec(&priv->vi_count);
		} else if (queue == AC_VO) {
			atomic_dec(&priv->vo_count);
		}
		dev_kfree_skb_any((struct sk_buff *)pTxData->pDriverInfo);
		kfree(pTxData);
	}
}


/**
* UMI_CB_Data_Received
*
*This function is called by UMAC to indicate to the driver
* that UMAC has received a packet from the device.
* returns the status code from UMAC.
*
* @UpperHandle: The upper layer driver instance for UMI callback.
* @Status: Completion status code.
* @pTxData:  structure that contains frame which transmitted on air.
*/
UMI_STATUS_CODE UMI_CB_Data_Received(UL_HANDLE	UpperHandle,
				UMI_STATUS_CODE	Status,
				uint16 Length,
				void *pFrame, void *pDriverInfo,
				void *pFrmStart, uint32 flags)
{
	struct CW1200_priv *priv = (struct CW1200_priv *)UpperHandle;
	struct sk_buff *skb = NULL;
	struct sk_buff *fw_skb = NULL;


	if (UMI_STATUS_SUCCESS != Status) {
		DEBUG(DBG_ERROR, "UMI_CB_Data_Received ()"
		"Callback returned error [%x]\n", Status);
		skb = (struct sk_buff *)pDriverInfo;
		dev_kfree_skb_any(skb);
		return UMI_STATUS_SUCCESS;
	}

	if (0 == Length) {
		DEBUG(DBG_ERROR, "UMI_CB_Data_Received () Invalid Length \n");
		return UMI_STATUS_SUCCESS;
	}

	if (unlikely(flags & MORE_DATA_FLAG)) {
		DEBUG(DBG_EIL, "%s: More Data Flag is set \n", __func__);
		if (flags & FRAME_COUNT) {
			skb = dev_alloc_skb(Length);
			if (!skb) {
				DEBUG(DBG_EIL, "%s:Out of Memory \n", __func__);
				atomic_xchg(&(priv->cw1200_unresponsive), TRUE);
				schedule_delayed_work(&sdio_cw_priv.unload_work, 0);
				return UMI_STATUS_SUCCESS;
			}
			skb_put(skb, Length);
			memcpy(skb->data, pFrame , Length);
		} else {
			DEBUG(DBG_EIL,"%s,Unknown Error:No Frame count \n",
					__func__);
			return UMI_STATUS_FAILURE;
		}
	} else {
		skb = (struct sk_buff *)pDriverInfo;
		DEBUG(DBG_EIL, "%s, Retrieved SKB pointer&SKB Len[%p],"
			"[%d], [%d] \n", __func__, skb, skb->len, Length);
		skb->data = pFrame;
		skb->len = Length;
		skb->tail = skb->data + (Length - 1);
	}

	if (unlikely(flags & BRIDGED_FRAME)) {
		fw_skb = skb_copy(skb , GFP_ATOMIC);
		if (!fw_skb) {
			DEBUG(DBG_EIL," Not able to allocate"
					"bridged frame \n");
			return UMI_STATUS_SUCCESS;
		}
		skb = fw_skb;
	}

	priv->stats.rx_packets++;
	priv->stats.rx_bytes += Length;
	skb->dev = priv->netdev;
	skb->protocol = eth_type_trans(skb, priv->netdev);
	skb->ip_summed = CHECKSUM_NONE;
	/*Give the packet to the Linux Kernel 802.3 stack */
	if(priv->eil_shutdown == SHUTDOWN_NOT_STARTED) {
		netif_rx_ni(skb);
	} else {
		dev_kfree_skb_any(skb);
	}

	return UMI_STATUS_SUCCESS;
}


/**
* UMI_CB_Schedule
*
* This function is called by UMAC to ask the driver for context.
*
* @UpperHandle: The upper layer driver instance for UMI callback.
*/
void UMI_CB_Schedule(UL_HANDLE	UpperHandle)
{

	struct CW1200_priv *priv = (struct CW1200_priv *)UpperHandle;

	/*Schedule Bottom Half to service UMAC */
	queue_work(priv->umac_WQ, &priv->umac_work);
}


/**
* UMI_CB_ConfigReqComplete
*
* This callback function indicates to the drive that the configuration request
* is completed.
*
* @UpperHandle: The upper layer driver instance for UMI callback.
* @pConfigCnf: indicate stationid and config request status
*/
void UMI_CB_ConfigReqComplete(UL_HANDLE UpperHandle,
			UMI_CONFIG_CNF *pConfigCnf)

{
	struct CW1200_priv *priv = (struct CW1200_priv *)UpperHandle;
	uint32_t retval = SUCCESS;
	uint32_t filter_enable = 1;
	uint32_t Disable_80211d = 0;
	uint32_t oprPowerMode = 0;
	/* Enable multiTX conf */
	uint32_t useMultiTxConfMsg = 1;
	uint32_t bg_scan=0;

	DEBUG(DBG_EIL, "ConfigReqComplete Called \n");

	if (UMI_STATUS_SUCCESS != pConfigCnf->result) {
		DEBUG(DBG_ERROR, "CONFIG Request"
		"Complete callback returned error \n");
		free_netdev(priv->netdev);
		UMI_Destroy(priv->umac_handle);
	} else {
		priv->umac_status = UMAC_STARTED;
		retval = CIL_Set(priv,
				UMI_DEVICE_OID_802_11_SET_HOST_MSG_TYPE_FILTER,
				&filter_enable, 4);
		if (retval) {
			DEBUG(DBG_CIL, "CIL_Set() for UMI_DEVICE"
			"_OID_802_11_SET_DATA_TYPE_FILTER returned error  \n");
		}
		DEBUG(DBG_EIL, "EIL:MAC address from MAC:%pM \n",
				&(pConfigCnf->dot11StationId[0]));
		/*Completed network device registration */
		memcpy(priv->netdev->dev_addr, &(pConfigCnf->dot11StationId[0]),
			 ETH_ALEN);
		register_netdev(priv->netdev);
		retval = CIL_Set(priv,
				UMI_DEVICE_OID_802_11_ENABLE_11D_FEATURE,
				&Disable_80211d, 4);
		if (retval) {
			DEBUG(DBG_EIL, "%s :CIL_Set for"
			"80211 D returned error \n", __func__);
		}
		/* Enable Awake mode  */
		oprPowerMode = WLAN_ACTIVE_MODE;
		retval = CIL_Set(priv,
				UMI_DEVICE_OID_802_11_OPERATIONAL_POWER_MODE,
				&oprPowerMode, 4);
		if (retval) {
			DEBUG(DBG_EIL, "CIL_Set() for UMI_DEVICE"
			"_OID_802_11_OPERATIONAL_POWER_MODE returned error  \n");
		}

		retval = CIL_Set(priv,
				UMI_DEVICE_OID_802_11_MULTI_TX_CONFIRMATION,
				&useMultiTxConfMsg, 4);
		if (retval) {
			DEBUG(DBG_EIL, "CIL_Set() for UMI_DEVICE"
			"_OID_802_11_MULTI_TX_CONFIRMATION returned error  \n");
		}

		/*Enable BG Scan */
		bg_scan = 0;
		retval = CIL_Set(priv,
			UMI_DEVICE_OID_802_11_DISABLE_BG_SCAN,
			&bg_scan, 4);
		if (retval) {
			DEBUG(DBG_EIL,
			"cw1200_connect():CIL_Set for BG Scan returned error \n");
			return ;
		}
	}
	return;
}

/* This function maps Destination MAC Address to Link ID */
int32_t map_mac_addr(struct CW1200_priv *priv, uint8_t *mac_addr)
{
	int32_t sta_link_id = 0;
	int i;
	UMI_EVT_DATA_CONNECT_INFO *sta_list = NULL;

	if (unlikely(is_multicast_ether_addr(mac_addr))) {
		sta_link_id = 0;
	} else {
		for (i = 0; i < MAX_SUPPORTED_STA; i++) {
			sta_list = (UMI_EVT_DATA_CONNECT_INFO *)
					(priv->sta_info + i);
			if (compare_ether_addr(mac_addr,
				&(sta_list->stationMacAddr[0])) == 0) {
				sta_link_id = sta_list->linkId;
				break;
			}
		}
		if (i >= MAX_SUPPORTED_STA) {
			DEBUG(DBG_ERROR, "%s:STA not found "
				"in internal list\n", __func__);
			priv->stats.tx_errors++;
			sta_link_id = -1;
		}
	}

	return sta_link_id;
}

