/*****************************************************************************
* Copyright (C) ST-Ericsson SA 2010. All rights reserved.
* This code is ST-Ericsson proprietary and confidential.
* Any use of the code for whatever purpose is subject to
* specific written permission of ST-Ericsson SA.
*****************************************************************************/
#ifndef _WOM_QUEUE_H_
#define _WOM_QUEUE_H_

class WOM_API Wom_PortQueue
//*************************************************************************************************************
{
public:
	typedef void* t_item;

	Wom_PortQueue();
	~Wom_PortQueue();

	int       init(unsigned int size, bool enabled);
	t_item    getItem(unsigned int i);

	t_item    pop_front();
	int       push_back(t_item);
	int       push_front(t_item);

	unsigned int  getSize()   const { return m_Size ; }
	unsigned int  itemCount() const { if (m_bEnabled) return m_count; else return 0; }
	bool isEmpty()            const { return (itemCount() == 0) ; }
	bool getEnabled()         const { return (m_bEnabled); }
	void setEnabled(bool enabled)   { m_bEnabled = enabled ; }

protected:
	t_item        * m_circ_array;
	unsigned int    m_Size;
	unsigned char   m_front;
	unsigned char   m_count;
	bool            m_bEnabled;
	ste_Mutex       m_Mutex;
};
#endif //_WOM_QUEUE_H_
