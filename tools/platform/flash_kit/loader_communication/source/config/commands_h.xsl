<?xml version="1.0" encoding="utf-8"?>
<!--
* Copyright (C) ST-Ericsson SA 2011
* License terms: 3-clause BSD license
-->
<stylesheet version="2.0" xmlns="http://www.w3.org/1999/XSL/Transform">

<import href="common.xsl"/>

<output method="text" indent="no"/>
<strip-space elements="*"/>
<param name="target" />

<template match="/commandspec">/* $Copyright ST-Ericsson 2010$ */
/* NOTE: This is an automatically generated file. DO NOT EDIT! */

#ifndef _COMMANDS_H_
#define _COMMANDS_H_

#include &quot;LCDriver.h&quot;
#include &quot;LcmInterface.h&quot;
#include &quot;t_basicdefinitions.h&quot;
#include &quot;CmdResult.h&quot;

class CLCDriverMethods;

class LoaderRpcInterface
{
public:
  LoaderRpcInterface(CLCDriverMethods* lcdMethods, CmdResult* cmdResult, LcmInterface* lcmInterface):
    cmdResult_(cmdResult),
    lcmInterface_(lcmInterface),
    lcdMethods_(lcdMethods)
  {
  }

  virtual ~LoaderRpcInterface()
  {
  }
<apply-templates select="group"/>
protected:
  CmdResult* cmdResult_;
  LcmInterface* lcmInterface_;
  CLCDriverMethods* lcdMethods_;
};

#endif /* _COMMANDS_H_ */
</template>

<template match="group/command">
<if test="(contains(@source, &apos;ME&apos;)) and (@ADbg!='true')">
<call-template name="prototype">
  <with-param name="name" select="concat(&apos;DoneRPC_&apos;, ../interface[@type=&apos;loader&apos;]/@name, &apos;_&apos;, ./interface[@type=&apos;loader&apos;]/@name)"/>
  <with-param name="ref" select="concat(&apos;DoRPC_&apos;, ../interface[@type=&apos;loader&apos;]/@name, &apos;_&apos;, ./interface[@type=&apos;loader&apos;]/@name, &apos;Impl&apos;)"/>
  <with-param name="direction" select="input"/>
  <with-param name="source" select="&apos;ME&apos;"/>
</call-template>
</if>
<if test="(contains(@source, &apos;PC&apos;)) and (@ADbg!='true')">
<call-template name="prototype">
  <with-param name="name" select="concat(&apos;DoRPC_&apos;, ../interface[@type=&apos;loader&apos;]/@name, &apos;_&apos;, ./interface[@type=&apos;loader&apos;]/@name)"/>
  <with-param name="ref" select="concat(&apos;DoneRPC_&apos;, ../interface[@type=&apos;loader&apos;]/@name, &apos;_&apos;, ./interface[@type=&apos;loader&apos;]/@name, &apos;Impl&apos;)"/>
  <with-param name="direction" select="output"/>
  <with-param name="source" select="&apos;PC&apos;"/>
</call-template>
</if>
</template>
</stylesheet>
