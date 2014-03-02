/*****************************************************************************
* Copyright (C) ST-Ericsson SA 2010. All rights reserved.
* This code is ST-Ericsson proprietary and confidential.
* Any use of the code for whatever purpose is subject to
* specific written permission of ST-Ericsson SA.
*****************************************************************************/
#include <stdio.h>
#include <string.h>
#include "wom_shared.h"
#include "wom_queue.h"

Wom_PortQueue::Wom_PortQueue()
: m_circ_array(0)
//*************************************************************************************************************
{
	m_Size       = 0;
	m_front      = 0;
	m_count      = 0;
	m_Mutex      = 0;
	m_bEnabled   = false;
	m_circ_array = NULL;
}

int Wom_PortQueue::init(unsigned int size, bool enabled)
//*************************************************************************************************************
{
	WOM_ASSERT(size  > 0);
	WOM_ASSERT(size  < 256);
	if (m_Mutex.Create() != S_OK)
		return -1;

	if (m_circ_array !=NULL)
		delete [] m_circ_array;
	m_circ_array    = new t_item[size];
	if (m_circ_array == 0)
		return -1;

	m_Size      = size;
	m_front     = 0;
	m_count     = 0;
	m_bEnabled  = enabled;

	return(S_OK);
} /* init */


Wom_PortQueue::~Wom_PortQueue()
//*************************************************************************************************************
{
	m_Mutex.Destroy();
	if (m_circ_array != 0)
	{
		delete [] m_circ_array;
		m_circ_array = 0;
	}
} 


Wom_PortQueue::t_item Wom_PortQueue::getItem(unsigned int i)
//*************************************************************************************************************
{
	unsigned int idx;
	WOM_ASSERT(i < (itemCount()));
	m_Mutex.Lock() ; // No race here, m_Mutex is for memory barriers
	idx = m_front + i;
	if (idx >= m_Size) 
		idx -= m_Size;
	m_Mutex.Unlock();
	return m_circ_array[idx];
}

Wom_PortQueue::t_item Wom_PortQueue::pop_front()
//*************************************************************************************************************
{
	t_item item;

	WOM_ASSERT(itemCount() != 0);

	m_Mutex.Lock();
	item = m_circ_array[m_front];
	m_front++;
	if (m_front == m_Size)
	{
		m_front = 0;
	}

	m_count--;
	m_Mutex.Unlock();

	return item;
}

int Wom_PortQueue::push_back(t_item item)
//*************************************************************************************************************
{
	unsigned int write_index;
	m_Mutex.Lock();
	/* We don't use itemCount() here, to avoid problem if queue is disabled  */
	/* We acces m_count with m_Mutex because we have a race with push_front */
	WOM_ASSERT(m_count < m_Size);

	write_index = m_front + m_count;

	if (write_index >= m_Size)
	{
		write_index -= m_Size;
	}
	m_circ_array[write_index] = item;

	m_count = m_count + 1;
	m_Mutex.Unlock();
	return(S_OK);
} /* push_back */


int Wom_PortQueue::push_front(t_item item)
//*************************************************************************************************************
{
	unsigned int write_index;
	m_Mutex.Lock();
	/* We don't use itemCount() here, to avoid problem if queue is disabled */
	/* We acces m_count with m_Mutex because we have a race with push_back */
	WOM_ASSERT(m_count < m_Size);

	write_index = m_front -1;
	if (write_index < 0)
	{
		write_index += m_Size;
	}

	m_circ_array[write_index] = item;

	m_front = (unsigned char)write_index;
	m_count++;

	m_Mutex.Unlock();
	return(S_OK);
}
