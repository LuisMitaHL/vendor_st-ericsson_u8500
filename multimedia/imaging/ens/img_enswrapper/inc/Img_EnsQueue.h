/*
* Copyright (C) ST-Ericsson SA 2010. All rights reserved.
* This code is ST-Ericsson proprietary and confidential.
* Any use of the code for whatever purpose is subject to
* specific written permission of ST-Ericsson SA.
*/
#ifndef _IMG_ENSQUEUE_H_
#define _IMG_ENSQUEUE_H_

class IMG_ENSWRAPPER_API Img_EnsQueue
{
public:
	/** Create a queue allowing to store queue_size elements */
	Img_EnsQueue(int queue_size);
	~Img_EnsQueue();
	int Push(void * elem);/** Return 0 in case of success, 1 if queue is full */
	void * Pop();         /** Returns NULL if queue is empty  */
	int nbElem() const;   /** Return the number of elements  */
	void  Reset();          /** Reset all entries */
protected:
	void ** queue;
	/* Head point always to an empty cell */
	int head;
	int tail;
	int size; /* The actual allocated size (queue_size + 1) */
	t_osal_mutex mutex;
};

#endif /* _IMG_ENSQUEUE_H_ */
