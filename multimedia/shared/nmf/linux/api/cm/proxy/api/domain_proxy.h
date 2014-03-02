/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
 
/***************************************************************************/
/* file    : domain.h
 * author  : NMF team
 * version : 1.0
 *
 * brief : NMF domain definitions
 */
/***************************************************************************/
/*!
 * \defgroup CM_DOMAIN_API CM Domain API
 * \ingroup CM_USER_API
 */
#ifndef DOMAIN_COMMON_H_
#define DOMAIN_COMMON_H_

#include <cm/engine/memory/inc/domain_type.h>

/*!
 * \brief Create a domain.
 *
 * Create a memory domain for use in the CM for component instantiation and memory allocation.
 *
 * \param[in]  domain Description of domain memories.
 * \param[out] handle Idetifier of the created domain
 *
 * \exception CM_INVALID_DOMAIN_DEFINITION
 * \exception CM_INTERNAL_DOMAIN_OVERFLOW
 * \exception CM_OK
 *
 * \return Error code.
 *
 * \ingroup CM_DOMAIN_API
 */
PUBLIC IMPORT_SHARED t_cm_error CM_CreateMemoryDomain(const t_cm_domain_memory *domain, t_cm_domain_id *handle);

/*!
 * \brief Same as CM_CreateMemoryDomain() but on behalf of another process
 *
 * \param See CM_CreateMemoryDomain()
 * \param[in] client The process id of the client process
 *
 * \exception See CM_CreateMemoryDomain()
 *
 * \return Error code.
 *
 * \ingroup CM_DOMAIN_API
 */
PUBLIC IMPORT_SHARED t_cm_error CM_CreateMemoryDomainForClient(
        const t_cm_domain_memory *domain,
        t_nmf_client_id client,
        t_cm_domain_id *handle
        );

/*!
 * \brief Create a scratch domain.
 *
 * Create a scratch memory domain. Scratch domains are used to perform overlapping allocations.
 *
 * \param[in]  parentId Identifier of the (logical) parent domain
 * \param[in]  domain Description of domain memories.
 * \param[out] handle Idetifier of the created domain
 *
 * \exception CM_INVALID_DOMAIN_DEFINITION
 * \exception CM_INTERNAL_DOMAIN_OVERFLOW
 * \exception CM_NO_MORE_MEMORY
 * \exception CM_OK
 *
 * \return Error code.
 *
 * \ingroup CM_DOMAIN_API
 */
PUBLIC IMPORT_SHARED t_cm_error CM_CreateMemoryDomainScratch(const t_cm_domain_id parentId, const t_cm_domain_memory *domain, t_cm_domain_id *handle);

/*!
 * \brief Destroy a memory domain.

 * \param[in] handle Domain identifier to destroy.
 *
 * \exception CM_INVALID_DOMAIN_HANDLE
 * \exception CM_OK
 *
 * \return Error code.
 *
 * \ingroup CM_DOMAIN_API
 */
PUBLIC IMPORT_SHARED t_cm_error CM_DestroyMemoryDomain(t_cm_domain_id handle);

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
 * \ingroup CM_DOMAIN_API
 */
PUBLIC IMPORT_SHARED t_cm_error CM_GetDomainCoreId(const t_cm_domain_id domainId, t_nmf_core_id *coreId);

#endif /* DOMAIN_COMMON_H_ */
