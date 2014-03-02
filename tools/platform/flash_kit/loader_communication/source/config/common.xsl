<?xml version="1.0" encoding="utf-8"?>
<!--
* Copyright (C) ST-Ericsson SA 2011
* License terms: 3-clause BSD license
-->
<stylesheet version="1.0" xmlns="http://www.w3.org/1999/XSL/Transform">

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
<when test="@type='uint32'"><text>        </text><value-of select="@name" /> = Serialization::get_uint32_le(&amp;Data_p);
</when>
<when test="@type='uint64'"><text>        </text><value-of select="@name" /> = Serialization::get_uint64_le(&amp;Data_p);
</when>
<when test="@type='string'"><text>        </text><value-of select="@name" /> = Serialization::skip_str(&amp;Data_p);
</when>
<when test="@type='buffer' and @length!='*'"><text>        </text><value-of select="@name" /> = Data_p;
<text>        </text>Serialization::skip_block(&amp;Data_p, <value-of select="@length" />);
</when>
<when test="@type='SupportedCommand' and @length!='*'"><text>        </text><value-of select="@name" /> = Serialization::get_supportedcommands(&amp;Data_p, <value-of select="@length" />);
</when>
<when test="@type='ListDevice' and @length!='*'"><text>        </text><value-of select="@name" /> = Serialization::get_devices(&amp;Data_p, <value-of select="@length" />);
</when>
<when test="@type='DirEntry' and @length!='*'"><text>        </text><value-of select="@name" /> = Serialization::get_direntries(&amp;Data_p, <value-of select="@length" />);
</when>
<when test="@type='Cipher' and @length!='*'"><text>        </text><value-of select="@name" /> = Data_p;
<text>        </text>Serialization::skip_str(&amp;Data_p);
</when>
</choose>
</template>

<template match="value" mode="deserialize_size">
<choose>
<when test="@type='uint32'">      uint32 <value-of select="@name" /> = 0;
</when>
<when test="@type='uint64'">      uint64 <value-of select="@name" /> = 0;
</when>
<when test="@type='string'">      const char *<value-of select="@name" /> = NULL;
</when>
<when test="@type='buffer' and @length!='*'">      const void *<value-of select="@name" /> = NULL;
</when>
<when test="@type='SupportedCommand' and @length!='*'">      SupportedCommand_t *<value-of select="@name" /> = NULL;
</when>
<when test="@type='ListDevice' and @length!='*'">      ListDevice_t *<value-of select="@name" /> = NULL;
</when>
<when test="@type='DirEntry' and @length!='*'">      DirEntry_t *<value-of select="@name" /> = NULL;
</when>
<when test="@type='Cipher' and @length!='*'">      Cipher_t *<value-of select="@name" /> = NULL;
</when>
</choose>
</template>

<template match="value" mode="serialize">
<choose>
<when test="@type='uint32'">  Serialization::put_uint32_le(&amp;Data_p, <value-of select="@name" />);
</when>
<when test="@type='uint64'">  Serialization::put_uint64_le(&amp;Data_p, <value-of select="@name" />);
</when>
<when test="@type='string'">  Serialization::put_uint32_le(&amp;Data_p, PLSize<value-of select="@name" />);
  Serialization::put_block(&amp;Data_p, <value-of select="@name" />, PLSize<value-of select="@name" />);
</when>
<when test="@type='buffer' and @length='*'">  Serialization::put_block(&amp;Data_p, <value-of select="@name" />, <value-of select="@name" />PLSize);
</when>
<when test="@type='buffer' and @length!='*'">  Serialization::put_block(&amp;Data_p, <value-of select="@name" />, <value-of select="@length" />);
</when>
<when test="@type='SupportedCommand' and @length!='*'">  Serialization::put_block(&amp;Data_p, <value-of select="@name" />, PLSize<value-of select="@name" />);
</when>
<when test="@type='ListDevice' and @length!='*'">  Serialization::serialize_device_entries(&amp;Data_p, <value-of select="@name" />, <value-of select="@length" />);<!--put_block(&amp;Data_p, <value-of select="@name" />, PLSize<value-of select="@name" />);-->
</when>
<when test="@type='DirEntry' and @length!='*'">  Serialization::serialize_directory_entries(&amp;Data_p, <value-of select="@name" />, <value-of select="@length" />);<!--put_block(&amp;Data_p, <value-of select="@name" />, PLSize<value-of select="@name" />);-->
</when>
<when test="@type='Cipher' and @length!='*'">  Serialization::put_block(&amp;Data_p, <value-of select="@name" />, PLSize<value-of select="@name" />);
</when>
</choose>
</template>

<template match="value" mode="serialize_size">
<choose>
<when test="@type='uint32'">  PLSize += sizeof(uint32);
</when>
<when test="@type='uint64'">  PLSize += sizeof(uint64);
</when>
<when test="@type='string'">  PLSize += sizeof(uint32);
  PLSize<value-of select="@name" /> = strlen(<value-of select="@name" />);
  PLSize += PLSize<value-of select="@name" />;
</when>
<when test="@type='buffer' and @length!='*'">  PLSize += <value-of select="@length" />;
</when>
<when test="@type='SupportedCommand' and @length!='*'">  PLSize += PLSize<value-of select="@name" /> = <value-of select="@length" /> * sizeof(SupportedCommand_t);
</when>
<when test="@type='ListDevice' and @length!='*'">  PLSize += Serialization::get_device_entries_len(<value-of select="@name" />, <value-of select="@length" />); <!--PLSize<value-of select="@name" /> = <value-of select="@length" /> * sizeof(DirEntry_t);-->
</when>
<when test="@type='DirEntry' and @length!='*'">  PLSize += Serialization::get_directory_entries_len(<value-of select="@name" />, <value-of select="@length" />); <!--PLSize<value-of select="@name" /> = <value-of select="@length" /> * sizeof(DirEntry_t);-->
</when>
<when test="@type='Cipher' and @length!='*'">  PLSize += PLSize<value-of select="@name" /> = <value-of select="@length" /> * sizeof(Cipher_t);
</when>
</choose>
</template>

<template match="value" mode="serialize_size_declaration">
<choose>
<when test="@type='string'">  uint32 PLSize<value-of select="@name" /> = 0;
</when>
<when test="@type='SupportedCommand'">  uint32 PLSize<value-of select="@name" /> = 0;
</when>
<when test="@type='Cipher'">  uint32 PLSize<value-of select="@name" /> = 0;
</when>
</choose>
</template>

<template match="value" mode="perm">
<if test="position() > 1">, </if><value-of select="@ref" />
</template>

<template match="value" mode="auth">
<if test="position() > 1">, </if><value-of select="@ref" />
</template>

<template match="value" mode="permnumber">
<if test="position() = last()"><value-of select="position()" /></if>
</template>

<template match="value" mode="authnumber">
<if test="position() = last()"><value-of select="position()" /></if>
</template>

<template match="value" mode="clean">
<choose>
<when test="@type='string'"></when>
</choose>
</template>

<template match="value" mode="document">
   * @param [in] <value-of select="@name" /><text> </text><value-of select="text()" />
</template>

<template match="value" mode="declare">
<param name="continue" select="'false'" />
<if test="($continue = 'true') or (position() > 0)">, </if>
<choose>
<when test="@type='uint32'">const uint32 <value-of select="@name" /></when>
<when test="@type='uint64'">const uint64 <value-of select="@name" /></when>
<when test="@type='string'">const char *<value-of select="@name" /></when>
<when test="@type='buffer' and @length='*'">int <value-of select="@name" />Length, const void *<value-of select="@name" /></when>
<when test="@type='buffer' and @length!='*'">const void *<value-of select="@name" /></when>
<when test="@type='SupportedCommand' and @length!='*'">SupportedCommand_t *<value-of select="@name" /></when>
<when test="@type='ListDevice' and @length!='*'">ListDevice_t *<value-of select="@name" /></when>
<when test="@type='DirEntry' and @length!='*'">DirEntry_t *<value-of select="@name" /></when>
<when test="@type='Cipher' and @length!='*'">Cipher_t *<value-of select="@name" /></when>
</choose>
</template>

<template match="input" mode="declare">
</template>

<template match="value" mode="call">
<param name="continue" select="'false'" />
<if test="($continue = 'true') or (position() > 0)">, </if>
<choose>
<when test="@type='uint32'">
<value-of select="@name" />
</when>
<when test="@type='uint64'">
<value-of select="@name" />
</when>
<when test="@type='string'">
<value-of select="@name" />
</when>
<when test="@type='buffer' and @length='*'">
<value-of select="@name" />Length, <value-of select="@name" />
</when>
<when test="@type='buffer' and @length!='*'">
<value-of select="@name" />
</when>
<when test="@type='SupportedCommand' and @length='*'">
<value-of select="@name" />Count, <value-of select="@name" />
</when>
<when test="@type='SupportedCommand' and @length!='*'">
<value-of select="@name" />
</when>
<when test="@type='ListDevice' and @length='*'">
<value-of select="@name" />Count, <value-of select="@name" />
</when>
<when test="@type='ListDevice' and @length!='*'">
<value-of select="@name" />
</when>
<when test="@type='DirEntry' and @length='*'">
<value-of select="@name" />Count, <value-of select="@name" />
</when>
<when test="@type='DirEntry' and @length!='*'">
<value-of select="@name" />
</when>
</choose>
</template>

<template match="group/documentation">
<if test="../interface/@name != 'ADbg'">
  /*
   * <value-of select="normalize-space(.)"/>
   */
</if>
</template>
  
<template name="prototype">
<param name="name" />
<param name="direction" />
<param name="source" />
<param name="ref" />
<choose>
  <when test="name($direction)='output' and contains(@source, 'PC')">
  /**
   * <value-of select="normalize-space(./documentation/text())"/>
   * \see <value-of select="$ref"/>
   * Call source: <value-of select="$source"/>
   * @param [in] SessionOut Output session.<apply-templates select="input/value" mode="document" />
   * 
   * @return ErrorCode_e ...
   */
  ErrorCode_e <value-of select="$name"/>(uint16 SessionOut<apply-templates select="input" mode="declare"/><apply-templates select="input/value" mode="declare"/>);
  </when>
  <when test="name($direction)='output' and contains(@source, 'ME')">
  /**
   * <value-of select="normalize-space(./documentation/text())"/>
   * \see <value-of select="$ref"/>
   * Call source: <value-of select="$source"/>
   * @param [in] Session Input session.<apply-templates select="input/value" mode="document" />
   * 
   * @return ErrorCode_e ...
   */
  ErrorCode_e <value-of select="$name"/>(uint16 Session<apply-templates select="input/value" mode="declare"/>);
  </when>
  <when test="name($direction)='input'">
  /**
   * Response to \see <value-of select="$ref"/>
   * Call source: <value-of select="$source"/>
   * @param [in] Session Transfered input session.
   * @param [in] Status Completion status code.<apply-templates select="output/value" mode="document" />
   * 
   * @return ErrorCode_e ...
   */
  ErrorCode_e <value-of select="$name"/>(uint16 Session, ErrorCode_e Status<apply-templates select="output/value" mode="declare">
  <with-param name="continue" select="'true'" />
  </apply-templates>);
  </when>
</choose>
</template>

</stylesheet>
