/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
 
#ifndef COMMON_MIGRATION_PROXY_H
#define COMMON_MIGRATION_PROXY_H

#include <cm/inc/cm_type.h>
#include <cm/engine/memory/inc/domain_type.h>

PUBLIC IMPORT_SHARED t_cm_error CM_Migrate(const t_cm_domain_id srcShared, const t_cm_domain_id src, const t_cm_domain_id dst);

PUBLIC IMPORT_SHARED t_cm_error CM_Unmigrate(void);

#endif /* COMMON_MIGRATION_PROXY_H */
