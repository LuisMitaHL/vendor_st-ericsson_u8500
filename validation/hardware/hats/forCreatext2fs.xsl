<?xml version="1.0" encoding="ASCII"?>

<!--
     Copyright (c) ST-Ericsson, 2010
     Author: Gerald Lejeune <gerald.lejeune@stericsson.com>
-->
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0">
  <xsl:output method="text" media-type="text/plain" indent="no"/>
  <xsl:template match="/">
    <xsl:for-each select="/files/file">
      <xsl:value-of select="type"/>
      <xsl:value-of select="mode"/><xsl:text>&#9;</xsl:text>
      <xsl:choose>
        <xsl:when test='type = "d"'><xsl:text>&#9;</xsl:text></xsl:when>
        <xsl:otherwise>
          <xsl:value-of select="major"/><xsl:text>,</xsl:text>
          <xsl:value-of select="minor"/><xsl:text>&#9;</xsl:text>
        </xsl:otherwise>
      </xsl:choose>
      <xsl:value-of select="name"/><xsl:text>&#10;</xsl:text>
    </xsl:for-each>
  </xsl:template>
</xsl:stylesheet>
