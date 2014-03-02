/*****************************************************************************/
/**
*  © ST-Ericsson, 2009 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \brief   Private Header file of System Trace Module (STM)          
* \author  ST-Ericsson
*/
/*****************************************************************************/

#ifndef _STMP_H_
#define _STMP_H_

/*--------------------------------------------------------------------------*
 * Includes																	*
 *--------------------------------------------------------------------------*/
#include "stm.h"

/*--------------------------------------------------------------------------*
 * Defines																	*
 *--------------------------------------------------------------------------*/

 #define STM_WRITE_BITS(reg, val, mask, sb)  ((reg) = (((reg) &~(mask)) | (((val) << (sb)) & (mask))))
 #define STM_READ_BITS(reg,mask,sb)            (reg & (mask << sb))

/*STM peripheral ids*/
#define STM_CELL_ID0        0x0D
#define STM_CELL_ID1        0xF0
#define STM_CELL_ID2        0x05
#define STM_CELL_ID3        0xB1

#define STM_P_ID0           0xEC
#define STM_P_ID1           0x0D
#define STM_P_ID2           0x18
#define STM_P_ID3           0x00

 #define STM_PARTNUMBER0     0xEB  /* STM part number 0 in periphiral id 0 */
 #define STM_PARTNUMBER1     0x0D  /* STM part number 1 in periphiral id 1 */
 #define STM_DESIGNER0       0x00  /* STM designer 0    in periphiral id 1 */ 
 #define STM_DESIGNER1       0x08  /* STM designer 1    in periphiral id 2 */
 #define STM_REVISION        0x00  /* STM revision      in periphiral id 2 */ 
 #define STM_CONFIGURATION   0x00  /* STM configuration in periphiral id 3 */ 
 
 
 #define STM_CELLID0         0x0D  /* STM Cell Id 0 */
 #define STM_CELLID1         0xF0  /* STM Cell Id 1 */
 #define STM_CELLID2         0x05  /* STM Cell Id 2 */
 #define STM_CELLID3         0xB1  /* STM Cell Id 3 */
 
 
 #define STM_MASKPARTNUM1    0x0F  /* Mask to obtain part number 1 from periphiral id 1 */
 #define STM_MASKDESIGNER0   0xF0  /* Mask to obtain designer 0 from periphiral id 1    */
 #define STM_MASKDESIGNER1   0x0F  /* Mask to obtain designer 0 from periphiral id 2    */
 #define STM_MASKREVISION    0xF0  /* Mask to obtain revision from periphiral id 2      */


/*------------------------------------------------------------------------------*
 * STM Configuration Register                                                   *
 *------------------------------------------------------------------------------*/
  
 #define STM_MASK_PLP0              0x01
 #define STM_SB_PLP0                0x00

 #define STM_MASK_PLP1              0x02
 #define STM_SB_PLP1                0x01

 #define STM_MASK_DWNG              0x30
 #define STM_SB_DWNG                0x04

 #define STM_MASK_SWAP_PRCMU        0x200
 #define STM_SB_SWAP_PRCMU          0x09

 #define STM_MASK_XCKDIV            0x1C0
 #define STM_SB_XCKDIV              0x06

 #define STM_MASK_HWMASTER0         0x01
 #define STM_SB_HWMASTER0           0x00

 #define STM_MASK_HWMASTER1         0x02
 #define STM_SB_HWMASTER1           0x01

 #define STM_MASK_HWMASTER2         0x04
 #define STM_SB_HWMASTER2           0x02

 #define STM_MASK_HWMASTER3         0x08
 #define STM_SB_HWMASTER3           0x03

 #define STM_MASK_HWMASTER4         0x10
 #define STM_SB_HWMASTER4           0x04
 
 #define STM_MASK_HWMASTER5         0x20
 #define STM_SB_HWMASTER5           0x05

/*------------------------------------------------------------------------------*
 * STM Trace Enable Register                                                    *
 *------------------------------------------------------------------------------*/

 #define STM_MASK_TE0               0x01
 #define STM_SB_TE0                 0x00

 #define STM_MASK_TE2               0x04
 #define STM_SB_TE2                 0x02

 #define STM_MASK_TE3               0x08
 #define STM_SB_TE3                 0x03

 #define STM_MASK_TE4               0x10
 #define STM_SB_TE4                 0x04

 #define STM_MASK_TE5               0x20
 #define STM_SB_TE5                 0x05

 #define STM_MASK_TE9               0x200
 #define STM_SB_TE9                 0x09

/*------------------------------------------------------------------------------*
 * STM Trace Disable Register                                                   *
 *------------------------------------------------------------------------------*/

 #define STM_READ_MASK_T0           0x001
 #define STM_READ_SB_T0             0x000

 #define STM_READ_MASK_T1           0x002
 #define STM_READ_SB_T1             0x001

 #define STM_READ_MASK_T2           0x004
 #define STM_READ_SB_T2             0x002

 #define STM_READ_MASK_T3           0x008
 #define STM_READ_SB_T3             0x003

 #define STM_READ_MASK_T4           0x010
 #define STM_READ_SB_T4             0x004

 #define STM_READ_MASK_T5           0x020
 #define STM_READ_SB_T5             0x005

 #define STM_READ_MASK_T9           0x200
 #define STM_READ_SB_T9             0x009


/*---------------------------------------------------------------------------*
 * STM Overflow Status Register                                         *
 *---------------------------------------------------------------------------*/
 
 #define STM_READ_MASK_OVF0           0x001
 #define STM_READ_SB_OVF0             0x000

 #define STM_READ_MASK_OVF1           0x002
 #define STM_READ_SB_OVF1             0x001

 #define STM_READ_MASK_OVF2           0x004
 #define STM_READ_SB_OVF2             0x002

 #define STM_READ_MASK_OVF3           0x008
 #define STM_READ_SB_OVF3             0x003

 #define STM_READ_MASK_OVF4           0x010
 #define STM_READ_SB_OVF4             0x004

 #define STM_READ_MASK_OVF5           0x020
 #define STM_READ_SB_OVF5             0x005

 #define STM_READ_MASK_OVF9           0x200
 #define STM_READ_SB_OVF9             0x009

 #define STM_READ_MASK_SECERR       0x001
 #define STM_READ_SB_SECERR         0x00A

 #define STM_READ_MASK_DECERR       0x001
 #define STM_READ_SB_DECERR         0x00B

 #define STM_READ_MASK_ADDERR       0x00F
 #define STM_READ_SB_ADDERR         0x00C

/*---------------------------------------------------------------------------*
 * STM Transmit FIFO Status Register                                         *
 *---------------------------------------------------------------------------*/

 #define STM_READ_MASK_RAM_EMPTY            0x01
 #define STM_READ_SB_RAM_EMPTY              0x00

 #define STM_READ_MASK_RAM_FULL             0x02
 #define STM_READ_SB_RAM_FULL               0x01
 
 #define STM_READ_MASK_PRCMU_EMPTY          0x04
 #define STM_READ_SB_PRCMU_EMPTY            0x02

 #define STM_READ_MASK_PRCMU_FULL           0x08
 #define STM_READ_SB_PRCMU_FULL             0x03

 #define STM_READ_MASK_CHGCONF              0x10
 #define STM_READ_SB_CHGCONF                0x04

/*---------------------------------------------------------------------------*
 * STM SBAG FIFO Status Register                                             *
 *---------------------------------------------------------------------------*/

 #define STM_READ_MASK_SBAG_FIFOEMPTY_FLAG  0x01
 #define STM_READ_SB_SBAG_FIFOEMPTY_FLAG    0x00

 #define STM_READ_MASK_SBAG_FIFOFULL_FLAG   0x02
 #define STM_READ_SB_SBAG_FIFOFULL_FLAG     0x01

 #define STM_READ_MASK_SBAG_OVERFLOW_FLAG   0x04
 #define STM_READ_SB_SBAG_OVERFLOW_FLAG     0x02

 #define STM_READ_MASK_SBAG_OVERFLOW_NUM    0xF1
 #define STM_READ_SB_SBAG_OVERFLOW_NUM      0x03

 #define STM_READ_MASK_SBAG_MISCOUNT        0x1000
 #define STM_READ_SB_SBAG_MISCOUNT          0x0C

 #define STM_READ_MASK_SBAG_BYTENB          0x2000
 #define STM_READ_SB_SBAG_BYTENB            0x0D

/*---------------------------------------------------------------------------*
 * STM Debug Register                                                        *
 *---------------------------------------------------------------------------*/

 #define STM_READ_MASK_DEBUG_ENABLE         0x01
 #define STM_READ_SB_DEBUG_ENABLE           0x00

 #define STM_READ_MASK_DEBUG_MODE           0x02
 #define STM_READ_SB_DEBUG_MODE             0x01

 #define STM_READ_MASK_DEBUG_PTICALIB       0x07
 #define STM_READ_SB_DEBUG_PTICALIB         0x03

 #define STM_READ_MASK_DEBUG_FTFSEL         0x1F
 #define STM_READ_SB_DEBUG_FTFSEL           0x06

 #define STM_READ_MASK_DEBUG_FSBFF          0x01
 #define STM_READ_SB_DEBUG_FSBFF            0x0D

 #define STM_READ_MASK_DEBUG_PTISTATE       0xFF
 #define STM_READ_SB_DEBUG_PTISTATE         0x10

/*---------------------------------------------------------------------------*
 * STM TimeStamp Register                                                    *
 *---------------------------------------------------------------------------*/

 #define STM_READ_MASK_TIMESTAMP            0xFF
 #define STM_READ_SB_TIMESTAMP              0x00

/*---------------------------------------------------------------------------*
 * STM Sleep Clock Counter Register                                          *
 *---------------------------------------------------------------------------*/

 #define STM_READ_MASK_SLEEPCLOCK           0xFFFFFFFF
 #define STM_READ_SB_SLEEPCLOCK             0x00

 
 
 #define STM_MASKTSCDIV      0x1C  /* Mask to set the time stamp clock divider  */
 #define STM_SBTSCDIV        0x02  /* The number of bits that should be shifted */
 
 #define STM_MASKXCKDIV      0x03  /* Mask to set the clock divisor for STM signal */
 #define STM_SBXCKDIV        0x00  /* The number of bits that should be shifted */
 
 #define STM_MASKTBE         0x02  /* Mask to get the status of transmit buffer empty */
  
 #define STM_MASKTBF         0x01  /* Mask to get the status of transmit buffer full */
 
 #define STM_MASKOSMO0       0x01  /* Mask to enable the CPU OSMO message */
 #define STM_SBOSMO0         0x00  /* The number of bits that should be shifted */
 
 #define STM_MASKOSMO1       0x02  /* Mask to enable the MM-DSP OSMO message */
 #define STM_SBOSMO1         0x01  /* The number of bits that should be shifted */
 
 #define STM_MASKOSMO2       0x03  /* Mask to enable both of the OSMO message */
 #define STM_SBOSMO2         0x00  /* The number of bits that should be shifted */
 
 #define STM_ENABLE          0x01  /* STM Trace enable bit */
 #define STM_DISABLE         0x01  /* STM trace disable bit */   
 #define STM_ENABLE_BOTH     0x03  /* STM Trace enable both the ports */

 #define STM_MASKTSR         0xFFFFFFFF /* Mask for the STM time stamp register */
 #define STM_SBTSR           0x00       /* The number of bits that should be shifted */
 
 #define STM_MASKITEN        0x01  /* Mask to select between normal mode or integration test mode */
 #define STM_SBITEN          0x00  /* The number of bits that should be shifted */ 
 
 #define STM_MASKITOPSTMCLK  0x01  /* Mask to select the STM CLK bit in integration output register */
 #define STM_SBITOPSTMCLK    0x00  /* The number of bits that should be shifted */ 
 
 #define STM_MASKITOPSTMDAT3 0x01  /* Mask to select the STM data 3 bit in integration output register */
 #define STM_SBITOPSTMDAT3   0x01  /* The number of bits that should be shifted */ 

 #define STM_MASKITOPSTMDAT2 0x01  /* Mask to select the STM data 2 bit in integration output register */
 #define STM_SBITOPSTMDAT2   0x02  /* The number of bits that should be shifted */ 

 #define STM_MASKITOPSTMDAT1 0x01  /* Mask to select the STM data 1 bit in integration output register */
 #define STM_SBITOPSTMDAT1   0x03  /* The number of bits that should be shifted */ 
 
 #define STM_MASKITOPSTMDAT0 0x01  /* Mask to select the STM data 0 bit in integration output register */
 #define STM_SBITOPSTMDAT0   0x04  /* The number of bits that should be shifted */  


 #define STM_SET_PLP0(reg,val)                  STM_WRITE_BITS(reg,val,STM_MASK_PLP0,STM_SB_PLP0)
 #define STM_SET_PLP1(reg,val)                  STM_WRITE_BITS(reg,val,STM_MASK_PLP1,STM_SB_PLP1)
 #define STM_SET_DWNG(reg,val)                  STM_WRITE_BITS(reg,val,STM_MASK_DWNG,STM_SB_DWNG)
 #define STM_SET_XCKDIV(reg,val)                STM_WRITE_BITS(reg,val,STM_MASK_XCKDIV,STM_SB_XCKDIV)
 #define STM_SET_PRCMU(reg,val)                 STM_WRITE_BITS(reg,val,STM_MASK_SWAP_PRCMU,STM_SB_SWAP_PRCMU)

 #define STM_SET_HWNSW0(reg,val)                STM_WRITE_BITS(reg,val,STM_MASK_HWMASTER0,STM_SB_HWMASTER0)
 #define STM_SET_HWNSW1(reg,val)                STM_WRITE_BITS(reg,val,STM_MASK_HWMASTER1,STM_SB_HWMASTER1)
 #define STM_SET_HWNSW2(reg,val)                STM_WRITE_BITS(reg,val,STM_MASK_HWMASTER2,STM_SB_HWMASTER2)
 #define STM_SET_HWNSW3(reg,val)                STM_WRITE_BITS(reg,val,STM_MASK_HWMASTER3,STM_SB_HWMASTER3)
 #define STM_SET_HWNSW4(reg,val)                STM_WRITE_BITS(reg,val,STM_MASK_HWMASTER4,STM_SB_HWMASTER4)
 #define STM_SET_HWNSW5(reg,val)                STM_WRITE_BITS(reg,val,STM_MASK_HWMASTER5,STM_SB_HWMASTER5)

 #define STM_SET_TE0(reg,val)                   STM_WRITE_BITS(reg,val,STM_MASK_TE0,STM_SB_TE0);
 #define STM_SET_TE1(reg,val)                   STM_WRITE_BITS(reg,val,STM_MASK_TE1,STM_SB_TE1);
 #define STM_SET_TE2(reg,val)                   STM_WRITE_BITS(reg,val,STM_MASK_TE2,STM_SB_TE2);
 #define STM_SET_TE3(reg,val)                   STM_WRITE_BITS(reg,val,STM_MASK_TE3,STM_SB_TE3);
 #define STM_SET_TE4(reg,val)                   STM_WRITE_BITS(reg,val,STM_MASK_TE4,STM_SB_TE4);
 #define STM_SET_TE5(reg,val)                   STM_WRITE_BITS(reg,val,STM_MASK_TE5,STM_SB_TE5);
 #define STM_SET_TE9(reg,val)                   STM_WRITE_BITS(reg,val,STM_MASK_TE9,STM_SB_TE9);
 #define STM_GET_TD0(reg)                       STM_READ_BITS(reg,STM_READ_MASK_T0,STM_READ_SB_T0);
 #define STM_GET_TD1(reg)                       STM_READ_BITS(reg,STM_READ_MASK_T1,STM_READ_SB_T1);
 #define STM_GET_TD2(reg)                       STM_READ_BITS(reg,STM_READ_MASK_T2,STM_READ_SB_T2);
 #define STM_GET_TD3(reg)                       STM_READ_BITS(reg,STM_READ_MASK_T3,STM_READ_SB_T3);
 #define STM_GET_TD4(reg)                       STM_READ_BITS(reg,STM_READ_MASK_T4,STM_READ_SB_T4);
 #define STM_GET_TD5(reg)                       STM_READ_BITS(reg,STM_READ_MASK_T5,STM_READ_SB_T5);
 #define STM_GET_TD9(reg)                       STM_READ_BITS(reg,STM_READ_MASK_T9,STM_READ_SB_T9);

 #define STM_GET_OV0(reg)                       STM_READ_BITS(reg,STM_READ_MASK_T0,STM_READ_SB_T0);
 #define STM_GET_OV1(reg)                       STM_READ_BITS(reg,STM_READ_MASK_T1,STM_READ_SB_T1);
 #define STM_GET_OV2(reg)                       STM_READ_BITS(reg,STM_READ_MASK_T2,STM_READ_SB_T2);
 #define STM_GET_OV3(reg)                       STM_READ_BITS(reg,STM_READ_MASK_T3,STM_READ_SB_T3);
 #define STM_GET_OV4(reg)                       STM_READ_BITS(reg,STM_READ_MASK_T4,STM_READ_SB_T4);
 #define STM_GET_OV5(reg)                       STM_READ_BITS(reg,STM_READ_MASK_T5,STM_READ_SB_T5);
 #define STM_GET_OV9(reg)                       STM_READ_BITS(reg,STM_READ_MASK_T9,STM_READ_SB_T9);
 #define STM_GET_SECERR(reg)                    STM_READ_BITS(reg,STM_READ_MASK_SECERR,STM_READ_SB_SECERR);
 #define STM_GET_DECERR(reg)                    STM_READ_BITS(reg,STM_READ_MASK_DECERR,STM_READ_SB_DECERR);
 #define STM_GET_ADDERR(reg)                    STM_READ_BITS(reg,STM_READ_MASK_ADDERR,STM_READ_SB_ADDERR);

 #define STM_GET_RAM_EMPTY_STATUS(reg)          STM_READ_BITS(reg,STM_READ_MASK_RAM_EMPTY,STM_READ_SB_RAM_EMPTY);
 #define STM_GET_RAM_FULL_STATUS(reg)           STM_READ_BITS(reg,STM_READ_MASK_RAM_EMPTY,STM_READ_SB_RAM_FULL);
 #define STM_GET_CHGCONF_STATUS(reg)            STM_READ_BITS(reg,STM_READ_MASK_CHGCONF,STM_READ_SB_CHGCONF);
 #define STM_GET_PRCMU_FULL_STATUS(reg)         STM_READ_BITS(reg,STM_READ_MASK_PRCMU_FULL,STM_READ_SB_PRCMU_FULL);
 #define STM_GET_PRCMU_EMPTY_STATUS(reg)        STM_READ_BITS(reg,STM_READ_MASK_PRCMU_EMPTY,STM_READ_SB_PRCMU_EMPTY);


 #define STM_GET_SBAG_BYTENB(reg)               STM_READ_BITS(reg,STM_READ_MASK_SBAG_BYTENB,STM_READ_SB_SBAG_BYTENB);
 #define STM_GET_SBAG_MISCOUNT(reg)             STM_READ_BITS(reg,STM_READ_MASK_SBAG_MISCOUNT,STM_READ_SB_SBAG_MISCOUNT);
 #define STM_GET_SBAG_OVERFLOW_NUM(reg)         STM_READ_BITS(reg,STM_READ_MASK_SBAG_OVERFLOW_NUM,STM_READ_SB_SBAG_OVERFLOW_NUM);
 #define STM_GET_SBAG_OVERFOW_FLAG(reg)         STM_READ_BITS(reg,STM_READ_MASK_SBAG_OVERFLOW_FLAG,STM_READ_SB_SBAG_OVERFLOW_FLAG);
 #define STM_GET_SBAG_FIFOFULL_FLAG(reg)        STM_READ_BITS(reg,STM_READ_MASK_SBAG_FIFOFULL_FLAG,STM_READ_SB_SBAG_FIFOFULL_FLAG);
 #define STM_GET_SBAG_FIFOEMPTY_FLAG(reg)       STM_READ_BITS(reg,STM_READ_MASK_SBAG_FIFOEMPTY_FLAG,STM_READ_SB_SBAG_FIFOEMPTY_FLAG);

 #define STM_SET_DBGEN(reg,a)                   STM_WRITE_BITS(reg,a,STM_READ_MASK_DEBUG_ENABLE,STM_READ_SB_DEBUG_ENABLE);
 #define STM_GET_DBGEN(reg)                     STM_READ_BITS(reg,STM_READ_MASK_DEBUG_ENABLE,STM_READ_SB_DEBUG_ENABLE);

 #define STM_SET_DBGM(reg,a)                    STM_WRITE_BITS(reg,a,STM_READ_MASK_DEBUG_MODE,STM_READ_SB_DEBUG_MODE);
 #define STM_GET_DBGM(reg)                      STM_READ_BITS(reg,STM_READ_MASK_DEBUG_MODE,STM_READ_SB_DEBUG_MODE);

 #define STM_SET_PTICSSEL(reg,a)                STM_WRITE_BITS(reg,a,STM_READ_MASK_DEBUG_PTICALIB,STM_READ_SB_DEBUG_PTICALIB);
 #define STM_GET_PTICSSEL(reg)                  STM_READ_BITS(reg,STM_READ_MASK_DEBUG_PTICALIB,STM_READ_SB_DEBUG_PTICALIB);

 #define STM_SET_FTFSEL(reg,a)                  STM_WRITE_BITS(reg,a,STM_READ_MASK_DEBUG_FTFSEL,STM_READ_SB_DEBUG_FTFSEL);
 #define STM_GET_FTFSEL(reg)                    STM_READ_BITS(reg,STM_READ_MASK_DEBUG_FTFSEL,STM_READ_SB_DEBUG_FTFSEL);

 #define STM_SET_FSBFF(reg,a)                   STM_WRITE_BITS(reg,a,STM_READ_MASK_DEBUG_FSBFF,STM_READ_SB_DEBUG_FSBFF);
 #define STM_GET_FSBFF(reg)                     STM_READ_BITS(reg,STM_READ_MASK_DEBUG_FSBFF,STM_READ_SB_DEBUG_FSBFF);

 #define STM_SET_PTISTATE(reg,a)                STM_WRITE_BITS(reg,a,STM_READ_MASK_DEBUG_PTISTATE,STM_READ_SB_DEBUG_PTISTATE);
 #define STM_GET_PTISTATE(reg)                  STM_READ_BITS(reg,STM_READ_MASK_DEBUG_PTISTATE,STM_READ_SB_DEBUG_PTISTATE);

 #define STM_GET_TIMESTAMP(reg)                 STM_READ_BITS(reg,STM_READ_MASK_TIMESTAMP,STM_READ_SB_TIMESTAMP);

 #define STM_GET_SLEEPCLOCK_COUNTER(reg)        STM_READ_BITS(reg,STM_READ_MASK_SLEEPCLOCK,STM_READ_SB_SLEEPCLOCK);
 #define STM_SET_CLOCKDIVISOR(xckdiv,a)         STM_WRITE_BITS(xckdiv,a,STM_MASKXCKDIV,STM_SBXCKDIV)   

/*--------------------------------------------------------------------------*
 * New internal types														*
 *--------------------------------------------------------------------------*/


/* Hardware registers description */

typedef struct
{
    /*Type	    Name								Description						     Offset     */
    t_uint32    stm_cr[2];                          /* STM Control Register              0x0000     */  
    t_uint32    stm_mmcr[2];                        /* STM MIPI modes Control Register   0x0008     */
    t_uint32    stm_ter[2];                         /* STM Trace Enable Register         0x0010     */
    t_uint32    stm_tdsr[2];                        /* STM Trace Disable Status Register 0x0018     */
    t_uint32    stm_ofsr[2];                        /* STM Overflow Status Register      0x0020     */
    t_uint32    stm_tfsr[2];                        /* STM Transmit FIFO Status Register 0x0028     */
    t_uint32    stm_sbsr[2];                        /* STM SBAG Status Register          0x0030     */
    t_uint32    stm_dbg[2];                         /* STM Debug Register                0x0038     */
    t_uint32    reserved_1[(0x50 - 0x40) >> 2];     /* Reserved							 0x038 to 0x050 */
    t_uint32    stm_tsr[2];                         /* STM Time Stamp Register           0x0050     */
    t_uint32    stm_sccr[2];                        /* STM Sleep Clock Counter Register  0x700F0058     */
    t_uint32    reserved_2[(0xFC0 - 0x60) >> 2];    /* Reserved							 0x060 to 0x078 */
    t_uint32    periph_id_0[2];                     /* Peripheral id: bits 7:0			 0x0FC0     */
    t_uint32    periph_id_1[2];                     /* Peripheral id: bits 15:8			 0x0FC8     */
    t_uint32    periph_id_2[2];                     /* Peripheral id: bits 23:16		 0x0FD0     */
    t_uint32    periph_id_3[2];                     /* Peripheral id: bits 31:24		 0x0FD8     */
    t_uint32    pcell_id_0[2];                      /* PrimeCell id: bits 7:0			 0x0FE0     */
    t_uint32    pcell_id_1[2];                      /* PrimeCell id: bits 15:8			 0x0FE8     */
    t_uint32    pcell_id_2[2];                      /* PrimeCell id: bits 23:16			 0x0FF0     */
    t_uint32    pcell_id_3[2];                      /* PrimeCell id: bits 31:24			 0x0FF8     */
}t_stm_register;

typedef struct
{
    t_stm_register         *p_stm_register;         /* The complete register map for the STM                                     */
    t_stm_xckdiv           stm_xcdiv;               /* The Clck Divider Value                                                    */
    t_stm_configuration    stm_configuration;       /* The structure used to configure the STM                                   */
    t_stm_tdsr             stm_tdsr;                /* The structure containing masters for which trace have to be disabled      */
    t_stm_ofsr             stm_ofsr;                /* The structure containing masters for which overflow status has to be read */
    t_stm_tfsr             stm_tfsr;                /* Contains the status of transmit FIFO Full and empty                       */
    t_stm_sbsr             stm_sbsr;                /* The structure for SBAG flags                                              */
    t_uint32                stm_ts;                  /* Stores the last value read of the free running time stamp counter         */
    t_uint32               stm_scc;                 /* Stores the last value read of the Sleep Clock counter running at 32kHz    */
	t_stm_initiator        *p_stm_initiator;        /* Stores the memory for any of the four initiators                          */
}t_stm_system_context;

#endif

/*_STMP_H_ */
/* End of file - stm_p.h */




