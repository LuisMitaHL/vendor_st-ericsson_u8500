<?xml version="1.0" encoding="utf-8"?>
<!--
* Copyright (C) ST-Ericsson SA 2011
* License terms: 3-clause BSD license
-->
<stylesheet version="1.0" xmlns="http://www.w3.org/1999/XSL/Transform">

<import href="common.xsl"/>

<output method="text"/>
<strip-space elements="*"/>
<param name="target"/>

<template match="group/documentation" mode="marshal">
<if test="../interface/@name != 'ADbg'">
/*
 * <value-of select="normalize-space(.)"/>
 */
</if>
</template>

<template match="group/documentation" mode="unmarshal">
<if test="../interface/@name != 'ADbg'">
  /*
   * <value-of select="normalize-space(.)"/>
   */
</if>
</template>

<template name="unmarshal">
 <param name="name"/>
 <param name="direction"/>

 <choose>
  <when test="name($direction)='output'">
    /* Command <value-of select="../@name"/> / <value-of select="@name"/> (<value-of select="../@number"/> / <value-of select="@number"/>) */ 
    case COMMAND(FALSE, <call-template name="groupid"><with-param name="path" select=".."/></call-template>, <call-template name="commandid"/>):
    {
<apply-templates select="input/value" mode="deserialize_size" /><apply-templates select="input/value" mode="deserialize" />      ReturnValue = <value-of select="$name" />(Session<apply-templates select="input/value" mode="call" />);<apply-templates select="input/value" mode="clean"/>
    }
    break;
  </when>
  <when test="name($direction)='input'">
    /* Response to <value-of select="../@name"/> / <value-of select="@name"/> (<value-of select="../@number"/> / <value-of select="@number"/>) */ 
    case COMMAND(TRUE, <call-template name="groupid"><with-param name="path" select=".."/></call-template>, <call-template name="commandid"/>):
    {
<apply-templates select="output/value" mode="deserialize_size" />      ResponseStatus = (ErrorCode_e)Serialization::get_uint32_le(&amp;Data_p);<if test="count(output/value) > 0">
      if (E_SUCCESS == ResponseStatus)
      {
<apply-templates select="output/value" mode="deserialize" />      }</if>
      ReturnValue = <value-of select="$name" />(Session, ResponseStatus<apply-templates select="output/value" mode="call"><with-param name="continue" select="'true'" /></apply-templates>);<apply-templates select="output/value" mode="clean"/>
    }
    break;
  </when>
 </choose>
</template>

<template match="/commandspec">/* $Copyright ST-Ericsson 2010$ */
/* NOTE: This is an automatically generated file. DO NOT EDIT! */
#include &lt;stdio.h&gt;
#include &lt;stdlib.h&gt;
#include &lt;string.h&gt;

#include "t_command_protocol.h"
#include "commands.h"
//#include "custom_commands.h" // when customer commands will be implemented
#include "command_ids.h"
#include "commands_impl.h"
#include "error_codes.h"
#include "Serialization.h"
#include "t_r15_network_layer.h"
#include "Event.h"
#include "LCDriverMethods.h"

#define COMMAND(response, group, id) ((((int)(response)) &lt;&lt; 30) | (((int)(group)) &lt;&lt; 16) | ((int)(id))) 
#define COMMANDDATA(TypeP,ApplicationP,CommandP,SessionP,SizeP) \
  memset((uint8*)&amp;CmdData, 0x00, sizeof(CommandData_t)); \
  CmdData.Type             = TypeP; \
  CmdData.ApplicationNr    = ApplicationP; \
  CmdData.CommandNr        = CommandP; \
  CmdData.SessionNr        = SessionP; \
  CmdData.Payload.Size     = SizeP; \
  CmdData.Payload.Data_p   = NULL; \
  if(SizeP != 0)\
  {\
    CmdData.Payload.Data_p = (uint8 *)malloc(sizeof(ErrorCode_e)+SizeP); \
    if(NULL == CmdData.Payload.Data_p) \
    { \
      return E_ALLOCATE_FAILED; \
    }\
  }

#define COMMANDDATAOUT(TypeP,ApplicationP,CommandP,SessionP,SizeP) \
  memset((uint8*)&amp;CmdData, 0x00, sizeof(CommandData_t)); \
  CmdData.Type                    = TypeP; \
  CmdData.ApplicationNr           = ApplicationP; \
  CmdData.CommandNr               = CommandP; \
  CmdData.SessionNr               = SessionP; \
  CmdData.Payload.Size            = SizeP; \
  CmdData.Payload.Data_p          = NULL; \
  if(SizeP != 0)\
  {\
    CmdData.Payload.Data_p = (uint8 *)malloc( SizeP); \
    if(NULL == CmdData.Payload.Data_p) \
    { \
      return E_ALLOCATE_FAILED; \
    }\
  }

  ErrorCode_e LoaderRpcInterfaceImpl::Do_CEH_Callback(CommandData_t *CmdData_p)
  {
    ErrorCode_e ReturnValue = E_GENERAL_FATAL_ERROR;
    ErrorCode_e ResponseStatus = E_GENERAL_FATAL_ERROR;
    boolean response = FALSE;
    const void *Data_p = CmdData_p-&gt;Payload.Data_p;
    uint16 Session = CmdData_p-&gt;SessionNr;
  
    if (CmdData_p-&gt;Type == GENERAL_RESPONSE_PACKAGE)
    {
      response = TRUE;
    }
    else
    {
      response = FALSE;
    }

    Session = CmdData_p-&gt;SessionNr;

    switch(COMMAND(response, CmdData_p-&gt;ApplicationNr, CmdData_p-&gt;CommandNr))
    {
<apply-templates select="group" mode="unmarshal"/>
    default:
      {
        return E_COMMAND_NO_ERROR; // Do_CustomCEH_Call(CmdData_p); // when customer commands will be implemented
      }
    }

    if (response)
    {
      lcdMethods_-&gt;AddEvent(new Event(EVENT_GR_RECEIVED, ResponseStatus, CmdData_p-&gt;ApplicationNr, CmdData_p-&gt;CommandNr));
    }
    else
    {
      lcdMethods_-&gt;AddEvent(new Event(EVENT_CMD_RECEIVED, 0, CmdData_p-&gt;ApplicationNr, CmdData_p-&gt;CommandNr));
    }

    return ReturnValue;
  }
<apply-templates select="group" mode="marshal"/>
</template>

<template match="group/command" mode="marshal">
<if test="(contains(@source, &apos;PC&apos;)) and (@ADbg!='true')">
ErrorCode_e <value-of select="concat('LoaderRpcInterface::DoRPC_', ../interface[@type='loader']/@name, '_', ./interface[@type='loader']/@name)" />(uint16 SessionOut<apply-templates select="input/value" mode="declare" />)
{
  ErrorCode_e Answer;
  CommandData_t CmdData;
  void *Data_p;
  uint32 PLSize = 0;
<apply-templates select="input/value" mode="serialize_size_declaration" />
<apply-templates select="input/value" mode="serialize_size"/>
  COMMANDDATAOUT(COMMAND_TYPE, <call-template name="groupidmain"/>, <call-template name="commandid"/>, SessionOut, PLSize);
  (void)(Data_p);
  Data_p = CmdData.Payload.Data_p;
<apply-templates select="input/value" mode="serialize"/>
  Answer = lcmInterface_->CommandSend(&amp;CmdData);

  if(NULL != CmdData.Payload.Data_p)
    free(CmdData.Payload.Data_p);
  return Answer;    
}
</if>
<if test="(contains(@source, &apos;ME&apos;)) and (@ADbg!='true')">
ErrorCode_e <value-of select="concat('LoaderRpcInterface::DoneRPC_', ../interface[@type='loader']/@name, '_', ./interface[@type='loader']/@name)" />(uint16 Session, ErrorCode_e Status<apply-templates select="output/value" mode="declare"><with-param name="continue" select="'true'" /></apply-templates>)
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
  
  Serialization::put_uint32_le(&amp;Data_p, Status);
<apply-templates select="output/value" mode="serialize"/>
  Answer = lcmInterface_->CommandSend(&amp;CmdData);

  if(NULL != CmdData.Payload.Data_p)
    free(CmdData.Payload.Data_p);
  return Answer;    
}
</if>
</template>
  
<template match="group/command" mode="unmarshal">
  <if test="(contains(@source, &apos;PC&apos;)) and (@ADbg!='true')">
    <call-template name="unmarshal">
      <with-param name="name" select="concat('DoneRPC_', ../interface[@type='loader']/@name, '_', ./interface[@type='loader']/@name, 'Impl')" />
      <with-param name="direction" select="input"/>
    </call-template>
  </if>
  <if test="(contains(@source, &apos;ME&apos;)) and (@ADbg!='true')">
    <call-template name="unmarshal">
      <with-param name="name" select="concat('DoRPC_', ../interface[@type='loader']/@name, '_', ./interface[@type='loader']/@name, 'Impl')" />
      <with-param name="direction" select="output"/>
    </call-template>
  </if>
</template>
  
</stylesheet>
