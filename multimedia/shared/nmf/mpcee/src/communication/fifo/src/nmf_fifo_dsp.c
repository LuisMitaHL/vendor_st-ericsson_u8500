/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
 
/* communication/fifo/src/nmf_fifo_dsp.c - EE.
 *
 * Copyright (c) 2006, 2007, 2008 STMicroelectronics.
 *
 * Reproduction and Communication of this document is strictly prohibited
 * unless specifically authorized in writing by STMicroelectronics.
 *
 * Written by NMF team.
 */
#include <communication.nmf>

#include <share/communication/inc/nmf_fifo_desc.h>
#include <share/inc/macros.h>
#include <communication/fifo/inc/nmf_fifo_dsp.h>
#include <inc/archi-wrapper.h>
#include <inc/mmdsp_mapping.h>
#include <semaphores/inc/semaphores.h>
#include <archi.h>

static t_uint16 fifo_getCount(
    t_uint16 writeIndex,
    t_uint16 readIndex,
    t_uint16 fifoSize
)
{
    if (writeIndex >= readIndex) {return writeIndex - readIndex;}
    else {return fifoSize - readIndex + writeIndex;}
}

static t_uint16 fifo_incrementIndex(
    t_uint16 index,
    t_uint16 wrappingValue
)
{
    if (++index == wrappingValue) {index = 0;}

    return index;
}

PUBLIC t_shared_addr fifo_getAndAckNextElemToWritePointer(t_nmf_fifo fifoId)
{
    t_shared_addr retValue;

    retValue = fifo_getNextElemToWritePointer(fifoId);
    if (retValue != 0)
    {
        fifo_acknowledgeWrite(fifoId);
    }

    return retValue;
}

PUBLIC t_shared_addr fifo_getAndAckNextElemToReadPointer(t_nmf_fifo fifoId)
{
    t_shared_addr retValue;

    retValue = fifo_getNextElemToReadPointer(fifoId);
    if (retValue != 0)
    {
        fifo_acknowledgeRead(fifoId);
    }

    return retValue;
}

#pragma force_dcumode
PUBLIC t_shared_addr fifo_getNextElemToWritePointer(t_nmf_fifo fifoId)
{
    __SHARED16 t_nmf_fifo_desc *pFifoDesc = (__SHARED16 t_nmf_fifo_desc *)fifoId;
    volatile t_shared_addr retValue = 0;
    t_uint16 count = fifo_getCount(pFifoDesc->writeIndex, pFifoDesc->readIndex,pFifoDesc->wrappingValue);

    if (count < pFifoDesc->fifoFullValue)
    {
        retValue = ((t_shared_addr)fifoId + sizeof(t_nmf_fifo_desc) + (pFifoDesc->writeIndex * pFifoDesc->elemSize));
    }

    return retValue;
}

#pragma force_dcumode
PUBLIC t_shared_addr fifo_getNextElemToReadPointer(t_nmf_fifo fifoId)
{
    __SHARED16 t_nmf_fifo_desc *pFifoDesc = (__SHARED16 t_nmf_fifo_desc *)fifoId;
    volatile t_shared_addr retValue = 0;
    t_uint16 count = fifo_getCount(pFifoDesc->writeIndex, pFifoDesc->readIndex,pFifoDesc->wrappingValue);

    if (count != 0)
    {
        retValue = ((t_shared_addr)fifoId + sizeof(t_nmf_fifo_desc) + (pFifoDesc->readIndex * pFifoDesc->elemSize));
    }

    return retValue;
}

#pragma force_dcumode
PUBLIC t_uint16 fifo_acknowledgeRead(t_nmf_fifo fifoId)
{
    __SHARED16 t_nmf_fifo_desc *pFifoDesc = (__SHARED16 t_nmf_fifo_desc *)fifoId;
    volatile t_uint16 retValue;

    pFifoDesc->readIndex = fifo_incrementIndex(pFifoDesc->readIndex, pFifoDesc->wrappingValue);
    retValue = fifo_getCount(pFifoDesc->writeIndex, pFifoDesc->readIndex,pFifoDesc->wrappingValue);

    return retValue;
}

#pragma force_dcumode
PUBLIC t_uint16 fifo_acknowledgeWrite(t_nmf_fifo fifoId)
{
    __SHARED16 t_nmf_fifo_desc *pFifoDesc = (__SHARED16 t_nmf_fifo_desc *)fifoId;
    volatile t_uint16 retValue;

    pFifoDesc->writeIndex = fifo_incrementIndex(pFifoDesc->writeIndex, pFifoDesc->wrappingValue);
    retValue = fifo_getCount(pFifoDesc->writeIndex, pFifoDesc->readIndex,pFifoDesc->wrappingValue);

    return retValue;
}

#pragma force_dcumode
PUBLIC void fifo_coms_acknowledgeWriteAndInterruptGeneration(t_nmf_fifo fifoId)
{
    __SHARED16 t_nmf_fifo_desc *pFifoDesc = (__SHARED16 t_nmf_fifo_desc *)fifoId;

    pFifoDesc->writeIndex = fifo_incrementIndex(pFifoDesc->writeIndex, pFifoDesc->wrappingValue);
    MMDSP_FLUSH_DATABUFFER();
    sem_GenerateIrq(pFifoDesc->semId);
}

/* End of nmf_fifo.c file */


