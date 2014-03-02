/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   ENS_ResourcesDB.cpp
 * \brief  
 * \author ST-Ericsson
 */
/*****************************************************************************/
#include "ENS_ResourcesDB.h"
#include <string.h>


ENS_API_EXPORT ENS_ResourcesDB::ENS_ResourcesDB(OMX_U32 nbOfDomains)
    : mNbOfDomains(nbOfDomains)
{
    // Init the NMF domains and set the default domain type (used for retrieving the corresponding domain handle)
    mNMFDomainHandles = new RM_NMFD_T[mNbOfDomains];
    mDefaultNMFDomainType = new RM_NMFD_E[mNbOfDomains];
    mNMFDomainsHandlesAvailable = new OMX_BOOL[mNbOfDomains];
    for (unsigned int i=0;i<mNbOfDomains;i++) {
        mNMFDomainsHandlesAvailable[i]= OMX_FALSE;
        mDefaultNMFDomainType[i] = RM_NMFD_PROCSVA;
        mNMFDomainHandles[i].nDidProcSIA = 0; //valid if != 0
        mNMFDomainHandles[i].nDidProcSVA = 0; //valid if != 0
        mNMFDomainHandles[i].nDidHWPipeSIA = 0; //valid if != 0
        mNMFDomainHandles[i].nDidHWPipeSVA = 0; //valid if != 0
        mNMFDomainHandles[i].nDidHWXP70SIA = 0; //valid if != 0
        mNMFDomainHandles[i].nPortIndex = -1; // valid if value is >= 0
    }
    
    // Init mNbOfMemoryPresets 
    mNbOfMemoryPresets = 0;
    
    // Init mMemoryPresetId 
    mMemoryPresetId = 0;
    
}

ENS_API_EXPORT ENS_ResourcesDB::~ENS_ResourcesDB() {
    delete [] mDefaultNMFDomainType;
    delete [] mNMFDomainHandles;
    delete [] mNMFDomainsHandlesAvailable;
}

ENS_API_EXPORT void ENS_ResourcesDB::setDefaultNMFDomainType(OMX_IN RM_NMFD_E defaultNMFDomainType, OMX_IN OMX_U32 portNumber)
{
    mDefaultNMFDomainType[portNumber] = defaultNMFDomainType;
}
 
ENS_API_EXPORT RM_NMFD_E ENS_ResourcesDB::getDefaultNMFDomainType(OMX_IN OMX_U32 portNumber)
{
    return mDefaultNMFDomainType[portNumber];
}

ENS_API_EXPORT void ENS_ResourcesDB::setNMFDomainHandle(OMX_IN const RM_SYSCTL_T* pRMcfg)
{
    unsigned int index;
    if (pRMcfg->ctl.sNMF.nPortIndex < 0) {
        // Same handle value for whole table entries
        for (OMX_U32 i=0 ; i<mNbOfDomains ; i++) {
            mNMFDomainHandles[i].nDidProcSIA   = pRMcfg->ctl.sNMF.nDidProcSIA;
            mNMFDomainHandles[i].nDidProcSVA   = pRMcfg->ctl.sNMF.nDidProcSVA;
            mNMFDomainHandles[i].nDidHWPipeSIA = pRMcfg->ctl.sNMF.nDidHWPipeSIA;
            mNMFDomainHandles[i].nDidHWPipeSVA = pRMcfg->ctl.sNMF.nDidHWPipeSVA;
            mNMFDomainHandles[i].nDidHWXP70SIA   = pRMcfg->ctl.sNMF.nDidHWXP70SIA;
            mNMFDomainHandles[i].nPortIndex   = pRMcfg->ctl.sNMF.nPortIndex;
            mNMFDomainsHandlesAvailable[i] = OMX_TRUE;
        }
    }
    else {
        index=pRMcfg->ctl.sNMF.nPortIndex;
        mNMFDomainHandles[index].nDidProcSIA   = pRMcfg->ctl.sNMF.nDidProcSIA;
        mNMFDomainHandles[index].nDidProcSVA   = pRMcfg->ctl.sNMF.nDidProcSVA;
        mNMFDomainHandles[index].nDidHWPipeSIA = pRMcfg->ctl.sNMF.nDidHWPipeSIA;
        mNMFDomainHandles[index].nDidHWPipeSVA = pRMcfg->ctl.sNMF.nDidHWPipeSVA;
        mNMFDomainHandles[index].nDidHWXP70SIA   = pRMcfg->ctl.sNMF.nDidHWXP70SIA;
        mNMFDomainHandles[index].nPortIndex   = pRMcfg->ctl.sNMF.nPortIndex;
        mNMFDomainsHandlesAvailable[index] = OMX_TRUE;
    }
}


ENS_API_EXPORT OMX_U32 ENS_ResourcesDB::getNMFDomainHandle(RM_NMFD_E domainType,OMX_U32 nPortIndex)
{
    OMX_U32 domain=0;
    
    if (mNMFDomainsHandlesAvailable[nPortIndex] == OMX_TRUE) {
        switch (domainType)
        {
        case rme::RM_NMFD_PROCSIA:
            domain = mNMFDomainHandles[nPortIndex].nDidProcSIA;
            break;
        case rme::RM_NMFD_PROCSVA:
            domain = mNMFDomainHandles[nPortIndex].nDidProcSVA;
            break;
        case rme::RM_NMFD_HWPIPESIA :
            domain = mNMFDomainHandles[nPortIndex].nDidHWPipeSIA;
            break;
        case rme::RM_NMFD_HWPIPESVA :
            domain = mNMFDomainHandles[nPortIndex].nDidHWPipeSVA;
            break;
        case rme::RM_NMFD_HWXP70SIA :
            domain = mNMFDomainHandles[nPortIndex].nDidHWXP70SIA;
            break;
        default:
            domain = mNMFDomainHandles[nPortIndex].nDidProcSIA;
            break;
        }
    }
    else
    {
        domain = ENS_HwRm::HWRM_Get_DefaultDDRDomain(domainType);
    }
    return (domain);

}

ENS_API_EXPORT OMX_ERRORTYPE ENS_ResourcesDB::setMemoryPreset(OMX_U32 nMempresetId) {
    if (nMempresetId >= mNbOfMemoryPresets) {
        return OMX_ErrorBadParameter;
    }
    mMemoryPresetId = nMempresetId;
    return OMX_ErrorNone;
}

ENS_API_EXPORT OMX_U32 ENS_ResourcesDB::getMemoryPreset() {
    return mMemoryPresetId;
}

ENS_API_EXPORT void ENS_ResourcesDB::setNbOfMemoryPresets(OMX_U32 nNbOfMemoryPresets) {
    mNbOfMemoryPresets = nNbOfMemoryPresets;
}

ENS_API_EXPORT OMX_U32 ENS_ResourcesDB::getNbOfMemoryPresets() {
    return mNbOfMemoryPresets;
}

