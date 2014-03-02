/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
 
/*!
 * \brief Public Component Manager Domain OS API.
 *
 * This file contains the Component Manager OS API for manipulating domains.
 */
#ifndef CM_DOMAIN_OS_H_
#define CM_DOMAIN_OS_H_

#include <cm/engine/memory/inc/domain_type.h>

/*!
 * \brief Create a memory domain.
 *
 * Creates a domain in the CM, valid for component instantiation or memory allocation.
 *
 * \param[in]  domain Domain description
 * \param[in]  client ClientId of the process to which the domain will be attached, NMF_CURRENT_CLIENT is a special value
 * \param[out] handle Domain handle
 *
 * \exception CM_INVALID_DOMAIN_DEFINITION
 * \exception CM_INTERNAL_DOMAIN_OVERFLOW
 * \exception CM_OK
 *
 * \return Error code.
 *
 * \ingroup CM_OS_API
 */
PUBLIC t_cm_error CM_OS_CreateMemoryDomain(
        const t_cm_domain_memory  *domain,
        t_nmf_client_id            client,
        t_cm_domain_id            *handle);

/*!
 * \brief Create a scratch domain.
 *
 * Create a scratch memory domain. Scratch domains
 * are used to perform overlapping allocations.
 *
 * \param[in]  parentId Identifier of the parent domain.
 * \param[in]  domain Description of domain memories.
 * \param[out] handle Idetifier of the created domain
 *
 * \exception CM_INVALID_DOMAIN_DEFINITION
 * \exception CM_INTERNAL_DOMAIN_OVERFLOW
 * \exception CM_NO_MORE_MEMORY
 * \exception CM_OK
 *
 * \return Error code.
 * \ingroup CM_OS_API
 */
PUBLIC t_cm_error CM_OS_CreateMemoryDomainScratch(
        const t_cm_domain_id parentId,
        const t_cm_domain_memory  *domain,
        t_cm_domain_id            *handle
        );

/*!
 * \brief Destroy a memory domain.
 *
 * \param[in]  domain Domain identifier to destroy.
 *
 * \ingroup CM_OS_API
 */
PUBLIC t_cm_error CM_OS_DestroyMemoryDomain(const t_cm_domain_id domain);

/*!
 * \brief Retrieve the coreId for a given domain. Utility.

 * \param[in]  domainId   Domain identifier.
 * \param[out] coreId     Core identifier.
 *
 * \exception CM_INVALID_DOMAIN_HANDLE  Invalid domain handle
 * \exception CM_OK
 *
 * \return Error code.
 *
 * \ingroup CM_OS_API
 */
PUBLIC t_cm_error CM_OS_GetDomainCoreId(const t_cm_domain_id domainId, t_nmf_core_id *coreId);
#endif
