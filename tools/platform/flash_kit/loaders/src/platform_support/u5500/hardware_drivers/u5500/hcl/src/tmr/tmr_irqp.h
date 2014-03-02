/*****************************************************************************/
/**
*  © ST-Ericsson, 2009 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \brief   Private Header file of TMR module
* \author  ST-Ericsson
*/
/*****************************************************************************/

#ifndef _TMR_IRQP_H_
#define _TMR_IRQP_H_


#ifdef __cplusplus
extern "C"
{
#endif

/*------------------------------------------------------------------------
 * Defines
 *----------------------------------------------------------------------*/

/* Generic macros to access bit fields */
#define TMR_HCL_FIELDMASK(fieldId)  ((t_uint32) (((((t_uint32) (~0)) >> (32 - (fieldId##width))) << (fieldId##shift))))
#define TMR_HCL_FIELDSET(datum, fieldId, val) \
    (datum) = ((t_uint32) (datum) & (~TMR_HCL_FIELDMASK(fieldId))) | ((t_uint32) (((t_uint32) (val)) << ((t_uint32) (fieldId##shift))) & (TMR_HCL_FIELDMASK(fieldId)))
#define TMR_HCL_FIELDGET(datum, fieldId)    ((t_uint32) (((datum) & ((t_uint32) TMR_HCL_FIELDMASK(fieldId))) >> (fieldId##shift)))


#define TMR_INSTANCES_OF_TMR_UNITS          2

/*------------------------------------------------------------------------
 * New types
 *----------------------------------------------------------------------*/

/*this short structure is used to easily access to the registers*/
typedef volatile struct
{
    t_uint32    tmr_load;       /* @0 */
    t_uint32    tmr_value;      /* @4 */
    t_uint32    tmr_control;    /* @8 */
    t_uint32    tmr_bgload;     /* @12 */
} t_tmr_register_subset;

typedef volatile struct
{
    t_uint32    tmr_imsc;       /* @0 */
    t_uint32    tmr_ris;        /* @4 */
    t_uint32    tmr_mis;        /* @8 */
    t_uint32    tmr_icr;        /* @12 */

    t_uint32    tmr_load1;      /* @16 */
    t_uint32    tmr_value1;     /* @20 */
    t_uint32    tmr_control1;   /* @24 */
    t_uint32    tmr_bgload1;    /* @28 */

    t_uint32    tmr_load2;      /* @32 */
    t_uint32    tmr_value2;     /* @36 */
    t_uint32    tmr_control2;   /* @40 */
    t_uint32    tmr_bgload2;    /* @44 */

    t_uint32    tmr_load3;      /* @48 */
    t_uint32    tmr_value3;     /* @52 */
    t_uint32    tmr_control3;   /* @56 */
    t_uint32    tmr_bgload3;    /* @60 */

    t_uint32    tmr_load4;      /* @64 */
    t_uint32    tmr_value4;     /* @68 */
    t_uint32    tmr_control4;   /* @72 */
    t_uint32    tmr_bgload4;    /* @76 */

    t_uint32    tmr_unused1[(3840 - 80) / sizeof(t_uint32)];

    t_uint32    tmr_itcr;       /* @3840 */
    t_uint32    tmr_itop;       /* @3844 */

    t_uint32    tmr_unused2[(4064 - 3848) / sizeof(t_uint32)];

    t_uint32    tmr_periph_id0; /* @4064 */
    t_uint32    tmr_periph_id1; /* @4068 */
    t_uint32    tmr_periph_id2; /* @4072 */
    t_uint32    tmr_periph_id3; /* @4076 */
    t_uint32    tmr_pcell0;     /* @4080 */
    t_uint32    tmr_pcell1;     /* @4084 */
    t_uint32    tmr_pcell2;     /* @4088 */
    t_uint32    tmr_pcell3;     /* @4092 */
} t_tmr_register;

#define TMR_TIMER_IDS_IN_TMR_UNIT0          ((t_uint32) TMR_ID_0 | (t_uint32) TMR_ID_1 | (t_uint32) TMR_ID_2 | (t_uint32) TMR_ID_3)
#define TMR_TIMER_IDS_IN_TMR_UNIT1          ((t_uint32) TMR_ID_4 | (t_uint32) TMR_ID_5 | (t_uint32) TMR_ID_6 | (t_uint32) TMR_ID_7)



#define TMR_NUMBER_OF_TIMERS_IN_ONE_UNIT    4

#ifdef __cplusplus
}   /* allow C++ to use these headers */
#endif /* __cplusplus */
#endif /* _TMR_IRQP_H_*/

/* End of file - tmr_irqp.h*/

