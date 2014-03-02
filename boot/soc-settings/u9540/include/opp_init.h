/*===========================================================================
| ST-Ericsson
| ===========================================================================
|
|  $RCSfile: /sources/opp_init.h$
|  $Revision: 1.4$
|  $Date: Fri Sep 14 08:01:58 2012 GMT$
|
|  $Source: /sources/opp_init.h$
|
|  Copyright Statement:
|  -------------------
| The confidential and proprietary information contained in this file may
| only be used by a person authorized under and to the extent permitted
| by a subsisting licensing agreement from ST-Ericsson S.A.
|
| Copyright (C) ST-Ericsson S.A. 2011. All rights reserved.
|
| This entire notice must be reproduced on all copies of this file
| and copies of this file may only be made by a person if such person is
| permitted to do so under the terms of a subsisting license agreement
| from ST-Ericsson S.A..
|
|
| Project :  AP9540
| -------
|
| Description:
| ------------
|
|
|===========================================================================
|                    Revision History
|===========================================================================
|
|  $Log: /sources/opp_init.h$
|  
|   Revision: 1.4 Fri Sep 14 08:01:58 2012 GMT nxp11764
|   Config pack Soc settings v3.0 - AVS V1 implementation
|  
|   Revision: 1.3 Fri Mar 30 12:32:28 2012 GMT frq06447
|   Update declaration of OPP_GetOppConfig
|  
|   Revision: 1.2 Mon Nov 28 13:31:24 2011 GMT frq02595
|   Updat include link of the PrcmuApi.h
|  
|   Revision: 1.1 Wed Nov 09 10:54:13 2011 GMT frq02595
|   
|  $Aliases: $
|
|
|  $KeysEnd $
|
|
|===========================================================================*/

#ifndef __OPP_INIT_H__
#define __OPP_INIT_H__

#include <types.h>

#include <prcmuApi.h>


extern Opp_status OPP_GetOppConfig(u32 avs_fuses[6], const u16 *Opp_Default, sPrcmuApi_InitOppData_t *stOpp_data);



#endif /*__OPP_INIT_H__*/
