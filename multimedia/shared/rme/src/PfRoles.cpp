/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/*! ****************************************************************************
 * \file PfRoles.cpp
 * Platform roles static classes
 *
 * \author philippe.tribolo@stericsson.com
 *
 * *****************************************************************************/

#include <rme_types.h>
#define _PLATFORMPATTERNSINIT_CPP_
#include "PlatformPatternsUC.h"
#undef _PLATFORMPATTERNSINIT_CPP_
#include "PfRoles.h"

namespace rme {

/// compares chains, with support of * wildcard (at the end only for the timebeing)
bool PfRoles::CheckRolesMatch(const char* role, const char* roleref,
        bool allowwildcard, bool *pgotexclusionmatch) {
  if ((role == NULL) || (roleref == NULL))
    return false;

  // check exclusion symbol '!' presence
  bool excludematch = false;
  if (*roleref == '!') {
    excludematch = true;
    roleref++;
  }

  // check wildcard '*' presence
  bool checkcount = false;
  int pos = 0;
  if (allowwildcard) {
    // allow wildcard only at end of roleref
    const char* star = strstr(roleref, "*");
    if (star) {
      pos = star - roleref;
      star++;
      if (*star == '\0')
        checkcount = true;
    }
  }

  bool statusmatch = false;
  if (PlatformPatternsUC::mIsRoleMatchingCaseSentive) {
    if (checkcount) {
      if (strncmp(role, roleref, pos) == 0)
        statusmatch = true;
    } else {
      if (strcmp(role, roleref) == 0)
        statusmatch = true;
    }
  } else {
    if (checkcount) {
      if (strncasecmp(role, roleref, pos) == 0)
        statusmatch = true;
    } else {
      if (strcasecmp(role, roleref) == 0)
        statusmatch = true;
    }
  }

  if (statusmatch) {
    if (excludematch && pgotexclusionmatch) {
      *pgotexclusionmatch = true;
    }
    return true;
  } else {
    return false;
  }
}

/// checks if role in provided role table
bool PfRoles::IsRoleInTable(const char* role, const char** tablerole) {
  if ((role == NULL) || (tablerole == NULL)) {
    return false;
  }
  for (unsigned int j = 0; j < PlatformPatternsUC::mMaxRoleCount; j++) {
    if (tablerole[j] == '\0') {
      return false;  // last table entry
    }
    if (CheckRolesMatch(role, tablerole[j], true))
      return true;
  }
  return false;
}

}  // namespace

