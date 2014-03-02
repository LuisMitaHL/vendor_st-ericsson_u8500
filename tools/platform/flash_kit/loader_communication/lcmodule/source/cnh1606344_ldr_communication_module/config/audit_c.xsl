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

<template match="group/documentation" mode="array">
<variable name="group" select="../@number" />
<if test='$target="lcm" or $supported_commands/group[@number=$group]'>
/*
 * <value-of select="normalize-space(.)"/>
 */
</if>
</template>
  
<template match="group/documentation" mode="autharray">
<variable name="group" select="../@number" />
<if test='$target="lcm" or $supported_commands/group[@number=$group]'>
/*
 * <value-of select="normalize-space(.)"/>
 */
</if>
</template>
  
  
<template match="group/documentation" mode="staticarray">
<variable name="group" select="../@number" />
<if test='$target="lcm" or $supported_commands/group[@number=$group]'>
    /*
     * <value-of select="normalize-space(.)"/>
     */
</if>
</template>

<template name="staticarray">
<param name="name" />
<param name="direction" />
<choose>
<when test="name($direction)='input'">
    /* Command <value-of select="../@name" /> / <value-of select="@name" /> (<value-of select="../@number" /> / <value-of select="@number" />) */
</when>
<when test="name($direction)='output'">
    /* Response to <value-of select="../@name" /> / <value-of select="@name" /> (<value-of select="../@number" /> / <value-of select="@number" />) */
</when>
</choose>
    {
        <call-template name="groupid" ><with-param name="path" select=".."/></call-template>,
        <call-template name="commandid" />,
      <variable name="group" select="../@number" />
      <variable name="command" select="@number" />
        <choose>
          <when test='not($supported_commands/group[@number=$group]/command[@number=$command]/permissions)'>
        <choose>
          <when test="permissions"><text>  </text><apply-templates select="permissions/value" mode="permnumber" />,
      </when>
          <otherwise>  0,
      </otherwise>
      </choose>
        <choose>
          <when test="permissions">  PermArray_<value-of select="concat(../interface[@type='loader']/@name, '_', ./interface[@type='loader']/@name)" />,
      </when>
          <otherwise>  NULL,
      </otherwise>
      </choose>
      </when>
      <otherwise>
        <choose>
          <when test="$supported_commands/group[@number=$group]/command[@number=$command]/permissions"><text>  </text><apply-templates    select="$supported_commands/group[@number=$group]/command[@number=$command]/permissions/value" mode="permnumber" />,
      </when>
        <otherwise>  0,
      </otherwise>
      </choose>
        <choose>
          <when test="$supported_commands/group[@number=$group]/command[@number=$command]/permissions">  PermArray_<value-of select="concat(../interface[@type='loader']/@name, '_', ./interface[@type='loader']/@name)" />,
      </when>
        <otherwise>  NULL,
      </otherwise>
      </choose>
      </otherwise>
      </choose>
        <choose>
          <when test="authentication"><text>  </text><apply-templates select="authentication/value" mode="authnumber" />,
      </when>
        <otherwise>  0,
      </otherwise>
      </choose>
        <choose>
        <when test="authentication">  AuthArray_<value-of select="concat(../interface[@type='loader']/@name, '_', ./interface[@type='loader']/@name)" />,
      </when>
        <otherwise>  NULL,
      </otherwise>
      </choose>
        <choose>
        <when test="authentication">
          <choose>
            <when test="./authentication/@depandancy='and'">  1,
      </when>
            <when test="./authentication/@depandancy='or'">  2,
      </when>
            <when test="./authentication/@depandancy='xor'">  3,
      </when>
          </choose>
        </when>
        <otherwise>  0,
      </otherwise>
      </choose>
        <choose>
          <when test="authentication"><text>  </text><value-of select="./authentication/@factory"/>,
      </when>
        <otherwise>  FALSE,
      </otherwise>
      </choose>
        <choose>
          <when test="authentication"><text>  </text><value-of select="./authentication/@rd"/>,
      </when>
        <otherwise>  FALSE,
      </otherwise>
      </choose>
        <choose>
          <when test="authentication"><text>  </text><value-of select="./authentication/@product"/>,
      </when>
        <otherwise>  FALSE,
      </otherwise>
      </choose>
        <choose>
          <when test="authentication"><text>  </text><value-of select="./authentication/@service"/>,
      </when>
        <otherwise>  FALSE,
      </otherwise>
      </choose>  NULL
    },
</template>

  
<template match="/commandspec">/* $Copyright ST-Ericsson 2010 $ */
/* NOTE: This is an automatically generated file. DO NOT EDIT! */
#include &lt;stdio.h&gt;
#include &lt;stdlib.h&gt;
#include &lt;string.h&gt;

#include "t_command_protocol.h"
#include "r_debug.h"
#include "r_communication_service.h"
#include "commands.h"
#include "command_ids.h"
#include "commands_impl.h"
#include "error_codes.h"
#include "r_serialization.h"
#include "r_command_protocol.h"
#include "audit.h"

#ifdef CFG_ENABLE_AUDIT_CMD
#include "r_loader_sec_lib.h"
#endif

#ifdef CFG_ENABLE_ADBG_LOADER
#include "r_adbg_command.h"
#endif

/*
 * Command Permissions Vectors.
 */
<apply-templates select="group" mode="array"/>
/*
 * Command Authentication Vectors.
 */
<apply-templates select="group" mode="autharray"/>

CommandPermissionList_t CommandPermissionList[] = {

<apply-templates select="group" mode="staticarray"/>
    /* End of array */
    {
        0,
        (CommandId_e)0,
        0,
        NULL,
        0,
        NULL,
        0,
        FALSE,
        FALSE,
        FALSE,
        FALSE,
        NULL
    }
};

CommandPermissionList_t *GetAuditData(CommandData_t *CmdData_p)
{
    /*
      boolean A1_Authentication = FALSE;
      boolean CA_Authentication = FALSE;

      boolean A1_Factory        = FALSE;
      boolean A1_RD             = FALSE;
      boolean A1_Product        = FALSE;
      boolean A1_Service        = FALSE;

      uint8 A1_depandancy       = 0x00;

      uint32 AuthenticationVector = 0; */
    int CounterList = 0;

    while (CommandPermissionList[CounterList].Group != 0) {
        if (CmdData_p-&gt;ApplicationNr == CommandPermissionList[CounterList].Group &amp;&amp; CmdData_p-&gt;CommandNr == CommandPermissionList[CounterList].Command) {

            CommandPermissionList[CounterList].DataParam_p = CmdData_p-&gt;Payload.Data_p;
            return (&amp;CommandPermissionList[CounterList]);
        }

        CounterList++;
    }

    return NULL;
}

#ifdef CFG_ENABLE_AUDIT_CMD
ErrorCode_e CommandAudit(CommandData_t *CmdData_p)
{
    ErrorCode_e ReturnValue = E_GENERAL_FATAL_ERROR;
    CommandPermissionList_t *CmdPermission_p = NULL;

    CmdPermission_p = GetAuditData(CmdData_p);

    if (NULL == CmdPermission_p) {
        ReturnValue = E_UNSUPPORTED_CMD;
        goto ErrorExit;
    }

    ReturnValue = (ErrorCode_e)Do_LoaderSecLib_Audit_Permission_Levels(CmdPermission_p);

    if (ReturnValue != E_SUCCESS) {
        ReturnValue = E_AUDITING_FAILED;
    }

ErrorExit:
    return ReturnValue;
}
#endif
</template>

<template match="group/command" mode="array">
<variable name="group" select="../@number" />
<variable name="command" select="@number" />
<if test='$target="lcm" or $supported_commands/group[@number=$group]/command[@number=$command]'>
  <variable name="group" select="../@number" />
  <variable name="command" select="@number" />
<if test="contains(@source, 'ME') and not(contains(@source, 'PC')) and permissions">
CommandPermission_e PermArray_<value-of select="concat(../interface[@type='loader']/@name, '_', ./interface[@type='loader']/@name)" />[]  = {
  <choose>
  <when test='not($supported_commands/group[@number=$group]/command[@number=$command]/permissions)'><apply-templates select="permissions/value" mode="perm" />
  </when>
  <otherwise>
  <apply-templates select="$supported_commands/group[@number=$group]/command[@number=$command]/permissions/value" mode="perm" />
  </otherwise>
  </choose>
};
</if>
<if test="contains(@source, 'PC') and permissions">
CommandPermission_e PermArray_<value-of select="concat( ../interface[@type='loader']/@name, '_', ./interface[@type='loader']/@name)" />[] = {
  <choose>
  <when test='not($supported_commands/group[@number=$group]/command[@number=$command]/permissions)'><apply-templates select="permissions/value" mode="perm" />
  </when>
  <otherwise>
  <apply-templates select="$supported_commands/group[@number=$group]/command[@number=$command]/permissions/value" mode="perm" />
  </otherwise>
  </choose>
};
</if>
</if>
</template>

<template match="group/command" mode="autharray">
<variable name="group" select="../@number" />
<variable name="command" select="@number" />
<if test='$target="lcm" or $supported_commands/group[@number=$group]/command[@number=$command]'>
<if test="contains(@source, 'ME') and not(contains(@source, 'PC')) and authentication">
CommandAuthentication_e AuthArray_<value-of select="concat(../interface[@type='loader']/@name, '_', ./interface[@type='loader']/@name)" />[]  = {

  <apply-templates select="authentication/value" mode="auth" />
};
</if>
<if test="contains(@source, 'PC') and authentication">
CommandAuthentication_e AuthArray_<value-of select="concat( ../interface[@type='loader']/@name, '_', ./interface[@type='loader']/@name)" />[] = {

  <apply-templates select="authentication/value" mode="auth" />
};
</if>
</if>
</template>


<template match="group/command" mode="staticarray">
<variable name="group" select="../@number" />
<variable name="command" select="@number" />
<if test='$target="lcm" or $supported_commands/group[@number=$group]/command[@number=$command]'>
<if test="contains(@source, 'PC') and not(contains(@source, 'ME'))">
<call-template name="staticarray">
  <with-param name="name" select="concat('Do_', ../interface[@type='loader']/@name, '_', ./interface[@type='loader']/@name, 'Impl')" />
  <with-param name="direction" select="input" />
</call-template>
</if>
<if test="contains(@source, 'ME')">
<call-template name="staticarray">
  <with-param name="name" select="concat('Done_', ../interface[@type='loader']/@name, '_', ./interface[@type='loader']/@name, 'Impl')" />
  <with-param name="direction" select="output" />
</call-template>
</if>
</if>
</template>
  
</stylesheet>
