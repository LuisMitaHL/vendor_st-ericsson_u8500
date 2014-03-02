/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
 
#ifndef MIGRATION_OS_H
#define MIGRATION_OS_H

#include <cm/inc/cm_type.h>
#include <cm/engine/memory/inc/domain_type.h>

PUBLIC t_cm_error CM_OS_Migrate(const t_cm_domain_id srcShared, const t_cm_domain_id src, const t_cm_domain_id dst);

PUBLIC t_cm_error CM_OS_Unmigrate(void);

#endif /* MIGRATION_OS_H */
