/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   PortQueue.h
 * \brief  
 * \author ST-Ericsson
 */
/*****************************************************************************/
#ifndef _QUEUE_H_
#define _QUEUE_H_

#include "ENS_Redefine_Class.h"
#include <stdbool.h>
#include "ENS_macros.h"
#include "armnmf_dbc.h"
#include "nmf_mutex.h" 

typedef void* queue_item_t;


class PortQueue {

public:
ENS_API_IMPORT                 PortQueue();

ENS_API_IMPORT                 ~PortQueue() ;

ENS_API_IMPORT int             init(int size, bool enabled);
ENS_API_IMPORT queue_item_t    getItem(int i); 

ENS_API_IMPORT queue_item_t    pop_front(); 
ENS_API_IMPORT void            push_back(queue_item_t); 
ENS_API_IMPORT void            push_front(queue_item_t); 

int  getSize() { return size ; }
int  itemCount() { 
	if (enabled) return count ;
	else return 0; 
}
bool empty() { return (itemCount() == 0) ; }
void setEnabled(bool enabled) { this->enabled = enabled ; }
bool getEnabled()             { return this->enabled ; }

private:
    queue_item_t *  circ_array ;
    unsigned char   size  ;
    unsigned char   front ;
    unsigned char   count ;
    bool			enabled ;
    
    hMutex			lock ;
} ;

#endif
