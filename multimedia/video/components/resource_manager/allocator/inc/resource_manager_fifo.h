/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef _RESOURCE_MANAGER_FIFO_H_
#define _RESOURCE_MANAGER_FIFO_H_

#define NB_MAX_PENDING_RESOURCE  (128)

#define INC_INDEX(index1, index2) index1=index2+1; if (index1 >= NB_MAX_PENDING_RESOURCE) index1=0;

/* Structure for Allocator descriptor */

typedef struct {
	Iresource_manager_api_inform_resource_status *cb;
//	t_t1xhv_resource  resource;
} ts_resource_pending;

typedef struct {
	t_uint16      rIndex;
	t_uint16      wIndex;
	ts_resource_pending  resourcePendingReq[NB_MAX_PENDING_RESOURCE];
} ts_allocator_desc, *tsp_allocator_desc;

EXTERN EXTMEM ts_allocator_desc s_allocatorDesc[RESOURCE_MAX];

/* Functions */

inline t_uint16 rm_fifo_push(t_t1xhv_resource resource, Iresource_manager_api_inform_resource_status *cb) {
	t_uint16  nextWIndex;

	INC_INDEX(nextWIndex, s_allocatorDesc[resource].wIndex);
	if (nextWIndex != s_allocatorDesc[resource].rIndex){
		/* Ok, there's a place to store pending resource. */
		s_allocatorDesc[resource].resourcePendingReq[s_allocatorDesc[resource].wIndex].cb = cb;
		s_allocatorDesc[resource].wIndex = nextWIndex;
		return TRUE;
	}
	else {
		// fifo full
		return FALSE;
	}
}

inline void rm_fifo_remove(t_t1xhv_resource resource, Iresource_manager_api_inform_resource_status *cb) {
	// cb needed for resource identification (flush)
	t_sint16 index, previous_index;
	t_uint16 found=FALSE;
	
	index = s_allocatorDesc[resource].rIndex;
	while ((index != s_allocatorDesc[resource].wIndex) && (found==FALSE)) {
		// loop through fifo, to find resource identified by cb
		if (s_allocatorDesc[resource].resourcePendingReq[index].cb == cb) {
			found=TRUE;
		}
		else {
			INC_INDEX(index, index);
		}
	}
	if (found==TRUE) {
		// remove the resource
		while (index != s_allocatorDesc[resource].rIndex) {
			previous_index = index-1;
			if (previous_index==-1) previous_index=NB_MAX_PENDING_RESOURCE-1;
			s_allocatorDesc[resource].resourcePendingReq[index].cb = 
				s_allocatorDesc[resource].resourcePendingReq[previous_index].cb;
			index = previous_index;
		}
		INC_INDEX(s_allocatorDesc[resource].rIndex, s_allocatorDesc[resource].rIndex);
	}
}

inline Iresource_manager_api_inform_resource_status *get_rm_fifo_cb(t_t1xhv_resource  resource) {
	return s_allocatorDesc[resource].resourcePendingReq[s_allocatorDesc[resource].rIndex].cb;
}

inline t_uint16 is_rm_fifo_empty(t_t1xhv_resource  resource) {
	return (s_allocatorDesc[resource].rIndex == s_allocatorDesc[resource].wIndex);
}

inline t_uint16 rm_fifo_flush(t_t1xhv_resource  resource) {
	t_sint16 nb_flush;
	nb_flush = s_allocatorDesc[resource].wIndex - s_allocatorDesc[resource].rIndex;
	if (nb_flush<0) nb_flush+=NB_MAX_PENDING_RESOURCE;

	s_allocatorDesc[resource].rIndex = 0;
	s_allocatorDesc[resource].wIndex = 0;

	return nb_flush;
}
	

#endif //_RESOURCE_MANAGER_FIFO_H_

