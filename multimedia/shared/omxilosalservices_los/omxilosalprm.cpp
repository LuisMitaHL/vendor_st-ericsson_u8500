/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include "omxilosal_debug.h"
#include "omxilosalservices.h"
#include "BSD_list.h"

#define LOG_NDEBUG 0
#define DBGT_LAYER 0
#define LOG_TAG "OmxILOsalPrm"
#include "linux_utils.h"

#define PRCMU_DEVICE_APE "/dev/ape_opp"
#define PRCMU_DEVICE_DDR "/dev/ddr_opp"

using namespace omxilosalservices;

typedef struct ResourceStateElem {
    LIST_ENTRY(ResourceStateElem) list_entry;
    
    OMXIL_OSAL_RESOUCE_ID ResourceId;
    OMX_HANDLETYPE ClientHandle;

    OMX_U32 Value;
    
    int fd_opp;		// The handle to /dev/ape_opp or /dev/ddr_opp, -1 if not opened

} ResourceStateElem_t;

typedef LIST_HEAD(ResourceStateListHead, ResourceStateElem) ResourceStateListHead_t;

// static so all members are NULL => Empty list on init
static ResourceStateListHead_t ResourceStateList;

static ResourceStateElem_t* getResourceState(OMXIL_OSAL_RESOUCE_ID aResourceId,
					     OMX_HANDLETYPE aClientHandle)
{
    ResourceStateElem_t* elem = NULL;
    
    LIST_FOREACH(elem, &ResourceStateList, list_entry) {
	if((elem->ResourceId == aResourceId) && (elem->ClientHandle == aClientHandle)) {
	    return elem;
	}
    }

    return NULL;
}

static ResourceStateElem_t* addResourceState(OMXIL_OSAL_RESOUCE_ID aResourceId,
					     OMX_HANDLETYPE aClientHandle)
{
    ResourceStateElem_t* elem = (ResourceStateElem_t*) malloc(sizeof(ResourceStateElem_t));
    
    if(elem == NULL) {
	return NULL;
    }
    LIST_INSERT_HEAD(&ResourceStateList, elem, list_entry);
    
    // Members init
    elem->ResourceId = aResourceId;
    elem->ClientHandle = aClientHandle;
    elem->Value = 0;
    elem->fd_opp = -1;
    
    return elem;
}

/********************************************************************************/

static OMX_ERRORTYPE updateSocDvfsOpp(ResourceStateElem_t* resState)
{
    if(resState->fd_opp == -1) {
	resState->fd_opp = open(PRCMU_DEVICE_APE, O_WRONLY);
	if(resState->fd_opp == -1) {
	    DBGT_ERROR("updateSocDvfsOpp - failed to open " PRCMU_DEVICE_APE "\n");
	    return OMX_ErrorNotImplemented;
	}
    }
    DBGT_PTRACE("updateSocDvfsOpp %d\n", (int) resState->Value);
    if(write(resState->fd_opp, &resState->Value, sizeof(OMX_U32)) != sizeof(OMX_U32)) {
	DBGT_ERROR("updateSocDvfsOpp - failed to write %d to " PRCMU_DEVICE_APE "\n",
	     (int) resState->Value);
	return OMX_ErrorBadParameter;
    }
    return OMX_ErrorNone;
}

static OMX_ERRORTYPE updateDDROpp(ResourceStateElem_t* resState)
{
    if(resState->fd_opp == -1) {
	resState->fd_opp = open(PRCMU_DEVICE_DDR, O_WRONLY);
	if(resState->fd_opp == -1) {
	    DBGT_ERROR("updateDDROpp - failed to open " PRCMU_DEVICE_DDR "\n");
	    return OMX_ErrorNotImplemented;
	}
    }
    DBGT_PTRACE("updateDDROpp %d\n", (int) resState->Value);
    if(write(resState->fd_opp, &resState->Value, sizeof(OMX_U32)) != sizeof(OMX_U32)) {
	DBGT_ERROR("updateDDROpp - failed to write %d to " PRCMU_DEVICE_DDR "\n",
	     (int) resState->Value);
	return OMX_ErrorBadParameter;
    }
    return OMX_ErrorNone;
}

static OMX_ERRORTYPE updatePwrResLatency(ResourceStateElem_t* resState)
{
    DBGT_PTRACE("updatePwrResLatency not implemented\n");
    return OMX_ErrorNone;
}

static OMX_ERRORTYPE updateResourceState(ResourceStateElem_t* resState)
{
    switch (resState->ResourceId) {
	
    case ESocDvfsOpp:
	return updateSocDvfsOpp(resState);
	
    case ESocDDROpp:
	return updateDDROpp(resState);
	
    case EPwrResLatency:
	return updatePwrResLatency(resState);
	
    default:
	return OMX_ErrorBadParameter;
    }
}

/********************************************************************************/

static OMX_ERRORTYPE freeSocOpp(ResourceStateElem_t* resState)
{
    if(resState->fd_opp != -1) {
	switch (resState->ResourceId) {
	    case ESocDvfsOpp:
		DBGT_PTRACE("freeSocDvfsOpp - closing " PRCMU_DEVICE_APE "\n");
		break;

	    case ESocDDROpp:
		DBGT_PTRACE("freeSocDDROpp - closing " PRCMU_DEVICE_DDR "\n");
		break;

            default:
                return OMX_ErrorUndefined;
	}
	if(close(resState->fd_opp) == -1) {
	    switch (resState->ResourceId) {
		    case ESocDvfsOpp:
			DBGT_ERROR("freeSocDvfsOpp - failed closing " PRCMU_DEVICE_APE "\n");
			break;

		    case ESocDDROpp:
			DBGT_ERROR("freeSocDDROpp - failed closing " PRCMU_DEVICE_DDR "\n");
			break;

                    default:
                        return OMX_ErrorUndefined;
		}
	    return OMX_ErrorUndefined;
	}
    }
    LIST_REMOVE(resState, list_entry);
    free(resState);
    
    return OMX_ErrorNone;
}

static OMX_ERRORTYPE freePwrResLatency(ResourceStateElem_t* resState)
{
    LIST_REMOVE(resState, list_entry);
    free(resState);

    return OMX_ErrorNone;
}

static OMX_ERRORTYPE freeResourceState(ResourceStateElem_t* resState)
{
    switch (resState->ResourceId) {
	
    case ESocDvfsOpp:
    case ESocDDROpp:
	return freeSocOpp(resState);
	
    case EPwrResLatency:
	return freePwrResLatency(resState);
	
    default:
	return OMX_ErrorBadParameter;
    }
}

/********************************************************************************/

OMX_ERRORTYPE OmxILOsalPrm::ChangeResourceState(OMXIL_OSAL_RESOUCE_ID aResourceId,
                                                OMX_U32 aValue,
                                                OMX_HANDLETYPE aClientHandle)
{
    DBGT_PTRACE("ChangeResourceState(%d, %d, %#08x)\n",
	 (int) aResourceId, (int) aValue, (unsigned int) aClientHandle);
    
    ResourceStateElem_t* resState = getResourceState(aResourceId, aClientHandle);
    int update_needed = 0;
    
    if(resState == NULL) {
	resState = addResourceState(aResourceId, aClientHandle);
	if(resState == NULL) {
	    DBGT_ERROR("ChangeResourceState - allocation failed\n");
	    return OMX_ErrorInsufficientResources;
	}
	resState->Value = aValue;
	update_needed = 1;
    } else {
	if(resState->Value != aValue) {
	    resState->Value = aValue;
	    update_needed = 1;
	}
    }
    
    if(update_needed) {
	return updateResourceState(resState);
    }
    
    return OMX_ErrorNone;
}

OMX_ERRORTYPE OmxILOsalPrm::ReleaseResourceState(OMXIL_OSAL_RESOUCE_ID aResourceId,
                                                 OMX_HANDLETYPE aClientHandle)
{
    DBGT_PTRACE("ReleaseResourceState(%d, %#08x)\n",
	 (int) aResourceId, (unsigned int) aClientHandle);

    ResourceStateElem_t* resState = getResourceState(aResourceId, aClientHandle);
    
    if(resState == NULL) {
	DBGT_ERROR("ReleaseResourceState - return OMX_ErrorBadParameter\n");
	return OMX_ErrorBadParameter;
    }
    
    return freeResourceState(resState);
}

