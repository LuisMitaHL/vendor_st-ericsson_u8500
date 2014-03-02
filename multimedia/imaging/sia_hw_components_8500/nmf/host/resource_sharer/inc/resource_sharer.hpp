/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
/*
* \brief   resource_sharer ARM-NMF component class header
*/
#ifndef _RESOURCE_SHARER_HPP
#define _RESOURCE_SHARER_HPP

#define NB_CLIENT 4

enum e_requesterID {
	CAMERA_RAW,
    ISPPROC_SINGLE_STILL,
    ISPPROC_SINGLE_SNAP,
	ISPPROC_BURST,
	CAMERA_VF,
	REQUESTER_MAX
};


#ifdef __SYMBIAN32__
NONSHARABLE_CLASS(resource_sharer);
#endif
class resource_sharer: public resource_sharerTemplate {


public:
	resource_sharer();
	
	/* interface collection resource_sharer.api.free */
	virtual void Free(t_uint16 res, t_uint8 collectionIdx);
	 /* interface collection resource_sharer.api.ping */
	virtual void Ping(t_uint16 res, t_uint8 collectionIdx);
	 /* interface collection resource_sharer.api.lock */
	virtual void Lock(t_uint16 requesterId, t_uint8 collectionIdx);
	 /* interface collection resource_sharer.api.release */
	virtual void Release(t_uint16 requesterId, t_uint8 collectionIdx);	
	
private:
	t_uint16 G_RequestQueue[REQUESTER_MAX][NB_CLIENT];
	t_uint16 G_Lock;	
    t_uint16 G_Client;
};

#endif /* _RESOURCE_SHARER_HPP */
