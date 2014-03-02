<?xml version="1.0" encoding="UTF-8"?>
<!--
* Copyright (C) ST-Ericsson SA 2011
* License terms: 3-clause BSD license
-->
<stylesheet version="1.0" xmlns="http://www.w3.org/1999/XSL/Transform" >

<!-- Ugly name-mangling templates. upper-case() did not work in this
     implementation -->
<template name="groupidmain">
  <param name="path" select="../." />
  <value-of select="concat('GROUP_', translate($path/interface[@type='loader']/@name,'abcdefghijklmnopqrstuvwxyz','ABCDEFGHIJKLMNOPQRSTUVWXYZ'))" />
</template>

  
<template name="groupid">
  <param name="path" select="." />
  <value-of select="concat('GROUP_', translate($path/interface[@type='loader']/@name,'abcdefghijklmnopqrstuvwxyz','ABCDEFGHIJKLMNOPQRSTUVWXYZ'))" />
</template>

<template name="commandid">
  <param name="path" select="." />
  <value-of select="translate(concat('COMMAND_', $path/../interface[@type='loader']/@name, '_', $path/interface[@type='loader']/@name),'abcdefghijklmnopqrstuvwxyz','ABCDEFGHIJKLMNOPQRSTUVWXYZ')" />
</template>



<template match="value" mode="deserialize">
<choose>
  <when test="@type='uint32'">
      uint32 <value-of select="@name" /> = get_uint32(&amp;Data_p);</when>
  <when test="@type='string'">
      const char *<value-of select="@name" /> = get_strdup(&amp;Data_p);</when>
  <when test="@type='buffer' and @length='*'">
      int <value-of select="@name" />Length = Meta_p-&gt;Header.PayloadLength - ((uint32)Data_p - (uint32)Meta_p-&gt;Payload_p);
      const void *<value-of select="@name" /> = Data_p;</when>
  <when test="@type='buffer' and @length!='*'">
      const void *<value-of select="@name" /> = Data_p;</when>
</choose>
<if test="position() = last()"><text>
</text></if>
</template>

<template match="value" mode="serialize">
<choose>
  <when test="@type='uint32'">
  put_uint32(&amp;Data_p, <value-of select="@name" />);</when>
  <when test="@type='string'">
  put_string(&amp;Data_p, <value-of select="@name" />);</when>
  <when test="@type='buffer' and @length='*'">
  put_block(&amp;Data_p, <value-of select="@name" />, <value-of select="@name" />PLSize);</when>
  <when test="@type='buffer' and @length!='*'">
  put_block(&amp;Data_p, <value-of select="@name" />, PLSize);</when>
</choose>
<if test="position() = last()"><text>
</text></if>
</template>

<template match="value" mode="clean">
<choose>
  <when test="@type='string'">
      free(<value-of select="@name" />);</when>
</choose>
</template>

<template match="value" mode="document">
 * @param [in] <value-of select="@name" /><text> </text><value-of select="text()" />
<if test="position() = last()">
 * </if>
</template>

<template match="value" mode="declare">
  <if test="position() > 1">, </if>
<choose>
  <when test="@type='uint32'">const uint32 <value-of select="@name" /></when>
  <when test="@type='string'">const char *<value-of select="@name" /></when>
  <when test="@type='buffer' and @length='*'">int <value-of select="@name" />Length, const void *<value-of select="@name" /></when>
  <when test="@type='buffer' and @length!='*'">const void *<value-of select="@name" /></when>
</choose>
</template>

<template match="input" mode="declare">
<choose>
  <when test="current()=''">void</when>
</choose>
</template> 

<template match="value" mode="call">
  <if test="position() > 1">, </if>
<choose>
  <when test="@type='uint32'"><value-of select="@name" /></when>
  <when test="@type='string'"><value-of select="@name" /></when>
  <when test="@type='buffer' and @length='*'"><value-of select="@name" />Length, <value-of select="@name" /></when>
  <when test="@type='buffer' and @length!='*'"><value-of select="@name" /></when>
</choose>
</template>

<template match="group/documentation">
/*
 * <value-of select="normalize-space(.)"/>
 */
</template>

<template name="prototype">
<param name="name" />
<param name="direction" />
<param name="source" />
<param name="ref" />
<choose>
<when test="name($direction)='input' and contains($source, 'PC')">
/**
 * <value-of select="normalize-space(./documentation/text())"/>
 * \see <value-of select="$ref"/>
 * Call source: <value-of select="$source"/>
 * <apply-templates select="input/value" mode="document" />
 * @return ErrorCode_e ...
 */
ErrorCode_e <value-of select="$name"/>(<choose><when test="count(input/value) > 0"><apply-templates select="input" mode="declare"/><apply-templates select="input/value" mode="declare"/></when><otherwise>void</otherwise></choose>);</when>
<when test="name($direction)='input' and contains($source, 'ME')">
/**
 * <value-of select="normalize-space(./documentation/text())"/>
 * \see <value-of select="$ref"/>
 * Call source: <value-of select="$source"/>
 * <apply-templates select="input/value" mode="document" />
 * @return ErrorCode_e ...
 */
ErrorCode_e <value-of select="$name"/>(uint32 PLSize<if test="count(input/value) > 0">, </if><apply-templates select="input/value" mode="declare"/>);</when>
<when test="name($direction)='output'">
/**
 * Response to \see <value-of select="$ref"/>
 * Call source: <value-of select="$source"/>
 *
 * @param [in] Status Completion status code.
 * <apply-templates select="output/value" mode="document" />
 * @return ErrorCode_e ...
 */
ErrorCode_e <value-of select="$name"/>(uint32 PLSize, ErrorCode_e Status<if test="count(output/value) > 0">, </if><apply-templates select="output/value" mode="declare"></apply-templates>);</when>
</choose>
<text>
</text>
</template>


</stylesheet>
