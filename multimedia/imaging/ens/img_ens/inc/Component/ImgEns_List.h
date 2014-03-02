/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
/*****************************************************************************/
#ifndef _IMG_ENS_LIST_H_
#define _IMG_ENS_LIST_H_

/// @defgroup ens_list_class ENS List
/// @ingroup ens_utility_classes
/// utility class for handling lists in ENS
/// @{

class ImgEns_ListItem;


/// Concrete class for a list item
class IMGENS_API ImgEns_ListItem
{
public:
	ImgEns_ListItem(void* pData, ImgEns_ListItem * prev, ImgEns_ListItem * next) :
			mData(pData), mPrev(prev), mNext(next)
	{
	}

	~ImgEns_ListItem() {
	}

	void * getData() const {return mData; }

	ImgEns_ListItem * getPrev() { return mPrev; }

	const ImgEns_ListItem * getPrev() const { return mPrev; }

	ImgEns_ListItem * getNext() { return mNext; }

	const ImgEns_ListItem * getNext() const { return mNext; }

	ImgEns_ListItem * setPrev(ImgEns_ListItem * item)
	{
		mPrev = item;
		return mPrev;
	}

	ImgEns_ListItem * setNext(ImgEns_ListItem * item)
	{
		mNext = item;
		return mNext;
	}
private:
	void* mData;
	ImgEns_ListItem * mPrev;
	ImgEns_ListItem * mNext;
};

class ImgEns_List;

/// Pointer to an ImgEns_List
typedef ImgEns_List * ImgEns_List_p;

/// Concrete class for a list
class ImgEns_List
{
public:
	ImgEns_List(): mFirst(0), mLast(0){}

	~ImgEns_List();
	void clear();

	int getSize() const;

	bool isEmpty() const {return (mFirst == 0?true: false); }

	ImgEns_ListItem * getFirst() const { return mFirst; }
	ImgEns_ListItem * getLast()  const { return mLast; }

	ImgEns_ListItem * pushFront(void* pData);
	ImgEns_ListItem * pushBack(void* pData);

	void* popFront();
	void* popBack();

	ImgEns_ListItem * search(void* pData);

protected:
	ImgEns_ListItem * mFirst;
	ImgEns_ListItem * mLast;

	// private copy-constructor and assignment operator to forbid their use
	ImgEns_List(const ImgEns_List &);
	ImgEns_List & operator=(const ImgEns_List &);
};

/// @}

#endif // _IMG_ENS_LIST_H_
