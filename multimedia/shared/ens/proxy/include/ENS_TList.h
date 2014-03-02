/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   ENS_TList.h
 * \brief  
 * \author ST-Ericsson
 */
/*****************************************************************************/
#ifndef _ENS_TLIST_H_
#define _ENS_TLIST_H_


#include "ENS_Redefine_Class.h"

namespace ens {

/**
 * Template list class
 */
template<typename T>
class TList
{
private:
    /** Internal list item class */
    template<typename U>
    class Item_t
    {
    public:
        Item_t(const U& data, Item_t* prev = 0, Item_t* next = 0)
            : m_tData(data), m_pPrev(prev), m_pNext(next)
            {};
        
        ~Item_t() {};
        
        const U&
        data() const { return m_tData; }

        U&
        data() { return m_tData; }
        
        Item_t*
        prev() { return m_pPrev;}
        
        const Item_t*
        prev() const { return m_pPrev; }
        
        Item_t*
        next() { return m_pNext; }
        
        const Item_t*
        next() const { return m_pNext; }
        
        Item_t*
        prev(Item_t* item) {
            m_pPrev = item;
            return m_pPrev;
        }
        
        Item_t* next(Item_t* item) {
            m_pNext = item;
            return m_pNext;
        }

    private:
        U     m_tData;
        Item_t* m_pPrev;
        Item_t* m_pNext;
    };

public:

    // Forward declaration
    template<typename V> class ConstIterator_t;
    template<typename V> class Iterator_t;

    /** Iterator class */
    template<typename V>
    class Iterator_t
    {
        typedef Item_t<V> Item_type;
        friend class ConstIterator_t<V>;
    public:
        Iterator_t() :m_pPos(0) {}
        
        Iterator_t(const Iterator_t &it) :m_pPos(it.m_pPos) {}

        Iterator_t(Item_type* pItem) :m_pPos(pItem) {}
        
        virtual
        ~Iterator_t() {}
        
        Iterator_t&
        operator=(const Iterator_t &it) {
            if (this == &it)
                return *this;
            m_pPos = it.pos();
            return *this;
        }
        
        Iterator_t&
        operator++() {
            if (this->pos())
                m_pPos = this->pos()->next();
            return *this;
        }
        
        Iterator_t
        operator++(int) {
            Iterator_t copy(*this);
            ++(*this);
            return copy;
        }

        bool
        operator==(const Iterator_t &it) const {
            return (this->pos() == it.pos());
        }

        bool
        operator!=(const Iterator_t &it) const {
            return !(*this == it);
        }
        
        V&
        operator*() {
            return this->pos()->data();
        }

        V*
        operator->() {
            return (&*(typename TList<V>::Iterator_t)*this);
        }

    protected:
        Item_type*
        pos() const {
            return m_pPos;
        }

    private:
        Item_type* m_pPos;
    };

    /** Const Iterator class */
    template<typename S>
    class ConstIterator_t
    {
        typedef Item_t<S> Item_type;
        friend class Iterator_t<S>;
    public:
        ConstIterator_t() :m_pPos(0) {}
        
        ConstIterator_t(const ConstIterator_t &it) :m_pPos(it.pos()) {}

        ConstIterator_t(const Iterator_t<S> &it) :m_pPos(it.pos()) {}

        ConstIterator_t(const Item_type* pItem) :m_pPos(pItem) {}
        
        virtual
        ~ConstIterator_t() {}
        
        ConstIterator_t&
        operator=(const ConstIterator_t &it) {
            if (this == &it)
                return *this;
            m_pPos = it.pos();
            return *this;
        }

        ConstIterator_t&
        operator=(const Iterator_t<S> &it) {
            if (this == &it)
                return *this;
            m_pPos = it.pos();
            return *this;
        }

        ConstIterator_t&
        operator++() {
            if (this->pos())
                m_pPos = this->pos()->next();
            return *this;
        }
        
        ConstIterator_t
        operator++(int) {
            ConstIterator_t copy(*this);
            ++(*this);
            return copy;
        }

        bool
        operator==(const ConstIterator_t &it) const {
            return (this->pos() == it.pos());
        }
        
        bool
        operator!=(const ConstIterator_t &it) const {
            return !(*this == it);
        }
        
        const S&
        operator*() const {
            return this->pos()->data();
        }

        const S*
        operator->() const {
            return (&*(typename TList<S>::ConstIterator_t)*this);
        }

    protected:
        const Item_type*
        pos() const {
            return m_pPos;
        }

    private:
        const Item_type* m_pPos;
    };


public:
    typedef ConstIterator_t<T> const_iterator;
    typedef Iterator_t<T>      iterator;

    TList() : m_pFirst(0), m_pLast(0) {};
    
    ~TList() { this->clear(); }
    
    /** Remove all element from the list */
    void
    clear() {
        Item_t<T>* it = this->first();
        Item_t<T>* next = 0;
        while(it) {
            next = it->next();
            delete it;
            it = next;         
        }
        m_pFirst = 0;
        m_pLast = 0;
    }
    
    /** Return size of the list */
    unsigned int
    size() const {
        Item_t<T>* it = this->first();
        unsigned int nSz = 0;
        while(it) {
            it = it->next();
            nSz++;         
        }
        return nSz;
    }
    
    /** Check if list is empty */
    bool
    empty() const { return (this->first() == 0); }
    
    /** Insert element at beginning */
    bool
    push_front(const T& pData) {
        Item_t<T>* item = new Item_t<T>(pData, 0, m_pFirst);  
        if (!item) {
            return false;
        }
        if (m_pFirst) {
            m_pFirst = m_pFirst->prev(item);
        } else {
            m_pFirst = item;
            m_pLast = item;
        }
        return true;
    }
    
    /** Insert element at the end */
    bool
    push_back(const T& pData) {
        Item_t<T>* item = new Item_t<T>(pData, m_pLast, 0);  
        if (!item) {
            return false;
        }
        if (m_pLast) {
            m_pLast = m_pLast->next(item);
        } else {
            m_pLast = item;
            m_pFirst = item;
        }
        return true;
    }
    
    /** Delete first element */
    void
    pop_front() {
        if (!m_pFirst) {
            return;
        }
        
        Item_t<T>* tmp = m_pFirst;
        m_pFirst = tmp->next();
        if (m_pFirst) {
            m_pFirst->prev(0);
        } else { 
            m_pLast = 0;
        }
        
        delete tmp;
    }
    
    /** Delete last element */
    void
    pop_back() {
        if (!m_pLast) {
            return;
        }
        Item_t<T>* tmp = m_pLast;
        m_pLast = tmp->prev();
        
        if (m_pLast) {
            m_pLast->next(0);
        } else {
            m_pFirst = 0;
        }
        delete tmp;
    }

// TODO: how to assert if first is null
    T&
    front() {
        Item_t<T>* first = this->first();
        return first->data();
    }

// TODO: how to assert if first is null
    const T&
    front() const {
        Item_t<T>* first = this->first();
        return first->data();
    }

// TODO: how to assert if first is null
    T&
    back() {
        Item_t<T>* last = this->last();
        return last->data();
    }

// TODO: how to assert if first is null
    const T&
    back() const {
        Item_t<T>* last = this->last();
        return last->data();
    }

    Iterator_t<T>
    begin() {
        return Iterator_t<T>(this->first());
    }
    
    Iterator_t<T>
    end() {
        return Iterator_t<T>(this->last());
    }

    
    ConstIterator_t<T>      
    begin() const {
        return ConstIterator_t<T>(this->first());
    }
    
    ConstIterator_t<T>
    end() const {
        return ConstIterator_t<T>(this->last());
    }

protected:
    Item_t<T>*
    first() const { return m_pFirst; }
    
    Item_t<T>*
    last() const { return m_pLast; }

private:
    // Dummy copy-constructor and assignment operator
    // Not implemented to forbid their use
    TList(const TList &);
    TList & operator= (const TList &);
    
private:
    Item_t<T>* m_pFirst;
    Item_t<T>* m_pLast;
};


} // namespace ens

/// @}

#endif // _ENS_TLIST_H_
