/*****************************************************************************/
/**
*  ST-Ericsson, 2011 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \file
* \brief   9P interfaces for DTH server helper
* \author  ST-Ericsson
*/
/*****************************************************************************/

#ifndef DTH_9PSERVER_ERROR_H_
#define DTH_9PSERVER_ERROR_H_

#include <errno.h>
#include <dthsrvhelper/taterror.h>

struct st_error_info {
    int code;
    const char *name;
    const char *info;
};

void get_error_info(struct st_error_info* pp_err);

#define GET_TAT_ERROR_STRING(vp_code)       \
    ( (((vp_code) >= TAT_ERROR_MIN) && ((vp_code) < TAT_ERROR_MAX)) ? TAT_ERROR_STRINGS[vp_code - TAT_ERROR_MIN] : "<unregistered TAT error>" )

#endif /* DTH_9PSERVER_ERROR_H_ */
