/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/*! ****************************************************************************
 * \file PfRoles.h
 * Platform roles static class interface
 *
 * \author philippe.tribolo@stericsson.com
 *
 * *****************************************************************************/
#ifndef _PFROLES_H_
#define _PFROLES_H_

#include "uosal.h"
#include "PlatformPatternsUC.h"

namespace rme {

/** Platform roles static class */
class PfRoles {
 public:
  /// checks if role is audio related
  static bool IsAudioRole(const char* role) {
    return IsRoleInTable(role, PlatformPatternsUC::msAudioRolesRoot);
  }

  /// checks if role is a qualified ALP source
  static bool IsAlpRoleSource(const char* role) {
    return (IsAlpRoleSingleSource(role) || IsAlpRoleLineSource(role));
  }
  /// checks if role is in ALP single sources roles table
  static bool IsAlpRoleSingleSource(const char* role) {
    return IsRoleInTable(role, PlatformPatternsUC::msAlpRolesSingleSources);
  }
  /// checks if role is in ALP line sources roles table
  static bool IsAlpRoleLineSource(const char* role) {
    return IsRoleInTable(role, PlatformPatternsUC::msAlpRolesLineSources);
  }

  /// checks if role is in ALP sink roles table
  static bool IsAlpRoleSink(const char* role) {
    return IsRoleInTable(role, PlatformPatternsUC::msAlpRolesSink);
  }

  /// checks if role is ALP forbidden
  static bool IsAlpRoleForbidden(const char* role) {
    if (IsSpeechProcessingRole(role)) {
      return true;
    } else {
      return IsRoleInTable(role, PlatformPatternsUC::msAlpRolesForbidden);
    }
  }

  /// checks if role is in ALP speech processing roles table
  static bool IsSpeechProcessingRole(const char* role) {
    return IsRoleInTable(role, PlatformPatternsUC::msSpeechProcRoles);
  }

  /// checks if role is in table of roles that can coexist with ALP
  static bool CanCoexistWithAlpUcc(const char* role) {
    if (IsCsCallRole(role)) {
      return true;
    } else {
      return IsRoleInTable(role, PlatformPatternsUC::msRolesCoexistWithAlpUcc);
    }
  }
  /// checks if role is in CsCall roles table
  static bool IsCsCallRole(const char* role) {
    return IsRoleInTable(role, PlatformPatternsUC::msCsCallRoles);
  }

 private:
  /// compares chains, with support of * wildcard (at the end only for the timebeing)
  static bool CheckRolesMatch(const char* role, const char* roleref, bool allowwildcard, bool *pgotexclusionmatch = NULL);

  /// checks if role in provided role table
  static bool IsRoleInTable(const char* role, const char** tablerole);
};

}  // namespace

#endif
