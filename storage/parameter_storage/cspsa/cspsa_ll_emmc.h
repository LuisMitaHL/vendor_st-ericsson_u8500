/* **************************************************************************
 *
 * cspsa_ll_emmc.h
 *
 * Copyright (C) 2010 ST-Ericsson SA
 *
 * This software is released either under the terms of the BSD-style
 * license accompanying CSPSA or a proprietary license obtained from
 * ST-Ericsson SA.
 *
 * Author: 2010, Louis Verhaard <louis.xl.verhaard@stericsson.com>
 *
 * DESCRIPTION:
 *
 * EMMC plugin for CSPSA, works in loader environment.
 *
 **************************************************************************** */

#ifndef INCLUSION_GUARD_CSPSA_LL_EMMC_H
#define INCLUSION_GUARD_CSPSA_LL_EMMC_H

// =============================================================================
//  Include Header Files
// =============================================================================

#include "cspsa.h"

/** Registers the EMMC plugin to CSPSA. */
CSPSA_Result_t CSPSA_LL_EMMC_Init(void);

/**
 * Finds the CSPSA TOC entries, and if it finds it, registers them to the CSPSA under name
 * <DeviceName_p>/TOC/<TOC id>, e.g. "/flash0/TOC/CSPSA0".
 *
 * @param [in] DeviceName_p First part of the CSPSA name that will be registered to CSPSA.
 *
 * @return The result of the operation.
 * @retval T_CSPSA_RESULT_OK If a trim area was found.
 */
CSPSA_Result_t RegisterEMMC_Areas(const char* DeviceName_p);

#endif // INCLUSION_GUARD_CSPSA_LL_EMMC_H
