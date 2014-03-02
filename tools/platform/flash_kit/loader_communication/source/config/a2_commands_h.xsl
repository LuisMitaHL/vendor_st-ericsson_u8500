<?xml version="1.0" encoding="utf-8"?>
<!--
* Copyright (C) ST-Ericsson SA 2011
* License terms: 3-clause BSD license
-->
<stylesheet version="2.0" xmlns="http://www.w3.org/1999/XSL/Transform">

<import href="a2_common.xsl"/>

<output method="text" indent="no"/>
<strip-space elements="*"/>
<param name="target" />

<template match="/commandspec">/* $Copyright ST-Ericsson 2010$ */
/* NOTE: This is an automatically generated file. DO NOT EDIT! */

#ifndef _A2_COMMANDS_H_
#define _A2_COMMANDS_H_

#include &quot;LCDriver.h&quot;
#include &quot;LcmInterface.h&quot;
#include &quot;t_basicdefinitions.h&quot;
#include &quot;CmdResult.h&quot;
//#include &quot;error_codes.h&quot;
//#include &quot;t_a2_protocol.h&quot;
//#include "a2_command_ids.h"

class CLCDriverMethods;

class A2LoaderRpcInterface
{
public:
  A2LoaderRpcInterface(CLCDriverMethods* lcdMethods, CmdResult* cmdResult, LcmInterface* lcmInterface):
    lcdMethods_(lcdMethods),
    cmdResult_(cmdResult),
    lcmInterface_(lcmInterface),
    targetCpu_(0),
    morePackets_(0)
  {
  }

  virtual ~A2LoaderRpcInterface()
  {
  }
<apply-templates select="group"/>
public:
  void setTargetCpu(uint8 targetCpu) { targetCpu_ = targetCpu; }
  void setMorePackets(uint8 morePackets) { morePackets_ = morePackets; }
protected:
  CLCDriverMethods* lcdMethods_;
  CmdResult* cmdResult_;
  LcmInterface* lcmInterface_;
  uint8 targetCpu_;
  uint8 morePackets_;
};

#endif /* _A2_COMMANDS_H_ */
</template>

<template match="group/command">
<if test="contains(@source, &apos;ME&apos;)">
<call-template name="prototype">
  <with-param name="name" select="concat(&apos;DoneRPC_&apos;, ../interface[@type=&apos;loader&apos;]/@name, &apos;_&apos;, ./interface[@type=&apos;loader&apos;]/@name)"/>
  <with-param name="ref" select="concat(&apos;DoRPC_&apos;, ../interface[@type=&apos;loader&apos;]/@name, &apos;_&apos;, ./interface[@type=&apos;loader&apos;]/@name, &apos;Impl&apos;)"/>
  <with-param name="direction" select="input"/>
  <with-param name="source" select="&apos;ME&apos;"/>
</call-template>
</if>
<if test="contains(@source, &apos;PC&apos;)">
<call-template name="prototype">
  <with-param name="name" select="concat(&apos;DoRPC_&apos;, ../interface[@type=&apos;loader&apos;]/@name, &apos;_&apos;, ./interface[@type=&apos;loader&apos;]/@name)"/>
  <with-param name="ref" select="concat(&apos;DoneRPC_&apos;, ../interface[@type=&apos;loader&apos;]/@name, &apos;_&apos;, ./interface[@type=&apos;loader&apos;]/@name, &apos;Impl&apos;)"/>
  <with-param name="direction" select="output"/>
  <with-param name="source" select="&apos;PC&apos;"/>
</call-template>
</if>
</template>
</stylesheet>
