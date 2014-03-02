<?xml version="1.0" encoding="UTF-8"?>
<!--
* Copyright (C) ST-Ericsson SA 2011
* License terms: 3-clause BSD license
-->
<stylesheet version="1.0" xmlns="http://www.w3.org/1999/XSL/Transform" >

<import href="custom_common.xsl"/>

<output method="text"/>
<strip-space elements="*"/>
<param name="target" />

<template match="/commandspec">/* $Copyright$ */
/* NOTE: This is an automatically generated file. DO NOT EDIT! */

#ifndef _CUSTOM_COMMANDS_H
#define _CUSTOM_COMMANDS_H
#include "error_codes.h"
#include "t_command_protocol.h"

ErrorCode_e Do_CustomCEH_Call(CommandData_t *CmdData_p);
<apply-templates select="group"/>
#endif /* _CUSTOM_COMMANDS_H */
</template>

<template match="group/command">
	<if test="contains(@source, 'ME')">
		<call-template name="prototype">
		<with-param name="name" select="concat('Do_', ../interface[@type='loader']/@name, '_', ./interface[@type='loader']/@name)" />
		<with-param name="ref" select="concat('Done_', ../interface[@type='loader']/@name, '_', ./interface[@type='loader']/@name, 'Impl')" />
		<with-param name="direction" select="input" />
		<with-param name="source" select="'ME'" />
		</call-template>
	</if>
	<if test="contains(@source, 'PC')">
		<call-template name="prototype">
		<with-param name="name" select="concat('Done_', ../interface[@type='loader']/@name, '_', ./interface[@type='loader']/@name)" />
		<with-param name="ref" select="concat('Do_', ../interface[@type='loader']/@name, '_', ./interface[@type='loader']/@name, 'Impl')" />
		<with-param name="direction" select="output" />
		<with-param name="source" select="'PC'" />
		</call-template>
	</if>
</template>

</stylesheet>
