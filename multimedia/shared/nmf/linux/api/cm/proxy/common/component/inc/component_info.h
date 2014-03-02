/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
 
/*!
 * \brief Component Manager Internal API for stubs.
 */
#ifndef COMMON_COMPONENT_INFO_H
#define COMMON_COMPONENT_INFO_H

/*!
 * \brief Component Memory Types.
 * \ingroup COMPONENT
 */
typedef t_uint32 t_cm_mpc_info_memory_type;
#define CM_MM_ESRAM_TYPE ((t_cm_mpc_info_memory_type)0)
#define CM_MM_SDRAM_TYPE ((t_cm_mpc_info_memory_type)1)
#define CM_MM_XRAM_TYPE  ((t_cm_mpc_info_memory_type)2)
#define CM_MM_YRAM_TYPE  ((t_cm_mpc_info_memory_type)3)
#define CM_MM_NB_MEMTYPES 4

/*!
 * \brief Purpose of the memory section - code or data.
 * \ingroup COMPONENT
 */
typedef t_uint8 t_cm_mpc_info_memory_purpose;
#define CM_MM_CODE_TYPE ((t_cm_mpc_info_memory_purpose)0)
#define CM_MM_DATA_TYPE ((t_cm_mpc_info_memory_purpose)1)
#define CM_MM_NB_PURPOSES 2

/*!
 * \brief Definition of structure returned by the component memory info functions.
 * \ingroup COMPONENT
 */
typedef struct {
    t_uint32 firstInstantiate[CM_MM_NB_PURPOSES][CM_MM_NB_MEMTYPES];      //!< First instantiation sizes
    t_uint32 subsequentInstantiate[CM_MM_NB_PURPOSES][CM_MM_NB_MEMTYPES]; //!< Following instantiation sizes
} t_cm_component_memory_info;

extern t_cm_error cm_getComponentMemoryInfo(
        const char* templateName,
        t_cm_component_memory_info *pInfo);

#endif /* COMMON_COMPONENT_INFO_H */
