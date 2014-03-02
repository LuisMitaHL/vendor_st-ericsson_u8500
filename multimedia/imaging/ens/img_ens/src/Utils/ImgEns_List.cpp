/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
/*****************************************************************************/
#include "ImgEns_Shared.h"
#include "ImgEns_List.h"


ImgEns_List::~ImgEns_List()
{
	ImgEns_List::clear();
}

void ImgEns_List::clear()
{
	ImgEns_ListItem * item = mFirst;
	ImgEns_ListItem * next_item;

	while (item) 
	{
		next_item = item->getNext();
		delete item;
		item = next_item;
	}
	mFirst = 0;
}

int ImgEns_List::getSize() const
{
	ImgEns_ListItem * item = mFirst;
	int size = 0;

	while (item)
	{
		item = item->getNext();
		size++;
	}

	return size;
}

ImgEns_ListItem * ImgEns_List::pushFront(void* pData) {
	ImgEns_ListItem * item = new ImgEns_ListItem(pData, 0, mFirst);
	if (!item)
	{
		return 0;
	}

	if (mFirst)
	{
		mFirst->setPrev(item);
	}
	else
	{
		mLast = item;
	}

	mFirst = item;

	return item;
}

ImgEns_ListItem * ImgEns_List::pushBack(void* pData)
{
	ImgEns_ListItem * item = new ImgEns_ListItem(pData, mLast, 0);

	if (!item)
	{
		return 0;
	}

	if (mLast)
	{
		mLast->setNext(item);
	}
	else
	{
		mFirst = item;
	}

	mLast = item;

	return item;
}

void* ImgEns_List::popFront()
{
	if (mFirst)
	{
		ImgEns_ListItem * tmp = mFirst;
		void* pData = mFirst->getData();

		mFirst = tmp->getNext();

		if (mFirst)
		{
			mFirst->setPrev(0);
		}
		else
		{
			mLast = 0;
		}

		delete tmp;
		return pData;
	}

	return 0;
}

void* ImgEns_List::popBack()
{
	if (mLast)
	{
		ImgEns_ListItem * tmp = mLast;
		void* pData = mLast->getData();

		mLast = tmp->getPrev();

		if (mLast)
		{
			mLast->setNext(0);
		}
		else
		{
			mFirst = 0;
		}

		delete tmp;
		return pData;
	}

	return 0;
}

ImgEns_ListItem * ImgEns_List::search(void* pData)
{
	ImgEns_ListItem * item = mFirst;

	while (item)
	{
		if (item->getData() == pData)
		{
			return item;
		}
		item = item->getNext();
	}

	return 0;
}

