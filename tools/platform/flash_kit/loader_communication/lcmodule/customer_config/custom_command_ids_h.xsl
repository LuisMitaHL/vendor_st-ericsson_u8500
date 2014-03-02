<?xml version="1.0" encoding="UTF-8"?>
<!--
* Copyright (C) ST-Ericsson SA 2011
* License terms: 3-clause BSD license
-->
<stylesheet xmlns="http://www.w3.org/1999/XSL/Transform"  version="2.0">

<import href="custom_common.xsl"/>

<output method="text" indent="no"/>
<strip-space elements="*"/>
<param name="target" />

<template match="/commandspec">/* $Copyright$ */
/* NOTE: This is an automatically generated file. DO NOT EDIT! */

#ifndef _CUSTOM_COMMAND_IDS_H
#define _CUSTOM_COMMAND_IDS_H
#include "t_basicdefinitions.h"

<!--
typedef enum {
<apply-templates select="group/command" mode="id"/>} CustomCommandId_e;
-->

<template match="group" mode="id">
  <text>  </text><call-template name="groupid" /> = <choose> <when test="@number&lt;10"> <value-of select="substring-after(@number, '0')" /> </when> <otherwise> <value-of select="@number"/> </otherwise> </choose>, /**&lt; <value-of select="@name" /> */
</template>

<template match="group[last()]" mode="id">
  <text>  </text><call-template name="groupid" /> = <choose> <when test="@number&lt;10"> <value-of select="substring-after(@number, '0')" /> </when> <otherwise> <value-of select="@number"/> </otherwise> </choose> /**&lt; <value-of select="@name" /> */
</template>

<template match="command" mode="id">
  <text>  </text><call-template name="commandid" /> = <value-of select="@number" />, /**&lt; <value-of select="@name" /> */ 
</template>

<template match="group[last()]/command[last()]" mode="id">
  <text>  </text><call-template name="commandid" /> = <value-of select="@number" />  /**&lt; <value-of select="@name" /> */ 
</template>

#endif /* _CUSTOM_COMMAND_IDS_H */
</template>

</stylesheet>
