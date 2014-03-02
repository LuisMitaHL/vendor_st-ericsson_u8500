<?xml version="1.0" encoding="utf-8"?>
<!--
* Copyright (C) ST-Ericsson SA 2011
* License terms: 3-clause BSD license
-->
<stylesheet version="1.0" xmlns="http://www.w3.org/1999/XSL/Transform">

<import href="a2_common.xsl"/>

<output method="text"/>
<strip-space elements="*"/>
<param name="target"/>

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
 <param name="name"/>
 <param name="direction"/>

 <choose>
  <when test="name($direction)='output'">
    /* Command <value-of select="../@name"/> / <value-of select="@name"/> (<value-of select="../@number"/> / <value-of select="@number"/>) */ 
    case A2_COMMAND(FALSE, <call-template name="groupid"><with-param name="path" select=".."/></call-template>, <call-template name="commandid"/>):
    {
<apply-templates select="input/value" mode="deserialize_size" /><if test="count(input/value) > 0">      if (0 != PayloadSize)
      {
<apply-templates select="input/value" mode="deserialize" />      }
</if>      ReturnValue = <value-of select="$name" />(Session<apply-templates select="input/value" mode="call" />);<apply-templates select="input/value" mode="clean"/>
    }
    break;
  </when>
  <when test="name($direction)='input'">
    /* Response to <value-of select="../@name"/> / <value-of select="@name"/> (<value-of select="../@number"/> / <value-of select="@number"/>) */ 
    case A2_COMMAND(TRUE, <call-template name="groupid"><with-param name="path" select=".."/></call-template>, <call-template name="commandid"/>):
    {
<if test="../@number != 16">      ResponseStatus = (ErrorCode_e)Serialization::get_uint16_le(&amp;Data_p);
      PayloadSize -= 2;</if><text>
</text>
<apply-templates select="output/value" mode="deserialize_size" />
<if test="count(output/value) > 0"><if test="../@number != 16">      if (E_SUCCESS == ResponseStatus)</if>
      {
<apply-templates select="output/value" mode="deserialize" />      }</if>
      ReturnValue = <value-of select="$name" />(Session<if test="../@number != 16">, ResponseStatus</if><apply-templates select="output/value" mode="call"><with-param name="continue" select="'true'" /></apply-templates>);<apply-templates select="output/value" mode="clean"/>
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

#include "Serialization.h"
#include "a2_commands_impl.h"
#include "t_a2_protocol.h"
//#include "a2_custom_commands.h" //when customer commands will be implemented
#include "LCDriverMethods.h"

#define A2_COMMAND(response, group, id) ((((int)(response)) &lt;&lt; 30) | (((int)(group)) &lt;&lt; 16) | ((int)(id))) 
#define A2_COMMANDDATA(TypeP,ApplicationP,CommandP,SessionP,SizeP) \
  memset((uint8*)&amp;CmdData, 0x00, sizeof(A2_CommandData_t)); \
  CmdData.Type             = TypeP; \
  CmdData.ApplicationNr    = ApplicationP; \
  CmdData.CommandNr        = CommandP; \
  CmdData.SessionNr        = SessionP; \
  CmdData.Payload.Size     = SizeP; \
  CmdData.Payload.Data_p   = NULL; \
  if(SizeP != 0)\
  { \
    CmdData.Payload.Data_p = (uint8*)malloc(sizeof(ErrorCode_e) + SizeP); \
    if(NULL == CmdData.Payload.Data_p) \
    { \
      return E_ALLOCATE_FAILED; \
    }\
  }

#define A2_COMMANDDATAOUT(TypeP,ApplicationP,CommandP,SessionP,SizeP,TargetCPU) \
  memset((uint8*)&amp;CmdData, 0x00, sizeof(A2_CommandData_t)); \
  CmdData.Type                    = TypeP; \
  CmdData.ApplicationNr           = ApplicationP; \
  CmdData.CommandNr               = CommandP; \
  CmdData.SessionNr               = SessionP; \
  CmdData.Payload.Size            = SizeP; \
  CmdData.Payload.Data_p          = NULL; \
  CmdData.DestAddress             = TargetCPU; \
  if(SizeP != 0) \
  { \
    CmdData.Payload.Data_p = (uint8*)malloc(SizeP); \
    if(NULL == CmdData.Payload.Data_p) \
    { \
      return E_ALLOCATE_FAILED; \
    } \
  }

ErrorCode_e A2LoaderRpcInterfaceImpl::Do_CEH_Callback(CommandData_t* pCmdData)
{
  A2_CommandData_t* CmdData_p = (A2_CommandData_t*)pCmdData;
  ErrorCode_e ReturnValue = E_GENERAL_FATAL_ERROR;
  ErrorCode_e ResponseStatus = E_GENERAL_FATAL_ERROR;
  boolean response = FALSE;
  const void *Data_p = CmdData_p-&gt;Payload.Data_p;
  uint32 PayloadSize = CmdData_p-&gt;Payload.Size;
  uint16 Session = CmdData_p-&gt;SessionNr;

  if (A2_GENERAL_RESPONSE == CmdData_p-&gt;Type)
  {
    // skip fields already handled by LCM: command group, 0xFF, more packets and original command
    Data_p = ((uint8*)Data_p) + 4;
    PayloadSize -= 4;
    response = TRUE;
  }
  else if (A2_COMMAND == CmdData_p-&gt;Type)
  {
    // skip fields already handled by LCM: command group, command and more packets fields
    Data_p = ((uint8*)Data_p) + 3;
    PayloadSize -= 3;
    response = FALSE;
  }
  else if (A2_CONTROL_MESSAGE == CmdData_p-&gt;Type)
  {
    // skip control message ID
    Data_p = ((uint8*)Data_p) + 1;
    PayloadSize -= 1;
    response = FALSE;
  }
  else if (A2_SPEEDFLASH_GR == CmdData_p->Type)
  {
    uint16 Status = Serialization::get_uint16_le(&amp;Data_p);
    lcdMethods_-&gt;AddEvent(new Event(EVENT_SPEEDFLASH, Status));
    return E_SUCCESS;
  }
  else
  {
    return E_GENERAL_FATAL_ERROR;
  }

  switch(A2_COMMAND(response, CmdData_p-&gt;ApplicationNr, CmdData_p-&gt;CommandNr))
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
<if test="contains(@source, &apos;PC&apos;)">
ErrorCode_e <value-of select="concat('A2LoaderRpcInterface::DoRPC_', ../interface[@type='loader']/@name, '_', ./interface[@type='loader']/@name)" />(uint16 SessionOut<apply-templates select="input/value" mode="declare" />)
{
  ErrorCode_e Answer;
  A2_CommandData_t CmdData;
  void *Data_p;
  uint32 PLSize = 3;
<apply-templates select="input/value" mode="serialize_size"/>
  A2_COMMANDDATAOUT(A2_COMMAND, <call-template name="groupidmain"/>, <call-template name="commandid"/>, SessionOut, PLSize, targetCpu_);
  Data_p = CmdData.Payload.Data_p;

  // set command group and command
  Serialization::put_uint8(&amp;Data_p, <call-template name="groupidmain"/>);
  Serialization::put_uint8(&amp;Data_p, <call-template name="commandid"/>);
  // set more packets
  Serialization::put_uint8(&amp;Data_p, morePackets_);

<apply-templates select="input/value" mode="serialize"/>
  Answer = lcmInterface_->A2CommandSend(&amp;CmdData);

  if(NULL != CmdData.Payload.Data_p)
    free(CmdData.Payload.Data_p);
  return Answer;
}
</if>
<if test="contains(@source, &apos;ME&apos;)">
ErrorCode_e <value-of select="concat('A2LoaderRpcInterface::DoneRPC_', ../interface[@type='loader']/@name, '_', ./interface[@type='loader']/@name)" />(uint16 Session<apply-templates select="output/value" mode="declare"><with-param name="continue" select="'true'" /></apply-templates>)
{
  ErrorCode_e Answer;
  A2_CommandData_t CmdData;
  void *Data_p;
  uint32 PLSize = 0;
<apply-templates select="output/value" mode="serialize_size_declaration" />
<apply-templates select="output/value" mode="serialize_size"/>
  A2_COMMANDDATA(A2_GENERAL_RESPONSE, <call-template name="groupidmain"/>, <call-template name="commandid"/>, Session, PLSize);
  (void)(Data_p);
  Data_p = CmdData.Payload.Data_p;

<apply-templates select="output/value" mode="serialize"/>
  Answer = lcmInterface_->A2CommandSend(&amp;CmdData);

  if(NULL != CmdData.Payload.Data_p)
    free(CmdData.Payload.Data_p);
  return Answer;
}
</if>
</template>

<template match="group/command" mode="unmarshal">
  <if test="contains(@source, &apos;PC&apos;)">
    <call-template name="unmarshal">
      <with-param name="name" select="concat('DoneRPC_', ../interface[@type='loader']/@name, '_', ./interface[@type='loader']/@name, 'Impl')" />
      <with-param name="direction" select="input"/>
    </call-template>
  </if>
  <if test="contains(@source, &apos;ME&apos;)">
    <call-template name="unmarshal">
      <with-param name="name" select="concat('DoRPC_', ../interface[@type='loader']/@name, '_', ./interface[@type='loader']/@name, 'Impl')" />
      <with-param name="direction" select="output"/>
    </call-template>
  </if>
</template>

</stylesheet>
