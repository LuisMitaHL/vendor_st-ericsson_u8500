//*************************************************************************
/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
//
// Filename codec_name_changes.h
//
//
// $Locker: $
//*************************************************************************
///
/// \ingroup codec specific name changes
///
/// \file
/// \brief
///
///
//*************************************************************************

#ifndef CODEC_NAME_CHANGES_H
#define CODEC_NAME_CHANGES_H

#define SDL_Bit_String BIT_STRING_t

//Status Code

#define unspecified StatusCode_unspecified
#define systemFailure StatusCode_systemFailure
#define unexpectedMessage StatusCode_unexpectedMessage
#define protocolError StatusCode_protocolError
#define dataMissing StatusCode_dataMissing
#define unexpectedDataValue StatusCode_unexpectedDataValue
#define posMethodFailure StatusCode_posMethodFailure
#define posMethodMismatch StatusCode_posMethodMismatch
#define posProtocolMismatch StatusCode_posProtocolMismatch
#define targetSETnotReachable StatusCode_targetSETnotReachable
#define versionNotSupported StatusCode_versionNotSupported
#define resourceShortage StatusCode_resourceShortage
#define invalidSessionId StatusCode_invalidSessionId
#define nonProxyModeNotSupported StatusCode_nonProxyModeNotSupported
#define proxyModeNotSupported StatusCode_proxyModeNotSupported
#define positioningNotPermitted StatusCode_positioningNotPermitted
#define authNetFailure StatusCode_authNetFailure
#define authSuplinitFailure StatusCode_authSuplinitFailure
#define consentDeniedByUser StatusCode_consentDeniedByUser
#define consentGrantedByUser StatusCode_consentGrantedByUser

#endif // CODEC_NAME_CHANGES_H
