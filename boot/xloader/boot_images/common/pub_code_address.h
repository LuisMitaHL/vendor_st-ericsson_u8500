/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
/**
 *
 * \file pub_code_addr.h
 * \author STMicroelectronics
 *
 * \addtogroup SCENARIO
 *
 * This header file provides definitions of the public-side addresses where
 * public code is copied. This file must contain only definition as it
 * is included in xloader scatter file.
 *
 * @{
 */
/*---------------------------------------------------------------------------*/
#ifndef _PUB_CODE_ADDRESS_H
#define _PUB_CODE_ADDRESS_H

#ifndef TOC_COPY_ADDRESS
#define TOC_COPY_ADDRESS          0x40022000
#endif

#ifndef XLOADER_COPY_ADDRESS
#define XLOADER_COPY_ADDRESS      (TOC_COPY_ADDRESS+0x07A8)
#endif

#ifndef SUBTOC_COPY_ADDRESS
#define SUBTOC_COPY_ADDRESS       0x400647A8
#endif

#endif /* _PUB_CODE_ADDRESS_H*/
/* end of pub_code_addr.h */
/** @} */
