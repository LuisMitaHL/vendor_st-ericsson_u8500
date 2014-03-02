/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 * \brief  Routing Control, PPP interface
 */

#ifndef STERC_PPP_H
#define STERC_PPP_H

/********************************************************************************
 *
 * Include files
 *
 ********************************************************************************/
#include "sterc_if.h"

/********************************************************************************
 *
 * Defines & Type definitions
 *
 ********************************************************************************/
extern sterc_if_ctrl_block_t sterc_ppp_if;


/*
 * sterc_pscc_handler - handle ppp event messages
 *
 * @msg      incoming message
 * @rsp_msg  response message (NULL if no response)
 *
 * Returns: (1) if handeled, (0) if not handeled
 *
 **/
int sterc_ppp_handler(struct stec_ctrl *ctrl,mpl_msg_t *msg_p, mpl_msg_t **rsp_msg);



#endif /* STERC_PPP_H */
