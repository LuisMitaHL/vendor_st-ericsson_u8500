/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ENS_Wrapper.h"
#include "ENS_WrapperLog.h"
#include "ENS_Index.h"
#include "ste_ost_group_map.h"

omx_cmp_trace_list_head_t ENS_Wrapper::omx_cmp_trace_list;

#define LINE_BUFFER_SIZE 1024

#define ALL_GROUPS_MASK 0xffff

#define DEFAULT_GROUPS_MASK ENS_Wrapper::OST_trace_default_mask

// Returns 0 in case of invalid group name other wise the mask value
OMX_U16 getTraceMask(char * name)
{
    int dummy;

    if(strcmp(name, "ERROR") == 0) {
	dummy = FIXED_GROUP_TRACE_ERROR; // used to ensure correct alignement with definitions
	return 1 << 0;
    }
    if (strcmp(name, "WARNING") == 0) {
	dummy = FIXED_GROUP_TRACE_WARNING; // used to ensure correct alignement with definitions
	return 1 << 1;
    }
    if (strcmp(name, "FLOW") == 0) {
	dummy = FIXED_GROUP_TRACE_FLOW; // used to ensure correct alignement with definitions
	return 1 << 2;
    }
    if (strcmp(name, "DEBUG") == 0) {
	dummy = FIXED_GROUP_TRACE_DEBUG; // used to ensure correct alignement with definitions
	return 1 << 3;
    }
    if (strcmp(name, "API") == 0) {
	dummy = FIXED_GROUP_TRACE_API; // used to ensure correct alignement with definitions
	return 1 << 4;
    }
    if (strcmp(name, "OMX_API") == 0) {
	dummy = FIXED_GROUP_TRACE_OMX_API; // used to ensure correct alignement with definitions
	return 1 << 5;
    }
    if (strcmp(name, "OMX_BUFFER") == 0) {
	dummy = FIXED_GROUP_TRACE_OMX_BUFFER; // used to ensure correct alignement with definitions
	return 1 << 6;
    }
    if (strcmp(name, "ALWAYS") == 0) {
	dummy = FIXED_GROUP_TRACE_ALWAYS; // used to ensure correct alignement with definitions
	return 1 << 7;
    }
    if (strcmp(name, "USER1") == 0) {
	dummy = FIXED_GROUP_TRACE_USER1; // used to ensure correct alignement with definitions
	return 1 << 8;
    }
    if (strcmp(name, "USER2") == 0) {
	dummy = FIXED_GROUP_TRACE_USER2; // used to ensure correct alignement with definitions
	return 1 << 9;
    }
    if (strcmp(name, "USER3") == 0) {
	dummy = FIXED_GROUP_TRACE_USER3; // used to ensure correct alignement with definitions
	return 1 << 10;
    }
    if (strcmp(name, "USER4") == 0) {
	dummy = FIXED_GROUP_TRACE_USER4; // used to ensure correct alignement with definitions
	return 1 << 11;
    }
    if (strcmp(name, "USER5") == 0) {
	dummy = FIXED_GROUP_TRACE_USER5; // used to ensure correct alignement with definitions
	return 1 << 12;
    }
    if (strcmp(name, "USER6") == 0) {
	dummy = FIXED_GROUP_TRACE_USER6; // used to ensure correct alignement with definitions
	return 1 << 13;
    }
    if (strcmp(name, "USER7") == 0) {
	dummy = FIXED_GROUP_TRACE_USER7; // used to ensure correct alignement with definitions
	return 1 << 14;
    }
    if (strcmp(name, "USER8") == 0) {
	dummy = FIXED_GROUP_TRACE_USER8; // used to ensure correct alignement with definitions
	return 1 << 15;
    }

    ALOGE("TraceSpecParsing: Invalid Group name='%s' - ignoring", name);

    return 0;
}

void ENS_Wrapper::addOSTtraceFilteringSpec(const char *componentName, OMX_U16 traceEnableMask)
{
    omx_cmp_trace_list_elem_t * list_elem =
        (omx_cmp_trace_list_elem_t *) malloc(sizeof(omx_cmp_trace_list_elem_t));
    if (list_elem != NULL) {
        strcpy(list_elem->name, componentName);
        list_elem->nTraceEnable = traceEnableMask;
        LIST_INSERT_HEAD(&omx_cmp_trace_list, list_elem, list_entry);
    } else {
        ALOGE("TraceSpecParsing: failed to allocate memory for '%s' specs", componentName);
    }
}

void ENS_Wrapper::processOSTtraceFilteringSpec(int enable, int mpc_trace_config)
{
    char line_buffer[LINE_BUFFER_SIZE];
    char filename[OMX_MAX_STRINGNAME_SIZE];
    int line_no = 0;
    OMX_U16 traceEnableMask;
    int old_OST_trace_enabled = OST_trace_enabled;
    
    // Update as requested
    OST_trace_enabled = enable;
    
    if(old_OST_trace_enabled) {
	if (!OST_trace_enabled) {
	    cleanupOSTtraceFilteringSpec(mpc_trace_config);
	}
	// else already active
	return;
    } else {
	if(!OST_trace_enabled)
	    // No change in value ; still inactive
	    return;
    }
    // (Re-)Activating trace
    {
        GET_PROPERTY("STE_TRACE_SPEC_FILE", value, "/data/STEtraceSpec.txt");
        strncpy(filename, value, OMX_MAX_STRINGNAME_SIZE);
    }

    { // VT traces shortcut
        GET_PROPERTY("persist.media.debug.vt", value, "0");
        if (!strcmp(value, "1")) {
            addOSTtraceFilteringSpec("OMX.ST.VFM.MPEG4Enc", ALL_GROUPS_MASK);
            addOSTtraceFilteringSpec("OMX.ST.VFM.MPEG4Dec", ALL_GROUPS_MASK);
            addOSTtraceFilteringSpec("OMX.ST.VFM.MPEG4HostDec", ALL_GROUPS_MASK);
	}
        activateOSTtraceFilteringSpec(mpc_trace_config);
    }
    
    FILE * file = fopen (filename, "r");
    
    if (file == NULL) {
	ALOGE("TraceSpecParsing: Unable to open STE trace specification file: %s", filename);
	return;
    } else {
	ALOGI("TraceSpecParsing: Processing STE trace specification file: %s", filename);
    }
    
    while (fgets(line_buffer, LINE_BUFFER_SIZE, file) != NULL) {
	char * ptr = line_buffer;
	char component_name[OMX_MAX_STRINGNAME_SIZE];
	char group_name[OMX_MAX_STRINGNAME_SIZE];
	char instance_spec[OMX_MAX_STRINGNAME_SIZE];
	char * curr_field;

	line_no++;

	/* skip heading white space */
	while((*ptr == ' ') || (*ptr == '\t')) {
	    ptr++;
	}
	if (*ptr == '#')
	    continue;
	if (*ptr == '\n')
	    continue;

	/* Read component name part */
	curr_field = ptr;
	while((*ptr != ':') && (*ptr != '\n')) { ptr++; }
	*(ptr++) = '\0';
	
	if(sscanf(curr_field, "%s", component_name) == 1) {
	    WRAPPER_DEBUG_LOG("TraceSpecParsing: Component name='%s'", component_name);
	} else {
	    ALOGE("%s:%d: TraceSpecParsing syntax error: skipping line", filename, line_no);
	    continue;
	}
	/* Read group part */
	curr_field = ptr;
	while((*ptr != ':') && (*ptr != '\n') && (*ptr != '\0')) { ptr++; }
	if(*ptr != '\0') {
	    *(ptr++) = '\0';
	    
	    int nb_groups = 0;
	    // reset group mask
	    traceEnableMask = 0;
	    
	    while(sscanf(curr_field, "%s", group_name) == 1) {
		WRAPPER_DEBUG_LOG("TraceSpecParsing: Group name='%s'\n", group_name);
		curr_field += (strlen(group_name)+1);
		nb_groups++;
		traceEnableMask |= getTraceMask(group_name);
	    }
	    if (nb_groups == 0) {
		// Default to all groups
		traceEnableMask = ALL_GROUPS_MASK;
		WRAPPER_DEBUG_LOG("TraceSpecParsing: all groups enabled\n");
	    }
      
	    /* Component instance part */
	    if(sscanf(ptr, "%s", instance_spec) == 1) {
		ALOGE("%s:%d: TraceSpecParsing warning: instance ID filtering is not yet "
                     "supported and is ignored\n", filename, line_no);
	    }
	} else {
	    // Default to all groups
	    traceEnableMask = ALL_GROUPS_MASK;
	    WRAPPER_DEBUG_LOG("TraceSpecParsing: all groups enabled\n");
	}
	// We got all the information so we record the Spec trace for this component
        addOSTtraceFilteringSpec(component_name, traceEnableMask);
    }
    
    fclose(file);
    
    // Activate the trace on running components
    activateOSTtraceFilteringSpec(mpc_trace_config);
}

void ENS_Wrapper::activateOSTtraceFilteringSpec(int mpc_trace_config)
{
    omx_cmp_list_elem_t * iter_cmp_elem = NULL;
    
    // Walk through active component to enable its trace if required
    OMXlistLock();
    LIST_FOREACH(iter_cmp_elem, &omx_cmp_list, list_entry) {
	ENS_Wrapper_p wrapper = (ENS_Wrapper_p) iter_cmp_elem->omx_component->pComponentPrivate;
	wrapper->ActivateOSTtrace(mpc_trace_config);
    }
    OMXlistUnLock();
}

void ENS_Wrapper::cleanupOSTtraceFilteringSpec(int mpc_trace_config)
{
    omx_cmp_trace_list_elem_t * iter_trace_elem = NULL;
    omx_cmp_trace_list_elem_t * tmp_next_trace_elem = NULL;
    omx_cmp_list_elem_t * iter_cmp_elem = NULL;

    LIST_FOREACH_SAFE(iter_trace_elem, &omx_cmp_trace_list, list_entry, tmp_next_trace_elem) {
	LIST_REMOVE(iter_trace_elem, list_entry);
	// Find matching active component to disable its non default trace
	LIST_FOREACH(iter_cmp_elem, &omx_cmp_list, list_entry) {
	    ENS_Wrapper_p wrapper = (ENS_Wrapper_p) iter_cmp_elem->omx_component->pComponentPrivate;
	    const char * name = wrapper->ensComponent->getName();
	    if(strcmp(name, iter_trace_elem->name) == 0) {
		ENS_CONFIG_TRACESETTINGTYPE Config;
		
		memset(&Config, 0, sizeof(ENS_CONFIG_TRACESETTINGTYPE));
		Config.nSize         = sizeof(ENS_CONFIG_TRACESETTINGTYPE);
		Config.nVersion      = wrapper->ensComponent->getVersion();
		Config.nTraceEnable  = DEFAULT_GROUPS_MASK;
		Config.nParentHandle = (unsigned int)wrapper->omxComponent;
		
		WRAPPER_OMX_LOG(iter_cmp_elem->omx_component,
                                "OSTTrace: enabling default trace for '%s'\n", name);

                ENS_CONFIG_TRACEMODEMPC MPCconfig;
                memset(&MPCconfig, 0, sizeof(ENS_CONFIG_TRACEMODEMPC));
                MPCconfig.nSize         = sizeof(ENS_CONFIG_TRACEMODEMPC);
                MPCconfig.nVersion      = wrapper->ensComponent->getVersion();
                MPCconfig.bEnable       = (mpc_trace_config ? OMX_FALSE : OMX_TRUE);

		// Make the call got though wrapper
		OMX_SetConfig(wrapper->omxComponent,
			      (OMX_INDEXTYPE) ENS_IndexConfigTraceMpc,
			      &MPCconfig);
		OMX_SetConfig(wrapper->omxComponent,
			      (OMX_INDEXTYPE) ENS_IndexConfigTraceSetting,
			      &Config);
		continue;
	    }
	}
	
	free(iter_trace_elem);
    }
}

OMX_ERRORTYPE ENS_Wrapper::ActivateOSTtrace(int mpc_trace_config)
{
    const char * name = ensComponent->getName();
 
    WRAPPER_LOCK(ENS_WRAPPER_ACTIVATEOSTTRACE_HANDLE_ID);

    omx_cmp_trace_list_elem_t * iter_elem = NULL;
    LIST_FOREACH(iter_elem, &omx_cmp_trace_list, list_entry) {
	if (strcmp(name, iter_elem->name) == 0) {

	    WRAPPER_UNLOCK(ENS_WRAPPER_ACTIVATEOSTTRACE_HANDLE_ID);

	    // Activate the trace on this component instance
	    ENS_CONFIG_TRACESETTINGTYPE Config;
	    memset(&Config, 0, sizeof(ENS_CONFIG_TRACESETTINGTYPE));
	    Config.nSize         = sizeof(ENS_CONFIG_TRACESETTINGTYPE);
	    Config.nVersion      = ensComponent->getVersion();
	    Config.nTraceEnable  = iter_elem->nTraceEnable;
	    Config.nParentHandle = (unsigned int) omxComponent;

            ENS_CONFIG_TRACEMODEMPC MPCconfig;
            memset(&MPCconfig, 0, sizeof(ENS_CONFIG_TRACEMODEMPC));
            MPCconfig.nSize         = sizeof(ENS_CONFIG_TRACEMODEMPC);
            MPCconfig.nVersion      = ensComponent->getVersion();
            MPCconfig.bEnable       = (mpc_trace_config ? OMX_FALSE : OMX_TRUE);

	    WRAPPER_OMX_LOG(omxComponent,
                            "OSTTrace: enabling trace for '%s' with group mask %#04x\n",
			    name, iter_elem->nTraceEnable);

	    // Make the call got though wrapper
	    OMX_SetConfig(omxComponent, (OMX_INDEXTYPE) ENS_IndexConfigTraceMpc, &MPCconfig);
	    return OMX_SetConfig(omxComponent, (OMX_INDEXTYPE) ENS_IndexConfigTraceSetting, &Config);
	}
    }
    WRAPPER_UNLOCK(ENS_WRAPPER_ACTIVATEOSTTRACE_HANDLE_ID);

    // In case no spec has been provided we activate the ERROR and WARNING in any case
    ENS_CONFIG_TRACESETTINGTYPE Config;
    memset(&Config, 0, sizeof(ENS_CONFIG_TRACESETTINGTYPE));
    Config.nSize         = sizeof(ENS_CONFIG_TRACESETTINGTYPE);
    Config.nVersion      = ensComponent->getVersion();
    Config.nTraceEnable  = DEFAULT_GROUPS_MASK;
    Config.nParentHandle = (unsigned int) omxComponent;

    ENS_CONFIG_TRACEMODEMPC MPCconfig;
    memset(&MPCconfig, 0, sizeof(ENS_CONFIG_TRACEMODEMPC));
    MPCconfig.nSize         = sizeof(ENS_CONFIG_TRACEMODEMPC);
    MPCconfig.nVersion      = ensComponent->getVersion();
    MPCconfig.bEnable       = (mpc_trace_config ? OMX_FALSE : OMX_TRUE);

    WRAPPER_OMX_LOG(omxComponent,
                    "OSTTrace: enabling default trace for '%s' with group mask %#04x\n",
                    name, DEFAULT_GROUPS_MASK);

    OMX_SetConfig(omxComponent, (OMX_INDEXTYPE) ENS_IndexConfigTraceMpc, &MPCconfig);
    return OMX_SetConfig(omxComponent, (OMX_INDEXTYPE) ENS_IndexConfigTraceSetting, &Config);
}



