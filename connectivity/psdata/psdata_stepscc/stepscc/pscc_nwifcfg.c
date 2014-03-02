/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 * \brief  PS Connection Control - Network interface configuration
 */

/********************************************************************************
 *
 * Include files
 *
 ********************************************************************************/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <linux/route.h>
#include <linux/ipv6.h>
#include <linux/ipv6_route.h>
#include <linux/sockios.h>
#include <sys/ioctl.h>
#include "pscc_msg.h"
#include "psccd.h"
#include "pscc_nwifcfg.h"

/********************************************************************************
 *
 * Defines & Type definitions
 *
 ********************************************************************************/

/********************************************************************************
 *
 * Local variables
 *
 ********************************************************************************/
static int ioctl_socket = -1;
static int ioctl_ipv4_socket = -1;
static pscc_pdp_type_t ioctl_pdp_type = -1;

/********************************************************************************
 *
 * Private function prototypes
 *
 ********************************************************************************/


/*******************************************************************************
 *
 * Public Functions
 *
 *******************************************************************************/

/**
 * pscc_nwifcfg_open()
 **/
int pscc_nwifcfg_open(pscc_pdp_type_t pdp_type)
{
  int fd = -1;
  int af = AF_INET; /* address family */

  PSCC_DBG_TRACE(LOG_DEBUG, "Opening network configuration socket for pdp_type %d.\n", pdp_type);

  /* check if already open socket */
  if (ioctl_socket > 0)
  {
    PSCC_DBG_TRACE(LOG_WARNING, "NWIFCFG socket already opened.\n");
    return -1;
  }

  switch(pdp_type)
  {
  /* ipv4 */
  case pscc_pdp_type_ipv4:
  case pscc_pdp_type_test:
    af = AF_INET;
    break;
  /* ipv6 */
  case pscc_pdp_type_ipv6:
  case pscc_pdp_type_ipv4v6:
    af = AF_INET6;
    break;
  default:
    PSCC_DBG_TRACE(LOG_WARNING, "Invalid pdp type (%d).\n", pdp_type);
    return -1;
  }

  /* open a socket for the ioctl calls */
  fd = socket(af, SOCK_DGRAM, 0);
  if (0 > fd)
  {
    PSCC_DBG_TRACE(LOG_WARNING, "Failed to open ioctl socket, error: %s \n", strerror(errno));
    return -1;
  }

  ioctl_pdp_type = pdp_type;
  ioctl_socket = fd;

  if (pscc_pdp_type_ipv4v6 == ioctl_pdp_type) {
    /* open a socket for the ioctl calls */
    ioctl_ipv4_socket = socket(AF_INET, SOCK_DGRAM, 0);
    if (0 > ioctl_ipv4_socket) {
      PSCC_DBG_TRACE(LOG_WARNING, "Failed to open ioctl ipv4 socket for pdp type ipv4v6, error: %s \n", strerror(errno));
      if(pscc_nwifcfg_close() < 0)
      {
        PSCC_DBG_TRACE(LOG_ERR, "failed to close nwifcfg socket.\n");
      }
      return -1;
    } else {
        PSCC_DBG_TRACE(LOG_WARNING, "Created ipv4 socket to handle ipv4 for pdp type ipv4v6 \n", ioctl_ipv4_socket);
    }
  }

  return 0;
}

/**
 * pscc_nwifcfg_close()
 **/
int pscc_nwifcfg_close(void)
{
  int err = 0;
  PSCC_DBG_TRACE(LOG_DEBUG, "Closing network configuration socket.\n");

  if ((0 <= ioctl_socket) || (0 <= ioctl_ipv4_socket))
  {
    if (0 <= ioctl_socket)
      close(ioctl_socket);
    if (0 <= ioctl_ipv4_socket)
      close(ioctl_ipv4_socket);
  }

  else
  {
    PSCC_DBG_TRACE(LOG_WARNING, "NWIFCFG sockets are already closed.\n");
    return -1;
  }

  ioctl_socket = -1;
  ioctl_pdp_type = -1;
  ioctl_ipv4_socket = -1;

  return 0;
}

/**
 * pscc_nwifcfg_if_up()
 **/
int pscc_nwifcfg_if_up(char* nwifname)
{
  struct ifreq ifr;
  PSCC_DBG_TRACE(LOG_DEBUG, "Taking up network interface %s\n", nwifname);

  if (0 > ioctl_socket)
  {
    PSCC_DBG_TRACE(LOG_WARNING, "NWIFCFG socket not initialized, call pscc_nwifcfg_open first.\n");
    return -1;
  }

  /* fetch current flags */
  memset((char *) &ifr, 0, sizeof(struct ifreq));
  strncpy(ifr.ifr_name, nwifname, IFNAMSIZ);
  ifr.ifr_name[IFNAMSIZ - 1] = 0;

  if (ioctl(ioctl_socket, SIOCGIFFLAGS, &ifr) < 0) {
    PSCC_DBG_TRACE(LOG_WARNING, "Failed to get flags, error: %s \n", strerror(errno));
    return -1;
  }

  ifr.ifr_flags = ifr.ifr_flags | IFF_UP;
  /* update with flag up set */
  if (ioctl(ioctl_socket, SIOCSIFFLAGS, &ifr) < 0) {
    PSCC_DBG_TRACE(LOG_WARNING, "Failed to set flags, error: %s \n", strerror(errno));
    return -1;
  }

  return 0;
}

/**
 * pscc_nwifcfg_if_down()
 **/
int pscc_nwifcfg_if_down(char* nwifname)
{
  struct ifreq ifr;
  PSCC_DBG_TRACE(LOG_DEBUG, "Taking down network interface %s\n", nwifname);

  if (0 > ioctl_socket)
  {
    PSCC_DBG_TRACE(LOG_WARNING, "NWIFCFG socket not initialized, call pscc_nwifcfg_open first.\n");
    return -1;
  }

  /* fetch current flags */
  memset((char *) &ifr, 0, sizeof(struct ifreq));
  strncpy(ifr.ifr_name, nwifname, IFNAMSIZ);
  ifr.ifr_name[IFNAMSIZ - 1] = 0;

  if (ioctl(ioctl_socket, SIOCGIFFLAGS, &ifr) < 0) {
    PSCC_DBG_TRACE(LOG_WARNING, "Error: Failed to get flags, error: %s \n", strerror(errno));
    return -1;
  }

  ifr.ifr_flags = ifr.ifr_flags & (~IFF_UP);
  /* update with flag up not set */
  if (ioctl(ioctl_socket, SIOCSIFFLAGS, &ifr) < 0) {
    PSCC_DBG_TRACE(LOG_WARNING, "Error: Failed to set flags, error: %s \n", strerror(errno));
    return -1;
  }

  return 0;
}

/**
 * pscc_nwifcfg_assign_ip()
 **/
int pscc_nwifcfg_assign_ip(char* nwifname, char* ipaddress, int addr_type)
{

  if ((NULL == nwifname) || (NULL == ipaddress))
  {
    PSCC_DBG_TRACE(LOG_WARNING, "Inparameter is null.\n");
    return -1;
  }

  if (0 > ioctl_socket)
  {
    PSCC_DBG_TRACE(LOG_WARNING, "NWIFCFG socket not initialized, call pscc_nwifcfg_open first.\n");
    return -1;
  }

  /* ipv4 */
  if ((pscc_pdp_type_ipv4 == addr_type) &&
     ((pscc_pdp_type_ipv4 == ioctl_pdp_type) || (pscc_pdp_type_test == ioctl_pdp_type) || (pscc_pdp_type_ipv4v6 == ioctl_pdp_type)))
  {
    struct ifreq ifr;
    struct sockaddr_in* sockaddr_p;
    in_addr_t addr;
    int socketfd = ioctl_socket;

    PSCC_DBG_TRACE(LOG_DEBUG, "Assigning ipv4address to network interface %s\n", ipaddress, nwifname);
    /* check if valid ipv4 address */
    if (inet_pton(AF_INET, ipaddress, &addr) <= 0) {
      PSCC_DBG_TRACE(LOG_WARNING, "Failed to convert ipv4 address %s \n", ipaddress);
      return -1;
    }

    memset((char *) &ifr, 0, sizeof(struct ifreq));
    sockaddr_p = (struct sockaddr_in*) &(ifr.ifr_addr);
    strncpy((char*) ifr.ifr_name, nwifname, IFNAMSIZ);
    ifr.ifr_name[IFNAMSIZ - 1] = 0;
    sockaddr_p->sin_family = AF_INET;
    sockaddr_p->sin_port = 0;
    sockaddr_p->sin_addr.s_addr = addr;

    if (pscc_pdp_type_ipv4v6 == ioctl_pdp_type)
      socketfd = ioctl_ipv4_socket;
    if (ioctl(socketfd, SIOCSIFADDR, &ifr) < 0) {
      /* if address is already set, we ignore this step */
      if (EEXIST == errno)
      {
        PSCC_DBG_TRACE(LOG_INFO, "Interface address already assigned.\n");
      }

      else
      {
        PSCC_DBG_TRACE(LOG_WARNING, "Failed to assign ipv4address %s to interface %s, error: %s \n", ipaddress, nwifname, strerror(errno));
        return -1;
      }
    }
  }

  /* ipv6 */
  else if ((pscc_pdp_type_ipv6 == addr_type) &&
     ((pscc_pdp_type_ipv6 == ioctl_pdp_type) || (pscc_pdp_type_ipv4v6 == ioctl_pdp_type)))
  {
    struct in6_ifreq ifr6;
    struct ifreq ifreq;

    PSCC_DBG_TRACE(LOG_DEBUG, "Assigning ipv6address to network interface %s\n", ipaddress, nwifname);

    /* check if valid ipv6 address */
    if (inet_pton(AF_INET6, ipaddress, &ifr6.ifr6_addr.s6_addr) <= 0) {
      PSCC_DBG_TRACE(LOG_WARNING, "Failed to convert ipv6 address %s\n", ipaddress);
      return -1;
    }

    /* fetch the if index */
    memset((char *) &ifreq, 0, sizeof(ifreq));
    strncpy(ifreq.ifr_name, nwifname, IFNAMSIZ - 1);
    if (ioctl(ioctl_socket, SIOCGIFINDEX, &ifreq) < 0) {
      PSCC_DBG_TRACE(LOG_WARNING, "Failed to fetch interface %s index, error: %s \n", nwifname, strerror(errno));
      return -1;
    }

    ifr6.ifr6_ifindex = ifreq.ifr_ifindex;
    ifr6.ifr6_prefixlen = 64;

    if (ioctl(ioctl_socket,SIOCSIFADDR,&ifr6) < 0)
    {
      /* if address is already set, we ignore this step */
      if (EEXIST == errno)
      {
        PSCC_DBG_TRACE(LOG_INFO, "Interface address already assigned.\n");
      }

      else
      {
        PSCC_DBG_TRACE(LOG_WARNING, "Failed to assign ipaddress %s to interface %s, error: %s \n", ipaddress, nwifname, strerror(errno));
        return -1;
      }
    }
  }

  /* neither ipv4 (and test) nor ipv6? */
  else
  {
    PSCC_DBG_TRACE(LOG_ERR, "Invalid pdp type (%d).\n", ioctl_pdp_type);
    return -1;
  }

  return 0;
}

/**
 * pscc_nwifcfg_set_default_route()
 **/
int pscc_nwifcfg_set_default_route(char* nwifname)
{
  PSCC_DBG_TRACE(LOG_DEBUG, "Setting default route for %s\n", nwifname);

  if (NULL == nwifname)
  {
    PSCC_DBG_TRACE(LOG_WARNING, "Inparameter is null.\n");
    return -1;
  }

  if (0 > ioctl_socket)
  {
    PSCC_DBG_TRACE(LOG_WARNING, "NWIFCFG socket not initialized, call pscc_nwifcfg_open first.\n");
    return -1;
  }

  /* ipv4 */
  if (pscc_pdp_type_ipv4 == ioctl_pdp_type || pscc_pdp_type_test == ioctl_pdp_type)
  {
    struct sockaddr_in *saddr_p;
    struct rtentry route;
    memset((char *) &route, 0, sizeof(route));

    route.rt_dst.sa_family = AF_INET;
    route.rt_flags = RTF_UP;
    route.rt_dev = (void *) nwifname;
    saddr_p = (struct sockaddr_in*) &route.rt_genmask;
    saddr_p->sin_family = AF_INET;

    if (ioctl(ioctl_socket, SIOCADDRT, &route) < 0) {
      PSCC_DBG_TRACE(LOG_WARNING, "Failed to set default route, error: %s \n", strerror(errno));
      return -1;
    }
  }

  /* ipv6 */
  else if ((pscc_pdp_type_ipv6 == ioctl_pdp_type) || (pscc_pdp_type_ipv4v6 == ioctl_pdp_type))
  {
    struct ifreq ifreq;
    struct in6_rtmsg rt;

    /* fetch the if index */
    memset((char *) &ifreq, 0, sizeof(ifreq));
    strncpy(ifreq.ifr_name, nwifname, IFNAMSIZ - 1);
    if (ioctl(ioctl_socket, SIOCGIFINDEX, &ifreq) < 0) {
      PSCC_DBG_TRACE(LOG_WARNING, "Failed to fetch interface index, error: %s \n", strerror(errno));
      return -1;
    }

    /* set default route to network interface */
    memset((char *) &rt, 0, sizeof(struct in6_rtmsg));
    rt.rtmsg_dst_len = 0;
    rt.rtmsg_flags = RTF_UP;
    /* metric 1024 is the priority of the routeentry */
    rt.rtmsg_metric = 1024;
    rt.rtmsg_ifindex = ifreq.ifr_ifindex;

    if (ioctl(ioctl_socket, SIOCADDRT, &rt) < 0) {
      PSCC_DBG_TRACE(LOG_WARNING, "Failed to set default route, error: %s \n", strerror(errno));
      return -1;
    }
  }

  /* neither ipv4 (and test) nor ipv6? */
  else
  {
    PSCC_DBG_TRACE(LOG_ERR, "Invalid pdp type (%d).\n", ioctl_pdp_type);
    return -1;
  }

  return 0;

}

/**
 * pscc_nwifcfg_set_txqueuelen()
 **/
int pscc_nwifcfg_set_txqueuelen(char* nwifname, uint32_t txqueuelen)
{
  struct ifreq ifreq;

  PSCC_DBG_TRACE(LOG_DEBUG, "Setting txqueue length to %d for %s\n", txqueuelen, nwifname);

  if (NULL == nwifname)
  {
    PSCC_DBG_TRACE(LOG_WARNING, "Inparameter is null.\n");
    return -1;
  }

  if (0 >= txqueuelen)
  {
    PSCC_DBG_TRACE(LOG_WARNING, "txqueuelen must be greater than 0.\n");
    return -1;
  }

  if (0 > ioctl_socket)
  {
    PSCC_DBG_TRACE(LOG_WARNING, "NWIFCFG socket not initialized, call pscc_nwifcfg_open first.\n");
    return -1;
  }

  memset((char *) &ifreq, 0, sizeof(ifreq));
  strncpy(ifreq.ifr_name, nwifname, IFNAMSIZ - 1);
  ifreq.ifr_qlen = txqueuelen;

  if (ioctl(ioctl_socket, SIOCSIFTXQLEN, &ifreq) < 0) {
    PSCC_DBG_TRACE(LOG_WARNING, "Failed to set txqueue length, error: %s \n", strerror(errno));
    return -1;
  }

  return 0;

}

/**
 * pscc_nwifcfg_set_mtu()
 **/
int pscc_nwifcfg_set_mtu(char* nwifname, uint32_t mtu)
{
  struct ifreq ifreq;

  PSCC_DBG_TRACE(LOG_DEBUG, "Setting mtu to %d for %s\n", mtu, nwifname);

  if (NULL == nwifname)
  {
    PSCC_DBG_TRACE(LOG_WARNING, "Inparameter is null.\n");
    return -1;
  }

  if (0 >= mtu)
  {
    PSCC_DBG_TRACE(LOG_WARNING, "mtu must be greater than 0.\n");
    return -1;
  }

  if (0 > ioctl_socket)
  {
    PSCC_DBG_TRACE(LOG_WARNING, "NWIFCFG socket not initialized, call pscc_nwifcfg_open first.\n");
    return -1;
  }

  memset((char *) &ifreq, 0, sizeof(ifreq));
  strncpy(ifreq.ifr_name, nwifname, IFNAMSIZ - 1);
  ifreq.ifr_mtu = mtu;

  if (ioctl(ioctl_socket, SIOCSIFMTU, &ifreq) < 0) {
    PSCC_DBG_TRACE(LOG_WARNING, "Failed to set mtu, error: %s \n", strerror(errno));
    return -1;
  }

  return 0;

}

/**
 * pscc_nwifcfg_rename_nwif()
 **/
int pscc_nwifcfg_rename_nwif(char* old_nwifname, char* new_nwifname)
{
  struct ifreq  ifr;

  PSCC_DBG_TRACE(LOG_DEBUG, "Renaming network interface %s to %s\n", old_nwifname, new_nwifname);

  if (strlen(new_nwifname) > (IFNAMSIZ - 1)) {
    PSCC_DBG_TRACE(LOG_WARNING, "Failed to rename nwif, new_nwifname too long: %d\n", strlen(new_nwifname));
    return -1;
  }

  memset((char *) &ifr, 0, sizeof(ifr));

  strncpy(ifr.ifr_name, old_nwifname, IFNAMSIZ - 1);
  strncpy(ifr.ifr_newname, new_nwifname, IFNAMSIZ - 1);

  if (ioctl(ioctl_socket, SIOCSIFNAME, &ifr) < 0) {
    PSCC_DBG_TRACE(LOG_WARNING, "Failed to rename nwif, error: %s \n", strerror(errno));
    return -1;
  }

  return 0;
}

/**
 * pscc_nwifcfg_reset_connections()
 **/
int pscc_nwifcfg_reset_connections(char* nwifname)
{
  struct ifreq ifr;

  if (NULL == nwifname)
  {
    PSCC_DBG_TRACE(LOG_WARNING, "Inparameter is null.\n");
    return -1;
  }

  PSCC_DBG_TRACE(LOG_DEBUG, "Reset connections: %s\n", nwifname);

  if (0 > ioctl_socket)
  {
    PSCC_DBG_TRACE(LOG_WARNING, "NWIFCFG socket not initialized, call pscc_nwifcfg_open first.\n");
    return -1;
  }

  /* ipv4 */
  if (pscc_pdp_type_ipv4 != ioctl_pdp_type)
  {
    PSCC_DBG_TRACE(LOG_WARNING, "pscc_reset_connections only supported for IPv4.\n");
    return -1;
  }

  /* fetch current flags */
  memset((char *) &ifr, 0, sizeof(struct ifreq));
  strncpy(ifr.ifr_name, nwifname, IFNAMSIZ);
  ifr.ifr_name[IFNAMSIZ - 1] = 0;

  if(ioctl(ioctl_socket, SIOCGIFADDR, &ifr) < 0) {
    PSCC_DBG_TRACE(LOG_WARNING, "Error: Failed to get addr, error: %s \n", strerror(errno));
    return -1;
  }

  if (ioctl(ioctl_socket, SIOCKILLADDR,  &ifr) < 0) {
    PSCC_DBG_TRACE(LOG_WARNING, "Error: Failed to kill addr, error: %s \n", strerror(errno));
    return -1;
  }

  return 0;
}
/*******************************************************************************
 *
 * Private/Static Functions
 *
 *******************************************************************************/




