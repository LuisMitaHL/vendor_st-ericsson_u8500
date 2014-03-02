/*****************************************************************************
* Copyright (C) ST-Ericsson SA 2010. All rights reserved.
* This code is ST-Ericsson proprietary and confidential.
* Any use of the code for whatever purpose is subject to
* specific written permission of ST-Ericsson SA.

* Author:     : laurent.regnier@st.com
* Filename:   : Img_omxloader_interface.h
* Description : shared interface for imaging omx component
*****************************************************************************/
#ifndef _IMG_OMXLOADER_INTERFACE_H_
#define _IMG_OMXLOADER_INTERFACE_H_

/** Stucture to register a given component along with its OMX std roles and the constructor setting up an ENS component */
typedef struct
{
	const char * name;                    /** The component name */
	const char **roles;                   /** The component roles, last element must be 'NULL' pointer */
	int (*fnImgFactory)(void * pHandle);  /** The component constructor */
} _tImgOmxFactoryDescription;

typedef int (*t_fnImgComponentEntry)(const _tImgOmxFactoryDescription *&pFactoryDescription);

#define IMG_OMXLOADER_COMPONENT_ENTRY_NAME  "ste_omx_img_component_register" /// Function name entry
#define IMG_ENV_OMX_COMPONENTS_DIR          "IMG_OMX_COMPONENTS_DIR"         /// Environment name for scanning Img component in dir 

typedef int (*t_fnImgComponentFactory)(const char *ActiveRole, void *usr_prt);

#endif /* _IMG_OMXLOADER_INTERFACE_H_ */
