<?xml version="1.0" encoding="utf-8"?>
<!--
* Copyright (C) ST-Ericsson SA 2011
* License terms: 3-clause BSD license
-->
<stylesheet version="1.0" xmlns="http://www.w3.org/1999/XSL/Transform">

<output method="text"/>
<strip-space elements="*"/>
<param name="target"/>

<template match="/commandspec">/* $Copyright ST-Ericsson 2010$ */
/* NOTE: This is an automatically generated file. DO NOT EDIT! */

#ifndef _LCDRIVERERRORCODE_H
#define _LCDRIVERERRORCODE_H

/**
 *  Error codes for LCDriver.
 */

/**
 * Table for Error groups range
 *
 * General Fatal 10001-10025
 * General Non-Fatal 10026-10050
 *
 * Interface Fatal 10051-10075
 * Interface Non-Fatal 10076-10100
 *
 * System Thread Fatal 10101-10125
 * System Thread Non-Fatal 10126-10150
 *
 * Bulk Fatal 10151-10175
 * Bulk Non-Fatal 10176-10200
 *
 * Hardware Fatal 10201-10225
 * Hardware Non-Fatal 10226-10250
 *
 * Loader Command Execution Fatal 10251-10275
 * Loader Command Execution Non-Fatal 10276-10300
 *
 * Buffers Fatal 10301-10325
 * Buffers Non-Fatal 10326-10350
 *
 * IO File Fatal 10351-10375
 * IO File Non-Fatal 10376-10400
 *
 * LCM DLL Fatal 10401-10425
 * LCM DLL Non-Fatal 10426-10450
 *
 * Exceptions Fatal 10451-10475
 * Exceptions Non-Fatal 10476-10500
 *
 * LCDriver Thread Fatal 10501-10525
 * LCDriver Thread Non-Fatal 10526-10550
 *
 */
<apply-templates select="status"/>
#endif /* _LCDRIVERERRORCODE_H */
</template>

<template match="status">
typedef enum {
<apply-templates select="value"/>} InternalErrorCodes_e;
</template>
  
<template match="value">
  <text>  </text><value-of select="@name"/> = <value-of select="@number"/><if test="position() != last()">,</if>  /** <value-of select="@short"/> */
</template>

</stylesheet>
