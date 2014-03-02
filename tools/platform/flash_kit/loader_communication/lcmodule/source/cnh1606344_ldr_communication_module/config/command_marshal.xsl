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

<template match="group/documentation" mode="marshal">
<variable name="group" select="../@number" />
<if test='$target="lcm" or $supported_commands/group[@number=$group]'>
/*
 * <value-of select="normalize-space(.)"/>
 */
</if>
</template>

<template match="group/documentation" mode="unmarshal">
<variable name="group" select="../@number" />
<if test='$target="lcm" or $supported_commands/group[@number=$group]'>
        /*
         * <value-of select="normalize-space(.)"/>
         */
</if>
</template>

<template name="unmarshal">
<param name="name"/>
<param name="direction"/>

<choose>
  <when test="@flag='true'">
#ifdef CFG_ENABLE_ADBG_LOADER</when>
</choose>
<choose>
  <when test="@flag='true1'">
#ifdef CFG_ENABLE_AUDIT_CMD</when>
</choose>
<choose>
  <when test="@flagbitsset='trueset'">
#ifndef CFG_ENABLE_WRITEBITS_CMD</when>
</choose>
<choose>
  <when test="@flagwritebits='truewrite'">
#ifdef CFG_ENABLE_WRITEBITS_CMD</when>
</choose>
<choose>
<when test="name($direction)='input'">
    /* Command <value-of select="../@name"/> / <value-of select="@name"/> (<value-of select="../@number"/> / <value-of select="@number"/>) */
    case COMMAND(FALSE, <call-template name="groupid"><with-param name="path" select=".."/></call-template>, <call-template name="commandid"/>): {
<apply-templates select="input/value" mode="deserialize_size" />
<apply-templates select="input/value" mode="deserialize" />
        Status = <value-of select="$name" />(Session<if test="count(input/value) > 0">, </if><apply-templates select="input/value" mode="call" />);
<apply-templates select="input/value" mode="clean"/>
    }
    break;
</when>
<when test="name($direction)='output'">
        /* Response to <value-of select="../@name"/> / <value-of select="@name"/> (<value-of select="../@number"/> / <value-of select="@number"/>) */
    case COMMAND(TRUE, <call-template name="groupid"><with-param name="path" select=".."/></call-template>, <call-template name="commandid"/>): {

  <apply-templates select="output/value" mode="deserialize_size" />
        ResponseStatus = (ErrorCode_e)get_uint32_le(&amp;Data_p);
<apply-templates select="output/value" mode="deserialize" />
        Status = <value-of select="$name" />(Session, ResponseStatus<if test="count(output/value) > 0">, </if><apply-templates select="output/value" mode="call"></apply-templates>);
<apply-templates select="output/value" mode="clean"/>
    }
    break;
</when>
</choose>

<choose>
  <when test="@flagbitsset='trueset'">#endif
  </when>
</choose>
<choose>
  <when test="@flagwritebits='truewrite'">#endif
  </when>
</choose>
<choose>
  <when test="@flag='true1'">#endif
  </when>
</choose>
<choose>
  <when test="@flag='true'">#endif
  </when>
</choose>
</template>


<template match="/commandspec">/* $Copyright$ */
/* NOTE: This is an automatically generated file. DO NOT EDIT! */
#include &lt;stdio.h&gt;
#include &lt;stdlib.h&gt;
#include &lt;string.h&gt;

#include "t_command_protocol.h"
#include "r_debug.h"
#include "r_communication_service.h"
#include "commands.h"
#include "custom_commands.h"
#include "command_ids.h"
#include "commands_impl.h"
#include "error_codes.h"
#include "r_serialization.h"
#include "r_command_protocol.h"
#ifdef CFG_ENABLE_ADBG_LOADER
#include "r_adbg_applications.h"
#endif

#ifdef CFG_ENABLE_AUDIT_CMD
#include "audit.h"
#endif

#ifdef CFG_ENABLE_ADBG_LOADER
#include "r_adbg_command.h"
#endif

#define COMMAND(response, group, id) ((((int)(response)) &lt;&lt; 30) | (((int)(group)) &lt;&lt; 16) | ((int)(id)))
#define COMMANDDATA(TypeP,ApplicationP,CommandP,SessionP,SizeP)\
  memset((uint8*)&amp;CmdData, 0x00, sizeof(CommandData_t));\
  CmdData.Type           = TypeP;\
  CmdData.ApplicationNr  = ApplicationP;\
  CmdData.CommandNr      = CommandP;\
  CmdData.SessionNr      = SessionP;\
  CmdData.Payload.Size   = SizeP;\
  CmdData.Payload.Data_p = NULL;\
  CmdData.Payload.Data_p = (uint8*)malloc(SizeP);\
  if (NULL == CmdData.Payload.Data_p)\
  {\
    A_(printf("command_marshal.c (%d): ** memory allocation failed! **\n",__LINE__);)\
    return E_ALLOCATE_FAILED;\
  }\

#define COMMANDDATAOUT(TypeP,ApplicationP,CommandP,SizeP)\
  memset((uint8*)&amp;CmdData, 0x00, sizeof(CommandData_t));\
  CmdData.Type           = TypeP;\
  CmdData.ApplicationNr  = ApplicationP;\
  CmdData.CommandNr      = CommandP;\
  CmdData.SessionNr      = 0;\
  CmdData.Payload.Size   = SizeP;\
  CmdData.Payload.Data_p = NULL;\
  if (0 != SizeP)\
  {\
    CmdData.Payload.Data_p = (uint8 *)malloc( SizeP);\
    if (NULL == CmdData.Payload.Data_p)\
    {\
      A_(printf("command_marshal.c (%d): ** memory allocation failed! **\n",__LINE__);)\
      return E_ALLOCATE_FAILED;\
    }\
  }

//static uint16 Session;

void disable_interrupt(void)
{
#ifndef WIN32
    /* todo implement this */
    //#warning This is disable_interrupt
#endif
}
void enable_interrupt(void)
{
#ifndef WIN32
    /* todo implement this */
    //#warning This is enable_interrupt
#endif
}

ErrorCode_e Do_CEH_Call(void *Object_p, CommandData_t *CmdData_p)
{
    ErrorCode_e Status = E_GENERAL_FATAL_ERROR;
    //  uint32 PL_GRSize = 0;
    ErrorCode_e ResponseStatus = E_GENERAL_FATAL_ERROR;
    boolean response = FALSE;
    //  CommandPermissionList_t* CmdPermission_p;
    void *Data_p = CmdData_p-&gt;Payload.Data_p;
    uint16 Session = CmdData_p-&gt;SessionNr;

    if (CmdData_p-&gt;Type == GENERAL_RESPONSE_PACKAGE) {

        response = TRUE;
        //    PL_GRSize = CmdData_p-&gt;Payload.Size;
        //    ResponseStatus = (ErrorCode_e)get_uint16(&amp;Data_p);
    }

#ifdef CFG_ENABLE_AUDIT_CMD

    if (COMMAND_TYPE == CmdData_p-&gt;Type) {
        ErrorCode_e AuditResponse = CommandAudit(CmdData_p);
        CommandData_t CmdData = {0};
        uint32 PLSize = sizeof(ErrorCode_e);
        uint8 AuditResponseData[PLSize];

        if (E_SUCCESS != AuditResponse) {

            memset((uint8 *)&amp;CmdData, 0x00, sizeof(CommandData_t));

            CmdData.Type           = GENERAL_RESPONSE;
            CmdData.CommandNr      = CmdData_p-&gt;CommandNr;
            CmdData.ApplicationNr  = CmdData_p-&gt;ApplicationNr;
            CmdData.SessionNr      = CmdData_p-&gt;SessionNr;
            CmdData.Payload.Size   = PLSize;
            CmdData.Payload.Data_p = AuditResponseData;

            Data_p = CmdData.Payload.Data_p;
            put_uint32(&amp;Data_p, AuditResponse);

            Status = Do_R15_Command_Send(GlobalCommunication_p, &amp;CmdData);

            if (E_SUCCESS != Status) {
                return Status;
            }

            if (E_UNSUPPORTED_CMD == AuditResponse) {
                Status = Do_System_ShutDownImpl(CmdData.SessionNr);
            }

            return Status;
        }
    }

#endif

    switch (COMMAND(response, CmdData_p-&gt;ApplicationNr, CmdData_p-&gt;CommandNr)) {

<apply-templates select="group" mode="unmarshal"/>
    default: {

        Status = Do_CustomCEH_Call(CmdData_p);

        if (E_SUCCESS != Status) {
            Status = Do_System_ShutDownImpl(Session);
        }
    }
    break;
    }

    return Status;
}

<apply-templates select="group" mode="marshal"/>

</template>

<template match="group/command" mode="marshal">
<variable name="group" select="../@number" />
<variable name="command" select="@number" />
<if test='$target="lcm" or $supported_commands/group[@number=$group]/command[@number=$command]'>
<choose>
  <when test="@flag='true'">
#ifdef CFG_ENABLE_ADBG_LOADER</when>
</choose>
  <if test="contains(@source, 'ME')">
ErrorCode_e <value-of select="concat('Do_', ../interface[@type='loader']/@name, '_', ./interface[@type='loader']/@name)" />(<choose><when test="count(input/value) > 0"><apply-templates select="input/value" mode="declare" /></when><otherwise>void</otherwise></choose>)
{
    ErrorCode_e Answer;
    CommandData_t CmdData;
    void *Data_p;
    uint32 PLSize = 0;
    <apply-templates select="input/value" mode="serialize_size_declaration" />
    <apply-templates select="input/value" mode="serialize_size"/>
    COMMANDDATAOUT(COMMAND_TYPE, <call-template name="groupidmain"/>, <call-template name="commandid"/>, PLSize);
    Data_p = CmdData.Payload.Data_p;
    (void)(Data_p);  //XVSZOAN Temporary solution!
    <apply-templates select="input/value" mode="serialize"/>
    Answer = Do_R15_Command_Send(GlobalCommunication_p, &amp;CmdData);
    if (NULL != CmdData.Payload.Data_p) {
        free(CmdData.Payload.Data_p);
    }

    return Answer;
}
</if>
  <if test="contains(@source, 'PC')">
ErrorCode_e <value-of select="concat('Done_', ../interface[@type='loader']/@name, '_', ./interface[@type='loader']/@name)" />(uint16 Session, ErrorCode_e Status<if test="count(output/value) > 0">, </if><apply-templates select="output/value" mode="declare"></apply-templates>)
{
    ErrorCode_e Answer;
    CommandData_t CmdData;
    void *Data_p;
    uint32 PLSize = 0;
    <apply-templates select="output/value" mode="serialize_size_declaration" />
    PLSize += sizeof(ErrorCode_e);
    <apply-templates select="output/value" mode="serialize_size"/>
    COMMANDDATA(GENERAL_RESPONSE, <call-template name="groupidmain"/>, <call-template name="commandid"/>, Session, PLSize);
    Data_p = CmdData.Payload.Data_p;

    put_uint32(&amp;Data_p, Status);
    <apply-templates select="output/value" mode="serialize"/>
    <choose>
    <when test="@ADbg!='true'">
#ifdef CFG_ENABLE_ADBG_LOADER
    Answer = ReleaseADbg_AppFunc(Status);</when>
    </choose>
    Answer = E_SUCCESS;
    <choose>
    <when test="@ADbg!='true'">
#else</when>
    </choose>
    Answer = Do_R15_Command_Send(GlobalCommunication_p, &amp;CmdData);
    <choose>
    <when test="@ADbg!='true'">
#endif</when>
    </choose>
    if (NULL != CmdData.Payload.Data_p) {
        free(CmdData.Payload.Data_p);
    }

    return Answer;
}
  </if>
<choose>
<when test="@flag='true'">#endif
</when>
</choose>
</if>
</template>
  
<template match="group/command" mode="unmarshal">
<variable name="group" select="../@number" />
<variable name="command" select="@number" />
<if test='$target="lcm" or $supported_commands/group[@number=$group]/command[@number=$command]'>
  <if test="contains(@source, 'PC')">
    <call-template name="unmarshal">
      <with-param name="name" select="concat('Do_', ../interface[@type='loader']/@name, '_', ./interface[@type='loader']/@name, 'Impl')" />
      <with-param name="direction" select="input"/>
    </call-template>
  </if>
  <if test="contains(@source, 'ME')">
    <call-template name="unmarshal">
      <with-param name="name" select="concat('Done_', ../interface[@type='loader']/@name, '_', ./interface[@type='loader']/@name, 'Impl')" />
      <with-param name="direction" select="output"/>
    </call-template>
  </if>
</if>
</template>
  
</stylesheet>
