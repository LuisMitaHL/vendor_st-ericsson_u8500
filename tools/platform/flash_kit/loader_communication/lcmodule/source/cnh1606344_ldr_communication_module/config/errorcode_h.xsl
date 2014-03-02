<?xml version="1.0" encoding="UTF-8"?>
<!--
* Copyright (C) ST-Ericsson SA 2011
* License terms: 3-clause BSD license
-->
<stylesheet version="1.0" xmlns="http://www.w3.org/1999/XSL/Transform">

<output method="text"/>
<strip-space elements="*"/>
<param name="target"/>

<template match="/commandspec">/* $Copyright$ */
/* NOTE: This is an automatically generated file. DO NOT EDIT! */

#ifndef _ERRORCODE_H
#define _ERRORCODE_H


/**
 *  @addtogroup ldr_LCM
 *  Error codes for internal loader commands.
 *  @{
 */

/*************************************************************************
* Includes
*************************************************************************/
#include &quot;t_basicdefinitions.h&quot;

#define A2_ERROR_CODES_OFFSET 5000
/**
 *  Internal loader command error codes. Possible range 0x0000 - 0x0FFE (4094).
 */


/**
 * Table for Error groups range
 *
 * General Fatal 0-50
 * General non-fatal 51-99
 *
 * IO Fatal 100-150
 * IO non-fatal 151-199
 *
 * Communication Fatal 200-250
 * Communication non-fatal 251-299
 *
 * Signature Fatal 300-350
 * Signature non-fatal 351-399
 *
 * Authentication Fatal 400-450
 * Authentication non-fatal 451-499
 *
 * Zip Parser Fatal 500-550
 * Zip Parser non-fatal 551-599
 *
 * System Fatal 600-650
 * System non-fatal 651-699
 *
 * Flash Fatal 700-750
 * Flash non-fatal 751-799
 *
 *
 * File management Fatal 900-950
 * File management non-fatal 951-999
 *
 * Command Auditing and execution Fatal 1000-1050
 * Command Auditing and execution non-fatal 1051-1099
 *
 *
 * Timers Fatal 1200-1250
 * Timers non-fatal 1251-1299
 *
 * Parameter Fatal 1300-1350
 * Parameter non-fatal 1351-1399
 *
 * Block device Fatal 1400-1450
 * Block device non-fatal 1451-1499
 *
 * Boot area Fatal 1500-1550
 * Boot area non-fatal 1551-1599
 *
 * Cops data Fatal 1600-1650
 * Cops data non-fatal 1651-1699
 *
 * PD NAND Fatal 1700-1750
 * PD NAND non-fatal 1751-1799
 *
 * Trim Area non-fatal 1851-1899
 *
 * Loader utilities Fatal 1900-1950
 * Loader utilities non-fatal 1951-1999
 *
 * Loader ADBG Fatal 2000-2050
 * Loader ADBG non-fatal 2051-2099
 *
 * OTP applications Fatal 2100-2150
 * OTP applications non-fatal 2151-2199
 *
 * Security applications Fatal 2200-2250
 * Security applications non-fatal 2251-2299
 *
 * Trim Area Fatal 2300-2350
 * Trim Area non-fatal 2351-2499
 *
 * Recovery applications Fatal 2500-2550
 * Recovery applications non-fatal 2551-2599
 *
 * PD CFI Flash Fatal 2600-2650
 * PD CFI Flash non-fatal 2651-2699
 *
 * HSI Driver Fatal 2700-2724
 * HSI Driver Non-Fatal 2725-2749
 *
 * HSI Driver Error Callback Fatal 2750-2774
 * HSI Driver Error Callback Non-Fatal 2775-2799
 *
 * Communication Relay Fatal 2800-2849
 * Communication Relay Non-Fatal 2850-2899
 *
 * SDIO Driver Fatal 2900-2924
 * SDIO Driver Non-Fatal 2925-2949
 *
 * SDIO Driver Error Callback Fatal 2950-2974
 * SDIO Driver Error Callback Non-Fatal 2975-2999
 *
 * External BAM Interface Fatal 3000-3049
 * External BAM Interface non-fatal 3050-3099
 *
 * Security Library Fatal 4000-4050
 * Security Library non-fatal 4051-4250
 *
 * Emulator Fatal 4300-4350
 * Emulator non-fatal 4351-4399
 *
 * A2 and lower versions error codes translation 5000+
 */

<apply-templates select="status"/>
/** @} */
#endif /* _ERRORCODE_H */
</template>

<template match="status">
typedef enum {
<apply-templates select="value"/>} ErrorCode_e;
</template>

<template match="value">
  <text>  </text><value-of select="@name"/> = <value-of select="@number"/>, /**&lt;  <value-of select="@short"/> */
</template>

<template match="value[last()]">
  <text>  </text><value-of select="@name"/> = <value-of select="@number"/>  /**&lt; <value-of select="@short"/> */
</template>

</stylesheet>
