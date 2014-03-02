/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 * \brief  PS Connection Control Protocol Configuration Option
 */


/********************************************************************************
 * Include Files
 ********************************************************************************/
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include "pscc_pco.h"
#include "pscc_msg.h"

/********************************************************************************
 *
 * Defines & Type definitions
 *
 ********************************************************************************/
#define PROTOCOL_ID_UNDEF                          0
#define PROTOCOL_ID_IPCP                           0x8021
#define PROTOCOL_ID_LCP                            0xC021
#define PROTOCOL_ID_PAP                            0xC023
#define PROTOCOL_ID_CHAP                           0xC223
#define CONTAINER_ID_PCSCF_ADDR                    0x0001
#define CONTAINER_ID_IM_CN_SIG_FLAG                0x0002
#define CONTAINER_ID_DNS6_ADDR                     0x0003
#define CONTAINER_ID_POLICY_CTRL_REJ_CODE          0x0004


#define CONFIGURE_REQUEST                 (1)
#define CONFIGURE_ACK                     (2)
#define CONFIGURE_NAK                     (3)
#define CONFIGURE_REJECT                  (4)
#define OPTION_IP_ADDRESS                 (3)
#define OPTION_PRIMARY_DNS_ADDRESS      (129)
#define OPTION_SECONDARY_DNS_ADDRESS    (131)

#define CHAP_CHALLENGE                    (1)
#define CHAP_RESPONSE                     (2)
#define UPAP_AUTHREQ                      (1)

#define CONFIGURATION_PROT_PPP         (0x80)

/********************************************************************************
 *
 * Global variables
 *
 ********************************************************************************/

/********************************************************************************
 *
 * Local variables
 *
 ********************************************************************************/

/********************************************************************************
 *
 * Private function prototypes
 *
 ********************************************************************************/
static int get_next_pco_protocol
  (
   bool    isfirst,
   const uint8_t *pco_p,
   uint8_t   pcolen,
   uint16_t *protocol_p,
   uint8_t *protocollen_p
  );

static int get_pco_ipcp
  (
   uint8_t pcolen,
   const uint8_t *pco_p,
   uint8_t protocollen,
   mpl_list_t **param_list_pp
  );

static int add_pco_chap
  (
   uint8_t        chapid,
   const char    *uid_p,
   const uint8_t *challenge_p,
   size_t         challengelen,
   const uint8_t *response_p,
   size_t         responselen,
   uint8_t       *pco_p,
   size_t         pcolen
  );

static int add_pco_pap
  (
   uint8_t     id,
   const char *uid_p,
   const char *pwd_p,
   uint8_t    *pco_p,
   size_t      pcolen
  );


/*******************************************************************************
 *
 * Public Functions
 *
 *******************************************************************************/
/*
 * pscc_pco_get()
 */
mpl_list_t *pscc_pco_get
  (
   const uint8_t *pco_p,
   uint8_t pcolen
  )
{
  uint16_t protocol;
  uint8_t protocollen=0;
  int indexpco = 0;
  mpl_list_t *param_list_p=NULL;

  if(pcolen == 0)
    return (NULL);

  indexpco += get_next_pco_protocol(true,
                                   pco_p,
                                   pcolen,
                                   &protocol,
                                   &protocollen);

  while ((protocol != PROTOCOL_ID_UNDEF) &&
         ((indexpco + protocollen) <= pcolen))
  {
    switch(protocol)
    {
    case PROTOCOL_ID_IPCP:
      indexpco += get_pco_ipcp((uint8_t)(pcolen - indexpco),
                               &pco_p[indexpco],
                               protocollen,
                               &param_list_p);

      break;
    case CONTAINER_ID_IM_CN_SIG_FLAG:
    case CONTAINER_ID_DNS6_ADDR:
    case CONTAINER_ID_POLICY_CTRL_REJ_CODE:
    case PROTOCOL_ID_LCP:
    case PROTOCOL_ID_PAP:
    case PROTOCOL_ID_CHAP:
    default:
      indexpco += protocollen;
      break;
    }
    indexpco += get_next_pco_protocol(false,
                                      &pco_p[indexpco],
                                      (uint8_t)(pcolen - indexpco),
                                      &protocol,
                                      &protocollen);
  }
  return (param_list_p);
}

/*
 * pscc_pco_add_auth()
 */
int pscc_pco_add_auth
  (
   pscc_auth_method_t auth_method,
   const char *uid_p,
   const char *pwd_p,
   uint8_t  *challenge_p,
   size_t    challengelen,
   uint8_t  *response_p,
   size_t    responselen,
   uint8_t   authid,
   bool      isempty,
   uint8_t  *pco_p,
   size_t    pcolen
  )
{
  int index = 0;
  int tmp;

  if((NULL == uid_p) || (NULL == pwd_p) || (NULL == pco_p))
    return 0;

  switch (auth_method)
  {
  case pscc_auth_method_none:
    /* Do nothing */
    break;

  case pscc_auth_method_pap:
    if (isempty)
    {
      if(1 > pcolen)
        return (-1);
      pco_p[index++] = CONFIGURATION_PROT_PPP;
    }

    tmp = add_pco_pap(authid,
                      uid_p,
                      pwd_p,
                      &pco_p[index],
                      (isempty?(pcolen-1):pcolen));
    if(0 > tmp)
      return (tmp);

    index += tmp;
    break;

  case pscc_auth_method_chap:
    if (isempty)
    {
      if(1 > pcolen)
        return (-1);
      pco_p[index++] = CONFIGURATION_PROT_PPP;
    }

    tmp = add_pco_chap(authid,
                       uid_p,
                       challenge_p,
                       challengelen,
                       response_p,
                       responselen,
                       &pco_p[index],
                       (isempty?(pcolen-1):pcolen));
    if(0 > tmp)
      return (tmp);

    index += tmp;
    break;
  default:
    break;
  }
  return index;
}

int pscc_pco_add_ipcp
  (
   uint32_t  static_ip,
   bool      isempty,
   uint8_t  *pco_p,
   size_t    pcolen
  )
{
  int index=0;
  static uint8_t ipcp_id = 1;
  static const uint32_t zerodns=0;

  /* can we fit the ipcp parameters in the pco array?
     13 = the number of times index is incremented by one
     3*4 =  3 options containing an ipv4 address
   */
  if(pcolen < (size_t)(13+(3*4)+(isempty?1:0)))
    return (-1);

  if (isempty)
    pco_p[index++] = CONFIGURATION_PROT_PPP;

  /*
   * IPCP request
   */
  pco_p[index++] = (PROTOCOL_ID_IPCP >> 8) & 0xff;   /* index++ #1 */
  pco_p[index++] = PROTOCOL_ID_IPCP & 0xff;          /* index++ #2 */

/* 4 (length of IPCP header) + 18 (3 options each of length 6) */
  pco_p[index++] = 22;                               /* index++ #3 */

  pco_p[index++] = CONFIGURE_REQUEST;                /* index++ #4 */
  pco_p[index++] = ipcp_id++;                        /* index++ #5 */
  pco_p[index++] = 0;                                /* index++ #6 */

  /* 4 (length of IPCP header) + 18 (3 options each of length 6) */
  pco_p[index++] = 22;                               /* index++ #7 */

  pco_p[index++] = OPTION_IP_ADDRESS;                /* index++ #8 */
  pco_p[index++] = 6; /* Length of option*/          /* index++ #9 */
  memcpy(&pco_p[index],&static_ip,4);
  index += 4;

  /* always add options DNS Servers */
  pco_p[index++] = OPTION_PRIMARY_DNS_ADDRESS;       /* index++ #10 */
  pco_p[index++] = 6; /* Length of option*/          /* index++ #11 */
  memcpy(&pco_p[index],&zerodns,4);
  index += 4;

  pco_p[index++] = OPTION_SECONDARY_DNS_ADDRESS;     /* index++ #12 */
  pco_p[index++] = 6; /* Length of option*/          /* index++ #13 */
  memcpy(&pco_p[index],&zerodns,4);
  index += 4;

  return index;
}


/*******************************************************************************
 *
 * Private Functions
 *
 *******************************************************************************/

/*
 * get_next_pco_protocol()
 */
static int get_next_pco_protocol
  (
   bool isfirst,
   const uint8_t *pco_p,
   uint8_t pcolen,
   uint16_t *protocol_p,
   uint8_t *protocollen_p
  )
{
  int index = 0;

  assert(protocol_p != NULL);
  assert(protocollen_p != NULL);
  assert(pco_p != NULL);

  *protocol_p = PROTOCOL_ID_UNDEF;

  if (isfirst)
  {
    if (pcolen < 4)
    {
      return (pcolen+1);
    }

    if (pco_p[index++] != 0x80)
    {
      return (pcolen+1);
    }
  }

  if ((index+2) < pcolen)
  {
    int nextprotstart;

    *protocol_p  = pco_p[index++]*256;
    *protocol_p += pco_p[index++];
    *protocollen_p = pco_p[index++];

    nextprotstart = index + *protocollen_p;

    if (nextprotstart > pcolen)
    {
      return (pcolen+1);
    }
  }
  else
    return (pcolen+1);

  return (index);
}


/*
 * get_pco_ipcp()
 */
static int get_pco_ipcp
  (
   uint8_t pcolen,
   const uint8_t *pco_p,
   uint8_t protocollen,
   mpl_list_t **param_list_pp
  )
{
  int index = 0;
  int nextprotstart;
  mpl_param_element_t *param_elem_p;
  char ipaddr[sizeof("255.255.255.255")];

  nextprotstart = index + protocollen;

  if (nextprotstart > pcolen)
  {
    return pcolen;
  }

  while ((index+3) < nextprotstart)
  {
    uint8_t code;
    uint16_t codelen;
    int next_ipcp_start;

    code = pco_p[index++];
    index++;
    codelen = pco_p[index++];
    codelen += pco_p[index++];

    next_ipcp_start = index + (codelen - 4); /* Subtract the read header octets */

    if (next_ipcp_start > nextprotstart)
    {
      return pcolen;
    }

    switch (code)
    {
    case CONFIGURE_REQUEST:
      while ((index+1) < next_ipcp_start)
      {
        uint8_t option = pco_p[index++];
        uint8_t optionlen = pco_p[index++];

        switch (option)
        {
        case OPTION_IP_ADDRESS:
          if (optionlen == 6)
          {
            /* Add GW address to parameter list*/
            if(NULL != inet_ntop(AF_INET,&pco_p[index],ipaddr,sizeof(ipaddr)))
            {
              param_elem_p = mpl_param_element_create_stringn(pscc_paramid_gw_address,
                                                                  ipaddr, strlen(ipaddr));
              if(param_elem_p != NULL)
                mpl_list_add(param_list_pp, &param_elem_p->list_entry);
            }
          }
          break;
        default:
          /* Ignore all other options in Configure Request */
          break;
        }
        index += (optionlen - 2);/* Subtract read header octets */
      }
      break;

    case CONFIGURE_ACK:     /* We don't care if we receive params as NAK or ACK */
    case CONFIGURE_NAK:
      while ((index+1) < next_ipcp_start)
      {
        uint8_t option = pco_p[index++];
        uint8_t optionlen = pco_p[index++];

        switch (option)
        {
        case OPTION_IP_ADDRESS:
          if (optionlen == 6)
          {
            if(NULL != inet_ntop(AF_INET,&pco_p[index],ipaddr,sizeof(ipaddr)))
            {
              param_elem_p = mpl_param_element_create_stringn(pscc_paramid_own_ip_address,
                                                                  ipaddr, strlen(ipaddr));
              if(param_elem_p != NULL)
                mpl_list_add(param_list_pp, &param_elem_p->list_entry);
            }
          }
          break;
        case OPTION_PRIMARY_DNS_ADDRESS:
          if (optionlen == 6)
          {
            if(NULL != inet_ntop(AF_INET,&pco_p[index],ipaddr,sizeof(ipaddr)))
            {
              param_elem_p = mpl_param_element_create_stringn(pscc_paramid_dns_address,
                                                              ipaddr, strlen(ipaddr));
              if(param_elem_p != NULL)
                mpl_list_add(param_list_pp, &param_elem_p->list_entry);
            }
          }
          break;
        case OPTION_SECONDARY_DNS_ADDRESS:
          if (optionlen == 6)
          {
            if(NULL != inet_ntop(AF_INET,&pco_p[index],ipaddr,sizeof(ipaddr)))
            {
              param_elem_p = mpl_param_element_create_stringn(pscc_paramid_secondary_dns_address,
                                                              ipaddr, strlen(ipaddr));
              if(param_elem_p != NULL)
                mpl_list_add(param_list_pp, &param_elem_p->list_entry);
            }
          }
          break;
        default:
          /* Ignore all other options in Configure Request */
          break;
        }
        index += (optionlen - 2);/* Subtract read header octets */
      }
      break;

    case CONFIGURE_REJECT:
      break;
    default:
      /* Ignore all other IPCP packet types */
      break;
    }
    index = next_ipcp_start;
  }
  return nextprotstart;
}

/*
 * add_pco_chap()
 */
static int add_pco_chap
  (
   uint8_t        chapid,
   const char    *uid_p,
   const uint8_t *challenge_p,
   size_t         challengelen,
   const uint8_t *response_p,
   size_t         responselen,
   uint8_t       *pco_p,
   size_t         pcolen
  )
{
  int index = 0;
  size_t uidlen;

  assert(uid_p != NULL);
  assert(pco_p != NULL);

  if((NULL == challenge_p) || (NULL == response_p))
    return (-1);

  uidlen = strlen(uid_p);

  /* can we fit the chap parameters in the pco array?
     17 = the number of times index is incremented by one
   */
  if(pcolen < (17+challengelen+responselen+uidlen))
    return (-1);

  /*
   * CHAP Challenge
   */
  pco_p[index++] = (PROTOCOL_ID_CHAP >> 8) & 0xff;        /* index++ #1 */
  pco_p[index++] = PROTOCOL_ID_CHAP & 0xff;               /* index++ #2 */
  pco_p[index++] = (uint8_t)challengelen+6;               /* index++ #3 */
  pco_p[index++] = CHAP_CHALLENGE;                        /* index++ #4 */
  pco_p[index++] = chapid;                                /* index++ #5 */
  pco_p[index++] = (uint8_t)((challengelen+6)/256);       /* index++ #6 */
  pco_p[index++] = (uint8_t)((challengelen+6)%256);       /* index++ #7 */
  pco_p[index++] = (uint8_t)challengelen;                 /* index++ #8 */
  memcpy(&pco_p[index], challenge_p, challengelen);
  index += challengelen;
  pco_p[index++] = ' ';            /* No name */          /* index++ #9 */

  /*
   * CHAP Response
   */
  pco_p[index++] = (PROTOCOL_ID_CHAP >> 8) & 0xff;        /* index++ #10 */
  pco_p[index++] = (PROTOCOL_ID_CHAP & 0xff);             /* index++ #11 */
  pco_p[index++] = (uint8_t)(responselen+uidlen+5);       /* index++ #12 */

  pco_p[index++] = CHAP_RESPONSE;                         /* index++ #13 */
  pco_p[index++] = chapid;                                /* index++ #14 */
  pco_p[index++] = (uint8_t)((responselen+uidlen+5)/256); /* index++ #15 */
  pco_p[index++] = (uint8_t)((responselen+uidlen+5)%256); /* index++ #16 */
  pco_p[index++] = (uint8_t)responselen;                  /* index++ #17 */
  memcpy(&pco_p[index], response_p, responselen);
  index += responselen;
  memcpy(&pco_p[index], uid_p, uidlen);
  index += uidlen;

  return (index);
}

/*
 * add_pco_pap()
 */
static int add_pco_pap
  (
   uint8_t     papid,
   const char *uid_p,
   const char *pwd_p,
   uint8_t    *pco_p,
   size_t      pcolen
  )
{
  int index = 0;
  size_t uidlen,pwdlen;

  assert(uid_p != NULL);
  assert(pco_p != NULL);

  uidlen = strlen(uid_p);
  pwdlen = strlen(pwd_p);

  /* can we fit the pap parameters in the pco array?
     9 = the number of times index is incremented by one
  */
  if(pcolen < (9+uidlen+pwdlen))
    return (-1);
  /*
   * PAP Request
   */
  pco_p[index++] = (PROTOCOL_ID_PAP >> 8) & 0xff;    /* index++ #1 */
  pco_p[index++] = PROTOCOL_ID_PAP & 0xff;           /* index++ #2 */
  pco_p[index++] = (uint8_t)(uidlen+pwdlen+6);       /* index++ #3 */

  pco_p[index++] = UPAP_AUTHREQ;                     /* index++ #4 */
  pco_p[index++] = papid;                            /* index++ #5 */
  pco_p[index++] = (uint8_t)((uidlen+pwdlen+6)/256); /* index++ #6 */
  pco_p[index++] = (uint8_t)((uidlen+pwdlen+6)%256); /* index++ #7 */
  pco_p[index++] = (uint8_t)uidlen;                  /* index++ #8 */
  memcpy(&pco_p[index], uid_p, uidlen);
  index += uidlen;
  pco_p[index++] = (uint8_t)pwdlen;                  /* index++ #9 */
  memcpy(&pco_p[index], pwd_p, pwdlen);
  index += pwdlen;

  return index;
}
