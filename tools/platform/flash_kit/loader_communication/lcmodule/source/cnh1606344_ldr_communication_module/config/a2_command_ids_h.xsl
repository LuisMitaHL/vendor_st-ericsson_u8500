<?xml version="1.0" encoding="UTF-8"?>
<!--
* Copyright (C) ST-Ericsson SA 2011
* License terms: 3-clause BSD license
-->
<stylesheet xmlns="http://www.w3.org/1999/XSL/Transform"  version="2.0">

<import href="a2_common.xsl"/>

<output method="text" indent="no"/>
<strip-space elements="*"/>
<param name="target" />

<template match="/commandspec">/* $Copyright$ */
/* NOTE: This is an automatically generated file. DO NOT EDIT! */

#ifndef _A2_COMMAND_IDS_H
#define _A2_COMMAND_IDS_H
#include "t_basicdefinitions.h"

TYPEDEF_ENUM {
<apply-templates select="group" mode="id"/>} ENUM8(A2_GroupId_e);

typedef enum {
<apply-templates select="group/command" mode="id"/>} A2_CommandId_e;

<apply-templates select="typedef" />
#endif /* _A2_COMMAND_IDS_H */
</template>

<template match="typedef">
typedef struct <value-of select="interface/@name" />_s
{
<apply-templates select="value" />}<value-of select="interface/@name" />_t;
</template>

<template match="value">
  <text>  </text>
  <choose>
  <when test="@type='string'">
  char *<text>  </text><value-of select="@name" />; /**&lt; <value-of select="text()" /> */
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
  <text>  </text><call-template name="groupid" /> = <value-of select="@number" />, /**&lt; <value-of select="@name" /> */ 
</template>

<template match="group[last()]" mode="id">
  <text>  </text><call-template name="groupid" /> = <value-of select="@number" /> /**&lt; <value-of select="@name" /> */ 
</template>

<template match="command" mode="id">
  <text>  </text><call-template name="commandid" /> = <value-of select="@number" />, /**&lt; <value-of select="@name" /> */ 
</template>

<template match="group[last()]/command[last()]" mode="id">
  <text>  </text><call-template name="commandid" /> = <value-of select="@number" />  /**&lt; <value-of select="@name" /> */ 
</template>

</stylesheet>
