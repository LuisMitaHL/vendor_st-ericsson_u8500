/*****************************************************************************
* Copyright (C) ST-Ericsson SA 2010. All rights reserved.
* This code is ST-Ericsson proprietary and confidential.
* Any use of the code for whatever purpose is subject to
* specific written permission of ST-Ericsson SA.
*****************************************************************************/
#ifndef __SMARTPOINTER_H
#define __SMARTPOINTER_H

// see more effective C++ for reference about this code
class NonCopyableClass
{
public : 
	NonCopyableClass() {};
	virtual ~NonCopyableClass() {};
private :
	NonCopyableClass(const NonCopyableClass & copy) {};
	virtual NonCopyableClass& operator= 
		(const NonCopyableClass& n) throw() 
	{return *this;};

};


class NonCopyableReferenceCountedClass :public NonCopyableClass
{
public:
	virtual void addReference();
	virtual void removeReference();
	int  GetCount() const;

public:
	NonCopyableReferenceCountedClass();
	virtual ~NonCopyableReferenceCountedClass() = 0;
private:
	int refCount;
};


NonCopyableReferenceCountedClass::NonCopyableReferenceCountedClass()
: refCount(0)
{
}

NonCopyableReferenceCountedClass::~NonCopyableReferenceCountedClass()
{
}

void NonCopyableReferenceCountedClass::addReference() 
{
	++refCount;
}

void NonCopyableReferenceCountedClass::removeReference()
{
	if (--refCount == 0) delete this;
}

int NonCopyableReferenceCountedClass::GetCount() const
{
	return refCount;
}  

///////////////////////////////////////////////////////////////////////////
// NCIRC :  Non Copyable Indirect Reference Counted class  for reference //
template<class T>
class NCIRC_AutoPtr 
{
public:
	NCIRC_AutoPtr(T* realPtr = 0);
	NCIRC_AutoPtr(const NCIRC_AutoPtr& rhs);
	~NCIRC_AutoPtr();
	NCIRC_AutoPtr& operator=(const NCIRC_AutoPtr& rhs);
	bool operator==(const T* rhs);
	bool operator!=(const T* rhs);

	T* operator->() const;
	T& operator*() const;

	NonCopyableReferenceCountedClass& getReferenceCounter()  // give clients access to
	{ return *counter; }     // isShared, etc.
	T* getPointee() // give access to a reference for type checking
	{ return counter->pointee; }

private:
	struct CountHolder: public NonCopyableReferenceCountedClass
	{
		~CountHolder() { delete pointee; }
		T *pointee;
	};

	CountHolder *counter;
	void init();
};

template<class T>
void NCIRC_AutoPtr<T>::init()
{
	counter->addReference();
}

template<class T>
NCIRC_AutoPtr<T>::NCIRC_AutoPtr(T* realPtr)
: counter(new CountHolder)
{ 
	counter->pointee = realPtr;
	init();
}

template<class T>
void NCIRC_AutoPtr<T>::SetPointee(T* NewPointee)
{
	if (counter->pointee != NULL)
	{
		delete counter->pointee;
	}
	counter->pointee = NewPointee;
}


template<class T>
NCIRC_AutoPtr<T>::NCIRC_AutoPtr(const NCIRC_AutoPtr& rhs)
: counter(rhs.counter)
{ init(); }

template<class T>
NCIRC_AutoPtr<T>::~NCIRC_AutoPtr()
{ counter->removeReference(); }

template<class T>
NCIRC_AutoPtr<T>& NCIRC_AutoPtr<T>::operator=(const NCIRC_AutoPtr& rhs)
{
	if (counter != rhs.counter) 
	{
		counter->removeReference();
		counter = rhs.counter;
		init();
	}
	return *this;
}

template<class T>
bool NCIRC_AutoPtr<T>::operator==(const T* rhs)
{
	return (counter->pointee == rhs);
}

template<class T>
bool NCIRC_AutoPtr<T>::operator!=(const T* rhs)
{
	return (counter->pointee != rhs);
}


template<class T>
T* NCIRC_AutoPtr<T>::operator->() const
{ return counter->pointee; }

template<class T>
T& NCIRC_AutoPtr<T>::operator*() const
{ return *(counter->pointee); }

#endif //__SMARTPOINTER_H
