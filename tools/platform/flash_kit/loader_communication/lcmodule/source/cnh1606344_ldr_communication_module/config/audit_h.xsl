<?xml version="1.0" encoding="UTF-8"?>
<!--
* Copyright (C) ST-Ericsson SA 2011
* License terms: 3-clause BSD license
-->
<stylesheet version="1.0" xmlns="http://www.w3.org/1999/XSL/Transform" >

<output method="text"/>
<strip-space elements="*"/>
<param name="target" />
          
<template match="/commandspec">
/* $Copyright$ */
/* NOTE: This is an automatically generated file. DO NOT EDIT! */

#ifndef _AUDIT_H
#define _AUDIT_H
#include "t_command_protocol.h"
#include "command_ids.h"

/**
 *  @addtogroup ldr_LCM
 *  Error codes for internal loader commands.
 *  @{
 */
<apply-templates select="AuthenticationList" />

<apply-templates select="PermissionList" />

typedef struct CommandPermissionList_s {

    GroupId_e               Group;
    CommandId_e             Command;
    int                     CommmandPermissionLength;
    CommandPermission_e     *CommmandPermmision_p;
    int                     ComandAuthenticationLength;
    CommandAuthentication_e *CommandAuthentication_p;
    int                     Depandancy;
    boolean                 factory;
    boolean                 rd;
    boolean                 product;
    boolean                 service;
    uint8                   *DataParam_p;
} CommandPermissionList_t;

CommandPermissionList_t *GetAuditData(CommandData_t *CmdData_p);
ErrorCode_e CommandAudit(CommandData_t *CmdData_p);
#endif /* _AUDIT_H */
</template>

<template match="AuthenticationList">
TYPEDEF_ENUM {
<apply-templates select="value" />} ENUM8(CommandAuthentication_e);
</template>

  
<template match="PermissionList">
TYPEDEF_ENUM {
<apply-templates select="value" />} ENUM8(CommandPermission_e);
</template>

<template match="value">
<text>    </text><value-of select="@name" /> = <value-of select="@number" />, /**&lt; <value-of select="text()" /> */
</template>

<template match="value[last()]">
<text>    </text><value-of select="@name" /> = <value-of select="@number" />  /**&lt; <value-of select="text()" /> */
</template>

</stylesheet>
