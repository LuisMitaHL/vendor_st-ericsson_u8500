/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef AGPS_VERSION_H
#define AGPS_VERSION_H

/*LBS host software version should be changed unless there is addition of a new Feature.
   Major revision indicates indicates significant jump in functionality. It would be aligned with Localin version. (Maj ver 5 is localin 2.1)
   Minor revision number indicates a  feature addition.
   Release patch version indicates a bug fix / insignificant change in host software .
*/

#define LBS_MAJOR_VERSION                  6

#define LBS_MINOR_VERSION                  4

#define LBS_PATCH_VERSION                  8

#define LBS_CSL_PROJECT_IDENT              0   /* This will be unique identifier assigned to the different CSL projects */

#define LBS_CSL_PATCH_VERSION              0   /* This can be modified by CSL teams for local patch releases */

#endif /* AGPS_VERSION_H */

