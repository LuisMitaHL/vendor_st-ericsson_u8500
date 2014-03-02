/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
 
/*!
 * \brief Common Nomadik Multiprocessing Framework limits definition
 *
 * This file contains the limit definitions used into NMF.
 *
 * \warning Don't modify it since it is also hardcoded in tools
 *
 * \defgroup NMF_LIMITS NMF limits definition
 * \ingroup COMMON
 */
#ifndef __INC_NMF_LIMITS_H
#define __INC_NMF_LIMITS_H

/*!
 * \brief Maximum interface name length
 *
 * Define the maximum interface name length allowed by NMF.
 *
 * \ingroup NMF_LIMITS
 */
#define MAX_INTERFACE_NAME_LENGTH               32

/*!
 * \brief Maximum interface method name length
 *
 * Define the maximum interface method name length allowed by NMF.
 *
 * \ingroup NMF_LIMITS
 */
#define MAX_INTERFACE_METHOD_NAME_LENGTH        64

/*!
 * \brief Maximum interface type name length
 *
 * Define the maximum interface type name length allowed by NMF.
 *
 * \ingroup NMF_LIMITS
 */
#define MAX_INTERFACE_TYPE_NAME_LENGTH          128


/*!
 * \brief Maximum template name length
 *
 * Define the maximum template name length allowed by NMF.
 *
 * \ingroup NMF_LIMITS
 */
#define MAX_TEMPLATE_NAME_LENGTH                128

/*!
 * \brief Maximum component local name length
 *
 * Define the maximum component local name length inside a composite component allowed by NMF.
 *
 * \ingroup NMF_LIMITS
 */
#define MAX_COMPONENT_NAME_LENGTH               32

/*!
 * \brief Maximum property name length
 *
 * Define the maximum property name length allowed by NMF.
 *
 * \ingroup NMF_LIMITS
 */
#define MAX_PROPERTY_NAME_LENGTH                32

/*!
 * \brief Maximum property value length
 *
 * Define the maximum property value length allowed by NMF.
 *
 * \ingroup NMF_LIMITS
 */
#define MAX_PROPERTY_VALUE_LENGTH               128

/*!
 * \brief Maximum attribute name length
 *
 * Define the maximum attribute name length allowed by NMF.
 *
 * \ingroup NMF_LIMITS
 */
#define MAX_ATTRIBUTE_NAME_LENGTH               32

/*!
 * \brief Maximum fifo size allowed for binding component
 *
 * Define the maximum fifo size allowed for binding component allowed by NMF when calling
 * CM_BindComponentFromHost and CM_BindComponentAsynchronous.
 *
 * \ingroup NMF_LIMITS
 */
#define MAX_COMMUNICATION_FIFO_SIZE             256

#define MAX_COMPONENT_FILE_PATH_LENGTH          1024

#endif /* __INC_NMF_LIMITS_H */
