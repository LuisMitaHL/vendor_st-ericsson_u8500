<?xml version="1.0" encoding="UTF-8"?>
<!--
* Copyright (C) ST-Ericsson SA 2011
* License terms: 3-clause BSD license
-->
<stylesheet version="1.0" xmlns="http://www.w3.org/1999/XSL/Transform">

<output method="text" indent="no"/>
<strip-space elements="*"/>
<param name="target" />
<param name="supportedCommands" />
<variable name="supported_commands" select="document($supportedCommands)/commandspec"/>

<template match="command">
<variable name="group" select="../@number" />
<variable name="command" select="@number" />
<if test='$target="lcm" or $supported_commands/group[@number=$group]/command[@number=$command]'>
<if test='contains(@supportedcmdtype, "longrunning")'>ErrorCode_e <value-of select="../interface/@name"/>_<value-of select="interface/@name"/>Repeat(void *ExecutionContext_p);
</if>
</if>
</template>

<template match="group">
<variable name="group" select="@number" />
<if test='$target="lcm" or $supported_commands/group[@number=$group]'>
<apply-templates select="command"/><if test='contains(@supportedcmdtype, "longrunning")'>ErrorCode_e <value-of select="interface/@name"/>_ErrorHandlerRepeat(void *ExecutionContext_p);
</if><text>
</text>
</if>
</template>

<template match="/commandspec">
/*******************************************************************************
 * $Copyright ST-Ericsson 2010 $
 ******************************************************************************/
#ifndef _CES_COMMANDS_H_
#define _CES_COMMANDS_H_
/**
 *  @addtogroup ldr_communication_module
 *  @{
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include &quot;error_codes.h&quot;

/*******************************************************************************
 * Declaration of functions
 ******************************************************************************/
<apply-templates select="group"/>uint32 Get_ApplicationInfoSize(void);
uint32 Get_ActiveApplicationsSize(void);

/*@}*/
#endif /*_CES_COMMANDS_H_*/
</template>

</stylesheet>
