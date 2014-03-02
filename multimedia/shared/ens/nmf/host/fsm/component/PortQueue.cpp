/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   PortQueue.cpp
 * \brief  
 * \author ST-Ericsson
 */
/*****************************************************************************/
#include "PortQueue.h"

/* WARNING : 
 * Assumptions for fifo synchronization are:
 *  - Init and destroy are not concurrent with others functions
 *  - One "reader" (getItem, pop_front, push_front) and one "writer" (push_back)
 */
 
/* SYNCHRONIZATION NOTE : 
 *  - 'count' is not always protected by a lock for read, to optimize a little bit. 
 *    It is supposed to happen only when reading an outdated version of 'count' is not a problem.
 * 
 *    By example, itemCount don't use lock. If a component read an outdated value (very rare) in its "process",
 *    we can ensure that another "process" will be scheduled and component will get the updated value.
 */


ENS_API_EXPORT PortQueue::PortQueue()
    : circ_array(0),
      size(0),
      front(0),
      count(0),
      enabled(false),
      lock(0)
{}

ENS_API_EXPORT int PortQueue::init(int size, bool enabled) {
	ARMNMF_DBC_PRECONDITION(size  < 256);
    this->circ_array = 0 ;
    this->lock		 = 0 ;
    
    this->lock = NMF_mutex_create() ;
    if (this->lock == 0) return -1 ;
    
    this->circ_array    = new queue_item_t[size] ;
	if (this->circ_array == 0) return -1 ;		

    this->size          = size;    
    this->front         = 0;
    this->count         = 0; 
    this->enabled 		= enabled;

	return 0 ;
} /* init */


ENS_API_EXPORT PortQueue::~PortQueue() {
	if (this->lock != 0) {
        NMF_mutex_destroy(lock) ;
        lock = 0;
    }
	if (this->circ_array != 0) {
        delete [] this->circ_array ;
        this->circ_array = 0;
    }
} /* ~Queue */


ENS_API_EXPORT queue_item_t PortQueue::getItem(int i) {
    int idx;
    
	ARMNMF_DBC_PRECONDITION(i < (itemCount()));

	NMF_mutex_lock(lock) ; // No race here, lock is for memory barriers
    idx = this->front + i;
    if (idx >= this->size) idx -= this->size;
	NMF_mutex_unlock(lock) ;
	
    return this->circ_array[idx];
} /* getItem */


ENS_API_EXPORT queue_item_t PortQueue::pop_front() {
    queue_item_t front;

	ARMNMF_DBC_PRECONDITION(itemCount() != 0);

    NMF_mutex_lock(lock) ;    
    front = this->circ_array[this->front];
    this->front++ ;
    if (this->front == this->size) {
		this->front = 0;
    }

    this->count--;
    NMF_mutex_unlock(lock) ;

    return front;
} /* pop_front */


ENS_API_EXPORT void PortQueue::push_back(queue_item_t item) {
    int write_index ;    

    NMF_mutex_lock(lock) ;
	
	/* We don't use itemCount() here, to avoid problem if queue is disabled  */
	/* We acces this->count with lock because we have a race with push_front */
    ARMNMF_DBC_PRECONDITION(this->count < this->size) ;
	
    write_index = this->front + count ;
        
    if (write_index >= this->size) {
        write_index -= this->size;
    }    
    this->circ_array[write_index] = item ;
    
    count = count + 1 ;
    NMF_mutex_unlock(lock) ;
} /* push_back */


ENS_API_EXPORT void PortQueue::push_front(queue_item_t item) {
    int write_index ;    

    NMF_mutex_lock(lock) ;
	
	/* We don't use itemCount() here, to avoid problem if queue is disabled */
	/* We acces this->count with lock because we have a race with push_back */
    ARMNMF_DBC_PRECONDITION(this->count < this->size) ;
	
    write_index = this->front -1 ;
    if (write_index < 0) {
        write_index += this->size;
    } 
	
    this->circ_array[write_index] = item ;
    
    this->front = write_index; 
    this->count++ ;
	
    NMF_mutex_unlock(lock) ;
} /* push_front */
