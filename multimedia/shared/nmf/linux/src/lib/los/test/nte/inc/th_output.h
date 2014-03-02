/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
 
/*! \ingroup CTE_TESTS_TOOLS */

/*!	\file	th_output.h
 *	\brief	tools to handle files for test reporting
 *
 */
#ifndef _TH_OUTPUT_H
#define _TH_OUTPUT_H


#include <cm/inc/cm.h>

#ifdef __cplusplus
    extern "C" {
#endif

/*! \brief debug trace display initialization */
void th_config(t_sint8 flag);

/*! \brief open file if first thread */
void th_fopen(const char*fileName);

/*! \brief write message to file */
void th_fprintf(const char*pStringMessage);

/*! \brief close file if last thread */
void th_fclose(void);

#ifdef __cplusplus
    }
#endif

#endif /* _TH_OUTPUT_H */
