/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef DEC_ARM_HPP_
#define DEC_ARM_HPP_

class tu_ost_test_dec_arm: public tu_ost_test_dec_armTemplate 
{
public:
    virtual t_nmf_error construct();
    virtual void oper(t_uint32 d, t_uint8 index);
    virtual void ConfigureTrace(t_uint16 aTraceFilter);
    virtual void ConfigureParentHandle(t_uint32 aHandle);
    virtual void Aoper(t_uint32 d);
    t_uint16 getTraceEnable() {return aTraceEnable;}
    t_uint32 getParentHandle() {return aParentHandle;}
		t_uint32 getId1() {return (t_uint32) this;}

	unsigned int aParentHandle;
	unsigned short aTraceEnable;
	unsigned int aInstance;
};


#endif /* DEC_ONE_HPP_ */
