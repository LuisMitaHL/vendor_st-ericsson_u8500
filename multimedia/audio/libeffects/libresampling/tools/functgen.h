/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   functgen.h
 * \brief  
 * \author ST-Ericsson
 */
/*****************************************************************************/
#include <iostream>
#define MAXCH 10
using namespace std;
class functgen
{
	private:
		double *Amp,*Theta,*Phase,*Offset,*Dutycycle,*Fc,*Fstop,*Fstart,*Astop,*Astart;
	public:
		functgen(double,double,double,double,double,double,double,double,double,double);
		~functgen();
		double singen(int);
		double pulsegen();
		double sawtoothgen();
		double trianglegen();
		double sweepfgen(int,int);
		double sweepagen(int,int);
		double dcgen();
		double noisegen();
		double ramp(int,int);
		void thetaup(double,int);
};

