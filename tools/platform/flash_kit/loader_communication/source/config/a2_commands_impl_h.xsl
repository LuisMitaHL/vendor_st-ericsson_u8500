<?xml version="1.0" encoding="utf-8"?>
<!--
* Copyright (C) ST-Ericsson SA 2011
* License terms: 3-clause BSD license
-->
<stylesheet version="1.0" xmlns="http://www.w3.org/1999/XSL/Transform" >

<import href="a2_common.xsl"/>

<output method="text"/>
<strip-space elements="*"/>
<param name="target" />

<template match="/commandspec">/* $Copyright ST-Ericsson 2010$ */
/* NOTE: This is an automatically generated file. DO NOT EDIT! */

#ifndef _A2_COMMANDS_IMPL_H_
#define _A2_COMMANDS_IMPL_H_

#include &quot;a2_commands.h&quot;
#include &quot;a2_command_ids.h&quot;
#include &quot;LCDriver.h&quot;
#include &quot;LcmInterface.h&quot;
#include &quot;error_codes.h&quot;

class CLCDriverMethods;

class A2LoaderRpcInterfaceImpl : public A2LoaderRpcInterface 
{
public:
	A2LoaderRpcInterfaceImpl(CLCDriverMethods* lcdMethods, CmdResult* cmdResult, LcmInterface* lcmInterface):
    A2LoaderRpcInterface(lcdMethods, cmdResult, lcmInterface)
  {
  }

	~A2LoaderRpcInterfaceImpl()
  {
  }

	ErrorCode_e	Do_CEH_Callback(CommandData_t* pCmdData);
<apply-templates select="group" />
};

#endif /* _A2_COMMANDS_IMPL_H_ */
</template>

<template match="group/documentation">
  /*
   * <value-of select="normalize-space(.)"/>
   */
</template>

<template match="group/command">
  <if test="contains(@source, &apos;PC&apos;)">
    <call-template name="prototype">
      <with-param name="name" select="concat(&apos;DoneRPC_&apos;, ../interface[@type=&apos;loader&apos;]/@name, &apos;_&apos;, ./interface[@type=&apos;loader&apos;]/@name, &apos;Impl&apos;)" />
			<with-param name="ref" select="concat(&apos;DoRPC_&apos;, ../interface[@type=&apos;loader&apos;]/@name, &apos;_&apos;, ./interface[@type=&apos;loader&apos;]/@name)" />
			<with-param name="direction" select="input" />
			<with-param name="source" select="&apos;PC&apos;" />
		</call-template>
	</if>
  <if test="contains(@source, &apos;ME&apos;)">
    <call-template name="prototype">
      <with-param name="name" select="concat(&apos;DoRPC_&apos;, ../interface[@type=&apos;loader&apos;]/@name, &apos;_&apos;, ./interface[@type=&apos;loader&apos;]/@name, &apos;Impl&apos;)" />
      <with-param name="ref" select="concat(&apos;DoneRPC_&apos;, ../interface[@type=&apos;loader&apos;]/@name, &apos;_&apos;, ./interface[@type=&apos;loader&apos;]/@name)" />
      <with-param name="direction" select="output" />
      <with-param name="source" select="&apos;ME&apos;" />
    </call-template>
  </if>
</template>

</stylesheet>
