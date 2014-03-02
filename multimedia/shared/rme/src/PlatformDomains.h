/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/*! ****************************************************************************
 * \file PlatformDomains.h
 * Platform domains static class interface
 *
 * \author philippe.tribolo@stericsson.com
 *
 * *****************************************************************************/
#ifndef _PLATFORMDOMAINS_H_
#define _PLATFORMDOMAINS_H_

#include <rme_types.h>
#if (defined(__STN_9540) || defined(__STN_8540))
#define MAPPING_x540
#endif
namespace rme {

/** Platform Domains static class */
class PlatformDomains {
 public:
  /// platform 'named' (NMF) domains (a.k.a. PNDs)
  enum PNDS_E {
    PND_NA = 0,  /// tag only: not a domain -- keep this fake PND + keep value to 0

    /* *************************** 'pure' DDR domains (i.e. no ESRAM) **************************/

    // generic processing domain: ARM + DDR unmigrable slot + NO ESRAM
    PND_PROCARM_DDR_GEN,
    // generic processing domain: SVA + DDR unmigrable slot + NO ESRAM
    PND_PROCSVA_DDR_GEN,
    // generic processing domain: SIA + DDR unmigrable slot + NO ESRAM
    PND_PROCSIA_DDR_GEN,

    /* **************************** 'mixed' domains (DDR + ESRAM)  ***************************/

    // Audio ESRAM low power
    PND_PROCSIA_DDRESRAM_AUDIOLOWPOWER,

    // Audio ESRAM (non low power)
    PND_PROCSIA_DDRESRAM_AUDIO,

    /* ************************** 'pure' ESRAM domains (i.e. no DDR) **************************/

    // ISP 'code' domain: SIA-XP70 + ESRAM ISP zone
    // corresponds to following domain in ESRAM management document:
    //     - domain4 'part of ISP code'
    PND_HWXP70SIA_ESRAM_ISP,

    // SVA HWpipe 'parent' domain for related scratch domains
    PND_HWPIPESVA_ESRAM_PARENT,

    // Video encode/decode 'DEFAULT' scratch dynamic data domain:
    // SVA-HWPIPE + ESRAM zone
    // => corresponds to multiple domains in ESRAM management document:
    // gathered in a single domain for simplicity sake
    //    - domain13 'FHD H264 encode dynamic data (no deblocking)'
    //    - domain32 'HDR H264 decode dynamic data'
    //    - domain46 'HDR VC1 decode dynamic data'
    PND_HWPIPESVA_ESRAM_DEFAULT_VIDENCDEC,

    // Video encode/decode 'SMALL' scratch dynamic data domain:
    // SVA-HWPIPE + ESRAM zone
    // => corresponds to following domain in ESRAM management document:
    //    - domain11 'VGA H264 encode dynamic data'
    //    - domain20 'HDR MPEG4 encode dynamic data'
    PND_HWPIPESVA_ESRAM_SMALL_VIDENCDEC,

    // Video encode/decode 'BIG' scratch dynamic data domain:
    // SVA-HWPIPE + ESRAM zone
    // => corresponds to following domain in ESRAM management document:
    //    - domain33 'FHD H264 decode dynamic data'
    PND_HWPIPESVA_ESRAM_BIG_VIDENCDEC,

    // Video encode/decode 'BIGGEST' scratch dynamic data domain:
    // SVA-HWPIPE + ESRAM zone
    // => corresponds to following domain in ESRAM management document:
    //    - domain12 'HDR H264 encode dynamic data'
    PND_HWPIPESVA_ESRAM_BIGGEST_VIDENCDEC,

    // SIA HWpipe 'parent' domain for related scratch domains
    PND_HWPIPESIA_ESRAM_PARENT,

    // Video grabbing+stabilization 'DEFAULT' scratch dynamic data domain:
    // SIA-HWPIPE + ESRAM zone
    // => corresponds to multiple domains in ESRAM management document:
    // gathered in a single domain for simplicity sake
    //    - domain60 'VGA grabbing + stab'
    //    - domain61 'HDR grabbing + stab'
    PND_HWPIPESIA_ESRAM_DEFAULT_VIDGRABSTAB,

    // Video grabbing+stabilization 'SMALL' scratch dynamic data domain:
    // SIA-HWPIPE + ESRAM zone
    // (=> no corresponding domain in ESRAM management document)
    PND_HWPIPESIA_ESRAM_SMALL_VIDGRABSTAB,

    // Video grabbing+stabilization 'BIG' scratch dynamic data domain:
    // SIA-HWPIPE + ESRAM zone
    // => corresponds to following domain in ESRAM management document:
    //    - domain62 'FHD grabbing + stab'
    PND_HWPIPESIA_ESRAM_BIG_VIDGRABSTAB,

    // Imaging burst still grabbing scratch dynamic data domain:
    // SIA-HWPIPE + ESRAM zone
    // => corresponds to following domain in ESRAM management document:
    //    - domain63 '(burst) still grabbing'
    PND_HWPIPESIA_ESRAM_BIGGEST_IMGSTILLGRAB,

    /* *********************** ************************/
    // number of named domains -- shall be last value
    PND_COUNT
  };

  /// domain HW mapping types
  typedef enum {
    /// default value for unknown or ARM domain type, shall be kept to 0
    DHM_NA = 0,

    /// NMF 'MMDSP' SVA domain type;
    DHM_PROCSVA,
    /// NMF MMDSP SIA domain type;
    DHM_PROCSIA,

    /// NMF SVA 'HWpipe' domain type
    DHM_HWPIPESVA,
    /// NMF SIA 'HWpipe' domain type
    DHM_HWPIPESIA,
    /// NMF 'SIA HWxp70' domain type
    DHM_HWXP70SIA
  } DHM_E;

  /// domain segment
  struct DomainSegment {
    unsigned int offset;  /// offset
    unsigned int size;    /// size in octets
  };
  /// domain descriptor
  struct DomainDesc {
    /// 'usecase' name (needed for debug only)
    const char* usecasename;
    /// custom RME identification: platform named domain
    PNDS_E pnd;
    /// custom RME domain type; implicitely defines core id;
    DHM_E type;
    /// SDRAM segment (code and data)
    DomainSegment sdram;
    /// ESRAM segment (code and data)
    DomainSegment esram;
    /// domain scratch id
    ///   - if id > 0 domain is scratch; scratch zones have EMPTY CODE sections
    ///   - if id == 0 domain is non scratch
    ///   - if id < 0 domain is non scratch but parent of scratch domains with ids equal to (-id)
    int scratchid;
    /// ESRAM domain 'fast' identification (if applicable)
    RM_ESRAMBUFID_E esramfid;
  };

 private:
  // friend platform domains manager
  friend class PfDomManager;
  /// domain descriptors table
  static const struct PlatformDomains::DomainDesc msDomains[PND_COUNT];
};

// domain descriptors table init
#ifdef _PLATFORMDOMAINSINIT_CPP_


// DDR MPC
#define DDR_MPC_OFFSET 0x40000 // 256kB octets: NMF CM provided value
#define DDR_MPC_SIZE (0x800000 - DDR_MPC_OFFSET) // 8Mo - NMF CM

// ESRAM MPC segmentation in order: DMAC zone, ISP (XP70-SIA) zone, VIDEO (SVA) zone, IMAGING (SIA) zone, AUDIO (SIA) zone
// DMAC zone is *not* usable/sharable by audio/video/imaging media; DMAC domain is not created by RME directly
// ISP zone is single domain
// VIDEO zone sub scratch/overlayable domains are *bottom*-aligned
// IMAGING zone sub scratch/overlayable domains are *top*-aligned
// AUDIO zone is single domain

// full MPC ESRAM zone
#ifdef MAPPING_x540
#define ESRAM_MPC_FULL_SIZE 0x65800 // 406kB: 3.171875 banks of 128kB -
#else
#define ESRAM_MPC_FULL_SIZE 0x70000 // 448kB: 3.5 banks of 128kB -
#endif
// DMAC ESRAM zone
#define ESRAM_MPC_DMAC_SIZE 0x1000 // 4kB (domain1 DMAC)

// ISP ESRAM zone
#define ESRAM_MPC_ISP_SIZE 0xF000  // 60kB

// AUDIO ESRAM zone
#ifdef SW_VARIANT_ANDROID
#ifdef MAPPING_x540
#define ESRAM_MPC_AUDIO_SIZE 0x2800  // 10kB
#else
#define ESRAM_MPC_AUDIO_SIZE 0x7800  // 30kB
#endif
#else
#define ESRAM_MPC_AUDIO_SIZE 0x1800  // 6kB
#endif

// IMAGING STILL GRAB ESRAM ZONE
#ifdef SW_VARIANT_ANDROID
// caution: don't modify size without taking care of hard coded value in Android imaging code (ISPPROC omx comp)..
#ifdef MAPPING_x540
#define ESRAM_MPC_IMGSTILLGRAB_SIZE 0x30000  // 192kB
#else
#define ESRAM_MPC_IMGSTILLGRAB_SIZE 0x3C800  // 242kB
#endif
#else
#define ESRAM_MPC_IMGSTILLGRAB_SIZE 0x2FC00  // 191kB
#endif

/// domain descriptors table
const struct PlatformDomains::DomainDesc PlatformDomains::msDomains[] = {
  /* **************************** 'pure' DDR domains (i.e. no ESRAM)  ***************************/

  { "GEN",
    PND_PROCARM_DDR_GEN,
    DHM_NA,  // associated to ARM_CORE_ID
    {DDR_MPC_OFFSET, DDR_MPC_SIZE},  // SDRAM section {offset, size}
    {ESRAM_MPC_DMAC_SIZE, 0x0},  // ESRAM section {offset, size}
    0,  // non scratch
    RM_ESBUFID_EMPTY  // n.a.
  },
  { "GEN",
    PND_PROCSVA_DDR_GEN,
    DHM_PROCSVA,  // associated to SVA_CORE_ID
    {DDR_MPC_OFFSET, DDR_MPC_SIZE},  // SDRAM section {offset, size}
    {ESRAM_MPC_DMAC_SIZE, 0x0},  // ESRAM section {offset, size}
    0,  // non scratch
    RM_ESBUFID_EMPTY  // n.a.
  },
  { "GEN",
    PND_PROCSIA_DDR_GEN,
    DHM_PROCSIA,  // associated to SIA_CORE_ID
    {DDR_MPC_OFFSET, DDR_MPC_SIZE},  // SDRAM section {offset, size}
    {ESRAM_MPC_DMAC_SIZE, 0x0},  // ESRAM section {offset, size}
    0,  // non scratch
    RM_ESBUFID_EMPTY  // n.a.
  },

  /* **************************** 'mixed' domains (DDR + ESRAM)  ***************************/

  // ******* AUDIO-SIA 'Low Power' domain *******
  { "AUDIO_LOWPOWER",
    PND_PROCSIA_DDRESRAM_AUDIOLOWPOWER,
    DHM_PROCSIA,  // associated to SIA_CORE_ID
    {DDR_MPC_OFFSET, DDR_MPC_SIZE},  // SDRAM section {offset, size}
#ifdef MAPPING_x540
    {ESRAM_MPC_DMAC_SIZE, ESRAM_MPC_FULL_SIZE - ESRAM_MPC_DMAC_SIZE - ESRAM_MPC_ISP_SIZE},  // ESRAM section {offset, size}: full MPC zone except DMAC zone and ISP zone
#else
    {ESRAM_MPC_DMAC_SIZE + ESRAM_MPC_ISP_SIZE, ESRAM_MPC_FULL_SIZE - ESRAM_MPC_DMAC_SIZE - ESRAM_MPC_ISP_SIZE},  // ESRAM section {offset, size}: full MPC zone except DMAC zone and ISP zone
#endif
    0,  // non scratch
    RM_ESBUFID_EMPTY  // n.a. : no ESRAM fast id associated to this PND
  },

  // ******* AUDIO-SIA 'non Low Power' domain : at top of MPC zone *******
  { "AUDIO",
    PND_PROCSIA_DDRESRAM_AUDIO,
    DHM_PROCSIA,  // associated to SIA_CORE_ID
    {DDR_MPC_OFFSET, DDR_MPC_SIZE},  // SDRAM section {offset, size}
#ifdef MAPPING_x540
    {ESRAM_MPC_FULL_SIZE - ESRAM_MPC_ISP_SIZE - ESRAM_MPC_AUDIO_SIZE, ESRAM_MPC_AUDIO_SIZE},  // ESRAM section {offset, size}
#else
    {ESRAM_MPC_FULL_SIZE - ESRAM_MPC_AUDIO_SIZE, ESRAM_MPC_AUDIO_SIZE},  // ESRAM section {offset, size}
#endif
    0,  // non scratch
    RM_ESBUFID_EMPTY  // n.a. : no ESRAM fast id associated to this PND
  },

  /* *************************** 'pure' ESRAM domains (i.e. no DDR) ***************************/

  // ******* ISP-SIA-XP70 domain : above DMAC zone *******
  { "ISP",
    PND_HWXP70SIA_ESRAM_ISP,
    DHM_HWXP70SIA,  // associated to SIA_CORE_ID
    {DDR_MPC_OFFSET, 0},  // SDRAM section {offset, size}
#ifdef MAPPING_x540
    {ESRAM_MPC_FULL_SIZE - ESRAM_MPC_ISP_SIZE, ESRAM_MPC_ISP_SIZE},  // ESRAM section {offset, size}
#else
    {ESRAM_MPC_DMAC_SIZE, ESRAM_MPC_ISP_SIZE},  // ESRAM section {offset, size}
#endif
    0,  // non scratch
    RM_ESBUFID_DEFAULT  // ESRAM buffer fast id
  },

  // ******* SVA-HWPIPE scratch domains (+parent domain) : above IPC zone, bottom aligned *******
  { "PARENT",
    PND_HWPIPESVA_ESRAM_PARENT,
    DHM_HWPIPESVA,  // associated to SVA_CORE_ID
    {DDR_MPC_OFFSET, 0},  // SDRAM section {offset, size}
#ifdef MAPPING_x540
    {ESRAM_MPC_DMAC_SIZE, 0x38000},  // ESRAM section {offset:ISP_SIZE, size:224kB}
#else
    {ESRAM_MPC_DMAC_SIZE + ESRAM_MPC_ISP_SIZE, 0x44C00},  // ESRAM section {offset:ISP_SIZE, size:275kB}
#endif
    -1,  // non scratch, PARENT of domain group 1
    RM_ESBUFID_EMPTY  // n.a.
  },
  { "VIDENCDEC",
    PND_HWPIPESVA_ESRAM_DEFAULT_VIDENCDEC,
    DHM_HWPIPESVA,  // associated to SVA_CORE_ID
    {DDR_MPC_OFFSET, 0},  // SDRAM section {offset, size}
#ifdef MAPPING_x540
    {ESRAM_MPC_DMAC_SIZE, 0xAC00},  // ESRAM section {offset:ISP_SIZE, size:43kB}
#else
    {ESRAM_MPC_DMAC_SIZE + ESRAM_MPC_ISP_SIZE, 0x2EC00},  // ESRAM section {offset:ISP_SIZE, size:187kB}
#endif
    1,  // scratch in domain group 1
    RM_ESBUFID_DEFAULT  // ESRAM buffer fast id: default
  },
  { "VIDENCDEC",
    PND_HWPIPESVA_ESRAM_SMALL_VIDENCDEC,
    DHM_HWPIPESVA,  // associated to SVA_CORE_ID
    {DDR_MPC_OFFSET, 0},  // SDRAM section {offset, size}
#ifdef MAPPING_x540
    {ESRAM_MPC_DMAC_SIZE, 0xAC00},  // ESRAM section {offset:ISP_SIZE, size:43kB}
#else
    {ESRAM_MPC_DMAC_SIZE + ESRAM_MPC_ISP_SIZE, 0x22000},  // ESRAM section {offset:ISP_SIZE, size:136kB}
#endif
    1,  // scratch in domain group 1
    RM_ESBUFID_SMALL  // ESRAM buffer fast id: small
  },
  { "VIDENCDEC",
    PND_HWPIPESVA_ESRAM_BIG_VIDENCDEC,
    DHM_HWPIPESVA,  // associated to SVA_CORE_ID
    {DDR_MPC_OFFSET, 0},  // SDRAM section {offset, size}
#ifdef MAPPING_x540
    {ESRAM_MPC_DMAC_SIZE, 0x21C00},  // ESRAM section {offset:ISP_SIZE, size:135kB}
#else
    {ESRAM_MPC_DMAC_SIZE + ESRAM_MPC_ISP_SIZE, 0x38800},  // ESRAM section {offset:ISP_SIZE, size:226kB}
#endif
    1,  // scratch in domain group 1
    RM_ESBUFID_BIG  // ESRAM buffer fast id: big
  },
  { "VIDENCDEC",
    PND_HWPIPESVA_ESRAM_BIGGEST_VIDENCDEC,
    DHM_HWPIPESVA,  // associated to SVA_CORE_ID
    {DDR_MPC_OFFSET, 0},  // SDRAM section {offset, size}
#ifdef MAPPING_x540
    {ESRAM_MPC_DMAC_SIZE, 0x38000},  // ESRAM section {offset:ISP_SIZE, size:224kB}
#else
    {ESRAM_MPC_DMAC_SIZE + ESRAM_MPC_ISP_SIZE, 0x44C00},  // ESRAM section {offset:ISP_SIZE, size:275kB}
#endif
    1,  // scratch in domain group 1
    RM_ESBUFID_BIGGEST  // ESRAM buffer fast id: biggest
  },

  // ******* SIA-HWPIPE scratch domains (+parent domain) : below AUDIO zone, top aligned *******
  { "PARENT",
    PND_HWPIPESIA_ESRAM_PARENT,
    DHM_HWPIPESIA,  // associated to SIA_CORE_ID
    {DDR_MPC_OFFSET, 0},  // SDRAM section {offset, size}
#ifdef MAPPING_x540
    {ESRAM_MPC_FULL_SIZE - ESRAM_MPC_ISP_SIZE - ESRAM_MPC_AUDIO_SIZE - ESRAM_MPC_IMGSTILLGRAB_SIZE, ESRAM_MPC_IMGSTILLGRAB_SIZE},  // ESRAM section {offset:, size:}
#else
    {ESRAM_MPC_FULL_SIZE - ESRAM_MPC_AUDIO_SIZE - ESRAM_MPC_IMGSTILLGRAB_SIZE, ESRAM_MPC_IMGSTILLGRAB_SIZE},  // ESRAM section {offset:, size:}
#endif
    -2,  // non scratch, PARENT of domain group 2
    RM_ESBUFID_EMPTY  // n.a.
  },
  { "VIDGRABSTAB",
    PND_HWPIPESIA_ESRAM_DEFAULT_VIDGRABSTAB,
    DHM_HWPIPESIA,  // associated to SIA_CORE_ID
    {DDR_MPC_OFFSET, 0},  // SDRAM section {offset, size}
#ifdef MAPPING_x540
    {ESRAM_MPC_FULL_SIZE - ESRAM_MPC_ISP_SIZE - ESRAM_MPC_AUDIO_SIZE - 0x1B000, 0x1B000},  // ESRAM section {offset:, size:108kB}
#else
    {ESRAM_MPC_FULL_SIZE - ESRAM_MPC_AUDIO_SIZE - 0x15400, 0x15400},  // ESRAM section {offset:, size:85kB}
#endif
    2,  // scratch in domain group 2
    RM_ESBUFID_DEFAULT  // ESRAM buffer fast id: default
  },
  { "VIDGRABSTAB",
    PND_HWPIPESIA_ESRAM_SMALL_VIDGRABSTAB,
    DHM_HWPIPESIA,  // associated to SIA_CORE_ID
    {DDR_MPC_OFFSET, 0},  // SDRAM section {offset, size}
#ifdef MAPPING_x540
    {ESRAM_MPC_FULL_SIZE - ESRAM_MPC_ISP_SIZE - ESRAM_MPC_AUDIO_SIZE - 0x12000, 0x12000},  // ESRAM section {offset:, size:72kB}
#else
    {ESRAM_MPC_FULL_SIZE - ESRAM_MPC_AUDIO_SIZE - 0x10000, 0x10000},  // ESRAM section {offset:, size:64kB}
#endif
    2,  // scratch in domain group 2
    RM_ESBUFID_SMALL // ESRAM buffer fast id: small
  },
  { "VIDGRABSTAB",
    PND_HWPIPESIA_ESRAM_BIG_VIDGRABSTAB,
    DHM_HWPIPESIA,  // associated to SIA_CORE_ID
    {DDR_MPC_OFFSET, 0},  // SDRAM section {offset, size}
#ifdef MAPPING_x540
    {ESRAM_MPC_FULL_SIZE - ESRAM_MPC_ISP_SIZE - ESRAM_MPC_AUDIO_SIZE - 0x1B000, 0x1B000},  // ESRAM section {offset:, size:108kB}
#else
    {ESRAM_MPC_FULL_SIZE - ESRAM_MPC_AUDIO_SIZE - 0x1FC00, 0x1FC00},  // ESRAM section {offset:, size:127kB}
#endif
    2,  // scratch in domain group 2
    RM_ESBUFID_BIG  // ESRAM buffer fast id: big
  },
  { "IMGSTILLGRAB",
    PND_HWPIPESIA_ESRAM_BIGGEST_IMGSTILLGRAB,
    DHM_HWPIPESIA,  // associated to SIA_CORE_ID
    {DDR_MPC_OFFSET, 0},  // SDRAM section {offset, size}
#ifdef MAPPING_x540
    {ESRAM_MPC_FULL_SIZE - ESRAM_MPC_ISP_SIZE - ESRAM_MPC_AUDIO_SIZE - ESRAM_MPC_IMGSTILLGRAB_SIZE, ESRAM_MPC_IMGSTILLGRAB_SIZE},  // ESRAM section {offset:, size:}
#else
    {ESRAM_MPC_FULL_SIZE - ESRAM_MPC_AUDIO_SIZE - ESRAM_MPC_IMGSTILLGRAB_SIZE, ESRAM_MPC_IMGSTILLGRAB_SIZE},  // ESRAM section {offset:, size:}
#endif
    2,  // scratch in domain group 2
    RM_ESBUFID_BIGGEST  // ESRAM buffer fast id: biggest
  }
};  // mDomains

#endif  // _PLATFORMDOMAINSINIT_CPP_

}  // namespace

#endif
