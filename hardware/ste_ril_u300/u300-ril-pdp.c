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
** Based on reference-ril by The Android Open Source Project.
**
** Heavily modified for ST-Ericsson U300 modems.
** Author: Christian Bejram <christian.bejram@stericsson.com>
*/

#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "atchannel.h"
#include "at_tok.h"
#include "misc.h"
#include <telephony/ril.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <linux/if.h>
#include <linux/sockios.h>
#include <linux/route.h>
#include <cutils/properties.h>

#include <linux/ipv6.h>
#include <linux/ipv6_route.h>
#include <errno.h>
#include <stdbool.h>
#include <pthread.h>

#define LOG_TAG "RILV"
#include <utils/Log.h>

#include "u300-ril.h"
#include "u300-ril-pdp.h"
#include "u300-ril-netif.h"
#include "misc.h"

/* Arbitrarily chosen OEM specific id for BIP specific data calls */
#define BIP_DATA_PROFILE_ID "1319"

/* Last data call fail cause, obtained by *CEER. */
static int s_lastDataCallFailCause = PDP_FAIL_ERROR_UNSPECIFIED;

/* OEM callback issued when a OEM activated context is deactivated */
static void (*pdpOemDeativatedCB)(int profileId) = NULL;

/* Connection ID will be implicitly decided by array index */
typedef struct pdpContextEntry {
    int state;      /* free(0), in use(1), reserved(2) */
    int active;     /* deactivated(0), activated(1) */
    int pid;        /* Profile ID */
    char *APN;      /* Access Point Name */
    int OEM;        /* locally created or external (OEM use) */
    char *dnses;    /* DNS IP address(es) */
    char *gateways; /* Gateway IP address(es) */
} pdpContextEntry;

/* Maintained list of PDP contexts */
#define __pdpContextList {0,0,-1,NULL,-1,NULL,NULL}
static pdpContextEntry pdpContextList[RIL_MAX_NUMBER_OF_PDP_CONTEXTS] =
                        {__pdpContextList,__pdpContextList,__pdpContextList,
                         __pdpContextList,__pdpContextList,__pdpContextList};

static pthread_mutex_t contextListMutex = PTHREAD_MUTEX_INITIALIZER;

/* convertAuthenticationMethod */
static char *convertAuthenticationMethod(const char *authentication)
{
    long int auth;
    char *end;

    /*
     * AT requires a bitstring for the authentication methods accepted
     * bit 0 - none authentication
     * bit 1 - pap
     * bit 2 - chap
     */

    if (authentication == NULL) {
        return "111";
    }

    auth = strtol(authentication, &end, 10);

    switch(auth) {
    case 0:    /*PAP and CHAP is never performed., only none */
        return "001";
    case 1:    /*PAP may be performed; CHAP is never performed.*/
        return "011";
    case 2:    /*CHAP may be performed; PAP is never performed.*/
        return "101";
    case 3:    /*PAP / CHAP may be performed - baseband dependent.*/
        return "111";
    default:
        break;
    }
    return NULL;
}

/**
 * cleanupPDPContextList
 *
 * Update internal list compared to modem list.
 * Deactivated entries in the modem will be removed both from internal list and
 * modem.
 */
static void cleanupPDPContextList(RIL_Data_Call_Response *list, int entries)
{
    int i;
    int profileId = -1;
    int oem = 0;
    int active = 0;
    int handle = 0;
    char *cmd = NULL;
    char curIfName[MAX_IFNAME_LEN] = "";
    char *property = NULL;

    if (list == NULL)
        return;

    /* Match deactivated entries with pdpcontext list */
    for (i = 0; i < entries; i++) {
        /* Find non activated entries in modem list */
        if (list[i].active > 0)
            continue;

        /* Find corresponding active entry in RIL list */
        char cidStr[3];
        (void)snprintf(cidStr, 3, "%d", list[i].cid);
        handle = pdpListGet(cidStr, NULL,
                            NULL, &profileId, curIfName, NULL, &active, &oem);
        if (handle < 0)
            continue;

        /*
         * NOTE: List handle must be unreserved (put|undo|free) before break or
         * continue is called.
         */
        if (active == 0) { /* Normal case. Configured not active */
            (void)pdpListUndo(handle);
            continue;
        }

        /* An entry is deactivated in modem but active in list - remove it */
        /*  -> from list */
        if (!pdpListFree(handle)) {
            ALOGE("%s() failed to remove entry from PDP context list", __func__);
            continue;
        }

        /*  -> from modem */
        asprintf(&cmd, "AT+CGDCONT=%d", list[i].cid);
        (void)at_send_command(cmd, NULL);
        free(cmd);

        /*  -> from interfaces (DOWN) */
        if (rtnl_if_disable(curIfName))
            ALOGE("%s() failed to bring down %s!", __func__, curIfName);

        /*  -> from properties */
        asprintf(&property, "net.%s.gw", curIfName);
        (void)property_set(property, "");
        free(property);
        asprintf(&property, "net.%s.dns1", curIfName);
        (void)property_set(property, "");
        free(property);
        asprintf(&property, "net.%s.dns2", curIfName);
        (void)property_set(property, "");
        free(property);

        /*  -> from OEM framework */
        if (pdpOemDeativatedCB != NULL && oem) {
            pdpOemDeativatedCB(profileId);
        }
    }
}

/* requestOrSendPDPContextList */
void requestOrSendPDPContextList(RIL_Token * token)
{
    ATResponse *atresponse = NULL;
    RIL_Data_Call_Response *responses = NULL;
    ATLine *cursor;
    int err;
    int number_of_contexts = 0;
    int i = 0;
    char *out;
#if RIL_VERSION >= 6
    char *out1;
#endif
    /* Read the activation states */
    err = at_send_command_multiline("AT+CGACT?", "+CGACT:", &atresponse);
    if (err < 0 || atresponse->success == 0)
        goto error;

    /* Calculate size of buffer to allocate*/
    for (cursor = atresponse->p_intermediates; cursor != NULL;
         cursor = cursor->p_next)
        number_of_contexts++;

    if (number_of_contexts == 0)
        /* return empty list (NULL with size 0)*/
        goto send_and_return;


    responses = alloca(number_of_contexts * sizeof(RIL_Data_Call_Response));
    memset (responses, 0, sizeof(responses));
    for (i = 0; i < number_of_contexts; i++) {
        responses[i].cid = -1;
        responses[i].active = -1;
#if RIL_VERSION >= 6
        responses[i].status = PDP_FAIL_NONE;
        responses[i].addresses = "";
        responses[i].dnses = "";
        responses[i].gateways = "";
        responses[i].ifname = "";
        responses[i].suggestedRetryTime = -1; // Use default value
#else
        responses[i].address = "";
#endif
    }

    /*parse the result*/
    i = 0;
    for (cursor = atresponse->p_intermediates; cursor != NULL;
         cursor = cursor->p_next) {
        char *line = cursor->line;
        int state;

        err = at_tok_start(&line);
        if (err < 0)
            goto error;

        err = at_tok_nextint(&line, &responses[i].cid);
        if (err < 0)
            goto error;

        err = at_tok_nextint(&line, &state);
        if (err < 0)
            goto error;

        if (state == 0)
                responses[i].active = 0;  /* 0=inactive */
            else
                responses[i].active = 2;  /* 2=active/physical link up */
        i++;
    }

    at_response_free(atresponse);


    /* Read the currend pdp settings */
    err = at_send_command_multiline("AT+CGDCONT?", "+CGDCONT:",&atresponse);
    if (err < 0 || atresponse->success == 0)
         goto error;

    for (cursor = atresponse->p_intermediates; cursor != NULL;
         cursor = cursor->p_next) {
        char *line = cursor->line;
        int cid;
        char cidStr[3];
        char *curIfName = NULL;
        char *dnsStr;
        char *gatewayStr;

        err = at_tok_start(&line);
        if (err < 0)
            goto error;

        err = at_tok_nextint(&line, &cid);
        if (err < 0)
            goto error;

        for (i = 0; i < number_of_contexts; i++)
            if (responses[i].cid == cid)
                break;

        if (i >= number_of_contexts)
            /* Details for a context we didn't hear about in the last request. */
            continue;

        err = at_tok_nextstr(&line, &out);
        if (err < 0)
            goto error;

        responses[i].type = alloca(strlen(out) + 1);
        strcpy(responses[i].type, out);

        err = at_tok_nextstr(&line, &out);
        if (err < 0)
            goto error;

#if RIL_VERSION < 6
        responses[i].apn = alloca(strlen(out) + 1);
        strcpy(responses[i].apn, out);
#else
        curIfName = alloca(MAX_IFNAME_LEN);
        curIfName[0]='\0';
        dnsStr = alloca(2 * INET_ADDRSTRLEN + 2 * INET6_ADDRSTRLEN + 4);
        gatewayStr = alloca(INET6_ADDRSTRLEN + 1);
        (void)snprintf(cidStr, sizeof(cidStr), "%d", responses[i].cid);
        if (pdpListExist(cidStr, NULL, NULL, NULL, curIfName, NULL, NULL, dnsStr, gatewayStr) >= 0) {
            responses[i].dnses = dnsStr;
            responses[i].gateways = gatewayStr;
            responses[i].ifname = curIfName;
        }
#endif
        err = at_tok_nextstr(&line, &out);
        if (err < 0) {
            ALOGE("%s : Err is getting ipv4 address !", __func__);
            goto error;
        }
#if RIL_VERSION >= 6
        err = at_tok_nextstr(&line, &out1);
        if (err < 0) {
            ALOGE("%s : Err is getting ipv6 address !", __func__);
            goto error;
        }
        if('"' != out1[0])
          out1 = NULL;
        if((NULL != out) && (NULL == out1)) {
          responses[i].addresses = alloca(strlen(out) + 1);
          strcpy(responses[i].addresses, out);
        }else if ((NULL == out) && (NULL != out1)) {
          responses[i].addresses = alloca(strlen(out1) + 1);
          strcpy(responses[i].addresses, out1);
        } else if ((NULL != out) && (NULL != out1)) {
          responses[i].addresses = alloca(strlen(out) + strlen(out1) + 2);
          strcpy(responses[i].addresses, out);
          strcat(responses[i].addresses, " ");
          strcat(responses[i].addresses, out1);
        }

#else
        responses[i].address = alloca(strlen(out) + 1);
        strcpy(responses[i].address, out);
#endif
    }

    at_response_free(atresponse);

send_and_return:
    if (token != NULL)
        RIL_onRequestComplete(*token, RIL_E_SUCCESS, responses,
                           number_of_contexts * sizeof(RIL_Data_Call_Response));
    else
        RIL_onUnsolicitedResponse(RIL_UNSOL_DATA_CALL_LIST_CHANGED, responses,
                           number_of_contexts * sizeof(RIL_Data_Call_Response));

    /*
     * To keep internal list up to date all deactivated contexts are removed
     * from modem and interface is set to DOWN...
     */
    cleanupPDPContextList(responses, number_of_contexts);

    return;

error:
    /*Send error and free allocated memory*/
    if (token != NULL)
        RIL_onRequestComplete(*token, RIL_E_GENERIC_FAILURE, NULL, 0);
    else
        RIL_onUnsolicitedResponse(RIL_UNSOL_DATA_CALL_LIST_CHANGED, NULL, 0);

    if (atresponse)
        at_response_free(atresponse);
}

/**
  * Extracts IP address, subnet mask, mtu size and dns server addresses from
  * an AT*EPPSD response.
  * The caller is responsible for freeing all the output strings.
  */
#if RIL_VERSION >= 6
static void parseEPPSDResponse(ATResponse *atresponse, char **ipAddrStr, char **ipv6AddrStr,
                char **subnetMaskStr, char **mtuStr, char **primaryDnsStr, char **ipv6primaryDnsStr,
                char **secondaryDnsStr, char **ipv6secondaryDnsStr, char *ril_iface, char **activationCause)
#else
static void parseEPPSDResponse(ATResponse *atresponse, char **ipAddrStr, char **subnetMaskStr,
                char **mtuStr, char **primaryDnsStr, char **secondaryDnsStr, char *ril_iface)
#endif
{
    int buflen = 0;
    int pos = 0;
    char *doc = NULL;
    char *docTail = NULL;
    char *line = NULL;
    char *value = NULL;
    char *property = NULL;
    ATLine *currentLine = NULL;

    /* Loop once to calculate buffer length. */
    for (currentLine = atresponse->p_intermediates;
         currentLine != NULL; currentLine = currentLine->p_next) {

        char *line = currentLine->line;

        if (line != NULL)
            buflen += strlen(line);
    }

    if (buflen > 0) {

        doc = calloc(buflen + 1, sizeof(char));
        assert(doc != NULL);

        /* Build the buffer containing all lines. */
        for (currentLine = atresponse->p_intermediates;
             currentLine != NULL; currentLine = currentLine->p_next) {

            line = currentLine->line;

            if (line != NULL) {
                strncpy(doc + pos, line, strlen(line));
                pos += strlen(line);
            }
        }

        /* Get IP address */
        value = getFirstElementValue(doc,"<ip_address>","</ip_address>",
                                     '\0', NULL);
        if (value != NULL) {
            ALOGI("IP Address: %s\n", value);
            *ipAddrStr = value;
            value = NULL;
        }

#if RIL_VERSION >= 6
        /* Get IPv6 address */
        value = getFirstElementValue(doc,"<ipv6_address>","</ipv6_address>",
                                     '\0', NULL);
        if (value != NULL) {
            ALOGI("IPv6 Address: %s\n", value);
            *ipv6AddrStr = value;
            value = NULL;
        }
#endif

        /* Get Subnet */
        value = getFirstElementValue(doc, "<subnet_mask>","</subnet_mask>",
                                     '\0', NULL);
        if (value != NULL) {
            ALOGI("Subnet Mask: %s\n", value);
            *subnetMaskStr = value;
            value = NULL;
        }

        /* Get mtu */
        value = getFirstElementValue(doc, "<mtu>", "</mtu>", '\0', NULL);
        if (value != NULL) {
            ALOGI("MTU: %s\n", value);
            *mtuStr = value;
            value = NULL;
        }

#if RIL_VERSION >= 6
        /*Get Activation cause */
        value = getFirstElementValue(doc, "<activation_cause>", "</activation_cause>", '\0', NULL);
        if (value != NULL) {
            ALOGI("Activation cause: %s\n", value);
            *activationCause = value;
            value = NULL;
        }
#endif
        /* We support two DNS servers */
        docTail = NULL;

        value = getFirstElementValue(doc,"<dns_server>","</dns_server>",
                                     '\0', &docTail);
        if (value != NULL) {
            // TODO: After android 3.0 this step is not needed
            asprintf(&property, "net.%s.dns1", ril_iface);
            ALOGI("1st DNS Server: %s\n", value);
            if (property_set(property, value))
                ALOGE("FAILED to set dns1 property!");

            *primaryDnsStr = value;
            free(property);
            value = NULL;
        }

        if (docTail != NULL) {
            /* One more DNS server found */
            value = getFirstElementValue(docTail,"<dns_server>",
                                         "</dns_server>", '\0', NULL);
            if (value != NULL) {
                // TODO: After android 3.0 this step is not needed
                asprintf(&property, "net.%s.dns2", ril_iface);
                ALOGI("2nd DNS Server: %s\n", value);
                if (property_set(property, value))
                    ALOGE("FAILED to set dns2 property!");

                *secondaryDnsStr = value;
                free(property);
                value = NULL;
            }
        }

#if RIL_VERSION >= 6
        /* We support two IPv6 DNS servers */
        docTail = NULL;

        value = getFirstElementValue(doc,"<ipv6_dns_server>","</ipv6_dns_server>",
                                     '\0', &docTail);
        if (value != NULL) {
            // TODO: After android 3.0 this step is not needed
            asprintf(&property, "net.%s.dns1", ril_iface);
            ALOGI("1st IPv6 DNS Server: %s\n", value);
            if (property_set(property, value))
                ALOGE("FAILED to set ipv6 dns1 property!");

            *ipv6primaryDnsStr = value;
            free(property);
            value = NULL;
        }

        if (docTail != NULL) {
            /* One more DNS server found */
            value = getFirstElementValue(docTail,"<ipv6_dns_server>",
                                         "</ipv6_dns_server>", '\0', NULL);
            if (value != NULL) {
                // TODO: After android 3.0 this step is not needed
                asprintf(&property, "net.%s.dns2", ril_iface);
                ALOGI("2nd Ipv6 DNS Server: %s\n", value);
                if (property_set(property, value))
                    ALOGE("FAILED to set ipv6 dns2 property!");

                *ipv6secondaryDnsStr = value;
                free(property);
                value = NULL;
            }
        }
#endif

        /* Note GW is not fetched. Default GW is calculated later. */
        free(doc);
    }
}

static int configureNetworkInterface(char *ipAddrStr, char *subnetMaskStr,
                                       char *mtuStr, char **defaultGatewayStr,
                                       char *ril_iface)
{
    in_addr_t addr, subaddr;
    in_addr_t tmpsubaddr;

    int status = 0;
    int prefix = 0;

    if (inet_pton(AF_INET, ipAddrStr, &addr) <= 0) {
        ALOGE("%s: inet_pton() failed for %s!", __func__, subnetMaskStr);
        goto error;
    }

    if (inet_pton(AF_INET, subnetMaskStr, &subaddr) <= 0) {
        ALOGE("%s: inet_pton() failed for %s!", __func__, subnetMaskStr);
        goto error;
    }

    /*
     * This will fake a /31 CIDR network as defined in RFC 3021 to enable us to
     * have 'normal' routes in the routing table.
     */
    if (*defaultGatewayStr == NULL && subaddr == htonl(0xFFFFFFFF)) {
        in_addr_t gw;
        struct in_addr gwaddr;
        char *property = NULL;
        subaddr = htonl(0xFFFFFFFE);    /* 255.255.255.254, CIDR /31. */

        gw = ntohl(addr) & 0xFFFFFF00;
        gw |= (ntohl(addr) & 0x000000FF) ^ 1;

        gwaddr.s_addr = htonl(gw);

        *defaultGatewayStr = strdup(inet_ntoa(gwaddr));

        // TODO: After android 3.0 this step is not needed
        asprintf(&property, "net.%s.gw", ril_iface);
        if (property_set(property, *defaultGatewayStr))
            ALOGE("Failed to set fake %s.", property);

        free(property);

        ALOGI("Generated new fake /31 subnet with gw: %s", *defaultGatewayStr);
    }

    /* Compute prefix length from subnet mask */
    tmpsubaddr = ntohl(subaddr);
    prefix = 0;
    while (((tmpsubaddr&0x80000000) != 0) && (prefix <= 32))
    {
        prefix++;
        tmpsubaddr <<= 1;
    }

    /* Flush any existing IPv4 address */
    if (rtnl_flush_ip_addresses(ril_iface, AF_INET) < 0) {
       ALOGE("%s() failed to flush IPv4 address !", __func__);
       goto error;
    }

    /* Config IPv4 address and prefix on network interface  */
    if (rtnl_set_ipv4_addr(ril_iface, ipAddrStr, prefix) < 0) {
        ALOGE("%s() failed to setup address %s/%d for interface %s!", __func__,
            ipAddrStr, prefix, ril_iface);
        goto error;
    }

    /* We should set mtu... */
    if (mtuStr != NULL) {
        int mtu = 0;
        mtu = atoi(mtuStr);

        if (mtu > 1500)
            mtu = 1500;

        if (mtu > 1) {
            if (rtnl_set_mtu(ril_iface, mtu) < 0)
                ALOGE("%s: failed to set mtu for %s!", __func__, ril_iface);
        }
    }

    if (rtnl_if_enable(ril_iface)) {
        ALOGE("%s() failed to bring up %s!", __func__, ril_iface);
        goto error;
    }

    goto exit;

error:
    status = -1;

exit:
    return status;
}

static int configureNetworkInterfaceIPv6(char **ipAddrStr, char *ipv6subnetMaskStr,
                                            char *mtuStr, char **defaultGatewayStr,
                                            char *ril_iface)
{
    struct in6_addr llIp6Addr;
    char llIp6AddrStr[INET6_ADDRSTRLEN];
    struct in6_addr ip6_addr;
    int prefix = 0;
    int res = -1;
    char *property = NULL;

    if (inet_pton(AF_INET6, *ipAddrStr, &llIp6Addr) <= 0) {
        ALOGE("%s() failed to convert PDP IPv6 address %s - %s!", __func__,
             ril_iface, *ipAddrStr);
        goto exit;
    }

    /* Convert the PDP IPv6 address into a LinkLocal address */
    memset(&llIp6Addr, 0, 8);
    llIp6Addr.s6_addr[0] = 0xfe;
    llIp6Addr.s6_addr[1] = 0x80;
    if (inet_ntop(AF_INET6, &llIp6Addr,
                  llIp6AddrStr, INET6_ADDRSTRLEN) == NULL) {
        ALOGE("%s() failed to convert LinkLocal IPv6 address %s!",__func__, ril_iface);
        goto exit;
    }

    /* Flush any existing IPv6 address */
    if (rtnl_flush_ip_addresses(ril_iface, AF_INET6) < 0) {
       ALOGE("%s() failed to flush IPv6 address !", __func__);
       goto exit;
    }

    /* Configure IPv6 LinkLocal address in interface */
    if (rtnl_set_ipv6_link_addr(ril_iface, llIp6AddrStr) < 0) {
        ALOGE("%s() failed to add IPv6 address %s!", __func__, ril_iface);
        goto exit;
    }

    if (rtnl_if_enable(ril_iface)) {
        ALOGE("%s() failed to bring up %s!", __func__, ril_iface);
        goto exit;
    }

    /* First check if there already exists a public address */
    if (rtnl_get_first_ip_address(ril_iface, AF_INET6,
                                   &prefix, &ip6_addr) < 0) {
        int err;
        ALOGI("%s(): No public IPv6 address found on %s, wait for SAA "
              "to generate...", __func__, ril_iface);

        /* ... no address, wait for SAA to generate address */
        err = rtnl_wait_for_ipv6_prefix(ril_iface, NULL, NULL);

        if (err < 0)
            goto exit;

        if (rtnl_get_first_ip_address(ril_iface, AF_INET6,
                                       &prefix, &ip6_addr) < 0) {
        ALOGE("%s(): No public IPv6 address foundon %s",__func__, ril_iface);
            goto exit;
        }
    }

    /* Replace PDP address with public address (from SAA) in ipAddrStr */
    free(*ipAddrStr);
    *ipAddrStr = malloc(INET6_ADDRSTRLEN);
    assert((*ipAddrStr) != NULL);
    if (inet_ntop(AF_INET6, &ip6_addr,
                   *ipAddrStr, INET6_ADDRSTRLEN) == NULL) {
        ALOGE("%s() failed calling inet_ntop for public IPv6!", __func__);
        goto exit;
     }

    *defaultGatewayStr = strdup("::");
    // TODO: After android 3.0 this step is not needed
    asprintf(&property, "net.%s.gw", ril_iface);
    if (property_set(property, *defaultGatewayStr))
        ALOGE("Failed to set fake %s.", property);

    free(property);
    ALOGI("Generated new fake /31 subnet with gw: %s", *defaultGatewayStr);

    /* We should set mtu... */
    if (mtuStr != NULL) {
        int mtu = 0;
        mtu = atoi(mtuStr);

        if (mtu > 1500)
            mtu = 1500;

        if (mtu > 1) {
            if (rtnl_set_mtu(ril_iface, mtu) < 0)
              ALOGE("%s: failed to set mtu for %s!", __func__, ril_iface);
        }
    }

    res = 0;

exit:
    return res;
}


/**
 * getActivationCause
 *
 * To assign an Activation cause.
 */
static int getActivationCause(char *activationCause)
{
    int cause = PDP_FAIL_NONE;
    if (activationCause != NULL) {
        switch (atoi(activationCause)){
            case 0x32:
                cause = PDP_FAIL_ONLY_IPV4_ALLOWED;
                break;
            case 0x33:
                cause = PDP_FAIL_ONLY_IPV6_ALLOWED;
                break;
            case 0x34:
                cause = PDP_FAIL_ONLY_SINGLE_BEARER_ALLOWED;
                break;
            default:
                break;
        }
    }
    return cause ;
}


 /**
 * pdpListExist()
 *
 * Looks for matching existing indexes of Connection ID, APN and Profile ID in
 * pdp context list. Note that cid/profileId set to NULL will not be
 * evaluated.
 *
 * Fills cid, ifName, profileid, active and oem if given and entry exists.
 * (can be set to NULL if not needed)
 *
 * Returns true if found, false if not found.
 */
bool pdpListExist(const char *cidToFind, const char *profileToFind,
                  int *cid, int *profile, char ifName[], int *active,
                  int *oem, char *dnses, char *gateways)
{
    int i;
    char *end = NULL;
    bool found = false;
    int err;

    if ((err = pthread_mutex_lock(&contextListMutex)) != 0) {
        ALOGE("%s() failed to take list mutex: %s!", __func__, strerror(err));
        assert(0);
    }

    for (i = 0; i < RIL_MAX_NUMBER_OF_PDP_CONTEXTS; i++) {
        if ((cidToFind != NULL &&
             strtol(cidToFind, &end, 10) == i + RIL_FIRST_CID_INDEX) ||
            (profileToFind != NULL &&
             strtol(profileToFind, &end, 10) == pdpContextList[i].pid)) {

            found = true;
            if (cid != NULL)
                *cid = i + RIL_FIRST_CID_INDEX;
            if (profile != NULL)
                *profile = pdpContextList[i].pid;
            if (active != NULL)
                *active = pdpContextList[i].active;
            if (oem != NULL)
                *oem = pdpContextList[i].OEM;
            if (ifName != NULL) {
                (void)snprintf(ifName, MAX_IFNAME_LEN, "%s%d", ril_iface, i);
                ifName[MAX_IFNAME_LEN - 1] = '\0';
            }

            if (dnses != NULL && pdpContextList[i].dnses != NULL)
                strcpy(dnses, pdpContextList[i].dnses);
            if (gateways != NULL && pdpContextList[i].gateways != NULL)
                strcpy(gateways, pdpContextList[i].gateways);

            break;
        }
    }

    if ((err = pthread_mutex_unlock(&contextListMutex)) != 0) {
        ALOGE("%s() failed to release state mutex: %s", __func__, strerror(err));
        assert(0);
    }

    return found;
}


/**
 * pdpListGet()
 *
 * Looks for matching indexes of Connection ID and Profile ID in pdp
 * context list. Note that cid/profileId set to NULL will not be evaluated.
 *
 * Fills cid, profile, ifName, apn, active and oem if given and entry is found.
 * (can be set to NULL if not needed)
 *
 * Note that if entry is found and a handle is returned the handle needs to be
 * released later through put/undo/free!
 *
 * Returns
 *  pdplistentry handle if found,
 *  -1 if not found.
 *  -2 if already reserved
 */
int pdpListGet(const char *cidToFind, const char *profileIdToFind,
               int *cid, int *profile, char ifName[], char **apn, int *active,
               int *oem)
{
    int i;
    int pdpListHandle = -1;
    char *end = NULL;
    int err;

    if ((err = pthread_mutex_lock(&contextListMutex)) != 0) {
        ALOGE("%s() failed to take list mutex: %s!", __func__, strerror(err));
        assert(0);
    }

    for (i = 0; i < RIL_MAX_NUMBER_OF_PDP_CONTEXTS; i++) {

        if (pdpContextList[i].state == 0)
            continue;

        if ((cidToFind != NULL &&
             strtol(cidToFind, &end, 10) == i + RIL_FIRST_CID_INDEX) ||
            (profileIdToFind != NULL &&
             strtol(profileIdToFind, &end, 10) == pdpContextList[i].pid)) {

            if (pdpContextList[i].state != 1) {
                ALOGD("%s() attempted on already reserved index", __func__);
                pdpListHandle = -2;
                break;
            }

            /* Entry found */
            pdpContextList[i].state = 2; /* Reserved */
            pdpListHandle = i;

            if (cid != NULL)
                *cid = i + RIL_FIRST_CID_INDEX;
            if (profile != NULL)
                *profile = pdpContextList[i].pid;
            if (apn != NULL)
                *apn = pdpContextList[i].APN;
            if (active != NULL)
                *active = pdpContextList[i].active;
            if (oem != NULL)
                *oem = pdpContextList[i].OEM;
            if (ifName != NULL) {
                (void)snprintf(ifName, MAX_IFNAME_LEN, "%s%d", ril_iface, i);
                ifName[MAX_IFNAME_LEN - 1] = '\0';
            }
            break;
        }
    }

    if ((err = pthread_mutex_unlock(&contextListMutex)) != 0) {
        ALOGE("%s() failed to release state mutex: %s", __func__,
             strerror(err));
        assert(0);
    }

    return pdpListHandle;
}

/**
 * pdpListGetFree()
 *
 * Finds a free entry in the pdp context list and RESERVES it.
 *
 * cid cannot be NULL and will always be set in successful case.
 * ifName will be set in case of success and if it is not NULL.
 *
 * Returns handle to entry if found, -1 if not found.
 */
int pdpListGetFree(int *cid, char ifName[])
{
    int i = -1;
    bool found = false;
    int err;

    if (cid == NULL)
        goto exit;

    if ((err = pthread_mutex_lock(&contextListMutex)) != 0) {
        ALOGE("%s() failed to take list mutex: %s!", __func__, strerror(err));
        assert(0);
    }

    for (i = 0; i < RIL_MAX_NUMBER_OF_PDP_CONTEXTS; i++) {
        if (pdpContextList[i].state == 0) {
            found = true;
            break;
        }
    }

    if (found) {
        /*
         * Generating:
         *  Connection ID (array index + first cid index)
         *  Interface Name (ril_iface(Connection ID - 1))
         */
        *cid = i + RIL_FIRST_CID_INDEX;
        if (ifName != NULL) {
            (void) snprintf(ifName, MAX_IFNAME_LEN, "%s%d", ril_iface, i);
            ifName[MAX_IFNAME_LEN -1] = '\0';
        }
        pdpContextList[i].state = 2; /* reserved */
    } else {
        i = -1;
    }

    if ((err = pthread_mutex_unlock(&contextListMutex)) != 0) {
        ALOGE("%s() failed to release state mutex: %s", __func__,
             strerror(err));
        assert(0);
    }

exit: /* Note: label does not release lock, use with caution */
    return i;
}

/**
 * pdpListPut()
 *
 * Sets an entry in the PDP context list and UNRESERVES it.
 * Returns true if successful, false if unsuccessful.
 */
bool pdpListPut(int pdpListHandle, int profile, const char *apn, int activated,
                int oem, const char *dnses, const char *gateways)
{
    bool success = true;
    int err;

    if ((err = pthread_mutex_lock(&contextListMutex)) != 0) {
        ALOGE("%s() failed to take list mutex: %s!", __func__, strerror(err));
        assert(0);
    }

    if (pdpListHandle < 0 || pdpListHandle >= RIL_MAX_NUMBER_OF_PDP_CONTEXTS ||
        pdpContextList[pdpListHandle].state != 2) {
        ALOGE("%s() attempted on a non-reserved list entry, error!", __func__);
        success = false;
    }

    if (success) {
        pdpContextList[pdpListHandle].pid = profile;
        if (apn != NULL)
            pdpContextList[pdpListHandle].APN = strdup(apn);
        pdpContextList[pdpListHandle].OEM = oem;
        pdpContextList[pdpListHandle].active = activated;
        pdpContextList[pdpListHandle].state = 1; /* in use */
        if (dnses != NULL)
            pdpContextList[pdpListHandle].dnses = strdup(dnses);
        if (gateways != NULL)
            pdpContextList[pdpListHandle].gateways = strdup(gateways);
    }

    if ((err = pthread_mutex_unlock(&contextListMutex)) != 0) {
        ALOGE("%s() failed to release state mutex: %s", __func__,
             strerror(err));
        assert(0);
    }

    return success;
}

/**
 * pdpListFree()
 *
 * Free/clears an entry in the PDP context list and UNRESERVES it.
 * Returns true if successful, false if unsuccessful.
 */
bool pdpListFree(int pdpListHandle)
{
    bool success = true;
    int err;

    if ((err = pthread_mutex_lock(&contextListMutex)) != 0) {
        ALOGE("%s() failed to take list mutex: %s!", __func__, strerror(err));
        assert(0);
    }

    if (pdpListHandle < 0 || pdpListHandle >= RIL_MAX_NUMBER_OF_PDP_CONTEXTS ||
        pdpContextList[pdpListHandle].state != 2) {
        ALOGE("%s() attempted on a non-reserved list entry, error!", __func__);
        success = false;
    }

    if (success) {
        pdpContextList[pdpListHandle].state = 0; /* free */
        pdpContextList[pdpListHandle].active = -1;
        pdpContextList[pdpListHandle].pid = -1;
        free(pdpContextList[pdpListHandle].APN);
        pdpContextList[pdpListHandle].APN = NULL;
        pdpContextList[pdpListHandle].OEM = -1;
        free(pdpContextList[pdpListHandle].dnses);
        pdpContextList[pdpListHandle].dnses = NULL;
        free(pdpContextList[pdpListHandle].gateways);
        pdpContextList[pdpListHandle].gateways = NULL;
    }

    if ((err = pthread_mutex_unlock(&contextListMutex)) != 0) {
        ALOGE("%s() failed to release state mutex: %s", __func__,
             strerror(err));
        assert(0);
    }

    return success;
}

/**
 * pdpListUndo()
 *
 * Unreserves an entry in the PDP context list without doing any changes to
 * pdp list entry. This also frees and newly reserved free entry.
 * Returns true if successful, false if unsuccessful.
 */
bool pdpListUndo(int pdpListHandle)
{
    bool success = true;
    int err;

    if ((err = pthread_mutex_lock(&contextListMutex)) != 0) {
        ALOGE("%s() failed to take list mutex: %s!", __func__, strerror(err));
        assert(0);
    }

    if (pdpListHandle < 0 || pdpListHandle >= RIL_MAX_NUMBER_OF_PDP_CONTEXTS ||
        pdpContextList[pdpListHandle].state != 2) {
        ALOGE("%s() attempted on a non-reserved list entry, error!", __func__);
        success = false;
    }

    if (success) {
        /* if just created... set free */
        if (pdpContextList[pdpListHandle].APN == NULL &&
            pdpContextList[pdpListHandle].pid == -1)
            pdpContextList[pdpListHandle].state = 0; /* free */
        else
            pdpContextList[pdpListHandle].state = 1; /* in use */
    }

    if ((err = pthread_mutex_unlock(&contextListMutex)) != 0) {
        ALOGE("%s() failed to release state mutex: %s", __func__,
             strerror(err));
       assert(0);
    }

    return success;
}

/**
 * RIL_UNSOL_DATA_CALL_LIST_CHANGED
 *
 * Indicate a PDP context state has changed, or a new context
 * has been activated or deactivated.
 *
 * See also: RIL_REQUEST_DATA_CALL_LIST
 */
void onPDPContextListChanged(void *param)
{
    requestOrSendPDPContextList(NULL);
}

/**
 * RIL_REQUEST_DATA_CALL_LIST
 *
 * Queries the status of PDP contexts, returning for each
 * its CID, whether or not it is active, and its PDP type,
 * APN, and PDP adddress.
 */
void requestPDPContextList(void *data, size_t datalen, RIL_Token t)
{
    requestOrSendPDPContextList(&t);
}

/**
 * RIL_REQUEST_SETUP_DATA_CALL
 *
 * Configure and activate PDP context for default IP connection.
 *
 * See also: RIL_REQUEST_DEACTIVATE_DATA_CALL
 */
void requestSetupDataCall(void *data, size_t datalen, RIL_Token t)
{
    const char *radioTech = NULL;
    const char *dataProfile = NULL;
    const char *APN = NULL;
    const char *username = NULL;
    const char *password = NULL;
    const char *authentication = NULL;
    const char *pdp_type = NULL;
    char *cmd = NULL;
    char *property = NULL;
    char *ipAddrStr = NULL;
#if RIL_VERSION >= 6
    char *ipv6AddrStr = NULL;
    char *ipAddrlist = NULL;
#endif
    char *subnetMaskStr = NULL;
#if RIL_VERSION >= 6
    char *ipv6subnetMaskStr = NULL;
    char *activationCause = NULL;
#endif
    char *defaultGatewayStr = NULL;
    char *mtuStr = NULL;
    size_t rilResponseLen;
    RIL_Errno rilErrno = RIL_E_GENERIC_FAILURE;
#if RIL_VERSION >= 6
    RIL_Data_Call_Response *rilResponse = NULL;
#else
    char **rilResponse = NULL;
#endif
    int err = 0;
    int pid = 0;
    int inet_type = AF_INET;
#if RIL_VERSION >= 6
    int dualmode = 0;
#endif
    char* auth = NULL;
    char* pdns = NULL;
    char* sdns = NULL;
#if RIL_VERSION >= 6
    char *ipv6pdns = NULL;
    char *ipv6sdns = NULL;
    struct addrinfo hint, *info =0;
#endif
    char* dnslist = NULL;
    char* type = NULL;
    ATResponse *atresponse = NULL;
    ATLine *currentLine = NULL;
    char *end = NULL;

    char curIfName[MAX_IFNAME_LEN] = "";
    char *curCidStr = NULL;
    int curCid = -1;
    int curActive = -1;
    int curOem = -1;
    char *curApn = NULL;
    int pdpListHandle = -1;
    unsigned short sdns_len = 1;
    unsigned short ipv6sdns_len = 1;

    /* Assign inparameters. */
    radioTech = ((const char **) data)[0];
    dataProfile = ((const char **) data)[1];
    APN = ((const char **) data)[2];
    username = ((const char **) data)[3];
    password = ((const char **) data)[4];
    authentication = ((const char **) data)[5];
    pdp_type = ((const char **) data)[6];
#if RIL_VERSION >= 6
    type = (strcmp(pdp_type, "IPV6") == 0 ? "ipv6" : (strcmp(pdp_type, "IPV4V6") == 0 ? "ipv4v6" : "ip"));
#else
    type = (strcmp(pdp_type, "IPV6") == 0 ? "ipv6" : "ip";
#endif
    /* Check type, only GSM/WCDMA support */
    if (!strtol(radioTech, &end, 10))
        goto exit;

    /* We set the inet_type for IPv6 */
    if (strcmp(pdp_type, "IPV6") == 0)
      inet_type = AF_INET6;

    /* We now handle IPV4V6 */
#if RIL_VERSION >= 6
    if (strcmp(pdp_type, "IPV4V6") == 0)
      dualmode = 1;
#endif
    /* have to convert authentication method to AT type */
    auth = convertAuthenticationMethod(authentication);
    if (auth == NULL)
        goto exit;

    /* DataProfile is only required to be unique for data calls
     * with OEM profiles (to allow DataProfile as identifier in RIL
     * OEM commands).
     * All other profiles are allowed to be equal in multipple data calls
    */
    pid = strtol(dataProfile, &end, 10);
    if (pid < RIL_DATA_PROFILE_OEM_BASE)
        dataProfile = NULL;

    /* ---------------------------------------------------------------------- *
     * ------------------ FINDING AVAILABLE CONNECTION ID ------------------- *
     * ---------------------------------------------------------------------- */
    /* Check for already existing entry to use (configured via OEM) */
    pdpListHandle = pdpListGet(NULL, dataProfile, &curCid, NULL, curIfName,
                               &curApn, &curActive, &curOem);
    if (pdpListHandle >= 0) {
        /* Found existing entry in internal list */
        if (curActive == 1) {
            ALOGE("%s() was called with setup on already activated PDP Context. "
                 "Rejecting data call setup.", __func__);
            goto error__unreserve_list_entry;
        }


        /* entry is found but not activated */
        /* Use APN from stored profile if not defined in request */
        if (APN == NULL)
            APN = curApn;

        ALOGI("%s() using existing but not activated Connection ID (%d) and "
             "Interface Name (%s)", __func__, curCid, curIfName);

    } else {
        /* Finding free entry in PDP List */
        if ((pdpListHandle = pdpListGetFree(&curCid, curIfName)) < 0) {
            ALOGE("%s() was called with already maximum number of activated PDP "
                 "contexts. Rejecting data call setup.", __func__);
            goto exit;
        }

        ALOGI("%s() selected new Connection ID (%d) and Interface Name (%s)",
             __func__, curCid, curIfName);
    }


#if RIL_VERSION >= 6
    /* Allocate and fill in response */
    rilResponseLen = sizeof(RIL_Data_Call_Response);
    rilResponse = alloca(rilResponseLen);
    if (rilResponse == NULL) {
        goto exit;
    }
    memset(rilResponse, 0, rilResponseLen);
#endif


    /*
     * NOTE:
     *  - List handle must be unreserved (put|undo|free) before exit.
     *    (goto exit or error not allowed!!)
     *  - Some networks support setting up PDP context without giving an APN.
     */

    /* ---------------------------------------------------------------------- *
     * ----------------- SETTING UP PDP ACCOUNT IN MODEM -------------------- *
     * ---------------------------------------------------------------------- */
    /* AT+CGDCONT=<cid>,<PDP_type>,<APN>,<PDP_addr> */
    asprintf(&cmd, "AT+CGDCONT=%d,\"%s\",\"%s\",\"\"",
                curCid, type, (APN ? APN : ""));
    err = at_send_command(cmd, &atresponse);
    free(cmd);

    if (err < 0 || atresponse->success == 0)
        goto error__unreserve_list_entry;

    at_response_free(atresponse);
    atresponse = NULL;

    /* AT*EIAAUW=<cid>,<bearer_id>,<userid>,<password>,<auth_prot>,<ask4pwd> */
    asprintf(&cmd, "AT*EIAAUW=%d,1,\"%s\",\"%s\",%s,0", curCid,
                (username ? username : ""), (password ? password : ""), auth);
    err = at_send_command(cmd, &atresponse);
    free(cmd);

    if (err < 0 || atresponse->success == 0)
        goto error__delete_account;

    at_response_free(atresponse);
    atresponse = NULL;

    /* ---------------------------------------------------------------------- *
     * ---------------------- ACTIVATING PDP CONTEXT ------------------------ *
     * ---------------------------------------------------------------------- */
    /* AT*EPPSD=<state>,<channel_id>,<cid> */
    asprintf(&cmd, "AT*EPPSD=1,%x,%d", curCid, curCid);
    err = at_send_command_multiline(cmd, "*EPPSD: <", &atresponse);
    free(cmd);

    if (err < 0)
        goto error__delete_account;

    if (atresponse->success == 0) {
        int status;
        at_response_free(atresponse);
        atresponse = NULL;

        err = at_send_command_singleline("AT+CEER", "+CEER:", &atresponse);
        if (err < 0 || atresponse->success == 0)
            goto error__delete_account;

        /*
         * rilResponse->status should map to TS 24.008 6.1.3.1.3 according
         * to RIL.h as expected received from AT+CEER
         */
        status = at_get_sm_cause(atresponse);
        ALOGE("PDP Context Activate failed with SM Cause Code %i",
             status);

        if (status == SM_CAUSE_NON_SM)
            status = PDP_FAIL_ERROR_UNSPECIFIED;

        s_lastDataCallFailCause = status;

#if RIL_VERSION >= 6
        /*
         * If PDP activation fails, the rilv6 requires return SUCCESS
         * with RIL_Data_Call_Response_v6.status containing the actual
         * failure status.
         */
        rilResponse->status = (RIL_DataCallFailCause)s_lastDataCallFailCause;
        rilResponse->active = 0;   /* link down */
        rilResponse->type = type;
        rilErrno = RIL_E_SUCCESS;
#endif
        goto error__delete_account;
    }

    /*Parse response from EPPSD*/
#if RIL_VERSION >= 6
    parseEPPSDResponse(atresponse, &ipAddrStr, &ipv6AddrStr, &subnetMaskStr, &mtuStr, &pdns, &ipv6pdns, &sdns, &ipv6sdns, curIfName, &activationCause);
    if (ipv6sdns)
        ipv6sdns_len = strlen(ipv6sdns) + 1;
#else
    parseEPPSDResponse(atresponse, &ipAddrStr, &subnetMaskStr, &mtuStr, &pdns, &sdns, curIfName);
#endif
    if (sdns)
        sdns_len = strlen(sdns) + 1;

    /* ---------------------------------------------------------------------- *
     * -------------------- CONFIGURING NET INTERFACE ----------------------- *
     * ---------------------------------------------------------------------- */
    /* Disabling any existing old interface with same ID */
    if (rtnl_if_disable(curIfName)) {
        ALOGE("%s() failed to bring down %s!", __func__, curIfName);
        goto error__deactivate_pdp;
    }

    /* Setup interface and add default route */
#if RIL_VERSION >= 6
    if (1 == dualmode) {
        ALOGI("%s : Dual Mode PDP",__func__);
        if (NULL != ipv6AddrStr) {
          if (configureNetworkInterfaceIPv6(&ipv6AddrStr, ipv6subnetMaskStr, mtuStr, &defaultGatewayStr, curIfName) < 0)
              goto error__netdev_down;
        }
        if (NULL != ipAddrStr) {
          memset(&hint, 0, sizeof(hint));
          hint.ai_family = AF_UNSPEC;
          if (getaddrinfo(ipAddrStr, 0, &hint, &info))
              goto error__netdev_down;
          if (AF_INET6 == info->ai_family) {
              if (configureNetworkInterfaceIPv6(&ipAddrStr, ipv6subnetMaskStr, mtuStr, &defaultGatewayStr, curIfName) < 0)
                 goto error__netdev_down;
          }
          else {
              if (configureNetworkInterface(ipAddrStr, subnetMaskStr, mtuStr, &defaultGatewayStr, curIfName) < 0)
                 goto error__netdev_down;
          }
        }
    }else
#endif
    /* Ipv6 specific */
    if (inet_type == AF_INET6)
    {
        if (configureNetworkInterfaceIPv6(&ipAddrStr, subnetMaskStr, mtuStr, &defaultGatewayStr, curIfName) < 0)
            goto error__netdev_down;
    } else { /* Ipv4 (Ipv4v6) specific */
        if (configureNetworkInterface(ipAddrStr, subnetMaskStr, mtuStr, &defaultGatewayStr, curIfName) < 0)
            goto error__netdev_down;
    }

    /* Android expects a space separated list of DNS servers */
#if RIL_VERSION >= 6
    if ((NULL != pdns) && (NULL == ipv6pdns)) {
        dnslist = alloca((strlen(pdns) + sdns_len ) * sizeof(char *));
        strcpy(dnslist, pdns);
        if (sdns) {
            strcat(dnslist, " ");
            strcat(dnslist, sdns);
        }
    } else if ((NULL != pdns) && (NULL != ipv6pdns)) {
        dnslist = alloca((strlen(pdns) + sdns_len + strlen(ipv6pdns) + ipv6sdns_len ) * sizeof(char *));
        strcpy(dnslist, pdns);
        if (sdns) {
            strcat(dnslist, " ");
            strcat(dnslist, sdns);
        }
        strcat(dnslist, " ");
        strcat(dnslist, ipv6pdns);
        if (ipv6sdns) {
            strcat(dnslist, " ");
            strcat(dnslist, ipv6sdns);
        }
    }

    if ((NULL != ipAddrStr)   && (NULL == ipv6AddrStr)) {
        ipAddrlist = alloca((strlen(ipAddrStr) + 1) * sizeof(char*));
        strcpy(ipAddrlist, ipAddrStr);
    } else if ((NULL != ipAddrStr)   && (NULL != ipv6AddrStr)) {
        ipAddrlist = alloca((strlen(ipAddrStr) + strlen(ipv6AddrStr) + 2) * sizeof(char*));
        strcpy(ipAddrlist, ipAddrStr);
        strcat(ipAddrlist, " ");
        strcpy(ipAddrlist, ipv6AddrStr);
    }
#else
    dnslist = alloca((strlen(pdns) + sdns_len ) * sizeof(char *));
    strcpy(dnslist, pdns);
    if (sdns) {
        strcat(dnslist, " ");
        strcat(dnslist, sdns);
    }
#endif

    /* Create new entry in Contextlist */
    if (!pdpListPut(pdpListHandle, pid, APN, 1, 0, dnslist, (defaultGatewayStr == NULL ? "" : defaultGatewayStr))) {
        ALOGE("%s() failed to add PDP to context list", __func__);
        goto error__netdev_down;
    }

    asprintf(&curCidStr, "%d", curCid);

#if RIL_VERSION >= 6
    rilResponse->cid = curCid; /* CID */
    rilResponse->ifname = curIfName;
    rilResponse->addresses = ipAddrlist;
    rilResponse->dnses = dnslist;
    rilResponse->gateways = (defaultGatewayStr==NULL)?"":defaultGatewayStr;
    rilResponse->active = 2;
    rilResponse->type = type;
    rilResponse->suggestedRetryTime = -1; // Use default value
    rilResponse->status = getActivationCause(activationCause);
    if ( rilResponse->status == PDP_FAIL_ONLY_SINGLE_BEARER_ALLOWED ) {
        rilResponse->type = (AF_INET6 == info->ai_family) ? "ipv6" : "ip";
        ALOGI("PDP type is updated to (%s) since activation cause is PDP_FAIL_ONLY_SINGLE_BEARER_ALLOWED",rilResponse->type);
    }
#else
    /* Allocate and fill in response */
    rilResponseLen = 5 * sizeof(char *);
    rilResponse = alloca(rilResponseLen);
    if (rilResponse == NULL) {
        goto exit;
    }
    memset(rilResponse, 0, rilResponseLen);
    rilResponse[0] = curCidStr;
    rilResponse[1] = curIfName;
    rilResponse[2] = ipAddrStr;
    rilResponse[3] = dnslist;
    rilResponse[4] = defaultGatewayStr;
#endif

    rilErrno = RIL_E_SUCCESS;
    goto exit;

error__netdev_down:
    ALOGD("%s() errorhandler: Trying to take down net interface", __func__);

     if (rtnl_if_disable(curIfName))
         ALOGE("%s() failed to bring down %s!", __func__, curIfName);

error__deactivate_pdp:
    ALOGD("%s() errorhandler: Trying to disconnect pdp context", __func__);

    /* remove any set properties */
    asprintf(&property, "net.%s.gw", curIfName);
    (void)property_set(property, "");
    free(property);
    asprintf(&property, "net.%s.dns1", curIfName);
    (void)property_set(property, "");
    free(property);
    asprintf(&property, "net.%s.dns2", curIfName);
    (void)property_set(property, "");
    free(property);

    asprintf(&cmd, "AT*EPPSD=0,%d,%d", curCid, curCid);
    err = at_send_command(cmd, &atresponse);
    free(cmd);

    if (err < 0 || atresponse->success == 0)
        ALOGE("%s() failed deactivating cid %d!", __func__, curCid);
    free(atresponse);
    atresponse = NULL;

error__delete_account:
    ALOGD("%s() errorhandler: Trying to remove account %d",
         __func__, curCid);

    asprintf(&cmd, "AT+CGDCONT=%d", curCid);
    (void) at_send_command(cmd, NULL);
    free(cmd);

error__unreserve_list_entry:
    ALOGD("%s() errorhandler: Trying to unreserve list entry", __func__);
    (void)pdpListUndo(pdpListHandle);

exit:
    RIL_onRequestComplete(t, rilErrno, rilResponse,
                          (rilResponse == NULL) ? 0 : rilResponseLen);

    if (NULL != ipAddrStr)
    free(ipAddrStr);
    free(subnetMaskStr);
    if (NULL != pdns)
    free(pdns);
    if (NULL != sdns)
    free(sdns);
    free(mtuStr);
    free(defaultGatewayStr);
    free(curCidStr);
#if RIL_VERSION >= 6
    if (NULL != ipv6AddrStr)
        free(ipv6AddrStr);
    if (NULL != ipv6pdns)
        free(ipv6pdns);
    if (NULL != ipv6sdns)
        free(ipv6sdns);
    if (NULL != activationCause)
        free(activationCause);
    if (NULL != info)
        freeaddrinfo(info);
#endif

    at_response_free(atresponse);
}

/**
 * RIL_REQUEST_DEACTIVATE_DATA_CALL
 *
 * Deactivate PDP context created by RIL_REQUEST_SETUP_DATA_CALL.
 *
 * See also: RIL_REQUEST_SETUP_DATA_CALL.
 */
void requestDeactivateDataCall(void *data, size_t datalen, RIL_Token t)
{
    const char *cidStr = NULL;
    int pdpListHandle = -1;
    char *cmd = NULL;
    ATResponse *atresponse = NULL;
    char *property = NULL;
    int err;
    char curIfName[MAX_IFNAME_LEN] = "";

    cidStr = ((const char **) data)[0];

    /* Finding element */
    pdpListHandle = pdpListGet(cidStr, NULL,
                               NULL, NULL, curIfName, NULL, NULL, NULL);
    if (pdpListHandle < 0) {
        ALOGE("%s() issued with non existing Connection ID (cid(%s))",
             __func__, cidStr);
        RIL_onRequestComplete(t, RIL_E_GENERIC_FAILURE, NULL, 0);
        goto exit;
    }

    /*
     * Important: handle must be unreserved before exit.
     * (goto exit not allowed)!!
     */
    ALOGI("%s() found Connection ID (%s) and Interface Name (%s). Deactivating.",
         __func__, cidStr, curIfName);

    /* Bringing down the interface */
    if (rtnl_if_disable(curIfName)) {
        ALOGE("%s() failed to bring down %s!", __func__, curIfName);
        goto error;
    }

    /*
     * Disconnect a PDP context,
     * AT*EPPSD=<state>,<channel_id>,<cid>  state=0 to disconnect
     */
    asprintf(&cmd, "AT*EPPSD=0,%s,%s", cidStr, cidStr);
    err = at_send_command(cmd, &atresponse);
    free(cmd);
    if (err < 0 || atresponse->success == 0)
        ALOGE("%s() failed sending AT*EPPSD for cid %s!", __func__, cidStr);

    /* remove any set properties for the given interface name */
    asprintf(&property, "net.%s.gw", curIfName);
    (void)property_set(property, "");
    free(property);
    asprintf(&property, "net.%s.dns1", curIfName);
    (void)property_set(property, "");
    free(property);
    asprintf(&property, "net.%s.dns2", curIfName);
    (void)property_set(property, "");
    free(property);

    RIL_onRequestComplete(t, RIL_E_SUCCESS, NULL, 0);
    goto exit_remove_account;

error:
    ALOGE("%s() failed for cid %s!", __func__, cidStr);
    RIL_onRequestComplete(t, RIL_E_GENERIC_FAILURE, NULL, 0);

exit_remove_account:
    /* regardless of outcome we remove the entry */
    asprintf(&cmd, "AT+CGDCONT=%s", cidStr);
    (void) at_send_command(cmd, NULL);
    free(cmd);

    pdpListFree(pdpListHandle);

exit:
    free(atresponse);
}

/**
 * RIL_REQUEST_LAST_DATA_CALL_FAIL_CAUSE
 *
 * Requests the failure cause code for the most recently failed PDP
 * context activate.
 *
 * See also: RIL_REQUEST_LAST_CALL_FAIL_CAUSE.
 *
 */
void requestLastPDPFailCause(void *data, size_t datalen, RIL_Token t)
{
    RIL_onRequestComplete(t, RIL_E_SUCCESS, &s_lastDataCallFailCause,
                          sizeof(int));
    /* Clear on read. */
    s_lastDataCallFailCause = PDP_FAIL_ERROR_UNSPECIFIED;
}

/**
 * pdpSetOnOemDeactivated
 *
 * Sets the callback to be called when an OEM activated context is deactivated.
 */
void pdpSetOnOemDeactivated(void (*onOemDeactivated)(int profileId))
{
    pdpOemDeativatedCB = onOemDeactivated;
}
