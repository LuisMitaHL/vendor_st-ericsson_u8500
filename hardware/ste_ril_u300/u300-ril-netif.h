/* ST-Ericsson U300 RIL
**
** Copyright (C) ST-Ericsson AB 2008-2010
** Copyright 2006, The Android Open Source Project
**
** Licensed under the Apache License, Version 2.0 (the "License");
** you may not use this file except in compliance with the License.
** You may obtain a copy of the License at
**
**     http://www.apache.org/licenses/LICENSE-2.0
**
** Unless required by applicable law or agreed to in writing, software
** distributed under the License is distributed on an "AS IS" BASIS,
** WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
** See the License for the specific language governing permissions and
** limitations under the License.
**
**  Author: Sjur Brendeland <sjur.brandeland@stericsson.com>
*/

#ifndef U300_RIL_NETIF_H
#define U300_RIL_NETIF_H 1

#include <inttypes.h>

/**
 * Set IPv4 address specified in  <addr-str> on network
 * interface <ifname>.
 * Return 0 on success and negative on error
 */
int rtnl_set_ipv4_addr(char *ifname, char *addr_str, int prefix);

/**
 * Set IPv6 link-local address specified in  <addr-str> on network
 * interface <ifname>.
 * Return 0 on success and negative on error
 */
int rtnl_set_ipv6_link_addr(char *ifname, char *addr_str);

/**
 * Enable network interface <ifname>.
 * Return 0 on success and negative on error
 */
int rtnl_if_enable(char *ifname);

/**
 * Disable network interface <ifname>.
 * Return 0 on success and negative on error
 */
int rtnl_if_disable(char *ifname);

/**
 * Set MTU on network interface <ifname>.
 * Return 0 on success and negative on error
 */
int rtnl_set_mtu(char *ifname, int mtu);

/**
 * Retrieve local IP address set on network interface <ifname>.
 * Only addresses with scope != link are considered.
 * Please note that the first matching address is returned in case
 * of multiple addresses on interface.
 * <family> - The address family to look for
 * <prefix_len> - Prefix length of the returned address
 * <ip_addr> - Returned address (should point at a network address
 *             structure corresponding to the family: 'struct in_addr'/
 *             'struct in6_addr')
 * Return 0 on success and negative on error/no address found
 */
int rtnl_get_first_ip_address(char *ifname, int family, int* prefix_len, void *ip_addr);

/**
 * Wait until a public IPv6 network prefix is assigned to network
 * interface <ifname>, then return the corresponding local IPv6 address.
 * <prefix_len> - Prefix length of the returned address
 * <prefix_addr> - Returned address (should point at a network address
 *                 structure corresponding to the family: 'struct in_addr'/
 *                 'struct in6_addr')
 * Return 0 on success and negative on error
 */
int rtnl_wait_for_ipv6_prefix(char *ifname, int* prefix_len, void *prefix_addr);

/**
 * Flush all IP addresses (with <family>) from network interface <ifname>.
 * Return 0 on success and negative on error
 */
int rtnl_flush_ip_addresses(char *ifname, int family);

#endif
