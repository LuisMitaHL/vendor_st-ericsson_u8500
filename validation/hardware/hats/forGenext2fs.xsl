<?xml version="1.0" encoding="ASCII"?>

<!--
     Copyright (c) ST-Ericsson, 2010
     Author: Gerald Lejeune <gerald.lejeune@stericsson.com>
-->
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0">
<xsl:output method="text" media-type="text/plain" indent="no"/>
<xsl:template match="/"><xsl:text>#name&#9;&#9;&#9;type&#9;mode&#9;uid&#9;gid&#9;major&#9;minor&#9;start&#9;inc&#9;count&#10;</xsl:text>
<xsl:for-each select="/files/file">
  <xsl:value-of select="name"/><xsl:text>&#9;&#9;&#9;</xsl:text>
  <xsl:value-of select="type"/><xsl:text>&#9;</xsl:text>
  <xsl:call-template name="alphaToNumericMode">
    <xsl:with-param name="value" select="mode"/>
  </xsl:call-template><xsl:text>&#9;</xsl:text>
  <xsl:value-of select="uid"/><xsl:text>&#9;</xsl:text>
  <xsl:value-of select="gid"/><xsl:text>&#9;</xsl:text>
  <xsl:value-of select="major"/><xsl:text>&#9;</xsl:text>
  <xsl:value-of select="minor"/><xsl:text>&#9;</xsl:text>
  <xsl:value-of select="start"/><xsl:text>&#9;</xsl:text>
  <xsl:value-of select="inc"/><xsl:text>&#9;</xsl:text>
  <xsl:value-of select="count"/><xsl:text>&#10;</xsl:text>
</xsl:for-each>
</xsl:template>
<xsl:template name="alphaToNumericMode">
  <xsl:param name="value"/>
    <xsl:choose>
      <xsl:when test='string-length($value) != 3'>0</xsl:when>
      <xsl:otherwise>
      <xsl:variable name='read'>
        <xsl:call-template name='charToFlag'>
          <xsl:with-param name='char' select='substring($value, 1, 1)'/>
        </xsl:call-template>
      </xsl:variable>
        <xsl:variable name='write'>
          <xsl:call-template name='charToFlag'>
            <xsl:with-param name='char' select='substring($value, 2, 1)'/>
          </xsl:call-template>
        </xsl:variable>
        <xsl:variable name='execute'>
          <xsl:call-template name='charToFlag'>
            <xsl:with-param name='char' select='substring($value, 3, 1)'/>
          </xsl:call-template>
        </xsl:variable>
        <xsl:value-of select='$read + $write + $execute'/>
        <xsl:value-of select='$read + $write + $execute'/>
        <xsl:value-of select='$read + $write + $execute'/>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:template>
  <xsl:template name='charToFlag'>
    <xsl:param name='char'/>
    <xsl:choose>
      <xsl:when test='$char = "-"'>0</xsl:when>
      <xsl:when test='$char = "x"'>1</xsl:when>
      <xsl:when test='$char = "w"'>2</xsl:when>
      <xsl:when test='$char = "r"'>4</xsl:when>
    </xsl:choose>
  </xsl:template>
</xsl:stylesheet>
