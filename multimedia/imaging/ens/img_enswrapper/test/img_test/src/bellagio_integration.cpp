/*
* Copyright (C) ST-Ericsson SA 2010. All rights reserved.
* This code is ST-Ericsson proprietary and confidential.
* Any use of the code for whatever purpose is subject to
* specific written permission of ST-Ericsson SA.
*/
#include <stdlib.h>
#include <stdio.h>

#include "omxil/OMX_Core.h"
#include "omxil/OMX_Component.h"
#include "Img_omxloader_interface.h"

extern int  Factory_ImgEns_Test_Demo(OMX_COMPONENTTYPE &Handle);

 // For test purpose
const char *ImgEns_Test_Roles1[]={ "roles0.ImgEns_Test" ,  "roles1.ImgEns_Test" ,  "roles2.ImgEns_Test", NULL };
const char *ImgEns_Test_Roles2[]={ "roles0.ImgEns_Test2",  "roles1.ImgEns_Test2", NULL };
const char *ImgEns_Test_Roles3[]={ "roles0.ImgEns_Test3",  "roles1.ImgEns_Test3",  "roles2.ImgEns_Test3", NULL };
const char *ImgEns_Test_Roles4[]={ NULL };

/** Definition of all the components this module will register to the STE ENS loader*/
static _tImgOmxFactoryDescription ImgEns_TestFactories[] =
{
	// Names                   , Roles[]                              , Factory
	{ "OMX.ST.ImgEns_Test1"        , ImgEns_Test_Roles1                       , (int (*)(void *)) Factory_ImgEns_Test_Demo },
	{ "OMX.ST.ImgEns_Test2"        , ImgEns_Test_Roles2                       , (int (*)(void *)) Factory_ImgEns_Test_Demo },
	{ "OMX.ST.ImgEns_Test3"        , ImgEns_Test_Roles3                       , (int (*)(void *)) Factory_ImgEns_Test_Demo },
	{ "OMX.ST.ImgEns_Test4"        , ImgEns_Test_Roles4                       , (int (*)(void *)) Factory_ImgEns_Test_Demo },
	{ "OMX.ST.ImgEns_Test5"        , ImgEns_Test_Roles1                       , (int (*)(void *)) Factory_ImgEns_Test_Demo },

	{ "OMX.ST.ImgEns_Demo0"        , ImgEns_Test_Roles4                       , (int (*)(void *)) Factory_ImgEns_Test_Demo },
};


extern "C" IMGENS_TEST_CPT_API int ste_omx_img_component_register (const _tImgOmxFactoryDescription *&pFactoryDescription)
{
	int nbr= sizeof(ImgEns_TestFactories) / sizeof(ImgEns_TestFactories[0]);
	pFactoryDescription = ImgEns_TestFactories;
	return(nbr);
}

