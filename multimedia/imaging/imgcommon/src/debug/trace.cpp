/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
#include "trace.h"



/*
 * Constructor for the Debug Class
 * It intialises some internal variables
 * */

CTrace::CTrace()
{
    bCallbackNeeded = OMX_FALSE;
    iListOfPeToRead.nSize = sizeof(IFM_CONFIG_READPETYPE);
    iListOfPeToWrite.nSize = sizeof(IFM_CONFIG_WRITEPETYPE);

    iPEMode = READ_LIST_OF_PE;
    for (int i=0;i<IFM_SIZE_LIST_PE_READ;i++) {
        iListOfPeToRead.nPEList_addr[i] = 0;
        iListOfPeToRead.nPEList_data[i] = 0;
    }
    for (int i=0;i<IFM_SIZE_LIST_PE_WRITE;i++) {
        iListOfPeToWrite.nPEList_addr[i] = 0;
        iListOfPeToWrite.nPEList_data[i] = 0;
    }
    iListOfPeToRead.nPEToRead = 0;
    iListOfPeToRead.nPortIndex = 0;
    iListOfPeToWrite.nPEToWrite = 0;
    iListOfPeToWrite.nPortIndex = 0;
}


/*
 * Reset the list of PE to be read. This list is filled under setConfig request from the user
 */
void CTrace::resetReadListPe() {
	t_uint16 i;
	for(i=0; i<MAX_NUMBER_OF_PE_IN_A_LIST; i++)
	{
		iListOfPeToRead.nPEList_addr[i] = 0;
	}
	iListOfPeToRead.nPEToRead = 0;
}

/*
 * Reset the list of PE to be written. This list is uner setConfig request from the user
 */
void CTrace::resetWriteListPe() {
	t_uint16 i;
	for(i=0; i<MAX_NUMBER_OF_PE_IN_A_LIST; i++)
	{
		iListOfPeToWrite.nPEList_addr[i]=0;
		iListOfPeToWrite.nPEList_data[i]=0;
	}
	iListOfPeToWrite.nPEToWrite = 0;
}



