/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/*! ****************************************************************************
 * \file PlatformPatternsUC.h
 * Platform patterns 'use cases' static class interface
 *
 * \author philippe.tribolo@stericsson.com
 *
 * *****************************************************************************/
#ifndef _PLATFORMPATTERNSUC_H_
#define _PLATFORMPATTERNSUC_H_

namespace rme {

/** Platform Patterns 'use cases' static class */
class PlatformPatternsUC {
 private:
  friend class PfRoles;

  /// roles matching can be made case sensitive or not
  static const bool mIsRoleMatchingCaseSentive = false;

  /// maximum number of roles per table (protection)
  static const unsigned int mMaxRoleCount = 8;

  /// audio all roles: root name
  static const char* msAudioRolesRoot[];

  /// roles for audio low power single sources
  static const char* msAlpRolesSingleSources[];

  /// "line" sources roles for audio low power
  static const char* msAlpRolesLineSources[];

  /// "sink" roles for audio low power sink
  static const char* msAlpRolesSink[];

  /// roles of components that prevent/forbid ALP
  static const char* msAlpRolesForbidden[];

  /// "speech processing" roles for audio low power sink
  static const char* msSpeechProcRoles[];

  /// roles of components that can coexist with active resources states
  /// - in *addition* to CsCall roles (implicitely considered from msCsCallRoles table) -
  /// with audio low power (activated) usecase
  static const char* msRolesCoexistWithAlpUcc[];

  /// CsCall roles
  static const char* msCsCallRoles[];
};

#ifdef _PLATFORMPATTERNSINIT_CPP_

// all role tables must end with '\0' entry

/// audio all roles: root name
const char* PlatformPatternsUC::msAudioRolesRoot[] = {
  "audio_*",
  "speech_*",
  "CS_*",
  "cscall*",
  '\0'
};

/// roles for audio low power single sources
/// wildcard "*" is allowed at end of strings
const char* PlatformPatternsUC::msAlpRolesSingleSources[] = {
  "audio_decoder*",
  '\0'
};

/// "line" sources roles for audio low power
/// wildcard "*" is allowed at end of strings
const char* PlatformPatternsUC::msAlpRolesLineSources[] = {
  "audio_splitter.pcm*",
  "audio_mixer.pcm*",
  "audio_processor.pcm*",
  '\0'
};

/// "sink" roles for audio low power sink
/// => roles of all low-powerable audio sinks
/// wildcard "*" is allowed at end of strings
const char* PlatformPatternsUC::msAlpRolesSink[] = {
  "audio_sink.pcm*",
  '\0'
};

/// roles of components that prevent/forbid ALP
/// wildcard "*" is allowed at end of strings
const char* PlatformPatternsUC::msAlpRolesForbidden[] = {
  '\0'  // table currently empty
};

/// "speech processing" roles for audio low power sink
const char* PlatformPatternsUC::msSpeechProcRoles[] = {
  "speech_processing",
  '\0'
};

/// roles of components that can coexist with active resources states
/// - in *addition* to CsCall roles (implicitely considered from msCsCallRoles table) -
/// with audio low power active usecase
const char* PlatformPatternsUC::msRolesCoexistWithAlpUcc[] = {
  '\0' // table currently empty
};

/// CsCall roles
const char* PlatformPatternsUC::msCsCallRoles[] = {
  "CS_sink_source",
  "cscall*",
  '\0'
};

#endif  // _PLATFORMPATTERNSINIT_CPP_

}  // namespace

#endif
