<?xml version="1.0" encoding="UTF-8"?>
<!--
* Copyright (C) ST-Ericsson SA 2011
* License terms: 3-clause BSD license
-->
<stylesheet version="1.0" xmlns="http://www.w3.org/1999/XSL/Transform" >

<import href="common.xsl"/>

<output method="text"/>
<strip-space elements="*"/>
<param name="target" />
<param name="supportedCommands" />
<variable name="supported_commands" select="document($supportedCommands)/commandspec"/>

<template match="/commandspec">/* $Copyright$ */
/* NOTE: This is an automatically generated file. DO NOT EDIT! */

#ifndef _COMMANDS_IMPL_H
#define _COMMANDS_IMPL_H
#include "error_codes.h"
<apply-templates select="group" />
#endif /* _COMMANDS_IMPL_H */
</template>

<template match="group/documentation">
<variable name="group" select="../@number" />
<if test='$target="lcm" or $supported_commands/group[@number=$group]'>
/*
 * <value-of select="normalize-space(.)"/>
 */
</if>
</template>

<template match="group/command">
<variable name="group" select="../@number" />
<variable name="command" select="@number" />
<if test='$target="lcm" or $supported_commands/group[@number=$group]/command[@number=$command]'>
  <if test="contains(@source, 'PC')">
    <call-template name="prototype">
      <with-param name="name" select="concat('Do_', ../interface[@type='loader']/@name, '_', ./interface[@type='loader']/@name, 'Impl')" />
			<with-param name="ref" select="concat('Done_', ../interface[@type='loader']/@name, '_', ./interface[@type='loader']/@name)" />
			<with-param name="direction" select="input" />
			<with-param name="source" select="'PC'" />
		</call-template>
	</if>
  <if test="contains(@source, 'ME')">
    <call-template name="prototype">
      <with-param name="name" select="concat('Done_', ../interface[@type='loader']/@name, '_', ./interface[@type='loader']/@name, 'Impl')" />
      <with-param name="ref" select="concat('Do_', ../interface[@type='loader']/@name, '_', ./interface[@type='loader']/@name)" />
      <with-param name="direction" select="output" />
      <with-param name="source" select="'PC'" />
    </call-template>
  </if>
</if>
</template>

</stylesheet>
