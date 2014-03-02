Instructions for building/running CSPSA in an OS-free environment (e.g. Loaders, ITP environment)
=================================================================================================

Building CSPSA
==============

The following files are needed at a minimum:

crc.c
crc.h
cspsa.h
cspsa_api_osfree.c
cspsa_core.c
cspsa_core.h
cspsa_ll.c
cspsa_ll.h
cspsa_plugin.h

In an OS free environment with a rather "normal" C lib (malloc, etc. available) it should be possible 
to build these files without setting any compiler flags.
When building in ITP environment, the files should be compiled with compiler switch -DCSPSA_ITP_CONFIG

Furthermore you need a plugin that can hook into CSPSA for provided media access. Currently only eMMC is 
provided which works in the loaders HCL environment. 
The following files provide eMMC support in a loaders environment (the C implementation must be adapted 
in an ITP environment): 

cspsa_ll_emmc.c
cspsa_ll_emmc.h

You can turn on logging of CSPSA operations on the eMMC by compiling cspsa_ll_emmc.c with compiler switch 
-DPRINT_A_, -DPRINT_B_ (more extensive), or -DPRINT_C_ (most extensive logging).

Using CSPSA
===========

No specific initialization function needs to be called to initialize CSPSA itself. But before using CSPSA,
parameter storage areas and CSPSA plugins must be registered to the CSPSA. 

In the eMMC case, you must call the following functions:

/** Registers the EMMC plugin to CSPSA. */
void CSPSA_LL_EMMC_Init(void);

/**
 * Finds the CSPSA TOC entries, and if it finds it, registers them to the CSPSA under name
 * <DeviceName_p>/TOC/<TOC id>, e.g. "/flash0/TOC/CSPSA0".
 *
 * @param [in] DeviceName_p First part of the CSPSA name that will be registered to CSPSA.
 *
 * @return The result of the operation.
 * @retval T_CSPSA_RESULT_OK If a trim area was found.
 */
CSPSA_Result_t RegisterEMMC_Areas(const char* DeviceName_p); 

for example:

#include "cspsa_ll_emmc.h"

...
  CSPSA_LL_EMMC_Init();
  Result = RegisterEMMC_Areas("/flash0");
  if (Result == T_CSPSA_RESULT_OK)
  {
    ...
  }

The RegisterEMMC_Areas function parses the TOC and looks for entries with TOC ID starting with "CSPSA"; 
all these TOC entries are registered as parameter storage areas.

In file cspsa.h all functions are defined that can be used to find registered CSPSA areas, open them, 
read/write/modify parameters etc. An example of usage can be found in example_osfree.c.


