/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include <inc/type.h>

#include "common.h"

#if 0
#ifndef LINUX
char   cfgComponentRepositoryPath[] = HDD_REPOSITORY;
#endif
#endif

t_cm_domain_id mpc2domainId(const char* mpcstring) 
{
    t_cm_domain_id domainId = 0;
	t_cm_domain_memory my_domain;

	my_domain.esramCode.offset = 0;
	my_domain.esramCode.size = 0;
	my_domain.esramData.offset = 0;
	my_domain.esramData.size = 0;

	my_domain.sdramData.offset = 0x40000;
	my_domain.sdramData.size = 0xFFFFFF;

	my_domain.sdramCode.offset = 0x40000;
	my_domain.sdramCode.size = 0xFFFFFF;
	

#ifndef WORKSTATION
	if(mpcstring[0] == 'S' && mpcstring[1] == 'V' && mpcstring[2] == 'A' && mpcstring[3] == 0) 
	{
		my_domain.coreId = SVA_CORE_ID;
        if (CM_CreateMemoryDomain(&my_domain, &domainId) != CM_OK) {
            NMF_LOG("Couldn't Create domain\n");
            NMF_ASSERT(0);
        }
    } 
	else if(mpcstring[0] == 'S' && mpcstring[1] == 'I' && mpcstring[2] == 'A' && mpcstring[3] == 0) 
	{
		my_domain.coreId = SIA_CORE_ID;		
        if (CM_CreateMemoryDomain(&my_domain, &domainId) != CM_OK) {
            NMF_LOG("Couldn't Create domain\n");
            NMF_ASSERT(0);
        }

    }
#endif
    return domainId;
}

