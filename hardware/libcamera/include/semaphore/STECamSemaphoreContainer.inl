/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
*/

#ifndef STECAMSEMAPHORECONTAINER_INL
#define STECAMSEMAPHORECONTAINER_INL

inline SemaphoreContainer::SemaphoreContainer():
                           mMaxOutstandingEvents(0)
{
    DBGT_PROLOG("");

    DBGT_EPILOG("");
    return;
}

SemaphoreContainer::~SemaphoreContainer()
{
    DBGT_PROLOG("");

    //destroy semaphore
    sem_destroy(&mSem);

    DBGT_EPILOG("");
    return;
}

inline status_t SemaphoreContainer::init(uint32_t aMaxOutstandingEvents)
{
    DBGT_PROLOG("Max events: %d", aMaxOutstandingEvents);

    //check
    DBGT_ASSERT(0 != aMaxOutstandingEvents, "Max events is 0");

    status_t err = NO_ERROR;

    err = sem_init(&mSem, 0, 0);
	if(-1 == err) {
		DBGT_CRITICAL("RequestSem init failed err = %d", err);
		DBGT_EPILOG("");
		return err;
	}

    //save
    mMaxOutstandingEvents = aMaxOutstandingEvents;

	DBGT_EPILOG("");
	return err;
}

inline void SemaphoreContainer::wait()
{
    DBGT_PROLOG("");

    check();
    sem_wait(&mSem);

    DBGT_EPILOG("");
    return;
}

inline void SemaphoreContainer::signal()
{
    DBGT_PROLOG("");

    sem_post(&mSem);
    check();

    DBGT_EPILOG("");
    return;
}

inline void SemaphoreContainer::check()
{
#ifdef CAM_DEBUG
    DBGT_PROLOG("");

    //check max events valid
    DBGT_ASSERT(0 != mMaxOutstandingEvents, "MaxOutstandingEvents is 0");

    int value;
    sem_getvalue(&mSem, &value);
    DBGT_ASSERT(mMaxOutstandingEvents >= (uint32_t)value,
               "Mismatch between semaphore count : %d and MaxOutstandingEvents: %u",
               value, mMaxOutstandingEvents);

    DBGT_EPILOG("");
    return;
#endif //CAM_DEBUG
}

#endif // STECAMSEMAPHORECONTAINER_INL
