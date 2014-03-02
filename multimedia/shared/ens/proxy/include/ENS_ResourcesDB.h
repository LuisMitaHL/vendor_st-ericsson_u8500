/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   ENS_ResourcesDB.h
 * \brief  
 * \author ST-Ericsson
 */
/*****************************************************************************/

#ifndef _ENS_RESOURCESDB_H_
#define _ENS_RESOURCESDB_H_

#include "ENS_Redefine_Class.h"

#include <ENS_HwRm.h>
#include "ENS_macros.h"
#include "ENS_IOMX.h"
#include "OMX_Core.h"

/** @defgroup ens_dre_comp_ext ENS Component_DRe RMP class
 * Abstract base class for Resources Management (DRe) Proxy in a Component Proxy
 * @ingroup ens_component_class
 * @{ 
 * */

/// Class implementing the ENS Component DRe interface (RM proxy in Component Proxy)
/// TEMPORARY interface (\b WILL change)
class ENS_ResourcesDB {
    public:
        /// Constructor
        ENS_API_IMPORT ENS_ResourcesDB(OMX_U32 nbOfDomains=1);

        /// Destructor
        ENS_API_IMPORT virtual ~ENS_ResourcesDB() = 0;

        // Methods relative to domains
        ENS_API_IMPORT void setNMFDomainHandle(OMX_IN const RM_SYSCTL_T* pRMcfg);
        ENS_API_IMPORT OMX_U32 getNMFDomainHandle(RM_NMFD_E domainType,OMX_U32 nPortIndex);
        ENS_API_IMPORT void setDefaultNMFDomainType(OMX_IN RM_NMFD_E defaultNMFDomainType,OMX_IN OMX_U32 portNumber=0);
        ENS_API_IMPORT RM_NMFD_E getDefaultNMFDomainType(OMX_IN OMX_U32 portNumber);

        // Default memory preset management
        ENS_API_IMPORT OMX_ERRORTYPE setMemoryPreset(OMX_U32 nMempresetId);
        ENS_API_IMPORT OMX_U32 getMemoryPreset();

        // Nb of memory preset management
        ENS_API_IMPORT void setNbOfMemoryPresets(OMX_U32 nNbOfMemoryPresets);
        ENS_API_IMPORT OMX_U32 getNbOfMemoryPresets();

    private:
        OMX_U32         mNbOfDomains; //!< number of domains : 1 for general case ; nbOfPorts in case of audio mixer
        // Default domain type (used for retrieving the corresponding domain handle)
        RM_NMFD_E*          mDefaultNMFDomainType;
        // Domain handles values to be used when mNMFDomainsHandlesAvailable = true
        // that is to say when the RME has set the component domains
        RM_NMFD_T*          mNMFDomainHandles;
        OMX_BOOL*           mNMFDomainsHandlesAvailable;

        OMX_U32             mMemoryPresetId; // Default memory preset
        OMX_U32             mNbOfMemoryPresets; // Nb of memory preset

};

/* @} */


#endif // _ENS_RESOURCESDB_H_
