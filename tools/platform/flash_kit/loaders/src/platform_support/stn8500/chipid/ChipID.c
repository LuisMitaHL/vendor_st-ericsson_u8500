/***********************************************************************
 * $Copyright Ericsson AB 2009 $
 **********************************************************************/

/**
 *  @addtogroup ldr_IO_subsystem
 *  @{
 */

/*************************************************************************
* Includes
*************************************************************************/
#include "c_system.h"
#include "t_basicdefinitions.h"
#include "services.h"
#include "ab8500_core_services.h"

#ifdef ST_HREFV2
#if defined(STE_AP9540) || defined(STE_AP8540)
#define CHIPIDADDR 0xFFFFDBF4
#else
/* The Chip ID can be found at 0x9001DBF4 according to documentation */
#define CHIPIDADDR 0x9001DBF4
#endif
#else
/* The Chip ID can be found at 0x9001FFF4 according to documentation */
#define CHIPIDADDR 0x9001FFF4
#endif

uint32 GetDBAsicChipID()
{
    return *((volatile uint32 *)CHIPIDADDR);
}

uint32 GetABAsicChipID()
{
#if (STE_PLATFORM_NUM == 8500) && !defined(STE_AP9540) && !defined(STE_AP8540)
    return (*(volatile uint32 *)(SOC_VERSION_ADDRESS));
#else
    return 0;
#endif
}

/** @} */

