/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef LIBMLR_H
#define LIBMLR_H

enum {
    SK_MODEM_LOG_RELAY,
    SK_MODEM_SUPVISOR
};



#define MSG_GEN_MODEM_COREDUMP  "generate_modem_coredump"
#define MSG_MODEM_READY         "modem_ready"
#define MLR_SUP_DGRAM_SK_LENGTH_MSG_MAX 0x100



#ifdef LIBMLR_C

enum {
    MLR_SUP_SK_REQ,
    MLR_SUP_SK_RSP,
    MLR_SUP_SK_MAX
};



char *mlr_sup_dgram_sk_name[] = {"/dev/socket/mlr_sup_sk_req",
                                 "/dev/socket/mlr_sup_sk_rsp"
                                };

struct dgram_conn_t  *mlr_sup_dgram_sk[MLR_SUP_SK_MAX];

int   mlr_sup_dgram_sk_type[2][2] = {   { DGRAM_SK_CLIENT, DGRAM_SK_SERVER }, \
    { DGRAM_SK_SERVER, DGRAM_SK_CLIENT }
};
#endif

int mlr_sup_init_sk(int ident);

int mlr_send_coredump_gen(int timeout);
void mlr_send_modem_ready(void);

int mlr_sup_sk_send(char *msg, int len);
int mlr_sup_sk_recv(char *msg);
int sup_mlr_sk_send(char *msg, int len);
int sup_mlr_sk_recv(char *msg, int timeout);


#undef LIBMLR_C

#endif //#ifndef LIBMLR_H
