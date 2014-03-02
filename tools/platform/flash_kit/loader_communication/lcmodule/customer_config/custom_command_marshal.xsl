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

<template match="group/documentation" mode="marshal">
/*
 * <value-of select="normalize-space(.)"/>
 */
</template>

<template match="group/documentation" mode="unmarshal">
        /*
         * <value-of select="normalize-space(.)"/>
         */
</template>

<template name="unmarshal">
<param name="name" />
<param name="direction" />
<choose>
  <when test="@flag='true'">
#ifdef CFG_ENABLE_ADBG_LOADER</when>
</choose>
<choose>
<when test="name($direction)='input'">
    /* Command <value-of select="../@name" /> / <value-of select="@name" /> (<value-of select="../@number" /> / <value-of select="@number" />) */ 
    case COMMAND(FALSE, <call-template name="groupid" ><with-param name="path" select=".."/></call-template>, <call-template name="commandid" />):
    { <apply-templates select="input/value" mode="deserialize" />
      Status = <value-of select="$name" />(<apply-templates select="input/value" mode="call" />);
      <apply-templates select="input/value" mode="clean" />
    }
    break;
</when>
<when test="name($direction)='output'">
    /* Response to <value-of select="../@name" /> / <value-of select="@name" /> (<value-of select="../@number" /> / <value-of select="@number" />) */ 
    case COMMAND(TRUE, <call-template name="groupid" ><with-param name="path" select=".."/></call-template>, <call-template name="commandid" />):
    { <apply-templates select="output/value" mode="deserialize" />
      Status = <value-of select="$name" />( PL_GRSize, ResponseStatus<if test="count(output/value) > 0">, </if><apply-templates select="output/value" mode="call"></apply-templates>);
      <apply-templates select="output/value" mode="clean" />
    }
    break;
</when>
</choose>
<choose>
  <when test="@flag='true'">#endif
  </when>
</choose>
</template>


<template match="/commandspec">
/* $Copyright$ */
/* NOTE: This is an automatically generated file. DO NOT EDIT! */
#include &lt;stdio.h&gt;
#include &lt;stdlib.h&gt;
#include &lt;string.h&gt;

#include "t_command_protocol.h"
#include "r_debug.h"
#include "r_communication_service.h"
#include "custom_commands.h"
#include "custom_command_ids.h"
#include "custom_commands_impl.h"
#include "error_codes.h"
#include "r_serialization.h"
#include "r_command_protocol.h"


#define CUSTOMCOMMAND(response, group, id) (((response) &lt;&lt; 31) | ((group) &lt;&lt; 16) | (id))
#define CUSTOMCOMMANDDATA(TypeP,ApplicationP,CommandP,SessionP,SizeP) \
  memset((uint8*)&amp;CmdData, 0x00, sizeof(CommandData_t)); \
  CmdData.Type             = TypeP; \
  CmdData.ApplicationNr    = ApplicationP; \
  CmdData.CommandNr        = CommandP; \
  CmdData.SessionNr        = SessionP; \
  CmdData.Payload.Size     = sizeof(ErrorCode_e)+SizeP; \
  CmdData.Payload.Data_p  = NULL; \
  if(SizeP !=0)\
  {\
    CmdData.Payload.Data_p = (uint8 *)malloc(sizeof(ErrorCode_e)+SizeP); \
    if(NULL == CmdData.Payload.Data_p) \
    { \
      A_(printf("command_marshal.c (%d): ** memory allocation failed! **\n",__LINE__);) \
      return  E_ALLOCATE_FAILED; \
    }\
  }

#define CUSTOMCOMMANDDATAOUT(TypeP,ApplicationP,CommandP,SizeP) \
  memset((uint8*)&amp;CmdData, 0x00, sizeof(CommandData_t)); \
  CmdData.Type             = TypeP; \
  CmdData.ApplicationNr    = ApplicationP; \
  CmdData.CommandNr        = CommandP; \
  CmdData.SessionNr        = 0; \
  CmdData.Payload.Size     = SizeP; \
  CmdData.Payload.Data_p  = NULL; \
  if(SizeP !=0)\
  {\
    CmdData.Payload.Data_p = (uint8 *)malloc( SizeP); \
    if(NULL == CmdData.Payload.Data_p) \
    { \
      A_(printf("command_marshal.c (%d): ** memory allocation failed! **\n",__LINE__);) \
      return  E_ALLOCATE_FAILED; \
    }\
  }

//static uint16 Session;

ErrorCode_e Do_CustomCEH_Call(CommandData_t *CmdData_p)
{
    ErrorCode_e Status = E_GENERAL_FATAL_ERROR;
    //  uint32 PL_GRSize = 0;
    //  ErrorCode_e ResponseStatus = E_SUCCESS;
    boolean response = FALSE;

    void *Data_p = CmdData_p-&gt;Payload.Data_p;
    (void)(Data_p);  //XVSZOAN Temporary solution!

    if (CmdData_p-&gt;Type == GENERAL_RESPONSE_PACKAGE) {

        response = TRUE;
        //    PL_GRSize = CmdData_p-&gt;Payload.Size;
        //    ResponseStatus = (ErrorCode_e)get_uint16(&amp;Data_p);
    }

    //  Session = CmdData_p-&gt;SessionNr;

    switch (CUSTOMCOMMAND(response, CmdData_p-&gt;ApplicationNr, CmdData_p-&gt;CommandNr)) {

<apply-templates select="group" mode="unmarshal"/>
    default:
        break;
    }

    return Status;
}

<apply-templates select="group" mode="marshal"/>

</template>

<template match="group/command" mode="marshal">
  <if test="contains(@source, 'ME')">
ErrorCode_e <value-of select="concat('Do_', ../interface[@type='loader']/@name, '_', ./interface[@type='loader']/@name)" />(uint32 PLSize<if test="count(output/value) > 0">, </if><apply-templates select="input/value" mode="declare" />)
{
  CommandData_t CmdData;
  void *Data_p;

  CUSTOMCOMMANDDATAOUT(COMMAND, <call-template name="groupidmain" />, <call-template name="commandid" />, PLSize);
  Data_p = CmdData.Payload.Data_p;
  <apply-templates select="input/value" mode="serialize" />
  return Do_R15_Command_Send(GlobalCommunication_p, &amp;CmdData);
}
</if>
  <if test="contains(@source, 'PC')">
ErrorCode_e <value-of select="concat('Done_', ../interface[@type='loader']/@name, '_', ./interface[@type='loader']/@name)" />(uint32 PLSize, ErrorCode_e Status<if test="count(output/value) > 0">, </if><apply-templates select="output/value" mode="declare"></apply-templates>)
{
  CommandData_t CmdData;
  void *Data_p;

  CUSTOMCOMMANDDATA( GENERAL_RESPONSE, <call-template name="groupidmain" />, <call-template name="commandid" />, Session, PLSize);
  Data_p = CmdData.Payload.Data_p;

  put_uint32(&amp;Data_p, Status);
  <apply-templates select="output/value" mode="serialize" />
  return Do_R15_Command_Send(GlobalCommunication_p, &amp;CmdData);
}
  </if>
</template>
  
<template match="group/command" mode="unmarshal">
	<if test="contains(@source, 'PC')">
		<call-template name="unmarshal">
			<with-param name="name" select="concat('Do_', ../interface[@type='loader']/@name, '_', ./interface[@type='loader']/@name, 'Impl')" />
			<with-param name="direction" select="input" />
		</call-template>
	</if>
  <if test="contains(@source, 'ME')">
    <call-template name="unmarshal">
      <with-param name="name" select="concat('Done_', ../interface[@type='loader']/@name, '_', ./interface[@type='loader']/@name, 'Impl')" />
      <with-param name="direction" select="output" />
    </call-template>
  </if>
</template>
  
</stylesheet>
