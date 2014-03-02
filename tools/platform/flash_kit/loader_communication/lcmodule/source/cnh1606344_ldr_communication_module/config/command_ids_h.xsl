<?xml version="1.0" encoding="UTF-8"?>
<!--
* Copyright (C) ST-Ericsson SA 2011
* License terms: 3-clause BSD license
-->
<stylesheet xmlns="http://www.w3.org/1999/XSL/Transform"  version="2.0">

<import href="common.xsl"/>

<output method="text" indent="no"/>
<strip-space elements="*"/>
<param name="target" />
<param name="supportedCommands" />
<variable name="supported_commands" select="document($supportedCommands)/commandspec"/>

<template match="/commandspec">/* $Copyright$ */
/* NOTE: This is an automatically generated file. DO NOT EDIT! */

#ifndef _COMMAND_IDS_H
#define _COMMAND_IDS_H
#include "t_basicdefinitions.h"
<apply-templates select="group/command" mode="define"/>

TYPEDEF_ENUM {
<apply-templates select="group" mode="id"/>} ENUM8(GroupId_e);

typedef enum {
<apply-templates select="group/command" mode="id"/>} CommandId_e;

<apply-templates select="typedef" />
#endif /* _COMMAND_IDS_H */
</template>

<template match="typedef">
typedef struct <value-of select="interface/@name" />_s {

<apply-templates select="value" />}<value-of select="interface/@name"/>_t;
</template>
<template match="value">
<text>  </text>
  <choose>
  <when test="@type='string'">
    char   *<text></text><value-of select="@name" />; /**&lt; <value-of select="text()" /> */
  </when>
  <when test="@type='uint32'">
  <value-of select="@type" /><text>  </text><value-of select="@name" />; /**&lt; <value-of select="text()" /> */
  </when>
  <when test="@type='uint64'">
  <value-of select="@type" /><text>  </text><value-of select="@name" />; /**&lt; <value-of select="text()" /> */
  </when>
</choose>
<if test="position() = last()"><text></text>
</if>
</template>

<template match="group" mode="id">
<variable name="group" select="@number" />
<if test='$target="lcm" or $supported_commands/group[@number=$group]'>
<text>    </text><call-template name="groupid" /> = <choose> <when test="@number&lt;10"> <value-of select="substring-after(@number, '0')" /> </when> <otherwise> <value-of select="@number"/> </otherwise> </choose>, /**&lt; <value-of select="@name" /> */
</if>
</template>

<template match="group[last()]" mode="id">
<variable name="group" select="@number" />
<if test='$target="lcm" or $supported_commands/group[@number=$group]'>
<text>    </text><call-template name="groupid" /> = <choose> <when test="@number&lt;10"> <value-of select="substring-after(@number, '0')" /> </when> <otherwise> <value-of select="@number"/> </otherwise> </choose>  /**&lt; <value-of select="@name" /> */
</if>
</template>

<template match="command" mode="id">
<variable name="group" select="../@number" />
<variable name="command" select="@number" />
<if test='$target="lcm" or $supported_commands/group[@number=$group]/command[@number=$command]'>
<text>    </text><call-template name="commandid" /> = <value-of select="@number" />, /**&lt; <value-of select="@name" /> */
</if>
</template>

<template match="group[last()]/command[last()]" mode="id">
<variable name="group" select="../@number" />
<variable name="command" select="@number" />
<if test='$target="lcm" or $supported_commands/group[@number=$group]/command[@number=$command]'>
<text>    </text><call-template name="commandid" /> = <value-of select="@number" />  /**&lt; <value-of select="@name" /> */
</if>
</template>

<template match="command" mode="define">
<variable name="group" select="../@number"/>
<variable name="command" select="@number"/>
<if test='$target=&quot;lcm&quot; or $supported_commands/group[@number=$group]/command[@number=$command]'>
#define <call-template name="commandsupported"/>
</if>
</template>

</stylesheet>
