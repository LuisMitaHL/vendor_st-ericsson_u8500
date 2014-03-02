/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef _PROXY_HPP_
#define _PROXY_HPP_

#include <ee/api/ee.h>
#ifndef WORKSTATION
#include <tu_ost_test/network.hpp>
#endif
#include <tu_ost_test/network2.hpp>
#define LOOP_MAX 20

// Define size of the communication fifo
#define FIFO_COMS_SIZE  	2 // Don't change, hardcoded in .conf
#define CB_FIFO_SIZE    	8
#define CONFIGURE_ARM_ITF	3

#ifndef __SYMBIAN32__
#define IMPORT_C
#define EXPORT_C
#endif


class CProxy;

class CTrace
{
	public:	
	CTrace(t_uint32 handle, t_uint16 trace) {aParentHandle=handle;aTraceEnable=trace;}
    t_uint16 getTraceEnable() {return aTraceEnable;}
    t_uint32 getParentHandle() {return aParentHandle;}

	t_uint32 aParentHandle;
	t_uint16 aTraceEnable;
};


class CProxycb: public tu_ost_test_api_uniopDescriptor, public CTrace
{
	virtual void oper(t_uint32 res);
	t_uint32 getId1() {return (t_uint32) this;}


public:
    CProxycb(void *ProxypHandle, t_uint32 ParentHandle, t_uint16 trace) : CTrace(ParentHandle, trace){pProxyHandle = (CProxy *)ProxypHandle; };
	CProxy *pProxyHandle;
};

class CProxy: public CTrace
{
public:

//    CProxy(t_uint32 handle, t_uint16 trace) : CTrace(handle, trace){running = FALSE;};		
	IMPORT_C CProxy(t_uint32 handle, t_uint16 trace);		
    IMPORT_C t_nmf_error Construct();
    IMPORT_C t_nmf_error UnConstruct();
    IMPORT_C t_nmf_error Execute();
    IMPORT_C t_nmf_error Configure(t_uint16 aTraceFilter);
	t_uint32 getId1() {return (t_uint32) this;}

	CProxycb *cb;

	t_nmf_channel cbChannel;
#ifndef WORKSTATION			
	tu_ost_test_network *ex;
	Itu_ost_test_api_configure configure_dsp;	
	Itu_ost_test_api_add input;
#else
	Itu_ost_test_api_uniop input;
#endif
	tu_ost_test_network2 *ex2;
	Itu_ost_test_api_configure configure_arm[CONFIGURE_ARM_ITF];	
	volatile t_uint32 a_table[LOOP_MAX];
	volatile t_uint32 b_table[LOOP_MAX];
	volatile t_uint16 reac_number;
	volatile t_bool running;


private:
	void PullMessage(t_nmf_channel cbChannel);

};


#endif /* _PROXY_HPP_ */
