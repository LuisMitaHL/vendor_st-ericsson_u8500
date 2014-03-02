#ifndef _PROCESS_H
#define _PROCESS_H

/************************************************************************
 *                                                                      *
 *  Copyright (C) 2010 ST-Ericsson                                      *
 *                                                                      *
 *  Author: Joakim AXELSSON <joakim.axelsson AT stericsson.com>         *
 *  Author: Sebastian RASMUSSEN <sebastian.rasmussen AT stericsson.com> *
 *                                                                      *
 ************************************************************************/

#include "fsa.h"

#include <stdbool.h>

/**********************************************
 * Functions
 **********************************************/

void process_request(fsa_packet_t *req, fsa_packet_t *resp);
void process_setRoot(const char *root);
void process_cleanup(void);


#endif // _PROCESS_H

