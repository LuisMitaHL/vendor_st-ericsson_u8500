/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef _ENS_QUEUE_H_
#define _ENS_QUEUE_H_

extern "C" {
#include <los/api/los_api.h>
}

class ENS_Queue {
 private:
    void ** queue;
    /* Head point always to an empty cell */
    int head;
    int tail;
    int size;			/* The actual allocated size (queue_size + 1) */
    t_los_mutex_id mutex;
 public:
    /** Create a queue allowing to store queue_size elements */
    ENS_Queue(int queue_size);
    ~ENS_Queue();
    
    /** Return 0 in case of success, 1 if queue is full */
    int Push(void * elem);
    /** Returns NULL if queue is empty  */
    void * Pop();
    /* Return the number of elements */
    int nbElem();
};

#endif /* _ENS_QUEUE_H_ */


