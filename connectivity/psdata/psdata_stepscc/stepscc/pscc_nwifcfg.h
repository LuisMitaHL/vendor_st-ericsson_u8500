/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 * \brief  PS Connection Control - Network interface configuration
 */

#ifndef PSCC_NWIFCFG_H
#define PSCC_NWIFCFG_H

/********************************************************************************
 *
 * Include files
 *
 ********************************************************************************/
#include "pscc_msg.h"

/********************************************************************************
 *
 * Defines & Type definitions
 *
 ********************************************************************************/


/*******************************************************************************
 *
 * Functions and macros
 *
 *******************************************************************************/

/**
 * pscc_nwifcfg_open - open the control socket
 *
 * @pdp_type      the pdp type of the connection
 *
 * Returns: 0 on success, -1 on error
 **/
int pscc_nwifcfg_open(pscc_pdp_type_t pdp_type);

/**
 * pscc_nwifcfg_close - close the control socket
 *
 * Returns: 0 on success, -1 on error
 **/
int pscc_nwifcfg_close(void);

/**
 * pscc_nwifcfg_if_up - take up the network interface
 *
 * @nwifname      the name of the network interface
 *
 * Returns: 0 on success, -1 on error
 **/
int pscc_nwifcfg_if_up(char* nwifname);

/**
 * pscc_nwifcfg_if_down - take down the network interface
 *
 * @nwifname      the name of the network interface
 *
 * Returns: 0 on success, -1 on error
 **/
int pscc_nwifcfg_if_down(char* nwifname);

/**
 * pscc_nwifcfg_assign_ip - assign an ip to the network interface
 *
 * @nwifname      the name of the network interface
 * @ipv4address  the ipv4address that should be assigned
 * @addr_type     the type of address to be assigned
 *
 * Returns: 0 on success, -1 on error
 **/
int pscc_nwifcfg_assign_ip(char* nwifname, char* ipv4address, int addr_type);

/**
 * pscc_nwifcfg_set_default_route - set the default route to the netdevice
 *
 * @nwifname      the name of the network interface
 *
 * Returns: 0 on success, -1 on error
 **/
int pscc_nwifcfg_set_default_route(char* nwifname);

/**
 * pscc_nwifcfg_set_txqueuelen - set the txqueue length of the network interface
 *
 * @nwifname      the name of the network interface
 * @txqueuelen    the length of the txqueue
 *
 * Returns: 0 on success, -1 on error
 **/
int pscc_nwifcfg_set_txqueuelen(char* nwifname, uint32_t txqueuelen);

/**
 * pscc_nwifcfg_set_mtu - set the mtu of the network interface
 *
 * @nwifname      the name of the network interface
 * @mtu           the mtu
 *
 * Returns: 0 on success, -1 on error
 **/
int pscc_nwifcfg_set_mtu(char* nwifname, uint32_t mtu);

/**
 * pscc_nwifcfg_rename_nwif - rename the network interface
 *
 * @old_nwifname   the old (current) name of the network interface
 * @new_nwifname   the new name of the network interface
 *
 * Returns: 0 on success, -1 on error
 **/
int pscc_nwifcfg_rename_nwif(char* old_nwifname, char* new_nwifname);

/**
 * pscc_nwifcfg_reset_connections - reset all sockets associated with the interface address
 *
 * @nwifname      the name of the network interface
 *
 * Returns: 0 on success, -1 on error
 **/
int pscc_nwifcfg_reset_connections(char* nwifname);

#endif //PSCC_NWIFCFG_H


