/*                               -*- Mode: C -*- 
 * Copyright (C) 2010, ST-Ericsson
 * 
 * File name       : catd_reader.h
 * Description     : Reader thread
 * 
 * Author          : Mats Bergstrom <mats.m.bergstrom@stericsson.com>
 * 
 */

// FIXME: Rename ste_catd_ --> ste_sim_

#ifndef __catd_reader_h__
#define __catd_reader_h__ (1)

#include "event_stream.h"


/**
 * @brief       Abstract type used to represent the reader thread.
 */
typedef struct ste_catd_reader_s ste_catd_reader_t;

/**
 * @brief       Create a new reader thread object
 * @param in input_queue        Not used.  To be removed.
 * @return      - Pointer to object representing the reader thread.
 *              Returns NULL to indicate failure.
 */
ste_catd_reader_t      *ste_catd_reader_new(void *input_queue);
void                    ste_catd_reader_delete(ste_catd_reader_t * r);

int                     ste_catd_reader_start(ste_catd_reader_t * r);
int                     ste_catd_reader_stop(ste_catd_reader_t * r);

int                     ste_catd_reader_add_es(ste_catd_reader_t * r,
                                               ste_es_t * es);
int                     ste_catd_reader_rem_es(ste_catd_reader_t * r,
                                               int fd);

#endif
