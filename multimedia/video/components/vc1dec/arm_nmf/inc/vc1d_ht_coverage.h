/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */


/*------------------------------------------------------------------------
 * Includes                                                            
 *----------------------------------------------------------------------*/

#include "vc1d_common.h"

/*------------------------------------------------------------------------
 * Defines                                                            
 *----------------------------------------------------------------------*/
#ifndef _VC1D_HT_COVERAGE_H_
#define _VC1D_HT_COVERAGE_H_


#ifdef COVERAGE

#define COV_INIT(cov_file_name,create) cov_init(cov_file_name,create)
#define COV_LOG_SEQUENCE(p_seq_param) cov_log_sequence_param((p_seq_param))
#define COV_LOG_FILE(file)  cov_log_bitstream(file)
#define COV_CLOSE() cov_close()

void cov_init(char *cov_file_name, t_bool create);
void cov_log_bitstream(char *bitstream_file_name);
void cov_log_sequence_param(tps_sequence_parameters p_seq_param);


#else
#define COV_INIT(cov_file_name,create) 
#define COV_LOG_SEQUENCE(p_seq_param) 
#define COV_LOG_FILE(file)  
#define COV_CLOSE() 

#endif


#endif /* _VC1D_HT_COVERAGE_H_ */
