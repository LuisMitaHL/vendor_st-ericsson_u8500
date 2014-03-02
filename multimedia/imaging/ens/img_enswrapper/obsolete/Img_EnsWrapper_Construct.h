/*
* Copyright (C) ST-Ericsson SA 2011. All rights reserved.
* This code is ST-Ericsson proprietary and confidential.
* Any use of the code for whatever purpose is subject to
* specific written permission of ST-Ericsson SA.
*/

#ifndef _IMG_ENSWRAPPER_CONSTRUCT_H_
#define _IMG_ENSWRAPPER_CONSTRUCT_H_

#include "Img_EnsQueue.h"


class Img_EnsWrapper_Construct
{
protected:
	// Semaphore to notify of a new message
	t_osal_sem_id m_Semaphore_Process;
	t_osal_sem_id m_Semaphore_Exit;
	// The message queue
	Img_EnsQueue mQueue;

public:

	Img_EnsWrapper_Construct();
	~Img_EnsWrapper_Construct();

	static void Thread(Img_EnsWrapper_Construct *);
	OMX_ERRORTYPE ENSProcessingPartAsync(OMX_HANDLETYPE hComponent);

};

#endif  /* #ifndef _IMG_ENSWRAPPER_CONSTRUCT_H_ */
