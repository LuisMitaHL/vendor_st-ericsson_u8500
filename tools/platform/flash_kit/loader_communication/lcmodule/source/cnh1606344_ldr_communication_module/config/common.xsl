<?xml version="1.0" encoding="UTF-8"?>
<!--
* Copyright (C) ST-Ericsson SA 2011
* License terms: 3-clause BSD license
-->
<stylesheet version="1.0" xmlns="http://www.w3.org/1999/XSL/Transform" >

<param name="supportedCommands" />
<variable name="supported_commands" select="document($supportedCommands)/commandspec"/>

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

<template name="commandsupported">
  <param name="path" select="." />
  <value-of select="translate(concat('COMMAND_', $path/../interface[@type='loader']/@name, '_', $path/interface[@type='loader']/@name, '_SUPPORTED'),'abcdefghijklmnopqrstuvwxyz','ABCDEFGHIJKLMNOPQRSTUVWXYZ')" />
</template>

<template match="value" mode="deserialize">
  <choose>
  <when test="@type='uint32'"><text>      </text><value-of select="@name" /> = get_uint32_le(&amp;Data_p);
  </when>
  <when test="@type='uint64'"><text>      </text><value-of select="@name" /> = get_uint64_le(&amp;Data_p);
  </when>
  <when test="@type='string'"><text>      </text><value-of select="@name" /> = skip_str(&amp;Data_p);
  </when>
  <when test="@type='buffer' and @length!='*'"><text>    </text><value-of select="@name" /> = Data_p;
    skip_block(&amp;Data_p, <value-of select="@length" />);
  </when>
  </choose>
  <if test="position() = last()"><text>
  </text>






  </if>
</template>

<template match="value" mode="deserialize_size">
  <choose>
  <when test="@type='uint32'">      uint32 <value-of select="@name" />;
  </when>
  <when test="@type='uint64'">      uint64 <value-of select="@name" />;
  </when>
  <when test="@type='string'">      char *<value-of select="@name" />;
  </when>
  <when test="@type='buffer' and @length!='*'">    void *<value-of select="@name" />;
  </when>
  </choose>
  <if test="position() = last()"><text>
  </text>
  </if>
</template>

<template match="value" mode="serialize">
  <choose>
  <when test="@type='uint32'">put_uint32_le(&amp;Data_p, <value-of select="@name" />);
  </when>
  <when test="@type='uint64'">put_uint64_le(&amp;Data_p, <value-of select="@name" />);
  </when>
  <when test="@type='string'">put_block(&amp;Data_p, <value-of select="@name" />, PLSize<value-of select="@name" />);
  </when>
  <when test="@type='buffer' and @length='*'">put_block(&amp;Data_p, <value-of select="@name" />, <value-of select="@name" />PLSize);
  </when>
  <when test="@type='buffer' and @length!='*'">put_block(&amp;Data_p, <value-of select="@name" />, <value-of select="@length" />);
  </when>
  <when test="@type='SupportedCommand' and @length!='*'">  if (<value-of select="@name" /> != NULL) {
        put_block(&amp;Data_p, <value-of select="@name" />, PLSize<value-of select="@name" />);
    }
  </when>
  <when test="@type='ListDevice' and @length!='*'">serialize_device_entries(&amp;Data_p, <value-of select="@name" />, <value-of select="@length" />);<!--put_block(&amp;Data_p, <value-of select="@name" />, PLSize<value-of select="@name" />);-->
  </when>
  <when test="@type='DirEntry' and @length!='*'">serialize_directory_entries(&amp;Data_p, <value-of select="@name" />, <value-of select="@length" />);<!--put_block(&amp;Data_p, <value-of select="@name" />, PLSize<value-of select="@name" />);-->
  </when>
  <when test="@type='Cipher' and @length!='*'">put_block(&amp;Data_p, <value-of select="@name" />, PLSize<value-of select="@name" />);
  </when>
  </choose>
  <if test="position() = last()"><text></text>
  </if>
</template>

<template match="value" mode="serialize_size">
  <choose>
  <when test="@type='uint32'">PLSize += sizeof(uint32);
  </when>
  <when test="@type='uint64'">PLSize += sizeof(uint64);
   </when>
  <when test="@type='string'">PLSize += PLSize<value-of select="@name" /> = get_uint32_string_le((void **)&amp;<value-of select="@name" />);
  </when>
  <when test="@type='buffer' and @length!='*'">PLSize += <value-of select="@length" />;
  </when>
  <when test="@type='SupportedCommand' and @length!='*'">  PLSize += PLSize<value-of select="@name" /> = <value-of select="@length" /> * sizeof(SupportedCommand_t);
  </when>
  <when test="@type='ListDevice' and @length!='*'">PLSize += get_device_entries_len(<value-of select="@name" />, <value-of select="@length" />); <!--PLSize<value-of select="@name" /> = <value-of select="@length" /> * sizeof(DirEntry_t);-->
  </when>
  <when test="@type='DirEntry' and @length!='*'">PLSize += get_directory_entries_len(<value-of select="@name" />, <value-of select="@length" />); <!--PLSize<value-of select="@name" /> = <value-of select="@length" /> * sizeof(DirEntry_t);-->
  </when>
  <when test="@type='Cipher' and @length!='*'">PLSize += PLSize<value-of select="@name" /> = <value-of select="@length" /> * sizeof(Cipher_t);
  </when>
  </choose>
  <if test="position() = last()"><text></text>
  </if>
</template>

<template match="value" mode="serialize_size_declaration">
  <choose>
  <when test="@type='string'">uint32 PLSize<value-of select="@name" /> = 0;
  </when>
  <when test="@type='SupportedCommand'">uint32 PLSize<value-of select="@name" /> = 0;
  </when>
  <!--<when test="@type='ListDevice'">uint32 PLSize<value-of select="@name" /> = 0;
  </when>-->
  <!--<when test="@type='DirEntry'">uint32 PLSize<value-of select="@name" /> = 0;
  </when>-->
  <when test="@type='Cipher'">uint32 PLSize<value-of select="@name" /> = 0;
  </when>
  </choose>
  <if test="position() = last()"><text></text>
  </if>
</template>

<template match="value" mode="perm">
  <if test="position() > 1">,
  </if>
  <value-of select="@ref" />
</template>

<template match="value" mode="auth">
  <if test="position() > 1">,
  </if>
  <value-of select="@ref" />
</template>
  
<template match="value" mode="permnumber">
  <if test="position() = last()">
    <value-of select="position()" />
  </if>
</template>

<template match="value" mode="authnumber">
  <if test="position() = last()">
    <value-of select="position()" />
  </if>
</template>
  
<template match="value" mode="clean">
<choose>
  <when test="@type='string'"></when>
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
  <when test="@type='uint64'">const uint64 <value-of select="@name" /></when>
  <when test="@type='string'">const char *<value-of select="@name" /></when>
  <when test="@type='buffer' and @length='*'">int <value-of select="@name" />Length, const void *<value-of select="@name" /></when>
  <when test="@type='buffer' and @length!='*'"> void *<value-of select="@name" /></when>
  <when test="@type='SupportedCommand' and @length!='*'">SupportedCommand_t <value-of select="@name" />[]</when>
  <when test="@type='ListDevice' and @length!='*'">ListDevice_t <value-of select="@name" />[]</when>
  <when test="@type='DirEntry' and @length!='*'">DirEntry_t <value-of select="@name" />[]</when>
  <when test="@type='Cipher' and @length!='*'">Cipher_t <value-of select="@name" />[]</when>
</choose>
</template>

<template match="value" mode="call">
  <if test="position() > 1">, </if>
<choose>
  <when test="@type='uint32'"><value-of select="@name" /></when>
  <when test="@type='uint64'"><value-of select="@name" /></when>
  <when test="@type='string'"><value-of select="@name" /></when>
  <when test="@type='buffer' and @length='*'"><value-of select="@name" />Length, <value-of select="@name" /></when>
  <when test="@type='buffer' and @length!='*'"><value-of select="@name" /></when>
</choose>
</template>

<template match="group/documentation">
<variable name="group" select="../@number" />
<if test='$target="lcm" or $supported_commands/group[@number=$group]'>
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
<when test="name($direction)='input' and contains($source, 'PC')">
/**
 * <value-of select="normalize-space(./documentation/text())"/>
 * \see <value-of select="$ref"/>
 * Call source: <value-of select="$source"/>
 * @param [in] Session Input session.<apply-templates select="input/value" mode="document"/>
 * @return ErrorCode_e ...
 */
ErrorCode_e <value-of select="$name"/>(uint16 Session<if test="count(input/value) > 0">, </if><apply-templates select="input/value" mode="declare"/>);</when>
<when test="name($direction)='input' and contains($source, 'ME')">
/**
 * <value-of select="normalize-space(./documentation/text())"/>
 * \see <value-of select="$ref"/>
 * Call source: <value-of select="$source"/>
 * <apply-templates select="input/value" mode="document"/>
 * @return ErrorCode_e ...
 */
ErrorCode_e <value-of select="$name"/>(<choose><when test="count(input/value) > 0"><apply-templates select="input/value" mode="declare"/></when><otherwise>void</otherwise></choose>);</when>
<when test="name($direction)='output'">
/**
 * Response to \see <value-of select="$ref"/>
 * Call source: <value-of select="$source"/>
 * @param [in] Session Transfered input session.
 * @param [in] Status Completion status code.
 * <apply-templates select="output/value" mode="document"/>
 * @return ErrorCode_e ...
 */
ErrorCode_e <value-of select="$name"/>(uint16 Session, ErrorCode_e Status<if test="count(output/value) > 0">, </if><apply-templates select="output/value" mode="declare"></apply-templates>);</when>
</choose>
<text>
</text>
</template>


</stylesheet>
