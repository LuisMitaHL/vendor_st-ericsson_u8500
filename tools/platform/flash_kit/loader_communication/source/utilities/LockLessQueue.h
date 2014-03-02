/*******************************************************************************
 * Copyright (C) ST-Ericsson SA 2011
 * License terms: 3-clause BSD license
 ******************************************************************************/

#ifndef _LOCKLESSQUEUE_H_
#define _LOCKLESSQUEUE_H_

/// <summary>
/// Class for store received data in Z-protocol in a lockless queue.
/// </summary>
class LockLessQueue
{
    unsigned char *m_puchQueue;
    unsigned int m_uiHead;
    unsigned int m_uiTail;
    unsigned int m_uiSize;
    unsigned int m_uiMask;

public:
    /// <summary>
    /// Constructor.
    /// </summary>
    /// <param name="uiSize">Size of buffer = 2^uiSize.</param>
    LockLessQueue(unsigned int uiSize): m_uiHead(0), m_uiTail(0) {
        m_uiSize = 1 << uiSize;
        m_uiMask = m_uiSize - 1;
        m_puchQueue = new unsigned char[m_uiSize];
    }

    /// <summary>
    /// Constructor. Default buffer size to 2^20.
    /// </summary>
    LockLessQueue(): m_uiHead(0), m_uiTail(0) {
        m_uiSize = 1 << 20;
        m_uiMask = m_uiSize - 1;
        m_puchQueue = new unsigned char[m_uiSize];
    }

    /// <summary>
    /// Destructor.
    /// </summary>
    ~LockLessQueue() {
        if (m_puchQueue != 0) {
            delete [] m_puchQueue;
        }
    }

    /// <summary>
    /// Store one byte in buffer queue.
    /// </summary>
    /// <param name="uchData">Data to store in buffer.</param>
    /// <param name="pbFull">Status of buffer, true = full buffer.</param>
    void Push(unsigned char uchData, bool *pbFull) {
        unsigned int uiHead =  m_uiHead;
        uiHead = uiHead & m_uiMask; //(Head++) & m_uiMask -> This does the required roll over to zero at the end of the array.
        uiHead++;

        if (uiHead == m_uiTail) {
            *pbFull = true;
        } else {
            m_puchQueue[uiHead] = uchData;
            m_uiHead = uiHead;
            *pbFull = false;
        }
    }


    /// <summary>
    /// Take out one byte from buffer queue.
    /// </summary>
    /// <param name="pbEmpty">Status of buffer. pbEmpty = true -> buffer is empty.</param>
    /// <returns> One byte data from buffer queue.</returns>
    unsigned char Pop(bool *pbEmpty) {
        unsigned char uchPopData = 0;
        unsigned int uiTail = m_uiTail;

        if (m_uiHead != uiTail) {
            uiTail = (uiTail + 1) & m_uiMask;       //This does the required roll over to zero at the end of the array.
            m_uiTail = uiTail;
            uchPopData = m_puchQueue[uiTail];
            *pbEmpty = false;
        } else {
            *pbEmpty = true;
        }

        return uchPopData;
    }

    /// <summary>
    /// Reset buffer queue.
    /// </summary>
    void Clear() {
        m_uiTail = m_uiHead;
    }
};

#endif // _LOCKLESSQUEUE_H_
