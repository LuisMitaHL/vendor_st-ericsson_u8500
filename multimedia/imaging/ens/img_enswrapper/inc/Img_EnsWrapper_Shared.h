/*
* Copyright (C) ST-Ericsson SA 2010. All rights reserved.
* This code is ST-Ericsson proprietary and confidential.
* Any use of the code for whatever purpose is subject to
* specific written permission of ST-Ericsson SA.
*/
#ifndef _IMG_ENSWRAPPER_SHARED_H_
#define _IMG_ENSWRAPPER_SHARED_H_

#ifdef _MSC_VER
	#define __func__ __FUNCTION__
	#ifndef _CRT_SECURE_NO_WARNINGS
		#define _CRT_SECURE_NO_WARNINGS 1
	#endif
	#pragma warning (disable: 4251) //4251:  : class 'xxx' needs to have dll-interface to be used by clients
	#pragma warning (disable: 4512) //C4512:  : assignment operator could not be generated
#endif

#ifndef IMG_ENSWRAPPER_API
	#ifdef _MSC_VER
		#define IMG_ENSWRAPPER_API __declspec(dllimport)
	#else
		#define IMG_ENSWRAPPER_API 
	#endif
#endif

#define IMG_DBC_ASSERT(a)


#endif /* _IMG_ENSWRAPPER_SHARED_H_ */
