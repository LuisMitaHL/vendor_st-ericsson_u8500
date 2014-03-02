<?xml version="1.0" encoding="UTF-8"?>
<!--
* Copyright (C) ST-Ericsson SA 2011
* License terms: 3-clause BSD license
-->
<stylesheet version="1.0" xmlns="http://www.w3.org/1999/XSL/Transform">

<import href="common.xsl"/>

<output method="text"/>
<strip-space elements="*"/>
<param name="target"/>
<param name="supportedCommands" />
<variable name="supported_commands" select="document($supportedCommands)/commandspec"/>

<template match="/commandspec">/* $Copyright$ */
/* NOTE: This is an automatically generated file. DO NOT EDIT! */

#ifndef _COMMANDS_H
#define _COMMANDS_H
#include &quot;error_codes.h&quot;
#include &quot;t_command_protocol.h&quot;
#include "command_ids.h"

ErrorCode_e Do_CEH_Call(void *Object_p, CommandData_t *CmdData_p);
<apply-templates select="group"/>
#endif /* _COMMANDS_H */
</template>

<template match="group/command">
<variable name="group" select="../@number" />
<variable name="command" select="@number" />
<if test='$target="lcm" or $supported_commands/group[@number=$group]/command[@number=$command]'>
  <if test="contains(@source, &apos;ME&apos;)">
    <call-template name="prototype">
    <with-param name="name" select="concat(&apos;Do_&apos;, ../interface[@type=&apos;loader&apos;]/@name, &apos;_&apos;, ./interface[@type=&apos;loader&apos;]/@name)"/>
    <with-param name="ref" select="concat(&apos;Done_&apos;, ../interface[@type=&apos;loader&apos;]/@name, &apos;_&apos;, ./interface[@type=&apos;loader&apos;]/@name, &apos;Impl&apos;)"/>
    <with-param name="direction" select="input"/>
    <with-param name="source" select="&apos;ME&apos;"/>
    </call-template>
  </if>
  <if test="contains(@source, &apos;PC&apos;)">
    <call-template name="prototype">
    <with-param name="name" select="concat(&apos;Done_&apos;, ../interface[@type=&apos;loader&apos;]/@name, &apos;_&apos;, ./interface[@type=&apos;loader&apos;]/@name)"/>
    <with-param name="ref" select="concat(&apos;Do_&apos;, ../interface[@type=&apos;loader&apos;]/@name, &apos;_&apos;, ./interface[@type=&apos;loader&apos;]/@name, &apos;Impl&apos;)"/>
    <with-param name="direction" select="output"/>
    <with-param name="source" select="&apos;PC&apos;"/>
    </call-template>
  </if>
</if>
</template>

</stylesheet>
