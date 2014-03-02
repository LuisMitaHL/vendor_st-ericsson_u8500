#ifndef _STATE_H_
#define _STATE_H_

/************************************************************************
 *                                                                      *
 *  Copyright (C) 2010 ST-Ericsson                                      *
 *                                                                      *
 *  Author: Joakim AXELSSON <joakim.axelsson AT stericsson.com>         *
 *  Author: Sebastian RASMUSSEN <sebastian.rasmussen AT stericsson.com> *
 *                                                                      *
 ************************************************************************/

fsa_fhandle_t state_insertPath(const char *path);
const char *state_lookupHandle(fsa_fhandle_t handle);
fsa_fhandle_t state_lookupPath(const char *path);
void state_removePath(const char *path);

#endif // _STATE_H_
