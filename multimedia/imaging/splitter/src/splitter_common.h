/*
* Copyright (C) ST-Ericsson SA 2010. All rights reserved.
* This code is ST-Ericsson proprietary and confidential.
* Any use of the code for whatever purpose is subject to
* specific written permission of ST-Ericsson SA.
*/

#ifndef _SPLITTER_COMMON_H_
#define _SPLITTER_COMMON_H_


//#define OMX_SYMBIANDRAFT_INDEX_SPLITTER_CONFIG "sequential"

typedef enum
{
	// Splitter ports indexes
	eSplitter_PortIndex_Input =0,
	eSplitter_PortIndex_Output1,
	eSplitter_PortIndex_Output2,
	eSplitter_PortIndex_Output3,
	eSplitter_PortIndex_Output4,
	// Splitter ports helpers

	eSplitter_Port_OutputLastShared = 2, //Index of the last shared port
	eSplitter_PortNbr = 5,               //Total number of ports

	eSplitter_PortIndex_Output_Last=eSplitter_PortNbr-1,
	eSplitter_Port_Output_Nbr=eSplitter_PortNbr-1
} SplitterPortIndex_t;

enum enumSplitterMode
{
	eSplitter_Sequential = 0,
	eSplitter_Broadcast  = 1, //Default broadcast
	eSplitter_Standard   = eSplitter_Broadcast, //Default broadcast
};

#define SPLITTER_CORE_FIFO_SIZE_MAX 5

#endif
