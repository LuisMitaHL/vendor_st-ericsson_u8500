/*                               -*- Mode: C -*- 
 * Copyright (C) 2010, ST-Ericsson
 * 
 * File name       : msgq.h
 * Description     : Message queue and base message definitions
 * 
 * Author          : Mats Bergstrom <mats.m.bergstrom@stericsson.com>
 * 
 */


#ifndef __msgq_h__
#define __msgq_h__ (1)

#include <stdint.h>

// FIXME: DOXYGEN!


// -----------------------------------------------------------------------------
// Generic messages.

typedef struct ste_msg_s ste_msg_t;
typedef void            ste_msg_delete_func(ste_msg_t *);

#define STE_MSG_COMMON uint32_t type; ste_msg_delete_func* delete_func; uintptr_t client_tag
struct ste_msg_s {
    STE_MSG_COMMON;
};

void                    ste_msg_delete(ste_msg_t * msg);

typedef struct ste_msg_s ste_msg_void_t;
ste_msg_void_t         *ste_msg_void_create(int type, uintptr_t client_tag);



// -----------------------------------------------------------------------------
// Message ranges

#define STE_MSG_RANGE_MASK      (0x00ff0000)
#define STE_MSG_RANGE_SIMD      (0x00000000)
#define STE_MSG_RANGE_CATD      (0x00010000)
#define STE_MSG_RANGE_SSIM      (0x00020000)
#define STE_MSG_RANGE_MTBL      (0x00030000)
#define STE_MSG_RANGE_FAIR      (0x00040000)
#define STE_MSG_RANGE_MTBL_SIM  (0x00050000)
#define STE_MSG_RANGE_UICC      (0x00060000)
#define STE_MSG_RANGE_STUB_SIM  (0x00070000)


// -----------------------------------------------------------------------------
// Message queue

typedef struct ste_msgq_s ste_msgq_t;

ste_msgq_t             *ste_msgq_new();
void                    ste_msgq_delete(ste_msgq_t * q);

/**
 * @brief Add a new message at the END of the queue.
 *
 * Adding a message to the queue means that the queue
 * behaves as a "regular" queue (FIFO).
 *
 * If the message cannot be pushed, then the message is lost.
 */
void ste_msgq_add(ste_msgq_t *q, ste_msg_t *msg);


/**
 * @brief Push (add) a new message at the BEGINNING of the queue.
 *
 * Pushing a message to the queue means that the queue
 * behaves as a stack (LIFO).
 *
 * If the message cannot be pushed, then the message is lost.
 */
void ste_msgq_push(ste_msgq_t * q, ste_msg_t * msg);

ste_msg_t              *ste_msgq_pop(ste_msgq_t * q);
ste_msg_t              *ste_msgq_try_pop(ste_msgq_t * q);
ste_msg_t              *ste_msgq_timed_pop(ste_msgq_t * q, unsigned ms);
ste_msg_t              *ste_msgq_pop_filtered(ste_msgq_t * q,const int* filter);

#endif
