/***********************************************************************
 * $Copyright Ericsson AB 2009 $
 **********************************************************************/

#ifdef DISABLE_SECURITY
#define SECURITY_ENABLED "false"
#else
#define SECURITY_ENABLED "true"
#endif

/**
 * var const unsigned char CurrentDate[]
 * brief ASCII string variable holding the loader version/build time/product number..
 */
__attribute__((section("versionstring")))
#define xstr(s) #s
#define str(s) xstr(s)
const unsigned char CurrentDate[] = "$Id: "__DATE__" "__TIME__" "LOADERNAME" "str(STE_PLATFORM_NUM)" HW:"STE_HW" SECURITY:"SECURITY_ENABLED" LDR:"LOADERVERSION " LCD/LCM:" LCVERSION " BASS_APP:" BASSAPVERSION " COPS:" COPSVERSION " CSPSA:" CSPSAVERSION " $";

/** @} */
