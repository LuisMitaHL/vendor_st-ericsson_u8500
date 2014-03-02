<?xml version="1.0" encoding="UTF-8"?>
<!--
* Copyright (C) ST-Ericsson SA 2011
* License terms: 3-clause BSD license
-->
<stylesheet version="1.0" xmlns="http://www.w3.org/1999/XSL/Transform">

<import href="common.xsl"/>

<output method="text" indent="no"/>
<strip-space elements="*"/>
<param name="target" />
<param name="supportedCommands" />
<variable name="supported_commands" select="document($supportedCommands)/commandspec"/>

<template match="command">
<variable name="group" select="../@number" />
<variable name="command" select="@number" />
<if test='$target="lcm" or $supported_commands/group[@number=$group]/command[@number=$command]'>
<if test='contains(@supportedcmdtype, "longrunning")'>
<text>  </text>  {
<text>   </text>     NULL,
<text>        </text><value-of select="../interface/@name"/>_<value-of select="interface/@name"/>Repeat,
<text>        </text><call-template name="commandid"/>,
<text>   </text>     1
<text>  </text>  },
</if>
</if>
</template>

<template match="group" mode="CommandsList">
<variable name="group" select="@number" />
<if test='$target="lcm" or $supported_commands/group[@number=$group]'>
<if test='contains(@supportedcmdtype, "longrunning")'>
CommandInformation_t Generic<value-of select="interface/@name"/>GroupCommandsList[] = {

<apply-templates select="command" />
    {
        NULL,
        <value-of select="interface/@name"/>_ErrorHandlerRepeat,
        COMMAND_ERRORHANDLER,
        1
    },
    /* ADD MORE COMMANDS */
    /*stop block. it can not be removed*/
    {
        NULL,
        NULL,
        0xFF,
        1
    }
};
</if>
</if>
</template>

<template match="group" mode="ActiveApplications">
<variable name="group" select="@number" />
<if test='$target="lcm" or $supported_commands/group[@number=$group]'>
<if test='contains(@supportedcmdtype, "longrunning")'>
<text>  </text>  {
<text>        </text><call-template name="groupid" />,
<text>    </text>    0,
<text>    </text>    Generic<value-of select="interface/@name"/>GroupCommandsList
<text>  </text>  },
</if>
</if>
</template>

<template match="/commandspec">
/*******************************************************************************
 * $Copyright ST-Ericsson 2010 $
 ******************************************************************************/
/**
 *  @addtogroup ldr_communication_module
 *  @{
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include &lt;stdio.h&gt;
#include &lt;stdlib.h&gt;
#include &lt;string.h&gt;

#include &quot;ces_commands.h&quot;
#include &quot;t_command_exec_service.h&quot;
#include &quot;command_ids.h&quot;
#include &quot;command_execution_control.h&quot;

/*******************************************************************************
 * File scope types, constants and variables
 ******************************************************************************/
/*
 *  Commands that need command execution service
 */
<apply-templates select="group" mode="CommandsList" />

ApplicationInfo_t ActiveApplications[] = {

<apply-templates select="group" mode="ActiveApplications" />
    /* ADD MORE APPLICATIONS HERE */
};

/*******************************************************************************
 * Definition of external functions
 ******************************************************************************/
uint32 Get_ApplicationInfoSize(void)
{
    return sizeof(ApplicationInfo_t);
}

uint32 Get_ActiveApplicationsSize(void)
{
    return sizeof(ActiveApplications);
}

/*@}*/
</template>


</stylesheet>
