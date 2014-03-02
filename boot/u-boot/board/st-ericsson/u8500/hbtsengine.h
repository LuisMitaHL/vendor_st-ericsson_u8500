/*------------------------------------------------------------------------
 * ST-Ericsson
 * Reproduction and Communication of this document is strictly prohibited
 * unless specifically authorized in writing by ST-Ericsson
 *------------------------------------------------------------------------
 * PROJECT:       MontBlanc / HBTS
 * FILE:          hbts_engine.h
 * DESCRIPTION:   header file for all functions of HBTS engine.
 *-----------------------------------------------------------------------*/

#ifndef __HBTS_ENGINE_H__
#define __HBTS_ENGINE_H__

/*************************************************
 * General Macros definition                     *
 *************************************************/

/* HBTS_VERSION must follow Clearcase baseline version */
#define HBTS_VERSION "2.0"

#define TRUE	1
#define FALSE	0

#define TEST_PASS         0     /* Test executed with result = PASS */
#define TEST_FAIL        -1     /* Test executed with result = FAIL */
#define TEST_NONE         1     /* Test not executed, no result available */

#define MOP_ED            1
#define MOP_V1            2
#define HREF_ED           3
#define HREF_V1           4
#define HREF_PLUS         5
#define MOP_V2            6
#define HREF_PLUS_2_0     7
#define HREF_PLUS_2_0_40  8
#define HREF_PLUS_2_0_60  9
#define HREF_PLUS_2_2_10  10

#define ARRAYSIZE(ar)	(sizeof(ar)/sizeof(ar[0]))

#define AB8500_ED_RELEASE    5
#define AB8500_V1_RELEASE   10
#define AB8500_V1_1_RELEASE 15
#define AB8500_V1_2_RELEASE 16
#define AB8500_V2_RELEASE   20
#define AB8500_V3_RELEASE   30
#define AB8500_V3_3_RELEASE 33

#define DB8500_ED_RELEASE    5
#define DB8500_V1_RELEASE   10
#define DB8500_V1_1_RELEASE 15
#define DB8500_V2_RELEASE   20
#define DB8500_V2_1_RELEASE 25
#define DB8500_V2_2_RELEASE 30

#define DB9540_V1_0_RELEASE   0xDB954010
#define AB9540_V1_0_RELEASE   0xAB954010

#define AV8100_1_1_RELEASE   1
#define AV8100_2_1_RELEASE   2
#define AV8100_2_2_RELEASE   3


/* + CAP_4513_001 DB8500 v2.0 cut id */
#if  (DB8500_HDW >= 20)
#define DB8500_CUT_ID_REG		0x9001DBF4
#else
#define DB8500_CUT_ID_REG		0x9001FFF4
#endif
/* - CAP_4513_001 DB8500 v2.0 cut id */
#define DB8500_CUT_ID_ED		0x00850001
#define DB8500_CUT_ID_V1		0x008500A0
#define DB8500_CUT_ID_V1_1		0x008500A1
/* + CAP_4513_001 DB8500 v2.0 cut id */
#define DB8500_CUT_ID_V2_0		0x008500B0
/* - CAP_4513_001 DB8500 v2.0 cut id */
#define DB8500_CUT_ID_V2_1		0x008500B1
#define DB8500_CUT_ID_V2_2		0x008500B2



/*************************************************
 * General Function definition                     *
 *************************************************/
void hbts_version(void);


/*************************************************
 * Formatted display functions                   *
 *************************************************/
void HBTS_DisplayTitle(char * blockname);
void HBTS_DisplayResult(char * blockname, int result);
void HBTS_DisplayDuration(unsigned int value);
void HBTS_DisplayReturnValue(int value);
void HBTS_printf(const char *fmt, ...);

/****************************************************************************
* DO NOT modify this enum.
****************************************************************************/
typedef enum
{
	HBTS_TIMER_32K,
	HBTS_TIMER_38M,

	HBTS_TIMER_NONE

} t_timer_type;

/****************************************************************************
* Time measurement functions
****************************************************************************/
unsigned int HBTS_TimersInit (void);
unsigned int HBTS_TimerAlloc (unsigned int * timer_id);
unsigned int HBTS_TimerFree  (unsigned int timer_id);
unsigned int HBTS_TimerStart (unsigned int timer_id, t_timer_type type);
unsigned int HBTS_TimerRead  (unsigned int timer_id);
unsigned int HBTS_TimerStop  (unsigned int timer_id);
unsigned int HBTS_TimerDelay (unsigned int delay, t_timer_type type);

#endif /* __HBTS_ENGINE_H__ */
