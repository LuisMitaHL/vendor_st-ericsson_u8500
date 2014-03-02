/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef _ISPCTL_COMMUNICATION_MANAGER_H_
#define _ISPCTL_COMMUNICATION_MANAGER_H_

#include "nmf_eventhandler.h"
#include "host/ispctl/api/cmd.hpp"
#include "client.h"
#include "OMX_Core.h"
#include "TraceObject.h"
#include "sensor.h"

/* TO DO : remove NB_MAX */
#define NB_MAX ISPCTL_SIZE_TAB_PE

#ifdef __SYMBIAN32__
NONSHARABLE_CLASS(CIspctlCom);
#endif
class CIspctlCom
{
    public :
        /* constructor */
        CIspctlCom(t_sia_client_id client_id, TraceObject *traceobj):queueIndex(0),clientId(client_id),mPanic(OMX_FALSE),mTraceObject(traceobj)
        {
            for (int i=0; i<NB_MAX ;i++) {
                queue[i].pe_addr = 0;
                queue[i].pe_data = 0;
            }
        }

		void setPanic(OMX_BOOL panic){mPanic=panic;}

		void setIspctlCmd(Iispctl_api_cmd val);

		void requestPE(t_uint16 addr);
		void writePE(t_uint16 addr, t_uint32 data);

		void queuePE(t_uint16 addr, t_uint32 data) ;
		void queuePE(t_uint16 addr, float data);
		void queuePE(t_uint16 addr, int data) ;
		void queuePE(t_uint16 addr, OMX_S32 data) ;
		void queuePE(t_uint16 addr, double data) ;

		void processQueue(void);
		void readQueue(void);

		void allowSleep(void);
		void preventSleep(void);

		t_uint16 checkPEValue(t_uint16 addr, t_uint32 pe_value);
		Iispctl_api_cmd getIspctlCmd();

		t_sia_client_id getClientId(void) {return clientId;};
		void setClientId(t_sia_client_id client_id) {clientId = client_id;};
		static float u32tofloat(t_uint32 value);
		static t_uint32 floattou32(float value);
		void setIspctlSensor(CSensor * iIspctlSensor); 

	private :
		Iispctl_api_cmd mIspctlCommand;
		ts_PageElement queue[NB_MAX];
		t_uint16 queueIndex;
		void writeListPE(void);
		void readListPE(void);
		void waitExpectedPeValue(t_uint16 addr, t_uint32 value_expected, t_uint32 * value, t_uint32 timeout);
		t_sia_client_id clientId;
		OMX_BOOL mPanic;
    public:
	    TraceObject* mTraceObject;
	    static CSensor* pIspctlSensor; 
};
#endif /*_ISPCTL_COM_H_*/
